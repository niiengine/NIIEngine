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
#ifndef JSONRPC_BASICTYPES_H
#define JSONRPC_BASICTYPES_H

#include "gsgsPreInclude.h"
#include "JSONObject.h"
#include <wx/sharedptr.h>

namespace LSP
{
//===----------------------------------------------------------------------------------
// TextDocumentContentChangeEvent
//===----------------------------------------------------------------------------------
class _gsgsAPI TextDocumentContentChangeEvent : public Serializable
{
    wxString m_text;

public:
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);

    TextDocumentContentChangeEvent() {}
    TextDocumentContentChangeEvent(const wxString& text)
        : m_text(text)
    {
    }
    virtual ~TextDocumentContentChangeEvent() {}
    TextDocumentContentChangeEvent& SetText(const wxString& text)
    {
        this->m_text = text;
        return *this;
    }
    const wxString& GetText() const { return m_text; }
};

//===----------------------------------------------------------------------------------
// TextDocumentIdentifier
//===----------------------------------------------------------------------------------
class _gsgsAPI TextDocumentIdentifier : public Serializable
{
    wxFileName m_filename;

public:
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);

    TextDocumentIdentifier() {}
    TextDocumentIdentifier(const wxFileName& filename)
        : m_filename(filename)
    {
    }
    virtual ~TextDocumentIdentifier() {}
    TextDocumentIdentifier& SetFilename(const wxFileName& filename)
    {
        this->m_filename = filename;
        return *this;
    }
    const wxFileName& GetFilename() const { return m_filename; }
};

//===----------------------------------------------------------------------------------
// VersionedTextDocumentIdentifier
//===----------------------------------------------------------------------------------
class _gsgsAPI VersionedTextDocumentIdentifier : public TextDocumentIdentifier
{
    int m_version = 1;

public:
    virtual JSONObject ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONObject& json);

    VersionedTextDocumentIdentifier() {}
    VersionedTextDocumentIdentifier(int version)
        : m_version(version)
    {
    }
    virtual ~VersionedTextDocumentIdentifier() {}
    VersionedTextDocumentIdentifier& SetVersion(int version)
    {
        this->m_version = version;
        return *this;
    }
    int GetVersion() const { return m_version; }
};

//===----------------------------------------------------------------------------------
// Position
//===----------------------------------------------------------------------------------
class _gsgsAPI Position : public Serializable
{
    int m_line = wxNOT_FOUND;
    int m_character = wxNOT_FOUND;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    Position(int line, int col)
        : m_line(line)
        , m_character(col)
    {
    }
    Position() {}
    virtual ~Position() {}
    Position& SetCharacter(int character)
    {
        this->m_character = character;
        return *this;
    }
    Position& SetLine(int line)
    {
        this->m_line = line;
        return *this;
    }
    int GetCharacter() const { return m_character; }
    int GetLine() const { return m_line; }
    bool IsOk() const { return m_line != wxNOT_FOUND && m_character != wxNOT_FOUND; }
};

//===----------------------------------------------------------------------------------
// Range
//===----------------------------------------------------------------------------------
class _gsgsAPI Range : public Serializable
{
    Position m_start;
    Position m_end;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    Range(const Position& start, const Position& end)
        : m_start(start)
        , m_end(end)
    {
    }
    Range() {}
    virtual ~Range() {}
    Range& SetEnd(const Position& end)
    {
        this->m_end = end;
        return *this;
    }
    Range& SetStart(const Position& start)
    {
        this->m_start = start;
        return *this;
    }
    const Position& GetEnd() const { return m_end; }
    const Position& GetStart() const { return m_start; }
    bool IsOk() const { return m_start.IsOk() && m_end.IsOk(); }
};

//===----------------------------------------------------------------------------------
// TextEdit
//===----------------------------------------------------------------------------------

class _gsgsAPI TextEdit : public LSP::Serializable
{
    Range m_range;
    wxString m_newText;

public:
    TextEdit() {}
    virtual ~TextEdit() {}
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;
    void SetNewText(const wxString& newText) { this->m_newText = newText; }
    void SetRange(const Range& range) { this->m_range = range; }
    const wxString& GetNewText() const { return m_newText; }
    const Range& GetRange() const { return m_range; }
    bool IsOk() const { return m_range.IsOk(); }
};

class _gsgsAPI Location : public Serializable
{
    wxString m_uri;
    Range m_range;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    Location(const wxString& uri, const Range& range)
        : m_uri(uri)
        , m_range(range)
    {
    }
    Location() {}
    virtual ~Location() {}
    Location& SetRange(const Range& range)
    {
        this->m_range = range;
        return *this;
    }
    Location& SetUri(const wxString& uri)
    {
        this->m_uri = uri;
        return *this;
    }
    const Range& GetRange() const { return m_range; }
    const wxString& GetUri() const { return m_uri; }
};

