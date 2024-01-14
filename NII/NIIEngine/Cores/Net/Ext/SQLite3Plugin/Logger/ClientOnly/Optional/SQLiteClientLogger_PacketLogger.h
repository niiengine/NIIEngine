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

#ifndef __SQL_LITE_CLIENT_LOGGER_PACKET_LOGGER_H_
#define __SQL_LITE_CLIENT_LOGGER_PACKET_LOGGER_H_

#include "PacketLogger.h"

namespace NII
{
namespace NII_NET
{
    /// \ingroup PACKETLOGGER_GROUP
    /// \brief Packetlogger that outputs to a file
    class RAK_DLL_EXPORT  SQLiteClientLogger_PacketLogger : public PacketLogger
    {
    public:
        SQLiteClientLogger_PacketLogger();
        virtual ~SQLiteClientLogger_PacketLogger();

        //virtual void onRawSend(const Nui8 * data, NBitCount count, Address * remote);
        //virtual void onRawReceive(const Nui8 * data, NBitCount count, Address * remote);
        //virtual void onReceiveLocal(const Nui8 * data, NBitCount count, Address * remote);
        //virtual void onReceivePacket(Packet * packet, unsigned frameNumber, Address * remote, TimeDurMS time, bool isSend);
        //virtual void onReceiveAck(NCount messageNumber, Address * remote, TimeDurMS time);
        virtual void WriteMiscellaneous(const char * type, const char * msg);
    protected:
        virtual void WriteLog(const char * str) {}
    };
}
}
#endif