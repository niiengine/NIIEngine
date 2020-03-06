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

#ifndef _NII_NETWORKOBJ_H_
#define _NII_NETWORKOBJ_H_

#include "NiiNetPreInclude.h"
#include "NiiThreadManager.h"
#include "NiiNetCommon.h"
#include "NiiNetPlugin.h"

namespace NII
{
namespace NII_NET
{
    /** 网络对象
    @version NIIEngine 4.0.0
    */
    class _EngineAPI NetworkObj
    {
        friend class NetObjSocketPrc;
    public:
        NetworkObj();
        virtual ~NetworkObj();
        
        /** 是否运行
        @version NIIEngine 4.0.0
        */
        bool isRun() const;
        
        /** 运行
        @version NIIEngien 4.0.0
        */
        bool run(Nui16 maxLink, NIIi prclevel = N_ThreadPriorityHigh);

        /** 停止运作
        @version NIIEngine 4.0.0
        */
        void stop();
        
        /** 创建服务端(监听)
        @version NIIEngien 4.0.0
        */
        Nid listen(const String & ip, Nui16 port, Nui16 maxLink = 64, Nui16 sfamily = AF_INET, 
            const String & certfile = "", const String & keyfile = "", const String & keypw = "");

        /** 创建客服端
        @version NIIEngine 4.0.0
        */
        Address * connect(const String & host, Nui16 port, Nui16 sfamily = AF_INET, 
            const String & certfile = "", const String & keyfile = "", const String & keypw = "");

        /** 关闭连接
        @version NIIEngine 4.0.0
        */
        void close(const Address & address);
        
        /** 启用SSL加密
        @version NIIEngine 4.0.0
        */
        void setSSL(const Address & address, bool set = true);

        /** 是否启用SSL加密
        @version NIIEngine 4.0.0
        */
        bool isSSLActive(const Address & address);

        /** 发送
        @version NIIEngine 4.0.0
        */
        virtual void send(const Nui8 * data, NCount size, const Address & address, bool broadcast);

        /** 获取发送剩余大小
        @version NIIEngine 4.0.0
        */
        Nui32 getOutputSize(const Address & address) const;

        /** 接收
        @version NIIEngine 4.0.0
        */
        virtual SocketMessage * receive();

        /** 获取所有连接
        @version NIIEngine 4.0.0
        */
        void getConnect(AddressList & remotelist) const;

        /** 接收(主动添加)
        @version NIIEngine 4.0.0
        */
        void addMessage(SocketMessage * msg, bool head);

        /** 附加网络插件
        @version NIIEngine 4.0.0
        */
        void attach(Plugin * plugin);

        /** 分离网络插件
        @version NIIEngine 4.0.0
        */
        void detach(Plugin * plugin);
    protected:
        /** 更新/交换数据，由主线程调用
        @version NIIEngine 4.0.0
        */
        void update();
        
        /** 当信息
        @version NIIEngine 4.0.0
        */
        void onMessage(SocketMessage * msg);

        /**
        @version NIIEngine 4.0.0
        */
        SocketMessage * createMessage(NCount size);

        /**
        @version NIIEngine 4.0.0
        */
        void onClientConnect(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onServerConnect(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onConnectFail(Address * address);

        /**
        @version NIIEngine 4.0.0
        */
        void onConnectClose(Address * address);
    protected:
        typedef list<SocketMessage *>::type MessageList;
        typedef list<Address *>::type AddressList;
    protected:
        SocketIOList mRemoteList;
        Plugins mPluginList;
        ThreadMutex mClientSocketMutex;
        ThreadMutex mClientConnectMutex;
        ThreadMutex mServerConnectMutex;
        ThreadMutex mConnectFailMutex;        
        ThreadMutex mConnectCloseMutex;
        ThreadMutex mInMutex;
        Thread * mUpdateThread;
        Thread * mConnectThread;
        AddressList mClientList;
        AddressList mServerList;
        AddressList mConnectFailList;
        AddressList mConnectClosetList;
        AddressList mPrcClientList;
        AddressList mPrcServerList;
        AddressList mPrcClientCloseList;
        AddressList mPrcClientFailList;

        NIIi mThreadPrcLevel;
        STNui32 mRunThreadCount;
        STbool mRun;

        ThreadMutex mSSLConnectMutex;
        ThreadMutex mSSLAddressMutex;
        AddressList mSSLAddressList;
        AddressList mSSLConnectList;
    };

    /** 集群
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ClusterNetworkObj : public NetworkObj
    {
    public:
        ClusterNetworkObj();
        virtual ~ClusterNetworkObj();

        /// @copydetails ClusterNetworkObj::stop
        void stop();

        /// @copydetails ClusterNetworkObj::send
        void send(const Nui8 * data, NCount size, const Address & address, bool broadcast);

        /// @copydetails ClusterNetworkObj::receive
        SocketMessage * receive();

        /// @copydetails ClusterNetworkObj::close
        void close(Address * address);

        /// @copydetails ClusterNetworkObj::onClientConnect
        void onClientConnect(Address * address);

        /// @copydetails ClusterNetworkObj::onServerConnect
        void onServerConnect(Address * address);
        
        /// @copydetails ClusterNetworkObj::onConnectFail()
        void onConnectFail(Address * address);

        /// @copydetails ClusterNetworkObj::onConnectClose
        void onConnectClose(Address * address);
    protected:
        typedef vector<SocketIO *>::type ConnectList; 
        ConnectList mConnectList;
    };
}
}
#endif