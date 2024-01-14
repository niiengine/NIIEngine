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
#if _RAKNET_SUPPORT_NatTypeDetectionClient==1

#ifndef __NAT_TYPE_DETECTION_CLIENT_H
#define __NAT_TYPE_DETECTION_CLIENT_H

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "PacketLevel.h"
#include "DS_OrderedList.h"
#include "RakString.h"
#include "NatTypeDetectionCommon.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Client code for NatTypeDetection
    /// \details See NatTypeDetectionServer.h for algorithm
    /// To use, just connect to the server, and call DetectNAT
    /// You will get back ID_NAT_TYPE_DETECTION_RESULT with one of the enumerated values of NATTypeDetectionResult found in NATTypeDetectionCommon.h
    /// See also http://www.jenkinssoftware.com/raknet/manual/natpunchthrough.html
    /// \sa RouterMapClient
    /// \sa NatTypeDetectionServer
    /// \ingroup NAT_TYPE_DETECTION_GROUP
    class _EngineAPI NatTypeDetectionClient : public Plugin, public RNS2EventHandler
    {
    public:
        // Constructor
        NatTypeDetectionClient();

        // Destructor
        virtual ~NatTypeDetectionClient();

        /// send the message to the server to detect the nat type
        /// Server must be running NatTypeDetectionServer
        /// We must already be connected to the server
        /// \param[in] serverAddress address of the server
        void DetectNATType(Address * _serverAddress);

        /// \internal For plugin handling
        virtual void update();

        /// \internal For plugin handling
        virtual ConnectReceiveType onMessage(SocketMessage * msg);

        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void shutdown();
        virtual void onDetach();

        virtual void onMessage(SocketMessageData * recvStruct);
        virtual void destroyMessage(SocketMessageData * s);
        virtual SocketMessageData * createMessage();
    protected:
        DataStructures::Queue<SocketMessageData *> bufferedPackets;
        ThreadMutex bufferedPacketsMutex;

        SocketObj * c2;
        void Shutdown(void);
        void OnCompletion(NATTypeDetectionResult result);
        bool IsInProgress(void) const;

        void OnTestPortRestricted(SocketMessage * msg);
        Address * serverAddress;
    };
}
}
#endif
#endif