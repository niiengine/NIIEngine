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
#include "ThreadsafePacketLogger.h"
#include <string.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    ThreadsafePacketLogger::ThreadsafePacketLogger()
    {

    }
    //------------------------------------------------------------------------
    ThreadsafePacketLogger::~ThreadsafePacketLogger()
    {
        char **msg;
        while ((msg = logMessages.ReadLock()) != 0)
        {
            N_free(*msg);
        }
    }
    //------------------------------------------------------------------------
    void ThreadsafePacketLogger::update()
    {
        char **msg;
        while ((msg = logMessages.ReadLock()) != 0)
        {
            WriteLog(*msg);
            N_free(*msg);
        }
    }
    //------------------------------------------------------------------------
    void ThreadsafePacketLogger::AddToLog(const String & str)
    {
        char **msg = logMessages.WriteLock();
        *msg = (char *)N_alloc(str.length() + 1);
        strcpy(*msg, str.c_str());
        logMessages.WriteUnlock();
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
