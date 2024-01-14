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

#include "SFTPSettingsDialog.h"
#include "gsgsWindowManager.h"
#include "sftp_settings.h"

SFTPSettingsDialog::SFTPSettingsDialog(wxWindow* parent)
    : SFTPSettingsDialogBase(parent)
{
    CenterOnParent();
    SetName("SFTPSettingsDialog");
    gsgs::WindowManager::Load(this);
    SFTPSettings s;
    s.Load();
    m_sshClientPath->SetPath(s.GetSshClient());
}

SFTPSettingsDialog::~SFTPSettingsDialog() {}

void SFTPSettingsDialog::OnOK(wxCommandEvent& event)
{
    // Save the data
    SFTPSettings settings;
    settings.Load();
    settings.SetSshClient(GetSshClientPath()->GetPath());
    settings.Save();
    event.Skip();
}
