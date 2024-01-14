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

#ifndef __NII_PACKET_MANAGER_H_
#define __NII_PACKET_MANAGER_H_

#include "NiiPreProcess.h"
#include "DS_LinkedList.h"
#include "PacketLevel.h"
#include "DS_Queue.h"
#include "NiiNetSerializer.h"
#include "NetStats.h"
#include "DR_SHA1.h"
#include "DS_OrderedList.h"
#include "DS_RangeList.h"
#include "DS_BPlusTree.h"
#include "NiiNetPlat.h"
#include "DS_Heap.h"
#include "NiiNetSerializer.h"
#include "NativeFeatureIncludes.h"
#include "SecureHandshake.h"
#include "Rand.h"
#include "SocketObj.h"
#include "NiiNetCommon.h"

#if N_Socket_SlidingWindow != 1
    #define INCLUDE_TIMESTAMP_WITH_DATAGRAMS 1
#else
    #define INCLUDE_TIMESTAMP_WITH_DATAGRAMS 0
#endif

#define N_NET_CH 32 // 2^5

namespace NII
{
namespace NII_NET
{
    typedef Nui16 SplitPacketIdType;
    typedef Nui32 SplitPacketIndexType;
    /// This is the counter used for holding packet numbers, so we can detect duplicate packets.  It should be large enough that if the variables
    /// Internally assumed to be 4 bytes, but written as 3 bytes in PacketPrc::write
    typedef uint24_t MessageNumberType;
    /// This is the counter used for holding ordered packet numbers, so we can detect out-of-order packets.  It should be large enough that if the variables
    /// were to wrap, the newly wrapped values would no longer be in use.  Warning: Too large of a value wastes bandwidth!
    typedef uint24_t OrderingIndexType;

    struct PacketDataRef
    {
        Nui8 * mShare;
        Nui32 mRefCount;
    };
    typedef vector<Packet *>::type PacketList;
    class _EngineAPI Packet
    {
    public:
        enum AllocType
        {
            NORMAL,
            Share,
            Refer,
            STACK
        };
    public:
        Packet();
        Packet(Nui8 * data);
        Packet(NCount size, bool stack);
        Packet(PacketDataRef ** ref, Nui8 * data, Nui8 * offset);
        ~Packet();

        static Packet * create(NetSerializer * src, TimeDurUS time);

        static Packet * create(Packet * src, NCount offset, NCount size, TimeDurUS time);

        NBitCount read(NetSerializer * target) const;

        void destroy();

        NBitCount getHeaderSize();
    private:
        void split(PacketList & packet, NCount blocksize);

        Packet * combine();
    public:
        TimeDurUS mCreate;
        TimeDurUS mLastAction;
        TimeDurUS mResendTime;

        Nui8 * mData;
        PacketDataRef * mRefData;
        AllocType mAllocType;
        NBitCount mBitCount;            ///< 数据大小(单位:位)
        NBitCount mExtCount;            ///< 额外大小(单位:位)
        Nui8 mPacketCh;                 ///< 使用的order通道
        PacketLevel mPacketLevel;
        MessageNumberType mACKMark;     ///< 确保包能到达的标记
        MessageNumberType mDebugMark;
        PacketType mPacketType;         ///< 包类型
        OrderingIndexType mOrder;       ///<
        OrderingIndexType mSequence;
        SplitPacketIdType mSplitID;
        SplitPacketIndexType mSplitIndex;
        SplitPacketIndexType mSplitCount;
        Nui32 mReceipt;
        bool mMakeSureInit;
        Packet * mPreMakeSure, *mNextMakeSure, *mPreWillLost, *mNextWillLost;
        Nui8 mSendCount;                ///< 总发送次数
        Nui8 mStackData[128];
    };

    int _EngineAPI SplitPacketChannelComp(SplitPacketIdType const & key, SplitPacketChannel * const & data);

    class _EngineAPI PacketListener
    {
    public:
        virtual ~PacketListener(){}

        /// \param[in] count How many bits long \a data is
        /// \param[in] remote Which system this message is being sent to
        virtual void onReport(const Nui8 * error, NBitCount count, Address * remote, bool isError)
        {
            (void) error;
            (void) count;
            (void) remote;
            (void) isError;
        }
    };
    /**
	@note 表示层涵盖二级传输层概念
    @version NIIEngine 3.0.0
    */
    class PacketPrc
    {
        friend class NetLink;
    public:
        PacketPrc();
        ~PacketPrc();

