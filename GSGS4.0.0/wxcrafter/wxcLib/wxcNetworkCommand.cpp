#include "wxcNetworkCommand.h"

wxcNetworkCommand::wxcNetworkCommand()
    : m_commandType(wxCrafter::kCommandTypeInvalid)
{
}

wxcNetworkCommand::wxcNetworkCommand(const wxString& json)
{
    JSONRoot root(json);
    FromJSON( root.getObject() );
}

wxcNetworkCommand::~wxcNetworkCommand()
{
}

void wxcNetworkCommand::FromJSON(const JSONElement& json)
{
    m_commandType = (wxCrafter::eCommandType) json.getObject(wxT("m_commandType")).toInt(wxCrafter::kCommandTypeInvalid);
    m_filename    = json.getObject(wxT("m_filename")).toString();
}

JSONElement wxcNetworkCommand::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty(wxT("m_commandType"), (int)m_commandType);
    json.addProperty(wxT("m_filename"), m_filename);
    return json;
}
