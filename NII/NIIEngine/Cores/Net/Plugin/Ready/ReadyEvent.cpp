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
#if _RAKNET_SUPPORT_ReadyEvent == 1

#include "ReadyEvent.h"
#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int ReadyEvent::RemoteSystemCompByGuid(const UniqueID & key, const RemoteSystem & data)
    {
        if (key < data.mID)
            return -1;
        else if (key==data.mID)
            return 0;
        else
            return 1;
    }
    //------------------------------------------------------------------------
    int ReadyEvent::ReadyEventNodeComp(const int & key, ReadyEvent::ReadyEventNode * const & data)
    {
        if (key < data->eventId)
            return -1;
        else if (key==data->eventId)
            return 0;
        else
            return 1;
    }
    //------------------------------------------------------------------------
    ReadyEvent::ReadyEvent()
    {
        channel=0;
    }
    //------------------------------------------------------------------------
    ReadyEvent::~ReadyEvent()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::SetEvent(int eventId, bool isReady)
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists==false)
        {
            // Totally new event
            CreateNewEvent(eventId, isReady);
        }
        else
        {
            return SetEventByIndex(eventIndex, isReady);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void ReadyEvent::ForceCompletion(int eventId)
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists==false)
        {
            // Totally new event
            CreateNewEvent(eventId, true);
            eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        }

        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        ren->eventStatus=ID_READY_EVENT_FORCE_ALL_SET;
        UpdateReadyStatus(eventIndex);
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::DeleteEvent(int eventId)
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            N_delete readyEventNodeList[eventIndex];
            readyEventNodeList.RemoveAtIndex(eventIndex);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::IsEventSet(int eventId)
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            return readyEventNodeList[eventIndex]->eventStatus==ID_READY_EVENT_SET || readyEventNodeList[eventIndex]->eventStatus==ID_READY_EVENT_ALL_SET;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::IsEventCompletionProcessing(int eventId) const
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            bool anyAllReady=false;
            bool allAllReady=true;
            ReadyEventNode *ren = readyEventNodeList[eventIndex];
            if (ren->eventStatus==ID_READY_EVENT_FORCE_ALL_SET)
                return false;
            for (Nui32 i=0; i < ren->systemList.Size(); i++)
            {
                if (ren->systemList[i].lastReceivedStatus==ID_READY_EVENT_ALL_SET)
                    anyAllReady=true;
                else
                    allAllReady=false;
            }
            return anyAllReady==true && allAllReady==false;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::IsEventCompleted(int eventId) const
    {
        bool objectExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            return IsEventCompletedByIndex(eventIndex);
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::HasEvent(int eventId)
    {
        return readyEventNodeList.HasData(eventId);
    }
    //------------------------------------------------------------------------
    Nui32 ReadyEvent::GetEventListSize(void) const
    {
        return readyEventNodeList.Size();
    }
    //------------------------------------------------------------------------
    int ReadyEvent::GetEventAtIndex(Nui32 index) const
    {
        return readyEventNodeList[index]->eventId;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::AddToWaitList(int eventId, UniqueID id)
    {
        bool eventExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &eventExists);
        if(eventExists == false)
            eventIndex = CreateNewEvent(eventId, false);

        // Don't do this, otherwise if we are trying to start a 3 player game, it will not allow the 3rd player to hit ready if the first two players have already done so
        //if (IsLocked(eventIndex))
        //	return false; // Not in the list, but event is already completed, or is starting to complete, and adding more waiters would fail this.

        Nui32 i;
        Nui32 numAdded = 0;
        if(id == UniqueID::INVALID)
        {
            for(i = 0; i < mPrc2->getRemoteCount(); ++i)
            {
                UniqueID firstGuid = mPrc2->getGUID(i);
                if(firstGuid != UniqueID::INVALID)
                {
                    numAdded += AddToWaitListInternal(eventIndex, firstGuid);
                }
            }
        }
        else
        {
            numAdded = AddToWaitListInternal(eventIndex, id);
        }

        if(numAdded > 0)
            UpdateReadyStatus(eventIndex);
        return numAdded > 0;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::RemoveFromWaitList(int eventId, UniqueID id)
    {
        bool eventExists;
        Nui32 eventIndex = readyEventNodeList.GetIndexFromKey(eventId, &eventExists);
        if (eventExists)
        {
            if (id==UniqueID::INVALID)
            {
                // Remove all waiters
                readyEventNodeList[eventIndex]->systemList.Clear(false);
                UpdateReadyStatus(eventIndex);
            }
            else
            {
                bool systemExists;
                Nui32 systemIndex = readyEventNodeList[eventIndex]->systemList.GetIndexFromKey(id, &systemExists);
                if (systemExists)
                {
                    bool isCompleted = IsEventCompletedByIndex(eventIndex);
                    readyEventNodeList[eventIndex]->systemList.RemoveAtIndex(systemIndex);

                    if (isCompleted==false && IsEventCompletedByIndex(eventIndex))
                        PushCompletionPacket(readyEventNodeList[eventIndex]->eventId);

                    UpdateReadyStatus(eventIndex);

                    return true;
                }
            }
        }

        return false;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::IsInWaitList(int eventId, UniqueID id)
    {
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            return readyEventNodeList[readyIndex]->systemList.HasData(id);
        }
        return false;
    }
    //------------------------------------------------------------------------
    Nui32 ReadyEvent::GetRemoteWaitListSize(int eventId) const
    {
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            return readyEventNodeList[readyIndex]->systemList.Size();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    UniqueID ReadyEvent::GetFromWaitListAtIndex(int eventId, Nui32 index) const
    {
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            return readyEventNodeList[readyIndex]->systemList[index].mID;
        }
        return UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
    ReadyEventSystemStatus ReadyEvent::GetReadyStatus(int eventId, UniqueID id)
    {
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            ReadyEventNode *ren = readyEventNodeList[readyIndex];
            Nui32 systemIndex = ren->systemList.GetIndexFromKey(id, &objectExists);
            if (objectExists==false)
                return RES_NOT_WAITING;
            if (ren->systemList[systemIndex].lastReceivedStatus==ID_READY_EVENT_SET)
                return RES_READY;
            if (ren->systemList[systemIndex].lastReceivedStatus==ID_READY_EVENT_UNSET)
                return RES_WAITING;
            if (ren->systemList[systemIndex].lastReceivedStatus==ID_READY_EVENT_ALL_SET)
                return RES_ALL_READY;
        }

        return RES_UNKNOWN_EVENT;
    }
    //------------------------------------------------------------------------
    void ReadyEvent::SetSendChannel(Nui8 newChannel)
    {
        channel=newChannel;
    }
    //------------------------------------------------------------------------
    ConnectReceiveType ReadyEvent::onMessage(SocketMessage * msg)
    {
        Nui8 packetIdentifier;
        packetIdentifier = (Nui8)msg->mData[0];

        switch (packetIdentifier)
        {
        case ID_READY_EVENT_UNSET:
        case ID_READY_EVENT_SET:
        case ID_READY_EVENT_ALL_SET:
            OnReadyEventPacketUpdate(msg);
            return CRT_Normal;
        case ID_READY_EVENT_FORCE_ALL_SET:
            OnReadyEventForceAllSet(msg);
            return CRT_Normal;
        case ID_READY_EVENT_QUERY:
            OnReadyEventQuery(msg);
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::AddToWaitListInternal(Nui32 eventIndex, UniqueID id)
    {
        ReadyEventNode * ren = readyEventNodeList[eventIndex];
        bool objectExists;
        Nui32 systemIndex = ren->systemList.GetIndexFromKey(id, &objectExists);
        if (objectExists==false)
        {
            RemoteSystem rs;
            rs.lastReceivedStatus = ID_READY_EVENT_UNSET;
            rs.lastSentStatus = ID_READY_EVENT_UNSET;
            rs.mID = id;
            ren->systemList.InsertAtIndex(rs, systemIndex);

            SendReadyStateQuery(ren->eventId, id);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void ReadyEvent::OnReadyEventForceAllSet(SocketMessage *msg)
    {
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        int eventId;
        in.read(eventId);
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            ReadyEventNode *ren = readyEventNodeList[readyIndex];
            if (ren->eventStatus!=ID_READY_EVENT_FORCE_ALL_SET)
            {
                ren->eventStatus=ID_READY_EVENT_FORCE_ALL_SET;
                PushCompletionPacket(ren->eventId);
            }
        }
    }
    //------------------------------------------------------------------------
    void ReadyEvent::OnReadyEventPacketUpdate(SocketMessage *msg)
    {
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        int eventId;
        in.read(eventId);
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            ReadyEventNode *ren = readyEventNodeList[readyIndex];
            bool systemExists;
            Nui32 systemIndex = ren->systemList.GetIndexFromKey(msg->mID, &systemExists);
            if (systemExists)
            {
                // Just return if no change
                if (ren->systemList[systemIndex].lastReceivedStatus==msg->mData[0])
                    return;

                bool wasCompleted = IsEventCompletedByIndex(readyIndex);
                ren->systemList[systemIndex].lastReceivedStatus=msg->mData[0];
                // If forced all set, doesn't matter what the new msg is
                if (ren->eventStatus == ID_READY_EVENT_FORCE_ALL_SET)
                    return;
                UpdateReadyStatus(readyIndex);
                if (wasCompleted == false && IsEventCompletedByIndex(readyIndex))
                    PushCompletionPacket(readyIndex);
            }
        }
    }
    //------------------------------------------------------------------------
    void ReadyEvent::OnReadyEventQuery(SocketMessage * msg)
    {
        NetSerializer in(msg->mData, msg->mLength, false);
        in.skipRead(1, 0);
        int eventId;
        in.read(eventId);
        bool objectExists;
        Nui32 readyIndex = readyEventNodeList.GetIndexFromKey(eventId, &objectExists);
        if (objectExists)
        {
            Nui32 systemIndex = readyEventNodeList[readyIndex]->systemList.GetIndexFromKey(msg->mID,&objectExists);
            // Force the non-default send, because our initial send may have arrived at a system that didn't yet create the ready event
            if (objectExists)
                SendReadyUpdate(readyIndex, systemIndex, true);
        }
    }
    //------------------------------------------------------------------------
    void ReadyEvent::onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason )
    {
        (void) address;
        (void) id;
        (void) lostConnectionReason;

        RemoveFromAllLists(id);
    }
    //------------------------------------------------------------------------
    void ReadyEvent::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::SetEventByIndex(int eventIndex, bool isReady)
    {
        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        if ((ren->eventStatus==ID_READY_EVENT_ALL_SET || ren->eventStatus==ID_READY_EVENT_SET) && isReady==true)
            return false; // Success - no change
        if (ren->eventStatus==ID_READY_EVENT_UNSET && isReady==false)
            return false; // Success - no change
        if (ren->eventStatus==ID_READY_EVENT_FORCE_ALL_SET)
            return false; // Can't change

        if (isReady)
            ren->eventStatus=ID_READY_EVENT_SET;
        else
            ren->eventStatus=ID_READY_EVENT_UNSET;

        UpdateReadyStatus(eventIndex);

        // Check if now completed, and if so, tell the user about it
        if (IsEventCompletedByIndex(eventIndex))
        {
            PushCompletionPacket(ren->eventId);
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool ReadyEvent::IsEventCompletedByIndex(Nui32 eventIndex) const
    {
        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        Nui32 i;
        if (ren->eventStatus==ID_READY_EVENT_FORCE_ALL_SET)
            return true;
        if (ren->eventStatus!=ID_READY_EVENT_ALL_SET)
            return false;
        for (i=0; i < ren->systemList.Size(); i++)
            if (ren->systemList[i].lastReceivedStatus!=ID_READY_EVENT_ALL_SET)
                return false;
        return true;
    }
    //------------------------------------------------------------------------
    void ReadyEvent::Clear(void)
    {
        Nui32 i;
        for (i=0; i < readyEventNodeList.Size(); ++i)
        {
            N_delete readyEventNodeList[i];
        }
        readyEventNodeList.Clear(false);
    }
    //------------------------------------------------------------------------
    Nui32 ReadyEvent::CreateNewEvent(int eventId, bool isReady)
    {
        ReadyEventNode *ren = N_new ReadyEventNode();
        ren->eventId=eventId;
        if (isReady==false)
            ren->eventStatus=ID_READY_EVENT_UNSET;
        else
            ren->eventStatus=ID_READY_EVENT_SET;
        return readyEventNodeList.Insert(eventId, ren, true);
    }
    //------------------------------------------------------------------------
    void ReadyEvent::UpdateReadyStatus(Nui32 eventIndex)
    {
        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        bool anyUnset;
        Nui32 i;
        if (ren->eventStatus==ID_READY_EVENT_SET)
        {
            // If you are set, and no other systems are ID_READY_EVENT_UNSET, then change your status to ID_READY_EVENT_ALL_SET
            anyUnset=false;
            for (i=0; i < ren->systemList.Size(); i++)
            {
                if (ren->systemList[i].lastReceivedStatus==ID_READY_EVENT_UNSET)
                {
                    anyUnset=true;
                    break;
                }
            }
            if (anyUnset==false)
            {
                ren->eventStatus=ID_READY_EVENT_ALL_SET;
            }
        }
        else if (ren->eventStatus==ID_READY_EVENT_ALL_SET)
        {
            // If you are all set, and any systems are ID_READY_EVENT_UNSET, then change your status to ID_READY_EVENT_SET
            anyUnset=false;
            for (i=0; i < ren->systemList.Size(); i++)
            {
                if (ren->systemList[i].lastReceivedStatus==ID_READY_EVENT_UNSET)
                {
                    anyUnset=true;
                    break;
                }
            }
            if (anyUnset==true)
            {
                ren->eventStatus=ID_READY_EVENT_SET;
            }
        }
        BroadcastReadyUpdate(eventIndex, false);
    }
    //------------------------------------------------------------------------
    void ReadyEvent::SendReadyUpdate(Nui32 eventIndex, Nui32 systemIndex, bool forceIfNotDefault)
    {
        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        NetSerializer bs;
        // I do this rather than write true or false, so users that do not use BitStreams can still read the data
        if ((ren->eventStatus!=ren->systemList[systemIndex].lastSentStatus) ||
            (forceIfNotDefault && ren->eventStatus!=ID_READY_EVENT_UNSET))
        {
            bs.write(ren->eventStatus);
            bs.write(ren->eventId);
            send(&bs, PL_HIGH, PT_ORDER_MAKE_SURE, channel, ren->systemList[systemIndex].mID, false);

            ren->systemList[systemIndex].lastSentStatus=ren->eventStatus;
        }

    }
    //------------------------------------------------------------------------
    void ReadyEvent::BroadcastReadyUpdate(Nui32 eventIndex, bool forceIfNotDefault)
    {
        ReadyEventNode *ren = readyEventNodeList[eventIndex];
        Nui32 systemIndex;
        for (systemIndex=0; systemIndex < ren->systemList.Size(); systemIndex++)
        {
            SendReadyUpdate(eventIndex, systemIndex, forceIfNotDefault);
        }
    }
    //------------------------------------------------------------------------
    void ReadyEvent::SendReadyStateQuery(Nui32 eventId, UniqueID id)
    {
        NetSerializer bs;
        bs.write((Token)ID_READY_EVENT_QUERY);
        bs.write(eventId);
        send(&bs, PL_HIGH, PT_ORDER_MAKE_SURE, channel, id, false);
    }
    //------------------------------------------------------------------------
    void ReadyEvent::RemoveFromAllLists(UniqueID id)
    {
        NCount eventIndex;
        for (eventIndex = 0; eventIndex < readyEventNodeList.Size(); ++eventIndex)
        {
            bool isCompleted = IsEventCompletedByIndex(eventIndex);
            bool systemExists;
            Nui32 systemIndex;

            systemIndex = readyEventNodeList[eventIndex]->systemList.GetIndexFromKey(id, &systemExists);
            if (systemExists)
                readyEventNodeList[eventIndex]->systemList.RemoveAtIndex(systemIndex);

            UpdateReadyStatus(eventIndex);

            if (isCompleted == false && IsEventCompletedByIndex(eventIndex))
                PushCompletionPacket(readyEventNodeList[eventIndex]->eventId);
        }
    }
    //------------------------------------------------------------------------
    void ReadyEvent::PushCompletionPacket(Nui32 eventId)
    {
        (void) eventId;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*