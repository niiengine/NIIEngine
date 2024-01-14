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

#include "OgreQuake3Shader.h"
#include "OgreSceneManager.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreTextureUnitState.h"
#include "OgreMath.h"
#include "OgreLogManager.h"
#include "OgreTextureManager.h"
#include "OgreRoot.h"
#include "OgreMaterialManager.h"

namespace Ogre {


    //-----------------------------------------------------------------------
    Quake3Shader::Quake3Shader(const String& name)
    {
        mName = name;
        numPasses = 0;
        deformFunc = DEFORM_FUNC_NONE;
        farbox = false;
        skyDome = false;
        flags = 0;
        fog = false;
        cullMode = SCM_BACK;

    }
    //-----------------------------------------------------------------------
    Quake3Shader::~Quake3Shader()
    {
    }
    //-----------------------------------------------------------------------
    MaterialPtr Quake3Shader::createAsMaterial(int lightmapNumber)
    {
		String matName;
		StringUtil::StrStreamType str;
        String resourceGroup = ResourceSchemeManager::getSingleton().getWorldResourceGroupName();

        str << mName << "#" << lightmapNumber;
		matName = str.str();

        MaterialPtr mat = N_Only(Material).create(matName, resourceGroup);

		NII::ShaderCh * ogrePass = mat->get(0)->get(0);

        LogManager::getSingleton().logMessage("Using Q3 shader " + mName, LML_CRITICAL);
        for(int p = 0; p < numPasses; ++p)
        {
            ShaderChTextureUnit * t;
            // Create basic texture
            if(pass[p].textureName == "$lightmap")
            {
                StringUtil::StrStreamType str2;
				str2 << "@lightmap" << lightmapNumber;
                t = ogrePass->getTexture().create(str2.str());
            }
            // Animated texture support
            else if(pass[p].animNumFrames > 0)
            {
                NIIf sequenceTime = pass[p].animNumFrames / pass[p].animFps;
                /* Pre-load textures
                   We need to know if each one was loaded OK since extensions may change for each
                   Quake3 can still include alternate extension filenames e.g. jpg instead of tga
                   Pain in the arse - have to check for each frame as letters<n>.tga for example
                   is different per frame!
                */
                for(unsigned int alt = 0; alt < pass[p].animNumFrames; ++alt)
                {
                    if(!ResourceSchemeManager::getSingleton().isExist(
                        resourceGroup, pass[p].frames[alt]))
                    {
                        // Try alternate extension
                        pass[p].frames[alt] = getAlternateName(pass[p].frames[alt]);
                        if(!ResourceSchemeManager::getSingleton().isExist(
                            resourceGroup, pass[p].frames[alt]))
                        {
                            // stuffed - no texture
                            continue;
                        }
                    }

                }
                t = ogrePass->getTexture().create("");
                t->setAni(pass[p].frames, pass[p].animNumFrames, sequenceTime);
            }
            else
            {
                // Quake3 can still include alternate extension filenames e.g. jpg instead of tga
                // Pain in the arse - have to check for failure
                if(!ResourceSchemeManager::getSingleton().isExist(
                    resourceGroup, pass[p].textureName))
                {
                    // Try alternate extension
                    pass[p].textureName = getAlternateName(pass[p].textureName);
                    if(!ResourceSchemeManager::getSingleton().isExist(
                        resourceGroup, pass[p].textureName))
                    {
                        // stuffed - no texture
                        continue;
                    }
                }
                t = ogrePass->getTexture().create(pass[p].textureName);
            }
            // Blending
            if(p == 0)
            {
                // scene blend
                N_Only(Material).setBlend(mat, pass[p].blendSrc, pass[p].blendDest);
                if(mat->isTransparent())
                    N_Only(Material).setDepthWrite(mat, false);

                t->getColourBlend()->setColourBlend(TBM_Src_Replace_Dst);
				// Alpha mode
				ogrePass->setAlphaRejectSettings(pass[p].alphaFunc, pass[p].alphaVal);
            }
            else
            {
                if (pass[p].customBlend)
                {
                    // Fallback for now
                    t->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
                }
                else
                {
                    // simple layer blend
                    t->getColourBlend()->setColourBlend(pass[p].blend);
                }
				// Alpha mode, prefer 'most alphary'
				CmpMode currFunc = ogrePass->getColour(ShaderCh::SOT_Get)->getAlphaReject();
				unsigned char currVal = ogrePass->getColour(ShaderCh::SOT_Get)->getValue();
				if (pass[p].alphaFunc > currFunc ||
					(pass[p].alphaFunc == currFunc && pass[p].alphaVal < currVal))
				{
					ogrePass->setAlphaRejectSettings(pass[p].alphaFunc, pass[p].alphaVal);
				}
            }
            // Tex coords
            if(pass[p].texGen == TEXGEN_BASE)
            {
                t->setCoordSet(0);
            }
            else if(pass[p].texGen == TEXGEN_LIGHTMAP)
            {
                t->setCoordSet(1);
            }
            else if(pass[p].texGen == TEXGEN_ENVIRONMENT)
            {
                t->setEnvMap(true);
                t->setEnvMapType(ShaderChTextureUnit::ENM_Planar);
            }
            // Tex mod
            // Scale
            t->setUScale(pass[p].tcModScale[0]);
            t->setVScale(pass[p].tcModScale[1]);
            // Procedural mods
            // Custom - don't use mod if generating environment
            // Because I do env a different way it look horrible
            if(pass[p].texGen != TEXGEN_ENVIRONMENT)
            {
                if(pass[p].tcModRotate)
                {
                    t->setRotateAnimation(pass[p].tcModRotate);
                }
                if (pass[p].tcModScroll[0] || pass[p].tcModScroll[1])
                {
                    if (pass[p].tcModTurbOn)
                    {
                        // Turbulent scroll
                        if(pass[p].tcModScroll[0])
                        {
                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, t, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(true, WFT_SINE,
                                pass[p].tcModTurb[0], pass[p].tcModTurb[3], pass[p].tcModTurb[2],
                                    pass[p].tcModTurb[1], true);
                            t->addAnimation(TTT_U_OFT, val, func);
                        }
                        if (pass[p].tcModScroll[1])
                        {
                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, t, false, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(true, WFT_SINE,
                                pass[p].tcModTurb[0], pass[p].tcModTurb[3], pass[p].tcModTurb[2],
                                    pass[p].tcModTurb[1], true);
                            t->addAnimation(TTT_V_OFT, val, func);
                        }
                    }
                    else
                    {
                        // Constant scroll
                        t->setScrollAnimation(pass[p].tcModScroll[0], pass[p].tcModScroll[1]);
                    }
                }
                if(pass[p].tcModStretchWave != SHADER_FUNC_NONE)
                {
                    WaveformType wft = WFT_SINE;
                    switch(pass[p].tcModStretchWave)
                    {
                    case SHADER_FUNC_SIN:
                        wft = WFT_SINE;
                        break;
                    case SHADER_FUNC_TRIANGLE:
                        wft = WFT_TRIANGLE;
                        break;
                    case SHADER_FUNC_SQUARE:
                        wft = WFT_SQUARE;
                        break;
                    case SHADER_FUNC_SAWTOOTH:
                        wft = WFT_SAWTOOTH;
                        break;
                    case SHADER_FUNC_INVERSESAWTOOTH:
                        wft = WFT_INVERSE_SAWTOOTH;
                        break;
                    default:
                        break;
                    }
                    // Create wave-based stretcher
                    DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, t, false, false, true);
                    DataFunc<NIIf, NIIf> * func =
                        N_new WaveformControllerFunction(true, wft, pass[p].tcModStretchParams[3],
                            pass[p].tcModStretchParams[0], pass[p].tcModStretchParams[2],
                                pass[p].tcModStretchParams[1], true);
                    t->addAnimation(TTT_U_SCALE, val, func);

                    DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, t, false, false, false, true);
                    DataFunc<NIIf, NIIf> * func =
                        N_new WaveformControllerFunction(true, wft, pass[p].tcModStretchParams[3],
                            pass[p].tcModStretchParams[0], pass[p].tcModStretchParams[2], pass[p].tcModStretchParams[1], true);
                    t->addAnimation(TTT_V_SCALE, val, func);
                }
            }
            // Address mode
            t->setUAddressing(pass[p].addressMode);
            t->setVAddressing(pass[p].addressMode);
            t->setWAddressing(pass[p].addressMode);
        }
        // Do farbox (create new material)

        // Set culling mode and lighting to defaults
        mat->setCullingMode(CM_None);
        mat->setSysCullingMode(cullMode);
        mat->setLight(false);
        mat->load();
        return mat;
    }
    String Quake3Shader::getAlternateName(const String & texName)
    {
        // Get alternative JPG to TGA and vice versa
        size_t pos;
        String ext, base;

        pos = texName.find_last_of(".");
        ext = texName.substr(pos, 4);
		StringUtil::toLowerCase(ext);
        base = texName.substr(0,pos);
        if(ext == ".jpg")
        {
            return base + ".tga";
        }
        else
        {
            return base + ".jpg";
        }

    }
}
