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
#if _RAKNET_SUPPORT_RakNetCommandParser==1

#include "NiiNetCmd.h"
#include "TransportInterface.h"
#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    Cmd::Cmd()
    {
        RegisterCommand(4, "init","(Nui32 maxConnections, Nui16 localPort, const char *forceHostAddress );");
        RegisterCommand(0,"init","();");
        RegisterCommand(0,"setSecure","(bool set);");
        RegisterCommand(1,"add","(const String & ip);");
        RegisterCommand(1,"remove","(const String & ip);");
        RegisterCommand(1,"isExist","(const String & ip);");
        RegisterCommand(1,"setMaxRemoteCount","(Nui16 numberAllowed);");
        RegisterCommand(0,"getMaxRemoteCount","() const;");
        RegisterCommand(4,"connect","(cons String & host, Nui16 port, const Nui8 * pwData, NCount pwSize);");
        RegisterCommand(2,"Disconnect","(Nui32 blockDuration, Nui8 pchannel = 0);");
        RegisterCommand(0,"isActive","() const;");
        RegisterCommand(0,"getConnect","() const;");
        RegisterCommand(3,"close","(const Address * target, bool sendDisconnectionNotification, Nui8 pchannel=0 );");
        RegisterCommand(2,"IsConnected","();");
        RegisterCommand(1,"getAddress","(Nui32 index);");
        RegisterCommand(2,"add","(const String & ip, TimeDurMS milliseconds = 0);");
        RegisterCommand(1,"remove","(const String & ip);");
        RegisterCommand(0,"removeAll","();");
        RegisterCommand(1,"isExist","(const String & ip);");
        RegisterCommand(1,"Ping1","(const Address & target);");
        RegisterCommand(3,"Ping2","(const String & host, Nui16 port, bool onlyReplyOnAcceptingConnections );");
        RegisterCommand(1,"getAvgPing","(const Address & address);");
        RegisterCommand(1,"getLastPing","(const Address & address) const;");
        RegisterCommand(1,"getWorstPing","(const Address & address) const;");
        RegisterCommand(1,"setPingTest","(bool doPing);");
        RegisterCommand(2,"setTempPing","(const Nui8 * data, NCount size);");
        RegisterCommand(0,"get","() const;");
        RegisterCommand(1,"getConnectTo","(Address target) const;");
        RegisterCommand(2,"setTimeOut","(const Address * target, TimeDurMS time);");
        RegisterCommand(0,"getMTU","() const;");
        RegisterCommand(0,"getLocalHostCount","();");
        RegisterCommand(1,"getLocalHost","(Nui32 index);");
        RegisterCommand(1,"setAlterIP","(bool allow);");
        RegisterCommand(4,"send","(const String & host, Nui16 port, const Nui8 * data, NCount size);");
        RegisterCommand(2,"setEnterPassword","(const Nui8 * pwData, int pwSize);");
        RegisterCommand(0,"getEnterPassword","();");
    }
    //------------------------------------------------------------------------
    Cmd::~Cmd()
    {
    }
    //------------------------------------------------------------------------
    void Cmd::setPrc(LinkBase * link)
    {
        mPeer = link;
    }
    //------------------------------------------------------------------------
    bool Cmd::OnCommand(const char * command, Nui32 numParameters,
        char ** parameterList, TransportInterface * transport, const Address & address,
            const char * originalString)
    {
        (void) originalString;
        (void) numParameters;

        if (mPeer==0)
            return false;

        if (strcmp(command, "init")==0)
        {
            HostPeer listener((Nui16)atoi(parameterList[1]), parameterList[2]);
            ReturnResult(mPeer->init((Nui16)atoi(parameterList[0]), &listener, 1), command, transport, address);
        }
        else if (strcmp(command, "init") == 0)
        {
            ReturnResult(mPeer->init(parameterList[0], parameterList[1]), command, transport, address);
        }
        else if (strcmp(command, "setSecure") == 0)
        {
            mPeer->setSecure(atoi(parameterList[1]) != 0);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "add") == 0)
        {
            mPeer->add(parameterList[1]);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "remove")==0)
        {
            mPeer->remove(parameterList[1]);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "isExist")==0)
        {
            ReturnResult(mPeer->isExist(parameterList[1]),command, transport, address);
        }
        else if (strcmp(command, "setMaxRemoteCount")==0)
        {
            mPeer->setMaxRemoteCount((Nui16)atoi(parameterList[0]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "getMaxRemoteCount") == 0)
        {
            ReturnResult((int) mPeer->getMaxRemoteCount(), command, transport, address);
        }
        else if (strcmp(command, "connect") == 0)
        {
            ReturnResult(mPeer->connect(parameterList[0], (Nui16)atoi(parameterList[1]),parameterList[2],atoi(parameterList[3]))==CONNECTION_ATTEMPT_STARTED, command, transport, address);
        }
        else if (strcmp(command, "Disconnect") == 0)
        {
            mPeer->shutdown(atoi(parameterList[0]), (Nui8)atoi(parameterList[1]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "isActive") == 0)
        {
            ReturnResult(mPeer->isActive(), command, transport, address);
        }
        else if (strcmp(command, "getConnect") == 0)
        {
            Address * remotes[32];
            Nui16 count=32;
            Nui32 i;
            if (mPeer->getConnect(remotes, &count))
            {
                if (count==0)
                {
                    transport->send(address, "getConnect() returned no systems connected.\r\n");
                }
                else
                {
                    transport->send(address, "getConnect() returned:\r\n");
                    for (i=0; i < count; i++)
                    {
                        String str1;
                        remotes[i].read(str1, true);
                        transport->send(address, "%i %s\r\n", i, str1.c_str());
                    }
                }
            }
            else
                transport->send(address, "getConnect() returned false.\r\n");
        }
        else if (strcmp(command, "close") == 0)
        {
            mPeer->close(Address(parameterList[0]), atoi(parameterList[1])!=0,(Nui8)atoi(parameterList[2]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "getState") == 0)
        {
            ReturnResult((int) mPeer->getState(Address(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "getAddress") == 0)
        {
            ReturnResult(mPeer->getAddress(atoi(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "add") == 0)
        {
            mPeer->add(parameterList[0], atoi(parameterList[1]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "remove") == 0)
        {
            mPeer->remove(parameterList[0]);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "removeAll") == 0)
        {
            mPeer->removeAll();
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "isExist") == 0)
        {
            ReturnResult(mPeer->isExist(parameterList[0]), command, transport, address);
        }
        else if (strcmp(command, "Ping1") == 0)
        {
            mPeer->ping(Address(parameterList[0]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "Ping2") == 0)
        {
            mPeer->ping(String(parameterList[0]), (Nui16) atoi(parameterList[1]), atoi(parameterList[2])!=0);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "getAvgPing") == 0)
        {
            ReturnResult(mPeer->getAvgPing(Address(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "getLastPing") == 0)
        {
            ReturnResult(mPeer->getLastPing(Address(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "getWorstPing") == 0)
        {
            ReturnResult(mPeer->getWorstPing(Address(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "setPingTest") == 0)
        {
            mPeer->setPingTest(atoi(parameterList[0]) != 0);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "setTempPing") == 0)
        {
            mPeer->setTempPing(parameterList[0], atoi(parameterList[1]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "get") == 0)
        {
            ReturnResult(mPeer->getLAN(), command, transport, address);
        }
        else if (strcmp(command, "getConnectTo") == 0)
        {
            ReturnResult(mPeer->getConnectTo(Address(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "setTimeOut") == 0)
        {
            mPeer->setTimeOut(&Address(parameterList[1]), atoi(parameterList[0]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "getMTU") == 0)
        {
            ReturnResult(mPeer->getMTU(Address::invalid), command, transport, address);
        }
        else if (strcmp(command, "getLocalHostCount") == 0)
        {
            ReturnResult((int)mPeer->getLocalHostCount(), command, transport, address);
        }
        else if (strcmp(command, "getLocalHost") == 0)
        {
            ReturnResult((char*) mPeer->getLocalHost(atoi(parameterList[0])), command, transport, address);
        }
        else if (strcmp(command, "setAlterIP") == 0)
        {
            mPeer->setAlterIP(atoi(parameterList[0]) != 0);
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "send") == 0)
        {
            mPeer->send(parameterList[0], (Nui16) atoi(parameterList[1]),parameterList[2],atoi(parameterList[3]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "setEnterPassword") == 0)
        {
            mPeer->setEnterPassword(parameterList[0], atoi(parameterList[1]));
            ReturnResult(command, transport, address);
        }
        else if (strcmp(command, "getEnterPassword") == 0)
        {
            char password[256];
            int passwordLength;
            mPeer->getEnterPassword(password, &passwordLength);
            if (passwordLength)
                ReturnResult((char *)password, command, transport, address);
            else
                ReturnResult(0, command, transport, address);
        }
        return true;
    }
    //------------------------------------------------------------------------
    const char * Cmd::GetName() const
    {
        return "RakNet";
    }
    //------------------------------------------------------------------------
    void Cmd::SendHelp(TransportInterface * transport, const Address & address)
    {
        if (mPeer)
        {
            transport->send(address, "The RakNet parser provides mirror functions to NetworkSys\r\n");
            transport->send(address, "SystemAddresss take two parameters: send <BinaryAddress> <Port>.\r\n");
            transport->send(address, "For bool, send 1 or 0.\r\n");
        }
        else
        {
            transport->send(address, "Parser not active.  Call setPrc.\r\n");
        }
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*