        /** 设置超时时限
        @param[in] time 单位毫秒
        @version NIIEngine 3.0.0
        */
        void setTimeOut(TimeDurMS time);

        /** 获取超时时限
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeOut();

        /** 当前链接是否超时
        @version NIIEngine 3.0.0
        */
        bool isTimeOut() const;

        /** 当前时间是否超时
        @version NIIEngine 3.0.0
        */
        bool isTimeOut(TimeDurMS curTime);

		/** 设置超时状态
		@version NIIEngine 3.0.0
		*/
		void setTimeout();

        /**
        @version NIIEngine 3.0.0
        */
        void setMessageTimeOut(TimeDurMS time);

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitFactor(int interval);

        /** 
        @param[in] address
        @param[in] buffer
        @param[in] length
        @param[in] plugins
        @param[in] mtu
        */
        bool process(const Address * address, const Nui8 * buffer, NCount length, NCount mtu, SocketObj * s,
            RakNetRandom * rnr, TimeDurUS read, NetSerializer & upload);

        /** Call once per game cycle.  Handles internal lists and actually does the send.
        @param[in] s the communication  end point
        @param[in] address The Unique Player Identifier who shouldhave sent some packets
        @param[in] mtu maximum datagram size
        @param[in] read current system time
        @param[in] maxBitsPerSecond if non-zero, enforces that outgoing bandwidth does not exceed this amount
        @param[in] plugins A list of registered plugins
        */
        void process(SocketObj * s, const Address * address, NCount mtu, TimeDurUS read, Nui32 bitsPerSecondLimit, 
            RakNetRandom * rnr, NetSerializer & upload);

        /** Puts data on the send queue
        @param[in] data The data to send
        @param[in] bitcount The length of \a data in bits
        @param[in] plevel The plevel level for the send
        @param[in] ptype The ptype for the send
        @param[in] pchannel 0 to 31.  Specifies what channel to use, for relational ordering and sequencing of packets.
        @param[in] makeDataCopy If true \a data will be copied.  Otherwise, only a pointer will be stored.
        @param[in] mtu maximum datagram size
        @param[in] currentTime Current time, as per N_Engine().getTimer().getMS()
        @param[in] receipt This number will be returned back with ID_SND_RECEIPT_ACKED or ID_SND_RECEIPT_LOSS and is only returned with the ptype that contain RECEIPT in the name
        @return True or false for success or failure.
        */
        bool send(Nui8 * data, NBitCount count, PacketLevel plevel, PacketType ptype,
            Nui8 pchannel, bool copy, NCount mtu, TimeDurUS current, Nui32 receipt);

        /**
        @param[out]
        */
        NBitCount receive(Nui8 ** data);

        /**
        @param[out]
        */
        NCount getMaxContentSize();

        /** 获取统计报告
        @return A pointer to a static struct, filled out with current statistical information.
        */
        void getReport(NetStats & rns);
    protected:
        /// 是否存在需要发送出去的数据
        bool isUpload();

        /// 是否存在需要发送出去的回馈
        bool isResponse();

		///
        void onMessage();

		///
        void onSend();
		
		///
        void onReset();
    private:
        /// Initialize the variables
        void reset();

        /// Resets the layer for reuse
        void reset(bool value, NCount mtu, bool secure);

        /// send the contents of a stream to the socket
        /// \param[in] s The socket used for sending data
        /// \param[in] address The address and port to send to
        /// \param[in] stream The data to send.
        void send(SocketObj * s, const Address * address, NetSerializer * stream, RakNetRandom * rnr, TimeDurUS currentTime);

		/** 添加数据发送
		*/
        void addSend(TimeDurUS time, Packet * packet, bool ack);

        /** 添加重发
		*/
        void addResend(Packet * packet, TimeDurUS time, bool firstResend, bool ackremain);

        /**
        */
        Nui32 removeResend(const MessageNumberType & ackmark, TimeDurUS time, const const Address * address);

        /**
        */
        void addWillLost(Packet * packet);

        /**
        */
        void removeWillLost(Packet * packet);

        /** 发送ACK确认
		*/
        void sendACK(DatagramSequenceNumberType ackmark, TimeDurUS time);

