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

#ifndef _NII_NET_LINUX_SOCKET_H_
#define _NII_NET_LINUX_SOCKET_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_NET
{
    class RNS2_Berkley : public SocketObj, public ThreadMain
    {
    public:
        RNS2_Berkley();
        virtual ~RNS2_Berkley();
        
        void run();
        
        int CreateRecvPollingThread(int level);

        void SignalStopRecvPollingThread();

        void BlockOnStopRecvPollingThread();

        const BSDSocketParam * GetBindings() const;

        Nsocket getSocket(void) const;

        virtual RNS2BindResult Bind(BSDSocketParam * param) = 0;

        void SetDoNotFragment(int opt);
    protected:
        // Used by other classes
        RNS2BindResult BindShared(BSDSocketParam * param);
        RNS2BindResult BindSharedIPV4(BSDSocketParam * param);
        RNS2BindResult BindSharedIPV4And6(BSDSocketParam * param);

        unsigned RecvFromLoopInt();
    protected:
        Thread * mThread;
        Nsocket mSocket;
        BSDSocketParam binding;

        STNui32 isRecvFromLoopThreadActive;
        volatile bool mStop;
    };

    class RNS2_Linux : public RNS2_Berkley
    {
    public:
        RNS2BindResult Bind(BSDSocketParam * param);
        RNS2SendResult send(SocketSendData * param);
    protected:
    };
}
}
#endif