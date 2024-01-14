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
#ifndef BUILD_CONFIGURATION_H
#define BUILD_CONFIGURATION_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"
#include <wx/xml/xml.h>

namespace gsgs
{
    class _gsgsAPI BuildConfigCommon : public ObjectData
    {
    public:
        BuildConfigCommon(wxXmlNode* node, wxString confType = wxT("Configuration"));
        virtual ~BuildConfigCommon();
        wxXmlNode *ToXml() const;
        BuildConfigCommon *Clone() const;

        wxString GetPreprocessor() const;

        inline void GetPreprocessor(wxArrayString &arr)                     { arr = m_preprocessor; }

        void SetPreprocessor(const wxString &prepr);

        inline const wxString &GetCompileOptions() const                    { return m_compileOptions; }

        inline void SetCompileOptions(const wxString &options)              { m_compileOptions = options; }

        inline void SetAssemblerOptions(const wxString& assemblerOptions)   { m_assemblerOptions = assemblerOptions; }
        inline const wxString& GetAssemblerOptions() const                  { return m_assemblerOptions; }
        
        inline void SetCCompileOptions(const wxString& cCompileOptions)     { m_cCompileOptions = cCompileOptions; }
        inline const wxString& GetCCompileOptions() const                   { return m_cCompileOptions; }
        inline void SetLinkOptions(const wxString &options)                 { m_linkOptions = options; }
        inline const wxString &GetLinkOptions() const                       { return m_linkOptions; }

        wxString GetIncludePath() const;
        void SetIncludePath(const wxString &path);
        inline void SetIncludePath(const wxArrayString &paths)              { m_includePath = paths; }

        wxString GetLibraries() const;
        void SetLibraries(const wxString &libs);
        inline void SetLibraries(const wxArrayString &libs)                 { m_libs = libs; }

        wxString GetLibPath() const;
        void SetLibPath(const wxString &path);
        inline void SetLibPath(const wxArrayString &libPaths)               { m_libPath = libPaths; }

        inline void SetResCmpIncludePath(const wxString &path)              { m_resCmpIncludePath = path; }
        inline const wxString& GetResCmpIncludePath() const                 { return m_resCmpIncludePath; }

        inline void SetResCmpOptions(const wxString &options)               { m_resCompileOptions = options; }
        inline const wxString &GetResCompileOptions() const                 { return m_resCompileOptions; }
    private:
        wxArrayString  m_includePath;
        wxArrayString  m_libs;
        wxArrayString  m_libPath;
        wxArrayString  m_preprocessor;
        wxString       m_compileOptions;
        wxString       m_linkOptions;
        wxString       m_resCompileOptions;
        wxString       m_resCmpIncludePath;
        wxString       m_cCompileOptions;
        wxString       m_assemblerOptions;
        wxString       m_confType; // xml node name
    };

    class _gsgsAPI BuildCommand
    {
    public:
        BuildCommand()
            : m_command(wxEmptyString)
            , m_enabled(false)
        {
        }

        BuildCommand(const wxString& command, bool enabled)
            : m_command(command)
            , m_enabled(enabled)
        {
        }

        ~BuildCommand() {}

        inline bool GetEnabled() const                                      { return m_enabled; }
        inline void SetEnabled(bool enabled)                                { m_enabled = enabled; }
        inline void SetCommand(const wxString& command)                     { m_command = command; }
        inline const wxString& GetCommand() const                           { return m_command; }
    private:
        wxString m_command;
        bool m_enabled;
    };

    typedef std::list<BuildCommand> BuildCommandList;

    class _gsgsAPI BuildConfig : public ObjectData
    {
    public:
        static const wxString OVERWRITE_GLOBAL_SETTINGS;
        static const wxString APPEND_TO_GLOBAL_SETTINGS;
        static const wxString PREPEND_GLOBAL_SETTINGS;
        typedef std::map<wxString, wxString> StringMap_t;

        enum ePCHPolicy {
            kPCHPolicyReplace = 0,
            kPCHPolicyAppend = 1,
        };
    public:
        BuildConfig(wxXmlNode* node);
        virtual ~BuildConfig();

        wxXmlNode* ToXml() const;

        BuildConfig* Clone() const;

