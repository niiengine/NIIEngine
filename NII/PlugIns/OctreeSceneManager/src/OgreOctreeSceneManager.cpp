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
octreescenemanager.cpp  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team

***************************************************************************/

#include <OgreOctreeSceneManager.h>
#include <OgreOctreeSpaceQuery.h>
#include <OgreOctreeNode.h>
#include <OgreOctreeCamera.h>
#include <OgreRenderSystem.h>


extern "C"
{
    void findNodesInBox( Ogre::SpaceManager *sm,
                         const Ogre::AABox &box,
						 Ogre::list< Ogre::SpaceNode * >::type &list,
                         Ogre::SpaceNode *exclude )
    {
        static_cast<Ogre::OctreeSceneManager*>( sm ) -> findNodesIn( box, list, exclude );
    }
    void findNodesInSphere( Ogre::SpaceManager *sm,
                            const Ogre::Sphere &sphere,
							Ogre::list< Ogre::SpaceNode * >::type &list,
                            Ogre::SpaceNode *exclude )
    {
        static_cast<Ogre::OctreeSceneManager*>( sm ) -> findNodesIn( sphere, list, exclude );
    }
}

namespace Ogre
{
enum Intersection
{
    OUTSIDE=0,
    INSIDE=1,
    INTERSECT=2
};
int OctreeSceneManager::intersect_call = 0;

Intersection intersects( const Ray &one, const AABox &two )
{
    OctreeSceneManager::intersect_call++;
    // Null box?
    if (two.isNull()) return OUTSIDE;

    bool inside = true;
    const Vector3& twoMin = two.getMin();
    const Vector3& twoMax = two.getMax();
    Vector3 origin = one.getOrigin();
    Vector3 dir = one.getDirection();

    Vector3 maxT(-1, -1, -1);

    int i = 0;
    for(i=0; i<3; i++ )
    {
        if( origin[i] < twoMin[i] )
        {
            inside = false;
            if( dir[i] > 0 )
            {
                maxT[i] = (twoMin[i] - origin[i])/ dir[i];
            }
        }
        else if( origin[i] > twoMax[i] )
        {
            inside = false;
            if( dir[i] < 0 )
            {
                maxT[i] = (twoMax[i] - origin[i]) / dir[i];
            }
        }
    }

    if( inside )
    {
        return INTERSECT;
    }
    int whichPlane = 0;
    if( maxT[1] > maxT[whichPlane])
        whichPlane = 1;
    if( maxT[2] > maxT[whichPlane])
        whichPlane = 2;

    if( ((int)maxT[whichPlane]) & 0x80000000 )
    {
        return OUTSIDE;
    }
    for(i=0; i<3; i++ )
    {
        if( i!= whichPlane )
        {
            float f = origin[i] + maxT[whichPlane] * dir[i];
            if ( f < (twoMin[i] - 0.00001f) ||
                    f > (twoMax[i] +0.00001f ) )
            {
                return OUTSIDE;
            }
        }
    }

    return INTERSECT;

}


/** Checks how the second box intersects with the first.
*/
Intersection intersects(const PlaneSpace & one, const AABox & two)
{
    OctreeSceneManager::intersect_call++;
    // Null box?
    if(two.isNull())
        return OUTSIDE;

    // For each plane, see if all points are on the negative side
    // If so, object is not visible.
    // If one or more are, it's partial.
    // If all aren't, full
    bool all_inside = true;
    PlaneList::const_iterator i, iend = one.mPlanes.end();
    for (i = one.mPlanes.begin(); i != iend; ++i)
    {
        const Plane& plane = *i;

        Plane::PositionType side = plane.getSide(two);
        if(side == one.mOuterType)
            return OUTSIDE;
        if(side == Plane::PT_Merge)
            all_inside = false;
    }
    if(all_inside)
        return INSIDE;
    else
        return INTERSECT;
}
/** Checks how the second box intersects with the first.
*/
Intersection intersects( const AABox &one, const AABox &two )
{
    OctreeSceneManager::intersect_call++;
    // Null box?
    if (one.isNull() || two.isNull()) return OUTSIDE;


    const Vector3& insideMin = two.getMin();
    const Vector3& insideMax = two.getMax();

    const Vector3& outsideMin = one.getMin();
    const Vector3& outsideMax = one.getMax();

    if (    insideMax.x < outsideMin.x ||
            insideMax.y < outsideMin.y ||
            insideMax.z < outsideMin.z ||
            insideMin.x > outsideMax.x ||
            insideMin.y > outsideMax.y ||
            insideMin.z > outsideMax.z )
    {
        return OUTSIDE;
    }

    bool full = ( insideMin.x > outsideMin.x &&
                  insideMin.y > outsideMin.y &&
                  insideMin.z > outsideMin.z &&
                  insideMax.x < outsideMax.x &&
                  insideMax.y < outsideMax.y &&
                  insideMax.z < outsideMax.z );

    if ( full )
        return INSIDE;
    else
        return INTERSECT;

}

/** Checks how the box intersects with the sphere.
*/
Intersection intersects( const Sphere &one, const AABox &two )
{
    OctreeSceneManager::intersect_call++;
    // Null box?
    if (two.isNull()) return OUTSIDE;

    float sradius = one.getRadius();

    sradius *= sradius;

    Vector3 scenter = one.getCenter();

    const Vector3& twoMin = two.getMin();
    const Vector3& twoMax = two.getMax();

    float s, d = 0;

    Vector3 mndistance = ( twoMin - scenter );
    Vector3 mxdistance = ( twoMax - scenter );

    if(mndistance.lengthSquared() < sradius && mxdistance.lengthSquared() < sradius )
    {
        return INSIDE;
    }

    //find the square of the distance
    //from the sphere to the box
    for ( int i = 0 ; i < 3 ; i++ )
    {
        if ( scenter[ i ] < twoMin[ i ] )
        {
            s = scenter[ i ] - twoMin[ i ];
            d += s * s;
        }

        else if ( scenter[ i ] > twoMax[ i ] )
        {
            s = scenter[ i ] - twoMax[ i ];
            d += s * s;
        }

    }

    bool partial = ( d <= sradius );

    if ( !partial )
    {
        return OUTSIDE;
    }

    else
    {
        return INTERSECT;
    }


}

unsigned long white = 0xFFFFFFFF;

unsigned short OctreeSceneManager::mIndexes[ 24 ] = {0, 1, 1, 2, 2, 3, 3, 0,       //back
        0, 6, 6, 5, 5, 1,             //left
        3, 7, 7, 4, 4, 2,             //right
        6, 7, 5, 4 };          //front
unsigned long OctreeSceneManager::mColors[ 8 ] = {white, white, white, white, white, white, white, white };


OctreeSceneManager::OctreeSceneManager(const String & name) : 
    SpaceManager(name)
{
    AABox b(-10000, -10000, -10000, 10000, 10000, 10000);
    int depth = 8;
    mOctree = 0;
    init(b, depth);
    mType = OctreeSceneManagerFactory::FACTORY_TYPE_NAME;
}

OctreeSceneManager::OctreeSceneManager(const String & name, AABox & box, int max_depth): 
    SpaceManager(name)
{
    mOctree = 0;
    init(box, max_depth);
    mType = OctreeSceneManagerFactory::FACTORY_TYPE_NAME;
}

void OctreeSceneManager::init( AABox &box, int depth )
{

    if ( mOctree != 0 )
        N_delete mOctree;

    mOctree = N_new Octree( 0 );

    mMaxDepth = depth;
    mBox = box;

    mOctree -> mBox = box;

    Vector3 min = box.getMin();

    Vector3 max = box.getMax();

    mOctree -> mHalfSize = ( max - min ) / 2;


    mShowBoxes = false;

    mNumObjects = 0;

    Vector3 v( 1.5, 1.5, 1.5 );

    mScaleFactor.setScale(v);

    // setNodeView( true );
    // setShowBoxes( true );

    //
    //mOriginNode isn't put into the octree since it has no volume.

}

OctreeSceneManager::~OctreeSceneManager()
{

    if ( mOctree )
	{
        N_delete mOctree;
		mOctree = 0;
	}
}

Camera * OctreeSceneManager::createCamera(const String & name)
{
	// Check name not used
	if(mCameraList.find(name) != mCameraList.end())
	{
		N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
			"A camera with the name " + name + " already exists");
	}

