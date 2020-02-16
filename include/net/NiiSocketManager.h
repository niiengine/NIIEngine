/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef _NII_SOCKETManager_H_
#define _NII_SOCKETManager_H_

#include "NiiNetPreInclude.h"

namespace NII
{
    /** 管理器
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SocketManager : public Singleton<SocketManager> , public NetworkAlloc
    {
    public:
        /**
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
        
        /** 
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
        @version NIIEngine 4.0.0
        */
        void addTimer(ThreadMain * func, void * data, Nui64 interval);
        
        /** 添加定时器
        @version NIIEngine 4.0.0
        */
        void removeTimer(ThreadMain * func, void * data);
        
        /**
        @version NIIEngine 4.0.0
        */
        static Ni32 listen(const String & ip, Nui16 port);

        /**
        @version NIIEngine 4.0.0
        */
        static Nid connect(const String & ip, Nui16 port);

        /**
        @version NIIEngine 4.0.0
        */
        static int send(Nid handle, void * buf, int size);

        /**
        @version NIIEngine 4.0.0
        */
        static int receive(Nid handle, void * buf, int size);

        /**
        @version NIIEngine 4.0.0
        */
        static int close(Nid handle);
        
        /**
        @version NIIEngine 4.0.0
        */
        static int setOption(Nid handle, OptionType type, void * value);

        /// @copydetails Singleton::getOnly
        static SocketManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static SocketManager * getOnlyPtr();
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
        bool mRunning;
    };
}
#endif
