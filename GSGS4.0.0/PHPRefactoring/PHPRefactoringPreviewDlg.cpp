#include "PHPRefactoringPreviewDlg.h"
#include "gsgsStyleManager.h"
#include "gsgsLexerStyle.h"

PHPRefactoringPreviewDlg::PHPRefactoringPreviewDlg(wxWindow* parent, const wxString& patchContent)
    : PHPRefactoringPreviewBaseDlg(parent)
{
    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("diff");
    if(lexer) {
        lexer->Apply(m_stcPreview, true);
    }
    m_stcPreview->SetText(patchContent);
    m_stcPreview->SetReadOnly(true);
    m_stcPreview->SetFirstVisibleLine(0);
    CentreOnParent();
}

PHPRefactoringPreviewDlg::~PHPRefactoringPreviewDlg() {}

void PHPRefactoringPreviewDlg::OnApplyChanges(wxCommandEvent& event)
{
    event.Skip();
    EndModal(wxID_OK);
}
