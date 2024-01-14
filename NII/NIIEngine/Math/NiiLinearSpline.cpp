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
#include "NiiLinearSpline.h"
#include "NiiVector4.h"
#include "NiiMatrix4.h"

namespace NII
{
    //---------------------------------------------------------------------
    LinearSpline::LinearSpline()
    {
        // Hermite polynomial
        mHermite[0][0] = 2;  mHermite[0][1] = -2; mHermite[0][2] = 1;  mHermite[0][3] = 1;
        mHermite[1][0] = -3; mHermite[1][1] = 3;  mHermite[1][2] = -2; mHermite[1][3] = -1;
        mHermite[2][0] = 0;  mHermite[2][1] = 0;  mHermite[2][2] = 1;  mHermite[2][3] = 0;
        mHermite[3][0] = 1;  mHermite[3][1] = 0;  mHermite[3][2] = 0;  mHermite[3][3] = 0;
    }
    //---------------------------------------------------------------------
    LinearSpline::~LinearSpline()
    {
    }
    //---------------------------------------------------------------------
    void LinearSpline::add(const Vector3f & p)
    {
        mPoints.push_back(p);
    }
    //---------------------------------------------------------------------
    void LinearSpline::set(Nidx index, const Vector3f & value)
    {
        N_assert (index < mPoints.size(), "Point index is out of bounds!!");

        mPoints[index] = value;

        rebuild();
    }
    //---------------------------------------------------------------------
    const Vector3f & LinearSpline::get(Nidx index) const
    {
        N_assert (index < mPoints.size(), "Point index is out of bounds!!");

        return mPoints[index];
    }
    //---------------------------------------------------------------------
    NCount LinearSpline::getCount() const
    {
        return mPoints.size();
    }
    //---------------------------------------------------------------------
    Vector3f LinearSpline::interpolate(NIIf t) const
    {
        // Currently assumes points are evenly spaced, will cause velocity
        // change where this is not the case
        // TODO: base on arclength?

        // Work out which segment this is in
        NIIf fSeg = t * (mPoints.size() - 1);
        Nui32 segIdx = (Nui32)fSeg;
        // Apportion t
        t = fSeg - segIdx;

        return interpolate(segIdx, t);
    }
    //---------------------------------------------------------------------
    Vector3f LinearSpline::interpolate(Nidx fromIndex, NIIf t) const
    {
        // Bounds check
        N_assert(fromIndex < mPoints.size(), "fromIndex out of bounds");

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
        // Form a vector of powers of t
        NIIf t2, t3;
        t2 = t * t;
        t3 = t2 * t;
        Vector4f powers(t3, t2, t, 1);


        // Algorithm is ret = powers * mHermite * Matrix4(point1, point2, tangent1, tangent2)
        const Vector3f& point1 = mPoints[fromIndex];
        const Vector3f& point2 = mPoints[fromIndex + 1];
        const Vector3f& tan1 = mTangents[fromIndex];
        const Vector3f& tan2 = mTangents[fromIndex + 1];
        Matrix4f pt;

        pt[0][0] = point1.x;
        pt[0][1] = point1.y;
        pt[0][2] = point1.z;
        pt[0][3] = 1.0f;
        pt[1][0] = point2.x;
        pt[1][1] = point2.y;
        pt[1][2] = point2.z;
        pt[1][3] = 1.0f;
        pt[2][0] = tan1.x;
        pt[2][1] = tan1.y;
        pt[2][2] = tan1.z;
        pt[2][3] = 1.0f;
        pt[3][0] = tan2.x;
        pt[3][1] = tan2.y;
        pt[3][2] = tan2.z;
        pt[3][3] = 1.0f;

        Vector4f ret = powers * mHermite * pt;

        return Vector3f(ret.x, ret.y, ret.z);
    }
    //---------------------------------------------------------------------
    void LinearSpline::rebuild()
    {
        // Catmull-Rom approach
        //
        // tangent[i] = 0.5 * (point[i+1] - point[i-1])
        //
        // Assume endpoint tangents are parallel with line with neighbour

        NCount i, numPoints;
        bool isClosed;

        numPoints = mPoints.size();
        if (numPoints < 2)
        {
            // Can't do anything yet
            return;
        }

        // Closed or open?
        if (mPoints[0] == mPoints[numPoints-1])
        {
            isClosed = true;
        }
        else
        {
            isClosed = false;
        }

        mTangents.resize(numPoints);

        for(i = 0; i < numPoints; ++i)
        {
            if (i ==0)
            {
                // Special case start
                if (isClosed)
                {
                    // Use numPoints-2 since numPoints-1 is the last point and == [0]
                    mTangents[i] = 0.5 * (mPoints[1] - mPoints[numPoints-2]);
                }
                else
                {
                    mTangents[i] = 0.5 * (mPoints[1] - mPoints[0]);
                }
            }
            else if (i == numPoints-1)
            {
                // Special case end
                if (isClosed)
                {
                    // Use same tangent as already calculated for [0]
                    mTangents[i] = mTangents[0];
                }
                else
                {
                    mTangents[i] = 0.5 * (mPoints[i] - mPoints[i-1]);
                }
            }
            else
            {
                mTangents[i] = 0.5 * (mPoints[i+1] - mPoints[i-1]);
            }

        }
    }
    //---------------------------------------------------------------------
    void LinearSpline::clear(void)
    {
        mPoints.clear();
        mTangents.clear();
    }
    //---------------------------------------------------------------------
}