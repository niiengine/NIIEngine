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

#include "NiiPreProcess.h"
#include "PacketLogger.h"
#include "NiiNetSerializer.h"
#include "NiiNetPacketPrc.h"
#include "NiiNetLinkBase.h"
#include "NiiNetCommon.h"
#include "StringTable.h"
#include "NiiCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    PacketLogger::PacketLogger()
    {
        printId = true;
        printAcks = true;
        prefix[0] = 0;
        suffix[0] = 0;
        logDirectMessages = true;
    }
    //------------------------------------------------------------------------
    PacketLogger::~PacketLogger()
    {
    }
    //------------------------------------------------------------------------
    void PacketLogger::FormatLine(char * into, const char * dir, const char * type,
        Nui32 makesureMark, Nui32 frame, Nui8 id, NBitCount bitLen, Nui64 time,
            const Address & local, const Address & remote, Nui32 splitid,
                Nui32 splitindex, Nui32 splitcount, Nui32 order)
    {
        char numericID[16];
        const char * idToPrint = NULL;
        if(printId)
        {
            if (splitcount > 0 && splitcount != (Nui32)-1)
                idToPrint="(SPLIT PACKET)";
            else
                idToPrint =	IDTOString(id);
        }
        // If printId is false, idToPrint will be NULL, as it will
        // in the case of an unrecognized id. Testing printId for false
        // would just be redundant.
        if(idToPrint == NULL)
        {
            sprintf(numericID, "%5u", id);
            idToPrint = numericID;
        }

        FormatLine(into, dir, type, makesureMark, frame, idToPrint, bitLen,
            time, local, remote,splitid,splitindex,splitcount, order);
    }
    //------------------------------------------------------------------------
    void PacketLogger::FormatLine(char* into, const char* dir, const char* type,
        Nui32 makesureMark, Nui32 frame, const char* idToPrint,
            NBitCount bitLen, Nui64 time, const Address & local, const Address * remote,
                Nui32 splitid, Nui32 splitindex, Nui32 splitcount, Nui32 order)
    {
        String str1, str2;
        char localtime[128];
        char str3[64];
        local.read(str1, true);
        remote->read(str2, true);
        GetLocalTime(localtime);
        if (makesureMark==(Nui32)-1)
        {
            str3[0]='N';
            str3[1]='/';
            str3[2]='A';
            str3[3]=0;
        }
        else
        {
            sprintf(str3, "%5u", makesureMark);
        }

        sprintf(into, "%s,%s%s,%s,%s,%5u,%s,%u,%" N_64BIT_MOD "u,%s,%s,%i,%i,%i,%i,%s,",
            localtime, prefix, dir, type, str3, frame, idToPrint, bitLen, time,
                str1.c_str(), str2.c_str(), splitid, splitindex, splitcount, order, suffix);
    }
    //------------------------------------------------------------------------
    void PacketLogger::onRawSend(const Nui8 * data, NBitCount count, Address * remote)
    {
        if (logDirectMessages == false)
            return;

        char str[256];
        FormatLine(str, "Snd", "Raw", 0, 0, data[0], count, N_Engine().getTimer().getMS(),
            mPrc2->getConnectTo(remote), remote, (Nui32)-1, (Nui32)-1, (Nui32)-1, (Nui32)-1);
        AddToLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::onRawReceive(const Nui8 * data, NBitCount count, Address * remote)
    {
        if(logDirectMessages == false)
            return;

        char str[256];
        FormatLine(str, "Rcv", "Raw", 0, 0, data[0], count, N_Engine().getTimer().getMS(),
            mPrc2->getLAN(0, 0), remote, (Nui32)-1, (Nui32)-1, (Nui32)-1, (Nui32)-1);
        AddToLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::LogHeader(void)
    {
        // Last 5 are splitpacket id, split packet index, split packet count, ordering index, suffix
        AddToLog("Clock,S|R,Typ,Reliable#,Frm #,PktID,BitLn,Time     ,Local IP:Port   ,RemoteIP:Port,SPID,SPIN,SPCO,OI,Suffix,Miscellaneous\n");
    }
    //------------------------------------------------------------------------
    void PacketLogger::onReport(const char * errorMessage, NBitCount count, Address * remote, bool isError)
    {
        char str[1024];
        char * type;
        if (isError)
            type = (char*) "RcvErr";
        else
            type = (char*) "RcvWrn";
        FormatLine(str, type, errorMessage, 0, 0, "", count, N_Engine().getTimer().getMS(),
            mPrc2->getLAN(0, 0), remote,(Nui32)-1,(Nui32)-1, (Nui32)-1,(Nui32)-1);
        AddToLog(str);
        N_assert(isError == false);
    }
    //------------------------------------------------------------------------
    void PacketLogger::onReceiveAck(Nui32 ackmark, Address * remote, TimeDurMS time)
    {
        char str[256];
        char localtime[128];
        String str1, str2;
        mPrc2->getConnectTo(remote)->read(str1, true);
        remote.read(str2, true);
        GetLocalTime(localtime);

        sprintf(str, "%s,Rcv,Ack,%i,,,,%" N_64BIT_MOD "u,%s,%s,,,,,,",
            localtime, ackmark, (Nui64)time, str1.c_str(), str2.c_str());
        AddToLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::onReceiveLocal(const Nui8 * data, NBitCount count, Address * remote)
    {
        char str[256];
        char localtime[128];
        String str1, str2;
        mPrc2->getConnectTo(remote)->read(str1, true);
        remote.read(str2, true);
        TimeDurMS time = N_Engine().getTimer().getMS();
        GetLocalTime(localtime);

        sprintf(str, "%s,Lcl,PBP,,,%s,%i,%" N_64BIT_MOD "u,%s,%s,,,,,,",
            localtime, BaseIDTOString(data[0]), count, (Nui64)time, str1.c_str(), str2.c_str());
        AddToLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::onReceivePacket(Packet * packet, Nui32 frameNumber, Address * remote, TimeDurMS time, int isSend)
    {
        char str[256];
        const char * sendTypes[] =
        {
            "Rcv",
            "Snd",
            "Err1",
            "Err2",
            "Err3",
            "Err4",
            "Err5",
            "Err6",
        };
        const char * sendType = sendTypes[isSend];
        Address * localSystemAddress = mPrc2->getConnectTo(remote);

        Nui32 makesureMark;
        if (packet->mPacketType == PT_WILL_LOST ||
            packet->mPacketType == PT_ONE_WILL_LOST ||
            packet->mPacketType == PT_RECEIPT_WILL_LOST)
        {
            makesureMark = (Nui32)-1;
        }
        else
        {
            makesureMark = packet->mACKMark;
        }
        if (packet->mData[0] == ID_TIMESTAMP)
        {
            FormatLine(str, sendType, "Tms", makesureMark, frameNumber, packet->mData[1+sizeof(TimeDurMS)], 
                packet->mBitCount, (Nui64)time, localSystemAddress, remote, packet->mSplitID, packet->mSplitIndex, packet->mSplitCount, packet->mOrder);
        }
        else
        {
            FormatLine(str, sendType, "Nrm", makesureMark, frameNumber, packet->mData[0], packet->mBitCount,
                (Nui64)time, localSystemAddress, remote, packet->mSplitID, packet->mSplitIndex, packet->mSplitCount, packet->mOrder);
        }

        AddToLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::AddToLog(const char * str)
    {
        WriteLog(str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::WriteLog(const char * str)
    {
        N_printf("%s\n", str);
    }
    //------------------------------------------------------------------------
    void PacketLogger::WriteMiscellaneous(const char * type, const char * msg)
    {
        char str[1024];
        char localtime[128];
        String str1;
        mPrc2->getLAN(0, 0)->read(str1, true);
        TimeDurMS time = N_Engine().getTimer().getMS();
        GetLocalTime(localtime);

        sprintf(str, "%s,Lcl,%s,,,,,%" N_64BIT_MOD "u,%s,,,,,,,%s",
            localtime, type, (Nui64) time, str1.c_str(), msg);

        AddToLog(msg);
    }
    //------------------------------------------------------------------------
    void PacketLogger::SetPrintID(bool print)
    {
        printId=print;
    }
    //------------------------------------------------------------------------
    void PacketLogger::SetPrintAcks(bool print)
    {
        printAcks=print;
    }
    //------------------------------------------------------------------------
    const char * PacketLogger::BaseIDTOString(Nui8 Id)
    {
        if (Id >= ID_USER_PACKET_ENUM)
            return 0;

        const char * IDTable[((int)ID_USER_PACKET_ENUM)+1]=
        {
            "ID_CONNECTED_PING",
            "ID_UNCONNECTED_PING",
            "ID_UNCONNECTED_PING_OPEN_CONNECTIONS",
            "ID_CONNECTED_PONG",
            "ID_DETECT_LOST_CONNECTIONS",
            "ID_OPEN_CONNECTION_REQUEST_1",
            "ID_OPEN_CONNECTION_REPLY_1",
            "ID_OPEN_CONNECTION_REQUEST_2",
            "ID_OPEN_CONNECTION_REPLY_2",
            "ID_CONNECTION_REQUEST",
            "ID_REMOTE_SYSTEM_REQUIRES_PUBLIC_KEY",
            "ID_OUR_SYSTEM_REQUIRES_SECURITY",
            "ID_PUBLIC_KEY_MISMATCH",
            "ID_OUT_OF_BAND_INTERNAL",
            "ID_SND_RECEIPT_ACKED",
            "ID_SND_RECEIPT_LOSS",
            "ID_CONNECTION_REQUEST_ACCEPTED",
            "ID_CONNECTION_ATTEMPT_FAILED",
            "ID_ALREADY_CONNECTED",
            "ID_NEW_INCOMING_CONNECTION",
            "ID_NO_FREE_INCOMING_CONNECTIONS",
            "ID_DISCONNECTION_NOTIFICATION",
            "ID_CONNECTION_LOST",
            "ID_CONNECTION_BANNED",
            "ID_INVALID_PASSWORD",
            "ID_INCOMPATIBLE_PROTOCOL_VERSION",
            "ID_IP_RECENTLY_CONNECTED",
            "ID_TIMESTAMP",
            "ID_UNCONNECTED_PONG",
            "ID_ADVERTISE_SYSTEM",
            "ID_DOWNLOAD_PROGRESS",
            "ID_REMOTE_DISCONNECTION_NOTIFICATION",
            "ID_REMOTE_CONNECTION_LOST",
            "ID_REMOTE_NEW_INCOMING_CONNECTION",
            "ID_FILE_LIST_TRANSFER_HEADER",
            "ID_FILE_LIST_TRANSFER_FILE",
            "ID_FILE_LIST_REFERENCE_PUSH_ACK",
            "ID_DDT_DOWNLOAD_REQUEST",
            "ID_TRANSPORT_STRING",
            "ID_REPLICA_MANAGER_CONSTRUCTION",
            "ID_REPLICA_MANAGER_SCOPE_CHANGE",
            "ID_REPLICA_MANAGER_SERIALIZE",
            "ID_REPLICA_MANAGER_DOWNLOAD_STARTED",
            "ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE",
            "ID_RAKVOICE_OPEN_CHANNEL_REQUEST",
            "ID_RAKVOICE_OPEN_CHANNEL_REPLY",
            "ID_RAKVOICE_CLOSE_CHANNEL",
            "ID_RAKVOICE_DATA",
            "ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE",
            "ID_AUTOPATCHER_CREATION_LIST",
            "ID_AUTOPATCHER_DELETION_LIST",
            "ID_AUTOPATCHER_GET_PATCH",
            "ID_AUTOPATCHER_PATCH_LIST",
            "ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR",
            "ID_AUTOPATCHER_CANNOT_DOWNLOAD_ORIGINAL_UNMODIFIED_FILES",
            "ID_AUTOPATCHER_FINISHED_INTERNAL",
            "ID_AUTOPATCHER_FINISHED",
            "ID_AUTOPATCHER_RESTART_APPLICATION",
            "ID_NAT_PUNCHTHROUGH_REQUEST",
            "ID_NAT_CONNECT_AT_TIME",
            "ID_NAT_GET_MOST_RECENT_PORT",
            "ID_NAT_CLIENT_READY",
            "ID_NAT_TARGET_NOT_CONNECTED",
            "ID_NAT_TARGET_UNRESPONSIVE",
            "ID_NAT_CONNECTION_TO_TARGET_LOST",
            "ID_NAT_ALREADY_IN_PROGRESS",
            "ID_NAT_PUNCHTHROUGH_FAILED",
            "ID_NAT_PUNCHTHROUGH_SUCCEEDED",
            "ID_READY_EVENT_SET",
            "ID_READY_EVENT_UNSET",
            "ID_READY_EVENT_ALL_SET",
            "ID_READY_EVENT_QUERY",
            "ID_LOBBY_GENERAL",
            "ID_RPC_REMOTE_ERROR",
            "ID_RPC_PLUGIN",
            "ID_FILE_LIST_REFERENCE_PUSH",
            "ID_READY_EVENT_FORCE_ALL_SET",
            "ID_ROOMS_EXECUTE_FUNC",
            "ID_ROOMS_LOGON_STATUS",
            "ID_ROOMS_HANDLE_CHANGE",
            "ID_LOBBY2_SEND_MESSAGE",
            "ID_LOBBY2_SERVER_ERROR",
            "ID_FCM2_NEW_HOST",
            "ID_FCM2_REQUEST_FCMGUID",
            "ID_FCM2_RESPOND_CONNECTION_COUNT",
            "ID_FCM2_INFORM_FCMGUID",
            "ID_FCM2_UPDATE_MIN_TOTAL_CONNECTION_COUNT",
            "ID_FCM2_VERIFIED_JOIN_START",
            "ID_FCM2_VERIFIED_JOIN_CAPABLE",
            "ID_FCM2_VERIFIED_JOIN_FAILED",
            "ID_FCM2_VERIFIED_JOIN_ACCEPTED",
            "ID_FCM2_VERIFIED_JOIN_REJECTED",
            "ID_UDP_PROXY_GENERAL",
            "ID_SQLite3_EXEC",
            "ID_SQLite3_UNKNOWN_DB",
            "ID_SQLLITE_LOGGER",
            "ID_NAT_TYPE_DETECTION_REQUEST",
            "ID_NAT_TYPE_DETECTION_RESULT",
            "ID_ROUTER_2_INTERNAL",
            "ID_ROUTER_2_FORWARDING_NO_PATH",
            "ID_ROUTER_2_FORWARDING_ESTABLISHED",
            "ID_ROUTER_2_REROUTED",
            "ID_TEAM_BALANCER_INTERNAL",
            "ID_TEAM_BALANCER_REQUESTED_TEAM_FULL",
            "ID_TEAM_BALANCER_REQUESTED_TEAM_LOCKED",
            "ID_TEAM_BALANCER_TEAM_REQUESTED_CANCELLED",
            "ID_TEAM_BALANCER_TEAM_ASSIGNED",
            "ID_LIGHTSPEED_INTEGRATION",
            "ID_XBOX_LOBBY",
            "ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS",
            "ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_SUCCESS",
            "ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE",
            "ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE",
            "ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT",
            "ID_TWO_WAY_AUTHENTICATION_NEGOTIATION",
            "ID_CLOUD_POST_REQUEST",
            "ID_CLOUD_RELEASE_REQUEST",
            "ID_CLOUD_GET_REQUEST",
            "ID_CLOUD_GET_RESPONSE",
            "ID_CLOUD_UNSUBSCRIBE_REQUEST",
            "ID_CLOUD_SERVER_TO_SERVER_COMMAND",
            "ID_CLOUD_SUBSCRIPTION_NOTIFICATION",
            "ID_LIB_VOICE",
            "ID_RELAY_PLUGIN",
            "ID_NAT_REQUEST_BOUND_ADDRESSES",
            "ID_NAT_RESPOND_BOUND_ADDRESSES",
            "ID_RESERVED_2",
            "ID_RESERVED_3",
            "ID_RESERVED_4",
            "ID_RESERVED_5",
            "ID_RESERVED_6",
            "ID_RESERVED_7",
            "ID_RESERVED_8",
            "ID_RESERVED_9",
            "ID_USER_PACKET_ENUM"
        };

        return (char*)IDTable[Id];
    }
    //------------------------------------------------------------------------
    const char* PacketLogger::UserIDTOString(Nui8 Id)
    {
        // Users should override this
        static char str[256];
        i_a(Id, str, 10);
        return (const char*) str;
    }
    //------------------------------------------------------------------------
    const char * PacketLogger::IDTOString(Nui8 Id)
    {
        const char * out;
        out=BaseIDTOString(Id);
        if (out)
            return out;
        return UserIDTOString(Id);
    }
    //------------------------------------------------------------------------
    void PacketLogger::SetPrefix(const char * _prefix)
    {
        strncpy(prefix, _prefix, 255);
        prefix[255]=0;
    }
    //------------------------------------------------------------------------
    void PacketLogger::SetSuffix(const char * _suffix)
    {
        strncpy(suffix, _suffix, 255);
        suffix[255]=0;
    }
    //------------------------------------------------------------------------
    void PacketLogger::GetLocalTime(char buffer[128])
    {
    #if (N_PLAT == N_PLAT_WIN32) && !defined(__GNUC__)  && !defined(__GCCXML__)
        time_t rawtime;
        struct timeval tv;
        // If you get an arror about an incomplete type, just delete this file
        struct timezone tz;
        gettimeofday(&tv, &tz);
        // time ( &rawtime );
        rawtime = tv.tv_sec;

        struct tm * timeinfo;
        timeinfo = localtime ( &rawtime );
        strftime (buffer,128,"%x %X",timeinfo);
        char buff[32];
        sprintf(buff, ".%i", tv.tv_usec);
        strcat(buffer,buff);
    #else
        buffer[0] = 0;
    #endif
    }
    //------------------------------------------------------------------------
    void PacketLogger::SetLogDirectMessages(bool send)
    {
        logDirectMessages = send;
    }
    //------------------------------------------------------------------------
}
}
#endif