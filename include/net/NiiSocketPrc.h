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

#ifndef _NII_SOCKETPRC_H__
#define _NII_SOCKETPRC_H__

#include "NiiNetPreInclude.h"

namespace NII
{
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

        /**
        @version NIIEngine 4.0.0
        */
        SOCKET getSocket() const { return mSocket; }
        
        /**
        @version NIIEngine 4.0.0
        */
        void setSocket(SOCKET s) { mSocket = s; }
        
        /**
        @version NIIEngine 4.0.0
        */
        void setState(State state) { mState = state; }
        
        /**
        @version NIIEngine 4.0.0
        */
        int send(void * buf, int len);

        /**
        @version NIIEngine 4.0.0
        */
        int receive(void * buf, int len);

        /**
        @version NIIEngine 4.0.0
        */
        int close();
        
        /**
        @version NIIEngine 4.0.0
        */
        void setRemoteIP(const VString & ip) { mRemoteIP = ip; }
        
        /**
        @version NIIEngine 4.0.0
        */
        void setRemotePort(Nui16 port) { mRemotePort = port; }
        
        /**
        @version NIIEngine 4.0.0
        */
        void setSendSize(Nui32 size);
        
        /**
        @version NIIEngine 4.0.0
        */
        Nui32 getSendSize() const;
        
        /**
        @version NIIEngine 4.0.0
        */
        void setReceiveSize(Nui32 size);
        
        /**
        @version NIIEngine 4.0.0
        */
        Nui32 getReceiveSize() const;

        /**
        @version NIIEngine 4.0.0
        */
        const VString & getRemoteIP() const { return mRemoteIP; }
        
        /**
        @version NIIEngine 4.0.0
        */
        const VString & GetLocalIP() const { return mIP; }
        
        /**
        @version NIIEngine 4.0.0
        */
        Nui16 getRemotePort() const { return mRemotePort; }
        
        /**
        @version NIIEngine 4.0.0
        */
        Nui16 GetLocalPort() const { return mPort; }
    public:
        /**
        @version NIIEngine 4.0.0
        */
        void onRead();
        
        /**
        @version NIIEngine 4.0.0
        */
        void onWrite();
        
        /**
        @version NIIEngine 4.0.0
        */
        void onClose();
    protected:
        /**
        @version NIIEngine 4.0.0
        */
        int listen(const String & ip, Nui16 port);

        /**
        @version NIIEngine 4.0.0
        */
        Nid connect(const String & ip, Nui16 port);
    
        /**
        @version NIIEngine 4.0.0
        */
        void setAddress(const String & ip, Nui16 port, sockaddr_in * addr);
        
        /**
        @version NIIEngine 4.0.0
        */
        bool isBlock(int error_code);

        /**
        @version NIIEngine 4.0.0
        */
        void setNonBlock(SOCKET fd);
        
        /**
        @version NIIEngine 4.0.0
        */
        void setReuseAddress(SOCKET fd);
        
        /**
        @version NIIEngine 4.0.0
        */
        void setNoDelay(SOCKET fd);
    private:
        SOCKET mSocket;
        VString mRemoteIP;
        VString mIP;        
        Nui16 mRemotePort;
        Nui16 mPort;
        State mState;
    };
}
#endif