		/** 发送ACK确认
		*/
        void sendACK(SocketObj * s, const Address * address, TimeDurUS time, RakNetRandom * rnr, NetSerializer & upload);

        /// Returns true if src is older than the current
        bool isValid(OrderingIndexType src, OrderingIndexType current);

        /** 合成分割数据包
		*/
        Packet * combine(Packet * packet, TimeDurUS time, SocketObj * s, const Address * address, RakNetRandom * rnr, NetSerializer & upload);

        void cleanup(bool immediately);

        MessageNumberNode * addMessageNode(MessageNumberNode * messageNumberNode, DatagramSequenceNumberType ackmark);
        
        void removeMessageNode(DatagramSequenceNumberType index);
        
        MessageNumberNode * getMessageNode(DatagramSequenceNumberType index, TimeDurUS & send_t);

        void addMessage(DatagramSequenceNumberType mark, TimeDurUS send_t);
        
        MessageNumberNode * addMessage(DatagramSequenceNumberType mark, DatagramSequenceNumberType ackmark, TimeDurUS send_t);

        Nui64 getNextWeigth(int level);

        bool isResendQueueValid() const;

        void addFragment();
        void removeFragment();
        void moveSend(Packet * packet);
        void removeSend(Packet * packet, bool ackremain);
    private:
        struct MessageNumberNode
        {
            DatagramSequenceNumberType mACKMark;
            MessageNumberNode * mNext;
        };

        struct DatagramHistoryNode
        {
            DatagramHistoryNode() {}
            DatagramHistoryNode(MessageNumberNode * _head, TimeDurMS ts) :
                mHead(_head),
                mSendTime(ts){}
            MessageNumberNode * mHead;
            TimeDurMS mSendTime;
        };

        struct UnreliableWithAckReceiptNode
        {
            UnreliableWithAckReceiptNode() {}
            UnreliableWithAckReceiptNode(DatagramSequenceNumberType mark,
                Nui32 receipt, TimeDurUS lastact) :
                    mMark(mark),
                    mReceipt(receipt),
                    mLastAction(lastact){}

            DatagramSequenceNumberType mMark;
            Nui32 mReceipt;
            TimeDurMS mLastAction;
        };

        // Queue length is programmatically restricted to DATAGRAM_MESSAGE_ID_ARRAY_LENGTH
        // This is essentially an O(1) lookup to get a DatagramHistoryNode given an index
        // mDatagram holds a linked list of MessageNumberNode. Each MessageNumberNode refers to one element in resendList which can be cleared on an ack.
        DataStructures::Queue<DatagramHistoryNode> mDatagram;
        DataStructures::Queue<Packet *> mDownload;
        DataStructures::List<UnreliableWithAckReceiptNode> mWillLostReceipt;
        DataStructures::Heap<Nui64, Packet *, false> mUploadBuffer;
        DataStructures::OrderedList<SplitPacketIdType, SplitPacketChannel *, SplitPacketChannelComp> mSplitPackets;

        NCount mUsrMTU;        ///< 实际用户能使用的的包大小
        int mSplitMessageFactor;
        TimeDurMS mMessageTimeOut;
        DatagramSequenceNumberType mSendMark;
        Packet * mResend[RESEND_BUFFER_ARRAY_LENGTH];
        Packet * mResendHead;
        Packet * mWillLostHead;
        TimeDurMS mLastRecevie;
        Nui64 mUploadWeight[PL_COUNT];
        MessageNumberType mACKSendMark;
        MessageNumberType mDebugSendMark;
        bool mTimeOutValid;
        SplitPacketIdType mNextSplitID;
        TimeDurMS mTimeOut;
        NetStats mProfile;

