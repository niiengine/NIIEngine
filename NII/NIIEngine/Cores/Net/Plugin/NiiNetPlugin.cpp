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
#include "NiiNetPlugin.h"
#include "NetworkObj.h"
#include "NiiNetLinkBase.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    Plugin::Plugin()
    {
        mPrc2 = 0;
        mPrc = 0;
    }
    //------------------------------------------------------------------------
    Plugin::~Plugin()
    {

    }
    //------------------------------------------------------------------------
    void Plugin::onConfirm(const Address * address, const UniqueID & id)
    {
        onConnect(address, id);
    }
    //------------------------------------------------------------------------
    void Plugin::onAttach()
    {
    }
    //------------------------------------------------------------------------
    void Plugin::onDetach()
    {
    }
    //------------------------------------------------------------------------
    void Plugin::update()
    {
    }
    //------------------------------------------------------------------------
    ConnectReceiveType Plugin::onMessage(SocketMessage *)
    {
        return CRT_Normal;
    }
    //------------------------------------------------------------------------
    void Plugin::send(const Nui8 * data, NCount size, PacketLevel plevel, PacketType ptype, PacketCH pch, 
        const UniqueAddress & usys, bool broadcast)
    {
        if(mPrc2)
        {
            mPrc2->send(data, size, plevel, ptype, pch, usys, broadcast);
            return;
        }
        else if(mPrc)
        {
            mPrc->send(data, size, usys.mAddress, broadcast);
            return;
        }

        // Offline mode
        if(broadcast == false && usys.mID == getID())
        {
            SocketMessage msg;
            msg.mBitCount = N_b_TO_B(size);
            msg.mData = data;
            msg.mAutoDestroy = false;
            msg.mID = UniqueID::INVALID;
            msg.mLength = size;
            msg.mAddress = 0;
            msg.mLocal = false;
            onMessage(&msg);
            update();
        }
    }
    //------------------------------------------------------------------------
    bool Plugin::send(const char ** data, const int * lengths, int pcount, PacketLevel plevel, PacketType ptype,
        PacketCH pch, const UniqueAddress & usys, bool broadcast)
    {
        if(mPrc2)
        {
            return mPrc2->send(data, lengths, pcount, plevel, ptype, pch, usys, broadcast) != 0;
        }
        else if(mPrc)
        {
            return mPrc->send(data, (const Nui32 *)lengths, pcount, usys.mAddress, broadcast);
        }

        if(broadcast == false && usys.mID == getID())
        {
            Nui32 totalLength = 0;
            Nui32 lengthOffset;
            NCount i;
            for(i = 0; i < pcount; ++i)
            {
                if(lengths[i] > 0)
                    totalLength += lengths[i];
            }
            if (totalLength == 0)
                return false;

            char * dataAggregate = (char *)N_alloc((size_t)totalLength);
            if(dataAggregate == 0)
            {
                return false;
            }
            for(i = 0, lengthOffset = 0; i < pcount; ++i)
            {
                if(lengths[i] > 0)
                {
                    memcpy(dataAggregate + lengthOffset, data[i], lengths[i]);
                    lengthOffset += lengths[i];
                }
            }

            send(dataAggregate, totalLength, plevel, ptype, pch, usys, broadcast);
            N_free(dataAggregate);
            return true;
        }

        return false;
    }
    //------------------------------------------------------------------------
    SocketMessage * Plugin::createMessage(NCount size)
    {
        if(mPrc)
        {
            return mPrc->createMessage(size);
        }
        SocketMessage * msg = N_new SocketMessage(size);
        return msg;
    }
    //------------------------------------------------------------------------
    void Plugin::addMessage(SocketMessage * msg, bool head)
    {
        if (mPrc2)
        {
            mPrc2->addMessage(msg, head);
            return;
        }
        else if(mPrc)
        {
            mPrc->addMessage(msg, head);
            return;
        }

        onMessage(msg);
        update();
    }
    //------------------------------------------------------------------------
    void Plugin::setPrc(NetworkPrc * prc)
    {
        mPrc = prc;
    }
    //------------------------------------------------------------------------
    UniqueID Plugin::getID() const
    {
        if(mPrc2)
            return mPrc2->getGuid();
        return UniqueID::INVALID;
    }
    //------------------------------------------------------------------------
}
}