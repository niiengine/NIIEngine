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
#include "FSM.h"
#include "State.h"
#include "N_assert.h"



FSM::FSM()
{

}
FSM::~FSM()
{
	Clear();
}
void FSM::Clear(void)
{
	unsigned i;
	if (stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnLeave(this, true);
	for (i=0; i < stateHistory.Size(); i++)
		stateHistory[i]->FSMRemoveRef(this);
	stateHistory.Clear(false);
}
State *FSM::CurrentState(void) const
{
	if (stateHistory.Size()==0)
		return 0;
	return stateHistory[stateHistory.Size()-1];
}
State *FSM::GetState(int index) const
{
	N_assert(index>=0 && index < (int) stateHistory.Size());
	return stateHistory[(unsigned) index];
}
int FSM::GetStateIndex(State *state) const
{
	return (int) stateHistory.GetIndexOf(state);
}
int FSM::GetStateHistorySize(void) const
{
	return stateHistory.Size();
}
void FSM::RemoveState(const int index)
{
	N_assert(index>=0 && index < (int) stateHistory.Size());
	if (index==stateHistory.Size()-1)
		stateHistory[index]->OnLeave(this, true);
	stateHistory[index]->FSMRemoveRef(this);
	stateHistory.RemoveAtIndex((const unsigned int)index);
	if (index==stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnEnter(this, false);
}
void FSM::AddState(State *state)
{
	if (stateHistory.Size())
		stateHistory[stateHistory.Size()-1]->OnLeave(this, false);
	state->FSMAddRef(this);
	state->OnEnter(this, true);
	stateHistory.Insert(state);
}
void FSM::ReplaceState(const int index, State *state)
{
	N_assert(index>=0 && index < (int) stateHistory.Size());
	if (state!=stateHistory[index])
	{
		if (index==stateHistory.Size()-1)
			stateHistory[index]->OnLeave(this, true);
		stateHistory[index]->FSMRemoveRef(this);
		state->FSMAddRef(this);
		if (index==stateHistory.Size()-1)
			state->OnEnter(this, true);
		stateHistory[index]=state;
	}
}
void FSM::SetPriorState(const int index)
{
	N_assert(index>=0 && index < (int) stateHistory.Size());
	stateHistory[stateHistory.Size()-1]->OnLeave(this, true);
	for (unsigned i=stateHistory.Size()-1; i > (unsigned) index; --i)
	{
		stateHistory[i]->FSMRemoveRef(this);
		stateHistory.RemoveFromEnd();
	}
	stateHistory[index]->OnEnter(this, false);
}
