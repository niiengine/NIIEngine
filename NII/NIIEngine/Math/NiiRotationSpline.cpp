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

#include "NiiPreProcess.h"
#include "NiiRotationSpline.h"

namespace NII
{
    //---------------------------------------------------------------------
    RotationSpline::RotationSpline()
    {
    }
    //---------------------------------------------------------------------
    RotationSpline::~RotationSpline()
    {
    }
    //---------------------------------------------------------------------
    void RotationSpline::add(const Quaternion & p)
    {
        mPoints.push_back(p);
    }
    //---------------------------------------------------------------------
    void RotationSpline::set(Nidx index, const Quaternion & p)
    {
        N_assert(index < mPoints.size(), "Point index is out of bounds!!");

        mPoints[index] = p;
    }
    //---------------------------------------------------------------------
    const Quaternion & RotationSpline::get(Nidx index) const
    {
        N_assert(index < mPoints.size(), "Point index is out of bounds!!");

        return mPoints[index];
    }
    //---------------------------------------------------------------------
    NCount RotationSpline::getCount() const
    {
        return mPoints.size();
    }
    //---------------------------------------------------------------------
    Quaternion RotationSpline::interpolate(NIIf t, bool shortpath)
    {
        // 计算出它在哪个段中
        NIIf fSeg = t * (mPoints.size() - 1);
        Nui32 segIdx = (Nui32)fSeg;
        // 分配 t
        t = fSeg - segIdx;
        return interpolate(segIdx, t, shortpath);
    }
    //---------------------------------------------------------------------
    Quaternion RotationSpline::interpolate(Nidx fromIndex, NIIf t, bool shortpath)
    {
        // Bounds check
        N_assert (fromIndex < mPoints.size(), "fromIndex out of bounds");

        if((fromIndex + 1) == mPoints.size())
        {
            // Duff request, cannot blend to nothing
            // Just return source
            return mPoints[fromIndex];
        }

        // Fast special cases
        if(t == 0.0f)
        {
            return mPoints[fromIndex];
        }
        else if(t == 1.0f)
        {
            return mPoints[fromIndex + 1];
        }

        // NIIf interpolation
        // Use squad using tangents we've already set up
        Quaternion & p = mPoints[fromIndex];
        Quaternion & q = mPoints[fromIndex+1];
        Quaternion & a = mTangents[fromIndex];
        Quaternion & b = mTangents[fromIndex+1];

        // NB interpolate to nearest rotation
        return Quaternion::squad(t, p, a, b, q, shortpath);
    }
    //---------------------------------------------------------------------
    void RotationSpline::rebuild()
    {
        // ShoeMake (1987) approach
        // Just like Catmull-Rom really, just more gnarly
        // And no, I don't understand how to derive this!
        //
        // let p = point[i], pInv = p.inverse
        // tangent[i] = p * exp( -0.25 * ( log(pInv * point[i+1]) + log(pInv * point[i-1]) ) )
        //
        // Assume endpoint tangents are parallel with line with neighbour

        Nui32 i, numPoints;
        bool isClosed;

        numPoints = (Nui32)mPoints.size();

        if (numPoints < 2)
        {
            // Can't do anything yet
            return;
        }

        mTangents.resize(numPoints);

        if (mPoints[0] == mPoints[numPoints-1])
        {
            isClosed = true;
        }
        else
        {
            isClosed = false;
        }

        Quaternion invp, part1, part2, preExp;
        for(i = 0; i < numPoints; ++i)
        {
            Quaternion & p = mPoints[i];
            invp = p.inverse();

            if(i == 0)
            {
                // special case start
                part1 = (invp * mPoints[i+1]).Ln();
                if(isClosed)
                {
                    // Use numPoints-2 since numPoints-1 == end == start == this one
                    part2 = (invp * mPoints[numPoints-2]).Ln();
                }
                else
                {
                    part2 = (invp * p).Ln();
                }
            }
            else if (i == numPoints-1)
            {
                // special case end
                if (isClosed)
                {
                    // Wrap to [1] (not [0], this is the same as end == this one)
                    part1 = (invp * mPoints[1]).Ln();
                }
                else
                {
                    part1 = (invp * p).Ln();
                }
                part2 = (invp * mPoints[i-1]).Ln();
            }
            else
            {
                part1 = (invp * mPoints[i+1]).Ln();
                part2 = (invp * mPoints[i-1]).Ln();
            }
            preExp = -0.25 * (part1 + part2);
            mTangents[i] = p * preExp.Exp();
        }
    }
    //---------------------------------------------------------------------
    void RotationSpline::clear()
    {
        mPoints.clear();
        mTangents.clear();
    }
    //---------------------------------------------------------------------
}