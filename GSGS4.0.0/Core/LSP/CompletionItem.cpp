#include "CompletionItem.h"

LSP::CompletionItem::CompletionItem() {}

LSP::CompletionItem::~CompletionItem() {}

JSONObject LSP::CompletionItem::ToJSON(const wxString& name) const { return JSONObject(NULL); }

void LSP::CompletionItem::FromJSON(const JSONObject& json)
{
    json.getObject("label").getValue(m_label);
    json.getObject("kind").getValue(m_kind, m_kind);
    json.getObject("detail").getValue(m_detail);
    json.getObject("documentation").getValue(m_documentation);
    json.getObject("filterText").getValue(m_filterText);
    json.getObject("insertText").getValue(m_insertText);

    m_insertText.Trim().Trim(false);
    m_label.Trim().Trim(false);
    m_detail.Trim().Trim(false);
    m_documentation.Trim().Trim(false);
    if(json.isObjectExist("textEdit") && !json.getObject("textEdit").isNull()) { 
        m_textEdit.reset(new LSP::TextEdit());
        m_textEdit->FromJSON(json.getObject("textEdit")); 
    }
}
