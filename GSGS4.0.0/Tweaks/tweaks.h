//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tweaks.h
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

#ifndef __Tweaks__
#define __Tweaks__

#include "gsgsEvent.h"
#include "plugin.h"
#include "tweaks_settings.h"
#include <map>

class Tweaks : public IPlugin
{
    typedef std::map<wxString, int> ProjectIconMap_t;
    TweaksSettings m_settings;
    ProjectIconMap_t m_project2Icon;

protected:
    IEditor* FindEditorByPage(wxWindow* page);

public:
    Tweaks(IManager* manager);
    ~Tweaks();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    // Event handlers
    void OnSettings(wxCommandEvent& e);
    void OnColourTab(gsgsColourEvent& e);
    void OnWorkspaceLoaded(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& e);
    void OnFileViewBuildTree(gsgs::Event& e);
    void OnCustomizeProject(gsgsColourEvent& e);
    void OnTabBorderColour(gsgsColourEvent& e);
};

#endif // Tweaks
