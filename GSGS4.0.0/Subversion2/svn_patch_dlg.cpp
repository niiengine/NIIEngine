//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_patch_dlg.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "svn_patch_dlg.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"

PatchDlg::PatchDlg(wxWindow* parent)
    : PatchDlgBase(parent)
{
    SetName("PatchDlg");
    gsgs::WindowManager::Load(this);

    long v = gsgs_EditorConfig().GetInteger(wxT("m_radioBoxEOLPolicy"));
    if(v != wxNOT_FOUND) {
        m_radioBoxEOLPolicy->SetSelection(v);
    }
}

PatchDlg::~PatchDlg()
{
    
    gsgs_EditorConfig().SetInteger(wxT("m_radioBoxEOLPolicy"), m_radioBoxEOLPolicy->GetSelection());
}
