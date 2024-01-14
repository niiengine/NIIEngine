/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |
                                            \___/


                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#include "gsgsBuilderGnuMake.h"
#include "gsgsBuildConfig.h"
#include "InnerCompilerLocator.h"
#include "gsgsJSON.h"
#include "gsgsEditorConfig.h"

#include "gsgsWorkspace.h"
#include "project.h"
#include "gsgsFileUtil.h"
#include "gsgsLogManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsCommon.h"
#include "globals.h"
#include "gsgsMacroManager.h"
#include "CxxPreProcessor.h"
#include "xmlutils.h"

#include <wx/regex.h>
#include <wx/choicdlg.h>
#include <wx/stream.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/app.h>
#include <wx/stopwatch.h>

namespace gsgs
{
    //---------------------------------------------------------------------------
    static bool OS_WINDOWS = wxGetOsVersion() & wxOS_WINDOWS ? true : false;
    //---------------------------------------------------------------------------
    static wxString ReplaceBackslashes(const wxString& instr)
    {
        wxString tmpstr = instr;
        tmpstr.Trim().Trim(false);

        if (tmpstr.StartsWith("\"")) {
            tmpstr = tmpstr.Mid(1);
            wxString strBefore = tmpstr.BeforeFirst('"');
            wxString strAfter = tmpstr.AfterFirst('"');
            strBefore.Prepend("\"").Append("\""); // restore the " that we removed
            strBefore.Replace("/", "\\");
            strBefore << strAfter;
            tmpstr.swap(strBefore);
        }
        else {
            wxString strBefore = tmpstr.BeforeFirst(' ');
            wxString strAfter = tmpstr.AfterFirst(' ');
            strAfter.Prepend(" ");
            strBefore.Replace("/", "\\");
            strBefore << strAfter;
            tmpstr.swap(strBefore);
        }
        return tmpstr;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // Builder
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    Builder::Builder(const wxString& name)
        : m_name(name)
        , m_isActive(false)
    {
        // Override values from configuration file
        BuilderConfigPtr config = gsgs_Settings().GetBuilderConfig(m_name);
        if (config) {
            m_isActive = config->GetIsActive();

        }
        else {
            m_isActive = (m_name == "Default");
        }
    }
    //---------------------------------------------------------------------------
    Builder::~Builder() {}
    //---------------------------------------------------------------------------
    wxString Builder::NormalizeConfigName(const wxString& confgName)
    {
        wxString normalized(confgName);
        TrimString(normalized);
        normalized.Replace(wxT(" "), wxT("_"));
        return normalized;
    }
    //---------------------------------------------------------------------------
    void Builder::SetActive()
    {
        std::list<wxString> builders;
        gsgs_Build().GetBuilders(builders);
        std::list<wxString>::iterator iter = builders.begin();
        for (; iter != builders.end(); iter++) {

            wxString builderName = *iter;
            BuilderPtr builder = gsgs_Build().GetBuilder(builderName);

            if (builder && builder->m_name == m_name)
                builder->m_isActive = true;

            else if (builder)
                builder->m_isActive = false;
        }
    }
    //---------------------------------------------------------------------------
    static wxString GetMakeDirCmd(BuildConfigPtr bldConf, const wxString& relPath = wxEmptyString)
    {
        wxString intermediateDirectory(bldConf->GetIntermediateDirectory());
        wxString relativePath(relPath);

        intermediateDirectory.Replace(wxT("\\"), wxT("/"));
        intermediateDirectory.Trim().Trim(false);
        if (intermediateDirectory.StartsWith(wxT("./")) && relativePath == wxT("./")) {
            relativePath.Clear();
        }

        if (intermediateDirectory.StartsWith(wxT("./")) && relativePath.IsEmpty() == false) {
            intermediateDirectory = intermediateDirectory.Mid(2);
        }

        wxString text;
        if (OS_WINDOWS) {
            text << wxT("@$(MakeDirCommand) \"") << relativePath << intermediateDirectory << wxT("\"");
        }
        else {
            // other OSs
            text << wxT("@test -d ") << relativePath << intermediateDirectory << wxT(" || $(MakeDirCommand) ")
                << relativePath << intermediateDirectory;
        }
        return text;
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // BuilderNMake
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    BuilderNMake::BuilderNMake()
        : Builder(wxT("NMakefile for MSVC toolset"))
        , m_objectChunks(1)
    {
    }
    //---------------------------------------------------------------------------
    BuilderNMake::BuilderNMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions)
        : Builder(name)
    {
    }
    //---------------------------------------------------------------------------
    BuilderNMake::~BuilderNMake() {}
    //---------------------------------------------------------------------------
    bool BuilderNMake::Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
        bool isProjectOnly, bool force, wxString& errMsg)
    {
        if (project.IsEmpty()) {
            return false;
        }
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            errMsg << _("Cant open project '") << project << wxT("'");
            return false;
        }

        // get the selected build configuration
        wxString bld_conf_name(confToBuild);

