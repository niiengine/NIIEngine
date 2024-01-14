//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : webtools.h
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

#ifndef __WebTools__
#define __WebTools__

#include "plugin.h"
#include "JavaScriptSyntaxColourThread.h"
#include "gsgsEvent.h"
#include "JSCodeCompletion.h"
#include <wx/timer.h>
#include "ieditor.h"
#include "XMLCodeCompletion.h"
#include "CSSCodeCompletion.h"
#include "gsgsCommonHandler.h"

class NodeDebuggerPane;
class NodeJSWorkspaceView;
class JavaScriptSyntaxColourThread;
class WebTools : public Plugin
{
    friend class JavaScriptSyntaxColourThread;

    JavaScriptSyntaxColourThread* m_jsColourThread;
    JSCodeCompletion::Ptr_t m_jsCodeComplete;
    XMLCodeCompletion::Ptr_t m_xmlCodeComplete;
    CSSCodeCompletion::Ptr_t m_cssCodeComplete;
    gsgs::JSCTags::Ptr_t m_jsctags;
    time_t m_lastColourUpdate;
    wxTimer* m_timer;

    /// Node.js
    bool m_clangOldFlag;
    NodeDebuggerPane* m_nodejsCliDebuggerPane = nullptr;
    wxString m_savePerspective;

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceLoaded(wxCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void DoRefreshColours(const wxString& filename);
    void OnFileLoaded(gsgs::Event& event);
    void OnEditorContextMenu(ContextMenuEvent& event);
    void OnFileSaved(gsgs::Event& event);
    void OnTheme(wxCommandEvent& event);
    void OnCodeComplete(CodeCompleteEvent& event);
    void ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply);
    void OnSettings(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnCommentLine(wxCommandEvent& e);
    void OnCommentSelection(wxCommandEvent& e);
    void OnNodeJSCliDebuggerStarted(gsgs::DebugEvent& event);
    void OnNodeJSDebuggerStopped(gsgs::DebugEvent& event);
    void OnIsDebugger(gsgs::DebugEvent& event);
    void OnNodeCommandCompleted(gsgs::ProcessEvent& event);

public:
    bool IsJavaScriptFile(const wxString& filename);
    bool IsJavaScriptFile(const wxFileName& filename);
    bool IsJavaScriptFile(IEditor* editor);
    bool IsHTMLFile(IEditor* editor);
    bool IsCSSFile(IEditor* editor);
    bool InsideJSComment(IEditor* editor);
    bool InsideJSString(IEditor* editor);
    void EnsureAuiPaneIsVisible(const wxString& paneName, bool update);

public:
    WebTools(IManager* manager);
    ~WebTools();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // WebTools
