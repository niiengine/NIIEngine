//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : symbolview.h
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

#ifndef __SymbolView__
#define __SymbolView__

#include "gsgsPreInclude.h"
#include "gsgsWindowManager.h"
#include "globals.h"
#include "plugin.h"
#include <wx/choice.h>
#include "outline_tab.h"
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/splitter.h>

class SymbolViewPlugin : public Plugin
{
protected:
    OutlineTab* m_view;

protected:
    bool IsPaneDetached();
    int DoFindTabIndex();
    void OnToggleTab(gsgs::Event& event);
    void OnPageChanged(wxBookCtrlEvent& e);

public:
    //--------------------------------------------
    // Constructors/Destructors
    //--------------------------------------------
    SymbolViewPlugin(IManager* manager);
    ~SymbolViewPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};
#endif // Outline
