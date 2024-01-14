#include "XDebugUnknownCommand.h"
#include <wx/sstream.h>
#include <wx/xml/xml.h>
#include "xdebugevent.h"
#include <gsgsEvent.h>

XDebugUnknownCommand::XDebugUnknownCommand(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugUnknownCommand::~XDebugUnknownCommand()
{
}

void XDebugUnknownCommand::Process(const wxXmlNode* response)
{
    wxXmlDocument doc;
    doc.SetRoot( const_cast<wxXmlNode*>(response) );
    
    wxString asString;
    wxStringOutputStream sos( &asString );
    doc.Save( sos );
    doc.DetachRoot();
    
    XDebugEvent event(wxEVT_XDEBUG_UNKNOWN_RESPONSE);
    event.SetEvaluated( asString );
    gsgs_Event().AddPendingEvent( event );
}
