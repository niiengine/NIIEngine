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
#include "OgreShaderFFPRenderStateBuilder.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderGenerator.h"
#include "OgreShaderRenderState.h"
#include "OgreShaderFFPTransform.h"
#include "OgreShaderFFPLighting.h"
#include "OgreShaderFFPColour.h"
#include "OgreShaderFFPTexturing.h"
#include "OgreShaderFFPFog.h"
#include "OgrePass.h"
#include "OgreSceneManager.h"
#include "OgreLogManager.h"
#include "OgreShaderFFPRenderState.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    template<> 
    RTShader::FFPRenderStateBuilder* Singleton<RTShader::FFPRenderStateBuilder>::msSingleton = 0;
namespace RTShader 
{
    //-----------------------------------------------------------------------
    FFPRenderStateBuilder* FFPRenderStateBuilder::getSingletonPtr()
    {
        assert( msSingleton );  
        return msSingleton;
    }
    //-----------------------------------------------------------------------
    FFPRenderStateBuilder& FFPRenderStateBuilder::getSingleton()
    {
        assert( msSingleton );  
        return ( *msSingleton );
    }
    //-----------------------------------------------------------------------------
    FFPRenderStateBuilder::FFPRenderStateBuilder()
    {
        

    }
    //-----------------------------------------------------------------------------
    FFPRenderStateBuilder::~FFPRenderStateBuilder()
    {
        

    }
    //-----------------------------------------------------------------------------
    bool FFPRenderStateBuilder::initialize()
    {
        SubRenderStateFactory* curFactory;

        curFactory = N_new FFPTransformFactory;	
        ShaderGenerator::getSingleton().addSubRenderStateFactory(curFactory);
        mFFPSubRenderStateFactoyList.push_back(curFactory);

        curFactory = N_new FFPColourFactory;	
        ShaderGenerator::getSingleton().addSubRenderStateFactory(curFactory);
        mFFPSubRenderStateFactoyList.push_back(curFactory);

        curFactory = N_new FFPLightingFactory;
        ShaderGenerator::getSingleton().addSubRenderStateFactory(curFactory);
        mFFPSubRenderStateFactoyList.push_back(curFactory);

        curFactory = N_new FFPTexturingFactory;
        ShaderGenerator::getSingleton().addSubRenderStateFactory(curFactory);
        mFFPSubRenderStateFactoyList.push_back(curFactory);

        curFactory = N_new FFPFogFactory;	
        ShaderGenerator::getSingleton().addSubRenderStateFactory(curFactory);
        mFFPSubRenderStateFactoyList.push_back(curFactory);

        return true;
    }
    //-----------------------------------------------------------------------------
    void FFPRenderStateBuilder::finalize()
    {
        SubRenderStateFactoryIterator it;

        for (it = mFFPSubRenderStateFactoyList.begin(); it != mFFPSubRenderStateFactoyList.end(); ++it)
        {
            ShaderGenerator::getSingleton().removeSubRenderStateFactory(*it);		
            N_delete *it;		
        }
        mFFPSubRenderStateFactoyList.clear();
    }
    //-----------------------------------------------------------------------------
    void FFPRenderStateBuilder::buildRenderState(ShaderGenerator::SGPass* sgPass, TargetRenderState* renderState)
    {
        renderState->reset();

        // Build transformation sub state.
        buildFFPSubRenderState(FFP_TRANSFORM, FFPTransform::Type, sgPass, renderState);	

        // Build colour sub state.
        buildFFPSubRenderState(FFP_COLOUR, FFPColour::Type, sgPass, renderState);

        // Build lighting sub state.
        buildFFPSubRenderState(FFP_LIGHTING, FFPLighting::Type, sgPass, renderState);

        // Build texturing sub state.
        buildFFPSubRenderState(FFP_TEXTURING, FFPTexturing::Type, sgPass, renderState);	
        
        // Build fog sub state.
        buildFFPSubRenderState(FFP_FOG, FFPFog::Type, sgPass, renderState);
        
        // Resolve colour stage flags.
        resolveColourStageFlags(sgPass, renderState);
    }
    //-----------------------------------------------------------------------------
    void FFPRenderStateBuilder::buildFFPSubRenderState(int subRenderStateOrder, 
        const String & subRenderStateType, ShaderGenerator::SGPass * sgPass, 
            TargetRenderState * renderState)
    {
        SubRenderState * subRenderState;

        subRenderState = sgPass->getCustomFFPSubState(subRenderStateOrder);

        if(subRenderState == NULL)	
        {
            subRenderState = ShaderGenerator::getSingleton().createSubRenderState(subRenderStateType);		
        }

        if(subRenderState->preAddToRenderState(renderState, sgPass->getSrcPass(), sgPass->getDstPass()))
        {
            renderState->addSubRenderStateInstance(subRenderState);
        }
        else
        {		
            ShaderGenerator::getSingleton().destroySubRenderState(subRenderState);				
        }
    }
    //-----------------------------------------------------------------------------
    void FFPRenderStateBuilder::resolveColourStageFlags( ShaderGenerator::SGPass* sgPass, TargetRenderState* renderState )
    {
        const SubRenderStateList& subRenderStateList = renderState->getTemplateSubRenderStateList();
        FFPColour* colourSubState = NULL;

        // Find the colour sub state.
        for (SubRenderStateListConstIterator it=subRenderStateList.begin(); it != subRenderStateList.end(); ++it)
        {
            SubRenderState* curSubRenderState = *it;

            if (curSubRenderState->getType() == FFPColour::Type)
            {
                colourSubState = static_cast<FFPColour*>(curSubRenderState);
                break;
            }
        }
        
        for (SubRenderStateListConstIterator it=subRenderStateList.begin(); it != subRenderStateList.end(); ++it)
        {
            SubRenderState* curSubRenderState = *it;

            // Add vertex shader specular lighting output in case of specular enabled.
            if (curSubRenderState->getType() == FFPLighting::Type)
            {
                colourSubState->addResolveStageMask(FFPColour::SF_VS_OUTPUT_DIFFUSE);

                ShaderCh* srcPass = sgPass->getSrcPass();

                if(srcPass->getColour(ShaderCh::SOT_Get)->getShininess() > 0.0 && 
                    srcPass->getColour(ShaderCh::SOT_Get)->getSpecular() != Colour::Black)
                {
                    colourSubState->addResolveStageMask(FFPColour::SF_VS_OUTPUT_SPECULAR);				
                }	
                break;
            }
        }
    }
}
}
#endif