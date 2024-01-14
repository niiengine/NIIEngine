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

#ifndef __CONGESTION_CONTROL_UDT_H
#define __CONGESTION_CONTROL_UDT_H
#include "NiiNetPlat.h"

#if N_Socket_SlidingWindow != 1

#include "NiiNetCommon.h"
#include "DS_Queue.h"

namespace NII
{
namespace NII_NET
{
    typedef uint24_t DatagramSequenceNumberType;
    typedef NIId BytesPerMicrosecond;
    typedef NIId BytesPerSecond;
    typedef NIId MicrosecondsPerByte;

    /// CC_RAKNET_UDT_PACKET_HISTORY_LENGTH should be a power of 2 for the writeIndex variables to wrap properly
    #define CC_RAKNET_UDT_PACKET_HISTORY_LENGTH 64
    #define RTT_HISTORY_LENGTH 64

    /// Sizeof an UDP header in byte
    #define UDP_HEADER_SIZE 28

    #define CC_DEBUG_PRINTF_1(x)
    #define CC_DEBUG_PRINTF_2(x,y)
    #define CC_DEBUG_PRINTF_3(x,y,z)
    #define CC_DEBUG_PRINTF_4(x,y,z,a)
    #define CC_DEBUG_PRINTF_5(x,y,z,a,b)
    //#define CC_DEBUG_PRINTF_1(x) printf(x)
    //#define CC_DEBUG_PRINTF_2(x,y) printf(x,y)
    //#define CC_DEBUG_PRINTF_3(x,y,z) printf(x,y,z)
    //#define CC_DEBUG_PRINTF_4(x,y,z,a) printf(x,y,z,a)
    //#define CC_DEBUG_PRINTF_5(x,y,z,a,b) printf(x,y,z,a,b)

    /// \brief Encapsulates UDT congestion control, as used by RakNet
    /// Requirements:
    /// <OL>
    /// <LI>Each datagram is no more than N_Net_MaxMtuSize, after accounting for the UDP header
    /// <LI>Each datagram containing a user message has a sequence number which is set after calling OnSendBytes(). Set it by calling GetAndIncrementNextDatagramSequenceNumber()
    /// <LI>System is designed to be used from a single thread.
    /// <LI>Each packet should have a timeout time based on GetSenderRTOForACK(). If this time elapses, add the packet to the head of the send list for retransmission.
    /// </OL>
    ///
    /// Recommended:
    /// <OL>
    /// <LI>Call sendto in its own thread. This takes a significant amount of time in high speed networks.
    /// </OL>
    ///
    /// Algorithm:
    /// <OL>
    /// <LI>On a new connection, call Init()
    /// <LI>On a periodic interval (SYN time is the best) call update(). Also call ShouldSendACKs(), and send buffered ACKS if it returns true.
    /// <LI>Call OnSendAck() when sending acks.
    /// <LI>When you want to send or resend data, call GetNumberOfBytesToSend(). It will return you enough bytes to keep you busy for \a estimatedTimeToNextTick. You can send more than this to fill out a datagram, or to send packet pairs
    /// <LI>Call OnSendBytes() when sending datagrams.
    /// <LI>When data arrives, record the sequence number and buffer an ACK for it, to be sent from update() if ShouldSendACKs() returns true
    /// <LI>Every 16 packets that you send, send two of them back to back (a packet pair) as long as both packets are the same size. If you don't have two packets the same size, it is fine to defer this until you do.
    /// <LI>When you get a packet, call OnGotPacket(). If the packet is also either of a packet pair, call OnGotPacketPair()
    /// <LI>If you get a packet, and the sequence number is not 1 + the last sequence number, send a NAK. On the remote system, call OnNAK() and resend that message.
    /// <LI>If you get an ACK, remove that message from retransmission. Call OnNonDuplicateAck().
    /// <LI>If a message is not ACKed for GetRTOForRetransmission(), resend it.
    /// </OL>
    class CCRakNetUDT
    {
        public:

        CCRakNetUDT();
        ~CCRakNetUDT();

        /// Reset all variables to their initial states, for a new connection
        void Init(TimeDurUS curTime, Nui32 maxDatagramPayload);

        /// update over time
        void update(TimeDurUS curTime, bool hasDataToSendOrResend);

        int GetRetransmissionBandwidth(TimeDurUS curTime, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend);
        int GetTransmissionBandwidth(TimeDurUS curTime, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend);

