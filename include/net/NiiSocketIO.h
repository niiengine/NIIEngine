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
    /** SocketIO
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SocketIO : public NetworkAlloc
    {
        friend class SocketManager;
        friend class SocketPrc;
        friend class NetworkObj;
    public:
        SocketIO();
        virtual ~SocketIO();

        /** 中断
        @version NIIEngine 4.0.0
        */
        inline void abort()
        {
            mStop = true;
            mAbort = true;
        }

        /** 是否中断
        @version NIIEngine 4.0.0
        */
        inline bool isAbort() const
        {
            return mStop;
        }

		/** 停止
		@version NIIEngine 4.0.0
		*/
		inline void stop()
		{
			mStop = true;
		}

		/** 是否停止
		@version NIIEngine 4.0.0
		*/
		inline bool isStop() const
		{
			return mStop;
		}
        
        /** 获取端口处理对象
        @version NIIEngine 4.0.0
        */
        inline SocketPrc * getPrc() const { return mSocketPrc; }
        
        /** 发送数据
        @version NIIEngine 4.0.0
        */
        NIIi send(void * data, NCount size);

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        inline NIIi send(Message * msg) { return send(msg->getBuffer(), msg->getSize()); }

		/** 设置定时器
        @param[in] sync 是否主线程执行调用
		@version NIIEngine 4.0.0
		*/
		void setTimer(bool set, TimeDurMS delay, TimeDurMS interval, bool sync = true);
        
        /** 设置信息处理方式
        @remark 关于 onMessage 处理方式，默认主线程处理
        @param[in] interval 异步处理间隔，当参数sync == false时有效。
        @param[in] sync 是否主线程执行调用
		@version NIIEngine 4.0.0
        */
        void setMessageTimer(TimeDurMS interval, bool sync = true);
        
        /** 数据读取时触发
        @version NIIEngine 4.0.0
        */
        virtual void onRead();

        /** 数据写入时触发
        @version NIIEngine 4.0.0
        */
        virtual void onWrite();

        /** 关闭连接时触发
        @version NIIEngine 4.0.0
        */
        virtual void onClose(){}
        
		/** 接收错误时触发
		@version 0.9.1
		*/
		virtual void onReceiveError(){}
        
        /** 发生错误时触发
        @version NIIEngine 4.0.0
        */
        virtual void onException{}

        /** 定时任务触发
        @version NIIEngine 4.0.0
        */
        virtual void onTimer(TimeDurMS cost) {}

        /** 收到消息时触发
        @version NIIEngine 4.0.0
        */
        virtual void onMessage(Message * msg) {}

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        inline NIIi send(Nui16 sid, Nui16 cid, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            
            return send(msg.getBuffer(), msg.getSize());
        }

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        inline NIIi send(Nui16 sid, Nui16 cid, Nui16 seq, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            msg.setSeqIdx(seq);
            
            return send(msg.getBuffer(), msg.getSize());
        }

        /** 发送数据
        @version NIIEngine 4.0.0
        */
        inline NIIi send(Nui16 sid, Nui16 cid, Nui16 seq, const google::protobuf::MessageLite * proto)
        {
            MdfMessage msg;
            msg.setProto(proto);
            msg.setMessageID(sid);
            msg.setCommandID(cid);
            msg.setSeqIdx(seq);
            
            return send(msg.getBuffer(), msg.getSize());
        }
        
        /** 创建消息
        @version NIIEngine 4.0.0
        */
        virtual SocketMessage * create(Nui8 * buf, Nui32 size) const;
        
        /** 创建实例
        @version NIIEngine 4.0.0
        */
        virtual SocketIO * createInstance() const = 0;
    protected:
        /** 设置端口处理对象
        @version NIIEngine 4.0.0
        */
        inline void setPrc(SocketPrc * prc){ mSocketPrc = prc; }
    protected:
        typedef list<RingBuffer *>::type BufferList;
        typedef list<MdfMessage *>::type MessageList;
        
        SocketPrc * mSocketPrc;
        ThreadMain * mTimer;
        ThreadMutex mInMutex;
        ThreadMutex mOutMutex;
        MessageList mInList;
        BufferList mOutList;
        RingBuffer mInBuffer;       
        TimeDurMS mLastSend;
        TimeDurMS mLastReceive;
        STbool mStop;
        STbool mAbort;
        STbool mBusy;
    };
    typedef vector<SocketIO *>::type SocketIOList; 
    
    /** ServerSocketIO
    @version NIIEngine 4.0.0
    */
    class _EngineAPI ServerSocketIO : public SocketIO
    {
        friend class SocketManager;
    public:
        ServerSocketIO();
        virtual ~ServerSocketIO();

        /** 连接时触发
        @version NIIEngine 4.0.0
        */
        virtual void onConnect() {}
        
        /// SocketIO::createInstance
        virtual SocketIO * createInstance() const;
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
        
        /** 确认连接时触发
        @version NIIEngine 4.0.0
        */
        virtual void onConfirm() {}
        
        /// SocketIO::createInstance
        virtual SocketIO * createInstance() const;
    };
}
#endif
