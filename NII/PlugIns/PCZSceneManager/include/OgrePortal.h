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
Portal.h  -  Portals are special constructs which which are used to connect
			 two Zones in a PCZScene.  Portals are defined by 4 coplanr
             corners and a direction.  Portals are contained within Zones and
             are essentially "one way" connectors.  Objects and entities can
			 use them to travel to other Zones, but to return, there must be
			 a corresponding Portal which connects back to the original zone
			 from the new zone.

-----------------------------------------------------------------------------
begin                : Thu Feb 22 2007
author               : Eric Cha
email                : ericc@xenopi.com
Code Style Update	 : Apr 5, 2007
-----------------------------------------------------------------------------
*/

#ifndef PORTAL_H
#define PORTAL_H

#include "OgrePortalBase.h"

namespace NII
{
	/** Portal datastructure for connecting zones. 
    @version NIIEngine 3.0.0
    */
	class _OgrePCZPluginExport Portal : public PortalBase
	{
	public:
		Portal(SpaceID sid, const PORTAL_TYPE type = PORTAL_TYPE_QUAD);
		virtual ~Portal();

		void setTargetZone(PCZone * zone);

		/** Get the Zone the Portal connects to */
		PCZone * getTargetZone();

		/** Set the target portal pointer */
		void setTargetPortal(Portal * portal);

		/** Get the connected portal (if any) */
		Portal * getTargetPortal();

		/// @copydoc SpaceObj::getFactoryID
		FactoryID getFactoryID() const;
	protected:
		///connected Zone
		PCZone * mTargetZone;
		/** Matching Portal in the target zone (usually in same world space
			as this portal, but pointing the opposite direction)
		*/
		Portal * mTargetPortal;
	};

	/** Factory object for creating Portal instances
    @version NIIEngine 3.0.0
    */
	class _OgrePCZPluginExport PortalFactory : public PortalBaseFactory
	{
	public:
		PortalFactory();
		~PortalFactory();

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
		void destroy(SpaceObj * obj);

		/** Return true here as we want to get a unique type flag.
        @version NIIEngine 3.0.0
        */
		bool requestTypeFlags() const
		{
            return true;
        }

        /**
        @version NIIEngine 3.0.0
        */
		SpaceID getID() const
		{
            return "Portal";
        }
        
		static unsigned long FACTORY_TYPE_FLAG;
	};
}
#endif