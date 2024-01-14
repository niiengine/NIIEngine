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

#ifndef _GSGS_TOOLSETMANAGER_H_
#define _GSGS_TOOLSETMANAGER_H_

#include "gsgsPreInclude.h"
#include "gsgsConfig.h"
#include "gsgsSingleton.h"
#include "project.h"

#define COMPILER_FAMILY_CLANG       "LLVM/Clang"
#define COMPILER_FAMILY_MINGW       "MinGW"
#define COMPILER_FAMILY_GCC         "GCC"
#define COMPILER_FAMILY_VC          "Visual C++"
#define COMPILER_FAMILY_CYGWIN      "Cygwin"

#ifdef __WXMSW__
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_MINGW
#elif defined(__WXMAC__)
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_CLANG
#else
#define COMPILER_DEFAULT_FAMILY COMPILER_FAMILY_GCC
#endif

namespace gsgs
{
    class _gsgsAPI Compiler : public ObjectData
    {
    public:
        enum { eErrorPattern, eWarningPattern };

        enum CmpFileKind { CmpFileKindSource, CmpFileKindResource };

        struct CmpFileTypeInfo {
            wxString extension;
            wxString compilation_line;
            CmpFileKind kind;
        };
        typedef std::map<wxString, CmpFileTypeInfo> CmpFileTypeInfos;

        struct CmpCmdLineOption {
            wxString name;
            wxString help;
        };
        typedef std::map<wxString, CmpCmdLineOption> CmpCmdLineOptions;

        struct CmpInfoPattern {
            wxString pattern;
            wxString lineNumberIndex;
            wxString fileNameIndex;
            wxString columnIndex;
        };

        enum eRegexType { kRegexVC = 0, kRegexGNU };
        typedef std::list<CmpInfoPattern> CmpListInfoPattern;
    public:
        typedef std::map<wxString, wxString>::const_iterator ConstIterator;

        Compiler(wxXmlNode* node, Compiler::eRegexType regexType = Compiler::kRegexGNU);
        virtual ~Compiler();

        /**
         * @brief return the compiler default include paths
         */
        wxArrayString GetDefaultIncludePaths();

        /**
         * @brief return true if this compiler is compatible with GNU compilers
         */
        bool IsGnuCompatibleCompiler() const;

        wxXmlNode* ToXml() const;
        void SetTool(const wxString & toolname, const wxString & cmd);
        void SetSwitch(const wxString & switchName, const wxString & switchValue);

        inline void SetInstallationPath(const wxString & path)          { m_installationPath = path; }
        inline const wxString& GetInstallationPath() const              { return m_installationPath; }
        inline void SetCompilerFamily(const wxString & compilerFamily)  { m_compilerFamily = compilerFamily; }
        inline void SetIsDefault(bool isDefault)                        { m_isDefault = isDefault; }
        inline const wxString& GetCompilerFamily() const                { return m_compilerFamily; }
        inline bool IsDefault() const                                   { return m_isDefault; }
        // iteration over switches
        inline Compiler::ConstIterator SwitchesBegin() const            { return m_switches.begin(); }
        inline Compiler::ConstIterator SwitchesEnd() const              { return m_switches.end(); }

        void AddCompilerOption(const wxString& name, const wxString& desc);
        void AddLinkerOption(const wxString& name, const wxString& desc);

        /**
         * @brief return list of builtin macros for this compiler instance
         * @return
         */
        const wxArrayString& GetBuiltinMacros();

        //---------------------------------------------------
        // setters/getters
        //---------------------------------------------------
        wxString GetTool(const wxString& name) const;
        wxString GetSwitch(const wxString& name) const;

        inline const wxString& GetObjectSuffix() const { return m_objectSuffix; }
        inline void SetObjectSuffix(const wxString & suffix) { m_objectSuffix = suffix; }
        inline const wxString& GetDependSuffix() const { return m_dependSuffix; }
        inline void SetDependSuffix(const wxString & suffix) { m_dependSuffix = suffix; }
        inline const wxString& GetPreprocessSuffix() const { return m_preprocessSuffix; }
        inline void SetPreprocessSuffix(const wxString & suffix) { m_preprocessSuffix = suffix; }

        inline void SetName(const wxString & name) { m_name = name; }
        inline const wxString& GetName() const { return m_name; }
        inline const CmpListInfoPattern & GetErrPatterns() const { return m_errorPatterns; }
        inline const CmpListInfoPattern & GetWarnPatterns() const { return m_warningPatterns; }

        inline void SetErrPatterns(const CmpListInfoPattern& p) { m_errorPatterns = p; }
        inline void SetWarnPatterns(const CmpListInfoPattern& p) { m_warningPatterns = p; }

        inline void SetGlobalIncludePath(const wxString& path) { m_globalIncludePath = path; }
        inline void SetGlobalLibPath(const wxString& path) { m_globalLibPath = path; }
        inline const wxString& GetGlobalIncludePath() const { return m_globalIncludePath; }
        inline const wxString& GetGlobalLibPath() const { return m_globalLibPath; }

        inline void SetPathVariable(const wxString& var) { m_pathVariable = var; }
        inline const wxString& GetPathVariable() const { return m_pathVariable; }

