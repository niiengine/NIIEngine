#include "ObjectPreview.h"
#include "PropertyPreview.h"

ObjectPreview::ObjectPreview() {}

ObjectPreview::~ObjectPreview() { DeleteProperties(); }

void ObjectPreview::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("type").getValue(m_type);
    json.getObject("subtype").getValue(m_subtype);
    json.getObject("description").getValue(m_description);
    json.getObject("overflow").getValue(m_overflow, m_overflow);

    DeleteProperties();
    if(json.isObjectExist("properties")) {
        gsgs::JSONObject props = json.getObject("properties");
        int size = props.getArraySize();
        for(int i = 0; i < size; ++i) {
            gsgs::JSONObject prop = props.getArray(i);
            PropertyPreview* pp = new PropertyPreview();
            pp->FromJSON(prop);
            m_properties.push_back(pp);
        }
    }
}

gsgs::JSONObject ObjectPreview::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("type", m_type);
    json.addProperty("subtype", m_subtype);
    json.addProperty("description", m_description);
    json.addProperty("overflow", m_overflow);
    if(!m_properties.empty()) {
        gsgs::JSONObject arr = gsgs::JSONObject::createArray("properties");
        for(size_t i = 0; i < m_properties.size(); ++i) {
            arr.arrayAppend(m_properties[i]->ToJSON(""));
        }
    }
    return json;
}

void ObjectPreview::DeleteProperties()
{
    for(size_t i = 0; i < m_properties.size(); ++i) {
        wxDELETE(m_properties[i]);
    }
    m_properties.clear();
}

wxString ObjectPreview::ToString() const
{
    wxString str;
    if(IsEmpty()) {
        return "{...}";
    } else {
        str << GetType();
        if(!GetSubtype().IsEmpty()) { str << " (" << GetSubtype() << ")"; }
        if(!GetProperties().empty()) {
            str << ": [";
            for(size_t i = 0; i < m_properties.size(); ++i) {
                str << m_properties[i]->ToString() << ", ";
            }
        }
        if(IsOverflow()) {
            str << "...";
        } else {
            // remove the last comma we added
            str.RemoveLast(2);
        }
        if(!GetProperties().empty()) { str << "]"; }
        return str;
    }
}
