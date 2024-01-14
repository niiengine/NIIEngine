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

#include "NiiPreProcess.h"
#include "NiiNetWinRTSocket.h"

#include <ppltasks.h>
#include <collection.h>
#include "RakString.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    public ref class OutputStreamAndDataWriter sealed
    {
    public:
        // http://msdn.microsoft.com/en-us/library/windows/apps/hh755807.aspx
         property IOutputStream ^outputStream;
         property DataWriter ^dataWriter;
    private:
    };
    //------------------------------------------------------------------------
    public ref class ListenerContext sealed
    {
    public:
        ListenerContext(Windows::Networking::Sockets::DatagramSocket^ listener);
        void OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket,
            Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
        OutputStreamAndDataWriter ^GetOutputStreamAndDataWriter(Nui64 s_addr);
    private:
        ~ListenerContext();

        ThreadMutex outputStreamMapMutex;
        Windows::Networking::Sockets::DatagramSocket^ listener;
        // http://msdn.microsoft.com/en-us/library/windows/apps/hh700103.aspx
        Platform::Collections::Map<Nui64,OutputStreamAndDataWriter^>^ outputStreamMap;

        void EchoMessage(Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments);
    };
    //------------------------------------------------------------------------
    ListenerContext::ListenerContext(Windows::Networking::Sockets::DatagramSocket^ listener)
    {
        this->listener = listener;
        outputStreamMap = ref new Platform::Collections::Map<Nui64,OutputStreamAndDataWriter^>();
    }
    //------------------------------------------------------------------------
    ListenerContext::~ListenerContext()
    {
        // The listener can be closed in two ways:
        //  - explicit: by using delete operator (the listener is closed even if there are outstanding references to it).
        //  - implicit: by removing last reference to it (i.e. falling out-of-scope).
        // In this case this is the last reference to the listener so both will yield the same result.
        delete listener;
        listener = nullptr;
    }
    //------------------------------------------------------------------------
    OutputStreamAndDataWriter ^ListenerContext::GetOutputStreamAndDataWriter(Nui64 s_addr)
    {
        outputStreamMapMutex.Lock();
        if (outputStreamMap->HasKey(s_addr))
        {
            OutputStreamAndDataWriter ^o = outputStreamMap->Lookup(s_addr);
            outputStreamMapMutex.Unlock();
            return o;
        }
        outputStreamMapMutex.Unlock();
        Nui16 port = s_addr & 0xFFFF;
        Nui32 addr = (Nui32) (s_addr >> 32);
        //addr = ntohl(addr);
        char buf[64];
        Nui8 * ucp = (Nui8 *)&addr;
        sprintf(buf, "%d.%d.%d.%d", ucp[0] & 0xff, ucp[1] & 0xff, ucp[2] & 0xff, ucp[3] & 0xff);
        char portStr[32];
        _itoa(port, portStr, 10);

        RakString rs1(buf);
        WCHAR * w_char1 = rs1.ToWideChar();
        HostName ^hostName = ref new HostName(ref new Platform::String(w_char1));
        RakString rs2(portStr);
        WCHAR * w_char2 = rs2.ToWideChar();
        task< IOutputStream^ > op(listener->GetOutputStreamAsync(hostName, ref new Platform::String(w_char2)));
        op.wait();
        OutputStreamAndDataWriter ^outputStreamAndDataWriter = ref new OutputStreamAndDataWriter;
        outputStreamAndDataWriter->outputStream = op.get();
        outputStreamAndDataWriter->dataWriter = ref new DataWriter(outputStreamAndDataWriter->outputStream);

        rs1.DeallocWideChar(w_char1);
        rs2.DeallocWideChar(w_char2);

        outputStreamMapMutex.Lock();
        if(outputStreamMap->HasKey(s_addr) == false)
        {
            outputStreamMap->Insert(s_addr, outputStreamAndDataWriter);
            outputStreamMapMutex.Unlock();
            return outputStreamAndDataWriter;
        }
        else
        {
            // Just use the one that was inserted from another thread
            OutputStreamAndDataWriter ^o = outputStreamMap->Lookup(s_addr);
            outputStreamMapMutex.Unlock();
            return o;
        }
    }
    //------------------------------------------------------------------------
    void ListenerContext::OnMessage(Windows::Networking::Sockets::DatagramSocket^ socket,
        Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs^ eventArguments)
    {
        HostName ^remoteHost = eventArguments->RemoteAddress;
        eventArguments->RemoteAddress->DisplayName;
        eventArguments->RemotePort;
        SocketMessageData * recv = NULL;
        WinStore8Socket * rns2 = WinStore8Socket::GetRNS2FromDatagramSocket(socket);
        if(rns2 == 0)
            return;

        RNS2EventHandler * eventHandler = rns2->GetEventHandler();
        eventHandler->alloc(&recv);

        // http://stackoverflow.com/questions/11853838/getting-an-array-of-bytes-out-of-windowsstoragestreamsibuffer
        IBuffer ^uselessBuffer = eventArguments->GetDataReader()->DetachBuffer();
        Windows::Storage::Streams::DataReader^ uselessReader =  Windows::Storage::Streams::DataReader::FromBuffer(uselessBuffer);
        Platform::Array<Nui8>^ managedBytes = ref new Platform::Array<Nui8>(uselessBuffer->Length);
        uselessReader->ReadBytes(managedBytes);

        for(Nui32 i = 0; i < uselessBuffer->Length; ++i)
            recv->mData[i] = managedBytes[i];

        recv->mCount = uselessBuffer->Length;
        char ip[64];
        RakString rs2;
        rs2.FromWideChar(eventArguments->RemoteAddress->DisplayName->Data());
        strcpy(ip, rs2.C_String());
        recv->mAddress.mSrc.sin_addr.s_addr = WinStore8Socket::inetAddr(ip);
        char portStr[64];
        rs2.FromWideChar(eventArguments->RemotePort->Data());
        strcpy(portStr, rs2.C_String());
        recv->mAddress.setHSBPort(atoi(portStr));
        recv->mRead = N_Engine().getTimer().getUS();
        recv->mSocket = rns2;
        eventHandler->receive(recv);
    }
    //------------------------------------------------------------------------
    void ListenerContext::EchoMessage(DatagramSocketMessageReceivedEventArgs^ eventArguments)
    {
    }
    //------------------------------------------------------------------------
    DataStructures::List<WinStore8Socket *> WinStore8Socket::rns2List;
    ThreadMutex WinStore8Socket::rns2ListMutex;
    //------------------------------------------------------------------------
    WinStore8Socket::WinStore8Socket()
    {
        rns2ListMutex.Lock();
        rns2List.Insert(this);
        rns2ListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    WinStore8Socket::~WinStore8Socket()
    {
        Nui32 i;
        rns2ListMutex.Lock();
        for(i = 0; i < rns2List.Size(); ++i)
        {
            if(rns2List[i] == this)
            {
                rns2List.RemoveAtIndexFast(i);
                break;
            }
        }
        rns2ListMutex.Unlock();
    }
    //------------------------------------------------------------------------
    WinStore8Socket * WinStore8Socket::GetRNS2FromDatagramSocket(
        Windows::Networking::Sockets::DatagramSocket^ s)
    {
        WinStore8Socket * out = 0;
        Nui32 i;
        rns2ListMutex.Lock();
        for(i = 0; i < rns2List.Size(); ++i)
        {
            if(rns2List[i]->listener == s)
            {
                out = rns2List[i];
                break;
            }
        }
        rns2ListMutex.Unlock();
        return out;
    }
    //------------------------------------------------------------------------
    RNS2BindResult WinStore8Socket::Bind(Platform::String ^localServiceName)
    {
        listener = ref new DatagramSocket();
        listenerContext = ref new ListenerContext(listener);
        listener->MessageReceived += ref new TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>(listenerContext, &ListenerContext::OnMessage);
        // http://msdn.microsoft.com/en-us/library/dd492427.aspx
        task<void> bindOp(listener->BindServiceNameAsync(localServiceName));
        try
        {
            bindOp.wait();
        }
        catch (Exception^ exception)
        {
            return BR_FAILED_TO_BIND_SOCKET;
        }

        return BR_SUCCESS;
    }
    //------------------------------------------------------------------------
    RNS2SendResult WinStore8Socket::send(SocketSendData * param)
    {
        Nui64 s_addr = param->mAddress.mSrc.sin_addr.s_addr;
        s_addr <<= 32;
        s_addr |= param->mAddress.getHSBPort();
        OutputStreamAndDataWriter ^outputStreamAndDataWriter = listenerContext->GetOutputStreamAndDataWriter(s_addr);
        // DataWriter ^dataWriter = ref new DataWriter(outputStream);

        auto platformBuffer = ref new Platform::Array<BYTE>(param->mData, (UINT)param->mLength);
        outputStreamAndDataWriter->dataWriter->WriteBytes(platformBuffer);


         // Write the locally buffered data to the network. Please note that write operation will succeed
        // even if the server is not listening.
        task<Nui32>(outputStreamAndDataWriter->dataWriter->StoreAsync()).
            then([&] (task<Nui32> writeTask)
            {
                try
                {

                }
                catch(Exception^ exception)
                {
                    OutputDebugStringW(exception->SocketMessage->Data());
                }
            });

        return 0;
    }
    //------------------------------------------------------------------------
    int WinStore8Socket::inetAddr(const char * str)
    {
        int parts[4];
        Nui8 curVal;

        const char * strIndex=str;
        int partsIndex;

        for(partsIndex = 0; partsIndex < 4; ++partsIndex)
            parts[partsIndex] = 0;

        partsIndex = 0;
        curVal = 0;
        while(partsIndex < 4)
        {
            if(*strIndex < '0' || *strIndex > '9')
            {
                parts[partsIndex] = curVal;
                partsIndex ++;
                curVal = 0;

                if(*strIndex == 0)
                    break;
            }
            else
            {
                curVal* = 10;
                curVal += *strIndex - '0';
            }
            strIndex++;
        }

        return htonl((parts[0]<<24) | (parts[1]<<16) | (parts[2]<<8) | (parts[3]<<0) );
    }
    //------------------------------------------------------------------------
    int WinStore8Socket::setSockOpt(Windows::Networking::Sockets::DatagramSocket ^s,
       int level, int optname, const char * optval, socklen_t optlen)
    {
        N_assert("不支持" && 0);
        return 0;
    }
    //------------------------------------------------------------------------
    int WinStore8Socket::ioctlSocket(Windows::Networking::Sockets::DatagramSocket ^s,
        NIIl cmd, Nul * argp)
    {
        N_assert("不支持" && 0);
        return 0;
    }
    //------------------------------------------------------------------------
    int WinStore8Socket::getSockName(Windows::Networking::Sockets::DatagramSocket ^s,
        struct sockaddr * name, socklen_t * len)
    {
        N_assert("不支持" && 0);
        return 0;
    }
    //------------------------------------------------------------------------
}
}