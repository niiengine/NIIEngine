#include "LSP/basic_types.h"
#include <wx/filesys.h>

//===------------------------------------------------
// Request
//===------------------------------------------------
namespace LSP
{

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
void TextDocumentIdentifier::FromJSON(const JSONObject& json)
{
    wxString uri;
    json.getObject("uri").getValue(uri);
    m_filename = wxFileSystem::URLToFileName(uri);
}

JSONObject TextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_filename));
    return json;
}
//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
void VersionedTextDocumentIdentifier::FromJSON(const JSONObject& json)
{
    TextDocumentIdentifier::FromJSON(json);
    json.getObject("version").getValue(m_version, m_version);
}

JSONObject VersionedTextDocumentIdentifier::ToJSON(const wxString& name) const
{
    JSONObject json = TextDocumentIdentifier::ToJSON(name);
    json.addProperty("version", m_version);
    return json;
}

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
void Position::FromJSON(const JSONObject& json)
{
    json.getObject("line").getValue(m_line, 0);
    json.getObject("character").getValue(m_character, 0);
}

JSONObject Position::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("line", m_line);
    json.addProperty("character", m_character);
    return json;
}

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
void TextDocumentItem::FromJSON(const JSONObject& json)
{
    wxString temp;
    json.getObject("uri").getValue(temp);
    m_uri = wxFileSystem::URLToFileName(temp);
    m_uri.Normalize();
    json.getObject("languageId").getValue(m_languageId);
    json.getObject("version").getValue(m_version);
    json.getObject("text").getValue(m_text);
}

JSONObject TextDocumentItem::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(GetUri()))
        .addProperty("languageId", GetLanguageId())
        .addProperty("version", GetVersion())
        .addProperty("text", GetText());
    return json;
}
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
void TextDocumentContentChangeEvent::FromJSON(const JSONObject& json) { json.getObject("text").getValue(m_text); }

JSONObject TextDocumentContentChangeEvent::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("text", m_text);
    return json;
}

void Range::FromJSON(const JSONObject& json)
{
    m_start.FromJSON(json.getObject("start"));
    m_end.FromJSON(json.getObject("end"));
}

JSONObject Range::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_start.ToJSON("start"));
    json.addProperty(m_start.ToJSON("end"));
    return json;
}

void Location::FromJSON(const JSONObject& json)
{
    wxString temp;
    json.getObject("uri").getValue(temp);
    wxFileName fn = wxFileSystem::URLToFileName(temp);
    fn.Normalize();
    m_uri = fn.GetFullPath();
    m_range.FromJSON(json.getObject("range"));
}

JSONObject Location::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("uri", wxFileSystem::FileNameToURL(m_uri));
    json.addProperty(m_range.ToJSON("range"));
    return json;
}

void TextEdit::FromJSON(const JSONObject& json)
{
    m_range.FromJSON(json.getObject("range"));
    json.getObject("newText").getValue(m_newText);
}

JSONObject TextEdit::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("newText", m_newText);
    json.addProperty(m_range.ToJSON("range"));
    return json;
}
}; // namespace LSP

void LSP::ParameterInformation::FromJSON(const JSONObject& json)
{
    json.getObject("label").getValue(m_label);
    json.getObject("documentation").getValue(m_documentation);
}

JSONObject LSP::ParameterInformation::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    return json;
}

void LSP::SignatureInformation::FromJSON(const JSONObject& json)
{
    json.getObject("label").getValue(m_label);
    json.getObject("documentation").getValue(m_documentation);
    m_parameters.clear();
    if(json.isObjectExist("parameters")) {
        JSONObject parameters = json.getObject("parameters");
        const int size = parameters.getArraySize();
        if(size > 0) {
            m_parameters.reserve(size);
            for(int i = 0; i < size; ++i) {
                ParameterInformation p;
                p.FromJSON(parameters.getArray(i));
                m_parameters.push_back(p);
            }
        }
    }
}

JSONObject LSP::SignatureInformation::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("label", m_label);
    json.addProperty("documentation", m_documentation);
    if(!m_parameters.empty()) {
        JSONObject params = JSONObject::createArray("parameters");
        json.addProperty(params);
        for(size_t i = 0; i < m_parameters.size(); ++i) {
            params.addProperty(m_parameters.at(i).ToJSON(""));
        }
    }
    return json;
}

void LSP::SignatureHelp::FromJSON(const JSONObject& json)
{
    // Read the signatures
    m_signatures.clear();
    JSONObject signatures = json.getObject("signatures");
    const int count = signatures.getArraySize();
    for(int i = 0; i < count; ++i) {
        SignatureInformation si;
        si.FromJSON(signatures.getArray(i));
        m_signatures.push_back(si);
    }

    json.getObject("activeSignature").getValue(m_activeSignature, 0);
    json.getObject("activeParameter").getValue(m_activeParameter, 0);
}

JSONObject LSP::SignatureHelp::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    JSONObject signatures = JSONObject::createArray("signatures");
    json.addProperty(signatures);
    for(const SignatureInformation& si : m_signatures) {
        signatures.arrayAppend(si.ToJSON(""));
    }
    json.addProperty("activeSignature", m_activeSignature);
    json.addProperty("activeParameter", m_activeParameter);
    return json;
}

///===------------------------------------------------------------------------
/// Diagnostic
///===------------------------------------------------------------------------
void LSP::Diagnostic::FromJSON(const JSONObject& json)
{
    m_range.FromJSON(json.getObject("range"));
    json.getObject("message").getValue(m_message);
}

JSONObject LSP::Diagnostic::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty(m_range.ToJSON("range"));
    json.addProperty("message", GetMessage());
    return json;
}
