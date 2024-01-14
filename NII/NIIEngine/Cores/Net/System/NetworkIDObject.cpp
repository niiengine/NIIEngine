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

#include "NetworkIDObject.h"
#include "NetworkIDManager.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    NetworkIDObject::NetworkIDObject()
    {
        networkID = UNASSIGNED_NETWORK_ID;
        parent = 0;
        networkIDManager = 0;
        nextInstanceForNetworkIDManager = 0;
    }
    //------------------------------------------------------------------------
    NetworkIDObject::~NetworkIDObject()
    {
        if(networkIDManager)
            networkIDManager->StopTrackingNetworkIDObject(this);
    }
    //------------------------------------------------------------------------
    void NetworkIDObject::SetNetworkIDManager(NetworkIDManager * manager)
    {
        if (manager == networkIDManager)
            return;

        if (networkIDManager)
            networkIDManager->StopTrackingNetworkIDObject(this);

        networkIDManager = manager;
        if (networkIDManager == 0)
        {
            networkID = UNASSIGNED_NETWORK_ID;
            return;
        }
        
        if (networkID == UNASSIGNED_NETWORK_ID)
        {
            // Prior ID not set
            networkID = networkIDManager->GetNewNetworkID();
        }

        networkIDManager->TrackNetworkIDObject(this);
    }
    //------------------------------------------------------------------------
    NetworkIDManager * NetworkIDObject::GetNetworkIDManager(void) const
    {
        return networkIDManager;
    }
    //------------------------------------------------------------------------
    NetworkID NetworkIDObject::GetNetworkID(void)
    {
        return networkID;
    }
    //------------------------------------------------------------------------
    void NetworkIDObject::SetNetworkID(NetworkID id)
    {
        if (networkID== id)
            return;

        if ( id == UNASSIGNED_NETWORK_ID )
        {
            SetNetworkIDManager(0);
            return;
        }

        if ( networkIDManager )
            networkIDManager->StopTrackingNetworkIDObject(this);

        networkID = id;

        if (networkIDManager)
            networkIDManager->TrackNetworkIDObject(this);
    }
    //------------------------------------------------------------------------
    void NetworkIDObject::SetParent(void * _parent)
    {
        parent=_parent;
    }
    //------------------------------------------------------------------------
    void * NetworkIDObject::GetParent(void) const
    {
        return parent;
    }
    //------------------------------------------------------------------------
}
}