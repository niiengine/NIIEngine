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

#ifndef __wxterminal__
#define __wxterminal__

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"

#include <wx/intl.h>

#include <wx/colour.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include <wx/textctrl.h>

namespace gsgs
{
    class TerminalHistory;
}
class IProcess;

wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_EXECUTE_COMMAND,  gsgs::Event);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_CTRL_C,           gsgs::Event);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_KILL_INFERIOR,    gsgs::Event);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_EXIT_WHEN_DONE,   gsgs::Event);

class _gsgsAPI wxTerminal : public wxPanel
{
public:
    wxTerminal(wxWindow* parent);
    virtual ~wxTerminal();

    void Focus() { m_textCtrl->SetFocus(); }
    void SetInteractive(bool interactive) { this->m_interactive = interactive; }
    bool IsInteractive() const { return m_interactive; }
    void Execute(const wxString& command, bool exitWhenDone = false, const wxString& workingDir = wxT(""));
    void KillInferior();
    bool IsRunning();
    void Clear();
    bool IsEmpty() const { return m_textCtrl->IsEmpty(); }
    // Give access to the text control
    wxStyledTextCtrl* GetCtrl() { return m_textCtrl; }

    // Add text with the EOL at the end
    void AddTextWithEOL(const wxString& text);
    void AddTextRaw(const wxString& text);

#if defined(__WXGTK__) || defined(__WXMAC__)
    wxString StartTTY();
    wxString GetTTY() const { return m_tty; }
    void StopTTY();
#endif
    void CaretToEnd();
protected:
    // Handlers for wxTerminalBase events.
    virtual void OnReadProcessOutput(gsgs::ProcessEvent& event);
    virtual void OnProcessEnd(gsgs::ProcessEvent& event);
    virtual void OnIdle(wxIdleEvent& event);

    // Edit events
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    // Keys
    void OnDown(wxKeyEvent& event);
    void OnUp(wxKeyEvent& event);
    void OnLeft(wxKeyEvent& event);
    void OnRight(wxKeyEvent& event);
    void OnEnter();
    void OnCtrlC(wxKeyEvent& event);

    // Theme
    void OnTheme(wxCommandEvent& event);

    void OnCharAdded(wxStyledTextEvent& event);
    void AddMarker();
protected:
    void DoProcessCommand(const wxString& command);
    void DoCtrlC();
    void DoFlushOutputBuffer();
    void InsertCommandText(const wxString& command);
    wxString GetCommandText();
    void OnKey(wxKeyEvent& event);
protected:
    wxStyledTextCtrl* m_textCtrl;
    IProcess* m_process;
    wxString m_workingDir;
    bool m_exitWhenProcessDies;
    bool m_exitOnKey;
#if defined(__WXGTK__) || defined(__WXMAC__)
    wxString m_tty;
    IProcess* m_dummyProcess;
    int m_slave;
#endif
    bool m_interactive;
    wxString m_outputBuffer;
    gsgs::TerminalHistory* m_history;
};

#endif // __wxterminal__
