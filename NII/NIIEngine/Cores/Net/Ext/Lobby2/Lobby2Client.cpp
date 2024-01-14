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
#include "Lobby2Client.h"
#include "N_assert.h"
#include "NiiNetCommon.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    Lobby2Client::Lobby2Client()
    {
        serverAddress = 0;
    }
    //------------------------------------------------------------------------
    Lobby2Client::~Lobby2Client()
    {

    }
    //------------------------------------------------------------------------
    void Lobby2Client::SetServerAddress(Address * addr)
    {
        serverAddress=addr;
    }
    //------------------------------------------------------------------------
    Address * Lobby2Client::GetServerAddress(void) const
    {
        return serverAddress;
    }
    //------------------------------------------------------------------------
    void Lobby2Client::SendMsg(Lobby2Message * msg)
    {
        // Callback must be ready to receive reply
        N_assert(callbacks.Size());
        msg->resultCode = L2RC_PROCESSING;

        NetSerializer bs;
        bs.write((Token)ID_LOBBY2_SEND_MESSAGE);
        bs.write((Token)msg->GetID());
        msg->read(false,&bs);
        send(&bs, packetPriority, RELIABLE_ORDERED, mPacketCh, serverAddress, false);
    }
    //------------------------------------------------------------------------
    void Lobby2Client::SendMsgAndDealloc(Lobby2Message * msg)
    {
        SendMsg(msg);
        msgFactory->Dealloc(msg);
    }
    //------------------------------------------------------------------------
    ConnectReceiveType Lobby2Client::onMessage(SocketMessage * msg)
    {
        N_assert(msg);

        switch (msg->data[0])
        {
        case ID_LOBBY2_SEND_MESSAGE:
            OnMessage1(msg);
            return CRT_Destroy;
        }

        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void Lobby2Client::onConnectClose(const Address & address, UniqueID id, ConnectCloseType lostConnectionReason )
    {
        (void)address;
        (void)id;
        (void)lostConnectionReason;
    }
    //------------------------------------------------------------------------
    void Lobby2Client::shutdown(void)
    {
    //	ClearIgnoreList();
    }
    //------------------------------------------------------------------------
    void Lobby2Client::OnMessage1(SocketMessage * msg)
    {
        NetSerializer bs(msg->data, msg->length, false);
        bs.skipRead(1, 0); // ID_LOBBY2_SEND_MESSAGE
        Token msgId;
        bs.read(msgId);
        Lobby2MessageID lobby2MessageID = (Lobby2MessageID) msgId;
        Lobby2Message *lobby2Message = msgFactory->Alloc(lobby2MessageID);
        if (lobby2Message)
        {
            lobby2Message->write(true, &bs);
            if (lobby2Message->ClientImpl(this))
            {
                for (unsigned long i=0; i < callbacks.Size(); i++)
                {
                    if (lobby2Message->callbackId==(uint32_t)-1 || lobby2Message->callbackId==callbacks[i]->callbackId)
                        lobby2Message->CallCallback(callbacks[i]);
                }
            }
            msgFactory->Dealloc(lobby2Message);
        }
        else
        {
            N_assert("Lobby2Client::OnMessage1 lobby2Message==0" && 0);
        }
    }
    //------------------------------------------------------------------------
}
}
/*
void Lobby2Client::AddToIgnoreList(RakString user)
{
	ignoreList.Insert(user,user,false);
}
void Lobby2Client::RemoveFromIgnoreList(RakString user)
{
	ignoreList.RemoveIfExists(user);
}
void Lobby2Client::SetIgnoreList(DataStructures::List<RakString> users)
{
	for (unsigned int i=0; i < users.Size(); i++)
		ignoreList.Insert(users[i],users[i],false);
}
bool Lobby2Client::IsInIgnoreList(RakString user) const
{
	return ignoreList.HasData(user);
}
void Lobby2Client::ClearIgnoreList(void)
{
	ignoreList.Clear();
}
const DataStructures::OrderedList<RakString, RakString>* Lobby2Client::GetIgnoreList(void) const
{
	return &ignoreList;
}
*/