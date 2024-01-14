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

#ifndef _NII_SOCKETManager_H_
#define _NII_SOCKETManager_H_

#include "NiiNetPreInclude.h"
#include "NiiSingleton.h"
#include "NiiNetAddress.h"

namespace NII
{
    /** 管理器
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SocketManager : public Singleton<SocketManager>, public NetworkAlloc
    {
    public:
        /** 操作类型
        @version NIIEngine 4.0.0
        */
        enum OptionType
        {
            OT_SET_SendSize	        = 1,
            OT_SET_ReceiveSize	    = 2,
            OT_SET_Block            = 3,
            OT_SET_NoDelay          = 4,
            OT_SET_ReuseAddress     = 5,
            OT_GET_RemoteIP		    = 6,
            OT_GET_RemotePort		= 7,
            OT_GET_LocalIP			= 8,
            OT_GET_LocalPort		= 9
        }
        
        /** 事件类型
        @version NIIEngine 4.0.0
        */
        enum SocketEventType
        {
            SET_Read		        = 0x1,
            SET_Write	            = 0x2,
            SET_Exception	        = 0x4,
            SET_All		            = 0x7
        };
    public:
        ~SocketManager();

        /** 初始化网络结构
        @version NIIEngine 4.0.0
        */
        int init();

        /** 删除网络结构
        @version NIIEngine 4.0.0
        */
        int destroy();
    
        /** 构建
        @version NIIEngine 4.0.0
        */
        void setup(TimeDurMS timeout = 60);
        
        /** 关闭
        @version NIIEngine 4.0.0
        */
        void shutdown();
        
        /** 创建对象
        @version NIIEngine 4.0.0
        */
        SocketObj * createSocket();
        
        /** 是否运行中
        @version NIIEngine 4.0.0
        */
        bool isRun() const { return mRunning; }
        
        /** 是否运行中
        @version NIIEngine 4.0.0
        */
        void addPrc(SocketPrc * prc);
        
        /** 是否运行中
        @version NIIEngine 4.0.0
        */
        void removePrc(SocketPrc * prc);
        
        /** 是否运行中
        @version NIIEngine 4.0.0
        */
        SocketPrc * getPrc(Nid fd) const;
        
        /** 添加socket消息
        @version NIIEngine 4.0.0
        */
        void addEvent(Nsocket fd, Nmark mark);
        
        /** 移去socket消息
        @version NIIEngine 4.0.0
        */
        void removeEvent(Nsocket fd, Nmark mark);

        /** 添加定时器
        @param[in] sync 是否主线程执行调用
        @version NIIEngine 4.0.0
        */
        void addTimer(ThreadMain * func, void * data, Nui64 interval, bool sync = true);
        
        /** 添加定时器
        @version NIIEngine 4.0.0
        */
        void removeTimer(ThreadMain * func, void * data);
        
        /** 监听
        @return 返回 -1 则代表失败
        @version NIIEngine 4.0.0
        */
        static Nid listen(const String & ip, Nui16 port, ServerSocketIO * io);

        /** 连接
        @return 返回 -1 则代表失败
        @version NIIEngine 4.0.0
        */
        static Nid connect(const String & ip, Nui16 port, ClientSocketIO * io);

        /** 发送
        @version NIIEngine 4.0.0
        */
        static NIIi send(Nid handle, void * buf, int size);

        /** 接收
        @version NIIEngine 4.0.0
        */
        static NIIi receive(Nid handle, void * buf, int size);

        /** 关闭连接
        @version NIIEngine 4.0.0
        */
        static NIIi close(Nid handle);
        
        /** 设置选项
        @version NIIEngine 4.0.0
        */
        static NIIi setOption(Nid handle, OptionType type, void * value);

        /** 获取本地主机
        @version NIIEngien 4.0.0
        */
        void getLocalHost(AddressList & out);
        
        /** 解析主机
        @version NIIEngien 4.0.0
        */
        VString resolveDomain(const VString & name);
    protected:
        SocketManager();
    protected:
        typedef HashMap<Nid, SocketPrc *> PrcList;
        typedef list<TimerItem *>::type TimerList;
        
        PrcList mPrcList;
        TimerList mTimerList;
#ifdef _WIN32
        fd_set mReadSet;
        fd_set mWriteSet;
        fd_set mExceptionSet;
#elif __APPLE__
        int mKqueue;
#else
        int mEpoll;
#endif
        ThreadMutex mMutex;
        STbool mRunning;
    };
}
#endif
