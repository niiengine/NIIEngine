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
#if _RAKNET_SUPPORT_LogCommandParser == 1 && _RAKNET_SUPPORT_PacketLogger == 1
#include "PacketConsoleLogger.h"
#include "NiiNetLogCmd.h"
#include <stdio.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PacketConsoleLogger::PacketConsoleLogger()
    {
        logCommandParser=0;
    }
    //------------------------------------------------------------------------
    void PacketConsoleLogger::SetLogCommandParser(LogCmd *lcp)
    {
        logCommandParser=lcp;
        if (logCommandParser)
            logCommandParser->AddChannel("PacketConsoleLogger");
    }
    //------------------------------------------------------------------------
    void PacketConsoleLogger::WriteLog(const String & str)
    {
        if (logCommandParser)
            logCommandParser->WriteLog("PacketConsoleLogger", str);
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
