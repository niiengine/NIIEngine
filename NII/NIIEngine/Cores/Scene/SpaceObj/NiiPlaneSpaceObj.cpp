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
#include "NiiPlaneSpaceObj.h"
#include "NiiPosNode.h"

namespace NII
{
    //-----------------------------------------------------------------------
    PlaneSpaceObj::PlaneSpaceObj(SpaceID sid) : 
        Plane(),
        SpaceObj(sid),
        mSpacePos(Vector3f::ZERO),
        mSpaceRot(Quaternion::IDENTITY),
        mDirty(true)
    {
    }
    //-----------------------------------------------------------------------
    PlaneSpaceObj::PlaneSpaceObj(const Plane & o) :
        Plane(o), 
        mSpacePos(Vector3f::ZERO),
        mSpaceRot(Quaternion::IDENTITY),
        mDirty(true)
    {
    }
    //-----------------------------------------------------------------------
    PlaneSpaceObj::PlaneSpaceObj(const Vector3f & normals, NIIf constant) :
        Plane(normals, constant),
        mSpacePos(Vector3f::ZERO), 
        mSpaceRot(Quaternion::IDENTITY),
        mDirty(true)
    {
    }
    //-----------------------------------------------------------------------
    PlaneSpaceObj::PlaneSpaceObj(const Vector3f & normals, const Vector3f & p) :
        Plane(normals, p),
        mSpacePos(Vector3f::ZERO), 
        mSpaceRot(Quaternion::IDENTITY),
        mDirty(true)
    {
    }
    //-----------------------------------------------------------------------
    PlaneSpaceObj::PlaneSpaceObj(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3) :
        Plane(p1, p2, p3),
        mSpacePos(Vector3f::ZERO),
        mSpaceRot(Quaternion::IDENTITY),
        mDirty(true)
    {
    }
    //-----------------------------------------------------------------------
    PlaneSpaceObj::~PlaneSpaceObj() 
    {
    }
    //-----------------------------------------------------------------------
    const Plane & PlaneSpaceObj::getSpacePlane() const
    {
        if(mSpaceNode)
        {
            if(mDirty || !(mSpaceNode->getSpaceOri() == mSpaceRot &&
                mSpaceNode->getSpacePos() == mSpacePos))
            {
                mSpaceRot = mSpaceNode->getSpaceOri();
                mSpacePos = mSpaceNode->getSpacePos();
                // Rotate normal
                mSpacePlane.mNormal = mSpaceRot * mNormal;
                // d remains the same in rotation, since rotation happens first
                mSpacePlane.mD = mD;
                // Add on the effect of the translation (project onto new normal)
                mSpacePlane.mD -= mSpacePlane.mNormal.dotProduct(mSpacePos);

                mDirty = false;
            }
        }
        else
        {
            return *this;
        }
        return mSpacePlane;
    }
    //-----------------------------------------------------------------------
    void PlaneSpaceObj::queue(RenderQueue *)
    {
    }
    //-----------------------------------------------------------------------
    void PlaneSpaceObj::_notify(Camera *) 
    {
    }
    //-----------------------------------------------------------------------
    const AABox & PlaneSpaceObj::getAABB() const 
    { 
        return AABox::Null;
    }
    //-----------------------------------------------------------------------
    NIIf PlaneSpaceObj::getCenterRange() const 
    { 
        return 0.0f; 
    }
    //-----------------------------------------------------------------------
    FactoryID PlaneSpaceObj::getFactoryID() const
    {
        return N_FACTORY_MovablePlane;
    }
    //-----------------------------------------------------------------------
    void PlaneSpaceObj::query(GeometryQuery *)
    {
    }    
    //-----------------------------------------------------------------------
}