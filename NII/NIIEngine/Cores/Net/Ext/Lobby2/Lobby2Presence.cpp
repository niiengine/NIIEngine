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
#include "Lobby2Presence.h"
#include "NiiNetSerializer.h"

namespace NII
{
namespace NII_NET
{
    Lobby2Presence::Lobby2Presence()
    {
        status = UNDEFINED;
        isVisible = true;
    }

    Lobby2Presence::Lobby2Presence(const Lobby2Presence & input)
    {
        status = input.status;
        isVisible = input.isVisible;
        titleNameOrID = input.titleNameOrID;
        statusString = input.statusString;
    }

    Lobby2Presence& Lobby2Presence::operator = (const Lobby2Presence & input)
    {
        status = input.status;
        isVisible = input.isVisible;
        titleNameOrID = input.titleNameOrID;
        statusString = input.statusString;
        return *this;
    }

    Lobby2Presence::~Lobby2Presence()
    {

    }

    void Lobby2Presence::write(NetSerializer * in)
    {
        unsigned char gs = (unsigned char) status;
        in->read(gs);
        status = (Status) gs;
        in->read(isVisible);
        in->read(titleNameOrID);
        in->read(statusString);
    }

    void Lobby2Presence::read(NetSerializer * out)
    {
        unsigned char gs = (unsigned char) status;
        out->write(gs);
        status = (Status) gs;
        out->write(isVisible);
        out->write(titleNameOrID);
        out->write(statusString);
    }
}
}