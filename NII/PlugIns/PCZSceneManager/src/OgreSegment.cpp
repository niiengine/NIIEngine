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

#include "OgreSegment.h"
#include "OgreCapsule.h"

using namespace Ogre;

const NIIf PARALLEL_TOLERANCE =	0.0001;

//----------------------------------------------------------------------------
Segment::Segment ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
Segment::Segment (const Vector3 & origin, const Vector3 & direction, NIIf extent) :
    mOrigin(origin),
    mDirection(direction),
    mExtent(extent)
{
}
//----------------------------------------------------------------------------
void Segment::set(const Vector3& newOrigin, const Vector3& newEnd)
{
	mOrigin = newOrigin;
	// calc the direction vector
	mDirection = newEnd - mOrigin;
	mExtent = mDirection.normalise();
}
//----------------------------------------------------------------------------
void Segment::setOrigin(const Vector3& newOrigin)
{
	mOrigin = newOrigin;
}
//----------------------------------------------------------------------------
void Segment::setEndPoint(const Vector3& newEnd)
{
	// calc the direction vector
	mDirection = newEnd - mOrigin;
	mExtent = mDirection.normalise();
}
//----------------------------------------------------------------------------
NIIf Segment::distance(const Segment& otherSegment) const
{
    NIIf fSqrDist = squaredDistance(otherSegment);
	return NII::Math::sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
NIIf Segment::squaredDistance(const Segment& otherSegment) const
{
    Vector3 kDiff = mOrigin - otherSegment.mOrigin;
    NIIf fA01 = -mDirection.dotProduct(otherSegment.mDirection);
    NIIf fB0 = kDiff.dotProduct(mDirection);
    NIIf fB1 = -kDiff.dotProduct(otherSegment.mDirection);
	NIIf fC = kDiff.lengthSquared();
	NIIf fDet = Math::Abs((NIIf)1.0 - fA01*fA01);
    NIIf fS0, fS1, fSqrDist, fExtDet0, fExtDet1, fTmpS0, fTmpS1;

	if (fDet >= PARALLEL_TOLERANCE)
    {
        // segments are not parallel
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;
        fExtDet0 = mExtent*fDet;
        fExtDet1 = otherSegment.mExtent*fDet;

        if (fS0 >= -fExtDet0)
        {
            if (fS0 <= fExtDet0)
            {
                if (fS1 >= -fExtDet1)
                {
                    if (fS1 <= fExtDet1)  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        NIIf fInvDet = ((NIIf)1.0)/fDet;
                        fS0 *= fInvDet;
                        fS1 *= fInvDet;
                        fSqrDist = fS0*(fS0+fA01*fS1+((NIIf)2.0)*fB0) +
                            fS1*(fA01*fS0+fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else  // region 3 (side)
                    {
                        fS1 = otherSegment.mExtent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if (fTmpS0 < -mExtent)
                        {
                            fS0 = -mExtent;
                            fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                                fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                        }
                        else if (fTmpS0 <= mExtent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = mExtent;
                            fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                                fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fS1 = -otherSegment.mExtent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 < -mExtent)
                    {
                        fS0 = -mExtent;
                        fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                            fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else if (fTmpS0 <= mExtent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = mExtent;
                        fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                            fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                }
            }
            else
            {
                if (fS1 >= -fExtDet1)
                {
                    if (fS1 <= fExtDet1)  // region 1 (side)
                    {
                        fS0 = mExtent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 < -otherSegment.mExtent)
                        {
                            fS1 = -otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                        else if (fTmpS1 <= otherSegment.mExtent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                        else
                        {
                            fS1 = otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fS1 = otherSegment.mExtent;
                        fTmpS0 = -(fA01*fS1+fB0);
                        if (fTmpS0 < -mExtent)
                        {
                            fS0 = -mExtent;
                            fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                                fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                        }
                        else if (fTmpS0 <= mExtent)
                        {
                            fS0 = fTmpS0;
                            fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                        }
                        else
                        {
                            fS0 = mExtent;
                            fTmpS1 = -(fA01*fS0+fB1);
                            if (fTmpS1 < -otherSegment.mExtent)
                            {
                                fS1 = -otherSegment.mExtent;
                                fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                    fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                            }
                            else if (fTmpS1 <= otherSegment.mExtent)
                            {
                                fS1 = fTmpS1;
                                fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)
                                    + fC;
                            }
                            else
                            {
                                fS1 = otherSegment.mExtent;
                                fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                    fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    fS1 = -otherSegment.mExtent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 < -mExtent)
                    {
                        fS0 = -mExtent;
                        fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                            fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else if (fTmpS0 <= mExtent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = mExtent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 > otherSegment.mExtent)
                        {
                            fS1 = otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                        else if (fTmpS1 >= -otherSegment.mExtent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = -otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if (fS1 >= -fExtDet1)
            {
                if (fS1 <= fExtDet1)  // region 5 (side)
                {
                    fS0 = -mExtent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if (fTmpS1 < -otherSegment.mExtent)
                    {
                        fS1 = -otherSegment.mExtent;
                        fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                            fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                    }
                    else if (fTmpS1 <= otherSegment.mExtent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                    }
                    else
                    {
                        fS1 = otherSegment.mExtent;
                        fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                            fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                    }
                }
                else  // region 4 (corner)
                {
                    fS1 = otherSegment.mExtent;
                    fTmpS0 = -(fA01*fS1+fB0);
                    if (fTmpS0 > mExtent)
                    {
                        fS0 = mExtent;
                        fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                            fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else if (fTmpS0 >= -mExtent)
                    {
                        fS0 = fTmpS0;
                        fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                    }
                    else
                    {
                        fS0 = -mExtent;
                        fTmpS1 = -(fA01*fS0+fB1);
                        if (fTmpS1 < -otherSegment.mExtent)
                        {
                            fS1 = -otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                        else if (fTmpS1 <= otherSegment.mExtent)
                        {
                            fS1 = fTmpS1;
                            fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)
                                + fC;
                        }
                        else
                        {
                            fS1 = otherSegment.mExtent;
                            fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                                fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                fS1 = -otherSegment.mExtent;
                fTmpS0 = -(fA01*fS1+fB0);
                if (fTmpS0 > mExtent)
                {
                    fS0 = mExtent;
                    fSqrDist = fS0*(fS0-((NIIf)2.0)*fTmpS0) +
                        fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                }
                else if (fTmpS0 >= -mExtent)
                {
                    fS0 = fTmpS0;
                    fSqrDist = -fS0*fS0+fS1*(fS1+((NIIf)2.0)*fB1)+fC;
                }
                else
                {
                    fS0 = -mExtent;
                    fTmpS1 = -(fA01*fS0+fB1);
                    if (fTmpS1 < -otherSegment.mExtent)
                    {
                        fS1 = -otherSegment.mExtent;
                        fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                            fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                    }
                    else if (fTmpS1 <= otherSegment.mExtent)
                    {
                        fS1 = fTmpS1;
                        fSqrDist = -fS1*fS1+fS0*(fS0+((NIIf)2.0)*fB0)
                            + fC;
                    }
                    else
                    {
                        fS1 = otherSegment.mExtent;
                        fSqrDist = fS1*(fS1-((NIIf)2.0)*fTmpS1) +
                            fS0*(fS0+((NIIf)2.0)*fB0)+fC;
                    }
                }
            }
        }
    }
    else
    {
        // The segments are parallel.  The average b0 term is designed to
        // ensure symmetry of the function.  That is, dist(seg0,seg1) and
        // dist(seg1,seg0) should produce the same number.
        NIIf fE0pE1 = mExtent + otherSegment.mExtent;
        NIIf fSign = (fA01 > (NIIf)0.0 ? (NIIf)-1.0 : (NIIf)1.0);
        NIIf fB0Avr = ((NIIf)0.5)*(fB0 - fSign*fB1);
        NIIf fLambda = -fB0Avr;
        if (fLambda < -fE0pE1)
        {
            fLambda = -fE0pE1;
        }
        else if (fLambda > fE0pE1)
        {
            fLambda = fE0pE1;
        }

//        fS1 = -fSign*fLambda*otherSegment.mExtent/fE0pE1;
//        fS0 = fLambda + fSign*fS1;
        fSqrDist = fLambda*(fLambda + ((NIIf)2.0)*fB0Avr) + fC;
    }
	// we don't need the following stuff - it's for calculating closest point
//    mClosestPoint0 = mOrigin + fS0*mDirection;
//    mClosestPoint1 = otherSegment.mOrigin + fS1*otherSegment.mDirection;
//    mSegment0Parameter = fS0;
//    mSegment1Parameter = fS1;
    return Math::Abs(fSqrDist);
}

//----------------------------------------------------------------------------
bool Segment::intersects(const Capsule &capsule) const
{
    NIIf fDist =  distance(capsule.mSegment);
    return fDist <= capsule.mRadius;
}
