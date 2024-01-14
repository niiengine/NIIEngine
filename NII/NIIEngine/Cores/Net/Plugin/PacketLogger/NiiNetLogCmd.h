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
#if _RAKNET_SUPPORT_LogCommandParser == 1

#ifndef __LOG_COMMAND_PARSER
#define __LOG_COMMAND_PARSER

#include "NiiNetPreInclude.h"
#include "NiiNetCmdBase.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Adds the ability to send logging output to a remote console
    class _EngineAPI LogCmd : public CmdBase
    {
    public:
        LogCmd();
        ~LogCmd();

        /// Given \a command with parameters \a parameterList , do whatever processing you wish.
        /// \param[in] command The command to process
        /// \param[in] numParameters How many parameters were passed along with the command
        /// \param[in] parameterList The list of parameters.  parameterList[0] is the first parameter and so on.
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that sent this command.
        /// \param[in] originalString The string that was actually sent over the network, in case you want to do your own parsing
        bool OnCommand(const char * command, Nui32 numParameters, char ** parameterList,
            TransportInterface * transport, const Address & address,
                const char * originalString);

        /// You are responsible for overriding this function and returning a static string, which will identifier your parser.
        /// This should return a static string
        /// \return The name that you return.
        const char *GetName(void) const;

        /// A callback for when you are expected to send a brief description of your parser to \a address
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that requested help.
        void SendHelp(TransportInterface *transport, const Address & address);

        /// All logs must be associated with a channel.  This is a filter so that remote clients only get logs for a system they care about.
        // If you call Log with a channel that is unknown, that channel will automatically be added
        /// \param[in] channelName A persistent string naming the channel.  Don't deallocate this string.
        void AddChannel(const char * channelName);

        /// Write a log to a channel.
        /// Logs are not buffered, so only remote consoles connected and subscribing at the time you write will get the output.
        /// \param[in] format Same as N_printf()
        /// \param[in] ... Same as N_printf()
        void WriteLog(const char * channelName, const char * format, ...);

        /// A callback for when \a address has connected to us.
        /// \param[in] address The player that has connected.
        /// \param[in] transport The transport interface that sent us this information.  Can be used to send messages to this or other players.
        void OnNewIncomingConnection(const Address & address, TransportInterface * transport);

        /// A callback for when \a address has disconnected, either gracefully or forcefully
        /// \param[in] address The player that has disconnected.
        /// \param[in] transport The transport interface that sent us this information.
        void OnConnectionLost(const Address & address, TransportInterface * transport);

        /// This is called every time transport interface is registered.  If you want to save a copy of the TransportInterface pointer
        /// This is the place to do it
        /// \param[in] transport The new TransportInterface
        void OnTransportChange(TransportInterface * transport);
    protected:
        /// Sends the currently active channels to the user
        /// \param[in] address The player to send to
        /// \param[in] transport The transport interface to use to send the channels
        void PrintChannels(const Address & address, TransportInterface * transport) const;

        /// Unsubscribe a user from a channel (or from all channels)
        /// \param[in] address The player to unsubscribe to
        /// \param[in] channelName If 0, then unsubscribe from all channels.  Otherwise unsubscribe from the named channel
        Nui32 Unsubscribe(const Address &address, const char *channelName);

        /// Subscribe a user to a channel (or to all channels)
        /// \param[in] address The player to subscribe to
        /// \param[in] channelName If 0, then subscribe from all channels.  Otherwise subscribe to the named channel
        Nui32 Subscribe(const Address &address, const char *channelName);

        /// Given the name of a channel, return the index into channelNames where it is located
        /// \param[in] channelName The name of the channel
        Nui32 GetChannelIndexFromName(const char *channelName);

        /// One of these structures is created per player
        struct SystemAddressAndChannel
        {
            /// The ID of the player
            Address * mAddress;

            /// Bitwise representations of the channels subscribed to.  If bit 0 is set, then we subscribe to channelNames[0] and so on.
            Nui32 channels;
        };

        /// The list of remote users.  Added to when users subscribe, removed when they disconnect or unsubscribe
        DataStructures::List<SystemAddressAndChannel> remoteUsers;

        /// Names of the channels at each bit, or 0 for an unused channel
        const char *channelNames[32];

        /// This is so I can save the current transport provider, solely so I can use it without having the user pass it to Log
        TransportInterface *trans;
    };
}
}
#endif
#endif