        if (confToBuild.IsEmpty()) {
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
            if (!bldConf) {
                errMsg << _("Cant find build configuration for project '") << project << wxT("'");
                return false;
            }
            bld_conf_name = bldConf->GetName();
        }

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, bld_conf_name);
        if (!bldConf) {
            errMsg << _("Cant find build configuration for project '") << project << wxT("'");
            return false;
        }
        if (!bldConf->GetCompiler()) {
            errMsg << _("Cant find proper compiler for project '") << project << wxT("'");
            return false;
        }

        wxArrayString depsArr = proj->GetDependencies(bld_conf_name);

        gsgs_Debug("Generating Makefile...");
        // Filter all disabled projects from the dependencies array
        wxArrayString updatedDepsArr;
        for (size_t i = 0; i < depsArr.GetCount(); ++i) {
            wxString errmsg;
            ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errmsg);
            if (!dependProj) {
                continue;
            }

            wxString projectSelConf = gsgs_CppWorker().GetBuildMatrix()->GetProjectSelectedConf(
                gsgs_CppWorker().GetBuildMatrix()->GetSelectedConfigurationName(), dependProj->GetName());
            BuildConfigPtr dependProjbldConf =
                gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
            if (dependProjbldConf && dependProjbldConf->IsProjectEnabled()) {
                updatedDepsArr.Add(depsArr.Item(i));
            }
        }

        // Update the dependencies with the correct list
        depsArr.swap(updatedDepsArr);

        wxArrayString removeList;
        if (!isProjectOnly) {
            // this function assumes that the working directory is located at the workspace path
            // make sure that all dependencies exists
            for (size_t i = 0; i < depsArr.GetCount(); i++) {
                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                // Missing dependencies project?
                // this can happen if a project was removed from the workspace, but
                // is still on the depdendencie list of this project
                if (!dependProj) {
                    wxString msg;
                    msg << _("gsgs can not find project '") << depsArr.Item(i) << _("' which is required\n");
                    msg << _("for building project '") << project
                        << _("'.\nWould you like to remove it from the dependency list?");
                    if (wxMessageBox(msg, _("光栅故事"), wxYES_NO | wxICON_QUESTION) == wxYES) {
                        // remove the project from the dependecie list, and continue
                        removeList.Add(depsArr.Item(i));
                    }
                }
            }
            bool settingsChanged(false);
            // remove the unfound projects from the dependencies array
            for (size_t i = 0; i < removeList.GetCount(); i++) {
                int where = depsArr.Index(removeList.Item(i));
                if (where != wxNOT_FOUND) {
                    depsArr.RemoveAt(where);
                    settingsChanged = true;
                }
            }
            // update the project dependencies
            bool modified = proj->IsModified();

            // Update the dependencies only if needed
            if (settingsChanged) proj->SetDependencies(depsArr, bld_conf_name);

            // the set settings functions marks the project as 'modified' this causes
            // an unneeded makefile generation if the settings was not really modified
            if (!modified && !settingsChanged) {
                proj->SetModified(false);
            }
        }

        wxString fn;
        fn << "Makefile";
        wxString text;

        wxFileName wspfile(gsgs_CppWorker().GetFileName());

        text << wxT("All:\n");

        // iterate over the dependencies projects and generate makefile
        wxString buildTool = GetBuildToolCommand(project, confToBuild, "", false);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        // buildTool.Replace(wxT("\\"), wxT("/"));

        // generate the makefile for the selected workspace configuration
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
        wxArrayString depsProjs;
        if (!isProjectOnly) {
            for (size_t i = 0; i < depsArr.GetCount(); ++i) {
                bool isCustom(false);
                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                if (!dependProj) {
                    continue;
                }

                wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, dependProj->GetName());
                BuildConfigPtr dependProjbldConf =
                    gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
                if (dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
                    isCustom = true;
                }

                // incase we manually specified the configuration to be built, set the project
                // as modified, so on next attempt to build it, gsgs will sync the configuration
                if (confToBuild.IsEmpty() == false) {
                    dependProj->SetModified(true);
                }

                if (!dependProjbldConf->IsProjectEnabled()) {
                    // Ignore disabled projects
                    continue;
                }

                text << wxT("\t@echo \"") << wxGetTranslation(BUILD_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ")
                    << projectSelConf << wxT(" ]----------\"\n");
                // make the paths relative, if it's sensible to do so
                wxFileName fn(dependProj->GetFileName());
                MakeRelativeIfSensible(fn, wspfile.GetPath());

                bool isPluginGeneratedMakefile =
                    SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, depsArr.Item(i), projectSelConf);

                // we handle custom builds and non-custom build separatly:
                if (isPluginGeneratedMakefile) {

                    // this project makefile is generated by a plugin
                    // query the plugin about the build command
                    gsgs::BuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
                    e.SetProjectName(depsArr.Item(i));
                    e.SetConfigurationName(projectSelConf);
                    e.SetProjectOnly(false);
                    gsgs_Event().ProcessEvent(e);
                    text << wxT("\t") << e.GetCommand() << wxT("\n");

                }
                else if (isCustom) {

                    CreateCustomPreBuildEvents(dependProjbldConf, text);

                    wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                    wxString build_cmd = dependProjbldConf->GetCustomBuildCmd();
                    wxString customWdCmd;

                    customWd = ExpandAllVariables(customWd, &gsgs_CppWorker(), dependProj->GetName(),
                        dependProjbldConf->GetName(), wxEmptyString);
                    build_cmd = ExpandAllVariables(build_cmd, &gsgs_CppWorker(), dependProj->GetName(),
                        dependProjbldConf->GetName(), wxEmptyString);

                    build_cmd.Trim().Trim(false);

                    if (build_cmd.empty()) {
                        build_cmd << wxT("@echo Project has no custom build command!");
                    }

                    // if a working directory is provided apply it, otherwise use the project
                    // path
                    customWd.Trim().Trim(false);
                    if (customWd.empty() == false) {
                        customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
                    }
                    else {
                        customWdCmd << GetCdCmd(wspfile, fn);
                    }

                    text << wxT("\t") << customWdCmd << build_cmd << wxT("\n");
                    CreateCustomPostBuildEvents(dependProjbldConf, text);

                }
                else {
                    // generate the dependency project makefile
                    wxString dep_file = DoGetMarkerFileDir(wxT(""));
                    dep_file << gsgsPathSep << dependProj->GetName();
                    depsProjs.Add(dep_file);

                    GenerateMakefile(dependProj, projectSelConf, confToBuild.IsEmpty() ? force : true, wxArrayString());
                    text << GetProjectMakeCommand(wspfile, fn, dependProj, projectSelConf);
                }
            }
        }

        // Generate makefile for the project itself
        GenerateMakefile(proj, confToBuild, confToBuild.IsEmpty() ? force : true, depsProjs);

        // incase we manually specified the configuration to be built, set the project
        // as modified, so on next attempt to build it, gsgs will sync the configuration
        if (confToBuild.IsEmpty() == false) {
            proj->SetModified(true);
        }

        wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
        if (isProjectOnly && confToBuild.IsEmpty() == false) {
            // incase we use to generate a 'Project Only' makefile,
            // we allow the caller to override the selected configuration with 'confToBuild' parameter
            projectSelConf = confToBuild;
        }

        text << wxT("\t@echo \"") << wxGetTranslation(BUILD_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf
            << wxT(" ]----------\"\n");

        // make the paths relative, if it's sensible to do so
        wxFileName projectPath(proj->GetFileName());
        MakeRelativeIfSensible(projectPath, wspfile.GetPath());

        wxString pname(proj->GetName());

        bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);
        if (isPluginGeneratedMakefile) {

            wxString cmd;
            gsgs::BuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
            e.SetProjectName(pname);
            e.SetConfigurationName(projectSelConf);
            e.SetProjectOnly(isProjectOnly);
            gsgs_Event().ProcessEvent(e);

            cmd = e.GetCommand();
            text << wxT("\t") << cmd << wxT("\n");

        }
        else {
            text << GetProjectMakeCommand(wspfile, projectPath, proj, projectSelConf);
        }

        // create the clean target
        text << wxT("clean:\n");
        if (!isProjectOnly) {
            for (size_t i = 0; i < depsArr.GetCount(); i++) {
                bool isCustom(false);
                wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, depsArr.Item(i));

                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                // Missing dependencies project? just skip it
                if (!dependProj) {
                    continue;
                }

                text << wxT("\t@echo \"") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ")
                    << projectSelConf << wxT(" ]----------\"\n");

                // make the paths relative
                wxFileName fn(dependProj->GetFileName());
                MakeRelativeIfSensible(fn, wspfile.GetPath());

                // if the dependencie project is project of type 'Custom Build' - do the custom build instead
                // of the geenrated makefile
                BuildConfigPtr dependProjbldConf =
                    gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
                if (dependProjbldConf && dependProjbldConf->IsCustomBuild()) {
                    isCustom = true;
                }

                // if we are using custom build command, invoke it instead of calling the generated makefile
                wxString pname = dependProj->GetName();
                bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);

                // we handle custom builds and non-custom build separately:
                if (isPluginGeneratedMakefile) {

                    // this project makefile is generated by a plugin
                    // query the plugin about the build command
                    gsgs::BuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
                    e.SetProjectOnly(false);
                    e.SetProjectName(pname);
                    e.SetConfigurationName(projectSelConf);
                    gsgs_Event().ProcessEvent(e);
                    text << wxT("\t") << e.GetCommand() << wxT("\n");

                }
                else if (!isCustom) {

                    text << wxT("\t") << GetCdCmd(wspfile, fn) << buildTool << wxT(" \"") << dependProj->GetName()
                        << wxT(".mk\"  clean\n");

                }
                else {

                    wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                    wxString clean_cmd = dependProjbldConf->GetCustomCleanCmd();

                    customWd = ExpandAllVariables(customWd, &gsgs_CppWorker(), dependProj->GetName(),
                        dependProjbldConf->GetName(), wxEmptyString);
                    clean_cmd = ExpandAllVariables(clean_cmd, &gsgs_CppWorker(), dependProj->GetName(),
                        dependProjbldConf->GetName(), wxEmptyString);

                    wxString customWdCmd;

                    clean_cmd.Trim().Trim(false);
                    if (clean_cmd.empty()) {
                        clean_cmd << wxT("@echo Project has no custom clean command!");
                    }

                    // if a working directory is provided apply it, otherwise use the project
                    // path
                    customWd.Trim().Trim(false);
                    if (customWd.empty() == false) {
                        customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
                    }
                    else {
                        customWdCmd << GetCdCmd(wspfile, fn);
                    }
                    text << wxT("\t") << customWdCmd << clean_cmd << wxT("\n");
                }
            }
        }

        // generate makefile for the project itself
        projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
        if (isProjectOnly && confToBuild.IsEmpty() == false) {
            // incase we use to generate a 'Project Only' makefile,
            // we allow the caller to override the selected configuration with 'confToBuild' parameter
            projectSelConf = confToBuild;
        }

        text << wxT("\t@echo \"") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf
            << wxT(" ]----------\"\n");
        if (isPluginGeneratedMakefile) {

            wxString cmd;

            gsgs::BuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
            e.SetProjectName(pname);
            e.SetConfigurationName(projectSelConf);
            e.SetProjectOnly(isProjectOnly);
            gsgs_Event().ProcessEvent(e);
            cmd = e.GetCommand();
            text << wxT("\t") << cmd << wxT("\n");

        }
        else {
            text << wxT("\t") << GetCdCmd(wspfile, projectPath) << buildTool << wxT(" \"") << proj->GetName()
                << wxT(".mk\" clean\n");
        }

        text << wxT(".PHONY: clean All\n\n");
        // dump the content to file
        wxFileOutputStream output(fn);
        wxStringInputStream content(text);
        output << content;

        gsgs_Debug("Generating Makefile...is completed");
        return true;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force, const wxArrayString& depsProj)
    {
        wxString pname(proj->GetName());
        wxString tmpConfigName(confToBuild.c_str());
        if (confToBuild.IsEmpty()) {
            BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
            tmpConfigName = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), proj->GetName());
        }

        bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, tmpConfigName);

        // we handle custom builds and non-custom build separatly:
        if (isPluginGeneratedMakefile) {
            if (force) {
                // Generate the makefile
                SendBuildEvent(wxEVT_PLUGIN_EXPORT_MAKEFILE, pname, tmpConfigName);
            }
            return;
        }

        ProjectSettingsPtr settings = proj->GetSettings();
        if (!settings) {
            return;
        }

        // get the selected build configuration for this project
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        if (!bldConf) {
            return;
        }

        wxString path = proj->GetFileName().GetPath();

        DirSaver ds;
        // change directory to the project base dir
        ::wxSetWorkingDirectory(path);

        // create new makefile file
        wxString fn(path);
        fn << gsgsPathSep << proj->GetName() << wxT(".mk");

        // skip the next test if the makefile does not exist
        if (wxFileName::FileExists(fn)) {
            if (!force) {
                if (proj->IsModified() == false) {
                    return;
                }
            }
        }

        // Load the current project files
        m_allFiles.clear();
        proj->GetFilesAsVector(m_allFiles);

        // generate the selected configuration for this project
        // wxTextOutputStream text(output);
        wxString text;

        text << wxT("##") << wxT("\n");
        text << wxT("## Auto Generated makefile by gsgs IDE") << wxT("\n");
        text << wxT("## any manual changes will be erased      ") << wxT("\n");
        text << wxT("##") << wxT("\n");

        // Create the makefile variables
        CreateConfigsVariables(proj, bldConf, text);

        //----------------------------------------------------------
        // copy environment variables to the makefile
        // We put them after the 'hard-coeded' ones
        // so user will be able to override any of the default
        // variables by defining its own
        //----------------------------------------------------------
        EvnVarList vars;
        gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
        EnvMap varMap = vars.GetVariables(wxT(""), true, proj->GetName(), bldConf->GetName());

        text << wxT("##") << wxT("\n");
        text << wxT("## User defined environment variables") << wxT("\n");
        text << wxT("##") << wxT("\n");

        for (size_t i = 0; i < varMap.GetCount(); i++) {
            wxString name, value;
            varMap.Get(i, name, value);
            text << name << wxT("=") << value << wxT("") << wxT("\n");
        }

        CreateListMacros(proj, confToBuild, text); // list of srcs and list of objects

        //-----------------------------------------------------------
        // create the build targets
        //-----------------------------------------------------------
        text << wxT("##\n");
        text << wxT("## Main Build Targets \n");
        text << wxT("##\n");

        // incase project is type exe or dll, force link
        // this is to workaround bug in the generated makefiles
        // which causes the makefile to report 'nothing to be done'
        // even when a dependency was modified
        wxString targetName(bldConf->GetIntermediateDirectory());
        CreateLinkTargets(proj->GetSettings()->GetProjectType(bldConf->GetName()), bldConf, text, targetName,
            proj->GetName(), depsProj);

        CreatePostBuildEvents(proj, bldConf, text);
        CreateMakeDirsTarget(proj, bldConf, targetName, text);
        CreatePreBuildEvents(proj, bldConf, text);
        CreatePreCompiledHeaderTarget(bldConf, text);

        //-----------------------------------------------------------
        // Create a list of targets that should be built according to
        // projects' file list
        //-----------------------------------------------------------
        CreateFileTargets(proj, confToBuild, text);
        CreateCleanTargets(proj, confToBuild, text);

        // dump the content to a file
        wxFFile output;
        output.Open(fn, wxT("w+"));
        if (output.IsOpened()) {
            output.Write(text);
            output.Close();
        }

        // mark the project as non-modified one
        proj->SetModified(false);
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName,
        wxString& text)
    {
        text << "\n";
        text << "MakeIntermediateDirs:\n";
        text << "\t" << GetMakeDirCmd(bldConf) << "\n\n";

        text << wxT("\n");
        text << targetName << wxT(":\n");
        text << wxT("\t") << wxT("@echo Creating Intermediate Directory\n");
        text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
        text << wxT("\t") << wxT("@echo Intermediate directories created");
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        std::vector<wxFileName> files;
        const Project::FilesMap_t& all_files = proj->GetFiles();

        // Remove excluded files
        std::for_each(all_files.begin(), all_files.end(), [&](const Project::FilesMap_t::value_type& vt) {
            clProjectFile::Ptr_t f = vt.second;
            if (!f->IsExcludeFromConfiguration(confToBuild)) {
                files.push_back(wxFileName(f->GetFilenameRelpath()));
            }
        });

        text << wxT("Srcs=");

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        wxString relPath;

        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        int counter = 1;
        Compiler::CmpFileTypeInfo ft;

        for (size_t i = 0; i < files.size(); i++) {

            // is this a valid file?
            if (!cmp->GetCmpFileType(files[i].GetExt(), ft)) continue;

            if (ft.kind == Compiler::CmpFileKindResource) {
    #ifndef __WXMSW__
                // Resource compiler "windres" is not supported under
                // *nix OS
                continue;
    #endif
            }

            relPath = files.at(i).GetPath(true, wxPATH_UNIX);
            relPath.Trim().Trim(false);

            // Compiler::CmpFileKindSource , Compiler::CmpFileKindResource file
            text << relPath << files[i].GetFullName() << wxT(" ");

            if (counter % 10 == 0) {
                text << wxT("\\\n\t");
            }
            counter++;
        }
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        m_objectChunks = 1;
        std::vector<wxFileName> files;

        std::for_each(m_allFiles.begin(), m_allFiles.end(), [&](clProjectFile::Ptr_t file) {
            // Include only files that don't have the 'exclude from build' flag set
            if (!file->IsExcludeFromConfiguration(confToBuild)) {
                files.push_back(wxFileName(file->GetFilename()));
            }
        });

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();

        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        int counter = 1;
        Compiler::CmpFileTypeInfo ft;
        wxString projectPath = proj->GetFileName().GetPath();

        wxString objectsList;
        size_t objCounter = 0;
        int numOfObjectsInCurrentChunk = 0;
        wxString curChunk;

        // We break the list of files into a seriese of objects variables
        // each variable contains up to 100 files.
        // This is needed because on MSW, the ECHO command can not handle over 8K bytes
        // and gsgs uses the ECHO command to create a file with all the objects
        // so we can pass it to the liker
        for (size_t i = 0; i < files.size(); ++i) {

            // Did we collect 100 objects yet?
            if (i && ((i % 100) == 0)) {

                // only if this chunk contains objects (even one), add it to the makefile
                // otherwise, clear it and continue collecting
                if (numOfObjectsInCurrentChunk) {
                    curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
                    curChunk << "\n\n";
                    text << curChunk;
                    objCounter++;
                }

                // start a new chunk
                curChunk.clear();
                numOfObjectsInCurrentChunk = 0;
            }

            // is this a valid file?
            if (!cmp->GetCmpFileType(files[i].GetExt(), ft)) continue;

            if (ft.kind == Compiler::CmpFileKindResource && !OS_WINDOWS) {
                continue;
            }

            wxString objPrefix = DoGetTargetPrefix(files.at(i), projectPath, cmp);
            curChunk << wxT("$(IntermediateDirectory)\\") << objPrefix << files[i].GetFullName() << wxT("$(ObjectSuffix) ");

            // for readability, break every 10 objects and start a new line
            if (counter % 10 == 0) {
                curChunk << wxT("\\\n\t");
            }
            counter++;
            numOfObjectsInCurrentChunk++;
        }

        text << wxT("##\n");
        text << wxT("## Object Targets Lists \n");
        text << wxT("##\n");

        // Add any leftovers...
        if (numOfObjectsInCurrentChunk) {
            curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
            curChunk << "\n\n";
            text << curChunk;
            objCounter++;
        }

        text << "\n\nObjects=";
        for (size_t i = 0; i < objCounter; ++i) text << "$(Objects" << i << ") ";

        text << wxT("\n\n");
        m_objectChunks = objCounter;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // get the project specific build configuration for the workspace active
        // configuration
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);
        bool generateDependenciesFiles = cmp->GetGenerateDependeciesFile() && !cmp->GetDependSuffix().IsEmpty();
        bool supportPreprocessOnlyFiles =
            !cmp->GetSwitch(wxT("PreprocessOnly")).IsEmpty() && !cmp->GetPreprocessSuffix().IsEmpty();

        std::vector<wxFileName> abs_files, rel_paths;

        abs_files.reserve(m_allFiles.size());
        rel_paths.reserve(m_allFiles.size());

        clProjectFile::Vec_t::const_iterator iterFile = m_allFiles.begin();
        for (; iterFile != m_allFiles.end(); ++iterFile) {
            // Include only files that don't have the 'exclude from build' flag set
            if (!(*iterFile)->IsExcludeFromConfiguration(confToBuild)) {
                abs_files.push_back(wxFileName((*iterFile)->GetFilename()));
                rel_paths.push_back(wxFileName((*iterFile)->GetFilenameRelpath()));
            }
        }

        text << wxT("\n\n");
        // create rule per object
        text << wxT("##\n");
        text << wxT("## Objects\n");
        text << wxT("##\n");

        Compiler::CmpFileTypeInfo ft;

        // Collect all the sub-directories that we generate files for
        wxArrayString subDirs;

        wxString cwd = proj->GetFileName().GetPath();

        for (size_t i = 0; i < abs_files.size(); i++) {
            // is this file interests the compiler?
            if (cmp->GetCmpFileType(abs_files.at(i).GetExt().Lower(), ft)) {
                wxString absFileName;
                wxFileName fn(abs_files.at(i));

                wxString filenameOnly = fn.GetName();
                wxString fullpathOnly = fn.GetFullPath();
                wxString fullnameOnly = fn.GetFullName();
                wxString compilationLine = ft.compilation_line;

                // use UNIX style slashes
                absFileName = abs_files[i].GetFullPath();
                // absFileName.Replace(wxT("\\"), wxT("/"));
                wxString relPath;

                relPath = rel_paths.at(i).GetPath(true, wxPATH_UNIX);
                relPath.Trim().Trim(false);

                wxString objPrefix = DoGetTargetPrefix(abs_files.at(i), cwd, cmp);

                compilationLine.Replace(wxT("$(FileName)"), filenameOnly);
                compilationLine.Replace(wxT("$(FileFullName)"), fullnameOnly);
                compilationLine.Replace(wxT("$(FileFullPath)"), fullpathOnly);
                compilationLine.Replace(wxT("$(FilePath)"), relPath);

                // The object name is handled differently when using resource files
                compilationLine.Replace(wxT("$(ObjectName)"), objPrefix + fullnameOnly);
                // compilationLine.Replace(wxT("\\"), wxT("/"));

                if (ft.kind == Compiler::CmpFileKindSource) {
                    wxString objectName;
                    wxString dependFile;
                    wxString preprocessedFile;

                    bool isCFile = FileUtil::GetType(rel_paths.at(i).GetFullName()) == FileUtil::TypeSourceC;

                    objectName << wxT("$(IntermediateDirectory)\\") << objPrefix << fullnameOnly << wxT("$(ObjectSuffix)");
                    if (generateDependenciesFiles) {
                        dependFile << wxT("$(IntermediateDirectory)\\") << objPrefix << fullnameOnly
                            << wxT("$(DependSuffix)");
                    }
                    if (supportPreprocessOnlyFiles) {
                        preprocessedFile << wxT("$(IntermediateDirectory)\\") << objPrefix << fullnameOnly
                            << wxT("$(PreprocessSuffix)");
                    }

                    if (!isCFile) {
                        // Add the PCH include line
                        compilationLine.Replace(wxT("$(CXX)"), wxT("$(CXX) $(IncludePCH)"));
                    }

                    // set the file rule
                    text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_WIN) << wxT(" ") << dependFile
                        << wxT("\n");
                    text << wxT("\t") << compilationLine << wxT("\n");

                    wxString cmpOptions(wxT("$(CXXFLAGS) $(IncludePCH)"));
                    if (isCFile) {
                        cmpOptions = wxT("$(CFLAGS)");
                    }

                    // set the source file we want to compile
                    wxString source_file_to_compile = rel_paths.at(i).GetFullPath(wxPATH_WIN);

                    wxString compilerMacro = DoGetCompilerMacro(rel_paths.at(i).GetFullPath(wxPATH_WIN));
                    if (generateDependenciesFiles) {
                        text << dependFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
                        text << wxT("\t") << wxT("@") << compilerMacro << wxT(" ") << cmpOptions
                            << wxT(" $(IncludePath) -MG -MP -MT") << objectName << wxT(" -MF") << dependFile
                            << wxT(" -MM \"") << source_file_to_compile << wxT("\"\n\n");
                    }

                    if (supportPreprocessOnlyFiles) {
                        text << preprocessedFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
                        text << wxT("\t") << compilerMacro << wxT(" ") << cmpOptions
                            << wxT(" $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ") << preprocessedFile
                            << wxT(" \"") << source_file_to_compile << wxT("\"\n\n");
                    }

                }
                else if (ft.kind == Compiler::CmpFileKindResource && OS_WINDOWS) {
                    // we construct an object name which also includes the full name of the reousrce file and appends a .o
                    // to the name (to be more
                    // precised, $(ObjectSuffix))
                    wxString objectName;
                    objectName << wxT("$(IntermediateDirectory)\\") << objPrefix << fullnameOnly << wxT("$(ObjectSuffix)");

                    text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_WIN) << wxT("\n");
                    text << wxT("\t") << compilationLine << wxT("\n");
                }
            }
        }

        if (generateDependenciesFiles) {
            text << wxT("\n");
            text << wxT("-include ") << wxT("$(IntermediateDirectory)\\*$(DependSuffix)\n");
        }
    }
    //---------------------------------------------------------------------------
    static wxString GetIntermediateFolder(BuildConfigPtr bldConf)
    {
        wxString IntermediateDirectory = bldConf->GetIntermediateDirectory();
        if (IntermediateDirectory == "/" || IntermediateDirectory.IsEmpty()) {
            return wxEmptyString;
        }

        // not empty and does not equal to "/"
        if (!IntermediateDirectory.EndsWith("/")) {
            IntermediateDirectory.Append('/');
        }
        return IntermediateDirectory;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // get the project specific build configuration for the workspace active
        // configuration
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        // Can we use asterisk in the clean command?
        wxString imd = GetIntermediateFolder(bldConf);

        // add clean target
        text << wxT("##\n");
        text << wxT("## Clean\n");
        text << wxT("##\n");
        text << wxT("clean:\n");

        wxString cwd = proj->GetFileName().GetPath();
        if (!imd.IsEmpty()) {
            // Remove the entire build folder
            text << wxT("\t") << wxT("@if exist $(IntermediateDirectory) rmdir /S /Q $(IntermediateDirectory)") << "\n";

            // Remove the pre-compiled header
            wxString pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);

            if (pchFile.IsEmpty() == false) {
                text << wxT("\t") << wxT("del /Q ") << pchFile << wxT(".gch") << wxT("\n");
            }

        }
        else if (OS_WINDOWS) {
            text << wxT("\t") << wxT("@del /Q ") << imd << "*$(ObjectSuffix)" << wxT("\n");
            text << wxT("\t") << wxT("@del /Q ") << imd << "*$(DependSuffix)" << wxT("\n");
            // delete the output file as well
            wxString exeExt(wxEmptyString);
            if (proj->GetSettings()->GetProjectType(bldConf->GetName()) == PROJECT_TYPE_EXECUTABLE) {
                // under windows, g++ automatically adds the .exe extension to executable
                // make sure we delete it as well
                exeExt = wxT(".exe");
            }

            text << wxT("\t") << wxT("@del /Q ") << wxT("$(OutputFile)") << wxT("\n");
            text << wxT("\t") << wxT("@del /Q ") << wxT("$(OutputFile)") << exeExt << wxT("\n");
            text << wxT("\t") << wxT("@del /Q ") << DoGetMarkerFileDir(proj->GetName(), proj->GetFileName().GetPath())
                << wxT("\n");

            // Remove the pre-compiled header
            wxString pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);

            if (pchFile.IsEmpty() == false) {
                text << wxT("\t") << wxT("@del /Q ") << pchFile << wxT(".gch") << wxT("\n");
            }
        }
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // create a list of objects
        CreateObjectList(proj, confToBuild, text);
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName,
        const wxString& projName, const wxArrayString& depsProj)
    {

        wxString extraDeps;
        wxString depsRules;

        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projName, errMsg);

        for (size_t i = 0; i < depsProj.GetCount(); i++) {
            wxFileName fn(depsProj.Item(i));
            fn.MakeRelativeTo(proj->GetProjectPath());
            extraDeps << "\"" << fn.GetFullPath() << wxT("\" ");

            depsRules << "\"" << fn.GetFullPath() << wxT("\":\n");
            // Make sure the dependecy directory exists
            depsRules << "\t@$(MakeDirCommand) \"" << fn.GetPath() << "\"\n";
            depsRules << wxT("\t@echo stam > ") << wxT("\"") << fn.GetFullPath() << wxT("\"\n");
            depsRules << wxT("\n\n");
        }

        if (type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            text << wxT("all: ");
            text << wxT("$(OutputFile)\n\n");

            text << wxT("$(OutputFile): $(IntermediateDirectory)\\.d ");
            if (extraDeps.IsEmpty() == false) text << extraDeps;

            text << wxT("$(Objects) \n");
            targetName = wxT("$(IntermediateDirectory)\\.d");

        }
        else {
            text << wxT("all: $(IntermediateDirectory) ");
            text << wxT("$(OutputFile)\n\n");
            text << wxT("$(OutputFile): $(Objects)\n");
        }

        if (bldConf->IsLinkerRequired()) {
            CreateTargets(type, bldConf, text, projName);

            if (type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
                if (depsRules.IsEmpty() == false) {
                    text << wxT("\n") << depsRules << wxT("\n");
                }
            }
        }
        // incase project is type exe or dll, force link
        // this is to workaround bug in the generated makefiles
        // which causes the makefile to report 'nothing to be done'
        // even when a dependency was modified
        text << wxT(".PHONY: clean PrePreBuild PreBuild MakeIntermediateDirs all PostBuild \n");
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, const wxString& projName)
    {
        bool markRebuilt(true);
        text << wxT("\t@$(MakeDirCommand) $(@D)\n");
        text << wxT("\t@echo \"\" > $(IntermediateDirectory)\\.d\n");

        CompilerPtr cmp = bldConf->GetCompiler();

        // this is a special target that creates a file with the content of the
        // $(Objects) variable (to be used with the @<file-name> option of the LD
        for (size_t i = 0; i < m_objectChunks; ++i) {
            wxString oper = ">>";
            if (i == 0) oper = " >";

            text << "\t@echo $(Objects" << i << ") " << oper << " $(ObjectsFileList)\n";
        }

        if (type == PROJECT_TYPE_STATIC_LIBRARY) {
            // create a static library
            // In any case add the 'objects_file' target here
            text << wxT("\t") << wxT("$(AR) $(ArchiveOutputSwitch)$(OutputFile)");
            if (cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) $(ArLibs)\n");
            }
            else {
                text << wxT(" $(Objects) $(ArLibs)\n");
            }

        }
        else if (type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            // create a shared library
            text << wxT("\t") << wxT("$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile)");
            if (cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) ");
            }
            else {
                text << wxT(" $(Objects) ");
            }
            text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");

        }
        else if (type == PROJECT_TYPE_EXECUTABLE) {
            // create an executable
            text << wxT("\t") << wxT("$(LinkerName) $(OutputSwitch)$(OutputFile)");
            if (cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) ");
            }
            else {
                text << wxT(" $(Objects) ");
            }
            text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");
            markRebuilt = false;
        }

        // If a link occurred, mark this project as "rebuilt" so the parent project will
        // know that a re-link is required
        if (bldConf->IsLinkerRequired() && markRebuilt) {
            text << wxT("\t@$(MakeDirCommand) \"") << DoGetMarkerFileDir(wxEmptyString) << wxT("\"\n");

            text << wxT("\t@echo rebuilt > ") << DoGetMarkerFileDir(projName) << wxT("\n");
        }
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        if (!HasPostbuildCommands(bldConf)) return;

        // generate postbuild commands
        BuildCommandList cmds;
        bldConf->GetPostBuildCommands(cmds);

        // Loop over the commands and replace any macros
        std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
            cmd.SetCommand(
                MacroManager::Instance()->Expand(cmd.GetCommand(), GetManager(), proj->GetName(), bldConf->GetName()));
        });

        text << wxT("\n");
        text << wxT("PostBuild:\n");
        text << wxT("\t@echo Executing Post Build commands ...\n");

        BuildCommandList::const_iterator iter = cmds.begin();
        for (; iter != cmds.end(); iter++) {
            if (iter->GetEnabled()) {
                // HACK:
                // If the command is 'copy' under Windows, make sure that
                // we set all slashes to backward slashes
                wxString command = iter->GetCommand();
                command.Trim().Trim(false);
                if (OS_WINDOWS && command.StartsWith(wxT("copy"))) {
                    command.Replace(wxT("/"), wxT("\\"));
                }

                if (OS_WINDOWS && command.EndsWith(wxT("\\"))) {
                    command.RemoveLast();
                }

                text << wxT("\t") << iter->GetCommand() << wxT("\n");
            }
        }
        text << wxT("\t@echo Done\n");
    }
    //---------------------------------------------------------------------------
    bool BuilderNMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
    {
        BuildCommandList cmds;
        bldConf->GetPreBuildCommands(cmds);

        BuildCommandList::const_iterator iter = cmds.begin();
        for (; iter != cmds.end(); iter++) {
            if (iter->GetEnabled()) {
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;
        wxString name = bldConf->GetName();
        name = NormalizeConfigName(name);

        // add PrePreBuild
        wxString preprebuild = bldConf->GetPreBuildCustom();
        preprebuild.Trim().Trim(false);
        if (preprebuild.IsEmpty() == false) {
            text << wxT("PrePreBuild: ");
            text << bldConf->GetPreBuildCustom() << wxT("\n");
        }
        text << wxT("\n");
        bldConf->GetPreBuildCommands(cmds);

        // Loop over the commands and replace any macros
        std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
            cmd.SetCommand(
                MacroManager::Instance()->Expand(cmd.GetCommand(), GetManager(), proj->GetName(), bldConf->GetName()));
        });

        bool first(true);
        text << wxT("PreBuild:\n");
        if (!cmds.empty()) {
            iter = cmds.begin();
            for (; iter != cmds.end(); iter++) {
                if (iter->GetEnabled()) {
                    if (first) {
                        text << wxT("\t@echo Executing Pre Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if (!first) {
                text << wxT("\t@echo Done\n");
            }
        }
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        wxString name = bldConf->GetName();
        name = NormalizeConfigName(name);

        wxString cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        wxString objectsFileName(proj->GetFileName().GetPath());
        objectsFileName << gsgsPathSep << proj->GetName() << wxT(".txt");

        text << wxT("## ") << name << wxT("\n");

        wxString outputFile = bldConf->GetOutputFileName();
        if (OS_WINDOWS && (bldConf->GetProjectType() == PROJECT_TYPE_EXECUTABLE || bldConf->GetProjectType().IsEmpty())) {
            outputFile.Trim().Trim(false);
            outputFile.Replace(wxT("/"), wxT("\\"));
        }

        // Expand the build macros into the generated makefile
        wxString projectName = proj->GetName();
        text << wxT("ProjectName            =") << projectName << wxT("\n");
        text << wxT("ConfigurationName      =") << name << wxT("\n");
        text << wxT("WorkspacePath          = \"") << gsgs_CppWorker().GetFileName().GetPath()
            << wxT("\"\n");
        text << wxT("ProjectPath            = \"") << proj->GetFileName().GetPath() << wxT("\"\n");
        text << wxT("IntermediateDirectory  =") << bldConf->GetIntermediateDirectory() << wxT("\n");
        text << wxT("OutDir                 = $(IntermediateDirectory)\n");
        text << wxT("CurrentFileName        =\n"); // TODO:: Need implementation
        text << wxT("CurrentFilePath        =\n"); // TODO:: Need implementation
        text << wxT("CurrentFileFullPath    =\n"); // TODO:: Need implementation
        text << wxT("User                   =") << wxGetUserName() << wxT("\n");
        text << wxT("Date                   =") << wxDateTime::Now().FormatDate() << wxT("\n");
        text << wxT("gsgsPath           =\"") << gsgs_Path().GetStartupDirectory() << wxT("\"\n");
        // replace all occurence of forward slash with double backslash
        wxString linkerStr(cmp->GetTool(wxT("LinkerName")));
        if (OS_WINDOWS) {
            linkerStr = ReplaceBackslashes(linkerStr);
        }
        text << wxT("LinkerName             =") << linkerStr << wxT("\n");
        wxString shObjlinkerStr(cmp->GetTool(wxT("SharedObjectLinkerName")));
        if (OS_WINDOWS) {
            shObjlinkerStr = ReplaceBackslashes(shObjlinkerStr);
        }

        text << wxT("SharedObjectLinkerName =") << shObjlinkerStr << wxT("\n");
        text << wxT("ObjectSuffix           =") << cmp->GetObjectSuffix() << wxT("\n");
        text << wxT("DependSuffix           =") << cmp->GetDependSuffix() << wxT("\n");
        text << wxT("PreprocessSuffix       =") << cmp->GetPreprocessSuffix() << wxT("\n");
        text << wxT("DebugSwitch            =") << cmp->GetSwitch(wxT("Debug")) << wxT("\n");
        text << wxT("IncludeSwitch          =") << cmp->GetSwitch(wxT("Include")) << wxT("\n");
        text << wxT("LibrarySwitch          =") << cmp->GetSwitch(wxT("Library")) << wxT("\n");
        text << wxT("OutputSwitch           =") << cmp->GetSwitch(wxT("Output")) << wxT("\n");
        text << wxT("LibraryPathSwitch      =") << cmp->GetSwitch(wxT("LibraryPath")) << wxT("\n");
        text << wxT("PreprocessorSwitch     =") << cmp->GetSwitch(wxT("Preprocessor")) << wxT("\n");
        text << wxT("Operation           =") << cmp->GetSwitch(wxT("Source")) << wxT("\n");
        text << wxT("OutputFile             =") << outputFile << wxT("\n");
        text << wxT("Preprocessors          =") << ParsePreprocessor(bldConf->GetPreprocessor()) << wxT("\n");
        text << wxT("Objective           =") << cmp->GetSwitch(wxT("Object")) << wxT("\n");
        text << wxT("ArchiveOutputSwitch    =") << cmp->GetSwitch(wxT("ArchiveOutput")) << wxT("\n");
        text << wxT("PreprocessOnlySwitch   =") << cmp->GetSwitch(wxT("PreprocessOnly")) << wxT("\n");

        wxFileName fnObjectsFileName(objectsFileName);
        fnObjectsFileName.MakeRelativeTo(proj->GetFileName().GetPath());

        text << wxT("ObjectsFileList        =\"") << fnObjectsFileName.GetFullPath() << wxT("\"\n");
        text << wxT("PCHCompileFlags        =") << bldConf->GetPchCompileFlags() << wxT("\n");

        wxString mkdirCommand = cmp->GetTool("MakeDirCommand");
        if (!mkdirCommand.IsEmpty()) {
            // use the compiler defined one
            if (OS_WINDOWS) {
                mkdirCommand = ReplaceBackslashes(mkdirCommand);
            }
            text << wxT("MakeDirCommand         =") << mkdirCommand << wxT("\n");
        }
        else {
            text << wxT("MakeDirCommand         =") << wxT("makedir") << wxT("\n");
        }

        wxString buildOpts = bldConf->GetCompileOptions();
        buildOpts.Replace(wxT(";"), wxT(" "));

        wxString cBuildOpts = bldConf->GetCCompileOptions();
        cBuildOpts.Replace(wxT(";"), wxT(" "));

        wxString asOptions = bldConf->GetAssmeblerOptions();
        asOptions.Replace(";", " ");

        // Let the plugins add their content here
        gsgs::BuildEvent e(wxEVT_GET_ADDITIONAL_COMPILEFLAGS);
        e.SetProjectName(proj->GetName());
        e.SetConfigurationName(bldConf->GetName());
        gsgs_Event().ProcessEvent(e);

        wxString additionalCompileFlags = e.GetCommand();
        if (additionalCompileFlags.IsEmpty() == false) {
            buildOpts << wxT(" ") << additionalCompileFlags;
            cBuildOpts << wxT(" ") << additionalCompileFlags;
        }

        // only if resource compiler required, evaluate the resource variables
        if (OS_WINDOWS) {
            wxString rcBuildOpts = bldConf->GetResCompileOptions();
            rcBuildOpts.Replace(wxT(";"), wxT(" "));
            text << wxT("RcCmpOptions           =") << rcBuildOpts << wxT("\n");
            wxString rcCompilerstr(cmp->GetTool(wxT("ResourceCompiler")));
            rcCompilerstr.Replace(wxT("/"), wxT("\\"));
            text << wxT("RcCompilerName         =") << rcCompilerstr << wxT("\n");
        }

        wxString linkOpt = bldConf->GetLinkOptions();
        linkOpt.Replace(wxT(";"), wxT(" "));

        // link options are kept with semi-colons, strip them
        text << wxT("LinkOptions            = ") << linkOpt << wxT("\n");

        // add the global include path followed by the project include path
        wxString pchFile;

        // If the PCH is required to be in the command line, add it here
        // otherwise, we just make sure it is generated and the compiler will pick it by itself
        if (bldConf->GetPchInCommandLine()) {
            pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);
            if (pchFile.IsEmpty() == false) {
                pchFile.Prepend(wxT(" -include ")).Append(wxT(" "));
            }
        }

        wxString libraries = bldConf->GetLibraries();
        wxArrayString libsArr = ::wxStringTokenize(libraries, wxT(";"), wxTOKEN_STRTOK);
        libraries.Clear();
        libraries << wxT(" ");
        for (size_t i = 0; i < libsArr.GetCount(); i++) {
            libsArr.Item(i).Trim().Trim(false);
            libraries << wxT("\"") << libsArr.Item(i) << wxT("\" ");
        }
        libraries.Replace(wxT("/"), wxT("\\"));

        text << wxT("IncludePath            = ")
            << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << wxT(" ")
            << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
        text << wxT("IncludePCH             = ") << pchFile << wxT("\n");
        text << wxT("RcIncludePath          = ")
            << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
        text << wxT("Libs                   = ") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");
        text << wxT("ArLibs                 = ") << libraries << wxT("\n");

        // add the global library path followed by the project library path
        text << wxT("LibPath                =")
            << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName()) << wxT(" ")
            << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << wxT("\n");

        text << wxT("\n");
        text << wxT("##\n");
        text << wxT("## Common variables\n");
        text << wxT("## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables\n");
        text << wxT("##\n");
        wxString arStr(cmp->GetTool(wxT("AR")));
        wxString cxxStr(cmp->GetTool(wxT("CXX")));
        wxString ccStr(cmp->GetTool(wxT("CC")));
        wxString asStr(cmp->GetTool(wxT("AS")));

        if (OS_WINDOWS) {
            arStr = ReplaceBackslashes(arStr);
            cxxStr = ReplaceBackslashes(cxxStr);
            ccStr = ReplaceBackslashes(ccStr);
            asStr = ReplaceBackslashes(asStr);
        }

        text << wxT("AR       = ") << arStr << wxT("\n");
        text << wxT("CXX      = ") << cxxStr << wxT("\n");
        text << wxT("CC       = ") << ccStr << wxT("\n");
        text << wxT("CXXFLAGS = ") << buildOpts << wxT(" $(Preprocessors)") << wxT("\n");
        text << wxT("CFLAGS   = ") << cBuildOpts << wxT(" $(Preprocessors)") << wxT("\n");
        text << wxT("ASFLAGS  = ") << asOptions << "\n";
        text << wxT("AS       = ") << asStr << "\n";
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf)
    {
        // convert semi-colon delimited string into GNU list of
        // include paths:
        wxString incluedPath(wxEmptyString);
        wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with -I
        while (tkz.HasMoreTokens()) {
            wxString path(tkz.NextToken());
            TrimString(path);
            // path.Replace(wxT("\\"), wxT("/"));

            wxString wrapper;
            if (path.Contains(wxT(" "))) wrapper = wxT("\"");

            incluedPath << wxT("$(IncludeSwitch)") << wrapper << path << wrapper << wxT(" ");
        }
        return incluedPath;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf)
    {
        // convert semi-colon delimited string into GNU list of
        // lib path
        wxString libPath(wxEmptyString);
        wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with libpath switch
        while (tkz.HasMoreTokens()) {
            wxString path(tkz.NextToken());
            path.Trim().Trim(false);
            // path.Replace(wxT("\\"), wxT("/"));
            wxString wrapper;
            if (path.Contains(wxT(" "))) wrapper = wxT("\"");

            libPath << wxT("$(LibraryPathSwitch)") << wrapper << path << wrapper << wxT(" ");
        }
        return libPath;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::ParsePreprocessor(const wxString& prep)
    {
        wxString preprocessor(wxEmptyString);
        wxStringTokenizer tkz(prep, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with libpath switch
        while (tkz.HasMoreTokens()) {
            wxString p(tkz.NextToken());
            p.Trim().Trim(false);
            preprocessor << wxT("$(PreprocessorSwitch)") << p << wxT(" ");
        }

        // if the macro contains # escape it
        // But first remove any manual escaping done by the user
        preprocessor.Replace(wxT("\\#"), wxT("#"));
        preprocessor.Replace(wxT("#"), wxT("\\#"));
        return preprocessor;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::ParseLibs(const wxString& libs)
    {
        // convert semi-colon delimited string into GNU list of
        // libs
        wxString slibs(wxEmptyString);
        wxStringTokenizer tkz(libs, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with -l and strip trailing lib string
        // also, if the file contains an extension (.a, .so, .dynlib) remove them as well
        while (tkz.HasMoreTokens()) {
            wxString lib(tkz.NextToken());
            TrimString(lib);
            // remove lib prefix
            if (lib.StartsWith(wxT("lib"))) {
                lib = lib.Mid(3);
            }

            // remove known suffixes
            if (lib.EndsWith(wxT(".a")) || lib.EndsWith(wxT(".so")) || lib.EndsWith(wxT(".dylib")) ||
                lib.EndsWith(wxT(".dll"))) {
                lib = lib.BeforeLast(wxT('.'));
            }

            slibs << wxT("$(LibrarySwitch)") << lib << wxT(" ");
        }
        return slibs;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
    {
        wxString errMsg, cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if (!bldConf) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, false, false, errMsg);

        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        // buildTool.Replace(wxT("\\"), wxT("/"));
        cmd << buildTool << wxT(" Makefile");
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments)
    {
        wxString errMsg, cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if (!bldConf) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, false, false, errMsg);

        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        // buildTool.Replace(wxT("\\"), wxT("/"));

        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
        cmd << buildTool << wxT(" Makefile clean");
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
        const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), false, false);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
        const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("clean"), false, true);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
        const wxString& fileName)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);

        // Build the target list
        wxString target;
        wxString cmpType;
        wxFileName fn(fileName);

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if (!bldConf) {
            return wxEmptyString;
        }

        cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);
        // fn.MakeRelativeTo(proj->GetFileName().GetPath());

        wxString relPath = fn.GetPath(true, wxPATH_UNIX);
        wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
        target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetFullName()
            << cmp->GetObjectSuffix();

        target = ExpandAllVariables(target, &gsgs_CppWorker(), proj->GetName(), confToBuild, wxEmptyString);
        cmd = GetProjectMakeCommand(proj, confToBuild, target, false, false);

        return gsgs_EditorConfig().ExpandVariables(cmd, true);
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
        const wxString& arguments, const wxString& fileName, wxString& errMsg)
    {
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            return wxEmptyString;
        }

        wxString cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if (!bldConf) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);

        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);

        // fix: replace all Windows like slashes to POSIX
        // buildTool.Replace(wxT("\\"), wxT("/"));

        // create the target
        wxString target;
        wxString objSuffix;
        wxFileName fn(fileName);

        wxString cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
        target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetFullName()
            << cmp->GetPreprocessSuffix();

        target = ExpandAllVariables(target, &gsgs_CppWorker(), proj->GetName(), confToBuild, wxEmptyString);
        cmd = GetProjectMakeCommand(proj, confToBuild, target, false, false);
        return gsgs_EditorConfig().ExpandVariables(cmd, true);
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetCdCmd(const wxFileName& path1, const wxFileName& path2)
    {
        wxString cd_cmd(wxT("@"));
        if (path2.GetPath().IsEmpty()) {
            return cd_cmd;
        }

        if (path1.GetPath() != path2.GetPath()) {
            cd_cmd << wxT("cd \"") << path2.GetPath() << wxT("\" && ");
        }
        return cd_cmd;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;

        cmds.clear();
        bldConf->GetPostBuildCommands(cmds);
        bool first(true);
        if (!cmds.empty()) {
            iter = cmds.begin();
            for (; iter != cmds.end(); iter++) {
                if (iter->GetEnabled()) {
                    if (first) {
                        text << wxT("\t@echo Executing Post Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if (!first) {
                text << wxT("\t@echo Done\n");
            }
        }
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;

        cmds.clear();
        bldConf->GetPreBuildCommands(cmds);
        bool first(true);
        if (!cmds.empty()) {
            iter = cmds.begin();
            for (; iter != cmds.end(); iter++) {
                if (iter->GetEnabled()) {
                    if (first) {
                        text << wxT("\t@echo Executing Pre Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if (!first) {
                text << wxT("\t@echo Done\n");
            }
        }
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj,
        const wxString& confToBuild)
    {
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);

        // iterate over the dependencies projects and generate makefile
        wxString makeCommand;
        wxString basicMakeCommand;

        wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", false);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);
        basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\"");

        makeCommand << wxT("\t") << GetCdCmd(wspfile, projectPath);

        if (bldConf) {
            wxString preprebuild = bldConf->GetPreBuildCustom();
            wxString precmpheader = bldConf->GetPrecompiledHeader();
            precmpheader.Trim().Trim(false);
            preprebuild.Trim().Trim(false);

            if (preprebuild.IsEmpty() == false) {
                makeCommand << basicMakeCommand << wxT(" PrePreBuild && ");
            }

            if (HasPrebuildCommands(bldConf)) {
                makeCommand << basicMakeCommand << wxT(" PreBuild && ");
            }

            // Run pre-compiled header compilation if any
            if (precmpheader.IsEmpty() == false) {
                makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
            }
        }

        // the main command
        makeCommand << basicMakeCommand;

        // post
        if (bldConf && HasPostbuildCommands(bldConf)) {
            makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
        }
        makeCommand << wxT("\n");
        return makeCommand;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
        bool addCleanTarget, bool cleanOnly)
    {
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);

        // iterate over the dependencies projects and generate makefile
        wxString makeCommand;
        wxString basicMakeCommand;

        wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);
        basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" ");

        if (addCleanTarget) {
            makeCommand << basicMakeCommand << wxT(" clean && ");
        }

        if (bldConf && !cleanOnly) {
            wxString preprebuild = bldConf->GetPreBuildCustom();
            wxString precmpheader = bldConf->GetPrecompiledHeader();
            precmpheader.Trim().Trim(false);
            preprebuild.Trim().Trim(false);

            makeCommand << basicMakeCommand << " MakeIntermediateDirs && ";

            if (preprebuild.IsEmpty() == false) {
                makeCommand << basicMakeCommand << wxT(" PrePreBuild && ");
            }

            if (HasPrebuildCommands(bldConf)) {
                makeCommand << basicMakeCommand << wxT(" PreBuild && ");
            }

            // Run pre-compiled header compilation if any
            if (precmpheader.IsEmpty() == false) {
                makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
            }
        }

        makeCommand << basicMakeCommand << wxT(" ") << target;

        // post
        if (bldConf && !cleanOnly && HasPostbuildCommands(bldConf)) {
            makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
        }
        return makeCommand;
    }
    //---------------------------------------------------------------------------
    void BuilderNMake::CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text)
    {
        wxString filename = bldConf->GetPrecompiledHeader();
        filename.Trim().Trim(false);

        if (filename.IsEmpty()) return;

        text << wxT("\n");
        text << wxT("# PreCompiled Header\n");
        text << filename << wxT(".gch: ") << filename << wxT("\n");
        if (bldConf->GetPCHFlagsPolicy() == BuildConfig::kPCHPolicyReplace) {
            text << wxT("\t") << DoGetCompilerMacro(filename) << wxT(" $(Operation) ") << filename
                << wxT(" $(PCHCompileFlags)\n");

        }
        else { // BuildConfig::kPCHPolicyAppend
            text << wxT("\t") << DoGetCompilerMacro(filename) << wxT(" $(Operation) ") << filename
                << wxT(" $(PCHCompileFlags) $(CXXFLAGS) $(IncludePath)\n");
        }
        text << wxT("\n");
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
        const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (!proj) {
            return wxEmptyString;
        }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), true, false);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
        const wxString& arguments, bool isCommandlineCommand) const
    {
        wxString jobsCmd;
        wxString buildTool;

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if (!bldConf) return wxEmptyString;

        CompilerPtr compiler = bldConf->GetCompiler();
        if (!compiler) return wxEmptyString;

        if (isCommandlineCommand) {
            buildTool = compiler->GetTool("MAKE");

        }
        else {
            jobsCmd = wxEmptyString;
            buildTool = wxT("\"$(MAKE)\"");
        }

        if (isCommandlineCommand) {
            return buildTool + " -e -f ";

        }
        else {
            return buildTool + " -f ";
        }
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::DoGetCompilerMacro(const wxString& filename)
    {
        wxString compilerMacro(wxT("$(CXX)"));
        switch (FileUtil::GetType(filename)) {
        case FileUtil::TypeSourceC:
            compilerMacro = wxT("$(CC)");
            break;
        case FileUtil::TypeSourceCpp:
        default:
            compilerMacro = wxT("$(CXX)");
            break;
        }
        return compilerMacro;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp)
    {
        size_t count = filename.GetDirCount();
        const wxArrayString& dirs = filename.GetDirs();
        wxString lastDir;

        if (cwd == filename.GetPath()) return wxEmptyString;

        if (cmp && cmp->GetObjectNameIdenticalToFileName()) return wxEmptyString;

        if (cwd == filename.GetPath()) {
            return wxEmptyString;
        }

        if (count) {
            lastDir = dirs.Item(count - 1);

            // Handle special directory paths
            if (lastDir == wxT("..")) {
                lastDir = wxT("up");

            }
            else if (lastDir == wxT(".")) {
                lastDir = wxT("cur");
            }

            if (lastDir.IsEmpty() == false) {
                lastDir << wxT("_");
            }
        }

        return lastDir;
    }
    //---------------------------------------------------------------------------
    wxString BuilderNMake::DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath)
    {
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

        workspaceSelConf = NormalizeConfigName(workspaceSelConf);
        workspaceSelConf.MakeLower();

        wxString path;
        if (projname.IsEmpty()) {
            path << gsgs_CppWorker().GetFileName().GetPath() << "/" << wxT(".build-") << workspaceSelConf;

        }
        else {
            path << gsgs_CppWorker().GetFileName().GetPath() << "/" << wxT(".build-") << workspaceSelConf
                << "/" << projname;
        }

        if (projectPath.IsEmpty() == false) {
            wxFileName fn(path);
            fn.MakeRelativeTo(projectPath);
            path = fn.GetFullPath(wxPATH_WIN);
        }

        if (!projname.IsEmpty())
            return "\"" + path + "\"";
        else
            return path;
    }
    //---------------------------------------------------------------------------
    bool BuilderNMake::HasPostbuildCommands(BuildConfigPtr bldConf) const
    {
        BuildCommandList cmds;
        bldConf->GetPostBuildCommands(cmds);

        BuildCommandList::const_iterator iter = cmds.begin();
        for (; iter != cmds.end(); iter++) {
            if (iter->GetEnabled()) {
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool BuilderNMake::SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName)
    {
        gsgs::BuildEvent e(eventId);
        e.SetProjectName(projectName);
        e.SetConfigurationName(configurationName);
        return gsgs_Event().ProcessEvent(e);
    }
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // BuilderGnuMake
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    BuilderGnuMake::BuilderGnuMake()
        : Builder("Default")
        , m_objectChunks(1)
        , m_projectFilesMetadata(nullptr)
    {
    }
    //---------------------------------------------------------------------------
    BuilderGnuMake::BuilderGnuMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions)
        : Builder(name)
        , m_projectFilesMetadata(nullptr)
    {
    }
    //---------------------------------------------------------------------------
    BuilderGnuMake::~BuilderGnuMake() 
    {
    }
    //---------------------------------------------------------------------------
    bool BuilderGnuMake::Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
                                bool isProjectOnly, bool force, wxString& errMsg)
    {
        if(project.IsEmpty()) { return false; }
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) {
            errMsg << _("Cant open project '") << project << wxT("'");
            return false;
        }

        // get the selected build configuration
        wxString bld_conf_name(confToBuild);

        if(confToBuild.IsEmpty()) {
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
            if(!bldConf) {
                errMsg << _("Cant find build configuration for project '") << project << wxT("'");
                return false;
            }
            bld_conf_name = bldConf->GetName();
        }

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, bld_conf_name);
        if(!bldConf) {
            errMsg << _("Cant find build configuration for project '") << project << wxT("'");
            return false;
        }
        if(!bldConf->GetCompiler()) {
            errMsg << _("Cant find proper compiler for project '") << project << wxT("'");
            return false;
        }

        wxArrayString depsArr = proj->GetDependencies(bld_conf_name);

        gsgs_Debug("Generating Makefile...");
        // Filter all disabled projects from the dependencies array
        wxArrayString updatedDepsArr;
        for(size_t i = 0; i < depsArr.GetCount(); ++i) {
            wxString errmsg;
            ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errmsg);
            if(!dependProj) { continue; }

            wxString projectSelConf = gsgs_CppWorker().GetBuildMatrix()->GetProjectSelectedConf(
                gsgs_CppWorker().GetBuildMatrix()->GetSelectedConfigurationName(), dependProj->GetName());
            BuildConfigPtr dependProjbldConf =
                gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
            if(dependProjbldConf && dependProjbldConf->IsProjectEnabled()) { updatedDepsArr.Add(depsArr.Item(i)); }
        }

        // Update the dependencies with the correct list
        depsArr.swap(updatedDepsArr);

        wxArrayString removeList;
        if(!isProjectOnly) {
            // this function assumes that the working directory is located at the workspace path
            // make sure that all dependencies exists
            for(size_t i = 0; i < depsArr.GetCount(); i++) {
                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                // Missing dependencies project?
                // this can happen if a project was removed from the workspace, but
                // is still on the depdendencie list of this project
                if(!dependProj) {
                    wxString msg;
                    msg << _("gsgs can not find project '") << depsArr.Item(i) << _("' which is required\n");
                    msg << _("for building project '") << project
                        << _("'.\nWould you like to remove it from the dependency list?");
                    if(wxMessageBox(msg, _("光栅故事"), wxYES_NO | wxICON_QUESTION) == wxYES) {
                        // remove the project from the dependecie list, and continue
                        removeList.Add(depsArr.Item(i));
                    }
                }
            }
            bool settingsChanged(false);
            // remove the unfound projects from the dependencies array
            for(size_t i = 0; i < removeList.GetCount(); i++) {
                int where = depsArr.Index(removeList.Item(i));
                if(where != wxNOT_FOUND) {
                    depsArr.RemoveAt(where);
                    settingsChanged = true;
                }
            }
            // update the project dependencies
            bool modified = proj->IsModified();

            // Update the dependencies only if needed
            if(settingsChanged) proj->SetDependencies(depsArr, bld_conf_name);

            // the set settings functions marks the project as 'modified' this causes
            // an unneeded makefile generation if the settings was not really modified
            if(!modified && !settingsChanged) { proj->SetModified(false); }
        }

        wxString fn;
        fn << "Makefile";
        wxString text;

        wxFileName wspfile(gsgs_CppWorker().GetFileName());

        text << wxT(".PHONY: clean All\n\n");
        text << wxT("All:\n");

        // iterate over the dependencies projects and generate makefile
        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, false);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        buildTool.Replace(wxT("\\"), wxT("/"));

        // generate the makefile for the selected workspace configuration
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString workspaceSelConf = matrix->GetSelectedConfigurationName();
        wxArrayString depsProjs;
        if(!isProjectOnly) {
            for(size_t i = 0; i < depsArr.GetCount(); ++i) {
                bool isCustom(false);
                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                if(!dependProj) { continue; }

                wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, dependProj->GetName());
                BuildConfigPtr dependProjbldConf =
                    gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
                if(dependProjbldConf && dependProjbldConf->IsCustomBuild()) { isCustom = true; }

                // incase we manually specified the configuration to be built, set the project
                // as modified, so on next attempt to build it, gsgs will sync the configuration
                if(confToBuild.IsEmpty() == false) { dependProj->SetModified(true); }

                if(!dependProjbldConf->IsProjectEnabled()) {
                    // Ignore disabled projects
                    continue;
                }

                text << wxT("\t@echo \"") << wxGetTranslation(BUILD_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ")
                     << projectSelConf << wxT(" ]----------\"\n");
                // make the paths relative, if it's sensible to do so
                wxFileName fn(dependProj->GetFileName());
                MakeRelativeIfSensible(fn, wspfile.GetPath());

                bool isPluginGeneratedMakefile =
                    SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, depsArr.Item(i), projectSelConf);

                // we handle custom builds and non-custom build separatly:
                if(isPluginGeneratedMakefile) {

                    // this project makefile is generated by a plugin
                    // query the plugin about the build command
                    gsgs::BuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
                    e.SetProjectName(depsArr.Item(i));
                    e.SetConfigurationName(projectSelConf);
                    e.SetProjectOnly(false);
                    gsgs_Event().ProcessEvent(e);
                    text << wxT("\t") << e.GetCommand() << wxT("\n");

                } else if(isCustom) {

                    CreateCustomPreBuildEvents(dependProjbldConf, text);

                    wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                    wxString build_cmd = dependProjbldConf->GetCustomBuildCmd();
                    wxString customWdCmd;

                    customWd = ExpandAllVariables(customWd, &gsgs_CppWorker(), dependProj->GetName(),
                                                  dependProjbldConf->GetName(), wxEmptyString);
                    build_cmd = ExpandAllVariables(build_cmd, &gsgs_CppWorker(), dependProj->GetName(),
                                                   dependProjbldConf->GetName(), wxEmptyString);

                    build_cmd.Trim().Trim(false);

                    if(build_cmd.empty()) { build_cmd << wxT("@echo Project has no custom build command!"); }

                    // if a working directory is provided apply it, otherwise use the project
                    // path
                    customWd.Trim().Trim(false);
                    if(customWd.empty() == false) {
                        customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
                    } else {
                        customWdCmd << GetCdCmd(wspfile, fn);
                    }

                    text << wxT("\t") << customWdCmd << build_cmd << wxT("\n");
                    CreateCustomPostBuildEvents(dependProjbldConf, text);

                } else {
                    // generate the dependency project makefile
                    wxString dep_file = DoGetMarkerFileDir(wxT(""));
                    dep_file << gsgsPathSep << dependProj->GetName();
                    depsProjs.Add(dep_file);

                    GenerateMakefile(dependProj, projectSelConf, confToBuild.IsEmpty() ? force : true, wxArrayString());
                    text << GetProjectMakeCommand(wspfile, fn, dependProj, projectSelConf);
                }
            }
        }

        // Generate makefile for the project itself
        GenerateMakefile(proj, confToBuild, confToBuild.IsEmpty() ? force : true, depsProjs);

        // incase we manually specified the configuration to be built, set the project
        // as modified, so on next attempt to build it, gsgs will sync the configuration
        if(confToBuild.IsEmpty() == false) { proj->SetModified(true); }

        wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
        if(isProjectOnly && confToBuild.IsEmpty() == false) {
            // incase we use to generate a 'Project Only' makefile,
            // we allow the caller to override the selected configuration with 'confToBuild' parameter
            projectSelConf = confToBuild;
        }

        text << wxT("\t@echo \"") << wxGetTranslation(BUILD_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf
             << wxT(" ]----------\"\n");

        // make the paths relative, if it's sensible to do so
        wxFileName projectPath(proj->GetFileName());
        MakeRelativeIfSensible(projectPath, wspfile.GetPath());

        wxString pname(proj->GetName());

        bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);
        if(isPluginGeneratedMakefile) {

            wxString cmd;
            gsgs::BuildEvent e(wxEVT_GET_PROJECT_BUILD_CMD);
            e.SetProjectName(pname);
            e.SetConfigurationName(projectSelConf);
            e.SetProjectOnly(isProjectOnly);
            gsgs_Event().ProcessEvent(e);

            cmd = e.GetCommand();
            text << wxT("\t") << cmd << wxT("\n");

        } else {
            text << GetProjectMakeCommand(wspfile, projectPath, proj, projectSelConf);
        }

        // create the clean target
        text << wxT("clean:\n");
        if(!isProjectOnly) {
            for(size_t i = 0; i < depsArr.GetCount(); i++) {
                bool isCustom(false);
                wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, depsArr.Item(i));

                ProjectPtr dependProj = gsgs_CppWorker().FindProjectByName(depsArr.Item(i), errMsg);
                // Missing dependencies project? just skip it
                if(!dependProj) { continue; }

                text << wxT("\t@echo \"") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << dependProj->GetName() << wxT(" - ")
                     << projectSelConf << wxT(" ]----------\"\n");

                // make the paths relative
                wxFileName fn(dependProj->GetFileName());
                MakeRelativeIfSensible(fn, wspfile.GetPath());

                // if the dependencie project is project of type 'Custom Build' - do the custom build instead
                // of the geenrated makefile
                BuildConfigPtr dependProjbldConf =
                    gsgs_CppWorker().GetProjBuildConf(dependProj->GetName(), projectSelConf);
                if(dependProjbldConf && dependProjbldConf->IsCustomBuild()) { isCustom = true; }

                // if we are using custom build command, invoke it instead of calling the generated makefile
                wxString pname = dependProj->GetName();
                bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, projectSelConf);

                // we handle custom builds and non-custom build separately:
                if(isPluginGeneratedMakefile) {

                    // this project makefile is generated by a plugin
                    // query the plugin about the build command
                    gsgs::BuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
                    e.SetProjectOnly(false);
                    e.SetProjectName(pname);
                    e.SetConfigurationName(projectSelConf);
                    gsgs_Event().ProcessEvent(e);
                    text << wxT("\t") << e.GetCommand() << wxT("\n");

                } else if(!isCustom) {

                    text << wxT("\t") << GetCdCmd(wspfile, fn) << buildTool << wxT(" \"") << dependProj->GetName()
                         << wxT(".mk\"  clean\n");

                } else {

                    wxString customWd = dependProjbldConf->GetCustomBuildWorkingDir();
                    wxString clean_cmd = dependProjbldConf->GetCustomCleanCmd();

                    customWd = ExpandAllVariables(customWd, &gsgs_CppWorker(), dependProj->GetName(),
                                                  dependProjbldConf->GetName(), wxEmptyString);
                    clean_cmd = ExpandAllVariables(clean_cmd, &gsgs_CppWorker(), dependProj->GetName(),
                                                   dependProjbldConf->GetName(), wxEmptyString);

                    wxString customWdCmd;

                    clean_cmd.Trim().Trim(false);
                    if(clean_cmd.empty()) { clean_cmd << wxT("@echo Project has no custom clean command!"); }

                    // if a working directory is provided apply it, otherwise use the project
                    // path
                    customWd.Trim().Trim(false);
                    if(customWd.empty() == false) {
                        customWdCmd << wxT("@cd \"") << ExpandVariables(customWd, dependProj, NULL) << wxT("\" && ");
                    } else {
                        customWdCmd << GetCdCmd(wspfile, fn);
                    }
                    text << wxT("\t") << customWdCmd << clean_cmd << wxT("\n");
                }
            }
        }

        // generate makefile for the project itself
        projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, project);
        if(isProjectOnly && confToBuild.IsEmpty() == false) {
            // incase we use to generate a 'Project Only' makefile,
            // we allow the caller to override the selected configuration with 'confToBuild' parameter
            projectSelConf = confToBuild;
        }

        text << wxT("\t@echo \"") << wxGetTranslation(CLEAN_PROJECT_PREFIX) << project << wxT(" - ") << projectSelConf
             << wxT(" ]----------\"\n");
        if(isPluginGeneratedMakefile) {

            wxString cmd;

            gsgs::BuildEvent e(wxEVT_GET_PROJECT_CLEAN_CMD);
            e.SetProjectName(pname);
            e.SetConfigurationName(projectSelConf);
            e.SetProjectOnly(isProjectOnly);
            gsgs_Event().ProcessEvent(e);
            cmd = e.GetCommand();
            text << wxT("\t") << cmd << wxT("\n");

        } else {
            text << wxT("\t") << GetCdCmd(wspfile, projectPath) << buildTool << wxT(" \"") << proj->GetName()
                 << wxT(".mk\" clean\n");
        }

        // dump the content to file
        wxFileOutputStream output(fn);
        wxStringInputStream content(text);
        output << content;

        gsgs_Debug("Generating Makefile...is completed");
        return true;
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force,
                                          const wxArrayString& depsProj)
    {
        wxString pname(proj->GetName());
        wxString tmpConfigName(confToBuild.c_str());
        if(confToBuild.IsEmpty()) {
            BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
            tmpConfigName = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), proj->GetName());
        }

        bool isPluginGeneratedMakefile = SendBuildEvent(wxEVT_GET_IS_PLUGIN_MAKEFILE, pname, tmpConfigName);

        // we handle custom builds and non-custom build separatly:
        if(isPluginGeneratedMakefile) {
            if(force) {
                // Generate the makefile
                SendBuildEvent(wxEVT_PLUGIN_EXPORT_MAKEFILE, pname, tmpConfigName);
            }
            return;
        }

        ProjectSettingsPtr settings = proj->GetSettings();
        if(!settings) { return; }

        // get the selected build configuration for this project
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        if(!bldConf) { return; }

        wxString path = proj->GetFileName().GetPath();

        DirSaver ds;
        // change directory to the project base dir
        ::wxSetWorkingDirectory(path);

        // create new makefile file
        wxString fn(path);
        fn << gsgsPathSep << proj->GetName() << wxT(".mk");

        // skip the next test if the makefile does not exist
        if(wxFileName::FileExists(fn)) {
            if(!force) {
                if(proj->IsModified() == false) { return; }
            }
        }

        // Load the current project files
        m_projectFilesMetadata = &(proj->GetFiles());

        // generate the selected configuration for this project
        // wxTextOutputStream text(output);
        wxString text;

        text << wxT("##") << wxT("\n");
        text << wxT("## Auto Generated makefile by gsgs IDE") << wxT("\n");
        text << wxT("## any manual changes will be erased      ") << wxT("\n");
        text << wxT("##") << wxT("\n");

        // Create the makefile variables
        CreateConfigsVariables(proj, bldConf, text);

        //----------------------------------------------------------
        // copy environment variables to the makefile
        // We put them after the 'hard-coeded' ones
        // so user will be able to override any of the default
        // variables by defining its own
        //----------------------------------------------------------
        EvnVarList vars;
        gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
        EnvMap varMap = vars.GetVariables(wxT(""), true, proj->GetName(), bldConf->GetName());

        text << wxT("##") << wxT("\n");
        text << wxT("## User defined environment variables") << wxT("\n");
        text << wxT("##") << wxT("\n");

        for(size_t i = 0; i < varMap.GetCount(); i++) {
            wxString name, value;
            varMap.Get(i, name, value);
            text << name << wxT(":=") << value << wxT("") << wxT("\n");
        }

        CreateListMacros(proj, confToBuild, text); // list of srcs and list of objects

        //-----------------------------------------------------------
        // create the build targets
        //-----------------------------------------------------------
        text << wxT("##\n");
        text << wxT("## Main Build Targets \n");
        text << wxT("##\n");

        // incase project is type exe or dll, force link
        // this is to workaround bug in the generated makefiles
        // which causes the makefile to report 'nothing to be done'
        // even when a dependency was modified
        wxString targetName(bldConf->GetIntermediateDirectory());
        CreateLinkTargets(proj->GetSettings()->GetProjectType(bldConf->GetName()), bldConf, text, targetName,
                          proj->GetName(), depsProj);

        CreatePostBuildEvents(proj, bldConf, text);
        CreateMakeDirsTarget(proj, bldConf, targetName, text);
        CreatePreBuildEvents(proj, bldConf, text);
        CreatePreCompiledHeaderTarget(bldConf, text);

        //-----------------------------------------------------------
        // Create a list of targets that should be built according to
        // projects' file list
        //-----------------------------------------------------------
        CreateFileTargets(proj, confToBuild, text);
        CreateCleanTargets(proj, confToBuild, text);

        // dump the content to a file
        wxFFile output;
        output.Open(fn, wxT("w+"));
        if(output.IsOpened()) {
            output.Write(text);
            output.Close();
        }

        // mark the project as non-modified one
        proj->SetModified(false);
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName, wxString& text)
    {
        text << "\n";
        text << "MakeIntermediateDirs:\n";
        text << "\t" << GetMakeDirCmd(bldConf) << "\n\n";

        text << wxT("\n");
        text << targetName << wxT(":\n");
        text << wxT("\t") << GetMakeDirCmd(bldConf) << wxT("\n");
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        std::vector<wxFileName> files;

        std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                      [&](const Project::FilesMap_t::value_type& vt) {
                          clProjectFile::Ptr_t file = vt.second;
                          // Include only files that don't have the 'exclude from build' flag set
                          if(!file->IsExcludeFromConfiguration(confToBuild)) {
                              files.push_back(wxFileName(file->GetFilenameRelpath()));
                          }
                      });

        text << wxT("Srcs=");

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        wxString relPath;

        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        int counter = 1;
        Compiler::CmpFileTypeInfo ft;

        for(size_t i = 0; i < files.size(); i++) {

            // is this a valid file?
            if(!cmp->GetCmpFileType(files[i].GetExt(), ft)) continue;

            if(ft.kind == Compiler::CmpFileKindResource) {
    #ifndef __WXMSW__
                // Resource compiler "windres" is not supported under
                // *nix OS
                continue;
    #endif
            }

            relPath = files.at(i).GetPath(true, wxPATH_UNIX);
            relPath.Trim().Trim(false);

            // Compiler::CmpFileKindSource , Compiler::CmpFileKindResource file
            text << relPath << files[i].GetFullName() << wxT(" ");

            if(counter % 10 == 0) { text << wxT("\\\n\t"); }
            counter++;
        }
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        m_objectChunks = 1;
        std::vector<wxFileName> files;

        std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                      [&](const Project::FilesMap_t::value_type& vt) {
                          clProjectFile::Ptr_t file = vt.second;
                          // Include only files that don't have the 'exclude from build' flag set
                          if(!file->IsExcludeFromConfiguration(confToBuild)) {
                              files.push_back(wxFileName(file->GetFilename()));
                          }
                      });

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();

        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        int counter = 1;
        Compiler::CmpFileTypeInfo ft;
        wxString projectPath = proj->GetFileName().GetPath();

        wxString objectsList;
        size_t objCounter = 0;
        int numOfObjectsInCurrentChunk = 0;
        wxString curChunk;

        // We break the list of files into a seriese of objects variables
        // each variable contains up to 100 files.
        // This is needed because on MSW, the ECHO command can not handle over 8K bytes
        // and gsgs uses the ECHO command to create a file with all the objects
        // so we can pass it to the liker
        for(size_t i = 0; i < files.size(); ++i) {

            // Did we collect 100 objects yet?
            if(i && ((i % 100) == 0)) {

                // only if this chunk contains objects (even one), add it to the makefile
                // otherwise, clear it and continue collecting
                if(numOfObjectsInCurrentChunk) {
                    curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
                    curChunk << "\n\n";
                    text << curChunk;
                    objCounter++;
                }

                // start a new chunk
                curChunk.clear();
                numOfObjectsInCurrentChunk = 0;
            }

            // is this a valid file?
            if(!cmp->GetCmpFileType(files[i].GetExt(), ft)) continue;

            if(ft.kind == Compiler::CmpFileKindResource && !OS_WINDOWS) { continue; }

            wxString objPrefix = DoGetTargetPrefix(files.at(i), projectPath, cmp);
            curChunk << wxT("$(IntermediateDirectory)/") << objPrefix << files[i].GetFullName() << wxT("$(ObjectSuffix) ");

            // for readability, break every 10 objects and start a new line
            if(counter % 10 == 0) { curChunk << wxT("\\\n\t"); }
            counter++;
            numOfObjectsInCurrentChunk++;
        }

        // Add any leftovers...
        if(numOfObjectsInCurrentChunk) {
            curChunk.Prepend(wxString() << "Objects" << objCounter << "=");
            curChunk << "\n\n";
            text << curChunk;
            objCounter++;
        }

        text << "\n\nObjects=";
        for(size_t i = 0; i < objCounter; ++i)
            text << "$(Objects" << i << ") ";

        text << wxT("\n\n");
        m_objectChunks = objCounter;
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // get the project specific build configuration for the workspace active
        // configuration
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);
        bool generateDependenciesFiles = cmp->GetGenerateDependeciesFile() && !cmp->GetDependSuffix().IsEmpty();
        bool supportPreprocessOnlyFiles =
            !cmp->GetSwitch(wxT("PreprocessOnly")).IsEmpty() && !cmp->GetPreprocessSuffix().IsEmpty();

        std::vector<wxFileName> abs_files, rel_paths;

        abs_files.reserve(m_projectFilesMetadata->size());
        rel_paths.reserve(m_projectFilesMetadata->size());

        std::for_each(m_projectFilesMetadata->begin(), m_projectFilesMetadata->end(),
                      [&](const Project::FilesMap_t::value_type& vt) {
                          clProjectFile::Ptr_t file = vt.second;
                          // Include only files that don't have the 'exclude from build' flag set
                          if(!file->IsExcludeFromConfiguration(confToBuild)) {
                              abs_files.push_back(wxFileName(file->GetFilename()));
                              rel_paths.push_back(wxFileName(file->GetFilenameRelpath()));
                          }
                      });

        text << wxT("\n\n");
        // create rule per object
        text << wxT("##\n");
        text << wxT("## Objects\n");
        text << wxT("##\n");

        Compiler::CmpFileTypeInfo ft;

        // Collect all the sub-directories that we generate files for
        wxArrayString subDirs;

        wxString cwd = proj->GetFileName().GetPath();

        for(size_t i = 0; i < abs_files.size(); i++) {
            // is this file interests the compiler?
            if(cmp->GetCmpFileType(abs_files.at(i).GetExt().Lower(), ft)) {
                wxString absFileName;
                wxFileName fn(abs_files.at(i));

                wxString filenameOnly = fn.GetName();
                wxString fullpathOnly = fn.GetFullPath();
                wxString fullnameOnly = fn.GetFullName();
                wxString compilationLine = ft.compilation_line;

                // use UNIX style slashes
                absFileName = abs_files[i].GetFullPath();
                absFileName.Replace(wxT("\\"), wxT("/"));
                wxString relPath;

                relPath = rel_paths.at(i).GetPath(true, wxPATH_UNIX);
                relPath.Trim().Trim(false);

                wxString objPrefix = DoGetTargetPrefix(abs_files.at(i), cwd, cmp);

                compilationLine.Replace(wxT("$(FileName)"), filenameOnly);
                compilationLine.Replace(wxT("$(FileFullName)"), fullnameOnly);
                compilationLine.Replace(wxT("$(FileFullPath)"), fullpathOnly);
                compilationLine.Replace(wxT("$(FilePath)"), relPath);

                // The object name is handled differently when using resource files
                compilationLine.Replace(wxT("$(ObjectName)"), objPrefix + fullnameOnly);
                compilationLine.Replace(wxT("\\"), wxT("/"));

                if(ft.kind == Compiler::CmpFileKindSource) {
                    wxString objectName;
                    wxString dependFile;
                    wxString preprocessedFile;

                    bool isCFile = FileUtil::GetType(rel_paths.at(i).GetFullName()) == FileUtil::TypeSourceC;

                    objectName << wxT("$(IntermediateDirectory)/") << objPrefix << fullnameOnly << wxT("$(ObjectSuffix)");
                    if(generateDependenciesFiles) {
                        dependFile << wxT("$(IntermediateDirectory)/") << objPrefix << fullnameOnly
                                   << wxT("$(DependSuffix)");
                    }
                    if(supportPreprocessOnlyFiles) {
                        preprocessedFile << wxT("$(IntermediateDirectory)/") << objPrefix << fullnameOnly
                                         << wxT("$(PreprocessSuffix)");
                    }

                    if(!isCFile) {
                        // Add the PCH include line
                        compilationLine.Replace(wxT("$(CXX)"), wxT("$(CXX) $(IncludePCH)"));
                    }

                    // set the file rule
                    text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT(" ") << dependFile
                         << wxT("\n");
                    text << wxT("\t") << compilationLine << wxT("\n");

                    wxString cmpOptions(wxT("$(CXXFLAGS) $(IncludePCH)"));
                    if(isCFile) { cmpOptions = wxT("$(CFLAGS)"); }

                    // set the source file we want to compile
                    wxString source_file_to_compile = rel_paths.at(i).GetFullPath(wxPATH_UNIX);
                    ::WrapWithQuotes(source_file_to_compile);

                    wxString compilerMacro = DoGetCompilerMacro(rel_paths.at(i).GetFullPath(wxPATH_UNIX));
                    if(generateDependenciesFiles) {
                        text << dependFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
                        text << wxT("\t") << wxT("@") << compilerMacro << wxT(" ") << cmpOptions
                             << wxT(" $(IncludePath) -MG -MP -MT") << objectName << wxT(" -MF") << dependFile
                             << wxT(" -MM ") << source_file_to_compile << wxT("\n\n");
                    }

                    if(supportPreprocessOnlyFiles) {
                        text << preprocessedFile << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
                        text << wxT("\t") << compilerMacro << wxT(" ") << cmpOptions
                             << wxT(" $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ") << preprocessedFile
                             << wxT(" ") << source_file_to_compile << wxT("\n\n");
                    }

                } else if(ft.kind == Compiler::CmpFileKindResource && OS_WINDOWS) {
                    // we construct an object name which also includes the full name of the reousrce file and appends a
                    // .o to the name (to be more precised, $(ObjectSuffix))
                    wxString objectName;
                    objectName << wxT("$(IntermediateDirectory)/") << objPrefix << fullnameOnly << wxT("$(ObjectSuffix)");

                    text << objectName << wxT(": ") << rel_paths.at(i).GetFullPath(wxPATH_UNIX) << wxT("\n");
                    text << wxT("\t") << compilationLine << wxT("\n");
                }
            }
        }

        if(generateDependenciesFiles) {
            text << wxT("\n");
            text << wxT("-include ") << wxT("$(IntermediateDirectory)/*$(DependSuffix)\n");
        }
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // get the project specific build configuration for the workspace active
        // configuration
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);
        wxString cmpType = bldConf->GetCompilerType();
        // get the compiler settings
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        // Can we use asterisk in the clean command?
        wxString imd = GetIntermediateFolder(bldConf);

        // add clean target
        text << wxT("##\n");
        text << wxT("## Clean\n");
        text << wxT("##\n");
        text << wxT("clean:\n");

        wxString cwd = proj->GetFileName().GetPath();
        if(!imd.IsEmpty()) {
            // Remove the entire build folder
            text << wxT("\t") << wxT("$(RM) -r ") << imd << "\n";

            // Remove the pre-compiled header
            wxString pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);

            if(pchFile.IsEmpty() == false) { text << wxT("\t") << wxT("$(RM) ") << pchFile << wxT(".gch") << wxT("\n"); }

        } else if(OS_WINDOWS) {
            text << wxT("\t") << wxT("$(RM) ") << imd << "*$(ObjectSuffix)" << wxT("\n");
            text << wxT("\t") << wxT("$(RM) ") << imd << "*$(DependSuffix)" << wxT("\n");
            // delete the output file as well
            wxString exeExt(wxEmptyString);
            if(proj->GetSettings()->GetProjectType(bldConf->GetName()) == PROJECT_TYPE_EXECUTABLE) {
                // under windows, g++ automatically adds the .exe extension to executable
                // make sure we delete it as well
                exeExt = wxT(".exe");
            }

            text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << wxT("\n");
            text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)") << exeExt << wxT("\n");
            text << wxT("\t") << wxT("$(RM) ") << DoGetMarkerFileDir(proj->GetName(), proj->GetFileName().GetPath())
                 << wxT("\n");

            // Remove the pre-compiled header
            wxString pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);

            if(pchFile.IsEmpty() == false) { text << wxT("\t") << wxT("$(RM) ") << pchFile << wxT(".gch") << wxT("\n"); }
        } else {
            text << wxT("\t") << wxT("$(RM) ") << imd << "*$(ObjectSuffix)" << wxT("\n");
            text << wxT("\t") << wxT("$(RM) ") << imd << "*$(DependSuffix)" << wxT("\n");

            // delete the output file as well
            text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)\n");
            text << wxT("\t") << wxT("$(RM) ") << DoGetMarkerFileDir(proj->GetName(), proj->GetFileName().GetPath())
                 << wxT("\n");

            // Remove the pre-compiled header
            wxString pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);

            if(pchFile.IsEmpty() == false) { text << wxT("\t") << wxT("$(RM) ") << pchFile << wxT(".gch") << wxT("\n"); }
        }
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // create a list of Sources
        // CreateSrcList(proj, confToBuild, text);  // Not used/needed for multi-step build
        // create a list of objects
        CreateObjectList(proj, confToBuild, text);
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text,
                                           wxString& targetName, const wxString& projName, const wxArrayString& depsProj)
    {
        // incase project is type exe or dll, force link
        // this is to workaround bug in the generated makefiles
        // which causes the makefile to report 'nothing to be done'
        // even when a dependency was modified
        text << wxT(".PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs\n");

        wxString extraDeps;
        wxString depsRules;

        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projName, errMsg);

        for(size_t i = 0; i < depsProj.GetCount(); i++) {
            wxFileName fn(depsProj.Item(i));
            // gsgs_Debug(wxT("making %s relative to %s"), fn.GetFullPath().c_str(),
            // proj->GetFileName().GetPath().c_str());
            fn.MakeRelativeTo(proj->GetProjectPath());
            extraDeps << "\"" << fn.GetFullPath() << wxT("\" ");

            depsRules << "\"" << fn.GetFullPath() << wxT("\":\n");
            // Make sure the dependecy directory exists
            depsRules << "\t@$(MakeDirCommand) \"" << fn.GetPath() << "\"\n";
            depsRules << wxT("\t@echo stam > ") << wxT("\"") << fn.GetFullPath() << wxT("\"\n");
            depsRules << wxT("\n\n");
        }

        if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            text << wxT("all: ");
            text << wxT("$(OutputFile)\n\n");

            text << wxT("$(OutputFile): $(IntermediateDirectory)/.d ");
            if(extraDeps.IsEmpty() == false) text << extraDeps;

            text << wxT("$(Objects) \n");
            targetName = wxT("$(IntermediateDirectory)/.d");

        } else {
            text << wxT("all: $(IntermediateDirectory) ");
            text << wxT("$(OutputFile)\n\n");
            text << wxT("$(OutputFile): $(Objects)\n");
        }

        if(bldConf->IsLinkerRequired()) {
            CreateTargets(type, bldConf, text, projName);

            if(type == PROJECT_TYPE_EXECUTABLE || type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
                if(depsRules.IsEmpty() == false) { text << wxT("\n") << depsRules << wxT("\n"); }
            }
        }
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text,
                                       const wxString& projName)
    {
        bool markRebuilt(true);
        text << wxT("\t@$(MakeDirCommand) $(@D)\n");
        text << wxT("\t@echo \"\" > $(IntermediateDirectory)/.d\n");

        CompilerPtr cmp = bldConf->GetCompiler();

        // this is a special target that creates a file with the content of the
        // $(Objects) variable (to be used with the @<file-name> option of the LD
        for(size_t i = 0; i < m_objectChunks; ++i) {
            wxString oper = ">>";
            if(i == 0) oper = " >";

            text << "\t@echo $(Objects" << i << ") " << oper << " $(ObjectsFileList)\n";
        }

        if(type == PROJECT_TYPE_STATIC_LIBRARY) {
            // create a static library
            // In any case add the 'objects_file' target here
            text << wxT("\t") << wxT("$(AR) $(ArchiveOutputSwitch)$(OutputFile)");
            if(cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) $(ArLibs)\n");
            } else {
                text << wxT(" $(Objects) $(ArLibs)\n");
            }

        } else if(type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            // create a shared library
            text << wxT("\t") << wxT("$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile)");
            if(cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) ");
            } else {
                text << wxT(" $(Objects) ");
            }
            text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");

        } else if(type == PROJECT_TYPE_EXECUTABLE) {
            // create an executable
            text << wxT("\t") << wxT("$(LinkerName) $(OutputSwitch)$(OutputFile)");
            if(cmp && cmp->GetReadObjectFilesFromList()) {
                text << wxT(" @$(ObjectsFileList) ");
            } else {
                text << wxT(" $(Objects) ");
            }
            text << wxT("$(LibPath) $(Libs) $(LinkOptions)\n");
            markRebuilt = false;
        }

        // If a link occurred, mark this project as "rebuilt" so the parent project will
        // know that a re-link is required
        if(bldConf->IsLinkerRequired() && markRebuilt) {
            text << wxT("\t@$(MakeDirCommand) \"") << DoGetMarkerFileDir(wxEmptyString) << wxT("\"\n");
            text << wxT("\t@echo rebuilt > ") << DoGetMarkerFileDir(projName) << wxT("\n");
        }
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        if(!HasPostbuildCommands(bldConf)) return;

        // generate postbuild commands
        BuildCommandList cmds;
        bldConf->GetPostBuildCommands(cmds);

        // Loop over the commands and replace any macros
        std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
            cmd.SetCommand(
                MacroManager::Instance()->Expand(cmd.GetCommand(), GetManager(), proj->GetName(), bldConf->GetName()));
        });

        text << wxT("\n");
        text << wxT("PostBuild:\n");
        text << wxT("\t@echo Executing Post Build commands ...\n");

        BuildCommandList::const_iterator iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) {
                // HACK:
                // If the command is 'copy' under Windows, make sure that
                // we set all slashes to backward slashes
                wxString command = iter->GetCommand();
                command.Trim().Trim(false);
                if(OS_WINDOWS && command.StartsWith(wxT("copy"))) { command.Replace(wxT("/"), wxT("\\")); }

                if(OS_WINDOWS && command.EndsWith(wxT("\\"))) { command.RemoveLast(); }

                text << wxT("\t") << iter->GetCommand() << wxT("\n");
            }
        }
        text << wxT("\t@echo Done\n");
    }
    //---------------------------------------------------------------------------
    bool BuilderGnuMake::HasPrebuildCommands(BuildConfigPtr bldConf) const
    {
        BuildCommandList cmds;
        bldConf->GetPreBuildCommands(cmds);

        BuildCommandList::const_iterator iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) { return true; }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;
        wxString name = bldConf->GetName();
        name = NormalizeConfigName(name);

        // add PrePreBuild
        wxString preprebuild = bldConf->GetPreBuildCustom();
        preprebuild.Trim().Trim(false);
        if(preprebuild.IsEmpty() == false) {
            text << wxT("PrePreBuild: ");
            text << bldConf->GetPreBuildCustom() << wxT("\n");
        }
        text << wxT("\n");
        bldConf->GetPreBuildCommands(cmds);

        // Loop over the commands and replace any macros
        std::for_each(cmds.begin(), cmds.end(), [&](BuildCommand& cmd) {
            cmd.SetCommand(
                MacroManager::Instance()->Expand(cmd.GetCommand(), GetManager(), proj->GetName(), bldConf->GetName()));
        });

        bool first(true);
        text << wxT("PreBuild:\n");
        if(!cmds.empty()) {
            iter = cmds.begin();
            for(; iter != cmds.end(); iter++) {
                if(iter->GetEnabled()) {
                    if(first) {
                        text << wxT("\t@echo Executing Pre Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if(!first) { text << wxT("\t@echo Done\n"); }
        }
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text)
    {
        wxString name = bldConf->GetName();
        name = NormalizeConfigName(name);

        wxString cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        wxString objectsFileName(proj->GetFileName().GetPath());
        objectsFileName << gsgsPathSep << proj->GetName() << wxT(".txt");

        text << wxT("## ") << name << wxT("\n");

        wxString outputFile = bldConf->GetOutputFileName();
        if(OS_WINDOWS && (bldConf->GetProjectType() == PROJECT_TYPE_EXECUTABLE || bldConf->GetProjectType().IsEmpty())) {
            outputFile.Trim().Trim(false);
        }

        // Expand the build macros into the generated makefile
        wxString projectName = proj->GetName();
        wxString projectpath, workspacepath, startupdir;
        workspacepath = gsgs_CppWorker().GetFileName().GetPath();
        projectpath = proj->GetFileName().GetPath();
        startupdir = gsgs_Path().GetStartupDirectory();
        workspacepath.Replace("\\", "/");
        projectpath.Replace("\\", "/");
        startupdir.Replace("\\", "/");

        text << "ProjectName            :=" << projectName << "\n";
        text << "ConfigurationName      :=" << name << "\n";
        text << "WorkspacePath          :=" << ::WrapWithQuotes(workspacepath) << "\n";
        text << "ProjectPath            :=" << ::WrapWithQuotes(projectpath) << "\n";
        text << "IntermediateDirectory  :=" << bldConf->GetIntermediateDirectory() << "\n";
        text << "OutDir                 := $(IntermediateDirectory)\n";
        text << "CurrentFileName        :=\n"; // TODO:: Need implementation
        text << "CurrentFilePath        :=\n"; // TODO:: Need implementation
        text << "CurrentFileFullPath    :=\n"; // TODO:: Need implementation
        text << "User                   :=" << wxGetUserName() << "\n";
        text << "Date                   :=" << wxDateTime::Now().FormatDate() << "\n";
        text << "gsgsPath           :=" << ::WrapWithQuotes(startupdir) << "\n";
        text << "LinkerName             :=" << cmp->GetTool("LinkerName") << "\n";
        text << "SharedObjectLinkerName :=" << cmp->GetTool("SharedObjectLinkerName") << "\n";
        text << "ObjectSuffix           :=" << cmp->GetObjectSuffix() << "\n";
        text << "DependSuffix           :=" << cmp->GetDependSuffix() << "\n";
        text << "PreprocessSuffix       :=" << cmp->GetPreprocessSuffix() << "\n";
        text << "DebugSwitch            :=" << cmp->GetSwitch("Debug") << "\n";
        text << "IncludeSwitch          :=" << cmp->GetSwitch("Include") << "\n";
        text << "LibrarySwitch          :=" << cmp->GetSwitch("Library") << "\n";
        text << "OutputSwitch           :=" << cmp->GetSwitch("Output") << "\n";
        text << "LibraryPathSwitch      :=" << cmp->GetSwitch("LibraryPath") << "\n";
        text << "PreprocessorSwitch     :=" << cmp->GetSwitch("Preprocessor") << "\n";
        text << "Operation           :=" << cmp->GetSwitch("Source") << "\n";
        text << "OutputFile             :=" << outputFile << "\n";
        text << "Preprocessors          :=" << ParsePreprocessor(bldConf->GetPreprocessor()) << "\n";
        text << "Objective           :=" << cmp->GetSwitch("Object") << "\n";
        text << "ArchiveOutputSwitch    :=" << cmp->GetSwitch("ArchiveOutput") << "\n";
        text << "PreprocessOnlySwitch   :=" << cmp->GetSwitch("PreprocessOnly") << "\n";

        wxFileName fnObjectsFileName(objectsFileName);
        fnObjectsFileName.MakeRelativeTo(proj->GetFileName().GetPath());

        text << wxT("ObjectsFileList        :=\"") << fnObjectsFileName.GetFullPath() << wxT("\"\n");
        text << wxT("PCHCompileFlags        :=") << bldConf->GetPchCompileFlags() << wxT("\n");

        wxString mkdirCommand = cmp->GetTool("MakeDirCommand");
        if(!mkdirCommand.IsEmpty()) {
            // use the compiler defined one
            text << wxT("MakeDirCommand         :=") << mkdirCommand << wxT("\n");

        } else {
            if(OS_WINDOWS) {
                text << wxT("MakeDirCommand         :=") << wxT("makedir") << wxT("\n");
            } else {
                text << wxT("MakeDirCommand         :=") << wxT("mkdir -p") << wxT("\n");
            }
        }

        wxString buildOpts = bldConf->GetCompileOptions();
        buildOpts.Replace(wxT(";"), wxT(" "));

        wxString cBuildOpts = bldConf->GetCCompileOptions();
        cBuildOpts.Replace(wxT(";"), wxT(" "));

        wxString asOptions = bldConf->GetAssmeblerOptions();
        asOptions.Replace(";", " ");

        // Let the plugins add their content here
        gsgs::BuildEvent e(wxEVT_GET_ADDITIONAL_COMPILEFLAGS);
        e.SetProjectName(proj->GetName());
        e.SetConfigurationName(bldConf->GetName());
        gsgs_Event().ProcessEvent(e);

        wxString additionalCompileFlags = e.GetCommand();
        if(additionalCompileFlags.IsEmpty() == false) {
            buildOpts << wxT(" ") << additionalCompileFlags;
            cBuildOpts << wxT(" ") << additionalCompileFlags;
        }

        // only if resource compiler required, evaluate the resource variables
        if(OS_WINDOWS) {
            wxString rcBuildOpts = bldConf->GetResCompileOptions();
            rcBuildOpts.Replace(wxT(";"), wxT(" "));
            text << wxT("RcCmpOptions           :=") << rcBuildOpts << wxT("\n");
            text << wxT("RcCompilerName         :=") << cmp->GetTool(wxT("ResourceCompiler")) << wxT("\n");
        }

        wxString linkOpt = bldConf->GetLinkOptions();
        linkOpt.Replace(wxT(";"), wxT(" "));

        // link options are kept with semi-colons, strip them
        text << wxT("LinkOptions            := ") << linkOpt << wxT("\n");

        // add the global include path followed by the project include path
        wxString pchFile;

        // If the PCH is required to be in the command line, add it here
        // otherwise, we just make sure it is generated and the compiler will pick it by itself
        if(bldConf->GetPchInCommandLine()) {
            pchFile = bldConf->GetPrecompiledHeader();
            pchFile.Trim().Trim(false);
            if(pchFile.IsEmpty() == false) { pchFile.Prepend(wxT(" -include ")).Append(wxT(" ")); }
        }

        wxString libraries = bldConf->GetLibraries();
        wxArrayString libsArr = ::wxStringTokenize(libraries, wxT(";"), wxTOKEN_STRTOK);
        libraries.Clear();
        libraries << wxT(" ");
        for(size_t i = 0; i < libsArr.GetCount(); i++) {
            libsArr.Item(i).Trim().Trim(false);
            libraries << wxT("\"") << libsArr.Item(i) << wxT("\" ");
        }

        text << wxT("IncludePath            := ")
             << ParseIncludePath(cmp->GetGlobalIncludePath(), proj->GetName(), bldConf->GetName()) << wxT(" ")
             << ParseIncludePath(bldConf->GetIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
        text << wxT("IncludePCH             := ") << pchFile << wxT("\n");
        text << wxT("RcIncludePath          := ")
             << ParseIncludePath(bldConf->GetResCmpIncludePath(), proj->GetName(), bldConf->GetName()) << wxT("\n");
        text << wxT("Libs                   := ") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");
        text << wxT("ArLibs                 := ") << libraries << wxT("\n");

        // add the global library path followed by the project library path
        text << wxT("LibPath                :=")
             << ParseLibPath(cmp->GetGlobalLibPath(), proj->GetName(), bldConf->GetName()) << wxT(" ")
             << ParseLibPath(bldConf->GetLibPath(), proj->GetName(), bldConf->GetName()) << wxT("\n");

        text << wxT("\n");
        text << wxT("##\n");
        text << wxT("## Common variables\n");
        text << wxT("## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables\n");
        text << wxT("##\n");
        text << wxT("AR       := ") << cmp->GetTool(wxT("AR")) << wxT("\n");
        text << wxT("CXX      := ") << cmp->GetTool(wxT("CXX")) << wxT("\n");
        text << wxT("CC       := ") << cmp->GetTool(wxT("CC")) << wxT("\n");
        text << wxT("CXXFLAGS := ") << buildOpts << wxT(" $(Preprocessors)") << wxT("\n");
        text << wxT("CFLAGS   := ") << cBuildOpts << wxT(" $(Preprocessors)") << wxT("\n");
        text << wxT("ASFLAGS  := ") << asOptions << "\n";
        text << wxT("AS       := ") << cmp->GetTool(wxT("AS")) << "\n";
        text << wxT("\n\n");
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf)
    {
        // convert semi-colon delimited string into GNU list of
        // include paths:
        wxString incluedPath(wxEmptyString);
        wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
        while(tkz.HasMoreTokens()) {
            wxString path(tkz.NextToken());
            TrimString(path);
            ::WrapWithQuotes(path);
            incluedPath << wxT("$(IncludeSwitch)") << path << wxT(" ");
        }
        return incluedPath;
    }

    wxString BuilderGnuMake::ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf)
    {
        // convert semi-colon delimited string into GNU list of
        // lib path
        wxString libPath(wxEmptyString);
        wxStringTokenizer tkz(paths, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with libpath switch
        while(tkz.HasMoreTokens()) {
            wxString path(tkz.NextToken());
            path.Trim().Trim(false);
            ::WrapWithQuotes(path);
            libPath << wxT("$(LibraryPathSwitch)") << path << wxT(" ");
        }
        return libPath;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::ParsePreprocessor(const wxString& prep)
    {
        wxString preprocessor(wxEmptyString);
        wxStringTokenizer tkz(prep, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with libpath switch
        while(tkz.HasMoreTokens()) {
            wxString p(tkz.NextToken());
            p.Trim().Trim(false);
            preprocessor << wxT("$(PreprocessorSwitch)") << p << wxT(" ");
        }

        // if the macro contains # escape it
        // But first remove any manual escaping done by the user
        preprocessor.Replace(wxT("\\#"), wxT("#"));
        preprocessor.Replace(wxT("#"), wxT("\\#"));
        return preprocessor;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::ParseLibs(const wxString& libs)
    {
        // convert semi-colon delimited string into GNU list of
        // libs
        wxString slibs(wxEmptyString);
        wxStringTokenizer tkz(libs, wxT(";"), wxTOKEN_STRTOK);
        // prepend each include path with -l and strip trailing lib string
        // also, if the file contains an extension (.a, .so, .dynlib) remove them as well
        while(tkz.HasMoreTokens()) {
            wxString lib(tkz.NextToken());
            TrimString(lib);
            // remove lib prefix
            if(lib.StartsWith(wxT("lib"))) { lib = lib.Mid(3); }

            // remove known suffixes
            if(lib.EndsWith(wxT(".a")) || lib.EndsWith(wxT(".so")) || lib.EndsWith(wxT(".dylib")) ||
               lib.EndsWith(wxT(".dll"))) {
                lib = lib.BeforeLast(wxT('.'));
            }

            slibs << wxT("$(LibrarySwitch)") << lib << wxT(" ");
        }
        return slibs;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetBuildCommand(const wxString& project, const wxString& confToBuild,
                                             const wxString& arguments)
    {
        wxString errMsg, cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if(!bldConf) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, false, false, errMsg);

        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        buildTool.Replace(wxT("\\"), wxT("/"));
        cmd << buildTool << wxT(" Makefile");
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetCleanCommand(const wxString& project, const wxString& confToBuild,
                                             const wxString& arguments)
    {
        wxString errMsg, cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if(!bldConf) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, false, false, errMsg);

        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);

        // fix: replace all Windows like slashes to POSIX
        buildTool.Replace(wxT("\\"), wxT("/"));

        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
        cmd << buildTool << wxT(" Makefile clean");
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetPOBuildCommand(const wxString& project, const wxString& confToBuild,
                                               const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), kIncludePreBuild | kIncludePostBuild);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetPOCleanCommand(const wxString& project, const wxString& confToBuild,
                                               const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("clean"), kCleanOnly | kIncludePreBuild);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetSingleFileCmd(const wxString& project, const wxString& confToBuild,
                                              const wxString& arguments, const wxString& fileName)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);

        // Build the target list
        wxString target;
        wxString cmpType;
        wxFileName fn(fileName);

        if(FileUtil::GetType(fileName) == FileUtil::TypeHeader) {
            // Attempting to build a header file, try to see if we got an implementation file instead
            // We had the current extension to the array so incase we loop over the entire array
            // we remain with the original file name unmodified
            std::vector<wxString> implExtensions = { "cpp", "cxx", "cc", "c++", "c", fn.GetExt() };
            for(const wxString& ext : implExtensions) {
                fn.SetExt(ext);
                if(fn.FileExists()) { break; }
            }
        }

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if(!bldConf) { return wxEmptyString; }

        cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);
        // fn.MakeRelativeTo(proj->GetFileName().GetPath());

        wxString relPath = fn.GetPath(true, wxPATH_UNIX);
        wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
        target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetFullName()
               << cmp->GetObjectSuffix();

        target = ExpandAllVariables(target, &gsgs_CppWorker(), proj->GetName(), confToBuild, wxEmptyString);
        cmd = GetProjectMakeCommand(proj, confToBuild, target, kIncludePreBuild);

        return gsgs_EditorConfig().ExpandVariables(cmd, true);
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
                                                  const wxString& arguments, const wxString& fileName, wxString& errMsg)
    {
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) { return wxEmptyString; }

        wxString cmd;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if(!bldConf) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);

        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString buildTool = GetBuildToolCommand(project, confToBuild, arguments, true);
        wxString type = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), project);

        // fix: replace all Windows like slashes to POSIX
        buildTool.Replace(wxT("\\"), wxT("/"));

        // create the target
        wxString target;
        wxString objSuffix;
        wxFileName fn(fileName);

        wxString cmpType = bldConf->GetCompilerType();
        CompilerPtr cmp = gsgs_Settings().GetCompiler(cmpType);

        wxString objNamePrefix = DoGetTargetPrefix(fn, proj->GetFileName().GetPath(), cmp);
        target << bldConf->GetIntermediateDirectory() << wxT("/") << objNamePrefix << fn.GetFullName()
               << cmp->GetPreprocessSuffix();

        target = ExpandAllVariables(target, &gsgs_CppWorker(), proj->GetName(), confToBuild, wxEmptyString);
        cmd = GetProjectMakeCommand(proj, confToBuild, target, kIncludePreBuild);
        return gsgs_EditorConfig().ExpandVariables(cmd, true);
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetCdCmd(const wxFileName& path1, const wxFileName& path2)
    {
        wxString cd_cmd(wxT("@"));
        if(path2.GetPath().IsEmpty()) { return cd_cmd; }

        if(path1.GetPath() != path2.GetPath()) { cd_cmd << wxT("cd \"") << path2.GetPath() << wxT("\" && "); }
        return cd_cmd;
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;

        cmds.clear();
        bldConf->GetPostBuildCommands(cmds);
        bool first(true);
        if(!cmds.empty()) {
            iter = cmds.begin();
            for(; iter != cmds.end(); iter++) {
                if(iter->GetEnabled()) {
                    if(first) {
                        text << wxT("\t@echo Executing Post Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if(!first) { text << wxT("\t@echo Done\n"); }
        }
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text)
    {
        BuildCommandList cmds;
        BuildCommandList::iterator iter;

        cmds.clear();
        bldConf->GetPreBuildCommands(cmds);
        bool first(true);
        if(!cmds.empty()) {
            iter = cmds.begin();
            for(; iter != cmds.end(); iter++) {
                if(iter->GetEnabled()) {
                    if(first) {
                        text << wxT("\t@echo Executing Pre Build commands ...\n");
                        first = false;
                    }
                    text << wxT("\t") << iter->GetCommand() << wxT("\n");
                }
            }
            if(!first) { text << wxT("\t@echo Done\n"); }
        }
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath,
                                                   ProjectPtr proj, const wxString& confToBuild)
    {
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);

        // iterate over the dependencies projects and generate makefile
        wxString makeCommand;
        wxString basicMakeCommand;

        wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", false);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);
        basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\"");

        makeCommand << wxT("\t") << GetCdCmd(wspfile, projectPath);

        if(bldConf) {
            wxString preprebuild = bldConf->GetPreBuildCustom();
            wxString precmpheader = bldConf->GetPrecompiledHeader();
            precmpheader.Trim().Trim(false);
            preprebuild.Trim().Trim(false);

            if(preprebuild.IsEmpty() == false) { makeCommand << basicMakeCommand << wxT(" PrePreBuild && "); }

            if(HasPrebuildCommands(bldConf)) { makeCommand << basicMakeCommand << wxT(" PreBuild && "); }

            // Run pre-compiled header compilation if any
            if(precmpheader.IsEmpty() == false) {
                makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
            }
        }

        // the main command
        makeCommand << basicMakeCommand;

        // post
        if(bldConf && HasPostbuildCommands(bldConf)) {
            makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
        }
        makeCommand << wxT("\n");
        return makeCommand;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
                                                   size_t flags)
    {
        bool bCleanOnly = flags & kCleanOnly;
        bool bIncludePreBuild = flags & kIncludePreBuild;
        bool bIncludePostBuild = flags & kIncludePostBuild;
        bool bAddCleanTarget = flags & kAddCleanTarget;

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), confToBuild);

        // iterate over the dependencies projects and generate makefile
        wxString makeCommand;
        wxString basicMakeCommand;

        wxString buildTool = GetBuildToolCommand(proj->GetName(), confToBuild, "", true);
        buildTool = gsgs_EditorConfig().ExpandVariables(buildTool, true);
        basicMakeCommand << buildTool << wxT(" \"") << proj->GetName() << wxT(".mk\" ");

        if(bAddCleanTarget) { makeCommand << basicMakeCommand << wxT(" clean && "); }

        if(bldConf && !bCleanOnly) {
            wxString preprebuild = bldConf->GetPreBuildCustom();
            wxString precmpheader = bldConf->GetPrecompiledHeader();
            precmpheader.Trim().Trim(false);
            preprebuild.Trim().Trim(false);

            makeCommand << basicMakeCommand << " MakeIntermediateDirs && ";

            if(!preprebuild.IsEmpty()) { makeCommand << basicMakeCommand << wxT(" PrePreBuild && "); }

            if(bIncludePreBuild && HasPrebuildCommands(bldConf)) {
                makeCommand << basicMakeCommand << wxT(" PreBuild && ");
            }

            // Run pre-compiled header compilation if any
            if(!precmpheader.IsEmpty()) {
                makeCommand << basicMakeCommand << wxT(" ") << precmpheader << wxT(".gch") << wxT(" && ");
            }
        }

        makeCommand << basicMakeCommand << wxT(" ") << target;

        // post
        if(bldConf && !bCleanOnly && bIncludePostBuild && HasPostbuildCommands(bldConf)) {
            makeCommand << wxT(" && ") << basicMakeCommand << wxT(" PostBuild");
        }
        return makeCommand;
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMake::CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text)
    {
        wxString filename = bldConf->GetPrecompiledHeader();
        filename.Trim().Trim(false);

        if(filename.IsEmpty()) return;

        text << wxT("\n");
        text << wxT("# PreCompiled Header\n");
        text << filename << wxT(".gch: ") << filename << wxT("\n");
        if(bldConf->GetPCHFlagsPolicy() == BuildConfig::kPCHPolicyReplace) {
            text << wxT("\t") << DoGetCompilerMacro(filename) << wxT(" $(Operation) ") << filename
                 << wxT(" $(PCHCompileFlags)\n");

        } else { // BuildConfig::kPCHPolicyAppend
            text << wxT("\t") << DoGetCompilerMacro(filename) << wxT(" $(Operation) ") << filename
                 << wxT(" $(PCHCompileFlags) $(CXXFLAGS) $(IncludePath)\n");
        }
        text << wxT("\n");
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
                                                 const wxString& arguments)
    {
        wxString errMsg, cmd;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!proj) { return wxEmptyString; }

        // generate the makefile
        Export(project, confToBuild, arguments, true, false, errMsg);
        cmd = GetProjectMakeCommand(proj, confToBuild, wxT("all"), kIncludePreBuild | kIncludePostBuild | kAddCleanTarget);
        return cmd;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
                                                 const wxString& arguments, bool isCommandlineCommand) const
    {
        wxString jobsCmd;
        wxString buildTool;

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, confToBuild);
        if(!bldConf) return wxEmptyString;

        CompilerPtr compiler = bldConf->GetCompiler();
        if(!compiler) return wxEmptyString;

        if(isCommandlineCommand) {
            buildTool = compiler->GetTool("MAKE");

        } else {
            jobsCmd = wxEmptyString;
            buildTool = wxT("\"$(MAKE)\"");
        }

        if(buildTool.Lower().Contains("make")) {
            if(isCommandlineCommand) {
                return buildTool + " -e -f ";

            } else {
                return buildTool + " -f ";
            }

        } else {
            // Just return the build command as appears in the toolchain
            return buildTool + " ";
        }
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::DoGetCompilerMacro(const wxString& filename)
    {
        wxString compilerMacro(wxT("$(CXX)"));
        switch(FileUtil::GetType(filename)) {
        case FileUtil::TypeSourceC:
            compilerMacro = wxT("$(CC)");
            break;
        case FileUtil::TypeSourceCpp:
        default:
            compilerMacro = wxT("$(CXX)");
            break;
        }
        return compilerMacro;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp)
    {
        wxString lastDir;
        wxString ret;

        if(cwd == filename.GetPath()) return wxEmptyString;

        if(cmp && cmp->GetObjectNameIdenticalToFileName()) return wxEmptyString;

        if(cwd == filename.GetPath()) { return wxEmptyString; }

        // remove cwd from filename
        wxFileName relpath = filename;
        relpath.MakeRelativeTo(cwd);

        const wxArrayString& dirs = relpath.GetDirs();
        for(int i = 0; i < (int)dirs.size(); ++i) {
            lastDir = dirs.Item(i);

            // Handle special directory paths
            if(lastDir == wxT("..")) {
                lastDir = wxT("up");

            } else if(lastDir == wxT(".")) {
                lastDir = wxT("cur");
            }

            if(lastDir.IsEmpty() == false) { lastDir << wxT("_"); }

            ret += lastDir;
        }

        return ret;
    }
    //---------------------------------------------------------------------------
    wxString BuilderGnuMake::DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath)
    {
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

        workspaceSelConf = NormalizeConfigName(workspaceSelConf);
        workspaceSelConf.MakeLower();

        wxString path;
        if(projname.IsEmpty()) {
            path << gsgs_CppWorker().GetFileName().GetPath() << "/" << wxT(".build-") << workspaceSelConf;

        } else {
            path << gsgs_CppWorker().GetFileName().GetPath() << "/" << wxT(".build-") << workspaceSelConf
                 << "/" << projname;
        }

        if(projectPath.IsEmpty() == false) {
            wxFileName fn(path);
            fn.MakeRelativeTo(projectPath);
            path = fn.GetFullPath(wxPATH_UNIX);
        }

        if(!projname.IsEmpty())
            return "\"" + path + "\"";
        else
            return path;
    }
    //---------------------------------------------------------------------------
    bool BuilderGnuMake::HasPostbuildCommands(BuildConfigPtr bldConf) const
    {
        BuildCommandList cmds;
        bldConf->GetPostBuildCommands(cmds);

        BuildCommandList::const_iterator iter = cmds.begin();
        for(; iter != cmds.end(); iter++) {
            if(iter->GetEnabled()) { return true; }
        }
        return false;
    }
    //---------------------------------------------------------------------------
    bool BuilderGnuMake::SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName)
    {
        gsgs::BuildEvent e(eventId);
        e.SetProjectName(projectName);
        e.SetConfigurationName(configurationName);
        return gsgs_Event().ProcessEvent(e);
    }
    //---------------------------------------------------------------------------

    class _gsgsAPI BuilderGnuMakeOneStep : public BuilderGnuMake
    {
    public:
        BuilderGnuMakeOneStep();
        virtual ~BuilderGnuMakeOneStep();

        /*	These methods available but supplied by baseclass BuilderGnuMake
                virtual bool Export(const wxString &project, const wxString &confToBuild, bool isProjectOnly, bool force,
           wxString &errMsg);
                virtual wxString GetBuildCommand(const wxString &project, const wxString &confToBuild);
                virtual wxString GetCleanCommand(const wxString &project, const wxString &confToBuild);
                virtual wxString GetPOBuildCommand(const wxString &project, const wxString &confToBuild);
                virtual wxString GetPOCleanCommand(const wxString &project, const wxString &confToBuild);
                virtual wxString GetSingleFileCmd(const wxString &project, const wxString &confToBuild, const wxString
           &fileName);
                virtual wxString GetPreprocessFileCmd(const wxString &project, const wxString &confToBuild, const wxString
           &fileName, wxString &errMsg);
                virtual wxString GetPORebuildCommand(const wxString &project, const wxString &confToBuild);
        */
    protected:
        virtual void CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        virtual void CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName);
        virtual void CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
    private:
        void CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text);
    };
    //---------------------------------------------------------------------------
