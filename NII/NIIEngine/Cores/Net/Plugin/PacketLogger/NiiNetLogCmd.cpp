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
#if _RAKNET_SUPPORT_LogCommandParser==1

#include "NiiNetLogCmd.h"
#include "TransportInterface.h"

#include <memory.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "LinuxStrings.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    LogCmd::LogCmd()
    {
        RegisterCommand(CmdBase::VARIABLE_NUMBER_OF_PARAMETERS,"Subscribe","[<ChannelName>] - Subscribes to a named channel, or all channels");
        RegisterCommand(CmdBase::VARIABLE_NUMBER_OF_PARAMETERS,"Unsubscribe","[<ChannelName>] - Unsubscribes from a named channel, or all channels");
        memset(channelNames,0,sizeof(channelNames));
    }
    //------------------------------------------------------------------------
    LogCmd::~LogCmd()
    {
    }
    //------------------------------------------------------------------------
    bool LogCmd::OnCommand(const char * command, Nui32 numParameters,
        char ** parameterList, TransportInterface * transport, const Address & address,
            const char * originalString)
    {
        (void) originalString;

        if (strcmp(command, "Subscribe")==0)
        {
            Nui32 channelIndex;
            if (numParameters==0)
            {
                Subscribe(address, 0);
                transport->send(address, "Subscribed to all channels.\r\n");
            }
            else if (numParameters==1)
            {
                if ((channelIndex=Subscribe(address, parameterList[0]))!=(Nui32)-1)
                {
                    transport->send(address, "You are now subscribed to channel %s.\r\n", channelNames[channelIndex]);
                }
                else
                {
                    transport->send(address, "Cannot find channel %s.\r\n", parameterList[0]);
                    PrintChannels(address, transport);
                }
            }
            else
            {
                transport->send(address, "Subscribe takes either 0 or 1 parameters.\r\n");
            }
        }
        else if (strcmp(command, "Unsubscribe")==0)
        {
            Nui32 channelIndex;
            if (numParameters==0)
            {
                Unsubscribe(address, 0);
                transport->send(address, "Unsubscribed from all channels.\r\n");
            }
            else if (numParameters==1)
            {
                if ((channelIndex=Unsubscribe(address, parameterList[0]))!=(Nui32)-1)
                {
                    transport->send(address, "You are now unsubscribed from channel %s.\r\n", channelNames[channelIndex]);
                }
                else
                {
                    transport->send(address, "Cannot find channel %s.\r\n", parameterList[0]);
                    PrintChannels(address, transport);
                }
            }
            else
            {
                transport->send(address, "Unsubscribe takes either 0 or 1 parameters.\r\n");
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    const char *LogCmd::GetName(void) const
    {
        return "Logger";
    }
    //------------------------------------------------------------------------
    void LogCmd::SendHelp(TransportInterface * transport, const Address & address)
    {
        transport->send(address, "The logger will accept user log data via the Log(...) function.\r\n");
        transport->send(address, "Each log is associated with a named channel.\r\n");
        transport->send(address, "You can subscribe to or unsubscribe from named channels.\r\n");
        PrintChannels(address, transport);
    }
    //------------------------------------------------------------------------
    void LogCmd::AddChannel(const char *channelName)
    {
        Nui32 channelIndex = 0;
        channelIndex = GetChannelIndexFromName(channelName);
        // Each channel can only be added once.
        N_assert(channelIndex == (Nui32)-1);

        NCount i;
        for (i = 0; i < 32; ++i)
        {
            if (channelNames[i]==0)
            {
                // Assuming a persistent static string.
                channelNames[i]=channelName;
                return;
            }
        }
        // No more available channels - max 32 with this implementation where I save subscribed channels with bit operations
        N_assert(0);
    }
    //------------------------------------------------------------------------
    void LogCmd::WriteLog(const char * channelName, const char * format, ...)
    {
        if (channelName==0 || format==0)
            return;

        Nui32 channelIndex;
        channelIndex = GetChannelIndexFromName(channelName);
        if (channelIndex==(Nui32)-1)
        {
            AddChannel(channelName);
        }

        char text[REMOTE_MAX_TEXT_INPUT];
        va_list ap;
        va_start(ap, format);
        _vsnprintf(text, REMOTE_MAX_TEXT_INPUT, format, ap);
        va_end(ap);
        text[REMOTE_MAX_TEXT_INPUT-1]=0;

        // Make sure that text ends in \r\n
        int textLen;
        textLen=(int)strlen(text);
        if (textLen==0)
            return;
        if (text[textLen-1]=='\n')
        {
            text[textLen-1]=0;
        }
        if (textLen < REMOTE_MAX_TEXT_INPUT-4)
            strcat(text, "\r\n");
        else
        {
            text[textLen-3]='\r';
            text[textLen-2]='\n';
            text[textLen-1]=0;
        }

        // For each user that subscribes to this channel, send to them.
        NCount i;
        for (i=0; i < remoteUsers.Size(); ++i)
        {
            if (remoteUsers[i].channels & (1 << channelIndex))
            {
                trans->send(remoteUsers[i].mAddress, text);
            }
        }
    }
    //------------------------------------------------------------------------
    void LogCmd::PrintChannels(const Address &address, TransportInterface *transport) const
    {
        NCount i;
        bool anyChannels=false;
        transport->send(address, "CHANNELS:\r\n");
        for (i = 0; i < 32; ++i)
        {
            if (channelNames[i])
            {
                transport->send(address, "%i. %s\r\n", i+1,channelNames[i]);
                anyChannels=true;
            }
        }
        if (anyChannels==false)
            transport->send(address, "None.\r\n");
    }
    //------------------------------------------------------------------------
    void LogCmd::OnNewIncomingConnection(const Address & address,
        TransportInterface * transport)
    {
        (void) address;
        (void) transport;
    }
    //------------------------------------------------------------------------
    void LogCmd::OnConnectionLost(const Address & address, TransportInterface * transport)
    {
        (void) transport;
        Unsubscribe(address, 0);
    }
    //------------------------------------------------------------------------
    Nui32 LogCmd::Unsubscribe(const Address & address, const char * channelName)
    {
        NCount i;
        for (i = 0; i < remoteUsers.Size(); ++i)
        {
            if (remoteUsers[i].mAddress == address)
            {
                if (channelName==0)
                {
                    // Unsubscribe from all and delete this user.
                    remoteUsers[i] = remoteUsers[remoteUsers.Size() - 1];
                    remoteUsers.RemoveFromEnd();
                    return 0;
                }
                else
                {
                    Nui32 channelIndex;
                    channelIndex = GetChannelIndexFromName(channelName);
                    if (channelIndex != (Nui32)-1)
                    {
                        remoteUsers[i].channels&=0xFFFF ^ (1<<channelIndex); // Unset this bit
                    }
                    return channelIndex;
                }
            }
        }
        return (Nui32)-1;
    }
    //------------------------------------------------------------------------
    Nui32 LogCmd::Subscribe(const Address & address, const char * channelName)
    {
        Nui32 i;
        Nui32 channelIndex = (Nui32)-1;
        if (channelName)
        {
            channelIndex = GetChannelIndexFromName(channelName);
            if (channelIndex == (Nui32)-1)
                return channelIndex;
        }

        for (i = 0; i < remoteUsers.Size(); ++i)
        {
            if (remoteUsers[i].mAddress == address)
            {
                if (channelName)
                    remoteUsers[i].channels| = 1<<channelIndex; // Set this bit for an existing user
                else
                    remoteUsers[i].channels = 0xFFFF;
                return channelIndex;
            }
        }

        // Make a new user
        SystemAddressAndChannel newUser;
        newUser.mAddress = address;
        if (channelName)
            newUser.channels = 1<<channelIndex;
        else
            newUser.channels = 0xFFFF;
        remoteUsers.Insert(newUser);
        return channelIndex;
    }
    //------------------------------------------------------------------------
    Nui32 LogCmd::GetChannelIndexFromName(const char * channelName)
    {
        NCount i;
        for (i = 0; i < 32; ++i)
        {
            if (channelNames[i] == 0)
                return (Nui32) -1;

            if (_stricmp(channelNames[i], channelName) == 0)
                return i;
        }
        return (Nui32)-1;
    }
    //------------------------------------------------------------------------
    void LogCmd::OnTransportChange(TransportInterface * transport)
    {
        // I don't want users to have to pass TransportInterface *transport to Log.
        trans = transport;
    }
    //------------------------------------------------------------------------
}
}
#endif