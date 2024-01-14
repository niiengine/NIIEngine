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

#ifndef __CLOUD_SERVER_H
#define __CLOUD_SERVER_H

#include "NiiNetPreInclude.h"
#include "NiiNetPlugin.h"
#include "RakString.h"
#include "DS_Hash.h"
#include "CloudCommon.h"
#include "DS_OrderedList.h"

/// If the data is smaller than this value, an allocation is avoid. However, this value exists for every row
#define CLOUD_SERVER_DATA_STACK_SIZE 32

namespace NII
{
namespace NII_NET
{
    /// \brief Zero or more instances of CloudServerQueryFilter can be attached to CloudServer to restrict client queries
    /// All attached instances of CloudServerQueryFilter on each corresponding operation, from all directly connected clients
    /// If any attached instance returns false for a given operation, that operation is silently rejected
    /// \ingroup CLOUD_GROUP
    class _EngineAPI CloudServerQueryFilter
    {
    public:
        CloudServerQueryFilter() {}
        virtual ~CloudServerQueryFilter() {}

        /// Called when a local client wants to post data
        /// \return true to allow, false to reject
        virtual bool OnPostRequest(UniqueID id, Address * address, CloudKey key, Nui32 dataLength, const Nui8 * data)=0;

        /// Called when a local client wants to release data that it has previously uploaded
        /// \return true to allow, false to reject
        virtual bool OnReleaseRequest(UniqueID id, Address * address, DataStructures::List<CloudKey> &cloudKeys)=0;

        /// Called when a local client wants to query data
        /// If you return false, the client will get no response at all
        /// \return true to allow, false to reject
        virtual bool OnGetRequest(UniqueID id, Address * address, CloudQuery &query, DataStructures::List<UniqueID> &specificSystems)=0;

        /// Called when a local client wants to stop getting updates for data
        /// If you return false, the client will keep getting updates for that data
        /// \return true to allow, false to reject
        virtual bool OnUnsubscribeRequest(UniqueID id, Address * address, DataStructures::List<CloudKey> &cloudKeys, DataStructures::List<UniqueID> &specificSystems)=0;
    };

    /// \brief Stores client data, and allows cross-server communication to retrieve this data
    /// \ingroup CLOUD_GROUP
    class _EngineAPI CloudServer : public Plugin, CloudAllocator
    {
    public:
        CloudServer();
        virtual ~CloudServer();

        /// \brief Max bytes a client can upload
        /// Data in excess of this value is silently ignored
        /// defaults to 0 (unlimited)
        /// \param[in] bytes Max bytes a client can upload. 0 means unlimited.
        void SetMaxUploadBytesPerClient(Nui64 bytes);

        /// \brief Max bytes returned by a download. If the number of bytes would exceed this amount, the returned list is truncated
        /// However, if this would result in no rows downloaded, then one row will be returned.
        /// \param[in] bytes Max bytes a client can download from a single Get(). 0 means unlimited.
        void SetMaxBytesPerDownload(Nui64 bytes);

        /// \brief Add a server, which is assumed to be connected in a fully connected mesh to all other servers and also running the CloudServer plugin
        /// The other system must also call AddServer before getting the subscription data, or it will be rejected.
        /// Sending a message telling the other system to call AddServer(), followed by calling AddServer() locally, would be sufficient for this to work.
        /// \note This sends subscription data to the other system, using PT_ORDER_MAKE_SURE on channel 0
        /// \param[in] id Identifier of the remote system
        void AddServer(UniqueID id);

        /// \brief Removes a server added through AddServer()
        /// \param[in] id Identifier of the remote system
        void RemoveServer(UniqueID id);

        /// Return list of servers added with AddServer()
        /// \param[out] remoteServers List of servers added
        void GetRemoteServers(DataStructures::List<UniqueID> &remoteServersOut);

        /// \brief Frees all memory. Does not remove query filters
        void Clear(void);

        /// \brief Report the specified Address to client queries, rather than what NetworkSys reads.
        /// This is useful if you already know your public IP
        /// This only applies to future updates, so call it before updating to apply to all queries
        /// \param[in] forcedAddress The systmeAddress to return in queries. Use Address::invalid (default) to use what NetworkSys returns
        void ForceExternalSystemAddress(Address * forcedAddress);