        //--------------------------------
        // Setters / Getters
        //--------------------------------
        inline void SetPCHFlagsPolicy(BuildConfig::ePCHPolicy policy)   { this->m_pchPolicy = policy; }
        inline BuildConfig::ePCHPolicy GetPCHFlagsPolicy() const        { return this->m_pchPolicy; }
        inline void SetIsProjectEnabled(bool isProjectEnabled)          { this->m_isProjectEnabled = isProjectEnabled; }
        inline bool IsProjectEnabled() const                            { return m_isProjectEnabled; }
        inline void SetIsGUIProgram(bool isGUIProgram)                  { this->m_isGUIProgram = isGUIProgram; }
        inline bool IsGUIProgram() const                                { return m_isGUIProgram; }
        inline void SetDebuggerSearchPaths(const wxArrayString& paths)  { m_debuggerSearchPaths = paths; }
        inline const wxArrayString& GetDebuggerSearchPaths() const      { return m_debuggerSearchPaths; }
        inline void SetClangC11(bool clangC11)                          { this->m_clangC11 = clangC11; }
        inline bool IsClangC11() const                                  { return m_clangC11; }
        inline void SetClangC14(bool clangC14)                          { this->m_clangC14 = clangC14; }
        inline bool IsClangC14() const                                  { return m_clangC14; }
        inline void SetClangC17(bool clangC17)                          { this->m_clangC17 = clangC17; }
        inline bool IsClangC17() const                                  { return m_clangC17; }
        inline void SetClangCcPCH(const wxString& ccPCH)                { this->m_ccPCH = ccPCH; }
        inline const wxString& GetClangCcPCH() const                    { return m_ccPCH; }
        inline void SetCcSearchPaths(const wxString& ccSearchPaths)     { this->m_ccSearchPaths = ccSearchPaths; }
        inline void SetClangCmpFlags(const wxString& clangCmpFlags)     { this->m_clangCmpFlags = clangCmpFlags; }
        inline void SetClangPPFlags(const wxString& clangPPFlags)       { this->m_clangPPFlags = clangPPFlags; }
        inline const wxString& GetCcSearchPaths() const                 { return m_ccSearchPaths; }
        inline const wxString& GetClangCmpFlags() const                 { return m_clangCmpFlags; }

        inline void SetClangCmpFlagsC(const wxString& clangCmpFlagsC)   { this->m_clangCmpFlagsC = clangCmpFlagsC; }
        inline const wxString& GetClangCmpFlagsC() const                { return m_clangCmpFlagsC; }
        inline const wxString& GetClangPPFlags() const                  { return m_clangPPFlags; }
        inline void SetPchCompileFlags(const wxString& pchCompileFlags) { this->m_pchCompileFlags = pchCompileFlags; }
        inline const wxString& GetPchCompileFlags() const               { return m_pchCompileFlags; }

        inline const wxString& GetAssmeblerOptions() const              { return m_commonConfig.GetAssemblerOptions(); }
        inline void SetAssmeblerOptions(const wxString& options)        { m_commonConfig.SetAssemblerOptions(options); }

        inline void SetPchInCommandLine(bool pchInCommandLine)          { this->m_pchInCommandLine = pchInCommandLine; }
        inline bool GetPchInCommandLine() const                         { return m_pchInCommandLine; }
        inline void SetEnvvars(const wxString& envvars)                 { this->m_envvars = envvars; }
        inline const wxString& GetEnvvars() const                       { return m_envvars; }
        wxString GetPreprocessor() const;
        inline void GetPreprocessor(wxArrayString& arr)                 { m_commonConfig.GetPreprocessor(arr); }
        void SetPreprocessor(const wxString& prepr);

        inline const wxString& GetCompilerType() const                  { return m_compilerType; }

        CompilerPtr GetCompiler() const;

        inline void SetDebugArgs(const wxString& debugArgs)             { this->m_debugArgs = debugArgs; }
        inline void SetUseSeparateDebugArgs(bool useSeparateDebugArgs)  { this->m_useSeparateDebugArgs = useSeparateDebugArgs; }
        inline const wxString& GetDebugArgs() const                     { return m_debugArgs; }
        inline bool GetUseSeparateDebugArgs() const                     { return m_useSeparateDebugArgs; }
        inline void SetCompilerType(const wxString& cmpType)            { m_compilerType = cmpType; }
        inline const wxString& GetDebuggerType() const                  { return m_debuggerType; }
        inline void SetDebuggerType(const wxString& type)               { m_debuggerType = type; }

