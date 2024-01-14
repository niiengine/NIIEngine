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
#include "NiiEventManager.h"

namespace NII
{
    using namespace NII_COMMAND;
    //-----------------------------------------------------------------------
    template<> EventManager * Singleton<EventManager>::mOnly = 0;
    //------------------------------------------------------------------------
    EventManager::EventManager():
        mGlobalEnable(false)
    {

    }
    //------------------------------------------------------------------------
    EventManager::~EventManager()
    {

    }
    //------------------------------------------------------------------------
    void EventManager::processEvent()
    {

    }
    //------------------------------------------------------------------------
    void EventManager::add(EventObj * obj)
    {

    }
    //------------------------------------------------------------------------
    void EventManager::add(CommandObj * obj)
    {

    }
    //------------------------------------------------------------------------
    void EventManager::removeEvent(Nid id)
    {

    }
    //------------------------------------------------------------------------
    void EventManager::removeCommand(Nid id)
    {

    }
    //------------------------------------------------------------------------
    EventObj * EventManager::getEvent(Nid id)
    {
        EventObjectMap::iterator i = mEventObjects.find(id);
        if (i != mEventObjects.end())
            return i->second;
        return 0;
    }
    //------------------------------------------------------------------------
    CommandObj * EventManager::getCommand(Nid id)
    {
        CommandObjectMap::iterator i = mCommandObjects.find(id);
        if (i != mCommandObjects.end())
            return i->second;
        return 0;
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    void EventManager::findUnder(Nid pid, Nid cid)
    {

    }
    //------------------------------------------------------------------------
}