        /// \brief Adds a callback called on each query. If all filters returns true for an operation, the operation is allowed.
        /// If the filter was already added, the function silently fails
        /// \param[in] filter An externally allocated instance of CloudServerQueryFilter. The instance must remain valid until it is removed with RemoveQueryFilter() or RemoveAllQueryFilters()
        void AddQueryFilter(CloudServerQueryFilter* filter);

        /// \brief Removes a callback added with AddQueryFilter()
        /// The instance is not deleted, only unreferenced. It is up to the user to delete the instance, if necessary
        /// \param[in] filter An externally allocated instance of CloudServerQueryFilter. The instance must remain valid until it is removed with RemoveQueryFilter() or RemoveAllQueryFilters()
        void RemoveQueryFilter(CloudServerQueryFilter* filter);

        /// \brief Removes all instances of CloudServerQueryFilter added with AddQueryFilter().
        /// The instances are not deleted, only unreferenced. It is up to the user to delete the instances, if necessary
        void RemoveAllQueryFilters(void);
    protected:
        virtual void update();
        virtual ConnectReceiveType onMessage(SocketMessage * msg);
        virtual void onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason );
        virtual void shutdown();
        virtual void OnPostRequest(SocketMessage * msg);
        virtual void OnReleaseRequest(SocketMessage * msg);
        virtual void OnGetRequest(SocketMessage * msg);
        virtual void OnUnsubscribeRequest(SocketMessage * msg);
        virtual void OnServerToServerGetRequest(SocketMessage * msg);
        virtual void OnServerToServerGetResponse(SocketMessage * msg);

        Nui64 maxUploadBytesPerClient, maxBytesPerDowload;

        // ----------------------------------------------------------------------------
        // For a given data key, quickly look up one or all systems that have uploaded
        // ----------------------------------------------------------------------------
        struct CloudData
        {
            CloudData() {}
            ~CloudData() {if (allocatedData) N_free(allocatedData);}
            bool IsUnused(void) const {return isUploaded==false && specificSubscribers.Size()==0;}
            void Clear(void) {if (dataPtr==allocatedData) N_free(allocatedData); allocatedData=0; dataPtr=0; dataLengthBytes=0; isUploaded=false;}

            Nui8 mStackData[CLOUD_SERVER_DATA_STACK_SIZE];
            Nui8 *allocatedData; // Uses allocatedData instead of mStackData if length of data exceeds CLOUD_SERVER_DATA_STACK_SIZE
            Nui8 *dataPtr; // Points to either mStackData or allocatedData
            Nui32 dataLengthBytes;
            bool isUploaded;

            /// System address of server that is holding this data, and the client is connected to
            Address * serverSystemAddress;

            /// System address of client that uploaded this data
            Address * clientSystemAddress;

            /// UniqueID of server that is holding this data, and the client is connected to
            UniqueID serverGUID;

            /// UniqueID of client that uploaded this data
            UniqueID clientGUID;

            /// When the key data changes from this particular system, notify these subscribers
            /// This list mutually exclusive with CloudDataList::nonSpecificSubscribers
            DataStructures::OrderedList<UniqueID, UniqueID> specificSubscribers;
        };
        void WriteCloudQueryRowFromResultList(Nui32 i, DataStructures::List<CloudData*> &cloudDataResultList, DataStructures::List<CloudKey> &cloudKeyResultList, NetSerializer *bsOut);
        void WriteCloudQueryRowFromResultList(DataStructures::List<CloudData*> &cloudDataResultList, DataStructures::List<CloudKey> &cloudKeyResultList, NetSerializer *bsOut);

        static int KeyDataPtrComp( const UniqueID &key, CloudData* const &data );
        struct CloudDataList
        {
            bool IsUnused(void) const {return keyData.Size()==0 && nonSpecificSubscribers.Size()==0;}
            bool IsNotUploaded(void) const {return uploaderCount==0;}
            bool RemoveSubscriber(UniqueID g) {
                bool objectExists;
                Nui32 index;
                index = nonSpecificSubscribers.GetIndexFromKey(g, &objectExists);
                if (objectExists)
                {
                    subscriberCount--;
                    nonSpecificSubscribers.RemoveAtIndex(index);
                    return true;
                }
                return false;
            }

