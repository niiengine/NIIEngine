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
#if _RAKNET_SUPPORT_MessageFilter==1

#include "NiiPreProcess.h"
#include "NiiNetMsgFilter.h"
#include "NiiNetCommon.h"
#include "NiiNetLinkBase.h"
#include "NiiNetworkObj.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int MessageFilterStrComp(char * const & key,char * const & data)
    {
        return strcmp(key,data);
    }
    //------------------------------------------------------------------------
    int FilterSetComp(const int & key, FilterSet * const & data)
    {
        if (key < data->filterSetID)
            return -1;
        else if (key==data->filterSetID)
            return 0;
        else
            return 1;
    }
    //------------------------------------------------------------------------
    MsgFilter::MsgFilter()
    {
            whenLastTimeoutCheck=N_Engine().getTimer().getMS();
    }
    //------------------------------------------------------------------------
    MsgFilter::~MsgFilter()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetAutoAddNewConnectionsToFilter(int filterSetID)
    {
        autoAddNewConnectionsToFilter=filterSetID;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd,int filterSetID)
    {
        N_assert(messageIDStart <= messageIDEnd);
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        int i;
        for (i=messageIDStart; i <= messageIDEnd; ++i)
            filterSet->allowedIDs[i]=allow;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetAllowRPC4(bool allow, const char* uniqueID, int filterSetID)
    {
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        bool objectExists;
        Nui32 idx = filterSet->allowedRPC4.GetIndexFromKey(uniqueID, &objectExists);
        if (allow)
        {
            if (objectExists==false)
            {
                filterSet->allowedRPC4.InsertAtIndex(uniqueID, idx);
                filterSet->allowedIDs[ID_RPC_PLUGIN]=true;
            }
        }
        else
        {
            if (objectExists==true)
            {
                filterSet->allowedRPC4.RemoveAtIndex(idx);
                if (filterSet->allowedRPC4.Size()==0)
                {
                    filterSet->allowedIDs[ID_RPC_PLUGIN]=false;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed, TimeDurMS banTimeMS, int filterSetID)
    {
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        filterSet->kickOnDisallowedMessage=kickOnDisallowed;
        filterSet->disallowedMessageBanTimeMS=banTimeMS;
        filterSet->banOnDisallowedMessage=banOnDisallowed;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetDisallowedMessageCallback(int filterSetID, void * userData, void (*invalidMessageCallback)(LinkBase * peer, UniqueAddress address, int filterSetID, void *userData, Nui8 messageID))
    {
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        filterSet->invalidMessageCallback=invalidMessageCallback;
        filterSet->disallowedCallbackUserData=userData;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetTimeoutCallback(int filterSetID, void * userData, void (*invalidMessageCallback)(LinkBase * peer, UniqueAddress address, int filterSetID, void *userData))
    {
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        filterSet->timeoutCallback=invalidMessageCallback;
        filterSet->timeoutUserData=userData;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, TimeDurMS banTimeMS, int filterSetID)
    {
        FilterSet *filterSet = GetFilterSetByID(filterSetID);
        filterSet->maxMemberTimeMS=allowedTimeMS;
        filterSet->banOnFilterTimeExceed=banOnExceed;
        filterSet->timeExceedBanTimeMS=banTimeMS;
    }
    //------------------------------------------------------------------------
    int MsgFilter::GetSystemFilterSet(UniqueAddress address)
    {
        DataStructures::HashIndex index = systemList.GetIndexOf(address);
        if (index.IsInvalid())
            return -1;
        else
            return systemList.ItemAtIndex(index).filter->filterSetID;
    }
    //------------------------------------------------------------------------
    void MsgFilter::SetSystemFilterSet(UniqueAddress addressOrGUID, int filterSetID)
    {
        // Allocate this filter set if it doesn't exist.
        N_assert(addressOrGUID.isValid() == true);
        DataStructures::HashIndex index = systemList.GetIndexOf(addressOrGUID);
        if (index.IsInvalid())
        {
            if (filterSetID<0)
                return;

            FilteredSystem filteredSystem;
            filteredSystem.filter = GetFilterSetByID(filterSetID);
            filteredSystem.timeEnteredThisSet=N_Engine().getTimer().getMS();
            systemList.Push(addressOrGUID,filteredSystem);
        }
        else
        {
            if (filterSetID>=0)
            {
                FilterSet *filterSet = GetFilterSetByID(filterSetID);
                systemList.ItemAtIndex(index).timeEnteredThisSet=N_Engine().getTimer().getMS();
                systemList.ItemAtIndex(index).filter=filterSet;
            }
            else
            {
                systemList.RemoveAtIndex(index);
            }
        }
    }
    //------------------------------------------------------------------------
    Nui32 MsgFilter::GetSystemCount(int filterSetID) const
    {
        if (filterSetID==-1)
        {
            return systemList.Size();
        }
        else
        {
            Nui32 i;
            Nui32 count=0;
            DataStructures::List<FilteredSystem> itemList;
            DataStructures::List<UniqueAddress> keyList;
            systemList.GetAsList(itemList, keyList);
            for (i=0; i < itemList.Size(); i++)
                if (itemList[i].filter->filterSetID==filterSetID)
                    ++count;
            return count;
        }
    }
    //------------------------------------------------------------------------
    Nui32 MsgFilter::GetFilterSetCount(void) const
    {
        return filterList.Size();
    }
    //------------------------------------------------------------------------
    int MsgFilter::GetFilterSetIDByIndex(Nui32 index)
    {
        return filterList[index]->filterSetID;
    }
    //------------------------------------------------------------------------
    void MsgFilter::DeleteFilterSet(int filterSetID)
    {
        FilterSet *filterSet;
        bool objectExists;
        Nui32 i,index;
        index = filterList.GetIndexFromKey(filterSetID, &objectExists);
        if (objectExists)
        {
            filterSet=filterList[index];
            DeallocateFilterSet(filterSet);
            filterList.RemoveAtIndex(index);

            DataStructures::List< FilteredSystem > itemList;
            DataStructures::List< UniqueAddress > keyList;
            systemList.GetAsList(itemList, keyList);
            for (i=0; i < itemList.Size(); i++)
            {
                if (itemList[i].filter==filterSet)
                {
                    systemList.Remove(keyList[i]);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MsgFilter::Clear(void)
    {
        Nui32 i;
        systemList.Clear();
        for (i=0; i < filterList.Size(); ++i)
            DeallocateFilterSet(filterList[i]);
        filterList.Clear(false);
    }
    //------------------------------------------------------------------------
    void MsgFilter::DeallocateFilterSet(FilterSet * filterSet)
    {
        N_delete filterSet;
    }
    //------------------------------------------------------------------------
    FilterSet * MsgFilter::GetFilterSetByID(int filterSetID)
    {
        N_assert(filterSetID>=0);
        bool objectExists;
        Nui32 index;
        index = filterList.GetIndexFromKey(filterSetID, &objectExists);
        if (objectExists)
            return filterList[index];
        else
        {
            FilterSet *newFilterSet = N_new FilterSet();
            memset(newFilterSet->allowedIDs, 0, MESSAGE_FILTER_MAX_MESSAGE_ID * sizeof(bool));
            newFilterSet->banOnFilterTimeExceed=false;
            newFilterSet->kickOnDisallowedMessage=false;
            newFilterSet->banOnDisallowedMessage=false;
            newFilterSet->disallowedMessageBanTimeMS=0;
            newFilterSet->timeExceedBanTimeMS=0;
            newFilterSet->maxMemberTimeMS=0;
            newFilterSet->filterSetID=filterSetID;
            newFilterSet->invalidMessageCallback=0;
            newFilterSet->timeoutCallback=0;
            newFilterSet->timeoutUserData=0;
            filterList.Insert(filterSetID, newFilterSet, true);
            return newFilterSet;
        }
    }
    //------------------------------------------------------------------------
    void MsgFilter::OnInvalidMessage(FilterSet * filterSet, UniqueAddress address, Nui8 messageID)
    {
        if(filterSet->invalidMessageCallback)
            filterSet->invalidMessageCallback(mPrc2, address, filterSet->filterSetID, filterSet->disallowedCallbackUserData, messageID);
        if(filterSet->banOnDisallowedMessage && mPrc2)
        {
            String str1;
            address.mAddress.read(str1, false);
            mPrc2->getBan()->add(str1, filterSet->disallowedMessageBanTimeMS);
        }
        if(filterSet->kickOnDisallowedMessage)
        {
            if(mPrc2)
                mPrc2->close(address, true, 0);
    #if _RAKNET_SUPPORT_NiiNetworkObj==1 && _RAKNET_SUPPORT_TCPInterface==1
            else
                mPrc->close(address.mAddress);
    #endif
        }
    }
    //------------------------------------------------------------------------
    void MsgFilter::update()
    {
        // update all timers for all systems.  If those systems' filter sets are expired, take the appropriate action.
        TimeDurMS curTime = N_Engine().getTimer().getMS();
        if(Timer::greater(curTime - 1000, whenLastTimeoutCheck))
        {
            DataStructures::List< FilteredSystem > itemList;
            DataStructures::List< UniqueAddress > keyList;
            systemList.GetAsList(itemList, keyList);

            Nui32 index;
            for(index=0; index < itemList.Size(); ++index)
            {
                if(itemList[index].filter && itemList[index].filter->maxMemberTimeMS>0 &&
                    curTime-itemList[index].timeEnteredThisSet >= itemList[index].filter->maxMemberTimeMS)
                {
                    if (itemList[index].filter->timeoutCallback)
                        itemList[index].filter->timeoutCallback(mPrc2, keyList[index], itemList[index].filter->filterSetID, itemList[index].filter->timeoutUserData);

                    if(itemList[index].filter->banOnFilterTimeExceed && mPrc2)
                    {
                        String str1;
                        keyList[index].read(str1, false);
                        mPrc2->getBan()->add(str1, itemList[index].filter->timeExceedBanTimeMS);
                    }
                    if(mPrc2)
                        mPrc2->close(keyList[index], true, 0);
    #if _RAKNET_SUPPORT_NiiNetworkObj == 1 && _RAKNET_SUPPORT_TCPInterface == 1
                    else
                        mPrc->close(keyList[index].mAddress);
    #endif
                    systemList.Remove(keyList[index]);
                }
            }
            whenLastTimeoutCheck = curTime + 1000;
        }
    }
    //------------------------------------------------------------------------
    void MsgFilter::onConnect(const Address & address, UniqueID id)
    {
        UniqueAddress aog;
        aog.mID=id;
        aog.mAddress=address;

        // New system, automatically assign to filter set if appropriate
        if (autoAddNewConnectionsToFilter>=0 && systemList.HasData(aog)==false)
            SetSystemFilterSet(aog, autoAddNewConnectionsToFilter);
    }
    //------------------------------------------------------------------------
    void MsgFilter::onConnectClose(const Address & address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) id;
        (void) lostConnectionReason;

        UniqueAddress aog;
        aog.mID=id;
        aog.mAddress=address;

        // Lost system, remove from the list
        systemList.Remove(aog);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType MsgFilter::onMessage(SocketMessage * msg)
    {
        DataStructures::HashIndex index;
        Nui8 messageId;

        switch (msg->mData[0])
        {
        case ID_NEW_INCOMING_CONNECTION:
        case ID_CONNECTION_REQUEST_ACCEPTED:
        case ID_CONNECTION_LOST:
        case ID_DISCONNECTION_NOTIFICATION:
        case ID_CONNECTION_ATTEMPT_FAILED:
        case ID_NO_FREE_INCOMING_CONNECTIONS:
        case ID_IP_RECENTLY_CONNECTED:
        case ID_CONNECTION_BANNED:
        case ID_INVALID_PASSWORD:
        case ID_UNCONNECTED_PONG:
        case ID_ALREADY_CONNECTED:
        case ID_ADVERTISE_SYSTEM:
        case ID_REMOTE_DISCONNECTION_NOTIFICATION:
        case ID_REMOTE_CONNECTION_LOST:
        case ID_REMOTE_NEW_INCOMING_CONNECTION:
        case ID_DOWNLOAD_PROGRESS:
            break;
        default:
            if(msg->mData[0] == ID_TIMESTAMP)
            {
                if (msg->mLength<sizeof(Token) + sizeof(TimeDurMS))
                    return CRT_Destroy; // Invalid message
                messageId = msg->mData[sizeof(Token) + sizeof(TimeDurMS)];
            }
            else
                messageId = msg->mData[0];
            // If this system is filtered, check if this message is allowed.  If not allowed, return CRT_Destroy
            // index = systemList.GetIndexFromKey(msg->addressOrGUID, &objectExists);
            index = systemList.GetIndexOf(msg);
            if (index.IsInvalid())
                break;
            if (systemList.ItemAtIndex(index).filter->allowedIDs[messageId]==false)
            {
                OnInvalidMessage(systemList.ItemAtIndex(index).filter, msg, msg->mData[0]);
                return CRT_Destroy;
            }
            if (msg->mData[0] == ID_RPC_PLUGIN)
            {
                NetSerializer bsIn(msg->mData, msg->mLength, false);
                bsIn.skipRead(2, 0);
                RakString functionName;
                bsIn.readCompress(functionName);
                if (systemList.ItemAtIndex(index).filter->allowedRPC4.HasData(functionName)==false)
                {
                    OnInvalidMessage(systemList.ItemAtIndex(index).filter, msg, msg->mData[0]);
                    return CRT_Destroy;
                }
            }
            break;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
#endif