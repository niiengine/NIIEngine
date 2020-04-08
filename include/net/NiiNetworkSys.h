/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_NetworkSys_H_
#define _NII_NetworkSys_H_

#include "NiiNetPreInclude.h"
#include "NiiNetSerializer.h"
#include "NiiThreadManager.h"
#include "NiiSharedPtr.h"

namespace NII
{
namespace NII_NET
{
    /** 
	@note 传输层概念
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NetworkSys
    {
    public:
        enum ConnectMode
        {
            CM_None,
            CM_Request,
            CM_Process,
            CM_Identify,
            CM_Connect,
            CM_Close,
            CM_CloseNotify,
            CM_ShutDown
        }
    public:
        NetworkSys();
        virtual ~NetworkSys();

        /**
        @version NIIEngine 3.0.0
        */
        void setup();

        /** 初始化本地连接点
        @version NIIEngine 3.0.0
        */
        StartupResult listen(const HostPeerList & hosts, NCount maxLink, NIIi prclevel = N_ThreadPriorityHigh);

        /** 连接
        @version NIIEngine 3.0.0
        */
        virtual ConnectResultType connect(const Address * dst, const String & password, SocketObj * socket, 
            Nindex sindex = 0, PublicKey * key = 0, NCount attempt = 5, TimeDurMS interval = 1000, TimeDurMS timeout = 10000);

        /**
        @param[in] data
        @param[in] size
        @version NIIEngine 3.0.0
        */
        Nui32 send(const Nui8 * data, NCount size, PacketLevel plevel, PacketType ptype, PacketCH pch, 
            const UniqueAddress & dst, bool broadcast, Nui32 receipt = 0);
            
        /** 发送到自己
        @param[in] data
        @param[in] size
        @version NIIEngine 3.0.0
        */
        void send(const Nui8 * data, NCount size);

        /** 
        @param[in] host
        @param[in] port
        @param[in] data
        @param[in] size
        @param[in] sindex
        @version NIIEngine 3.0.0
        */
        bool send(const String & host, Nui16 port, const Nui8 * data, NCount size, Nindex sindex = 0);
        
        /**
        @version NIIEngine 3.0.0
        */
        bool sendExpedite(const String & host, Nui16 port, const Nui8 * data, NCount size, Nindex sindex = 0);

        /**
        @version NIIEngine 3.0.0
        */
        void close(const UniqueAddress & dst, bool notify, PacketCH pch = 0, PacketLevel plevel = PL_LOW);

        /**
        @version NIIEngine 3.0.0
        */
        void cancel(const Address * dst);

        /**
        @version NIIEngine 3.0.0
        */
        SocketMessage * receive();

        /**
        @version NIIEngine 3.0.0
        */
        void shutdown(TimeDurMS delay, PacketCH pch = 0, PacketLevel plevel = PL_LOW);

        /**
        @version NIIEngine 3.0.0
        */
        bool isActive() const;

        /**
        @param[in] host
        @version NIIEngine 3.0.0
        */
        void addLocalHost(const Address * host);

        /**
        @param[in] index
        @version NIIEngine 3.0.0
        */
        String getLocalHost(Nindex index);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getLocalHostCount();

        /**
        @param[in] index
        @param[in] host
        @version NIIEngine 3.0.0
        */
        Address * getLAN(Nindex index, const Address * host) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isLocalHost(const String & ip);

        /**
        @param[in] count
        @version NIIEngine 3.0.0
        */
        void setMaxRemoteCount(NCount cnt);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getMaxRemoteCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        NCount getRemoteCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        NCount getActiveRemoteCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setEnterPassword(const String & pw);

        /**
        @version NIIEngine 3.0.0
        */
        const String & getEnterPassword() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setConnectTimeLimit(bool b);
        
        /**
        @version NIIEngine 3.0.0
        */
        bool isConnectTimeLimit() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setAlterIP(bool b);

		/**
        @version NIIEngine 3.0.0
		*/
        bool getConnect(AddressList & remotes) const;

        /**
        @param[in] host
        @param[in] port
        @version NIIEngine 3.0.0
        */
        void sendTTL(const String & host, Nui16 port, NCount ttl, Nindex sindex = 0);

        /**
        @param[in] uadr
        @version NIIEngine 3.0.0
        */
        ConnectionState getState(const UniqueAddress & uadr);

        /**
        @param[in] index
        @version NIIEngine 3.0.0
        */
        Address * getAddress(Nindex index);
        
        /**
        @version NIIEngine 3.0.0
        */
        Address * getAddress(const UniqueID & input) const;

        /**
        @version NIIEngine 3.0.0
        */
        const UniqueID & getGUID(Nindex index);

