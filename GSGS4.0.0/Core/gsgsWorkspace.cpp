
#include "gsgsPreProcess.h"
#include "gsgsWorkspace.h"
#include "gsgsTagManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsEvent.h"
#include "globals.h"
#include "plugin.h"
#include "xmlutils.h"
#include "gsgsLogManager.h"
#include "gsgsMacroManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsCompilerCmdParser.h"
#include "gsgsFileUtil.h"

#include <wx/app.h>
#include <wx/thread.h>
#include <wx/regex.h>
namespace gsgs
{
    //----------------------------------------------------------------------------
    CxxWS::CxxWS()
        : m_saveOnExit(true)
    {
        SetWorkspaceType(_("C++"));
        m_localWorkspace = new LocalWorkspace();
    }
    //----------------------------------------------------------------------------
    CxxWS::~CxxWS()
    {
        if(m_saveOnExit && m_doc.IsOk()) { SaveXmlFile(); }
        delete m_localWorkspace;
        m_localWorkspace = nullptr;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetName() const
    {
        if(m_doc.IsOk()) { return XmlUtils::ReadString(m_doc.GetRoot(), wxT("Name")); }
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    void CxxWS::CloseWorkspace()
    {
        m_buildMatrix.Reset(NULL);
        if(m_doc.IsOk()) {
            SaveXmlFile();
            m_doc = wxXmlDocument();
        }

        m_fileName.Clear();
        // reset the internal cache objects
        m_projects.clear();

        TagsManagerST::Get()->CloseDatabase();
    }
    //----------------------------------------------------------------------------
    bool CxxWS::OpenReadOnly(const wxString& fileName, wxString& errMsg)
    {
        m_buildMatrix.Reset(NULL);
        wxFileName workSpaceFile(fileName);
        if(!workSpaceFile.FileExists()) { return false; }
        m_fileName = workSpaceFile;
        m_doc.Load(m_fileName.GetFullPath());
        if(!m_doc.IsOk()) { return false; }

        m_saveOnExit = false;

        // Make sure we have the WORKSPACE/.gsgs folder exists
        {
            wxLogNull nolog;
            wxMkdir(GetPrivateFolder());
        }

        // Load all projects from the XML file
        std::vector<wxXmlNode*> removedChildren;
        DoLoadProjectsFromXml(m_doc.GetRoot(), "", removedChildren);

        DoUpdateBuildMatrix();
        return true;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::OpenWorkspace(const wxString& fileName, wxString& errMsg)
    {
        if (DoLoadWorkspace(fileName, errMsg))
        {
            // Notify about active project changed
            ProjectPtr activeProject = GetActiveProject();
            if (activeProject) {
                ProjectSettingsEvent evt(wxEVT_ACTIVE_PROJECT_CHANGED);
                evt.SetProjectName(activeProject->GetName());
                evt.SetFileName(activeProject->GetFileName().GetFullPath());
                gsgs_Event().AddPendingEvent(evt);
            }
            return true;
        }
        return false;
    }
    //----------------------------------------------------------------------------
    BuildMatrixPtr CxxWS::GetBuildMatrix() const { return m_buildMatrix; }
    //----------------------------------------------------------------------------
    wxXmlNode* CxxWS::GetWorkspaceEditorOptions() const
    {
        return XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Options"));
    }
    //----------------------------------------------------------------------------
    void CxxWS::SetWorkspaceEditorOptions(LocalOptionsConfigPtr opts)
    {
        wxXmlNode* parent = m_doc.GetRoot();
        wxXmlNode* oldOptions = XmlUtils::FindFirstByTagName(parent, wxT("Options"));
        if(oldOptions) {
            oldOptions->GetParent()->RemoveChild(oldOptions);
            delete oldOptions;
        }
        parent->AddChild(opts->ToXml());
        SaveXmlFile();
    }
    //----------------------------------------------------------------------------
    void CxxWS::SetBuildMatrix(BuildMatrixPtr mapping)
    {
        wxXmlNode* parent = m_doc.GetRoot();
        wxXmlNode* oldMapping = XmlUtils::FindFirstByTagName(parent, wxT("BuildMatrix"));
        if(oldMapping) {
            parent->RemoveChild(oldMapping);
            wxDELETE(oldMapping);
        }
        parent->AddChild(mapping->ToXml());
        SaveXmlFile();

        // force regeneration of makefiles for all projects
        for(ProjectMap_t::iterator iter = m_projects.begin(); iter != m_projects.end(); iter++) {
            iter->second->SetModified(true);
        }

        DoUpdateBuildMatrix();
    }
    //----------------------------------------------------------------------------
    bool CxxWS::CreateWorkspace(const wxString& name, const wxString& path, wxString& errMsg)
    {
        // If we have an open workspace, close it
        if(m_doc.IsOk()) {
            if(!SaveXmlFile()) {
                errMsg = wxT("Failed to save current workspace");
                return false;
            }
        }

        if(name.IsEmpty()) {
            errMsg = wxT("Invalid workspace name");
            return false;
        }

        // Create new
        // Open workspace database
        m_fileName = wxFileName(path, name + wxT(".gsgsws"));

        // Make sure we have the WORKSPACE/.gsgs folder exists
        {
            wxLogNull nolog;
            wxMkdir(GetPrivateFolder());
        }

        // This function sets the working directory to the workspace directory!
        ::wxSetWorkingDirectory(m_fileName.GetPath());
        m_buildMatrix.Reset(NULL);

        wxFileName dbFileName = GetTagsFileName();
        TagsManagerST::Get()->OpenDatabase(dbFileName);

        wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("GSGSWP"));
        m_doc.SetRoot(root);
        m_doc.GetRoot()->XmlAddProperty(wxT("Name"), name);
        m_doc.GetRoot()->XmlAddProperty(wxT("Database"), dbFileName.GetFullPath(wxPATH_UNIX));

        m_doc.GetRoot()->DeleteAttribute(wxT("SWTLW"));
        if(GetLocalWorkspace()->GetParserFlags() & LocalWorkspace::EnableSWTLW) {
            m_doc.GetRoot()->XmlAddProperty(wxT("SWTLW"), "Yes");
        }

        SaveXmlFile();
        DoUpdateBuildMatrix();
        return true;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetStringProperty(const wxString& propName, wxString& errMsg)
    {
        if(!m_doc.IsOk()) {
            errMsg = wxT("No workspace open");
            return wxEmptyString;
        }

        wxXmlNode* rootNode = m_doc.GetRoot();
        if(!rootNode) {
            errMsg = wxT("Corrupted workspace file");
            return wxEmptyString;
        }

        return rootNode->XmlGetPropVal(propName, wxEmptyString);
    }
    //----------------------------------------------------------------------------
    void CxxWS::AddProjectToBuildMatrix(ProjectPtr prj)
    {
        if(!prj) {
            wxMessageBox(_("AddProjectToBuildMatrix was called with NULL project"), _("gsgs"), wxICON_WARNING | wxOK);
            return;
        }

        BuildMatrixPtr matrix = GetBuildMatrix();
        wxString selConfName = matrix->GetSelectedConfigurationName();

        std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
        std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
        for(; iter != wspList.end(); iter++) {
            WorkspaceConfigurationPtr workspaceConfig = (*iter);
            WorkspaceConfiguration::ConfigMappingList prjList = workspaceConfig->GetMapping();
            wxString wspCnfName = workspaceConfig->GetName();

            ProjectSettingsCookie cookie;

            // getSettings is a bit misleading, since it actually create new instance which represents the layout
            // of the XML
            ProjectSettingsPtr settings = prj->GetSettings();
            BuildConfigPtr prjBldConf = settings->GetFirstBuildConfiguration(cookie);
            BuildConfigPtr matchConf;

            if(!prjBldConf) {
                // the project does not have any settings, create new one and add it
                prj->SetSettings(settings);

                settings = prj->GetSettings();
                prjBldConf = settings->GetFirstBuildConfiguration(cookie);
                matchConf = prjBldConf;

            } else {

                matchConf = prjBldConf;

                // try to locate the best match to add to the workspace
                while(prjBldConf) {
                    wxString projBldConfName = prjBldConf->GetName();
                    if(wspCnfName == projBldConfName) {
                        // we found a suitable match use it instead of the default one
                        matchConf = prjBldConf;
                        break;
                    }
                    prjBldConf = settings->GetNextBuildConfiguration(cookie);
                }
            }

            ConfigMappingEntry entry(prj->GetName(), matchConf->GetName());
            prjList.push_back(entry);
            (*iter)->SetConfigMappingList(prjList);
            matrix->SetConfiguration((*iter));
        }

        // and set the configuration name
        matrix->SetSelectedConfigurationName(selConfName);

        // this will also reset the build matrix pointer
        SetBuildMatrix(matrix);
    }
    //----------------------------------------------------------------------------
    void CxxWS::RemoveProjectFromBuildMatrix(ProjectPtr prj)
    {
        BuildMatrixPtr matrix = GetBuildMatrix();
        wxString selConfName = matrix->GetSelectedConfigurationName();

        std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
        std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
        for(; iter != wspList.end(); iter++) {
            WorkspaceConfiguration::ConfigMappingList prjList = (*iter)->GetMapping();

            WorkspaceConfiguration::ConfigMappingList::iterator it = prjList.begin();
            for(; it != prjList.end(); it++) {
                if((*it).m_project == prj->GetName()) {
                    prjList.erase(it);
                    break;
                }
            }

            (*iter)->SetConfigMappingList(prjList);
            matrix->SetConfiguration((*iter));
        }

        // and set the configuration name
        matrix->SetSelectedConfigurationName(selConfName);

        // this will also reset the build matrix pointer
        SetBuildMatrix(matrix);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::CreateProject(const wxString& name, const wxString& path, const wxString& type,
                                       const wxString& workspaceFolder, bool addToBuildMatrix, wxString& errMsg)
    {
        if(!m_doc.IsOk()) {
            errMsg = wxT("No workspace open");
            return false;
        }

        ProjectPtr proj(new Project());
        proj->Create(name, wxEmptyString, path, type);
        proj->AssociateToWorkspace(this);
        proj->SetWorkspaceFolder(workspaceFolder);
        m_projects[name] = proj;

        // make the project path to be relative to the workspace, if it's sensible to do so
        wxFileName tmp(path + gsgsPathSep + name + wxT(".project"));
        tmp.MakeRelativeTo(m_fileName.GetPath());

        // Add an entry to the workspace file
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        node->XmlAddProperty(wxT("Name"), name);
        node->XmlAddProperty(wxT("Path"), tmp.GetFullPath(wxPATH_UNIX));

        // Create the workspace folder and add the project
        wxXmlNode* parentNode = DoCreateWorkspaceFolder(workspaceFolder);
        parentNode->AddChild(node);

        if(m_projects.size() == 1) { SetActiveProject(name); }

        SaveXmlFile();
        if(addToBuildMatrix) { AddProjectToBuildMatrix(proj); }
        return true;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::AddProject(const wxString& path,const wxString& workspaceFolder, wxString& errMsg)
    {
        if(!m_doc.IsOk()) {
            errMsg = _("No workspace open");
            return false;
        }

        ProjectPtr proj(new Project());
        if(!proj->Load(path)) {
            errMsg << _("Failed to load project file: ") << path;
            return false;
        }
        proj->AssociateToWorkspace(this);
        proj->SetWorkspaceFolder(workspaceFolder);
        m_projects[proj->GetName()] = proj;

        // make the project path to be relative to the workspace, if it's sensible to do so
        wxFileName tmp(path);
        tmp.MakeRelativeTo(m_fileName.GetPath());

        // Add an entry to the workspace file
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
        node->AddProperty(wxT("Name"), proj->GetName());
        node->AddProperty(wxT("Path"), tmp.GetFullPath(wxPATH_UNIX));

        // Create the workspace folder and add the project
        wxXmlNode* parentNode = DoCreateWorkspaceFolder(workspaceFolder);
        parentNode->AddChild(node);

        if(m_projects.size() == 1) { SetActiveProject(proj->GetName()); }

        SaveXmlFile();
        AddProjectToBuildMatrix(proj);
        return true;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::AddProject(const wxString& path, wxString& errMsg)
    {
        if(!m_doc.IsOk()) {
            errMsg = wxT("No workspace open");
            return false;
        }

        wxFileName fn(path);
        if(!fn.FileExists()) {
            errMsg = wxT("File does not exist");
            return false;
        }

        // Load the project into
        ProjectPtr newProject(new Project());
        if(!newProject->Load(path)) {
            errMsg = wxT("Corrupted project file '");
            errMsg << path << wxT("'");
            return false;
        }

        // Try first to find a project with similar name in the workspace
        ProjectPtr proj = FindProjectByName(newProject->GetName(), errMsg);
        if(!proj) {

            // No project could be find, add it to the workspace
            DoAddProject(newProject);

            // Add an entry to the workspace file
            fn.MakeRelativeTo(m_fileName.GetPath());

            wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
            node->AddProperty(wxT("Name"), fn.GetName());
            node->AddProperty(wxT("Path"), fn.GetFullPath(wxPATH_UNIX));
            node->AddProperty(wxT("Active"), m_projects.size() == 1 ? wxT("Yes") : wxT("No"));
            m_doc.GetRoot()->AddChild(node);
            if(!SaveXmlFile()) {
                wxMessageBox(
                    _("Failed to save workspace file to disk. Please check that you have permission to write to disk"),
                    _("光栅故事"), wxICON_ERROR | wxOK);
                return false;
            }

            AddProjectToBuildMatrix(newProject);
            return true;

        } else {
            errMsg = wxString::Format(wxT("A project with a similar name '%s' already exists in the workspace"),
                                      proj->GetName().c_str());
            return false;
        }
    }
    //----------------------------------------------------------------------------
    ProjectPtr CxxWS::FindProjectByName(const wxString& projName, wxString& errMsg) const
    {
        if(!m_doc.IsOk()) {
            errMsg = wxT("No workspace open");
            return NULL;
        }

        ProjectMap_t::const_iterator iter = m_projects.find(projName);
        if(iter == m_projects.end()) {
            errMsg = wxT("Invalid project name '");
            errMsg << projName << wxT("'");
            return NULL;
        }
        return iter->second;
    }
    //----------------------------------------------------------------------------
    void CxxWS::GetProjectList(wxArrayString& list) const
    {
        list.reserve(m_projects.size());
        ProjectMap_t::const_iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); iter++) {
            wxString name;
            name = iter->first;
            list.Add(name);
        }
    }
    //----------------------------------------------------------------------------
    ProjectPtr CxxWS::DoAddProject(ProjectPtr proj)
    {
        if(!proj) { return NULL; }

        m_projects.insert(std::make_pair(proj->GetName(), proj));
        proj->AssociateToWorkspace(this);
        return proj;
    }
    //----------------------------------------------------------------------------
    ProjectPtr CxxWS::DoAddProject(const wxString& path, const wxString& projectVirtualFolder, wxString& errMsg)
    {
        // Add the project
        ProjectPtr proj(new Project());

        // Convert the path to absolute path
        wxFileName projectFile(path);
        if(projectFile.IsRelative()) { projectFile.MakeAbsolute(m_fileName.GetPath()); }

        if(!proj->Load(projectFile.GetFullPath())) {
            errMsg = wxT("Corrupted project file '");
            errMsg << projectFile.GetFullPath() << wxT("'");
            return NULL;
        }

        // Add an entry to the projects map
        m_projects.insert(std::make_pair(proj->GetName(), proj));
        proj->AssociateToWorkspace(this);
        proj->SetWorkspaceFolder(projectVirtualFolder);
        return proj;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::RemoveProject(const wxString& name, wxString& errMsg, const wxString& workspaceFolder)
    {
        ProjectPtr proj = FindProjectByName(name, errMsg);
        if(!proj) { return false; }

        // remove the associated build configuration with this
        // project
        RemoveProjectFromBuildMatrix(proj);

        // remove the project from the internal map
        ProjectMap_t::iterator iter = m_projects.find(proj->GetName());
        if(iter != m_projects.end()) { m_projects.erase(iter); }

        // update the xml file
        // Incase we got a workspace folder, the project node will exists under the
        // workspace folder node and not under the root
        wxXmlNode* root = m_doc.GetRoot();
        if(!workspaceFolder.IsEmpty()) {
            wxXmlNode* node = DoGetWorkspaceFolderXmlNode(workspaceFolder);
            if(node) { root = node; }
        }

        wxXmlNode* child = root->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Project") && child->XmlGetPropVal(wxT("Name"), wxEmptyString) == name) {
                if(child->XmlGetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0) {
                    // the removed project was active,
                    // select new project to be active
                    if(!m_projects.empty()) {
                        ProjectMap_t::iterator iter = m_projects.begin();
                        SetActiveProject(iter->first);
                    }
                }
                root->RemoveChild(child);
                delete child;
                break;
            }
            child = child->GetNext();
        }

        // go over the dependencies list of each project and remove the project
        iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            ProjectPtr p = iter->second;
            if(p) {
                wxArrayString configs;
                // populate the choice control with the list of available configurations for this project
                ProjectSettingsPtr settings = p->GetSettings();
                if(settings) {
                    ProjectSettingsCookie cookie;
                    BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
                    while(bldConf) {
                        configs.Add(bldConf->GetName());
                        bldConf = settings->GetNextBuildConfiguration(cookie);
                    }
                }

                // update each configuration of this project
                for(size_t i = 0; i < configs.GetCount(); i++) {

                    wxArrayString deps = p->GetDependencies(configs.Item(i));
                    int where = deps.Index(name);
                    if(where != wxNOT_FOUND) { deps.RemoveAt((size_t)where); }

                    // update the configuration
                    p->SetDependencies(deps, configs.Item(i));
                }
            }
        }
        return SaveXmlFile();
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetActiveProjectName() const
    {
        if (m_doc.IsOk())
        {
            std::list<wxXmlNode*> xmls = DoGetProjectsXmlNodes();
            std::list<wxXmlNode*>::iterator iter = std::find_if(xmls.begin(), xmls.end(), [&](wxXmlNode* node) {
                return (node->GetAttribute("Active", wxEmptyString).CmpNoCase("yes") == 0);
            });

            if (iter == xmls.end()) return "";
            return (*iter)->GetAttribute("Name", wxEmptyString);
        }
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    void CxxWS::SetActiveProject(const wxString& name)
    {
        if (m_doc.IsOk())
        {
            // Clear all other projects
            DoUnselectActiveProject();

            std::list<wxXmlNode*> xmls = DoGetProjectsXmlNodes();
            std::for_each(xmls.begin(), xmls.end(), [&](wxXmlNode* node) {
                XmlUtils::UpdateProperty(node, "Active", (node->XmlGetPropVal(wxT("Name"), wxEmptyString) == name) ? "Yes" : "No");
            });

            SaveXmlFile();
        }
    }
    //----------------------------------------------------------------------------
    bool CxxWS::CreateVirtualDirectory(const wxString& vdFullPath, wxString& errMsg, bool mkPath)
    {
        wxStringTokenizer tkz(vdFullPath, wxT(":"));
        wxString projName = tkz.GetNextToken();

        wxString fixedPath;
        // Construct new path excluding the first token
        size_t count = tkz.CountTokens();

        for(size_t i = 0; i < count - 1; i++) {
            fixedPath += tkz.GetNextToken();
            fixedPath += wxT(":");
        }
        fixedPath += tkz.GetNextToken();

        ProjectPtr proj = FindProjectByName(projName, errMsg);
        return proj->CreateVirtualDir(fixedPath, mkPath);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::RemoveVirtualDirectory(const wxString& vdFullPath, wxString& errMsg)
    {
        wxStringTokenizer tkz(vdFullPath, wxT(":"));
        wxString projName = tkz.GetNextToken();

        wxString fixedPath;
        // Construct new path excluding the first token
        size_t count = tkz.CountTokens();

        for(size_t i = 0; i < count - 1; i++) {
            fixedPath += tkz.GetNextToken();
            fixedPath += wxT(":");
        }
        fixedPath += tkz.GetNextToken();

        ProjectPtr proj = FindProjectByName(projName, errMsg);
        return proj->DeleteVirtualDir(fixedPath);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::SaveXmlFile()
    {
        // We first remove the Save Workspace To Local Workspace (SWTLW) attribute
        // and then check the current state in the Code Completion tab. Then
        // we read new path values from the LW and set the appropiate attribute value.
        if(m_doc.GetRoot()->GetAttribute(wxT("SWTLW")) != wxEmptyString) { m_doc.GetRoot()->DeleteAttribute(wxT("SWTLW")); }

        if(GetLocalWorkspace()->GetParserFlags() & LocalWorkspace::EnableSWTLW) {
            m_doc.GetRoot()->XmlAddProperty(wxT("SWTLW"), "Yes");
            SyncFromLocalWorkspaceSTParserPaths();
            SyncFromLocalWorkspaceSTParserMacros();
        }

        // Set the workspace XML version
        wxString version;
        if(!m_doc.GetRoot()->GetAttribute("Version", &version)) {
            m_doc.GetRoot()->AddAttribute("Version", WORKSPACE_XML_VERSION);
        }

        bool ok = m_doc.Save(m_fileName.GetFullPath());
        SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());
        gsgs_Event().PostFileSavedEvent(m_fileName.GetFullPath());
        DoUpdateBuildMatrix();
        return ok;
    }
    //----------------------------------------------------------------------------
    void CxxWS::SyncToLocalWorkspaceSTParserPaths()
    {
        wxArrayString inclduePaths;
        wxArrayString excludePaths;
        wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
        if(workspaceInclPaths) {
            wxXmlNode* child = workspaceInclPaths->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Exclude")) {
                    wxString path = child->XmlGetPropVal(wxT("Path"), wxT(""));
                    path.Trim().Trim(false);
                    if(path.IsEmpty() == false) { excludePaths.Add(path); }
                }

                else if(child->GetName() == wxT("Include")) {
                    wxString path = child->XmlGetPropVal(wxT("Path"), wxT(""));
                    path.Trim().Trim(false);
                    if(path.IsEmpty() == false) { inclduePaths.Add(path); }
                }

                child = child->GetNext();
            }
            GetLocalWorkspace()->SetParserPaths(inclduePaths, excludePaths);
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::SyncFromLocalWorkspaceSTParserPaths()
    {
        //
        // Here we just get the parser paths from the LocalWorkspaceST and write it into the worspace project file.
        //
        wxXmlNode* workspaceInclPaths = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserPaths"));
        if(workspaceInclPaths) {
            m_doc.GetRoot()->RemoveChild(workspaceInclPaths);
            delete workspaceInclPaths;
        }

        //
        // Get workspace parse paths from local workspace file.
        //
        wxArrayString inclduePaths;
        wxArrayString excludePaths;
        GetLocalWorkspace()->GetParserPaths(inclduePaths, excludePaths);

        workspaceInclPaths = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("WorkspaceParserPaths"));
        for(size_t i = 0; i < inclduePaths.GetCount(); i++) {
            wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Include"));
            child->XmlAddProperty(wxT("Path"), inclduePaths.Item(i));
        }

        for(size_t i = 0; i < excludePaths.GetCount(); i++) {
            wxXmlNode* child = new wxXmlNode(workspaceInclPaths, wxXML_ELEMENT_NODE, wxT("Exclude"));
            child->XmlAddProperty(wxT("Path"), excludePaths.Item(i));
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::SyncToLocalWorkspaceSTParserMacros()
    {
        wxString macros;
        wxXmlNode* workspaceMacros = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
        if(workspaceMacros) {
            macros = workspaceMacros->GetNodeContent();
            macros.Trim().Trim(false);
            GetLocalWorkspace()->SetParserMacros(macros);
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::SyncFromLocalWorkspaceSTParserMacros()
    {
        //
        // Here we just get the parser macros from the LocalWorkspaceST and write it into the worspace project file.
        //
        wxXmlNode* workspaceMacros = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
        if(workspaceMacros) {
            m_doc.GetRoot()->RemoveChild(workspaceMacros);
            delete workspaceMacros;
        }

        //
        // Get workspace parse macros from local workspace file.
        //
        wxString macros;
        GetLocalWorkspace()->GetParserMacros(macros);
        workspaceMacros = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("WorkspaceParserMacros"));
        if(!macros.IsEmpty()) {
            wxXmlNode* contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, macros);
            workspaceMacros->AddChild(contentNode);
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::Save()
    {
        if(m_doc.IsOk()) {
            ProjectMap_t::iterator iter = m_projects.begin();
            for(; iter != m_projects.end(); iter++) {
                iter->second->Save();
            }
            SaveXmlFile();
        }
    }
    //----------------------------------------------------------------------------
    bool CxxWS::AddNewFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg)
    {
        wxStringTokenizer tkz(vdFullPath, wxT(":"));

        // We should have at least 2 tokens:
        // project:virtual directory
        if(tkz.CountTokens() < 2) return false;

        wxString projName = tkz.GetNextToken();
        wxString fixedPath;
        // Construct new path excluding the first token
        size_t count = tkz.CountTokens();

        for(size_t i = 0; i < count - 1; i++) {
            fixedPath += tkz.GetNextToken();
            fixedPath += wxT(":");
        }
        fixedPath += tkz.GetNextToken();

        ProjectPtr proj = FindProjectByName(projName, errMsg);
        if(!proj) {
            errMsg = wxT("No such project");
            return false;
        }

        return proj->AddFile(fileName, fixedPath);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::RemoveFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg)
    {
        wxStringTokenizer tkz(vdFullPath, wxT(":"));
        wxString projName = tkz.GetNextToken();
        wxString fixedPath;

        // Construct new path excluding the first token
        size_t count = tkz.CountTokens();
        if(!count) {
            errMsg = _("Malformed project name");
            return false;
        }

        for(size_t i = 0; i < count - 1; i++) {
            fixedPath += tkz.GetNextToken();
            fixedPath += wxT(":");
        }
        fixedPath += tkz.GetNextToken();

        ProjectPtr proj = FindProjectByName(projName, errMsg);
        if(!proj) {
            errMsg = _("No such project");
            return false;
        }

        bool result = proj->RemoveFile(fileName, fixedPath);
        if(!result) { errMsg = _("File removal failed"); }
        return result;
    }
    //----------------------------------------------------------------------------
    BuildConfigPtr CxxWS::GetProjBuildConf(const wxString& projectName, const wxString& confName) const
    {
        BuildMatrixPtr matrix = GetBuildMatrix();
        if(!matrix) { return NULL; }

        wxString projConf(confName);

        if(projConf.IsEmpty()) {
            wxString workspaceConfig = matrix->GetSelectedConfigurationName();
            projConf = matrix->GetProjectSelectedConf(workspaceConfig, projectName);
        }

        // Get the project setting and retrieve the selected configuration
        wxString errMsg;
        ProjectPtr proj = FindProjectByName(projectName, errMsg);
        if(proj) {
            ProjectSettingsPtr settings = proj->GetSettings();
            if(settings) { return settings->GetBuildConfiguration(projConf, true); }
        }
        return NULL;
    }
    //----------------------------------------------------------------------------
    void CxxWS::ReloadWorkspace()
    {
        m_doc = wxXmlDocument();

        wxLogNull noLog;
        // reset the internal cache objects
        m_projects.clear();

        TagsManager* mgr = TagsManagerST::Get();
        mgr->CloseDatabase();

        wxString err_msg;
        if(!OpenWorkspace(m_fileName.GetFullPath(), err_msg)) { gsgsDEBUG() << "Reload workspace:" << err_msg; }
    }
    //----------------------------------------------------------------------------
    time_t CxxWS::GetFileLastModifiedTime() const 
    { 
        return GetFileModificationTime(GetFileName()); 
    }
    //----------------------------------------------------------------------------
    static CxxWS * gs_Workspace = NULL;
    void CxxWS::Release()
    {
        if(gs_Workspace) 
        { 
            delete gs_Workspace; 
        }
        gs_Workspace = NULL;
    }
    //----------------------------------------------------------------------------
    CxxWS & CxxWS::singleton()
    {
        if(gs_Workspace == NULL)
        {
            gs_Workspace = new CxxWS();
        }
        return *gs_Workspace;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetParserMacros()
    {
        if(!m_doc.IsOk()) return wxEmptyString;

        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("WorkspaceParserMacros"));
        if(node) {
            wxString nodeContent = node->GetNodeContent();
            nodeContent.Trim().Trim(false);
            return nodeContent;
        }
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetEnvironmentVariabels()
    {
        if(!m_doc.IsOk()) return wxEmptyString;

        // Use the environment variables set in the build matrix ("workspace configuration")
        wxString env;
        if(GetSelectedConfig()) {
            env = GetSelectedConfig()->GetEnvironmentVariables();
            if(!env.IsEmpty()) { return env; }
        }

        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
        if(node) {
            wxString nodeContent = node->GetNodeContent();
            nodeContent.Trim().Trim(false);
            return nodeContent;
        }
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    void CxxWS::SetEnvironmentVariabels(const wxString& envvars)
    {
        if(!m_doc.IsOk()) return;
        if(GetSelectedConfig()) {
            GetSelectedConfig()->SetEnvironmentVariables(envvars);
            SetBuildMatrix(GetBuildMatrix()); // force an XML update
            SaveXmlFile();
            return;
        }

        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
        if(node) {
            m_doc.GetRoot()->RemoveChild(node);
            delete node;
        }

        node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
        m_doc.GetRoot()->AddChild(node);

        wxString nodeContent = envvars;
        nodeContent.Trim().Trim(false);

        wxXmlNode* contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, nodeContent);
        node->AddChild(contentNode);
        SaveXmlFile();
    }
    //----------------------------------------------------------------------------
    wxArrayString CxxWS::GetAllProjectPaths()
    {
        wxArrayString projects;
        ProjectMap_t::iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); iter++) {
            projects.Add(iter->second->GetFileName().GetFullPath());
        }
        return projects;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::IsOpen() const { return m_doc.IsOk(); }
    //----------------------------------------------------------------------------
    bool CxxWS::IsVirtualDirectoryExists(const wxString& vdFullPath)
    {
        wxStringTokenizer tkz(vdFullPath, wxT(":"));
        wxString projName = tkz.GetNextToken();

        wxString fixedPath;
        // Construct new path excluding the first token
        size_t count = tkz.CountTokens();

        for(size_t i = 0; i < count - 1; i++) {
            fixedPath += tkz.GetNextToken();
            fixedPath += wxT(":");
        }
        fixedPath += tkz.GetNextToken();

        wxString errMsg;
        ProjectPtr proj = FindProjectByName(projName, errMsg);
        if(!proj) { return false; }

        wxXmlNode* vdNode = proj->GetVirtualDir(fixedPath);
        return vdNode != NULL;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetPrivateFolder() const
    {
        wxFileName workspacePath;
        if(IsOpen()) {
            workspacePath = GetFileName();
        } else {
            // try maybe the workspace is opened by a plugin
            Event event(wxEVT_CMD_IS_WORKSPACE_OPEN);
            event.SetAnswer(false);
            gsgs_Event().ProcessEvent(event);
            if(event.IsAnswer()) { workspacePath = event.GetFileName(); }
        }
        if(workspacePath.Exists()) {
            // append the .gsgsworkspace folder
            workspacePath.AppendDir(".gsgsworkspace");

            // ensure the path exists
            workspacePath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

            return workspacePath.GetPath();
        }
        return "";
    }
    //----------------------------------------------------------------------------
    wxFileName CxxWS::GetTagsFileName() const
    {
        if(!IsOpen()) { return wxFileName(); }

        wxFileName fn_tags(GetPrivateFolder(), GetFileName().GetFullName());
        fn_tags.SetName(fn_tags.GetName() + "-" + GetUserName());
        fn_tags.SetExt("tags");
        return fn_tags;
    }
    //----------------------------------------------------------------------------
    cJSON* CxxWS::CreateCompileCommandsJSON() const
    {
        // Build the global compiler paths, we will need this later on...
        gsgsStringMap compilersGlobalPaths;
        std::unordered_map<wxString, wxArrayString> pathsMap = gsgs_Settings().GetCompilersGlobalPaths();
        for(const auto& vt : pathsMap) {
            wxString compiler_name = vt.first;
            wxArrayString pathsArr = vt.second;
            wxString paths;
            std::for_each(pathsArr.begin(), pathsArr.end(), [&](wxString& path) {
                path.Trim().Trim(false);
                if(path.EndsWith("\\")) { path.RemoveLast(); }
                paths << path << ";";
            });
            compilersGlobalPaths.insert({ compiler_name, paths });
        }

        // Check if the active project is using custom build
        ProjectPtr activeProject = GetActiveProject();
        if(activeProject) {
            BuildConfigPtr buildConf = activeProject->GetBuildConfiguration();
            if(buildConf && buildConf->IsCustomBuild()) {
                // Using custom build
                wxString buildWorkingDirectory = MacroManager::Instance()->Expand(
                    buildConf->GetCustomBuildWorkingDir(), NULL, activeProject->GetName(), buildConf->GetName());
                wxFileName fnWorkingDirectory(buildWorkingDirectory, "compile_commands.json");
                if(fnWorkingDirectory.FileExists()) {

                    JSON root(fnWorkingDirectory);
                    if(root.isValid()) {
                        JSON newFile(cJSON_Array);
                        JSONObject newArr = newFile.getObject();
                        JSONObject arr = root.getObject();
                        int size = arr.getArraySize();
                        for(int i = 0; i < size; ++i) {
                            JSONObject file = arr.getArray(i);
                            wxString command;
                            wxString file_name;
                            wxString directory;
                            file.getObject("command").getValue(command);
                            file.getObject("file").getValue(file_name);
                            file.getObject("directory").getValue(directory);

                            std::vector<wxString> files;
                            files.push_back(file_name);

                            wxFileName headerFile(file_name);
                            headerFile.SetExt("h");
                            if(headerFile.FileExists()) { files.push_back(headerFile.GetFullPath()); }
                        
                            // Create two entries: header & cpp (if the header exists)
                            for(const wxString& filename : files) {
                                JSONObject fileItem = JSONObject::createObject();
                                fileItem.addProperty("file", filename).addProperty("directory", directory);

                                // Fix the build command
                                CompilerCmdParser cclp(command, directory);
                                cclp.MakeAbsolute(directory);
                                fileItem.addProperty("command",
                                                     wxString() << "clang " << cclp.GetCompileLine() << " -c " << filename);
                                newArr.arrayAppend(fileItem);
                            }
                        }
                        return newFile.release();
                    }
                }
            }
        }

        JSONObject compile_commands = JSONObject::createArray();
        CxxWS::ProjectMap_t::const_iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            BuildConfigPtr buildConf = iter->second->GetBuildConfiguration();
            if(buildConf && buildConf->IsProjectEnabled() && !buildConf->IsCustomBuild() &&
               buildConf->IsCompilerRequired()) {
                iter->second->CreateCompileCommandsJSON(compile_commands, compilersGlobalPaths);
            }
        }
        return compile_commands.release();
    }
    //----------------------------------------------------------------------------
    ProjectPtr CxxWS::GetActiveProject() const 
    { 
        return GetProject(GetActiveProjectName()); 
    }
    //----------------------------------------------------------------------------
    ProjectPtr CxxWS::GetProject(const wxString& name) const
    {
        CxxWS::ProjectMap_t::const_iterator iter = m_projects.find(name);
        if(iter == m_projects.end()) { return NULL; }
        return iter->second;
    }
    //----------------------------------------------------------------------------
    void CxxWS::GetCompilers(gsgsStringSet& compilers)
    {
        CxxWS::ProjectMap_t::iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            iter->second->GetCompilers(compilers);
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::ReplaceCompilers(gsgsStringMap & compilers)
    {
        CxxWS::ProjectMap_t::iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            iter->second->ReplaceCompilers(compilers);
        }
    }
    //----------------------------------------------------------------------------
    void CxxWS::DoUpdateBuildMatrix()
    {
        m_buildMatrix.Reset(new BuildMatrix(XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "BuildMatrix")));
    }
    //----------------------------------------------------------------------------
    void CxxWS::RenameProject(const wxString& oldname, const wxString& newname)
    {
        // Update the build matrix (we work on the XML directly here)
        wxXmlNode* buildMatrixNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "BuildMatrix");
        if(buildMatrixNode) {
            wxXmlNode* child = buildMatrixNode->GetChildren();
            while(child) {
                if(child->GetName() == "WorkspaceConfiguration") {
                    wxXmlNode* projectNode = child->GetChildren();
                    while(projectNode) {
                        if(projectNode->GetName() == "Project") {
                            wxString name = projectNode->GetAttribute("Name");
                            if(name == oldname) { XmlUtils::UpdateProperty(projectNode, "Name", newname); }
                        }
                        projectNode = projectNode->GetNext();
                    }
                }
                child = child->GetNext();
            }
        }

        // Update the list of projects in the workspace
        wxXmlNode* projectNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), "Project");
        while(projectNode) {
            if(projectNode->GetAttribute("Name") == oldname) { XmlUtils::UpdateProperty(projectNode, "Name", newname); }
            projectNode = projectNode->GetNext();
        }
        // Update dependenices for each project
        CxxWS::ProjectMap_t::iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            iter->second->ProjectRenamed(oldname, newname);
        }

        CxxWS::ProjectMap_t tmpProjects;
        iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            tmpProjects.insert(std::make_pair(iter->first, iter->second));
        }
        m_projects.swap(tmpProjects);

        // Save everything
        Save();

        // Notify about this change
        Event event(wxEVT_PROJ_RENAMED);
        event.SetOldName(oldname);
        event.SetString(newname);
        gsgs_Event().AddPendingEvent(event);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::IsBuildSupported() const { return true; }
    //----------------------------------------------------------------------------
    bool CxxWS::IsProjectSupported() const { return true; }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetFilesMask() const
    {
        wxString findInFilesMask = "*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;*.ini;*.xml";
        if(IsOpen()) {
            wxString fifMask;
            GetLocalWorkspace()->GetSearchInFilesMask(fifMask, findInFilesMask);
            if(fifMask.IsEmpty()) { fifMask = findInFilesMask; }
        }
        return findInFilesMask;
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetProjectFromFile(const wxFileName& filename) const
    {
        wxString filenameFP = filename.GetFullPath();
        CxxWS::ProjectMap_t::const_iterator iter = m_projects.begin();
        for(; iter != m_projects.end(); ++iter) {
            if(iter->second->GetFiles().count(filenameFP)) { return iter->first; }
        }
        return "";
    }
    //----------------------------------------------------------------------------
    void CxxWS::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
    {
        ProjectPtr p = GetProject(projectName.IsEmpty() ? GetActiveProjectName() : projectName);
        CHECK_PTR_RET(p);

        const Project::FilesMap_t& filesMap = p->GetFiles();
        if(filesMap.empty()) { return; }
        files.Alloc(filesMap.size());
        std::for_each(filesMap.begin(), filesMap.end(),
                      [&](const Project::FilesMap_t::value_type& vt) { files.Add(vt.first); });
    }
    //----------------------------------------------------------------------------
    void CxxWS::GetWorkspaceFiles(wxArrayString& files) const
    {
        size_t totalFiles = 0;
        std::for_each(m_projects.begin(), m_projects.end(), [&](const CxxWS::ProjectMap_t::value_type& v) {
            totalFiles += v.second->GetFiles().size();
        });

        if(totalFiles) {
            files.Alloc(totalFiles);
            std::for_each(m_projects.begin(), m_projects.end(), [&](const CxxWS::ProjectMap_t::value_type& v) {
                const Project::FilesMap_t& filesMap = v.second->GetFiles();

                // Convert the set wxArrayString
                std::for_each(filesMap.begin(), filesMap.end(),
                              [&](const Project::FilesMap_t::value_type& vt) { files.Add(vt.first); });
            });
        }
    }
    //----------------------------------------------------------------------------
    WorkspaceConfigurationPtr CxxWS::GetSelectedConfig() const
    {
        if(!GetBuildMatrix()) return NULL;
        wxString buildConf = GetBuildMatrix()->GetSelectedConfigurationName();
        return GetBuildMatrix()->GetConfigurationByName(buildConf);
    }
    //----------------------------------------------------------------------------
    void CxxWS::ClearIncludePathCache()
    {
        std::for_each(m_projects.begin(), m_projects.end(),
                      [&](const CxxWS::ProjectMap_t::value_type& v) { v.second->ClearIncludePathCache(); });
    }
    //----------------------------------------------------------------------------
    void CxxWS::DoUnselectActiveProject()
    {
        if(!m_doc.IsOk()) return;

        std::list<wxXmlNode*> xmls = DoGetProjectsXmlNodes();
        std::for_each(xmls.begin(), xmls.end(), [&](wxXmlNode* node) { XmlUtils::UpdateProperty(node, "Active", "No"); });
    }
    //----------------------------------------------------------------------------
    void CxxWS::DoLoadProjectsFromXml(wxXmlNode* parentNode, const wxString& folder,
                                               std::vector<wxXmlNode*>& removedChildren)
    {
        wxXmlNode* child = parentNode->GetChildren();
        while(child) {
            if(child->GetName() == wxT("Project")) {
                wxString projectPath = child->XmlGetPropVal(wxT("Path"), wxEmptyString);
                wxString errmsg;
                if(!DoAddProject(projectPath, folder, errmsg)) { removedChildren.push_back(child); }
            } else if(child->GetName() == wxT("VirtualDirectory")) {
                // Virtual directory
                wxString currentFolder = folder;
                wxString vdName = child->GetAttribute("Name", wxEmptyString);
                if(!currentFolder.IsEmpty()) { currentFolder << "/"; }
                currentFolder << vdName;
                DoLoadProjectsFromXml(child, currentFolder, removedChildren);
            } else if((child->GetName() == wxT("WorkspaceParserPaths")) ||
                      (child->GetName() == wxT("WorkspaceParserMacros"))) {
                wxString swtlw = XmlUtils::ReadString(m_doc.GetRoot(), "SWTLW");
                if(swtlw.CmpNoCase("yes") == 0) {
                    GetLocalWorkspace()->SetParserFlags(GetLocalWorkspace()->GetParserFlags() |
                                                        LocalWorkspace::EnableSWTLW);
                    SyncToLocalWorkspaceSTParserPaths();
                    SyncToLocalWorkspaceSTParserMacros();
                }
            }
            child = child->GetNext();
        }
    }
    //----------------------------------------------------------------------------
    wxXmlNode * CxxWS::DoGetWorkspaceFolderXmlNode(const wxString& path)
    {
        wxArrayString parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
        if(parts.IsEmpty()) return m_doc.GetRoot();

        wxXmlNode* parent = m_doc.GetRoot();
        for(size_t i = 0; i < parts.size(); ++i) {
            parent = XmlUtils::FindNodeByName(parent, "VirtualDirectory", parts.Item(i));
            if(!parent) return NULL;
        }
        return parent;
    }
    //----------------------------------------------------------------------------
    wxXmlNode * CxxWS::DoCreateWorkspaceFolder(const wxString& path)
    {
        wxXmlNode* node = DoGetWorkspaceFolderXmlNode(path);
        if(node) return node; // already exists

        // Create it
        wxArrayString parts = ::wxStringTokenize(path, "/", wxTOKEN_STRTOK);
        if(parts.IsEmpty()) return m_doc.GetRoot();

        // Starting from the root node
        wxXmlNode* parent = m_doc.GetRoot();
        for(size_t i = 0; i < parts.size(); ++i) {
            wxXmlNode* child = XmlUtils::FindNodeByName(parent, "VirtualDirectory", parts.Item(i));
            if(!child) {
                // add this child
                child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "VirtualDirectory");
                child->AddAttribute("Name", parts.Item(i));
                parent->AddChild(child);
            }
            parent = child;
        }
        return parent;
    }
    //----------------------------------------------------------------------------
    bool CxxWS::MoveProjectToFolder(const wxString& projectName, const wxString& folderPath, bool saveAndReload)
    {
        wxXmlNode* folderXml = DoGetWorkspaceFolderXmlNode(folderPath);
        if(!folderXml) {
            folderXml = DoCreateWorkspaceFolder(folderPath);
            if(!folderXml) { return false; }
        }

        // Locate the project XML node
        wxXmlNode* projectXml = DoGetProjectXmlNode(projectName);
        if(!projectXml || !projectXml->GetParent()) { return false; }

        projectXml->GetParent()->RemoveChild(projectXml);
        folderXml->AddChild(projectXml);

        if(!saveAndReload) return true;

        // Store the XML file and reload the workspace
        if(!SaveXmlFile()) { return false; }

        // Reload the workspace XML file
        wxString errMsg;
        return DoLoadWorkspace(m_fileName.GetFullPath(), errMsg);
    }
    //----------------------------------------------------------------------------
    void CxxWS::DeleteWorkspaceFolder(const wxString& path)
    {
        wxXmlNode* node = DoGetWorkspaceFolderXmlNode(path);
        if(!node || !node->GetParent()) return;

        node->GetParent()->RemoveChild(node);
        wxDELETE(node);

        wxString errMsg;
        DoLoadWorkspace(m_fileName.GetFullPath(), errMsg);
    }
    //----------------------------------------------------------------------------
    bool CxxWS::CreateWorkspaceFolder(const wxString& path) 
    { 
        return (DoCreateWorkspaceFolder(path) != NULL); 
    }
    //----------------------------------------------------------------------------
    bool CxxWS::DoLoadWorkspace(const wxString& fileName, wxString& errMsg)
    {
        CloseWorkspace();
        //m_buildMatrix.Reset(NULL);
        wxFileName workSpaceFile(fileName);
        if(workSpaceFile.FileExists() == false) {
            errMsg = wxString::Format(wxT("Could not open workspace file: '%s'"), fileName.c_str());
            return false;
        }

        m_fileName = workSpaceFile;
        m_doc.Load(m_fileName.GetFullPath());
        if(!m_doc.IsOk()) {
            errMsg = wxT("Corrupted workspace file");
            return false;
        }

        // Make sure we have the WORKSPACE/.gsgs folder exists
        {
            wxLogNull nolog;
            wxMkdir(GetPrivateFolder());
        }

        SetWorkspaceLastModifiedTime(GetFileLastModifiedTime());

        // This function sets the working directory to the workspace directory!
        ::wxSetWorkingDirectory(m_fileName.GetPath());

        // Load all projects from the XML file
        std::vector<wxXmlNode*> removedChildren;
        DoLoadProjectsFromXml(m_doc.GetRoot(), wxEmptyString, removedChildren);

        // Delete the faulty projects
        for(size_t i = 0; i < removedChildren.size(); i++) {
            wxXmlNode* ch = removedChildren.at(i);
            ch->GetParent()->RemoveChild(ch);
            wxDELETE(ch);
        }

        errMsg.Clear();
        TagsManager* mgr = TagsManagerST::Get();
        mgr->CloseDatabase();
        mgr->OpenDatabase(GetTagsFileName().GetFullPath());

        // Update the build matrix
        DoUpdateBuildMatrix();
        return true;
    }
    //----------------------------------------------------------------------------
    wxXmlNode* CxxWS::DoGetProjectXmlNode(const wxString& projectName)
    {
        std::list<wxXmlNode*> xmls = DoGetProjectsXmlNodes();
        std::list<wxXmlNode*>::iterator iter = std::find_if(xmls.begin(), xmls.end(), [&](wxXmlNode* node) {
            return (node->GetAttribute("Name", wxEmptyString) == projectName);
        });

        if(iter == xmls.end()) return NULL;
        return (*iter);
    }
    //----------------------------------------------------------------------------
    std::list<wxXmlNode*> CxxWS::DoGetProjectsXmlNodes() const
    {
        std::list<wxXmlNode*> queue;
        queue.push_back(m_doc.GetRoot());

        std::list<wxXmlNode*> projectsXmls;

        while(!queue.empty()) {
            wxXmlNode* node = queue.back();
            queue.pop_back();

            wxXmlNode* child = node->GetChildren();
            while(child) {
                if(child->GetName() == "VirtualDirectory") {
                    queue.push_back(child);
                } else if(child->GetName() == "Project") {
                    projectsXmls.push_back(child);
                }
                child = child->GetNext();
            }
        }
        return projectsXmls;
    }
    //----------------------------------------------------------------------------
    void CxxWS::DoVisitWorkspaceFolders(wxXmlNode* parent, const wxString& curpath, wxArrayString& paths) const
    {
        if((XmlUtils::FindFirstByTagName(parent, "VirtualDirectory") == NULL) && !curpath.IsEmpty()) {
            paths.Add(curpath);
            return;
        }

        wxXmlNode* child = parent->GetChildren();
        while(child) {
            if(child->GetName() == "VirtualDirectory") {
                wxString tmppath = curpath;
                if(!tmppath.IsEmpty()) { tmppath << "/"; }
                tmppath << child->GetAttribute("Name", "");
                DoVisitWorkspaceFolders(child, tmppath, paths);
            }
            child = child->GetNext();
        }
    }
    //----------------------------------------------------------------------------
    wxString CxxWS::GetVersion() const
    {
        if(!m_doc.IsOk() || m_doc.GetRoot()) return wxEmptyString;
        return m_doc.GetRoot()->GetAttribute("Version");
    }
    //----------------------------------------------------------------------------
    wxFileName CxxWS::GetProjectFileName(const wxString& projectName) const
    {
        ProjectPtr p = GetProject(projectName);
        if(p)
            return p->GetFileName();
        return wxFileName();
    }
    //----------------------------------------------------------------------------
    wxArrayString CxxWS::GetWorkspaceProjects() const
    {
        wxArrayString projects;
        GetProjectList(projects);
        return projects;
    }
    //----------------------------------------------------------------------------
    size_t CxxWS::GetExcludeFilesForConfig(std::vector<wxString>& files, const wxString& workspaceConfigName)
    {
        std::for_each(m_projects.begin(), m_projects.end(), [&](const ProjectMap_t::value_type& vt) {
            ProjectPtr proj = vt.second;
            BuildConfigPtr conf = GetProjBuildConf(proj->GetName(), workspaceConfigName);
            if(conf) {
                const wxString& confname = conf->GetName();
                const gsgsStringSet& excludeFiles = proj->GetExcludeFiles();
                for(const wxString& filename : excludeFiles) {
                    clProjectFile::Ptr_t file = proj->GetFile(filename);
                    if(file && file->IsExcludeFromConfiguration(confname)) { files.push_back(filename); }
                }
            }
        });
        return files.size();
    }
    //----------------------------------------------------------------------------
    void CxxWS::CreateCompileFlags() const
    {
        // Build the global compiler paths, we will need this later on...
        gsgsStringMap compilersGlobalPaths;
        std::unordered_map<wxString, wxArrayString> pathsMap = gsgs_Settings().GetCompilersGlobalPaths();
        for(const std::unordered_map<wxString, wxArrayString>::value_type& vt : pathsMap) {
            wxString compiler_name = vt.first;
            wxArrayString pathsArr = vt.second;
            wxString paths;
            std::for_each(pathsArr.begin(), pathsArr.end(), [&](wxString& path) {
                path.Trim().Trim(false);
                if(path.EndsWith("\\")) { path.RemoveLast(); }
                paths << path << ";";
            });
            compilersGlobalPaths.insert({ compiler_name, paths });
        }

        for(const ProjectMap_t::value_type& vt : m_projects) {
            vt.second->CreateCompileFlags(compilersGlobalPaths);
        }
    }
    //----------------------------------------------------------------------------
}