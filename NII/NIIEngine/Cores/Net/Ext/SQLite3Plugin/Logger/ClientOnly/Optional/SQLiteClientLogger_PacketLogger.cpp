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
#include "SQLiteClientLogger_PacketLogger.h"
#include "SQLiteClientLoggerPlugin.h"
#include "NiiNetLinkBase.h"
#include "NiiNetPacketPrc.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    static const char * DEFAULT_PACKET_LOGGER_TABLE = "PacketLogger";
    //------------------------------------------------------------------------
    SQLiteClientLogger_PacketLogger::SQLiteClientLogger_PacketLogger()
    {
    }
    //------------------------------------------------------------------------
    SQLiteClientLogger_PacketLogger::~SQLiteClientLogger_PacketLogger()
    {
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::onRawSend(const Nui8 * data, NBitCount count, Address * remote)
    {
        String str1, str2, str3, str4;
        mPrc2->GetExternalID(remote)->read(str1, true);
        mPrc2->getGUID(0).read(str2);
        mPrc2->getGUID(remote)->read(str4);
        remote->read(str3, true);

        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            ("Snd", "Raw",0,0,IDTOString(data[0]), count, str1, str2, str3, str4, "","","","","") );
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::onRawReceive(const Nui8 * data, NBitCount count, Address * remote)
    {
        String str1, str2, str3, str4;
        mPrc2->GetExternalID(remote)->read(str1, true);
        mPrc2->getGUID(0).read(str2);
        mPrc2->getGUID(remote)->read(str4);
        remote->read(str3, true);

        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            ("Rcv", "Raw", "", "", IDTOString(data[0]), count, str1, str2, str3, str4, "","","","","") );
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::onReceivePacket(Packet * packet, unsigned frameNumber, Address * remote, TimeDurMS time, bool isSend)
    {
        String str1, str2, str3, str4;
        mPrc2->GetExternalID(remote)->read(str1, true);
        mPrc2->getGUID(0).read(str2);
        mPrc2->getGUID(remote)->read(str4);
        remote->read(str3, true);

        unsigned char typeByte;
        char * typeStr;
        if (packet->data[0]==ID_TIMESTAMP && BITS_TO_BYTES(packet->dataBitLength)>sizeof(TimeDurMS)+1)
        {
            typeByte = packet->data[1+sizeof(TimeDurMS)];
            typeStr = "Timestamp";
        }
        else
        {
            typeByte = packet->data[0];
            typeStr = "Normal";
        }

        const char * sendType = (isSend) ? "Snd" : "Rcv";

        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            (sendType, typeStr, packet->reliableMessageNumber, frameNumber, IDTOString(typeByte), packet->dataBitLength, str1, str2, str3, str4, packet->splitPacketId, packet->splitPacketIndex, packet->splitPacketCount, packet->orderingIndex,"") );
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::onReceiveAck(NCount messageNumber, Address * remote, TimeDurMS time)
    {
        String str1, str2, str3, str4;
        mPrc2->GetExternalID(remote)->read(str1, true);
        mPrc2->getGUID(0).read(str2);
        mPrc2->getGUID(remote)->read(str4);
        remote->read(str3, true);

        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            ("Rcv", "Ack", messageNumber, "", "", "", str1, str2, str3, str4, "","","","","") );
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::onReceiveLocal(const Nui8 * data, NBitCount count, Address * remote)
    {
        String str1, str2, str3, str4;
        mPrc2->GetExternalID(remote)->read(str1, true);
        mPrc2->getGUID(0).read(str2);
        mPrc2->getGUID(remote)->read(str4);
        remote->read(str3, true);

        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            ("Local", "addMessage","",  "", IDTOString(data[0]), count, str1, str2, str3, str4, "","","","","") );
    }
    //------------------------------------------------------------------------
    void SQLiteClientLogger_PacketLogger::WriteMiscellaneous(const char * type, const char * msg)
    {
        rakSqlLog(DEFAULT_PACKET_LOGGER_TABLE, "SndRcv,Type,PacketNumber,FrameNumber,PacketID,BitLength,LocalIP,LocalGuid,RemoteIP,RemoteGuid,splitPacketId,SplitPacketIndex,splitPacketCount,orderingIndex,misc", \
            ("Local", type,"",  "", "", "", "", "", "","","","","","",msg) );
    }
    //------------------------------------------------------------------------
}
}