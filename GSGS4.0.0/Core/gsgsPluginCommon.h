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
#ifndef _GSGS_Plugin_Common_h
#define _GSGS_Plugin_Common_h

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"
#include "gsgsException.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include <wx/dnd.h>
#include <wx/treectrl.h>

namespace gsgs
{
    class _gsgsAPI TerminalHistory
    {
    public:
        TerminalHistory();
        virtual ~TerminalHistory();

        void Add(const wxString& command);
        const wxString& ArrowUp();
        const wxString& ArrowDown();
        wxArrayString GetItems() const;
        void SetItems(const wxArrayString& items);
    protected:
        void DoReset()          { m_where = -1; }
    private:
        std::vector<wxString> m_history;
        int m_where;
    };

    class _gsgsAPI ConsoleFinder
    {
    public:
        ConsoleFinder();
        virtual ~ConsoleFinder();

        bool FindConsole(const wxString &title, wxString &consoleName);
        void FreeConsole();
        wxString GetConsoleName();

        void SetConsoleCommand(const wxString& cmd) {
            m_consoleCommand = cmd;
        }
        const wxString& GetConsoleCommand() const {
            return m_consoleCommand;
        }
    private:
        int RunConsole(const wxString &title);
        wxString GetConsoleTty(int ConsolePid);
    private:
        wxString  m_ConsoleTty;
        int       m_nConsolePid;
        wxString  m_consoleCommand;
    };

    class _gsgsAPI WindowUpdateLocker 
    {
    public:
        WindowUpdateLocker(wxWindow *win) : m_win(win) {
    #if wxVERSION_NUMBER < 2900
        #ifndef __WXGTK__
            m_win->Freeze();
        #endif
    #endif
        }
        
        ~WindowUpdateLocker() {
    #if wxVERSION_NUMBER < 2900
        #ifndef __WXGTK__
            m_win->Thaw();
        #endif
    #endif
        }
    private:
        wxWindow *m_win;
    };

    class _gsgsAPI TreeAccess
    {
    public:
        TreeAccess(wxTreeCtrl *tree);
        virtual ~TreeAccess();
        
        /**
         * @brief traverse the tree starting with given 'item'
         * If item is invalid, the traverse will start from the root item
         * @param item
         */
        virtual void Traverse(const wxTreeItemId &item = wxTreeItemId());
        
        /**
         * @brief this method is called by the traverser for every item in the tree
         * By default this method does nothing. Override it to do something meaningfull
         * @param item current item
         */
        virtual void OnItem(const wxTreeItemId& item);
    protected:
        void DoTraverse(const wxTreeItemId &item);
    protected:
        wxTreeCtrl *m_tree;
    };

    class _gsgsAPI QuickDebugInfo : public ObjectData
    {
    public:
        QuickDebugInfo();
        ~QuickDebugInfo();

        virtual void DeSerialize(Serializer& arch);
        virtual void Serialize(Serializer& arch);

        void SetArguments(const wxString& arg)              { m_arguments = arg; }
        void SetExeFilepaths(const wxArrayString& path)     { m_exeFilepaths = path; }
        void SetSelectedDbg(int select)                     { m_selectedDbg = select; }
        void SetStartCmds(const wxArrayString& cmd)         { m_startCmds = cmd; }
        void SetWDs(const wxArrayString& wds)               { m_wds = wds; }
        void SetAlternateDebuggerExec(const wxString& cmd)  { m_alternateDebuggerExec = cmd; }

        const wxString& GetArguments() const                { return m_arguments; }
        const wxArrayString& GetExeFilepaths() const        { return m_exeFilepaths; }
        const int& GetSelectedDbg() const                   { return m_selectedDbg; }
        const wxArrayString& GetStartCmds() const           { return m_startCmds; }
        const wxArrayString& GetWds() const                 { return m_wds; }
        const wxString& GetAlternateDebuggerExec() const    { return m_alternateDebuggerExec; }
    private:
        wxArrayString m_exeFilepaths;
        wxArrayString m_wds;
        wxString m_arguments;
        wxArrayString m_startCmds;
        int m_selectedDbg;
        wxString m_alternateDebuggerExec;
    };

    class _gsgsAPI FileOrFolderDropTarget : public wxFileDropTarget
    {
        wxEvtHandler* m_sink;
    public:
        FileOrFolderDropTarget(wxEvtHandler* eventSink);
        virtual ~FileOrFolderDropTarget();
        virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    };

    class _gsgsAPI ExeLocator 
    {
    public:
        ExeLocator (){}
        ~ExeLocator (){}

        static bool locate(const wxString &name, wxString &where);
    };

    class _gsgsAPI wxMD5
    {
    public:
        wxMD5();
        wxMD5(const wxString& szText);
        wxMD5(const wxFileName& filename);
        virtual ~wxMD5();

        // Other Methods
        void SetText(const wxString& szText);

        const wxString GetDigest();

