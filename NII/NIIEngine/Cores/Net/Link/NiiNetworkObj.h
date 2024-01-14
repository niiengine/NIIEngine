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
        PluginList mPluginList;
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
        AddressList mPrcClientList;
        AddressList mPrcServerList;
        AddressList mPrcClientCloseList;
        AddressList mPrcClientFailList;
        
        NIIi mThreadPrcLevel;
        STNui32 mRunThreadCount;
        STbool mRun;

        ThreadRWMutex mSSLConnectMutex;
        ThreadRWMutex mSSLAddressMutex;
        SSLAddressList mSSLAddressList;
        AddressList mSSLConnectList;
    };
}
}
#endif