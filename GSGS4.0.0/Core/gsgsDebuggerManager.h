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
#ifndef DEBUGGER_MANAGER_H
#define DEBUGGER_MANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"
#include "debugger.h"
#include "plugin.h"

namespace gsgs
{
    class BreakPointManager;
    
    // sent when a "QueryLocals" command is completed (only for locals - not for function arguments)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_QUERY_LOCALS, gsgs::Event);

    // sent when a variable object createion is completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_VAROBJECT_CREATED, gsgs::Event);

    // sent by gsgs when a pane is needed to refresh its content
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_UPDATE_VIEWS, gsgs::Event);

    // sent by the debugger when a "ListChildren" command is completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_LIST_CHILDREN, gsgs::Event);

    // sent by the debugger after a successfull evaluation of variable object
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_VAROBJ_EVALUATED, gsgs::Event);

    // sent by the debugger when a "disasseble" command returned
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, gsgs::Event);

    // sent by the debugger when a "disasseble" current line command returns
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_DISASSEBLE_CURLINE, gsgs::Event);

    // sent by the debugger when a "QueryFileLine" command has completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_QUERY_FILELINE, gsgs::Event);

    // sent by the debugger in case "ResolveType" command failed (i.e. gdb could not resolve its type)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, gsgs::Event);

    // sent by the debugger when 'ListRegisters' function completed
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_LIST_REGISTERS, gsgs::Event);

    // Call stack
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_LIST_FRAMES, gsgs::Event);

    // frame selected (user double clicked a stack entry)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DEBUGGER_FRAME_SELECTED, gsgs::Event);

    class _gsgsAPI DebuggerCmdData : public ObjectData
    {
    public:
        DebuggerCmdData();
        virtual ~DebuggerCmdData();

        void Serialize(Serializer &arhc);
        void DeSerialize(Serializer &arhc);

        //Setters
        inline void SetCommand(const wxString& command) {
            this->m_command = command;
        }

        inline void SetName(const wxString& name) {
            this->m_name = name;
        }

        //Getters
        inline const wxString& GetCommand() const {
            return m_command;
        }
        inline const wxString& GetName() const {
            return m_name;
        }

        inline void SetDbgCommand(const wxString& dbgCommand) {
            this->m_dbgCommand = dbgCommand;
        }
        inline const wxString& GetDbgCommand() const {
            return m_dbgCommand;
        }
    private:
        wxString m_name;
        wxString m_command;
        wxString m_dbgCommand;
    };

    typedef std::vector<DebuggerCmdData> DebuggerCmdDataVec;

    class _gsgsAPI DebuggerPreDefinedTypes : public ObjectData
    {
    public:
        DebuggerPreDefinedTypes();
        virtual ~DebuggerPreDefinedTypes();

        void Serialize(Serializer &arch);
        void DeSerialize(Serializer &arch);

        inline void SetCmds(const DebuggerCmdDataVec& cmds) {
            this->m_cmds = cmds;
        }

        inline const DebuggerCmdDataVec& GetCmds() const {
            return m_cmds;
        }
        inline void SetName(const wxString& name) {
            this->m_name = name;
        }
        inline const wxString& GetName() const {
            return m_name;
        }

        inline void SetActive(bool active) {
            this->m_active = active;
        }
        inline bool IsActive() const {
            return m_active;
        }
        
        wxString GetPreDefinedTypeForTypename(const wxString& expr, const wxString& name);
    private:
        DebuggerCmdDataVec m_cmds;
        wxString           m_name;
        bool               m_active;
    };

    class _gsgsAPI DebuggerSettingsPreDefMap : public ObjectData
    {
    public:

        DebuggerSettingsPreDefMap();
        virtual ~DebuggerSettingsPreDefMap();

        void Serialize(Serializer &arch);
        void DeSerialize(Serializer &arch);

        inline void SePreDefinedTypesMap(const std::map<wxString, DebuggerPreDefinedTypes>& cmds) {
            this->m_cmds = cmds;
        }

        inline const std::map<wxString, DebuggerPreDefinedTypes>& GePreDefinedTypesMap() const {
            return m_cmds;
        }

        /**
         * @brief return the active set, in this context it means return the one with the name 'Default'
         */
        DebuggerPreDefinedTypes GetActiveSet() const;
        bool                    IsSetExist(const wxString &name);
        void                    SetActive(const wxString &name);
    private:
        std::map<wxString, DebuggerPreDefinedTypes> m_cmds;
    };

    class _gsgsAPI DebuggerMgr
    {
    public:
        /**
         * Set the base dir for the debugger manager. On Linux this is
         * equivalent to $(HOME)/.liteeditor/, and on windows it is set
         * to C:\Program Files\LiteEditor\
         */
        inline void Initialize(wxEvtHandler* parent, EditorConfig* env, const wxString& dir)
        {
            m_baseDir = dir;
            m_env = env;
        }

        inline const wxString& GetActiveDebuggerName() const { return m_activeDebuggerName; }
        /**
         * Load all available debuggers. This functions searches for dll/so/sl
         * which are located udner $(HOME)/.liteeditor/debuggers/ on Linux, and on Windows
         * under C:\Program Files\LiteEditor\debuggers\
         */
        bool LoadDebuggers();

        /**
         * Return list of all available debuggers which were loaded
         * successfully into the debugger manager
         */
        wxArrayString GetAvailableDebuggers();

        /**
         * Set the active debugger to be 'name'. If a debugger with name does not
         * exist, this function does nothing
         */
        void SetActiveDebugger(const wxString& name);

        /**
         * Return the currently selected debugger. The debugger is selected
         * based on previous call to SetActiveDebugger(). If no active debugger is
         * set, this function may return NULL
         */
        IDebugger* GetActiveDebugger();

        // get/set debugger information
        void SetDebuggerInformation(const wxString& name, const DebuggerInformation& info);
        bool GetDebuggerInformation(const wxString& name, DebuggerInformation& info);

        /**
         * @brief do we have an active debugger which is running?
         */
        bool IsNativeDebuggerRunning() const;
        static DebuggerMgr & Get();
        static void Free();

    protected:
        DebuggerMgr();
        virtual ~DebuggerMgr();

    private:
        std::map<wxString, IDebugger*> m_debuggers;
        wxArrayString m_pluginsDebuggers;
        wxString m_baseDir;
        std::vector<DynModule*> m_dl;
        wxString m_activeDebuggerName;
        EditorConfig* m_env;
    };
}
#endif
