
#include "gsgsBuildConfig.h"
#include "xmlutils.h"
#include "project.h"
#include "gsgsEditorConfig.h"
#include "gsgsBuilderGnuMake.h"
#include "gsgsDebuggerManager.h"
#include "gsgsToolsetManager.h"
#include "globals.h"
#include <wx/sstream.h>
#include <wx/thread.h>

namespace gsgs
{
    const wxString BuildConfig::OVERWRITE_GLOBAL_SETTINGS = "overwrite";
    const wxString BuildConfig::APPEND_TO_GLOBAL_SETTINGS = "append";
    const wxString BuildConfig::PREPEND_GLOBAL_SETTINGS = "prepend";
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // BuildConfigCommon
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    BuildConfigCommon::BuildConfigCommon(wxXmlNode* node, wxString confType)
        : m_confType(confType)
    {
        if(node) {
            // read the compile options
            wxXmlNode* compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
            if(compile) {
                m_compileOptions = XmlUtils::ReadString(compile, wxT("Options"));
                m_assemblerOptions = XmlUtils::ReadString(compile, "Assembler");
                if(!compile->XmlGetPropVal(wxT("C_Options"), &m_cCompileOptions)) {
                    // the attribute "C_Options" does not exist,
                    // copy the values from the "Options" attribute
                    m_cCompileOptions = m_compileOptions;
                }

                wxXmlNode* child = compile->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("IncludePath")) {
                        m_includePath.Add(XmlUtils::ReadString(child, wxT("Value")));
                    } else if(child->GetName() == wxT("Preprocessor")) {
                        m_preprocessor.Add(XmlUtils::ReadString(child, wxT("Value")));
                    }
                    child = child->GetNext();
                }
            }

