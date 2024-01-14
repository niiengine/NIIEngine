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