        wxString GetIncludePath() const;
        inline const wxString& GetCompileOptions() const                { return m_commonConfig.GetCompileOptions(); }
        inline const wxString& GetCCompileOptions() const               { return m_commonConfig.GetCCompileOptions(); }
        inline const wxString& GetLinkOptions() const                   { return m_commonConfig.GetLinkOptions(); }
        wxString GetLibraries() const;
        wxString GetLibPath() const;
        inline void GetPreBuildCommands(BuildCommandList& cmds)         { cmds = m_preBuildCommands; }
        inline void GetPostBuildCommands(BuildCommandList& cmds)        { cmds = m_postBuildCommands; }
        inline const wxString& GetName() const                          { return m_name; }
        inline bool IsCompilerRequired() const                          { return m_compilerRequired; }
        inline bool IsLinkerRequired() const                            { return m_linkerRequired; }
        wxString GetOutputFileName() const;
        wxString GetIntermediateDirectory() const;
        inline const wxString& GetCommand() const                       { return m_command; }
        inline const wxString& GetCommandArguments() const              { return m_commandArguments; }
        wxString GetWorkingDirectory() const;
        inline bool IsCustomBuild() const                               { return m_enableCustomBuild; }
        inline const wxString& GetCustomBuildCmd() const                { return m_customBuildCmd; }
        inline const wxString& GetCustomCleanCmd() const                { return m_customCleanCmd; }
        inline const wxString& GetCustomRebuildCmd() const              { return m_customRebuildCmd; }
        inline void SetIncludePath(const wxArrayString& paths)          { m_commonConfig.SetIncludePath(paths); }
        void SetIncludePath(const wxString& path);
        void SetLibraries(const wxString& libs);

        void SetLibPath(const wxString& path);
        inline void SetCompileOptions(const wxString& options)          { m_commonConfig.SetCompileOptions(options); }
        inline void SetCCompileOptions(const wxString& options)         { m_commonConfig.SetCCompileOptions(options); }
        inline void SetLinkOptions(const wxString& options)             { m_commonConfig.SetLinkOptions(options); }
        inline void SetPreBuildCommands(const BuildCommandList& cmds)   { m_preBuildCommands = cmds; }
        inline void SetPostBuildCommands(const BuildCommandList& cmds)  { m_postBuildCommands = cmds; }
        inline void SetLibraries(const wxArrayString& libs)             { m_commonConfig.SetLibraries(libs); }
        inline void SetLibPath(const wxArrayString& libPaths)           { m_commonConfig.SetLibPath(libPaths); }
        inline void SetName(const wxString& name)                       { m_name = name; }
        inline void SetCompilerRequired(bool required)                  { m_compilerRequired = required; }
        inline void SetLinkerRequired(bool required)                    { m_linkerRequired = required; }
        inline void SetOutputFileName(const wxString& name)             { m_outputFile = name; }
        inline void SetIntermediateDirectory(const wxString& dir)       { m_intermediateDirectory = dir; }
        inline void SetCommand(const wxString& cmd)                     { m_command = cmd; }
        inline void SetCommandArguments(const wxString& cmdArgs)        { m_commandArguments = cmdArgs; }
        inline void SetWorkingDirectory(const wxString& dir)            { m_workingDirectory = dir; }
        inline void SetCustomBuildCmd(const wxString& cmd)              { m_customBuildCmd = cmd; }
        inline void SetCustomCleanCmd(const wxString& cmd)              { m_customCleanCmd = cmd; }
        inline void SetCustomRebuildCmd(const wxString& cmd)            { m_customRebuildCmd = cmd; }
        inline void EnableCustomBuild(bool enable)                      { m_enableCustomBuild = enable; }

        inline void SetResCompilerRequired(bool required)               { m_isResCmpNeeded = required; }
        inline bool IsResCompilerRequired() const                       { return m_isResCmpNeeded; }

        inline void SetResCmpIncludePath(const wxString& path)          { m_commonConfig.SetResCmpIncludePath(path); }
        inline const wxString& GetResCmpIncludePath() const             { return m_commonConfig.GetResCmpIncludePath(); }

        inline void SetResCmpOptions(const wxString& options)           { m_commonConfig.SetResCmpOptions(options); }
        inline const wxString& GetResCompileOptions() const             { return m_commonConfig.GetResCompileOptions(); }

        // special custom rules
        inline wxString GetPreBuildCustom() const                       { return m_customPreBuildRule; }
        inline wxString GetPostBuildCustom() const                      { return m_customPostBuildRule; }

        inline void SetPreBuildCustom(const wxString& rule)             { m_customPreBuildRule = rule; }
        inline void SetPostBuildCustom(const wxString& rule)            { m_customPostBuildRule = rule; }

