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
/***************************************************************************
octree.cpp  -  description
-------------------
begin                : Mon Sep 30 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team

***************************************************************************/

#include <OgreOctree.h>
#include <OgreOctreeNode.h>

namespace Ogre
{

/** Returns true is the box will fit in a child.
*/
bool Octree::_isTwiceSize(const AABox & box) const
{
    Vector3 halfMBoxSize = mBox.getSize() * 0.5;
    Vector3 boxSize = box.getSize();
    return ((boxSize.x <= halfMBoxSize.x) && (boxSize.y <= halfMBoxSize.y) && (boxSize.z <= halfMBoxSize.z));

}

/** It's assumed the the given box has already been proven to fit into
* a child.  Since it's a loose octree, only the centers need to be
* compared to find the appropriate node.
*/
void Octree::_getChildIndexes(const AABox & box, int * x, int * y, int * z) const
{
    Vector3 center = mBox.getMax().middle(mBox.getMin());

    Vector3 ncenter = box.getMax().middle(box.getMin());

    if ( ncenter.x > center.x )
        * x = 1;
    else
        *x = 0;

    if ( ncenter.y > center.y )
        * y = 1;
    else
        *y = 0;

    if ( ncenter.z > center.z )
        * z = 1;
    else
        *z = 0;

}

Octree::Octree( Octree * parent ) 
    : mWireBoundingBox(0),
      mHalfSize( 0, 0, 0 )
{
    //initialize all children to null.
    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++ )
        {
            for ( int k = 0; k < 2; k++ )
            {
                mChildren[ i ][ j ][ k ] = 0;
            }
        }
    }

    mParent = parent;
    mNumNodes = 0;
}

Octree::~Octree()
{
    //initialize all children to null.
    for ( int i = 0; i < 2; i++ )
    {
        for ( int j = 0; j < 2; j++ )
        {
            for ( int k = 0; k < 2; k++ )
            {
                if ( mChildren[ i ][ j ][ k ] != 0 )
                    N_delete mChildren[ i ][ j ][ k ];
            }
        }
    }

    if(mWireBoundingBox)
        N_delete mWireBoundingBox;

    mParent = 0;
}

void Octree::_addNode( OctreeNode * n )
{
    mNodes.push_back( n );
    n -> setOctant( this );

    //update total counts.
    _ref();

}

void Octree::_removeNode(OctreeNode * n)
{
    mNodes.erase(std::find(mNodes.begin(), mNodes.end(), n));
    n -> setOctant(0);

    //update total counts.
    _unref();
}

void Octree::_getCullBounds(AABox * b) const
{
    b ->setMin(mBox.getMin() - mHalfSize);
    b ->setMax(mBox.getMax() + mHalfSize);
}

WireAABGeo* Octree::getWireBoundingBox()
{
    // Create a WireAABGeo if needed
    if(mWireBoundingBox == 0)
        mWireBoundingBox = N_new WireAABGeo();

    mWireBoundingBox->setAABBGeo(mBox);
    return mWireBoundingBox;
}

}
