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

//	Copyright: 2013 Brandon Captain
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

#ifndef ZOOM_NAV_TEXT
#define ZOOM_NAV_TEXT

#include <wx/stc/stc.h>
#include "ieditor.h"

class ZoomText : public wxStyledTextCtrl
{
    int m_zoomFactor;
    wxColour m_colour;
    wxString m_filename;
    wxString m_classes;
    wxString m_locals;
    wxTimer* m_timer;
    
protected:
    void OnTheme(wxCommandEvent& e);
    void OnTimer(wxTimerEvent& event);
    void DoClear();
    
public:
    ZoomText(wxWindow* parent,
             wxWindowID id = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxSTCNameStr);
    virtual ~ZoomText();
    void UpdateLexer(IEditor* editor);
    void OnSettingsChanged(wxCommandEvent& e);
    void UpdateText(IEditor* editor);
    void HighlightLines(int start, int end);
    
    void Startup();
};

#endif // ZOOM_NAV_TEXT
