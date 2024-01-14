#include "LSP/Request.h"
#include "gsgsJSON.h"

LSP::Request::Request() { m_id = Message::GetNextID(); }

LSP::Request::~Request() {}

JSONObject LSP::Request::ToJSON(const wxString& name) const
{
    JSONObject json = MessageWithParams::ToJSON(name);
    json.addProperty("id", GetId());
    return json;
}

void LSP::Request::FromJSON(const JSONObject& json) { wxUnusedVar(json); }
