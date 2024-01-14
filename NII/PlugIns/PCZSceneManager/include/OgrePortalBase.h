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
PortalBase.h  -  PortalBase is the base class for Portal and AntiPortal.

*/

#ifndef PORTALBASE_H
#define PORTALBASE_H

#include "OgrePCZPrerequisites.h"
#include "OgrePCZSceneNode.h"
#include "OgreMovableObject.h"
#include "OgreAABox.h"
#include "OgreCapsule.h"
#include "OgreSphere.h"

namespace Ogre
{
	class PCZSceneNode;

	/** PortalBase - Base class to Portal and AntiPortal classes. */
	class _OgrePCZPluginExport PortalBase : public SpaceObj
	{
	public:
		enum PORTAL_TYPE
		{
			PORTAL_TYPE_QUAD,
			PORTAL_TYPE_AABB,
			PORTAL_TYPE_SPHERE
		};

		PortalBase(SpaceID sid, const PORTAL_TYPE type = PORTAL_TYPE_QUAD);
		virtual ~PortalBase();

		/** Retrieves the axis-aligned bounding box for this object in world coordinates. */
		virtual const AABox  & getSpaceAABB(bool derive = false) const;

		/** Retrieves the worldspace bounding sphere for this object. */
		virtual const Sphere & getSpaceBS(bool derive = false) const;

		/** Set the SpaceNode the Portal is associated with */
		void setNode(SpaceNode * sn);

		/** Set the current home zone of the portal */
		void setCurrentHomeZone(PCZone * zone);

		/** Set the zone this portal should be moved to */
		void setNewHomeZone(PCZone* zone);

		/** Set the local coordinates of one of the portal corners */
		void setCorner(int index, const Vector3 & point);

		/** Set the local coordinates of all of the portal corners */
		void setCorners(const Vector3 * corners);

		/** Set the "inward/outward norm" direction of AAB or SPHERE portals
			NOTE: Z = "outward" norm, INV_Z = "inward" norm
			NOTE: Remember, Portal norms always point towards the zone they are "in".
		*/
		void setDirection(const Vector3 & d)
		{
			switch(mType)
			{
			default:
			case PORTAL_TYPE_QUAD:
				N_EXCEPT(Exception::ERR_INVALIDPARAMS,
					"Cannot setDirection on a Quad type portal");
				break;
			case PORTAL_TYPE_AABB:
			case PORTAL_TYPE_SPHERE:
				if(d != Vector3::Z && d != Vector3::INV_Z)
				{
					N_EXCEPT(Exception::ERR_INVALIDPARAMS,
						"Valid parameters are Vector3::Z or Vector3::INV_Z");
					return;
				}
				mDirection = d;
				break;
			}
		}
		/** Calculate the local direction and radius of the portal */
		void calcDirectionAndRadius() const;

		/** get the type of portal */
		PORTAL_TYPE getType() const {return mType;}
		/** Retrieve the radius of the portal (calculates if necessary for quad portals) */
		NIIf getRadius() const;

		/** Get the Zone the Portal is currently "in" */
		PCZone* getCurrentHomeZone()
		{ return mCurrentHomeZone; }
		/** Get the Zone the Portal should be moved to */
		PCZone* getNewHomeZone()
		{ return mNewHomeZone; }

		/** Get the coordinates of one of the portal corners in local space */
		const Vector3& getCorner(int index) const
		{ return mCorners[index]; }
		/** Get the direction vector of the portal in local space */
		const Vector3& getDirection() const
		{ return mDirection; }

		/** Get the derived (world) coordinates of one of the portal corners */
		const Vector3& getDerivedCorner(int index) const
		{ return mDerivedCorners[index]; }
		/** Get the direction of the portal in world coordinates */
		const Vector3& getSpaceDirection() const
		{ return mDerivedDirection; }
		/** Get the position (centerpoint) of the portal in world coordinates */
		const Vector3& getDerivedCP() const
		{ return mDerivedCP; }
		/** Get the sphere centered on the derived CP of the portal in world coordinates */
		const Sphere& getDerivedSphere() const
		{ return mDerivedSphere; }
		/** Get the portal plane in world coordinates */
		const Plane& getDerivedPlane() const
		{ return mDerivedPlane; }

		/** Get the previous position (centerpoint) of the portal in world coordinates */
		const Vector3& getPrevDerivedCP() const
		{ return mPrevDerivedCP; }
		/** Get the previous portal plane in world coordinates */
		const Plane& getPrevDerivedPlane() const
		{ return mPrevDerivedPlane; }

		/** Update the derived values */
		void updateDerivedValues() const;
		/** Adjust the portal so that it is centered and oriented on the given node */
		void adjustNodeToMatch(SpaceNode* node);
		/** enable the portal */
		void setEnabled(bool value)
		{ mEnabled = value; }
		/** check if portal is enabled */
		bool getEnabled() const {return mEnabled;}


		enum PortalIntersectResult
		{
			NO_INTERSECT,
			INTERSECT_NO_CROSS,
			INTERSECT_BACK_NO_CROSS,
			INTERSECT_CROSS
		};

