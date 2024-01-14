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

#include "NativeFeatureIncludes.h"
#if _RAKNET_SUPPORT_TelnetTransport == 1 && _RAKNET_SUPPORT_TCPInterface == 1

#ifndef __TELNET_TRANSPORT
#define __TELNET_TRANSPORT

#include "NiiNetPreInclude.h"
#include "TransportInterface.h"
#include "DS_List.h"

namespace NII
{
namespace NII_NET
{
    /// \brief Use TelnetTransport to easily allow windows telnet to connect to your ConsoleServer
    /// \details To run Windows telnet, go to your start menu, click run, and in the edit box type "telnet <IP>" where <IP> is the ip address.<BR>
    /// of your ConsoleServer (most likely the same IP as your game).<BR>
    /// This implementation always echos commands.
    class _EngineAPI TelnetTransport : public TransportInterface
    {
    public:
        TelnetTransport();
        virtual ~TelnetTransport();
        bool Start(Nui16 port, bool serverMode);
        void Stop(void);
        void send(Address * address, const Nui8 * data, ...);
        void close(Address * address);
        SocketMessage * receive();
        void HasNewIncomingConnection(Address * address);
        void onConnectClose(Address * address);
        CmdBase * GetCommandParser();
        void SetSendSuffix(const Nui8 * suffix);
        void SetSendPrefix(const Nui8 * prefix);
    protected:
        struct TelnetClient
        {
            Address * mAddress;
            char textInput[REMOTE_MAX_TEXT_INPUT];
            char lastSentTextInput[REMOTE_MAX_TEXT_INPUT];
            Nui32 cursorPosition;
        };
        void AutoAllocate(void);
        bool ReassembleLine(TelnetTransport::TelnetClient* telnetClient, Nui8 c);

        NetworkObj * mPrc;    
        // Crap this sucks but because windows telnet won't send line at a time, I have to reconstruct the lines at the server per player
        DataStructures::List<TelnetClient *> mRemoteList;

        Nui8 *sendSuffix, *sendPrefix;

    };
}
}
#endif
#endif