        /**
        @version NIIEngine 3.0.0
        */
        const UniqueID & getGUID(const Address * input) const;

        /**
        @version NIIEngine 3.0.0
        */
        void getActive(UniqueAddressList & uaddr) const;

        /**
        @version NIIEngine 3.0.0
        */
        void ping(const Address * dst);

        /**
        @version NIIEngine 3.0.0
        */
        bool ping(const String & host, Nui16 port, bool connect, Nindex sindex = 0);

        /**
        @version NIIEngine 3.0.0
        */
        int getAvgPing(const UniqueAddress & uadr) const;

        /**
        @version NIIEngine 3.0.0
        */
        int getLastPing(const UniqueAddress & uadr) const;

        /**
        @version NIIEngine 3.0.0
        */
        int getBestPing(const UniqueAddress & uadr) const;

        /**
        @version NIIEngine 3.0.0
        */
        int getWorstPing(const UniqueAddress & uadr) const;

        /** 设置ping测试
        @version NIIEngine 3.0.0
        */
        void setPingTest(bool b);

        /** 获取时差
        @remark
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeEquation(const UniqueAddress & uadr);

        /**
        @version NIIEngine 3.0.0
        */
        void setTempPing(const Nui8 * data, NCount size);

        /**
        @version NIIEngine 3.0.0
        */
        void getTempPing(Nui8 ** data, NCount * size);

        /**
        @version NIIEngine 3.0.0
        */
        Address * getConnectTo(const Address * dst) const;

        /**
        @version NIIEngine 3.0.0
        */
        const UniqueID & getGuid() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setTimeOut(const Address * dst, TimeDurMS time);

        /**
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeOut(const Address * dst);
        
        /**
        @version NIIEngine 3.0.0
        */
        void setMessageTimeOut(TimeDurMS ms);
        
        /**
        @version NIIEngine 3.0.0
        */
        TimeDurMS getMessageTimeOut() const;   

        /**
        @version NIIEngine 3.0.0
        */
        NCount getMTU(const Address * dst) const;

        /** 获取加密列表
        @version NIIEngine 3.0.0
        */
        Secure * getSecure() const;

        /** 获取限制列表
        @version NIIEngine 3.0.0
        */
        Ban * getBan() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitFactor(NCount interval);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getSplitFactor() const;
        
        /**
        @version NIIEngine 3.0.0
        */
        void setSendSize(NCount count);

        /**
        @version NIIEngine 3.0.0
        */
        void attach(Plugin * plugin);

        /**
        @version NIIEngine 3.0.0
        */
        void detach(Plugin * plugin);

        /**
        @version NIIEngine 3.0.0
        */
        void addMessage(SocketMessage * msg, bool head);

        /**
        @version NIIEngine 3.0.0
        */
        void sendSwap(UniqueID id, const Address * dst);

        /**
        @version NIIEngine 3.0.0
        */
        virtual SocketObj * getSocket(const Address * dst);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void getSockets(SocketObjList & sockets);

        /**
        @version NIIEngine 3.0.0
        */
        NetStats * getReport(const Address * dst, NetStats * rns = 0);

        /**
        @version NIIEngine 3.0.0
        */
        bool getReport(Nindex index, NetStats & rns);

