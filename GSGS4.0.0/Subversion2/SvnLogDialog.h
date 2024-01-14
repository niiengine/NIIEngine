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

#ifndef SVNLOGDIALOG_H
#define SVNLOGDIALOG_H
#include "subversion2_ui.h"

class SvnLogDialog : public SvnLogDialogBase
{
public:
    SvnLogDialog(wxWindow* parent);
    virtual ~SvnLogDialog();

    void SetCompact(wxCheckBox* compact) {
        this->m_compact = compact;
    }
    void SetFrom(wxTextCtrl* from) {
        this->m_from = from;
    }
    void SetTo(wxTextCtrl* to) {
        this->m_to = to;
    }
    wxCheckBox* GetCompact() {
        return m_compact;
    }
    wxTextCtrl* GetFrom() {
        return m_from;
    }
    wxTextCtrl* GetTo() {
        return m_to;
    }
};
#endif // SVNLOGDIALOG_H
