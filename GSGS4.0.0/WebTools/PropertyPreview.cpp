#include "ObjectPreview.h"
#include "PropertyPreview.h"

PropertyPreview::PropertyPreview() {}

PropertyPreview::~PropertyPreview() { wxDELETE(m_valuePreview); }

void PropertyPreview::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("name").getValue(m_name);
    json.getObject("type").getValue(m_type);
    json.getObject("value").getValue(m_value);
    if(json.isObjectExist("valuePreview")) {
        m_valuePreview = new ObjectPreview();
        m_valuePreview->FromJSON(json.getObject("valuePreview"));
    }
}

gsgs::JSONObject PropertyPreview::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("name", m_name);
    json.addProperty("type", m_type);
    json.addProperty("value", m_value);
    if(m_valuePreview) { json.arrayAppend(m_valuePreview->ToJSON("valuePreview")); }
    return json;
}

wxString PropertyPreview::ToString() const
{
    wxString str;
    if(IsObject() && IsEmpty()) { return "{...}"; }
    str << GetName() << " : ";
    if(!GetSubtype().IsEmpty()) {
        str << GetSubtype();
    } else {
        str << GetType();
    }
    if(!GetValue().IsEmpty()) {
        if(IsString()) {
            str << ": \"" << GetValue() << "\"";
        } else {
            str << ": " << GetValue();
        }
    }
    if(GetValuePreview()) { str << "[ " << GetValuePreview()->ToString() << "]"; }
    return str;
}