		/** check if portal intersects an aab */
		bool intersects(const AABox & aab);

		/** check if portal intersects an sphere */
		bool intersects(const Sphere & sphere);

		/** check if portal intersects a plane bounded volume */
		bool intersects(const PlaneSpace & pbv);

		/** check if portal intersects a ray */
		bool intersects(const Ray & ray);

		/** check for intersection between portal & scenenode (also determines
		 * if scenenode crosses over portal
		 */
		PortalIntersectResult intersects(PCZSceneNode* sn);

		/** check if portal crossed over portal */
		bool crossedPortal(const PortalBase* otherPortal);
		/** check if portal touches another portal */
		bool closeTo(const PortalBase* otherPortal);

		/** @copydoc SpaceObj::getAABB. */
		const AABox& getAABB() const;

		/** @copydoc SpaceObj::getCenterRange. */
		NIIf getCenterRange() const
		{ return getRadius(); }

		/** @copydoc SpaceObj::queue. */
		void queue(RenderQueue * queue)
		{ /* Draw debug info if needed? */ }

		/** @copydoc SpaceObj::query. */
		void query(GeometryQuery * query)
		{ }

		/** Called when attached to a scene node. */
		void attach(Node * parent, bool tag = false)
		{
			SpaceObj::attach(parent, tag);
			mDerivedUpToDate = false;
		}

		/** Called when scene node moved. */
		void _notify()
		{
			updateDerivedValues();
			mWasMoved = true;
		}

		/** Returns true if portal needs update. */
		bool needUpdate();

		/** Returns an updated capsule of the portal for intersection test. */
		const Capsule& getCapsule() const;

		/** Returns an updated AAB of the portal for intersection test. */
		const AABox& getAAB();

	protected:
		// Type of portal (quad, aabb, or sphere)
		PORTAL_TYPE mType;
		/// Zone this portal is currently owned by (in)
		PCZone * mCurrentHomeZone;
		///zone to transfer this portal to
		PCZone * mNewHomeZone;
		/// Corners of the portal - coordinates are relative to the sceneNode
		// NOTE: there are 4 corners if the portal is a quad type
		//       there are 2 corners if the portal is an AABB type
		//       there are 2 corners if the portal is a sphere type (center and point on sphere)
		Vector3 * mCorners;
		/// Direction ("Norm") of the portal -
		// NOTE: For a Quad portal, determined by the 1st 3 corners.
		// NOTE: For AABB & SPHERE portals, we only have "inward" or "outward" cases.
		//       To indicate "outward", the Direction is Z
		//       to indicate "inward", the Direction is INV_Z
		mutable Vector3 mDirection;
		/// Radius of the sphere enclosing the portal
		// NOTE: For aabb portals, this value is the distance from the center of the aab to a corner
		mutable NIIf mRadius;
		// Local Centerpoint of the portal
		mutable Vector3 mLocalCP;
		/// Derived (world coordinates) Corners of the portal
		// NOTE: there are 4 corners if the portal is a quad type
		//       there are 2 corners if the portal is an AABB type (min corner & max corner)
		//       there are 2 corners if the portal is a sphere type (center and point on sphere)
		Vector3 * mDerivedCorners;
		/// Derived (world coordinates) direction of the portal
		// NOTE: Only applicable for a Quad portal
		mutable Vector3 mDerivedDirection;
		/// Derived (world coordinates) of portal (center point)
		mutable Vector3 mDerivedCP;
		/// Sphere of the portal centered on the derived CP
		mutable Sphere mDerivedSphere;
		/// Derived (world coordinates) Plane of the portal
		// NOTE: Only applicable for a Quad portal
		mutable Plane mDerivedPlane;
		/// Previous frame portal cp (in world coordinates)
		mutable Vector3 mPrevDerivedCP;
		/// Previous frame derived plane
		// NOTE: Only applicable for a Quad portal
		mutable Plane mPrevDerivedPlane;
		/// flag indicating whether or not local values are up-to-date
		mutable bool mLocalsUpToDate;
		/// flag indicating whether or not derived values are up-to-date
		mutable bool mDerivedUpToDate;
		// previous world transform
		mutable Matrix4 mPrevWorldTransform;
		// flag defining if portal is enabled or disabled.
		bool mEnabled;
		// cache of portal's capsule.
		mutable Capsule mPortalCapsule;
		// cache of portal's AAB that contains the bound of portal movement.
		mutable AABox mPortalAAB;
		// cache of portal's previous AAB.
		mutable AABox mPrevPortalAAB;
		// cache of portal's local AAB.
		mutable AABox mLocalPortalAAB;
		// defined if portal was moved previously.
		mutable bool mWasMoved;
	};

	/** Factory object for creating Portal instances */
	class _OgrePCZPluginExport PortalBaseFactory : public SpaceObjFactory
	{
	protected:
		/** get the portal type from name value pair. */
		PortalBase::PORTAL_TYPE getPortalType(const PropertyData * params);

	};

}

#endif