        inline void SetCustomBuildWorkingDir(const wxString& customBuildWorkingDir)
        {
            this->m_customBuildWorkingDir = customBuildWorkingDir;
        }
        inline const wxString& GetCustomBuildWorkingDir() const         { return m_customBuildWorkingDir; }

        inline void SetPauseWhenExecEnds(bool pauseWhenExecEnds)        { this->m_pauseWhenExecEnds = pauseWhenExecEnds; }
        inline bool GetPauseWhenExecEnds() const                        { return m_pauseWhenExecEnds; }

        inline void SetMakeGenerationCommand(const wxString& makeGenerationCommand)
        {
            this->m_makeGenerationCommand = makeGenerationCommand;
        }
        inline void SetToolName(const wxString& toolName)               { this->m_toolName = toolName; }

        inline const wxString& GetMakeGenerationCommand() const         { return m_makeGenerationCommand; }
        inline const wxString& GetToolName() const                      { return m_toolName; }

        inline void SetSingleFileBuildCommand(const wxString& singleFileBuildCommand)
        {
            this->m_singleFileBuildCommand = singleFileBuildCommand;
        }
        inline const wxString& GetSingleFileBuildCommand() const        { return m_singleFileBuildCommand; }

        inline void SetPreprocessFileCommand(const wxString& preprocessFileCommand)
        {
            this->m_preprocessFileCommand = preprocessFileCommand;
        }
        inline const wxString& GetPreprocessFileCommand() const         { return m_preprocessFileCommand; }

        inline const wxString& GetProjectType() const                   { return m_projectType; }
        inline void SetProjectType(const wxString& projectType)         { m_projectType = projectType; }

        inline void SetDebuggerStartupCmds(const wxString& cmd)         { m_debuggerStartupCmds = cmd; }
        inline const wxString& GetDebuggerStartupCmds() const           { return m_debuggerStartupCmds; }

        inline void SetIsDbgRemoteTarget(bool isDbgRemoteTarget)        { m_isDbgRemoteTarget = isDbgRemoteTarget; }
        inline bool GetIsDbgRemoteTarget() const                        { return m_isDbgRemoteTarget; }

        inline void SetDbgHostName(const wxString& dbgHostName)         { this->m_dbgHostName = dbgHostName; }
        inline void SetDbgHostPort(const wxString& dbgHostPort)         { this->m_dbgHostPort = dbgHostPort; }

        inline const wxString& GetDbgHostName() const                   { return m_dbgHostName; }
        inline const wxString& GetDbgHostPort() const                   { return m_dbgHostPort; }

        inline void SetIsDbgRemoteExtended(bool isDbgRemoteExtended)    { m_isDbgRemoteExtended = isDbgRemoteExtended; }
        inline bool GetIsDbgRemoteExtended() const                      { return m_isDbgRemoteExtended; }

        inline void SetCustomTargets(const BuildConfig::StringMap_t & customTargets) { this->m_customTargets = customTargets; }
        inline const BuildConfig::StringMap_t& GetCustomTargets() const { return m_customTargets; }
        inline void SetDebuggerPath(const wxString& debuggerPath)       { this->m_debuggerPath = debuggerPath; }
        inline const wxString& GetDebuggerPath() const                  { return m_debuggerPath; }

        inline void SetDebuggerPostRemoteConnectCmds(const wxString& cmds)
        {
            m_debuggerPostRemoteConnectCmds = cmds;
        }
        inline const wxString& GetDebuggerPostRemoteConnectCmds() const { return m_debuggerPostRemoteConnectCmds; }

        inline const wxString& GetBuildCmpWithGlobalSettings() const    { return m_buildCmpWithGlobalSettings; }
        inline void SetBuildCmpWithGlobalSettings(const wxString& type) { m_buildCmpWithGlobalSettings = type; }

        inline const wxString& GetBuildLnkWithGlobalSettings() const    { return m_buildLnkWithGlobalSettings; }
        inline void SetBuildLnkWithGlobalSettings(const wxString& type) { m_buildLnkWithGlobalSettings = type; }

        inline const wxString& GetBuildResWithGlobalSettings() const    { return m_buildResWithGlobalSettings; }
        inline void SetBuildResWithGlobalSettings(const wxString& type) { m_buildResWithGlobalSettings = type; }