        /// Acks do not have to be sent immediately. Instead, they can be buffered up such that groups of acks are sent at a time
        /// This reduces overall bandwidth usage
        /// How long they can be buffered depends on the retransmit time of the sender
        /// Should call once per update tick, and send if needed
        bool ShouldSendACKs(TimeDurUS curTime, TimeDurUS estimatedTimeToNextTick);

        /// Every data packet sent must contain a sequence number
        /// Call this function to get it. The sequence number is passed into OnGotPacketPair()
        DatagramSequenceNumberType GetAndIncrementNextDatagramSequenceNumber(void);
        DatagramSequenceNumberType GetNextDatagramSequenceNumber(void);

        /// Call this when you send packets
        /// Every 15th and 16th packets should be sent as a packet pair if possible
        /// When packets marked as a packet pair arrive, pass to OnGotPacketPair()
        /// When any packets arrive, (additionally) pass to OnGotPacket
        /// Packets should contain our system time, so we can pass rtt to OnNonDuplicateAck()
        void OnSendBytes(TimeDurUS curTime, Nui32 numBytes);

        /// Call this when you get a packet pair
        void OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, Nui32 sizeInBytes, TimeDurUS curTime);

        /// Call this when you get a packet (including packet pairs)
        /// If the DatagramSequenceNumberType is out of order, skippedMessageCount will be non-zero
        /// In that case, send a NAK for every sequence number up to that count
        bool OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, TimeDurUS curTime, Nui32 sizeInBytes, Nui32 *skippedMessageCount);

        /// Call when you get a NAK, with the sequence number of the lost message
        /// Affects the congestion control
        void OnResend(TimeDurUS curTime, TimeDurUS lastact);
        void OnNAK(TimeDurUS curTime, DatagramSequenceNumberType nakSequenceNumber);

        /// Call this when an ACK arrives.
        /// hasBAndAS are possibly written with the ack, see OnSendAck()
        /// B and AS are used in the calculations in UpdateWindowSizeAndAckOnAckPerSyn
        /// B and AS are updated at most once per SYN
        void onReceiveAck(TimeDurUS curTime, TimeDurUS rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, NIId totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber );
        void OnDuplicateAck( TimeDurUS curTime, DatagramSequenceNumberType sequenceNumber ) {}

        /// Call when you send an ack, to see if the ack should have the B and AS parameters transmitted
        /// Call before calling OnSendAck()
        void OnSendAckGetBAndAS(TimeDurUS curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS);

        /// Call when we send an ack, to write B and AS if needed
        /// B and AS are only written once per SYN, to prevent slow calculations
        /// Also updates SND, the period between sends, since data is written out
        /// Be sure to call OnSendAckGetBAndAS() before calling OnSendAck(), since whether you write it or not affects \a numBytes
        void OnSendAck(TimeDurUS curTime, Nui32 numBytes);

        /// Call when we send a NACK
        /// Also updates SND, the period between sends, since data is written out
        void OnSendNACK(TimeDurUS curTime, Nui32 numBytes);

        /// Retransmission time out for the sender
        /// If the time difference between when a message was last transmitted, and the current time is greater than RTO then packet is eligible for retransmission, pending congestion control
        /// RTO = (RTT + 4 * RTTVar) + SYN
        /// If we have been continuously sending for the last RTO, and no ACK or NAK at all, SND*=2;
        /// This is per message, which is different from UDT, but RakNet supports packetloss with continuing data where UDT is only PT_ORDER_MAKE_SURE
        /// Minimum value is 100 milliseconds
        TimeDurUS GetRTOForRetransmission(Nui8 sendCount) const;

        /// Set the maximum amount of data that can be sent in one datagram
        /// Default to N_Net_MaxMtuSize-UDP_HEADER_SIZE
        void SetMTU(Nui32 bytes);

        /// Return what was set by SetMTU()
        Nui32 GetMTU(void) const;

        /// Query for statistics
        BytesPerMicrosecond GetLocalSendRate(void) const {return 1.0 / SND;}
        BytesPerMicrosecond GetLocalReceiveRate(TimeDurUS currentTime) const;
        BytesPerMicrosecond GetRemoveReceiveRate(void) const {return AS;}
        BytesPerMicrosecond GetEstimatedBandwidth(void) const {return GetLinkCapacityBytesPerSecond()*1000000.0;}
        NIId GetLinkCapacityBytesPerSecond(void) const {return estimatedLinkCapacityBytesPerSecond;};

