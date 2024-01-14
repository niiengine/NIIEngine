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
PCZCamera.h  -  description
-----------------------------------------------------------------------------
begin                : Wed Feb 21 2007
author               : Eric Cha
email                : ericc@xenopi.com
Code Style Update	 :
-----------------------------------------------------------------------------
*/

#ifndef PCZCAMERA_H
#define PCZCAMERA_H

#include <OgreCamera.h>
#include "OgrePCPlane.h"
#include "OgrePortalBase.h"
#include "OgrePCZFrustum.h"
#include "OgrePCZPrerequisites.h"

namespace Ogre
{
    #define MAX_EXTRA_CULLING_PLANES    40

    class PCZone;
    /** Specialized viewpoint from which an PCZone Scene can be rendered.
    @remarks
    */
    class _OgrePCZPluginExport PCZCamera : public Camera
    {
    public:
		/** Visibility types */
		enum Visibility
		{
			NONE,
			PARTIAL,
			FULL
		};

        PCZCamera(const String & name, SpaceManager * sm);
        ~PCZCamera();

        /** Overridden: Retrieves the local axis-aligned bounding box for this object.
        @remarks This bounding box is in local coordinates.
        */
        virtual const AABox & getAABB() const;

		/* Overridden isIntersect function for aabb */
		virtual bool isIntersect(const AABox & bound) const;

		/* isIntersect() function for portals */
		bool isIntersect(PortalBase * portal) const;

        /** Returns the visiblity of the box
        */
        bool isVisibile(const AABox & bound);

        /** Returns the detailed visiblity of the box
		*/
		PCZCamera::Visibility getVisibility(const AABox & bound);

        /* Update function (currently used for making sure the origin stuff for the
           extra culling frustum is up to date */
        void update();

		/// Sets the type of projection to use (orthographic or perspective).
		void setProjType(Type pt);

		// calculate extra culling planes from portal and camera
		// origin and add to list of extra culling planes
		int addPortalCullingPlanes(PortalBase * portal);

		// remove extra culling planes created from the given portal
		void removePortalCullingPlanes(PortalBase * portal);

		// remove all extra culling planes
        void removeAllExtraCullingPlanes();
    protected:
		AABox mBox;
        PCZFrustum mExtraCullingFrustum;
    };
}
#endif