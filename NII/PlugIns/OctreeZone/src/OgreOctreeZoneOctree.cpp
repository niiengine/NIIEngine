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

#include <OgreOctreeZoneOctree.h>
#include <OgrePCZSpaceNode.h>
#include "OgreOctreeZone.h"

namespace Ogre
{

    /* INTERSECTION UTILITY FUNCTIONS ***/

    enum Intersection
    {
        OUTSIDE=0,
        INSIDE=1,
        INTERSECT=2
    };

    Intersection intersects( const Ray &one, const AABox &two )
    {
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


    /** Checks how the axis aligned box intersects with the plane bounded volume
    */
    Intersection intersects(const PlaneSpace & one, const AABox & two)
    {
        // Null box?
        if(two.isNull())
            return OUTSIDE;

        // For each plane, see if all points are on the negative side
        // If so, object is not visible.
        // If one or more are, it's partial.
        // If all aren't, full
        bool all_inside = true;
        PlaneList::const_iterator i, iend = one.mPlanes.end();
        for(i = one.mPlanes.begin(); i != iend; ++i)
        {
            const Plane & plane = *i;

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

        if ( mndistance.lengthSquared() < sradius &&
                mxdistance.lengthSquared() < sradius )
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
    /***************************************************/

    /** Returns true is the box will fit in a child.
    */
    bool Octree::_isTwiceSize( const AABox & box ) const
    {
        Vector3 halfMBoxSize = mBox.getSize() * 0.5;
        Vector3 boxSize = box.getSize();
        return ((boxSize.x <= halfMBoxSize.x) && (boxSize.y <= halfMBoxSize.y) && (boxSize.z <= halfMBoxSize.z));

    }

    /** It's assumed the the given box has already been proven to fit into
    * a child.  Since it's a loose octree, only the centers need to be
    * compared to find the appropriate node.
    */
    void Octree::_getChildIndexes( const AABox &box, int *x, int *y, int *z ) const
    {
        Vector3 center = mBox.getMax().middle( mBox.getMin() );

        Vector3 ncenter = box.getMax().middle( box.getMin() );

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

    Octree::Octree(PCZone * oz, Octree * parent )
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
	    mZone = oz;
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

    void Octree::_addNode( PCZSpaceNode * n )
    {
        mNodes.insert(n);
        ((OctreeZoneData*)n ->getZoneData(mZone))->setOctant( this );

        //update total counts.
        _ref();

    }

    void Octree::_removeNode( PCZSpaceNode * n )
    {
        mNodes.erase(n);
		((OctreeZoneData*)n ->getZoneData(mZone))->setOctant( 0 );

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

    void Octree::_findNodes(const AABox &t,
                            PCZSpaceNodeList &list,
                            PCZSpaceNode *exclude,
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AABox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersects( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSpaceNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSpaceNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					list.insert( on );
			    }

			    else
			    {
				    Intersection nsect = intersects( t, on -> getSpaceAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						list.insert( on );
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

    void Octree::_findNodes(const Ray &t,
                            PCZSpaceNodeList &list,
                            PCZSpaceNode *exclude,
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AABox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersects( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSpaceNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSpaceNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					list.insert( on );
			    }

			    else
			    {
				    Intersection nsect = intersects( t, on -> getSpaceAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						list.insert( on );
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

    void Octree::_findNodes(const Sphere &t,
                            PCZSpaceNodeList &list,
                            PCZSpaceNode *exclude,
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AABox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersects( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSpaceNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSpaceNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					list.insert( on );
			    }

			    else
			    {
				    Intersection nsect = intersects( t, on -> getSpaceAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						list.insert( on );
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }


    void Octree::_findNodes(const PlaneSpace &t,
                            PCZSpaceNodeList &list,
                            PCZSpaceNode *exclude,
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AABox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersects( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSpaceNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSpaceNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					list.insert( on );
			    }

			    else
			    {
				    Intersection nsect = intersects( t, on -> getSpaceAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						list.insert( on );
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

}
