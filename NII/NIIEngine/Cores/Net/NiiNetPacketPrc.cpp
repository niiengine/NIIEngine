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

#include "NiiPreProcess.h"
#include "NiiNetPacketPrc.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"
#include "Rand.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // Packet
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    Packet::Packet()
    {
        mACKMark = (MessageNumberType)(Nui32) - 1;
        mAllocType = Packet::NORMAL;
        mSendCount = 0;
        mMakeSureInit = false;
        mLastAction = 0;
        mSplitCount = 0;
        mSplitIndex = 0;
        mSplitID = 0;
        mData = 0;
    }
    //------------------------------------------------------------------------
    Packet::Packet(Nui8 * data)
    {
        mAllocType = Packet::NORMAL;
        mData = data;
    }
    //------------------------------------------------------------------------
    Packet::Packet(NCount size, bool stack)
    {
        if(stack && size <= sizeof(mStackData))
        {
            mAllocType = Packet::STACK;
            mData = mStackData;
        }
        else
        {
            mAllocType = Packet::NORMAL;
            mData = (Nui8 *)N_alloc(size);
        }
    }
    //------------------------------------------------------------------------
    Packet::Packet(PacketDataRef ** ref, Nui8 * data, Nui8 * offset)
    {
        mAllocType = Packet::Share;
        mData = offset;
        if(*ref == 0)
        {
            *ref = N_alloc_t(PacketDataRef, 1);
            (*ref)->mRefCount = 1;
            (*ref)->mShare = data;
        }
        else
        {
            (*ref)->mRefCount++;
        }
        mRefData = *ref;
    }
    //------------------------------------------------------------------------
    Packet::~Packet()
    {
        destroy();
    }
    //------------------------------------------------------------------------
    Packet * Packet::create(NetSerializer * src, TimeDurUS time)
    {
        if (src->getRemainCount() < sizeof(MessageNumberType) * 8)
            return 0;

        Nui8 tempChar;
        Nui16 s;
        bool hasSplitPacket = false;
        src->readAlign();
        src->readBit(&tempChar, 3);
        bool readSuccess = src->read(hasSplitPacket); // read 1 bit to indicate if mSplitCount>0
        N_assert(readSuccess);
        src->readAlign();
        src->read(s);
        Packet * packet = N_new Packet((Nui8 *)N_alloc(N_B_TO_b(s)));
        packet->mPacketType = (PacketType)tempChar;
        packet->mCreate = time;
        packet->mBitCount = s; // Length of message (2 bytes)
        if (packet->mPacketType == PT_BE_SURE || packet->mPacketType == PT_ONE_MAKE_SURE ||
            packet->mPacketType == PT_ORDER_MAKE_SURE)
        {
            src->read(packet->mACKMark); // SocketMessage sequence number
        }
        else
        {
            packet->mACKMark = (MessageNumberType)(const Nui32)-1;
        }
        src->readAlign(); // Potentially nothing else to read

        if (packet->mPacketType == PT_ONE_WILL_LOST || packet->mPacketType == PT_ONE_MAKE_SURE)
        {
            src->read(packet->mSequence);
            // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE.
        }

        if (packet->mPacketType == PT_ONE_WILL_LOST ||
            packet->mPacketType == PT_ONE_MAKE_SURE ||
            packet->mPacketType == PT_ORDER_MAKE_SURE ||
            packet->mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            src->read(packet->mOrder);
            // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE. 4 bytes.
            readSuccess = src->read((Nui8)packet->mPacketCh);
            // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE. 5 bits needed, read one byte
        }
        else
        {
            packet->mPacketCh = 0;
        }

        if (hasSplitPacket)
        {
            src->read((Nui32)packet->mSplitCount);
            // Only needed if mSplitCount>0. 4 bytes
            src->read((Nui16)packet->mSplitID);
            // Only needed if mSplitCount>0.
            readSuccess = src->read((Nui32)packet->mSplitIndex);
            // Only needed if mSplitCount>0. 4 bytes
            N_assert(readSuccess);
        }
        else
        {
            packet->mSplitCount = 0;
        }

        if (readSuccess == false || packet->mBitCount == 0 ||
            packet->mPacketType >= PT_COUNT || packet->mPacketCh >= 32 ||
            (hasSplitPacket && (packet->mSplitIndex >= packet->mSplitCount)))
        {
            N_assert(0, "Encoding is garbage");
            N_delete packet;
            return 0;
        }

        N_assert(N_B_TO_b(packet->mBitCount) < N_Net_MaxMtuSize);

        if (packet->mData == 0)
        {
            N_assert(0, "Out of memory");
            N_delete packet;
            return 0;
        }

        // Set the last byte to 0 so if readBit does not read a multiple of 8 the last bits are 0'ed out
        packet->mData[N_B_TO_b(packet->mBitCount) - 1] = 0;

        if(!src->read(packet->mData, N_B_TO_b(packet->mBitCount)))
        {
            N_assert(0, "Couldn't read all the data");
            N_delete packet;
            return 0;
        }
        return packet;
    }
    //------------------------------------------------------------------------
    Packet * Packet::create(Packet * src, NCount offset, NCount size, TimeDurUS time)
    {
        Packet * copy = N_new Packet(N_B_TO_b(size), false);

        if (size > 0)
        {
            memcpy(copy->mData, src->mData + offset, size);
        }
        else
        {
            copy->mData = 0;
        }
        copy->mBitCount = size << 3;
        copy->mCreate = time;
        copy->mLastAction = 0;
        copy->mOrder = src->mOrder;
        copy->mSequence = src->mSequence;
        copy->mPacketCh = src->mPacketCh;
        copy->mACKMark = src->mACKMark;
        copy->mPacketLevel = src->mPacketLevel;
        copy->mPacketType = src->mPacketType;
#if N_Socket_PreEnlargePacket == 1
        copy->mSplitCount = src->mSplitCount;
        copy->mSplitID = src->mSplitID;
        copy->mSplitIndex = src->mSplitIndex;
#endif

        return copy;
    }
    //------------------------------------------------------------------------
    NBitCount Packet::getHeaderSize()
    {
        NBitCount bitLength;
        bitLength = 8 * 1;
        bitLength += 8 * 2;

        if (mPacketType == PT_BE_SURE ||
            mPacketType == PT_ONE_MAKE_SURE ||
            mPacketType == PT_ORDER_MAKE_SURE ||
            mPacketType == PT_RECEIPT_MAKE_SURE ||
            mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            bitLength += 8 * 3;
            // bitStream->write(mACKMark); // SocketMessage sequence number
        }

        if (mPacketType == PT_ONE_WILL_LOST || mPacketType == PT_ONE_MAKE_SURE)
        {
            bitLength += 8 * 3;;
            // bitStream->write(mSequence); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE.
        }

        if (mPacketType == PT_ONE_WILL_LOST ||
            mPacketType == PT_ONE_MAKE_SURE ||
            mPacketType == PT_ORDER_MAKE_SURE ||
            mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            bitLength += 8 * 3;
            // bitStream->write(mOrder); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE.
            bitLength += 8 * 1;
            // tempChar = mPacketCh; bitStream->write((Nui8)tempChar); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE. 5 bits needed, write one byte
        }
        if (mSplitCount > 0)
        {
            bitLength += 8 * 4;
            // bitStream->write((Nui32)mSplitCount); N_assert(sizeof(SplitPacketIndexType)==4); // Only needed if mSplitCount>0. 4 bytes
            bitLength += 8 * sizeof(SplitPacketIdType);
            // bitStream->write((Nui6)mSplitID); N_assert(sizeof(SplitPacketIdType)==2); // Only needed if mSplitCount>0.
            bitLength += 8 * 4;
            // bitStream->write((Nui32)mSplitIndex); // Only needed if mSplitCount>0. 4 bytes
        }
        return bitLength;
    }
    //------------------------------------------------------------------------
    NBitCount Packet::read(NetSerializer * target) const
    {
        NBitCount start = target->getWriteOffset();
        Nui8 tempChar;

        target->writeAlign(); // Potentially unaligned
        if (mPacketType == PT_RECEIPT_WILL_LOST)
            tempChar = PT_WILL_LOST;
        else if (mPacketType == PT_RECEIPT_MAKE_SURE)
            tempChar = PT_BE_SURE;
        else if (mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
            tempChar = PT_ORDER_MAKE_SURE;
        else
            tempChar = (Nui8)mPacketType;

        target->writeBit(&tempChar, 3, true); // 3 bits to write ptype.

        bool hasSplitPacket = mSplitCount > 0;
        target->write(hasSplitPacket); // write 1 bit to indicate if mSplitCount>0
        target->writeAlign();
        N_assert(mBitCount < 65535);
        Nui16 s = (Nui16)mBitCount;
        target->write(s);
        if (mPacketType == PT_BE_SURE || mPacketType == PT_ONE_MAKE_SURE ||
            mPacketType == PT_ORDER_MAKE_SURE || mPacketType == PT_RECEIPT_MAKE_SURE ||
            mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            target->write(mACKMark); // Used for all reliable types
        }

        target->writeAlign(); // Potentially nothing else to write

        if (mPacketType == PT_ONE_WILL_LOST ||
            mPacketType == PT_ONE_MAKE_SURE)
        {
            target->write(mSequence); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE.
        }

        if (mPacketType == PT_ONE_WILL_LOST ||
            mPacketType == PT_ONE_MAKE_SURE ||
            mPacketType == PT_ORDER_MAKE_SURE ||
            mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            target->write(mOrder); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE.
            tempChar = mPacketCh;
            target->write((Nui8)tempChar); // Used for PT_ONE_WILL_LOST, PT_ONE_MAKE_SURE, PT_ORDER_MAKE_SURE. 5 bits needed, write one byte
        }

        if (mSplitCount > 0)
        {
            target->write((Nui32)mSplitCount);
            N_assert(sizeof(SplitPacketIndexType) == 4);
            target->write((Nui16)mSplitID);
            N_assert(sizeof(SplitPacketIdType) == 2);
            target->write((Nui32)mSplitIndex);
        }

        target->write(mData, N_B_TO_b(mBitCount));

        return target->getWriteOffset() - start;
    }
    //------------------------------------------------------------------------
    void Packet::destroy()
    {
        switch (mAllocType)
        {
        case Packet::Share:
        {
            if (mRefData == 0)
                return;

            mRefData->mRefCount--;
            if (mRefData->mRefCount == 0)
            {
                N_free(mRefData->mShare);
                mRefData->mShare = 0;
                N_free(mRefData);
                mRefData = 0;
            }
            break;
        }
        case Packet::NORMAL:
        {
            if(mData == 0)
                return;

            N_free(mData);
            mData = 0;
            break;
        }
        default:
            mData = 0;
        }
    }
    //------------------------------------------------------------------------
    void Packet::split(PacketList & packet, NCount blocksize)
    {
    }
    //------------------------------------------------------------------------
    Packet * Packet::combine()
    {
    }
    //------------------------------------------------------------------------
    
    struct SplitPacketChannel
    {
        TimeDurUS lastUpdateTime;

        DataStructures::List<Packet *> splitPacketList;
#if N_Socket_PreEnlargePacket == 1
        Packet * returnedPacket;
        bool gotFirstPacket;
        Nui32 stride;
        Nui32 splitPacketsArrived;
#else
        // This is here for progress notifications, since progress notifications return the first packet data, if available
        Packet * firstPacket;
#endif
    };

    // Helper class
    struct BPSTracker
    {
        BPSTracker(){ Reset(); }
        ~BPSTracker(){}
        
        inline void Reset(){ total1 = lastSec1 = 0; dataQueue.Clear(file,line);}
        
        inline void Push1(TimeDurUS time, Nui64 value1)
        {
            dataQueue.Push(TimeAndValue2(time, value1));
            total1 += value1;
            lastSec1 += value1;
        }
        inline Nui64 GetBPS1(TimeDurUS time)
        {
            (void)time;
            return lastSec1;
        }
        inline Nui64 GetBPS1Threadsafe(TimeDurUS time)
        {
            (void)time;
            return lastSec1;
        }
        inline Nui64 GetTotal1() const { return total1; }

        void ClearExpired1(TimeDurUS time)
        {
            while(dataQueue.IsEmpty() == false && dataQueue.Peek().time + 1000 < time)
            {
                lastSec1 -= dataQueue.Peek().value1;
                dataQueue.Pop();
            }
        }

        struct TimeAndValue2
        {
            TimeAndValue2(){}
            ~TimeAndValue2(){}
            TimeAndValue2(TimeDurUS t, Nui64 v1): value1(v1), time(t){}
            Nui64 value1;
            TimeDurUS time;
        };

        Nui64 total1;
        Nui64 lastSec1;
        DataStructures::Queue<TimeAndValue2> dataQueue;
    };
    //------------------------------------------------------------------------
    // Can't figure out which library has this function on the PS3
    NIId Ceil(NIId d)
    {
        if(((NIId)((int)d)) == d)
            return d;
        return (int) (d + 1.0);
    }
    //------------------------------------------------------------------------
    static const int DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE = 512;
	//------------------------------------------------------------------------
    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
        static Nui32 packetNumber = 0;
        static FILE * fp = 0;
    #endif
    //------------------------------------------------------------------------
    struct DatagramHeaderFormat
    {
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
        TimeDurUS sourceSystemTime;
    #endif
        DatagramSequenceNumberType mMark;

        NIIf AS; // Data arrival rate
        bool isACK;
        bool isNAK;
        bool isPacketPair;
        bool hasBAndAS;
        bool isContinuousSend;
        bool needsBAndAs;
        bool isValid; // To differentiate between what I serialized, and offline data
    //------------------------------------------------------------------------
        static NBitCount GetDataHeaderBitLength()
        {
            return N_b_TO_B(GetDataHeaderByteLength());
        }
    //------------------------------------------------------------------------
        static Nui32 GetDataHeaderByteLength()
        {
            return 2 + 3 + sizeof(NIIf) * 1
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                + sizeof(RakNetTimeMS)
    #endif
                ;
        }
    //------------------------------------------------------------------------
        void read(NetSerializer * b) const
        {
            b->write1(); // IsValid
            if (isACK)
            {
                b->write1();
                b->write(hasBAndAS);
                b->writeAlign();
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                TimeDurMS timeMSLow = (TimeDurMS)sourceSystemTime & 0xFFFFFFFF;
                b->write(timeMSLow);
    #endif
                if (hasBAndAS)
                {
                    b->write(AS);
                }
            }
            else if (isNAK)
            {
                b->write0();
                b->write1();
            }
            else
            {
                b->write0();
                b->write0();
                b->write(isPacketPair);
                b->write(isContinuousSend);
                b->write(needsBAndAs);
                b->writeAlign();
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                TimeDurMS timeMSLow = (TimeDurMS)sourceSystemTime & 0xFFFFFFFF;
                b->write(timeMSLow);
    #endif
                b->write(mMark);
            }
        }
    //------------------------------------------------------------------------
        void write(const NetSerializer * b)
        {
            b->read(isValid);
            b->read(isACK);
            if (isACK)
            {
                isNAK = false;
                isPacketPair = false;
                b->read(hasBAndAS);
                b->readAlign();
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                TimeDurMS timeMS;
                b->read(timeMS);
                sourceSystemTime = (TimeDurUS)timeMS;
    #endif
                if (hasBAndAS)
                {
                    b->read(AS);
                }
            }
            else
            {
                b->read(isNAK);
                if(isNAK)
                {
                    isPacketPair = false;
                }
                else
                {
                    b->read(isPacketPair);
                    b->read(isContinuousSend);
                    b->read(needsBAndAs);
                    b->readAlign();
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                    TimeDurMS timeMS;
                    b->read(timeMS);
                    sourceSystemTime = (TimeDurUS)timeMS;
    #endif
                    b->read(mMark);
                }
            }
        }
    };
    //------------------------------------------------------------------------
    #if !defined(__GNUC__) && !defined(__ARMCC)
        #pragma warning(disable:4702)   // unreachable code
    #endif
    //------------------------------------------------------------------------
    #if defined (DEBUG_SPLIT_PACKET_PROBLEMS)
    static int waitFlag = -1;
    #endif
    //------------------------------------------------------------------------
    int SplitPacketChannelComp(SplitPacketIdType const & key, SplitPacketChannel * const & data)
    {
    #if N_Socket_PreEnlargePacket == 1
        if(key < data->returnedPacket->mSplitID)
            return -1;
        if(key == data->returnedPacket->mSplitID)
            return 0;
    #else
        if(key < data->splitPacketList[0]->mSplitID)
            return -1;
        if(key == data->splitPacketList[0]->mSplitID)
            return 0;
    #endif
        return 1;
    }
    //------------------------------------------------------------------------
    int SplitPacketIndexComp(SplitPacketIndexType const & key, Packet * const & data)
    {
        if(key < data->mSplitIndex)
            return -1;
        if(key == data->mSplitIndex)
            return 0;
        return 1;
    }
    //------------------------------------------------------------------------
    // Add 21 to the default MTU so if we encrypt it can hold potentially 21 more bytes of extra data + padding.
    PacketPrc::PacketPrc():
        mUsrMTU(512)
    {
#ifdef _DEBUG
        mTimeOut = 30000;
#else
        mTimeOut = 10000;
#endif

#ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
        if(fp == 0 && 0)
        {
            fp = fopen("reliableorderedoutput.txt", "wt");
        }
#endif
        reset();
        mListener = N_new PacketListener();
    }
    //------------------------------------------------------------------------
    PacketPrc::~PacketPrc()
    {
        N_delete mListener;
        cleanup(true);
    }
    //------------------------------------------------------------------------
    void PacketPrc::reset(bool reset, NCount mtu, bool _useSecurity)
    {
        cleanup(true); // true because making a memory reset pending in the update cycle causes resets after reconnects.  Instead, just call reset from a single thread
        if(reset)
        {
            reset();
#if LIBCAT_SECURITY == 1
            useSecurity = _useSecurity;

            if(_useSecurity)
                mtu -= cat::AuthenticatedEncryption::OVERHEAD_BYTES;
#else
            (void) _useSecurity;
#endif // LIBCAT_SECURITY
            mCongestion.Init(N_Engine().getTimer().getUS(), mtu - UDP_HEADER_SIZE);

            NCount val = mCongestion.GetMTU() - DatagramHeaderFormat::GetDataHeaderByteLength();

#if LIBCAT_SECURITY == 1
        if(useSecurity)
            mUsrMTU = val - cat::AuthenticatedEncryption::OVERHEAD_BYTES;
#endif
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::setTimeOut(TimeDurMS time)
    {
        mTimeOut = time;
    }
    //------------------------------------------------------------------------
    TimeDurMS PacketPrc::getTimeOut()
    {
        return mTimeOut;
    }
    //------------------------------------------------------------------------
    void PacketPrc::reset()
    {
        memset(mOrderExport, 0, N_NET_CH * sizeof(OrderingIndexType));
        memset(mSequenceExport, 0, N_NET_CH * sizeof(OrderingIndexType));
        memset(mOrderImport, 0, N_NET_CH * sizeof(OrderingIndexType));
        memset(mSequenceImport, 0, N_NET_CH * sizeof(OrderingIndexType));
        memset(&mProfile, 0, sizeof(mProfile));
        memset(&mOrderQueueMark, 0, sizeof(mOrderQueueMark));

        mProfile.connectionStartTime = N_Engine().getTimer().getUS();
        mNextSplitID = 0;
        elapsedTimeSinceLastUpdate = 0;
        throughputCapCountdown = 0;
        mACKSendMark = 0;
        mDebugSendMark = 0;
        timeToNextUnreliableCull = 0;
        mWillLostHead = 0;
        lastUpdateTime = N_Engine().getTimer().getUS();
        bandwidthExceededStatistic = false;
        remoteSystemTime = 0;
        mMessageTimeOut = 0;
        mResetBSPTime = 0;

        // Disable packet pairs
        countdownToNextPacketPair = 15;

        mTimeOutValid = false;
        timeOfLastContinualSend = 0;
        mLastRecevie = N_Engine().getTimer().getMS();
        mProfile.messagesInResendBuffer = 0;
        mProfile.bytesInResendBuffer = 0;

        receivedPacketsBaseIndex = 0;
        resetReceivedPackets = true;
        receivePacketCount = 0;
        ackPingIndex = 0;

        mCheckACK = 0;
        mResendHead = 0;
        totalUserDataBytesAcked = 0;

        mSendMark = 0;

        for(NCount i = 0; i < PL_COUNT; ++i)
        {
            mUploadWeight[i] = (1 << i) * i + i;
        }

        for(NCount i = 0; i < PL_COUNT; ++i)
        {
            mProfile.messageInSendBuffer[i] = 0;
            mProfile.bytesInSendBuffer[i] = 0.0;
        }

        for(NCount i = 0; i < RNS_PER_SECOND_METRICS_COUNT; ++i)
        {
            mBSP[i].Reset();
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::cleanup(bool)
    {
        removeFragment();
        NCount i, j;
        for (i = 0; i < mSplitPackets.Size(); ++i)
        {
            for (j = 0; j < mSplitPackets[i]->splitPacketList.Size(); ++j)
            {
                N_delete mSplitPackets[i]->splitPacketList[j];
            }
#if N_Socket_PreEnlargePacket == 1
            if (mSplitPackets[i]->returnedPacket)
            {
                N_delete mSplitPackets[i]->returnedPacket;
            }
#endif
            N_delete mSplitPackets[i];
        }
        mSplitPackets.Clear(false);

        Packet * packet;
        while (mDownload.Size() > 0)
        {
            packet = mDownload.Pop();
            N_delete packet;
        }
        mDownload.ClearAndForceAllocation(32);
        for (i = 0; i < N_NET_CH; ++i)
        {
            for (j = 0; j < mOrderQueue[i].Size(); ++j)
            {
                N_delete mOrderQueue[i][j];
            }
            mOrderQueue[i].Clear(true);
        }

        memset(mResend, 0, sizeof(mResend));
        mProfile.messagesInResendBuffer = 0;
        mProfile.bytesInResendBuffer = 0;

        if(mResendHead)
        {
            Packet * prev;
            Packet * iter = mResendHead;

#ifdef _MSC_VER
#pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#endif
            while (1)
            {
                if (iter->mData)
                    iter->destroy();
                prev = iter;
                iter = iter->mNextMakeSure;
                if (iter == mResendHead)
                {
                    N_delete prev;
                    break;
                }
                N_delete prev;
            }
            mResendHead = 0;
        }
        mCheckACK = 0;

        for(j = 0; j < mUploadBuffer.Size(); ++j)
        {
            if (mUploadBuffer[j]->mData)
                N_delete mUploadBuffer[j];
        }
        mUploadBuffer.Clear(true);

        mWillLostReceipt.Clear(false);
        mSend.Clear(false);
        mSend.Preallocate(512);
        mFree.Clear(false);
        mFree.Preallocate(512);
        mUploadCountQueue.Clear(false);
        mUploadCountQueue.Preallocate(128);
        mUploadSizeQueue.Clear(false);
        mUploadSizeQueue.Preallocate(128);

        while(mDatagram.Size())
        {
            removeMessageNode(mSendMark);
            mDatagram.Pop();
            mSendMark++;
        }
        mSendMark = 0;

        mACK.Clear();
        mNAK.Clear();

        mWillLostHead = 0;
    }
    //------------------------------------------------------------------------
    bool PacketPrc::process(const Address * address, const Nui8 * buffer, NCount length,
        NCount mtu, SocketObj * s, RakNetRandom * rnr, TimeDurUS readtime, NetSerializer & upload)
    {
        readtime /= 1000;

        mBSP[ACTUAL_BYTES_RECEIVED].Push1(readtime, length);

        if(length <= 2 || buffer == 0)   // Length of 1 is a connection request resend that we just ignore
        {
            mListener->onReport("length <= 2 || buffer == 0", N_b_TO_B(length), address, true);
            return true;
        }

        mLastRecevie = N_Engine().getTimer().getMS();

        DatagramSequenceNumberType holeCount;
        NCount i;

#if LIBCAT_SECURITY == 1
        if(useSecurity)
        {
            Nui32 received = length;

            if(!auth_enc.Decrypt((cat::u8 *)buffer, received))
                return false;

            length = received;
        }
#endif
        NetSerializer socketData(buffer, length, false); // Convert the incoming data to a bitstream for easy parsing

        DatagramHeaderFormat dhf;
        dhf.write(&socketData);
        if(dhf.isValid == false)
        {
            mListener->onReport("dhf.isValid == false", N_b_TO_B(length), address, true);
            return true;
        }
        if(dhf.isACK)
        {
            DatagramSequenceNumberType mark;

#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
            TimeDurMS timeMSLow = (TimeDurMS)readtime & 0xFFFFFFFF;
            TimeDurUS rtt = timeMSLow - dhf.sourceSystemTime;

            if(rtt > 10000)
            {
                // Sanity check. This could happen due to type overflow, especially since I only send the low 4 bytes to reduce bandwidth
                rtt = (TimeDurUS) mCongestion.GetRTT();
            }
#endif

#ifdef _DEBUG
            if(dhf.hasBAndAS == false)
            {
                dhf.AS = 0;
            }
#endif

            mTempACK.Clear();
            if(mTempACK.write(&socketData) == false)
            {
                mListener->onReport("mTempACK.import failed", N_b_TO_B(length), address, true);
                return false;
            }

            for(i = 0; i < mTempACK.ranges.Size(); ++i)
            {
                if(mTempACK.ranges[i].minIndex > mTempACK.ranges[i].maxIndex)
                {
                    N_assert(mTempACK.ranges[i].minIndex <= mTempACK.ranges[i].maxIndex);

                    mListener->onReport("mTempACK minIndex > maxIndex", N_b_TO_B(length), address, true);
                    return false;
                }
                for(mark = mTempACK.ranges[i].minIndex;
                    mark >= mTempACK.ranges[i].minIndex && mark <= mTempACK.ranges[i].maxIndex;
                    ++mark)
                {
                    TimeDurUS whenSent;

                    if(mWillLostReceipt.Size() > 0)
                    {
                        Nui32 k = 0;
                        while(k < mWillLostReceipt.Size())
                        {
                            if(mWillLostReceipt[k].mMark == mark)
                            {
                                Packet * ackReceipt = N_new Packet(5, false);
                                ackReceipt->mBitCount = N_b_TO_B(5);
                                ackReceipt->mData[0] = (Token)ID_SND_RECEIPT_ACKED;
                                memcpy(ackReceipt->mData+sizeof(Token), &mWillLostReceipt[k].mReceipt, sizeof(Nui32));
                                mDownload.Push(ackReceipt);

                                // Remove, swap with last
                                mWillLostReceipt.RemoveAtIndex(k);
                            }
                            else
                                k++;
                        }
                    }

                    MessageNumberNode * messageNumberNode = getMessageNode(mark, whenSent);
                    if(messageNumberNode)
                    {
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS==1
                        mCongestion.onReceiveAck(readtime, rtt, dhf.hasBAndAS, 0, dhf.AS, totalUserDataBytesAcked, bandwidthExceededStatistic, mark);
    #else
                        TimeDurUS ping;
                        if(readtime > whenSent)
                            ping = readtime - whenSent;
                        else
                            ping = 0;
                        mCongestion.onReceiveAck(readtime, ping, dhf.hasBAndAS, 0, dhf.AS, totalUserDataBytesAcked, bandwidthExceededStatistic, mark);
    #endif
                        while(messageNumberNode)
                        {
                            removeResend(messageNumberNode->mACKMark, readtime, address);
                            messageNumberNode = messageNumberNode->mNext;
                        }

                        removeMessageNode(mark);
                    }
                }
            }
        }
        else if(dhf.isNAK)
        {
            DatagramSequenceNumberType ackmark;
            DataStructures::RangeList<DatagramSequenceNumberType> incomingNAKs;
            if(incomingNAKs.write(&socketData) == false)
            {
                mListener->onReport("incomingNAKs.import failed", N_b_TO_B(length), address, true);
                return false;
            }
            for(i = 0; i < incomingNAKs.ranges.Size(); ++i)
            {
                if(incomingNAKs.ranges[i].minIndex > incomingNAKs.ranges[i].maxIndex)
                {
                    N_assert(incomingNAKs.ranges[i].minIndex <= incomingNAKs.ranges[i].maxIndex);

                    mListener->onReport("incomingNAKs minIndex>maxIndex", N_b_TO_B(length), address, true);
                    return false;
                }

                for (ackmark = incomingNAKs.ranges[i].minIndex;
                    ackmark >= incomingNAKs.ranges[i].minIndex &&
                        ackmark <= incomingNAKs.ranges[i].maxIndex; ++ackmark)
                {
                    mCongestion.OnNAK(readtime, ackmark);

                    TimeDurUS send_t;
                    MessageNumberNode * messageNumberNode = getMessageNode(ackmark, send_t);
                    while (messageNumberNode)
                    {
                        // Update timers so resends occur immediately
                        Packet * packet = mResend[messageNumberNode->mACKMark & RESEND_BUFFER_ARRAY_MASK];
                        if(packet)
                        {
                            if(packet->mLastAction != 0)
                            {
                                packet->mLastAction = readtime;
                            }
                        }
                        messageNumberNode = messageNumberNode->mNext;
                    }
                }
            }
        }
        else
        {
            Nui32 skipend;
            if(!mCongestion.OnGotPacket(dhf.mMark, dhf.isContinuousSend, readtime, length, &skipend))
            {
                mListener->onReport("mCongestion.OnGotPacket failed", N_b_TO_B(length), address, true);
                return true;
            }
            if (dhf.isPacketPair)
                mCongestion.OnGotPacketPair(dhf.mMark, length, readtime);

            DatagramHeaderFormat dhfNAK;
            dhfNAK.isNAK = true;
            NCount skip;
            for (skip = skipend; skip > 0; --skip)
            {
                mNAK.Insert(dhf.mMark - skip);
            }
            remoteSystemNeedsBAndAS = dhf.needsBAndAs;

            // Ack dhf.mMark
            // Ack even unreliable messages for congestion control, just don't resend them on no ack
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
            sendACK(dhf.mMark, dhf.sourceSystemTime);
    #else
            sendACK(dhf.mMark, 0);
    #endif
            Packet * packet = Packet::create(&socketData, readtime);
            if(packet == 0)
            {
                mListener->onReport("createPacket failed", N_b_TO_B(length), address, true);
                return true;
            }

            while(packet)
            {
                {
                    // resetReceivedPackets is set from a non-threadsafe function.
                    // We do the actual reset in this function so the data is not modified by multiple threads
                    if (resetReceivedPackets)
                    {
                        hasReceivedPacketQueue.ClearAndForceAllocation(DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE);
                        receivedPacketsBaseIndex = 0;
                        resetReceivedPackets = false;
                    }

                    // Check for corrupt pchannel
                    if (packet->mPacketType == PT_ONE_MAKE_SURE ||
                        packet->mPacketType == PT_ONE_WILL_LOST ||
                        packet->mPacketType == PT_ORDER_MAKE_SURE)
                    {
                        if(packet->mPacketCh >= N_NET_CH)
                        {
                            mListener->onReport("packet->mPacketCh >= N_NET_CH", N_b_TO_B(length), address, true);

                            mBSP[USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                            N_delete packet;
                            goto JUMP_TO_NEXT_PACKET;
                        }
                    }

                    // 8/12/09 was previously not checking if the message was reliable. However, on packetloss this would mean you'd eventually exceed the
                    // hole count because unreliable messages were never resent, and you'd stop getting messages
                    if (packet->mPacketType == PT_BE_SURE ||
                        packet->mPacketType == PT_ONE_MAKE_SURE ||
                        packet->mPacketType == PT_ORDER_MAKE_SURE)
                    {
                        // If the following conditional is true then this either a duplicate packet
                        // or an older out of order packet
                        // The subtraction Nui32 overflow is intentional
                        holeCount = (DatagramSequenceNumberType)(packet->mACKMark-receivedPacketsBaseIndex);
                        const DatagramSequenceNumberType typeRange = (DatagramSequenceNumberType)(const Nui32)-1;

                        if (holeCount == (DatagramSequenceNumberType)0)
                        {
                            // Got what we were expecting
                            if (hasReceivedPacketQueue.Size())
                                hasReceivedPacketQueue.Pop();
                            ++receivedPacketsBaseIndex;
                        }
                        else if (holeCount > typeRange/(DatagramSequenceNumberType)2)
                        {
                            mBSP[USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                            mListener->onReport("holeCount > typeRange/(DatagramSequenceNumberType) 2", N_b_TO_B(length), address, false);

                            N_delete packet;

                            goto JUMP_TO_NEXT_PACKET;
                        }
                        else if ((Nui32) holeCount<hasReceivedPacketQueue.Size())
                        {
                            // Got a higher count out of order packet that was missing in the sequence or we already got
                            if(hasReceivedPacketQueue[holeCount] != false) // non-zero means this is a hole
                            {
                                mListener->onReport("Higher count pushed to hasReceivedPacketQueue", N_b_TO_B(length), address, false);
                                // Fill in the hole
                                hasReceivedPacketQueue[holeCount] = false; // We got the packet at holeCount
                            }
                            else
                            {
                                mBSP[USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                                mListener->onReport("Duplicate packet ignored", N_b_TO_B(length), address, false);

                                N_delete packet;

                                goto JUMP_TO_NEXT_PACKET;
                            }
                        }
                        else // holeCount>=receivedPackets.Size()
                        {
                            if(holeCount > (DatagramSequenceNumberType) 1000000)
                            {
                                N_assert("Hole count too high. See PacketPrc.h" && 0);

                                mListener->onReport("holeCount > 1000000", N_b_TO_B(length), address, true);

                                mBSP[USER_MESSAGE_BYTES_RECEIVED_IGNORED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                                N_delete packet;

                                goto JUMP_TO_NEXT_PACKET;
                            }

                            mListener->onReport("Adding to hasReceivedPacketQueue later ordered message", N_b_TO_B(length), address, false);

                            // Fix - sending on a higher plevel gives us a very very high received packets base index if we formerly had pre-split a lot of messages and
                            // used that as the message number.  Because of this, a lot of time is spent in this linear loop and the timeout time expires because not
                            // all of the message is sent in time.
                            // Fixed by late assigning message IDs on the sender

                            // Add 0 times to the queue until (mACKMark - baseIndex) < queue size.
                            while ((Nui32)holeCount > hasReceivedPacketQueue.Size())
                                hasReceivedPacketQueue.Push(true); // time+(TimeDurUS)60 * (TimeDurUS)1000 * (TimeDurUS)1000); // Didn't get this packet - set the time to give up waiting
                            hasReceivedPacketQueue.Push(false); // Got the packet
    #ifdef _DEBUG
                            // If this N_assert hits then DatagramSequenceNumberType has overflowed
                            N_assert(hasReceivedPacketQueue.Size() < (Nui32)((DatagramSequenceNumberType)(const Nui32)(-1)));
    #endif
                        }

                        while (hasReceivedPacketQueue.Size() > 0 && hasReceivedPacketQueue.Peek()==false )
                        {
                            hasReceivedPacketQueue.Pop();
                            ++receivedPacketsBaseIndex;
                        }
                    }

                    // If the allocated buffer is > DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE and it is 3x greater than the number of elements actually being used
                    if (hasReceivedPacketQueue.AllocationSize() > (Nui32) DEFAULT_HAS_RECEIVED_PACKET_QUEUE_SIZE && hasReceivedPacketQueue.AllocationSize() > hasReceivedPacketQueue.Size() * 3)
                        hasReceivedPacketQueue.Compress();

                    if (packet->mSplitCount > 0)
                    {
                        // Check for a rebuilt packet
                        if (packet->mPacketType != PT_ORDER_MAKE_SURE &&
                            packet->mPacketType != PT_ONE_MAKE_SURE &&
                            packet->mPacketType != PT_ONE_WILL_LOST)
                        {
                            packet->mPacketCh = 255; // Use 255 to designate not sequenced and not ordered
                        }

                        packet = combine(packet, readtime, s, address, rnr, upload);

                        if(packet == 0)
                        {
                            mListener->onReport("combine did not return anything.", N_b_TO_B(length), address, false);

                            // Don't have all the parts yet
                            goto JUMP_TO_NEXT_PACKET;
                        }
                    }
    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                    Nui8 packetId;
                    char * type = "UNDEFINED";
    #endif
                    if(packet->mPacketType == PT_ONE_MAKE_SURE ||
                        packet->mPacketType == PT_ONE_WILL_LOST ||
                        packet->mPacketType == PT_ORDER_MAKE_SURE)
                    {
    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                        // ___________________
                        NetSerializer bitStream(packet->mData, N_B_TO_b(packet->mBitCount), false);
                        Nui32 receivedPacketNumber;
                        TimeDurMS receivedTime;
                        Nui8 streamNumber;
                        PacketType ptype;
                        // ___________________
                        bitStream.skipRead(1, 0);// Ignore ID_TIMESTAMP
                        bitStream.read(receivedTime);
                        bitStream.read(packetId);
                        bitStream.read(receivedPacketNumber);
                        bitStream.read(streamNumber);
                        bitStream.read(ptype);
                        if (packetId == ID_USER_PACKET_ENUM + 1)
                        {
                            if (ptype == PT_ONE_WILL_LOST)
                                type = "PT_ONE_WILL_LOST";
                            else if (ptype == PT_ORDER_MAKE_SURE)
                                type = "PT_ORDER_MAKE_SURE";
                            else
                                type = "PT_ONE_MAKE_SURE";
                        }
                        // ___________________
    #endif
                        if(packet->mOrder == mOrderImport[packet->mPacketCh])
                        {
                            // Has current ordering index
                            if(packet->mPacketType == PT_ONE_MAKE_SURE ||
                                packet->mPacketType == PT_ONE_WILL_LOST)
                            {
                                // Is sequenced
                                if(isValid(packet->mSequence, mSequenceImport[packet->mPacketCh]) == true)
                                {
                                    // Expected or highest known value
    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                    if (packetId == ID_USER_PACKET_ENUM + 1 && fp)
                                    {
                                        fprintf(fp, "Returning %i, %s by fallthrough. OI=%i. SI=%i.\n", receivedPacketNumber, type, packet->mOrder.val, packet->mSequence);
                                        fflush(fp);
                                    }

                                    if (packetId == ID_USER_PACKET_ENUM + 1)
                                    {
                                        if (receivedPacketNumber < packetNumber)
                                        {
                                            if (fp)
                                            {
                                                fprintf(fp, "Out of order packet from fallthrough! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                                fflush(fp);
                                            }
                                        }
                                        packetNumber = receivedPacketNumber + 1;
                                    }
    #endif
                                    mSequenceImport[packet->mPacketCh] = packet->mSequence + 1;
                                }
                                else
                                {
    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                    if(packetId == ID_USER_PACKET_ENUM + 1 && fp)
                                    {
                                        fprintf(fp, "Discarding %i, %s late sequenced. OI=%i. SI=%i.\n", receivedPacketNumber, type, packet->mOrder.val, packet->mSequence);
                                        fflush(fp);
                                    }
    #endif
                                    mListener->onReport("Sequenced rejected: lower than highest known value", N_b_TO_B(length), address, false);
                                    N_delete packet;

                                    goto JUMP_TO_NEXT_PACKET;
                                }
                            }
                            else
                            {
                                // Push to output buffer immediately
                                mBSP[USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(readtime, N_B_TO_b(packet->mBitCount));
                                mDownload.Push(packet);

    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                if (packetId == ID_USER_PACKET_ENUM + 1 && fp)
                                {
                                    fprintf(fp, "outputting immediate %i, %s. OI=%i. SI=%i.", receivedPacketNumber, type, packet->mOrder.val, packet->mSequence);
                                    if (mOrderQueue[packet->mPacketCh].Size() == 0)
                                        fprintf(fp, "heap empty\n");
                                    else
                                        fprintf(fp, "heap head=%i\n", mOrderQueue[packet->mPacketCh].Peek()->mOrder.val);

                                    if (receivedPacketNumber < packetNumber)
                                    {
                                        if (packetId == ID_USER_PACKET_ENUM + 1 && fp)
                                        {
                                            fprintf(fp, "Out of order packet arrived! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                            fflush(fp);
                                        }
                                    }
                                    packetNumber = receivedPacketNumber + 1;

                                    fflush(fp);
                                }
    #endif

                                ++mOrderImport[packet->mPacketCh];
                                mSequenceImport[packet->mPacketCh] = 0;

                                // Return off heap until order lost
                                while(mOrderQueue[packet->mPacketCh].Size() > 0 &&
                                    mOrderQueue[packet->mPacketCh].Peek()->mOrder == mOrderImport[packet->mPacketCh])
                                {
                                    packet = mOrderQueue[packet->mPacketCh].Pop(0);

    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                                    NetSerializer bitStream2(packet->mData, N_B_TO_b(packet->mBitCount), false);
                                    bitStream2.skipRead(1, 0); // Ignore ID_TIMESTAMP
                                    bitStream2.read(receivedTime);
                                    bitStream2.skipRead(1, 0); // Ignore ID_USER_ENUM+1
                                    bitStream2.read(receivedPacketNumber);
                                    bitStream2.read(streamNumber);
                                    bitStream2.read(ptype);
                                    char * type = "UNDEFINED";
                                    if (ptype == PT_ONE_WILL_LOST)
                                        type = "PT_ONE_WILL_LOST";
                                    else if (ptype == PT_ORDER_MAKE_SURE)
                                        type = "PT_ORDER_MAKE_SURE";

                                    if (packetId == ID_USER_PACKET_ENUM+1 && fp)
                                    {
                                        fprintf(fp, "Heap pop %i, %s. OI=%i. SI=%i.\n", receivedPacketNumber, type, packet->mOrder.val, packet->mSequence);
                                        fflush(fp);

                                        if (receivedPacketNumber < packetNumber)
                                        {
                                            if (packetId == ID_USER_PACKET_ENUM+1 && fp)
                                            {
                                                fprintf(fp, "Out of order packet from heap! Expecting %i got %i\n", receivedPacketNumber, packetNumber);
                                                fflush(fp);
                                            }
                                        }
                                        packetNumber = receivedPacketNumber + 1;
                                    }
    #endif

                                    mBSP[USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                                    mDownload.Push(packet);

                                    if (packet->mPacketType == PT_ORDER_MAKE_SURE)
                                    {
                                        ++mOrderImport[packet->mPacketCh];
                                    }
                                    else
                                    {
                                        mSequenceImport[packet->mPacketCh] = packet->mSequence;
                                    }
                                }
                                // Done
                                goto JUMP_TO_NEXT_PACKET;
                            }
                        }
                        else if (isValid(packet->mOrder, mOrderImport[packet->mPacketCh]) == true)
                        {
                            // packet->mOrder is greater
                            // If a message has a greater ordering index, and is sequenced or ordered, buffer it
                            // Sequenced has a lower heap weight, ordered has max sequenced weight

                            // Keep orderedHoleCount count small
                            if (mOrderQueue[packet->mPacketCh].Size() == 0)
                                mOrderQueueMark[packet->mPacketCh] = mOrderImport[packet->mPacketCh];

                            Nui64 orderedHoleCount = packet->mOrder - mOrderQueueMark[packet->mPacketCh];
                            Nui64 weight = orderedHoleCount * 1048576;
                            if (packet->mPacketType == PT_ONE_MAKE_SURE ||
                                packet->mPacketType == PT_ONE_WILL_LOST)
                                weight += packet->mSequence;
                            else
                                weight += (1048576 - 1);
                            mOrderQueue[packet->mPacketCh].Push(weight, packet);

    #ifdef PRINT_TO_FILE_RELIABLE_ORDERED_TEST
                            if (packetId == ID_USER_PACKET_ENUM + 1 && fp)
                            {
                                fprintf(fp, "Heap push %i, %s, weight=%" N_64BIT_MOD "u. OI=%i. waiting on %i. SI=%i.\n",
                                    receivedPacketNumber, type, weight, packet->mOrder.val,
                                        mOrderImport[packet->mPacketCh].val, packet->mSequence);
                                fflush(fp);
                            }
    #endif
                            mListener->onReport("Larger number ordered packet leaving holes", N_b_TO_B(length), address, false);
                            // Buffered, nothing to do
                            goto JUMP_TO_NEXT_PACKET;
                        }
                        else
                        {
                            N_delete packet;
                            mListener->onReport("Rejected older resend", N_b_TO_B(length), address, false);
                            goto JUMP_TO_NEXT_PACKET;
                        }

                    }

                    mBSP[USER_MESSAGE_BYTES_RECEIVED_PROCESSED].Push1(readtime, N_B_TO_b(packet->mBitCount));

                    // Nothing special about this packet.  Add it to the output queue
                    mDownload.Push(packet);

                    packet = 0;
                }

                // Used for a goto to jump to the mNextMakeSure packet immediately

    JUMP_TO_NEXT_PACKET:
                // Parse the bitstream to create an internal packet
                packet = Packet::create(&socketData, readtime);
            }
        }
        receivePacketCount++;
        return true;
    }
    //------------------------------------------------------------------------
    NBitCount PacketPrc::receive(Nui8 ** data)
    {
        Packet * packet;
        NBitCount bcnt;
        if(mDownload.Size() > 0)
        {
            packet = mDownload.Pop();
            *data = packet->mData;
            bcnt = packet->mBitCount;
            packet->mAllocType = Packet::Refer;
            N_delete packet;
            return bcnt;
        }
        else
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
    bool PacketPrc::send(Nui8 * data, NBitCount cnt, PacketLevel plevel, PacketType ptype, 
        Nui8 pchannel, bool copy, NCount mtu, TimeDurUS current, Nui32 receipt)
    {
        N_assert(ptype < PT_COUNT && ptype > 0);
        N_assert(plevel < PL_COUNT && plevel > 0);
        N_assert(pchannel < N_NET_CH);
        N_assert(cnt > 0);

        current /= 1000;

        //if (ptype > PT_ORDER_RECEIPT_MAKE_SURE || ptype < 0)
        //    ptype = PT_BE_SURE;

        //if (plevel > PL_COUNT || plevel < 0)
        //    plevel = PL_HIGH;

        //if(pchannel >= N_NET_CH)
        //    pchannel = 0;

        NCount numberOfBytesToSend = N_B_TO_b(cnt);
        mBSP[USER_MESSAGE_BYTES_PUSHED].Push1(current, numberOfBytesToSend);

        if(copy)
        {
            Packet * packet = N_new Packet(numberOfBytesToSend, true);
            memcpy(packet->mData, data, numberOfBytesToSend);
        }
        else
        {
            Packet * packet = Packet(data);
        }
        packet->mCreate = current;
        packet->mBitCount = cnt;
        packet->mDebugMark = mDebugSendMark++;
        packet->mPacketLevel = plevel;
        packet->mPacketType = ptype;
        packet->mReceipt = receipt;

        NCount maxDataSizeBytes = getMaxContentSize();

        bool split = numberOfBytesToSend > maxDataSizeBytes;
        if (split)
        {
            // Split packets cannot be unreliable, in case that one part doesn't arrive and the whole cannot be reassembled.
            // One part could not arrive either due to packetloss or due to unreliable discard
            if (packet->mPacketType == PT_WILL_LOST)
                packet->mPacketType = PT_BE_SURE;
            else if (packet->mPacketType == PT_RECEIPT_WILL_LOST)
                packet->mPacketType = PT_RECEIPT_MAKE_SURE;
            else if (packet->mPacketType == PT_ONE_WILL_LOST)
                packet->mPacketType = PT_ONE_MAKE_SURE;
        }

        if (packet->mPacketType == PT_ONE_MAKE_SURE ||
            packet->mPacketType == PT_ONE_WILL_LOST)
        {
            packet->mPacketCh = pchannel;
            packet->mOrder = mOrderExport[pchannel];
            packet->mSequence = mSequenceExport[pchannel]++;
        }
        else if (packet->mPacketType == PT_ORDER_MAKE_SURE ||
            packet->mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
        {
            packet->mPacketCh = pchannel;
            packet->mOrder = mOrderExport[pchannel]++;
            mSequenceExport[pchannel] = 0;
        }

        if (split)
        {
            split(packet);
        }
        else
        {
            N_assert(packet->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
            N_assert(packet->mMakeSureInit == false);
            addWillLost(packet);

            mUploadBuffer.Push(getNextWeigth(packet->mPacketLevel), packet);
            N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
            mProfile.messageInSendBuffer[packet->mPacketLevel]++;
            mProfile.bytesInSendBuffer[packet->mPacketLevel] += (NIId)N_B_TO_b(packet->mBitCount);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void PacketPrc::process(SocketObj * s, const Address * address, NCount mtu,
        TimeDurUS time, Nui32 bpslimit, RakNetRandom * rnr, NetSerializer & upload)
    {
        TimeDurMS timeMs;
        time /= 1000;
        timeMs = time;

        // This line is necessary because the timer isn't accurate
        if(time <= lastUpdateTime)
        {
            // Always set the last time in case of overflow
            lastUpdateTime = time;
            return;
        }

        TimeDurUS timeSinceLastTick = time - lastUpdateTime;
        lastUpdateTime = time;

        if(timeSinceLastTick > 100)
            timeSinceLastTick = 100;

        if(mMessageTimeOut > 0)
        {
            if(timeSinceLastTick >= timeToNextUnreliableCull)
            {
                if(mWillLostHead)
                {
                    // Cull out all unreliable messages that have exceeded the timeout
                    Packet * cur = mWillLostHead;
                    Packet * end = mWillLostHead->mPreWillLost;
    #ifdef _MSC_VER
    #pragma warning(disable : 4127) // warning C4127: conditional expression is constant
    #endif
                    while(1)
                    {
                        if(time > cur->mCreate + (TimeDurUS)mMessageTimeOut)
                        {
                            // Flag invalid, and reset the memory. Still needs to be removed from the sendPacketSet later
                            // This fixes a problem where a remote system disconnects, but we don't know it yet, and memory consumption increases to a huge value
                            cur->destroy();
                            cur->mData = 0;
                            Packet * next = cur->mNextWillLost;
                            removeWillLost(cur);

                            if (cur == end)
                                break;

                            cur = next;
                        }
                        else
                        {
                            // They should be inserted in-order, so no need to iterate past the first failure
                            break;
                        }
                    }
                }

                timeToNextUnreliableCull = mMessageTimeOut/(TimeDurUS)2;
            }
            else
            {
                timeToNextUnreliableCull -= timeSinceLastTick;
            }
        }
        // Due to thread vagarities and the way I store the time to avoid slow calls to N_Engine().getTimer().getMS
        // time may be less than lastAck
        if(mProfile.messagesInResendBuffer != 0 && isTimeOut(time))
        {
            // SHOW - dead connection
            // We've waited a very long time for a reliable packet to get an ack and it never has
            mTimeOutValid = true;
            return;
        }

        if (mCongestion.ShouldSendACKs(time, timeSinceLastTick))
        {
            sendACK(s, address, time, rnr, upload);
        }

        if (mNAK.Size() > 0)
        {
            upload.seekWrite(0);
            upload.seekRead(0);
            DatagramHeaderFormat dhfNAK;
            dhfNAK.isNAK = true;
            dhfNAK.isACK = false;
            dhfNAK.isPacketPair = false;
            dhfNAK.read(&upload);
            mNAK.read(&upload, N_b_TO_B(mUsrMTU), true);
            send(s, address, &upload, rnr, time);
        }

        DatagramHeaderFormat dhf;
        dhf.needsBAndAs = mCongestion.GetIsInSlowStart();
        dhf.isContinuousSend = bandwidthExceededStatistic;
        bandwidthExceededStatistic = mUploadBuffer.Size() > 0;

        bool hasDataToSendOrResend = mResendHead != 0 || bandwidthExceededStatistic;
        N_assert(PL_COUNT == 4);
        mCongestion.update(time, hasDataToSendOrResend);

        mProfile.mUploadLimitValue = N_B_TO_b(bpslimit);
        mProfile.BPSLimitByCongestionControl = mCongestion.GetBytesPerSecondLimitByCongestionControl();

        NCount i;
        if(time > mResetBSPTime + 100)
        {
            for(i = 0; i < RNS_PER_SECOND_METRICS_COUNT; ++i)
            {
                mBSP[i].ClearExpired1(time);
            }

            mResetBSPTime = time;
        }

        if(mWillLostReceipt.Size() > 0)
        {
            i = 0;
            while (i < mWillLostReceipt.Size())
            {
                if (time - mWillLostReceipt[i].mLastAction < (((TimeDurUS) - 1)/2) )
                {
                    Packet * ackReceipt = N_new Packet(5, false);
                    ackReceipt->mBitCount = N_b_TO_B(5);
                    ackReceipt->mData[0] = (Token)ID_SND_RECEIPT_LOSS;
                    memcpy(ackReceipt->mData + sizeof(Token), &mWillLostReceipt[i].mReceipt, sizeof(Nui32));
                    mDownload.Push(ackReceipt);

                    // Remove, swap with last
                    mWillLostReceipt.RemoveAtIndex(i);
                }
                else
                    ++i;
            }
        }

        if(hasDataToSendOrResend == true)
        {
            Packet * packet;
            bool pushedAnything;
            NBitCount nextPacketBitLength;
            dhf.isACK = false;
            dhf.isNAK = false;
            dhf.hasBAndAS = false;
            mSend.Clear(true);
            mFree.Clear(true);
            mUploadCountQueue.Clear(true);
            mSendPairQueue.Clear(true);
            mUploadSizeQueue.Clear(true);
            mUploadRemain = 0;

            int sendbandwidth = mCongestion.GetTransmissionBandwidth(time, timeSinceLastTick, mCheckACK, dhf.isContinuousSend);
            int resendbandwidth = mCongestion.GetRetransmissionBandwidth(time, timeSinceLastTick, mCheckACK, dhf.isContinuousSend);
            if (resendbandwidth > 0 || sendbandwidth > 0)
            {
                mProfile.isLimitedByCongestionControl = false;

                mCongestionRemain = 0;

                // Keep filling datagrams until we exceed retransmission bandwidth
                while(N_B_TO_b(mCongestionRemain) < resendbandwidth)
                {
                    pushedAnything = false;

                    // Fill one datagram, then break
                    while(mResendHead != 0)
                    {
                        packet = mResendHead;
                        N_assert(packet->mMakeSureInit == true);

                        if (time - packet->mLastAction < (((TimeDurUS)-1)/2))
                        {
                            nextPacketBitLength = packet->mExtCount + packet->mBitCount;
                            if (mUploadRemain + nextPacketBitLength > N_b_TO_B(mUsrMTU))
                            {
                                // Gathers all PushPackets()
                                addFragment();
                                break;
                            }

                            removeSend(mResendHead, false);

                            CC_DEBUG_PRINTF_2("Rs %i ", packet->mACKMark.val);

                            mBSP[USER_MESSAGE_BYTES_RESENT].Push1(time,N_B_TO_b(packet->mBitCount));

                            addSend(time, packet, true);

                            mCongestion.OnResend(time, packet->mLastAction);
                            packet->mResendTime = mCongestion.GetRTOForRetransmission(packet->mSendCount);
                            packet->mLastAction = packet->mResendTime + time;

                            pushedAnything = true;

                            addResend(packet, time, false, false);
                        }
                        else
                        {
                            // Filled one datagram.
                            // If the 2nd and it's time to send a datagram pair, will be marked as a pair
                            addFragment();
                            break;
                        }
                    }
                    if (pushedAnything == false)
                        break;
                }
            }
            else
            {
                mProfile.isLimitedByCongestionControl = true;
            }

            if (N_B_TO_b(mCongestionRemain) < sendbandwidth)
            {
                mCongestionRemain = 0;

                // Keep filling datagrams until we exceed transmission bandwidth
                while (isResendQueueValid() == false &&
                    (N_B_TO_b(mCongestionRemain) < sendbandwidth ||
                    // This condition means if we want to send a datagram pair, and only have one datagram buffered, exceed bandwidth to add another
                    (countdownToNextPacketPair == 0 && mSendPairQueue.Size() == 1)))
                {
                    pushedAnything = false;

                    mProfile.mUploadLimit = (bpslimit != 0 && N_B_TO_b(bpslimit) < mBSP[USER_MESSAGE_BYTES_SENT].GetBPS1(time)); //nii

                    while (mUploadBuffer.Size() && mProfile.mUploadLimit == false)
                    {
                        packet = mUploadBuffer.Peek();
                        N_assert(packet->mMakeSureInit == false);
                        N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));

                        if (packet->mData == 0)
                        {
                            mUploadBuffer.Pop(0);
                            N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
                            mProfile.messageInSendBuffer[packet->mPacketLevel]--;
                            mProfile.bytesInSendBuffer[packet->mPacketLevel]-=(NIId) N_B_TO_b(packet->mBitCount);
                            N_delete packet;
                            continue;
                        }

                        packet->mExtCount = packet->getHeaderSize();
                        nextPacketBitLength = packet->mExtCount + packet->mBitCount;
                        if (mUploadRemain + nextPacketBitLength > N_b_TO_B(mUsrMTU))
                        {
                            // Hit MTU. May still push packets if smaller ones exist at a lower plevel
                            N_assert(mUploadRemain != 0);
                            N_assert(packet->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
                            break;
                        }

                        bool ack;
                        if (packet->mPacketType == PT_BE_SURE ||
                            packet->mPacketType == PT_ONE_MAKE_SURE ||
                            packet->mPacketType == PT_ORDER_MAKE_SURE ||
                            packet->mPacketType == PT_RECEIPT_MAKE_SURE  ||
                            packet->mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
                        {
                            ack = true;
                        }
                        else
                            ack = false;

                        mUploadBuffer.Pop(0);
                        N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
                        N_assert(packet->mMakeSureInit == false);
                        mProfile.messageInSendBuffer[packet->mPacketLevel]--;
                        mProfile.bytesInSendBuffer[packet->mPacketLevel]-=(NIId) N_B_TO_b(packet->mBitCount);
                        if(ack)
                        {
                            packet->mMakeSureInit = true;
                            packet->mACKMark = mACKSendMark;
                            packet->mResendTime = mCongestion.GetRTOForRetransmission(packet->mSendCount + 1);
                            packet->mLastAction = packet->mResendTime + time;

                            const TimeDurUS threshhold = 10000;
                            if (packet->mLastAction - time > threshhold)
                            {
                                N_assert(time - packet->mLastAction < threshhold);
                            }
                            N_assert(mResend[packet->mACKMark & RESEND_BUFFER_ARRAY_MASK] != 0);

                            mResend[packet->mACKMark & RESEND_BUFFER_ARRAY_MASK] = packet;
                            ++mProfile.messagesInResendBuffer;
                            mProfile.bytesInResendBuffer += N_B_TO_b(packet->mBitCount);
                            addResend(packet, time, true, ack);
                            ++mACKSendMark;
                        }
                        else if (packet->mPacketType == PT_RECEIPT_WILL_LOST)
                        {
                            mWillLostReceipt.Push(UnreliableWithAckReceiptNode(
                                mCongestion.GetNextDatagramSequenceNumber() + mUploadCountQueue.Size(),
                                packet->mReceipt,
                                mCongestion.GetRTOForRetransmission(packet->mSendCount + 1) + time));
                        }

                        // If ack is false, the packet and its contents will be added to a list to be freed in removeFragment
                        // However, the packet structure will remain allocated and be in the mResend list if it requires a receipt
                        mBSP[USER_MESSAGE_BYTES_SENT].Push1(time,N_B_TO_b(packet->mBitCount));

                        addSend(time, packet, ack);

                        pushedAnything = true;

                        if (isResendQueueValid())
                            break;
                    }
                    // No datagrams pushed?
                    if (mUploadRemain == 0)
                        break;

                    // Filled one datagram.
                    // If the 2nd and it's time to send a datagram pair, will be marked as a pair
                    addFragment();
                }
            }

            for(Nui32 datagramIndex = 0; datagramIndex < mUploadCountQueue.Size(); ++datagramIndex)
            {
                if(datagramIndex > 0)
                    dhf.isContinuousSend = true;
                MessageNumberNode * messageNumberNode = 0;
                dhf.mMark = mCongestion.GetAndIncrementNextDatagramSequenceNumber();
                dhf.isPacketPair = mSendPairQueue[datagramIndex];

                bool isSecondOfPacketPair = dhf.isPacketPair && datagramIndex>0 && mSendPairQueue[datagramIndex-1];
                Nui32 msgIndex, msgTerm;
                if(datagramIndex == 0)
                {
                    msgIndex = 0;
                    msgTerm = mUploadCountQueue[0];
                }
                else
                {
                    msgIndex = mUploadCountQueue[datagramIndex-1];
                    msgTerm = mUploadCountQueue[datagramIndex];
                }

                // More accurate time to reset here
    #if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
                dhf.sourceSystemTime = N_Engine().getTimer().getUS();
    #endif
                upload.seekWrite(0);
                upload.seekRead(0);
                dhf.read(&upload);
                CC_DEBUG_PRINTF_2("S%i ",dhf.mMark.val);

                while(msgIndex < msgTerm)
                {
                    // If reliable or needs receipt
                    if (mSend[msgIndex]->mPacketType != PT_WILL_LOST &&
                        mSend[msgIndex]->mPacketType != PT_ONE_WILL_LOST)
                    {
                        if (messageNumberNode == 0)
                        {
                            messageNumberNode = addMessage(dhf.mMark, mSend[msgIndex]->mACKMark, time);
                        }
                        else
                        {
                            messageNumberNode = addMessageNode(messageNumberNode, mSend[msgIndex]->mACKMark);
                        }
                    }
                    N_assert(upload.getSize() <= N_Net_MaxMtuSize - UDP_HEADER_SIZE);
                    mSend[msgIndex]->read(&upload);
                    N_assert(upload.getSize() <= N_Net_MaxMtuSize - UDP_HEADER_SIZE);
                    ++msgIndex;
                }

                if(isSecondOfPacketPair)
                {
                    // Pad to size of first datagram
                    N_assert(upload.getSize() <= N_Net_MaxMtuSize - UDP_HEADER_SIZE);
                    upload.writeAlign(mUploadSizeQueue[datagramIndex - 1]);
                    N_assert(upload.getSize() <= N_Net_MaxMtuSize - UDP_HEADER_SIZE);
                }

                if(messageNumberNode == 0)
                {
                    // Unreliable and no ACK, add dummy node
                    addMessage(dhf.mMark, time);
                }
                mCongestion.OnSendBytes(time, UDP_HEADER_SIZE + DatagramHeaderFormat::GetDataHeaderByteLength());
                send(s, address, &upload, rnr, time);

                bandwidthExceededStatistic = mUploadBuffer.Size() > 0;

                if(bandwidthExceededStatistic == true)
                    timeOfLastContinualSend = time;
                else
                    timeOfLastContinualSend = 0;
            }

            removeFragment();

            // Any data waiting to send after attempting to send, then bandwidth is exceeded
            bandwidthExceededStatistic = mUploadBuffer.Size() > 0;
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::send(SocketObj * host, const Address * address, NetSerializer * data, RakNetRandom * rnr, TimeDurUS current)
    {
        NCount length;
        length = data->getSize();

#if LIBCAT_SECURITY == 1
        if(useSecurity)
        {
            Nui8 * buffer = reinterpret_cast<Nui8 *>(data->getData());

            int buffer_size = data->getSize();

            // Verify there is enough room for encrypted output and encrypt
            // Encrypt() will increase length
            bool success = auth_enc.Encrypt(buffer, buffer_size, length);
            N_assert(success);
        }
#endif
        mBSP[ACTUAL_BYTES_SENT].Push1(current, length);
        N_assert(length <= mCongestion.GetMTU());
#ifdef USE_THREADED_SEND
        SocketUtil::SendToThreadBlock * block = SocketUtil::AllocateBlock();
        memcpy(block->mData, data->getData(), length);
        block->dataWriteOffset = length;
        block->extraSocketOptions = extraSocketOptions;
        block->s = host;
        block->mAddress = address;
        SocketUtil::ProcessBlock(block);
#else
        SocketSendData bsp;
        bsp.mData = data->getData();
        bsp.mLength = length;
        bsp.mAddress = address;
        host->send(&bsp);
#endif
    }
    //------------------------------------------------------------------------
    bool PacketPrc::isUpload()
    {
        if (mUploadBuffer.Size() > 0)
            return true;
        return mProfile.messagesInResendBuffer != 0;
    }
    //------------------------------------------------------------------------
    bool PacketPrc::isResponse()
    {
        return mACK.Size() > 0;
    }
    //------------------------------------------------------------------------
    bool PacketPrc::onMessage()
    {

    }
    //------------------------------------------------------------------------
    void PacketPrc::onSend()
    {

    }
    //------------------------------------------------------------------------
    void PacketPrc::onReset()
    {

    }
    //------------------------------------------------------------------------
    void PacketPrc::setSplitFactor(int interval)
    {
        mSplitMessageFactor = interval;
    }
    //------------------------------------------------------------------------
    void PacketPrc::setMessageTimeOut(TimeDurMS time)
    {
        mMessageTimeOut = time;
    }
    //------------------------------------------------------------------------
    Nui32 PacketPrc::removeResend(const MessageNumberType & ackmark, TimeDurUS time, const Address * address)
    {
        Packet * packet;

        packet = mResend[ackmark & RESEND_BUFFER_ARRAY_MASK];
        // May ask to remove twice, for example resend twice, then second ack
        if (packet && packet->mACKMark == ackmark)
        {
            mResend[ackmark & RESEND_BUFFER_ARRAY_MASK] = 0;
            CC_DEBUG_PRINTF_2("AckRcv %i ", ackmark);

            mProfile.messagesInResendBuffer--;
            mProfile.bytesInResendBuffer-=N_B_TO_b(packet->mBitCount);

            totalUserDataBytesAcked += (NIId) N_B_TO_b(packet->mExtCount + packet->mBitCount);

            // Return receipt if asked for
            if(packet->mPacketType >= PT_RECEIPT_MAKE_SURE &&
                (packet->mSplitCount == 0 || packet->mSplitIndex + 1 == packet->mSplitCount))
            {
                Packet * ackReceipt = N_new Packet(5, false);
                ackReceipt->mBitCount=N_b_TO_B(5);
                ackReceipt->mData[0]=(Token)ID_SND_RECEIPT_ACKED;
                memcpy(ackReceipt->mData+sizeof(Token), &packet->mReceipt, sizeof(packet->mReceipt));
                mDownload.Push(ackReceipt);
            }

            bool ack;
            if (packet->mPacketType == PT_BE_SURE ||
                packet->mPacketType == PT_ONE_MAKE_SURE ||
                packet->mPacketType == PT_ORDER_MAKE_SURE ||
                packet->mPacketType == PT_RECEIPT_MAKE_SURE  ||
                packet->mPacketType == PT_ORDER_RECEIPT_MAKE_SURE)
            {
                ack = true;
            }
            else
            {
                ack = false;
            }

            removeSend(packet, ack);
            N_delete packet;
            return 0;
        }
        return (Nui32)-1;
    }
    //------------------------------------------------------------------------
    void PacketPrc::sendACK(DatagramSequenceNumberType ackmark, TimeDurUS time)
    {
        nextAckTimeToSend = time;
        mACK.Insert(ackmark);

        CC_DEBUG_PRINTF_2("AckPush %i ", ackmark);
    }
    //------------------------------------------------------------------------
    NCount PacketPrc::getMaxContentSize()
    {
        Packet ip;
        ip.mPacketType = PT_ONE_MAKE_SURE;
        ip.mSplitCount = 1;
        return mUsrMTU - N_B_TO_b(ip.getHeaderSize());
    }
    //------------------------------------------------------------------------
    bool PacketPrc::isValid(OrderingIndexType src, OrderingIndexType current)
    {
        OrderingIndexType maxRange = ((OrderingIndexType)(Nui32)-1) / 2;

        if(current > maxRange)
        {
            if(src >= (current - maxRange + 1) && src < current)
            {
                return false;
            }
        }
        else
        {
            if(src >= (current - maxRange + 1) || src < current)
            {
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    void PacketPrc::split(Packet * packet)
    {
        NCount dataByteLength = N_B_TO_b(packet->mBitCount);
        NCount maximumSendBlockBytes = getMaxContentSize();
        packet->mSplitCount = ((dataByteLength - 1) / (maximumSendBlockBytes) + 1);

        Packet ** combine;
        bool usedAlloca = false;
    #if N_AllocaStackSize != 0
        if(sizeof(Packet *) * packet->mSplitCount < N_AllocaStackSize)
        {
            combine = (Packet**)alloca(sizeof(Packet *) * packet->mSplitCount);
            usedAlloca = true;
        }
        else
    #endif
            combine = (Packet**)N_alloc(sizeof(Packet *) * packet->mSplitCount);

        for(NCount i = 0; i < packet->mSplitCount; ++i)
        {
            combine[i] = N_new Packet();
            *combine[i] = *packet;
            combine[i]->mMakeSureInit = false;
        }

        // This identifies which packet this is in the set
        SplitPacketIndexType splitindex = 0;
        PacketDataRef * refCounter = N_alloc_t(PacketDataRef, 1);
        refCounter->mRefCount = 1;
        refCounter->mShare = packet->mData;
        // Do a loop to send out all the packets
        do
        {
            int byteOffset = splitindex * maximumSendBlockBytes;
            int bytesToSend = dataByteLength - byteOffset;

            if(bytesToSend > maximumSendBlockBytes)
                bytesToSend = maximumSendBlockBytes;

            combine[splitindex]->mSplitIndex = splitindex;
            combine[splitindex]->mSplitID = mNextSplitID;
            combine[splitindex]->mSplitCount = packet->mSplitCount;
            combine[splitindex]->mData = packet->mData + byteOffset;
            combine[splitindex]->mAllocType = Packet::Share;
            combine[splitindex]->mRefData = refCounter;
            (*refCounter)->mRefCount++;

            if (bytesToSend != maximumSendBlockBytes)
                combine[splitindex]->mBitCount = packet->mBitCount - splitindex * (maximumSendBlockBytes << 3);
            else
                combine[splitindex]->mBitCount = bytesToSend << 3;

            N_assert(combine[splitindex]->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
        } while (++splitindex < packet->mSplitCount);

        ++mNextSplitID; // It's ok if this wraps to 0

        // Tell the heap we are going to push a list of elements where each element in the list follows the heap order
        N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
        mUploadBuffer.StartSeries();

        NBitCount extcount = packet->getHeaderSize();
        // Copy all the new packets into the split packet list
        for (NCount i = 0; i < packet->mSplitCount; ++i)
        {
            if (i != 0)
                combine[i]->mDebugMark = mDebugSendMark++;
            combine[i]->mExtCount = extcount;
            N_assert(combine[i]->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
            addWillLost(combine[i]);
            N_assert(combine[i]->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
            N_assert(combine[i]->mMakeSureInit == false);
            mUploadBuffer.PushSeries(getNextWeigth(combine[i]->mPacketLevel), combine[i]);
            N_assert(mUploadBuffer.Size() == 0 || mUploadBuffer.Peek()->mBitCount < N_b_TO_B(N_Net_MaxMtuSize));
            mProfile.messageInSendBuffer[combine[i]->mPacketLevel]++;
            mProfile.bytesInSendBuffer[combine[i]->mPacketLevel] += (NIId) N_B_TO_b(combine[ i ]->mBitCount);
        }
        packet->mAllocType = Packet::Refer;
        N_delete packet;

        if(usedAlloca == false)
            N_free(combine);
    }
    //------------------------------------------------------------------------
    Packet * PacketPrc::combine(Packet * packet, TimeDurUS time, SocketObj * s,
		const Address * address, RakNetRandom * rnr, NetSerializer & upload)
    {
        bool objectExists;
        // Find in mSplitPackets if a SplitPacketChannel with this mSplitID was already allocated. If not, allocate and insert the channel into the list.
        Nui32 index = mSplitPackets.GetIndexFromKey(packet->mSplitID, &objectExists);
        if (objectExists == false)
        {
            SplitPacketChannel * newChannel = N_new SplitPacketChannel();
#if N_Socket_PreEnlargePacket == 1
            index = mSplitPackets.Insert(packet->mSplitID, newChannel, true);
            newChannel->gotFirstPacket = false;
            newChannel->splitPacketsArrived = 0;
            newChannel->returnedPacket = Packet::create(packet, 0, 0, readtime);
            newChannel->returnedPacket->mAllocType = Packet::NORMAL;
            newChannel->returnedPacket->mData = (Nui8 *)N_alloc(N_B_TO_b(packet->mBitCount * packet->mSplitCount));
#else
            newChannel->firstPacket = 0;
            index = mSplitPackets.Insert(packet->mSplitID, newChannel, true);
            // Preallocate to the final size, to avoid runtime copies
            newChannel->splitPacketList.Preallocate(packet->mSplitCount);
#endif
        }
        SplitPacketChannel * splits = mSplitPackets[index];
#if N_Socket_PreEnlargePacket == 1
        splits->lastUpdateTime = readtime;
        splits->splitPacketsArrived++;
        splits->returnedPacket->mBitCount += packet->mBitCount;

        bool dealloc;
        if (packet->mSplitIndex == 0)
        {
            splits->gotFirstPacket = true;
            splits->stride = N_B_TO_b(packet->mBitCount);

            for (NCount j = 0; j < splits->splitPacketList.Size(); ++j)
            {
                memcpy(splits->returnedPacket->mData + packet->mSplitIndex * splits->stride, packet->mData, N_B_TO_b(packet->mBitCount));
                N_delete splits->splitPacketList[j];
            }

            memcpy(splits->returnedPacket->mData, packet->mData, N_B_TO_b(packet->mBitCount));
            splits->splitPacketList.Clear(true);
            dealloc = true;
        }
        else
        {
            if (splits->gotFirstPacket == true)
            {
                memcpy(splits->returnedPacket->mData + packet->mSplitIndex * splits->stride, packet->mData, N_B_TO_b(packet->mBitCount));
                dealloc = true;
            }
            else
            {
                splits->splitPacketList.Push(packet);
                dealloc = false;
            }
        }

        if (mSplitMessageFactor && splits->gotFirstPacket &&
            splits->splitPacketsArrived != splits->returnedPacket->mSplitCount &&
            (splits->splitPacketsArrived % mSplitMessageFactor) == 0)
        {
            // Return ID_DOWNLOAD_PROGRESS
            // write mSplitIndex (SplitPacketIndexType)
            // write mSplitCount (SplitPacketIndexType)
            // write byteLength (4)
            // write data, splits->splitPacketList[0]->mData

            Nui32 temp;
            NCount len = sizeof(Token) + sizeof(Nui32) * 2 + sizeof(Nui32) + splits->stride;
            Packet * progress = N_new Packet(len, false);
            progress->mBitCount = N_b_TO_B(len);
            progress->mData[0] = (Token)ID_DOWNLOAD_PROGRESS;
            temp = splits->splitPacketsArrived;
            memcpy(progress->mData + sizeof(Token), &temp, sizeof(Nui32));
            temp = packet->mSplitCount;
            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 1, &temp, sizeof(Nui32));
            temp = N_B_TO_b(splits->stride);
            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 2, &temp, sizeof(Nui32));
            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 3, splits->returnedPacket->mData, N_B_TO_b(splits->stride));
            mDownload.Push(progress);
        }

        if (dealloc)
        {
            N_delete packet;
        }
#else
        splits->splitPacketList.Insert(packet);
        splits->lastUpdateTime = readtime;

        // If the index is 0, then this is the first packet. Record this so it can be returned to the user with download progress
        if (packet->mSplitIndex == 0)
            splits->firstPacket = packet;

        // Return download progress if we have the first packet, the list is not complete, and there are enough packets to justify it
        if (mSplitMessageFactor && splits->firstPacket &&
            splits->splitPacketList.Size() != splits->firstPacket->mSplitCount &&
            (splits->splitPacketList.Size() % mSplitMessageFactor) == 0)
        {
            // Return ID_DOWNLOAD_PROGRESS
            // write mSplitIndex (SplitPacketIndexType)
            // write mSplitCount (SplitPacketIndexType)
            // write byteLength (4)
            // write data, splits->splitPacketList[0]->mData
            Nui32 temp;
            Nui32 length = sizeof(Token) + sizeof(Nui32) * 2 + sizeof(Nui32) + N_B_TO_b(splits->firstPacket->mBitCount);
            Packet * progress = N_new Packet(length, false);
            progress->mBitCount = N_b_TO_B(length);
            progress->mData[0] = (Token)ID_DOWNLOAD_PROGRESS;
            temp = splits->splitPacketList.Size();
            memcpy(progress->mData + sizeof(Token), &temp, sizeof(Nui32));
            temp = packet->mSplitCount;
            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 1, &temp, sizeof(Nui32));
            temp = N_B_TO_b(splits->firstPacket->mBitCount);
            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 2, &temp, sizeof(Nui32));

            memcpy(progress->mData + sizeof(Token) + sizeof(Nui32) * 3, splits->firstPacket->mData, N_B_TO_b(splits->firstPacket->mBitCount));
            mDownload.Push(progress);
        }
#endif

    #if N_Socket_PreEnlargePacket == 1
        if (splits->splitPacketsArrived == splits->returnedPacket->mSplitCount)
    #else
        if (splits->splitPacketList.Size() == splits->splitPacketList[0]->mSplitCount)
    #endif
        {
            sendACK(s, address, time, rnr, upload);
#if N_Socket_PreEnlargePacket == 1
            packet = splits->returnedPacket;
#else
            packet = Packet::create(splits->splitPacketList[0], 0, 0, time);
            packet->mBitCount = 0;

            NCount j;

            for (j = 0; j < splits->splitPacketList.Size(); ++j)
                packet->mBitCount += splits->splitPacketList[j]->mBitCount;
            int splitPacketPartLength = N_B_TO_b(splits->firstPacket->mBitCount);

            packet->mData = (Nui8 *)N_alloc(N_B_TO_b(packet->mBitCount));
            packet->mAllocType = Packet::NORMAL;

            Packet * splitPacket;
            for (j = 0; j < splits->splitPacketList.Size(); ++j)
            {
                splitPacket = splits->splitPacketList[j];
                memcpy(packet->mData + splitPacket->mSplitIndex * splitPacketPartLength, splitPacket->mData, (size_t)N_B_TO_b(splits->splitPacketList[j]->mBitCount));
                N_delete splits->splitPacketList[j];
            }
#endif
            N_delete splits;
            mSplitPackets.RemoveAtIndex(index);
            return packet;
        }
        else
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::addResend(Packet * packet, TimeDurUS, bool, bool ackremain)
    {
        if (ackremain)
        {
            mCheckACK += N_B_TO_b(packet->mExtCount + packet->mBitCount);
        }

        if (mResendHead == 0)
        {
            packet->mNextMakeSure = packet;
            packet->mPreMakeSure = packet;
            mResendHead = packet;
            return;
        }
        packet->mNextMakeSure = mResendHead;
        packet->mPreMakeSure = mResendHead->mPreMakeSure;
        packet->mPreMakeSure->mNextMakeSure = packet;
        mResendHead->mPreMakeSure = packet;

        N_assert(packet->mLastAction != 0);
    }
    //------------------------------------------------------------------------
    bool PacketPrc::isTimeOut() const
    {
        return mTimeOutValid;
    }
	//------------------------------------------------------------------------
	bool PacketPrc::isTimeOut(TimeDurMS current)
	{
		// I check mLastRecevie - current because with threading it is possible that mLastRecevie is
		// slightly greater than current, in which case this is NOT an ack timeout
		return (mLastRecevie - current) > 10000 && current - mLastRecevie > mTimeOut;
	}
    //------------------------------------------------------------------------
    void PacketPrc::setTimeout()
    {
        mTimeOutValid = true;
    }
    //------------------------------------------------------------------------
    void PacketPrc::getReport(NetStats & rns)
    {
        TimeDurUS time = N_Engine().getTimer().getUS();
        Nui64 uint64Denominator;
        NIId doubleDenominator;

        NCount i;
        for (i = 0; i < RNS_PER_SECOND_METRICS_COUNT; ++i)
        {
            mProfile.valueOverLastSecond[i] = mBSP[i].GetBPS1Threadsafe(time);
            mProfile.runningTotal[i] = mBSP[i].GetTotal1();
        }

        memcpy(&rns, &mProfile, sizeof(mProfile));

        if (rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT] + rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]>0)
            rns.packetlossLastSecond = (NIIf)((NIId)rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]/((NIId) rns.valueOverLastSecond[USER_MESSAGE_BYTES_SENT]+(NIId) rns.valueOverLastSecond[USER_MESSAGE_BYTES_RESENT]));
        else
            rns.packetlossLastSecond = 0.0f;

        rns.packetlossTotal = 0.0f;
        uint64Denominator = (rns.runningTotal[USER_MESSAGE_BYTES_SENT] + rns.runningTotal[USER_MESSAGE_BYTES_RESENT]);
        if (uint64Denominator != 0&&rns.runningTotal[USER_MESSAGE_BYTES_SENT]/uint64Denominator>0)
        {
            doubleDenominator = ((NIId) rns.runningTotal[USER_MESSAGE_BYTES_SENT] + (NIId) rns.runningTotal[USER_MESSAGE_BYTES_RESENT]);
            if(doubleDenominator != 0)
            {
                rns.packetlossTotal = (NIIf)((NIId) rns.runningTotal[USER_MESSAGE_BYTES_RESENT]/doubleDenominator);
            }
        }

        rns.isLimitedByCongestionControl = mProfile.isLimitedByCongestionControl;
        rns.BPSLimitByCongestionControl = mProfile.BPSLimitByCongestionControl;
        rns.mUploadLimit = mProfile.mUploadLimit;
        rns.mUploadLimitValue = mProfile.mUploadLimitValue;
    }
    //------------------------------------------------------------------------
    void PacketPrc::addSend(TimeDurUS time, Packet * packet, bool ack)
    {
        NBitCount bit = N_b_TO_B(N_B_TO_b(packet->mBitCount) + N_B_TO_b(packet->mExtCount));
        mUploadRemain += bit;
        N_assert(N_B_TO_b(mUploadRemain) < N_Net_MaxMtuSize - UDP_HEADER_SIZE);
        mCongestionRemain += bit;
        mSend.Push(packet);
        mFree.Push(ack == false);
        N_assert(packet->mExtCount == packet->getHeaderSize());
        ++packet->mSendCount;
        mCongestion.OnSendBytes(time, N_B_TO_b(packet->mBitCount) + N_B_TO_b(packet->mExtCount));
    }
    //------------------------------------------------------------------------
    void PacketPrc::addFragment()
    {
        if (mUploadRemain > 0)
        {
            mUploadCountQueue.Push(mSend.Size());
            mSendPairQueue.Push(false);
            N_assert(N_B_TO_b(mUploadRemain) < N_Net_MaxMtuSize - UDP_HEADER_SIZE);
            mUploadSizeQueue.Push(N_B_TO_b(mUploadRemain));
            mUploadRemain = 0;
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::removeFragment()
    {
        NCount i;
        for (i = 0; i < mFree.Size(); ++i)
        {
            // mFree holds a boolean indicating if mSend at this index should be freed
            if (mFree[i])
            {
                removeWillLost(mSend[i]);
                N_delete mSend[i];
            }
        }
        mFree.Clear(true);
    }
    //------------------------------------------------------------------------
    void PacketPrc::moveSend(Packet * packet)
    {
        if (packet == mResendHead)
            return;
        if (mResendHead == 0)
        {
            packet->mNextMakeSure = packet;
            packet->mPreMakeSure = packet;
            mResendHead = packet;
            return;
        }
        packet->mPreMakeSure->mNextMakeSure = packet->mNextMakeSure;
        packet->mNextMakeSure->mPreMakeSure = packet->mPreMakeSure;
        packet->mNextMakeSure = mResendHead;
        packet->mPreMakeSure = mResendHead->mPreMakeSure;
        packet->mPreMakeSure->mNextMakeSure = packet;
        mResendHead->mPreMakeSure = packet;
        mResendHead = packet;
        N_assert(packet->mExtCount + packet->mBitCount > 0);
    }
    //------------------------------------------------------------------------
    void PacketPrc::removeSend(Packet * packet, bool ackremain)
    {
        Packet * newPosition;
        packet->mPreMakeSure->mNextMakeSure = packet->mNextMakeSure;
        packet->mNextMakeSure->mPreMakeSure = packet->mPreMakeSure;
        newPosition = packet->mNextMakeSure;

        if (packet == mResendHead)
            mResendHead = newPosition;
        if (mResendHead == packet)
            mResendHead = 0;

        if(ackremain)
        {
            N_assert(mCheckACK >= N_B_TO_b(packet->mExtCount + packet->mBitCount));
            mCheckACK -= N_B_TO_b(packet->mExtCount + packet->mBitCount);
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::sendACK(SocketObj * s, const Address * address, TimeDurUS time, 
		RakNetRandom * rnr, NetSerializer & upload)
    {
        NBitCount maxPayload = N_b_TO_B(mUsrMTU);
        while(mACK.Size() > 0)
        {
            DatagramHeaderFormat dhf;
            dhf.isACK = true;
            dhf.isNAK = false;
            dhf.isPacketPair = false;
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
            dhf.sourceSystemTime = time;
#endif
            NIId B;
            NIId AS;
            bool hasBAndAS;
            if (remoteSystemNeedsBAndAS)
            {
                mCongestion.OnSendAckGetBAndAS(time, &hasBAndAS, &B, &AS);
                dhf.AS = (NIIf)AS;
                dhf.hasBAndAS = hasBAndAS;
            }
            else
                dhf.hasBAndAS = false;
#if INCLUDE_TIMESTAMP_WITH_DATAGRAMS == 1
            dhf.sourceSystemTime = nextAckTimeToSend;
#endif
            upload.seekWrite(0);
            upload.seekRead(0);
            dhf.read(&upload);
            CC_DEBUG_PRINTF_1("AckSnd ");
            mACK.read(&upload, maxPayload, true);
            send(s, address, &upload, rnr, time);
            mCongestion.OnSendAck(time,upload.getSize());
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::addWillLost(Packet * packet)
    {
        if (packet->mPacketType == PT_WILL_LOST ||
            packet->mPacketType == PT_ONE_WILL_LOST ||
            packet->mPacketType == PT_RECEIPT_WILL_LOST)
        {
            if (mWillLostHead == 0)
            {
                packet->mNextWillLost = packet;
                packet->mPreWillLost = packet;
                mWillLostHead = packet;
                return;
            }
            packet->mNextWillLost = mWillLostHead;
            packet->mPreWillLost = mWillLostHead->mPreWillLost;
            packet->mPreWillLost->mNextWillLost = packet;
            mWillLostHead->mPreWillLost = packet;
        }
    }
    //------------------------------------------------------------------------
    void PacketPrc::removeWillLost(Packet * packet)
    {
        if(packet->mPacketType == PT_WILL_LOST ||
            packet->mPacketType == PT_ONE_WILL_LOST ||
            packet->mPacketType == PT_RECEIPT_WILL_LOST)
        {
            Packet * newPosition;
            packet->mPreWillLost->mNextWillLost = packet->mNextWillLost;
            packet->mNextWillLost->mPreWillLost = packet->mPreWillLost;
            newPosition = packet->mNextWillLost;
            if(packet == mWillLostHead)
                mWillLostHead = newPosition;
            if(mWillLostHead == packet)
                mWillLostHead = 0;
        }
    }
    //------------------------------------------------------------------------
    bool PacketPrc::isResendQueueValid() const
    {
        int index1 = mACKSendMark & RESEND_BUFFER_ARRAY_MASK;
        N_assert(index1 < RESEND_BUFFER_ARRAY_LENGTH);
        return mResend[index1] != 0;
    }
    //------------------------------------------------------------------------
    PacketPrc::MessageNumberNode * PacketPrc::getMessageNode(
        DatagramSequenceNumberType index, TimeDurUS & send_t)
    {
        if (mDatagram.IsEmpty())
            return 0;

        if (mCongestion.LessThan(index, mSendMark))
            return 0;

        DatagramSequenceNumberType offsetIntoList = index - mSendMark;
        if (offsetIntoList >= mDatagram.Size())
            return 0;

        send_t = mDatagram[offsetIntoList].mSendTime;
        return mDatagram[offsetIntoList].mHead;
    }
    //------------------------------------------------------------------------
    void PacketPrc::removeMessageNode(DatagramSequenceNumberType index)
    {
        DatagramSequenceNumberType offsetIntoList = index - mSendMark;
        MessageNumberNode * mnm = mDatagram[offsetIntoList].mHead;
        MessageNumberNode * next;
        while (mnm)
        {
            next = mnm->mNext;
            N_free(mnm);
            mnm = next;
        }
        mDatagram[offsetIntoList].mHead = 0;
    }
    //------------------------------------------------------------------------
    void PacketPrc::addMessage(DatagramSequenceNumberType mark, TimeDurMS send)
    {
        (void)mark;
        if (mDatagram.Size() > DATAGRAM_MESSAGE_ID_ARRAY_LENGTH)
        {
            removeMessageNode(mSendMark);
            mDatagram.Pop();
            ++mSendMark;
        }

        mDatagram.Push(DatagramHistoryNode(0, send));
    }
    //------------------------------------------------------------------------
    PacketPrc::MessageNumberNode * PacketPrc::addMessage(DatagramSequenceNumberType mark, 
        DatagramSequenceNumberType ackmark, TimeDurMS send)
    {
        (void)mark;
        if (mDatagram.Size() > DATAGRAM_MESSAGE_ID_ARRAY_LENGTH)
        {
            removeMessageNode(mSendMark);
            mDatagram.Pop();
            ++mSendMark;
        }

        MessageNumberNode * mnm = N_alloc_t(MessageNumberNode, 1);
        mnm->mNext = 0;
        mnm->mACKMark = ackmark;
        mDatagram.Push(DatagramHistoryNode(mnm, send));
        return mnm;
    }
    //------------------------------------------------------------------------
    PacketPrc::MessageNumberNode * PacketPrc::addMessageNode(MessageNumberNode * msgnode, 
        DatagramSequenceNumberType ackmark)
    {
        msgnode->mNext = N_alloc_t(MessageNumberNode, 1);
        msgnode->mNext->mACKMark = ackmark;
        msgnode->mNext->mNext = 0;
        return msgnode->mNext;
    }
    //------------------------------------------------------------------------
    Nui64 PacketPrc::getNextWeigth(int level)
    {
        Nui64 next = mUploadWeight[level];
        if (mUploadBuffer.Size() > 0)
        {
            int peekPL = mUploadBuffer.Peek()->mPacketLevel;
            Nui64 weight = mUploadBuffer.PeekWeight();
            Nui64 min = weight - (1 << peekPL) * peekPL + peekPL;
            if (next < min)
                next = min + (1 << level) * level + level;
            mUploadWeight[level] = next + (1 << level) * (level + 1) + level;
        }
        else
        {
            for (NCount i = 0; i < PL_COUNT; ++i)
                mUploadWeight[i] = (1 << i) * i + i;
        }
        return next;
    }
    //------------------------------------------------------------------------
}
}