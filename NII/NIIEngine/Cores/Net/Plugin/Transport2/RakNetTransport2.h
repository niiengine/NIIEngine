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

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_TelnetTransport==1

#ifndef __RAKNET_TRANSPORT_2
#define __RAKNET_TRANSPORT_2

#include "NiiNetPreInclude.h"
#include "TransportInterface.h"
#include "DS_Queue.h"
#include "NiiNetCmdBase.h"
#include "NiiNetPlugin.h"

namespace NII
{
namespace NII_NET
{

    /// \defgroup RAKNET_TRANSPORT_GROUP RakNetTransport
    /// \brief UDP based transport implementation for the ConsoleServer
    /// \details
    /// \ingroup PLUGINS_GROUP

    /// \brief Use RakNetTransport if you need a secure connection between the client and the console server.
    /// \details RakNetTransport automatically initializes security for the system.  Use the project CommandConsoleClient to connect
    /// To the ConsoleServer if you use RakNetTransport
    /// \ingroup RAKNET_TRANSPORT_GROUP
    class _EngineAPI RakNetTransport2 : public TransportInterface, public Plugin
    {
    public:
        RakNetTransport2();
        virtual ~RakNetTransport2();

        /// Start the transport provider on the indicated port.
        /// \param[in] port The port to start the transport provider on
        /// \param[in] serverMode If true, you should allow incoming connections (I don't actually use this anywhere)
        /// \return Return true on success, false on failure.
        bool Start(Nui16 port, bool serverMode);

        /// Stop the transport provider.  You can clear memory and shutdown threads here.
        void Stop(void);

        /// send a null-terminated string to \a address
        /// If your transport method requires particular formatting of the outgoing data (e.g. you don't just send strings) you can do it here
        /// and parse it out in receive().
        /// \param[in] address The player to send the string to
        /// \param[in] data format specifier - same as N_printf
        /// \param[in] ... format specification arguments - same as N_printf
        void send( Address * address, const Nui8 * data, ... );

        /// Disconnect \a address .  The binary address and port defines the Address structure.
        /// \param[in] address The player/address to disconnect
        void close( Address * address );

        /// Return a string. The string should be allocated and written to SocketMessage::mData .
        /// The byte length should be written to SocketMessage::mLength .  The player/address should be written to SocketMessage::mAddress
        /// If your transport protocol adds special formatting to the data stream you should parse it out before returning it in the msg
        /// and thus only return a string in SocketMessage::mData
        /// \return The msg structure containing the result of receive, or 0 if no data is available
        SocketMessage * receive();

        /// If a new system connects to you, you should queue that event and return the address of that player in this function.
        /// \return The Address/address of the system
        void onServerConnect(Address * address);

        /// If a system loses the connection, you should queue that event and return the address of that player in this function.
        /// \return The Address/address of the system
        void onConnectClose(Address * address);

        virtual CmdBase * GetCommandParser() { return 0; }

        /// \internal
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        /// \internal
        virtual void onConnectClose(const Address & address, UniqueID id,
            ConnectCloseType lostConnectionReason);
        /// \internal
        virtual void onConnect(const Address & address, UniqueID id);
    protected:
        DataStructures::Queue<Address *> newConnections, lostConnections;
        DataStructures::Queue<SocketMessage*> packetQueue;
    };
}
}
#endif
#endif