#include "wxcNetworkThread.h"
#include "SocketAPI/gsgsSocketBase.h"
#include "SocketAPI/gsgsSocketServer.h"
#include "allocator_mgr.h"
#include "wxcEvent.h"
#include "wxcLib/wxcLibAPI.h"
#include <gsgsEvent.h>
#include <json_node.h>

wxcNetworkThread::wxcNetworkThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

wxcNetworkThread::~wxcNetworkThread() {}

void* wxcNetworkThread::Entry()
{
    SocketServer server;
    try {
        wxString connectionString;
        connectionString << "tcp://127.0.0.1" << WXC_PORT_NUMBER_INT;
        server.Start(connectionString);

        gsgs::SocketBase::Ptr_t client;
        while(!TestDestroy() && !client) {
            client = server.WaitForNewConnection(1);
        }

        if(!client) {
            // Delete was called, return NULL
            return NULL;
        }

        // Dont close the connection on exit
        client->SetCloseOnExit(false);

        // send the connection to the main thread as well
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_CONN_ESTASBLISHED);
        event.SetInt(client->GetSocket());
        gsgs_Event().AddPendingEvent(event);

        // Main Loop
        while(!TestDestroy()) {
            wxString message;
            if(client->ReadMessage(message, 1) == gsgs::SocketBase::kSuccess) {
                // Got a message
                ProcessMessage(message);
            }
        }

    } catch(gsgs::SocketException& e) {
        // something bad had happened
        return NULL;
    }
    return NULL;
}

void wxcNetworkThread::ProcessMessage(const wxString& message)
{
    wxcNetworkCommand command(message);
    switch(command.GetCommandType()) {
    case wxCrafter::kCommandTypeShowDesigner: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_SHOW_DESIGNER);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeExit: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_EXIT);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeLoadFile: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_LOAD_FILE);
        event.SetFileName(command.GetFilename());
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeGenerateCode: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_CONN_GEN_CODE);
        event.SetFileName(command.GetFilename());
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewDialog: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXDIALOG);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewFrame: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXFRAME);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewPanel: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXPANEL_TOPLEVEL);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewPopupWindow: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXPOPUPWINDOW);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewWizard: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXWIZARD);
        gsgs_Event().AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewImageList: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXIMAGELIST);
        gsgs_Event().AddPendingEvent(event);
    } break;
    default:
        break;
    }
}
