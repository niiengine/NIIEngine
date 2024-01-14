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
#include "NiiNetSocket.h"

namespace NII
{
    //------------------------------------------------------------------------
    SocketObj::SocketObj()
    {
        eventHandler = 0;
    }
    //------------------------------------------------------------------------
    SocketObj::~SocketObj()
    {
    }
    //------------------------------------------------------------------------
    RNS2Type SocketObj::GetSocketType() const
    {
        return socketType;
    }
    //------------------------------------------------------------------------
    void SocketObj::SetSocketType(RNS2Type t)
    {
        socketType = t;
    }
    //------------------------------------------------------------------------
    void SocketObj::SetRecvEventHandler(RNS2EventHandler * handler)
    {
        eventHandler = handler;
    }
    //------------------------------------------------------------------------
    RNS2EventHandler * SocketObj::GetEventHandler() const
    {
        return eventHandler;
    }
    //------------------------------------------------------------------------
    bool SocketObj::IsBerkleySocket() const
    {
        return socketType != RNS2T_CHROME && socketType != RNS2T_WINDOWS_STORE_8;
    }
    //------------------------------------------------------------------------
    const Address * SocketObj::GetBoundAddress() const
    {
        return boundAddress;
    }
    //------------------------------------------------------------------------
}