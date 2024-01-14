#include "enter_strings_dlg.h"
#include <gsgsStyleManager.h>
#include <gsgsLexerStyle.h>
#include "gsgsWindowManager.h"

EnterStringsDlg::EnterStringsDlg(wxWindow* parent, const wxString& value)
    : EnterStringsDlgBase(parent)
{
    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text", "Default");
    if(lexer) { lexer->Apply(m_stc); }
    m_stc->SetText(value);
    SetName("EnterStringsDlg");
    gsgs::WindowManager::Load(this);
}

EnterStringsDlg::~EnterStringsDlg() {}

wxString EnterStringsDlg::GetValue() const { return m_stc->GetText(); }

void EnterStringsDlg::SetMessage(const wxString& msg)
{
    if(msg.IsEmpty() == false) {
        m_staticTextMessage->SetLabel(msg);
        m_staticTextMessage->Show();
    } else {
        m_staticTextMessage->Hide();
    }
}
