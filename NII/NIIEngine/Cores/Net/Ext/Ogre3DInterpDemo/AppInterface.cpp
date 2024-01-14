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
#include "AppInterface.h"
#include "N_assert.h"
#include "FSM.h"
#include "RunnableState.h"

#ifdef _CONSOLE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if defined(__GNUC__)
#define _vsnprintf vsnprintf
#endif
#endif

AppInterface::AppInterface()
{
	hasFocus=false;
	primaryFSM=0;
	primaryStates=0;
	quit=false;
	primaryStatesLength=0;
	lastElapsedTimeMS=0;
	lastCurTimeMS=0;
}
AppInterface::~AppInterface()
{


}
void AppInterface::PreConfigure(void)
{
	primaryFSM = new FSM;
}
void AppInterface::PostConfigure(const char *defaultResourceConfigurationPath)
{
}
void AppInterface::update(AppTime curTimeMS,AppTime elapsedTimeMS)
{
	lastCurTimeMS=curTimeMS;
	lastElapsedTimeMS=elapsedTimeMS;
}
void AppInterface::OnAppShutdown(void)
{
	delete primaryFSM;
	if (primaryStates)
	{
		int i;
		for (i=0; i < primaryStatesLength; i++)
			delete primaryStates[i];
		delete [] primaryStates;
	}
}
void AppInterface::DebugOut(unsigned int lifetimeMS, const char *format, ...)
{
#ifdef _CONSOLE
	char text[8096];
	va_list ap;
	va_start(ap, format);
	_vsnprintf(text, 8096-1, format, ap);
	va_end(ap);
	strcat(text, "\n");
	text[8096-1]=0;
	printf(text);
#else
	// Don't call this without an implementation.  Perhaps you meant to use MainApp() instead
	N_assert(0);
#endif
}
bool AppInterface::HasFocus(void) const
{
	return hasFocus;
}
void AppInterface::SetFocus(bool hasFocus)
{
	if (this->hasFocus!=hasFocus)
	{
		if (primaryFSM->CurrentState())
			((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(hasFocus);
		this->hasFocus=hasFocus;
	}
}
void AppInterface::PushState(RunnableState* state)
{
	if (hasFocus && primaryFSM->CurrentState())
		((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(false);
	primaryFSM->AddState(state);
	if (hasFocus)
		((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(true);
	else
		((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(false);
}
void AppInterface::PushState(int stateType)
{
	PushState(primaryStates[stateType]);
}
void AppInterface::PopState(int popCount)
{
	N_assert(popCount>=1);
	N_assert(primaryFSM->GetStateHistorySize()>=1+popCount);
	if (hasFocus && primaryFSM->CurrentState())
		((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(false);
	primaryFSM->SetPriorState(primaryFSM->GetStateHistorySize()-1-popCount);
	if (hasFocus)
		((RunnableState*)(primaryFSM->CurrentState()))->SetFocus(true);
}
RunnableState* AppInterface::GetCurrentState(void) const
{
	return (RunnableState*) primaryFSM->CurrentState();
}
int AppInterface::GetStateHistorySize(void) const
{
	return primaryFSM->GetStateHistorySize();
}
void AppInterface::AllocateStates(int numStates)
{
	N_assert(primaryStates==0);
	primaryStates = new RunnableState*[numStates];

	primaryStatesLength=numStates;
}
void AppInterface::SetState(int stateType, RunnableState* state)
{
	primaryStates[stateType] = state;
	state->SetParentApp(this);
	state->OnStateReady();
}
RunnableState* AppInterface::GetState(int stateType) const
{
	return primaryStates[stateType];
}
bool AppInterface::ShouldQuit(void) const
{
	return quit;
}
void AppInterface::Quit(void)
{
	quit=true;
}
AppTime AppInterface::GetLastCurrentTime(void) const
{
	return lastCurTimeMS;
}
AppTime AppInterface::GetLastElapsedTime(void) const
{
	return lastElapsedTimeMS;
}
