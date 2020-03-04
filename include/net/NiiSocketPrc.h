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

#ifndef _NII_SOCKETPRC_H_
#define _NII_SOCKETPRC_H_

#include "NiiNetPreInclude.h"

namespace NII
{
    /** socket处理
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SocketPrc : public NetworkAlloc
    {
        friend class SocketManager;
    public:
        enum State
        {
            S_Unknow,           ///< 未初始化
            S_Listening,        ///< 监听
            S_Connecting,       ///< 连接中
            S_Connected,        ///< 已连接
            S_Closing,          ///< 关闭中
            S_Closed            ///< 已关闭
        };
    public:
        SocketPrc(SocketIO * io);
        virtual ~SocketPrc();

        /** 设置socket
        @version NIIEngine 4.0.0 adv api
        */
        inline SocketIO * getStream() const { return mIO; }
        
        /** 设置socket
        @version NIIEngine 4.0.0 adv api
        */
        inline void setSocket(Nsocket s) { mSocket = s; }
        
        /** 获取socket
        @version NIIEngine 4.0.0
        */
        inline Nsocket getSocket() const { return mSocket; }

        /** 设置状态
        @version NIIEngine 4.0.0
        */
        inline void setState(State state) { mState = state; }

        /** 关闭
        @version NIIEngine 4.0.0
        */
        virtual NIIi close();

        /** 设置发送缓存大小
        @version NIIEngine 4.0.0
        */
        void setSendSize(Nui32 size);

        /** 获取发送缓存大小
        @version NIIEngine 4.0.0
        */
        Nui32 getSendSize() const;

        /** 设置接收大小
        @version NIIEngine 4.0.0
        */
        void setReceiveSize(Nui32 size);

        /** 获取接收大小
        @version NIIEngine 4.0.0
        */
        Nui32 getReceiveSize() const;

        /** 获取连向的远端ip
        @version NIIEngine 4.0.0
        */
        inline const VString & getRemoteIP() const { return mRemoteIP; }

        /** 获取本地接收的ip
        @version NIIEngine 4.0.0
        */
        inline const VString & GetLocalIP() const { return mIP; }

        /** 获取连向的远端port
        @version NIIEngine 4.0.0
        */
        inline Nui16 getRemotePort() const { return mRemotePort; }

        /** 获取本地接收的port
        @version NIIEngine 4.0.0
        */
        inline Nui16 GetLocalPort() const { return mPort; }
    public:
        /** 当读取时
        @version NIIEngine 4.0.0
        */
        virtual void onRead();

        /** 当写入时
        @version NIIEngine 4.0.0
        */
        virtual void onWrite();

        /** 当关闭时
        @version NIIEngine 4.0.0
        */
        virtual void onClose();
    protected:
        SocketPrc();
        
        /** 创建监听(被动连接)socket
        @version NIIEngine 4.0.0
        */
        virtual Nid listen(const String & ip, Nui16 port, NCount maxlink);

        /** 创建主动连接socket
        @version NIIEngine 4.0.0
        */
        virtual Nid connect(const String & ip, Nui16 port);
        
        /** 发送
        @version NIIEngine 4.0.0
        */
        virtual NIIi send(void * data, NCount len);

        /** 接收
        @version NIIEngine 4.0.0
        */
        virtual NIIi receive(void * data, NCount len);

        /** 设置地址
        @version NIIEngine 4.0.0
        */
        void setAddress(const String & ip, Nui16 port, sockaddr_in * addr);

        /** 当前操作失败是否属于阻塞问题
        @version NIIEngine 4.0.0
        */
        bool isBlock(int ecode);

        /** 设置非阻塞连接
        @version NIIEngine 4.0.0
        */
        void setNonBlock(Nsocket fd);
        
        /** 设置重新连接地址
        @version NIIEngine 4.0.0
        */
        void setReuseAddress(Nsocket fd);
        
        /** 设置非延迟发送
        @version NIIEngine 4.0.0
        */
        void setNoDelay(Nsocket fd);
        
        /** 设置广播模式
        @version NIIEngine 4.0.0
        */
        void setBroadcast(Nsocket fd);
        
        /** 设置延时关闭
        @version NIIEngine 4.0.0
        */
        void setLinger(Nsocket fd, bool set, TimeDurMS time);
        
        /** 设置接收超时
        @version NIIEngine 4.0.0
        */
        void setSendTimeOut(Nsocket fd, TimeDurMS time);

        /** 设置发送超时
        @version NIIEngine 4.0.0
        */
        void setRecvTimeOut(Nsocket fd, TimeDurMS time);
        
        /** 设置优先级
        @version NIIEngine 4.0.0
        */
        void setTOS(Nsocket fd, Ni32 tos);
        
        /** 设置生命周期
        @version NIIEngine 4.0.0
        */
        void setTTL(Nsocket fd, Ni32 ttl);
    protected:
        SocketIO * mIO;
        Address * mAddress;
        Nsocket mSocket;
        VString mRemoteIP;
        VString mIP; 
        Nui16 mRemotePort;
        Nui16 mPort;
        State mState;
    };
}
#endif
