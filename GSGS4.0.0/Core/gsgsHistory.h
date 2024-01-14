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

#ifndef UNREDOBASE_H
#define UNREDOBASE_H

#include "gsgsPreInclude.h"
#include <wx/bitmap.h>
#include <wx/aui/auibar.h>
#include <wx/gdicmn.h>

namespace gsgs
{
    enum CLC_types { CLC_insert, CLC_delete, CLC_unknown };

    class _gsgsAPI Command
    {
    public:
        typedef wxSharedPtr<Command> Ptr_t;
        typedef std::vector<Command::Ptr_t> Vec_t;

        Command(CLC_types type = CLC_unknown, const wxString& name = "")
            : m_commandType(type)
            , m_commandName(name)
            , m_isOpen(true)
        {
        }
        virtual ~Command() {}

        inline int GetCommandType() const                       { return m_commandType; }
        inline void SetName(const wxString& name)               { m_commandName = name; }
        inline const wxString & GetName() const                 { return m_commandName; }
        inline void SetText(const wxString& text)               { m_text = text; }
        inline const wxString& GetText() const                  { return m_text; }
        inline bool IsOpen() const                              { return m_isOpen; }
        inline void Close()                                     { m_isOpen = false; }
        inline void SetUserLabel(const wxString& label)         { m_userLabel = label; }
        inline const wxString & GetUserLabel() const            { return m_userLabel; }
        inline void AppendText(const wxString& text, int position) { m_text << text; }

        virtual bool GetIsAppendable() const = 0;
        virtual bool Do()                                       { return true; }
        virtual bool Undo()                                     { return true; }

    protected:
        const int m_commandType;
        wxString m_commandName;
        wxString m_text;
        wxString m_userLabel;
        bool m_isOpen;
    };

    class _gsgsAPI CommandProcessorBase : public wxEvtHandler
    {
    public:
        CommandProcessorBase();
        virtual ~CommandProcessorBase();

        inline bool CanAppend(CLC_types type)
        {
            return GetOpenCommand()->GetIsAppendable() && GetOpenCommand()->GetCommandType() == type;
        }

        virtual void ProcessOpenCommand();

        void PopulateUnRedoMenu(gsgs::ToolBar* tb, wxWindowID toolId);
        virtual void DoPopulateUnRedoMenu(wxMenu& menu, bool undoing);

        void PrepareLabelledStatesMenu(wxMenu * menu);
        void PopulateLabelledStatesMenu(wxMenu * menu);

        void UnBindLabelledStatesMenu(wxMenu * menu);
        void DoUnBindLabelledStatesMenu(wxMenu * menu);

        inline Command::Ptr_t GetInitialCommand() const      { return m_initialCommand; }

        inline int GetCurrentCommand() const                   { return m_currentCommand; }

        void IncrementCurrentCommand();
        void DecrementCurrentCommand();

        inline int GetNextUndoCommand() const                  { return GetCommands().size() - m_currentCommand - 1; }

        inline void Add(Command::Ptr_t command)
        {
            m_commands.push_back(command);
            m_currentCommand = m_commands.size() - 1;
        }

        Command::Ptr_t GetOpenCommand();
        inline bool HasOpenCommand()                           { return (GetOpenCommand() != NULL); }

        virtual void CloseOpenCommand();

        inline Command::Vec_t& GetCommands()                 { return m_commands; }

        inline const Command::Vec_t& GetCommands() const     { return m_commands; }

        void OnTBUnRedo(wxCommandEvent& event);

        virtual bool DoUndo() = 0;

        virtual bool DoRedo() = 0;

        inline bool CanUndo() const                            { return (GetCurrentCommand() > -1); }

        inline bool CanRedo() const
        {
            return (GetCommands().size() > 0) && (GetCurrentCommand() < (int)(GetCommands().size() - 1));
        }

        inline void ClearRedos()
        { // Remove any redos invalidated by a new addition
            while(GetCurrentCommand() < (int)(GetCommands().size() - 1)) {
                GetCommands().pop_back();
            }
        }

        void SetUserLabel(const wxString& label);

        Command::Ptr_t GetActiveCommand() const; // The command indexed by m_currentCommand

        virtual void OnUndoDropdownItem(wxCommandEvent& event);
        virtual void OnRedoDropdownItem(wxCommandEvent& event);

        inline void Clear()
        {
            m_commands.clear();
            m_initialCommand = NULL;
        }

    protected:
        virtual void OnLabelledStatesMenuItem(wxCommandEvent& event);

        Command::Ptr_t m_initialCommand; // A command to hold any initial-state user-label, and to store any initial state
                                           // if we're state-storing
        Command::Vec_t m_commands;
        int m_currentCommand; // The next one to be undone
    };
}
#endif
