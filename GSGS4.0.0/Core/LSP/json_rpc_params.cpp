#include "LSP/json_rpc_params.h"

//===----------------------------------------------------------------------------------
// TextDocumentPositionParams
//===----------------------------------------------------------------------------------
namespace LSP
{
TextDocumentPositionParams::TextDocumentPositionParams() {}

void TextDocumentPositionParams::FromJSON(const JSONObject& json)
{
    m_textDocument.FromJSON(json);
    m_position.FromJSON(json);
}

JSONObject TextDocumentPositionParams::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_textDocument.ToJSON("textDocument"));
    json.addProperty(m_position.ToJSON("position"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidOpenTextDocumentParams
//===----------------------------------------------------------------------------------
DidOpenTextDocumentParams::DidOpenTextDocumentParams() {}

void DidOpenTextDocumentParams::FromJSON(const JSONObject& json) { m_textDocument.FromJSON(json); }

JSONObject DidOpenTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidCloseTextDocumentParams
//===----------------------------------------------------------------------------------
DidCloseTextDocumentParams::DidCloseTextDocumentParams() {}

void DidCloseTextDocumentParams::FromJSON(const JSONObject& json) { m_textDocument.FromJSON(json); }

JSONObject DidCloseTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_textDocument.ToJSON("textDocument"));
    return json;
}

//===----------------------------------------------------------------------------------
// DidChangeTextDocumentParams
//===----------------------------------------------------------------------------------
DidChangeTextDocumentParams::DidChangeTextDocumentParams() {}

void DidChangeTextDocumentParams::FromJSON(const JSONObject& json)
{
    m_textDocument.FromJSON(json);
    m_contentChanges.clear();
    if(json.isObjectExist("contentChanges")) {
        JSONObject arr = json.getObject("contentChanges");
        int count = arr.getArraySize();
        for(int i = 0; i < count; ++i) {
            TextDocumentContentChangeEvent c;
            c.FromJSON(arr.getArray(i));
            m_contentChanges.push_back(c);
        }
    }
}

JSONObject DidChangeTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_textDocument.ToJSON("textDocument"));
    JSONObject arr = JSONObject::createArray("contentChanges");
    for(size_t i = 0; i < m_contentChanges.size(); ++i) {
        arr.arrayAppend(m_contentChanges[i].ToJSON(""));
    }
    json.addProperty(arr);
    return json;
}

//===----------------------------------------------------------------------------------
// DidSaveTextDocumentParams
//===----------------------------------------------------------------------------------
DidSaveTextDocumentParams::DidSaveTextDocumentParams() {}

void DidSaveTextDocumentParams::FromJSON(const JSONObject& json)
{
    m_textDocument.FromJSON(json);
    json.getObject("text").getValue(m_text);
}

JSONObject DidSaveTextDocumentParams::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_textDocument.ToJSON("textDocument"));
    json.addProperty("text", m_text);
    return json;
}

//===----------------------------------------------------------------------------------
// CompletionParams
//===----------------------------------------------------------------------------------
CompletionParams::CompletionParams() {}

void CompletionParams::FromJSON(const JSONObject& json) { TextDocumentPositionParams::FromJSON(json); }

JSONObject CompletionParams::ToJSON(const wxString& name) const
{
    JSONObject json = TextDocumentPositionParams::ToJSON(name);
    return json;
}
}; // namespace LSP
