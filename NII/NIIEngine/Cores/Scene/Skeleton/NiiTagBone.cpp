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
#include "NiiTagBone.h"
#include "NiiMatrix4.h"
#include "NiiMatrix3.h"
#include "NiiGeometrySpace.h"
#include "NiiSpaceNode.h"
#include "NiiSkeleton.h"
#include "NiiQuaternion.h"

namespace NII
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // TagBone
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    TagBone::TagBone(SpaceID id, Skeleton * target) :
        Bone(id, target),
        mOwner(0),
        mAttach(0),
        mUseOwnerSpacePos(true),
        mUseOwnerSpaceOri(true),
        mUseOwnerSpaceScale(true)
    {
    }
    //-----------------------------------------------------------------------------
    TagBone::~TagBone()
    {
    }
    //-----------------------------------------------------------------------------
    void TagBone::setOwner(SpaceObj * obj)
    {
        mOwner = obj;
    }
    //-----------------------------------------------------------------------------
    SpaceObj * TagBone::getOwner() const
    {
        return mOwner;
    }
    //-----------------------------------------------------------------------------
    void TagBone::setAttach(SpaceObj * obj)
    {
        mAttach = obj;
    }
    //-----------------------------------------------------------------------------
    SpaceObj * TagBone::getAttach() const
    {
        return mAttach;
    }
    //-----------------------------------------------------------------------------
    void TagBone::setUseOwnerSpacePos(bool b)
    {
        mUseOwnerSpacePos = b;
    }
    //-----------------------------------------------------------------------------
    bool TagBone::isUseOwnerSpacePos() const
    {
        return mUseOwnerSpacePos;
    }
    //-----------------------------------------------------------------------------
    void TagBone::setUseOwnerSpaceOri(bool b)
    {
        mUseOwnerSpaceOri = b;
        notify();
    }
    //-----------------------------------------------------------------------------
    bool TagBone::isUseOwnerSpaceOri() const
    {
        return mUseOwnerSpaceOri;
    }
    //-----------------------------------------------------------------------------
    void TagBone::setUseOwnerSpaceScale(bool b)
    {
        mUseOwnerSpaceScale = b;
        notify();
    }
    //-----------------------------------------------------------------------------
    bool TagBone::isUseOwnerSpaceScale() const
    {
        return mUseOwnerSpaceScale;
    }
    //-----------------------------------------------------------------------------
    void TagBone::notify(bool force)
    {
        Bone::notify(force);

        // We need to tell parent entities node
        if(mOwner)
        {
            PosNode * n = mOwner->getSpaceNode();
            if(n)
            {
                n->notify();
            }
        }
    }
    //-----------------------------------------------------------------------------
    void TagBone::updateImpl() const
    {
        Bone::updateImpl();

        // 包含实体的变换
        if(mOwner)
        {
            PosNode * entityParentNode = mOwner->getSpaceNode();
            if(entityParentNode)
            {
                // Combine orientation with that of parent entity
                const Quaternion & parentOrientation = entityParentNode->getSpaceOri();
                if(mUseOwnerSpaceOri)
                {
                    mSpaceOri = parentOrientation * mSpaceOri;
                }

                // Incorporate parent entity scale
                const Vector3f & parentScale = entityParentNode->getSpaceScale();
                if(mUseOwnerSpaceScale)
                {
                    mSpaceScale *= parentScale;
                }

                // Change position vector based on parent entity's orientation & scale
                mSpacePos = parentOrientation * (parentScale * mSpacePos);

                if(mUseOwnerSpacePos)
                {
                    // Add altered position vector to parent entity
                    mSpacePos += entityParentNode->getSpacePos();
                }
            }
        }

        if(mAttach)
        {
            mAttach->_notify();
        }
    }
    //-----------------------------------------------------------------------------
    void TagBone::getLight(LightList & ll) const
    {
        return mOwner->query(ll);
    }
    //-----------------------------------------------------------------------------
}