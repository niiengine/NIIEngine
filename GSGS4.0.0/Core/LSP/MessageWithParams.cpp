#include "LSP/MessageWithParams.h"
#include <wx/string.h>
#include "gsgsJSON.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include <sstream>

LSP::MessageWithParams::MessageWithParams() {}

LSP::MessageWithParams::~MessageWithParams() {}

JSONObject LSP::MessageWithParams::ToJSON(const wxString& name) const
{
    JSONObject json = Message::ToJSON(name);
    json.addProperty("method", GetMethod());
    if(m_params) { json.addProperty(m_params->ToJSON("params")); }
    return json;
}

void LSP::MessageWithParams::FromJSON(const JSONObject& json)
{
    // we dont need to un-serialize a request object
    wxUnusedVar(json);
}

std::string LSP::MessageWithParams::ToString() const
{
    // Serialize the object and construct a JSON-RPC message
    JSONObject json = ToJSON("");
    wxString data;
    json.getString(data, false);
    data.Trim().Trim(false);

    std::string s = FileUtil::ToStdString(data);
    size_t len = s.length();

    std::stringstream ss;
    // Build the request
    ss << "Content-Length: " << len << "\r\n";
    ss << "\r\n";
    ss << s;
    return ss.str();
}

LSP::MessageWithParams::Ptr_t LSP::MessageWithParams::MakeRequest(LSP::MessageWithParams* message_ptr)
{
    LSP::MessageWithParams::Ptr_t p(message_ptr);
    return p;
}
