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
#if _RAKNET_SUPPORT_PacketLogger == 1

#ifndef __PACKET_FILE_LOGGER_H_
#define __PACKET_FILE_LOGGER_H_

#include "PacketLogger.h"
#include <stdio.h>

namespace NII
{
namespace NII_NET
{
    /// \ingroup PACKETLOGGER_GROUP
    /// \brief Packetlogger that outputs to a file
    class _EngineAPI  PacketFileLogger : public PacketLogger
    {
    public:
        PacketFileLogger();
        virtual ~PacketFileLogger();
        void StartLog(const String & filenamePrefix);
        virtual void WriteLog(const String & str);
    protected:
        FILE * packetLogFile;
    };

}
}
#endif
#endif