        /// Query for statistics
        NIId GetRTT(void) const;

        bool GetIsInSlowStart(void) const {return isInSlowStart;}
        Nui32 GetCWNDLimit(void) const {return (Nui32) (CWND*MAXIMUM_MTU_INCLUDING_UDP_HEADER);}


        /// Is a > b, accounting for variable overflow?
        static bool GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);
        /// Is a < b, accounting for variable overflow?
        static bool LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);

        Nui64 GetBytesPerSecondLimitByCongestionControl(void) const;
	protected:
        // --------------------------- PROTECTED VARIABLES ---------------------------
        /// time interval between bytes, in microseconds.
        /// Only used when slowStart==false
        /// Increased over time as we continually get messages
        /// Decreased on NAK and timeout
        /// Starts at 0 (invalid)
        MicrosecondsPerByte SND;

        /// Supportive window mechanism, controlling the maximum number of in-flight packets
        /// Used both during and after slow-start, but primarily during slow-start
        /// Starts at 2, which is also the low threshhold
        /// Max is the socket receive buffer / MTU
        /// CWND = AS * (RTT + SYN) + 16
        NIId CWND;

        /// When we do an update process on the SYN interval, nextSYNUpdate is set to the next time we should update
        /// Normally this is nextSYNUpdate+=SYN, in order to update on a consistent schedule
        /// However, if this would result in an immediate update yet again, it is set to SYN microseconds past the current time (in case the thread did not update for a long time)
        TimeDurUS nextSYNUpdate;

        /// Index into packetPairRecieptHistory where we will next write
        /// The history is always full (starting with default values) so no read index is needed
        int packetPairRecieptHistoryWriteIndex;

        /// Running round trip time (ping*2)
        /// Only sender needs to know this
        /// Initialized to UNSET
        /// Set to rtt on first calculation
        /// Updated gradually by RTT = RTT * 0.875 + rtt * 0.125
        NIId RTT;

        /// update: Use min/max, RTTVar follows current variance too closely resulting in packetloss
        NIId minRTT, maxRTT;

        /// Used to calculate packet arrival rate (in UDT) but data arrival rate (in RakNet, where not all datagrams are the same size)
        /// Filter is used to cull lowest half of values for bytesPerMicrosecond, to discount spikes and inactivity
        /// Referred to in the documentation as AS, data arrival rate
        /// AS is sent to the sender and calculated every 10th ack
        /// Each node represents (curTime-lastPacketArrivalTime)/bytes
        /// Used with ReceiverCalculateDataArrivalRate();
        BytesPerMicrosecond packetArrivalHistory[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH];
        BytesPerMicrosecond packetArrivalHistoryContinuousGaps[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH];
        Nui8 packetArrivalHistoryContinuousGapsIndex;
        Nui64 continuousBytesReceived;
        TimeDurUS continuousBytesReceivedStartTime;
        Nui32 packetArrivalHistoryWriteCount;

        /// Index into packetArrivalHistory where we will next write
        /// The history is always full (starting with default values) so no read index is needed
        int packetArrivalHistoryWriteIndex;

        /// Tracks the time the last packet that arrived, so BytesPerMicrosecond can be calculated for packetArrivalHistory when a new packet arrives
        TimeDurUS lastPacketArrivalTime;

        /// Data arrival rate from the sender to the receiver, as told to us by the receiver
        /// Used to calculate initial sending rate when slow start stops
        BytesPerMicrosecond AS;

        /// When the receiver last calculated and send B and AS, from packetArrivalHistory and packetPairRecieptHistory
        /// Used to prevent it from being calculated and send too frequently, as they are slow operations
        TimeDurUS lastTransmitOfBAndAS;

        /// New connections start in slow start
        /// During slow start, SND is not used, only CWND
        /// Slow start ends when we get a NAK, or the maximum size of CWND is reached
        /// SND is initialized to the inverse of the receiver's packet arrival rate when slow start ends
        bool isInSlowStart;

        /// How many NAKs arrived this congestion period
        /// Initialized to 1 when the congestion period starts
        Nui32 NAKCount;

        /// How many NAKs do you get on average during a congestion period?
        /// Starts at 1
        /// Used to generate a random number, DecRandom, between 1 and AvgNAKNum
        Nui32 AvgNAKNum;

        /// How many times we have decremented SND this congestion period. Used to limit the number of decrements to 5
        Nui32 DecCount;

        /// Every DecInterval NAKs per congestion period, we decrease the send rate
        Nui32 DecInterval;

        /// Every outgoing datagram is assigned a sequence number, which increments by 1 every assignment
        DatagramSequenceNumberType nextDatagramSequenceNumber;

        /// If a packet is marked as a packet pair, lastPacketPairPacketArrivalTime is set to the time it arrives
        /// This is used so when the 2nd packet of the pair arrives, we can calculate the time interval between the two
        TimeDurUS lastPacketPairPacketArrivalTime;

        /// If a packet is marked as a packet pair, lastPacketPairSequenceNumber is checked to see if the last packet we got
        /// was the packet immediately before the one that arrived
        /// If so, we can use lastPacketPairPacketArrivalTime to get the time between the two packets, and thus estimate the link capacity
        /// Initialized to -1, so the first packet of a packet pair won't be treated as the second
        DatagramSequenceNumberType lastPacketPairSequenceNumber;

        /// Used to cap UpdateWindowSizeAndAckOnAckPerSyn() to once speed increase per SYN
        /// This is to prevent speeding up faster than congestion control can compensate for
        TimeDurUS lastUpdateWindowSizeAndAck;

        /// Every time SND is halved due to timeout, the RTO is increased
        /// This is to prevent massive retransmissions to an unresponsive system
        /// Reset on any data arriving
        NIId ExpCount;

        /// Total number of user data bytes sent
        /// Used to adjust the window size, on ACK, during slow start
        Nui64 totalUserDataBytesSent;

        /// When we get an ack, if oldestUnsentAck==0, set it to the current time
        /// When we send out acks, set oldestUnsentAck to 0
        TimeDurUS oldestUnsentAck;

        // Maximum amount of bytes that the user can send, e.g. the size of one full datagram
        Nui32 MAXIMUM_MTU_INCLUDING_UDP_HEADER;

        // Max window size
        NIId CWND_MAX_THRESHOLD;

        /// Track which datagram sequence numbers have arrived.
        /// If a sequence number is skipped, send a NAK for all skipped messages
        DatagramSequenceNumberType expectedNextSequenceNumber;

        // How many times have we sent B and AS? Used to force it to send at least CC_RAKNET_UDT_PACKET_HISTORY_LENGTH times
        // Otherwise, the default values in the array generate inaccuracy
        Nui32 sendBAndASCount;

        /// Most recent values read into the corresponding lists
        /// Used during the beginning of a connection, when the median filter is still inaccurate
        BytesPerMicrosecond mostRecentPacketArrivalHistory;

        bool hasWrittenToPacketPairReceiptHistory;

        NIId estimatedLinkCapacityBytesPerSecond;

        // --------------------------- PROTECTED METHODS ---------------------------
        /// update nextSYNUpdate by SYN, or the same amount past the current time if no updates have occurred for a long time
        void SetNextSYNUpdate(TimeDurUS currentTime);

        /// Returns the rate of data arrival, based on packets arriving on the sender.
        BytesPerMicrosecond ReceiverCalculateDataArrivalRate(TimeDurUS curTime) const;
        /// Returns the median of the data arrival rate
        BytesPerMicrosecond ReceiverCalculateDataArrivalRateMedian(void) const;

        /// Calculates the median an array of BytesPerMicrosecond
        static BytesPerMicrosecond CalculateListMedianRecursive(const BytesPerMicrosecond inputList[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum);

        /// Same as GetRTOForRetransmission, but does not factor in ExpCount
        /// This is because the receiver does not know ExpCount for the sender, and even if it did, acks shouldn't be delayed for this reason
        TimeDurUS GetSenderRTOForACK(void) const;

        /// Stop slow start, and enter normal transfer rate
        void EndSlowStart(void);

        /// Does the named conversion
        inline NIId BytesPerMicrosecondToPacketsPerMillisecond(BytesPerMicrosecond in);

        /// update the corresponding variables pre-slow start
        void UpdateWindowSizeAndAckOnAckPreSlowStart(NIId totalUserDataBytesAcked);

        /// update the corresponding variables post-slow start
        void UpdateWindowSizeAndAckOnAckPerSyn(TimeDurUS curTime, TimeDurUS rtt, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber);

        /// Sets halveSNDOnNoDataTime to the future, and also resets ExpCount, which is used to multiple the RTO on no data arriving at all
        void ResetOnDataArrivalHalveSNDOnNoDataTime(TimeDurUS curTime);

        // Init array
        void InitPacketArrivalHistory(void);

        // Printf
        void PrintLowBandwidthWarning(void);

        // Bug: SND can sometimes get super high - have seen 11693
        void CapMinSnd();

        void DecreaseTimeBetweenSends(void);
        void IncreaseTimeBetweenSends(void);

        int bytesCanSendThisTick;

        TimeDurUS lastRttOnIncreaseSendRate;
        TimeDurUS lastRtt;

        DatagramSequenceNumberType nextCongestionControlBlock;
        bool hadPacketlossThisBlock;
        DataStructures::Queue<TimeDurUS> pingsLastInterval;
    };
    
}
}
#endif

