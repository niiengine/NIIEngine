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
#ifndef COMPLETIONITEM_H
#define COMPLETIONITEM_H

#include "gsgsPreInclude.h"
#include "LSP/JSONObject.h"
#include "LSP/basic_types.h"

namespace LSP
{
class _gsgsAPI CompletionItem : public Serializable
{
    wxString m_label;
    int m_kind = wxNOT_FOUND;
    wxString m_detail;
    wxString m_documentation;
    wxString m_filterText;
    wxString m_insertText;
    wxSharedPtr<LSP::TextEdit> m_textEdit;

public:
    enum eTriggerKind {
        kTriggerUnknown = -1,
        kTriggerKindInvoked = 1,              // Completion was triggered by typing an identifier (24x7 code complete)
        kTriggerCharacter = 2,                // Completion was triggered by a trigger character (e.g. ".")
        kTriggerForIncompleteCompletions = 3, // user internally
        kTriggerUser = 4,                     // manual invocation (e.g Ctrl+Space)
    };

    enum eCompletionItemKind {
        kKindText = 1,
        kKindMethod = 2,
        kKindFunction = 3,
        kKindConstructor = 4,
        kKindField = 5,
        kKindVariable = 6,
        kKindClass = 7,
        kKindInterface = 8,
        kKindModule = 9,
        kKindProperty = 10,
        kKindUnit = 11,
        kKindValue = 12,
        kKindEnum = 13,
        kKindKeyword = 14,
        kKindSnippet = 15,
        kKindColor = 16,
        kKindFile = 17,
        kKindReference = 18,
        kKindFolder = 19,
        kKindEnumMember = 20,
        kKindConstant = 21,
        kKindStruct = 22,
        kKindEvent = 23,
        kKindOperator = 24,
        kKindTypeParameter = 25,
    };

public:
    typedef wxSharedPtr<CompletionItem> Ptr_t;
    typedef std::vector<CompletionItem::Ptr_t> Vec_t;

public:
    CompletionItem();
    virtual ~CompletionItem();
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);
    void SetDetail(const wxString& detail) { this->m_detail = detail; }
    void SetDocumentation(const wxString& documentation) { this->m_documentation = documentation; }
    void SetFilterText(const wxString& filterText) { this->m_filterText = filterText; }
    void SetInsertText(const wxString& insertText) { this->m_insertText = insertText; }
    void SetKind(int kind) { this->m_kind = kind; }
    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetDetail() const { return m_detail; }
    const wxString& GetDocumentation() const { return m_documentation; }
    const wxString& GetFilterText() const { return m_filterText; }
    const wxString& GetInsertText() const { return m_insertText; }
    int GetKind() const { return m_kind; }
    const wxString& GetLabel() const { return m_label; }
    wxSharedPtr<LSP::TextEdit> GetTextEdit() { return m_textEdit; }
    bool HasTextEdit() const { return m_textEdit != nullptr; }
};

}; // namespace LSP

#endif // COMPLETIONITEM_H
