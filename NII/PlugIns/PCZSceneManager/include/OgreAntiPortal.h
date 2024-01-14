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

#ifndef ANTIPORTAL_H
#define ANTIPORTAL_H

#include "OgrePortalBase.h"

namespace Ogre
{
	/** AntiPortal datastructure for occlusion culling.
    */
	class _OgrePCZPluginExport AntiPortal : public PortalBase
	{
	public:
		AntiPortal(SpaceID sid, const PORTAL_TYPE type = PORTAL_TYPE_QUAD);
		virtual ~AntiPortal();

		/** @copydoc SpaceObj::getFactoryID. */
		FactoryID getFactoryID() const;
	};

	/** Factory object for creating AntiPortal instances
    */
	class _OgrePCZPluginExport AntiPortalFactory : public PortalBaseFactory
	{
	public:
		AntiPortalFactory() {}
		~AntiPortalFactory() {}

		static unsigned long FACTORY_TYPE_FLAG;

        /// @copydetails SpaceObjFactory::getID
		SpaceID getID() const
		{
            return "AntiPortal";
        }

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
		void destroy(SpaceObj * obj);

		/** Return true here as we want to get a unique type flag. */
		bool requestTypeFlags() const
		{
            return true;
        }
	};
}
#endif