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

#ifndef _NII_NET_BSD_TYPE_H_
#define _NII_NET_BSD_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_NET
{
    class BSDSocketParam : public NetAlloc
    {
    public:
        // For addressFamily, use AF_INET
        // For type, use SOCK_DGRAM
        static bool IsPortInUse(Nui16 port, const char * host, Nui16 family, int type);
    public:
        Nui16 port;
        Nui16 addressFamily;// AF_INET or AF_INET6
        char * hostAddress;
        int type;           // SOCK_DGRAM
        int protocol;       // 0
        bool mBlock;
        bool mBroadcast;
        bool mIPHdrIncl;
        int doNotFragment;
        int pollingThreadPriority;
        RNS2EventHandler * eventHandler;
    };
}
}
#endif