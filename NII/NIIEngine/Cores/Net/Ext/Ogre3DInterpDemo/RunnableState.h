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
#ifndef __RUNNABLE_STATE_H
#define __RUNNABLE_STATE_H

#include "AppTypes.h"
#include "State.h"

class AppInterface;

class RunnableState : public State
{
public:
	virtual void update(AppTime curTimeMS, AppTime elapsedTimeMS)=0;
	virtual void UpdateEnd(AppTime curTimeMS, AppTime elapsedTimeMS) {};
	virtual void Render(AppTime curTimeMS)=0;
	void SetParentApp(AppInterface *parent);
	// Call when internal variables set and the state is ready to use
	virtual void OnStateReady(void);

	// Don't do this so I can override by return type in derived classes
	//virtual AppInterface *GetParentApp(void) const;

	virtual void SetFocus(bool hasFocus);

protected:
	AppInterface *parent;

};


#endif
