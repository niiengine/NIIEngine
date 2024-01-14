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
#if _RAKNET_SUPPORT_ConsoleServer==1

#ifndef __CONSOLE_SERVER_H
#define __CONSOLE_SERVER_H

#include "NiiNetPreInclude.h"
#include "DS_List.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    /// \brief The main entry point for the server portion of your remote console application support.
    /// \details ConsoleServer takes one TransportInterface and one or more CmdBase (s)
    /// The TransportInterface will be used to send data between the server and the client.  The connecting client must support the
    /// protocol used by your derivation of TransportInterface . TelnetTransport and RakNetTransport are two such derivations .
    /// When a command is sent by a remote console, it will be processed by your implementations of CmdBase
    class _EngineAPI ConsoleServer
    {
    public:
        ConsoleServer();
        ~ConsoleServer();

        /// \brief Call this with a derivation of TransportInterface so that the console server can send and receive commands
        /// \param[in] transportInterface Your interface to use.
        /// \param[in] port The port to host on.  Telnet uses port 23 by default.  RakNet can use whatever you want.
        void SetTransportProvider(TransportInterface *transportInterface, Nui16 port);

        /// \brief Add an implementation of CmdBase to the list of command parsers.
        /// \param[in] commandParserInterface The command parser referred to
        void AddCommandParser(CmdBase *commandParserInterface);

        /// \brief Remove an implementation of CmdBase previously added with AddCommandParser().
        /// \param[in] commandParserInterface The command parser referred to
        void RemoveCommandParser(CmdBase *commandParserInterface);

        /// \brief Call update to read packet sent from your TransportInterface.
        /// You should do this fairly frequently.
        void update();

        /// \brief Sets a prompt to show when waiting for user input.
        /// \details Pass an empty string to clear the prompt
        /// Defaults to no prompt
        /// \param[in] _prompt Null-terminated string of the prompt to use. If you want a newline, be sure to use /r/n
        void SetPrompt(const char *_prompt);
    protected:
        void ListParsers(Address * address);
        void ShowPrompt(Address * address);
        
        TransportInterface * transport;
        DataStructures::List<CmdBase *> commandParserList;
        char * password[256];
        char * prompt;
    };
}
}
#endif
#endif