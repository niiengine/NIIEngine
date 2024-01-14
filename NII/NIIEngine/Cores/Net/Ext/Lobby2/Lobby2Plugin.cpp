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
#include "Lobby2Plugin.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    Lobby2Plugin::Lobby2Plugin()
    {
        mPacketCh=0;
        packetPriority=HIGH_PRIORITY;
    }
    //------------------------------------------------------------------------
    Lobby2Plugin::~Lobby2Plugin()
    {

    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::SetOrderingChannel(char oc)
    {
        mPacketCh = oc;
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::SetSendPriority(PacketLevel pp)
    {
        packetPriority = pp;
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::SetMessageFactory(Lobby2MessageFactory *f)
    {
        msgFactory = f;
    }
    //------------------------------------------------------------------------
    Lobby2MessageFactory * Lobby2Plugin::GetMessageFactory(void) const
    {
        return msgFactory;
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::SetCallbackInterface(Lobby2Callbacks * cb)
    {
        ClearCallbackInterfaces();
        callbacks.Insert(cb);
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::AddCallbackInterface(Lobby2Callbacks * cb)
    {
        RemoveCallbackInterface(cb);
        callbacks.Insert(cb);
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::RemoveCallbackInterface(Lobby2Callbacks * cb)
    {
        unsigned long index = callbacks.GetIndexOf(cb);
        if (index!=MAX_UNSIGNED_LONG)
            callbacks.RemoveAtIndex(index);
    }
    //------------------------------------------------------------------------
    void Lobby2Plugin::ClearCallbackInterfaces()
    {
        callbacks.Clear(false);
    }
    //------------------------------------------------------------------------
}
}