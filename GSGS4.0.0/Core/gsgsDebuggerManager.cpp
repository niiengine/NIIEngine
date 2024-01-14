//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : debuggermanager.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "gsgsEvent.h"
#include "gsgsPathManager.h"
#include "gsgsConfig.h"
#include "gsgsDebuggerManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsMacroManager.h"
#include <wx/msgdlg.h>

namespace gsgs
{
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // DebuggerCmdData
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    DebuggerCmdData::DebuggerCmdData()
    {
    }
    //----------------------------------------------------------------------------
    DebuggerCmdData::~DebuggerCmdData()
    {
    }
    //----------------------------------------------------------------------------
    void DebuggerCmdData::DeSerialize(Serializer &arch)
    {
        arch.Read(wxT("m_name"), m_name);
        arch.Read(wxT("m_command"), m_command);
        if( arch.Read(wxT("m_dbgCommand"), m_dbgCommand) == false ) {
            m_dbgCommand = wxT("print");
        }
    }
    //----------------------------------------------------------------------------
    void DebuggerCmdData::Serialize(Serializer &arch)
    {
        arch.Write(wxT("m_name"), m_name);
        arch.Write(wxT("m_command"), m_command);
        arch.Write(wxT("m_dbgCommand"), m_dbgCommand);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // DebuggerPreDefinedTypes
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    DebuggerPreDefinedTypes::DebuggerPreDefinedTypes()
        : m_name(wxT("Default"))
        , m_active(true)
    {
    }
    //----------------------------------------------------------------------------
    DebuggerPreDefinedTypes::~DebuggerPreDefinedTypes()
    {
    }
    //----------------------------------------------------------------------------
    void DebuggerPreDefinedTypes::DeSerialize(Serializer &arch)
    {
        size_t size(0);
        arch.Read(wxT("m_name"),   m_name);
        arch.Read(wxT("m_active"), m_active);
        arch.Read(wxT("size"),     size);
        for(size_t i=0; i<size; i++) {
            wxString cmdname;
            cmdname << wxT("DebuggerCmd") << i;
            DebuggerCmdData cmdData;
            arch.Read(cmdname, &cmdData);
            m_cmds.push_back(cmdData);
        }
    }
    //----------------------------------------------------------------------------
    void DebuggerPreDefinedTypes::Serialize(Serializer &arch)
    {
        size_t size = m_cmds.size();
        arch.Write(wxT("m_name"),   m_name);
        arch.Write(wxT("m_active"), m_active);
        arch.Write(wxT("size"),     size);

        for(size_t i=0; i<m_cmds.size(); i++) {
            wxString cmdname;
            cmdname << wxT("DebuggerCmd") << i;
            arch.Write(cmdname, &m_cmds.at(i));
        }
    }
    //----------------------------------------------------------------------------
    wxString GetRealType(const wxString& gdbType)
    {
        wxString realType ( gdbType );
        realType.Replace(wxT("*"), wxT(""));
        realType.Replace(wxT("const"), wxT(""));
        realType.Replace(wxT("&"), wxT(""));
        
        // remove any template initialization:
        int depth(0);
        wxString noTemplateType;
        for(size_t i=0; i<realType.Length(); i++) {
            switch((wxChar)realType.GetChar(i)) {
            case wxT('<'):
                depth++;
                break;
            case wxT('>'):
                depth--;
                break;
            default:
                if(depth == 0)
                    noTemplateType << realType.GetChar(i);
                break;
            }
        }
        
        noTemplateType.Trim().Trim(false);
        return noTemplateType;
    }
    //----------------------------------------------------------------------------
    wxString DebuggerPreDefinedTypes::GetPreDefinedTypeForTypename(const wxString& expr, const wxString& name)
    {
        wxString realType = GetRealType( expr );
        for(size_t i=0; i<m_cmds.size(); i++) {
            DebuggerCmdData dcd = m_cmds.at(i);
            if(dcd.GetName() == realType) {
                // Create variable object for this variable
                // and display the content
                wxString expression = dcd.GetCommand();
                expression = MacroManager::Instance()->Replace(expression, wxT("variable"), name, true);
                return expression;
            }
        }
        return wxT("");
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // DebuggerSettingsPreDefMap
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    DebuggerSettingsPreDefMap::DebuggerSettingsPreDefMap()
    {
    }
    //----------------------------------------------------------------------------
    DebuggerSettingsPreDefMap::~DebuggerSettingsPreDefMap()
    {
    }
    //----------------------------------------------------------------------------
    void DebuggerSettingsPreDefMap::Serialize(Serializer& arch)
    {
        arch.Write(wxT("size"), m_cmds.size());
        std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iter = m_cmds.begin();
        size_t i(0);
        for(; iter != m_cmds.end(); iter++, i++) {
            wxString cmdname;
            cmdname << wxT("PreDefinedSet") << i;
            arch.Write(cmdname, (ObjectData*)&(iter->second));
        }
    }
    //----------------------------------------------------------------------------
    void DebuggerSettingsPreDefMap::DeSerialize(Serializer& arch)
    {
        size_t count(0);
        arch.Read(wxT("size"), count);
        m_cmds.clear();

        for(size_t i=0; i<count; i++) {
            wxString cmdname;
            cmdname << wxT("PreDefinedSet") << i;
            DebuggerPreDefinedTypes preDefSet;
            arch.Read(cmdname, &preDefSet);
            m_cmds[preDefSet.GetName()] = preDefSet;
        }
    }
    //----------------------------------------------------------------------------
    DebuggerPreDefinedTypes DebuggerSettingsPreDefMap::GetActiveSet() const
    {
        std::map<wxString, DebuggerPreDefinedTypes>::const_iterator iter = m_cmds.begin();
        for(; iter != m_cmds.end(); iter++) {
            if(iter->second.IsActive())
                return iter->second;
        }

        // no match, search for the one with the name 'Default'
        iter = m_cmds.begin();
        for(; iter != m_cmds.end(); iter++) {
            if(iter->second.GetName() == wxT("Default"))
                return iter->second;
        }

        // still no match
        // return the first entry
        if(m_cmds.empty() == false)
            return m_cmds.begin()->second;

        // no entries at all?
        return DebuggerPreDefinedTypes();
    }
    //----------------------------------------------------------------------------
    bool DebuggerSettingsPreDefMap::IsSetExist(const wxString& name)
    {
        return m_cmds.find(name) != m_cmds.end();
    }
    //----------------------------------------------------------------------------
    void DebuggerSettingsPreDefMap::SetActive(const wxString& name)
    {
        std::map<wxString, DebuggerPreDefinedTypes>::iterator iter = m_cmds.begin();
        for(; iter != m_cmds.end(); iter++) {
            m_cmds[iter->first].SetActive(iter->first == name ? true : false);
        }
    }
    //----------------------------------------------------------------------------
    static DebuggerMgr* gInstance = NULL;

    wxDEFINE_EVENT(wxEVT_DEBUGGER_UPDATE_VIEWS, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_QUERY_LOCALS, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_CHILDREN, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_VAROBJ_EVALUATED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_VAROBJECT_CREATED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_DISASSEBLE_CURLINE, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_QUERY_FILELINE, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_REGISTERS, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_LIST_FRAMES, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_DEBUGGER_FRAME_SELECTED, gsgs::Event);
    //----------------------------------------------------------------------------
    DebuggerMgr::DebuggerMgr() {}
    //----------------------------------------------------------------------------
    DebuggerMgr::~DebuggerMgr()
    {
        std::vector<gsgs::DynModule*>::iterator iter = m_dl.begin();
        for(; iter != m_dl.end(); iter++) {
            (*iter)->Detach();
            delete(*iter);
        }
        m_dl.clear();
        m_debuggers.clear();
    }
    //----------------------------------------------------------------------------
    DebuggerMgr& DebuggerMgr::Get()
    {
        if(!gInstance) { gInstance = new DebuggerMgr(); }
        return *gInstance;
    }
    //----------------------------------------------------------------------------
    void DebuggerMgr::Free()
    {
        delete gInstance;
        gInstance = NULL;
    }
    //----------------------------------------------------------------------------
    bool DebuggerMgr::LoadDebuggers()
    {
        wxString ext;

    #if defined(__WXMSW__)
        ext = wxT("dll");

    #elif defined(__WXMAC__)
        ext = wxT("dylib");

    #else
        ext = wxT("so");

    #endif

        wxString fileSpec(wxT("*.") + ext);

        // get list of dlls
        wxArrayString files;
    #ifdef __WXGTK__
        wxString debuggersPath(PLUGINS_DIR, wxConvUTF8);
        debuggersPath += wxT("/debuggers");
    #elif defined(__WXMSW__)
    #ifdef USE_POSIX_LAYOUT
        wxString debuggersPath(gsgs_Path().GetPluginsDirectory() + wxT("/debuggers"));
    #else
        wxString debuggersPath(m_baseDir + wxT("/debuggers"));
    #endif
    #else
        // OSX
        wxFileName debuggersFolder(gsgs_Path().GetDataDir(), "");
        debuggersFolder.AppendDir("debuggers");
        wxString debuggersPath(debuggersFolder.GetPath());
    #endif

        gsgsDEBUG() << "Loading debuggers from:" << debuggersPath;
        wxDir::GetAllFiles(debuggersPath, &files, fileSpec, wxDIR_FILES);

        for(size_t i = 0; i < files.GetCount(); i++) {
            gsgs::DynModule* dl = new gsgs::DynModule();
            wxString fileName(files.Item(i));
            
    #if defined(__WXMSW__) && CL_DEBUG_BUILD
            // Under MSW loading a release plugin while in debug mode will cause a crash
            if(!fileName.EndsWith("-dbg.dll")) { continue; }
    #elif defined(__WXMSW__)
            // filter debug plugins
            if(fileName.EndsWith("-dbg.dll")) { continue; }
    #endif
            gsgsDEBUG() << "Attempting to load debugger:" << fileName;
            if(!dl->Load(fileName)) {
                gsgs_Warning("Failed to load debugger: %s", fileName);
                if(!dl->GetError().IsEmpty()) { gsgs_Warning("%s", dl->GetError()); }
                wxDELETE(dl);
                continue;
            }

            bool success(false);
            GET_DBG_INFO_FUNC pfn = (GET_DBG_INFO_FUNC)dl->GetSymbol(wxT("GetDebuggerInfo"), &success);
            if(!success) {
                gsgsLogMessage(wxT("Failed to find GetDebuggerInfo() in dll: ") + fileName);
                if(!dl->GetError().IsEmpty()) { gsgsLogMessage(dl->GetError()); }
                // dl->Unload();
                delete dl;
                continue;
            }

            DebuggerInfo info = pfn();
            // Call the init method to create an instance of the debugger
            success = false;
            GET_DBG_CREATE_FUNC pfnInitDbg = (GET_DBG_CREATE_FUNC)dl->GetSymbol(info.initFuncName, &success);
            if(!success) {
                gsgsLogMessage(wxT("Failed to find init function in dll: ") + fileName);
                if(!dl->GetError().IsEmpty()) { gsgsLogMessage(dl->GetError()); }
                dl->Detach();
                delete dl;
                continue;
            }

            gsgsLogMessage(wxT("Loaded debugger: ") + info.name + wxT(", Version: ") + info.version);
            IDebugger* dbg = pfnInitDbg();

            // set the environment
            dbg->SetEnvironment(m_env);

            m_debuggers[info.name] = dbg;

            // keep the dynamic load library
            m_dl.push_back(dl);
        }

        // Load all debuggers in the form of plugin (i.e. they dont implement the IDebugger interface)
        // and append them to a special list
        gsgs::DebugEvent queryPlugins(wxEVT_DBG_IS_PLUGIN_DEBUGGER);
        gsgs_Event().ProcessEvent(queryPlugins);
        m_pluginsDebuggers.swap(queryPlugins.GetStrings());
        return true;
    }
    //----------------------------------------------------------------------------
    wxArrayString DebuggerMgr::GetAvailableDebuggers()
    {
        wxArrayString dbgs;
        std::map<wxString, IDebugger*>::iterator iter = m_debuggers.begin();
        for(; iter != m_debuggers.end(); iter++) {
            dbgs.Add(iter->first);
        }

        // append all the plugins that were registered themself as debugger
        dbgs.insert(dbgs.end(), m_pluginsDebuggers.begin(), m_pluginsDebuggers.end());
        return dbgs;
    }
    //----------------------------------------------------------------------------
    IDebugger* DebuggerMgr::GetActiveDebugger()
    {
        if(m_activeDebuggerName.IsEmpty()) {
            // no active debugger is set, use the first one
            std::map<wxString, IDebugger*>::const_iterator iter = m_debuggers.begin();
            if(iter != m_debuggers.end()) {
                SetActiveDebugger(iter->first);
                return iter->second;
            }
            return NULL;
        }

        std::map<wxString, IDebugger*>::iterator iter = m_debuggers.find(m_activeDebuggerName);
        if(iter != m_debuggers.end()) { return iter->second; }
        return NULL;
    }
    //----------------------------------------------------------------------------
    void DebuggerMgr::SetActiveDebugger(const wxString& name) 
    { 
        m_activeDebuggerName = name; 
    }
    //----------------------------------------------------------------------------
    void DebuggerMgr::SetDebuggerInformation(const wxString& name, const DebuggerInformation& info)
    {
        gsgs_Config().Write(name, (ObjectData*)&info);
    }
    //----------------------------------------------------------------------------
    bool DebuggerMgr::GetDebuggerInformation(const wxString& name, DebuggerInformation& info)
    {
        return gsgs_Config().Read(name, &info);
    }
    //----------------------------------------------------------------------------
    bool DebuggerMgr::IsNativeDebuggerRunning() const
    {
        std::map<wxString, IDebugger*>::const_iterator iter = m_debuggers.find(m_activeDebuggerName);
        if(iter == m_debuggers.end()) { return false; }

        IDebugger* d = iter->second;
        return d && d->IsRunning();
    }
    //----------------------------------------------------------------------------
}