/*
http://www.ssfnet.org/Exchange/tcp/tcpTutorialNotes.html

cwnd=max bytes allowed on wire at once

Start:
cwnd=mtu
ssthresh=unlimited

Slow start:
On ack cwnd*=2

congestion avoidance:
On ack during new period
cwnd+=mtu*mtu/cwnd

on loss or duplicate ack during period:
sshtresh=cwnd/2
cwnd=MTU
This reenters slow start

If cwnd < ssthresh, then use slow start
else use congestion avoidance


*/

#if N_Socket_SlidingWindow == 1

#include "DS_Queue.h"
/// Sizeof an UDP header in byte
#define UDP_HEADER_SIZE 28

#define CC_DEBUG_PRINTF_1(x)
#define CC_DEBUG_PRINTF_2(x,y)
#define CC_DEBUG_PRINTF_3(x,y,z)
#define CC_DEBUG_PRINTF_4(x,y,z,a)
#define CC_DEBUG_PRINTF_5(x,y,z,a,b)

namespace NII
{
namespace NII_NET
{
    typedef uint24_t DatagramSequenceNumberType;
    typedef NIId BytesPerMicrosecond;
    typedef NIId BytesPerSecond;
    typedef NIId MicrosecondsPerByte;

    class CCRakNetSlidingWindow
    {
	public:
        CCRakNetSlidingWindow();
        ~CCRakNetSlidingWindow();

