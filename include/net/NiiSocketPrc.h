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
    class SocketPrc
    {
        friend class SocketManager;
    public:
        enum State
        {
            S_Unknow,
            S_Listening,
            S_Connecting,
            S_Connected,
            S_Closing,
            S_Closed
        };
    public:
        SocketPrc();
        virtual ~SocketPrc();

        /** 设置socket
        @version NIIEngine 4.0.0
        */
        void setSocket(SOCKET s) { mSocket = s; }
        
        /** 获取socket
        @version NIIEngine 4.0.0
        */
        SOCKET getSocket() const { return mSocket; }

        /** 设置状态
        @version NIIEngine 4.0.0
        */
        void setState(State state) { mState = state; }

        /** 发送
        @version NIIEngine 4.0.0
        */
        int send(void * buf, int len);

        /** 接收
        @version NIIEngine 4.0.0
        */
        int receive(void * buf, int len);

        /** 关闭
        @version NIIEngine 4.0.0
        */
        int close();

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
        const VString & getRemoteIP() const { return mRemoteIP; }

        /** 获取本地接收的ip
        @version NIIEngine 4.0.0
        */
        const VString & GetLocalIP() const { return mIP; }

        /** 获取连向的远端port
        @version NIIEngine 4.0.0
        */
        Nui16 getRemotePort() const { return mRemotePort; }

        /** 获取本地接收的port
        @version NIIEngine 4.0.0
        */
        Nui16 GetLocalPort() const { return mPort; }
    public:
        /** 当读取时
        @version NIIEngine 4.0.0
        */
        void onRead();

        /** 当写入时
        @version NIIEngine 4.0.0
        */
        void onWrite();

        /** 当关闭时
        @version NIIEngine 4.0.0
        */
        void onClose();
    protected:
        /** 创建监听(被动连接)socket
        @version NIIEngine 4.0.0
        */
        int listen(const String & ip, Nui16 port);

        /** 创建主动连接socket
        @version NIIEngine 4.0.0
        */
        Nid connect(const String & ip, Nui16 port);

        /** 设置地址
        @version NIIEngine 4.0.0
        */
        void setAddress(const String & ip, Nui16 port, sockaddr_in * addr);

        /** 当前操作失败是否属于阻塞问题
        @version NIIEngine 4.0.0
        */
        bool isBlock(int error_code);

        /** 设置非阻塞连接
        @version NIIEngine 4.0.0
        */
        void setNonBlock(SOCKET fd);
        
        /** 设置重新连接地址
        @version NIIEngine 4.0.0
        */
        void setReuseAddress(SOCKET fd);
        
        /** 设置非延迟发送
        @version NIIEngine 4.0.0
        */
        void setNoDelay(SOCKET fd);
    protected:
        SOCKET mSocket;
        VString mRemoteIP;
        VString mIP;        
        Nui16 mRemotePort;
        Nui16 mPort;
        State mState;
    };
}
#endif
