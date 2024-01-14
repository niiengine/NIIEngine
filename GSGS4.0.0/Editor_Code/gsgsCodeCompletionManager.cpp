
#include "gsgsCodeCompletionManager.h"
#include "gsgsTagManager.h"
#include "gsgsFrame.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsEditor.h"
#include "gsgsCompilerCmdParser.h"
#include "gsgsLanguage.h"
#include "code_completion_api.h"
#include "gsgsParseWorker.h"
#include "NewProjectDialog.h"
#include "imanager.h"
#include "gsgsManager.h"
#include "globals.h"
#include "plugin.h"
#include "gsgsImageManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsCodeBoxManager.h"
#include "CxxUsingNamespaceCollector.h"
#include "CxxScannerTokens.h"
#include "CxxPreProcessor.h"
#include "CxxLexerAPI.h"
#include "gsgsPathManager.h"
#include "gsgsJSON.h"
#include "gsgsWorkspace.h"
#include <wx/wxsqlite3.h>
#include "project.h"

namespace gsgs
{
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // CompilationDatabase
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    class CompilationDatabase
    {
    public:
        typedef wxSharedPtr<CompilationDatabase> Ptr_t;
    public:
        CompilationDatabase();
        CompilationDatabase(const wxString& filename);
        /**
         * @brief an "whole in one" method which attempts to find compile_commands.json file
         * create a .db out of it and return a CompilationDatabase object
         */
        static wxArrayString FindIncludePaths(const wxString& rootFolder, wxFileName& lastCompileCommands, time_t& lastCompileCommandsModified);

        virtual ~CompilationDatabase();

        static bool IsDbVersionUpToDate(const wxFileName& fn);
        void Open();
        void Close();

        inline bool IsOpened() const                { return m_db && m_db->IsOpen(); }

        /**
         * @brief return the database file name (compilation.db), usually under the workspace private folder
         * WORKSPACE_PATH/.gsgs/compilation.db
         */
        wxFileName GetFileName() const;
        /**
         * @brief return the location of the CMake (usually compile_commands.json)
         * Note that this function does not check for the existance of the file
         */
        FileNameVector_t GetCompileCommandsFiles() const;
        static FileNameVector_t GetCompileCommandsFiles(const wxString& rootFolder);
        void CompilationLine(const wxString& filename, wxString& compliationLine, wxString& cwd);
        void Initialize();
        bool IsOk() const;
    protected:
        void DropTables();
        void CreateDatabase();
        wxString GetDbVersion();
        /**
         * @brief create our compilation database out of CMake's compile_commands.json file
         */
        void ProcessCMakeCompilationDatabase(const wxFileName& compile_commands);

        wxFileName ConvertGSGSCompilationDatabaseToCMake(const wxFileName& compile_file);
    private:
        wxSQLite3Database* m_db;
        wxFileName m_filename;
    };

