#include "Location.h"

Location::~Location() {}

gsgs::JSONObject Location::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("lineNumber", m_lineNumber);
    json.addProperty("scriptId", m_scriptId);
    return json;
}

void Location::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("lineNumber").getValue(m_lineNumber, (long)0);
    json.getObject("scriptId").getValue(m_scriptId);
}
