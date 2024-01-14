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

#ifndef SFTPMANAGEBOOKMARKDLG_H
#define SFTPMANAGEBOOKMARKDLG_H
#include "UI.h"

class SFTPManageBookmarkDlg : public SFTPManageBookmarkDlgBase
{
public:
    SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks);
    virtual ~SFTPManageBookmarkDlg();
    wxArrayString GetBookmarks() const;
protected:
    virtual void OnDelete(wxCommandEvent& event);
    virtual void OnDeleteUI(wxUpdateUIEvent& event);
};
#endif // SFTPMANAGEBOOKMARKDLG_H
