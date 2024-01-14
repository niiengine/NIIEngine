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

#ifndef _gsgs_progress_dialog__
#define _gsgs_progress_dialog__

#if wxUSE_GUI

#include "TerminalEmulatorUIBase.h"
#include "gsgsPreInclude.h"

namespace gsgs
{
#if defined(__WXGTK__) || defined(__WXMAC__)
    #include <wx/progdlg.h>
    class ProgressDlg : public wxProgressDialog
    {
    public:
        ProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue);
        virtual ~ProgressDlg();
        bool Update(int value, const wxString& msg);
        bool Pulse(const wxString& msg);
    };
#else
    class _gsgsAPI ProgressDlg : public gsgsProgressDlgBase
    {
    public:
        /** Constructor */
        ProgressDlg(wxWindow* parent, const wxString& title, const wxString& msg, int maxValue);
        virtual ~ProgressDlg();
        bool Update(int value, const wxString& msg);
        bool Pulse(const wxString& msg);
    };
#endif
}
#endif
#endif