	Camera * c = N_new OctreeCamera(name, this);
    mCameraList.insert(CameraMap::value_type(name, c));

    return c;
}

void OctreeSceneManager::destroySpaceNode(SpaceID sid)
{
    OctreeNode * on = static_cast<OctreeNode *> (getSpaceNode(sid));

    if ( on != 0 )
        _removeOctreeNode( on );

    SpaceManager::destroySpaceNode( sid );
}

bool OctreeSceneManager::getPropertyList(PropertyArray * refKeys) const
{
    SpaceManager::getPropertyList(refKeys);
    refKeys->push_back("Size");
    refKeys->push_back("ShowOctree");
    refKeys->push_back("Depth");

    return true;
}

void OctreeSceneManager::_updateOctreeNode(OctreeNode * onode)
{
    const AABox& box = onode -> getSpaceAABB();

    if ( box.isNull() )
        return ;

	// Skip if octree has been destroyed (shutdown conditions)
	if(!mOctree)
		return;

    if(onode -> getOctant() == 0)
    {
        //if outside the octree, force into the root node.
        if(!onode -> _isIn(mOctree -> mBox))
            mOctree->_addNode(onode);
        else
            _addOctreeNode(onode, mOctree);
        return ;
    }

    if(!onode -> _isIn(onode -> getOctant() -> mBox))
    {
        _removeOctreeNode(onode);

        //if outside the octree, force into the root node.
        if(!onode -> _isIn(mOctree -> mBox))
            mOctree->_addNode(onode);
        else
            _addOctreeNode(onode, mOctree);
    }
}