    //-----------------------------------------------------------------------------------
    const wxString DB_VERSION = "2.0";
    //-----------------------------------------------------------------------------------
    struct wxFileNameSorter {
        bool operator()(const wxFileName& one, const wxFileName& two) const
        {
            return one.GetModificationTime().GetTicks() > two.GetModificationTime().GetTicks();
        }
    };
    //-----------------------------------------------------------------------------------
    CompilationDatabase::CompilationDatabase()
        : m_db(NULL)
    {
    }
    //-----------------------------------------------------------------------------------
    CompilationDatabase::CompilationDatabase(const wxString& filename)
        : m_db(NULL)
        , m_filename(filename)
    {
    }
    //-----------------------------------------------------------------------------------
    CompilationDatabase::~CompilationDatabase() { Close(); }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::Open()
    {
        // Close the old database
        if(m_db) { Close(); }

        // Create new one
        try {

            m_db = new wxSQLite3Database();
            wxFileName dbfile(gsgs_CppWorker().GetPrivateFolder(), "compilation.db");
            m_db->Open(dbfile.GetFullPath());
            CreateDatabase();

        } catch(wxSQLite3Exception& e) {

            delete m_db;
            m_db = NULL;
        }
    }
    //-----------------------------------------------------------------------------------
    wxFileName CompilationDatabase::GetFileName() const
    {
        wxFileName dbfile;
        if(!m_filename.IsOk()) {
            dbfile = wxFileName(gsgs_CppWorker().GetPrivateFolder(), "compilation.db");

        } else {
            dbfile = m_filename;
        }
        return dbfile;
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::CompilationLine(const wxString& filename, wxString& compliationLine, wxString& cwd)
    {
        if(!IsOpened()) return;

        try {

            wxFileName file(filename);
            if(FileUtil::GetType(file.GetFullName()) == FileUtil::TypeHeader) {
                // This file is a header file, try locating the C++ file for it
                file.SetExt(wxT("cpp"));
            }

            wxString sql;
            sql = wxT("SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_NAME=?");
            wxSQLite3Statement st = m_db->PrepareStatement(sql);
            st.Bind(1, file.GetFullPath());
            wxSQLite3ResultSet rs = st.ExecuteQuery();

            if(rs.NextRow()) {
                compliationLine = rs.GetString(0);
                cwd = rs.GetString(1);
                return;

            } else {
                // Could not find the cpp file for this file, try to locate *any* file from this directory
                sql = "SELECT COMPILE_FLAGS,CWD FROM COMPILATION_TABLE WHERE FILE_PATH=?";
                wxSQLite3Statement st2 = m_db->PrepareStatement(sql);
                st2.Bind(1, file.GetPath());
                wxSQLite3ResultSet rs2 = st2.ExecuteQuery();
                if(rs2.NextRow()) {
                    compliationLine = rs2.GetString(0);
                    cwd = rs2.GetString(1);
                    return;
                }
            }

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::Close()
    {
        if(m_db) {

            try {
                m_db->Close();
                delete m_db;

            } catch(wxSQLite3Exception& e) {
                wxUnusedVar(e);
            }
        }
        m_db = NULL;
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::Initialize()
    {
        Open();
        if(!IsOpened()) return;

        // get list of files created by cmake
        FileNameVector_t files = GetCompileCommandsFiles();

        // pick gsgs's compilation database created by gsgs-cc
        // - convert it to compile_commands.json
        // - append it the list of files
        wxFileName clCustomCompileFile = GetFileName();
        clCustomCompileFile.SetExt("db.txt");
        if(clCustomCompileFile.Exists()) {
            wxFileName compile_commands = ConvertGSGSCompilationDatabaseToCMake(clCustomCompileFile);
            if(compile_commands.IsOk()) { files.push_back(compile_commands); }
        }
        // Sort the files by modification time
        std::sort(files.begin(), files.end(), wxFileNameSorter());

        for(size_t i = 0; i < files.size(); ++i) {
            ProcessCMakeCompilationDatabase(files.at(i));
        }
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::CreateDatabase()
    {
        if(!IsOpened()) return;

        try {
            if(GetDbVersion() != DB_VERSION) { DropTables(); }

            // Create the schema
            m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS COMPILATION_TABLE (FILE_NAME TEXT, FILE_PATH TEXT, CWD TEXT, "
                                "COMPILE_FLAGS TEXT)");
            m_db->ExecuteUpdate("CREATE TABLE IF NOT EXISTS SCHEMA_VERSION (PROPERTY TEXT, VERSION TEXT)");
            m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX1 ON COMPILATION_TABLE(FILE_NAME)");
            m_db->ExecuteUpdate("CREATE UNIQUE INDEX IF NOT EXISTS SCHEMA_VERSION_IDX1 ON SCHEMA_VERSION(PROPERTY)");
            m_db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX2 ON COMPILATION_TABLE(FILE_PATH)");
            m_db->ExecuteUpdate("CREATE INDEX IF NOT EXISTS COMPILATION_TABLE_IDX3 ON COMPILATION_TABLE(CWD)");

            wxString versionSql;
            versionSql << "INSERT OR IGNORE INTO SCHEMA_VERSION (PROPERTY, VERSION) VALUES ('Db Version', '" << DB_VERSION
                       << "')";
            m_db->ExecuteUpdate(versionSql);

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::DropTables()
    {
        if(!IsOpened()) return;

        try {

            // Create the schema
            m_db->ExecuteUpdate("DROP TABLE COMPILATION_TABLE");
            m_db->ExecuteUpdate("DROP TABLE SCHEMA_VERSION");

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }
    //-----------------------------------------------------------------------------------
    wxString CompilationDatabase::GetDbVersion()
    {
        if(!IsOpened()) return wxT("");

        try {

            // Create the schema
            wxString sql;
            sql = wxT("SELECT VERSION FROM SCHEMA_VERSION WHERE PROPERTY = 'Db Version' ");
            wxSQLite3Statement st = m_db->PrepareStatement(sql);
            wxSQLite3ResultSet rs = st.ExecuteQuery();

            if(rs.NextRow()) {
                wxString schemaVersion = rs.GetString(0);
                return schemaVersion;
            }

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
        return wxT("");
    }
    //-----------------------------------------------------------------------------------
    bool CompilationDatabase::IsDbVersionUpToDate(const wxFileName& fn)
    {
        try {
            wxString sql;
            wxSQLite3Database db;
            db.Open(fn.GetFullPath());
            sql = "SELECT VERSION FROM SCHEMA_VERSION WHERE PROPERTY = 'Db Version' ";
            wxSQLite3Statement st = db.PrepareStatement(sql);
            wxSQLite3ResultSet rs = st.ExecuteQuery();

            if(rs.NextRow()) { return rs.GetString(0) == DB_VERSION; }
            return false;

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool CompilationDatabase::IsOk() const
    {
        wxFileName fnDb = GetFileName();
        return fnDb.Exists() && IsDbVersionUpToDate(fnDb);
    }
    //-----------------------------------------------------------------------------------
    FileNameVector_t CompilationDatabase::GetCompileCommandsFiles() const
    {
        wxFileName databaseFile(GetFileName());
        wxFileName fn(databaseFile);

        // Usually it will be under the top folder
        fn.RemoveLastDir();

        // Since we can have multiple "compile_commands.json" files, we take the most updated file
        // Prepare a list of files to check
        FileNameVector_t files;
        std::queue<std::pair<wxString, int> > dirs;

        // we start with the current path
        dirs.push(std::make_pair(fn.GetPath(), 0));

        const int MAX_DEPTH = 2; // If no files were found, scan 2 levels only

        while(!dirs.empty()) {
            std::pair<wxString, int> curdir = dirs.front();
            dirs.pop();
            if(files.empty() && (curdir.second > MAX_DEPTH)) {
                gsgs_Debug("Could not find compile_commands.json files while reaching depth 2, aborting");
                break;
            }

            wxFileName fn(curdir.first, "compile_commands.json");
            if(fn.Exists()) {
                gsgs_Debugs("CompilationDatabase: found file: " + fn.GetFullPath());
                files.push_back(fn);
            }

            // Check to see if there are more directories to recurse
            wxDir dir;
            if(dir.Open(curdir.first)) {
                wxString dirname;
                bool cont = dir.GetFirst(&dirname, "", wxDIR_DIRS);
                while(cont) {
                    wxString new_dir;
                    new_dir << curdir.first << gsgsPathSep << dirname;
                    dirs.push(std::make_pair(new_dir, curdir.second + 1));
                    dirname.Clear();
                    cont = dir.GetNext(&dirname);
                }
            }
        }
        return files;
    }
    //-----------------------------------------------------------------------------------
    void CompilationDatabase::ProcessCMakeCompilationDatabase(const wxFileName& compile_commands)
    {
        JSON root(compile_commands);
        JSONObject arr = root.getObject();

        try {

            wxString sql;
            sql = wxT("REPLACE INTO COMPILATION_TABLE (FILE_NAME, FILE_PATH, CWD, COMPILE_FLAGS) VALUES(?, ?, ?, ?)");
            wxSQLite3Statement st = m_db->PrepareStatement(sql);
            m_db->ExecuteUpdate("BEGIN");

            for(int i = 0; i < arr.getArraySize(); ++i) {
                // Each object has 3 properties:
                // directory, command, file
                JSONObject element = arr.getArray(i);
                if(element.isObjectExist("file") && element.isObjectExist("directory") &&
                   element.isObjectExist("command")) {
                    wxString cmd, file, cwd;
                    element.getObject("command").getValue(cmd);
                    element.getObject("file").getValue(file);
                    element.getObject("directory").getValue(cwd);
                    wxString path = wxFileName(file).GetPath();

                    cwd = wxFileName(cwd, "").GetPath();
                    file = wxFileName(file).GetFullPath();

                    st.Bind(1, file);
                    st.Bind(2, path);
                    st.Bind(3, cwd);
                    st.Bind(4, cmd);
                    st.ExecuteUpdate();
                }
            }

            m_db->ExecuteUpdate("COMMIT");

        } catch(wxSQLite3Exception& e) {
            wxUnusedVar(e);
        }
    }
    //-----------------------------------------------------------------------------------
    wxFileName CompilationDatabase::ConvertGSGSCompilationDatabaseToCMake(const wxFileName& compile_file)
    {
        wxFFile fp(compile_file.GetFullPath(), wxT("rb"));
        if(fp.IsOpened()) {
            wxString content;
            fp.ReadAll(&content, wxConvUTF8);

            if(content.IsEmpty()) return wxFileName();

            JSON root(cJSON_Array);
            JSONObject arr = root.getObject();
            wxArrayString lines = ::wxStringTokenize(content, "\n\r", wxTOKEN_STRTOK);
            for(size_t i = 0; i < lines.GetCount(); ++i) {
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), wxT("|"), wxTOKEN_STRTOK);
                if(parts.GetCount() != 3) continue;

                wxString file_name = wxFileName(parts.Item(0).Trim().Trim(false)).GetFullPath();
                wxString cwd = parts.Item(1).Trim().Trim(false);
                wxString cmp_flags = parts.Item(2).Trim().Trim(false);

                JSONObject element = JSONObject::createObject();
                element.addProperty("directory", cwd);
                element.addProperty("command", cmp_flags);
                element.addProperty("file", file_name);
                arr.arrayAppend(element);
            }

            wxFileName fn(compile_file.GetPath(), "compile_commands.json");
            root.save(fn);
            // Delete the old file
            {
                wxLogNull nl;
                fp.Close();
                if(compile_file.Exists()) { gsgsRemoveFile(compile_file.GetFullPath()); }
            }
            return fn;
        }
        return wxFileName();
    }
    //-----------------------------------------------------------------------------------
    wxArrayString CompilationDatabase::FindIncludePaths(const wxString& rootFolder, wxFileName& lastCompileCommands,
                                                        time_t& lastCompileCommandsModified)
    {
        FileNameVector_t files = GetCompileCommandsFiles(rootFolder);
        if(files.empty()) { return wxArrayString(); }
        const wxFileName& compile_commands = files[0]; // we take the first file, which is the most up to date

        // If the last compile_commands.json file was already processed, return an empty array
        if((lastCompileCommands == compile_commands) &&
           (compile_commands.GetModificationTime().GetTicks() == lastCompileCommandsModified)) {
            gsgsDEBUG() << "File" << compile_commands << "already processed. Nothing more to be done here";
            return wxArrayString();
        }

        lastCompileCommands = compile_commands;
        lastCompileCommandsModified = compile_commands.GetModificationTime().GetTicks();

        gsgsStringSet paths;
        JSON root(compile_commands);
        JSONObject arr = root.getObject();
        const int file_size = arr.getArraySize();
        for(int i = 0; i < file_size; ++i) {
            // Each object has 3 properties:
            // directory, command, file
            JSONObject element = arr.getArray(i);
            if(element.isObjectExist("file") && element.isObjectExist("directory") && element.isObjectExist("command")) {
                wxString cmd,cwd;
                element.getObject("command").getValue(cmd);
                element.getObject("directory").getValue(cwd);
                CompilerCmdParser cclp(cmd, cwd);
                const wxArrayString& includes = cclp.GetIncludes();
                std::for_each(includes.begin(), includes.end(),
                              [&](const wxString& includePath) { paths.insert(includePath); });
            }
        }
        // Convert the set back to array
        wxArrayString includePaths;
        std::for_each(paths.begin(), paths.end(), [&](const wxString& path) { includePaths.Add(path); });
        return includePaths;
    }
    //-----------------------------------------------------------------------------------
    FileNameVector_t CompilationDatabase::GetCompileCommandsFiles(const wxString& rootFolder)
    {
        // Since we can have multiple "compile_commands.json" files, we take the most updated file
        // Prepare a list of files to check
        FileNameVector_t files;
        std::queue<std::pair<wxString, int> > dirs;

        // we start with the current path
        dirs.push(std::make_pair(rootFolder, 0));

        const int MAX_DEPTH = 2; // If no files were found, scan 2 levels only

        while(!dirs.empty()) {
            std::pair<wxString, int> curdir = dirs.front();
            dirs.pop();
            if(files.empty() && (curdir.second > MAX_DEPTH)) {
                gsgsDEBUG() << "Could not find compile_commands.json files while reaching depth 2, aborting";
                break;
            }

            wxFileName fn(curdir.first, "compile_commands.json");
            if(fn.FileExists()) {
                gsgsDEBUG() << "CompilationDatabase: found file: " << fn.GetFullPath();
                files.push_back(fn);
            }

            // Check to see if there are more directories to recurse
            wxDir dir;
            if(dir.Open(curdir.first)) {
                wxString dirname;
                bool cont = dir.GetFirst(&dirname, "", wxDIR_DIRS);
                while(cont) {
                    wxString new_dir;
                    new_dir << curdir.first << gsgsPathSep << dirname;
                    dirs.push(std::make_pair(new_dir, curdir.second + 1));
                    dirname.Clear();
                    cont = dir.GetNext(&dirname);
                }
            }
        }
        std::sort(files.begin(), files.end(), [](const wxFileName& one, const wxFileName& two) {
            return one.GetModificationTime() > two.GetModificationTime();
        });
        return files;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // CxxPreProcessorThread
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    CxxPreProcessorThread::CxxPreProcessorThread()
    {
    }
    //-----------------------------------------------------------------------------------
    CxxPreProcessorThread::~CxxPreProcessorThread()
    {
    }
    //-----------------------------------------------------------------------------------
    void CxxPreProcessorThread::ProcessRequest(ThreadRequest* request)
    {
        CxxPreProcessorThread::Request* req = dynamic_cast<CxxPreProcessorThread::Request*>(request);
        CHECK_PTR_RET(req);

        CxxPreProcessor pp;
        for(size_t i = 0; i < req->includePaths.GetCount(); ++i) {
            pp.AddIncludePath(req->includePaths.Item(i));
        }

        for(size_t i = 0; i < req->definitions.GetCount(); ++i) {
            pp.AddDefinition(req->definitions.Item(i));
        }

        gsgs_Debug("Parsing of file: %s started\n", req->filename);
        pp.Parse(req->filename, kLexerOpt_CollectMacroValueNumbers | kLexerOpt_DontCollectMacrosDefinedInThisFile);
        gsgs_Debug("Parsing of file: %s completed\n", req->filename);

        CodeCompletionManager::Get().CallAfter(
            &CodeCompletionManager::OnParseThreadCollectedMacros, pp.GetDefinitions(), req->filename);
    }
    //-----------------------------------------------------------------------------------
    void CxxPreProcessorThread::QueueFile(const wxString& filename,
                                          const wxArrayString& definitions,
                                          const wxArrayString& includePaths)
    {
        CxxPreProcessorThread::Request* req = new CxxPreProcessorThread::Request();
        req->definitions = definitions;
        req->includePaths = includePaths;
        req->filename = filename;
        Add(req);
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // CxxUsingNamespaceCollectorThread
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    CxxUsingNamespaceCollectorThread::CxxUsingNamespaceCollectorThread()
        : WorkerThread()
    {
    }
    //-----------------------------------------------------------------------------------
    CxxUsingNamespaceCollectorThread::~CxxUsingNamespaceCollectorThread() {}
    //-----------------------------------------------------------------------------------
    void CxxUsingNamespaceCollectorThread::ProcessRequest(ThreadRequest* request)
    {
        CxxUsingNamespaceCollectorThread::Request* req = dynamic_cast<CxxUsingNamespaceCollectorThread::Request*>(request);
        CHECK_PTR_RET(req);

        CxxPreProcessor pp;
        pp.SetMaxDepth(20);
        CxxUsingNamespaceCollector collector(&pp, req->filename);
        for(size_t i = 0; i < req->includePaths.GetCount(); ++i) {
            pp.AddIncludePath(req->includePaths.Item(i));
        }

        gsgs_Debug("Collecting 'using namespace' statements for file '%s' started\n", req->filename);
        collector.Parse();
        gsgs_Debug("Collecting 'using namespace' statements for file '%s' completed\n", req->filename);

        CodeCompletionManager::Get().CallAfter(
            &CodeCompletionManager::OnFindUsingNamespaceDone, collector.GetUsingNamespaces(), req->filename);
    }
    //-----------------------------------------------------------------------------------
    void CxxUsingNamespaceCollectorThread::QueueFile(const wxString& filename,
                                                     const wxArrayString& searchPaths)
    {
        CxxUsingNamespaceCollectorThread::Request* req = new CxxUsingNamespaceCollectorThread::Request();
        req->filename = filename;
        req->includePaths = searchPaths;
        Add(req);
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // CxxPreProcessorCache
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    CxxPreProcessorCache::CxxPreProcessorCache() {}
    //-----------------------------------------------------------------------------------
    CxxPreProcessorCache::~CxxPreProcessorCache() {}
    //-----------------------------------------------------------------------------------
    bool CxxPreProcessorCache::Find(const wxString& filename,
                                    const wxString& project,
                                    const wxString& config,
                                    wxArrayString& definitions)
    {
        wxString key;
        key << project << "|" << config << "|" << filename;
        CxxPreProcessorCache::Map_t::iterator iter = m_impl.find(key);
        if(iter == m_impl.end()) return false;

        if(wxFileName(filename).GetModificationTime().GetTicks() > iter->second.lastUpdated) {
            // the file file was saved since we last cached it
            // Next:
            // Check if the cached entry preabmle is the same as the current file
            wxString currentPreamble = GetPreamble(filename);
            wxString cachedPreamble = iter->second.preamble;
            if(cachedPreamble == currentPreamble) {
                // no change was done to the preamble
                definitions = iter->second.definitions;
                return true;
            } else {
                // preamble was modified - we need to re-cache it
                // remove the current cache and return false
                m_impl.erase(iter);
                return false;
            }
        } else {
            // the file was not saved since we cached it
            definitions = iter->second.definitions;
            return false;
        }
    }
    //-----------------------------------------------------------------------------------
    void CxxPreProcessorCache::Clear() { m_impl.clear(); }
    //-----------------------------------------------------------------------------------
    wxString CxxPreProcessorCache::GetPreamble(const wxString& filename) const
    {
        Scanner_t scanner = ::LexerNew(filename, 0);
        if(!scanner) return { "" };

        CxxLexerToken token;
        wxString preamble;
        while(::LexerNext(scanner, token)) {
            if(token.GetType() == T_PP_INCLUDE_FILENAME) {
                preamble << token.GetText() << "\n";
            }
        }
        ::LexerDestroy(&scanner);

        preamble.Trim();
        return preamble;
    }
    //-----------------------------------------------------------------------------------
    bool CxxPreProcessorCache::Insert(const wxString& filename,
                                      const wxString& project,
                                      const wxString& config,
                                      const wxArrayString& definitions)
    {
        wxString key;
        key << project << "|" << config << "|" << filename;

        // remove old entry
        CxxPreProcessorCache::Map_t::iterator iter = m_impl.find(key);
        if(iter != m_impl.end()) m_impl.erase(iter);
        
        CacheEntry entry;
        entry.config = config;
        entry.definitions = definitions;
        entry.lastUpdated = time(NULL); // Now
        entry.filename = filename;
        entry.project = project;
        entry.preamble = GetPreamble(filename);
        m_impl.insert(std::make_pair(key, entry));
        return true;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // CodeCompletionManager
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    static CodeCompletionManager* ms_CodeCompletionManager = NULL;
    //-----------------------------------------------------------------------------------
    struct EditorDimmerDisabler {
        Editor* m_editor;
        EditorDimmerDisabler(Editor* editor)
            : m_editor(editor)
        {
            if(m_editor) {
                m_editor->SetPreProcessorsWords("");
                m_editor->GetCtrl()->SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("0"));
                m_editor->GetCtrl()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("0"));
                m_editor->GetCtrl()->Colourise(0, wxSTC_INVALID_POSITION);
            }
        }
        ~EditorDimmerDisabler() {}
    };
    //-----------------------------------------------------------------------------------
    CodeCompletionManager::CodeCompletionManager()
        : ServiceProvider("BuiltIn C++ Code Completion", eServiceType::kCodeCompletion)
        , m_options(CC_CTAGS_ENABLED)
        , m_wordCompletionRefreshNeeded(false)
        , m_buildInProgress(false)
    {
        SetPriority(75);
        gsgs_Event().Connect(wxEVT_BUILD_STARTED, gsgsBuildEventHandler(CodeCompletionManager::OnBuildStarted), NULL, this);
        gsgs_Event().Bind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED, &CodeCompletionManager::OnCompileCommandsFileGenerated, this);

        gsgs_Event().Connect(wxEVT_FILE_SAVED, gsgsEventHandler(CodeCompletionManager::OnFileSaved), NULL, this);
        gsgs_Event().Connect(wxEVT_FILE_LOADED, gsgsEventHandler(CodeCompletionManager::OnFileLoaded), NULL, this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_CONFIG_CHANGED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
        gsgs_Event().Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED, wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
        wxTheApp->Bind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);

        gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED,
                                      wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
        gsgs_Event().Bind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                                   &CodeCompletionManager::OnEnvironmentVariablesModified, this);
        gsgs_Event().Bind(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE, &CodeCompletionManager::OnBlockCommentCodeComplete,
                                   this);
        gsgs_Event().Bind(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE, &CodeCompletionManager::OnBlockCommentWordComplete,
                                   this);
        gsgs_Event().Bind(wxEVT_BUILD_ENDED, &CodeCompletionManager::OnBuildEnded, this);
        gsgs_Event().Bind(wxEVT_PROJ_FILE_ADDED, &CodeCompletionManager::OnFilesAdded, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &CodeCompletionManager::OnWorkspaceLoaded, this);

        // Connect ourself to the cc event system
        Bind(wxEVT_CC_CODE_COMPLETE, &CodeCompletionManager::OnCodeCompletion, this);
        Bind(wxEVT_CC_FIND_SYMBOL, &CodeCompletionManager::OnFindSymbol, this);
        Bind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &CodeCompletionManager::OnFindDecl, this);
        Bind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &CodeCompletionManager::OnFindImpl, this);
        Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &CodeCompletionManager::OnFunctionCalltip, this);
        Bind(wxEVT_CC_TYPEINFO_TIP, &CodeCompletionManager::OnTypeInfoToolTip, this);

        // Start the worker threads
        m_preProcessorThread.Start();
        m_usingNamespaceThread.Start();
        m_compileCommandsGenerator.reset(new CompileCommandsGenerator());
    }
    //-----------------------------------------------------------------------------------
    CodeCompletionManager::~CodeCompletionManager()
    {
        m_preProcessorThread.Stop();
        m_usingNamespaceThread.Stop();
        gsgs_Event().Unbind(wxEVT_PROJ_FILE_ADDED, &CodeCompletionManager::OnFilesAdded, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &CodeCompletionManager::OnWorkspaceLoaded, this);
        gsgs_Event().Unbind(wxEVT_CC_BLOCK_COMMENT_CODE_COMPLETE,
                                     &CodeCompletionManager::OnBlockCommentCodeComplete, this);
        gsgs_Event().Unbind(wxEVT_CC_BLOCK_COMMENT_WORD_COMPLETE,
                                     &CodeCompletionManager::OnBlockCommentWordComplete, this);

        gsgs_Event().Unbind(wxEVT_BUILD_ENDED, &CodeCompletionManager::OnBuildEnded, this);
        gsgs_Event().Disconnect(wxEVT_BUILD_STARTED, gsgsBuildEventHandler(CodeCompletionManager::OnBuildStarted),
                                         NULL, this);
        gsgs_Event().Unbind(wxEVT_COMPILE_COMMANDS_JSON_GENERATED,
                                     &CodeCompletionManager::OnCompileCommandsFileGenerated, this);
        gsgs_Event().Disconnect(wxEVT_FILE_SAVED, gsgsEventHandler(CodeCompletionManager::OnFileSaved), NULL,
                                         this);
        gsgs_Event().Disconnect(wxEVT_FILE_LOADED, gsgsEventHandler(CodeCompletionManager::OnFileLoaded),
                                         NULL, this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_CONFIG_CHANGED,
                                         wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
        gsgs_Event().Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                         wxCommandEventHandler(CodeCompletionManager::OnWorkspaceConfig), NULL, this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_CLOSED,
                                         wxCommandEventHandler(CodeCompletionManager::OnWorkspaceClosed), NULL, this);
        wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &CodeCompletionManager::OnAppActivated, this);
        gsgs_Event().Unbind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED,
                                     &CodeCompletionManager::OnEnvironmentVariablesModified, this);

        Unbind(wxEVT_CC_CODE_COMPLETE, &CodeCompletionManager::OnCodeCompletion, this);
        Unbind(wxEVT_CC_FIND_SYMBOL, &CodeCompletionManager::OnFindSymbol, this);
        Unbind(wxEVT_CC_FIND_SYMBOL_DECLARATION, &CodeCompletionManager::OnFindDecl, this);
        Unbind(wxEVT_CC_FIND_SYMBOL_DEFINITION, &CodeCompletionManager::OnFindImpl, this);
        Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &CodeCompletionManager::OnFunctionCalltip, this);
        Unbind(wxEVT_CC_TYPEINFO_TIP, &CodeCompletionManager::OnTypeInfoToolTip, this);

        if(m_compileCommandsThread) {
            m_compileCommandsThread->join();
            wxDELETE(m_compileCommandsThread);
        }
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::WordCompletion(Editor* editor, const wxString& expr, const wxString& word)
    {
        wxString expression = expr;
        wxString tmp;

        DoUpdateOptions();

        // Trim whitespace from right and left
        static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

        expression = expression.erase(0, expression.find_first_not_of(trimString));
        expression = expression.erase(expression.find_last_not_of(trimString) + 1);

        if(expression.EndsWith(word, &tmp)) { expression = tmp; }
        return DoCtagsWordCompletion(editor, expr, word);
    }
    //-----------------------------------------------------------------------------------
    CodeCompletionManager& CodeCompletionManager::Get()
    {
        if(!ms_CodeCompletionManager) { ms_CodeCompletionManager = new CodeCompletionManager; }
        return *ms_CodeCompletionManager;
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::DoCtagsWordCompletion(Editor* editor, const wxString& expr, const wxString& word)
    {
        std::vector<TagEntryPtr> candidates;
        // get the full text of the current page
        wxString text = editor->GetTextRange(0, editor->GetCurrentPosition());
        int lineNum = editor->LineFromPosition(editor->GetCurrentPosition()) + 1;

        if(TagsManagerST::Get()->WordCompletionCandidates(editor->GetFileName(), lineNum, expr, text, word, candidates) &&
           !candidates.empty()) {
            editor->ShowCompletionBox(candidates, word);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::DoCtagsCalltip(Editor* editor, int line, const wxString& expr, const wxString& text,
                                               const wxString& word)
    {
        // Get the calltip
        gsgsTipInfoPtr tip = TagsManagerST::Get()->GetFunctionTip(editor->GetFileName(), line, expr, text, word);
        if(!tip || !tip->Count()) {
            // try the Clang engine...
            return false;
        }
        editor->ShowCalltip(tip);
        return true;
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::Calltip(Editor* editor, int line, const wxString& expr, const wxString& text,
                                        const wxString& word)
    {
        DoUpdateOptions();
        if(::IsCppKeyword(word)) return false;
        return DoCtagsCalltip(editor, line, expr, text, word);
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::CodeComplete(Editor* editor, int line, const wxString& expr, const wxString& text)
    {
        DoUpdateOptions();
        return DoCtagsCodeComplete(editor, line, expr, text);
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::DoCtagsCodeComplete(Editor* editor, int line, const wxString& expr, const wxString& text)
    {
        std::vector<TagEntryPtr> candidates;
        if(TagsManagerST::Get()->AutoCompleteCandidates(editor->GetFileName(), line, expr, text, candidates) &&
           !candidates.empty()) {
            editor->ShowCompletionBox(candidates, wxEmptyString);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::DoUpdateOptions()
    {
        const TagsOptionsData& options = TagsManagerST::Get()->GetCtagsOptions();
        m_options = options.GetClangOptions();

        m_options |= CC_CTAGS_ENABLED; // For now, we always enables CTAGS

        // Incase CLANG is set as the main CC engine, remove the CTAGS options BUT
        // only if CLANG is enabled...
        if((m_options & CC_CLANG_FIRST) && (m_options & CC_CLANG_ENABLED)) { m_options &= ~CC_CTAGS_ENABLED; }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::ProcessMacros(Editor* editor)
    {
        // Sanity
        CHECK_PTR_RET(editor);

        /// disable the editor pre-processor dimming
        EditorDimmerDisabler eds(editor);

        wxArrayString macros;
        wxArrayString includePaths;
        if(!GetDefinitionsAndSearchPaths(editor, includePaths, macros)) return;

        // Queue this request in the worker thread
        m_preProcessorThread.QueueFile(editor->GetFileName().GetFullPath(), macros, includePaths);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::GotoImpl(Editor* editor)
    {
        DoUpdateOptions();

        // Let the plugins handle this first
        CodeCompleteEvent event(wxEVT_CC_FIND_SYMBOL_DEFINITION);
        event.SetEditor(editor);
        gsgs::ServiceManager::Get().ProcessEvent(event);
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::DoCtagsGotoImpl(Editor* editor)
    {
        TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, true);
        if(tag) {
            Editor* editor =
                gsgs_Frame()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
            if(!editor) { return false; }
            // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
            // EnsureVisible code is called too early and fails
            editor->FindAndSelectV(tag->GetPattern(), tag->GetName());
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::DoCtagsGotoDecl(Editor* editor)
    {
        TagEntryPtr tag = editor->GetContext()->GetTagAtCaret(true, false);
        if(tag) {
            Editor* editor =
                gsgs_Frame()->GetMainBook()->OpenFile(tag->GetFile(), wxEmptyString, tag->GetLine() - 1);
            if(!editor) { return false; }
            // Use the async funtion here. Synchronously usually works but, if the file wasn't loaded, sometimes the
            // EnsureVisible code is called too early and fails
            if(!editor->FindAndSelect(tag->GetPattern(), tag->GetName())) {
                editor->SetCaretAt(editor->PosFromLine(tag->GetLine() - 1));
                editor->CenterLineIfNeeded(tag->GetLine() - 1);
            }
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::GotoDecl(Editor* editor)
    {
        DoUpdateOptions();
        CodeCompleteEvent event(wxEVT_CC_FIND_SYMBOL_DECLARATION);
        event.SetEditor(editor);
        gsgs::ServiceManager::Get().ProcessEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnBuildEnded(gsgs::BuildEvent& e)
    {
        e.Skip();
        m_compileCommandsGenerator->GenerateCompileCommands();
        m_buildInProgress = false;
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnAppActivated(wxActivateEvent& e) { e.Skip(); }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::Release() { wxDELETE(ms_CodeCompletionManager); }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnBuildStarted(gsgs::BuildEvent& e)
    {
        e.Skip();
        m_buildInProgress = true;
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnCompileCommandsFileGenerated(gsgs::Event& event)
    {
        event.Skip();
        gsgsDEBUG() << "-- Code Completion Manager: process file" << event.GetFileName();
        this->CompileCommandsFileProcessed(event.GetStrings());
        gsgs_Frame()->SetStatusText("Ready");
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnParseThreadCollectedMacros(const wxArrayString& definitions, const wxString& filename)
    {
        gsgs_Debug("Parser thread returned %d macros\n", (int)definitions.GetCount());
        // We got a list of macros from the parser thead
        // prepare a space delimited list out of it
        wxString macrosAsString;
        for(size_t i = 0; i < definitions.GetCount(); ++i) {
            // gsgs_Debug("%s\n", definitions.Item(i));
            macrosAsString << definitions.Item(i) << " ";
        }
        Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(filename);
        if(editor) {
            gsgs_Debug("Updating editor colours...");
            // its the same file that triggered the request, update its pre processor colouring
            // turn off the macro colouring (until new set is arrived)
            editor->SetPreProcessorsWords(macrosAsString);
            editor->GetCtrl()->SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("1"));
            editor->GetCtrl()->SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("1"));
            editor->GetCtrl()->SetKeyWords(4, macrosAsString);
            editor->GetCtrl()->Colourise(0, wxSTC_INVALID_POSITION);
            gsgs_Debug("Updating editor colours...done");
        }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFileSaved(gsgs::Event& event)
    {
        event.Skip();
        if(TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS) {
            ProcessMacros(gsgs_Frame()->GetMainBook()->FindEditor(event.GetFileName()));
        }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFileLoaded(gsgs::Event& event)
    {
        event.Skip();
        Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(event.GetFileName());
        CHECK_PTR_RET(editor);

        // Handle Pre Processor block colouring
        const size_t colourOptions = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags();
        const size_t ccFlags = TagsManagerST::Get()->GetCtagsOptions().GetFlags();
        if(!(colourOptions & CC_COLOUR_MACRO_BLOCKS)) {
            editor->SetPreProcessorsWords("");
            editor->SetProperty("lexer.cpp.track.preprocessor", "0");
            editor->SetProperty("lexer.cpp.update.preprocessor", "0");
        } else {
            ProcessMacros(editor);
        }

        if(editor && (ccFlags & CC_DEEP_SCAN_USING_NAMESPACE_RESOLVING)) { ProcessUsingNamespace(editor); }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::RefreshPreProcessorColouring()
    {
        bool enableBlockColouring = TagsManagerST::Get()->GetCtagsOptions().GetCcColourFlags() & CC_COLOUR_MACRO_BLOCKS;
        EditorVec_t editors;
        gsgs_Frame()->GetMainBook()->GetAllEditors(editors, gsgs::Book::kGetAll_IncludeDetached);
        if(!enableBlockColouring) {
            for(size_t i = 0; i < editors.size(); ++i) {
                Editor* editor = editors.at(i);
                editor->SetPreProcessorsWords("");
                editor->SetProperty("lexer.cpp.track.preprocessor", "0");
                editor->SetProperty("lexer.cpp.update.preprocessor", "0");
            }
        } else {
            for(size_t i = 0; i < editors.size(); ++i) {
                Editor* editor = editors.at(i);
                editor->SetProperty("lexer.cpp.track.preprocessor", "0");
                editor->SetProperty("lexer.cpp.update.preprocessor", "0");
                ProcessMacros(editor);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnWorkspaceConfig(wxCommandEvent& event)
    {
        event.Skip();
        if(gsgs_CppWorker().IsOpen()) { gsgs_Workspace().ClearBacktickCache(); }
        RefreshPreProcessorColouring();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFindUsingNamespaceDone(const wxArrayString& usingNamespace, const wxString& filename)
    {
        gsgs_Debug("OnFindUsingNamespaceDone called");

        gsgs_Debug("Found the following 'using namespace' statements for file %s", filename);
        gsgs_DEBUG_ARR(usingNamespace);

        // We got a list of macros from the parser thead
        // prepare a space delimited list out of it
        std::vector<wxString> additionalScopes;
        additionalScopes.insert(additionalScopes.end(), usingNamespace.begin(), usingNamespace.end());

        LanguageST::Get()->UpdateAdditionalScopesCache(filename, additionalScopes);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::ProcessUsingNamespace(Editor* editor)
    {
        // Sanity
        CHECK_PTR_RET(editor);

        /// disable the editor pre-processor dimming
        EditorDimmerDisabler eds(editor);

        wxArrayString macros;
        wxArrayString includePaths;
        if(!GetDefinitionsAndSearchPaths(editor, includePaths, macros)) return;

        wxUnusedVar(macros);
        // Queue this request in the worker thread
        m_usingNamespaceThread.QueueFile(editor->GetFileName().GetFullPath(), includePaths);
    }
    //-----------------------------------------------------------------------------------
    bool CodeCompletionManager::GetDefinitionsAndSearchPaths(Editor* editor, wxArrayString& searchPaths,
                                                             wxArrayString& definitions)
    {
        // Sanity
        CHECK_PTR_RET_FALSE(editor);

        if(editor->GetProjectName().IsEmpty()) return false;
        if(!gsgs_CppWorker().IsOpen()) return false;

        // Support only C/C++ files
        if(!FileUtil::IsCxxFile(editor->GetFileName().GetFullName())) return false;

        // Get the file's project and get the build configuration settings
        // for it
        ProjectPtr proj = gsgs_CppWorker().GetProject(editor->GetProjectName());
        CHECK_PTR_RET_FALSE(proj);

        BuildConfigPtr buildConf = proj->GetBuildConfiguration();
        CHECK_PTR_RET_FALSE(buildConf);

        CompilerPtr compiler = buildConf->GetCompiler();
        CHECK_PTR_RET_FALSE(compiler);

    #if 0
        if(buildConf->IsCustomBuild()) {
            definitions = proj->GetPreProcessors();
            gsgs_Debug("CxxPreProcessor will use the following macros:");
            gsgs_DEBUG_ARR(definitions);
            // Custom builds are handled differently
            CompilationDatabase compileDb;
            compileDb.Open();
            if(compileDb.IsOpened()) {
                // we have compilation database for this workspace
                wxString compileLine, cwd;
                compileDb.CompilationLine(editor->GetFileName().GetFullPath(), compileLine, cwd);

                gsgs_Debug("Pre Processor dimming: %s\n", compileLine);
                CompilerCmdParser cclp(compileLine, cwd);
                searchPaths = cclp.GetIncludes();

                // get the mcros
                definitions << cclp.GetMacros();
            }
        }
    #endif
        // get the include paths based on the project settings (this is per build configuration)
        searchPaths = proj->GetIncludePaths();
        gsgs_Debug("CxxPreProcessor will use the following include paths:");
        gsgs_DEBUG_ARR(searchPaths);

        // get the compiler include paths
        // wxArrayString compileIncludePaths = compiler->GetDefaultIncludePaths();

        // includePaths.insert(includePaths.end(), compileIncludePaths.begin(), compileIncludePaths.end());
        definitions = proj->GetPreProcessors();

        // get macros out of workspace
        wxString strWorkspaceMacros = gsgs_CppWorker().GetParserMacros();
        wxArrayString workspaceMacros = wxStringTokenize(strWorkspaceMacros, wxT("\n\r"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < workspaceMacros.GetCount(); i++)
            definitions.Add(workspaceMacros.Item(i).Trim().Trim(false).c_str());

        gsgs_Debug("CxxPreProcessor will use the following macros:");
        gsgs_DEBUG_ARR(definitions);

        // Append the compiler builtin macros
        wxArrayString builtinMacros = compiler->GetBuiltinMacros();
        definitions.insert(definitions.end(), builtinMacros.begin(), builtinMacros.end());

        return true;
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnWorkspaceClosed(wxCommandEvent& event)
    {
        event.Skip();
        LanguageST::Get()->ClearAdditionalScopesCache();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnEnvironmentVariablesModified(gsgs::Event& event)
    {
        event.Skip();
        if(gsgs_CppWorker().IsOpen()) { gsgs_Workspace().ClearBacktickCache(); }
        RefreshPreProcessorColouring();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::DoProcessCompileCommands()
    {
        // return; // for now, dont use it
        if(m_compileCommandsThread) { return; }

        // Create a thread that will process the current workspace folder and search for any compile_commands.json file
        m_compileCommandsThread = new std::thread(&CodeCompletionManager::ThreadProcessCompileCommandsEntry, this,
                                                  gsgs_CppWorker().GetFileName().GetPath());
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::ThreadProcessCompileCommandsEntry(CodeCompletionManager* owner, const wxString& rootFolder)
    {
        // Search for compile_commands file, process it and send back the results to the main thread
        wxArrayString includePaths = CompilationDatabase::FindIncludePaths(rootFolder, owner->m_compileCommands,
                                                                           owner->m_compileCommandsLastModified);
        owner->CallAfter(&CodeCompletionManager::CompileCommandsFileProcessed, includePaths);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::CompileCommandsFileProcessed(const wxArrayString& includePaths)
    {
        if(m_compileCommandsThread) {
            m_compileCommandsThread->join();
            wxDELETE(m_compileCommandsThread);
        }
        if(includePaths.IsEmpty()) { return; }
        ParseThreadST::Get()->AddPaths(includePaths, {});
        
        // Update the parser search paths
        wxArrayString inc, exc;
        ParseThreadST::Get()->GetSearchPaths(inc, exc);
        gsgsDEBUG() << "Parser thread search paths are now updated to:" << inc;

        // Trigger a quick parse
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnBlockCommentCodeComplete(CodeCompleteEvent& event)
    {
        event.Skip();
        wxStyledTextCtrl* ctrl = GetManager()->GetActiveEditor()->GetCtrl();
        CHECK_PTR_RET(ctrl);

        wxCodeCompletionBoxEntry::Vec_t entries;
        if(CreateBlockCommentKeywordsList(entries) == 0) { return; }
        CodeBox::BmpVec_t bitmaps;
        bitmaps.push_back(gsgs_Image().LoadBitmap("cpp_keyword"));

        int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
        CodeBoxManager::Get().ShowCompletionBox(ctrl, entries, bitmaps, CodeBox::kRefreshOnKeyType,
                                                            startPos);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnBlockCommentWordComplete(CodeCompleteEvent& event)
    {
        event.Skip();
        wxStyledTextCtrl* ctrl = GetManager()->GetActiveEditor()->GetCtrl();
        CHECK_PTR_RET(ctrl);

        wxCodeCompletionBoxEntry::Vec_t entries;
        if(CreateBlockCommentKeywordsList(entries) == 0) { return; }
        CodeBox::BmpVec_t bitmaps;
        bitmaps.push_back(gsgs_Image().LoadBitmap("cpp_keyword"));

        int startPos = ctrl->WordStartPosition(ctrl->GetCurrentPos(), true);
        CodeBoxManager::Get().ShowCompletionBox(ctrl, entries, bitmaps, CodeBox::kRefreshOnKeyType,
                                                            startPos);
    }
    //-----------------------------------------------------------------------------------
    size_t CodeCompletionManager::CreateBlockCommentKeywordsList(wxCodeCompletionBoxEntry::Vec_t& entries) const
    {
        entries.clear();
        std::vector<wxString> keywords = { "api",        "author",   "brief",         "category",       "copyright",
                                           "deprecated", "example",  "filesource",    "global",         "ignore",
                                           "internal",   "license",  "link",          "method",         "package",
                                           "param",      "property", "property-read", "property-write", "return",
                                           "see",        "since",    "source",        "subpackage",     "throws",
                                           "todo",       "uses",     "var",           "version" };
        std::for_each(keywords.begin(), keywords.end(),
                      [&](const wxString& keyword) { entries.push_back(wxCodeCompletionBoxEntry::New(keyword, 0)); });
        return entries.size();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::UpdateParserPaths()
    {
        if(gsgs_CppWorker().IsOpen()) { DoProcessCompileCommands(); }
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFilesAdded(gsgs::Event& e)
    {
        e.Skip();
        m_compileCommandsGenerator->GenerateCompileCommands();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        m_compileCommandsGenerator->GenerateCompileCommands();
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnCodeCompletion(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor(true);
        CHECK_PTR_RET(editor);

        // This class only handles C++/C code completion
        if(!FileUtil::IsCxxFile(editor->GetFileName())) { return; }

        // Try to code complete
        bool completionSucceed = event.GetTriggerKind() == LSP::CompletionItem::kTriggerCharacter
                                     ? editor->GetContext()->CodeComplete()
                                     : editor->GetContext()->CompleteWord();

        // Skip the event if we managed to process
        event.Skip(!completionSucceed);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFindSymbol(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor(true);
        CHECK_PTR_RET(editor);

        // This class only handles C++/C code completion
        if(!FileUtil::IsCxxFile(editor->GetFileName())) { return; }

        // Try to code complete
        bool completionSucceed = editor->GetContext()->GotoDefinition();

        // Skip the event if we managed to process
        event.Skip(!completionSucceed);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnWordCompletion(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = dynamic_cast<Editor*>(event.GetEditor());
        CHECK_PTR_RET(editor);

        // This class only handles C++/C code completion
        if(!FileUtil::IsCxxFile(editor->GetFileName())) { return; }

        // Dont attempt to code complete when inside comment blocks
        if(editor->GetContext()->IsCommentOrString(editor->GetCurrentPos())) {
            event.Skip(false);
            return;
        }

        // Try to code complete
        bool completionSucceed = editor->GetContext()->CompleteWord();

        // Skip the event if we managed to process
        event.Skip(!completionSucceed);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFindDecl(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = dynamic_cast<Editor*>(event.GetEditor());
        bool res = editor && FileUtil::IsCxxFile(editor->GetFileName()) && DoCtagsGotoDecl(editor);
        event.Skip(!res);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFindImpl(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = dynamic_cast<Editor*>(event.GetEditor());
        bool res = editor && FileUtil::IsCxxFile(editor->GetFileName()) && DoCtagsGotoImpl(editor);
        event.Skip(!res);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnFunctionCalltip(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = dynamic_cast<Editor*>(event.GetEditor());
        bool res = editor && FileUtil::IsCxxFile(editor->GetFileName()) &&
                   editor->GetContext()->CodeComplete(event.GetPosition());
        event.Skip(!res);
    }
    //-----------------------------------------------------------------------------------
    void CodeCompletionManager::OnTypeInfoToolTip(CodeCompleteEvent& event)
    {
        event.Skip();
        Editor* editor = dynamic_cast<Editor*>(event.GetEditor());
        bool res = editor && FileUtil::IsCxxFile(editor->GetFileName()) &&
                   editor->GetContext()->GetHoverTip(event.GetPosition());
        event.Skip(!res);
    }
    //-----------------------------------------------------------------------------------
}