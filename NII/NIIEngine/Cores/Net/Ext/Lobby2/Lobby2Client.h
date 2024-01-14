/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef __LOBBY_2_CLIENT_H
#define __LOBBY_2_CLIENT_H

#include "Lobby2Plugin.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
    struct Lobby2Message;

    /// \brief Class used to send commands to Lobby2Server
    /// \details The lobby system works by sending implementations of Lobby2Message from Lobby2Client to Lobby2Server, and getting the results via Lobby2Client::SetCallbackInterface()<BR>
    /// The client itself is a thin shell that does little more than call export on the messages.<BR>
    /// To use:<BR>
    /// <OL>
    /// <LI>Call Lobby2Client::SetServerAddress() after connecting to the system running Lobby2Server.
    /// <LI>Instantiate an instance of Lobby2MessageFactory and register it with Lobby2Plugin::SetMessageFactory() (the base class of Lobby2Client)
    /// <LI>Call messageFactory.Alloc(command); where command is one of the Lobby2MessageID enumerations.
    /// <LI>Instantiate a (probably derived) instance of Lobby2Callbacks and register it with Lobby2Client::SetCallbackInterface()
    /// <LI>Cast the returned structure, fill in the input parameters, and call Lobby2Client::SendMsg() to send this command to the server.
    /// <LI>Wait for the result of the operation to be sent to your callback. The message will contain the original input parameters, possibly output parameters, and Lobby2Message::resultCode will be filled in.
    /// </OL>
    /// \ingroup LOBBY_2_CLIENT
    class RAK_DLL_EXPORT Lobby2Client : public Lobby2Plugin
    {
    public:
        Lobby2Client();
        virtual ~Lobby2Client();

        /// \brief Set the address of the server. When you call SendMsg() the msg will be sent to this address.
        void SetServerAddress(Address * addr);

        // \brief Return whatever was passed to SetServerAddress()
        Address * GetServerAddress(void) const;

        /// \brief Send a command to the server
        /// \param[in] msg The message that represents the command
        /// \param[in] callbackId Which callback, registered with SetCallbackInterface() or AddCallbackInterface(), should process the result. -1 for all
        virtual void SendMsg(Lobby2Message * msg);

        /// \brief Same as SendMsg()
        /// Also calls Dealloc on the message factory
        virtual void SendMsgAndDealloc(Lobby2Message * msg);
    protected:
        ConnectReceiveType onMessage(SocketMessage * msg);
        void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason);
        void shutdown();
        void OnMessage1(SocketMessage * msg);

        Address serverAddress;
        Lobby2Callbacks * callback;
    };
}
}
#endif