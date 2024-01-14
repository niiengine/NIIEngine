

#include "editsnippetsdlg.h"
#include "snipwiz.h"
#include "AboutHtml.h"
#include "gsgsWindowManager.h"

EditSnippetsDlg::EditSnippetsDlg(wxWindow* parent, SnipWiz* plugin, IManager* manager)
    : EditSnippetsBaseDlg(parent)
    , m_pPlugin(plugin)
    , m_manager(manager)
{
    Initialize();
    m_listBox1->SetFocus();
}

void EditSnippetsDlg::OnItemSelected(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString selection = m_listBox1->GetStringSelection();
    DoItemSelected(selection);
}

void EditSnippetsDlg::SelectItem(long index)
{
    m_listBox1->SetSelection(index);
    wxString snippetStr = m_listBox1->GetString((unsigned int)index);
    DoItemSelected(snippetStr);
}

void EditSnippetsDlg::DoItemSelected(const wxString& text)
{
    m_textCtrlMenuEntry->SetValue(text);
    m_textCtrlSnippet->SetValue(GetStringDb()->GetSnippetString(text));
    gsgs::WindowManager::HotKeyList accelMap;
    gsgs_Window().getHotKeyList(accelMap);
    if(text.IsEmpty()) {
        return;
    }

    m_textCtrlAccelerator->SetValue(wxT(""));
    gsgs::WindowManager::HotKeyList::iterator iter = accelMap.begin();
    for(; iter != accelMap.end(); ++iter) {
        WindowManager::HotKey mid = iter->second;
        if(mid.action == text) {
            m_textCtrlAccelerator->SetValue(mid.accel);
        }
    }
}

void EditSnippetsDlg::OnAddSnippet(wxCommandEvent& event)
{
    if(GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
        wxMessageBox(_("Menu entry is not unique!"));
        return;
    }
    GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
    long index = m_listBox1->Append(m_textCtrlMenuEntry->GetValue());
    m_listBox1->SetSelection(index);
    m_modified = true;
}

void EditSnippetsDlg::OnAddSnippetUI(wxUpdateUIEvent& event)
{
    if(m_textCtrlMenuEntry->GetValue().IsEmpty() || m_textCtrlSnippet->GetValue().IsEmpty())
        event.Enable(false);
    else
        event.Enable(true);
}

void EditSnippetsDlg::OnChangeSnippet(wxCommandEvent& event)
{
    wxString curListKey = m_listBox1->GetStringSelection();
    int index = m_listBox1->GetSelection();

    // check if list key is not equal new menu entry and if it is an used key
    if(curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0 &&
       GetStringDb()->IsSnippetKey(m_textCtrlMenuEntry->GetValue())) {
        ::wxMessageBox(_("Menu entry is not unique!"));
        return;
    }

    // if menu entry has changed, delete old entry in list
    if(curListKey.Cmp(m_textCtrlMenuEntry->GetValue()) != 0) GetStringDb()->DeleteSnippetKey(curListKey);

    GetStringDb()->SetSnippetString(m_textCtrlMenuEntry->GetValue(), m_textCtrlSnippet->GetValue());
    m_listBox1->SetString(index, m_textCtrlMenuEntry->GetValue());
    m_modified = true;
}

void EditSnippetsDlg::OnChangeSnippetUI(wxUpdateUIEvent& event)
{
    if(m_textCtrlMenuEntry->GetValue().IsEmpty() || m_textCtrlSnippet->GetValue().IsEmpty())
        event.Enable(false);
    else
        event.Enable(true);
}

void EditSnippetsDlg::OnRemoveSnippet(wxCommandEvent& event)
{
    wxString key = m_listBox1->GetStringSelection();
    int index = m_listBox1->GetSelection();

    GetStringDb()->DeleteSnippetKey(key);
    m_listBox1->Delete(index);

    if(m_listBox1->GetCount()) SelectItem(0);
    m_modified = true;
}

void EditSnippetsDlg::OnRemoveSnippetUI(wxUpdateUIEvent& event)
{
    if(m_listBox1->GetSelection() != wxNOT_FOUND)
        event.Enable(true);
    else
        event.Enable(false);
}

void EditSnippetsDlg::Initialize()
{
    wxTextAttr attribs = m_textCtrlSnippet->GetDefaultStyle();
    wxArrayInt tabs = attribs.GetTabs();
    int tab = 70;
    for(int i = 1; i < 20; i++) {
        tabs.Add(tab * i);
    }

    attribs.SetTabs(tabs);
    m_textCtrlSnippet->SetDefaultStyle(attribs);

    wxArrayString keys;
    GetStringDb()->GetAllSnippetKeys(keys);
    m_listBox1->Append(keys);

    if(m_listBox1->IsEmpty() == false) {
        SelectItem(0);
    }
    m_htmlWinAbout->SetPage(wxString::FromUTF8(snipwizhtml_txt));
}

swStringDb* EditSnippetsDlg::GetStringDb() { return m_pPlugin->GetStringDb(); }

void EditSnippetsDlg::OnButtonKeyShortcut(wxCommandEvent& e)
{
    wxArrayString keys;
    GetStringDb()->GetAllSnippetKeys(keys);
    keys.Sort();

    int index = keys.Index(m_textCtrlMenuEntry->GetValue());
    if(index != wxNOT_FOUND) {
        int id = 20050 + index;

        WindowManager::HotKey mid;
        mid.resourceID << id;
        mid.action = m_textCtrlMenuEntry->GetValue();
        
        if(gsgs_Window().showAddHotKeyDlg(this, mid) == wxID_OK) {

            if(gsgs_Window().isHotKeyExist(mid.accel) && mid.accel.IsEmpty() == false) {
                wxMessageBox(_("That accelerator already exists"), _("光栅故事"), wxOK | wxCENTRE, this);
                return;
            }

            gsgs_Window().addHotKey(mid.resourceID, mid.accel, mid.action);
            gsgs_Window().Update();
            m_textCtrlAccelerator->ChangeValue(mid.accel);
        }
    }
}
