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

#include "NiiNetContrlWindow.h"

#if N_Socket_SlidingWindow != 1

#include "Rand.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    static const NIId UNSET_TIME_US = -1;
    static const NIId CWND_MIN_THRESHOLD = 2.0;
    static const NIId UNDEFINED_TRANSFER_RATE = 0.0;
    //------------------------------------------------------------------------
    /// Interval at which to update aspects of the system
    /// 1. send acks
    /// 2. update time interval between outgoing packets
    /// 3, Yodate retransmit timeout

    static const TimeDurUS SYN = 10;
    #define MAX_RTT 1000
    #define RTT_TOLERANCE 30
    NIId RTTVarMultiple=4.0;
    //------------------------------------------------------------------------
    CCRakNetUDT::CCRakNetUDT()
    {
    }
    //------------------------------------------------------------------------
    CCRakNetUDT::~CCRakNetUDT()
    {
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::Init(TimeDurUS curTime, Nui32 maxDatagramPayload)
    {
        (void)curTime;

        nextSYNUpdate = 0;
        packetPairRecieptHistoryWriteIndex = 0;
        packetArrivalHistoryWriteIndex = 0;
        packetArrivalHistoryWriteCount = 0;
        RTT = UNSET_TIME_US;
        isInSlowStart = true;
        NAKCount = 1000;
        AvgNAKNum = 1;
        DecInterval = 1;
        DecCount = 0;
        nextDatagramSequenceNumber = 0;
        lastPacketPairPacketArrivalTime = 0;
        lastPacketPairSequenceNumber = (DatagramSequenceNumberType)(const Nui32)-1;
        lastPacketArrivalTime = 0;
        CWND=CWND_MIN_THRESHOLD;
        lastUpdateWindowSizeAndAck = 0;
        lastTransmitOfBAndAS = 0;
        ExpCount = 1.0;
        totalUserDataBytesSent = 0;
        oldestUnsentAck = 0;
        MAXIMUM_MTU_INCLUDING_UDP_HEADER = maxDatagramPayload;
        CWND_MAX_THRESHOLD = RESEND_BUFFER_ARRAY_LENGTH;

        const BytesPerMicrosecond DEFAULT_TRANSFER_RATE = (BytesPerMicrosecond) 3.6;

        lastRttOnIncreaseSendRate=1000;

        nextCongestionControlBlock=0;
        lastRtt=0;
        AS=UNDEFINED_TRANSFER_RATE;
        const MicrosecondsPerByte DEFAULT_BYTE_INTERVAL=(MicrosecondsPerByte) (1.0/DEFAULT_TRANSFER_RATE);
        SND=DEFAULT_BYTE_INTERVAL;
        expectedNextSequenceNumber = 0;
        sendBAndASCount = 0;
        packetArrivalHistoryContinuousGapsIndex = 0;
        hasWrittenToPacketPairReceiptHistory = false;
        InitPacketArrivalHistory();

        estimatedLinkCapacityBytesPerSecond = 0;
        bytesCanSendThisTick = 0;
        hadPacketlossThisBlock = false;
        pingsLastInterval.Clear();
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::SetMTU(Nui32 bytes)
    {
        MAXIMUM_MTU_INCLUDING_UDP_HEADER=bytes;
    }
    //------------------------------------------------------------------------
    Nui32 CCRakNetUDT::GetMTU() const
    {
        return MAXIMUM_MTU_INCLUDING_UDP_HEADER;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::update(TimeDurUS, bool)
    {
        return;
    }
    //------------------------------------------------------------------------
    int CCRakNetUDT::GetRetransmissionBandwidth(TimeDurUS curTime, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend)
    {
        if (isInSlowStart)
        {
            Nui32 CWNDLimit = CWND * MAXIMUM_MTU_INCLUDING_UDP_HEADER;
            return CWNDLimit;
        }
        return GetTransmissionBandwidth(curTime,timeSinceLastTick,ackremain,isContinuousSend);
    }
    //------------------------------------------------------------------------
    int CCRakNetUDT::GetTransmissionBandwidth(TimeDurUS, TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend)
    {
        if (isInSlowStart)
        {
            Nui32 CWNDLimit = CWND * MAXIMUM_MTU_INCLUDING_UDP_HEADER - ackremain;
            return CWNDLimit;
        }
        if (bytesCanSendThisTick>0)
            bytesCanSendThisTick=0;

        if (isContinuousSend==false && timeSinceLastTick>100)
            timeSinceLastTick=100;

        bytesCanSendThisTick=(int)((NIId)timeSinceLastTick*((NIId)1.0/SND)+(NIId)bytesCanSendThisTick);
        if (bytesCanSendThisTick > 0)
            return bytesCanSendThisTick;
        return 0;
    }
    //------------------------------------------------------------------------
    Nui64 CCRakNetUDT::GetBytesPerSecondLimitByCongestionControl() const
    {
        if(isInSlowStart)
            return 0;

        return (Nui64) ((NIId)1.0/(SND*1000.0));
    }
    //------------------------------------------------------------------------
    bool CCRakNetUDT::ShouldSendACKs(TimeDurUS curTime, TimeDurUS estimatedTimeToNextTick)
    {
        TimeDurUS rto = GetSenderRTOForACK();

        // iphone crashes on comparison between NIId and int64 http://www.jenkinssoftware.com/forum/index.php?topic=2717.0
        if (rto==(TimeDurUS) UNSET_TIME_US)
        {
            // Unknown how long until the remote system will retransmit, so better send right away
            return true;
        }

        // Simplified equation
        // GU: At least one ACK should be sent per SYN, otherwise your protocol will increase slower.
        return curTime >= oldestUnsentAck + SYN ||
            estimatedTimeToNextTick+curTime < oldestUnsentAck+rto-RTT;
    }
    //------------------------------------------------------------------------
    DatagramSequenceNumberType CCRakNetUDT::GetNextDatagramSequenceNumber(void)
    {
        return nextDatagramSequenceNumber;
    }
    //------------------------------------------------------------------------
    DatagramSequenceNumberType CCRakNetUDT::GetAndIncrementNextDatagramSequenceNumber(void)
    {
        DatagramSequenceNumberType dsnt=nextDatagramSequenceNumber;
        nextDatagramSequenceNumber++;
        return dsnt;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnSendBytes(TimeDurUS curTime, Nui32 numBytes)
    {
        (void) curTime;

        totalUserDataBytesSent += numBytes;
        if(isInSlowStart == false)
            bytesCanSendThisTick -= numBytes;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::SetNextSYNUpdate(TimeDurUS currentTime)
    {
        nextSYNUpdate+=SYN;
        if (nextSYNUpdate < currentTime)
            nextSYNUpdate=currentTime+SYN;
    }
    //------------------------------------------------------------------------
    BytesPerMicrosecond CCRakNetUDT::ReceiverCalculateDataArrivalRate(TimeDurUS curTime) const
    {
        (void) curTime;

        if (packetArrivalHistoryWriteCount<CC_RAKNET_UDT_PACKET_HISTORY_LENGTH)
            return UNDEFINED_TRANSFER_RATE;

        BytesPerMicrosecond median = ReceiverCalculateDataArrivalRateMedian();
        int i;
        const BytesPerMicrosecond oneEighthMedian=median*(1.0/8.0);
        const BytesPerMicrosecond eightTimesMedian=median*8.0f;
        BytesPerMicrosecond medianListLength=0.0;
        BytesPerMicrosecond sum=0.0;
        // Find average of acceptedMedianValues
        for (i=0; i < CC_RAKNET_UDT_PACKET_HISTORY_LENGTH; i++)
        {
            if (packetArrivalHistory[i]>=oneEighthMedian &&
                packetArrivalHistory[i]<eightTimesMedian)
            {
                medianListLength=medianListLength+1.0;
                sum+=packetArrivalHistory[i];
            }
        }
        if (medianListLength==0.0)
            return UNDEFINED_TRANSFER_RATE;
        return sum/medianListLength;
    }
    //------------------------------------------------------------------------
    BytesPerMicrosecond CCRakNetUDT::ReceiverCalculateDataArrivalRateMedian(void) const
    {
        return CalculateListMedianRecursive(packetArrivalHistory, CC_RAKNET_UDT_PACKET_HISTORY_LENGTH, 0, 0);
    }
    //------------------------------------------------------------------------
    BytesPerMicrosecond CCRakNetUDT::CalculateListMedianRecursive(const BytesPerMicrosecond inputList[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH], int inputListLength, int lessThanSum, int greaterThanSum)
    {
        BytesPerMicrosecond lessThanMedian[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH], greaterThanMedian[CC_RAKNET_UDT_PACKET_HISTORY_LENGTH];
        int lessThanMedianListLength=0, greaterThanMedianListLength=0;
        BytesPerMicrosecond median=inputList[0];
        int i;
        for (i=1; i < inputListLength; i++)
        {
            // If same value, spread among lists evenly
            if (inputList[i]<median || ((i&1)==0 && inputList[i]==median))
                lessThanMedian[lessThanMedianListLength++]=inputList[i];
            else
                greaterThanMedian[greaterThanMedianListLength++]=inputList[i];
        }
        N_assert(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH%2==0);
        if (lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum+1 ||
            lessThanMedianListLength+lessThanSum==greaterThanMedianListLength+greaterThanSum-1)
            return median;

        if (lessThanMedianListLength+lessThanSum < greaterThanMedianListLength+greaterThanSum)
        {
            lessThanMedian[lessThanMedianListLength++]=median;
            return CalculateListMedianRecursive(greaterThanMedian, greaterThanMedianListLength, lessThanMedianListLength+lessThanSum, greaterThanSum);
        }
        else
        {
            greaterThanMedian[greaterThanMedianListLength++]=median;
            return CalculateListMedianRecursive(lessThanMedian, lessThanMedianListLength, lessThanSum, greaterThanMedianListLength+greaterThanSum);
        }
    }
    //------------------------------------------------------------------------
    bool CCRakNetUDT::GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
    {
        // a > b?
        const DatagramSequenceNumberType halfSpan =(DatagramSequenceNumberType) (((DatagramSequenceNumberType)(const Nui32)-1)/(DatagramSequenceNumberType)2);
        return b!=a && b-a>halfSpan;
    }
    //------------------------------------------------------------------------
    bool CCRakNetUDT::LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
    {
        // a < b?
        const DatagramSequenceNumberType halfSpan = ((DatagramSequenceNumberType)(const Nui32)-1)/(DatagramSequenceNumberType)2;
        return b!=a && b-a<halfSpan;
    }
    //------------------------------------------------------------------------
    TimeDurUS CCRakNetUDT::GetSenderRTOForACK() const
    {
        if (RTT==UNSET_TIME_US)
            return (TimeDurUS) UNSET_TIME_US;
        NIId RTTVar = maxRTT-minRTT;
        return (TimeDurUS)(RTT + RTTVarMultiple * RTTVar + SYN);
    }
    //------------------------------------------------------------------------
    TimeDurUS CCRakNetUDT::GetRTOForRetransmission(Nui8 sendCount) const
    {
        const TimeDurUS maxThreshold = 10000;
        const TimeDurUS minThreshold = 100;

        if(RTT==UNSET_TIME_US)
        {
            return (TimeDurUS) maxThreshold;
        }

        TimeDurUS ret = lastRttOnIncreaseSendRate * 2;

        if (ret < minThreshold)
            return minThreshold;
        if (ret > maxThreshold)
            return maxThreshold;
        return ret;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnResend(TimeDurUS curTime, TimeDurUS lastact)
    {
        (void) curTime;

        if (isInSlowStart)
        {
            if (AS!=UNDEFINED_TRANSFER_RATE)
                EndSlowStart();
            return;
        }

        if (hadPacketlossThisBlock==false)
        {
            IncreaseTimeBetweenSends();
            hadPacketlossThisBlock=true;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnNAK(TimeDurUS curTime, DatagramSequenceNumberType nakSequenceNumber)
    {
        (void) nakSequenceNumber;
        (void) curTime;

        if (isInSlowStart)
        {
            if (AS!=UNDEFINED_TRANSFER_RATE)
                EndSlowStart();
            return;
        }

        if (hadPacketlossThisBlock==false)
        {
            if (pingsLastInterval.Size()>10)
            {
                for (int i=0; i < 10; i++)
                    printf("%i, ", pingsLastInterval[pingsLastInterval.Size()-1-i]/1000);
            }
            printf("\n");
            IncreaseTimeBetweenSends();

            hadPacketlossThisBlock=true;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::EndSlowStart(void)
    {
        N_assert(isInSlowStart==true);
        N_assert(AS!=UNDEFINED_TRANSFER_RATE);

        // This overestimates
        estimatedLinkCapacityBytesPerSecond=AS * 1000000.0;

        isInSlowStart=false;
        SND=1.0/AS;
        CapMinSnd();

        if (SND > .1)
            PrintLowBandwidthWarning();
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber,
        Nui32 sizeInBytes, TimeDurUS curTime)
    {
        (void) datagramSequenceNumber;
        (void) sizeInBytes;
        (void) curTime;
    }
    //------------------------------------------------------------------------
    bool CCRakNetUDT::OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber,
        bool isContinuousSend, TimeDurUS curTime, Nui32 sizeInBytes,
            Nui32 * skippedMessageCount)
    {
        CC_DEBUG_PRINTF_2("R%i ",datagramSequenceNumber.val);

        if (datagramSequenceNumber == expectedNextSequenceNumber)
        {
            *skippedMessageCount = 0;
            expectedNextSequenceNumber = datagramSequenceNumber+(DatagramSequenceNumberType)1;
        }
        else if (GreaterThan(datagramSequenceNumber, expectedNextSequenceNumber))
        {
            *skippedMessageCount = datagramSequenceNumber-expectedNextSequenceNumber;
            // Sanity check, just use timeout resend if this was really valid
            if (*skippedMessageCount > 1000)
            {
                // During testing, the nat punchthrough server got 51200 on the first packet. I have no idea where this comes from, but has happened twice
                if (*skippedMessageCount > (Nui32)50000)
                    return false;
                *skippedMessageCount = 1000;
            }
            expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
        }
        else
        {
            *skippedMessageCount = 0;
        }

        if (curTime>lastPacketArrivalTime)
        {
            TimeDurUS interval = curTime-lastPacketArrivalTime;

            if (isContinuousSend)
            {
                continuousBytesReceived+=sizeInBytes;
                if (continuousBytesReceivedStartTime==0)
                    continuousBytesReceivedStartTime=lastPacketArrivalTime;


                mostRecentPacketArrivalHistory=(BytesPerMicrosecond)sizeInBytes/(BytesPerMicrosecond)interval;
                packetArrivalHistoryContinuousGaps[packetArrivalHistoryContinuousGapsIndex++]=(int) interval;
                packetArrivalHistoryContinuousGapsIndex&=(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH-1);

                packetArrivalHistoryWriteCount++;
                packetArrivalHistory[packetArrivalHistoryWriteIndex++]=mostRecentPacketArrivalHistory;
                // Wrap to 0 at the end of the range
                // Assumes power of 2 for CC_RAKNET_UDT_PACKET_HISTORY_LENGTH
                packetArrivalHistoryWriteIndex&=(CC_RAKNET_UDT_PACKET_HISTORY_LENGTH-1);
            }
            else
            {
                continuousBytesReceivedStartTime=0;
                continuousBytesReceived=0;
            }

            lastPacketArrivalTime=curTime;
        }
        return true;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::onReceiveAck(TimeDurUS curTime, TimeDurUS rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS, 
        NIId totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber )
    {
        N_assert(rtt < 10000);

        (void) _B;
        if(hasBAndAS)
        {
            ///	N_assert(_B!=UNDEFINED_TRANSFER_RATE && _AS!=UNDEFINED_TRANSFER_RATE);
            //	B=B * .875 + _B * .125;
            // AS is packet arrival rate
            N_assert(_AS!=UNDEFINED_TRANSFER_RATE);
            AS=_AS;
            CC_DEBUG_PRINTF_4("ArrivalRate=%f linkCap=%f incomingLinkCap=%f\n", _AS,B,_B);
        }

        if(oldestUnsentAck==0)
            oldestUnsentAck=curTime;

        if(isInSlowStart==true)
        {
            nextCongestionControlBlock=nextDatagramSequenceNumber;
            lastRttOnIncreaseSendRate=rtt;
            UpdateWindowSizeAndAckOnAckPreSlowStart(totalUserDataBytesAcked);
        }
        else
        {
            UpdateWindowSizeAndAckOnAckPerSyn(curTime, rtt, isContinuousSend, sequenceNumber);
        }

        lastUpdateWindowSizeAndAck=curTime;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnSendAckGetBAndAS(TimeDurUS curTime, bool *hasBAndAS, BytesPerMicrosecond *_B, BytesPerMicrosecond *_AS)
    {
        if(curTime>lastTransmitOfBAndAS+SYN)
        {
            *_B=0;
            *_AS=ReceiverCalculateDataArrivalRate(curTime);

            if (*_AS==UNDEFINED_TRANSFER_RATE)
            {
                *hasBAndAS=false;
            }
            else
            {
                *hasBAndAS=true;
            }
        }
        else
        {
            *hasBAndAS=false;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnSendAck(TimeDurUS, Nui32)
    {
        oldestUnsentAck=0;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::OnSendNACK(TimeDurUS, Nui32)
    {
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::UpdateWindowSizeAndAckOnAckPreSlowStart(NIId totalUserDataBytesAcked)
    {
        // During slow start, max window size is the number of full packets that have been sent out
        // CWND=(NIId) ((NIId)totalUserDataBytesSent/(NIId)MAXIMUM_MTU_INCLUDING_UDP_HEADER);
        CC_DEBUG_PRINTF_3("CWND increasing from %f to %f\n", CWND, (NIId) ((NIId)totalUserDataBytesAcked/(NIId)MAXIMUM_MTU_INCLUDING_UDP_HEADER));
        CWND=(NIId) ((NIId)totalUserDataBytesAcked/(NIId)MAXIMUM_MTU_INCLUDING_UDP_HEADER);
        if (CWND>=CWND_MAX_THRESHOLD)
        {
            CWND=CWND_MAX_THRESHOLD;

            if (AS!=UNDEFINED_TRANSFER_RATE)
                EndSlowStart();
        }
        if (CWND<CWND_MIN_THRESHOLD)
            CWND=CWND_MIN_THRESHOLD;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::UpdateWindowSizeAndAckOnAckPerSyn(TimeDurUS curTime, TimeDurUS rtt, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber)
    {
        (void) curTime;
        (void) sequenceNumber;
        if (isContinuousSend==false)
        {
            nextCongestionControlBlock=nextDatagramSequenceNumber;
            pingsLastInterval.Clear();
            return;
        }

        pingsLastInterval.Push(rtt);
        static const int intervalSize=33; // Should be odd
        if (pingsLastInterval.Size()>intervalSize)
            pingsLastInterval.Pop();
        if (GreaterThan(sequenceNumber, nextCongestionControlBlock) &&
            sequenceNumber-nextCongestionControlBlock>=intervalSize &&
            pingsLastInterval.Size()==intervalSize)
        {
            NIId slopeSum=0.0;
            NIId average=(NIId) pingsLastInterval[0];
            int sampleSize=pingsLastInterval.Size();
            for (int i=1; i < sampleSize; i++)
            {
                slopeSum+=(NIId)pingsLastInterval[i]-(NIId)pingsLastInterval[i-1];
                average+=pingsLastInterval[i];
            }
            average/=sampleSize;

            if (hadPacketlossThisBlock==true)
            {
            }
            else if (slopeSum < -.10*average)
            {
            }
            else if (slopeSum > .10*average)
            {
                IncreaseTimeBetweenSends();
            }
            else
            {
                // No packetloss over time threshhold, and rtt decreased, so send faster
                lastRttOnIncreaseSendRate = rtt;
                DecreaseTimeBetweenSends();
            }

            pingsLastInterval.Clear();
            hadPacketlossThisBlock=false;
            nextCongestionControlBlock=nextDatagramSequenceNumber;
        }

        lastRtt=rtt;
    }

    //------------------------------------------------------------------------
    NIId CCRakNetUDT::BytesPerMicrosecondToPacketsPerMillisecond(BytesPerMicrosecond in)
    {
        const BytesPerMicrosecond factor = 1.0 / (BytesPerMicrosecond) MAXIMUM_MTU_INCLUDING_UDP_HEADER;

        return in * factor;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::InitPacketArrivalHistory(void)
    {
        Nui32 i;
        for (i=0; i < CC_RAKNET_UDT_PACKET_HISTORY_LENGTH; i++)
        {
            packetArrivalHistory[i]=UNDEFINED_TRANSFER_RATE;
            packetArrivalHistoryContinuousGaps[i]=0;
        }

        packetArrivalHistoryWriteCount=0;
        continuousBytesReceived=0;
        continuousBytesReceivedStartTime=0;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::PrintLowBandwidthWarning(void)
    {
        /*
        printf("\n-------LOW BANDWIDTH -----\n");
        if (isInSlowStart==false)
        printf("SND=%f Megabytes per second\n", 1.0/SND);
        printf("Window size=%f\n", CWND);
        printf("Pipe from packet pair = %f megabytes per second\n", B);
        printf("RTT=%f milliseconds\n", RTT/1000.0);
        printf("RTT Variance=%f milliseconds\n", RTTVar/1000.0);
        printf("Retransmission=%i milliseconds\n", GetRTOForRetransmission(1)/1000);
        printf("SocketMessage arrival rate on the remote system=%f megabytes per second\n", AS);
        printf("SocketMessage arrival rate on our system=%f megabytes per second\n", ReceiverCalculateDataArrivalRate());
        printf("isInSlowStart=%i\n", isInSlowStart);
        printf("---------------\n");
        */
    }
    //------------------------------------------------------------------------
    BytesPerMicrosecond CCRakNetUDT::GetLocalReceiveRate(TimeDurUS currentTime) const
    {
        return ReceiverCalculateDataArrivalRate(currentTime);
    }
    //------------------------------------------------------------------------
    NIId CCRakNetUDT::GetRTT(void) const
    {
        if (RTT==UNSET_TIME_US)
            return 0.0;
        return RTT;
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::CapMinSnd()
    {
        if (SND > 500)
        {
            SND=500;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::IncreaseTimeBetweenSends(void)
    {
        // In order to converge, bigger numbers have to increase slower and decrease faster
        // SND==500 then increment is .02
        // SND==0 then increment is near 0
        // (SND+1.0) brings it to the range of 1 to 501
        // Square the number, which is the range of 1 to 251001
        // Divide by 251001, which is the range of 1/251001 to 1

        NIId increment;
        increment = .02 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
        // SND=500 then increment=.02
        // SND=0 then increment=near 0
        SND*=(1.02 - increment);

        // SND=0 then fast increase, slow decrease
        // SND=500 then slow increase, fast decrease
        CapMinSnd();
    }
    //------------------------------------------------------------------------
    void CCRakNetUDT::DecreaseTimeBetweenSends(void)
    {
        NIId increment;
        increment = .01 * ((SND+1.0) * (SND+1.0)) / (501.0*501.0) ;
        // SND=500 then increment=.01
        // SND=0 then increment=near 0
        SND*=(.99 - increment);
    }
    //------------------------------------------------------------------------
}
}
#endif

#if N_Socket_SlidingWindow == 1

static const NIId UNSET_TIME_US     = -1;
static const TimeDurUS SYN          = 10;

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    CCRakNetSlidingWindow::CCRakNetSlidingWindow()
    {
    }
    //------------------------------------------------------------------------
    CCRakNetSlidingWindow::~CCRakNetSlidingWindow()
    {

    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::Init(TimeDurUS curTime, Nui32 maxDatagramPayload)
    {
        (void) curTime;

        lastRtt=estimatedRTT=deviationRtt=UNSET_TIME_US;
        N_assert(maxDatagramPayload <= N_Net_MaxMtuSize);
        MAXIMUM_MTU_INCLUDING_UDP_HEADER=maxDatagramPayload;
        cwnd=maxDatagramPayload;
        ssThresh=0.0;
        oldestUnsentAck=0;
        nextDatagramSequenceNumber=0;
        nextCongestionControlBlock=0;
        backoffThisBlock=speedUpThisBlock=false;
        expectedNextSequenceNumber=0;
        _isContinuousSend=false;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::update(TimeDurUS curTime, bool hasDataToSendOrResend)
    {
        (void) curTime;
        (void) hasDataToSendOrResend;
    }
    //------------------------------------------------------------------------
    int CCRakNetSlidingWindow::GetRetransmissionBandwidth(TimeDurUS curTime,
        TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend)
    {
        (void) curTime;
        (void) isContinuousSend;
        (void) timeSinceLastTick;

        return ackremain;
    }
    //------------------------------------------------------------------------
    int CCRakNetSlidingWindow::GetTransmissionBandwidth(TimeDurUS curTime,
        TimeDurUS timeSinceLastTick, Nui32 ackremain, bool isContinuousSend)
    {
        (void) curTime;
        (void) timeSinceLastTick;

        _isContinuousSend=isContinuousSend;

        if(ackremain <= cwnd)
            return cwnd - ackremain;
        else
            return 0;
    }
    //------------------------------------------------------------------------
    bool CCRakNetSlidingWindow::ShouldSendACKs(TimeDurUS curTime, 
        TimeDurUS estimatedTimeToNextTick)
    {
        TimeDurUS rto = GetSenderRTOForACK();
        (void) estimatedTimeToNextTick;

        // iphone crashes on comparison between NIId and int64 http://www.jenkinssoftware.com/forum/index.php?topic=2717.0
        if (rto==(TimeDurUS) UNSET_TIME_US)
        {
            // Unknown how long until the remote system will retransmit, so better send right away
            return true;
        }

        return curTime >= oldestUnsentAck + SYN;
    }
    //------------------------------------------------------------------------
    DatagramSequenceNumberType CCRakNetSlidingWindow::GetNextDatagramSequenceNumber(void)
    {
        return nextDatagramSequenceNumber;
    }
    //------------------------------------------------------------------------
    DatagramSequenceNumberType CCRakNetSlidingWindow::GetAndIncrementNextDatagramSequenceNumber(void)
    {
        DatagramSequenceNumberType dsnt=nextDatagramSequenceNumber;
        nextDatagramSequenceNumber++;
        return dsnt;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnSendBytes(TimeDurUS curTime, Nui32 numBytes)
    {
        (void) curTime;
        (void) numBytes;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnGotPacketPair(DatagramSequenceNumberType datagramSequenceNumber,
        Nui32 sizeInBytes, TimeDurUS curTime)
    {
        (void) curTime;
        (void) sizeInBytes;
        (void) datagramSequenceNumber;
    }
    //------------------------------------------------------------------------
    bool CCRakNetSlidingWindow::OnGotPacket(DatagramSequenceNumberType datagramSequenceNumber,
        bool isContinuousSend, TimeDurUS curTime, Nui32 sizeInBytes, Nui32 *skippedMessageCount)
    {
        (void) curTime;
        (void) sizeInBytes;
        (void) isContinuousSend;

        if (oldestUnsentAck==0)
            oldestUnsentAck=curTime;

        if (datagramSequenceNumber==expectedNextSequenceNumber)
        {
            *skippedMessageCount=0;
            expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
        }
        else if (GreaterThan(datagramSequenceNumber, expectedNextSequenceNumber))
        {
            *skippedMessageCount=datagramSequenceNumber-expectedNextSequenceNumber;
            // Sanity check, just use timeout resend if this was really valid
            if (*skippedMessageCount>1000)
            {
                // During testing, the nat punchthrough server got 51200 on the first packet. I have no idea where this comes from, but has happened twice
                if (*skippedMessageCount>(Nui32)50000)
                    return false;
                *skippedMessageCount=1000;
            }
            expectedNextSequenceNumber=datagramSequenceNumber+(DatagramSequenceNumberType)1;
        }
        else
        {
            *skippedMessageCount=0;
        }

        return true;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnResend(TimeDurUS curTime, TimeDurUS lactact)
    {
        (void) curTime;
        (void) lactact;

        if (_isContinuousSend && backoffThisBlock==false && cwnd>MAXIMUM_MTU_INCLUDING_UDP_HEADER*2)
        {
            ssThresh = cwnd / 2;
            if (ssThresh < MAXIMUM_MTU_INCLUDING_UDP_HEADER)
                ssThresh = MAXIMUM_MTU_INCLUDING_UDP_HEADER;
            cwnd = MAXIMUM_MTU_INCLUDING_UDP_HEADER;

            // Only backoff once per period
            nextCongestionControlBlock=nextDatagramSequenceNumber;
            backoffThisBlock=true;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnNAK(TimeDurUS curTime, DatagramSequenceNumberType nakSequenceNumber)
    {
        (void) nakSequenceNumber;
        (void) curTime;

        if (_isContinuousSend && backoffThisBlock==false)
        {
            // Start congestion avoidance
            ssThresh = cwnd / 2;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::onReceiveAck(TimeDurUS curTime, TimeDurUS rtt, bool hasBAndAS, BytesPerMicrosecond _B, BytesPerMicrosecond _AS,
        NIId totalUserDataBytesAcked, bool isContinuousSend, DatagramSequenceNumberType sequenceNumber)
    {
        (void) _B;
        (void) totalUserDataBytesAcked;
        (void) _AS;
        (void) hasBAndAS;
        (void) curTime;
        (void) rtt;

        lastRtt = (NIId)rtt;
        if (estimatedRTT == UNSET_TIME_US)
        {
            estimatedRTT = (NIId) rtt;
            deviationRtt = (NIId)rtt;
        }
        else
        {
            NIId d = .05;
            NIId difference = rtt - estimatedRTT;
            estimatedRTT = estimatedRTT + d * difference;
            deviationRtt = deviationRtt + d * (abs(difference) - deviationRtt);
        }

        _isContinuousSend = isContinuousSend;

        if (isContinuousSend == false)
            return;

        bool isNewCongestionControlPeriod;
        isNewCongestionControlPeriod = GreaterThan(sequenceNumber, nextCongestionControlBlock);

        if(isNewCongestionControlPeriod)
        {
            backoffThisBlock = false;
            speedUpThisBlock = false;
            nextCongestionControlBlock = nextDatagramSequenceNumber;
        }

        if(IsInSlowStart())
        {
            cwnd += MAXIMUM_MTU_INCLUDING_UDP_HEADER;
            if (cwnd > ssThresh && ssThresh!=0)
                cwnd = ssThresh + MAXIMUM_MTU_INCLUDING_UDP_HEADER*MAXIMUM_MTU_INCLUDING_UDP_HEADER/cwnd;
        }
        else if(isNewCongestionControlPeriod)
        {
            cwnd+=MAXIMUM_MTU_INCLUDING_UDP_HEADER * MAXIMUM_MTU_INCLUDING_UDP_HEADER/cwnd;
        }
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnDuplicateAck(TimeDurUS curTime, DatagramSequenceNumberType sequenceNumber)
    {
        (void) curTime;
        (void) sequenceNumber;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnSendAckGetBAndAS(TimeDurUS curTime, bool * hasBAndAS,
        BytesPerMicrosecond * _B, BytesPerMicrosecond * _AS)
    {
        (void) curTime;
        (void) _B;
        (void) _AS;

        *hasBAndAS=false;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnSendAck(TimeDurUS curTime, Nui32 numBytes)
    {
        (void) curTime;
        (void) numBytes;

        oldestUnsentAck=0;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::OnSendNACK(TimeDurUS curTime, Nui32 numBytes)
    {
        (void) curTime;
        (void) numBytes;
    }
    //------------------------------------------------------------------------
    TimeDurUS CCRakNetSlidingWindow::GetRTOForRetransmission(Nui8 sendCount) const
    {
        (void) sendCount;
        const TimeDurUS maxThreshold = 2000;
        const TimeDurUS additionalVariance = 30;
        if (estimatedRTT == UNSET_TIME_US)
            return maxThreshold;

        NIId u = 2.0f;
        NIId q = 4.0f;

        TimeDurUS threshhold = (TimeDurUS)(u * estimatedRTT + q * deviationRtt) + additionalVariance;
        if (threshhold > maxThreshold)
            return maxThreshold;
        return threshhold;
    }
    //------------------------------------------------------------------------
    void CCRakNetSlidingWindow::SetMTU(Nui32 bytes)
    {
        N_assert(bytes < N_Net_MaxMtuSize);
        MAXIMUM_MTU_INCLUDING_UDP_HEADER=bytes;
    }
    //------------------------------------------------------------------------
    Nui32 CCRakNetSlidingWindow::GetMTU(void) const
    {
        return MAXIMUM_MTU_INCLUDING_UDP_HEADER;
    }
    //------------------------------------------------------------------------
    BytesPerMicrosecond CCRakNetSlidingWindow::GetLocalReceiveRate(TimeDurUS currentTime) const
    {
        (void) currentTime;

        return 0; // TODO
    }
    //------------------------------------------------------------------------
    NIId CCRakNetSlidingWindow::GetRTT(void) const
    {
        if (lastRtt==UNSET_TIME_US)
            return 0.0;
        return lastRtt;
    }
    //------------------------------------------------------------------------
    bool CCRakNetSlidingWindow::GreaterThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
    {
        // a > b?
        const DatagramSequenceNumberType halfSpan =(DatagramSequenceNumberType) (((DatagramSequenceNumberType)(const Nui32)-1)/(DatagramSequenceNumberType)2);
        return b!=a && b-a>halfSpan;
    }
    //------------------------------------------------------------------------
    bool CCRakNetSlidingWindow::LessThan(DatagramSequenceNumberType a, DatagramSequenceNumberType b)
    {
        // a < b?
        const DatagramSequenceNumberType halfSpan = ((DatagramSequenceNumberType)(const Nui32)-1)/(DatagramSequenceNumberType)2;
        return b!=a && b-a<halfSpan;
    }
    //------------------------------------------------------------------------
    Nui64 CCRakNetSlidingWindow::GetBytesPerSecondLimitByCongestionControl(void) const
    {
        return 0; // TODO
    }
    //------------------------------------------------------------------------
    TimeDurUS CCRakNetSlidingWindow::GetSenderRTOForACK(void) const
    {
        if (lastRtt==UNSET_TIME_US)
            return (TimeDurUS) UNSET_TIME_US;
        return (TimeDurUS)(lastRtt + SYN);
    }
    //------------------------------------------------------------------------
    bool CCRakNetSlidingWindow::IsInSlowStart(void) const
    {
        return cwnd <= ssThresh || ssThresh==0;
    }
    //------------------------------------------------------------------------
}
}
#endif