        inline const BuildConfigCommon& GetCommonConfiguration() const  { return m_commonConfig; }
        inline void SetPrecompiledHeader(const wxString& header)        { this->m_precompiledHeader = header; }
        inline const wxString& GetPrecompiledHeader() const             { return m_precompiledHeader; }

        inline void SetDbgEnvSet(const wxString& str)                   { m_dbgEnvSet = str; }
        inline void SetEnvVarSet(const wxString& str)                   { m_envVarSet = str; }
        inline const wxString& GetDbgEnvSet() const                     { return m_dbgEnvSet; }
        inline const wxString& GetEnvVarSet() const                     { return m_envVarSet; }

        inline void SetBuildSystem(const wxString& str)                 { m_buildSystem = str; }
        inline const wxString& GetBuildSystem() const                   { return m_buildSystem; }
        inline void SetBuildSystemArguments(const wxString& str)        { m_buildSystemArguments = str; }
        inline const wxString& GetBuildSystemArguments() const          { return m_buildSystemArguments; }
        
        /**
         * @brief return the build defined for this build configuration
         * @return 
         */
        BuilderPtr GetBuilder();
    private:
        BuildConfigCommon m_commonConfig;
        wxString m_name;
        BuildCommandList m_preBuildCommands;
        BuildCommandList m_postBuildCommands;
        wxString m_outputFile;
        wxString m_intermediateDirectory;
        wxString m_command;
        wxString m_commandArguments;
        wxString m_workingDirectory;
        wxString m_compilerType;
        wxString m_projectType;
        wxString m_customBuildCmd;
        wxString m_customCleanCmd;
        wxString m_customRebuildCmd;
        wxString m_debuggerType;
        wxString m_customPostBuildRule;
        wxString m_customPreBuildRule;
        wxString m_customBuildWorkingDir;
        wxString m_toolName;
        wxString m_makeGenerationCommand;
        wxString m_singleFileBuildCommand;
        wxString m_preprocessFileCommand;
        wxString m_debuggerStartupCmds;
        wxString m_debuggerPostRemoteConnectCmds;
        wxString m_dbgHostName;
        wxString m_dbgHostPort;
        StringMap_t m_customTargets;
        wxString m_debuggerPath;
        wxString m_buildCmpWithGlobalSettings;
        wxString m_buildLnkWithGlobalSettings;
        wxString m_buildResWithGlobalSettings;
        wxString m_precompiledHeader;
        wxString m_envVarSet;
        wxString m_dbgEnvSet;
        wxString m_debugArgs;
        wxString m_envvars;
        wxString m_pchCompileFlags;
        wxString m_clangPPFlags;   // clang only pre-processors (useful when using a custom makefile)
        wxString m_clangCmpFlags;  // clang only compilation flags C++ (useful when using a custom makefile)
        wxString m_clangCmpFlagsC; // clang only compilation flags C   (useful when using a custom makefile)
        wxString m_ccSearchPaths;
        wxString m_ccPCH;
        wxArrayString m_debuggerSearchPaths;
        ePCHPolicy m_pchPolicy;
        wxString m_buildSystem;
        wxString m_buildSystemArguments;
        bool m_pauseWhenExecEnds;
        bool m_useSeparateDebugArgs;
        bool m_pchInCommandLine;
        bool m_isDbgRemoteTarget;
        bool m_isDbgRemoteExtended;
        bool m_isGUIProgram;
        bool m_isProjectEnabled;
        bool m_compilerRequired;
        bool m_linkerRequired;
        bool m_enableCustomBuild;
        bool m_isResCmpNeeded;
        bool m_clangC11;
        bool m_clangC14;
        bool m_clangC17;
    };

    class _gsgsAPI BuilderConfig
    {
    public:
        BuilderConfig(wxXmlNode *node);
        virtual ~BuilderConfig();
        wxXmlNode *ToXml() const;

        inline void SetIsActive(bool set)               { m_isActive = set; }
        inline bool GetIsActive() const                 { return m_isActive; }
        inline void SetName(const wxString &name)       { m_name = name; }
        inline const wxString &GetName() const          { return m_name; }
        inline void SetToolPath(const wxString &path)   { m_toolPath = path; }
        inline const wxString &GetToolPath() const      { return m_toolPath; }
        inline void SetToolOptions(const wxString &options) { m_toolOptions = options; }
        inline const wxString &GetToolOptions() const   { return m_toolOptions; }
        inline void SetToolJobs(const wxString &jobs)   { m_toolJobs = jobs; }
        inline const wxString &GetToolJobs() const      { return m_toolJobs; }
    private:
        wxString m_name;
        wxString m_toolPath;
        wxString m_toolOptions;
        wxString m_toolJobs;
        bool     m_isActive;
    };

