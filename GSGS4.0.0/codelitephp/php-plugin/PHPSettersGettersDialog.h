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

#ifndef PHPSETTERSGETTERSDIALOG_H
#define PHPSETTERSGETTERSDIALOG_H

#include "php_ui.h"
#include "PHPSetterGetterEntry.h"

class PHPSettersGettersDialog : public PHPSettersGettersDialogBase
{
    gsgs::IEditor* m_editor;
    IManager* m_mgr;
    wxString m_scope;

protected:
    void DoPopulate(const PHPEntityBase::List_t& members);
    void Clear();

public:
    PHPSettersGettersDialog(wxWindow* parent, gsgs::IEditor* editor, gsgs::IManager* mgr);
    virtual ~PHPSettersGettersDialog();
    PHPSetterGetterEntry::Vec_t GetMembers();
    size_t GetFlags();
    const wxString& GetScope() const { return m_scope; }
};
#endif // PHPSETTERSGETTERSDIALOG_H
