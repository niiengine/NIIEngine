
#include "gsgsMacroManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsBuildConfig.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsToolsetManager.h"
#include "gsgsWorkspace.h"
#include "imanager.h"
#include "globals.h"
#include <wx/regex.h>

namespace gsgs
{
    //----------------------------------------------------------------------------
    MacroManager::MacroManager() {}
    //----------------------------------------------------------------------------
    MacroManager::~MacroManager() {}
    //----------------------------------------------------------------------------
    MacroManager* MacroManager::Instance()
    {
        static MacroManager gInstance;
        return &gInstance;
    }
    //----------------------------------------------------------------------------
    wxString MacroManager::Expand(const wxString& expression, IManager* manager, const wxString& project,
                                  const wxString& confToBuild)
    {
        return DoExpand(expression, manager, project, true, confToBuild);
    }
    //----------------------------------------------------------------------------
    wxString MacroManager::Replace(const wxString& inString, const wxString& variableName, const wxString& replaceWith,
                                   bool bIgnoreCase)
    {
        size_t flags = wxRE_DEFAULT;
        if(bIgnoreCase) flags |= wxRE_ICASE;

        wxString strRe1;
        wxString strRe2;
        wxString strRe3;
        wxString strRe4;

        strRe1 << wxT("\\$\\((") << variableName << wxT(")\\)");
        strRe2 << wxT("\\$\\{(") << variableName << wxT(")\\}");
        strRe3 << wxT("\\$(") << variableName << wxT(")");
        strRe4 << wxT("%(") << variableName << wxT(")%");

        wxRegEx reOne(strRe1, flags);   // $(variable)
        wxRegEx reTwo(strRe2, flags);   // ${variable}
        wxRegEx reThree(strRe3, flags); // $variable
        wxRegEx reFour(strRe4, flags);  // %variable%

        wxString result = inString;
        if(reOne.Matches(result)) { reOne.ReplaceAll(&result, replaceWith); }

        if(reTwo.Matches(result)) { reTwo.ReplaceAll(&result, replaceWith); }

        if(reThree.Matches(result)) { reThree.ReplaceAll(&result, replaceWith); }

        if(reFour.Matches(result)) { reFour.ReplaceAll(&result, replaceWith); }
        return result;
    }
    //----------------------------------------------------------------------------
    bool MacroManager::FindVariable(const wxString& inString, wxString& name, wxString& fullname)
    {
        size_t flags = wxRE_DEFAULT | wxRE_ICASE;

        wxString strRe1;
        wxString strRe2;
        wxString strRe3;
        wxString strRe4;

        strRe1 << wxT("\\$\\((") << wxT("[a-z_0-9]+") << wxT(")\\)");
        strRe2 << wxT("\\$\\{(") << wxT("[a-z_0-9]+") << wxT(")\\}");
        strRe3 << wxT("\\$(") << wxT("[a-z_0-9]+") << wxT(")");
        strRe4 << wxT("%(") << wxT("[a-z_0-9]+") << wxT(")%");

        wxRegEx reOne(strRe1, flags);   // $(variable)
        wxRegEx reTwo(strRe2, flags);   // ${variable}
        wxRegEx reThree(strRe3, flags); // $variable
        wxRegEx reFour(strRe4, flags);  // %variable%

        if(reOne.Matches(inString)) {
            name = reOne.GetMatch(inString, 1);
            fullname = reOne.GetMatch(inString);
            return true;
        }

        if(reTwo.Matches(inString)) {
            name = reTwo.GetMatch(inString, 1);
            fullname = reTwo.GetMatch(inString);
            return true;
        }

        if(reThree.Matches(inString)) {
            name = reThree.GetMatch(inString, 1);
            fullname = reThree.GetMatch(inString);
            return true;
        }

        if(reFour.Matches(inString)) {
            name = reFour.GetMatch(inString, 1);
            fullname = reFour.GetMatch(inString);
            return true;
        }
        return false;
    }
    //----------------------------------------------------------------------------
    wxString MacroManager::ExpandNoEnv(const wxString& expression, const wxString& project, const wxString& confToBuild)
    {
        return DoExpand(expression, NULL, project, false, confToBuild);
    }
    //----------------------------------------------------------------------------
    wxString MacroManager::DoExpand(const wxString& expression, IManager* manager, const wxString& project, bool applyEnv,
                                    const wxString& confToBuild)
    {
        wxString expandedString(expression);
        CxxWS* workspace = &gsgs_CppWorker();
        // IWorkspace* workspace = gsgs_Workspace().IsValid();

        if(!manager) { manager = GetManager(); }

        size_t retries = 0;
        wxString dummyname, dummfullname;
        while((retries < 5) && FindVariable(expandedString, dummyname, dummyname)) {
            ++retries;
            DollarEscaper de(expandedString);
            if(workspace) {
                expandedString.Replace(wxT("$(WorkspaceName)"), workspace->GetName());
                // Support the new $(WorkspaceConfiguration) macro
                WorkspaceConfigurationPtr wspConfig = workspace->GetSelectedConfig();
                if(wspConfig) {
                    expandedString.Replace("$(WorkspaceConfiguration)", wspConfig->GetName());
                } else {
                    expandedString.Replace("$(WorkspaceConfiguration)", "");
                }

                ProjectPtr proj = workspace->GetProject(project);
                if(proj) {
                    wxString prjBuildWd;
                    wxString prjRunWd;

                    wxString project_name(proj->GetName());

                    // make sure that the project name does not contain any spaces
                    project_name.Replace(wxT(" "), wxT("_"));

                    BuildConfigPtr bldConf = workspace->GetProjBuildConf(proj->GetName(), confToBuild);
                    if(bldConf) {
                        bool isCustom = bldConf->IsCustomBuild();
                        expandedString.Replace(wxT("$(ProjectOutputFile)"), bldConf->GetOutputFileName());
                        // An alias
                        expandedString.Replace(wxT("$(OutputFile)"), bldConf->GetOutputFileName());

                        // When custom build project, use the working directory set in the
                        // custom build tab, otherwise use the project file's path
                        prjBuildWd = isCustom ? bldConf->GetCustomBuildWorkingDir() : proj->GetFileName().GetPath();
                        prjRunWd = bldConf->GetWorkingDirectory();
                    }

                    expandedString.Replace(wxT("$(ProjectWorkingDirectory)"), prjBuildWd);
                    expandedString.Replace(wxT("$(ProjectRunWorkingDirectory)"), prjRunWd);
                    expandedString.Replace(wxT("$(ProjectPath)"), proj->GetFileName().GetPath());
                    expandedString.Replace(wxT("$(WorkspacePath)"), workspace->GetFileName().GetPath());
                    expandedString.Replace(wxT("$(ProjectName)"), project_name);

                    if(bldConf) {
                        expandedString.Replace(wxT("$(IntermediateDirectory)"), bldConf->GetIntermediateDirectory());
                        expandedString.Replace(wxT("$(ConfigurationName)"), bldConf->GetName());
                        expandedString.Replace(wxT("$(OutDir)"), bldConf->GetIntermediateDirectory());

                        // Compiler-related variables

                        wxString cFlags = bldConf->GetCCompileOptions();
                        cFlags.Replace(wxT(";"), wxT(" "));
                        expandedString.Replace(wxT("$(CC)"), bldConf->GetCompiler()->GetTool("CC"));
                        expandedString.Replace(wxT("$(CFLAGS)"), cFlags);

                        wxString cxxFlags = bldConf->GetCompileOptions();
                        cxxFlags.Replace(wxT(";"), wxT(" "));
                        expandedString.Replace(wxT("$(CXX)"), bldConf->GetCompiler()->GetTool("CXX"));
                        expandedString.Replace(wxT("$(CXXFLAGS)"), cxxFlags);

                        wxString ldFlags = bldConf->GetLinkOptions();
                        ldFlags.Replace(wxT(";"), wxT(" "));
                        expandedString.Replace(wxT("$(LDFLAGS)"), ldFlags);

                        wxString asFlags = bldConf->GetAssmeblerOptions();
                        asFlags.Replace(wxT(";"), wxT(" "));
                        expandedString.Replace(wxT("$(AS)"), bldConf->GetCompiler()->GetTool("AS"));
                        expandedString.Replace(wxT("$(ASFLAGS)"), asFlags);

                        wxString resFlags = bldConf->GetResCompileOptions();
                        resFlags.Replace(wxT(";"), wxT(" "));
                        expandedString.Replace(wxT("$(RES)"), bldConf->GetCompiler()->GetTool("ResourceCompiler"));
                        expandedString.Replace(wxT("$(RESFLAGS)"), resFlags);

                        expandedString.Replace(wxT("$(AR)"), bldConf->GetCompiler()->GetTool("AR"));

                        expandedString.Replace(wxT("$(MAKE)"), bldConf->GetCompiler()->GetTool("MAKE"));

                        expandedString.Replace(wxT("$(IncludePath)"), bldConf->GetIncludePath());
                        expandedString.Replace(wxT("$(LibraryPath)"), bldConf->GetLibPath());
                        expandedString.Replace(wxT("$(ResourcePath)"), bldConf->GetResCmpIncludePath());
                        expandedString.Replace(wxT("$(LinkLibraries)"), bldConf->GetLibraries());
                    }

                    if(expandedString.Find(wxT("$(ProjectFiles)")) != wxNOT_FOUND)
                        expandedString.Replace(wxT("$(ProjectFiles)"), proj->GetFilesAsString(false));

                    if(expandedString.Find(wxT("$(ProjectFilesAbs)")) != wxNOT_FOUND)
                        expandedString.Replace(wxT("$(ProjectFilesAbs)"), proj->GetFilesAsString(true));
                }
            }

            if(manager) {
                IEditor* editor = manager->GetActiveEditor();
                if(editor) {
                    wxFileName fn(editor->GetFileName());

                    expandedString.Replace(wxT("$(CurrentFileName)"), fn.GetName());

                    wxString fpath(fn.GetPath());
                    fpath.Replace(wxT("\\"), wxT("/"));
                    expandedString.Replace(wxT("$(CurrentFilePath)"), fpath);
                    expandedString.Replace(wxT("$(CurrentFileExt)"), fn.GetExt());
                    expandedString.Replace(wxT("$(CurrentFileFullName)"), fn.GetFullName());

                    wxString ffullpath(fn.GetFullPath());
                    ffullpath.Replace(wxT("\\"), wxT("/"));
                    expandedString.Replace(wxT("$(CurrentFileFullPath)"), ffullpath);
                    expandedString.Replace(wxT("$(CurrentSelection)"), editor->GetSelection());
                    if(expandedString.Find(wxT("$(CurrentSelectionRange)")) != wxNOT_FOUND) {
                        int start = editor->GetSelectionStart(), end = editor->GetSelectionEnd();

                        wxString output = wxString::Format(wxT("%i:%i"), start, end);
                        expandedString.Replace(wxT("$(CurrentSelectionRange)"), output);
                    }
                }
            }

            // exapand common macros
            wxDateTime now = wxDateTime::Now();
            expandedString.Replace(wxT("$(User)"), wxGetUserName());
            expandedString.Replace(wxT("$(Date)"), now.FormatDate());

            if(manager && applyEnv) {
                expandedString.Replace(wxT("$(gsgsPath)"), manager->GetInstallDirectory());

                // Apply the environment and expand the variables
                EnvSetter es(NULL, NULL, project, confToBuild);
                expandedString = manager->GetEnv()->ExpandVariables(expandedString, false);
            }
        }
        return expandedString;
    }
    //----------------------------------------------------------------------------
}