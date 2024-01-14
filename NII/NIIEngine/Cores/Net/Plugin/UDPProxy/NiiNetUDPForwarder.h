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
#if _RAKNET_SUPPORT_UDPForwarder==1

#ifndef __UDP_FORWARDER_H
#define __UDP_FORWARDER_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "UDPProxyCommon.h"
#include "NiiThreadManager.h"
#include "RakString.h"
#include "NiiThreadManager.h"
#include "DS_Queue.h"
#include "DS_OrderedList.h"
#include "NiiThreadManager.h"
#include "DS_ThreadsafeAllocatingQueue.h"

namespace NII
{
namespace NII_NET
{
    enum UDPForwarderResult
    {
        UDPFORWARDER_FORWARDING_ALREADY_EXISTS,
        UDPFORWARDER_NO_SOCKETS,
        UDPFORWARDER_BIND_FAILED,
        UDPFORWARDER_INVALID_PARAMETERS,
        UDPFORWARDER_NOT_RUNNING,
        UDPFORWARDER_SUCCESS,
        UDPFORWARDER_RESULT_COUNT
    };

    /// \brief Forwards UDP datagrams. Independent of RakNet's protocol.
    /// \ingroup NAT_PUNCHTHROUGH_GROUP
    class _EngineAPI UDPForwarder : public ThreadMain
    {
    public:
        UDPForwarder();
        virtual ~UDPForwarder();

        /// Starts the system.
        /// Required to call before StartForwarding
        void Startup(void);

        /// Stops the system, and frees all sockets
        void Shutdown(void);

        void run();
        
        /// Sets the maximum number of forwarding entries allowed
        /// Set according to your available bandwidth and the estimated average bandwidth per forwarded address.
        /// \param[in] maxEntries The maximum number of simultaneous forwarding entries. Defaults to 64 (32 connections)
        void SetMaxForwardEntries(Nui16 maxEntries);

        /// \return The \a maxEntries parameter passed to SetMaxForwardEntries(), or the default if it was never called
        int GetMaxForwardEntries(void) const;

        /// \return How many entries have been used
        int GetUsedForwardEntries(void) const;

        /// Forwards datagrams from source to destination, and vice-versa
        /// Does nothing if this forward entry already exists via a previous call
        /// \pre Call Startup()
        /// \note RakNet's protocol will ensure a message is sent at least every 15 seconds, so if routing RakNet messages, it is a reasonable value for timeoutOnNoDataMS, plus an some extra seconds for latency
        /// \param[in] source The source IP and port
        /// \param[in] destination Where to forward to (and vice-versa)
        /// \param[in] timeoutOnNoDataMS If no messages are forwarded for this many MS, then automatically remove this entry.
        /// \param[in] forceHostAddress Force binding on a particular address. 0 to use any.
        /// \param[in] socketFamily IP version: For IPV4, use AF_INET (default). For IPV6, use AF_INET6. To autoselect, use AF_UNSPEC.
        /// \param[out] forwardingPort New opened port for forwarding
        /// \param[out] forwardingSocket New opened socket for forwarding
        /// \return UDPForwarderResult
        UDPForwarderResult StartForwarding(Address * source, Address * destination, 
            TimeDurMS timeoutOnNoDataMS, const char * forceHostAddress, Nui16 socketFamily,
                Nui16 * forwardingPort, Nudpsocket * forwardingSocket);

        /// No longer forward datagrams from source to destination
        /// \param[in] source The source IP and port
        /// \param[in] destination Where to forward to
        void StopForwarding(Address * source, Address * destination);

        struct ForwardEntry
        {
            ForwardEntry();
            ~ForwardEntry();
            Address * addr1Unconfirmed, * addr2Unconfirmed, * addr1Confirmed, * addr2Confirmed;
            TimeDurMS timeLastDatagramForwarded;
            Nudpsocket socket;
            TimeDurMS timeoutOnNoDataMS;
            short socketFamily;
        };
    protected:
        void UpdateUDPForwarder(void);
        void RecvFrom(TimeDurMS curTime, ForwardEntry *forwardEntry);

        struct StartForwardingInputStruct
        {
            Address * source;
            Address * destination;
            TimeDurMS timeoutOnNoDataMS;
            RakString forceHostAddress;
            Nui16 socketFamily;
            Nui32 inputId;
        };

        DataStructures::ThreadsafeAllocatingQueue<StartForwardingInputStruct> startForwardingInput;

        struct StartForwardingOutputStruct
        {
            Nui16 forwardingPort;
            Nudpsocket forwardingSocket;
            UDPForwarderResult result;
            Nui32 inputId;
        };
        DataStructures::Queue<StartForwardingOutputStruct> startForwardingOutput;
        ThreadMutex startForwardingOutputMutex;

        struct StopForwardingStruct
        {
            Address *source;
            Address * destination;
        };
        DataStructures::ThreadsafeAllocatingQueue<StopForwardingStruct> stopForwardingCommands;
        Thread * mThread;
        Nui32 nextInputId;
        // New entries are added to forwardListNotUpdated
        DataStructures::List<ForwardEntry *> forwardListNotUpdated;
        Nui16 maxForwardEntries;
        STNui32 isRunning, threadRunning;
    };
}
}
#endif
#endif