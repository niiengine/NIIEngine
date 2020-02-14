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

#ifndef _NII_SOCKETIO_H_
#define _NII_SOCKETIO_H_

#include "NiiNetPreInclude.h"
#include "MdfMessage.h"

namespace google 
{ 
namespace protobuf 
{
    class MessageLite;
}
}

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SocketIO
    {
        friend class SocketManager;
    public:
        SocketIO();
        virtual ~SocketIO();

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        int send(void * data, NCount size);

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        inline int send(MdfMessage * msg) { return send(msg->getBuffer(), msg->getSize()); }

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void onRead();

        /**
        @version NIIEngine 4.0.0
        */
        virtual void onWrite();

        /**
        @version NIIEngine 4.0.0
        */
        virtual void onClose() {}

        /**
        @version NIIEngine 4.0.0
        */
        virtual void onTimer(TimeDurMS tick) {}

        /**
        @version NIIEngine 4.0.0
        */
        virtual void onMessage(MdfMessage * msg) {}

        /**
        @version NIIEngine 4.0.0
        */
        inline int send(Nui16 sid, Nui16 cid, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            
            return send(msg.getBuffer(), msg.getSize());
        }

        /**
        @version NIIEngine 4.0.0
        */
        inline int send(Nui16 sid, Nui16 cid, Nui16 seq, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            msg.setSeqIdx(seq);
            
            return send(msg.getBuffer(), msg.getSize());
        }

        /**
        @version NIIEngine 4.0.0
        */
        inline int send(Nui16 sid, Nui16 cid, Nui16 seq, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            msg.setSeqIdx(seq);
            
            return send(msg.getBuffer(), msg.getSize());
        }
    protected:
        RingBuffer mInBuffer;
        RingBuffer mOutBuffer;
        VString mIP;
        Nui16 mPort;        
        TimeDurMS mLastSend;
        TimeDurMS mLastReceive;
        bool mBusy;
    };
    
    
    /** ServerSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ServerSocketIO : public SocketIO
    {
        friend class SocketManager;
    public:
        ServerSocketIO();
        virtual ~ServerSocketIO();

        /** 确认连接时触发
        @version NIIEngine 4.0.0
        */
        virtual void onConfirm() {}
    };
    
    /** ClientSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ClientSocketIO : public SocketIO
    {
        friend class SocketManager;
    public:
        ClientSocketIO();
        virtual ~ClientSocketIO();

        /** 连接时触发
        @version NIIEngine 4.0.0
        */
        virtual void onConnect() {}
    };
}
#endif
