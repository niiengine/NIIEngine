/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
PCZCamera.cpp  -  description
-----------------------------------------------------------------------------
begin                : Wed Feb 21 2007
author               : Eric Cha
email                : ericc@xenopi.com
-----------------------------------------------------------------------------
*/

#include <OgreMath.h>
#include <OgreAABox.h>
#include <OgreRoot.h>

#include "OgreSceneNode.h"
#include "OgrePCZCamera.h"
#include "OgrePCZFrustum.h"
#include "OgrePortal.h"

namespace Ogre
{
    PCZCamera::PCZCamera( const String& name, SpaceManager* sm ) : Camera( name, sm )
    {
        mBox.mMinimum.x = -0.1;
        mBox.mMinimum.y = -0.1;
        mBox.mMinimum.z = -0.1;

        mBox.mMaximum.x = 0.1;
        mBox.mMaximum.y = 0.1;
        mBox.mMaximum.z = 0.1;
        mExtraCullingFrustum.setUseOriginPlane(true);
    }

    PCZCamera::~PCZCamera()
    {
    }

	const AABox & PCZCamera::getAABB(void) const
	{
		return mBox;
	}

    // this version checks against extra culling planes
    bool PCZCamera::isIntersect(const AABox & bound) const
    {
        // Null boxes always invisible
        if(bound.isNull())
            return false;

        // Make any pending updates to the calculated frustum planes
        updateFrustumPlanes();

		// check extra culling planes
        bool extraResults;
        extraResults = mExtraCullingFrustum.isIntersect(bound);
        if(!extraResults)
        {
            return false;
        }

		// check "regular" camera frustum
		bool regcamresults = Camera::isIntersect(bound);

		if(!regcamresults)
		{
			// culled by regular culling planes
			return regcamresults;
		}
		return true;
   }

	/* A 'more detailed' check for visibility of an AAB.  This function returns
	  none, partial, or full for visibility of the box.  This is useful for
	  stuff like Octree leaf culling */
	PCZCamera::Visibility PCZCamera::getVisibility(const AABox & bound)
	{
		// Null boxes always invisible
		if(bound.isNull())
			return NONE;

		bool all_inside = true;
		for(int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if(plane == Frustum::PT_Far && mFarExtent == 0)
				continue;

			// This updates frustum planes and deals with cull frustum
			Plane::PositionType side = getClipPlane(plane).getSide(bound);
			if(side == Plane::PT_Negative)
                return NONE;
			// We can't return now as the box could be later on the negative side of a plane.
			if(side == Plane::PT_Merge)
                all_inside = false;
		}

        switch(mExtraCullingFrustum.getVisibility(bound))
        {
        case PCZFrustum::NONE:
            return NONE;
        case PCZFrustum::PARTIAL:
            return PARTIAL;
        case PCZFrustum::FULL:
            break;
        }

		if(all_inside)
			return FULL;
		else
			return PARTIAL;
	}

