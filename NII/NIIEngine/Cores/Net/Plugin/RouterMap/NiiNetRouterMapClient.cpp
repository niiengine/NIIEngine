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
#if NII_RouterMapClient == 1

#include "NiiNetRouterMapClient.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "PacketLogger.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    void NatPunchthroughDebugInterface_Printf::OnClientMessage(const char * msg)
    {
        printf("%s\n", msg);
    }
    //------------------------------------------------------------------------
#if _RAKNET_SUPPORT_PacketLogger == 1
    void NatPunchthroughDebugInterface_PacketLogger::OnClientMessage(const char * msg)
    {
        if (pl)
        {
            pl->WriteMiscellaneous("Nat", msg);
        }
    }
#endif
    //------------------------------------------------------------------------
    RouterMapClient::RouterMapClient()
    {
        natPunchthroughDebugInterface = 0;
        mostRecentExternalPort = 0;
        sp.mLastActive = 0;
        portStride = 0;
        hasPortStride = UNKNOWN_PORT_STRIDE;
    }
    //------------------------------------------------------------------------
    RouterMapClient::~RouterMapClient()
    {
        mPrc2 = 0;
        Clear();
    }
    //------------------------------------------------------------------------
    void RouterMapClient::FindRouterPortStride(const Address & facilitator)
    {
        ConnectionState cs = mPrc2->getState(facilitator);
        if (cs != IS_CONNECTED)
            return;
        if (hasPortStride!=UNKNOWN_PORT_STRIDE)
            return;

        hasPortStride=CALCULATING_PORT_STRIDE;
        portStrideCalTimeout = N_Engine().getTimer().getMS()+5000;

        if (natPunchthroughDebugInterface)
        {
            String temp1;
            facilitator.read(temp1, true);
            natPunchthroughDebugInterface->OnClientMessage(RakString("Calculating port stride from %s", temp1.c_str()));
        }

        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_NAT_REQUEST_BOUND_ADDRESSES);
        mPrc2->send(&outgoingBs,PL_HIGH,PT_ORDER_MAKE_SURE,0,facilitator,false);
    }
    //------------------------------------------------------------------------
    bool RouterMapClient::OpenNAT(UniqueID destination, const Address & facilitator)
    {
        ConnectionState cs = mPrc2->getState(facilitator);
        if (cs != IS_CONNECTED)
            return false;
        if (hasPortStride==UNKNOWN_PORT_STRIDE)
        {
            FindRouterPortStride(facilitator);
            QueueOpenNAT(destination, facilitator);
        }
        else if (hasPortStride==CALCULATING_PORT_STRIDE)
        {
            QueueOpenNAT(destination, facilitator);
        }
        else
        {
            SendPunchthrough(destination, facilitator);
        }

        return true;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::SetDebugInterface(NatPunchthroughDebugInterface * i)
    {
        natPunchthroughDebugInterface = i;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::update()
    {
        TimeDurMS time = N_Engine().getTimer().getMS();

        if (hasPortStride == CALCULATING_PORT_STRIDE && time > portStrideCalTimeout)
        {
            if (natPunchthroughDebugInterface)
            {
                natPunchthroughDebugInterface->OnClientMessage("CALCULATING_PORT_STRIDE timeout");
                SendQueuedOpenNAT();
            }

            hasPortStride=UNKNOWN_PORT_STRIDE;
        }

        if (sp.mLastActive && sp.mLastActive < time)
        {
            TimeDurMS delta = time - sp.mLastActive;
            if (sp.testMode==SendPing::TESTING_INTERNAL_IPS)
            {
                sendExpedite(sp.internalIds[sp.attemptCount], ID_NAT_ESTABLISH_UNIDIRECTIONAL);

                if (++sp.retryCount>=pc.UDP_SENDS_PER_PORT_INTERNAL)
                {
                    ++sp.attemptCount;
                    sp.retryCount=0;
                }

                if (sp.attemptCount>=pc.MAXIMUM_NUMBER_OF_INTERNAL_IDS_TO_CHECK)
                {
                    sp.testMode=SendPing::WAITING_FOR_INTERNAL_IPS_RESPONSE;
                    if (pc.INTERNAL_IP_WAIT_AFTER_ATTEMPTS>0)
                    {
                        sp.mLastActive=time+pc.INTERNAL_IP_WAIT_AFTER_ATTEMPTS-delta;
                    }
                    else
                    {
                        sp.testMode=SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT;
                        sp.attemptCount=0;
                        sp.sentTTL=false;
                    }
                }
                else
                {
                    sp.mLastActive=time+pc.TIME_BETWEEN_PUNCH_ATTEMPTS_INTERNAL-delta;
                }
            }
            else if (sp.testMode==SendPing::WAITING_FOR_INTERNAL_IPS_RESPONSE)
            {
                sp.testMode=SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT;
                sp.attemptCount=0;
                sp.sentTTL=false;
            }
            else if(sp.testMode == SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT)
            {
                if(sp.sentTTL == false)
                {
                    Address * sa = sp.targetAddress;
                    sa.setHSBPort((Nui16) (sa.getHSBPort()+sp.attemptCount));
                    sendTTL(sa);

                    if(natPunchthroughDebugInterface)
                    {
                        String temp;
                        sa.read(temp, true);
                        natPunchthroughDebugInterface->OnClientMessage(RakString("send with TTL 2 to %s", temp));
                    }

                    sp.mLastActive = time + pc.EXTERNAL_IP_WAIT_AFTER_FIRST_TTL - delta;
                    sp.sentTTL=true;
                }
                else
                {
                    Address * sa=sp.targetAddress;
                    sa.setHSBPort((Nui16) (sa.getHSBPort()+sp.attemptCount));
                    sendExpedite(sa, ID_NAT_ESTABLISH_UNIDIRECTIONAL);

                    IncrementExternalAttemptCount(time, delta);

                    if (sp.attemptCount>pc.MAX_PREDICTIVE_PORT_RANGE)
                    {
                        sp.testMode=SendPing::WAITING_AFTER_ALL_ATTEMPTS;
                        sp.mLastActive=time+pc.EXTERNAL_IP_WAIT_AFTER_ALL_ATTEMPTS-delta;

                        // Skip TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT, etc.
                        /*
                        sp.testMode=SendPing::TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT;
                        sp.attemptCount=0;
                        */
                    }
                }
            }
            else if (sp.testMode == SendPing::TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT)
            {
                Address * sa = sp.targetAddress;
                if(sp.targetGuid < mPrc2->getGUID((Address *)0))
                    sa.setHSBPort((Nui16) (1024+sp.attemptCount));
                else
                    sa.setHSBPort((Nui16) (sa.getHSBPort()+sp.attemptCount));
                sendExpedite(sa, ID_NAT_ESTABLISH_UNIDIRECTIONAL);

                IncrementExternalAttemptCount(time, delta);

                if (sp.attemptCount>pc.MAX_PREDICTIVE_PORT_RANGE)
                {
                    // From 1024 disabled, never helps as I've seen, but slows down the process by half
                    sp.testMode=SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_1024;
                    sp.attemptCount=0;
                }

            }
            else if (sp.testMode==SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_1024)
            {
                Address * sa=sp.targetAddress;
                if ( sp.targetGuid > mPrc2->getGUID((Address *)0) )
                    sa.setHSBPort((Nui16) (1024+sp.attemptCount));
                else
                    sa.setHSBPort((Nui16) (sa.getHSBPort()+sp.attemptCount));
                sendExpedite(sa, ID_NAT_ESTABLISH_UNIDIRECTIONAL);

                IncrementExternalAttemptCount(time, delta);

                if (sp.attemptCount>pc.MAX_PREDICTIVE_PORT_RANGE)
                {
                    // From 1024 disabled, never helps as I've seen, but slows down the process by half
                    sp.testMode=SendPing::TESTING_EXTERNAL_IPS_1024_TO_1024;
                    sp.attemptCount=0;
                }
            }
            else if (sp.testMode==SendPing::TESTING_EXTERNAL_IPS_1024_TO_1024)
            {
                Address * sa = sp.targetAddress;
                sa.setHSBPort((Nui16) (1024+sp.attemptCount));
                sendExpedite(sa, ID_NAT_ESTABLISH_UNIDIRECTIONAL);

                IncrementExternalAttemptCount(time, delta);

                if (sp.attemptCount>pc.MAX_PREDICTIVE_PORT_RANGE)
                {
                    if (natPunchthroughDebugInterface)
                    {
                        String ipAddressString;
                        sp.targetAddress.read(ipAddressString, true);
                        String guidString;
                        sp.targetGuid.read(guidString);
                        natPunchthroughDebugInterface->OnClientMessage(RakString("Likely bidirectional punchthrough failure to id %s, system address %s.", guidString, ipAddressString));
                    }

                    sp.testMode = SendPing::WAITING_AFTER_ALL_ATTEMPTS;
                    sp.mLastActive = time+pc.EXTERNAL_IP_WAIT_AFTER_ALL_ATTEMPTS-delta;
                }
            }
            else if(sp.testMode == SendPing::WAITING_AFTER_ALL_ATTEMPTS)
            {
                // Failed. Tell the user
                OnPunchthroughFailure();
            }

            if(sp.testMode == SendPing::PUNCHING_FIXED_PORT)
            {
                sendExpedite(sp.targetAddress, ID_NAT_ESTABLISH_BIDIRECTIONAL);
                if(++sp.retryCount >= sp.punchingFixedPortAttempts)
                {
                    if (natPunchthroughDebugInterface)
                    {
                        String ipAddressString;
                        sp.targetAddress.read(ipAddressString, true);
                        String guidString;
                        sp.targetGuid.read(guidString);
                        natPunchthroughDebugInterface->OnClientMessage(RakString("Likely unidirectional punchthrough failure to id %s, system address %s.", guidString, ipAddressString));
                    }

                    sp.testMode=SendPing::WAITING_AFTER_ALL_ATTEMPTS;
                    sp.mLastActive=time+pc.EXTERNAL_IP_WAIT_AFTER_ALL_ATTEMPTS-delta;
                }
                else
                {
                    if ((sp.retryCount%pc.UDP_SENDS_PER_PORT_EXTERNAL)==0)
                        sp.mLastActive=time+pc.EXTERNAL_IP_WAIT_BETWEEN_PORTS-delta;
                    else
                        sp.mLastActive=time+pc.TIME_BETWEEN_PUNCH_ATTEMPTS_EXTERNAL-delta;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::PushFailure(void)
    {
        SocketMessage * p = createMessage(sizeof(Token)+sizeof(Nui8));
        p->mData[0] = ID_NAT_PUNCHTHROUGH_FAILED;
        p->mAddress = sp.targetAddress;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = sp.targetGuid;

        if(sp.weAreSender)
            p->mData[1] = 1;
        else
            p->mData[1] = 0;

        p->mLocal = true;
        mPrc2->addMessage(p, true);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::OnPunchthroughFailure(void)
    {
        if(pc.retryOnFailure == false)
        {
            if(natPunchthroughDebugInterface)
            {
                String ipAddressString;
                sp.targetAddress.read(ipAddressString, true);
                String guidString;
                sp.targetGuid.read(guidString);
                natPunchthroughDebugInterface->OnClientMessage(RakString("Failed punchthrough once. Returning failure to id %s, system address %s to user.", guidString, ipAddressString));
            }

            PushFailure();
            OnReadyForNextPunchthrough();
            return;
        }

        Nui32 i;
        for (i=0; i < failedAttemptList.Size(); i++)
        {
            if (failedAttemptList[i].mID==sp.targetGuid)
            {
                if (natPunchthroughDebugInterface)
                {
                    String ipAddressString;
                    sp.targetAddress.read(ipAddressString, true);
                    String guidString;
                    sp.targetGuid.read(guidString);
                    natPunchthroughDebugInterface->OnClientMessage(RakString("Failed punchthrough twice. Returning failure to id %s, system address %s to user.", guidString, ipAddressString));
                }

                // Failed a second time, so return failed to user
                PushFailure();

                OnReadyForNextPunchthrough();

                failedAttemptList.RemoveAtIndexFast(i);
                return;
            }
        }

        if(mPrc2->getState(sp.facilitator)!=IS_CONNECTED)
        {
            if (natPunchthroughDebugInterface)
            {
                String ipAddressString;
                sp.targetAddress.read(ipAddressString, true);
                String guidString;
                sp.targetGuid.read(guidString);
                natPunchthroughDebugInterface->OnClientMessage(RakString("Not connected to facilitator, so cannot retry punchthrough after first failure. Returning failure onj id %s, system address %s to user.", guidString, ipAddressString));
            }

            // Failed, and can't try again because no facilitator
            PushFailure();
            return;
        }

        if(natPunchthroughDebugInterface)
        {
            String ipAddressString;
            sp.targetAddress.read(ipAddressString, true);
            String guidString;
            sp.targetGuid.read(guidString);
            natPunchthroughDebugInterface->OnClientMessage(RakString("First punchthrough failure on id %s, system address %s. Reattempting.", guidString, ipAddressString));
        }

        // Failed the first time. Add to the failure queue and try again
        AddrAndGuid aag;
        aag.addr=sp.targetAddress;
        aag.mID=sp.targetGuid;
        failedAttemptList.Push(aag);

        // Tell the server we are ready
        OnReadyForNextPunchthrough();

        // If we are the sender, try again, immediately if possible, else added to the queue on the faciltiator
        if(sp.weAreSender)
            SendPunchthrough(sp.targetGuid, sp.facilitator);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType RouterMapClient::onMessage(SocketMessage * msg)
    {
        switch(msg->mData[0])
        {
        case ID_NAT_GET_MOST_RECENT_PORT:
            {
                OnGetMostRecentPort(msg);
                return CRT_Destroy;
            }
        case ID_NAT_PUNCHTHROUGH_FAILED:
        case ID_NAT_PUNCHTHROUGH_SUCCEEDED:
            if(msg->mLocal == false)
                return CRT_Destroy;
            break;
        case ID_NAT_RESPOND_BOUND_ADDRESSES:
            {
                String temp;
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(1, 0);
                Nui8 boundAddressCount;
                bs.read(boundAddressCount);
                if(boundAddressCount<2)
                {
                    if(natPunchthroughDebugInterface)
                    {
                        mPrc2->getConnectTo(msg->mAddress).read(temp);
                        natPunchthroughDebugInterface->OnClientMessage(RakString("INCAPABLE_PORT_STRIDE. My external ID is %s", temp));
                    }
                    hasPortStride=INCAPABLE_PORT_STRIDE;
                    SendQueuedOpenNAT();
                }

                Address * boundAddresses[N_Socket_MaxLocalIP];
                for(NCount i = 0; i < boundAddressCount && i < N_Socket_MaxLocalIP; ++i)
                {
                    bs.read(boundAddresses[i]);
                    if(boundAddresses[i] != msg->mAddress)
                    {
                        NetSerializer outgoingBs;
                        outgoingBs.write((Token)ID_NAT_PING);
                        Nui16 externalPort = mPrc2->getConnectTo(msg->mAddress).getHSBPort();
                        outgoingBs.write(externalPort);
                        boundAddresses[i].read(temp, false);
                        mPrc2->sendExpedite(temp, boundAddresses[i].getHSBPort(),
                            (const char *)outgoingBs.getData(), outgoingBs.getSize());
                        break;
                    }
                }
            }
            break;
        case ID_OUT_OF_BAND_INTERNAL:
            if (msg->mLength >= 2 && msg->mData[1] == ID_NAT_PONG)
            {
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(2, 0);
                Nui16 externalPort;
                bs.read(externalPort);
                Nui16 externalPort2;
                bs.read(externalPort2);
                portStride = externalPort2 - externalPort;
                mostRecentExternalPort = externalPort2;
                hasPortStride = HAS_PORT_STRIDE;

                if(natPunchthroughDebugInterface)
                    natPunchthroughDebugInterface->OnClientMessage(RakString("HAS_PORT_STRIDE %i. First external port %i. Second external port %i.", portStride, externalPort, externalPort2));

                SendQueuedOpenNAT();
                return CRT_Destroy;
            }
            else if (msg->mLength >= 2 &&
                (msg->mData[1] == ID_NAT_ESTABLISH_UNIDIRECTIONAL || msg->mData[1] == ID_NAT_ESTABLISH_BIDIRECTIONAL) &&
                sp.mLastActive != 0)
            {
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(2, 0);
                Nui16 sessionId;
                bs.read(sessionId);
                if(sessionId != sp.sessionId)
                    break;

                String ipAddressString;
                msg->mAddress.read(ipAddressString, true);
                // sp.targetGuid == msg->mID is because the internal IP addresses reported may include loopbacks not reported by NetworkSys::isLocalHost()
                if(msg->mData[1] == ID_NAT_ESTABLISH_UNIDIRECTIONAL && sp.targetGuid==msg->mID)
                {
                    if(sp.testMode != SendPing::PUNCHING_FIXED_PORT)
                    {
                        sp.testMode = SendPing::PUNCHING_FIXED_PORT;
                        sp.retryCount += sp.attemptCount * pc.UDP_SENDS_PER_PORT_EXTERNAL;
                        sp.targetAddress = msg->mAddress;
                        // Keeps trying until the other side gives up too, in case it is unidirectional
                        sp.punchingFixedPortAttempts=pc.UDP_SENDS_PER_PORT_EXTERNAL*(pc.MAX_PREDICTIVE_PORT_RANGE+1);

                        if(natPunchthroughDebugInterface)
                        {
                            String guidString;
                            sp.targetGuid.read(guidString);
                            natPunchthroughDebugInterface->OnClientMessage(
                                RakString("PUNCHING_FIXED_PORT: Received ID_NAT_ESTABLISH_UNIDIRECTIONAL from id %s, system address %s.", guidString, ipAddressString));
                        }
                    }
                    else
                    {
                        if(natPunchthroughDebugInterface)
                        {
                            String guidString;
                            sp.targetGuid.read(guidString);
                            natPunchthroughDebugInterface->OnClientMessage(
                                RakString("Received ID_NAT_ESTABLISH_UNIDIRECTIONAL from id %s, system address %s.", guidString, ipAddressString));
                        }
                    }

                    sendExpedite(sp.targetAddress, ID_NAT_ESTABLISH_BIDIRECTIONAL);
                }
                else if (msg->mData[1] == ID_NAT_ESTABLISH_BIDIRECTIONAL &&
                    sp.targetGuid == msg->mID)
                {
                    // They send back our port
                    Nui16 ourExternalPort;
                    bs.read(ourExternalPort);
                    if (mostRecentExternalPort==0)
                    {
                        mostRecentExternalPort=ourExternalPort;

                        if (natPunchthroughDebugInterface)
                        {
                            natPunchthroughDebugInterface->OnClientMessage(RakString("ID_NAT_ESTABLISH_BIDIRECTIONAL mostRecentExternalPort first time set to %i", mostRecentExternalPort));
                        }
                    }
                    else
                    {
                        if (sp.testMode!=SendPing::TESTING_INTERNAL_IPS && sp.testMode!=SendPing::WAITING_FOR_INTERNAL_IPS_RESPONSE)
                        {
                            if (hasPortStride!=HAS_PORT_STRIDE)
                            {
                                portStride = ourExternalPort - mostRecentExternalPort;
                                hasPortStride=HAS_PORT_STRIDE;

                                if (natPunchthroughDebugInterface)
                                {
                                    natPunchthroughDebugInterface->OnClientMessage(RakString("ID_NAT_ESTABLISH_BIDIRECTIONAL: Estimated port stride from incoming connection at %i. ourExternalPort=%i mostRecentExternalPort=%i", portStride, ourExternalPort, mostRecentExternalPort));
                                }

                                SendQueuedOpenNAT();
                            }

                            //nextExternalPort += portStride * (pc.MAX_PREDICTIVE_PORT_RANGE+1);
                            mostRecentExternalPort = ourExternalPort;

                            if (natPunchthroughDebugInterface)
                            {
                                natPunchthroughDebugInterface->OnClientMessage(RakString("ID_NAT_ESTABLISH_BIDIRECTIONAL: New mostRecentExternalPort %i", mostRecentExternalPort));
                            }
                        }
                    }
                    sendExpedite(msg->mAddress, ID_NAT_ESTABLISH_BIDIRECTIONAL);

                    // Tell the user about the success
                    sp.targetAddress=msg->mAddress;
                    PushSuccess();
                    OnReadyForNextPunchthrough();
                    bool removedFromFailureQueue=RemoveFromFailureQueue();

                    if(natPunchthroughDebugInterface)
                    {
                        String guidString;
                        sp.targetGuid.read(guidString);
                        if(removedFromFailureQueue)
                            natPunchthroughDebugInterface->OnClientMessage(RakString("Punchthrough to id %s, system address %s succeeded on 2nd attempt.", guidString, ipAddressString));
                        else
                            natPunchthroughDebugInterface->OnClientMessage(RakString("Punchthrough to id %s, system address %s succeeded on 1st attempt.", guidString, ipAddressString));
                    }
                }
            }
            return CRT_Destroy;
        case ID_NAT_ALREADY_IN_PROGRESS:
            {
                NetSerializer incomingBs(msg->mData, msg->mLength, false);
                incomingBs.skipRead(1, 0);
                UniqueID targetGuid;
                incomingBs.read(targetGuid);
                if(natPunchthroughDebugInterface)
                {
                    String guidString;
                    targetGuid.read(guidString);
                    natPunchthroughDebugInterface->OnClientMessage(RakString("Punchthrough retry to id %s failed due to ID_NAT_ALREADY_IN_PROGRESS. Returning failure.", guidString));
                }

            }
            break;
        case ID_NAT_TARGET_NOT_CONNECTED:
        case ID_NAT_CONNECTION_TO_TARGET_LOST:
        case ID_NAT_TARGET_UNRESPONSIVE:
            {
                const char *reason;
                if (msg->mData[0]==ID_NAT_TARGET_NOT_CONNECTED)
                    reason=(char *)"ID_NAT_TARGET_NOT_CONNECTED";
                else if (msg->mData[0]==ID_NAT_CONNECTION_TO_TARGET_LOST)
                    reason=(char *)"ID_NAT_CONNECTION_TO_TARGET_LOST";
                else
                    reason=(char *)"ID_NAT_TARGET_UNRESPONSIVE";


                NetSerializer incomingBs(msg->mData, msg->mLength, false);
                incomingBs.skipRead(1, 0);
                UniqueID targetGuid;
                incomingBs.read(targetGuid);

                if (msg->mData[0]==ID_NAT_CONNECTION_TO_TARGET_LOST ||
                    msg->mData[0]==ID_NAT_TARGET_UNRESPONSIVE)
                {
                    Nui16 sessionId;
                    incomingBs.read(sessionId);
                    if (sessionId!=sp.sessionId)
                        break;
                }

                NCount i;
                for (i = 0; i < failedAttemptList.Size(); ++i)
                {
                    if (failedAttemptList[i].mID == targetGuid)
                    {
                        if (natPunchthroughDebugInterface)
                        {
                            String guidString;
                            targetGuid.read(guidString);
                            natPunchthroughDebugInterface->OnClientMessage(RakString("Punchthrough retry to id %s failed due to %s.", guidString, reason));

                        }

                        failedAttemptList.RemoveAtIndexFast(i);
                        break;
                    }
                }

                if (natPunchthroughDebugInterface)
                {
                    String guidString;
                    targetGuid.read(guidString);
                    natPunchthroughDebugInterface->OnClientMessage(RakString("Punchthrough attempt to id %s failed due to %s.", guidString, reason));
                }

                // Stop trying punchthrough
                sp.mLastActive=0;
            }
            break;
        case ID_TIMESTAMP:
            if (msg->mData[sizeof(Token)+sizeof(TimeDurMS)]==ID_NAT_CONNECT_AT_TIME)
            {
                OnConnectAtTime(msg);
                return CRT_Destroy;
            }
            break;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::OnConnectAtTime(SocketMessage * msg)
    {
        NetSerializer bs(msg->mData, msg->mLength, false);
        bs.skipRead(1, 0);
        bs.read(sp.mLastActive);
        bs.skipRead(1, 0);
        bs.read(sp.sessionId);
        bs.read(sp.targetAddress);
        Nui8 count;
        bs.read((Nui8)count);
        int j;
        for(j = 0; j < count; ++j)
            bs.read(sp.internalIds[j]);

        sp.attemptCount=0;
        sp.retryCount=0;
        if (pc.MAXIMUM_NUMBER_OF_INTERNAL_IDS_TO_CHECK>0)
        {
            sp.testMode=SendPing::TESTING_INTERNAL_IPS;
        }
        else
        {
            // TESTING: Try sending to unused ports on the remote system to reserve our own ports while not getting banned
            //sp.testMode=SendPing::SEND_WITH_TTL;
            sp.testMode=SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT;
            sp.attemptCount=0;
            sp.sentTTL=false;
        }
        bs.read(sp.targetGuid);
        bs.read(sp.weAreSender);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::sendTTL(const Address & sa)
    {
        if(sa == 0 || sa->isInvalid())
            return;
        if(sa.getHSBPort() == 0)
            return;

        String ipAddressString;
        sa.read(ipAddressString, false);
        // TTL of 1 doesn't get past the router, 2 might hit the other system on a LAN
        mPrc2->sendTTL(ipAddressString, sa.getHSBPort(), 2);
    }
    //------------------------------------------------------------------------
    char *TestModeToString(RouterMapClient::SendPing::TestMode tm)
    {
        switch (tm)
        {
            case RouterMapClient::SendPing::TESTING_INTERNAL_IPS:
                return "TESTING_INTERNAL_IPS";
            break;
            case RouterMapClient::SendPing::WAITING_FOR_INTERNAL_IPS_RESPONSE:
                return "WAITING_FOR_INTERNAL_IPS_RESPONSE";
            break;
            case RouterMapClient::SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT:
                return "TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_FACILITATOR_PORT";
            break;
            case RouterMapClient::SendPing::TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT:
                return "TESTING_EXTERNAL_IPS_1024_TO_FACILITATOR_PORT";
            break;
            case RouterMapClient::SendPing::TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_1024:
                return "TESTING_EXTERNAL_IPS_FACILITATOR_PORT_TO_1024";
            break;
            case RouterMapClient::SendPing::TESTING_EXTERNAL_IPS_1024_TO_1024:
                return "TESTING_EXTERNAL_IPS_1024_TO_1024";
            break;
            case RouterMapClient::SendPing::WAITING_AFTER_ALL_ATTEMPTS:
                return "WAITING_AFTER_ALL_ATTEMPTS";
            break;
            case RouterMapClient::SendPing::PUNCHING_FIXED_PORT:
                return "PUNCHING_FIXED_PORT";
            break;
        }
        return "";
    }
    //------------------------------------------------------------------------
    void RouterMapClient::sendExpedite(Address * sa, Token oobId)
    {
        if(sa == 0 || sa->isInvalid())
            return;
        if(sa.getHSBPort() == 0)
            return;

        NetSerializer oob;
        oob.write(oobId);
        oob.write(sp.sessionId);
        if(oobId == ID_NAT_ESTABLISH_BIDIRECTIONAL)
            oob.write(sa.getHSBPort());
        String ip;
        sa.read(ip, false);
        mPrc2->sendExpedite(ip, sa.getHSBPort(), oob.getData(), oob.getSize());

        if(natPunchthroughDebugInterface)
        {
            sa.read(ip, true);
            String guidString;
            sp.targetGuid.read(guidString);

            // server - diff = my time
            // server = myTime + diff
            TimeDurMS clockDifferential = mPrc2->getTimeEquation(sp.facilitator);
            TimeDurMS serverTime = N_Engine().getTimer().getMS() + clockDifferential;

            if(oobId == ID_NAT_ESTABLISH_UNIDIRECTIONAL)
                natPunchthroughDebugInterface->OnClientMessage(RakString("%I64d: %s: OOB ID_NAT_ESTABLISH_UNIDIRECTIONAL to id %s, system address %s.\n", serverTime, TestModeToString(sp.testMode), guidString, ip));
            else
                natPunchthroughDebugInterface->OnClientMessage(RakString("%I64d: %s: OOB ID_NAT_ESTABLISH_BIDIRECTIONAL to id %s, system address %s.\n", serverTime, TestModeToString(sp.testMode), guidString, ip));
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::onConnect(const Address * address, UniqueID id)
    {
        (void) id;
        // Try to track new port mappings on the router. Not reliable, but better than nothing.
        Address * ourExternalId = mPrc2->getConnectTo(address);
        if(ourExternalId != 0 && mostRecentExternalPort == 0)
        {
            mostRecentExternalPort = ourExternalId->getHSBPort();

            if(natPunchthroughDebugInterface)
            {
                natPunchthroughDebugInterface->OnClientMessage(RakString("onConnect mostRecentExternalPort first time set to %i", mostRecentExternalPort));
            }
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::onConnectClose(const Address & address,
        UniqueID id, ConnectCloseType lostConnectionReason )
    {
        (void) id;
        (void) lostConnectionReason;

        if (sp.facilitator == address)
        {
            // If we lose the connection to the facilitator, all previous failures not currently in progress are returned as such
            NCount i = 0;
            while (i < failedAttemptList.Size())
            {
                if (sp.mLastActive != 0 && sp.targetGuid == failedAttemptList[i].mID)
                {
                    ++i;
                    continue;
                }

                PushFailure();

                failedAttemptList.RemoveAtIndexFast(i);
            }
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::GetUPNPPortMappings(char * externalPort,
        char * internalPort, const Address * natPunchthroughServerAddress)
    {
        DataStructures::List<SocketObj *> sockets;
        mPrc2->getSockets(sockets);
        i_a(sockets[0]->GetBoundAddress().getHSBPort(),internalPort,10);
        if(mostRecentExternalPort == 0)
            mostRecentExternalPort = mPrc2->getConnectTo(natPunchthroughServerAddress).getHSBPort();
        i_a(mostRecentExternalPort,externalPort,10);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::OnFailureNotification(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(1, 0);
        UniqueID senderGuid;
        incomingBs.read(senderGuid);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::OnGetMostRecentPort(SocketMessage * msg)
    {
        NetSerializer incomingBs(msg->mData, msg->mLength, false);
        incomingBs.skipRead(1, 0);
        Nui16 sessionId;
        incomingBs.read(sessionId);

        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_NAT_GET_MOST_RECENT_PORT);
        outgoingBs.write(sessionId);
        if(mostRecentExternalPort == 0)
        {
            mostRecentExternalPort = mPrc2->getConnectTo(msg->mAddress).getHSBPort();
            N_assert(mostRecentExternalPort!=0);

            if(natPunchthroughDebugInterface)
            {
                natPunchthroughDebugInterface->OnClientMessage(RakString("OnGetMostRecentPort mostRecentExternalPort first time set to %i", mostRecentExternalPort));
            }
        }

        Nui16 portWithStride;
        if (hasPortStride==HAS_PORT_STRIDE)
            portWithStride = mostRecentExternalPort + portStride;
        else
            portWithStride = mostRecentExternalPort;
        outgoingBs.write(portWithStride);

        mPrc2->send(&outgoingBs,PL_HIGH,PT_ORDER_MAKE_SURE,0,msg->mAddress,false);
        sp.facilitator = msg->mAddress;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::QueueOpenNAT(UniqueID destination, const Address &facilitator)
    {
        DSTAndFac daf;
        daf.destination=destination;
        daf.facilitator=facilitator;
        queuedOpenNat.Push(daf);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::SendQueuedOpenNAT(void)
    {
        while (queuedOpenNat.IsEmpty()==false)
        {
            DSTAndFac daf = queuedOpenNat.Pop();
            SendPunchthrough(daf.destination, daf.facilitator);
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::SendPunchthrough(UniqueID destination, const Address & facilitator)
    {
        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_NAT_PUNCHTHROUGH_REQUEST);
        outgoingBs.write(destination);
        mPrc2->send(&outgoingBs, PL_HIGH,PT_ORDER_MAKE_SURE, 0, facilitator, false);

        if(natPunchthroughDebugInterface)
        {
            String guidString;
            destination.read(guidString);
            natPunchthroughDebugInterface->OnClientMessage(RakString("Starting ID_NAT_PUNCHTHROUGH_REQUEST to id %s.", guidString));
        }
    }
    //------------------------------------------------------------------------
    void RouterMapClient::onAttach()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RouterMapClient::onDetach()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RouterMapClient::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void RouterMapClient::Clear(void)
    {
        OnReadyForNextPunchthrough();

        failedAttemptList.Clear(false);

        queuedOpenNat.Clear();
    }
    //------------------------------------------------------------------------
    PunchthroughConfiguration* RouterMapClient::GetPunchthroughConfiguration(void)
    {
        return &pc;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::OnReadyForNextPunchthrough(void)
    {
        if (mPrc2 == 0)
            return;

        sp.mLastActive = 0;

        NetSerializer outgoingBs;
        outgoingBs.write((Token)ID_NAT_CLIENT_READY);
        mPrc2->send(&outgoingBs,PL_HIGH,PT_ORDER_MAKE_SURE,0,sp.facilitator,false);
    }
    //------------------------------------------------------------------------
    void RouterMapClient::PushSuccess(void)
    {
        SocketMessage * p = createMessage(sizeof(Token) + sizeof(Nui8));
        p->mData[0] = ID_NAT_PUNCHTHROUGH_SUCCEEDED;
        p->mAddress = sp.targetAddress;
        p->mAddress.mIndex = (Nindex)-1;
        p->mID = sp.targetGuid;

        if(sp.weAreSender)
            p->mData[1] = 1;
        else
            p->mData[1] = 0;

        p->mLocal = true;
        mPrc2->addMessage(p, true);
    }
    //------------------------------------------------------------------------
    bool RouterMapClient::RemoveFromFailureQueue(void)
    {
        NCount i;
        for (i=0; i < failedAttemptList.Size(); ++i)
        {
            if (failedAttemptList[i].mID == sp.targetGuid)
            {
                failedAttemptList.RemoveAtIndexFast(i);
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void RouterMapClient::IncrementExternalAttemptCount(TimeDurMS time, TimeDurMS delta)
    {
        if (++sp.retryCount >= pc.UDP_SENDS_PER_PORT_EXTERNAL)
        {
            ++sp.attemptCount;
            sp.retryCount = 0;
            sp.mLastActive = time + pc.EXTERNAL_IP_WAIT_BETWEEN_PORTS-delta;
            sp.sentTTL = false;
        }
        else
        {
            sp.mLastActive = time + pc.TIME_BETWEEN_PUNCH_ATTEMPTS_EXTERNAL-delta;
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*