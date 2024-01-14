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
#ifndef __APP_INTERFACE_H
#define __APP_INTERFACE_H

#include "AppTypes.h"


class FSM;
class RunnableState;
class AppInterface
{
public:
	AppInterface();
	virtual ~AppInterface();
	// Called first to initialize memory
	virtual void PreConfigure(void);
	// Called after LoadDefaultResources.  Return false to quit.
	virtual bool Configure(void)=0;
	// Called to do startup required after Configuration
	virtual void PostConfigure(const char *defaultResourceConfigurationPath);
	virtual void update(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual void OnAppShutdown(void);

	// Logging, lifetime may be ignored
	virtual void DebugOut(unsigned int lifetimeMS, const char *format, ...);

	// Like focus in windows - start and stop rendering but don't necessarily stop running.
	bool HasFocus(void) const;
	virtual void SetFocus(bool hasFocus);

	// Built-in state machine
	void AllocateStates(int numStates);
	virtual void SetState(int stateType, RunnableState* state);
	RunnableState* GetState(int stateType) const;
	RunnableState* GetCurrentState(void) const;
	void PushState(RunnableState* state);
	void PushState(int stateType);
	void PopState(int popCount=1);
	int GetStateHistorySize(void) const;

	// Just setting a flag
	virtual bool ShouldQuit(void) const;
	void Quit(void);
	AppTime GetLastCurrentTime(void) const;
	AppTime GetLastElapsedTime(void) const;

protected:
	// Allocated in PostConfigure
	FSM *primaryFSM;
	// Up to the derived class to allocate this using allocateStates
	RunnableState **primaryStates;
	AppTime lastElapsedTimeMS, lastCurTimeMS;
	int primaryStatesLength;
	bool hasFocus;
	bool quit;
};

#endif
