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

#include "gsgsProgressDialog.h"
namespace gsgs
{
#if wxUSE_GUI
#ifdef __WXMSW__

    ProgressDlg::ProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue)
        : gsgsProgressDlgBase(parent)
    {
        SetSizeHints(600, -1);
        SetTitle(title);
        m_staticLabel->SetLabel(msg);
        m_gauge->SetRange(maxValue);
        GetSizer()->Fit(this);
        CenterOnParent();
        Show();
        wxYieldIfNeeded();
    }

    ProgressDlg::~ProgressDlg() {}

    bool ProgressDlg::Update(int value, const wxString& msg)
    {
        m_staticLabel->SetLabel(msg);
        m_gauge->SetValue(value);

        if(value % 20 == 0) { wxSafeYield(this, true); }
        return true;
    }

    bool ProgressDlg::Pulse(const wxString& msg)
    {
        int curval = m_gauge->GetValue();
        m_gauge->SetValue(curval + 1);
        if(!msg.IsEmpty()) m_staticLabel->SetLabel(msg);
        return true;
    }

#else  // UNIX

    ProgressDlg::ProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue)
        : wxProgressDialog(title, msg, maxValue, NULL, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE)
    {
        SetSizeHints(400, -1);
        GetSizer()->Fit(this);
        CenterOnScreen();
    }

    ProgressDlg::~ProgressDlg() {}

    bool ProgressDlg::Update(int value, const wxString& msg) { return wxProgressDialog::Update(value, msg, NULL); }

    bool ProgressDlg::Pulse(const wxString& msg) { return wxProgressDialog::Pulse(msg, NULL); }
#endif
#endif
}
