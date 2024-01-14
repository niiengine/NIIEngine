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
octreenode.cpp  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team

***************************************************************************/

#include <OgreRoot.h>

#include <OgreOctreeNode.h>
#include <OgreOctreeSceneManager.h>

namespace Ogre
{
unsigned long green = 0xFFFFFFFF;

unsigned short OctreeNode::mIndexes[ 24 ] = {0, 1, 1, 2, 2, 3, 3, 0,       //back
        0, 6, 6, 5, 5, 1,             //left
        3, 7, 7, 4, 4, 2,             //right
        6, 7, 5, 4 };          //front
unsigned long OctreeNode::mColors[ 8 ] = {green, green, green, green, green, green, green, green };

OctreeNode::OctreeNode(SpaceManager * creator) : SpaceNode(creator)
{
    mOctant = 0;
}

OctreeNode::OctreeNode(SpaceManager * creator, const String & name) : SpaceNode(creator, name)
{
    mOctant = 0;
}

OctreeNode::~OctreeNode(){}

void OctreeNode::_removeNodeAndChildren( )
{
    static_cast< OctreeSceneManager * > ( mCreator ) -> _removeOctreeNode( this ); 
    //remove all the children nodes as well from the octree.
    ChildList::iterator it = mChildList.begin();
    while( it != mChildList.end() )
    {
        static_cast<OctreeNode *>( it->second ) -> _removeNodeAndChildren();
        ++it;
    }
}
Node * OctreeNode::remove(unsigned short index)
{
    OctreeNode *on = static_cast<OctreeNode *>(SpaceNode::remove(index));
    on -> _removeNodeAndChildren(); 
    return on; 
}
Node * OctreeNode::remove(Node * child)
{
    OctreeNode *on = static_cast<OctreeNode *>(SpaceNode::remove(child));
    on -> _removeNodeAndChildren(); 
    return on; 
}

void OctreeNode::removeAll()
{
	ChildList::iterator i, iend = mChildList.end();
	for (i = mChildList.begin(); i != iend; ++i)
	{
		OctreeNode* on = static_cast<OctreeNode *>(i->second);
		on->setParent(0);
		on->_removeNodeAndChildren();
	}
	mChildList.clear();
	mChildrenToUpdate.clear();
	
}
    
Node * OctreeNode::remove( const String & name )
{
    OctreeNode *on = static_cast< OctreeNode * >( SpaceNode::remove(  name ) );
    on -> _removeNodeAndChildren( ); 
    return on; 
}

//same as SpaceNode, only it doesn't care about children...
void OctreeNode::updateSpaceAABB( void )
{
    mWorldAABB.setNull();
    mLocalAABB.setNull();

    // Update bounds from own attached objects
    AttachList::iterator i = mAttachList.begin();
    AABox bx;

    while(i != mAttachList.end())
    {
        // Get local bounds of object
        bx = i->second ->getAABB();

        mLocalAABB.merge( bx );

        mWorldAABB.merge( i->second ->getSpaceAABB(true) );
        ++i;
    }

    //update the OctreeSceneManager that things might have moved.
    // if it hasn't been added to the octree, add it, and if has moved
    // enough to leave it's current node, we'll update it.
    if ( ! mWorldAABB.isNull() )
    {
        static_cast<OctreeSceneManager *>(mCreator)->_updateOctreeNode(this);
    }

}

/** Since we are loose, only check the center.
*/
bool OctreeNode::_isIn(AABox & box)
{
	// Always fail if not in the scene graph or box is null
	if(!mEnable || box.isNull()) 
        return false;

    Vector3 center = mWorldAABB.getMax().middle(mWorldAABB.getMin());

    Vector3 bmin = box.getMin();
    Vector3 bmax = box.getMax();

    bool centre = (bmax > center && bmin < center);
	if(!centre)
		return false;

	// Even if covering the centre line, need to make sure this BB is not large
	// enough to require being moved up into parent. When added, bboxes would
	// end up in parent due to cascade but when updating need to deal with
	// bbox growing too large for this child
	Vector3 octreeSize = bmax - bmin;
	Vector3 nodeSize = mWorldAABB.getMax() - mWorldAABB.getMin();
	return nodeSize < octreeSize;
}

/** Addes the attached objects of this OctreeScene node into the queue. */
void OctreeNode::_addToRenderQueue(Camera * cam, RenderQueue * queue, 
	bool onlyShadowCasters, RenderBound * visibleBounds)
{
    AttachList::iterator mit = mAttachList.begin();

    while(mit != mAttachList.end())
    {
        SpaceObj * mo = mit->second;
		
		mo->queue(queue, cam, visibleBounds, onlyShadowCasters);

        ++mit;
    }
}
void OctreeNode::getGeometry(GeometryRaw & rend) const
{
    /* TODO
    rend.add(GRT_Index);
    rend.numTextureCoordSets = 0; // no textures
    rend.vertexOptions = LegacyRenderOperation::VO_DIFFUSE_COLOURS;
    rend.mType = LegacyRenderOperation::OT_LINE_LIST;
    rend.numVertices = 8;
    rend.numIndexes = 24;

    rend.pVertices = mCorners;
    rend.pIndexes = mIndexes;
    rend.pDiffuseColour = mColors;

    const Vector3 * corner[8];

    corner[0] = _getLocalAABB().mMinimum;

    corner[1].x = _getLocalAABB().mMinimum.x;
    corner[1].y = _getLocalAABB().mMaximum.y;
    corner[1].z = _getLocalAABB().mMinimum.z;

    corner[2].x = _getLocalAABB().mMaximum.x;
    corner[2].y = _getLocalAABB().mMaximum.y;
    corner[2].z = _getLocalAABB().mMinimum.z;

    corner[3].x = _getLocalAABB().mMaximum.x;
    corner[3].y = _getLocalAABB().mMinimum.y;
    corner[3].z = _getLocalAABB().mMinimum.z;

    corner[4] = _getLocalAABB().mMaximum;
            
    corner[5].x = _getLocalAABB().mMinimum.x;
    corner[5].y = _getLocalAABB().mMaximum.y;
    corner[5].z = _getLocalAABB().mMaximum.z;

    corner[6].x = _getLocalAABB().mMinimum.x;
    corner[6].y = _getLocalAABB().mMinimum.y;
    corner[6].z = _getLocalAABB().mMaximum.z;

    corner[7].x = _getLocalAABB().mMaximum.x;
    corner[7].y = _getLocalAABB().mMinimum.y;
    corner[7].z = _getLocalAABB().mMaximum.z;
            
    int index = 0;

    for(int i = 0; i < 8; ++i)
    {
        rend.pVertices[index] = corner[ i ].x;
        index++;
        rend.pVertices[index] = corner[ i ].y;
        index++;
        rend.pVertices[index] = corner[ i ].z;
        index++;
    }
    */
}
}