        inline void SetFileTypes(const CmpFileTypeInfos& flist)
        {
            m_fileTypes.clear();
            m_fileTypes = flist;
        }

        inline const CmpFileTypeInfos & GetFileTypes() const { return m_fileTypes; }

        inline const CmpCmdLineOptions & GetCompilerOptions() const { return m_compilerOptions; }

        inline void SetCompilerOptions(const CmpCmdLineOptions & cmpOptions) { m_compilerOptions = cmpOptions; }

        inline const CmpCmdLineOptions& GetLinkerOptions() const { return m_linkerOptions; }

        inline void SetLinkerOptions(const CmpCmdLineOptions& cmpOptions) { m_linkerOptions = cmpOptions; }

        inline void SetGenerateDependeciesFile(const bool& generateDependeciesFile)
        {
            m_generateDependeciesFile = generateDependeciesFile;
        }
        inline const bool& GetGenerateDependeciesFile() const { return m_generateDependeciesFile; }
        inline void SetReadObjectFilesFromList(bool readObjectFilesFromList)
        {
            m_readObjectFilesFromList = readObjectFilesFromList;
        }
        inline bool GetReadObjectFilesFromList() const { return m_readObjectFilesFromList; }
        void AddCmpFileType(const wxString& extension, CmpFileKind type, const wxString& compile_line);
        bool GetCmpFileType(const wxString& extension, Compiler::CmpFileTypeInfo& ft);
        inline void SetObjectNameIdenticalToFileName(bool objectNameIdenticalToFileName)
        {
            m_objectNameIdenticalToFileName = objectNameIdenticalToFileName;
        }
        inline bool GetObjectNameIdenticalToFileName() const { return m_objectNameIdenticalToFileName; }
    private:
        void
            AddPattern(int type, const wxString& pattern, int fileNameIndex, int lineNumberIndex, int colIndex = wxNOT_FOUND);

        void AddDefaultGnuComplierOptions();

        void AddDefaultGnuLinkerOptions();

        wxString GetGCCVersion() const;

        wxString GetIncludePath(const wxString& pathSuffix) const;

        wxArrayString POSIXGetIncludePaths() const;
    protected:
        wxString m_name;
        std::map<wxString, wxString> m_switches;
        std::map<wxString, Compiler::CmpFileTypeInfo> m_fileTypes;
        std::map<wxString, wxString> m_tools;
        CmpCmdLineOptions m_compilerOptions;
        CmpCmdLineOptions m_linkerOptions;
        wxString m_objectSuffix;
        wxString m_dependSuffix;
        wxString m_preprocessSuffix;
        CmpListInfoPattern m_errorPatterns;
        CmpListInfoPattern m_warningPatterns;
        wxString m_globalIncludePath;
        wxString m_globalLibPath;
        wxString m_pathVariable;
        wxString m_compilerFamily;
        wxString m_installationPath;
        wxArrayString m_compilerBuiltinDefinitions;
        bool m_generateDependeciesFile;
        bool m_readObjectFilesFromList;
        bool m_objectNameIdenticalToFileName;
        bool m_isDefault;
    };

    class _gsgsAPI ToolsetLocator
    {
    public:
        typedef wxSharedPtr<ToolsetLocator>        Ptr_t;
        typedef std::vector<ToolsetLocator::Ptr_t> Vect_t;
        typedef std::vector<CompilerPtr>             CompilerVec_t;
    public:
        ToolsetLocator();
        virtual ~ToolsetLocator();

        /**
         * @brief locate the compiler
         */
        virtual bool Locate() = 0;

        /**
         * @brief locate a compiler in a directory
         */
        virtual CompilerPtr Locate(const wxString &folder) = 0;

        /**
         * @brief return the compiler
         */
        const ToolsetLocator::CompilerVec_t& GetCompilers() const {
            return m_compilers;
        }
    protected:
        ToolsetLocator::CompilerVec_t m_compilers;
    };

    class _gsgsAPI ToolsetManager
    {
    public:
        ToolsetManager();
        virtual ~ToolsetManager();

        /**
         * @brief return true if the locator found at least one instance
         * on MinGW installed on this machine
         */
        bool FoundMinGWCompiler() const;

        inline void SetCompilersFound(const ToolsetLocator::CompilerVec_t& found) { m_compilersFound = found; }

        inline void SetDetectors(const ToolsetLocator::Vect_t& detectors) { m_detectors = detectors; }

        inline const ToolsetLocator::CompilerVec_t& GetCompilersFound() const { return m_compilersFound; }

        inline const ToolsetLocator::Vect_t& GetDetectors() const { return m_detectors; }

        bool Locate();

        CompilerPtr Locate(const wxString& folder);

        /**
         * @brief prompt the user to download MinGW installer
         */
        static void MSWSuggestToDownloadMinGW(bool prompt);

        void MSWFixClangToolChain(CompilerPtr compiler,
            const ToolsetLocator::CompilerVec_t& allCompilers = ToolsetLocator::CompilerVec_t());
    private:
        ToolsetLocator::Vect_t m_detectors;
        ToolsetLocator::CompilerVec_t m_compilersFound;
    };
}
#endif
