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

#ifndef _NII_NET_NETWORK_ID_MANAGER_H_
#define _NII_NET_NETWORK_ID_MANAGER_H_

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "NetworkIDObject.h"
#include "Rand.h"

namespace NII
{
namespace NII_NET
{
    #define NETWORK_ID_MANAGER_HASH_LENGTH 1024

    /// This class is simply used to generate a unique number for a group of instances of NetworkIDObject
    /// An instance of this class is required to use the ObjectID to pointer lookup system
    /// You should have one instance of this class per game instance.
    /// Call SetIsNetworkIDAuthority before using any functions of this class, or of NetworkIDObject
    class _EngineAPI NetworkIDManager
    {
    public:
        NetworkIDManager();
        virtual ~NetworkIDManager();

        /// Returns the parent object, or this instance if you don't use a parent.
        /// Supports NetworkIDObject anywhere in the inheritance hierarchy
        /// \pre You must first call SetNetworkIDManager before using this function
        template <class returnType>
        returnType GET_OBJECT_FROM_ID(NetworkID x) 
        {
            NetworkIDObject *nio = GET_BASE_OBJECT_FROM_ID(x);
            if (nio == 0)
                return 0;
            if (nio->GetParent())
                return (returnType) nio->GetParent();
            return (returnType) nio;
        }

        void Clear(void);

        NetworkIDObject * GET_BASE_OBJECT_FROM_ID(NetworkID x);
    protected:
        /// \internal
        void TrackNetworkIDObject(NetworkIDObject *networkIdObject);
        void StopTrackingNetworkIDObject(NetworkIDObject *networkIdObject);

        friend class NetworkIDObject;

        NetworkIDObject * networkIdHash[NETWORK_ID_MANAGER_HASH_LENGTH];
        Nui32 NetworkIDToHashIndex(NetworkID networkId);
        Nui64 startingOffset;
        /// \internal
        NetworkID GetNewNetworkID(void);
    };
}
}
#endif