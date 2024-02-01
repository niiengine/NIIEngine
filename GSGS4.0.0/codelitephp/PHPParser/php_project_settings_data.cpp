#include "php_project_settings_data.h"
#include "php_configuration_data.h"
#include "php_workspace.h"
#include <wx/tokenzr.h>
#include <map>
#include <set>
#include <wx/uri.h>
#include "php_utils.h"
#include "globals.h"

PHPProjectSettingsData::PHPProjectSettingsData()
    : m_runAs(0)
    , m_flags(kOpt_PauseWhenExeTermiantes | kOpt_RunCurrentEditor)
{
}

PHPProjectSettingsData::~PHPProjectSettingsData() {}

wxArrayString PHPProjectSettingsData::GetIncludePathAsArray() const
{
    PHPProjectSettingsData s = *this;
    s.MergeWithGlobalSettings();
    wxArrayString paths = wxStringTokenize(s.m_includePath, "\r\n", wxTOKEN_STRTOK);
    return paths;
}

wxArrayString PHPProjectSettingsData::GetCCIncludePathAsArray() const
{
    PHPProjectSettingsData s = *this;
    s.MergeWithGlobalSettings();
    wxArrayString paths = wxStringTokenize(s.m_ccIncludePath, "\r\n", wxTOKEN_STRTOK);
    return paths;
}

wxArrayString PHPProjectSettingsData::GetAllIncludePaths()
{
    std::multimap<wxString, wxArrayString> extraIncludePaths;
    gsgsStringSet setIncludePaths;

    const PHPProject::Map_t& projects = PHPWorkspace::Get()->GetProjects();
    PHPProject::Map_t::const_iterator itp = projects.begin();
    for(; itp != projects.end(); ++itp) {
        const PHPProjectSettingsData& settings = itp->second->GetSettings();

        extraIncludePaths.insert(std::make_pair(itp->second->GetName(), settings.GetIncludePathAsArray()));
        extraIncludePaths.insert(std::make_pair(itp->second->GetName(), settings.GetCCIncludePathAsArray()));
    }

    std::map<wxString, wxArrayString>::const_iterator iter = extraIncludePaths.begin();
    for(; iter != extraIncludePaths.end(); iter++) {
        setIncludePaths.insert(iter->second.begin(), iter->second.end());
    }

    wxArrayString includes;
    gsgsStringSet::const_iterator iterSet = setIncludePaths.begin();
    for(; iterSet != setIncludePaths.end(); ++iterSet) {
        wxString path = *iterSet;
        path.Trim().Trim(false);
        if(wxFileName::DirExists(path)) {
            includes.Add(path);
        }
    }
    return includes;
}

void PHPProjectSettingsData::FromJSON(const gsgs::JSONObject& ele)
{
    ele.getObject("m_runAs").getValue(m_runAs, 0);
    ele.getObject("m_phpExe").getValue(m_phpExe);
    ele.getObject("m_indexFile").getValue(m_indexFile);
    ele.getObject("m_args").getValue(m_args);
    ele.getObject("m_workingDirectory").getValue(m_workingDirectory, ::wxGetCwd());
    ele.getObject("m_projectURL").getValue(m_projectURL);
    ele.getObject("m_includePath").getValue(m_includePath);
    ele.getObject("m_ccIncludePath").getValue(m_ccIncludePath);
    ele.getObject("m_flags").getValue(m_flags, m_flags);
    ele.getObject("m_phpIniFile").getValue(m_phpIniFile);
    ele.getObject("m_fileMapping").getValue(m_fileMapping);
}

gsgs::JSONObject PHPProjectSettingsData::ToJSON() const
{
    JSONObject settings = JSONObject::createObject("settings");
    settings.addProperty("m_runAs", m_runAs);
    settings.addProperty("m_phpExe", m_phpExe);
    settings.addProperty("m_indexFile", m_indexFile);
    settings.addProperty("m_args", m_args);
    settings.addProperty("m_workingDirectory", m_workingDirectory);
    settings.addProperty("m_projectURL", m_projectURL);
    settings.addProperty("m_includePath", m_includePath);
    settings.addProperty("m_ccIncludePath", m_ccIncludePath);
    settings.addProperty("m_flags", m_flags);
    settings.addProperty("m_phpIniFile", m_phpIniFile);
    settings.addProperty("m_fileMapping", m_fileMapping);
    return settings;
}

void PHPProjectSettingsData::MergeWithGlobalSettings()
{
    PHPConfigurationData globalData;
    globalData.Load();

    if(GetPhpExe().IsEmpty()) {
        SetPhpExe(globalData.GetPhpExe());
    }

    // Append the include paths (keep uniqueness)
    wxArrayString paths = ::wxStringTokenize(m_includePath, "\r\n", wxTOKEN_STRTOK);
    const wxArrayString& globalIncPaths = globalData.GetIncludePaths();
    for(size_t i = 0; i < globalIncPaths.GetCount(); ++i) {
        wxString path = wxFileName(globalIncPaths.Item(i), "").GetPath(wxPATH_UNIX | wxPATH_GET_VOLUME);
        if(paths.Index(path) == wxNOT_FOUND) {
            paths.Add(path);
        }
    }

    // Append the code completion paths (keep uniqueness)
    wxArrayString cc_paths = ::wxStringTokenize(m_ccIncludePath, "\r\n", wxTOKEN_STRTOK);
    const wxArrayString& globalCCIncPaths = globalData.GetCcIncludePath();
    for(size_t i = 0; i < globalCCIncPaths.GetCount(); ++i) {
        wxString ccpath = wxFileName(globalCCIncPaths.Item(i), "").GetPath(wxPATH_UNIX | wxPATH_GET_VOLUME);
        if(cc_paths.Index(ccpath) == wxNOT_FOUND) {
            cc_paths.Add(ccpath);
        }
    }

    m_includePath = ::wxJoin(paths, '\n');
    m_ccIncludePath = ::wxJoin(cc_paths, '\n');
}

wxString PHPProjectSettingsData::GetMappdPath(const wxString& sourcePath,
                                              bool useUrlScheme,
                                              const gsgsStringMap& additionalMapping) const
{
    wxFileName fnSource(sourcePath);
    gsgsStringMap fullMapping;
    fullMapping.insert(m_fileMapping.begin(), m_fileMapping.end());
    fullMapping.insert(additionalMapping.begin(), additionalMapping.end());

    wxString sourceFullPath = fnSource.GetFullPath();
    gsgsStringMap::const_iterator iter = fullMapping.begin();
    for(; iter != fullMapping.end(); ++iter) {
        if(sourceFullPath.StartsWith(iter->first)) {
            sourceFullPath.Remove(0, iter->first.length());
            sourceFullPath.Prepend(iter->second + "/");
            sourceFullPath.Replace("\\", "/");
            while(sourceFullPath.Replace("//", "/")) {
            }

            if(useUrlScheme) {
                sourceFullPath = ::FileNameToURI(sourceFullPath);
            }
            return sourceFullPath;
        }
    }

    if(useUrlScheme) {
        
        wxString asUrlScheme = sourcePath;
        asUrlScheme.Replace("\\", "/");
        
        while(asUrlScheme.Replace("//", "/"))
            ;

        asUrlScheme = ::FileNameToURI(asUrlScheme);
        return asUrlScheme;

    } else {
        wxString filePath;
        if(sourcePath.Contains(" ")) {
            filePath = sourcePath;
            filePath.Prepend('"').Append('"');
        }
        // return the path without changing it
        return filePath;
    }
}