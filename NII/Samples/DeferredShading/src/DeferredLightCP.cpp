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

#include "DeferredLightCP.h"

#include "Ogre.h"
using namespace Ogre;

#include "LightMaterialGenerator.h"

//-----------------------------------------------------------------------
DeferredLightRenderOperation::DeferredLightRenderOperation(
	CompositorInstance* instance, const CompositionPass* pass)
{
	mViewport = instance->getChain()->getViewport();

	//Get the names of the GBuffer textures
	const CompositionPass::InputTex& input0 = pass->getInput(0);
	mTexName0 = instance->getTextureInstanceName(input0.name, input0.mrtIndex);
	const CompositionPass::InputTex& input1 = pass->getInput(1);
	mTexName1 = instance->getTextureInstanceName(input1.name, input1.mrtIndex);

	// Create lights material generator
	mLightMaterialGenerator = new LightMaterialGenerator();

	// Create the ambient light
	mAmbientLight = new AmbientLight();
	const MaterialPtr& mat = mAmbientLight->getMaterial();
	mat->load();
}
//-----------------------------------------------------------------------
DLight* DeferredLightRenderOperation::createDLight(Ogre::Light* light)
{
	DLight *rv = new DLight(mLightMaterialGenerator,light);
	mLights[light] = rv;
	return rv;
}
//-----------------------------------------------------------------------
void injectTechnique(SpaceManager * sm, Technique * tech, GeometryObj * geo,
    const Ogre::LightList * lightList)
{
    for(NCount i = 0; i < tech->getCount(); ++i)
	{
		NII::ShaderCh * pass = tech->get(i);
        if(lightList != 0)
		{
            const ShaderCh * ch = sm->applyPass(pass, false, false);

            sm->render(geo, ch, lightList, false);
		}
		else
		{
            const ShaderCh * ch = sm->applyPass(pass, false, false);

            sm->render(geo, ch, LightList(), false);
		}
	}
}
//-----------------------------------------------------------------------
void DeferredLightRenderOperation::execute(SpaceManager * sm, RenderSystem * rs)
{
    Ogre::Camera * cam = mViewport->getCamera();

	mAmbientLight->updateFromCamera(cam);
    ShaderFusion * tech = mAmbientLight->getMaterial()->getLOD();
	injectTechnique(sm, tech, mAmbientLight, 0);

	const LightList & lightList = sm->getLightList();
    for(LightList::const_iterator it = lightList.begin(); it != lightList.end(); it++)
	{
        Light * light = *it;
		Ogre::LightList ll;
		ll.push_back(light);

		//if (++i != 2) continue;
        //if (light->getType() != LT_DIRECTIONAL) continue;
		//if (light->getDiffuse() != Colour::Red) continue;

		LightsMap::iterator dLightIt = mLights.find(light);
		DLight * dLight = 0;
		if(dLightIt == mLights.end())
		{
			dLight = createDLight(light);
		}
		else
		{
			dLight = dLightIt->second;
			dLight->updateFromParent();
		}
		dLight->updateFromCamera(cam);
		tech = dLight->getMaterial()->getLOD();

		//Update shadow texture
		if(dLight->getCastChadows())
		{
			SpaceManager::RenderBase context;
            
            sm->pauseRender(context);
			sm->prepareSTs(cam, mViewport, &ll);
			sm->resumeRender(context);

			ShaderCh * pass = tech->get(0);
			ShaderChTextureUnit * tus = pass->getTexture().get("ShadowMap");
			assert(tus);
			const Texture * shadowTex = sm->getRenderPattern()->getTexture(0);
			if(tus->getTexture() != shadowTex)
			{
				tus->setTexture(shadowTex);
			}
		}
        injectTechnique(sm, tech, dLight, &ll);
	}
}
//-----------------------------------------------------------------------
DeferredLightRenderOperation::~DeferredLightRenderOperation()
{
	for (LightsMap::iterator it = mLights.begin(); it != mLights.end(); ++it)
	{
		delete it->second;
	}
	mLights.clear();

	delete mAmbientLight;
	delete mLightMaterialGenerator;
}
//-----------------------------------------------------------------------
