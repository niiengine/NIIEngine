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
#ifndef _EVENTCONTAINER_H_
#define _EVENTCONTAINER_H_

#include <map>

#include <boost/function.hpp>
#include <boost/signal.hpp>

class EventArgs;

typedef boost::function<void (EventArgs&)> EventHandler;
typedef boost::signal<void (EventArgs&)> Delegate;
typedef std::map<int, Delegate*> DelegateMap;


class EventContainer
{
public:
	EventContainer();
	virtual ~EventContainer();

	void subscribe(int eventId, EventHandler handler);
	void unsubscribe(int eventId, EventHandler handler);
	
protected:
	void registerEvent(int eventId);
	void fireEvent(int eventId, EventArgs& args);
	
private:
	DelegateMap mDelegates;
};

#endif // _EVENTCONTAINER_H_