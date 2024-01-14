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
#ifndef _GSGS_FILEUTIL_H
#define _GSGS_FILEUTIL_H

#include "gsgsPreInclude.h"
#include "gsgsSharedPtr.h"
#include <wx/regex.h>
#include <wx/timer.h>
#include <wx/docview.h>

#ifdef __WXMSW__
#define CL_FSW_USE_TIMER 1
#else
#define CL_FSW_USE_TIMER 1
#endif

#if !CL_FSW_USE_TIMER
#include <wx/fswatcher.h>
#endif

typedef struct yy_buffer_state* BufferState;
struct fcState
{
    BufferState buffer;
    wxString filename;
};

namespace gsgs
{
    class _gsgsAPI FileHistory : public wxFileHistory
    {
    public:
        FileHistory();
        virtual ~FileHistory();
        
        void GetFiles(wxArrayString &files);

        /**
         * \brief add files to the designated menu, first removing any extension
         */
        void AddFilesToMenuWithoutExt();

    protected:
        void AddFilesToMenuWithoutExt(wxMenu* menu);
    };

    class _gsgsAPI FileWatch : public wxEvtHandler
    {
    public:
        struct File 
        {
            wxFileName filename;
            time_t lastModified;
            size_t file_size;
            typedef std::map<wxString, File> Map_t;
        };

        wxEvtHandler* m_owner;
    #if CL_FSW_USE_TIMER
        FileWatch::File::Map_t m_files;
        wxTimer* m_timer;
    #else
        wxFileSystemWatcher m_watcher;
        wxFileName m_watchedFile;
    #endif

        typedef wxSharedPtr<FileWatch> Ptr_t;
    protected:
    #if CL_FSW_USE_TIMER
        void OnTimer(wxTimerEvent& event);
    #else
        void OnFileModified(wxFileSystemWatcherEvent& event);
    #endif
    public:
        FileWatch();
        virtual ~FileWatch();

        bool IsRunning() const;
            
        inline void SetOwner(wxEvtHandler* owner)   { this->m_owner = owner; }
        inline wxEvtHandler* GetOwner()             { return m_owner; }

        void SetFile(const wxFileName& filename);

        void RemoveFile(const wxFileName& filename);

        void Start();

        void Stop();

        void Clear();
    };

    class _gsgsAPI FileOpen
    {
    public:
        typedef std::unordered_set<wxString> Set_t;
        typedef std::vector<wxString> Vector_t;
        typedef std::stack<fcState> Stack_t;
    public:
        static FileOpen* Get();
        static void Release();

        FILE* try_open(const wxString& path, const wxString& name, wxString& filepath);

        // Flex buffer states
        BufferState PopBufferState();
        void PushBufferState(BufferState buffer, const wxString& filename);
        inline bool IsStateStackEmpty() const       { return _states.empty(); }

        inline void SetCwd(const wxString& _cwd)    { this->_cwd = _cwd; }
        void AddSearchPath(const wxString& path);
        void AddExcludePath(const wxString& path);

        /**
         * @brief open a file based on the include paths and return a pointer to it.
         * @param include_path the string as appears inside the #include statement
         * @param filepath [output] the file that was opened
         */
        FILE* OpenFile(const wxString& include_path, wxString& filepath);

        void ClearResults()
        {
            _matchedfiles.clear();
            _scannedfiles.clear();
            _namespaces.clear();
            _namespaceAliases.clear();
            _includeStatements.clear();
            _depth = 0;
            _cwd.Clear();
            while(!_states.empty()) {
                _states.pop();
            }
        }

        inline void ClearSearchPath()
        {
            _searchPath.clear();
            _excludePaths.clear();
        }

        inline void incDepth() { _depth++; }

        inline void decDepth()
        {
            _depth--;
            if(_depth < 0) {
                _depth = 0;
            }
        }

        inline int getDepth() const                     { return _depth; }

        inline void setMaxDepth(int depth)              { _maxDepth = depth; }

        inline const int& getMaxDepth() const           { return _maxDepth; }

        // getters
        inline const FileOpen::Set_t& GetResults() const { return _matchedfiles; }

