/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_NET_PLAT_H_
#define _NII_NET_PLAT_H_

#if (N_PLAT == N_PLAT_OSX) || (N_PLAT == N_PLAT_IOS)
    #import <CoreFoundation/CoreFoundation.h>
    #include <sys/socket.h>
    #include <sys/event.h>
    #include <sys/time.h>
    #include <sys/syscall.h>
    #include <netinet/in.h>
    #include <net/if.h>
#endif

#if N_PLAT == N_PLAT_WINRT
    #include <windows.h>
    #include <winsock.h>
    #include "WinRTSockAddr.h"
	#include "WinRTSocketAdapter.h"
    #define FORMAT_MESSAGE_ALLOCATE_BUFFER 0
    #define FIONBIO 0
    #define LocalFree(x)
    typedef unsigned int                socklen_t;    
    #define NPP_Instance                int
	#define N_net_accept                WinRTAccept
	#define N_net_connect               WinRTConnect
	#define N_net_close                 WinRTClose
	#define N_net_socket                WinRTCreateDatagramSocket
	#define N_net_bind                  WinRTBind
	#define N_net_getsockname           WinStore8Socket::getSockName
	#define N_net_getsockopt            WinRTGetSockOpt
	#define N_net_inet_addr             WinStore8Socket::inetAddr
	#define N_net_ioctl                 WinStore8Socket::ioctlSocket
	#define N_net_listen                WinRTListen
	#define N_net_recv                  WinRTRecv
	#define N_net_recvfrom              WinRTRecvFrom
	#define N_net_select                WinRTSelect
	#define N_net_send                  WinRTSend
	#define N_net_sendto                WinRTSendTo
	#define N_net_setsockopt            WinStore8Socket::setSockOpt
	#define N_net_shutdown              WinRTShutdown
    #define N_SocketError               SOCKET_ERROR
    #define N_InvalidSocket             INVALID_SOCKET    
    typedef Windows::Networking::Sockets::DatagramSocket^   Nudpsocket;
    typedef Windows::Networking::Sockets::StreamSocket^     Ntcpsocket;
    typedef SOCKET                                          Nsocket;
#elif N_PLAT == N_PLAT_WIN32
    #include <winsock2.h>
    #include <winbase.h>
    #include <windows.h>
    #include <direct.h>
    #include <ws2tcpip.h>    
    typedef int                         socklen_t;
    #define NPP_Instance                int
    #define N_net_close                 closesocket
    #define N_net_select                select
	#define N_net_accept                accept
	#define N_net_connect               connect
    #define N_net_socket                socket
	#define N_net_bind                  bind
	#define N_net_getsockname           getsockname
	#define N_net_getsockopt            getsockopt
    #define N_net_inet_addr             inet_addr
	#define N_net_ioctl                 ioctlsocket
	#define N_net_listen                listen
	#define N_net_recv                  recv
	#define N_net_recvfrom              recvfrom
    #define N_net_sendto                sendto
	#define N_net_send                  send
    #define N_net_setsockopt            setsockopt
	#define N_net_shutdown              shutdown
    #define N_SocketError               SOCKET_ERROR
    #define N_InvalidSocket             INVALID_SOCKET
    typedef SOCKET                      Nudpsocket;
    typedef SOCKET                      Ntcpsocket;
    typedef SOCKET                      Nsocket;
#else N_PLAT == N_PLAT_NACL
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <sys/epoll.h>
    #include <netinet/in.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <ifaddrs.h>
    #include <stdint.h>
    #include <signal.h>
    #include "ppapi/cpp/private/net_address_private.h"
    #include "ppapi/c/pp_bool.h"
    #include "ppapi/c/pp_errors.h"
    #include "ppapi/cpp/completion_callback.h"
    #include "ppapi/cpp/instance_handle.h"
    #include "ppapi/cpp/module.h"
    #include "ppapi/cpp/module_impl.h"
    #include "ppapi/c/pp_errors.h"
    #include "ppapi/c/pp_module.h"
    #include "ppapi/c/pp_var.h"
    #include "ppapi/c/pp_resource.h"
    #include "ppapi/c/ppb.h"
    #include "ppapi/c/ppb_instance.h"
	#include "ppapi/c/ppb_messaging.h"
	#include "ppapi/c/ppb_var.h"
	#include "ppapi/c/ppp.h"
	#include "ppapi/c/ppb_core.h"
	#include "ppapi/c/ppp_instance.h"
	#include "ppapi/c/ppp_messaging.h"
	#include "ppapi/c/pp_input_event.h"
	#include "ppapi/c/pp_completion_callback.h"
	#include "ppapi/c/private/ppb_udp_socket_private.h"
	#include "ppapi/cpp/private/net_address_private.h"
    #define NPP_Instance                PP_Instance
    #define N_net_select                select
	#define N_net_accept                accept
	#define N_net_connect               connect
    #define N_net_socket                socket
	#define N_net_bind                  bind
	#define N_net_getsockname           getsockname
	#define N_net_getsockopt            getsockopt
    #define N_net_inet_addr             inet_addr
	#define N_net_ioctl                 ioctlsocket
	#define N_net_listen                listen
	#define N_net_recv                  recv
	#define N_net_recvfrom              recvfrom
    #define N_net_sendto                sendto
	#define N_net_send                  send
    #define N_net_setsockopt            setsockopt
	#define N_net_shutdown              shutdown
    #define N_SocketError               -1
    #define N_InvalidSocket             -1
	typedef PP_Resource                 Nudpsocket;
	typedef PP_Resource                 Ntcpsocket;
    typedef int                         Nsocket;
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <stdint.h>
    #include <signal.h>
    #if (N_PLAT != N_PLAT_OSX) && (N_PLAT != N_PLAT_IOS)
        #include <sys/epoll.h>
    #endif
    #if N_PLAT != N_PLAT_ANDROID
        #include <ifaddrs.h>
    #endif
    #define NPP_Instance                int
    #define N_net_close                 close
    #define N_net_select                select
	#define N_net_accept                accept
	#define N_net_connect               connect
    #define N_net_socket                socket
	#define N_net_bind                  bind
	#define N_net_getsockname           getsockname
	#define N_net_getsockopt            getsockopt
    #define N_net_inet_addr             inet_addr
	#define N_net_ioctl                 ioctlsocket
	#define N_net_listen                listen
	#define N_net_recv                  recv
	#define N_net_recvfrom              recvfrom
    #define N_net_sendto                sendto
	#define N_net_send                  send
    #define N_net_setsockopt            setsockopt
	#define N_net_shutdown              shutdown
    #define N_SocketError               -1
    #define N_InvalidSocket             -1
    typedef int                         Nudpsocket;
    typedef int                         Ntcpsocket;
    typedef int                         Nsocket;
#endif

#endif