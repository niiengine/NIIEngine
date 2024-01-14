#include "wxTerminalOptions.h"
#include "gsgsFileUtil.h"
#include <gsgsStyleManager.h>
#include <gsgsPathManager.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

void wxTerminalOptions::SetCommandFromFile(const wxString& command)
{
    wxString content;
    if(FileUtil::ReadFileContent(command, content)) {
        content.Trim().Trim(false);
        SetCommand(content);
    }
}

wxTerminalOptions::wxTerminalOptions()
{
    m_font = gsgs_Style().GetDefaultFixedFont();
    m_textColour = wxColour("rgb(248, 248, 242)");
    m_bgColour = wxColour("rgb(41, 43, 55)");
}

wxTerminalOptions::~wxTerminalOptions() {}

void wxTerminalOptions::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("font").getValue(m_font, m_font);
    json.getObject("bgColour").getValue(m_bgColour, m_bgColour);
    json.getObject("textColour").getValue(m_textColour, m_textColour);
    json.getObject("history").getValue(m_history);
}

gsgs::JSONObject wxTerminalOptions::ToJSON() const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject();
    json.addProperty("font", m_font);
    json.addProperty("bgColour", m_bgColour);
    json.addProperty("textColour", m_textColour);
    json.addProperty("history", m_history);
    return json;
}

wxTerminalOptions& wxTerminalOptions::Get()
{
    static wxTerminalOptions options;
    return options;
}

wxTerminalOptions& wxTerminalOptions::Load()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir(), _T("wxterminalctrl.conf"));
    JSON root(fn);
    if(root.isValid()) { FromJSON(root.getObject()); }
    return *this;
}

wxTerminalOptions& wxTerminalOptions::Save()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir(), "wxterminalctrl.conf");
    JSON json(ToJSON());
    json.save(fn);
    return *this;
}

void wxTerminalOptions::SetHistory(const wxArrayString& history)
{
    const std::size_t threshold = 500;
    if(history.size() > threshold + 1) {
        m_history = wxArrayString();
        m_history.Alloc(threshold);
        for(std::size_t i = 0; i < threshold; ++i) {
            m_history.Add(history[i]);
        }
    } else {
        m_history = history;
    }
}
