#include "XDebugBreakpoint.h"

XDebugBreakpoint::XDebugBreakpoint(const wxString &filename, int line)
    : m_fileName(filename)
    , m_line(line)
    , m_breakpointId(wxNOT_FOUND)
{
}

XDebugBreakpoint::XDebugBreakpoint()
    : m_line(wxNOT_FOUND)
    , m_breakpointId(wxNOT_FOUND)
{
}

XDebugBreakpoint::~XDebugBreakpoint()
{
}

bool XDebugBreakpoint::operator==(const XDebugBreakpoint& other) const
{
    return this->m_fileName == other.m_fileName && this->m_line == other.m_line;
}

void XDebugBreakpoint::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_fileName").getValue(m_fileName);
    json.getObject("m_line").getValue(m_line, wxNOT_FOUND);
}

gsgs::JSONObject XDebugBreakpoint::ToJSON() const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject();
    json.addProperty("m_fileName", m_fileName);
    json.addProperty("m_line", m_line);
    return json;
}