#if PERFORMANCE
#define TIMER_START() \
        {                 \
            g_sw.Start(); \
        }

#define PRINT_TIMESTAMP(msg)                                     \
        {                                                            \
            wxString log_msg(msg);                                   \
            wxPrintf(wxT("%08d: %s"), g_sw.Time(), log_msg.c_str()); \
        }
#else
#define TIMER_START()
#define PRINT_TIMESTAMP(msg)
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// BuilderGnuMakeOneStep
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
    BuilderGnuMakeOneStep::BuilderGnuMakeOneStep()
        : BuilderGnuMake(wxT("GNU makefile onestep build"), wxT("make"), wxT("-f"))
    {
    }
    //---------------------------------------------------------------------------
    BuilderGnuMakeOneStep::~BuilderGnuMakeOneStep() {}

    void BuilderGnuMakeOneStep::CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // create a list of Sources
        BuilderGnuMake::CreateSrcList(proj, confToBuild, text);
        // create a list of objects
        BuilderGnuMake::CreateObjectList(proj, confToBuild, text);
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMakeOneStep::CreateLinkTargets(
        const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName)
    {
        // specify outfile dependency directly on source files (only)
        text << wxT("all: $(OutputFile)\n\n");
        text << wxT("$(OutputFile): makeDirStep $(Srcs)\n");
        targetName = wxT("makeDirStep");
        CreateTargets(type, bldConf, text); // overridden CreateTargets

        // if (bldConf->IsLinkerRequired()) {
        //	CreateTargets(type, bldConf, text);
        //}
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMakeOneStep::CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text)
    {
        // override to do do nothing (no link objects) build rule already given in CreateLinkTargets
    }
    //---------------------------------------------------------------------------
    void BuilderGnuMakeOneStep::CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text)
    {
        if (OS_WINDOWS) {
            text << wxT("\t") << wxT("@makedir $(@D)\n");
        }
        else {
            text << wxT("\t") << wxT("@mkdir -p $(@D)\n");
        }

        // these patterns below should be all set and loadable from configuration file.
        // compilerName is tool that does single stage construction srcs -> exe/lib

        if (type == PROJECT_TYPE_STATIC_LIBRARY) {
            // create a static library
            text << wxT("\t")
                << wxT("$(CXX) $(ArchiveOutputSwitch) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
        }
        else if (type == PROJECT_TYPE_DYNAMIC_LIBRARY) {
            // create a shared library
            text << wxT("\t") << wxT("$(CXX) $(Objective) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
        }
        else if (type == PROJECT_TYPE_EXECUTABLE) {
            // create an executable
            text << wxT("\t") << wxT("$(CXX) $(Operation) $(OutputSwitch)$(OutputFile) $(Libs) $(CXXFLAGS) $(Srcs)\n");
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // BuildManager
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BuildManager::BuildManager()
    {
        // register all builders here
        AddBuilder(new BuilderGnuMake());
        AddBuilder(new BuilderGnuMakeOneStep());
#ifdef __WXMSW__
        AddBuilder(new BuilderNMake());
#endif
    }
    //---------------------------------------------------------------------------
    BuildManager::~BuildManager() { m_builders.clear(); }
    //---------------------------------------------------------------------------
    void BuildManager::AddBuilder(BuilderPtr builder)
    {
        if (!builder) {
            return;
        }

        m_builders[builder->GetName()] = builder;
    }
    //---------------------------------------------------------------------------
    void BuildManager::RemoveBuilder(const wxString& name)
    {

        std::map<wxString, BuilderPtr>::iterator iter = m_builders.find(name);
        if (iter != m_builders.end()) {
            m_builders.erase(iter);
        }
    }
    //---------------------------------------------------------------------------
    void BuildManager::GetBuilders(std::list<wxString>& list)
    {
        std::map<wxString, BuilderPtr>::iterator iter = m_builders.begin();
        for (; iter != m_builders.end(); iter++) {
            list.push_back(iter->second->GetName());
        }
    }
    //---------------------------------------------------------------------------
    BuilderPtr BuildManager::GetBuilder(const wxString& name)
    {
        std::map<wxString, BuilderPtr>::iterator iter = m_builders.begin();
        for (; iter != m_builders.end(); iter++) {
            if (iter->first == name) {
                return iter->second;
            }
        }

        // return the default builder
        return m_builders.begin()->second;
    }
    //---------------------------------------------------------------------------
    static BuildManager * gs_BuildManager = NULL;
    //---------------------------------------------------------------------------
    void BuildManager::Release()
    {
        if (gs_BuildManager)
        {
            delete gs_BuildManager;
            gs_BuildManager = NULL;
        }
    }
    //---------------------------------------------------------------------------
    BuildManager & BuildManager::singleton()
    {
        if (gs_BuildManager == NULL)
            gs_BuildManager = new BuildManager;
        return *gs_BuildManager;
    }
    //---------------------------------------------------------------------------
}
