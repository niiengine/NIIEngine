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
#if NII_LinkState == 1

#include "NiiPreProcess.h"
#include "NiiNetLinkState.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int LinkState::RemoteSystemComp(const UniqueID & key, RemoteSystem * const & data)
    {
        if (key < data->mID)
            return -1;
        if (key > data->mID)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int LinkState::SystemAddressAndGuidComp(const SystemAddressAndGuid & key,
        const SystemAddressAndGuid & data)
    {
        if (key.mID < data.mID)
            return -1;
        if (key.mID > data.mID)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    LinkState::LinkState()
    {
        autoProcessNewConnections=true;
    }
    //------------------------------------------------------------------------
    LinkState::~LinkState()
    {

    }
    //------------------------------------------------------------------------
    bool LinkState::GetConnectionListForRemoteSystem(UniqueID remoteSystemGuid,
        Address * saOut, UniqueID * guidOut, Nui32 * outLength)
    {
        if((saOut==0 && guidOut==0) || outLength==0 || *outLength==0 || 
            remoteSystemGuid==UniqueID::INVALID)
        {
            *outLength=0;
            return false;
        }

        bool objectExists;
        Nui32 idx = mRemotes.GetIndexFromKey(remoteSystemGuid, &objectExists);
        if (objectExists==false)
        {
            *outLength=0;
            return false;
        }

        Nui32 idx2;
        if (mRemotes[idx]->remoteConnections.Size() < *outLength)
            *outLength=mRemotes[idx]->remoteConnections.Size();
        for (idx2=0; idx2 < *outLength; idx2++)
        {
            if (guidOut)
                guidOut[idx2]=mRemotes[idx]->remoteConnections[idx2].mID;
            if (saOut)
                saOut[idx2]=mRemotes[idx]->remoteConnections[idx2].mAddress;
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool LinkState::ConnectionExists(UniqueID g1, UniqueID g2)
    {
        if (g1 == g2)
            return false;

        bool objectExists;
        Nui32 idx = mRemotes.GetIndexFromKey(g1, &objectExists);
        if (objectExists == false)
        {
            return false;
        }
        SystemAddressAndGuid sag;
        sag.mID = g2;
        return mRemotes[idx]->remoteConnections.HasData(sag);
    }
    //------------------------------------------------------------------------
    Nui16 LinkState::GetPingBetweenSystems(UniqueID g1, UniqueID g2) const
    {
        if (g1==g2)
            return 0;

        if (g1==mPrc2->getGuid())
            return (Nui16) mPrc2->getAvgPing(g2);
        if (g2==mPrc2->getGuid())
            return (Nui16) mPrc2->getAvgPing(g1);

        bool objectExists;
        Nui32 idx = mRemotes.GetIndexFromKey(g1, &objectExists);
        if (objectExists==false)
        {
            return (Nui16) -1;
        }

        SystemAddressAndGuid sag;
        sag.mID=g2;
        Nui32 idx2 = mRemotes[idx]->remoteConnections.GetIndexFromKey(sag, &objectExists);
        if (objectExists==false)
        {
            return (Nui16) -1;
        }
        return mRemotes[idx]->remoteConnections[idx2].sendersPingToThatSystem;
    }
    //------------------------------------------------------------------------
    /// Returns the system with the lowest total ping among all its connections. This can be used as the 'best host' for a peer to peer session
    UniqueID LinkState::GetLowestAveragePingSystem(void) const
    {
        NIIf lowestPing = -1.0;
        Nui32 lowestPingIdx = (Nui32) -1;
        NIIf thisAvePing = 0.0f;
        Nui32 idx, idx2;
        int ap, count = 0;

        for (idx = 0; idx < mRemotes.Size(); idx++)
        {
            thisAvePing = 0.0f;

            ap = mPrc2->getAvgPing(mRemotes[idx]->mID);
            if (ap != -1)
            {
                thisAvePing+=(NIIf) ap;
                count++;
            }
        }

        if (count>0)
        {
            lowestPing=thisAvePing/count;
        }

        for (idx=0; idx<mRemotes.Size(); idx++)
        {
            thisAvePing = 0.0f;
            count = 0;

            RemoteSystem *remote = mRemotes[idx];
            for (idx2 = 0; idx2 < remote->remoteConnections.Size(); idx2++)
            {
                ap = remote->remoteConnections[idx2].sendersPingToThatSystem;
                if (ap != -1)
                {
                    thisAvePing+=(NIIf) ap;
                    count++;
                }
            }

            if (count>0 && (lowestPing==-1.0f || thisAvePing/count < lowestPing))
            {
                lowestPing=thisAvePing/count;
                lowestPingIdx=idx;
            }
        }

        if (lowestPingIdx == (Nui32) -1)
            return mPrc2->getGuid();
        return mRemotes[lowestPingIdx]->mID;
    }
    //------------------------------------------------------------------------
    void LinkState::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        // send notice to all existing connections
        NetSerializer bs;
        if (lostConnectionReason==CCT_Exception)
            bs.write((Token)ID_REMOTE_CONNECTION_LOST);
        else
            bs.write((Token)ID_REMOTE_DISCONNECTION_NOTIFICATION);
        bs.write(address);
        bs.write(id);
        send(&bs, PL_HIGH, PT_ORDER_MAKE_SURE, 0, address, true);

        bool objectExists;
        Nui32 idx = mRemotes.GetIndexFromKey(id, &objectExists);
        if (objectExists)
        {
            N_delete mRemotes[idx];
            mRemotes.RemoveAtIndex(idx);
        }
    }
    //------------------------------------------------------------------------
    void LinkState::SetAutoProcessNewConnections(bool b)
    {
        autoProcessNewConnections=b;
    }
    //------------------------------------------------------------------------
    bool LinkState::GetAutoProcessNewConnections(void) const
    {
        return autoProcessNewConnections;
    }
    //------------------------------------------------------------------------
    void LinkState::AddParticipant(const Address & address, UniqueID id)
    {
        // Relay the new connection to other systems.
        NetSerializer bs;
        bs.write((Token)ID_REMOTE_NEW_INCOMING_CONNECTION);
        bs.write((Nui32)1);
        bs.write(address);
        bs.write(id);
        bs.write((Nui16)mPrc2->getAvgPing(id));
        send(&bs, PL_HIGH, PT_ORDER_MAKE_SURE, 0, address, true);

        // send everyone to the new guy
        DataStructures::List<Address *> addresses;
        DataStructures::List<UniqueID> guids;
        mPrc2->getActive(addresses, guids);
        bs.seekWrite(0);
        bs.seekRead(0);
        bs.write((Token)ID_REMOTE_NEW_INCOMING_CONNECTION);
        NBitCount writeBOffset = bs.getWriteOffset();
        bs.write((Nui32) addresses.Size());

        NCount i, count;
        for(i = 0, count = 0; i < addresses.Size(); ++i)
        {
            if(addresses[i] == address)
                continue;

            bs.write(addresses[i]);
            bs.write(guids[i]);
            bs.write((Nui16)mPrc2->getAvgPing(guids[i]));
            ++count;
        }

        if(count > 0)
        {
            NBitCount writeBOffset2 = bs.getWriteOffset();
            bs.seekWrite(writeBOffset);
            bs.write(count);
            bs.seekWrite(writeBOffset2);
            send(&bs, PL_HIGH, PT_ORDER_MAKE_SURE, 0, address, false);
        }

        bool objectExists;
        Nui32 ii = mRemotes.GetIndexFromKey(id, &objectExists);
        if(objectExists == false)
        {
            RemoteSystem * remote = N_new RemoteSystem();
            remote->mID = id;
            mRemotes.InsertAtIndex(remote,ii);
        }
    }
    //------------------------------------------------------------------------
    void LinkState::GetParticipantList(DataStructures::OrderedList<UniqueID, UniqueID> & participantList)
    {
        participantList.Clear(true);
        NCount i;
        for (i = 0; i < mRemotes.Size(); ++i)
            participantList.InsertAtEnd(mRemotes[i]->mID);
    }
    //------------------------------------------------------------------------
    void LinkState::onConnect(const Address & address, UniqueID id)
    {
        if (autoProcessNewConnections)
            AddParticipant(address, id);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType LinkState::onMessage(SocketMessage * msg)
    {
        if (msg->mData[0] == ID_REMOTE_CONNECTION_LOST || msg->mData[0] == ID_REMOTE_DISCONNECTION_NOTIFICATION)
        {
            bool objectExists;
            Nui32 idx = mRemotes.GetIndexFromKey(msg->mID, &objectExists);
            if (objectExists)
            {
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(1, 0);
                SystemAddressAndGuid saag;
                bs.read(saag.mAddress);
                bs.read(saag.mID);
                Nul idx2 = mRemotes[idx]->remoteConnections.GetIndexFromKey(saag, &objectExists);
                if (objectExists)
                    mRemotes[idx]->remoteConnections.RemoveAtIndex(idx2);
            }
        }
        else if (msg->mData[0]==ID_REMOTE_NEW_INCOMING_CONNECTION)
        {
            bool objectExists;
            Nui32 idx = mRemotes.GetIndexFromKey(msg->mID, &objectExists);
            if (objectExists)
            {
                Nui32 numAddresses;
                NetSerializer bs(msg->mData, msg->mLength, false);
                bs.skipRead(1, 0);
                bs.read(numAddresses);
                for (NCount idx2 = 0; idx2 < numAddresses; ++idx2)
                {
                    SystemAddressAndGuid saag;
                    bs.read(saag.mAddress);
                    bs.read(saag.mID);
                    bs.read(saag.sendersPingToThatSystem);
                    bool objectExists;
                    Nui32 ii = mRemotes[idx]->remoteConnections.GetIndexFromKey(saag, &objectExists);
                    if (objectExists==false)
                        mRemotes[idx]->remoteConnections.InsertAtIndex(saag,ii);
                }
            }
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
#endif