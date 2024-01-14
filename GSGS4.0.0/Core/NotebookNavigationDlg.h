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

#ifndef NOTEBOOKNAVIGATIONDLG_H
#define NOTEBOOKNAVIGATIONDLG_H

#include "gsgsPreInclude.h"
#include "gsgsNotebook.h"
#include "gsgsMultiBook.h"
#include "wxcrafter_plugin.h"

class _gsgsAPI NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    Notebook* m_book = NULL;
    gsgs::MultiBook* m_multibook = NULL;
    int m_selection = wxNOT_FOUND;

public:
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
    NotebookNavigationDlg(wxWindow* parent, gsgs::MultiBook* book);
    virtual ~NotebookNavigationDlg();
    void CloseDialog();

    int GetSelection() const { return m_selection; }

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnKeyUp(wxKeyEvent& event);
    void FinalizeCtor();
};
#endif // NOTEBOOKNAVIGATIONDLG_H
