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
#include "gsgsConsole.h"
#include "gsgsConfig.h"
#include "gsgsPathManager.h"
#include "gsgsFileUtil.h"
#include "gsgsCommon.h"
#include "gsgsLogManager.h"
#include "gsgsProcess.h"
#include "gsgsJSON.h"
#include <wx/xml/xml.h>
#include <wx/thread.h>
#include "wx/menu.h"
#include <fstream>
#ifdef __WXGTK__
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <cstdio>
#include <cctype>

#ifdef __WXMSW__
#define FC_PATH_SEP "\\"
#else
#define FC_PATH_SEP "/"
#endif
namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FileHistory
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FileHistory::FileHistory()
    {
    }
    //-----------------------------------------------------------------------------------
    FileHistory::~FileHistory()
    {
    }
    //-----------------------------------------------------------------------------------
    void FileHistory::GetFiles(wxArrayString &files)
    {
        for(size_t i=0; i<GetCount(); i++) 
        {
            files.Add(m_fileHistory[i]);
        }
    }
    //-----------------------------------------------------------------------------------
    void FileHistory::AddFilesToMenuWithoutExt()
    {
        if ( m_fileHistory.empty() )
            return;

        for ( wxList::compatibility_iterator node = m_fileMenus.GetFirst();
              node; node = node->GetNext() ) {
            AddFilesToMenuWithoutExt((wxMenu *) node->GetData());
        }
    }
    //-----------------------------------------------------------------------------------
    // return the string used for the MRU list items in the menu
    //
    // NB: the index n is 0-based, as usual, but the strings start from 1
    wxString GetMRUEntryLabel(int n, const wxString& path)
    {
        // we need to quote '&' characters which are used for mnemonics
        wxString pathInMenu(path);
        pathInMenu.Replace(wxT("&"), "&&");

        return wxString::Format("&%d %s", n + 1, pathInMenu);
    }
    //-----------------------------------------------------------------------------------
    void FileHistory::AddFilesToMenuWithoutExt(wxMenu* menu)
    {
        if ( m_fileHistory.empty() )
            return;

        if ( menu->GetMenuItemCount() )
            menu->AppendSeparator();

        for ( size_t i = 0; i < m_fileHistory.GetCount(); i++ ) {
            wxString label = GetMRUEntryLabel(i, m_fileHistory[i]);
            menu->Append(GetBaseId() + i, label);
        }
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // gsgs::FileEvent
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FileEvent::FileEvent(wxEventType commandType, int winid)
        : Event(commandType, winid)
    {
    }
    //-----------------------------------------------------------------------------------
    FileEvent::~FileEvent()
    {
    }
    //-----------------------------------------------------------------------------------
    FileEvent::FileEvent(const FileEvent& event)
        : Event(event)
    {
        *this = event;
    }
    //-----------------------------------------------------------------------------------
    FileEvent& FileEvent::operator=(const FileEvent& src)
    {
        Event::operator=(src);

        // gsgs::FileEvent specifics
        m_path = src.m_path;
        m_newpath = src.m_newpath;
        m_paths = src.m_paths;
        return *this;
    }
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_FILE_MODIFIED, FileEvent);
    wxDEFINE_EVENT(wxEVT_FILE_NOT_FOUND, FileEvent);
    //-----------------------------------------------------------------------------------
    // In milliseconds
    #define FILE_CHECK_INTERVAL 500
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FileWatch
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FileWatch::FileWatch()
        : m_owner(NULL)
    #if CL_FSW_USE_TIMER
        , m_timer(NULL)
    #endif
    {
    #if CL_FSW_USE_TIMER
        Bind(wxEVT_TIMER, &FileWatch::OnTimer, this);
    #else
        m_watcher.SetOwner(this);
        Bind(wxEVT_FSWATCHER, &FileWatch::OnFileModified, this);
    #endif
    }
    //-----------------------------------------------------------------------------------
    FileWatch::~FileWatch()
    {
    #if CL_FSW_USE_TIMER
        Stop();
        Unbind(wxEVT_TIMER, &FileWatch::OnTimer, this);
    #else
        m_watcher.RemoveAll();
        Unbind(wxEVT_FSWATCHER, &FileWatch::OnFileModified, this);
    #endif
    }
    //-----------------------------------------------------------------------------------
    void FileWatch::SetFile(const wxFileName& filename)
    {
    #if CL_FSW_USE_TIMER
        if(filename.Exists()) {
            m_files.clear();
            File f;
            f.filename = filename;
            f.lastModified = FileUtil::GetFileModificationTime(filename);
            f.file_size = FileUtil::GetFileSize(filename);
            m_files.insert(std::make_pair(filename.GetFullPath(), f));
        }
    #else
        m_watcher.RemoveAll();
        wxFileName dironly = filename;
        dironly.SetFullName(""); // must be directory only
        m_watcher.Add(dironly);
        m_watchedFile = filename; // but we keep the file name for later usage
    #endif
    }
    //-----------------------------------------------------------------------------------
    void FileWatch::Start()
    {
    #if CL_FSW_USE_TIMER
        Stop();

        m_timer = new wxTimer(this);
        m_timer->Start(FILE_CHECK_INTERVAL, true);
    #else
    #endif
    }
    //-----------------------------------------------------------------------------------
    void FileWatch::Stop()
    {
    #if CL_FSW_USE_TIMER
        if(m_timer) {
            m_timer->Stop();
        }
        wxDELETE(m_timer);
    #else
        m_watcher.RemoveAll();
    #endif
    }
    //-----------------------------------------------------------------------------------
    void FileWatch::Clear()
    {
    #if CL_FSW_USE_TIMER
        Stop();
        m_files.clear();
    #else
        m_watcher.RemoveAll();
    #endif
    }
    //-----------------------------------------------------------------------------------
    #if CL_FSW_USE_TIMER
    void FileWatch::OnTimer(wxTimerEvent& event)
    {
        std::set<wxString> nonExistingFiles;
        std::for_each(m_files.begin(), m_files.end(), [&](const std::pair<wxString, FileWatch::File>& p) {
            const File& f = p.second;
            const wxFileName& fn = f.filename;
            if(!fn.Exists()) {

                // fire file not found event
                if(GetOwner()) {
                    gsgs::FileEvent evt(wxEVT_FILE_NOT_FOUND);
                    evt.SetPath(fn.GetFullPath());
                    GetOwner()->AddPendingEvent(evt);
                }

                // add the missing file to a set
                nonExistingFiles.insert(fn.GetFullPath());
            } else {
                
    #ifdef __WXMSW__
                size_t prev_value = f.file_size;
                size_t curr_value = FileUtil::GetFileSize(fn);
    #else
                time_t prev_value = f.lastModified;
                time_t curr_value = FileUtil::GetFileModificationTime(fn);
    #endif

                if(prev_value != curr_value) {
                    // Fire a modified event
                    if(GetOwner()) {
                        gsgs::FileEvent evt(wxEVT_FILE_MODIFIED);
                        evt.SetPath(fn.GetFullPath());
                        GetOwner()->AddPendingEvent(evt);
                    }
                }
    #ifdef __WXMSW__
                File updatdFile = f;
                updatdFile.file_size = curr_value;
    #else
                // Always update the last modified timestamp
                File updatdFile = f;
                updatdFile.lastModified = curr_value;
    #endif
                m_files[fn.GetFullPath()] = updatdFile;
            }
        });

        // Remove the non existing files
        std::for_each(nonExistingFiles.begin(), nonExistingFiles.end(), [&](const wxString& fn) { m_files.erase(fn); });

        if(m_timer) {
            m_timer->Start(FILE_CHECK_INTERVAL, true);
        }
    }
    //#endif

    #else // !CL_FSW_USE_TIMER
    void FileWatch::OnFileModified(wxFileSystemWatcherEvent& event)
    {
        if(event.GetChangeType() == wxFSW_EVENT_MODIFY) {
            const wxFileName& modpath = event.GetPath();
            if(modpath == m_watchedFile) {
                if(GetOwner()) {
                    gsgs::FileEvent evt(wxEVT_FILE_MODIFIED);
                    evt.SetPath(modpath.GetFullPath());
                    GetOwner()->AddPendingEvent(evt);
                }
            }
        }
    }
    #endif
    //-----------------------------------------------------------------------------------
    void FileWatch::RemoveFile(const wxFileName& filename)
    {
    #if CL_FSW_USE_TIMER
        if(m_files.count(filename.GetFullPath())) {
            m_files.erase(filename.GetFullPath());
        }
    #endif
    }
    //-----------------------------------------------------------------------------------
    bool FileWatch::IsRunning() const
    {
    #if CL_FSW_USE_TIMER
        return m_timer;
    #else
        return m_watcher.GetWatchedPathsCount();
    #endif
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FileOpen
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FileOpen * FileOpen::gInstance = 0;
    //-----------------------------------------------------------------------------------
    FileOpen::FileOpen()
        : _depth(0)
        , _maxDepth(20){}
    //-----------------------------------------------------------------------------------
    FileOpen::~FileOpen() {}
    //-----------------------------------------------------------------------------------
    FileOpen* FileOpen::Get()
    {
        if(gInstance == 0) {
            gInstance = new FileOpen();
        }
        return gInstance;
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::Release()
    {
        if(gInstance) {
            delete gInstance;
        }
        gInstance = 0;
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::AddSearchPath(const wxString& path)
    {
        wxFileName fn(path, "");
        if(!wxFileName::DirExists(fn.GetPath())) return;
        _searchPath.push_back(fn.GetPath());
    }
    //-----------------------------------------------------------------------------------
    FILE* FileOpen::OpenFile(const wxString& include_path, wxString& filepath)
    {
        filepath.Clear();
        if(include_path.empty()) {
            return NULL;
        }

        wxString mod_path(include_path);

        static wxString trimString("\"<> \t");

        mod_path.erase(0, mod_path.find_first_not_of(trimString));
        mod_path.erase(mod_path.find_last_not_of(trimString) + 1);

        if(_scannedfiles.count(mod_path)) {
            // we already scanned this file
            filepath.Clear();
            return NULL;
        }

        FILE* fp(NULL);

        // first try to cwd
        fp = try_open(_cwd, mod_path, filepath);
        if(fp) {
            return fp;
        }

        // Now try the search directories
        for(size_t i = 0; i < _searchPath.size(); ++i) {
            fp = try_open(_searchPath.at(i), mod_path, filepath);
            if(fp) return fp;
        }

        _scannedfiles.insert(mod_path);
        filepath.Clear();
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    FILE* FileOpen::try_open(const wxString& path, const wxString& name, wxString& filepath)
    {
        wxString fullpath(path + FC_PATH_SEP + name);
        wxFileName fn(fullpath);

        fullpath = fn.GetFullPath();
        FILE* fp = wxFopen(fullpath, "rb");
        if(fp) {

            _scannedfiles.insert(name);
            wxString pathPart = fn.GetPath();

            for(size_t i = 0; i < _excludePaths.size(); ++i) {
                if(pathPart.StartsWith(_excludePaths.at(i))) {
                    ::fclose(fp);
                    return NULL;
                }
            }

            _matchedfiles.insert(fullpath);
            filepath = fullpath;
            return fp;
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::AddExcludePath(const wxString& path)
    {
        wxFileName fn(path, "");
        if(!wxFileName::DirExists(fn.GetPath())) return;
        _excludePaths.push_back(fn.GetPath());
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::AddNamespace(const char* ns) 
    { 
        _namespaces.insert(ns); 
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::AddIncludeStatement(const wxString& statement)
    {
        if(_includeStatements.count(statement) == 0) 
        {
            _includeStatements.insert(statement);
        }
    }
    //-----------------------------------------------------------------------------------
    BufferState FileOpen::PopBufferState()
    {
        if(_states.empty()) {
            return NULL;
        }

        fcState curstate = _states.top();
        BufferState state = curstate.buffer;

        // update the current directory
        _cwd = wxFileName(curstate.filename).GetPath();

        _states.pop();
        decDepth();
        return state;
    }
    //-----------------------------------------------------------------------------------
    void FileOpen::PushBufferState(BufferState buffer, const wxString& filename)
    {
        fcState curstate;
        curstate.buffer = buffer;
        curstate.filename = filename;
        _states.push(curstate);

        // update the current directory
        _cwd = wxFileName(curstate.filename).GetPath();
        incDepth();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FileScan
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FileScan::FileScan() {}
    //-----------------------------------------------------------------------------------
    FileScan::~FileScan() {}
    //-----------------------------------------------------------------------------------
    size_t FileScan::Scan(const wxString& rootFolder, std::vector<wxString>& filesOutput, const wxString& filespec,
                                const wxString& excludeFilespec, const gsgsStringSet& excludeFolders)
    {
        filesOutput.clear();
        if(!wxFileName::DirExists(rootFolder)) {
            gsgsDEBUG() << "FileScan: No such dir:" << rootFolder << clEndl;
            return 0;
        }

        wxArrayString specArr = ::wxStringTokenize(filespec.Lower(), ";,|", wxTOKEN_STRTOK);
        wxArrayString excludeSpecArr = ::wxStringTokenize(excludeFilespec.Lower(), ";,|", wxTOKEN_STRTOK);
        std::queue<wxString> Q;
        Q.push(rootFolder);

        while(!Q.empty()) {
            wxString dirpath = Q.front();
            Q.pop();

            wxDir dir(dirpath);
            if(!dir.IsOpened()) { continue; }

            wxString filename;
            bool cont = dir.GetFirst(&filename);
            while(cont) {
                // Check to see if this is a folder
                wxString fullpath;
                fullpath << dir.GetNameWithSep() << filename;
                bool isDirectory = wxFileName::DirExists(fullpath);
                // Use FileUtil::RealPath() here to cope with symlinks on Linux
                if(isDirectory && (excludeFolders.count(FileUtil::RealPath(fullpath)) == 0)) {
                    // Traverse into this folder
                    Q.push(fullpath);
                } else if(!isDirectory && FileUtil::WildMatch(excludeSpecArr, filename)) {
                    // Do nothing
                } else if(!isDirectory && FileUtil::WildMatch(specArr, filename)) {
                    // Include this file
                    filesOutput.push_back(fullpath);
                }
                cont = dir.GetNext(&filename);
            }
        }
        return filesOutput.size();
    }
    //-----------------------------------------------------------------------------------
    size_t FileScan::ScanNoRecurse(const wxString& rootFolder, FileScan::EntryData::Vec_t& results,
                                         const wxString& matchSpec)
    {
        results.clear();
        if(!wxFileName::DirExists(rootFolder)) {
            gsgsDEBUG() << "FileScan::ScanNoRecurse(): No such dir:" << rootFolder << clEndl;
            return 0;
        }
        wxArrayString specArr = ::wxStringTokenize(matchSpec.Lower(), ";,|", wxTOKEN_STRTOK);
        wxDir dir(rootFolder);
        if(!dir.IsOpened()) {
            gsgsDEBUG() << "Failed to open root dir:" << rootFolder;
            return 0;
        }
        wxString dirWithSep = dir.GetNameWithSep();

        wxString filename;
        bool cont = dir.GetFirst(&filename);
        while(cont) {
            if(FileUtil::WildMatch(specArr, filename)) {
                wxString fullpath;
                fullpath << dirWithSep << filename;
                EntryData ed;
                if(FileUtil::IsDirectory(fullpath)) {
                    ed.flags |= kIsFolder;
                } else {
                    ed.flags |= kIsFile;
                }
                if(FileUtil::IsSymlink(fullpath)) { ed.flags |= kIsSymlink; }
                if(FileUtil::IsHidden(fullpath)) { ed.flags |= kIsHidden; }
                ed.fullpath = fullpath;
                results.push_back(ed);
            }
            cont = dir.GetNext(&filename);
        }
        return results.size();
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OpenFileExplorer(const wxString& path)
    {
        // Wrap the path with quotes if needed
        wxString strPath = path;
        if(strPath.Contains(" ")) { strPath.Prepend("\"").Append("\""); }
        wxString cmd;

    #ifdef __WXMSW__
        cmd << "explorer ";
    #elif defined(__WXGTK__)
        cmd << "xdg-open ";
    #elif defined(__WXMAC__)
        cmd << "open ";
    #endif
        if(!cmd.IsEmpty()) {
            cmd << strPath;
            ::wxExecute(cmd);
        }
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OpenTerminal(const wxString& path, const wxString& user_command, bool pause_when_exit)
    {
        ConsoleBase::Ptr_t console = ConsoleBase::GetTerminal();
        console->SetCommand(user_command, "");
        console->SetWorkingDirectory(path);
        console->SetWaitWhenDone(pause_when_exit);
        console->Start();
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv)
    {
        wxFFile file(fn.GetFullPath(), wxT("w+b"));
        if(!file.IsOpened()) { return false; }

        if(!file.Write(content, conv)) { return false; }
        return true;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)
    {
        wxString filename = fn.GetFullPath();
        data.clear();
        //const char* cfile = filename.mb_str(wxConvUTF8).data();
        const char* cfile = filename.data();
        FILE* fp = fopen(cfile, "rb");
        if(!fp) {
            // Nothing to be done
            gsgsERROR() << "Failed to open file:" << fn << "." << strerror(errno);
            return false;
        }

        // Get the file size
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // Allocate buffer for the read
        char* buffer = (char*)malloc(fsize + 1);
        long bytes_read = fread(buffer, 1, fsize, fp);
        if(bytes_read != fsize) {
            // failed to read
            gsgsERROR() << "Failed to read file content:" << fn << "." << strerror(errno);
            fclose(fp);
            free(buffer);
            return false;
        }
        buffer[fsize] = 0;

        // Close the handle
        fclose(fp);

        // Convert it into wxString
        data = wxString(buffer, conv, fsize);
        if(data.IsEmpty() && fsize != 0) {
            // Conversion failed
            data = wxString::From8BitData(buffer, fsize);
        }

        // Release the C-buffer allocated earlier
        free(buffer);
        return true;
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OpenFileExplorerAndSelect(const wxFileName& filename)
    {
    #ifdef __WXMSW__
        wxString strPath = filename.GetFullPath();
        if(strPath.Contains(" ")) { strPath.Prepend("\"").Append("\""); }
        wxString cmd;
        cmd << "explorer /select," << strPath;
        ::wxExecute(cmd);
    #else
        OpenFileExplorer(filename.GetPath());
    #endif
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OSXOpenDebuggerTerminalAndGetTTY(const wxString& path, const wxString& appname, wxString& tty,
                                                     long& pid)
    {
        tty.Clear();
        wxString command;
        wxString tmpfile;
        tmpfile << "/tmp/terminal.tty." << ::wxGetProcessId();
        wxFileName helperScript("/tmp", "gsgs-lldb-helper.sh");
        wxString fileContent;
        fileContent << "#!/bin/bash\n";
        fileContent << "tty > " << tmpfile << "\n";
        fileContent << "sleep 12345";
        FileUtil::WriteFileContent(helperScript, fileContent);
        int rc = system("chmod +x /tmp/gsgs-lldb-helper.sh");
        wxUnusedVar(rc);

        command << "/usr/bin/open -a " << appname << " /tmp/gsgs-lldb-helper.sh";
        gsgsDEBUG() << "Executing: " << command;
        long res = ::wxExecute(command);
        if(res == 0) {
            gsgsWARNING() << "Failed to execute command:" << command;
            return;
        }

        // Read the tty from the file, wait for it up to 10 seconds
        wxFileName ttyFile(tmpfile);
        pid = wxNOT_FOUND;
        for(size_t i = 0; i < 10; ++i) {
            if(!ttyFile.Exists()) {
                ::wxSleep(1);
                continue;
            }
            ReadFileContent(ttyFile, tty);
            tty.Trim().Trim(false);

            // Remove the file
            gsgsRemoveFile(ttyFile.GetFullPath());

            // Get the parent process ID (we want the parent PID and not
            // the sleep command PID)
            wxString psCommand;
            psCommand << "ps -A -o ppid,command";
            wxString psOutput = ProcUtils::SafeExecuteCommand(psCommand);
            gsgsDEBUG() << "ps command output:\n" << psOutput;
            wxArrayString lines = ::wxStringTokenize(psOutput, "\n", wxTOKEN_STRTOK);
            for(size_t u = 0; u < lines.GetCount(); ++u) {
                wxString l = lines.Item(u);
                l.Trim().Trim(false);
                if(l.Contains("sleep") && l.Contains("12345")) {
                    // we got a match
                    gsgsDEBUG() << "Got a match!";
                    wxString ppidString = l.BeforeFirst(' ');
                    ppidString.ToCLong(&pid);
                    break;
                }
            }
            break;
        }
        gsgsDEBUG() << "PID is:" << pid;
        gsgsDEBUG() << "TTY is:" << tty;
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OpenSSHTerminal(const wxString& sshClient, const wxString& connectString, const wxString& password,
                                    int port)
    {
        ConsoleBase::Ptr_t console = ConsoleBase::GetTerminal();
        wxString args;
    #ifdef __WXMSW__
        args << "-P " << port << " " << connectString;
        if(!password.IsEmpty()) { args << " -pw " << password; }
        console->SetExecExtraFlags(wxEXEC_HIDE_CONSOLE);
    #else
        args << "-p " << port << " " << connectString;
    #endif
        console->SetCommand(sshClient, args);
        console->Start();
    }
    //-----------------------------------------------------------------------------------
    static void SplitMask(const wxString& maskString, wxArrayString& includeMask, wxArrayString& excludeMask)
    {
        wxString lcMask = maskString.Lower();
        wxArrayString masks = ::wxStringTokenize(lcMask, ";,", wxTOKEN_STRTOK);
        for(size_t i = 0; i < masks.size(); ++i) {
            wxString& mask = masks.Item(i);
            mask.Trim().Trim(false);
            // exclude mask starts with "!" or "-"
            if((mask[0] == '!') || (mask[0] == '-')) {
                mask.Remove(0, 1);
                excludeMask.Add(mask);
            } else {
                includeMask.Add(mask);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::WildMatch(const wxString& mask, const wxFileName& filename)
    {

        wxArrayString incMasks;
        wxArrayString excMasks;
        SplitMask(mask, incMasks, excMasks);

        if(incMasks.Index("*") != wxNOT_FOUND) {
            // If one of the masks is plain "*" - we match everything
            return true;
        }

        wxString lcFilename = filename.GetFullName().Lower();
        // Try to the "exclude" masking first
        for(size_t i = 0; i < excMasks.size(); ++i) {
            const wxString& pattern = excMasks.Item(i);
            if((!pattern.Contains("*") && lcFilename == pattern) ||
               (pattern.Contains("*") && ::wxMatchWild(pattern, lcFilename))) {
                // use exact match
                return false;
            }
        }

        for(size_t i = 0; i < incMasks.size(); ++i) {
            const wxString& pattern = incMasks.Item(i);
            if((!pattern.Contains("*") && lcFilename == pattern) ||
               (pattern.Contains("*") && ::wxMatchWild(pattern, lcFilename))) {
                // use exact match
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::WildMatch(const wxString& mask, const wxString& filename)
    {
        return WildMatch(mask, wxFileName(filename));
    }
    //-----------------------------------------------------------------------------------
    wxString FileUtil::DecodeURI(const wxString& uri)
    {
        static gsgsStringMap T = { { "%20", " " }, { "%21", "!" }, { "%23", "#" }, { "%24", "$" }, { "%26", "&" },
                                   { "%27", "'" }, { "%28", "(" }, { "%29", ")" }, { "%2A", "*" }, { "%2B", "+" },
                                   { "%2C", "," }, { "%3B", ";" }, { "%3D", "=" }, { "%3F", "?" }, { "%40", "@" },
                                   { "%5B", "[" }, { "%5D", "]" } };
        wxString decodedString;
        wxString escapeSeq;
        int state = 0;
        for(size_t i = 0; i < uri.size(); ++i) {
            wxChar ch = uri[i];
            switch(state) {
            case 0: // Normal
                switch(ch) {
                case '%':
                    state = 1;
                    escapeSeq << ch;
                    break;
                default:
                    decodedString << ch;
                    break;
                }
                break;
            case 1: // Escaping mode
                escapeSeq << ch;
                if(escapeSeq.size() == 3) {
                    // Try to decode it
                    gsgsStringMap::iterator iter = T.find(escapeSeq);
                    if(iter != T.end()) {
                        decodedString << iter->second;
                    } else {
                        decodedString << escapeSeq;
                    }
                    state = 0;
                    escapeSeq.Clear();
                }
                break;
            }
        }
        return decodedString;
    }
    //-----------------------------------------------------------------------------------
    wxString FileUtil::EncodeURI(const wxString& uri)
    {
        static std::unordered_map<int, wxString> sEncodeMap = {
            { (int)'!', "%21" }, { (int)'#', "%23" }, { (int)'$', "%24" }, { (int)'&', "%26" }, { (int)'\'', "%27" },
            { (int)'(', "%28" }, { (int)')', "%29" }, { (int)'*', "%2A" }, { (int)'+', "%2B" }, { (int)',', "%2C" },
            { (int)';', "%3B" }, { (int)'=', "%3D" }, { (int)'?', "%3F" }, { (int)'@', "%40" }, { (int)'[', "%5B" },
            { (int)']', "%5D" }, { (int)' ', "%20" }
        };

        wxString encoded;
        for(size_t i = 0; i < uri.length(); ++i) {
            wxChar ch = uri[i];
            std::unordered_map<int, wxString>::iterator iter = sEncodeMap.find((int)ch);
            if(iter != sEncodeMap.end()) {
                encoded << iter->second;
            } else {
                encoded << ch;
            }
        }
        return encoded;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::FuzzyMatch(const wxString& needle, const wxString& haystack)
    {
        wxString word;
        size_t offset = 0;
        wxString lcHaystack = haystack.Lower();
        while(NextWord(needle, offset, word, true)) {
            if(!lcHaystack.Contains(word)) { return false; }
        }
        return true;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsHidden(const wxString& filename)
    {
    #ifdef __WXMSW__
        DWORD dwAttrs = GetFileAttributes(filename.c_str());
        if(dwAttrs == INVALID_FILE_ATTRIBUTES) return false;
        return (dwAttrs & FILE_ATTRIBUTE_HIDDEN) || (wxFileName(filename).GetFullName().StartsWith("."));
    #else
        // is it enough to test for file name?
        wxFileName fn(filename);
        return fn.GetFullName().StartsWith(".");
    #endif
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsHidden(const wxFileName& filename) { return IsHidden(filename.GetFullPath()); }
    //-----------------------------------------------------------------------------------
    bool FileUtil::WildMatch(const wxArrayString& masks, const wxString& filename)
    {
        if(masks.IsEmpty()) { return false; }

        if(masks.Index("*") != wxNOT_FOUND) {
            // If one of the masks is plain "*" - we match everything
            return true;
        }

        for(size_t i = 0; i < masks.size(); ++i) {
            const wxString& pattern = masks.Item(i);
            if((!pattern.Contains("*") && filename == pattern) ||
               (pattern.Contains("*") && ::wxMatchWild(pattern, filename))) {
                // use exact match
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::SetFilePermissions(const wxFileName& filename, mode_t perm)
    {
        wxString strFileName = filename.GetFullPath();
        return (::chmod(strFileName.mb_str(wxConvUTF8).data(), perm & 07777) != 0);
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::GetFilePermissions(const wxFileName& filename, mode_t& perm)
    {
        struct stat b;
        wxString strFileName = filename.GetFullPath();
        if(::stat(strFileName.mb_str(wxConvUTF8).data(), &b) == 0) {
            perm = b.st_mode;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    time_t FileUtil::GetFileModificationTime(const wxFileName& filename)
    {
        wxString file = filename.GetFullPath();
        struct stat buff;
        const wxCharBuffer cname = file.mb_str(wxConvUTF8);
        if(stat(cname.data(), &buff) < 0) { return 0; }
        return buff.st_mtime;
    }
    //-----------------------------------------------------------------------------------
    size_t FileUtil::GetFileSize(const wxFileName& filename)
    {
        struct stat b;
        wxString file_name = filename.GetFullPath();
        const char* cfile = file_name.mb_str(wxConvUTF8).data();
        if(::stat(cfile, &b) == 0) {
            return b.st_size;
        } else {
            gsgsERROR() << "Failed to open file:" << file_name << "." << strerror(errno);
            return 0;
        }
    }
    //-----------------------------------------------------------------------------------
    wxString FileUtil::EscapeString(const wxString& str)
    {
        wxString modstr = str;
        modstr.Replace(" ", "\\ ");
        modstr.Replace("\"", "\\\"");
        return modstr;
    }
    //-----------------------------------------------------------------------------------
    wxString FileUtil::GetOSXTerminalCommand(const wxString& command, const wxString& workingDirectory)
    {
        wxFileName script(gsgs_Path().GetBinFolder(), "osx-terminal.sh");

        wxString cmd;
        cmd << EscapeString(script.GetFullPath()) << " \"";
        if(!workingDirectory.IsEmpty()) { cmd << "cd " << EscapeString(workingDirectory) << " && "; }
        cmd << EscapeString(command) << "\"";
        gsgsDEBUG() << "GetOSXTerminalCommand returned:" << cmd << clEndl;
        return cmd;
    }
    //-----------------------------------------------------------------------------------
    wxString FileUtil::NormaliseName(const wxString& name)
    {
        static bool initialised = false;
        static int invalidChars[256];
        if(!initialised) 
        {
            memset(invalidChars, 0, sizeof(invalidChars));
            std::vector<int> V = { '@', '-', '^', '%', '&', '$', '#', '@', '!', '(', ')',
                                   '{', '}', '[', ']', '+', '=', ';', ',', '.', ' ' };
            for(size_t i = 0; i < V.size(); ++i) {
                invalidChars[V[i]] = 1;
            }
            initialised = true;
        }

        wxString normalisedName;
        for(size_t i = 0; i < name.size(); ++i) 
        {
            if(invalidChars[name[i]]) {
                // an invalid char was found
                normalisedName << "_";
            } else {
                normalisedName << name[i];
            }
        }
        return normalisedName;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::WriteFileUTF8(const wxString& fileName, const wxString& content)
    {
        wxFFile file(fileName, wxT("w+b"));
        if (!file.IsOpened()) { return false; }

        // first try the Utf8
        return file.Write(content, wxConvUTF8);
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower)
    {
        if(offset == str.size()) { return false; }
        size_t start = wxString::npos;
        word.Clear();
        for(; offset < str.size(); ++offset) 
        {
            wxChar ch = str[offset];
            bool isWhitespace = ((ch == ' ') || (ch == '\t'));
            if(isWhitespace && (start != wxString::npos)) {
                // we found a trailing whitespace
                break;
            } else if(isWhitespace && (start == wxString::npos)) {
                // skip leading whitespace
                continue;
            } else if(start == wxString::npos) {
                start = offset;
            }
            if(makeLower) { ch = wxTolower(ch); }
            word << ch;
        }

        if((start != wxString::npos) && (offset > start)) { return true; }
        return false;
    }
    //-----------------------------------------------------------------------------------
    size_t FileUtil::SplitWords(const wxString& str, gsgsStringSet& outputSet, bool makeLower)
    {
        size_t offset = 0;
        wxString word;
        outputSet.clear();
        while(NextWord(str, offset, word, makeLower)) {
            outputSet.insert(word);
        }
        return outputSet.size();
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::RemoveFile(const wxString& filename, const wxString& context)
    {
        gsgsDEBUG1() << "Deleting file:" << filename << "(" << context << ")";
        wxLogNull NOLOG;
        return ::wxRemoveFile(filename);
    }
    //-----------------------------------------------------------------------------------
    unsigned int FileUtil::UTF8Length(const wchar_t* uptr, unsigned int tlen)
    {
    #define SURROGATE_LEAD_FIRST 0xD800
    #define SURROGATE_TRAIL_FIRST 0xDC00
    #define SURROGATE_TRAIL_LAST 0xDFFF
        unsigned int len = 0;
        for(unsigned int i = 0; i < tlen && uptr[i];) {
            unsigned int uch = uptr[i];
            if(uch < 0x80) {
                len++;
            } else if(uch < 0x800) {
                len += 2;
            } else if((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
                len += 4;
                i++;
            } else {
                len += 3;
            }
            i++;
        }
    #undef SURROGATE_LEAD_FIRST
    #undef SURROGATE_TRAIL_FIRST
    #undef SURROGATE_TRAIL_LAST
        return len;
    }
    //-----------------------------------------------------------------------------------
    // This is readlink on steroids: it also makes-absolute, and dereferences any symlinked dirs in the path
    wxString FileUtil::RealPath(const wxString& filepath)
    {
    #if defined(__WXGTK__)
        if(!filepath.empty()) {
            char* buf = realpath(filepath.mb_str(wxConvUTF8), NULL);
            if(buf != NULL) {
                wxString result(buf, wxConvUTF8);
                free(buf);
                return result;
            }
        }
    #endif

        return filepath;
    }
    //-----------------------------------------------------------------------------------
    void FileUtil::OpenBuiltInTerminal(const wxString& wd, const wxString& user_command, bool pause_when_exit)
    {
        wxString title(user_command);

        wxFileName fngsgsTerminal(gsgs_Path().GetExecutablePath());
        fngsgsTerminal.SetFullName("gsgs-terminal");

        wxString newCommand;
        newCommand << fngsgsTerminal.GetFullPath() << " --exit ";
        if(pause_when_exit) { newCommand << " --wait "; }
        if(wxDirExists(wd)) {
            wxString workingDirectory = wd;
            workingDirectory.Trim().Trim(false);
            if(workingDirectory.Contains(" ") && !workingDirectory.StartsWith("\"")) {
                workingDirectory.Prepend("\"").Append("\"");
            }
            newCommand << " --working-directory " << wd;
        }
        newCommand << " --cmd " << title;
        ::wxExecute(newCommand, wxEXEC_ASYNC);
    }
    //-----------------------------------------------------------------------------------
    std::string FileUtil::ToStdString(const wxString& str) { return StringUtils::ToStdString(str); }
    //-----------------------------------------------------------------------------------
    bool FileUtil::ReadBufferFromFile(const wxFileName& fn, wxString& data, size_t bufferSize)
    {
        if(!fn.FileExists()) 
        { 
            return false; 
        }
        std::wifstream fin((const wchar_t*)fn.GetFullPath(), std::ios::binary);
        if(fin.bad()) {
            gsgsERROR() << "Failed to open file:" << fn;
            return false;
        }

        std::vector<wchar_t> buffer(bufferSize, 0);
        if(!fin.eof()) { fin.read(buffer.data(), buffer.size()); }
        data.reserve(buffer.size());
        data << std::wstring(buffer.begin(), buffer.begin() + buffer.size());
        return true;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsSymlink(const wxString& filename)
    {
    #ifdef __WXMSW__
        DWORD dwAttrs = GetFileAttributesW(filename.c_str());
        if(dwAttrs == INVALID_FILE_ATTRIBUTES) { return false; }
        return dwAttrs & FILE_ATTRIBUTE_REPARSE_POINT;
    #else
        wxStructStat buff;
        if(wxLstat(filename, &buff) != 0) { return false; }
        return S_ISLNK(buff.st_mode);
    #endif
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsDirectory(const wxString& filename)
    {
    #ifdef __WXMSW__
        DWORD dwAttrs = GetFileAttributesW(filename.c_str());
        if(dwAttrs == INVALID_FILE_ATTRIBUTES) { return false; }
        return dwAttrs & FILE_ATTRIBUTE_DIRECTORY;
    #else
        wxStructStat buff;
        if(wxLstat(filename, &buff) != 0) { return false; }
        return S_ISDIR(buff.st_mode);
    #endif
    }
    //-----------------------------------------------------------------------------------
    struct Matcher {
        SharedPtr<wxRegEx> m_regex;
        wxString m_exactMatch;
        FileUtil::FileType m_fileType;

        Matcher(const wxString& pattern, FileUtil::FileType fileType, bool regex = true)
            : m_fileType(fileType)
        {
            if(regex) {
                m_regex = new wxRegEx(pattern, wxRE_ADVANCED | wxRE_ICASE);
            } else {
                m_exactMatch = pattern;
            }
        }

        bool Matches(const wxString& in) const
        {
            if(m_regex) {
                return m_regex->Matches(in);
            } else {
                return in.Find(m_exactMatch) != wxNOT_FOUND;
            }
        }
    };
    //-----------------------------------------------------------------------------------
    static std::unordered_map<wxString, FileUtil::FileType> m_map;
    static std::vector<Matcher> m_matchers;
    static wxCriticalSection m_CS;
    //-----------------------------------------------------------------------------------
    void FileUtil::Init()
    {
        wxCriticalSectionLocker locker(m_CS);

        // per thread initialization
        static bool init_done(false);
        if(!init_done) {
            init_done = true;
            m_map[wxT("cc")] = TypeSourceCpp;
            m_map[wxT("cpp")] = TypeSourceCpp;
            m_map[wxT("cxx")] = TypeSourceCpp;
            m_map[wxT("c++")] = TypeSourceCpp;
            m_map[wxT("as")] = TypeSourceCpp;  // AngelScript files are handled as C++ source files in gsgs
            m_map[wxT("ino")] = TypeSourceCpp; // Arduino sketches
            m_map[wxT("c")] = TypeSourceC;

            m_map[wxT("h")] = TypeHeader;
            m_map[wxT("hpp")] = TypeHeader;
            m_map[wxT("hxx")] = TypeHeader;
            m_map[wxT("hh")] = TypeHeader;
            m_map[wxT("h++")] = TypeHeader;
            m_map[wxT("inl")] = TypeHeader;

            m_map[wxT("rc")] = TypeResource;
            m_map[wxT("res")] = TypeResource;

            m_map[wxT("y")] = TypeYacc;
            m_map[wxT("l")] = TypeLex;
            m_map[wxT("ui")] = TypeQtForm;
            m_map[wxT("qrc")] = TypeQtResource;
            m_map[wxT("qml")] = TypeJS;

            m_map[wxT("project")] = TypeProject;
            m_map[wxT("workspace")] = TypeWorkspace;
            m_map[wxT("fbp")] = TypeFormbuilder;
            m_map[wxT("cdp")] = TypeCodedesigner;
            m_map[wxT("erd")] = TypeErd;

            m_map[wxT("php")] = TypePhp;
            m_map[wxT("php5")] = TypePhp;
            m_map[wxT("inc")] = TypePhp;
            m_map[wxT("phtml")] = TypePhp;
            m_map[wxT("ctp")] = TypePhp;

            m_map[wxT("xml")] = TypeXml;
            m_map[wxT("xrc")] = TypeXRC;
            m_map[wxT("css")] = TypeCSS;
            m_map[wxT("less")] = TypeCSS;
            m_map[wxT("sass")] = TypeCSS;
            m_map[wxT("js")] = TypeJS;
            m_map[wxT("javascript")] = TypeJS;
            m_map[wxT("py")] = TypePython;

            // Java file
            m_map[wxT("java")] = TypeJava;

            m_map[wxT("exe")] = TypeExe;
            m_map[wxT("html")] = TypeHtml;
            m_map[wxT("htm")] = TypeHtml;

            m_map[wxT("tar")] = TypeArchive;
            m_map[wxT("a")] = TypeArchive;
            m_map[wxT("lib")] = TypeArchive;
            m_map[wxT("zip")] = TypeArchive;
            m_map[wxT("rar")] = TypeArchive;
            m_map[wxT("targz")] = TypeArchive;

            m_map[wxT("dll")] = TypeDll;
            m_map[wxT("so")] = TypeDll;
            m_map[wxT("dylib")] = TypeDll;

            m_map[wxT("bmp")] = TypeBmp;
            m_map[wxT("jpeg")] = TypeBmp;
            m_map[wxT("jpg")] = TypeBmp;
            m_map[wxT("png")] = TypeBmp;
            m_map[wxT("ico")] = TypeBmp;
            m_map[wxT("xpm")] = TypeBmp;
            m_map[wxT("svg")] = TypeSvg;

            m_map[wxT("mk")] = TypeMakefile;

            m_map[wxT("log")] = TypeText;
            m_map[wxT("txt")] = TypeText;
            m_map[wxT("ini")] = TypeText;

            m_map[wxT("script")] = TypeScript;
            m_map[wxT("sh")] = TypeScript;
            m_map[wxT("bat")] = TypeScript;
            m_map[wxT("bash")] = TypeScript;

            m_map[wxT("wxcp")] = TypeWxCrafter;
            m_map[wxT("xrc")] = TypeXRC;

            m_map[wxT("sql")] = TypeSQL;
            m_map[wxT("sqlite")] = TypeSQL;
            m_map[wxT("phpwsp")] = TypeWorkspacePHP;
            m_map[wxT("phptags")] = TypeWorkspacePHPTags;

            m_map["pro"] = TypeQMake;
            m_map["pri"] = TypeQMake;
            m_map["cmake"] = TypeCMake;
            m_map["s"] = TypeAsm;
            m_map["yaml"] = TypeYAML;
            m_map["yml"] = TypeYAML;
            m_map["db"] = TypeDatabase;
            m_map["tags"] = TypeDatabase;

            // Initialize regexes:
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/bin/bash", TypeScript));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/bin/sh", TypeScript));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/usr/bin/sh", TypeScript));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/usr/bin/bash", TypeScript));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/bin/python", TypePython));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/usr/bin/python", TypePython));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/bin/node", TypeJS));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/usr/bin/node", TypeJS));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/bin/nodejs", TypeJS));
            m_matchers.push_back(Matcher("#[ \t]*![ \t]*/usr/bin/nodejs", TypeJS));
            m_matchers.push_back(Matcher("<?xml", TypeXml, false));
            m_matchers.push_back(Matcher("<?php", TypePhp, false));
            m_matchers.push_back(Matcher("#!/usr/bin/env node", TypeJS, false));
            m_matchers.push_back(Matcher("#!/usr/bin/env nodejs", TypeJS, false));
            m_matchers.push_back(Matcher("SQLite format 3", TypeDatabase, false));

            // STL sources places "-*- C++ -*-" at the top of their headers
            m_matchers.push_back(Matcher("-*- C++ -*-", TypeSourceCpp, false));

            // #ifndef WORD
            m_matchers.push_back(Matcher("#ifndef[ \t]+[a-zA-Z0-9_]+", TypeSourceCpp));

            // vim modlines
            m_matchers.push_back(Matcher("/\\* \\-\\*\\- Mode:[ \t]+c\\+\\+", TypeSourceCpp));
            m_matchers.push_back(Matcher("# \\-\\*\\- Mode:[ \t]+python", TypePython));

            // #include <
            m_matchers.push_back(Matcher("#include[ \t]+[\\<\"]", TypeSourceCpp));
        }
    }
    //-----------------------------------------------------------------------------------
    FileUtil::FileType FileUtil::GetType(const wxString& filename, FileUtil::FileType defaultType)
    {
        Init();
        wxCriticalSectionLocker locker(m_CS);

        wxFileName fn(filename);
        if(fn.IsOk() == false) { return defaultType; }

        wxString e(fn.GetExt());
        e.MakeLower();
        e.Trim().Trim(false);

        std::unordered_map<wxString, FileType>::iterator iter = m_map.find(e);
        if(iter == m_map.end()) {
            // try to see if the file is a makefile
            if(fn.GetFullName().CmpNoCase(wxT("makefile")) == 0) {
                return TypeMakefile;
            } else if(fn.GetFullName().Lower() == "dockerfile") {
                return TypeDockerfile;
            } else {
                // try auto detecting
                FileType autoDetectType = defaultType;
                if(AutoDetectByContent(filename, autoDetectType)) { return autoDetectType; }
            }
            return defaultType;
        } else if((iter->second == TypeText) && (fn.GetFullName().CmpNoCase("CMakeLists.txt") == 0)) {
            return TypeCMake;
        }

        FileUtil::FileType type = iter->second;
        if(fn.Exists() && (type == TypeWorkspace)) {
            wxString content;
            if(FileUtil::ReadFileContent(fn, content)) {
                if(content.Contains("<GSGSWP")) {
                    return TypeWorkspace;
                } else {
                    JSON root(content);
                    if(!root.isValid()) return TypeWorkspace;
                    if(root.getObject().isObjectExist("NodeJS")) {
                        return TypeWorkspaceNodeJS;
                    } else if(root.getObject().isObjectExist("Docker")) {
                        return TypeWorkspaceDocker;
                    } else {
                        return TypeWorkspacePHP;
                    }
                }
            } else {
                return TypeWorkspace;
            }
        }
        return iter->second;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsCxxFile(const wxString& filename)
    {
        wxCriticalSectionLocker locker(m_CS);
        FileType ft = GetType(filename);
        if(ft == TypeOther) {
            // failed to detect the type
            if(!AutoDetectByContent(filename, ft)) { return false; }
        }
        return (ft == TypeSourceC) || (ft == TypeSourceCpp) || (ft == TypeHeader);
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::AutoDetectByContent(const wxString& filename, FileUtil::FileType& fileType)
    {
        wxCriticalSectionLocker locker(m_CS);
        wxString fileContent;
        if(!FileUtil::ReadBufferFromFile(filename, fileContent, 4096)) { return false; }

        for(size_t i = 0; i < m_matchers.size(); ++i) {
            if(m_matchers[i].Matches(fileContent)) {
                fileType = m_matchers[i].m_fileType;
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsFileType(const wxString& filename, FileUtil::FileType type)
    {
        wxCriticalSectionLocker locker(m_CS);
        FileType ft = GetType(filename);
        if(ft == TypeOther) 
        {
            // failed to detect the type
            if(!AutoDetectByContent(filename, ft)) 
            { 
                return false; 
            }
        }
        return (ft == type);
    }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsJavascriptFile(const wxString& filename) { return FileUtil::IsFileType(filename, TypeJS); }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsPHPFile(const wxString& filename) { return FileUtil::IsFileType(filename, TypePhp); }
    //-----------------------------------------------------------------------------------
    bool FileUtil::IsJavaFile(const wxString& filename) { return FileUtil::IsFileType(filename, TypeJava); }
    //-----------------------------------------------------------------------------------
    FileUtil::FileType FileUtil::GetTypeFromExtension(const wxFileName& filename)
    {
        wxCriticalSectionLocker locker(m_CS);
        std::unordered_map<wxString, FileUtil::FileType>::iterator iter = m_map.find(filename.GetExt().Lower());
        if(iter == m_map.end()) return TypeOther;
        return iter->second;
    }
    //-----------------------------------------------------------------------------------
}