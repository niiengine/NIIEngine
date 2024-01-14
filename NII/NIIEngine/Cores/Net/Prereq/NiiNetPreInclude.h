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

#ifndef _NII_NET_PRE_INCLUDE_H_
#define _NII_NET_PRE_INCLUDE_H_

#include "NiiPreInclude.h"
#include "NiiNetPlat.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    struct Remote;
    class SocketMessage;
    class Address;
    struct TelnetClient;
    struct ConnectRequest;
    struct DownloadRequest;
    struct FileListReceiver;
    struct RemoteHash;
    class LinkBase;
    class NetworkSys;
    class RakNetTransport;
    class NetworkObj;
    class TransportInterface;
    class Plugin;
    class RakNetRandom;
    class FileList;
    class FileListProgress;
    class FileListTransfer;
    class FileListTransferCBInterface;
    class BufferReadBase;
    class NetworkIDManager;
    class CloudClientCallback;
    class CmdBase;
    class LogCmd;
    class HuffmanEncodingTree;
    class ThreadMutex;
	class RakString;
    class TM_Team;
    class TM_TeamMember;
    class TM_World;
    class TeamManager;

    typedef bool            Mbool;
    typedef char            Mi8;
    typedef wchar_t			Mw16;
    typedef Nui8            Mui8;
    typedef Ni16            Mi16;
    typedef Nui16           Mui16;
    typedef Ni32            Mi32;
    typedef Nui32           Mui32;
    typedef Ni64            Mi64;
    typedef Nui64           Mui64;
    typedef Nui32           MCount;
    typedef long            Mlong;

    #define N_B_TO_b(x)     (((x) + 7) >> 3)
    #define N_b_TO_B(x)     ((x) << 3)
    typedef Nui8            Token;
}
}

#define ATOMIC_ADD(src_ptr, v)                  (void)__sync_add_and_fetch(src_ptr, v)
#define ATOMIC_SUB_AND_FETCH(src_ptr, v)        __sync_sub_and_fetch(src_ptr, v)
#define ATOMIC_ADD_AND_FETCH(src_ptr, v)        __sync_add_and_fetch(src_ptr, v)
#define ATOMIC_FETCH(src_ptr)                   __sync_add_and_fetch(src_ptr, 0)
#define ATOMIC_SET(src_ptr, v)                  (void)__sync_bool_compare_and_swap(src_ptr, *(src_ptr), v)

#define N_Socket_ServerTimeOut				    50000
#define N_Socket_ClientTimeOut			        90000
#define N_Socket_ServerHeartBeatInterval	    5000
#define N_Socket_ClientHeartBeatInterval	    15000
#define N_Socket_ReadBufferSize	                2048
#define N_Socket_MaxBufferSize                  (64 * 4096)

#ifndef N_AllocaStackSize
#define N_AllocaStackSize                       1048576
#endif

#ifndef N_OpenSSLSupport
#define N_OpenSSLSupport                        0
#endif

#ifndef N_Socket_MaxLocalIP
#define N_Socket_MaxLocalIP                     10
#endif

/// This controls the amount of memory used per connection.
/// This many datagrams are tracked by mMark. If more than this many datagrams are sent, then an ack for an older datagram would be ignored
/// This results in an unnecessary resend in that case
#ifndef DATAGRAM_MESSAGE_ID_ARRAY_LENGTH
#define DATAGRAM_MESSAGE_ID_ARRAY_LENGTH        512
#endif

#ifndef RESEND_BUFFER_ARRAY_LENGTH
#define RESEND_BUFFER_ARRAY_LENGTH              512
#define RESEND_BUFFER_ARRAY_MASK                511
#endif

#ifndef N_Socket_SlidingWindow
#define N_Socket_SlidingWindow                  1
#endif

#ifndef N_Socket_PreEnlargePacket
#define N_Socket_PreEnlargePacket               0
#endif

#ifndef N_Socket_MaxRPCFuncCount
#define N_Socket_MaxRPCFuncCount                48
#endif

#ifndef N_Socket_MaxRPCFuncName
#define N_Socket_MaxRPCFuncName                 48
#endif

// If defined to 1, the user is responsible for calling NetworkSys::updateImpl and NetworkSys::RunRecvfrom
#ifndef RAKPEER_USER_THREADED
#define RAKPEER_USER_THREADED                   0
#endif

#endif