            Nui32 uploaderCount, subscriberCount;
            CloudKey key;

            // Data uploaded from or subscribed to for various systems
            DataStructures::OrderedList<UniqueID, CloudData*, CloudServer::KeyDataPtrComp> keyData;

            /// When the key data changes from any system, notify these subscribers
            /// This list mutually exclusive with CloudData::specificSubscribers
            DataStructures::OrderedList<UniqueID, UniqueID> nonSpecificSubscribers;
        };

        static int KeyDataListComp( const CloudKey &key, CloudDataList * const &data );
        DataStructures::OrderedList<CloudKey, CloudDataList*, CloudServer::KeyDataListComp> dataRepository;

        struct KeySubscriberID
        {
            CloudKey key;
            DataStructures::OrderedList<UniqueID, UniqueID> specificSystemsSubscribedTo;
        };
        static int KeySubscriberIDComp(const CloudKey &key, KeySubscriberID * const &data );

        // Remote systems
        struct RemoteCloudClient
        {
            bool IsUnused(void) const {return uploadedKeys.Size()==0 && subscribedKeys.Size()==0;}

            DataStructures::OrderedList<CloudKey,CloudKey,CloudKeyComp> uploadedKeys;
            DataStructures::OrderedList<CloudKey,KeySubscriberID*,CloudServer::KeySubscriberIDComp> subscribedKeys;
            Nui64 uploadedBytes;
        };
        DataStructures::Hash<UniqueID, RemoteCloudClient*, 2048, UniqueID::toNum> mRemotes;

        // For a given user, release all subscribed and uploaded keys
        void ReleaseSystem(UniqueID clientAddress );

        // For a given user, release a set of keys
        void ReleaseKeys(UniqueID clientAddress, DataStructures::List<CloudKey> &keys );

        void NotifyClientSubscribersOfDataChange( CloudData *cloudData, CloudKey &key, DataStructures::OrderedList<UniqueID, UniqueID> &subscribers, bool wasUpdated );
        void NotifyClientSubscribersOfDataChange( CloudQueryRow *row, DataStructures::OrderedList<UniqueID, UniqueID> &subscribers, bool wasUpdated );
        void NotifyServerSubscribersOfDataChange( CloudData *cloudData, CloudKey &key, bool wasUpdated );

        struct RemoteServer
        {
            UniqueID serverAddress;
            // This server needs to know about these keys when they are updated or deleted
            DataStructures::OrderedList<CloudKey,CloudKey,CloudKeyComp> subscribedKeys;
            // This server has uploaded these keys, and needs to know about Get() requests
            DataStructures::OrderedList<CloudKey,CloudKey,CloudKeyComp> uploadedKeys;

            // Just for processing
            bool workingFlag;

            // If false, we don't know what keys they have yet, so send everything
            bool gotSubscribedAndUploadedKeys;
        };

        static int RemoteServerComp(const UniqueID &key, RemoteServer* const &data );
        DataStructures::OrderedList<UniqueID, RemoteServer*, CloudServer::RemoteServerComp> remoteServers;

        struct BufferedGetResponseFromServer
        {
            void Clear(CloudAllocator *allocator);

            UniqueID serverAddress;
            CloudQueryResult queryResult;
            bool gotResult;
        };

        struct CloudQueryWithAddresses
        {
            // Inputs
            CloudQuery cloudQuery;
            DataStructures::List<UniqueID> specificSystems;

            void write(NetSerializer * in);
            void read(NetSerializer * out);
        };

        static int BufferedGetResponseFromServerComp(const UniqueID & key,
            BufferedGetResponseFromServer * const & data);
        struct GetRequest
        {
            void Clear(CloudAllocator * allocator);
            bool AllRemoteServersHaveResponded() const;
            CloudQueryWithAddresses cloudQueryWithAddresses;

            // When request started. If takes too long for a response from another system, can abort remaining systems
            TimeDurMS requestStartTime;

            // Assigned by server that gets the request to identify response. See nextGetRequestId
            Nui32 requestId;