//===----------------------------------------------------------------------------------
// TextDocumentItem
//===----------------------------------------------------------------------------------
class _gsgsAPI TextDocumentItem : public Serializable
{
    wxFileName m_uri;
    wxString m_languageId;
    wxString m_text;
    int m_version = 1;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    TextDocumentItem(const wxFileName& uri, const wxString& langId, const wxString& text, int version = 1)
        : m_uri(uri)
        , m_languageId(langId)
        , m_text(text)
        , m_version(version)
    {
    }
    TextDocumentItem() {}
    virtual ~TextDocumentItem() {}
    TextDocumentItem& SetLanguageId(const wxString& languageId)
    {
        this->m_languageId = languageId;
        return *this;
    }
    TextDocumentItem& SetText(const wxString& text)
    {
        this->m_text = text;
        return *this;
    }
    TextDocumentItem& SetUri(const wxFileName& uri)
    {
        this->m_uri = uri;
        return *this;
    }
    TextDocumentItem& SetVersion(int version)
    {
        this->m_version = version;
        return *this;
    }
    const wxString& GetLanguageId() const { return m_languageId; }
    const wxString& GetText() const { return m_text; }
    const wxFileName& GetUri() const { return m_uri; }
    int GetVersion() const { return m_version; }
};

class _gsgsAPI ParameterInformation : public LSP::Serializable
{
    wxString m_label;
    wxString m_documentation;

public:
    ParameterInformation() {}
    virtual ~ParameterInformation() {}
    ParameterInformation& SetDocumentation(const wxString& documentation)
    {
        this->m_documentation = documentation;
        return *this;
    }
    ParameterInformation& SetLabel(const wxString& label)
    {
        this->m_label = label;
        return *this;
    }
    const wxString& GetDocumentation() const { return m_documentation; }
    const wxString& GetLabel() const { return m_label; }
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    typedef std::vector<ParameterInformation> Vec_t;
};

class _gsgsAPI SignatureInformation : public LSP::Serializable
{
    wxString m_label;
    wxString m_documentation;
    ParameterInformation::Vec_t m_parameters;

public:
    typedef std::vector<LSP::SignatureInformation> Vec_t;

public:
    SignatureInformation() {}
    virtual ~SignatureInformation() {}
    SignatureInformation& SetParameters(const ParameterInformation::Vec_t& parameters)
    {
        this->m_parameters = parameters;
        return *this;
    }
    const ParameterInformation::Vec_t& GetParameters() const { return m_parameters; }
    SignatureInformation& SetDocumentation(const wxString& documentation)
    {
        this->m_documentation = documentation;
        return *this;
    }
    SignatureInformation& SetLabel(const wxString& label)
    {
        this->m_label = label;
        return *this;
    }
    const wxString& GetDocumentation() const { return m_documentation; }
    const wxString& GetLabel() const { return m_label; }
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;
};

class _gsgsAPI SignatureHelp : public LSP::Serializable
{
    SignatureInformation::Vec_t m_signatures;
    int m_activeSignature = 0;
    int m_activeParameter = 0;

public:
    SignatureHelp() {}
    virtual ~SignatureHelp() {}

    SignatureHelp& SetActiveParameter(int activeParameter)
    {
        this->m_activeParameter = activeParameter;
        return *this;
    }
    SignatureHelp& SetActiveSignature(int activeSignature)
    {
        this->m_activeSignature = activeSignature;
        return *this;
    }
    SignatureHelp& SetSignatures(const SignatureInformation::Vec_t& signatures)
    {
        this->m_signatures = signatures;
        return *this;
    }
    int GetActiveParameter() const { return m_activeParameter; }
    int GetActiveSignature() const { return m_activeSignature; }
    const SignatureInformation::Vec_t& GetSignatures() const { return m_signatures; }
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;
};

class _gsgsAPI Diagnostic : public Serializable
{
    Range m_range;
    wxString m_message;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON(const wxString& name) const;

    Diagnostic(const Range& range, const wxString& message)
        : m_range(range)
        , m_message(message)
    {
    }
    Diagnostic() {}
    virtual ~Diagnostic() {}
    Diagnostic& SetRange(const Range& range)
    {
        this->m_range = range;
        return *this;
    }
    const Range& GetRange() const { return m_range; }
    Diagnostic& SetMessage(const wxString& message)
    {
        this->m_message = message;
        return *this;
    }
    const wxString& GetMessage() const { return m_message; }
};
};     // namespace LSP
#endif // JSONRPC_BASICTYPES_H
