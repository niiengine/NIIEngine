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
    class _EngineAPI NetworkObj : public ThreadMain
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
        bool start(Nui16 maxLink, NIIi prclevel = N_ThreadPriorityHigh);

        /** 停止运作
        @version NIIEngine 4.0.0
        */
        void stop();

        /** 创建服务端(监听)
        @version NIIEngien 4.0.0
        */
        Nid listen(const String & ip, Nui16 port, Nui16 sfamily = AF_INET, Nui16 maxConnect = 128);

        /** 创建客服端
        @version NIIEngine 4.0.0
        */
        Address * connect(const String & host, Nui16 port, Nui16 sfamily = AF_INET);

        /** 关闭连接
        @version NIIEngine 4.0.0
        */
        void close(const Address & address);
        
        /** 启用SSL加密
        @version NIIEngine 4.0.0
        */
        void setSSL(const Address & address, const String & certfile = "", const String & keyfile = "", const String & keypw = "");

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
        /// @copydetails ThreadMain::run
        virtual void run(void * arg);

        /** 创建消息包
        @version NIIEngine 4.0.0
        */
        virtual SocketMessage * createMessage(NCount size);

        /** 当接收到信息时触发
        @version NIIEngine 4.0.0
        */
        virtual void onMessage(SocketMessage * msg) = 0;
        
        /** 当以客服端连接时
        @version NIIEngine 4.0.0
        */
        virtual void onClientConnect(Address * address);

        /** 当以服务端连接时
        @version NIIEngine 4.0.0
        */
        virtual void onServerConnect(Address * address);

        /** 连接失败时
        @version NIIEngine 4.0.0
        */
        virtual void onConnectFail(Address * address);

        /** 连接关闭时时
        @version NIIEngine 4.0.0
        */
        virtual void onConnectClose(Address * address);
    protected:
        typedef list<SocketMessage *>::type MessageList;
        typedef list<Address *>::type AddressList;
        
        typedef struct _SSLData
        {
            Address * mAddres;
            String mCertFile;
            String mKeyFile;
            String mKeyPW;
        }SSLData;

        typedef list<SSLData>::type SSLAddressList;
    protected:
        SocketPrcList mListenList;
        SocketIOList mRemoteList;
        MessageList mInList;
        Plugins mPluginList;
        ThreadMutex mRemoteListMutex;
        ThreadMutex mClientConnectMutex;
        ThreadMutex mServerConnectMutex;
        ThreadMutex mConnectFailMutex;        
        ThreadMutex mConnectCloseMutex;
        ThreadMutex mInMutex;
        Thread * mUpdateThread;
        AddressList mClientList;
        AddressList mServerList;
        AddressList mConnectFailList;
        AddressList mConnectClosetList;

        NIIi mThreadPrcLevel;
        STNui32 mRunThreadCount;
        STbool mRun;

        ThreadRWMutex mSSLConnectMutex;
        ThreadRWMutex mSSLAddressMutex;
        SSLAddressList mSSLAddressList;
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

        /// @copydetails ClusterNetworkObj::close
        void close(Address * address);
    protected:
        /// @copydetails ThreadMain::run
        void run(void * arg);
        
        /// @copydetails NetworkObj::onClientConnect
        void onClientConnect(Address * address);

        /// @copydetails NetworkObj::onServerConnect
        void onServerConnect(Address * address);
        
        /// @copydetails NetworkObj::onConnectFail()
        void onConnectFail(Address * address);

        /// @copydetails NetworkObj::onConnectClose
        void onConnectClose(Address * address);
    protected:
        AddressList mPrcClientList;
        AddressList mPrcServerList;
        AddressList mPrcClientCloseList;
        AddressList mPrcClientFailList;
    };
}
}
#endif