            // read the linker options
            wxXmlNode* linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
            if(linker) {
                m_linkOptions = XmlUtils::ReadString(linker, wxT("Options"));
                wxXmlNode* child = linker->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("Library")) {
                        m_libs.Add(XmlUtils::ReadString(child, wxT("Value")));
                    } else if(child->GetName() == wxT("LibraryPath")) {
                        m_libPath.Add(XmlUtils::ReadString(child, wxT("Value")));
                    }
                    child = child->GetNext();
                }
            }

            // read the resource compile options
            wxXmlNode* resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
            if(resCmp) {
                m_resCompileOptions = XmlUtils::ReadString(resCmp, wxT("Options"));
                wxXmlNode* child = resCmp->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("IncludePath")) {
                        m_resCmpIncludePath << XmlUtils::ReadString(child, wxT("Value")) << wxT(";");
                    }
                    child = child->GetNext();
                }
            }
        } else {
            m_includePath.Add(wxT("."));
            m_libPath.Add(wxT("."));
        }
    }
    //-------------------------------------------------------------------------------------
    BuildConfigCommon::~BuildConfigCommon() {}
    //-------------------------------------------------------------------------------------
    wxXmlNode* BuildConfigCommon::ToXml() const
    {
        wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, m_confType);

        // create the compile node
        wxXmlNode* compile = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
        compile->XmlAddProperty(wxT("Options"), m_compileOptions);
        compile->XmlAddProperty(wxT("C_Options"), m_cCompileOptions);
        compile->XmlAddProperty("Assembler", m_assemblerOptions);
        node->AddChild(compile);

        size_t i = 0;
        for(i = 0; i < m_includePath.GetCount(); i++) {
            wxXmlNode* option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
            option->XmlAddProperty(wxT("Value"), m_includePath.Item(i));
            compile->AddChild(option);
        }

        for(i = 0; i < m_preprocessor.GetCount(); i++) {
            wxXmlNode* prep = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Preprocessor"));
            prep->XmlAddProperty(wxT("Value"), m_preprocessor.Item(i));
            compile->AddChild(prep);
        }

        // add the link node
        wxXmlNode* link = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Linker"));
        link->XmlAddProperty(wxT("Options"), m_linkOptions);
        node->AddChild(link);

        for(i = 0; i < m_libPath.GetCount(); i++) {
            wxXmlNode* option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LibraryPath"));
            option->XmlAddProperty(wxT("Value"), m_libPath.Item(i));
            link->AddChild(option);
        }

        for(i = 0; i < m_libs.GetCount(); i++) {
            wxXmlNode* option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Library"));
            option->XmlAddProperty(wxT("Value"), m_libs.Item(i));
            link->AddChild(option);
        }

        // add the resource compiler node
        wxXmlNode* resCmp = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ResourceCompiler"));
        resCmp->XmlAddProperty(wxT("Options"), m_resCompileOptions);
        node->AddChild(resCmp);

        wxStringTokenizer tok(m_resCmpIncludePath, wxT(";"));
        while(tok.HasMoreTokens()) {
            wxXmlNode* option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
            option->XmlAddProperty(wxT("Value"), tok.NextToken());
            resCmp->AddChild(option);
        }
        return node;
    }
    //-------------------------------------------------------------------------------------
    void BuildConfigCommon::SetPreprocessor(const wxString& pre) { FillFromSmiColonString(m_preprocessor, pre); }
    //-------------------------------------------------------------------------------------
    void BuildConfigCommon::SetIncludePath(const wxString& path) { FillFromSmiColonString(m_includePath, path); }
    //-------------------------------------------------------------------------------------
    void BuildConfigCommon::SetLibraries(const wxString& libs) { FillFromSmiColonString(m_libs, libs); }
    //-------------------------------------------------------------------------------------
    void BuildConfigCommon::SetLibPath(const wxString& paths) { FillFromSmiColonString(m_libPath, paths); }
    //-------------------------------------------------------------------------------------
    static wxString ConvertToSmiColonString(const wxArrayString& array)
    {
        wxString result;
        for(size_t i = 0; i < array.GetCount(); i++) {
            wxString tmp = array.Item(i);
            tmp.Trim().Trim(false);
            if(!tmp.IsEmpty()) {
                result += array.Item(i);
                result += ";";
            }
        }
        return result.BeforeLast(';');
    }
    //-------------------------------------------------------------------------------------
    wxString BuildConfigCommon::GetLibPath() const { return ConvertToSmiColonString(m_libPath); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfigCommon::GetLibraries() const { return ConvertToSmiColonString(m_libs); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfigCommon::GetIncludePath() const { return ConvertToSmiColonString(m_includePath); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfigCommon::GetPreprocessor() const
    {
        wxString asString;
        for(size_t i = 0; i < m_preprocessor.GetCount(); i++) {
            wxString tmp = m_preprocessor.Item(i);
            tmp.Trim().Trim(false);
            if(tmp.IsEmpty()) continue;

            asString << tmp << wxT(";");
        }
        if(asString.IsEmpty() == false) asString.RemoveLast();

        return asString;
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // BuildConfig
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    BuildConfig::BuildConfig(wxXmlNode* node)
        : m_commonConfig(node)
        , m_useSeparateDebugArgs(false)
        , m_pchInCommandLine(false)
        , m_clangC11(false)
        , m_clangC14(false)
        , m_isGUIProgram(false)
        , m_isProjectEnabled(true)
        , m_pchPolicy(BuildConfig::kPCHPolicyReplace)
        , m_buildSystem("Default")
    {
        if(node) {
            m_name = XmlUtils::ReadString(node, wxT("Name"));
            m_compilerType = XmlUtils::ReadString(node, wxT("CompilerType"));
            m_debuggerType = XmlUtils::ReadString(node, wxT("DebuggerType"));
            m_projectType = XmlUtils::ReadString(node, wxT("Type"));
            m_buildCmpWithGlobalSettings =
                XmlUtils::ReadString(node, wxT("BuildCmpWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);
            m_buildLnkWithGlobalSettings =
                XmlUtils::ReadString(node, wxT("BuildLnkWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);
            m_buildResWithGlobalSettings =
                XmlUtils::ReadString(node, wxT("BuildResWithGlobalSettings"), APPEND_TO_GLOBAL_SETTINGS);

            wxXmlNode* buildSystem = XmlUtils::FindFirstByTagName(node, wxT("BuildSystem"));
            if(buildSystem) {
                m_buildSystem = XmlUtils::ReadString(buildSystem, "Name", m_buildSystem);
                m_buildSystemArguments = buildSystem->GetNodeContent();
            }

            wxXmlNode* completion = XmlUtils::FindFirstByTagName(node, wxT("Completion"));
            if(completion) {
                wxXmlNode* search_paths = XmlUtils::FindFirstByTagName(completion, wxT("SearchPaths"));
                if(search_paths) {
                    m_ccSearchPaths = search_paths->GetNodeContent();
                    m_ccSearchPaths.Trim().Trim(false);
                }

                wxXmlNode* clang_pp = XmlUtils::FindFirstByTagName(completion, wxT("ClangPP"));
                if(clang_pp) {
                    m_clangPPFlags = clang_pp->GetNodeContent();
                    m_clangPPFlags.Trim().Trim(false);
                }

                wxXmlNode* clang_cmp_flags = XmlUtils::FindFirstByTagName(completion, wxT("ClangCmpFlags"));
                if(clang_cmp_flags) {
                    m_clangCmpFlags = clang_cmp_flags->GetNodeContent();
                    m_clangCmpFlags.Trim().Trim(false);
                }

                wxXmlNode* clang_c_cmp_flags = XmlUtils::FindFirstByTagName(completion, wxT("ClangCmpFlagsC"));
                if(clang_c_cmp_flags) {
                    m_clangCmpFlagsC = clang_c_cmp_flags->GetNodeContent();
                    m_clangCmpFlagsC.Trim().Trim(false);
                }

                wxXmlNode* clang_pch = XmlUtils::FindFirstByTagName(completion, wxT("ClangPCH"));
                if(clang_pch) {
                    m_ccPCH = clang_pch->GetNodeContent();
                    m_ccPCH.Trim().Trim(false);
                }

                m_clangC11 = XmlUtils::ReadBool(completion, wxT("EnableCpp11"));
                m_clangC14 = XmlUtils::ReadBool(completion, wxT("EnableCpp14"));
            }

            wxXmlNode* compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
            if(compile) {
                m_compilerRequired = XmlUtils::ReadBool(compile, wxT("Required"), true);
                m_precompiledHeader = XmlUtils::ReadString(compile, wxT("PreCompiledHeader"));
                m_pchInCommandLine = XmlUtils::ReadBool(compile, wxT("PCHInCommandLine"), false);
                m_pchCompileFlags = XmlUtils::ReadString(compile, wxT("PCHFlags"));
                m_pchPolicy = (ePCHPolicy)XmlUtils::ReadLong(compile, "PCHFlagsPolicy", m_pchPolicy);
            }

            wxXmlNode* linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
            if(linker) {
                m_linkerRequired = XmlUtils::ReadBool(linker, wxT("Required"), true);
            }

            wxXmlNode* resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
            if(resCmp) {
                m_isResCmpNeeded = XmlUtils::ReadBool(resCmp, wxT("Required"), true);
            }

            // read the postbuild commands
            wxXmlNode* debugger = XmlUtils::FindFirstByTagName(node, wxT("Debugger"));
            m_isDbgRemoteTarget = false;

            if(debugger) {
                m_isDbgRemoteTarget = XmlUtils::ReadBool(debugger, wxT("IsRemote"));
                m_dbgHostName = XmlUtils::ReadString(debugger, wxT("RemoteHostName"));
                m_dbgHostPort = XmlUtils::ReadString(debugger, wxT("RemoteHostPort"));
                m_debuggerPath = XmlUtils::ReadString(debugger, wxT("DebuggerPath"));
                m_isDbgRemoteExtended = XmlUtils::ReadBool(debugger, wxT("IsExtended"));

                wxXmlNode* child = debugger->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("StartupCommands")) {
                        m_debuggerStartupCmds = child->GetNodeContent();

                    } else if(child->GetName() == wxT("PostConnectCommands")) {
                        m_debuggerPostRemoteConnectCmds = child->GetNodeContent();

                    } else if(child->GetName() == "DebuggerSearchPaths") {
                        wxArrayString searchPaths = ::wxStringTokenize(child->GetNodeContent(), "\r\n", wxTOKEN_STRTOK);
                        m_debuggerSearchPaths.swap(searchPaths);
                    }

                    child = child->GetNext();
                }
            }

            // read the prebuild commands
            wxXmlNode* preBuild = XmlUtils::FindFirstByTagName(node, wxT("PreBuild"));
            if(preBuild) {
                wxXmlNode* child = preBuild->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("Command")) {
                        bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));

                        BuildCommand cmd(child->GetNodeContent(), enabled);
                        m_preBuildCommands.push_back(cmd);
                    }
                    child = child->GetNext();
                }
            }
            // read the postbuild commands
            wxXmlNode* postBuild = XmlUtils::FindFirstByTagName(node, wxT("PostBuild"));
            if(postBuild) {
                wxXmlNode* child = postBuild->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("Command")) {
                        bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));
                        BuildCommand cmd(child->GetNodeContent(), enabled);
                        m_postBuildCommands.push_back(cmd);
                    }
                    child = child->GetNext();
                }
            }

            SetEnvVarSet(USE_WORKSPACE_ENV_VAR_SET);
            SetDbgEnvSet(USE_GLOBAL_SETTINGS);

            // read the environment page
            wxXmlNode* envNode = XmlUtils::FindFirstByTagName(node, wxT("Environment"));
            if(envNode) {
                SetEnvVarSet(XmlUtils::ReadString(envNode, wxT("EnvVarSetName")));
                SetDbgEnvSet(XmlUtils::ReadString(envNode, wxT("DbgSetName")));
                m_envvars = envNode->GetNodeContent();
            }

            wxXmlNode* customBuild = XmlUtils::FindFirstByTagName(node, wxT("CustomBuild"));
            if(customBuild) {
                m_enableCustomBuild = XmlUtils::ReadBool(customBuild, wxT("Enabled"), false);
                wxXmlNode* child = customBuild->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("BuildCommand")) {
                        m_customBuildCmd = child->GetNodeContent();
                    } else if(child->GetName() == wxT("CleanCommand")) {
                        m_customCleanCmd = child->GetNodeContent();
                    } else if(child->GetName() == wxT("RebuildCommand")) {
                        m_customRebuildCmd = child->GetNodeContent();
                    } else if(child->GetName() == wxT("SingleFileCommand")) {
                        m_singleFileBuildCommand = child->GetNodeContent();
                    } else if(child->GetName() == wxT("PreprocessFileCommand")) {
                        m_preprocessFileCommand = child->GetNodeContent();
                    } else if(child->GetName() == wxT("WorkingDirectory")) {
                        m_customBuildWorkingDir = child->GetNodeContent();
                    } else if(child->GetName() == wxT("ThirdPartyToolName")) {
                        m_toolName = child->GetNodeContent();
                    } else if(child->GetName() == wxT("MakefileGenerationCommand")) {
                        m_makeGenerationCommand = child->GetNodeContent();
                    } else if(child->GetName() == wxT("Target")) {
                        wxString target_name = child->XmlGetPropVal(wxT("Name"), wxT(""));
                        wxString target_cmd = child->GetNodeContent();
                        if(target_name.IsEmpty() == false) {
                            m_customTargets[target_name] = target_cmd;
                        }
                    }
                    child = child->GetNext();
                }
            } else {
                m_enableCustomBuild = false;
            }

            wxXmlNode* customPreBuild = XmlUtils::FindFirstByTagName(node, wxT("AdditionalRules"));
            if(customPreBuild) {
                wxXmlNode* child = customPreBuild->GetChildren();
                while(child) {
                    if(child->GetName() == wxT("CustomPreBuild")) {
                        m_customPreBuildRule = child->GetNodeContent();
                        m_customPreBuildRule.Trim().Trim(false);

                    } else if(child->GetName() == wxT("CustomPostBuild")) {
                        m_customPostBuildRule = child->GetNodeContent();
                        m_customPostBuildRule.Trim().Trim(false);
                    }
                    child = child->GetNext();
                }
            }

            wxXmlNode* general = XmlUtils::FindFirstByTagName(node, wxT("General"));
            if(general) {
                m_outputFile = XmlUtils::ReadString(general, wxT("OutputFile"));
                m_intermediateDirectory = XmlUtils::ReadString(general, wxT("IntermediateDirectory"), wxT("."));
                m_command = XmlUtils::ReadString(general, wxT("Command"));
                m_commandArguments = XmlUtils::ReadString(general, wxT("CommandArguments"));
                m_workingDirectory = XmlUtils::ReadString(general, wxT("WorkingDirectory"), wxT("."));
                m_pauseWhenExecEnds = XmlUtils::ReadBool(general, wxT("PauseExecWhenProcTerminates"), true);
                m_useSeparateDebugArgs = XmlUtils::ReadBool(general, wxT("UseSeparateDebugArgs"), false);
                m_debugArgs = XmlUtils::ReadString(general, wxT("DebugArguments"));
                m_isGUIProgram = XmlUtils::ReadBool(general, "IsGUIProgram", false);
                m_isProjectEnabled = XmlUtils::ReadBool(general, "IsEnabled", true);
            }

        } else {

            // create default project settings
            m_commonConfig.SetCompileOptions(wxT("-g -Wall"));
            m_commonConfig.SetLinkOptions(wxT("-O0"));
            m_commonConfig.SetLibPath(wxT(".;Debug"));

            m_name = wxT("Debug");
            m_compilerRequired = true;
            m_linkerRequired = true;
            m_intermediateDirectory = wxT("./Debug");
            m_workingDirectory = wxT("./Debug");
            m_projectType = PROJECT_TYPE_EXECUTABLE;
            m_enableCustomBuild = false;
            m_customBuildCmd = wxEmptyString;
            m_customCleanCmd = wxEmptyString;
            m_isResCmpNeeded = false;
            m_customPostBuildRule = wxEmptyString;
            m_customPreBuildRule = wxEmptyString;
            m_makeGenerationCommand = wxEmptyString;
            m_toolName = wxEmptyString;
            m_singleFileBuildCommand = wxEmptyString;
            m_preprocessFileCommand = wxEmptyString;
            m_debuggerStartupCmds = wxEmptyString;
            m_debuggerPostRemoteConnectCmds = wxEmptyString;
            m_isDbgRemoteExtended = false;
            m_isDbgRemoteTarget = false;
            m_useSeparateDebugArgs = false;
            m_debugArgs = wxEmptyString;

            SetEnvVarSet(wxT("<Use Workspace Settings>"));
            SetDbgEnvSet(wxT("<Use Global Settings>"));

            BuildSettingsConfigCookie cookie;
            CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
            if(cmp) {
                m_compilerType = cmp->GetName();
            }
            wxArrayString dbgs = DebuggerMgr::Get().GetAvailableDebuggers();
            if(dbgs.GetCount() > 0) {
                m_debuggerType = dbgs.Item(0);
            }
            m_buildCmpWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
            m_buildLnkWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
            m_buildResWithGlobalSettings = APPEND_TO_GLOBAL_SETTINGS;
        }
    }
    //-------------------------------------------------------------------------------------
    BuildConfig::~BuildConfig() {}
    //-------------------------------------------------------------------------------------
    BuildConfig* BuildConfig::Clone() const
    {
        wxXmlNode* node = ToXml();
        BuildConfig* cloned = new BuildConfig(node);
        delete node;
        return cloned;
    }
    //-------------------------------------------------------------------------------------
    wxXmlNode* BuildConfig::ToXml() const
    {
        // Create the common nodes
        wxXmlNode* node = m_commonConfig.ToXml();

        node->XmlAddProperty(wxT("Name"), m_name);
        node->XmlAddProperty(wxT("CompilerType"), m_compilerType);
        node->XmlAddProperty(wxT("DebuggerType"), m_debuggerType);
        node->XmlAddProperty(wxT("Type"), m_projectType);
        node->XmlAddProperty(wxT("BuildCmpWithGlobalSettings"), m_buildCmpWithGlobalSettings);
        node->XmlAddProperty(wxT("BuildLnkWithGlobalSettings"), m_buildLnkWithGlobalSettings);
        node->XmlAddProperty(wxT("BuildResWithGlobalSettings"), m_buildResWithGlobalSettings);

        wxXmlNode* compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
        if(compile) {
            compile->XmlAddProperty(wxT("Required"), BoolToString(m_compilerRequired));
            compile->XmlAddProperty(wxT("PreCompiledHeader"), m_precompiledHeader);
            compile->XmlAddProperty(wxT("PCHInCommandLine"), BoolToString(m_pchInCommandLine));
            compile->XmlAddProperty(wxT("PCHFlags"), m_pchCompileFlags);
            compile->XmlAddProperty("PCHFlagsPolicy", wxString() << (int)m_pchPolicy);
        }

        wxXmlNode* link = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
        if(link) {
            link->XmlAddProperty(wxT("Required"), BoolToString(m_linkerRequired));
        }

        wxXmlNode* resCmp = XmlUtils::FindFirstByTagName(node, wxT("ResourceCompiler"));
        if(resCmp) {
            resCmp->XmlAddProperty(wxT("Required"), BoolToString(m_isResCmpNeeded));
        }

        wxXmlNode* general = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("General"));
        general->XmlAddProperty(wxT("OutputFile"), m_outputFile);
        general->XmlAddProperty(wxT("IntermediateDirectory"), m_intermediateDirectory);
        general->XmlAddProperty(wxT("Command"), m_command);
        general->XmlAddProperty(wxT("CommandArguments"), m_commandArguments);
        general->XmlAddProperty(wxT("UseSeparateDebugArgs"), BoolToString(m_useSeparateDebugArgs));
        general->XmlAddProperty(wxT("DebugArguments"), m_debugArgs);
        general->XmlAddProperty(wxT("WorkingDirectory"), m_workingDirectory);
        general->XmlAddProperty(wxT("PauseExecWhenProcTerminates"), BoolToString(m_pauseWhenExecEnds));
        general->XmlAddProperty("IsGUIProgram", BoolToString(m_isGUIProgram));
        general->XmlAddProperty("IsEnabled", BoolToString(m_isProjectEnabled));
        node->AddChild(general);

        wxXmlNode* buildSystem = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildSystem"));
        buildSystem->XmlAddProperty("Name", m_buildSystem);
        XmlUtils::SetNodeContent(buildSystem, m_buildSystemArguments);
        node->AddChild(buildSystem);

        wxXmlNode* debugger = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Debugger"));
        debugger->XmlAddProperty(wxT("IsRemote"), BoolToString(m_isDbgRemoteTarget));
        debugger->XmlAddProperty(wxT("RemoteHostName"), m_dbgHostName);
        debugger->XmlAddProperty(wxT("RemoteHostPort"), m_dbgHostPort);
        debugger->XmlAddProperty(wxT("DebuggerPath"), m_debuggerPath);
        debugger->XmlAddProperty(wxT("IsExtended"), BoolToString(m_isDbgRemoteExtended));

        wxXmlNode* envNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Environment"));
        envNode->XmlAddProperty(wxT("EnvVarSetName"), GetEnvVarSet());
        envNode->XmlAddProperty(wxT("DbgSetName"), GetDbgEnvSet());

        // Add CDATA section with project environment variables
        wxXmlNode* envContent = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, m_envvars);
        envNode->AddChild(envContent);
        node->AddChild(envNode);

        wxXmlNode* dbgStartupCommands = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("StartupCommands"));
        XmlUtils::SetNodeContent(dbgStartupCommands, m_debuggerStartupCmds);

        wxXmlNode* dbgPostConnectCommands = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("PostConnectCommands"));
        XmlUtils::SetNodeContent(dbgPostConnectCommands, m_debuggerPostRemoteConnectCmds);

        wxXmlNode* dbgSearchPaths = new wxXmlNode(debugger, wxXML_ELEMENT_NODE, wxT("DebuggerSearchPaths"));
        XmlUtils::SetNodeContent(dbgSearchPaths, ::wxImplode(m_debuggerSearchPaths, "\n"));

        node->AddChild(debugger);

        // add prebuild commands
        wxXmlNode* preBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PreBuild"));
        node->AddChild(preBuild);

        BuildCommandList::const_iterator iter = m_preBuildCommands.begin();
        for(; iter != m_preBuildCommands.end(); iter++) {
            wxXmlNode* command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
            command->XmlAddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
            XmlUtils::SetNodeContent(command, iter->GetCommand());
            preBuild->AddChild(command);
        }

        // add postbuild commands
        wxXmlNode* postBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PostBuild"));
        node->AddChild(postBuild);
        iter = m_postBuildCommands.begin();
        for(; iter != m_postBuildCommands.end(); iter++) {
            wxXmlNode* command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
            command->XmlAddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
            XmlUtils::SetNodeContent(command, iter->GetCommand());
            postBuild->AddChild(command);
        }

        // add postbuild commands
        wxXmlNode* customBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CustomBuild"));
        node->AddChild(customBuild);
        customBuild->XmlAddProperty(wxT("Enabled"), BoolToString(m_enableCustomBuild));

        // add the working directory of the custom build
        wxXmlNode* customBuildWd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("WorkingDirectory"));
        XmlUtils::SetNodeContent(customBuildWd, m_customBuildWorkingDir);

        // add the makefile generation command
        wxXmlNode* toolName = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("ThirdPartyToolName"));
        XmlUtils::SetNodeContent(toolName, m_toolName);

        // add the makefile generation command
        wxXmlNode* makeGenCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("MakefileGenerationCommand"));
        XmlUtils::SetNodeContent(makeGenCmd, m_makeGenerationCommand);

        // add the makefile generation command
        wxXmlNode* singleFileCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("SingleFileCommand"));
        XmlUtils::SetNodeContent(singleFileCmd, m_singleFileBuildCommand);

        // add the makefile generation command
        wxXmlNode* preprocessFileCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("PreprocessFileCommand"));
        XmlUtils::SetNodeContent(preprocessFileCmd, m_preprocessFileCommand);

        // add build and clean commands
        wxXmlNode* bldCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("BuildCommand"));
        XmlUtils::SetNodeContent(bldCmd, m_customBuildCmd);

        wxXmlNode* clnCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("CleanCommand"));
        XmlUtils::SetNodeContent(clnCmd, m_customCleanCmd);

        wxXmlNode* rebldCmd = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("RebuildCommand"));
        XmlUtils::SetNodeContent(rebldCmd, m_customRebuildCmd);

        // add all 'Targets'
        std::map<wxString, wxString>::const_iterator ir = m_customTargets.begin();
        for(; ir != m_customTargets.end(); ir++) {
            wxString target_name = ir->first;
            wxString target_cmd = ir->second;

            wxXmlNode* customTarget = new wxXmlNode(customBuild, wxXML_ELEMENT_NODE, wxT("Target"));
            customTarget->XmlAddProperty(wxT("Name"), target_name);
            XmlUtils::SetNodeContent(customTarget, target_cmd);
        }

        // add the additional rules
        wxXmlNode* additionalCmds = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("AdditionalRules"));
        node->AddChild(additionalCmds);

        wxXmlNode* preCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPreBuild"));
        XmlUtils::SetNodeContent(preCmd, m_customPreBuildRule);
        wxXmlNode* postCmd = new wxXmlNode(additionalCmds, wxXML_ELEMENT_NODE, wxT("CustomPostBuild"));
        XmlUtils::SetNodeContent(postCmd, m_customPostBuildRule);

        // Set the completion flags
        wxXmlNode* completion = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Completion"));
        node->AddChild(completion);
        completion->XmlAddProperty(wxT("EnableCpp11"), BoolToString(m_clangC11));
        completion->XmlAddProperty(wxT("EnableCpp14"), BoolToString(m_clangC14));

        wxXmlNode* search_paths = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("SearchPaths"));
        XmlUtils::SetNodeContent(search_paths, m_ccSearchPaths);

        wxXmlNode* clang_pp = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangPP"));
        XmlUtils::SetNodeContent(clang_pp, m_clangPPFlags);

        wxXmlNode* clang_cmp_flags = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangCmpFlags"));
        XmlUtils::SetNodeContent(clang_cmp_flags, m_clangCmpFlags);

        wxXmlNode* clang_c_cmp_flags = new wxXmlNode(completion, wxXML_ELEMENT_NODE, wxT("ClangCmpFlagsC"));
        XmlUtils::SetNodeContent(clang_c_cmp_flags, m_clangCmpFlagsC);

        return node;
    }
    //-------------------------------------------------------------------------------------
    void BuildConfig::SetPreprocessor(const wxString& pre) { m_commonConfig.SetPreprocessor(pre); }
    //-------------------------------------------------------------------------------------
    void BuildConfig::SetIncludePath(const wxString& path) { m_commonConfig.SetIncludePath(path); }
    //-------------------------------------------------------------------------------------
    void BuildConfig::SetLibraries(const wxString& libs) { m_commonConfig.SetLibraries(libs); }
    //-------------------------------------------------------------------------------------
    void BuildConfig::SetLibPath(const wxString& paths) { m_commonConfig.SetLibPath(paths); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetLibPath() const { return m_commonConfig.GetLibPath(); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetLibraries() const { return m_commonConfig.GetLibraries(); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetIncludePath() const { return m_commonConfig.GetIncludePath(); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetPreprocessor() const { return m_commonConfig.GetPreprocessor(); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetOutputFileName() const { return NormalizePath(m_outputFile); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetIntermediateDirectory() const { return NormalizePath(m_intermediateDirectory); }
    //-------------------------------------------------------------------------------------
    wxString BuildConfig::GetWorkingDirectory() const { return NormalizePath(m_workingDirectory); }
    //-------------------------------------------------------------------------------------
    CompilerPtr BuildConfig::GetCompiler() const { return gsgs_Settings().GetCompiler(GetCompilerType()); }
    //-------------------------------------------------------------------------------------
    BuilderPtr BuildConfig::GetBuilder() { return gsgs_Build().GetBuilder(GetBuildSystem()); }


    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // BuilderConfig
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    BuilderConfig::BuilderConfig(wxXmlNode *node)
        : m_isActive(false)
    {
        if(node) {
            m_name        = XmlUtils::ReadString(node, wxT("Name"));
            m_toolPath    = XmlUtils::ReadString(node, wxT("ToolPath"));
            m_toolOptions = XmlUtils::ReadString(node, wxT("Options"));
            m_toolJobs    = wxString() << wxThread::GetCPUCount();
            m_isActive    = XmlUtils::ReadBool  (node, wxT("Active"), m_isActive);
        }
    }
    //-------------------------------------------------------------------------------------
    BuilderConfig::~BuilderConfig()
    {
    }
    //-------------------------------------------------------------------------------------
    wxXmlNode *BuilderConfig::ToXml() const
    {
        wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("BuildSystem"));
        node->XmlAddProperty(wxT("Name"),     m_name);
        node->XmlAddProperty(wxT("ToolPath"), m_toolPath);
        node->XmlAddProperty(wxT("Options"),  m_toolOptions);
        node->XmlAddProperty(wxT("Jobs"),     m_toolJobs);
        node->XmlAddProperty(wxT("Active"),   m_isActive ? wxString(wxT("yes")) : wxString(wxT("no")));
        return node;
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // BuildSettingsConfig
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    BuildSettingsConfig::BuildSettingsConfig()
    {
        m_doc = new wxXmlDocument();
        m_compilers.clear();
    }
    //-------------------------------------------------------------------------------------
    BuildSettingsConfig::~BuildSettingsConfig()
    {
        wxDELETE(m_doc);
        m_compilers.clear();
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::SetTargets(const wxString& projectName, const BuildConfig::StringMap_t& targets)
    {
        Clear();
        
        m_projectName = projectName;
        BuildConfig::StringMap_t::const_iterator iter = targets.begin();
        
        int id = ID_MENU_CUSTOM_TARGET_FIRST;
        for(; iter != targets.end(); ++iter) {
            m_targets.insert( std::make_pair( id, std::make_pair(iter->first, iter->second) ) );
            ++id;
        }
    }
    //-------------------------------------------------------------------------------------
    BuildSettingsConfig::Pair_t BuildSettingsConfig::GetTarget(int menuId) const
    {
        if ( m_targets.count(menuId) ) {
            return m_targets.find(menuId)->second;
        }
        return Pair_t();
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::Clear()
    {
        m_projectName.Clear();
        m_targets.clear();
    }
    //-------------------------------------------------------------------------------------
    bool BuildSettingsConfig::Load(const wxString& version, const wxString& xmlFilePath)
    {
        bool loaded = false;
        m_version = version;
        if(xmlFilePath.IsEmpty()) 
        {
            wxString initialSettings = gsgs_Path().Locate(wxT("config/build_settings.xml"));
            loaded = m_doc->Load(initialSettings);
            CHECK_PTR_RET_FALSE(m_doc->GetRoot());

            wxString xmlVersion = m_doc->GetRoot()->XmlGetPropVal(wxT("Version"), wxEmptyString);
            if(xmlVersion != version) 
            {
                loaded = m_doc->Load(gsgs_Path().GetDefault(wxT("config/build_settings.xml")));
            }
            m_fileName = gsgs_Path().GetUserDataDir() + wxT("/") + wxT("config/build_settings.xml");

            if(loaded) 
            { 
                DoUpdateCompilers(); 
            }
        } 
        else 
        {
            wxFileName xmlPath(xmlFilePath);
            loaded = m_doc->Load(xmlPath.GetFullPath());
            if(loaded) 
            {
                DoUpdateCompilers();
                m_fileName = xmlPath;
            }
        }
        if(loaded) 
        { 
            SaveXmlFile(); 
        }
        return loaded;
    }
    //-------------------------------------------------------------------------------------
    wxXmlNode* BuildSettingsConfig::GetCompilerNode(const wxString& name) const
    {
        wxXmlNode* cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
        if(cmpsNode) {
            if(name.IsEmpty()) {
                // return the first compiler
                return XmlUtils::FindFirstByTagName(cmpsNode, wxT("Compiler"));
            } else {
                return XmlUtils::FindNodeByName(cmpsNode, wxT("Compiler"), name);
            }
        }
        return NULL;
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::SetCompiler(CompilerPtr cmp)
    {
        wxXmlNode* node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
        if(node) {
            wxXmlNode* oldCmp = NULL;
            wxXmlNode* child = node->GetChildren();
            while(child) {
                if(child->GetName() == wxT("Compiler") && XmlUtils::ReadString(child, wxT("Name")) == cmp->GetName()) {
                    oldCmp = child;
                    break;
                }
                child = child->GetNext();
            }
            if(oldCmp) {
                node->RemoveChild(oldCmp);
                delete oldCmp;
            }
            node->AddChild(cmp->ToXml());

        } else {
            wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compilers"));
            m_doc->GetRoot()->AddChild(node);
            node->AddChild(cmp->ToXml());
        }

        SaveXmlFile();
        DoUpdateCompilers();
    }
    //-------------------------------------------------------------------------------------
    CompilerPtr BuildSettingsConfig::GetCompiler(const wxString& name) const
    {
        if(!m_compilers.count(name)) {

            // no such compiler...
            return new Compiler(NULL);

        } else {

            return m_compilers.find(name)->second;
        }
    }
    //-------------------------------------------------------------------------------------
    CompilerPtr BuildSettingsConfig::GetFirstCompiler(BuildSettingsConfigCookie& cookie)
    {
        wxXmlNode* cmps = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
        if(cmps) {
            cookie.parent = cmps;
            cookie.child = NULL;
            return GetNextCompiler(cookie);
        }
        return NULL;
    }
    //-------------------------------------------------------------------------------------
    CompilerPtr BuildSettingsConfig::GetNextCompiler(BuildSettingsConfigCookie& cookie)
    {
        if(cookie.parent == NULL) { return NULL; }

        if(cookie.child == NULL) { cookie.child = cookie.parent->GetChildren(); }

        while(cookie.child) {
            if(cookie.child->GetName() == wxT("Compiler")) {
                wxXmlNode* n = cookie.child;
                // advance the child to the next child and bail out
                cookie.child = cookie.child->GetNext();

                // incase we dont have more childs to iterate
                // reset the parent as well so the next call to GetNexeLexer() will fail
                if(cookie.child == NULL) { cookie.parent = NULL; }
                return new Compiler(n);
            }
            cookie.child = cookie.child->GetNext();
        }
        return NULL;
    }
    //-------------------------------------------------------------------------------------
    bool BuildSettingsConfig::IsCompilerExist(const wxString& name) const { return m_compilers.count(name); }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::DeleteCompiler(const wxString& name)
    {
        wxXmlNode* node = GetCompilerNode(name);
        if(node) {
            node->GetParent()->RemoveChild(node);
            delete node;
            SaveXmlFile();
            DoUpdateCompilers();
        }
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::SetBuildSystem(BuilderConfigPtr bs)
    {
        // find the old setting
        wxXmlNode* node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"), bs->GetName());
        if(node) {
            node->GetParent()->RemoveChild(node);
            delete node;
        }
        m_doc->GetRoot()->AddChild(bs->ToXml());
        SaveXmlFile();
        DoUpdateCompilers();
    }
    //-------------------------------------------------------------------------------------
    BuilderConfigPtr BuildSettingsConfig::GetBuilderConfig(const wxString& name)
    {
        wxXmlNode* node = XmlUtils::FindNodeByName(m_doc->GetRoot(), wxT("BuildSystem"),
                                                   name.IsEmpty() ? GetSelectedBuildSystem() : name);
        if(node) { return new BuilderConfig(node); }
        return NULL;
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::SaveBuilderConfig(BuilderPtr builder)
    {
        // update configuration file
        BuilderConfigPtr bsptr(new BuilderConfig(NULL));
        bsptr->SetName(builder->GetName());
        bsptr->SetIsActive(builder->IsActive());
        SetBuildSystem(bsptr);
    }
    //-------------------------------------------------------------------------------------
    wxString BuildSettingsConfig::GetSelectedBuildSystem()
    {
        wxString active("Default");

        wxXmlNode* node = m_doc->GetRoot()->GetChildren();
        while(node) {
            if(node->GetName() == wxT("BuildSystem")) {
                if(node->XmlGetPropVal(wxT("Active"), wxT("")) == wxT("yes")) {
                    active = node->XmlGetPropVal(wxT("Name"), wxT(""));
                    break;
                }
            }
            node = node->GetNext();
        }

        return active;
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::RestoreDefaults()
    {
        // Delete the local copy of the build settings
        gsgs_Path().DeleteLocal(wxT("config/build_settings.xml"));

        // free the XML dodcument loaded into the memory and allocate new one
        wxDELETE(m_doc);
        m_doc = new wxXmlDocument();

        // call Load again, this time the default settings will be loaded
        // since we just deleted the local settings
        Load(m_version);

        gsgs::Event event(wxEVT_COMPILER_LIST_UPDATED);
        gsgs_Event().AddPendingEvent(event);
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::DeleteAllCompilers(bool notify)
    {
        // Delete all compilers
        wxXmlNode* node = GetCompilerNode("");
        while(node) {
            node->GetParent()->RemoveChild(node);
            wxDELETE(node);
            node = GetCompilerNode("");
        }
        SaveXmlFile();
        m_compilers.clear();

        if(notify) {
            gsgs::Event event(wxEVT_COMPILER_LIST_UPDATED);
            gsgs_Event().AddPendingEvent(event);
        }
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::SetCompilers(const std::vector<CompilerPtr>& compilers)
    {
        DeleteAllCompilers(false);

        wxXmlNode* cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
        if(cmpsNode) {
            for(size_t i = 0; i < compilers.size(); ++i) {
                cmpsNode->AddChild(compilers.at(i)->ToXml());
            }
        }
        SaveXmlFile();
        DoUpdateCompilers();

        gsgs::Event event(wxEVT_COMPILER_LIST_UPDATED);
        gsgs_Event().AddPendingEvent(event);
    }
    //-------------------------------------------------------------------------------------
    wxArrayString BuildSettingsConfig::GetAllCompilersNames() const
    {
        wxArrayString allCompilers;
        wxXmlNode* compilersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), "Compilers");
        if(compilersNode) {
            wxXmlNode* child = compilersNode->GetChildren();
            while(child) {
                if(child->GetName() == "Compiler") { allCompilers.Add(XmlUtils::ReadString(child, "Name")); }
                child = child->GetNext();
            }
        }
        return allCompilers;
    }
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::DoUpdateCompilers()
    {
        m_compilers.clear();
        wxArrayString compilers = GetAllCompilersNames();
        for(size_t i = 0; i < compilers.GetCount(); ++i) {
            CompilerPtr pCompiler(new Compiler(GetCompilerNode(compilers.Item(i))));
            m_compilers.insert(std::make_pair(compilers.Item(i), pCompiler));
        }
    }
    //-------------------------------------------------------------------------------------
    bool BuildSettingsConfig::SaveXmlFile()
    {
        // Store this information for later retrieval
        wxArrayString compilers = GetAllCompilersNames();
        JSON root(cJSON_Array);
        JSONObject e = root.getObject();
        for(size_t i = 0; i < compilers.size(); ++i) {
            CompilerPtr cmp = GetCompiler(compilers[i]);
            if (cmp)
            {
                JSONObject o = JSONObject::createObject();
                o.addProperty("name", cmp->GetName()).addProperty("paths", cmp->GetDefaultIncludePaths());
                e.arrayAppend(o);
            }
        }
        wxFileName compilersFile(gsgs_Path().GetUserDataDir(), "compilers_paths.json");
        root.save(compilersFile);
        return XmlUtils::SaveXmlToFile(m_doc, m_fileName.GetFullPath());
    }
    //-------------------------------------------------------------------------------------
    static BuildSettingsConfig * gs_buildSettingsInstance = NULL;
    //-------------------------------------------------------------------------------------
    void BuildSettingsConfig::Release()
    {
        if (gs_buildSettingsInstance) {
            delete gs_buildSettingsInstance;
            gs_buildSettingsInstance = NULL;
        }
    }
    //-------------------------------------------------------------------------------------
    BuildSettingsConfig& BuildSettingsConfig::singleton()
    {
        if (gs_buildSettingsInstance == NULL)
            gs_buildSettingsInstance = new BuildSettingsConfig;
        return *gs_buildSettingsInstance;
    }
    //-------------------------------------------------------------------------------------
    CompilerPtr BuildSettingsConfig::GetDefaultCompiler(const wxString& compilerFamilty) const
    {
    #ifdef __WXMSW__
        wxString DEFAULT_COMPILER = COMPILER_FAMILY_MINGW;
    #else
        wxString DEFAULT_COMPILER = COMPILER_FAMILY_GCC;
    #endif

        CompilerPtr defaultComp;
        wxString family = compilerFamilty.IsEmpty() ? DEFAULT_COMPILER : compilerFamilty;

        std::unordered_map<wxString, CompilerPtr>::const_iterator iter = m_compilers.begin();
        for(; iter != m_compilers.end(); ++iter) {
            if(iter->second->GetCompilerFamily() == family) {
                if(!defaultComp) {
                    // keep the first one, just incase
                    defaultComp = iter->second;
                }
                if(iter->second->IsDefault()) { return iter->second; }
            }
        }
        return defaultComp;
    }
    //-------------------------------------------------------------------------------------
    CompilerPtrVec_t BuildSettingsConfig::GetAllCompilers(const wxString& family) const
    {
        CompilerPtrVec_t all;
        std::for_each(m_compilers.begin(), m_compilers.end(), [&](const std::pair<wxString, CompilerPtr>& p) {
            if(!family.IsEmpty() && p.second->GetCompilerFamily() == family) {
                all.push_back(p.second);
            } else if(family.IsEmpty()) {
                all.push_back(p.second);
            }
        });
        return all;
    }
    //-------------------------------------------------------------------------------------
    std::unordered_map<wxString, wxArrayString> BuildSettingsConfig::GetCompilersGlobalPaths() const
    {
        std::unordered_map<wxString, wxArrayString> M;
        wxArrayString compilers = GetAllCompilersNames();
        for(const wxString & name : compilers) 
        {
            CompilerPtr cmp = GetCompiler(name);
            if(!cmp) { continue; }
            M.insert({ name, cmp->GetDefaultIncludePaths() });
        }
        return M;
    }
    //-------------------------------------------------------------------------------------
}