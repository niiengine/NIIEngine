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

#include "NiiNetCmdBase.h"
#include "TransportInterface.h"
#include <string.h>
#include <stdio.h>

#if defined(N_PLAT_WINRT)
#elif N_PLAT == N_PLAT_WIN32
// IP_DONTFRAGMENT is different between winsock 1 and winsock 2.  Therefore, Winsock2.h must be linked againt Ws2_32.lib
// winsock.h must be linked against WSock32.lib.  If these two are mixed up the flag won't work correctly
#include <winsock2.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "LinuxStrings.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    const Nui8 CmdBase::VARIABLE_NUMBER_OF_PARAMETERS=255;
    //------------------------------------------------------------------------
    int RegisteredCommandComp( const char* const & key, const RegisteredCommand &data )
    {
        return _stricmp(key,data.command);
    }
    //------------------------------------------------------------------------
    CmdBase::CmdBase() {}
    CmdBase::~CmdBase() {}
    //------------------------------------------------------------------------
    void CmdBase::ParseConsoleString(char *str, const char delineator,
        Nui8 delineatorToggle, Nui32 *numParameters, char **parameterList,
            Nui32 parameterListLength)
    {
        Nui32 strIndex, parameterListIndex;
        Nui32 strLen;
        bool replaceDelineator=true;

        strLen = (Nui32) strlen(str);

        // Replace every instance of delineator, \n, \r with 0
        for (strIndex=0; strIndex < strLen; strIndex++)
        {
            if (str[strIndex]==delineator && replaceDelineator)
                str[strIndex]=0;

            if (str[strIndex]=='\n' || str[strIndex]=='\r')
                str[strIndex]=0;

            if (str[strIndex]==delineatorToggle)
            {
                str[strIndex]=0;
                replaceDelineator=!replaceDelineator;
            }
        }

        // Fill up parameterList starting at each non-0
        for (strIndex=0, parameterListIndex=0; strIndex < strLen; )
        {
            if (str[strIndex]!=0)
            {
                parameterList[parameterListIndex]=str+strIndex;
                parameterListIndex++;
                N_assert(parameterListIndex < parameterListLength);
                if (parameterListIndex >= parameterListLength)
                    break;

                strIndex++;
                while (str[strIndex]!=0 && strIndex < strLen)
                    strIndex++;
            }
            else
                strIndex++;
        }

        parameterList[parameterListIndex]=0;
        *numParameters=parameterListIndex;
    }
    //------------------------------------------------------------------------
    void CmdBase::SendCommandList(TransportInterface * transport,
        const Address & address)
    {
        Nui32 i;
        if (commandList.Size())
        {
            for (i=0; i < commandList.Size(); i++)
            {
                transport->send(address, "%s", commandList[i].command);
                if (i < commandList.Size()-1)
                    transport->send(address, ", ");
            }
            transport->send(address, "\r\n");
        }
        else
            transport->send(address, "No registered commands\r\n");
    }
    //------------------------------------------------------------------------
    void CmdBase::RegisterCommand(Nui8 parameterCount, const char * command,
        const char * commandHelp)
    {
        RegisteredCommand rc;
        rc.command=command;
        rc.commandHelp=commandHelp;
        rc.parameterCount=parameterCount;
        commandList.Insert( command, rc, true);
    }
    //------------------------------------------------------------------------
    bool CmdBase::GetRegisteredCommand(const char * command,
        RegisteredCommand * rc)
    {
        bool objectExists;
        Nui32 index;
        index=commandList.GetIndexFromKey(command, &objectExists);
        if (objectExists)
            *rc=commandList[index];
        return objectExists;
    }
    //------------------------------------------------------------------------
    void CmdBase::OnTransportChange(TransportInterface *transport)
    {
        (void) transport;
    }
    //------------------------------------------------------------------------
    void CmdBase::OnNewIncomingConnection(const Address & address,
        TransportInterface * transport)
    {
        (void) address;
        (void) transport;
    }
    //------------------------------------------------------------------------
    void CmdBase::OnConnectionLost(const Address & address,
        TransportInterface * transport)
    {
        (void) address;
        (void) transport;
    }
    //------------------------------------------------------------------------
    void CmdBase::ReturnResult(bool res, const char * command,
        TransportInterface * transport, const Address & address)
    {
        if (res)
            transport->send(address, "%s returned true.\r\n", command);
        else
            transport->send(address, "%s returned false.\r\n", command);
    }
    //------------------------------------------------------------------------
    void CmdBase::ReturnResult(int res, const char * command,
        TransportInterface * transport, const Address & address)
    {
        transport->send(address, "%s returned %i.\r\n", command, res);
    }
    //------------------------------------------------------------------------
    void CmdBase::ReturnResult(const char * command,
        TransportInterface * transport, const Address & address)
    {
        transport->send(address, "Successfully called %s.\r\n", command);
    }
    //------------------------------------------------------------------------
    void CmdBase::ReturnResult(char * res, const char * command,
        TransportInterface * transport, const Address & address)
    {
        transport->send(address, "%s returned %s.\r\n", command, res);
    }
    //------------------------------------------------------------------------
    void CmdBase::ReturnResult(Address * res, const char * command,
        TransportInterface * transport, const Address & address)
    {
        String addr;
        address.read(addr, false);
        String addr2;
        res.read(addr2, false);
        transport->send(address, "%s returned %s %s:%i\r\n",
            command, addr.c_str(), addr2.c_str(), res.getHSBPort());
    }
    //------------------------------------------------------------------------
}
}