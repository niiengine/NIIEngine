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

#ifndef _NII_NET_WIN32_SOCKET_H_
#define _NII_NET_WIN32_SOCKET_H_

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
        int CreateRecvPollingThread(int level);

        void run();
        
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
    };

    class _EngineAPI SocketLayerOverride
    {
    public:
        SocketLayerOverride(){}
        virtual ~SocketLayerOverride(){}

        /// Called when SendTo would otherwise occur.
        virtual int RakNetSendTo(const char * data, int length, const Address & address)=0;

        /// Called when RecvFrom would otherwise occur. Return number of bytes read. Write data into dataOut
        // Return -1 to use RakNet's normal recvfrom, 0 to abort RakNet's normal recvfrom, and positive to return data
        virtual int RakNetRecvFrom(char dataOut[N_Net_MaxMtuSize], Address *& senderOut, bool calledFromMainThread )=0;
    };

    class RNS2_Windows : public RNS2_Berkley
    {
    public:
        RNS2_Windows();
        
        virtual ~RNS2_Windows();
        
        RNS2BindResult Bind(BSDSocketParam * param);
        
        RNS2SendResult send(SocketSendData * param);
        
        void SetSocketLayerOverride(SocketLayerOverride * _slo);
        
        SocketLayerOverride * GetSocketLayerOverride();
    protected:
        SocketLayerOverride * slo;
    };
}
}
#endi