/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#ifndef __PROCEDURAL_MANUAL_OBJECT_H__
#define __PROCEDURAL_MANUAL_OBJECT_H__

#include <OgreCustomSpaceObj.h>
#include <OgreRenderObj.h>
#include <OgreRenderToVertexBuffer.h>

namespace Ogre
{
	class ProceduralCustomSpaceObj : public RenderObj
	{
	public:
		ProceduralCustomSpaceObj():
            mParentSceneManager(NULL){}
		virtual ~ProceduralCustomSpaceObj() {}

		void setRenderToVertexBuffer(DisplayBuffer * obj)
        {
            mR2vbObject = obj;
        }

		const DisplayBuffer * getRenderToVertexBuffer()
        {
            return mR2vbObject;
        }

		void setCustomSpaceObj(CustomSpaceObj * obj);

		CustomSpaceObj * getCustomSpaceObj() const;

		/** @copydoc RenderObj::_update. */
		void _update(RenderQueue* queue);

		/** @copydoc RenderObj::getFactoryID. */
		FactoryID getFactoryID() const;

		/** @copydoc RenderObj::getGeometry. */
		void getGeometry(GeometryRaw & op);

		//Delegate to the manual object
		NIIf getCenterRange() const
        {
            return mCustomSpaceObj->getCenterRange();
        }

		NIIf distanceSquared(const Ogre::Camera * cam) const
        {
            return mCustomSpaceObj->getGeo(0)->distanceSquared(cam);
        }
	protected:
        SpaceManager * mParentSceneManager;
		CustomSpaceObj* mCustomSpaceObj;
		DisplayBuffer * mR2vbObject;
	};

	class ProceduralCustomSpaceObjFactory : public SpaceObjFactory
	{
    public:
        ProceduralCustomSpaceObjFactory();
        ~ProceduralCustomSpaceObjFactory();

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        /// @copydetails SpaceObjFactory::destroy
        void destroy(SpaceObj * obj);
	};

}
#endif
