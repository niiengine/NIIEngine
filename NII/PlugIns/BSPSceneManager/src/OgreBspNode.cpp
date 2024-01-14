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

#include "OgreBspNode.h"
#include "OgreBspLevel.h"
#include "OgreException.h"
#include "OgreLogManager.h"

namespace Ogre
{
    //-----------------------------------------------------------------------
    BspNode::BspNode(BspLevel* owner, bool inIsLeaf)
    {
        mOwner = owner;
        mIsLeaf = inIsLeaf;
    }
    //-----------------------------------------------------------------------
    BspNode::BspNode()
    {
    }
    //-----------------------------------------------------------------------
    BspNode::~BspNode()
    {
    }
    //-----------------------------------------------------------------------
    bool BspNode::isLeaf(void) const
    {
        return mIsLeaf;
    }
    //-----------------------------------------------------------------------
    BspNode * BspNode::getFront(void) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");
        return mFront;
    }
    //-----------------------------------------------------------------------
    BspNode * BspNode::getBack(void) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");
        return mBack;
    }

    //-----------------------------------------------------------------------
    const Plane & BspNode::getSplitPlane(void) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");

        return mSplitPlane;
    }
    //-----------------------------------------------------------------------
    const AABox& BspNode::getAABB(void) const
    {
        if(!mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is only valid on a leaf node.");
        return mBounds;
    }
    //-----------------------------------------------------------------------
    int BspNode::getNumFaceGroups(void) const
    {
        if(!mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is only valid on a leaf node.");
        return mNumFaceGroups;
    }

    //-----------------------------------------------------------------------
    int BspNode::getFaceGroupStart(void) const
    {
        if(!mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is only valid on a leaf node.");
        return mFaceGroupStart;
    }
    //-----------------------------------------------------------------------
    bool BspNode::isLeafVisible(const BspNode * leaf) const
    {
        return mOwner->isLeafVisible(this, leaf);
    }
    //-----------------------------------------------------------------------
    Plane::PositionType BspNode::getSide(const Vector3 & point) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");

        return mSplitPlane.getSide(point);
    }
    //-----------------------------------------------------------------------
    BspNode * BspNode::getNextNode(const Vector3 & point) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");

        Plane::PositionType sd = getSide(point);
        if(sd == Plane::PT_Negative)
        {
            //LogManager::getSingleton().logMessage("back");
            return getBack();
        }
        else
        {
            //LogManager::getSingleton().logMessage("front");
            return getFront();
        }
    }
    //-----------------------------------------------------------------------
    void BspNode::_addMovable(const SpaceObj * mov)
    {
        mMovables.insert(mov);
    }
    //-----------------------------------------------------------------------
    void BspNode::_removeMovable(const SpaceObj * mov)
    {
        mMovables.erase(mov);
    }
    //-----------------------------------------------------------------------
    NIIf BspNode::getDistance(const Vector3& pos) const
    {
        if(mIsLeaf)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "This method is not valid on a leaf node.");

        return mSplitPlane.getDistance(pos);

    }
    //-----------------------------------------------------------------------
    const BspNode::NodeBrushList& BspNode::getSolidBrushes(void) const
    {
        return mSolidBrushes;
    }
    //-----------------------------------------------------------------------
    std::ostream& operator<< (std::ostream& o, BspNode& n)
    {
        o << "BspNode(";
        if (n.mIsLeaf)
        {
            o << "leaf, bbox=" << n.mBounds << ", cluster=" << n.mVisCluster;
            o << ", faceGrps=" << n.mNumFaceGroups << ", faceStart=" << n.mFaceGroupStart << ")";
        }
        else
        {
            o <<  "splitter, plane=" << n.mSplitPlane << ")";
        }
        return o;
    }
}