/** Only removes the node from the octree.  It leaves the octree, even if it's empty.
*/
void OctreeSceneManager::_removeOctreeNode( OctreeNode * n )
{
	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

    Octree * oct = n -> getOctant();

    if ( oct )
    {
        oct -> _removeNode( n );
    }

    n->setOctant(0);
}


void OctreeSceneManager::_addOctreeNode( OctreeNode * n, Octree *octant, int depth )
{

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

	const AABox& bx = n -> getSpaceAABB();


    //if the octree is twice as big as the scene node,
    //we will add it to a child.
    if ( ( depth < mMaxDepth ) && octant -> _isTwiceSize( bx ) )
    {
        int x, y, z;
        octant -> _getChildIndexes( bx, &x, &y, &z );

        if ( octant -> mChildren[ x ][ y ][ z ] == 0 )
        {
            octant -> mChildren[ x ][ y ][ z ] = N_new Octree( octant );
            const Vector3& octantMin = octant -> mBox.getMin();
            const Vector3& octantMax = octant -> mBox.getMax();
            Vector3 min, max;

            if ( x == 0 )
            {
                min.x = octantMin.x;
                max.x = ( octantMin.x + octantMax.x ) / 2;
            }

            else
            {
                min.x = ( octantMin.x + octantMax.x ) / 2;
                max.x = octantMax.x;
            }

            if ( y == 0 )
            {
                min.y = octantMin.y;
                max.y = ( octantMin.y + octantMax.y ) / 2;
            }

            else
            {
                min.y = ( octantMin.y + octantMax.y ) / 2;
                max.y = octantMax.y;
            }

            if(z == 0)
            {
                min.z = octantMin.z;
                max.z = (octantMin.z + octantMax.z) / 2;
            }

            else
            {
                min.z = (octantMin.z + octantMax.z) / 2;
                max.z = octantMax.z;
            }

            octant -> mChildren[ x ][ y ][ z ] ->mBox.setMin(min);
            octant -> mChildren[ x ][ y ][ z ] ->mBox.setMax(max);
            octant -> mChildren[ x ][ y ][ z ] -> mHalfSize = ( max - min ) / 2;
        }

        _addOctreeNode( n, octant -> mChildren[ x ][ y ][ z ], ++depth );

    }

    else
    {
        octant -> _addNode( n );
    }
}


SpaceNode * OctreeSceneManager::createNodeImpl()
{
    return N_new OctreeNode(this);
}

SpaceNode * OctreeSceneManager::createNodeImpl(const String & name)
{
    return N_new OctreeNode(this, name);
}

void OctreeSceneManager::_alertVisibleObjects()
{
    N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Function doesn't do as advertised");

//    Octree::NodeList::iterator it = mVisible.begin();
//
//    while ( it != mVisible.end() )
//    {
//        OctreeNode * node = *it;
//
//        ++it;
//    }
}

void OctreeSceneManager::findSpace(Camera * cam, RenderBound * visibleBounds, bool onlycast)
{

    getRenderQueue()->clear();
    mBoxes.clear();
    mVisible.clear();

    mNumObjects = 0;

    //walk the octree, adding all visible Octreenodes nodes to the render queue.
    walkOctree(static_cast < OctreeCamera * > (cam), getRenderQueue(), mOctree,
        visibleBounds, false, onlycast);

    // Show the octree boxes & cull camera if required
    if(mShowBoxes)
    {
        for(BoxList::iterator it = mBoxes.begin(); it != mBoxes.end(); ++it)
        {
            getRenderQueue()->add(*it);
        }
    }
}

