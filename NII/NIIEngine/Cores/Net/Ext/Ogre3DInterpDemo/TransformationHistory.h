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
#ifndef __TRANFORMATION_HISTORY_H
#define __TRANFORMATION_HISTORY_H

#include "NiiNetCommon.h"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "DS_Queue.h"
#include "RakMemoryOverride.h"

struct TransformationHistoryCell
{
	TransformationHistoryCell();
	TransformationHistoryCell(TimeDurMS t, const Ogre::Vector3& pos, const Ogre::Vector3& vel, const Ogre::Quaternion& quat  );

	TimeDurMS time;
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Ogre::Vector3 velocity;
};

/// \brief This class stores a series of data points consisting of position, orientation, and velocity
/// Data points are written using write(). The number of points stored is determined by the values passed to Init()
/// Points are read out using read(). read() will interpolate between two known points given a time between those points, or between the last known and current point.
/// For smooth interpolation, render entities by reading their past positions by whatever amount of time your update interval is.
class TransformationHistory
{
public:
	/// \brief Call to setup this class with required parameters
	/// maxWriteInterval should be equal to or less than the interval between updates for a given entity
	/// maxHistoryTime is the maximum amount of time you want to read in the past
	/// \param[in] maxWriteInterval Minimum amount of time that must elapse between new data points added with write.
	/// \param[in] maxHistoryTime How long to store data points before they expire
	void Init(TimeDurMS maxWriteInterval, TimeDurMS maxHistoryTime);

	/// \brief Adds a new data point to the end of the queue
	/// If less than maxWriteInterval has elapsed since the last call, the write() call is ignored.
	/// \param[in] position Position to write
	/// \param[in] velocity Velocity to write
	/// \param[in] orientation Orientation to write
	/// \param[in] curTimeMS Time when data point was generated, which should generally increment each call
	void write(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, TimeDurMS curTimeMS);

	/// \brief Same as write(), except that if the point is in the past, an older point updated
	void Overwrite(const Ogre::Vector3 &position, const Ogre::Vector3 &velocity, const Ogre::Quaternion &orientation, TimeDurMS when);

	enum ReadResult
	{
		// We are reading so far in the past there is no data yet
		READ_OLDEST,
		// We are not reading in the past, so the input parameters stay the same
		VALUES_UNCHANGED,
		// We are reading in the past
		INTERPOLATED
	};
	/// \brief read an interpolated point in the psast
	/// Parameters are in/out, modified to reflect the history.
	/// \param[in/out] position As input, the current position of the object at \a curTime. As output, the position of the object at \when. Pass 0 to ignore.
	/// \param[in/out] velocity As input, the current velocity of the object at \a curTime. As output, the velocity of the object at \when. Pass 0 to ignore.
	/// \param[in/out] orientation As input, the current orientation of the object at \a curTime. As output, the orientation of the object at \when. Pass 0 to ignore.
	/// \param[in] when The time at which you want to read.
	/// \param[in] curTime Right now
	/// \return What method was used to calculate outputs
	ReadResult read(Ogre::Vector3 *position, Ogre::Vector3 *velocity, Ogre::Quaternion *orientation,
		TimeDurMS when, TimeDurMS curTime);

	/// \brief Clear all values in the history
	void Clear(void);
protected:
	DataStructures::Queue<TransformationHistoryCell> history;
	unsigned maxHistoryLength;
	TimeDurMS writeInterval;
};

#endif
