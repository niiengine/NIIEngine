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
#include "NiiNetBSDType.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    bool BSDSocketParam::IsPortInUse(Nui16 port, const char * host, Nui16 family, int type)
    {
        BSDSocketParam bbp;
        bbp.port = port;
        bbp.hostAddress = host;
        bbp.addressFamily = family;
        bbp.type = type;
        bbp.protocol = 0;
        bbp.mBlock = true;
        bbp.mBroadcast = false;
        bbp.doNotFragment = false;
        bbp.mIPHdrIncl = false;
        RNS2_Berkley * rns2;
        N_Only(Net).alloc(rns2);
        RNS2BindResult bindResult = rns2->Bind(&bbp);
        N_delete rns2;
        return bindResult == BR_FAILED_TO_BIND_SOCKET;
    }
    //------------------------------------------------------------------------
}
}