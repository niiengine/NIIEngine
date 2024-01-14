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

#ifndef _NII_COMMAND_PARSER_INTERFACE_
#define _NII_COMMAND_PARSER_INTERFACE_

#include "NiiNetPreInclude.h"
#include "NiiNetCommon.h"
#include "DS_OrderedList.h"

namespace NII
{
namespace NII_NET
{
    /// \internal
    /// Contains the information related to one command registered with RegisterCommand()
    /// Implemented so I can have an automatic help system via SendCommandList()
    struct _EngineAPI RegisteredCommand
    {
        const char * command;
        const char * commandHelp;
        Nui8 parameterCount;
    };

    /// List of commands registered with RegisterCommand()
    int _EngineAPI RegisteredCommandComp(const char * const & key, const RegisteredCommand &data );

    /// \brief The interface used by command parsers.
    /// \details CmdBase provides a set of functions and interfaces that plug into the ConsoleServer class.
    /// Each CmdBase works at the same time as other interfaces in the system.
    class _EngineAPI CmdBase
    {
    public:
        CmdBase();
        virtual ~CmdBase();

        /// You are responsible for overriding this function and returning a static string, which will identifier your parser.
        /// This should return a static string
        /// \return The name that you return.
        virtual const char * GetName(void) const=0;

        /// \brief A callback for when \a address has connected to us.
        /// \param[in] address The player that has connected.
        /// \param[in] transport The transport interface that sent us this information.  Can be used to send messages to this or other players.
        virtual void  OnNewIncomingConnection(const Address & address, TransportInterface *transport);

        /// \brief A callback for when \a address has disconnected, either gracefully or forcefully
        /// \param[in] address The player that has disconnected.
        /// \param[in] transport The transport interface that sent us this information.
        virtual void OnConnectionLost(const Address & address, TransportInterface *transport);

        /// \brief A callback for when you are expected to send a brief description of your parser to \a address
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that requested help.
        virtual void SendHelp(TransportInterface *transport, const Address & address)=0;

        /// \brief Given \a command with parameters \a parameterList , do whatever processing you wish.
        /// \param[in] command The command to process
        /// \param[in] numParameters How many parameters were passed along with the command
        /// \param[in] parameterList The list of parameters.  parameterList[0] is the first parameter and so on.
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player that sent this command.
        /// \param[in] originalString The string that was actually sent over the network, in case you want to do your own parsing
        virtual bool OnCommand(const char * command, Nui32 numParameters, char ** parameterList, TransportInterface *transport,
            const Address & address, const char * originalString)=0;

        /// \brief This is called every time transport interface is registered.
        /// \details If you want to save a copy of the TransportInterface pointer
        /// This is the place to do it
        /// \param[in] transport The new TransportInterface
        virtual void OnTransportChange(TransportInterface * transport);

        /// \internal
        /// Scan commandList and return the associated array
        /// \param[in] command The string to find
        /// \param[out] rc Contains the result of this operation
        /// \return True if we found the command, false otherwise
        virtual bool GetRegisteredCommand(const char * command, RegisteredCommand * rc);

        /// \internal
        /// Goes through str, replacing the delineating character with 0's.
        /// \param[in] str The string sent by the transport interface
        /// \param[in] delineator The character to scan for to use as a delineator
        /// \param[in] delineatorToggle When encountered the delineator replacement is toggled on and off
        /// \param[out] numParameters How many pointers were written to \a parameterList
        /// \param[out] parameterList An array of pointers to characters.  Will hold pointers to locations inside \a str
        /// \param[in] parameterListLength How big the \a parameterList array is
        static void ParseConsoleString(char * str, const char delineator, Nui8 delineatorToggle,
            Nui32 * numParameters, char ** parameterList, Nui32 parameterListLength);

        /// \internal
        /// Goes through the variable commandList and sends the command portion of each struct
        /// \param[in] transport The transport interface we can use to write to
        /// \param[in] address The player to write to
        virtual void SendCommandList(TransportInterface * transport, const Address & address);

        static const Nui8 VARIABLE_NUMBER_OF_PARAMETERS;

        // Currently only takes static strings - doesn't make a copy of what you pass.
        // parameterCount is the number of parameters that the sender has to include with the command.
        // Pass 255 to parameterCount to indicate variable number of parameters

        /// Registers a command.
        /// \param[in] parameterCount How many parameters your command requires.  If you want to accept a variable number of commands, pass CmdBase::VARIABLE_NUMBER_OF_PARAMETERS
        /// \param[in] command A pointer to a STATIC string that has your command.  I keep a copy of the pointer here so don't deallocate the string.
        /// \param[in] commandHelp A pointer to a STATIC string that has the help information for your command.  I keep a copy of the pointer here so don't deallocate the string.
        virtual void RegisterCommand(Nui8 parameterCount, const char *command, const char *commandHelp);

        /// \brief Just writes a string to the remote system based on the result ( \a res ) of your operation
        /// \details This is not necessary to call, but makes it easier to return results of function calls.
        /// \param[in] res The result to write
        /// \param[in] command The command that this result came from
        /// \param[in] transport The transport interface that will be written to
        /// \param[in] address The player this result will be sent to
        virtual void ReturnResult(bool res, const char *command, TransportInterface * transport, const Address & address);
        virtual void ReturnResult(char *res, const char *command, TransportInterface * transport, const Address & address);
        virtual void ReturnResult(Address * res, const char *command, TransportInterface * transport, const Address & address);
        virtual void ReturnResult(int res, const char *command,TransportInterface * transport, const Address & address);

        /// \brief Just writes a string to the remote system when you are calling a function that has no return value.
        /// \details This is not necessary to call, but makes it easier to return results of function calls.
        /// \param[in] res The result to write
        /// \param[in] command The command that this result came from
        /// \param[in] transport The transport interface that will be written to
        /// \param[in] address The player this result will be sent to
        virtual void ReturnResult(const char *command,TransportInterface *transport, const Address & address);

    protected:
        DataStructures::OrderedList<const char*, RegisteredCommand, RegisteredCommandComp> commandList;
    };
}
}
#endif