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
#if _RAKNET_SUPPORT_HTTPConnection2==1 && _RAKNET_SUPPORT_TCPInterface==1

#ifndef __HTTP_CONNECTION_2
#define __HTTP_CONNECTION_2

#include "NiiNetPreInclude.h"
#include "RakString.h"
#include "NiiNetCommon.h"
#include "DS_List.h"
#include "DS_Queue.h"
#include "NiiNetPlugin.h"
#include "NiiThreadManager.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Use HTTP to communicate with a web server.
    /// \details Start an instance of NetworkObj via the Start() command.
    /// This class will handle connecting to transmit a request
    class _EngineAPI HTTP : public Plugin
    {
    public:
        HTTP();
        virtual ~HTTP();

        /// \brief Connect to, then transmit a request to a TCP based server
        /// \param[in] tcp An instance of NetworkObj that previously had NetworkObj::Start() called
        /// \param[in] stringToTransmit What string to transmit. See RakString::FormatForPOST(), RakString::FormatForGET(), RakString::FormatForDELETE()
        /// \param[in] host The IP address to connect to
        /// \param[in] port The port to connect to
        /// \param[in] useSSL If to use SSL to connect. N_OpenSSLSupport must be defined to 1 in NiiNetPlat.h
        /// \param[in] ipVersion 4 for IPV4, 6 for IPV6
        /// \param[in] useAddress Assume we are connected to this address and send to it, rather than do a lookup
        /// \return false if host is not a valid IP address or domain name
        bool TransmitRequest(const char * stringToTransmit, const char * host,
            Nui16 port = 80, bool useSSL = false, int ipVersion = 4,
                Address * useAddress = IPV4Address::INVALID);

        /// \brief Check for and return a response from a prior call to TransmitRequest()
        /// As TCP is stream based, you may get a webserver reply over several calls to NetworkObj::receive()
        /// HTTP will store SocketMessage::mData and return the response to you either when the connection to the webserver is lost, or enough data has been received()
        /// This will only potentially return true after a call to ProcessTCPPacket() or OnLostConnection()
        /// \param[out] stringTransmitted The original string transmitted
        /// \param[out] hostTransmitted The parameter of the same name passed to TransmitRequest()
        /// \param[out] responseReceived The response, if any
        /// \param[out] hostReceived The Address from ProcessTCPPacket() or OnLostConnection()
        /// \param[out] contentOffset The offset from the start of responseReceived to the data body. Equivalent to searching for \r\n\r\n in responseReceived.
        /// \return true if there was a response. false if not.
        bool GetResponse(RakString & stringTransmitted, RakString & hostTransmitted,
            RakString & responseReceived, Address & hostReceived, int & contentOffset);

        /// \brief Return if any requests are pending
        bool IsBusy(void) const;

        /// \brief Return if any requests are waiting to be read by the user
        bool HasResponse(void) const;

        struct Request
        {
            RakString stringToTransmit;
            RakString stringReceived;
            RakString host;
            Address * hostEstimatedAddress;
            Address * hostCompletedAddress;
            Nui16 port;
            bool useSSL;
            int contentOffset;
            int contentLength;
            int ipVersion;
            bool chunked;
            size_t thisChunkSize;
            size_t bytesReadForThisChunk;
        };

        /// \internal
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void onConnect(const Address & address, UniqueID id);
        virtual void onConnectFail(SocketMessage * msg, ConnectFailType failedConnectionAttemptReason);
    protected:
        bool IsConnected(Address * sa);
        void SendRequest(Request * request);
        void RemovePendingRequest(Address * sa);
        void SendNextPendingRequest(void);
        void SendPendingRequestToConnectedSystem(Address * sa);

        DataStructures::Queue<Request *> pendingRequests;
        DataStructures::List<Request *> sentRequests;
        DataStructures::List<Request *> completedRequests;

        ThreadMutex pendingRequestsMutex, sentRequestsMutex, completedRequestsMutex;
    };
}
}
#endif
#endif