        /**
        @version NIIEngine 3.0.0
        */
        virtual SocketMessageData * createMessage();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void destroyMessage(SocketMessageData * msg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMessage(SocketMessageData * msg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void getReport(UniqueAddressList & guids, NetStatsList & slist);
    protected:
        /**
        @param[in] id
        @version NIIEngine 3.0.0
        */
        Nindex getRemoteIndex(const UniqueID & id);

        /**
        @param[in] dst
        @param[in] thread
        @version NIIEngine 3.0.0
        */
        Nindex getRemoteIndex(const Address * dst, bool thread = false) const;

        /**
        @param[in] dst
        */
        Nindex getIndexImpl(const Address * dst) const;

        /**
        @param[in]
        @param[in]
        @param[in]
        @version NIIEngine 3.0.0
        */
        ConnectResultType connectImpl(const Address * dst, const String & password, PublicKey * key, 
            NCount attempt, TimeDurMS interval, TimeDurMS timeout, Nindex sindex, SocketObj * socket = 0);

        /**
        @param[in] data
        @param[in]
        @param[in]
        @param[in]
        @param[in]
        @version NIIEngine 3.0.0
        */
        void sendImpl(const Nui8 * data, NBitCount count, PacketLevel plevel, PacketType ptype, PacketCH pch,
			const UniqueAddress & uadr, bool broadcast, ConnectMode mode, Nui32 receipt);

        /**
        @param[in] data
        @param[in]
        @param[in]
        @param[in]
        @param[in]
        @version NIIEngine 3.0.0
        */
        bool sendRapid(Nui8 * data, NBitCount count, PacketLevel plevel, PacketType ptype, PacketCH pch, 
			const UniqueAddress & uadr, bool broadcast, bool calleralloc, TimeDurUS current, Nui32 receipt);

        /**
        @param[in] dst
        @param[in] rapid
        @param[in] ptype
        @version NIIEngine 3.0.0
        */
        void pingImpl(const Address * dst, bool rapid, PacketType ptype);

        /**
        @param[in] uadr
        @param[in] notify
        @param[in] rapid
        @version NIIEngine 3.0.0
        */
        void closeImpl(const UniqueAddress & uadr, bool notify, bool rapid, PacketCH pch, PacketLevel plevel);

        /**
        @param[in] id
        @param[in] active
        @version NIIEngine 3.0.0
        */
        Remote * getRemoteSystem(const UniqueID & id, bool active) const;

        /**
        @param[in] dst
        @param[in] thread
        @param[in] active
        @version NIIEngine 3.0.0
        */
        Remote * getRemoteSystem(const Address * dst, bool thread = true, bool active = false) const;

        /**
        @version NIIEngine 3.0.0
        */
        Remote * getRemoteSystem(const UniqueAddress & uadr, bool thread, bool active) const;

        /**
        @version NIIEngine 3.0.0
        */
        void shutdown(const Address * dst, bool rapid, PacketCH pch, PacketLevel plevel);

        /**
        @version NIIEngine 3.0.0
        */
        Remote * createConnect(const Address * dst, SocketObj * socket, bool & recently, Address * addr, 
            NCount mtu, UniqueID id, bool useSecurity);

        /**
        @version NIIEngine 3.0.0
        */
        void processConnect(Remote * remote, const Address * dst, const Nui8 * data, NCount size);

        /**
        @version NIIEngine 3.0.0
        */
        void process(SocketMessage * msg, PluginList & plist);

        /** 获取时差
        @remark
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeEquation(const Address * dst) const;

        /**
        @version NIIEngine 3.0.0
        */
        bool isLocalHost(const UniqueAddress & uadr, bool port) const;

        /**
        @version NIIEngine 3.0.0
        */
        void attach(const Address * src, Nindex index);

        /**
        @version NIIEngine 3.0.0
        */
        void detach(const Address * src);
    protected:
        /**
        @version NIIEngine 3.0.0
        */
        void onConnect(Remote * remote, TimeDurMS send, TimeDurMS receive);

        /**
        @version NIIEngine 3.0.0
        */
        void onRequest(Remote * remote, TimeDurMS time);

        friend bool process(Address * dst, const Nui8 * data, NCount size, NetworkSys * sys, SocketObj * socket, TimeDurUS read);

        friend void process(Address * dst, const Nui8 * data, NCount size, NetworkSys * sys, SocketObj * socket, TimeDurUS read, NetSerializer & upload);
    protected:
        typedef vector<Remote *>::type RemoteList;
        typedef vector<SocketObj *>::type SocketList;
        typedef list<SocketMessageData *>::type MsgDataList;
        
        PluginList mPluginList;
        UniqueID mLocalHostID;
        AddressList mLocalHost;
        SocketList mSocketList;
        RemoteList mRemote;
        RemoteList mActiveRemote;
        Ban * mBan;
        Secure * mSecure;
        Thread * mThread;
        ThreadEvent * mInputEvent;
        NCount mMaxRemoteCount;
        String mPassword;
		TimeDurMS mMessageTimeOut;
        TimeDurMS mTimeOut;
        NCount mMTUSize;
        list<OutMessage *>::type mSendList;
        list<SocketMessage *>::type mReceiveList;
        list<ConnectRequest *>::type mRequestConnectList;
        list<SocketList>::type mQuerySocketList;
        vector<RemoteHash *>::type mRemoteHashList;
        MsgDataList mMessagePoolList;
        MsgDataList mReveiceDataList;

        ThreadMutex mReceiveMute;
        ThreadMutex mSendMute;
        ThreadMutex mRequestConnectMutex;
        ThreadMutex mPingDataMutex;
        ThreadMutex mNextReceiptMutex;
        ThreadMutex mCreateMessageMutex;
        ThreadMutex mReveiceMessageMute;
        NetSerializer mPingData;
        NCount mSplitMessageFactor;
        NCount mSendSize;
        Nui32 mNextReceipt;
        volatile bool mRun;
        volatile bool mStop;
        bool mConnectTimeLimit;
        bool mPingTest;
        bool mAlterIP;
    };
}
}
#endif