//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : externaltools.h
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

#ifndef __ExternalTools__
#define __ExternalTools__

#include "plugin.h"
#include "externaltoolsdata.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"

class ExternalToolsPlugin : public Plugin
{
    wxEvtHandler* topWin;
    wxMenu* m_parentMenu;
    ExternalToolsData m_externalTools;
    
protected:
    void OnSettings(wxCommandEvent& e);
    void OnShowRunningTools(wxCommandEvent& e);
    void DoRecreateToolbar();
    void DoCreatePluginMenu();
    void OnRecreateTB();
    void OnLaunchExternalTool(wxCommandEvent& e);

    void OnFileSave(gsgs::Event& event);

public:
    ExternalToolsPlugin(IManager* manager);
    ~ExternalToolsPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();
};

#endif // ExternalTools