    typedef SharedPtr<BuilderConfig> BuilderConfigPtr;

    class _gsgsAPI BuildSettingsConfigCookie
    {
    public:
        wxXmlNode* child;
        wxXmlNode* parent;
    public:
        BuildSettingsConfigCookie()
            : child(NULL)
            , parent(NULL)
        {
        }
        ~BuildSettingsConfigCookie() {}
    };

    class _gsgsAPI BuildSettingsConfig
    {
    public:
        typedef std::pair<wxString, wxString> Pair_t;
        typedef std::map<int, Pair_t>         Map_t;
    public:
        BuildSettingsConfig();
        virtual ~BuildSettingsConfig();

        static BuildSettingsConfig & singleton();

        static void Release();
        
        void Clear();
        
        void SetTargets( const wxString &projectName, const BuildConfig::StringMap_t& targets );
        
        BuildSettingsConfig::Pair_t GetTarget(int menuId) const;
        
        const BuildSettingsConfig::Map_t& GetTargets() const   { return m_targets; }
        
        const wxString& GetProjectName() const              { return m_projectName; }

        /**
         * @brief return a map for the available compilers and their global include paths
         */
        std::unordered_map<wxString, wxArrayString> GetCompilersGlobalPaths() const;

        /**
         * Load the configuration file
         * @param version XML version which to be loaded, any version different from this one, will cause
         * gsgs to override the user version
         */
        bool Load(const wxString& version, const wxString& xmlFilePath = "");

        /**
         * @brief delete all compilers
         */
        void DeleteAllCompilers(bool notify = true);

        /**
         * @brief return list of all compiler names
         */
        wxArrayString GetAllCompilersNames() const;

        /**
         * @brief return vector with all compilers defined
         * @param family the compiler family. Leave empty to get list of
         * all compilers regardless their family
         */
        CompilerPtrVec_t GetAllCompilers(const wxString& family = "") const;

        /**
         * @brief replace the current compilers list with a new one
         */
        void SetCompilers(const std::vector<CompilerPtr>& compilers);

        /**
         * Set or update a given compiler using its name as the index
         */
        void SetCompiler(CompilerPtr cmp);

        /**
         * @brief return the default compiler for a given family
         */
        CompilerPtr GetDefaultCompiler(const wxString& compilerFamilty) const;
        /**
         * Find and return compiler by name
         */
        CompilerPtr GetCompiler(const wxString& name) const;

        /**
         * Returns the first compiler found.
         * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is
         * necessary for the library to make these functions reentrant
         * (i.e. allow more than one enumeration on one and the same object simultaneously).
         */
        CompilerPtr GetFirstCompiler(BuildSettingsConfigCookie& cookie);

        /**
         * Returns the next compiler.
         * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is
         * necessary for the library to make these functions reentrant
         * (i.e. allow more than one enumeration on one and the same object simultaneously).
         */
        CompilerPtr GetNextCompiler(BuildSettingsConfigCookie& cookie);

        /**
         * check whether a compiler with a given name already exist
         */
        bool IsCompilerExist(const wxString& name) const;

        /**
         * delete compiler
         */
        void DeleteCompiler(const wxString& name);

        /**
         * Add build system
         */
        void SetBuildSystem(BuilderConfigPtr bs);

        /**
         * get build system from configuration by name
         */
        BuilderConfigPtr GetBuilderConfig(const wxString& name);

        /**
         * @brief save builder configurtation to the XML file
         * @param builder
         */
        void SaveBuilderConfig(BuilderPtr builder);

        /*
         * get name of current selected build system from configuration
         */
        wxString GetSelectedBuildSystem();

        /**
         * @brief discard the current settings and reload the default build
         * settings
         */
        void RestoreDefaults();
    protected:
        wxXmlNode* GetCompilerNode(const wxString& name) const;
        void DoUpdateCompilers();
        bool SaveXmlFile();
        
    private:
        wxXmlDocument* m_doc;
        wxFileName m_fileName;
        wxString m_version;
        std::unordered_map<wxString, CompilerPtr> m_compilers;
        Map_t    m_targets;
        wxString m_projectName;
    };
}
#define gsgs_Settings() gsgs::BuildSettingsConfig::singleton()
#endif // BUILD_CONFIGURATION_H
