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
#include "NiiAABox.h"
#include "NiiVector3.h"
#include "NiiMatrix4.h"
#include "NiiMath.h"
#include "NiiPlane.h"
#include "NiiSphere.h"

namespace NII
{
    const AABox AABox::Null = AABox(0,0,0,0,0,0);
    //----------------------------------------------------------------------------
    AABox::AABox()
    {
    }
    //----------------------------------------------------------------------------
    AABox::AABox(const AABox & o)
    {
        setMin(o.mMinimum);
        setMax(o.mMaximum);
    }
    //----------------------------------------------------------------------------
    AABox::AABox(const Vector3f & min, const Vector3f & max)
    {
        setMin(min);
        setMax(max);
    }
    //----------------------------------------------------------------------------
    AABox::AABox(NIIf minx, NIIf miny, NIIf minz, NIIf maxx, NIIf maxy, NIIf maxz)
    {
        mMinimum.x = minx;
        mMinimum.y = miny;
        mMinimum.z = minz;

        mMaximum.x = maxx;
        mMaximum.y = maxy;
        mMaximum.z = maxz;
    }
    //----------------------------------------------------------------------------
    AABox::~AABox()
    {
    }
    //----------------------------------------------------------------------------
    void AABox::transform(const Matrix4f & matrix)
    {
        Vector3f oldMin, oldMax, currentCorner;

        // Getting the old values so that we can use the existing merge method.
        oldMin = mMinimum;
        oldMax = mMaximum;

        // reset
        setNull();

        // 我们依次按照以下顺序计算角落:
        // 0, 6, 5, 1, 2, 4 ,7 , 3
        // 这个序列允许一次仅可以改变所有角落的一个成员

        // 对于每一个,我们使用矩阵变换它
        // 获取产生点和合并产生点

        // First corner
        // min min min
        currentCorner = oldMin;
        merge(matrix * currentCorner);

        // min,min,max
        currentCorner.z = oldMax.z;
        merge(matrix * currentCorner);

        // min max max
        currentCorner.y = oldMax.y;
        merge(matrix * currentCorner);

        // min max min
        currentCorner.z = oldMin.z;
        merge(matrix * currentCorner);

        // max max min
        currentCorner.x = oldMax.x;
        merge(matrix * currentCorner);

        // max max max
        currentCorner.z = oldMax.z;
        merge(matrix * currentCorner);

        // max min max
        currentCorner.y = oldMin.y;
        merge(matrix * currentCorner);

        // max min min
        currentCorner.z = oldMin.z;
        merge(matrix * currentCorner);
    }
    //----------------------------------------------------------------------------
    void AABox::affine(const Matrix4f & m)
    {
        N_assert(m.isAffine(), "error");

        Vector3f centre = getCenter();
        Vector3f halfSize = (mMaximum - mMinimum) * 0.5;

        Vector3f newCentre = m.affine(centre);
        Vector3f newHalfSize(
            Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z,
            Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
            Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);

        setMin(newCentre - newHalfSize);
        setMax(newCentre + newHalfSize);
    }
    //-------------------------------------------------------------------------------
    AABox AABox::intersection(const AABox & o) const
    {
        Vector3f intMin = mMinimum.ceil(o.getMin());
        Vector3f intMax = mMaximum.floor(o.getMax());

        // Check intersection isn't null
        if(intMin.x < intMax.x && intMin.y < intMax.y && intMin.z < intMax.z)
        {
            return AABox(intMin, intMax);
        }
        return AABox();
    }
    //-------------------------------------------------------------------------------
    bool AABox::intersects(const Sphere & s) const
    {
        return s.intersects(*this);
    }
    //-------------------------------------------------------------------------------
    bool AABox::intersects(const Plane & p) const
    {
        return p.intersects(*this);
    }
    //--------------------------------------------------------------------
    NIIf AABox::calcRadius()
    {
        Vector3f magnitude = mMaximum;
        magnitude = magnitude.ceil(-mMaximum);
        magnitude = magnitude.ceil(mMinimum);
        magnitude = magnitude.ceil(-mMinimum);

        return magnitude.length();
    }
    //-------------------------------------------------------------------------------
    NIIf AABox::distance(const Vector3f & v) const
    {
        if(this->intersects(v))
            return 0;
        else
        {
            NIIf maxDist = N_TYPE_MIN(NIIf);

            if(v.x < mMinimum.x)
                maxDist = std::max(maxDist, mMinimum.x - v.x);
            if(v.y < mMinimum.y)
                maxDist = std::max(maxDist, mMinimum.y - v.y);
            if(v.z < mMinimum.z)
                maxDist = std::max(maxDist, mMinimum.z - v.z);

            if(v.x > mMaximum.x)
                maxDist = std::max(maxDist, v.x - mMaximum.x);
            if(v.y > mMaximum.y)
                maxDist = std::max(maxDist, v.y - mMaximum.y);
            if(v.z > mMaximum.z)
                maxDist = std::max(maxDist, v.z - mMaximum.z);

            return maxDist;
        }
    }
    //--------------------------------------------------------------------------------
}