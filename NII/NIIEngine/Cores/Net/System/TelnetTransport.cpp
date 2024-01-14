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

#include "NiiPreProcess.h"
#include "TelnetTransport.h"
#include "NetworkObj.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "LinuxStrings.h"

#define ECHO_INPUT

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    TelnetTransport::TelnetTransport()
    {
        mPrc = 0;
        sendSuffix = 0;
        sendPrefix = 0;
    }
    //------------------------------------------------------------------------
    TelnetTransport::~TelnetTransport()
    {
        Stop();
        if (sendSuffix)
            N_free(sendSuffix);
        if (sendPrefix)
            N_free(sendPrefix);
    }
    //------------------------------------------------------------------------
    bool TelnetTransport::Start(Nui16 port, bool serverMode)
    {
        (void) serverMode;
        AutoAllocate();
        N_assert(serverMode);
        return mPrc->Start(port, 64);
    }
    //------------------------------------------------------------------------
    void TelnetTransport::Stop(void)
    {
        if (mPrc == 0) 
            return;
        mPrc->Stop();
        Nui32 i;
        for(i = 0; i < mRemoteList.Size(); i++)
            N_delete mRemoteList[i];
        mRemoteList.Clear(false);
        N_delete mPrc;
        mPrc=0;
    }
    //------------------------------------------------------------------------
    void TelnetTransport::send(Address * address, const Nui8 * data,... )
    {
        if(mPrc == 0) 
			return;

        if(data == 0 || data[0] == 0)
            return;

        char text[REMOTE_MAX_TEXT_INPUT];
        size_t prefixLength;
        if(sendPrefix)
        {
            strcpy(text, sendPrefix);
            prefixLength = strlen(sendPrefix);
        }
        else
        {
            text[0] = 0;
            prefixLength = 0;
        }
        va_list ap;
        va_start(ap, data);
        _vsnprintf(text+prefixLength, REMOTE_MAX_TEXT_INPUT-prefixLength, data, ap);
        va_end(ap);
        text[REMOTE_MAX_TEXT_INPUT-1]=0;

        if (sendSuffix)
        {
            size_t length = strlen(text);
            size_t availableChars = REMOTE_MAX_TEXT_INPUT-length-1;
            strncat(text, sendSuffix, availableChars);
        }

        mPrc->send(text, (Nui32) strlen(text), address, false);
    }
    //------------------------------------------------------------------------
    void TelnetTransport::close(Address * address)
    {
        mPrc->close(address);
    }
    //------------------------------------------------------------------------
    SocketMessage * TelnetTransport::receive()
    {
        if(mPrc == 0) 
            return 0;
        SocketMessage * p = mPrc->receive();
        if(p == 0)
            return 0;

        // Get this guy's cursor buffer.  This is real bullcrap that I have to do this.
        Nui32 i;
        TelnetClient * remoteClient = 0;
        for (i = 0; i < mRemoteList.Size(); i++)
        {
            if (*mRemoteList[i]->mAddress == *p->mAddress)
                remoteClient = mRemoteList[i];
        }

        if (remoteClient == 0)
        {
            N_delete p;
            return 0;
        }


        if (p->mLength == 3 && p->mData[0] == 27 && p->mData[1] == 91 && p->mData[2] == 65)
        {
            if (remoteClient->lastSentTextInput[0])
            {
                // Up arrow, return last string
                for (int i = 0; remoteClient->textInput[i]; i++)
                    remoteClient->textInput[i]=8;
                strcat(remoteClient->textInput, remoteClient->lastSentTextInput);
                mPrc->send((const char *)remoteClient->textInput, (Nui32) strlen(remoteClient->textInput), p->mAddress, false);
                strcpy(remoteClient->textInput,remoteClient->lastSentTextInput);
                remoteClient->cursorPosition=(Nui32) strlen(remoteClient->textInput);
            }

            return 0;
        }


        // 127 is delete - ignore that
        // 9 is tab
        // 27 is escape
        if (p->mData[0] >= 127 || p->mData[0] == 9 || p->mData[0] == 27)
        {
            N_delete p;
            return 0;
        }

        // Hack - I don't know what the hell this is about but cursor keys send 3 characters at a time.  I can block these
        //Up=27,91,65
        //Down=27,91,66
        //Right=27,91,67
        //Left=27,91,68
        if (p->mLength == 3 && p->mData[0] == 27 && p->mData[1] == 91 && p->mData[2] >= 65 && p->mData[2] <= 68)
        {
            N_delete p;
            return 0;
        }

        // Echo
    #ifdef ECHO_INPUT
        mPrc->send((const char *)p->mData, p->mLength, p->mAddress, false);
    #endif

        bool gotLine;
        // Process each character in turn
        for (i = 0; i < p->mLength; i++)
        {
    #ifdef ECHO_INPUT
            if (p->mData[i]==8)
            {
                char spaceThenBack[2];
                spaceThenBack[0]=' ';
                spaceThenBack[1]=8;
                mPrc->send((const char *)spaceThenBack, 2, p->mAddress, false);
            }
    #endif

            gotLine=ReassembleLine(remoteClient, p->mData[i]);
            if (gotLine && remoteClient->textInput[0])
            {
                SocketMessage * reassembledLine = (SocketMessage *)N_alloc(sizeof(SocketMessage));
                reassembledLine->mLength=(Nui32) strlen(remoteClient->textInput);
                memcpy(remoteClient->lastSentTextInput, remoteClient->textInput, reassembledLine->mLength+1);
                N_assert(reassembledLine->mLength < REMOTE_MAX_TEXT_INPUT);
                reassembledLine->mData= (Nui8 *) N_alloc(reassembledLine->mLength + 1);
                memcpy(reassembledLine->mData, remoteClient->textInput, reassembledLine->mLength);
    #ifdef _PRINTF_DEBUG
                memset(remoteClient->textInput, 0, REMOTE_MAX_TEXT_INPUT);
    #endif
                reassembledLine->mData[reassembledLine->mLength]=0;
                reassembledLine->mAddress = p->mAddress;
                N_delete p;
                return reassembledLine;
            }
        }

        N_delete p;
        return 0;
    }
    //------------------------------------------------------------------------
    Address * TelnetTransport::onServerConnect(Address * newConnection)
    {
        mPrc->onServerConnect(newConnection);
        // 03/16/06 Can't force the stupid windows telnet to use line mode or local echo so now I have to track all the remote players and their
        // input buffer
        if (newConnection && !newConnection->isInvalid())
        {
            Nui8 command[10];
            // http://www.pcmicro.com/netfoss/RFC857.html
            // IAC WON'T ECHO
            command[0] = 255; // IAC
            command[1] = 251; // WILL
            command[2] = 1; // ECHO
            mPrc->send((const char *)command, 3, newConnection, false);

            TelnetClient * remoteClient = 0;
            for(Nui32 i = 0; i < mRemoteList.Size(); ++i)
            {
                if(mRemoteList[i]->mAddress == *newConnection)
                {
                    remoteClient = mRemoteList[i];
                    remoteClient->cursorPosition = 0;
                }
            }

            if (remoteClient == 0)
            {
                remoteClient = new TelnetClient;
                remoteClient->lastSentTextInput[0] = 0;
                remoteClient->cursorPosition = 0;
                remoteClient->mAddress = newConnection;
    #ifdef _PRINTF_DEBUG
                memset(remoteClient->textInput, 0, REMOTE_MAX_TEXT_INPUT);
    #endif
            }

            mRemoteList.Insert(remoteClient);
        }
        return newConnection;
    }
    //------------------------------------------------------------------------
    void TelnetTransport::onConnectClose(Address * address)
    {
        mPrc->onConnectClose(address);
        if (address && !address->isInvalid())
        {
            for (Nui32 i = 0; i < mRemoteList.Size(); ++i)
            {
                if (mRemoteList[i]->mAddress == *address)
                {
                    N_delete mRemoteList[i];
                    mRemoteList[i]=mRemoteList[mRemoteList.Size()-1];
                    mRemoteList.RemoveFromEnd();
                }
            }
        }
        return address;
    }
    //------------------------------------------------------------------------
    CmdBase * TelnetTransport::GetCommandParser(void)
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void TelnetTransport::SetSendSuffix(const Nui8 * suffix)
    {
        if(sendSuffix)
        {
            N_free(sendSuffix);
            sendSuffix = 0;
        }
        if(suffix)
        {
            sendSuffix = N_alloc_t(Nui8, strlen(suffix) + 1);
            strcpy(sendSuffix, suffix);
        }
    }
    //------------------------------------------------------------------------
    void TelnetTransport::SetSendPrefix(const Nui8 * prefix)
    {
        if(sendPrefix)
        {
            N_free(sendPrefix);
            sendPrefix = 0;
        }
        if(prefix)
        {
            sendPrefix = N_alloc_t(Nui8, strlen(prefix) + 1);
            strcpy(sendPrefix, prefix);
        }
    }
    //------------------------------------------------------------------------
    void TelnetTransport::AutoAllocate(void)
    {
        if (mPrc == 0)
            mPrc = new NetworkObj;
    }
    //------------------------------------------------------------------------
    bool TelnetTransport::ReassembleLine(TelnetTransport::TelnetClient* remoteClient, Nui8 c)
    {
        if (c =='\n')
        {
            remoteClient->textInput[remoteClient->cursorPosition]=0;
            remoteClient->cursorPosition=0;
    #ifdef _PRINTF_DEBUG
            N_printf("[Done] %s\n", remoteClient->textInput);
    #endif
            return true;
        }
        else if (c == 8) // backspace
        {
            if (remoteClient->cursorPosition > 0)
            {
                remoteClient->textInput[--remoteClient->cursorPosition]=0;
    #ifdef _PRINTF_DEBUG
                N_printf("[Back] %s\n", remoteClient->textInput);
    #endif
            }
        }
        else if (c >= 32 && c < 127)
        {
            if (remoteClient->cursorPosition < REMOTE_MAX_TEXT_INPUT)
            {
                remoteClient->textInput[remoteClient->cursorPosition++] = c;
    #ifdef _PRINTF_DEBUG
                N_printf("[Norm] %s\n", remoteClient->textInput);
    #endif
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
}
}
#endif // _RAKNET_SUPPORT_*