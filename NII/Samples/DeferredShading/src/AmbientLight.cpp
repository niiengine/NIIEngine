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

#include "AmbientLight.h"
#include "GeomUtils.h"
#include "OgreMaterialManager.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"

using namespace Ogre;

AmbientLight::AmbientLight()
{
	setRenderGroup(RQG_FarEnd);

    N_Only(GBuffer).create(mRenderOp.mVertex);
	mRenderOp.mIndex = 0;
	GeomUtils::createQuad(mRenderOp.mVertex);

	mRenderOp.mType = GeometryRaw::OT_TRIANGLE_STRIP;
	mRenderOp.remove(GRT_Index);

	// Set bounding
	setAABB(AABox(-10000,-10000,-10000,10000,10000,10000));
	mRadius = 15000;

	mMatPtr = N_Only(Material).getByName("DeferredShading/AmbientLight");
	assert(mMatPtr.isNull()==false);
	mMatPtr->load();

    //This shader needs to be aware if its running under OpenGL or DirectX.
    //NIIf depthFactor = (Root::getSingleton().getRender()->getName() ==
    //    "OpenGL Rendering Subsystem") ? 2.0 : 1.0;
    // GpuProgramParam * param;
    // mMatPtr->get(0)->get(0)->getProgram().get(0, ST_FS, NULL, param);
    // param->set("depthFactor", depthFactor);
}

AmbientLight::~AmbientLight()
{
	// need to release Index and Vertex created for renderable
	delete mRenderOp.mIndex;
	delete mRenderOp.mVertex;
}

/** @copydoc SpaceObj::getCenterRange */
NIIf AmbientLight::getCenterRange() const
{
	return mRadius;

}
/** @copydoc GeometryObj::distanceSquared */
NIIf AmbientLight::distanceSquared(const Camera *) const
{
	return 0.0;
}

const MaterialPtr & AmbientLight::getMaterial(void) const
{
	return mMatPtr;
}

void AmbientLight::getMatrix(Ogre::Matrix4 * out, NCount & count) const
{
	*out = Matrix4::IDENTITY;
    count = 1;
}

void AmbientLight::updateFromCamera(Ogre::Camera * camera)
{
	NII::ShaderFusion * sf = getMaterial()->getLOD();
	NII::Vector3f farCorner = camera->getViewMatrix(false) * camera->getClipPlaneIntersect()[4];

	for(NCount i = 0; i < sf->getCount(); ++i)
	{
		NII::ShaderCh * pass = sf->get(i);
		// get the vertex shader parameters
		NII::GpuProgramParam * params;
        pass->getProgram().get(0, ST_VS, NULL, params);
        if(params->getDefine() != 0)
        {
            // set the camera's far-top-right corner
            if(params->getDefine()->get("farCorner"))
                params->set("farCorner", farCorner);
        }
		pass->getProgram().get(0, ST_FS, NULL, params);
        if(params->getDefine() != 0)
        {
            if(params->getDefine()->get("farCorner"))
                params->set("farCorner", farCorner);
        }
	}
}
