#include "Message.h"

LSP::Message::Message() {}

LSP::Message::~Message() {}

JSONObject LSP::Message::ToJSON(const wxString& name) const
{
    JSONObject json = JSONObject::createObject(name);
    json.addProperty("jsonrpc", m_jsonrpc);
    return json;
}

void LSP::Message::FromJSON(const JSONObject& json) { json.getObject("jsonrpc").getValue(m_jsonrpc); }

int LSP::Message::GetNextID()
{
    static int requestId = 0;
    return ++requestId;
}
