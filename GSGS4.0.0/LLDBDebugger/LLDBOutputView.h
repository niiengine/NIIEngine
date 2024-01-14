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

#ifndef LLDBBREAKPOINTSPANE_H
#define LLDBBREAKPOINTSPANE_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "LLDBProtocol/LLDBConnector.h"

class LLDBDebugger;
class LLDBPlugin;
class LLDBOutputView : public LLDBOutputViewBase
{
    LLDBPlugin *m_plugin;
    LLDBConnector* m_connector;
    
public:
    LLDBOutputView(wxWindow* parent, LLDBPlugin *plugin);
    virtual ~LLDBOutputView();
    
    void Initialize();
    void Clear();
    
    // Event handlers
    void OnBreakpointsUpdated(LLDBEvent &event);
    void OnConsoleOutput(LLDBEvent &event);
    void OnLLDBStarted(LLDBEvent &event);
    LLDBBreakpoint::Ptr_t GetBreakpoint(const wxTreeItemId& item);
protected:
    virtual void OnBpActivated(wxTreeEvent& event);
    virtual void OnSendCommandToLLDB(wxCommandEvent& event);
    void GotoBreakpoint(LLDBBreakpoint::Ptr_t bp);
    
protected:
    virtual void OnDeleteAll(wxCommandEvent& event);
    virtual void OnDeleteAllUI(wxUpdateUIEvent& event);
    virtual void OnDeleteBreakpoint(wxCommandEvent& event);
    virtual void OnDeleteBreakpointUI(wxUpdateUIEvent& event);
    virtual void OnNewBreakpoint(wxCommandEvent& event);
    virtual void OnNewBreakpointUI(wxUpdateUIEvent& event);
    
    void OnSelectAll(wxCommandEvent &event);
    void OnCopy(wxCommandEvent &event);
};

#endif // LLDBBREAKPOINTSPANE_H
