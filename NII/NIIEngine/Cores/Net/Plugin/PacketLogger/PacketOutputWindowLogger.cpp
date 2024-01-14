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


#if _RAKNET_SUPPORT_PacketLogger==1
#include "NativeFeatureIncludes.h"

#if defined(UNICODE)
#include "RakWString.h"
#endif

#include "PacketOutputWindowLogger.h"
#include "RakString.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PacketOutputWindowLogger::PacketOutputWindowLogger()
    {
    }
    //------------------------------------------------------------------------
    PacketOutputWindowLogger::~PacketOutputWindowLogger()
    {
    }
    //------------------------------------------------------------------------
    void PacketOutputWindowLogger::WriteLog(const String & str)
    {
    #if N_PLAT == N_PLAT_WIN32
        #if defined(UNICODE)
            RakWString str2 = str;
            str2+="\n";
            OutputDebugString(str2.C_String());
        #else
            RakString str2 = str;
            str2+="\n";
            OutputDebugString(str2.C_String());
        #endif
    // DS_APR
    #elif defined(N_PLAT_NACL)
        fprintf(stderr, "%s\n", str);
    // /DS_APR
    #endif
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
