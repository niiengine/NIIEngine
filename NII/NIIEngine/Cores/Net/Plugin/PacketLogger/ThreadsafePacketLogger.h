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

#ifndef __THREADSAFE_PACKET_LOGGER_H
#define __THREADSAFE_PACKET_LOGGER_H

#if _RAKNET_SUPPORT_PacketLogger == 1
#include "NativeFeatureIncludes.h"

#include "PacketLogger.h"
#include "SingleProducerConsumer.h"

namespace NII
{
namespace NII_NET
{
    /// \ingroup PACKETLOGGER_GROUP
    /// \brief Same as PacketLogger, but writes output in the user thread.
    class _EngineAPI ThreadsafePacketLogger : public PacketLogger
    {
    public:
        ThreadsafePacketLogger();
        virtual ~ThreadsafePacketLogger();

        virtual void update(void);
    protected:
        virtual void AddToLog(const String & str);
	protected:
        DataStructures::SingleProducerConsumer<char*> logMessages;
    };
}
}
#endif

#endif