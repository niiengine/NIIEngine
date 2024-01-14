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
#ifndef OWL_VOLUMERENDERABLE
#define OWL_VOLUMERENDERABLE
#include <OgrePrerequisites.h>
#include <NiiRenderObj.h>

/** Direct Volume Rendering.
 	TODO: LOD: reduce number of slices in distance
 	TODO: option to generate normals for lighting
 	@author W.J. van der Laan
*/
class VolumeRenderable: public NII::RenderObj 
{
public:
	VolumeRenderable(size_t nSlices, float size, const Ogre::String & texture);
	~VolumeRenderable();
	
	// copydetails NII::RenderObj::_notify
	void _notify(NII::Camera * cam);
	void getMatrix(NII::Matrix4 * out, NCount & count) const;
	
	/**
	 * Retrieves ratios of the origin-centered bounding sphere for this
	 * object.
	 */
	Ogre::NIIf getCenterRange() const;
	
	/**
	 * Returns the camera-relative squared depth of this renderable.
	 */
	Ogre::NIIf distanceSquared(const Ogre::Camera*) const;
protected:
	void init();

	size_t mSlices;
	float mSize;
	float mRadius;
	Ogre::Matrix3 mFakeOrientation;
	Ogre::String mTexture;
	Ogre::ShaderChTextureUnit *mUnit;
};
#endif
