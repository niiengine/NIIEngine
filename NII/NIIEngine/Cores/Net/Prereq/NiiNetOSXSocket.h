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

#ifndef _NII_NET_OSX_SOCKET_H_
#define _NII_NET_OSX_SOCKET_H_

#include "NiiPreInclude.h"

namespace NII
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

        Nsocket getSocket() const;

        virtual RNS2BindResult Bind(BSDSocketParam * param) = 0;

        void SetDoNotFragment(int opt);
    protected:
        RNS2BindResult BindShared(BSDSocketParam * param);
        RNS2BindResult BindSharedIPV4(BSDSocketParam * param);
        RNS2BindResult BindSharedIPV4And6(BSDSocketParam * param);

        unsigned RecvFromLoopInt();

        Thread * mThread;
        Nsocket mSocket;
        BSDSocketParam binding;
        STNui32 isRecvFromLoopThreadActive;
        volatile bool mStop;
        // http://sourceforge.net/p/open-dis/discussion/683284/thread/0929d6a0
        CFSocketRef _cfSocket;

    };
}
#endif