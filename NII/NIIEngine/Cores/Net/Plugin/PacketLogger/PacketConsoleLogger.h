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

#ifndef __PACKET_CONSOLE_LOGGER_H_
#define __PACKET_CONSOLE_LOGGER_H_

#include "PacketLogger.h"

namespace NII
{
namespace NII_NET
{
    /// \ingroup PACKETLOGGER_GROUP
    /// \brief Packetlogger that logs to a remote command console
    class _EngineAPI  PacketConsoleLogger : public PacketLogger
    {
    public:
        PacketConsoleLogger();
        // Writes to the command parser used for logging, which is accessed through a secondary communication layer (such as Telnet or RakNet) - See ConsoleServer.h
        virtual void SetLogCommandParser(LogCmd * lcp);
        virtual void WriteLog(const String & str);
    protected:
        LogCmd *logCommandParser;
    };
}
}
#endif
#endif