        // Algorithm for blending ordered and sequenced on the same channel:
        // 1. Each ordered message transmits OrderingIndexType mOrderExport. There are N_NET_CH independent values of these. The value
        //    starts at 0. Every time an ordered message is sent, the value increments by 1
        // 2. Each sequenced message contains the current value of mOrderExport for that channel, and additionally OrderingIndexType mSequenceExport.
        //    mSequenceExport resets to 0 every time mOrderExport increments. It increments by 1 every time a sequenced message is sent.
        // 3. The receiver maintains the next expected value for the mOrderExport, stored in mOrderImport.
        // 4. As messages arrive:
        //    If a message has the current ordering index, and is sequenced, and is < the current highest sequence value, discard
        //    If a message has the current ordering index, and is sequenced, and is >= the current highest sequence value, return immediately
        //    If a message has a greater ordering index, and is sequenced or ordered, buffer it
        //    If a message has the current ordering index, and is ordered, buffer, then push off messages from buffer
        // 5. Pushing off messages from buffer:
        //    Messages in buffer are put in a minheap. The value of each node is calculated such that messages are returned:
        //    A. (lowest ordering index, lowest sequence index)
        //    B. (lowest ordering index, no sequence index)
        //    Messages are pushed off until the heap is empty, or the next message to be returned does not preserve the ordered index
        //    For an empty heap, the heap weight should start at the lowest value based on the next expected ordering index, to avoid variable overflow

        OrderingIndexType mOrderExport[N_NET_CH];
        OrderingIndexType mOrderImport[N_NET_CH];
        OrderingIndexType mSequenceExport[N_NET_CH];
        OrderingIndexType mSequenceImport[N_NET_CH];
        DataStructures::Heap<Nui64, Packet *, false> mOrderQueue[N_NET_CH];
        OrderingIndexType mOrderQueueMark[N_NET_CH];

        /// Memory-efficient receivedPackets algorithm:
        /// receivedPacketsBaseIndex is the packet number we are expecting
        /// Everything under receivedPacketsBaseIndex is a packet we already got
        /// Everything over receivedPacketsBaseIndex is stored in hasReceivedPacketQueue
        /// It stores the time to stop waiting for a particular packet number, where the packet number is receivedPacketsBaseIndex + the index into the queue
        /// If 0, we got got that packet.  Otherwise, the time to give up waiting for that packet.
        /// If we get a packet number where (receivedPacketsBaseIndex-packetNumber) is less than half the range of receivedPacketsBaseIndex then it is a duplicate
        /// Otherwise, it is a duplicate packet (and ignore it).
        DataStructures::Queue<bool> hasReceivedPacketQueue;
        DatagramSequenceNumberType receivedPacketsBaseIndex;
        bool resetReceivedPackets;

        TimeDurUS lastUpdateTime;
        Nui8 ackPingIndex;
        TimeDurUS remoteSystemTime;
        bool bandwidthExceededStatistic;

        // If process::maxBitsPerSecond > 0, then throughputCapCountdown is used as a timer to prevent sends for some amount of time after each send, depending on
        // the amount of data sent
        long long throughputCapCountdown;

        Nui32 receivePacketCount;
        TimeDurUS elapsedTimeSinceLastUpdate;
        TimeDurUS nextAckTimeToSend;

#if N_Socket_SlidingWindow == 1
        CCRakNetSlidingWindow mCongestion;
#else
        CCRakNetUDT mCongestion;
#endif
        Nui32 mCheckACK;
        DataStructures::List<Packet *> mSend;
        DataStructures::List<bool> mFree;
        // boundary is in mSend, inclusive
        DataStructures::List<Nui32> mUploadCountQueue;
        DataStructures::List<Nui32> mUploadSizeQueue;
        DataStructures::List<bool> mSendPairQueue;
        NBitCount mUploadRemain;
        NBitCount mCongestionRemain;
        NIId totalUserDataBytesAcked;
        TimeDurUS timeOfLastContinualSend;
        TimeDurUS timeToNextUnreliableCull;

        // Every 16 datagrams, we make sure the 17th datagram goes out the same update tick, and is the same size as the 16th
        int countdownToNextPacketPair;

        DataStructures::RangeList<DatagramSequenceNumberType> mACK;
        DataStructures::RangeList<DatagramSequenceNumberType> mNAK;
        DataStructures::RangeList<DatagramSequenceNumberType> mTempACK;

        bool remoteSystemNeedsBAndAS;
        PacketListener * mListener;
        TimeDurUS mResetBSPTime;
        BPSTracker mBSP[RNS_PER_SECOND_METRICS_COUNT];
#if LIBCAT_SECURITY == 1
    public:
        cat::AuthenticatedEncryption * GetAuthenticatedEncryption()
        {
            return &auth_enc;
        }
    protected:
        cat::AuthenticatedEncryption auth_enc;
        bool useSecurity;
#endif // LIBCAT_SECURITY
    };
}
}
#endif