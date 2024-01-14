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

#include "NiiPreProcess.h"
#include "NetworkIDManager.h"
#include "NetworkIDObject.h"
#include "NiiNetLinkBase.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    NetworkIDManager::NetworkIDManager()
    {
        startingOffset = UniqueID::genLocalGuid();
        Clear();
    }
    //------------------------------------------------------------------------
    NetworkIDManager::~NetworkIDManager(void)
    {

    }
    //------------------------------------------------------------------------
    void NetworkIDManager::Clear(void)
    {
        memset(networkIdHash, 0, sizeof(networkIdHash));
    }
    //------------------------------------------------------------------------
    NetworkIDObject * NetworkIDManager::GET_BASE_OBJECT_FROM_ID(NetworkID x)
    {
        Nui32 hashIndex = NetworkIDToHashIndex(x);
        NetworkIDObject * nio = networkIdHash[hashIndex];
        while (nio)
        {
            if (nio->GetNetworkID() == x)
                return nio;
            nio = nio->nextInstanceForNetworkIDManager;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NetworkID NetworkIDManager::GetNewNetworkID(void)
    {
        while (GET_BASE_OBJECT_FROM_ID(++startingOffset))
            ;
        if (startingOffset == UNASSIGNED_NETWORK_ID)
        {
            while (GET_BASE_OBJECT_FROM_ID(++startingOffset))
                ;
        }
        return startingOffset;
    }
    //------------------------------------------------------------------------
    Nui32 NetworkIDManager::NetworkIDToHashIndex(NetworkID networkId)
    {
        return (Nui32) (networkId % NETWORK_ID_MANAGER_HASH_LENGTH);
    }
    //------------------------------------------------------------------------
    void NetworkIDManager::TrackNetworkIDObject(NetworkIDObject *networkIdObject)
    {
        N_assert(networkIdObject->GetNetworkIDManager()==this);
        NetworkID rawId = networkIdObject->GetNetworkID();
        N_assert(rawId != UNASSIGNED_NETWORK_ID);

        networkIdObject->nextInstanceForNetworkIDManager=0;

        Nui32 hashIndex = NetworkIDToHashIndex(rawId);
        if (networkIdHash[hashIndex] == 0)
        {
            networkIdHash[hashIndex] = networkIdObject;
            return;
        }
        NetworkIDObject * nio = networkIdHash[hashIndex];
        // Duplicate insertion?
        N_assert(nio != networkIdObject);
        // Random UniqueID conflict?
        N_assert(nio->GetNetworkID() != rawId);

        while (nio->nextInstanceForNetworkIDManager!=0)
        {
            nio = nio->nextInstanceForNetworkIDManager;

            // Duplicate insertion?
            N_assert(nio!=networkIdObject);
            // Random UniqueID conflict?
            N_assert(nio->GetNetworkID()!=rawId);
        }
        nio->nextInstanceForNetworkIDManager = networkIdObject;
    }
    //------------------------------------------------------------------------
    void NetworkIDManager::StopTrackingNetworkIDObject(NetworkIDObject *networkIdObject)
    {
        N_assert(networkIdObject->GetNetworkIDManager()==this);
        NetworkID rawId = networkIdObject->GetNetworkID();
        N_assert(rawId!=UNASSIGNED_NETWORK_ID);

        Nui32 hashIndex = NetworkIDToHashIndex(rawId);

        NetworkIDObject *nio=networkIdHash[hashIndex];
        if (nio==0)
        {
            N_assert("NetworkIDManager::StopTrackingNetworkIDObject didn't find object" && 0);
            return;
        }
        if (nio == networkIdObject)
        {
            networkIdHash[hashIndex] = nio->nextInstanceForNetworkIDManager;
            return;
        }

        while (nio)
        {
            if (nio->nextInstanceForNetworkIDManager==networkIdObject)
            {
                nio->nextInstanceForNetworkIDManager=networkIdObject->nextInstanceForNetworkIDManager;
                return;
            }
            nio=nio->nextInstanceForNetworkIDManager;
        }

        N_assert("NetworkIDManager::StopTrackingNetworkIDObject didn't find object" && 0);
    }
    //------------------------------------------------------------------------
}
}