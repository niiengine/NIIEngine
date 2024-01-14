//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : continuousbuild.h
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

#ifndef __ContinuousBuild__
#define __ContinuousBuild__

#include "plugin.h"
#include "buildprocess.h"
#include "gsgsEvent.h"
#include "gsgsCommonHandler.h"

class wxEvtHandler;
class ContinousBuildPane;
class ShellCommand;

class ContinuousBuild : public Plugin
{
    ContinousBuildPane* m_view;
    wxEvtHandler* m_topWin;
    BuildProcess m_buildProcess;
    wxArrayString m_files;
    bool m_buildInProgress;
    gsgs::TabTogglerHelper::Ptr_t m_tabHelper;
    
public:
    void DoBuild(const wxString& fileName);

public:
    ContinuousBuild(IManager* manager);
    ~ContinuousBuild();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void HookPopupMenu(wxMenu* menu, MenuType type);
    virtual void UnPlug();

    void StopAll();

    void OnFileSaved(gsgs::Event& e);
    void OnIgnoreFileSaved(wxCommandEvent& e);
    void OnStopIgnoreFileSaved(wxCommandEvent& e);
    void OnBuildProcessEnded(gsgs::ProcessEvent& e);
    void OnBuildProcessOutput(gsgs::ProcessEvent& e);
};

#endif // ContinuousBuild
