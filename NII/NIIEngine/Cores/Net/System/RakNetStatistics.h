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

#ifndef _NII_NET_STATISTICS_H_
#define _NII_NET_STATISTICS_H_

#include "NiiNetPreInclude.h"
#include "PacketLevel.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    enum RNSPerSecondMetrics
    {
        /// How many bytes per pushed via a call to LinkBase::send()
        USER_MESSAGE_BYTES_PUSHED = 0,

        /// How many user message bytes were sent via a call to LinkBase::send(). This is less than or equal to USER_MESSAGE_BYTES_PUSHED.
        /// A message would be pushed, but not yet sent, due to congestion control
        USER_MESSAGE_BYTES_SENT  = 1,

        /// How many user message bytes were resent. A message is resent if it is marked as reliable, and either the message didn't arrive or the message ack didn't arrive.
        USER_MESSAGE_BYTES_RESENT = 2,

        /// How many user message bytes were received, and returned to the user successfully.
        USER_MESSAGE_BYTES_RECEIVED_PROCESSED = 3,

        /// How many user message bytes were received, but ignored due to data format errors. This will usually be 0.
        USER_MESSAGE_BYTES_RECEIVED_IGNORED = 4,

        /// How many actual bytes were sent, including per-message and per-datagram overhead, and reliable message acks
        ACTUAL_BYTES_SENT = 5,

        /// How many actual bytes were received, including overead and acks.
        ACTUAL_BYTES_RECEIVED = 6,

        /// \internal
        RNS_PER_SECOND_METRICS_COUNT = 7
    };

    /// \brief Network Statisics Usage
    ///
    /// Store Statistics information related to network usage
    struct _EngineAPI NetStats
    {
        /// For each type in RNSPerSecondMetrics, what is the value over the last 1 second?
        Nui64 valueOverLastSecond[RNS_PER_SECOND_METRICS_COUNT];

        /// For each type in RNSPerSecondMetrics, what is the total value over the lifetime of the connection?
        Nui64 runningTotal[RNS_PER_SECOND_METRICS_COUNT];

        /// When did the connection start?
        /// \sa N_Engine().getTimer().getUS()
        TimeDurUS connectionStartTime;

        /// Is our current send rate throttled by congestion control?
        /// This value should be true if you send more data per second than your bandwidth capacity
        bool isLimitedByCongestionControl;

        /// If \a isLimitedByCongestionControl is true, what is the limit, in bytes per second?
        Nui64 BPSLimitByCongestionControl;
        
        bool mUploadLimit;
        
        /// If \a mUploadLimit is true, what is the limit, in bytes per second?
        Nui64 mUploadLimitValue;

        /// For each plevel, how many messages are waiting to be sent out?
        Nui32 messageInSendBuffer[PL_COUNT];

        /// For each plevel, how many bytes are waiting to be sent out?
        NIId bytesInSendBuffer[PL_COUNT];

        /// How many messages are waiting in the resend buffer? This includes messages waiting for an ack, so should normally be a small value
        /// If the value is rising over time, you are exceeding the bandwidth capacity. See BPSLimitByCongestionControl
        Nui32 messagesInResendBuffer;

        /// How many bytes are waiting in the resend buffer. See also messagesInResendBuffer
        Nui64 bytesInResendBuffer;

        /// Over the last second, what was our packetloss? This number will range from 0.0 (for none) to 1.0 (for 100%)
        NIIf packetlossLastSecond;

        /// What is the average total packetloss over the lifetime of the connection?
        NIIf packetlossTotal;

        NetStats & operator +=(const NetStats & other)
        {
            NCount i;
            for (i = 0; i < PL_COUNT; ++i)
            {
                messageInSendBuffer[i] += other.messageInSendBuffer[i];
                bytesInSendBuffer[i] += other.bytesInSendBuffer[i];
            }

            for (i = 0; i < RNS_PER_SECOND_METRICS_COUNT; ++i)
            {
                valueOverLastSecond[i] += other.valueOverLastSecond[i];
                runningTotal[i] += other.runningTotal[i];
            }

            return *this;
        }
    };

    typedef vector<NetStats *>::type NetStatsList;
    /// Verbosity level currently supports 0 (low), 1 (medium), 2 (high)
    /// \param[in] s The Statistical information to format out
    /// \param[in] buffer The buffer containing a formated report
    /// \param[in] verbosityLevel
    /// 0 low
    /// 1 medium
    /// 2 high
    /// 3 debugging congestion control
    void _EngineAPI StatisticsToString(NetStats * s, char * buffer, int verbosityLevel);
}
}
#endif