        /// Reset all variables to their initial states, for a new connection
        void Init(TimeDurUS curTime, Nui32 maxDatagramPayload);

        /// update over time
        void update(TimeDurUS curTime, bool hasDataToSendOrResend);

        int GetRetransmissionBandwidth(TimeDurUS curTime, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend);
        int GetTransmissionBandwidth(TimeDurUS curTime, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend);

        /// Acks do not have to be sent immediately. Instead, they can be buffered up such that groups of acks are sent at a time
        /// This reduces overall bandwidth usage
        /// How long they can be buffered depends on the retransmit time of the sender
        /// Should call once per update tick, and send if needed
        bool ShouldSendACKs(TimeDurUS curTime, TimeDurUS estimatedTimeToNextTick);

        /// Every data packet sent must contain a sequence number
        /// Call this function to get it. The sequence number is passed into OnGotPacketPair()
        DatagramSequenceNumberType GetAndIncrementNextDatagramSequenceNumber(void);
        DatagramSequenceNumberType GetNextDatagramSequenceNumber(void);

        /// Call this when you send packets
        /// Every 15th and 16th packets should be sent as a packet pair if possible
        /// When packets marked as a packet pair arrive, pass to OnGotPacketPair()
        /// When any packets arrive, (additionally) pass to OnGotPacket
        /// Packets should contain our system time, so we can pass rtt to OnNonDuplicateAck()
        void OnSendBytes(TimeDurUS curTime, Nui32 numBytes);

        /// Call this when you get a packet pair
        void OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber, Nui32 sizeInBytes, TimeDurUS curTime);

        /// Call this when you get a packet (including packet pairs)
        /// If the DatagramSequenceNumberType is out of order, skippedMessageCount will be non-zero
        /// In that case, send a NAK for every sequence number up to that count
        bool OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber, bool isContinuousSend, TimeDurUS curTime, Nui32 sizeInBytes, Nui32 *skippedMessageCount);

        /// Call when you get a NAK, with the sequence number of the lost message
        /// Affects the congestion control
        void OnResend(TimeDurUS curTime, TimeDurUS lastact);
        void OnNAK(TimeDurUS curTime, DatagramSequenceNumberType nakSequenceNumber);

