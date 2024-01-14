#include "CallFrameScope.h"

CallFrameScope::CallFrameScope() {}

CallFrameScope::~CallFrameScope() {}

void CallFrameScope::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("type").getValue(m_type);
    json.getObject("name").getValue(m_name);
    m_remoteObject.FromJSON(json.getObject("object"));
}

gsgs::JSONObject CallFrameScope::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("type", m_type);
    if(!m_name.IsEmpty()) { json.addProperty("name", m_name); }
    json.arrayAppend(m_remoteObject.ToJSON("object"));
    return json;
}

wxString CallFrameScope::GetDisplayName() const
{
    if(GetName().IsEmpty()) {
        return GetType();
    } else {
        return GetName() + " : " + GetType();
    }
}
