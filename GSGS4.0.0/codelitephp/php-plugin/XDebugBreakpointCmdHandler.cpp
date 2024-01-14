#include "XDebugBreakpointCmdHandler.h"
#include "xdebugbreakpointsmgr.h"
#include <gsgsLogManager.h>
#include <wx/xml/xml.h>
#include "php_event.h"
#include <gsgsEvent.h>
#include "xdebugevent.h"

XDebugBreakpointCmdHandler::XDebugBreakpointCmdHandler(XDebugManager* mgr, int transcationId, XDebugBreakpoint &breakpoint)
    : XDebugCommandHandler(mgr, transcationId)
    , m_breakpoint(breakpoint)
{
}

XDebugBreakpointCmdHandler::~XDebugBreakpointCmdHandler()
{
}

void XDebugBreakpointCmdHandler::Process(const wxXmlNode* response)
{
    // Breakpoint assigned successfully (or not)
    wxString breakpointId = response->GetAttribute("id");
    if ( !breakpointId.IsEmpty() ) {
        long bpid(wxNOT_FOUND);
        breakpointId.ToCLong( &bpid );
        m_breakpoint.SetBreakpointId( bpid );
        gsgs_Debug("gsgs >>> Breakpoint applied successfully. Breakpoint ID: %ld", bpid);
        
        // Refresh the view
        XDebugEvent e(wxEVT_XDEBUG_BREAKPOINTS_UPDATED);
        gsgs_Event().AddPendingEvent( e );
    }
}
