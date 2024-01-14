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
#if _RAKNET_SUPPORT_ConsoleServer == 1

#include "NiiPreProcess.h"
#include "ConsoleServer.h"
#include "TransportInterface.h"
#include "NiiNetCmdBase.h"
#include <string.h>
#include <stdlib.h>

#define COMMAND_DELINATOR ' '
#define COMMAND_DELINATOR_TOGGLE '"'

#include "LinuxStrings.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    ConsoleServer::ConsoleServer()
    {
        transport=0;
        password[0]=0;
        prompt=0;
    }
    //------------------------------------------------------------------------
    ConsoleServer::~ConsoleServer()
    {
        if (prompt)
            N_free(prompt);
    }
    //------------------------------------------------------------------------
    void ConsoleServer::SetTransportProvider(TransportInterface *transportInterface, Nui16 port)
    {
        // Replace the current TransportInterface, stopping the old one, if present, and starting the new one.
        if (transportInterface)
        {
            if (transport)
            {
                RemoveCommandParser(transport->GetCommandParser());
                transport->Stop();
            }
            transport=transportInterface;
            transport->Start(port, true);

            Nui32 i;
            for (i=0; i < commandParserList.Size(); i++)
                commandParserList[i]->OnTransportChange(transport);

            //  The transport itself might have a command parser - for example password for the RakNet transport
            AddCommandParser(transport->GetCommandParser());
        }
    }
    //------------------------------------------------------------------------
    void ConsoleServer::AddCommandParser(CmdBase *commandParserInterface)
    {
        if (commandParserInterface==0)
            return;

        // Non-duplicate insertion
        Nui32 i;
        for (i=0; i < commandParserList.Size(); i++)
        {
            if (commandParserList[i]==commandParserInterface)
                return;

            if (_stricmp(commandParserList[i]->GetName(), commandParserInterface->GetName())==0)
            {
                // Naming conflict between two command parsers
                N_assert(0);
                return;
            }
        }

        commandParserList.Insert(commandParserInterface);
        if (transport)
            commandParserInterface->OnTransportChange(transport);
    }
    //------------------------------------------------------------------------
    void ConsoleServer::RemoveCommandParser(CmdBase *commandParserInterface)
    {
        if (commandParserInterface==0)
            return;

        // Overwrite the element we are removing from the back of the list and delete the back of the list
        Nui32 i;
        for (i=0; i < commandParserList.Size(); i++)
        {
            if (commandParserList[i]==commandParserInterface)
            {
                commandParserList[i]=commandParserList[commandParserList.Size()-1];
                commandParserList.RemoveFromEnd();
                return;
            }
        }
    }
    //------------------------------------------------------------------------
    void ConsoleServer::update()
    {
        Nui32 i;
        char * parameterList[20]; // Up to 20 parameters
        Nui32 numParameters;
        RegisteredCommand rc;

        SocketMessage * p = transport->receive();
        Address * newOrLostConnectionId = transport->onServerConnect(newOrLostConnectionId);

        if(newOrLostConnectionId && !newOrLostConnectionId->isInvalid())
        {
            for(i = 0; i < commandParserList.Size(); i++)
            {
                commandParserList[i]->OnNewIncomingConnection(newOrLostConnectionId, transport);
            }

            transport->send(newOrLostConnectionId, "Connected to remote command console.\r\nType 'help' for help.\r\n");
            ListParsers(newOrLostConnectionId);
            ShowPrompt(newOrLostConnectionId);
        }

        newOrLostConnectionId = transport->onConnectClose(newOrLostConnectionId);
        if(newOrLostConnectionId && !newOrLostConnectionId->isInvalid())
        {
            for(i = 0; i < commandParserList.Size(); i++)
                commandParserList[i]->OnConnectionLost(newOrLostConnectionId, transport);
        }

        while (p)
        {
            bool commandParsed=false;
            char copy[REMOTE_MAX_TEXT_INPUT];
            memcpy(copy, p->mData, p->mLength);
            copy[p->mLength] = 0;
            CmdBase::ParseConsoleString((char *)p->mData, COMMAND_DELINATOR, COMMAND_DELINATOR_TOGGLE, &numParameters, parameterList, 20); // Up to 20 parameters
            if (numParameters == 0)
            {
                N_delete p;
                p = transport->receive();
                continue;
            }
            if (_stricmp(*parameterList, "help")==0 && numParameters<=2)
            {
                // Find the parser specified and display help for it
                if (numParameters==1)
                {
                    transport->send(p->mAddress, "\r\nINSTRUCTIONS:\r\n");
                    transport->send(p->mAddress, "Enter commands on your keyboard, using spaces to delineate parameters.\r\n");
                    transport->send(p->mAddress, "You can use quotation marks to toggle space delineation.\r\n");
                    transport->send(p->mAddress, "You can connect multiple times from the same computer.\r\n");
                    transport->send(p->mAddress, "You can direct commands to a parser by prefixing the parser name or number.\r\n");
                    transport->send(p->mAddress, "COMMANDS:\r\n");
                    transport->send(p->mAddress, "help                                        Show this display.\r\n");
                    transport->send(p->mAddress, "help <ParserName>                           Show help on a particular parser.\r\n");
                    transport->send(p->mAddress, "help <CommandName>                          Show help on a particular command.\r\n");
                    transport->send(p->mAddress, "quit                                        Disconnects from the server.\r\n");
                    transport->send(p->mAddress, "[<ParserName>]   <Command> [<Parameters>]   Execute a command\r\n");
                    transport->send(p->mAddress, "[<ParserNumber>] <Command> [<Parameters>]   Execute a command\r\n");
                    ListParsers(p->mAddress);
                }
                else // numParameters == 2, including the help tag
                {
                    for (i=0; i < commandParserList.Size(); i++)
                    {
                        if (_stricmp(parameterList[1], commandParserList[i]->GetName())==0)
                        {
                            commandParsed=true;
                            commandParserList[i]->SendHelp(transport, p->mAddress);
                            transport->send(p->mAddress, "COMMAND LIST:\r\n");
                            commandParserList[i]->SendCommandList(transport, p->mAddress);
                            transport->send(p->mAddress, "\r\n");
                            break;
                        }
                    }

                    if (commandParsed==false)
                    {
                        // Try again, for all commands for all parsers.
                        RegisteredCommand rc;
                        for (i=0; i < commandParserList.Size(); i++)
                        {
                            if (commandParserList[i]->GetRegisteredCommand(parameterList[1], &rc))
                            {
                                if (rc.parameterCount==CmdBase::VARIABLE_NUMBER_OF_PARAMETERS)
                                    transport->send(p->mAddress, "(Variable parms): %s %s\r\n", rc.command, rc.commandHelp);
                                else
                                    transport->send(p->mAddress, "(%i parms): %s %s\r\n", rc.parameterCount, rc.command, rc.commandHelp);
                                commandParsed=true;
                                break;
                            }
                        }
                    }

                    if (commandParsed==false)
                    {
                        // Don't know what to do
                        transport->send(p->mAddress, "Unknown help topic: %s.\r\n", parameterList[1]);
                    }
                }
            }
            else if (_stricmp(*parameterList, "quit")==0 && numParameters==1)
            {
                transport->send(p->mAddress, "Goodbye!\r\n");
                transport->close(p->mAddress);
            }
            else
            {
                bool tryAllParsers=true;
                bool failed=false;

                if (numParameters >=2) // At minimum <CommandParserName> <Command>
                {
                    Nui32 commandParserIndex=(Nui32)-1;
                    // Prefixing with numbers directs to a particular parser
                    if (**parameterList>='0' && **parameterList<='9')
                    {
                        commandParserIndex=atoi(*parameterList); // Use specified parser unless it's an invalid number
                        commandParserIndex--; // Subtract 1 since we displayed numbers starting at index+1
                        if (commandParserIndex >= commandParserList.Size())
                        {
                            transport->send(p->mAddress, "Invalid index.\r\n");
                            failed=true;
                        }
                    }
                    else
                    {
                        // // Prefixing with the name of a command parser directs to that parser.  See if the first word matches a parser
                        for (i=0; i < commandParserList.Size(); i++)
                        {
                            if (_stricmp(parameterList[0], commandParserList[i]->GetName())==0)
                            {
                                commandParserIndex=i; // Matches parser at index i
                                break;
                            }
                        }
                    }

                    if (failed==false)
                    {
                        // -1 means undirected, so otherwise this is directed to a target
                        if (commandParserIndex!=(Nui32)-1)
                        {
                            // Only this parser should use this command
                            tryAllParsers=false;
                            if (commandParserList[commandParserIndex]->GetRegisteredCommand(parameterList[1], &rc))
                            {
                                commandParsed=true;
                                if (rc.parameterCount==CmdBase::VARIABLE_NUMBER_OF_PARAMETERS || rc.parameterCount==numParameters-2)
                                    commandParserList[commandParserIndex]->OnCommand(rc.command, numParameters-2, parameterList+2, transport, p->mAddress, copy);
                                else
                                    transport->send(p->mAddress, "Invalid parameter count.\r\n(%i parms): %s %s\r\n", rc.parameterCount, rc.command, rc.commandHelp);
                            }
                        }
                    }
                }

                if (failed == false && tryAllParsers)
                {
                    for (i=0; i < commandParserList.Size(); i++)
                    {
                        // Undirected command.  Try all the parsers to see if they understand the command
                        // Pass the 1nd element as the command, and the remainder as the parameter list
                        if (commandParserList[i]->GetRegisteredCommand(parameterList[0], &rc))
                        {
                            commandParsed=true;

                            if (rc.parameterCount==CmdBase::VARIABLE_NUMBER_OF_PARAMETERS || rc.parameterCount==numParameters-1)
                                commandParserList[i]->OnCommand(rc.command, numParameters-1, parameterList+1, transport, p->mAddress, copy);
                            else
                                transport->send(p->mAddress, "Invalid parameter count.\r\n(%i parms): %s %s\r\n", rc.parameterCount, rc.command, rc.commandHelp);
                        }
                    }
                }
                if (commandParsed==false && commandParserList.Size() > 0)
                {
                    transport->send(p->mAddress, "Unknown command:  Type 'help' for help.\r\n");
                }

            }

            ShowPrompt(p->mAddress);

            N_delete p;
            p = transport->receive();
        }
    }
    //------------------------------------------------------------------------
    void ConsoleServer::ListParsers(Address * address)
    {
        transport->send(address,"INSTALLED PARSERS:\r\n");
        for (Nui32 i = 0; i < commandParserList.Size(); i++)
        {
            transport->send(address, "%i. %s\r\n", i+1, commandParserList[i]->GetName());
        }
    }
    //------------------------------------------------------------------------
    void ConsoleServer::ShowPrompt(Address * address)
    {
         transport->send(address, prompt);
    }
    //------------------------------------------------------------------------
    void ConsoleServer::SetPrompt(const char * _prompt)
    {
        if (prompt)
            N_free(prompt);
        if (_prompt && _prompt[0])
        {
            size_t len = strlen(_prompt);
            prompt = (char *) N_alloc(len+1);
            strcpy(prompt, _prompt);
        }
        else
            prompt=0;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*