#include "NodeFileManager.h"
#include "NodeJSDebuggerBreakpoint.h"

NodeJSBreakpoint::NodeJSBreakpoint() {}

NodeJSBreakpoint::~NodeJSBreakpoint() {}

void NodeJSBreakpoint::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("url").getValue(m_filename);
    m_filename = NodeFileManager::URIToFileName(m_filename);
    json.getObject("lineNumber").getValue(m_line);
}

gsgs::JSONObject NodeJSBreakpoint::ToJSON(const wxString& name) const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(name);
    json.addProperty("url", NodeFileManager::FileNameToURI(m_filename));
    json.addProperty("lineNumber", m_line);
    return json;
}
