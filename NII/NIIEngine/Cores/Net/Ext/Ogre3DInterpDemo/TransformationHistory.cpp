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
#include "RakMemoryOverride.h"
#include "TransformationHistory.h"

TransformationHistoryCell::TransformationHistoryCell()
{

}
TransformationHistoryCell::TransformationHistoryCell(TimeDurMS t, const Ogre::Vector3& pos, const Ogre::Vector3& vel, const Ogre::Quaternion& quat  ) :
time(t),
velocity(vel),
position(pos),
orientation(quat)
{
}

void TransformationHistory::Init(TimeDurMS maxWriteInterval, TimeDurMS maxHistoryTime)
{
	writeInterval=maxWriteInterval;
	maxHistoryLength = maxHistoryTime/maxWriteInterval + 1;
	history.ClearAndForceAllocation(maxHistoryLength + 1);
	N_assert(writeInterval>0);
}
void TransformationHistory::write(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, TimeDurMS curTimeMS)
{
	if (history.Size()==0)
	{
		history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation));
	}
	else
	{
		const TransformationHistoryCell &lastCell = history.PeekTail();
		if (curTimeMS-lastCell.time>=writeInterval)
		{
			history.Push(TransformationHistoryCell(curTimeMS,position,velocity,orientation));
			if (history.Size()>maxHistoryLength)
				history.Pop();
		}
	}	
}
void TransformationHistory::Overwrite(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, TimeDurMS when)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		history.Push(TransformationHistoryCell(when,position,velocity,orientation));
	}
	else
	{
		// Find the history matching this time, and change the values.
		int i;
		for (i=historySize-1; i>=0; i--)
		{
			TransformationHistoryCell &cell = history[i];
			if (when >= cell.time)
			{
				if (i==historySize-1 && when-cell.time>=writeInterval)
				{
					// Not an overwrite at all, but a new cell
					history.Push(TransformationHistoryCell(when,position,velocity,orientation));
					if (history.Size()>maxHistoryLength)
						history.Pop();
					return;
				}

				cell.time=when;
				cell.position=position;
				cell.velocity=velocity;
				cell.orientation=orientation;
				return;
			}
		}
	}	
}
TransformationHistory::ReadResult TransformationHistory::read(Ogre::Vector3 *position, Ogre::Vector3 *velocity, Ogre::Quaternion *orientation,
								 TimeDurMS when, TimeDurMS curTime)
{
	int historySize = history.Size();
	if (historySize==0)
	{
		return VALUES_UNCHANGED;
	}

	int i;
	for (i=historySize-1; i>=0; i--)
	{
		const TransformationHistoryCell &cell = history[i];
		if (when >= cell.time)
		{
			if (i==historySize-1)
			{
				if (curTime<=cell.time)
					return VALUES_UNCHANGED;

				NIIf divisor = (NIIf)(curTime-cell.time);
				N_assert(divisor!=0.0f);
				NIIf lerp = (NIIf)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (*position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (*velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, *orientation,true);
			}
			else
			{
				const TransformationHistoryCell &nextCell = history[i+1];
				NIIf divisor = (NIIf)(nextCell.time-cell.time);
				N_assert(divisor!=0.0f);
				NIIf lerp = (NIIf)(when - cell.time) / divisor;
				if (position)
					*position=cell.position + (nextCell.position-cell.position) * lerp;
				if (velocity)
					*velocity=cell.velocity + (nextCell.velocity-cell.velocity) * lerp;
				if (orientation)
					*orientation = Ogre::Quaternion::Slerp(lerp, cell.orientation, nextCell.orientation,true);
			}
			return INTERPOLATED;
		}
	}

	// Return the oldest one
	const TransformationHistoryCell &cell = history.Peek();
	if (position)
		*position=cell.position;
	if (orientation)
		*orientation=cell.orientation;
	if (velocity)
		*velocity=cell.velocity;
	return READ_OLDEST;
}
void TransformationHistory::Clear(void)
{
	history.Clear();
}