            UniqueID requestingClient;

            DataStructures::OrderedList<UniqueID, BufferedGetResponseFromServer*, CloudServer::BufferedGetResponseFromServerComp> remoteServerResponses;
        };
        static int GetRequestComp(const Nui32 &key, GetRequest* const &data );
        DataStructures::OrderedList<Nui32, GetRequest*, CloudServer::GetRequestComp> getRequests;
        TimeDurMS nextGetRequestsCheck;

        Nui32 nextGetRequestId;

        void ProcessAndTransmitGetRequest(GetRequest *getRequest);

        void ProcessCloudQueryWithAddresses(
            CloudServer::CloudQueryWithAddresses &cloudQueryWithAddresses,
            DataStructures::List<CloudData*> &cloudDataResultList,
            DataStructures::List<CloudKey> &cloudKeyResultList
            );

        void SendUploadedAndSubscribedKeysToServer(UniqueID id);
        void SendUploadedKeyToServers(CloudKey & cloudKey);
        void SendSubscribedKeyToServers(CloudKey & cloudKey);
        void RemoveUploadedKeyFromServers(CloudKey & cloudKey);
        void RemoveSubscribedKeyFromServers(CloudKey & cloudKey);

        void OnSendUploadedAndSubscribedKeysToServer(SocketMessage * msg);
        void OnSendUploadedKeyToServers(SocketMessage * msg);
        void OnSendSubscribedKeyToServers(SocketMessage * msg);
        void OnRemoveUploadedKeyFromServers(SocketMessage * msg);
        void OnRemoveSubscribedKeyFromServers(SocketMessage * msg);
        void OnServerDataChanged(SocketMessage * msg);

        void GetServersWithUploadedKeys(
            DataStructures::List<CloudKey> & keys,
            DataStructures::List<RemoteServer *> & remoteServersWithData);

        CloudServer::CloudDataList *GetOrAllocateCloudDataList(CloudKey key, bool *dataRepositoryExists, Nui32 &dataRepositoryIndex);

        void UnsubscribeFromKey(RemoteCloudClient *remoteCloudClient, UniqueID remoteCloudClientGuid, Nui32 keySubscriberIndex, CloudKey &cloudKey, DataStructures::List<UniqueID> &specificSystems);
        void RemoveSpecificSubscriber(UniqueID specificSubscriber, CloudDataList *cloudDataList, UniqueID remoteCloudClientGuid);

        DataStructures::List<CloudServerQueryFilter*> queryFilters;

        Address * forceAddress;
    };
}
}
#endif


// Key subscription
//
// A given system can subscribe to one or more keys.
// The subscription can be further be defined as only subscribing to keys uploaded by or changed by a given system.
// It is possible to subscribe to keys not yet uploaded, or uploaded to another system
//
// Operations:
//
// 1. SubscribeToKey() - Get() operation with subscription
// A. Add to key subscription list for the client, which contains a keyId / specificUploaderList pair
// B. send to remote servers that for this key, they should send us updates
// C. (Done, get operation returns current values)
//
// 2. UpdateData() - Post() operation
// A. Find all subscribers to this data, for the uploading system.
// B. send them the uploaded data
// C. Find all servers that subscribe to this data
// D. send them the uploaded data
//
// 3. DeleteData() - Release() operation
// A. Find all subscribers to this data, for the deleting system.
// B. Inform them of the deletion
// C. Find all servers that subscribe to this data
// D. Inform them of the deletion
//
// 4. Unsubscribe()
// A. Find this subscriber, and remove their subscription
// B. If no one else is subscribing to this key for any system, notify remote servers we no longer need subscription updates
//
// Internal operations:
//
// 1. Find if any connected client has subscribed to a given key
// A. This is used add and remove our subscription for this key to remote servers
//
// 2. For a given key and updating address, find all connected clients that care
// A. First find connected clients that have subscribed to this key, regardless of address
// B. Then find connected clients that have subscribed to this key for this particular address
//
// 3. Find all remote servers that have subscribed to a given key
// A. This is so when the key is updated or deleted, we know who to send it to
//
// 4. For a given client (such as on disconnect), remove all records of their subscriptions

#endif // _RAKNET_SUPPORT_*
