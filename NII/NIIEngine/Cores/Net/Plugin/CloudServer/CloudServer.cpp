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
#if _RAKNET_SUPPORT_CloudServer == 1

#include "CloudServer.h"
#include "NiiNetCommon.h"
#include "NiiNetSerializer.h"
#include "NiiNetLinkBase.h"

enum ServerToServerCommands
{
	STSC_PROCESS_GET_REQUEST,
	STSC_PROCESS_GET_RESPONSE,
	STSC_ADD_UPLOADED_AND_SUBSCRIBED_KEYS,
	STSC_ADD_UPLOADED_KEY,
	STSC_ADD_SUBSCRIBED_KEY,
	STSC_REMOVE_UPLOADED_KEY,
	STSC_REMOVE_SUBSCRIBED_KEY,
	STSC_DATA_CHANGED,
};

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    int CloudServer::RemoteServerComp(const UniqueID & key, RemoteServer * const & data)
    {
        if (key < data->serverAddress)
            return -1;
        if (key > data->serverAddress)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int CloudServer::KeySubscriberIDComp(const CloudKey & key, KeySubscriberID * const & data)
    {
        if (key.primaryKey < data->key.primaryKey)
            return -1;
        if (key.primaryKey > data->key.primaryKey)
            return 1;
        if (key.secondaryKey < data->key.secondaryKey)
            return -1;
        if (key.secondaryKey > data->key.secondaryKey)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int CloudServer::KeyDataPtrComp(const UniqueID & key, CloudData * const & data)
    {
        if (key < data->clientGUID)
            return -1;
        if (key > data->clientGUID)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int CloudServer::KeyDataListComp(const CloudKey & key, CloudDataList * const & data)
    {
        if(key.primaryKey < data->key.primaryKey)
            return -1;
        if(key.primaryKey > data->key.primaryKey)
            return 1;
        if(key.secondaryKey < data->key.secondaryKey)
            return -1;
        if(key.secondaryKey > data->key.secondaryKey)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int CloudServer::BufferedGetResponseFromServerComp(const UniqueID & key,
        CloudServer::BufferedGetResponseFromServer * const & data)
    {
        if(key < data->serverAddress)
            return -1;
        if(key > data->serverAddress)
            return 1;
        return 0;
    }
    //------------------------------------------------------------------------
    int CloudServer::GetRequestComp(const Nui32 & key, CloudServer::GetRequest * const & data)
    {
        if(key < data->requestId)
            return -1;
        if(key > data->requestId)
            return -1;
        return 0;
    }
    //------------------------------------------------------------------------
    void CloudServer::CloudQueryWithAddresses::write(NetSerializer * in)
    {
        cloudQuery.write(in);

        Nui16 syscount;
        UniqueID addressOrGuid;
        in->read(syscount);
        for(NCount i = 0; i < syscount; ++i)
        {
            in->read(addressOrGuid);
            specificSystems.Push(addressOrGuid);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::CloudQueryWithAddresses::read(NetSerializer * out)
    {
        cloudQuery.read(out);
        out->write((Nui16)specificSystems.Size());
        for(NCount i = 0; i < specificSystems.Size(); ++i)
        {
            out->write(specificSystems[i]);
        }
    }
    //------------------------------------------------------------------------
    bool CloudServer::GetRequest::AllRemoteServersHaveResponded(void) const
    {
        Nui32 i;
        for (i=0; i < remoteServerResponses.Size(); i++)
            if (remoteServerResponses[i]->gotResult==false)
                return false;
        return true;
    }
    //------------------------------------------------------------------------
    void CloudServer::GetRequest::Clear(CloudAllocator *allocator)
    {
        Nui32 i;
        for (i=0; i < remoteServerResponses.Size(); i++)
        {
            remoteServerResponses[i]->Clear(allocator);
            N_delete remoteServerResponses[i];
        }
        remoteServerResponses.Clear(false);
    }
    //------------------------------------------------------------------------
    void CloudServer::BufferedGetResponseFromServer::Clear(CloudAllocator *allocator)
    {
        Nui32 i;
        for (i=0; i < queryResult.rowsReturned.Size(); i++)
        {
            allocator->DeallocateRowData(queryResult.rowsReturned[i]->mData);
            allocator->DeallocateCloudQueryRow(queryResult.rowsReturned[i]);
        }
        queryResult.rowsReturned.Clear(false);
    }
    //------------------------------------------------------------------------
    CloudServer::CloudServer()
    {
        maxUploadBytesPerClient=0;
        maxBytesPerDowload=0;
        nextGetRequestId=0;
        nextGetRequestsCheck=0;
    }
    //------------------------------------------------------------------------
    CloudServer::~CloudServer()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void CloudServer::SetMaxUploadBytesPerClient(Nui64 bytes)
    {
        maxUploadBytesPerClient=bytes;
    }
    //------------------------------------------------------------------------
    void CloudServer::SetMaxBytesPerDownload(Nui64 bytes)
    {
        maxBytesPerDowload=bytes;
    }
    //------------------------------------------------------------------------
    void CloudServer::update(void)
    {
        // Timeout getRequests
        TimeDurMS time = TimeDurMS();
        if (time > nextGetRequestsCheck)
        {
            nextGetRequestsCheck=time+1000;

            Nui32 i=0;
            while (i < getRequests.Size())
            {
                if (time - getRequests[i]->requestStartTime > 3000)
                {
                    // Remote server is not responding, just send back data with whoever did respond
                    ProcessAndTransmitGetRequest(getRequests[i]);
                    getRequests[i]->Clear(this);
                    N_delete getRequests[i];
                    getRequests.RemoveAtIndex(i);
                }
                else
                {
                    i++;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    ConnectReceiveType CloudServer::onMessage(SocketMessage * msg)
    {
        switch (msg->mData[0])
        {
        case ID_CLOUD_POST_REQUEST:
            OnPostRequest(msg);
            return CRT_Destroy;
        case ID_CLOUD_RELEASE_REQUEST:
            OnReleaseRequest(msg);
            return CRT_Destroy;
        case ID_CLOUD_GET_REQUEST:
            OnGetRequest(msg);
            return CRT_Destroy;
        case ID_CLOUD_UNSUBSCRIBE_REQUEST:
            OnUnsubscribeRequest(msg);
            return CRT_Destroy;
        case ID_CLOUD_SERVER_TO_SERVER_COMMAND:
            if (msg->mLength>1)
            {
                switch (msg->mData[1])
                {
                case STSC_PROCESS_GET_REQUEST:
                    OnServerToServerGetRequest(msg);
                    return CRT_Destroy;
                case STSC_PROCESS_GET_RESPONSE:
                    OnServerToServerGetResponse(msg);
                    return CRT_Destroy;
                case STSC_ADD_UPLOADED_AND_SUBSCRIBED_KEYS:
                    OnSendUploadedAndSubscribedKeysToServer(msg);
                    return CRT_Destroy;
                case STSC_ADD_UPLOADED_KEY:
                    OnSendUploadedKeyToServers(msg);
                    return CRT_Destroy;
                case STSC_ADD_SUBSCRIBED_KEY:
                    OnSendSubscribedKeyToServers(msg);
                    return CRT_Destroy;
                case STSC_REMOVE_UPLOADED_KEY:
                    OnRemoveUploadedKeyFromServers(msg);
                    return CRT_Destroy;
                case STSC_REMOVE_SUBSCRIBED_KEY:
                    OnRemoveSubscribedKeyFromServers(msg);
                    return CRT_Destroy;
                case STSC_DATA_CHANGED:
                    OnServerDataChanged(msg);
                    return CRT_Destroy;
                }
            }
            return CRT_Destroy;
        }
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void CloudServer::OnPostRequest(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        CloudKey key;
        key.write(&bsIn);
        Nui32 dataLengthBytes;
        bsIn.read(dataLengthBytes);
        if(maxUploadBytesPerClient>0 && dataLengthBytes>maxUploadBytesPerClient)
            return; // Exceeded max upload bytes

        bsIn.readAlign();
        for(NCount filterIndex = 0; filterIndex < queryFilters.Size(); ++filterIndex)
        {
            if(queryFilters[filterIndex]->OnPostRequest(msg->mID, msg->mAddress, key,
                dataLengthBytes, (const char *)bsIn.getData() + N_B_TO_b(bsIn.getReadOffset()))==false)
                return;
        }

        Nui8 * data;
        if(dataLengthBytes > CLOUD_SERVER_DATA_STACK_SIZE)
        {
            data = (Nui8 *)N_alloc(dataLengthBytes);
            if(data == 0)
            {
                return;
            }
            bsIn.read(data, dataLengthBytes);
        }
        else
            data = 0;

        // Add this system to remotes if they aren't there already
        DataStructures::HashIndex remoteSystemsHashIndex = mRemotes.GetIndexOf(msg->mID);
        RemoteCloudClient *remoteCloudClient;
        if(remoteSystemsHashIndex.IsInvalid())
        {
            remoteCloudClient = N_new RemoteCloudClient();
            remoteCloudClient->uploadedKeys.Insert(key,key,true);
            remoteCloudClient->uploadedBytes=0;
            mRemotes.Push(msg->mID, remoteCloudClient);
        }
        else
        {
            remoteCloudClient = mRemotes.ItemAtIndex(remoteSystemsHashIndex);
            bool objectExists;
            // Add to RemoteCloudClient::uploadedKeys if it isn't there already
            Nui32 uploadedKeysIndex = remoteCloudClient->uploadedKeys.GetIndexFromKey(key,&objectExists);
            if (objectExists==false)
            {
                remoteCloudClient->uploadedKeys.InsertAtIndex(key, uploadedKeysIndex);
            }
        }

        bool cloudDataAlreadyUploaded;
        Nui32 dataRepositoryIndex;
        bool dataRepositoryExists;
        CloudDataList* cloudDataList = GetOrAllocateCloudDataList(key, &dataRepositoryExists, dataRepositoryIndex);
        if (dataRepositoryExists==false)
        {
            cloudDataList->uploaderCount=1;
            cloudDataAlreadyUploaded=false;
        }
        else
        {
            cloudDataAlreadyUploaded=cloudDataList->uploaderCount>0;
            cloudDataList->uploaderCount++;
        }

        CloudData *cloudData;
        bool keyDataListExists;
        Nui32 keyDataListIndex = cloudDataList->keyData.GetIndexFromKey(msg->mID, &keyDataListExists);
        if (keyDataListExists==false)
        {
            if (maxUploadBytesPerClient>0 && remoteCloudClient->uploadedBytes+dataLengthBytes>maxUploadBytesPerClient)
            {
                // Undo prior insertion of cloudDataList into cloudData if needed
                if (keyDataListExists==false)
                {
                    N_delete cloudDataList;
                    dataRepository.RemoveAtIndex(dataRepositoryIndex);
                }

                if (remoteCloudClient->IsUnused())
                {
                    N_delete remoteCloudClient;
                    mRemotes.Remove(msg->mID);
                }

                if (dataLengthBytes>CLOUD_SERVER_DATA_STACK_SIZE)
                    N_free(data);

                return;
            }

            cloudData = N_new CloudData();
            cloudData->dataLengthBytes=dataLengthBytes;
            cloudData->isUploaded=true;
            if (forceAddress && !forceAddress->isInvalid())
            {
                cloudData->serverSystemAddress = forceAddress;
                cloudData->serverSystemAddress.setHSBPort(mPrc2->getConnectTo(msg->mAddress).getHSBPort());
            }
            else
            {
                cloudData->serverSystemAddress = mPrc2->getConnectTo(msg->mAddress);
                if (cloudData->serverSystemAddress.isLocalHost())
                    cloudData->serverSystemAddress.write(mPrc2->getLocalHost(0));
            }
            if (cloudData->serverSystemAddress.getHSBPort() == 0)
            {
                // Fix localhost port
                cloudData->serverSystemAddress.setHSBPort(mPrc2->getSocket(0)->GetBoundAddress().getHSBPort());
            }
            cloudData->clientSystemAddress = msg->mAddress;
            cloudData->serverGUID = mPrc2->getGuid();
            cloudData->clientGUID = msg->mID;
            cloudDataList->keyData.Insert(msg->mID,cloudData,true);
        }
        else
        {
            cloudData = cloudDataList->keyData[keyDataListIndex];

            if (cloudDataAlreadyUploaded==false)
            {
                if (forceAddress && !forceAddress->isInvalid())
                {
                    cloudData->serverSystemAddress=forceAddress;
                    cloudData->serverSystemAddress.setHSBPort(mPrc2->getConnectTo(msg->mAddress).getHSBPort());
                }
                else
                {
                    cloudData->serverSystemAddress=mPrc2->getConnectTo(msg->mAddress);
                }
                if (cloudData->serverSystemAddress.getHSBPort()==0)
                {
                    // Fix localhost port
                    cloudData->serverSystemAddress.setHSBPort(mPrc2->getSocket(0)->GetBoundAddress().getHSBPort());
                }

                cloudData->clientSystemAddress=msg->mAddress;
            }

            if (maxUploadBytesPerClient>0 && remoteCloudClient->uploadedBytes-cloudData->dataLengthBytes+dataLengthBytes>maxUploadBytesPerClient)
            {
                // Undo prior insertion of cloudDataList into cloudData if needed
                if (dataRepositoryExists==false)
                {
                    N_delete cloudDataList;
                    dataRepository.RemoveAtIndex(dataRepositoryIndex);
                }
                return;
            }
            else
            {
                // Subtract already used bytes we are overwriting
                remoteCloudClient->uploadedBytes-=cloudData->dataLengthBytes;
            }

            if (cloudData->allocatedData!=0)
                N_free(cloudData->allocatedData);
        }

        if (dataLengthBytes>CLOUD_SERVER_DATA_STACK_SIZE)
        {
            // Data already allocated
            cloudData->allocatedData = data;
            cloudData->dataPtr = data;
        }
        else
        {
            // read to stack
            if (dataLengthBytes>0)
                bsIn.read(cloudData->mStackData,dataLengthBytes);
            cloudData->allocatedData=0;
            cloudData->dataPtr=cloudData->mStackData;
        }
        // update how many bytes were written for this data
        cloudData->dataLengthBytes=dataLengthBytes;
        remoteCloudClient->uploadedBytes+=dataLengthBytes;

        if (cloudDataAlreadyUploaded==false)
        {
            // New data field
            SendUploadedKeyToServers(cloudDataList->key);
        }

        // Existing data field changed
        NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudData->specificSubscribers, true );
        NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudDataList->nonSpecificSubscribers, true );

        // send update to all remote servers that subscribed to this key
        NotifyServerSubscribersOfDataChange(cloudData, cloudDataList->key, true);

        // I could have also subscribed to a key not yet updated locally
        // This means I have to go through every SocketIO that wants this key
        // Seems like cloudData->specificSubscribers is unnecessary in that case
    }
    //------------------------------------------------------------------------
    void CloudServer::OnReleaseRequest(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        Nui16 keyCount;
        bsIn.read(keyCount);

        if(keyCount == 0)
            return;

        DataStructures::HashIndex hindex = mRemotes.GetIndexOf(msg->mID);
        if(hindex.IsInvalid() == true)
            return;

        RemoteCloudClient * remoteCloudClient = mRemotes.ItemAtIndex(hindex);

        CloudKey key;

        // read all in a list first so I can run filter on it
        DataStructures::List<CloudKey> cloudKeys;
        for (Nui16 keyCountIndex=0; keyCountIndex < keyCount; keyCountIndex++)
        {
            key.write(&bsIn);
            cloudKeys.Push(key);
        }

        for (Nui32 filterIndex=0; filterIndex < queryFilters.Size(); filterIndex++)
        {
            if (queryFilters[filterIndex]->OnReleaseRequest(msg->mID, msg->mAddress, cloudKeys)==false)
                return;
        }

        for (Nui16 keyCountIndex=0; keyCountIndex < keyCount; keyCountIndex++)
        {
            // read in list above so I can run the filter on it
            // key.write(&bsIn);
            key=cloudKeys[keyCountIndex];

            // Remove remote systems uploaded keys
            bool objectExists;
            Nui32 uploadedKeysIndex = remoteCloudClient->uploadedKeys.GetIndexFromKey(key,&objectExists);
            if (objectExists)
            {
                bool dataRepositoryExists;
                Nui32 dataRepositoryIndex = dataRepository.GetIndexFromKey(key, &dataRepositoryExists);
                CloudDataList* cloudDataList = dataRepository[dataRepositoryIndex];
                N_assert(cloudDataList);

                CloudData *cloudData;
                bool keyDataListExists;
                Nui32 keyDataListIndex = cloudDataList->keyData.GetIndexFromKey(msg->mID, &keyDataListExists);
                cloudData = cloudDataList->keyData[keyDataListIndex];

                remoteCloudClient->uploadedKeys.RemoveAtIndex(uploadedKeysIndex);
                remoteCloudClient->uploadedBytes-=cloudData->dataLengthBytes;
                cloudDataList->uploaderCount--;

                // Broadcast destruction of this key to subscribers
                NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudData->specificSubscribers, false );
                NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudDataList->nonSpecificSubscribers, false );
                NotifyServerSubscribersOfDataChange(cloudData, cloudDataList->key, false );

                cloudData->Clear();

                if (cloudData->IsUnused())
                {
                    N_delete cloudData;
                    cloudDataList->keyData.RemoveAtIndex(keyDataListIndex);
                    if (cloudDataList->IsNotUploaded())
                    {
                        // Tell other servers that this key is no longer uploaded, so they do not request it from us
                        RemoveUploadedKeyFromServers(cloudDataList->key);
                    }

                    if (cloudDataList->IsUnused())
                    {
                        N_delete cloudDataList;
                        dataRepository.RemoveAtIndex(dataRepositoryIndex);
                    }
                }

                if (remoteCloudClient->IsUnused())
                {
                    N_delete remoteCloudClient;
                    mRemotes.RemoveAtIndex(hindex);
                    break;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::OnGetRequest(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);
        Nui16 specificSystemsCount;
        CloudKey cloudKey;

        // Create a new GetRequest
        GetRequest *getRequest;
        getRequest = N_new GetRequest();
        getRequest->cloudQueryWithAddresses.cloudQuery.write(&bsIn);
        getRequest->requestingClient=msg->mID;

        UniqueID addressOrGuid;
        bsIn.read(specificSystemsCount);
        for (Nui16 i=0; i < specificSystemsCount; i++)
        {
            bsIn.read(addressOrGuid);
            getRequest->cloudQueryWithAddresses.specificSystems.Push(addressOrGuid);
        }

        if (getRequest->cloudQueryWithAddresses.cloudQuery.keys.Size()==0)
        {
            N_delete getRequest;
            return;
        }

        for (Nui32 filterIndex=0; filterIndex < queryFilters.Size(); filterIndex++)
        {
            if (queryFilters[filterIndex]->OnGetRequest(msg->mID, msg->mAddress, getRequest->cloudQueryWithAddresses.cloudQuery, getRequest->cloudQueryWithAddresses.specificSystems )==false)
                return;
        }

        getRequest->requestStartTime=N_Engine().getTimer().getMS();
        getRequest->requestId=nextGetRequestId++;

        // send request to servers that have this data
        DataStructures::List<RemoteServer*> remoteServersWithData;
        GetServersWithUploadedKeys(getRequest->cloudQueryWithAddresses.cloudQuery.keys, remoteServersWithData);

        if (remoteServersWithData.Size()==0)
        {
            ProcessAndTransmitGetRequest(getRequest);
        }
        else
        {
            NetSerializer bsOut;
            bsOut.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
            bsOut.write((Token)STSC_PROCESS_GET_REQUEST);
            getRequest->cloudQueryWithAddresses.read(&bsOut);
            bsOut.write(getRequest->requestId);

            for (Nui32 remoteServerIndex=0; remoteServerIndex < remoteServersWithData.Size(); remoteServerIndex++)
            {
                BufferedGetResponseFromServer* bufferedGetResponseFromServer = N_new BufferedGetResponseFromServer();
                bufferedGetResponseFromServer->serverAddress=remoteServersWithData[remoteServerIndex]->serverAddress;
                bufferedGetResponseFromServer->gotResult=false;
                getRequest->remoteServerResponses.Insert(remoteServersWithData[remoteServerIndex]->serverAddress, bufferedGetResponseFromServer, true);

                send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServersWithData[remoteServerIndex]->serverAddress, false);
            }

            // Record that this system made this request
            getRequests.Insert(getRequest->requestId, getRequest, true);
        }

        if (getRequest->cloudQueryWithAddresses.cloudQuery.subscribeToResults)
        {
            // Add to key subscription list for the client, which contains a keyId / specificUploaderList pair
            DataStructures::HashIndex remoteSystemsHashIndex = mRemotes.GetIndexOf(msg->mID);
            RemoteCloudClient *remoteCloudClient;
            if (remoteSystemsHashIndex.IsInvalid())
            {
                remoteCloudClient = N_new RemoteCloudClient();
                remoteCloudClient->uploadedBytes=0;
                mRemotes.Push(msg->mID, remoteCloudClient);
            }
            else
            {
                remoteCloudClient = mRemotes.ItemAtIndex(remoteSystemsHashIndex);
            }

            Nui32 keyIndex;
            for (keyIndex=0; keyIndex < getRequest->cloudQueryWithAddresses.cloudQuery.keys.Size(); keyIndex++)
            {
                cloudKey = getRequest->cloudQueryWithAddresses.cloudQuery.keys[keyIndex];

                Nui32 keySubscriberIndex;
                bool hasKeySubscriber;
                keySubscriberIndex = remoteCloudClient->subscribedKeys.GetIndexFromKey(cloudKey, &hasKeySubscriber);
                KeySubscriberID* keySubscriberId;
                if (hasKeySubscriber)
                {
                    DataStructures::List<UniqueID> specificSystems;
                    UnsubscribeFromKey(remoteCloudClient, msg->mID, keySubscriberIndex, cloudKey, specificSystems);
                }

                keySubscriberId = N_new KeySubscriberID();
                keySubscriberId->key=cloudKey;

                Nui32 specificSystemIndex;
                for (specificSystemIndex=0; specificSystemIndex < getRequest->cloudQueryWithAddresses.specificSystems.Size(); specificSystemIndex++)
                {
                    keySubscriberId->specificSystemsSubscribedTo.Insert(getRequest->cloudQueryWithAddresses.specificSystems[specificSystemIndex], getRequest->cloudQueryWithAddresses.specificSystems[specificSystemIndex], true);
                }

                remoteCloudClient->subscribedKeys.InsertAtIndex(keySubscriberId, keySubscriberIndex);

                // Add CloudData in a similar way
                Nui32 dataRepositoryIndex;
                bool dataRepositoryExists;
                CloudDataList* cloudDataList = GetOrAllocateCloudDataList(cloudKey, &dataRepositoryExists, dataRepositoryIndex);

                // If this is the first local client to subscribe to this key, call SendSubscribedKeyToServers
                if (cloudDataList->subscriberCount==0)
                    SendSubscribedKeyToServers(cloudKey);

                // If the subscription is specific, may have to also allocate CloudData
                if (getRequest->cloudQueryWithAddresses.specificSystems.Size())
                {
                    CloudData *cloudData;
                    bool keyDataListExists;

                    Nui32 specificSystemIndex;
                    for (specificSystemIndex=0; specificSystemIndex < getRequest->cloudQueryWithAddresses.specificSystems.Size(); specificSystemIndex++)
                    {
                        UniqueID specificSystem = getRequest->cloudQueryWithAddresses.specificSystems[specificSystemIndex];

                        Nui32 keyDataListIndex = cloudDataList->keyData.GetIndexFromKey(specificSystem, &keyDataListExists);
                        if (keyDataListExists==false)
                        {
                            cloudData = N_new CloudData();
                            cloudData->dataLengthBytes=0;
                            cloudData->allocatedData=0;
                            cloudData->isUploaded=false;
                            cloudData->dataPtr=0;
                            cloudData->serverSystemAddress = 0;
                            cloudData->clientSystemAddress = 0;
                            cloudData->serverGUID=mPrc2->getGuid();
                            cloudData->clientGUID=specificSystem;
                            cloudDataList->keyData.Insert(specificSystem,cloudData,true);
                        }
                        else
                        {
                            cloudData = cloudDataList->keyData[keyDataListIndex];
                        }

                        ++cloudDataList->subscriberCount;
                        cloudData->specificSubscribers.Insert(msg->mID, msg->mID, true);
                    }
                }
                else
                {
                    ++cloudDataList->subscriberCount;
                    cloudDataList->nonSpecificSubscribers.Insert(msg->mID, msg->mID, true);

                    // Remove msg->mID from CloudData::specificSubscribers among all instances of cloudDataList->keyData
                    Nui32 subscribedKeysIndex;
                    bool subscribedKeysIndexExists;
                    subscribedKeysIndex = remoteCloudClient->subscribedKeys.GetIndexFromKey(cloudDataList->key, &subscribedKeysIndexExists);
                    if (subscribedKeysIndexExists)
                    {
                        KeySubscriberID* keySubscriberId;
                        keySubscriberId = remoteCloudClient->subscribedKeys[subscribedKeysIndex];
                        Nui32 specificSystemIndex;
                        for (specificSystemIndex=0; specificSystemIndex < keySubscriberId->specificSystemsSubscribedTo.Size(); specificSystemIndex++)
                        {
                            bool keyDataExists;
                            Nui32 keyDataIndex = cloudDataList->keyData.GetIndexFromKey(keySubscriberId->specificSystemsSubscribedTo[specificSystemIndex], &keyDataExists);
                            if (keyDataExists)
                            {
                                CloudData *keyData = cloudDataList->keyData[keyDataIndex];
                                keyData->specificSubscribers.Remove(msg->mID);
                                --cloudDataList->subscriberCount;
                            }
                        }
                    }
                }
            }

            if (remoteCloudClient->subscribedKeys.Size()==0)
            {
                // Didn't do anything
                mRemotes.Remove(msg->mID);
                N_delete remoteCloudClient;
            }
        }

        if (remoteServersWithData.Size()==0)
            N_delete getRequest;
    }
    //------------------------------------------------------------------------
    void CloudServer::OnUnsubscribeRequest(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(1, 0);

        DataStructures::HashIndex hindex = mRemotes.GetIndexOf(msg->mID);
        if(hindex.IsInvalid() == true)
            return;

        RemoteCloudClient * remoteCloudClient = mRemotes.ItemAtIndex(hindex);

        Nui16 keyCount, specificSystemCount;
        DataStructures::List<CloudKey> cloudKeys;
        DataStructures::List<UniqueID> specificSystems;
        Nui16 index;

        CloudKey cloudKey;
        bsIn.read(keyCount);
        for(index = 0; index < keyCount; ++index)
        {
            cloudKey.write(&bsIn);
            cloudKeys.Push(cloudKey);
        }

        UniqueID specificSystem;
        bsIn.read(specificSystemCount);
        for(index = 0; index < specificSystemCount; ++index)
        {
            bsIn.read(specificSystem);
            specificSystems.Push(specificSystem);
        }

        for(Nui32 filterIndex=0; filterIndex < queryFilters.Size(); ++filterIndex)
        {
            if (queryFilters[filterIndex]->OnUnsubscribeRequest(msg->mID, msg->mAddress, cloudKeys, specificSystems )==false)
                return;
        }

        bool dataRepositoryExists;

        for(index=0; index < keyCount; ++index)
        {
            CloudKey cloudKey = cloudKeys[index];

            dataRepository.GetIndexFromKey(cloudKey, &dataRepositoryExists);
            if (dataRepositoryExists==false)
                continue;

            Nui32 keySubscriberIndex;
            bool hasKeySubscriber;
            keySubscriberIndex = remoteCloudClient->subscribedKeys.GetIndexFromKey(cloudKey, &hasKeySubscriber);

            if (hasKeySubscriber==false)
                continue;

            UnsubscribeFromKey(remoteCloudClient, msg->mID, keySubscriberIndex, cloudKey, specificSystems);
        }

        if (remoteCloudClient->IsUnused())
        {
            N_delete remoteCloudClient;
            mRemotes.RemoveAtIndex(hindex);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::OnServerToServerGetRequest(SocketMessage * msg)
    {
        bool objectExists;
        remoteServers.GetIndexFromKey(msg->mID, &objectExists);
        if(objectExists == false)
            return;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        CloudQueryWithAddresses cloudQueryWithAddresses;
        Nui32 requestId;
        cloudQueryWithAddresses.write(&bsIn);
        bsIn.read(requestId);

        DataStructures::List<CloudData *> cloudDataResultList;
        DataStructures::List<CloudKey> cloudKeyResultList;
        ProcessCloudQueryWithAddresses(cloudQueryWithAddresses, cloudDataResultList, cloudKeyResultList);

        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        bsOut.write((Token)STSC_PROCESS_GET_RESPONSE);
        bsOut.write(requestId);
        WriteCloudQueryRowFromResultList(cloudDataResultList, cloudKeyResultList, &bsOut);
        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, msg->mID, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnServerToServerGetResponse(SocketMessage * msg)
    {
        Nui32 remoteServerIndex;
        bool objectExists;
        remoteServerIndex = remoteServers.GetIndexFromKey(msg->mID, &objectExists);
        if (objectExists==false)
            return;

        RemoteServer *remoteServer = remoteServers[remoteServerIndex];
        if (remoteServer==0)
            return;

        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        Nui32 requestId;
        bsIn.read(requestId);

        // Lookup request id
        bool hasGetRequest;
        Nui32 getRequestIndex;
        getRequestIndex = getRequests.GetIndexFromKey(requestId, &hasGetRequest);
        if (hasGetRequest==false)
            return;
        GetRequest *getRequest = getRequests[getRequestIndex];
        bool hasRemoteServer;
        Nui32 remoteServerResponsesIndex;
        remoteServerResponsesIndex = getRequest->remoteServerResponses.GetIndexFromKey(msg->mID, &hasRemoteServer);
        if (hasRemoteServer==false)
            return;
        BufferedGetResponseFromServer *bufferedGetResponseFromServer;
        bufferedGetResponseFromServer = getRequest->remoteServerResponses[remoteServerResponsesIndex];
        if (bufferedGetResponseFromServer->gotResult==true)
            return;
        bufferedGetResponseFromServer->gotResult=true;
        Nui32 numRows;
        bufferedGetResponseFromServer->queryResult.writeNumRows(numRows, &bsIn);
        bufferedGetResponseFromServer->queryResult.writeCloudQueryRows(numRows, &bsIn, this);

        // If all results returned, then also process locally, and return to user
        if (getRequest->AllRemoteServersHaveResponded())
        {
            ProcessAndTransmitGetRequest(getRequest);

            getRequest->Clear(this);
            N_delete getRequest;

            getRequests.RemoveAtIndex(getRequestIndex);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::onConnectClose(const Address &address, UniqueID id,
        ConnectCloseType lostConnectionReason)
    {
        (void) lostConnectionReason;
        (void) address;

        Nui32 remoteServerIndex;
        bool objectExists;
        remoteServerIndex = remoteServers.GetIndexFromKey(id, &objectExists);
        if (objectExists)
        {
            // update remoteServerResponses by removing this server and sending the response if it is now complete
            Nui32 getRequestIndex=0;
            while (getRequestIndex < getRequests.Size())
            {
                GetRequest *getRequest = getRequests[getRequestIndex];
                bool waitingForThisServer;
                Nui32 remoteServerResponsesIndex = getRequest->remoteServerResponses.GetIndexFromKey(id, &waitingForThisServer);
                if (waitingForThisServer)
                {
                    getRequest->remoteServerResponses[remoteServerResponsesIndex]->Clear(this);
                    N_delete getRequest->remoteServerResponses[remoteServerResponsesIndex];
                    getRequest->remoteServerResponses.RemoveAtIndex(remoteServerResponsesIndex);

                    if (getRequest->AllRemoteServersHaveResponded())
                    {
                        ProcessAndTransmitGetRequest(getRequest);
                        getRequest->Clear(this);
                        N_delete getRequest;

                        getRequests.RemoveAtIndex(getRequestIndex);
                    }
                    else
                        getRequestIndex++;
                }
                else
                    getRequestIndex++;
            }

            N_delete remoteServers[remoteServerIndex];
            remoteServers.RemoveAtIndex(remoteServerIndex);
        }

        DataStructures::HashIndex hindex = mRemotes.GetIndexOf(id);
        if (hindex.IsInvalid()==false)
        {
            RemoteCloudClient* remoteCloudClient = mRemotes.ItemAtIndex(hindex);
            Nui32 uploadedKeysIndex;
            for (uploadedKeysIndex=0; uploadedKeysIndex < remoteCloudClient->uploadedKeys.Size(); uploadedKeysIndex++)
            {
                // Delete keys this system has uploaded
                bool keyDataRepositoryExists;
                Nui32 dataRepositoryIndex = dataRepository.GetIndexFromKey(remoteCloudClient->uploadedKeys[uploadedKeysIndex], &keyDataRepositoryExists);
                if (keyDataRepositoryExists)
                {
                    CloudDataList* cloudDataList = dataRepository[dataRepositoryIndex];
                    bool keyDataExists;
                    Nui32 keyDataIndex = cloudDataList->keyData.GetIndexFromKey(id, &keyDataExists);
                    if (keyDataExists)
                    {
                        CloudData *cloudData = cloudDataList->keyData[keyDataIndex];
                        cloudDataList->uploaderCount--;

                        NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudData->specificSubscribers, false );
                        NotifyClientSubscribersOfDataChange(cloudData, cloudDataList->key, cloudDataList->nonSpecificSubscribers, false );
                        NotifyServerSubscribersOfDataChange(cloudData, cloudDataList->key, false );

                        cloudData->Clear();

                        if (cloudData->IsUnused())
                        {
                            N_delete cloudData;
                            cloudDataList->keyData.RemoveAtIndex(keyDataIndex);

                            if (cloudDataList->IsNotUploaded())
                            {
                                // Tell other servers that this key is no longer uploaded, so they do not request it from us
                                RemoveUploadedKeyFromServers(cloudDataList->key);
                            }

                            if (cloudDataList->IsUnused())
                            {
                                // Tell other servers that this key is no longer uploaded, so they do not request it from us
                                RemoveUploadedKeyFromServers(cloudDataList->key);

                                N_delete cloudDataList;
                                dataRepository.RemoveAtIndex(dataRepositoryIndex);
                            }
                        }
                    }
                }
            }

            Nui32 subscribedKeysIndex;
            for (subscribedKeysIndex=0; subscribedKeysIndex < remoteCloudClient->subscribedKeys.Size(); subscribedKeysIndex++)
            {
                KeySubscriberID* keySubscriberId;
                keySubscriberId = remoteCloudClient->subscribedKeys[subscribedKeysIndex];

                bool keyDataRepositoryExists;
                Nui32 keyDataRepositoryIndex = dataRepository.GetIndexFromKey(remoteCloudClient->subscribedKeys[subscribedKeysIndex]->key, &keyDataRepositoryExists);
                if (keyDataRepositoryExists)
                {
                    CloudDataList* cloudDataList = dataRepository[keyDataRepositoryIndex];
                    if (keySubscriberId->specificSystemsSubscribedTo.Size()==0)
                    {
                        cloudDataList->nonSpecificSubscribers.Remove(id);
                        --cloudDataList->subscriberCount;
                    }
                    else
                    {
                        Nui32 specificSystemIndex;
                        for (specificSystemIndex=0; specificSystemIndex < keySubscriberId->specificSystemsSubscribedTo.Size(); specificSystemIndex++)
                        {
                            bool keyDataExists;
                            Nui32 keyDataIndex = cloudDataList->keyData.GetIndexFromKey(keySubscriberId->specificSystemsSubscribedTo[specificSystemIndex], &keyDataExists);
                            if (keyDataExists)
                            {
                                CloudData *keyData = cloudDataList->keyData[keyDataIndex];
                                keyData->specificSubscribers.Remove(id);
                                --cloudDataList->subscriberCount;
                            }
                        }
                    }
                }
                N_delete keySubscriberId;
            }

            // Delete and remove from remotes
            N_delete remoteCloudClient;
            mRemotes.RemoveAtIndex(hindex);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::shutdown()
    {
        Clear();
    }
    //------------------------------------------------------------------------
    void CloudServer::Clear()
    {
        Nui32 i, j;
        for(i = 0; i < dataRepository.Size(); ++i)
        {
            CloudDataList * cloudDataList = dataRepository[i];
            for(j = 0; j < cloudDataList->keyData.Size(); ++j)
            {
                cloudDataList->keyData[j]->Clear();
                N_delete cloudDataList->keyData[j];
            }
            N_delete cloudDataList;
        }
        dataRepository.Clear(false);

        for(i = 0; i < remoteServers.Size(); ++i)
        {
            N_delete remoteServers[i];
        }
        remoteServers.Clear(false);

        for(i = 0; i < getRequests.Size(); ++i)
        {
            GetRequest * getRequest = getRequests[i];
            getRequest->Clear(this);
            N_delete getRequests[i];
        }
        getRequests.Clear(false);

        DataStructures::List<UniqueID> keyList;
        DataStructures::List<RemoteCloudClient*> itemList;
        mRemotes.GetAsList(itemList, keyList);
        for (i=0; i < itemList.Size(); i++)
        {
            RemoteCloudClient* remoteCloudClient = itemList[i];
            for (j=0; j < remoteCloudClient->subscribedKeys.Size(); j++)
            {
                N_delete remoteCloudClient->subscribedKeys[j];
            }
            N_delete remoteCloudClient;
        }
        mRemotes.Clear();
    }
    //------------------------------------------------------------------------
    void CloudServer::WriteCloudQueryRowFromResultList(DataStructures::List<CloudData*> &cloudDataResultList, DataStructures::List<CloudKey> &cloudKeyResultList, NetSerializer *bsOut)
    {
        bsOut->write((Nui32)cloudKeyResultList.Size());
        Nui32 i;
        for (i=0; i < cloudKeyResultList.Size(); i++)
        {
            WriteCloudQueryRowFromResultList(i, cloudDataResultList, cloudKeyResultList, bsOut);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::WriteCloudQueryRowFromResultList(Nui32 i,
        DataStructures::List<CloudData *> &cloudDataResultList,
            DataStructures::List<CloudKey> &cloudKeyResultList, NetSerializer * bsOut)
    {
        CloudQueryRow cloudQueryRow;
        CloudData * cloudData = cloudDataResultList[i];
        cloudQueryRow.key = cloudKeyResultList[i];
        cloudQueryRow.mData = cloudData->dataPtr;
        cloudQueryRow.mLength = cloudData->dataLengthBytes;
        cloudQueryRow.serverSystemAddress = cloudData->serverSystemAddress;
        cloudQueryRow.clientSystemAddress = cloudData->clientSystemAddress;
        cloudQueryRow.serverGUID = cloudData->serverGUID;
        cloudQueryRow.clientGUID = cloudData->clientGUID;
        cloudQueryRow.read(bsOut, 0);
    }
    //------------------------------------------------------------------------
    void CloudServer::NotifyClientSubscribersOfDataChange(CloudData * cloudData,
        CloudKey & key, DataStructures::OrderedList<UniqueID, UniqueID> & subscribers,
            bool wasUpdated)
    {
        NetSerializer bsOut;
        bsOut.write((Token) ID_CLOUD_SUBSCRIPTION_NOTIFICATION);
        bsOut.write(wasUpdated);
        CloudQueryRow row;
        row.key = key;
        row.mData = cloudData->dataPtr;
        row.mLength = cloudData->dataLengthBytes;
        row.serverSystemAddress = cloudData->serverSystemAddress;
        row.clientSystemAddress = cloudData->clientSystemAddress;
        row.serverGUID = cloudData->serverGUID;
        row.clientGUID = cloudData->clientGUID;
        row.read(&bsOut, 0);

        NCount i;
        for (i = 0; i < subscribers.Size(); ++i)
        {
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, subscribers[i], false);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::NotifyClientSubscribersOfDataChange(CloudQueryRow * row,
        DataStructures::OrderedList<UniqueID, UniqueID> & subscribers, bool wasUpdated)
    {
        NetSerializer out;
        out.write((Token) ID_CLOUD_SUBSCRIPTION_NOTIFICATION);
        out.write(wasUpdated);
        row->read(&out, 0);

        NCount i;
        for (i = 0; i < subscribers.Size(); ++i)
        {
            send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, subscribers[i], false);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::NotifyServerSubscribersOfDataChange(CloudData * cloudData,
        CloudKey & key, bool wasUpdated)
    {
        // Find every server that has subscribed
        // send them change notifications
        NetSerializer out;
        out.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        out.write((Token)STSC_DATA_CHANGED);
        out.write(wasUpdated);
        CloudQueryRow row;
        row.key = key;
        row.mData = cloudData->dataPtr;
        row.mLength = cloudData->dataLengthBytes;
        row.serverSystemAddress = cloudData->serverSystemAddress;
        row.clientSystemAddress = cloudData->clientSystemAddress;
        row.serverGUID = cloudData->serverGUID;
        row.clientGUID = cloudData->clientGUID;
        row.read(&out, 0);

        NCount i;
        for(i = 0; i < remoteServers.Size(); ++i)
        {
            if(remoteServers[i]->gotSubscribedAndUploadedKeys == false ||
                remoteServers[i]->subscribedKeys.HasData(key))
            {
                send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServers[i]->serverAddress, false);
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::AddServer(UniqueID id)
    {
        ConnectionState cs = mPrc2->getState(id);
        if (cs == IS_DISCONNECTED || cs == IS_NOT_CONNECTED)
            return;
        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(id, &objectExists);
        if (objectExists == false)
        {
            RemoteServer * remoteServer = N_new RemoteServer();
            remoteServer->gotSubscribedAndUploadedKeys = false;
            remoteServer->serverAddress = id;
            remoteServers.InsertAtIndex(remoteServer, index);

            SendUploadedAndSubscribedKeysToServer(id);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveServer(UniqueID address)
    {
        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(address, &objectExists);
        if(objectExists == true)
        {
            N_delete remoteServers[index];
            remoteServers.RemoveAtIndex(index);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::GetRemoteServers(DataStructures::List<UniqueID> & remoteServersOut)
    {
        remoteServersOut.Clear(true);

        Nui32 i;
        for (i=0; i < remoteServers.Size(); i++)
        {
            remoteServersOut.Push(remoteServers[i]->serverAddress);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::ProcessAndTransmitGetRequest(GetRequest * getRequest)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_GET_RESPONSE);

        //	BufferedGetResponseFromServer getResponse;
        CloudQueryResult cloudQueryResult;
        cloudQueryResult.cloudQuery = getRequest->cloudQueryWithAddresses.cloudQuery;
        cloudQueryResult.subscribeToResults = getRequest->cloudQueryWithAddresses.cloudQuery.subscribeToResults;
        cloudQueryResult.readHeader(&bsOut);

        DataStructures::List<CloudData *> cloudDataResultList;
        DataStructures::List<CloudKey> cloudKeyResultList;
        ProcessCloudQueryWithAddresses(getRequest->cloudQueryWithAddresses, cloudDataResultList, cloudKeyResultList);
        bool unlimitedRows = getRequest->cloudQueryWithAddresses.cloudQuery.maxRowsToReturn == 0;

        Nui32 localNumRows = (Nui32) cloudDataResultList.Size();
        if(unlimitedRows == false &&
            localNumRows > getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex &&
            localNumRows - getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex > getRequest->cloudQueryWithAddresses.cloudQuery.maxRowsToReturn )
            localNumRows = getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex + getRequest->cloudQueryWithAddresses.cloudQuery.maxRowsToReturn;

        NBitCount bitStreamBOffset = bsOut.getWriteOffset();
        Nui32 localRowsToWrite;
        Nui32 skipRows;

        if(localNumRows > getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex)
        {
            localRowsToWrite = localNumRows-getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex;
            skipRows = 0;
        }
        else
        {
            localRowsToWrite = 0;
            skipRows = getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex-localNumRows;
        }
        cloudQueryResult.readNumRows(localRowsToWrite, &bsOut);
        for(Nui32 i = getRequest->cloudQueryWithAddresses.cloudQuery.startingRowIndex; i < localNumRows; i++)
        {
            WriteCloudQueryRowFromResultList(i, cloudDataResultList, cloudKeyResultList, &bsOut);
        }

        // Append remote systems for remaining rows
        if(unlimitedRows == true || getRequest->cloudQueryWithAddresses.cloudQuery.maxRowsToReturn>localRowsToWrite)
        {
            Nui32 remainingRows = 0;
            Nui32 additionalRowsWritten = 0;
            if(unlimitedRows == false)
                remainingRows = getRequest->cloudQueryWithAddresses.cloudQuery.maxRowsToReturn-localRowsToWrite;

            Nui32 remoteServerResponseIndex;
            for(remoteServerResponseIndex = 0; remoteServerResponseIndex < getRequest->remoteServerResponses.Size(); remoteServerResponseIndex++)
            {
                BufferedGetResponseFromServer * bufferedGetResponseFromServer = getRequest->remoteServerResponses[remoteServerResponseIndex];
                Nui32 cloudQueryRowIndex;
                for(cloudQueryRowIndex = 0; cloudQueryRowIndex < bufferedGetResponseFromServer->queryResult.rowsReturned.Size(); cloudQueryRowIndex++)
                {
                    if(skipRows>0)
                    {
                        --skipRows;
                        continue;
                    }
                    bufferedGetResponseFromServer->queryResult.rowsReturned[cloudQueryRowIndex]->read(&bsOut, this);

                    ++additionalRowsWritten;
                    if(unlimitedRows == false && --remainingRows == 0)
                        break;
                }

                if(unlimitedRows == false && remainingRows == 0)
                    break;
            }

            if(additionalRowsWritten > 0)
            {
                NBitCount curBOffset = bsOut.getWriteOffset();
                bsOut.seekWrite(bitStreamBOffset);
                localRowsToWrite += additionalRowsWritten;
                cloudQueryResult.readNumRows(localRowsToWrite, &bsOut);
                bsOut.seekWrite(curBOffset);
            }
        }

        send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, getRequest->requestingClient, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::ProcessCloudQueryWithAddresses(CloudServer::CloudQueryWithAddresses &cloudQueryWithAddresses, DataStructures::List<CloudData*> &cloudDataResultList, DataStructures::List<CloudKey> &cloudKeyResultList )
    {
        CloudQueryResult cloudQueryResult;
        CloudQueryRow cloudQueryRow;
        Nui32 queryIndex;
        bool dataRepositoryExists;
        CloudDataList* cloudDataList;
        Nui32 keyDataIndex;

        // If specificSystems list empty, applies to all systems
        // For each of keys in cloudQueryWithAddresses, return that data, limited by maxRowsToReturn
        for (queryIndex=0; queryIndex < cloudQueryWithAddresses.cloudQuery.keys.Size(); queryIndex++)
        {
            const CloudKey &key = cloudQueryWithAddresses.cloudQuery.keys[queryIndex];

            Nui32 dataRepositoryIndex = dataRepository.GetIndexFromKey(key, &dataRepositoryExists);
            if (dataRepositoryExists)
            {
                cloudDataList=dataRepository[dataRepositoryIndex];

                if (cloudDataList->uploaderCount>0)
                {
                    // Return all keyData that was uploaded by specificSystems, or all if not specified
                    if (cloudQueryWithAddresses.specificSystems.Size()>0)
                    {
                        // Return data for matching systems
                        Nui32 specificSystemIndex;
                        for (specificSystemIndex=0; specificSystemIndex < cloudQueryWithAddresses.specificSystems.Size(); specificSystemIndex++)
                        {
                            bool uploaderExists;
                            keyDataIndex = cloudDataList->keyData.GetIndexFromKey(cloudQueryWithAddresses.specificSystems[specificSystemIndex], &uploaderExists);
                            if (uploaderExists)
                            {
                                cloudDataResultList.Push(cloudDataList->keyData[keyDataIndex]);
                                cloudKeyResultList.Push(key);
                            }
                        }
                    }
                    else
                    {
                        // Return data for all systems
                        for (keyDataIndex=0; keyDataIndex < cloudDataList->keyData.Size(); keyDataIndex++)
                        {
                            cloudDataResultList.Push(cloudDataList->keyData[keyDataIndex]);
                            cloudKeyResultList.Push(key);
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::SendUploadedAndSubscribedKeysToServer(UniqueID address)
    {
        NetSerializer bsOut;
        bsOut.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        bsOut.write((Token)STSC_ADD_UPLOADED_AND_SUBSCRIBED_KEYS);
        bsOut.write((Nui16)dataRepository.Size());
        for (NCount i = 0; i < dataRepository.Size(); ++i)
            dataRepository[i]->key.read(&bsOut);

        NBitCount startBOffset, endBOffset;
        Nui16 subscribedKeyCount = 0;
        startBOffset = bsOut.getWriteOffset();
        bsOut.write((Nui16)subscribedKeyCount); // ¡Ÿ ±
        for(NCount i = 0; i < dataRepository.Size(); ++i)
        {
            if(dataRepository[i]->subscriberCount > 0)
            {
                dataRepository[i]->key.read(&bsOut);
                ++subscribedKeyCount;
            }
        }

        endBOffset = bsOut.getWriteOffset();
        bsOut.seekWrite(startBOffset);
        bsOut.write((Nui16)subscribedKeyCount);// ’Ê’˝
        bsOut.seekWrite(endBOffset);

        if (dataRepository.Size() > 0 || subscribedKeyCount > 0)
            send(&bsOut, PL_HIGH, PT_ORDER_MAKE_SURE, 0, address, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::SendUploadedKeyToServers(CloudKey & cloudKey)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        out.write((Token)STSC_ADD_UPLOADED_KEY);
        cloudKey.read(&out);
        for (NCount i = 0; i < remoteServers.Size(); ++i)
            send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServers[i]->serverAddress, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::SendSubscribedKeyToServers(CloudKey & cloudKey)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        out.write((Token)STSC_ADD_SUBSCRIBED_KEY);
        cloudKey.read(&out);
        for (NCount i = 0; i < remoteServers.Size(); ++i)
            send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServers[i]->serverAddress, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveUploadedKeyFromServers(CloudKey & cloudKey)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        out.write((Token)STSC_REMOVE_UPLOADED_KEY);
        cloudKey.read(&out);
        for (NCount i = 0; i < remoteServers.Size(); ++i)
            send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServers[i]->serverAddress, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveSubscribedKeyFromServers(CloudKey & cloudKey)
    {
        NetSerializer out;
        out.write((Token)ID_CLOUD_SERVER_TO_SERVER_COMMAND);
        out.write((Token)STSC_REMOVE_SUBSCRIBED_KEY);
        cloudKey.read(&out);
        for (NCount i = 0; i < remoteServers.Size(); ++i)
            send(&out, PL_HIGH, PT_ORDER_MAKE_SURE, 0, remoteServers[i]->serverAddress, false);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnSendUploadedAndSubscribedKeysToServer(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(msg->mID,&objectExists);
        if (objectExists==false)
            return;
        RemoteServer *remoteServer = remoteServers[index];
        remoteServer->gotSubscribedAndUploadedKeys = true;

        bool alreadyHasKey;
        Nui16 numUploadedKeys, numSubscribedKeys;
        bsIn.read(numUploadedKeys);
        for (Nui16 i=0; i < numUploadedKeys; ++i)
        {
            CloudKey cloudKey;
            cloudKey.write(&bsIn);

            remoteServer->uploadedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);
            if (alreadyHasKey==false)
                remoteServer->uploadedKeys.Insert(cloudKey,cloudKey,true);
        }

        bsIn.read(numSubscribedKeys);
        for (Nui16 i=0; i < numSubscribedKeys; ++i)
        {
            CloudKey cloudKey;
            cloudKey.write(&bsIn);

            //insertionIndex =
                remoteServer->subscribedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);
            if (alreadyHasKey==false)
                remoteServer->subscribedKeys.Insert(cloudKey,cloudKey,true);
        }

        // Potential todo - join servers
        // For each uploaded key that we subscribe to, query it
        // For each subscribed key that we have, send it
    }
    //------------------------------------------------------------------------
    void CloudServer::OnSendUploadedKeyToServers(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(msg->mID,&objectExists);
        if (objectExists == false)
            return;
        RemoteServer * remoteServer = remoteServers[index];
        CloudKey cloudKey;
        cloudKey.write(&bsIn);
        bool alreadyHasKey;
        remoteServer->uploadedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);
        if (alreadyHasKey == false)
            remoteServer->uploadedKeys.Insert(cloudKey,cloudKey,true);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnSendSubscribedKeyToServers(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(msg->mID,&objectExists);
        if (objectExists == false)
            return;
        RemoteServer * remoteServer = remoteServers[index];
        CloudKey cloudKey;
        cloudKey.write(&bsIn);
        bool alreadyHasKey;
        remoteServer->subscribedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);

        // Do not need to send current values, the Get request will do that as the Get request is sent at the same time
        if (alreadyHasKey == false)
            remoteServer->subscribedKeys.Insert(cloudKey,cloudKey,true);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnRemoveUploadedKeyFromServers(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(msg->mID,&objectExists);
        if (objectExists==false)
            return;
        RemoteServer *remoteServer = remoteServers[index];
        CloudKey cloudKey;
        cloudKey.write(&bsIn);
        Nui32 insertionIndex;
        bool alreadyHasKey;
        insertionIndex = remoteServer->uploadedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);
        if (alreadyHasKey==true)
            remoteServer->uploadedKeys.RemoveAtIndex(insertionIndex);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnRemoveSubscribedKeyFromServers(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        Nui32 index = remoteServers.GetIndexFromKey(msg->mID,&objectExists);
        if (objectExists==false)
            return;
        RemoteServer *remoteServer = remoteServers[index];
        CloudKey cloudKey;
        cloudKey.write(&bsIn);
        Nui32 insertionIndex;
        bool alreadyHasKey;
        insertionIndex = remoteServer->subscribedKeys.GetIndexFromKey(cloudKey, &alreadyHasKey);
        if (alreadyHasKey==true)
            remoteServer->subscribedKeys.RemoveAtIndex(insertionIndex);
    }
    //------------------------------------------------------------------------
    void CloudServer::OnServerDataChanged(SocketMessage * msg)
    {
        NetSerializer bsIn(msg->mData, msg->mLength, false);
        bsIn.skipRead(2, 0);

        bool objectExists;
        remoteServers.GetIndexFromKey(msg->mID, &objectExists);
        if (objectExists==false)
            return;

        // Find everyone that cares about this change and relay
        bool wasUpdated=false;
        bsIn.read(wasUpdated);
        CloudQueryRow row;
        row.write(&bsIn, this);

        CloudDataList *cloudDataList;
        bool dataRepositoryExists;
        Nui32 dataRepositoryIndex;
        dataRepositoryIndex = dataRepository.GetIndexFromKey(row.key, &dataRepositoryExists);
        if (dataRepositoryExists==false)
        {
            DeallocateRowData(row.mData);
            return;
        }
        cloudDataList = dataRepository[dataRepositoryIndex];
        CloudData *cloudData;
        bool keyDataListExists;
        Nui32 keyDataListIndex = cloudDataList->keyData.GetIndexFromKey(row.clientGUID, &keyDataListExists);
        if (keyDataListExists==true)
        {
            cloudData = cloudDataList->keyData[keyDataListIndex];
            NotifyClientSubscribersOfDataChange(&row, cloudData->specificSubscribers, wasUpdated );
        }

        NotifyClientSubscribersOfDataChange(&row, cloudDataList->nonSpecificSubscribers, wasUpdated );
        DeallocateRowData(row.mData);
    }
    //------------------------------------------------------------------------
    void CloudServer::GetServersWithUploadedKeys(
        DataStructures::List<CloudKey> &keys,
            DataStructures::List<CloudServer::RemoteServer*> &remoteServersWithData)
    {
        remoteServersWithData.Clear(true);

        Nui32 i,j;
        for (i=0; i < remoteServers.Size(); i++)
        {
            remoteServers[i]->workingFlag=false;
        }

        for (i=0; i < remoteServers.Size(); i++)
        {
            if (remoteServers[i]->workingFlag==false)
            {
                if (remoteServers[i]->gotSubscribedAndUploadedKeys==false)
                {
                    remoteServers[i]->workingFlag=true;
                    remoteServersWithData.Push(remoteServers[i]);
                }
                else
                {
                    remoteServers[i]->workingFlag=false;
                    for (j=0; j < keys.Size(); j++)
                    {
                        if (remoteServers[i]->workingFlag==false && remoteServers[i]->uploadedKeys.HasData(keys[j]))
                        {
                            remoteServers[i]->workingFlag=true;
                            remoteServersWithData.Push(remoteServers[i]);
                            break;
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    CloudServer::CloudDataList *CloudServer::GetOrAllocateCloudDataList(CloudKey key, bool *dataRepositoryExists, Nui32 &dataRepositoryIndex)
    {
        CloudDataList *cloudDataList;

        dataRepositoryIndex = dataRepository.GetIndexFromKey(key, dataRepositoryExists);
        if (*dataRepositoryExists==false)
        {
            cloudDataList = N_new CloudDataList();
            cloudDataList->key=key;
            cloudDataList->uploaderCount=0;
            cloudDataList->subscriberCount=0;
            dataRepository.InsertAtIndex(cloudDataList,dataRepositoryIndex);
        }
        else
        {
            cloudDataList = dataRepository[dataRepositoryIndex];
        }

        return cloudDataList;
    }
    //------------------------------------------------------------------------
    void CloudServer::UnsubscribeFromKey(RemoteCloudClient *remoteCloudClient, UniqueID remoteCloudClientGuid, Nui32 keySubscriberIndex, CloudKey &cloudKey, DataStructures::List<UniqueID> &specificSystems)
    {
        KeySubscriberID* keySubscriberId = remoteCloudClient->subscribedKeys[keySubscriberIndex];

        // If removing specific systems, but global subscription, fail
        if (keySubscriberId->specificSystemsSubscribedTo.Size()==0 && specificSystems.Size()>0)
            return;

        bool dataRepositoryExists;
        CloudDataList *cloudDataList;
        Nui32 dataRepositoryIndex = dataRepository.GetIndexFromKey(cloudKey, &dataRepositoryExists);
        if (dataRepositoryExists==false)
            return;

        Nui32 i,j;

        cloudDataList = dataRepository[dataRepositoryIndex];
        if (specificSystems.Size()==0)
        {
            // Remove global subscriber. If returns false, have to remove specific subscribers
            if (cloudDataList->RemoveSubscriber(remoteCloudClientGuid)==false)
            {
                for (i=0; i < keySubscriberId->specificSystemsSubscribedTo.Size(); i++)
                {
                    RemoveSpecificSubscriber(keySubscriberId->specificSystemsSubscribedTo[i], cloudDataList, remoteCloudClientGuid);
                }
            }
            keySubscriberId->specificSystemsSubscribedTo.Clear(true);
        }
        else
        {
            for (j=0; j < specificSystems.Size(); j++)
            {
                Nui32 specificSystemsSubscribedToIndex;
                bool hasSpecificSystemsSubscribedTo;
                specificSystemsSubscribedToIndex=keySubscriberId->specificSystemsSubscribedTo.GetIndexFromKey(specificSystems[j], &hasSpecificSystemsSubscribedTo);
                if (hasSpecificSystemsSubscribedTo)
                {
                    RemoveSpecificSubscriber(specificSystems[j], cloudDataList, remoteCloudClientGuid);
                    keySubscriberId->specificSystemsSubscribedTo.RemoveAtIndex(specificSystemsSubscribedToIndex);
                }
            }
        }

        if (keySubscriberId->specificSystemsSubscribedTo.Size()==0)
        {
            N_delete keySubscriberId;
            remoteCloudClient->subscribedKeys.RemoveAtIndex(keySubscriberIndex);
        }

        if (cloudDataList->subscriberCount==0)
            RemoveSubscribedKeyFromServers(cloudKey);

        if (cloudDataList->IsUnused())
        {
            N_delete cloudDataList;
            dataRepository.RemoveAtIndex(dataRepositoryIndex);
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveSpecificSubscriber(UniqueID specificSubscriber,
        CloudDataList *cloudDataList, UniqueID remoteCloudClientGuid)
    {
        bool keyDataListExists;
        Nui32 keyDataListIndex = cloudDataList->keyData.GetIndexFromKey(specificSubscriber, &keyDataListExists);
        if (keyDataListExists==false)
            return;
        CloudData *cloudData = cloudDataList->keyData[keyDataListIndex];
        bool hasSpecificSubscriber;
        Nui32 specificSubscriberIndex = cloudData->specificSubscribers.GetIndexFromKey(remoteCloudClientGuid, &hasSpecificSubscriber);
        if (hasSpecificSubscriber)
        {
            cloudData->specificSubscribers.RemoveAtIndex(specificSubscriberIndex);
            cloudDataList->subscriberCount--;

            if (cloudData->IsUnused())
            {
                N_delete cloudData;
                cloudDataList->keyData.RemoveAtIndex(keyDataListIndex);
            }
        }
    }
    //------------------------------------------------------------------------
    void CloudServer::ForceExternalSystemAddress(Address * forcedAddress)
    {
        forceAddress=forcedAddress;
    }
    //------------------------------------------------------------------------
    void CloudServer::AddQueryFilter(CloudServerQueryFilter* filter)
    {
        if (queryFilters.GetIndexOf(filter)!=(Nui32) -1)
            return;
        queryFilters.Push(filter);
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveQueryFilter(CloudServerQueryFilter* filter)
    {
        Nui32 index;
        index = queryFilters.GetIndexOf(filter);
        if (index != (Nui32) -1)
            queryFilters.RemoveAtIndex(index);
    }
    //------------------------------------------------------------------------
    void CloudServer::RemoveAllQueryFilters(void)
    {
        queryFilters.Clear(true);
    }
    //------------------------------------------------------------------------
}
}
#endif
