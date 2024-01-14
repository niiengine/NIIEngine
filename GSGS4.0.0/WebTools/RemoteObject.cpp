#include "PropertyPreview.h"
#include "RemoteObject.h"

RemoteObject::RemoteObject() {}

RemoteObject::~RemoteObject() {}

void RemoteObject::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("type").getValue(m_type);
    json.getObject("subtype").getValue(m_subtype);
    json.getObject("className").getValue(m_className);
    json.getObject("objectId").getValue(m_objectId);
    gsgs::JSONObject value = json.getObject("value");
    if(value.isNull()) {
        m_value = "null";
    } else if(value.isBool()) {
        bool re;
        value.getValue(re);
        m_value << (re ? "true" : "false");
    } else if(value.isNumber()) {
        int re;
        value.getValue(re);
        m_value << re;
    } else {
        wxString re;
        value.getValue(re);
        m_value << re;
    }
    if(json.isObjectExist("preview")) { m_preview.FromJSON(json.getObject("preview")); }
}

gsgs::JSONObject RemoteObject::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("type", m_type);
    json.addProperty("subtype", m_subtype);
    json.addProperty("className", m_className);
    json.addProperty("value", m_value);
    json.addProperty("objectId", m_objectId);
    if(!m_preview.IsEmpty()) { json.arrayAppend(m_preview.ToJSON("preview")); }
    return json;
}

wxString RemoteObject::ToString() const
{
    wxString str;
    if(IsFunction()) {
        return "Function";
    } else if(IsObject()) {
        str << this->GetClassName() << " : " << GetPreview().ToString();

    } else if(IsString()) {
        // Simple type
        str << "\"" << GetValue() << "\"";
    } else if(IsUndefined()) {
        str << "undefined";
    } else {
        str << GetValue();
    }
    return str;
}

wxString RemoteObject::GetTextPreview() const
{
    if(IsObject()) { return "{...}"; }
    return ToString();
}

bool RemoteObject::HasChildren() const { return IsObject(); }
