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

#ifndef __svn_patch_dlg__
#define __svn_patch_dlg__

/**
@file
Subclass of PatchDlgBase, which is generated by wxFormBuilder.
*/

#include "subversion2_ui.h"

//// end generated include

/** Implementing PatchDlgBase */
class PatchDlg : public PatchDlgBase
{
public:
    /** Constructor */
    PatchDlg( wxWindow* parent );
    //// end generated class members

    virtual ~PatchDlg();
    
    wxFilePickerCtrl* GetFilePicker() const {
        return m_filePicker;
    }
    
    wxRadioBox* GetRadioBoxPolicy() const {
        return m_radioBoxEOLPolicy;
    }
};

#endif // __svn_patch_dlg__