        ////////////////////////////////////////////////////
        // Using namespace support
        inline const FileOpen::Set_t& GetNamespaces() const { return _namespaces; }

        inline void ClearNamespace()                    { _namespaces.clear(); }
        void AddNamespace(const char* ns);

        ////////////////////////////////////////////////////
        // Namespace aliasing
        inline const FileOpen::Set_t& GetNamespaceAliases() const { return _namespaceAliases; }
        inline void ClearNamespaceAliases()             { _namespaceAliases.clear(); }
        inline void AddNamespaceAlias(const char* alias){ _namespaceAliases.insert(alias); }
        inline void AddIncludeStatement(const wxString& statement);
        inline const FileOpen::Set_t& GetIncludeStatements() const { return _includeStatements; }
        inline FileOpen::Set_t& GetIncludeStatements()  { return _includeStatements; }
    private:
        FileOpen();
        virtual ~FileOpen();
    protected:
        static FileOpen* gInstance;

        FileOpen::Vector_t _searchPath;
        FileOpen::Vector_t _excludePaths;
        FileOpen::Set_t _matchedfiles;
        FileOpen::Set_t _scannedfiles;
        int _depth;
        int _maxDepth;
        FileOpen::Set_t _namespaces;
        FileOpen::Set_t _namespaceAliases;
        FileOpen::Set_t _includeStatements;
        FileOpen::Stack_t _states;
        wxString _cwd;
    };

    class _gsgsAPI FileScan
    {
    public:
        struct EntryData {
            size_t flags = 0;
            wxString fullpath;
            typedef std::vector<EntryData> Vec_t;
        };

        enum eFileAttributes {
            kInvalid = 0,
            kIsFile = (1 << 0),
            kIsFolder = (1 << 1),
            kIsHidden = (1 << 2),
            kIsSymlink = (1 << 3),
        };
    public:
        FileScan();
        virtual ~FileScan();

        /**
         * @brief collect all files matching a given pattern from a root folder
         * @param rootFolder the scan root folder
         * @param filesOutput [output] output result full path entries
         * @param filespec files spec
         * @param excludeFolders list of folder to exclude from the search
         * @return number of files found
         */
        size_t Scan(const wxString& rootFolder, std::vector<wxString>& filesOutput, const wxString& filespec = "*",
            const wxString& excludeFilespec = "", const gsgsStringSet& excludeFolders = gsgsStringSet());

        /**
         * @brief scan folder for files and folders. This function does not recurse into folders. Everything that matches
         * "matchSpec" will get collected.
         */
        size_t ScanNoRecurse(const wxString& rootFolder, FileScan::EntryData::Vec_t& results, const wxString& matchSpec = "*");
    };

    extern _gsgsAPI int crawlerScan(const char* filePath);

    #define gsgsRemoveFile(filename) FileUtil::RemoveFile(filename, (wxString() << __FILE__ << ":" << __LINE__))

    class _gsgsAPI FileUtil
    {
    public:
        class Deleter
        {
        public:
            Deleter(const wxFileName & filename)
                : m_filename(filename){}

            ~Deleter()
            {
                if(m_filename.Exists()) { gsgsRemoveFile(m_filename); }
            }
        private:
            wxFileName m_filename;
        };
        
        enum FileType {
            TypeOther = wxNOT_FOUND,
            TypeSourceC, // 0
            TypeSourceCpp,
            TypeHeader,
            TypeResource,
            TypeYacc,
            TypeLex,
            TypeQtForm,
            TypeQtResource,
            TypeWorkspace,
            TypeProject,
            TypeFormbuilder,
            TypeCodedesigner,
            TypeErd,
            TypePhp,
            TypeXml,
            TypeCSS,
            TypeJS,
            TypePython,
            TypeExe,
            TypeHtml,
            TypeArchive,
            TypeDll,
            TypeBmp,
            TypeSvg,
            TypeMakefile,
            TypeText,
            TypeScript,
            TypeWxCrafter,
            TypeXRC,
            TypeSQL,
            TypeFolder,
            TypeFolderExpanded, // For UI purposes only
            TypeProjectActive,  // For UI purposes only
            TypeProjectExpanded, // For UI purposes only
            TypeWorkspaceFolder, // For UI purposes only
            TypeWorkspaceFolderExpanded, // For UI purposes only
            TypeWorkspacePHP,
            TypeWorkspaceDocker,
            TypeWorkspaceNodeJS,
            TypeWorkspacePHPTags,
            TypeWorkspaceDatabase,
            TypeAsm,
            TypeJava,
            TypeQMake,
            TypeCMake,
            TypeDockerfile,
            TypeYAML,
            TypeDatabase,
            TypeLast,
        };
        
