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
#ifndef gsgsSPLITTERWINDOW_H
#define gsgsSPLITTERWINDOW_H

#include "gsgsPreInclude.h"
#include <wx/splitter.h>
#include "gsgsEvent.h"

namespace gsgs
{
    class _gsgsAPI SplitterWindow : public wxSplitterWindow
    {
    public:
        SplitterWindow() {}
        SplitterWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize, long style = wxSP_3DBORDER,
                               const wxString& name = "splitterWindow");
        virtual ~SplitterWindow();
        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = wxSP_3DBORDER, const wxString& name = "splitterWindow");
                    
    protected:
        void OnPaint(wxPaintEvent& event);
        void OnEraseBg(wxEraseEvent& event);
        void OnDClick(wxMouseEvent& event);
        void DoDrawSash(wxDC& dc);
        void DrawSash(wxDC& dc);
        void OnSysColoursChanged(Event& event);
    };
}
#endif