        // Static Methods
        static const wxString GetDigest(const wxString& szText);
        static const wxString GetDigest(const wxFileName& filename);

    protected:
        wxString m_szText;
    };

    class _gsgsAPI FolderColour
    {
    public:
        typedef std::list<FolderColour> List_t;
        typedef std::map<wxString, FolderColour> Map_t;
    public:
        FolderColour();
        FolderColour(const wxString& path, const wxColour& colour)
            : m_path(path)
            , m_colour(colour)
        {
        }
        virtual ~FolderColour();

        bool IsOk() const                                   { return !m_path.IsEmpty(); }
        void SetColour(const wxColour& colour)              {  m_colour = colour; }
        void SetPath(const wxString& path)                  {  m_path = path; }
        const wxColour& GetColour() const                   { return m_colour; }
        const wxString& GetPath() const                     { return m_path; }

        /**
         * @brief sort map and store the result in a list
         */
        static void SortToList(const FolderColour::Map_t& m, FolderColour::List_t& l);

        /**
         * @brief Search the list for best match for a given path
         */
        static const FolderColour& FindForPath(const FolderColour::List_t& sortedList, const wxString& path);
    private:
        wxString m_path;
        wxColour m_colour;
    };

    class _gsgsAPI DebuggerTerminalPOSIX
    {
    public:
        DebuggerTerminalPOSIX();
        virtual ~DebuggerTerminalPOSIX();

        /**
         * @brief launch the terminal. Check the IsValid() after this call
         */
        void Launch(const wxString &title);

        /**
         * @brief is this a valid terminal?
         * @return
         */
        bool IsValid() const;

        /**
         * @brief return the associated pty for this terminal
         */
        const wxString & GetTty() const { return m_tty; }

        /**
         * @brief terminate the process associated with the terminal and clear the object
         * making it ready for another use
         */
        void Clear();

        //! Make a console title for debugging an exe.
        static wxString MakeExeTitle(const wxString &exePath, const wxString &args);

        //! Make a console title for debugging a core file.
        static wxString MakeCoreTitle(const wxString &coreFile);

        //! Make a console title for debugging a running process.
        static wxString MakePidTitle(const int pid);
    private:
        wxString m_title;
        wxString m_tty;
        long     m_pid;
    };

    class _gsgsAPI Patch
    {
    public:
        Patch();
        virtual ~Patch();

        /**
         * @brief apply patch sepcified by 'pathFile' onto filename file
         * @param filename
         * @param workingDirectory execute the patch command from this folder
         * @throws Exception on error
         */
        void patch(const wxFileName& patchFile, const wxString& workingDirectory = "", const wxString& args = "") ;
    private:
        wxFileName m_patch;
    };

    class _gsgsAPI EditorStateLocker
    {
    public:
        typedef std::vector<int> VecInt_t;
    public:
        EditorStateLocker(wxStyledTextCtrl* ctrl);
        virtual ~EditorStateLocker();

        /**
         * @brief serialize the editor bookmarks into an array
         * @param bookmarks [output]
         */
        static void SerializeBookmarks(wxStyledTextCtrl* ctrl, wxArrayString& bookmarks);
        /**
         * @brief apply bookmarks to the editor (serialized by SerializeBookmarks earlier)
         * @param bookmarks serialized bookmarks
         */
        static void ApplyBookmarks(wxStyledTextCtrl* ctrl, const wxArrayString& bookmarks);

        /**
         * Apply collapsed folds from a vector
         */
        static void ApplyFolds(wxStyledTextCtrl* ctrl, const EditorStateLocker::VecInt_t& folds);

        /**
         * Store any collapsed folds to a vector, so they can be serialised
         */
        static void SerializeFolds(wxStyledTextCtrl* ctrl, EditorStateLocker::VecInt_t& folds);
        
        /**
         * @brief serialize Breakpoints markers into an array
         */
        static void SerializeBreakpoints(wxStyledTextCtrl* ctrl, wxArrayString &breapoints);
        
        /**
         * @brief apply breapoints markers
         */
        static void ApplyBreakpoints(wxStyledTextCtrl* ctrl, const wxArrayString &breapoints);
    private:
        void SerializeBookmarks();
        void SerializeFolds();
        void SerializeBreakpoints();
        
        void ApplyBookmarks();
        void ApplyFolds();
        void ApplyBreakpoints();
    private:
        EditorStateLocker::VecInt_t m_folds;
        // The editor
        wxStyledTextCtrl* m_ctrl;

        // States:
        // Visible line
        int m_firstVisibleLine;

        // Bookrmarks
        wxArrayString m_bookmarks;
        
        // Breakpoints
        wxArrayString m_breakpoints;
        
        // Caret position
        int m_position;
    };

    class _gsgsAPI STCLineKeeper
    {
    public:
        STCLineKeeper(IEditor* editor);
        STCLineKeeper(wxStyledTextCtrl* stc);
        virtual ~STCLineKeeper();
    private:
        EditorStateLocker * m_locker;
    };
}
#endif