    /* isIntersect() function for portals */
    // NOTE: Everything needs to be updated spatially before this function is
    //       called including portal corners, frustum planes, etc.
    bool PCZCamera::isIntersect(PortalBase * portal) const
    {
		// if portal isn't enabled, it's not visible
		if (!portal->getEnabled()) return false;

        // check the extra frustum first
        if (!mExtraCullingFrustum.isIntersect(portal))
        {
            return false;
        }

		// if portal is of type AABB or Sphere, then use simple bound check against planes
		if (portal->getType() == PortalBase::PORTAL_TYPE_AABB)
		{
			AABox aabb;
            aabb.setMin(portal->getDerivedCorner(0));
            aabb.setMax(portal->getDerivedCorner(1));
			return Camera::isIntersect(aabb);
		}
		else if (portal->getType() == PortalBase::PORTAL_TYPE_SPHERE)
		{
			return Camera::isIntersect(portal->getDerivedSphere());
		}

		// only do this check if it's a portal. (anti portal doesn't care about facing)
		if (portal->getTypeMark() == PortalFactory::FACTORY_TYPE_FLAG)
		{
			// check if the portal norm is facing the camera
			Vector3 cameraToPortal = portal->getDerivedCP() - getSpacePos();
			Vector3 portalDirection = portal->getSpaceDirection();
			NIIf dotProduct = cameraToPortal.dotProduct(portalDirection);
			if ( dotProduct > 0 )
			{
				// portal is faced away from camera
				return false;
			}
		}

        // check against regular frustum planes
        bool visible_flag;
		if (mCull)
		{
            // For each frustum plane, see if all points are on the negative side
            // If so, object is not visible
			// NOTE: We skip the NEAR plane (plane #0) because Portals need to
			// be visible no matter how close you get to them.

            for (int plane = 1; plane < 6; ++plane)
            {
                // set the visible flag to false
                visible_flag = false;
                // Skip far plane if infinite view frustum
                if (plane == Frustum::PT_Far && mFarExtent == 0)
                    continue;

                // we have to check each corner of the portal
                for (int corner = 0; corner < 4; corner++)
                {
                    Plane::PositionType side = mCull->getClipPlane(plane).getSide(portal->getDerivedCorner(corner));
                    if (side != Plane::PT_Negative)
                    {
                        visible_flag = true;
						break;
                    }
                }
                // if the visible_flag is still false, then this plane
                // culled all the portal points
                if (visible_flag == false)
                {
                    // ALL corners on negative side therefore out of view
                    return false;
                }
            }
		}
		else
		{
            // Make any pending updates to the calculated frustum planes
            Frustum::updateFrustumPlanes();

            // For each frustum plane, see if all points are on the negative side
            // If so, object is not visible
			// NOTE: We skip the NEAR plane (plane #0) because Portals need to
			// be visible no matter how close you get to them.
			// BUGBUG: This can cause a false positive situation when a portal is
			// behind the camera but close.  This could be fixed by having another
			// culling plane at the camera location with normal same as camera direction.
            for (int plane = 1; plane < 6; ++plane)
            {
                // set the visible flag to false
                visible_flag = false;
                // Skip far plane if infinite view frustum
                if (plane == Frustum::PT_Far && mFarExtent == 0)
                    continue;

                // we have to check each corner of the portal
                for (int corner = 0; corner < 4; corner++)
                {
                    Plane::PositionType side = mFrustumPlanes[plane].getSide(portal->getDerivedCorner(corner));
                    if (side != Plane::PT_Negative)
                    {
                        visible_flag = true;
						break;
                    }
                }
                // if the visible_flag is still false, then this plane
                // culled all the portal points
                if (visible_flag == false)
                {
                    // ALL corners on negative side therefore out of view
                    return false;
                }
            }
		}
        // no plane culled all the portal points and the norm
        // was facing the camera, so this portal is visible
        return true;
    }

	/// Sets the type of projection to use (orthographic or perspective).
	void PCZCamera::setProjType(Frustum::Type pt)
	{
		mExtraCullingFrustum.setProjType(pt);
		Camera::setProjType(pt);
	}

    /* Update function (currently used for making sure the origin stuff for the
        extra culling frustum is up to date */
    void PCZCamera::update(void)
    {
        // make sure the extra culling frustum origin stuff is up to date
		if (mProjType == Frustum::T_Perspective)
		//if (!mCustomViewMatrix)
		{
			mExtraCullingFrustum.setUseOriginPlane(true);
			mExtraCullingFrustum.setOrigin(getSpacePos());
			mExtraCullingFrustum.setOriginPlane(getSpaceDirection(), getSpacePos());
		}
		else
		{
			// In ortho mode, we don't want to cull things behind camera.
			// This helps for back casting which is useful for texture shadow projection on directional light.
			mExtraCullingFrustum.setUseOriginPlane(false);
		}
    }

    // calculate extra culling planes from portal and camera
    // origin and add to list of extra culling planes
	// NOTE: returns 0 if portal was completely culled by existing planes
	//		 returns > 0 if culling planes are added (# is planes added)
	int PCZCamera::addPortalCullingPlanes(PortalBase* portal)
    {
        // add the extra culling planes from the portal
        return mExtraCullingFrustum.addPortalCullingPlanes(portal);
    }

    // remove extra culling planes created from the given portal
	// NOTE: This should only be used during visibility traversal (backing out of a recursion)
	void PCZCamera::removePortalCullingPlanes(PortalBase* portal)
    {
        mExtraCullingFrustum.removePortalCullingPlanes(portal);
    }

	// remove all extra culling planes
    void PCZCamera::removeAllExtraCullingPlanes(void)
    {
        mExtraCullingFrustum.removeAllCullingPlanes();
    }

}