        /// Call this when an ACK arrives.
        /// hasBAndAS are possibly written with the ack, see OnSendAck()
        /// B and AS are used in the calculations in UpdateWindowSizeAndAckOnAckPerSyn
        /// B and AS are updated at most once per SYN
        void onReceiveAck(TimeDurUS curTime, TimeDurUS rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, NIId totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber );
        void OnDuplicateAck( TimeDurUS curTime, DatagramSequenceNumberType sequenceNumber );

        /// Call when you send an ack, to see if the ack should have the B and AS parameters transmitted
        /// Call before calling OnSendAck()
        void OnSendAckGetBAndAS(TimeDurUS curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS);

        /// Call when we send an ack, to write B and AS if needed
        /// B and AS are only written once per SYN, to prevent slow calculations
        /// Also updates SND, the period between sends, since data is written out
        /// Be sure to call OnSendAckGetBAndAS() before calling OnSendAck(), since whether you write it or not affects \a numBytes
        void OnSendAck(TimeDurUS curTime, Nui32 numBytes);

        /// Call when we send a NACK
        /// Also updates SND, the period between sends, since data is written out
        void OnSendNACK(TimeDurUS curTime, Nui32 numBytes);

        /// Retransmission time out for the sender
        /// If the time difference between when a message was last transmitted, and the current time is greater than RTO then packet is eligible for retransmission, pending congestion control
        /// RTO = (RTT + 4 * RTTVar) + SYN
        /// If we have been continuously sending for the last RTO, and no ACK or NAK at all, SND*=2;
        /// This is per message, which is different from UDT, but RakNet supports packetloss with continuing data where UDT is only PT_ORDER_MAKE_SURE
        /// Minimum value is 100 milliseconds
        TimeDurUS GetRTOForRetransmission(Nui8 sendCount) const;

        /// Set the maximum amount of data that can be sent in one datagram
        /// Default to N_Net_MaxMtuSize-UDP_HEADER_SIZE
        void SetMTU(Nui32 bytes);

        /// Return what was set by SetMTU()
        Nui32 GetMTU(void) const;

        /// Query for statistics
        BytesPerMicrosecond GetLocalSendRate(void) const {return 0;}
        BytesPerMicrosecond GetLocalReceiveRate(TimeDurUS currentTime) const;
        BytesPerMicrosecond GetRemoveReceiveRate(void) const {return 0;}
        BytesPerMicrosecond GetEstimatedBandwidth(void) const {return GetLinkCapacityBytesPerSecond()*1000000.0;}
        NIId GetLinkCapacityBytesPerSecond(void) const {return 0;}

        /// Query for statistics
        NIId GetRTT(void) const;

        bool GetIsInSlowStart(void) const {return IsInSlowStart();}
        Nui32 GetCWNDLimit(void) const {return (Nui32) 0;}

        /// Is a > b, accounting for variable overflow?
        static bool GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);
        /// Is a < b, accounting for variable overflow?
        static bool LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b);

        Nui64 GetBytesPerSecondLimitByCongestionControl(void) const;
	protected:
        // Maximum amount of bytes that the user can send, e.g. the size of one full datagram
        Nui32 MAXIMUM_MTU_INCLUDING_UDP_HEADER;

        NIId cwnd; // max bytes on wire
        NIId ssThresh; // Threshhold between slow start and congestion avoidance

        /// When we get an ack, if oldestUnsentAck==0, set it to the current time
        /// When we send out acks, set oldestUnsentAck to 0
        TimeDurUS oldestUnsentAck;

        TimeDurUS GetSenderRTOForACK(void) const;

        /// Every outgoing datagram is assigned a sequence number, which increments by 1 every assignment
        DatagramSequenceNumberType nextDatagramSequenceNumber;
        DatagramSequenceNumberType nextCongestionControlBlock;
        bool backoffThisBlock, speedUpThisBlock;
        /// Track which datagram sequence numbers have arrived.
        /// If a sequence number is skipped, send a NAK for all skipped messages
        DatagramSequenceNumberType expectedNextSequenceNumber;

        bool _isContinuousSend;

        bool IsInSlowStart(void) const;

        NIId lastRtt, estimatedRTT, deviationRtt;
    };
}
}

#endif

#endif