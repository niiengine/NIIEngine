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

#ifndef _NULLSCHEMEHANDLER_H
#define _NULLSCHEMEHANDLER_H

#include <OgreMaterialManager.h>

/** Class for skipping materials which do not have the scheme defined
 */
class NullSchemeHandler : public NII::MaterialMatchListener
{
public:
	/** @copydoc MaterialMatchListener::miss */
	virtual NII::ShaderFusion * miss(NII::Material * originalMaterial, SchemeID schemeName,  
        const NII::GeometryObj * rend, unsigned short lodIndex)
	{
		//Creating a technique so the handler only gets called once per material
		NII::ShaderFusion * emptyTech = originalMaterial->createTechnique();
		emptyTech->removeAll();
		emptyTech->setSchemeID(schemeName);
		return emptyTech;
	}
};
#endif