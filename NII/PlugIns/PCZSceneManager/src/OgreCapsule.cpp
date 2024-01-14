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

#include "OgreCapsule.h"

using namespace Ogre;

//----------------------------------------------------------------------------

Capsule::Capsule()
{
	// uninitialized
}
//----------------------------------------------------------------------------
Capsule::Capsule(const Segment& segment, NIIf radius)
	: mSegment(segment),
	mRadius(radius)
{
}
//----------------------------------------------------------------------------
void Capsule::set(const Vector3& newOrigin, const Vector3& newEnd, NIIf newRadius)
{
	mSegment.set(newOrigin, newEnd);
	mRadius = newRadius;
}
//----------------------------------------------------------------------------
void Capsule::setOrigin(const Vector3& newOrigin)
{
	mSegment.mOrigin = newOrigin;
}
//----------------------------------------------------------------------------
void Capsule::setEndPoint(const Vector3& newEndpoint)
{
	mSegment.setEndPoint(newEndpoint);
}
//----------------------------------------------------------------------------
void Capsule::setRadius(NIIf newRadius)
{
	mRadius = newRadius;
}
//----------------------------------------------------------------------------
bool Capsule::intersects(const Capsule& otherCapsule) const
{
	NIIf fDistance = mSegment.distance(otherCapsule.mSegment);
	NIIf fRSum = mRadius + otherCapsule.mRadius;
	return fDistance <= fRSum;
}
//----------------------------------------------------------------------------
bool Capsule::intersects(const Segment& segment) const
{
	NIIf fDist = segment.distance(mSegment);
	return fDist <= mRadius;
}
//----------------------------------------------------------------------------
