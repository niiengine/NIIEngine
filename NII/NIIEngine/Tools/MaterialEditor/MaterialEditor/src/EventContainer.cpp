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
#include "EventContainer.h"

#include "EventArgs.h"

EventContainer::EventContainer()
{
}

EventContainer::~EventContainer()
{
}

void EventContainer::subscribe(int eventId, EventHandler handler)
{
	DelegateMap::iterator it = mDelegates.find(eventId);
	it->second->connect(handler);
}

// TODO: Implement unsubscribe
void EventContainer::unsubscribe(int eventId,EventHandler handler)
{
	DelegateMap::iterator it = mDelegates.find(eventId);
	//it->second->disconnect(handler);
}

// Should we throw an exception on duplicate event IDs?
void EventContainer::registerEvent(int eventId)
{
	mDelegates[eventId] = new Delegate();
}

void EventContainer::fireEvent(int eventId, EventArgs& args)
{
	DelegateMap::iterator it = mDelegates.find(eventId);
	(*(it->second))(args);
}