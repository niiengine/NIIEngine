
#include "LLDBRemoteHandshakePacket.h"

LLDBRemoteHandshakePacket::LLDBRemoteHandshakePacket()
{
}

LLDBRemoteHandshakePacket::~LLDBRemoteHandshakePacket()
{
}

LLDBRemoteHandshakePacket::LLDBRemoteHandshakePacket(const wxString& json)
{
    JSON root(json);
    FromJSON( root.getObject() );
}

void LLDBRemoteHandshakePacket::FromJSON(const JSONObject& json)
{
    m_host = json.getObject("m_host").toString();
}

JSONObject LLDBRemoteHandshakePacket::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    json.addProperty("m_host", m_host);
    return json;
}
