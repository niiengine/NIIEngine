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
#include "SFTPGrep.h"
#include "gsgsConfig.h"

SFTPGrep::SFTPGrep(wxWindow* parent)
    : SFTPGrepBase(parent)
{
    wxString find_what; gsgs_Config().Read("sftp/grep/finw_what", find_what, wxString());
    wxString searchIn; gsgs_Config().Read("sftp/grep/search_in", searchIn, wxString("*"));
    bool ignoreCase; gsgs_Config().Read("sftp/grep/ignore_case", ignoreCase, false);
    bool wholeWord; gsgs_Config().Read("sftp/grep/whole_word", wholeWord, true);

    m_checkBoxIgnoreCase->SetValue(ignoreCase);
    m_checkBoxWholeWord->SetValue(wholeWord);
    m_textCtrlFindWhat->ChangeValue(find_what);
    m_textCtrlSeachIn->ChangeValue(searchIn);
}

SFTPGrep::~SFTPGrep() 
{
    gsgs_Config().Write("sftp/grep/finw_what", m_textCtrlFindWhat->GetValue());
    gsgs_Config().Write("sftp/grep/search_in", m_textCtrlSeachIn->GetValue());
    gsgs_Config().Write("sftp/grep/ignore_case", m_checkBoxIgnoreCase->IsChecked());
    gsgs_Config().Write("sftp/grep/whole_word", m_checkBoxWholeWord->IsChecked());
}

void SFTPGrep::OnOkUI(wxUpdateUIEvent& event) 
{ 
    event.Enable(!m_textCtrlFindWhat->IsEmpty()); 
}

GrepData SFTPGrep::GetData() const
{
    GrepData gd;
    gd.SetFindWhat(m_textCtrlFindWhat->GetValue());
    gd.SetSearchIn(m_textCtrlSeachIn->GetValue());
    gd.SetIgnoreCase(m_checkBoxIgnoreCase->IsChecked());
    gd.SetWholeWord(m_checkBoxWholeWord->IsChecked());
    return gd;
}

wxString GrepData::GetGrepCommand(const wxString& path) const
{
    wxString command;
    command << "find " << path << " -name \"" << GetSearchIn() << "\" | xargs grep -n ";
    if(IsIgnoreCase()) { command << " -i "; }
    if(IsWholeWord()) { command << " -w "; }
    command << " \"" << GetFindWhat() << "\"";
    return command;
}
