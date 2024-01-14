#include "PropertyDescriptor.h"

PropertyDescriptor::PropertyDescriptor() {}

PropertyDescriptor::~PropertyDescriptor() {}

void PropertyDescriptor::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("name").getValue(m_name);
    if(json.isObjectExist("value")) { m_value.FromJSON(json.getObject("value")); }
}

gsgs::JSONObject PropertyDescriptor::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("name", m_name);
    if(!m_value.IsEmpty()) { json.arrayAppend(m_value.ToJSON("value")); }
    return json;
}

wxString PropertyDescriptor::ToString() const
{
    wxString str = GetName();
    if(!GetValue().IsEmpty()) { str << ": " << GetValue().ToString(); }
    return str;
}

bool PropertyDescriptor::IsEmpty() const { return GetName().IsEmpty(); }

wxString PropertyDescriptor::GetTextPreview() const
{
    if(GetValue().IsObject()) { return "{...}"; }
    return GetValue().ToString();
}

bool PropertyDescriptor::HasChildren() const { return GetValue().IsObject(); }
