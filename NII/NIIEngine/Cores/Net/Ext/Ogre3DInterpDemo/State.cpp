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
#include "N_assert.h"
#include "State.h"



State::State()
{
	fsmRefCount=0;
}
State::~State()
{

}
void State::OnEnter(const FSM *caller, bool loadResources)
{

}
void State::OnLeave(const FSM *caller, bool unloadResources)
{
	
}
void State::FSMAddRef(const FSM *caller)
{
	++fsmRefCount;
}
void State::FSMRemoveRef(const FSM *caller)
{
	N_assert(fsmRefCount!=0);
	--fsmRefCount;
}
unsigned State::FSMRefCount(void) const
{
	return fsmRefCount;
}
void ManagedState::FSMRemoveRef(const FSM *caller)
{
	N_assert(fsmRefCount!=0);
	if (--fsmRefCount)
		delete this;
}
