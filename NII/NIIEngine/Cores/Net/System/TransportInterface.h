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

#ifndef __TRANSPORT_INTERFACE_H
#define __TRANSPORT_INTERFACE_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"

#define REMOTE_MAX_TEXT_INPUT 2048

namespace NII
{
namespace NII_NET
{
    /// \brief Defines an interface that is used to send and receive null-terminated strings.
    /// \details In practice this is only used by the CommandParser system for for servers.
    class _EngineAPI TransportInterface
    {
    public:
        TransportInterface() {}
        virtual ~TransportInterface() {}

        /// Start the transport provider on the indicated port.
        /// \param[in] port The port to start the transport provider on
        /// \param[in] serverMode If true, you should allow incoming connections (I don't actually use this anywhere)
        /// \return Return true on success, false on failure.
        virtual bool Start(Nui16 port, bool serverMode)=0;

        /// Stop the transport provider.  You can clear memory and shutdown threads here.
        virtual void Stop(void)=0;

        /// send a null-terminated string to \a address
        /// If your transport method requires particular formatting of the outgoing data (e.g. you don't just send strings) you can do it here
        /// and parse it out in receive().
        /// \param[in] address The player to send the string to
        /// \param[in] data format specifier - same as N_printf
        /// \param[in] ... format specification arguments - same as N_printf
        virtual void send(Address * address, const Nui8 * data, ...)=0;

        /// Disconnect \a address .  The binary address and port defines the Address structure.
        /// \param[in] address The player/address to disconnect
        virtual void close(Address * address)=0;

        /// Return a string. The string should be allocated and written to SocketMessage::data .
        /// The byte length should be written to SocketMessage::length .  The player/address should be written to SocketMessage::mAddress
        /// If your transport protocol adds special formatting to the data stream you should parse it out before returning it in the msg
        /// and thus only return a string in SocketMessage::data
        /// \return The msg structure containing the result of receive, or 0 if no data is available
        virtual SocketMessage * receive()=0;

        /// If a new system connects to you, you should queue that event and return the address of that player in this function.
        /// \return The Address/address of the system
        virtual void onServerConnect(Address * address) = 0;

        /// If a system loses the connection, you should queue that event and return the address of that player in this function.
        /// \return The Address/address of the system
        virtual void onConnectClose(Address * address) = 0;

        /// Your transport provider can itself have command parsers if the transport layer has user-modifiable features
        /// For example, your transport layer may have a password which you want remote users to be able to set or you may want
        /// to allow remote users to turn on or off command echo
        /// \return 0 if you do not need a command parser - otherwise the desired derivation of CmdBase
        virtual CmdBase * GetCommandParser() = 0;
    protected:
    };
}
}
#endif