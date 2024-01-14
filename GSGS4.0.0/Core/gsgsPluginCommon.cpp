
#include "gsgsPluginCommon.h"
#include "gsgsBookmarkManager.h"
#include "gsgsProcess.h"
#include "gsgsLogManager.h"
#include "gsgsEvent.h"
#include "gsgsFileUtil.h"
#include "gsgsPathManager.h"
#include "gsgsCommon.h"
#include "globals.h"
#include "md5_header.h"

#ifndef __WXMSW__
#   include <sys/wait.h>
#endif

static wxString sEmptyStr;
namespace gsgs
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TerminalHistory
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    TerminalHistory::TerminalHistory()
        : m_where(-1)
    {
    }
    //-----------------------------------------------------------------------------
    TerminalHistory::~TerminalHistory() {}
    //-----------------------------------------------------------------------------
    void TerminalHistory::Add(const wxString& command)
    {
        std::vector<wxString>::iterator iter =
            std::find_if(m_history.begin(), m_history.end(), [&](const wxString& str) { return str == command; });
        if(iter != m_history.end()) { m_history.erase(iter); }
        m_history.insert(m_history.begin(), command);
        DoReset();
    }
    //-----------------------------------------------------------------------------
    const wxString& TerminalHistory::ArrowUp()
    {
        if((m_where + 1) < (int)m_history.size()) {
            ++m_where;
            return m_history[m_where];
        }
        return sEmptyStr;
    }
    //-----------------------------------------------------------------------------
    const wxString& TerminalHistory::ArrowDown()
    {
        if(m_where > 0 && m_where < (int)m_history.size()) {
            m_where--;
            return m_history[m_where];
        }
        return sEmptyStr;
    }
    //-----------------------------------------------------------------------------
    wxArrayString TerminalHistory::GetItems() const
    {
        wxArrayString items;
        std::for_each(m_history.begin(), m_history.end(), [&](const wxString& item) { items.Add(item); });
        return items;
    }
    //-----------------------------------------------------------------------------
    void TerminalHistory::SetItems(const wxArrayString& items)
    {
        m_history.clear();
        for(size_t i = 0; i < items.GetCount(); ++i) {
            Add(items.Item(i));
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ConsoleFinder
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ConsoleFinder::ConsoleFinder()
        : m_nConsolePid(0)
        , m_consoleCommand(TERMINAL_CMD)
    {
    }
    //-----------------------------------------------------------------------------
    ConsoleFinder::~ConsoleFinder() { FreeConsole(); }
    //-----------------------------------------------------------------------------
    void ConsoleFinder::FreeConsole()
    {
        if(m_nConsolePid) {
            wxKill(m_nConsolePid, wxSIGKILL, NULL, wxKILL_CHILDREN);
            m_nConsolePid = 0;
        }
    }
    //-----------------------------------------------------------------------------
    int ConsoleFinder::RunConsole(const wxString& title)
    {
    // start the xterm and put the shell to sleep with -e sleep 80000
    // fetch the xterm tty so we can issue to gdb a "tty /dev/pts/#"
    // redirecting program stdin/stdout/stderr to the xterm console.

    #ifndef __WXMSW__
        wxString cmd;

        cmd = GetConsoleCommand();
        cmd.Replace(wxT("$(TITLE)"), title);
        cmd.Replace(wxT("$(CMD)"), wxString::Format(wxT("sleep %lu"), 80000 + wxGetProcessId()));

        gsgsDEBUG() << "Launching console:" << cmd;

        m_nConsolePid = wxExecute(cmd, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
        if(m_nConsolePid <= 0) {
            return -1;
        }

        // Issue the PS command to get the /dev/tty device name
        // First, wait for the xterm to settle down, else PS won't see the sleep task
        wxSleep(1);
        m_ConsoleTty = GetConsoleTty(m_nConsolePid);
        if(m_ConsoleTty.IsEmpty()) {
            FreeConsole();
            return -1;
        }
        return m_nConsolePid;

    #else //__WXMSW__
        wxUnusedVar(title);
        return -1;
    #endif
    }
    //-----------------------------------------------------------------------------
    wxString ConsoleFinder::GetConsoleTty(int ConsolePid)
    {
    #ifndef __WXMSW__
        // execute the ps x -o command  and read PS output to get the /dev/tty field
        unsigned long ConsPid = ConsolePid;
        wxString psCmd;
        wxArrayString psOutput;
        wxArrayString psErrors;

        psCmd << wxT("ps x -o tty,pid,command");
        ProcUtils::ExecuteCommand(psCmd, psOutput);

        wxString ConsTtyStr;
        wxString ConsPidStr;
        ConsPidStr << ConsPid;
        // find task with our unique sleep time
        wxString uniqueSleepTimeStr;
        uniqueSleepTimeStr << wxT("sleep ") << wxString::Format(wxT("%lu"), 80000 + ::wxGetProcessId());
        // search the output of "ps pid" command
        int knt = psOutput.GetCount();
        for(int i = knt - 1; i > -1; --i) {
            psCmd = psOutput.Item(i);
            // find the pts/# or tty/# or whatever it's called
            // by seaching the output of "ps x -o tty,pid,command" command.
            // The output of ps looks like:
            // TT       PID   COMMAND
            // pts/0    13342 /bin/sh ./run.sh
            // pts/0    13343 /home/pecanpecan/devel/trunk/src/devel/codeblocks
            // pts/0    13361 /usr/bin/gdb -nx -fullname -quiet -args ./conio
            // pts/0    13362 xterm -font -*-*-*-*-*-*-20-*-*-*-*-*-*-* -T Program Console -e sleep 93343
            // pts/2    13363 sleep 93343
            // ?        13365 /home/pecan/proj/conio/conio
            // pts/1    13370 ps x -o tty,pid,command

            if(psCmd.Contains(uniqueSleepTimeStr)) do {  // check for correct "sleep" line
                    if(psCmd.Contains(wxT("-T"))) break; // error;wrong sleep line.
                    // found "sleep 93343" string, extract tty field
                    ConsTtyStr = wxT("/dev/") + psCmd.BeforeFirst(' ');
                    return ConsTtyStr;
                } while(0); // if do
        }                   // for
        return wxEmptyString;
    #else
        wxUnusedVar(ConsolePid);
        return wxEmptyString;
    #endif
    }
    //-----------------------------------------------------------------------------
    bool ConsoleFinder::FindConsole(const wxString& title, wxString& consoleName)
    {
        int pid = RunConsole(title);
        if(pid > 0) {
            consoleName = m_ConsoleTty;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------------
    wxString ConsoleFinder::GetConsoleName()
    {
        wxString cmd;
    #ifdef __WXMSW__
        cmd = wxGetenv(wxT("COMSPEC"));
        if(cmd.IsEmpty()) {
            cmd = wxT("CMD.EXE");
        }
    #else // non-windows
        // try to locate the default terminal
        wxString terminal;
        wxString where;
        if(ExeLocator::locate(wxT("gnome-terminal"), where)) {
            terminal = wxT("gnome-terminal -e ");
        } else if(ExeLocator::locate(wxT("konsole"), where)) {
            terminal = wxT("konsole");
        } else if(ExeLocator::locate(wxT("terminal"), where)) {
            terminal = wxT("terminal -e");
        } else if(ExeLocator::locate(wxT("lxterminal"), where)) {
            terminal = wxT("lxterminal -e");
        } else if(ExeLocator::locate(wxT("xterm"), where)) {
            terminal = wxT("xterm -e ");
        }

        if(cmd.IsEmpty()) {
            cmd = wxT("xterm -e ");
        }

        cmd = terminal;
    #endif
        return cmd;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // wxMD5
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    wxMD5::wxMD5() {}
    //-----------------------------------------------------------------------------
    wxMD5::wxMD5(const wxString& szText) { m_szText = szText; }
    //-----------------------------------------------------------------------------
    wxMD5::~wxMD5() {}
    //-----------------------------------------------------------------------------
    void wxMD5::SetText(const wxString& szText) { m_szText = szText; }
    //-----------------------------------------------------------------------------
    const wxString wxMD5::GetDigest()
    {
        MD5 context;
        context.update((unsigned char*)m_szText.mb_str().data(), m_szText.Len());
        context.finalize();

        wxString md5(context.hex_digest());
        md5.MakeUpper();
        return md5;
    }
    //-----------------------------------------------------------------------------
    const wxString wxMD5::GetDigest(const wxString& szText)
    {
        wxMD5 md5(szText);
        return md5.GetDigest();
    }
    //-----------------------------------------------------------------------------
    wxMD5::wxMD5(const wxFileName& filename) { FileUtil::ReadFileContent(filename, m_szText); }
    //-----------------------------------------------------------------------------
    const wxString wxMD5::GetDigest(const wxFileName& filename)
    {
        wxMD5 md5(filename);
        return md5.GetDigest();
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TreeAccess
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    TreeAccess::TreeAccess(wxTreeCtrl* tree)
        : m_tree(tree)
    {
    }
    //-----------------------------------------------------------------------------
    TreeAccess::~TreeAccess()
    {
    }
    //-----------------------------------------------------------------------------
    void TreeAccess::DoTraverse(const wxTreeItemId& item)
    {
        if(item.IsOk() == false)
            return;
        
        // Call the user callback
        OnItem(item);
        
        // Recurse the children
        if(m_tree->ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
            while ( child.IsOk() ) {
                DoTraverse(child);
                child = m_tree->GetNextChild(item, cookie);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void TreeAccess::OnItem(const wxTreeItemId& item)
    {
    }
    //-----------------------------------------------------------------------------
    void TreeAccess::Traverse(const wxTreeItemId& item)
    {
        DoTraverse(item.IsOk() ? item : m_tree->GetRootItem());
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // FileOrFolderDropTarget
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    FileOrFolderDropTarget::FileOrFolderDropTarget(wxEvtHandler* eventSink)
        : m_sink(eventSink)
    {
    }
    //-----------------------------------------------------------------------------
    FileOrFolderDropTarget::~FileOrFolderDropTarget() {}
    //-----------------------------------------------------------------------------
    bool FileOrFolderDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
    {
        wxUnusedVar(x);
        wxUnusedVar(y);
        
        // Split the list into 2: files and folders
        wxArrayString files, folders;
        for(size_t i = 0; i < filenames.size(); ++i) {
            if(wxFileName::DirExists(filenames.Item(i))) {
                folders.Add(filenames.Item(i));
            } else {
                files.Add(filenames.Item(i));
            }
        }
        
        if(m_sink) {
            // fire the events, folders before files
            if(!folders.IsEmpty()) {
                Event eventFolders(wxEVT_DND_FOLDER_DROPPED);
                eventFolders.SetStrings(folders);
                m_sink->AddPendingEvent(eventFolders);
            }
            
            if(!files.IsEmpty()) {
                Event eventFiles(wxEVT_DND_FILE_DROPPED);
                eventFiles.SetStrings(files);
                m_sink->AddPendingEvent(eventFiles);
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // QuickDebugInfo
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    QuickDebugInfo::QuickDebugInfo()
        : m_selectedDbg(0)
    {
    }

    QuickDebugInfo::~QuickDebugInfo() {}

    void QuickDebugInfo::DeSerialize(Serializer& arch)
    {
        arch.Read(wxT("m_arguments"), m_arguments);
        arch.Read(wxT("m_exeFilepaths"), m_exeFilepaths);
        arch.Read(wxT("m_selectedDbg"), m_selectedDbg);
        arch.Read(wxT("m_startCmds"), m_startCmds);
        arch.Read(wxT("m_wds"), m_wds);
        arch.Read(wxT("m_alternateDebuggerExec"), m_alternateDebuggerExec);
    }

    void QuickDebugInfo::Serialize(Serializer& arch)
    {
        arch.Write(wxT("m_arguments"), m_arguments);
        arch.Write(wxT("m_exeFilepaths"), m_exeFilepaths);
        arch.Write(wxT("m_selectedDbg"), m_selectedDbg);
        arch.Write(wxT("m_startCmds"), m_startCmds);
        arch.Write(wxT("m_wds"), m_wds);
        arch.Write(wxT("m_alternateDebuggerExec"), m_alternateDebuggerExec);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ExeLocator
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    bool ExeLocator::locate(const wxString& name, wxString& where)
    {
        wxString filename = name;
        filename.Trim().Trim(false);
        if(filename.StartsWith("\"")) {
            filename = filename.Mid(1);
        }

        if(filename.EndsWith("\"")) {
            filename = filename.RemoveLast();
        }

        // Incase the name is a full path, just test for the file existance
        wxFileName fn(filename);
        if(fn.IsAbsolute() && fn.FileExists()) {
            where = name;
            return true;
        }

        // Check the path
        wxString path = wxGetenv("PATH");
        wxArrayString paths = ::wxStringTokenize(path, wxPATH_SEP, wxTOKEN_STRTOK);
        for(size_t i = 0; i < paths.size(); ++i) {
            wxFileName fnExe(paths.Item(i), fn.GetFullName());
            if(fnExe.FileExists()) {
                where = fnExe.GetFullPath();
                return true;
            }
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // FolderColour
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    FolderColour::FolderColour() {}
    //-------------------------------------------------------------------------------------
    FolderColour::~FolderColour() {}
    //-------------------------------------------------------------------------------------
    void FolderColour::SortToList(const FolderColour::Map_t& m, FolderColour::List_t& l)
    {
        l.clear();
        std::for_each(m.begin(), m.end(), [&](const FolderColour::Map_t::value_type& p) { l.push_back(p.second); });

        l.sort([&](const FolderColour& first, const FolderColour& second) {
            return first.GetPath().Cmp(second.GetPath()) > 0;
        });
    }
    //-------------------------------------------------------------------------------------
    const FolderColour& FolderColour::FindForPath(const FolderColour::List_t& sortedList, const wxString& path)
    {
        static FolderColour nullValue;
        FolderColour::List_t::const_iterator iter = std::find_if(
            sortedList.begin(), sortedList.end(), [&](const FolderColour& vdc) { return path.StartsWith(vdc.GetPath()); });
        if(iter == sortedList.end()) {
            return nullValue;
        } else {
            return (*iter);
        }
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // Patch
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    Patch::Patch()
    {
        // Locate the patch.exe on the system
        wxArrayString hints;
    #ifdef __WXMSW__
        hints.Add(gsgs_Path().GetExecutablePath());
    #endif
        ::FindExec("patch", m_patch, hints);
    }
    //-------------------------------------------------------------------------------------
    Patch::~Patch() {}
    //-------------------------------------------------------------------------------------
    void Patch::patch(const wxFileName& patchFile, const wxString& workingDirectory, const wxString& args) 
    {
        // Sanity
        if(!m_patch.FileExists()) {
            throw Exception("Could not locate patch executable");
        }

        if(!patchFile.FileExists()) {
            throw Exception("Patch failed. File: '" + patchFile.GetFullPath() + "' does not exist");
        }

        // Prepare the command
        wxString command;
        command << m_patch.GetFullPath();

        ::WrapWithQuotes(command);

        if(!args.IsEmpty()) {
            command << " " << args;
        }

        // Change directory to the working directory requested by the user
        DirSaver ds;
        wxSetWorkingDirectory(workingDirectory.IsEmpty() ? ::wxGetCwd() : workingDirectory);

        wxString patch = patchFile.GetFullPath();

        command << " " << ::WrapWithQuotes(patch);
        ::WrapInShell(command);

        ProcUtils::SafeExecuteCommand(command);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerTerminalPOSIX
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DebuggerTerminalPOSIX::DebuggerTerminalPOSIX()
        : m_pid(wxNOT_FOUND)
    {
    }
    //-----------------------------------------------------------------------------
    DebuggerTerminalPOSIX::~DebuggerTerminalPOSIX()
    {
    }
    //-----------------------------------------------------------------------------
    void DebuggerTerminalPOSIX::Launch(const wxString &title)
    {
        m_title = title;
        wxString symblink;
        ::LaunchTerminalForDebugger(m_title, symblink, m_tty, m_pid);
        wxUnusedVar(symblink);

        if ( IsValid() ) {
            gsgs_Debug("DebuggerTerminalPOSIX successfully started. Process %d. TTY: %s", (int)m_pid, m_tty);
        }
    }
    //-----------------------------------------------------------------------------
    bool DebuggerTerminalPOSIX::IsValid() const
    {
    #ifdef __WXMAC__
        return !m_tty.IsEmpty();
    #else
        return m_pid != wxNOT_FOUND && !m_tty.IsEmpty();
    #endif
    }
    //-----------------------------------------------------------------------------
    void DebuggerTerminalPOSIX::Clear()
    {
    #ifndef __WXMSW__
        if ( m_pid != wxNOT_FOUND ) {
            // konsole and and its descendent qterminal hang on exit
            // That's because we made them call /bin/sleep, which becomes the process stored in m_pid
            // Killing 'sleep' makes other terminals self-close, but not these two
            // (At least for konsole, it displays "Warning: Program '/bin/sleep' crashed" instead)
            // I can't find any way to prevent this, so grab the real terminal pid so in this situation we can kill it too
            bool killParent(false); // There's no need to kill most terminals
            wxString command(wxString::Format("ps -o ppid= -p %i", (int)m_pid));
            wxString result = ProcUtils::SafeExecuteCommand(command);
            long parentID;
            if (result.Trim().ToLong(&parentID)) {
                wxString command(wxString::Format("ps -o command= -p %i", (int)parentID));
                wxString name = ProcUtils::SafeExecuteCommand(command);
                if (name.Contains("--separate") || name.Contains("qterminal")) {
                    killParent = true; // as it _is_ konsole, which will have been launched with --separate as an option, or qterminal
                }
            }

            // terminate the process
            ::wxKill(m_pid, wxSIGTERM);

            if (killParent) {
                ::wxKill(parentID, wxSIGTERM);
            }
        }
    #endif
        m_pid = wxNOT_FOUND;
        m_tty.Clear();
        m_title.Clear();
    }
    //-----------------------------------------------------------------------------
    wxString DebuggerTerminalPOSIX::MakeExeTitle(const wxString &exePath, const wxString &args)
    {
        return wxString(wxT("Debugging: ")) << exePath << wxT(" ") << args;
    }
    //-----------------------------------------------------------------------------
    wxString DebuggerTerminalPOSIX::MakeCoreTitle(const wxString &coreFile)
    {
        return wxString(wxT("Debugging core: ")) << coreFile;
    }
    //-----------------------------------------------------------------------------
    wxString DebuggerTerminalPOSIX::MakePidTitle(const int pid)
    {
        return wxString(wxT("Debugging console pid: ")) << pid;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // EditorStateLocker
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    EditorStateLocker::EditorStateLocker(wxStyledTextCtrl* ctrl)
        : m_ctrl(ctrl)
    {
        // store the first visible line
        m_firstVisibleLine = m_ctrl->GetFirstVisibleLine();
        m_position = m_ctrl->GetCurrentPos();

        // Store the bookmarks
        SerializeBookmarks();
        // Store breakpoints
        SerializeBreakpoints();
        // Store the folds
        SerializeFolds();
    }
    //-----------------------------------------------------------------------------------
    EditorStateLocker::~EditorStateLocker()
    {
        // restore the position.
        if(m_position > m_ctrl->GetLastPosition()) {
            m_position = m_ctrl->GetLastPosition();
        }

        // If the caret is out of screen, scroll the editor to make it visible again
        int caretLine = m_ctrl->LineFromPosition(m_position);
        if(caretLine < m_ctrl->GetFirstVisibleLine() ||
           (caretLine > (m_ctrl->GetFirstVisibleLine() + m_ctrl->LinesOnScreen()))) {
            // center the caret line
            m_ctrl->SetFirstVisibleLine(caretLine - (m_ctrl->LinesOnScreen() / 2));
        }

        m_ctrl->ClearSelections();
        m_ctrl->SetCurrentPos(m_position);
        m_ctrl->SetSelectionStart(m_position);
        m_ctrl->SetSelectionEnd(m_position);
        m_ctrl->EnsureVisible(m_ctrl->LineFromPosition(m_position));

        ApplyBookmarks();
        ApplyBreakpoints();
        ApplyFolds();
        
        m_ctrl->SetFirstVisibleLine(m_firstVisibleLine); // We must do this _after_ ApplyFolds() or the display may scroll down
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyBookmarks() 
    { 
        ApplyBookmarks(m_ctrl, m_bookmarks); 
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeBookmarks() 
    { 
        SerializeBookmarks(m_ctrl, m_bookmarks); 
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyFolds() 
    { 
        ApplyFolds(m_ctrl, m_folds); 
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeFolds() 
    { 
        SerializeFolds(m_ctrl, m_folds); 
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyBookmarks(wxStyledTextCtrl* ctrl, const wxArrayString& bookmarks)
    {
        for(size_t i = 0; i < bookmarks.GetCount(); i++) {
            // Unless this is an old file, each bookmark will have been stored in the form: "linenumber:type"
            wxString lineno = bookmarks.Item(i).BeforeFirst(':');
            long bmt = smt_bookmark1;
            wxString type = bookmarks.Item(i).AfterFirst(':');
            if(!type.empty()) {
                type.ToCLong(&bmt);
            }
            long line = 0;
            if(lineno.ToCLong(&line)) {
                ctrl->MarkerAdd(line, bmt);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeBookmarks(wxStyledTextCtrl* ctrl, wxArrayString& bookmarks)
    {
        for(int line = 0; (line = ctrl->MarkerNext(line, mmt_all_bookmarks)) >= 0; ++line) {
            for(int type = smt_FIRST_BMK_TYPE; type <= smt_LAST_BMK_TYPE; ++type) {
                int mask = (1 << type);
                if(ctrl->MarkerGet(line) & mask) {
                    // We need to serialise both the line and BM type. To keep things simple in sessionmanager, just merge
                    // their strings
                    bookmarks.Add(wxString::Format("%d:%d", line, type));
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyFolds(wxStyledTextCtrl* ctrl, const EditorStateLocker::VecInt_t& folds)
    {
        for(size_t i = 0; i < folds.size(); ++i) {
            int line = folds.at(i);
            // 'line' was collapsed when serialised, so collapse it now. That assumes that the line-numbers haven't changed
            // in the meanwhile.
            // If we cared enough, we could have saved a fold-level too, and/or the function name +/- the line's
            // displacement within the function. But for now...
            if(ctrl->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) {
    #if wxVERSION_NUMBER >= 3100
                ctrl->FoldLine(line, wxSTC_FOLDACTION_CONTRACT);
    #else
                if (ctrl->GetFoldExpanded(line)) { // For <wx3.1 check first, and only toggle if needed
                    ctrl->ToggleFold(line);
                }
    #endif
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeFolds(wxStyledTextCtrl* ctrl, EditorStateLocker::VecInt_t& folds)
    {
        for(int line = 0; line < ctrl->GetLineCount(); ++line) {
            if((ctrl->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) && (ctrl->GetFoldExpanded(line) == false)) {
                folds.push_back(line);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyBreakpoints(wxStyledTextCtrl* ctrl, const wxArrayString& breapoints)
    {
        for(size_t i = 0; i < breapoints.GetCount(); i++) {
            // Unless this is an old file, each bookmark will have been stored in the form: "linenumber:type"
            wxString lineno = breapoints.Item(i).BeforeFirst(':');
            long bmt = smt_bookmark1;
            wxString type = breapoints.Item(i).AfterFirst(':');
            if(!type.empty()) {
                type.ToCLong(&bmt);
            }
            long line = 0;
            if(lineno.ToCLong(&line)) {
                ctrl->MarkerAdd(line, bmt);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeBreakpoints(wxStyledTextCtrl* ctrl, wxArrayString& breapoints)
    {
        for(int line = 0; (line = ctrl->MarkerNext(line, mmt_all_breakpoints)) >= 0; ++line) {
            for(int type = smt_FIRST_BP_TYPE; type <= smt_LAST_BP_TYPE; ++type) {
                int mask = (1 << type);
                if(ctrl->MarkerGet(line) & mask) {
                    // We need to serialise both the line and BM type. To keep things simple in sessionmanager, just merge
                    // their strings
                    breapoints.Add(wxString::Format("%d:%d", line, type));
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::ApplyBreakpoints() 
    { 
        ApplyBreakpoints(m_ctrl, m_breakpoints); 
    }
    //-----------------------------------------------------------------------------------
    void EditorStateLocker::SerializeBreakpoints() 
    { 
        SerializeBreakpoints(m_ctrl, m_breakpoints); 
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // STCLineKeeper
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    STCLineKeeper::STCLineKeeper(IEditor* editor)
    {
        m_locker = new EditorStateLocker(editor->GetCtrl());
    }
    //-----------------------------------------------------------------------------------
    STCLineKeeper::STCLineKeeper(wxStyledTextCtrl* stc)
    {
        m_locker = new EditorStateLocker(stc);
    }
    //-----------------------------------------------------------------------------------
    STCLineKeeper::~STCLineKeeper()
    {
        wxDELETE(m_locker);
    }
    //-----------------------------------------------------------------------------------
}