void OctreeSceneManager::walkOctree( OctreeCamera *camera, RenderQueue *queue,
	Octree *octant, RenderBound* visibleBounds,
	bool foundvisible, bool onlyShadowCasters )
{

    //return immediately if nothing is in the node.
    if ( octant -> numNodes() == 0 )
        return ;

    OctreeCamera::Visibility v = OctreeCamera::NONE;

    if ( foundvisible )
    {
        v = OctreeCamera::FULL;
    }

    else if ( octant == mOctree )
    {
        v = OctreeCamera::PARTIAL;
    }

    else
    {
        AABox box;
        octant -> _getCullBounds( &box );
        v = camera -> getVisibility( box );
    }


    // if the octant is visible, or if it's the root node...
    if ( v != OctreeCamera::NONE )
    {

        //Add stuff to be rendered;
        Octree::NodeList::iterator it = octant -> mNodes.begin();

        if ( mShowBoxes )
        {
            mBoxes.push_back( octant->getWireBoundingBox() );
        }

        bool vis = true;

        while ( it != octant -> mNodes.end() )
        {
            OctreeNode * sn = *it;

            // if this octree is partially visible, manually cull all
            // scene nodes attached directly to this level.

            if ( v == OctreeCamera::PARTIAL )
                vis = camera -> isIntersect( sn -> getSpaceAABB() );

            if ( vis )
            {

                mNumObjects++;
                sn -> _addToRenderQueue(camera, queue, onlyShadowCasters, visibleBounds );

                mVisible.push_back( sn );

                if ( mNodeView )
                    queue -> add( sn->getGeometry() );

                // check if the scene manager or this node wants the bounding box shown.
                if (sn->isAABBView() || mShowBoundingBoxes)
                    sn->queueAABB(queue);
            }

            ++it;
        }

        Octree* child;
        bool childfoundvisible = (v == OctreeCamera::FULL);
        if ( (child = octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

        if ( (child = octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
            walkOctree( camera, queue, child, visibleBounds, childfoundvisible, onlyShadowCasters );

    }

}

// --- non template versions
void _findNodes( const AABox &t, list< SpaceNode * >::type &list, SpaceNode *exclude, bool full, Octree *octant )
{

	if ( !full )
	{
		AABox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersects( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	Octree::NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		OctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersects( t, on -> getSpaceAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	Octree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void _findNodes( const Sphere &t, list< SpaceNode * >::type &list, SpaceNode *exclude, bool full, Octree *octant )
{

	if ( !full )
	{
		AABox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersects( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	Octree::NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		OctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersects( t, on -> getSpaceAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	Octree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}


void _findNodes( const PlaneSpace &t, list< SpaceNode * >::type &list, SpaceNode *exclude, bool full, Octree *octant )
{

	if ( !full )
	{
		AABox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersects( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	Octree::NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		OctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersects( t, on -> getSpaceAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	Octree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void _findNodes( const Ray &t, list< SpaceNode * >::type &list, SpaceNode *exclude, bool full, Octree *octant )
{

	if ( !full )
	{
		AABox obox;
		octant -> _getCullBounds( &obox );

		Intersection isect = intersects( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}


	Octree::NodeList::iterator it = octant -> mNodes.begin();

	while ( it != octant -> mNodes.end() )
	{
		OctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}

			else
			{
				Intersection nsect = intersects( t, on -> getSpaceAABB() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}

		}

		++it;
	}

	Octree* child;

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant -> mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

}

void OctreeSceneManager::findNodesIn( const AABox &box, list< SpaceNode * >::type &list, SpaceNode *exclude )
{
    _findNodes( box, list, exclude, false, mOctree );
}

void OctreeSceneManager::findNodesIn( const Sphere &sphere, list< SpaceNode * >::type &list, SpaceNode *exclude )
{
    _findNodes( sphere, list, exclude, false, mOctree );
}

void OctreeSceneManager::findNodesIn( const PlaneSpace &volume, list< SpaceNode * >::type &list, SpaceNode *exclude )
{
    _findNodes( volume, list, exclude, false, mOctree );
}

void OctreeSceneManager::findNodesIn( const Ray &r, list< SpaceNode * >::type &list, SpaceNode *exclude )
{
    _findNodes( r, list, exclude, false, mOctree );
}

void OctreeSceneManager::resize( const AABox &box )
{
    list< SpaceNode * >::type nodes;
    list< SpaceNode * >::type ::iterator it;

    _findNodes( mOctree->mBox, nodes, 0, true, mOctree );

    N_delete mOctree;

    mOctree = N_new Octree( 0 );
    mOctree->mBox = box;

	const Vector3 min = box.getMin();
	const Vector3 max = box.getMax();
	mOctree->mHalfSize = ( max - min ) * 0.5f;

    it = nodes.begin();

    while ( it != nodes.end() )
    {
        OctreeNode * on = static_cast < OctreeNode * > ( *it );
        on -> setOctant( 0 );
        _updateOctreeNode( on );
        ++it;
    }
}

bool OctreeSceneManager::setProperty(PropertyID key, const void * val )
{
    if ( key == "Size" )
    {
        resize( * static_cast < const AABox * > ( val ) );
        return true;
    }

    else if ( key == "Depth" )
    {
        mMaxDepth = * static_cast < const int * > ( val );
		// copy the box since resize will delete mOctree and reference won't work
		AABox box = mOctree->mBox;
        resize(box);
        return true;
    }

    else if ( key == "ShowOctree" )
    {
        mShowBoxes = * static_cast < const bool * > ( val );
        return true;
    }
    return SpaceManager::setProperty( key, val );
}

bool OctreeSceneManager::getProperty(PropertyID key, void * val)
{
    if(key == "Size")
    {
        AABox * b = static_cast<AABox *>(val);
        b->setMin(mOctree->mBox.getMin());
        b->setMax(mOctree->mBox.getMax());
        return true;
    }

    else if(key == "Depth")
    {
        * static_cast < int * > ( val ) = mMaxDepth;
        return true;
    }

    else if ( key == "ShowOctree" )
    {

        * static_cast < bool * > ( val ) = mShowBoxes;
        return true;
    }

    return SpaceManager::getProperty( key, val );
}

void OctreeSceneManager::destroyAllCreateObj(void)
{
    SpaceManager::destroyAllCreateObj();
    init(mBox, mMaxDepth);
}
//---------------------------------------------------------------------
AABSpaceQuery * OctreeSceneManager::createAABBQuery(const AABox& box, Nmark mark)
{
    OctreeAxisAlignedBoxSpaceQuery * q = N_new OctreeAxisAlignedBoxSpaceQuery(this);
    q->setBox(box);
    q->setQueryMask(mark);
    return q;
}
//---------------------------------------------------------------------
SphereSpaceQuery * OctreeSceneManager::createSphereQuery(const Sphere & sphere, Nmark mark)
{
    OctreeSphereSpaceQuery* q = N_new OctreeSphereSpaceQuery(this);
    q->setSphere(sphere);
    q->setQueryMask(mark);
    return q;
}
//---------------------------------------------------------------------
PlaneSpaceQuery * OctreeSceneManager::createPlaneSpaceQuery(const PlaneSpaceList & volumes, Nmark mark)
{
    OctreePlaneSpaceQuery * q = N_new OctreePlaneSpaceQuery(this);
    q->setSpaceList(volumes);
    q->setQueryMask(mark);
    return q;
}
//---------------------------------------------------------------------
RaySpaceQuery * OctreeSceneManager::createRayQuery(const Ray & ray, Nmark mark)
{
    OctreeRaySpaceQuery * q = N_new OctreeRaySpaceQuery(this);
    q->setRay(ray);
    q->setQueryMask(mark);
    return q;
}
//---------------------------------------------------------------------
AreaSpaceQuery * OctreeSceneManager::createIntersectQuery(Nmark mark)
{
    // Octree implementation performs WORSE for < 500 objects
    // TODO: optimise it so it's better in all cases
    //OctreeIntersectionSpaceQuery* q = N_new OctreeIntersectionSpaceQuery(this);
    AreaSpaceQuery* q = N_new AreaSpaceQuery(this);
    q->setQueryMask(mark);
    return q;
}
//-----------------------------------------------------------------------
const String OctreeSceneManagerFactory::FACTORY_TYPE_NAME = "OctreeSceneManager";
//-----------------------------------------------------------------------
const SpaceTypeVerbose & OctreeSceneManagerFactory::getVerboseImpl() const
{
	mVerbose.mTypeID = FACTORY_TYPE_NAME;
	mVerbose.mVerbose = "Scene manager organising the scene on the basis of an octree.";
	mVerbose.mMark = SpaceTypeVerbose::ST_Normal | SpaceTypeVerbose::ST_Intersect; // support all types
    return mVerbose;
}
//-----------------------------------------------------------------------
SpaceManager * OctreeSceneManagerFactory::create(const String & name)
{
	return N_new OctreeSceneManager(name);
}
//-----------------------------------------------------------------------
void OctreeSceneManagerFactory::destroy(SpaceManager * obj)
{
	N_delete obj;
}
//-----------------------------------------------------------------------

}
