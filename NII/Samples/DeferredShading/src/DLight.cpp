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

#include "DLight.h"

#include "OgreGBufferManager.h"
#include "OgreCamera.h"
#include "OgreSceneNode.h"
#include "OgreLight.h"
#include "GeomUtils.h"
#include "LightMaterialGenerator.h"
#include "OgreTechnique.h"
#include "OgreSceneManager.h"

#define ENABLE_BIT(mask, flag) (mask) |= (flag)
#define DISABLE_BIT(mask, flag) (mask) &= ~(flag)

using namespace NII;
//-----------------------------------------------------------------------
DLight::DLight(MaterialGenerator * sys, NII::Light * parentLight):
    mParentLight(parentLight),
    bIgnoreWorld(false),
    mGenerator(sys),
    mPermutation(0)
{
	// Set up geometry
	// Allocate render operation
	mRenderOp.mType = GeometryRaw::OT_TRIANGLE_LIST;
	mRenderOp.mIndex = 0;
	mRenderOp.mVertex = 0;
	mRenderOp.add(GRT_Index);

	updateFromParent();
}
//-----------------------------------------------------------------------
DLight::~DLight()
{
	// need to release index and vertex created for renderable
    delete mRenderOp.mIndex;
    delete mRenderOp.mVertex;
}
//-----------------------------------------------------------------------
void DLight::setAttenuation(float c, float b, float a)
{
	// Set Attenuation parameter to shader
	//setCustonParam(3, Vector4(c, b, a, 0));
	float outerRadius = mParentLight->getRange();
	/// There is attenuation? Set material accordingly
	if(c != 1.0f || b != 0.0f || a != 0.0f)
	{
		ENABLE_BIT(mPermutation, LightMaterialGenerator::MI_ATTENUATED);
		if (mParentLight->getType() == LT_POINT)
		{
			//// Calculate radius from Attenuation
			int threshold_level = 10;// difference of 10-15 levels deemed unnoticeable
			float threshold = 1.0f / ((float)threshold_level / 256.0f);

			//// Use quadratic formula to determine outer radius
			c = c - threshold;
			float d = sqrt(b * b - 4 * a * c);
			outerRadius = (-2 * c) / (b + d);
			outerRadius *= 1.2;
		}
	}
	else
	{
		DISABLE_BIT(mPermutation,LightMaterialGenerator::MI_ATTENUATED);
	}

	rebuildGeometry(outerRadius);
}
//-----------------------------------------------------------------------
void DLight::setSpecular(const Colour & col)
{
	//setCustonParam(2, Vector4(col.r, col.g, col.b, col.a));
	/// There is a specular component? Set material accordingly

	if(col.r != 0.0f || col.g != 0.0f || col.b != 0.0f)
		ENABLE_BIT(mPermutation,LightMaterialGenerator::MI_SPECULAR);
	else
		DISABLE_BIT(mPermutation,LightMaterialGenerator::MI_SPECULAR);
}
//-----------------------------------------------------------------------
void DLight::rebuildGeometry(float radius)
{
	//Disable all 3 bits
	DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_POINT);
	DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_SPOTLIGHT);
	DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_DIRECTIONAL);

	switch (mParentLight->getType())
	{
	case LT_DIRECTIONAL:
		createRectangle2D();
        ENABLE_BIT(mPermutation,LightMaterialGenerator::MI_DIRECTIONAL);
		break;
	case LT_POINT:
		/// XXX some more intelligent expression for rings and segments
		createSphere(radius, 10, 10);
		ENABLE_BIT(mPermutation,LightMaterialGenerator::MI_POINT);
		break;
	case LT_SPOT:
		NIIf height = mParentLight->getRange();
		Radian coneRadiusAngle = mParentLight->getSpotlightOuter() / 2;
        NIIf rad = Math::Tan(coneRadiusAngle) * height;
		createCone(rad, height, 20);
		ENABLE_BIT(mPermutation,LightMaterialGenerator::MI_SPOTLIGHT);
		break;
	}
}
//-----------------------------------------------------------------------
void DLight::createRectangle2D()
{
	/// XXX this RenderOp should really be re-used between DLight objects,
	/// not generated every time
	delete mRenderOp.mVertex;
	delete mRenderOp.mIndex;

    N_Only(GBuffer).create(mRenderOp.mVertex);
    mRenderOp.mIndex = 0;

	GeomUtils::createQuad(mRenderOp.mVertex);

    mRenderOp.mType = GeometryRaw::OT_TRIANGLE_STRIP;
    mRenderOp.remove(GRT_Index);

	// Set bounding
    setAABB(AABox(-10000,-10000,-10000,10000,10000,10000));
	mRadius = 15000;
	bIgnoreWorld = true;
}
//-----------------------------------------------------------------------
void DLight::createSphere(float radius, int nRings, int nSegments)
{
	delete mRenderOp.mVertex;
	delete mRenderOp.mIndex;
	mRenderOp.mType = GeometryRaw::OT_TRIANGLE_LIST;
	mRenderOp.mIndex = new IndexData();
    N_Only(GBuffer).create(mRenderOp.mVertex);
	mRenderOp.add(GRT_Index);

	GeomUtils::createSphere(mRenderOp.mVertex, mRenderOp.mIndex
		, radius
		, nRings, nSegments
		, false // no normals
		, false // no texture coordinates
		);

	// Set bounding box and sphere
	setAABB( AABox( Vector3(-radius, -radius, -radius), Vector3(radius, radius, radius) ) );
	mRadius = radius;
	bIgnoreWorld = false;
}
//-----------------------------------------------------------------------
void DLight::createCone(float radius, float height, int nVerticesInBase)
{
	delete mRenderOp.mVertex;
	delete mRenderOp.mIndex;
	mRenderOp.mType = GeometryRaw::OT_TRIANGLE_LIST;
	mRenderOp.mIndex = new IndexData();
    N_Only(GBuffer).create(mRenderOp.mVertex);
	mRenderOp.add(GRT_Index);

	GeomUtils::createCone(mRenderOp.mVertex, mRenderOp.mIndex, radius, height, nVerticesInBase);

	// Set bounding box and sphere
	setAABB(AABox(Vector3(-radius, 0, -radius), Vector3(radius, height, radius)));

	mRadius = radius;
	bIgnoreWorld = false;
}
//-----------------------------------------------------------------------
NIIf DLight::getCenterRange() const
{
	return mRadius;
}
//-----------------------------------------------------------------------
NIIf DLight::distanceSquared(const Camera * cam) const
{
	if(bIgnoreWorld)
	{
		return 0.0f;
	}
	else
	{
		Vector3 dist = cam->getSpacePos() - getSceneNode()->getSpacePos();
		return dist.lengthSquared();
	}
}
//-----------------------------------------------------------------------
const MaterialPtr& DLight::getMaterial(void) const
{
	return mGenerator->getMaterial(mPermutation);
}
//-----------------------------------------------------------------------
void DLight::getMatrix(Matrix4 * xform, NCount & count) const
{
	if(mParentLight->getType() == LT_SPOT)
	{
		Quaternion quat = Vector3::Y.getRotation(mParentLight->getSpaceDirection());
		xform->set(mParentLight->getSpacePos(), Vector3::UNIT, quat);
	}
	else
	{
		xform->set(mParentLight->getSpacePos(), Vector3::UNIT, Quaternion::IDENTITY);
	}
    count = 1;
}
//-----------------------------------------------------------------------
void DLight::updateFromParent()
{
	//TODO : Don't do this unless something changed
	setAttenuation(mParentLight->getAttenuationConstant(),
		mParentLight->getAttenuationLinear(), mParentLight->getAttenuationQuadric());
	setSpecular(mParentLight->getSpecular());

	if (getCastChadows())
	{
		ENABLE_BIT(mPermutation,LightMaterialGenerator::MI_SHADOW_CASTER);
	}
	else
	{
		DISABLE_BIT(mPermutation, LightMaterialGenerator::MI_SHADOW_CASTER);
	}
}
//-----------------------------------------------------------------------
bool DLight::isCameraInsideLight(Ogre::Camera* camera)
{
	switch (mParentLight->getType())
	{
	case Ogre::LT_DIRECTIONAL:
		return false;
	case Ogre::LT_POINT:
		{
		Ogre::NIIf distanceFromLight = camera->getSpacePos()
			.distance(mParentLight->getSpacePos());
		//Small epsilon fix to account for the fact that we aren't a true sphere.
		return distanceFromLight <= mRadius + camera->getNearClipDistance() + 0.1;
		}
	case Ogre::LT_SPOT:
		{
		Ogre::Vector3 lightPos = mParentLight->getSpacePos();
		Ogre::Vector3 lightDir = mParentLight->getSpaceDirection();
		Ogre::Radian attAngle = mParentLight->getSpotlightOuter();

		//Extend the analytic cone's radius by the near clip range by moving its tip accordingly.
		//Some trigonometry needed here.
		Ogre::Vector3 clipRangeFix = -lightDir * (camera->getNearClipDistance() / Ogre::Math::Tan(attAngle/2));
		lightPos = lightPos + clipRangeFix;

		Ogre::Vector3 lightToCamDir = camera->getSpacePos() - lightPos;
		Ogre::NIIf distanceFromLight = lightToCamDir.normalise();

		Ogre::NIIf cosAngle = lightToCamDir.dotProduct(lightDir);
		Ogre::Radian angle = Ogre::Math::ACos(cosAngle);
		//Check whether we will see the cone from our current POV.
		return (distanceFromLight <= (mParentLight->getRange() + clipRangeFix.length()))
			&& (angle <= attAngle);
		}
	default:
		//Please the compiler
		return false;
	}
}
//-----------------------------------------------------------------------
bool DLight::getCastChadows() const
{
	return mParentLight->getManager()->isShadowTechniqueInUse() &&
		mParentLight->isCastShadow() &&
		(mParentLight->getType() == LT_DIRECTIONAL || mParentLight->getType() == LT_SPOT);
}
//-----------------------------------------------------------------------
void DLight::updateFromCamera(Ogre::Camera * camera)
{
	//Set shader params
	const Ogre::MaterialPtr & mat = getMaterial();
	if(mat->getState() != LS_LOADED)
	{
		mat->load();
	}

	NII::ShaderFusion * sf = mat->getLOD();
	NII::Vector3f farCorner = camera->getViewMatrix(false) * camera->getClipPlaneIntersect()[4];

	for(NCount i = 0; i < sf->getCount(); ++i)
	{
		Ogre::Pass * pass = sf->get(i);
		// get the vertex shader parameters
		Ogre::GpuProgramParam * params;
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
		//If inside light geometry, render back faces with CM_GREATER, otherwise normally
		if (mParentLight->getType() == Ogre::LT_DIRECTIONAL)
		{
			pass->setCullingMode(NII::CM_Clockwise);
			pass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);
		}
		else
		{
			pass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(true);
			if(isCameraInsideLight(camera))
			{
				pass->setCullingMode(NII::CM_Anticlockwise);
				pass->setStencil(ShaderCh::SOT_Set)->setDepthCompareFunc(NII::CM_GREATER_EQUAL);
			}
			else
			{
				pass->setCullingMode(NII::CM_Clockwise);
				pass->setStencil(ShaderCh::SOT_Set)->setDepthCompareFunc(NII::CM_LESS_EQUAL);
			}
		}

		Camera shadowCam("ShadowSetupCam", 0);
		shadowCam.setViewport(camera->getViewport());
		//SpaceManager * sm = mParentLight->getManager();
		mParentLight->getCameraSetup()->setupShadow(&shadowCam, sm, camera, camera->getViewport(), mParentLight, 0);

        if(params->getDefine() != 0)
        {
            //Get the shadow camera position
            if(params->getDefine()->get("shadowCamPos"))
            {
                params->set("shadowCamPos", shadowCam.getPos());
            }
            if(params->getDefine()->get("shadowFarClip"))
            {
                params->set("shadowFarClip", shadowCam.getFarClipDistance());
            }
        }
	}
}