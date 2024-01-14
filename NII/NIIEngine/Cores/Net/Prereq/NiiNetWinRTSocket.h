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

#ifndef _NII_NET_WINRT_SOCKET_H_
#define _NII_NET_WINRT_SOCKET_H_

#include "NiiPreInclude.h"
#include "DS_List.h"

namespace NII
{
namespace NII_NET
{
    ref class ListenerContext;

    class WinStore8Socket : public SocketObj
    {
    public:
        WinStore8Socket();
        ~WinStore8Socket();

        virtual RNS2SendResult send(SocketSendData * param);

        RNS2BindResult Bind(Platform::String ^localServiceName);

        static int inetAddr(const char * cp);

        static int setSockOpt(Windows::Networking::Sockets::DatagramSocket ^s,
           int level, int optname, const char * optval, socklen_t optlen);

        static int ioctlSocket(Windows::Networking::Sockets::DatagramSocket ^s,
            long cmd, Nul * argp);

        static int getSockName(Windows::Networking::Sockets::DatagramSocket ^s,
            struct sockaddr * name, socklen_t * namelen);

        static WinStore8Socket * GetRNS2FromDatagramSocket(Windows::Networking::Sockets::DatagramSocket^ s);
    protected:
        static DataStructures::List<WinStore8Socket *> rns2List;
        static ThreadMutex rns2ListMutex;

        Windows::Networking::Sockets::DatagramSocket^ listener;
        ListenerContext^ listenerContext;
    };
}
}
#endif