    public:
        static bool ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv = wxConvUTF8);

        /**
         * @brief attempt to read up to bufferSize from the beginning of file
         */
        static bool ReadBufferFromFile(const wxFileName& fn, wxString& data, size_t bufferSize);

        /**
         * @brief set the file content (replacing it)
         */
        static bool WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv = wxConvUTF8);

        /**
         * @brief open file explorer at given path
         */
        static void OpenFileExplorer(const wxString& path);

        /**
         * @brief open file explorer at given path and select the input file
         */
        static void OpenFileExplorerAndSelect(const wxFileName& filename);

        /**
         * @brief launch the OS default terminal at a given path
         */
        static void OpenTerminal(const wxString& path, const wxString& user_command = "", bool pause_when_exit = false);

        /**
         * @brief open the built-in terminal
         */
        static void OpenBuiltInTerminal(const wxString& wd, const wxString& user_command, bool pause_when_exit = false);

        /**
         * @brief open ssh terminal
         * @param sshClient ssh client to use (putty, ssh etc)
         * @param connectString e.g. eran@host
         * @param password the password
         * @param port ssh port
         */
        static void OpenSSHTerminal(const wxString& sshClient, const wxString& connectString, const wxString& password, int port = 22);

        /**
         * @brief OSX only: open Terminal and return its TTY
         * @param path working directory
         * @param [output] tty the TTY of the launched terminal
         */
        static void OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, const wxString& appname, wxString& tty, long& pid);

        /**
         * @brief return the command needed to open OSX terminal at a given directory and launch a command
         */
        static wxString GetOSXTerminalCommand(const wxString& command, const wxString& workingDirectory);

        /**
         * @brief file masking search
         */
        static bool WildMatch(const wxString& mask, const wxString& filename);
        /**
         * @brief file masking search
         */
        static bool WildMatch(const wxString& mask, const wxFileName& filename);

        /**
         * @brief file masking search
         */
        static bool WildMatch(const wxArrayString& masks, const wxString& filename);

        /**
         * @brief return true if needle exists in the haystack. Supports fuzzy search
         * @param needle the pattern to search
         * @param haystack the string to search on
         */
        static bool FuzzyMatch(const wxString& needle, const wxString& haystack);

        /**
         * @brief an efficient way to tokenize string into words (separated by SPACE and/or TAB)
         * @code
         * wxString str = "My String That Requires Tokenize";
         * wxString word; // The output
         * size_t offset = 0;
         * while(NextWord(str, offset, word)) {
         *      // Do something with "word" here
         * }
         * @codeend
         * @param str the string to tokenize
         * @param offset used internally, allocate one on the stack and initialise it to 0
         * @param word [output]
         * @return true if a word was found
         */
        static bool NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower = false);

        /**
         * @brief split a string by whitespace
         */
        static size_t SplitWords(const wxString& str, gsgsStringSet& outputSet, bool makeLower = false);

        /**
         * @brief decode URI using percent encoding
         */
        static wxString DecodeURI(const wxString& uri);

        /**
         * @brief encode URI using percent encoding
         */
        static wxString EncodeURI(const wxString& uri);

        /**
         * @brief escape string. Each space and double quotes marker is escaped with backslash
         */
        static wxString EscapeString(const wxString& str);

        /**
         * @brief is the file or folder a hidden file?
         */
        static bool IsHidden(const wxFileName& path);
        /**
         * @brief is the file or folder a hidden file?
         */
        static bool IsHidden(const wxString& path);

        /**
         * @brief return true if filename is a symlink
         */
        static inline bool IsSymlink(const wxFileName& filename) { return IsSymlink(filename.GetFullPath()); }
        /**
         * @brief return true if filename is a symlink
         */
        static bool IsSymlink(const wxString& filename);
        
        /**
         * @brief return true if filename is a symlink
         */
        static inline bool IsDirectory(const wxFileName& filename) { return IsDirectory(filename.GetFullPath()); }
        /**
         * @brief return true if filename is a symlink
         */
        static bool IsDirectory(const wxString& filename);
        
        /**
         * @brief set permissions to filename
         */
        static bool SetFilePermissions(const wxFileName& filename, mode_t perm);

        /**
         * @brief get file permissions
         */
        static bool GetFilePermissions(const wxFileName& filename, mode_t& perm);

        /**
         * @brief return the file modification time
         */
        static time_t GetFileModificationTime(const wxFileName& filename);

        /**
         * @brief return the file size, in bytes
         */
        static size_t GetFileSize(const wxFileName& filename);

        /**
         * @brief replace any unwanted characters with underscore
         * The chars that we replace are:
         * @-^%&$#@!(){}[]+=;,.
         * @param name
         * @return modified name excluding the above chars (will be replaced with _)
         */
        static wxString NormaliseName(const wxString& name);

        /**
         * \brief write file using UTF8 converter
         * \param fileName file path
         * \param content file's content
         * \return true on success, false otherwise
         */
        static bool WriteFileUTF8(const wxString& fileName, const wxString& content);

        /**
         * @brief remove a file from the file system
         * @param filename file name to remove
         * @param context unique context string which will be logged to the log file
         */
        static inline bool RemoveFile(const wxFileName& filename, const wxString& context = "")
        {
            return RemoveFile(filename.GetFullPath(), context);
        }
        /**
         * @brief same as above
         */
        static bool RemoveFile(const wxString& filename, const wxString& context = "");

        static unsigned int UTF8Length(const wchar_t* uptr, unsigned int tlen);

        /**
         * @brief (on Linux) makes-absolute filepath, and dereferences it and any symlinked dirs in the path
         */
        static wxString RealPath(const wxString& filepath);

        /**
         * @brief convert string into std::string
         */
        static std::string ToStdString(const wxString& str);
        
        static FileType GetType(const wxString& filename, FileUtil::FileType defaultType = FileUtil::TypeOther);
        static void Init();

        /**
         * @brief return true if the file is a C/C++ file
         */
        static bool IsCxxFile(const wxString& filename);
        static inline bool IsCxxFile(const wxFileName& filename) { return IsCxxFile(filename.GetFullPath()); }

        /**
         * @brief return true if a file is a JavaScript file
         */
        static bool IsJavascriptFile(const wxString& filename);
        static inline bool IsJavascriptFile(const wxFileName& filename) { return IsJavascriptFile(filename.GetFullPath()); }

        /**
         * @param return true if a file is a PHP file
         */
        static bool IsPHPFile(const wxString& filename);
        static inline bool IsPHPFile(const wxFileName& filename) { return IsPHPFile(filename.GetFullPath()); }

        /**
         * @param return true if a file is a Java file
         */
        static bool IsJavaFile(const wxString& filename);
        static inline bool IsJavaFile(const wxFileName& filename) { return IsJavaFile(filename.GetFullPath()); }

        /**
         * @param return true if the file is of type 'type'
         */
        static bool IsFileType(const wxString& filename, FileUtil::FileType type);
        static inline bool IsFileType(const wxFileName& filename, FileUtil::FileType type) {  return IsFileType(filename.GetFullPath(), type);}

        /**
         * @brief attempt to autodetect the file type by examining its content
         */
        static bool AutoDetectByContent(const wxString& filename, FileUtil::FileType& fileType);

        /**
         * @brief return the file type only by checking its extension
         */
        static inline FileUtil::FileType GetTypeFromExtension(const wxString& filename) { return GetTypeFromExtension(wxFileName(filename)); }
        static FileUtil::FileType GetTypeFromExtension(const wxFileName& filename);
    };
}
#endif
