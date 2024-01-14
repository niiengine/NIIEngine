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

#ifndef __PACKET_OUTPUT_WINDOW_LOGGER_H_
#define __PACKET_OUTPUT_WINDOW_LOGGER_H_

#if _RAKNET_SUPPORT_PacketLogger==1
#include "NativeFeatureIncludes.h"
#include "PacketLogger.h"
namespace NII
{
namespace NII_NET
{
    /// \ingroup PACKETLOGGER_GROUP
    /// \brief Packetlogger that outputs to the output window in the debugger. Windows only.
    class _EngineAPI  PacketOutputWindowLogger : public PacketLogger
    {
    public:
        PacketOutputWindowLogger();
        virtual ~PacketOutputWindowLogger();
        virtual void WriteLog(const String & str);
    protected:
    };
}
}
#endif

#endif // _RAKNET_SUPPORT_*
