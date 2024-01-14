#include "XDebugStopCmdHandler.h"
#include <wx/socket.h>
#include <gsgsLogManager.h>
#include <wx/xml/xml.h>
#include "XDebugManager.h"
#include <gsgsEvent.h>
#include "xdebugevent.h"

XDebugStopCmdHandler::XDebugStopCmdHandler(XDebugManager* mgr, int transcationId)
    : XDebugCommandHandler(mgr, transcationId)
{
}

XDebugStopCmdHandler::~XDebugStopCmdHandler()
{
}

void XDebugStopCmdHandler::Process(const wxXmlNode* response)
{
    gsgs_Debug("gsgs: Stop command completed.");
    wxString status = response->GetAttribute("status");
    if ( status == "stopping" ) {
        gsgs_Debug("gsgs: xdebug entered status 'stopping'");
        
        // Notify about control
        XDebugEvent focusEvent(wxEVT_XDEBUG_STOPPED);
        gsgs_Event().AddPendingEvent( focusEvent );

    } else if ( status == "stopped" ) {
        gsgs_Debug("gsgs: xdebug entered status 'stopped'");
        m_mgr->SendStopCommand();
        
    } else {
        // default
        m_mgr->CloseDebugSession();
    }
}
