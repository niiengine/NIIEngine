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

#ifndef _NII_SSLSOCKETPRC_H_
#define _NII_SSLSOCKETPRC_H_

#include "NiiNetPreInclude.h"
#include "NiiSocketPrc.h"

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace NII
{
    /** ssl socket处理
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SSLSocketPrc : public SocketPrc
    {
        friend class SocketManager;
    public:
        SSLSocketPrc(SocketIO * io);
        virtual ~SSLSocketPrc();

        /** 初始化ssl上下文
        @version NIIEngine 4.0.0
        */
        SSL_CTX * initSSLCTX(const String & certfile, const String & keyfile, const String & keypw);

        /** 构建ssl
        @version NIIEngine 4.0.0
        */
        SSL * setupSSL(SSL_CTX * ctx);

        /** 关闭ssl
        @version NIIEngine 4.0.0
        */
        void shutdownSSL();

        /** 获取ssl上下文
        @version NIIEngine 4.0.0
        */
        inline SSL_CTX * getSSLCTX() const { return mSSLctx; }

        /** 获取ssl
        @version NIIEngine 4.0.0
        */
        inline SSL * getSSL() const { return mSSL; }

        /** 关闭
        @version NIIEngine 4.0.0
        */
        NIIi close();
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
        SSLSocketPrc();

        /** 设置ssl上下文
        @version NIIEngine 4.0.0
        */
        void setSSLCTX(SSL_CTX * ctx);

        /** 发送
        @version NIIEngine 4.0.0
        */
        NIIi send(void * data, NCount size);

        /** 接收
        @version NIIEngine 4.0.0
        */
        NIIi receive(void * data, NCount size);

        /// 连接ssl
        Ni32 connectSSL();

        /// 输出ssl错误
        void getSSLError(NIIi res);
    protected:
        SSL * mSSL;
        SSL_CTX * mSSLctx;
        String mCertFile;
        String mKeyFile;
        String mKeyPW;
        bool mSSLConnect;
    };
}
#endif
