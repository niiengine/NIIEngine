#include "gsgsPathManager.h"
#include "lintoptions.h"
#ifndef __WXMSW__
#include "globals.h"
#endif
using namespace gsgs;
LintOptions::LintOptions()
    : ObjectData("phplint")
    , m_lintOnFileLoad(false)
    , m_lintOnFileSave(true)
    , m_phpcsPhar("")
    , m_phpmdPhar("")
    , m_phpmdRules("")
    , m_phpstanPhar("")
{
    wxFileName newConfigFile = gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                               gsgsPathSep + "phplint.conf";
    if(!newConfigFile.FileExists()) {
        wxFileName oldConfigFile = gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                                   gsgsPathSep + "php.conf";

        // first time, copy the values from the old settings
        JSON root(oldConfigFile);
        JSONObject oldJson = root.getObject().getObject("PHPConfigurationData");
        size_t m_flags;
        oldJson.getObject("m_flags").getValue(m_flags);
        m_flags &= (1 << 1);
        m_lintOnFileSave = m_flags & (1 << 1);

        // Save it
        JSON newRoot(newConfigFile);
        JSONObject e = JSONObject::createObject(GetName());
        e.addProperty("lintOnFileSave", m_lintOnFileSave);
        newRoot.getObject().arrayAppend(e);
        newRoot.save(newConfigFile);
    }
}

LintOptions::~LintOptions() {}

void LintOptions::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("lintOnFileLoad").getValue(m_lintOnFileLoad, m_lintOnFileLoad);
    json.getObject("lintOnFileSave").getValue(m_lintOnFileSave, m_lintOnFileSave);
    json.getObject("phpcsPhar").getValue(m_phpcsPhar, m_phpcsPhar);
    json.getObject("phpmdPhar").getValue(m_phpmdPhar, m_phpmdPhar);
    json.getObject("phpmdRules").getValue(m_phpmdRules, m_phpmdRules);
    json.getObject("phpstanPhar").getValue(m_phpstanPhar, m_phpstanPhar);

#ifndef __WXMSW__
    // Find an installed version of phpcs
    if(m_phpcsPhar.IsEmpty()) {
        wxFileName phpcsFile;
        ::FindExec("phpcs", phpcsFile);
        SetPhpcsPhar(phpcsFile);
    }

    // Find an installed version of phpmd
    if(m_phpmdPhar.IsEmpty()) {
        wxFileName phpmdFile;
        ::FindExec("phpmd", phpmdFile);
        SetPhpmdPhar(phpmdFile);
    }

    // Find an installed version of phpstan
    if(m_phpstanPhar.IsEmpty()) {
        wxFileName phpstanFile;
        ::FindExec("phpstan", phpstanFile);
        SetPhpstanPhar(phpstanFile);
    }
#endif
}

gsgs::JSONObject LintOptions::ToJSON() const
{
    JSONObject element = JSONObject::createObject(GetName());
    element.addProperty("lintOnFileLoad", m_lintOnFileLoad);
    element.addProperty("lintOnFileSave", m_lintOnFileSave);
    element.addProperty("phpcsPhar", m_phpcsPhar);
    element.addProperty("phpmdPhar", m_phpmdPhar);
    element.addProperty("phpmdRules", m_phpmdRules);
    element.addProperty("phpstanPhar", m_phpstanPhar);
    return element;
}

LintOptions& LintOptions::Load()
{
    gsgs::Config config("phplint.conf");
    config.ReadItem(this);
    return *this;
}

LintOptions& LintOptions::Save()
{
    gsgs::Config config("phplint.conf");
    config.WriteItem(this);
    return *this;
}
