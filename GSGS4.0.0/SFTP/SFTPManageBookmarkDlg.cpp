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

#include "SFTPManageBookmarkDlg.h"
#include "gsgsWindowManager.h"

SFTPManageBookmarkDlg::SFTPManageBookmarkDlg(wxWindow* parent, const wxArrayString& bookmarks)
    : SFTPManageBookmarkDlgBase(parent)
{
    m_listBoxBookmarks->Append(bookmarks);
    SetName("SFTPManageBookmarkDlg");
    gsgs::WindowManager::Load(this);
}

SFTPManageBookmarkDlg::~SFTPManageBookmarkDlg() {}

void SFTPManageBookmarkDlg::OnDelete(wxCommandEvent& event)
{
    int sel = m_listBoxBookmarks->GetSelection();
    if(sel == wxNOT_FOUND) return;

    m_listBoxBookmarks->Delete(sel);
}

void SFTPManageBookmarkDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listBoxBookmarks->GetSelection() != wxNOT_FOUND);
}

wxArrayString SFTPManageBookmarkDlg::GetBookmarks() const
{
    wxArrayString bookmarks;
    for(unsigned int i = 0; i < m_listBoxBookmarks->GetCount(); ++i) {
        bookmarks.Add(m_listBoxBookmarks->GetString(i));
    }
    return bookmarks;
}
