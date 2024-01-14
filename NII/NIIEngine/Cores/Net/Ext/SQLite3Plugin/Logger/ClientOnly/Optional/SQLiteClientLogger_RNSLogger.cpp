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
#include "SQLiteClientLogger_RNSLogger.h"
#include "NetStats.h"
#include "NiiNetLinkBase.h"
#include "SQLiteClientLoggerPlugin.h"

namespace NII
{
namespace NII_NET
{
    static const char * DEFAULT_RAKNET_STATISTICS_TABLE="NetStats";

    SQLiteClientLogger_RakNetStatistics::SQLiteClientLogger_RakNetStatistics()
    {
        lastUpdate = 0;
    }
    SQLiteClientLogger_RakNetStatistics::~SQLiteClientLogger_RakNetStatistics()
    {
    }
    void SQLiteClientLogger_RakNetStatistics::update()
    {
        TimeDurUS time = N_Engine().getTimer().getUS();
        if (time-lastUpdate > 1000000)
        {
            lastUpdate=time;
            unsigned int i;
            NetStats rns;
            for (i=0; i < mPrc2->GetMaximumNumberOfPeers(); i++)
            {
                if (mPrc2->getReport(i, &rns))
                {
                    /*
                    rns.valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT],
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT],
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
                        rns.valueOverLastSecond[ACTUAL_BYTES_SENT],
                        rns.valueOverLastSecond[ACTUAL_BYTES_RECEIVED],
                        rns.runningTotal[USER_MESSAGE_BYTES_PUSHED],
                        rns.runningTotal[USER_MESSAGE_BYTES_SENT],
                        rns.runningTotal[USER_MESSAGE_BYTES_RESENT],
                        rns.runningTotal[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],
                        rns.runningTotal[USER_MESSAGE_BYTES_RECEIVED_IGNORED],
                        rns.runningTotal[ACTUAL_BYTES_SENT],
                        rns.runningTotal[ACTUAL_BYTES_RECEIVED],
                        rns.connectionStartTime,
                        rns.BPSLimitByCongestionControl,
                        rns.isLimitedByCongestionControl,
                        rns.BPSLimitByOutgoingBandwidthLimit,
                        rns.isLimitedByOutgoingBandwidthLimit,
                        rns.messageInSendBuffer[IMMEDIATE_PRIORITY],
                        rns.messageInSendBuffer[HIGH_PRIORITY],
                        rns.messageInSendBuffer[MEDIUM_PRIORITY],
                        rns.messageInSendBuffer[LOW_PRIORITY],
                        rns.bytesInSendBuffer[IMMEDIATE_PRIORITY],
                        rns.bytesInSendBuffer[HIGH_PRIORITY],
                        rns.bytesInSendBuffer[MEDIUM_PRIORITY],
                        rns.bytesInSendBuffer[LOW_PRIORITY],
                        rns.messagesInResendBuffer,
                        rns.bytesInResendBuffer,
                        rns.packetlossLastSecond,
                        rns.packetlossTotal,
                        */

                

                    rakSqlLog(
                        DEFAULT_RAKNET_STATISTICS_TABLE,
                        "valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED],"
                        "valueOverLastSecond[USER_MESSAGE_BYTES_SENT],"
                        "valueOverLastSecond[USER_MESSAGE_BYTES_RESENT],"
                        "valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED],"
                        "valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED],"
                        "valueOverLastSecond[ACTUAL_BYTES_SENT],"
                        "valueOverLastSecond[ACTUAL_BYTES_RECEIVED],"
                        "BPSLimitByCongestionControl,"
                        "BPSLimitByOutgoingBandwidthLimit,"
                        "bytesInSendBuffer,"
                        "messagesInResendBuffer,"
                        "bytesInResendBuffer,"
                        "packetlossLastSecond,"
                        "packetlossTotal",
                        ( \
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_PUSHED], \
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT], \
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT], \
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_PROCESSED], \
                        rns.valueOverLastSecond[USER_MESSAGE_BYTES_RECEIVED_IGNORED], \
                        rns.valueOverLastSecond[ACTUAL_BYTES_SENT], \
                        rns.valueOverLastSecond[ACTUAL_BYTES_RECEIVED], \
                        rns.BPSLimitByCongestionControl, \
                        rns.BPSLimitByOutgoingBandwidthLimit, \
                        rns.bytesInSendBuffer[IMMEDIATE_PRIORITY]+rns.bytesInSendBuffer[HIGH_PRIORITY]+rns.bytesInSendBuffer[MEDIUM_PRIORITY]+rns.bytesInSendBuffer[LOW_PRIORITY], \
                        rns.messagesInResendBuffer, \
                        rns.bytesInResendBuffer, \
                        rns.packetlossLastSecond, \
                        rns.packetlossTotal \
                        ));
                }
            }
        }
    }
}
}