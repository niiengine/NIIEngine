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

#include "GBufferSchemeHandler.h"

#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

using namespace NII;

const String GBufferSchemeHandler::NORMAL_MAP_PATTERN = "normal";

ShaderFusion * GBufferSchemeHandler::miss(Material * originalMaterial,
    SchemeID schemeName, const GeometryObj * rend, unsigned short lodIndex)
{
	Ogre::MaterialManager & matMgr = Ogre::MaterialManager::getSingleton();
	SchemeID curSchemeName = matMgr.getCurrent();
	matMgr.setCurrent(0);
	ShaderFusion * originalTechnique = originalMaterial->getLOD(rend, lodIndex);
	matMgr.setCurrent(curSchemeName);

	ShaderFusion * gBufferTech = originalMaterial->createTechnique();
	gBufferTech->removeAll();
	gBufferTech->setSchemeID(schemeName);

	ShaderFusion * noGBufferTech = originalMaterial->createTechnique();
	noGBufferTech->removeAll();
	noGBufferTech->setSchemeID("NoGBuffer");

	for(NCount i = 0; i < originalTechnique->getCount(); i++)
	{
		ShaderCh * originalPass = originalTechnique->get(i);
		PassProperties props = inspectPass(originalPass, lodIndex, rend);

		if(!props.isDeferred)
		{
			//Just copy the technique so it gets rendered regularly
			ShaderCh * clonePass = noGBufferTech->create();
			*clonePass = *originalPass;
			continue;
		}

		ShaderCh * newPass = gBufferTech->create();
		MaterialGenerator::Perm perm = getPermutation(props);

		const Ogre::MaterialPtr & templateMat = mMaterialGenerator.getMaterial(perm);

		//We assume that the GBuffer technique contains only one pass. But its true.
		*newPass = *(templateMat->get(0)->get(0));
		fillPass(newPass, originalPass, props);
	}

	return gBufferTech;
}

bool GBufferSchemeHandler::checkNormalMap(ShaderChTextureUnit * tus,
    GBufferSchemeHandler::PassProperties & props)
{
	bool isNormal = false;
	Ogre::String lowerCaseAlias = tus->getName();
	Ogre::StringUtil::toLowerCase(lowerCaseAlias);
	if(lowerCaseAlias.find(NORMAL_MAP_PATTERN) != Ogre::String::npos)
	{
		isNormal = true;
	}
	else
	{
		Ogre::String lowerCaseName = tus->getResourceID();
		Ogre::StringUtil::toLowerCase(lowerCaseName);
		if(lowerCaseName.find(NORMAL_MAP_PATTERN) != Ogre::String::npos)
		{
			isNormal = true;
		}
	}

	if(isNormal)
	{
		if(props.normalMap == 0)
		{
			props.normalMap = tus;
		}
		else
		{
			N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
				"Multiple normal map patterns matches");
		}
	}
	return isNormal;
}

GBufferSchemeHandler::PassProperties GBufferSchemeHandler::inspectPass(
	Pass * pass, unsigned short lodIndex, const GeometryObj * rend)
{
	PassProperties props;

	//TODO : Use renderable to indicate wether this has skinning.
	//Probably use same const cast that render uses.
	if(pass->getProgram().isExist(ST_VS))
	{
        GpuProgram * prog;
        pass->getProgram().get(0, ST_VS, prog);
		props.isSkinned = prog->isVertexMatrixFusion();
	}
	else
	{
		props.isSkinned = false;
	}

	for(NCount i = 0; i < pass->getTexture().getCount(); ++i)
	{
		ShaderChTextureUnit * tus = pass->getTexture().get(i);
		if(!checkNormalMap(tus, props))
		{
			props.regularTextures.push_back(tus);
		}
		if(tus->getEffects().size() > 0)
		{
			props.isDeferred = false;
		}

	}

    if(pass->getColour(ShaderCh::SOT_Get)->getDiffuse() != Colour::White)
    {
        props.hasDiffuseColour = true;
    }

    //Check transparency
    if(pass->getBlend(ShaderCh::SOT_Get)->mDstFactor != Ogre::CF_ZERO)
    {
        //TODO : Better ways to do this
        props.isDeferred = false;
    }
	return props;
}

MaterialGenerator::Perm GBufferSchemeHandler::getPermutation(const PassProperties& props)
{
	MaterialGenerator::Perm perm = 0;
	switch (props.regularTextures.size())
	{
	case 0:
		perm |= GBufferMaterialGenerator::GBP_NO_TEXTURES;

		if (props.normalMap != 0)
		{
			perm |= GBufferMaterialGenerator::GBP_ONE_TEXCOORD;
		}
		else
		{
			perm |= GBufferMaterialGenerator::GBP_NO_TEXCOORDS;
		}
		break;
	case 1:
		perm |= GBufferMaterialGenerator::GBP_ONE_TEXTURE;
		perm |= GBufferMaterialGenerator::GBP_ONE_TEXCOORD;
		break;
	case 2:
		perm |= GBufferMaterialGenerator::GBP_TWO_TEXTURES;
		//TODO : When do we use two texcoords?
		perm |= GBufferMaterialGenerator::GBP_ONE_TEXCOORD;
		break;
	case 3:
		perm |= GBufferMaterialGenerator::GBP_THREE_TEXTURES;
		perm |= GBufferMaterialGenerator::GBP_ONE_TEXCOORD;
		break;
	default:
		N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
			"Can not generate G-Buffer materials for '>3 regular-texture' objects",
			"GBufferSchemeHandler::inspectPass");
	}

	if (props.isSkinned)
	{
		perm |= GBufferMaterialGenerator::GBP_SKINNED;
	}

	if (props.normalMap != 0)
	{
		perm |= GBufferMaterialGenerator::GBP_NORMAL_MAP;
	}

    if (props.hasDiffuseColour)
    {
        perm |= GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR;
    }
	return perm;
}

void GBufferSchemeHandler::fillPass(ShaderCh * gBufferPass, ShaderCh * originalPass,
    const PassProperties & props)
{
	//Reference the correct textures. Normal map first!
	int texUnitIndex = 0;
	if(props.normalMap != 0)
	{
		*(gBufferPass->getTexture().get(texUnitIndex)) = *(props.normalMap);
		texUnitIndex++;
	}

	for(size_t i = 0; i < props.regularTextures.size(); ++i)
	{
		*(gBufferPass->getTexture().get(texUnitIndex)) = *(props.regularTextures[i]);
		texUnitIndex++;
	}

    *(gBufferPass->setColour(ShaderCh::SOT_Set)) = *(originalPass->getColour(ShaderCh::SOT_Get));

    gBufferPass->setCullingMode(originalPass->getCullingMode());
    gBufferPass->setLight(false);
}
