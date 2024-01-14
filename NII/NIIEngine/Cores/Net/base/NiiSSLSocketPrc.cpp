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

#include "NiiPreProcess.h"
#include "NiiSSLSocketPrc.h"
#include "NiiSocketManager.h"

#ifdef _WIN32
    #define _GetErrorCode return WSAGetLastError();
#else
    #define _GetErrorCode return errno;
#endif

namespace NII
{
    //------------------------------------------------------------------------
    SSLSocketPrc::SSLSocketPrc(SocketIO * io) :
        SocketPrc(io),
        mSSL(0),
        mSSLctx(0),
        mSSLConnect(false)
    {
    }
    //------------------------------------------------------------------------
    SSLSocketPrc::SSLSocketPrc():
        mSSL(0),
        mSSLctx(0),
        mSSLConnect(false)
    {
    }
    //------------------------------------------------------------------------
    SSLSocketPrc::~SSLSocketPrc()
    {
        if (mSSL)
        {
            Ni32 re = SSL_shutdown(mSSL);
            if (re <= 0)
            {
                Ni32 nErrorCode = SSL_get_error(mSSL, re);
                N_warning("ssl shutdown not finished, errno: %d.", nErrorCode);
            }
            else if (re == 1)
            {
                N_log("ssl shutdown successed.");
            }
            SSL_free(mSSL);
            mSSL = 0;
        }

        if(mState == S_Listening)
        {
            if(mSSLctx)
            {
                SSL_CTX_free(mSSLctx);
                mSSLctx = 0;
            }
        }
        mSSLConnect = false;
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::setSSLCTX(SSL_CTX * ctx) 
    { 
        if(mSSLctx)
        {
            SSL_CTX_free(mSSLctx);
            mSSLctx = ctx; 
        }
    }
    //------------------------------------------------------------------------
    NIIi SSLSocketPrc::send(void * data, NCount size)
    {
        if(mState != S_Connected)
            return -1;

        int re = SSL_write(mSSL, data, size);

        if (re < 0)
        {
            Ni32 ecode = SSL_get_error(mSSL, re);
            if (SSL_ERROR_WANT_WRITE == ecode || SSL_ERROR_WANT_READ == ecode)
            {
    #if ((defined _WIN32) || (defined __APPLE__))
                N_Only(Socket)->addEvent(mSocket, SET_Write);
    #endif
                re = 0;
            }
            else
            {
                N_error("send ssl data error, errno: %d.", ecode);
                onClose();
            }
        }
        else if (re == 0)
        {
            Ni32 ecode = SSL_get_error(mSSL, re);
            if (SSL_ERROR_ZERO_RETURN == ecode)
            {
                N_warning("send ssl data error, peer closed.");
            }
            else
            {
                N_error("send ssl data error, errno: %d.", ecode);
            }
            onClose();
        }

        return re;
    }
    //------------------------------------------------------------------------
    NIIi SSLSocketPrc::receive(void * data, NCount size)
    {
        if (mSSLConnect)
        {
            int recsize =  SSL_read(mSSL, data, size);
            if (recsize == 0)
            {
                Ni32 eCode = SSL_get_error(mSSL, recsize);
                if (SSL_ERROR_ZERO_RETURN == eCode)
                    N_warning("recv ssl data error, peer closed.");
                else
                    N_error("recv ssl data error.");
                close();
            }
            else if (recsize < 0)
            {
                Ni32 eCode = SSL_get_error(mSSL, recsize);
                if (SSL_ERROR_WANT_READ == eCode || SSL_ERROR_WANT_WRITE == eCode)
                {
                    break;
                }
                else
                {
                    N_error("recv ssl data error, errno: %d.", eCode);
                    close();
                }
            }
        }
        else
        {
            if(connectSSL() == -1)
                close();
        }
    }
    //------------------------------------------------------------------------
    NIIi SSLSocketPrc::close()
    {
        if (mSSL)
        {
            Ni32 re = SSL_shutdown(mSSL);
            if (re <= 0)
            {
                Ni32 nErrorCode = SSL_get_error(mSSL, re);
                N_warning("ssl shutdown not finished, errno: %d.", nErrorCode);
            }
            else if (re == 1)
            {
                N_log("ssl shutdown successed.");
            }
            SSL_free(mSSL);
            mSSL = 0;
        }

        mSSLConnect = false;

        N_Only(Socket)->removeEvent(mSocket, SET_All);
        N_Only(Socket)->removePrc(this);

        N_net_close(mSocket);

        return 0;
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::onRead()
    {
        if (mState == S_Listening)
        {
            Nsocket fd = 0;
            
        #if N_NET_PROTOCOL_VER == 6
            struct sockaddr_storage peer_addr;
            socklen_t addr_len = sizeof(sockaddr_storage);
        #else
            struct sockaddr_in peer_addr;
            socklen_t addr_len = sizeof(sockaddr_in);
        #endif

            char ip_str[64];
            while((fd = accept(mSocket, (sockaddr *)&peer_addr, &addr_len)) != N_InvalidSocket)
            {
                ServerSocketIO * io = mIO->createInstance();
                SSLSocketPrc * prc = N_new SSLSocketPrc(io);
                io->setPrc(prc);

                Nui32 ip = ntohl(peer_addr.sin_addr.s_addr);
                snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

                prc->setSocket(fd);
                prc->setState(S_Connected);
                prc->mRemoteIP = ip_str;
                prc->mRemotePort = ntohs(peer_addr.sin_port);
                N_log("AcceptNewSocket, socket=%d from %s:%d\n", fd, ip_str, prc->mRemotePort);

        #if N_NET_PROTOCOL_VER == 6
                if (sockAddr.ss_family == AF_INET)
                {
                    IVP6Address * tempaddr = N_new IVP6Address();
                    prc->mAddress = tempaddr;
                    memcpy(&tempaddr->mSrc,(sockaddr_in *)&sockAddr,sizeof(sockaddr_in));
                }
                else
                {
                    IVP4Address * tempaddr = N_new IVP4Address();
                    prc->mAddress = tempaddr;
                    memcpy(&tempaddr->mSrc,(sockaddr_in6 *)&sockAddr,sizeof(sockaddr_in6));
                }
        #else
                IVP4Address * addr = N_new IVP4Address();
                prc->mAddress = addr;
                addr->mSrc.sin_addr.s_addr = sockAddr.sin_addr.s_addr;
                prc->mAddress->setNSBPort(sockAddr.sin_port);
                prc->mAddress->mIndex = reidx;
        #endif
        
                prc->setNoDelay(fd);
                prc->setNonBlock(fd);
                prc->setSSLCTX(mSSLctx);
                SSL * tssl = prc->setupSSL(mSSLctx);
                if (tssl == 0)
                {
                    N_error("init ssl, create SSL object failed.");
                    prc->onClose();
                    return;
                }

                SSL_set_mode(tssl, SSL_MODE_AUTO_RETRY);

                if(SSL_set_fd(tssl, fd) != 1)
                {
                    N_error("ssl set fd failed");
                    prc->onClose();
                    return;
                }

                if(SSL_accept(tssl) == -1) 
                {
                    N_error("ssl accept failed");
                    prc->onClose();
                    return;
                }
                
                if(prc->connectSSL() == -1)
                {
                    prc->onClose();
                    return;
                }
                io->onConnect();
                N_Only(Socket)->addPrc(prc);
                N_Only(Socket)->addEvent(fd, SET_Read | SET_Exception);
                io->onConnect();
            }
        }
        else
        {
            u_long avail = 0;
            if((N_net_ioctl(mSocket, FIONREAD, &avail) == N_SocketError) || (avail == 0))
            {
                onClose();
            }
            else
            {
                mIO->onRead();
            }
        }
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::onWrite()
    {
    #if ((defined _WIN32) || (defined __APPLE__))
        N_Only(Socket)->removeEvent(mSocket, SET_Write);
    #endif

        if(mState == S_Connecting)
        {
            int error = 0;
            socklen_t len = sizeof(error);
    #ifdef _WIN32
            getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
    #else
            getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (void*)&error, &len);
    #endif
            if(error) 
            {
                onClose();
            } 
            else 
            {
                onConfirm(mIO->mAddress);
                mState = S_Connected;
                SSL_set_mode(mSSL, SSL_MODE_AUTO_RETRY);
                if (SSL_set_fd(mSSL, mIO->getSocket()) != 1)
                {
                    N_error("ssl set fd failed");
                    onClose();
                    return;
                }
                if(connectSSL() == -1)
                {
                    onClose();
                    return;
                }
                static_cast<ClientSocketIO *>(mIO)->onConfirm();
            }
        }
        else
        {
            mIO->onWrite();
        }
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::onClose()
    {
        mState = S_Closing;
        mIO->abort();
        mIO->onClose();
    }
    //------------------------------------------------------------------------
    SSL_CTX * SSLSocketPrc::initSSLCTX(const String & certfile, const String & keyfile, const String & keypw)
    {
        if(mSSLctx)
            return mSSLctx;
        mCertFile = certfile;
        mKeyFile = keyfile;
        mKeyPW = keypw;

        SSLeay_add_ssl_algorithms();
        SSL_load_error_strings();
        
        mSSLctx = SSL_CTX_new(SSLv23_client_method());
        if (mSSLctx)
        {
            if (mCertFile.size())
            {
                if (!SSL_CTX_use_certificate_file(mSSLctx, mCertFile.c_str(), SSL_FILETYPE_PEM))
                {
                    MlogError("init ssl: use certificate file failed.");
                    SSL_CTX_free(mSSLctx);
                    mSSLctx = 0;
                }
            }

            SSL_CTX_set_default_passwd_cb_userdata(mSSLctx, (void*)keypw.c_str());

            if (mKeyFile.size())
            {
                if (!SSL_CTX_use_PrivateKey_file(mSSLctx, mKeyFile.c_str(), SSL_FILETYPE_PEM))
                {
                    MlogError("init ssl: use private key file failed.");
                    SSL_CTX_free(mSSLctx);
                    mSSLctx = 0;
                }
            }

            if (!SSL_CTX_check_private_key(mSSLctx))
            {
                MlogError("init ssl: check private key file failed.");
                SSL_CTX_free(mSSLctx);
                mSSLctx = 0;
            }
        }
        else
        {
            MlogError("init ssl: create new SSL_CTX object failed.");
        }
        return mSSLctx;
    }
    //------------------------------------------------------------------------
    SSL * SSLSocketPrc::setupSSL(SSL_CTX * ctx)
    {
        if (NULL == mSSL)
        {
            if(mSSLctx != ctx)
            {
                SSL_CTX_free(mSSLctx);
                mSSLctx = ctx;
            }
            mSSL = SSL_new(mSSLctx);
            if (NULL == mSSL)
            {
                MlogError("init ssl, create SSL object failed.");
                return 0;
            }
        }
        return mSSL;
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::shutdownSSL()
    {
        if(mSSL)
        {
            SSL_shutdown(mSSL);  /* send SSL/TLS close_notify */
            SSL_free(mSSL);
            mSSL = 0;
        }
    }
    //------------------------------------------------------------------------
    Ni32 SSLSocketPrc::connectSSL()
    {
        Ni32 re = SSL_connect(mSSL);
        if (re == 1)
        {
            mSSLConnect = true;
        }
        else if (re == 0)
        {
            Ni32 ecode = SSL_get_error(mSSL, re);
            N_log("ssl connect was shut down, remote ip: %s, port: %d, error code: %d.",
                GetRemoteIP(), GetRemotePort(), ecode);
            return -1;
        }
        else
        {
            Ni32 ecode = SSL_get_error(mSSL, re);
            if (SSL_ERROR_WANT_READ == ecode || SSL_ERROR_WANT_WRITE == ecode)
            {
                N_warning("ssl connect is blocking, remote ip: %s, port: %d, error code: %d.",
                    GetRemoteIP(), GetRemotePort(), ecode);
            }
            else
            {
                N_error("ssl connect failed, remote ip: %s, port: %d, error code: %d.",
                    GetRemoteIP(), GetRemotePort(), ecode);
                return -1;
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void SSLSocketPrc::getSSLError(NIIi res)
    {
        // The TLS/SSL handshake was not successful but was shut down
        // controlled and by the specifications of the TLS/SSL protocol.
        // Call SSL_get_error() with the return value ret to find out the reason.
        int err = SSL_get_error(mSSL, res);
        switch (err)
        {
        case SSL_ERROR_NONE:
            printf("SSL_ERROR_NONE\n");
            break;
        case SSL_ERROR_ZERO_RETURN:
            printf("SSL_ERROR_ZERO_RETURN\n");
            break;
        case SSL_ERROR_WANT_READ:
            printf("SSL_ERROR_WANT_READ\n");
            break;
        case SSL_ERROR_WANT_WRITE:
            printf("SSL_ERROR_WANT_WRITE\n");
            break;
        case SSL_ERROR_WANT_CONNECT:
            printf("SSL_ERROR_WANT_CONNECT\n");
            break;
        case SSL_ERROR_WANT_ACCEPT:
            printf("SSL_ERROR_WANT_ACCEPT\n");
            break;
        case SSL_ERROR_WANT_X509_LOOKUP:
            printf("SSL_ERROR_WANT_X509_LOOKUP\n");
            break;
        case SSL_ERROR_SYSCALL:
            {
                // http://www.openssl.org/docs/ssl/SSL_get_error.html
                char buff[1024];
                Nul ege = ERR_get_error();
                if (ege==0)
                    printf("SSL_ERROR_SYSCALL SSL_get_error ret=\n", res);
                else
                    printf("SSL_ERROR_SYSCALL %s\n", ERR_error_string(ege, buff));
            }
            break;
        case SSL_ERROR_SSL:
            printf("SSL_ERROR_SSL\n");
            break;
        }
    }
    //------------------------------------------------------------------------
}