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
#include "OgreShaderFFPColour.h"
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"

namespace Ogre 
{
namespace RTShader 
{
    /************************************************************************/
    /*                                                                      */
    /************************************************************************/
    String FFPColour::Type = "FFP_Colour";

    //-----------------------------------------------------------------------
    FFPColour::FFPColour()
    {
        mResolveStageFlags	= SF_PS_OUTPUT_DIFFUSE;
    }
    //-----------------------------------------------------------------------
    const String& FFPColour::getType() const
    {
        return Type;
    }
    //-----------------------------------------------------------------------
    int	FFPColour::getExecutionOrder() const
    {
        return FFP_COLOUR;
    }
    //-----------------------------------------------------------------------
    bool FFPColour::resolveParameters(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Program* psProgram = programSet->getCpuFragmentProgram();
        Function* vsMain   = vsProgram->getEntryPointFunction();
        Function* psMain   = psProgram->getEntryPointFunction();	

        if (mResolveStageFlags & SF_VS_INPUT_DIFFUSE)
            mVSInputDiffuse  = vsMain->resolveInputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
        
        if (mResolveStageFlags & SF_VS_INPUT_SPECULAR)
            mVSInputSpecular = vsMain->resolveInputParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
        
        // Resolve VS color outputs if have inputs from vertex stream.
        if (mVSInputDiffuse.get() != NULL || mResolveStageFlags & SF_VS_OUTPUT_DIFFUSE)		
            mVSOutputDiffuse = vsMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);								

        if (mVSInputSpecular.get() != NULL || mResolveStageFlags & SF_VS_OUTPUT_SPECULAR)		
            mVSOutputSpecular = vsMain->resolveOutputParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);			

        // Resolve PS color inputs if have inputs from vertex shader.
        if (mVSOutputDiffuse.get() != NULL || mResolveStageFlags & SF_PS_INPUT_DIFFUSE)		
            mPSInputDiffuse = psMain->resolveInputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);

        if (mVSOutputSpecular.get() != NULL || mResolveStageFlags & SF_PS_INPUT_SPECULAR)		
            mPSInputSpecular = psMain->resolveInputParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);


        // Resolve PS output diffuse color.
        if (mResolveStageFlags & SF_PS_OUTPUT_DIFFUSE)
        {
            mPSOutputDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
            if (mPSOutputDiffuse.get() == NULL)
                return false;
        }

        // Resolve PS output specular color.
        if (mResolveStageFlags & SF_PS_OUTPUT_SPECULAR)
        {
            mPSOutputSpecular = psMain->resolveOutputParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
            if (mPSOutputSpecular.get() == NULL)
                return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool FFPColour::resolveDependencies(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Program* psProgram = programSet->getCpuFragmentProgram();

        vsProgram->addDependency(FFP_LIB_COMMON);
        psProgram->addDependency(FFP_LIB_COMMON);

        return true;
    }
    //-----------------------------------------------------------------------
    bool FFPColour::addFunctionInvocations(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Program* psProgram = programSet->getCpuFragmentProgram();
        Function* vsMain   = vsProgram->getEntryPointFunction();
        Function* psMain   = psProgram->getEntryPointFunction();	
        FunctionInvocation* curFuncInvocation = NULL;	
        int internalCounter;

        
        // Create vertex shader colour invocations.
        ParameterPtr vsDiffuse;
        ParameterPtr vsSpecular;
        internalCounter = 0;	
        if (mVSInputDiffuse.get() != NULL)
        {
            vsDiffuse = mVSInputDiffuse;
        }
        else
        {
            vsDiffuse = vsMain->resolveLocalParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_CONSTRUCT, FFP_VS_COLOUR, internalCounter++);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(vsDiffuse, Operand::OPS_OUT);
            vsMain->addAtomInstance(curFuncInvocation);
        }

        if (mVSOutputDiffuse.get() != NULL)
        {
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_VS_COLOUR, internalCounter++);
            curFuncInvocation->pushOperand(vsDiffuse, Operand::OPS_IN);
            curFuncInvocation->pushOperand(mVSOutputDiffuse, Operand::OPS_OUT);
            vsMain->addAtomInstance(curFuncInvocation);
        }
        
        if (mVSInputSpecular.get() != NULL)
        {
            vsSpecular = mVSInputSpecular;		
        }
        else
        {
            vsSpecular = vsMain->resolveLocalParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);

            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_CONSTRUCT, FFP_VS_COLOUR, internalCounter++);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(vsSpecular, Operand::OPS_OUT);
            vsMain->addAtomInstance(curFuncInvocation);
        }

        if (mVSOutputSpecular.get() != NULL)
        {
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_VS_COLOUR, internalCounter++);
            curFuncInvocation->pushOperand(vsSpecular, Operand::OPS_IN);
            curFuncInvocation->pushOperand(mVSOutputSpecular, Operand::OPS_OUT);
            vsMain->addAtomInstance(curFuncInvocation);
        }
        
        

        // Create fragment shader colour invocations.
        ParameterPtr psDiffuse;
        ParameterPtr psSpecular;
        internalCounter = 0;
        
        // Handle diffuse colour.
        if (mPSInputDiffuse.get() != NULL)
        {
            psDiffuse = mPSInputDiffuse;				
        }
        else
        {
            psDiffuse = psMain->resolveLocalParameter(Parameter::SPS_COLOR, 0, Parameter::SPC_COLOR_DIFFUSE, GDT_Float4X);
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_CONSTRUCT, FFP_PS_COLOUR_BEGIN, internalCounter++);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(1.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(psDiffuse, Operand::OPS_OUT);
            psMain->addAtomInstance(curFuncInvocation);
        }

        // Handle specular colour.
        if (mPSInputSpecular.get() != NULL)
        {
            psSpecular = mPSInputSpecular;		
        }
        else
        {
            psSpecular = psMain->resolveLocalParameter(Parameter::SPS_COLOR, 1, Parameter::SPC_COLOR_SPECULAR, GDT_Float4X);
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_CONSTRUCT, FFP_PS_COLOUR_BEGIN, internalCounter++);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(ParameterFactory::createConstParamFloat(0.0), Operand::OPS_IN);
            curFuncInvocation->pushOperand(psSpecular, Operand::OPS_OUT);
            psMain->addAtomInstance(curFuncInvocation);
        }

        // Assign diffuse colour.
        if (mPSOutputDiffuse.get() != NULL)
        {	
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_PS_COLOUR_BEGIN, internalCounter++);
            curFuncInvocation->pushOperand(psDiffuse, Operand::OPS_IN);
            curFuncInvocation->pushOperand(mPSOutputDiffuse, Operand::OPS_OUT);		
            psMain->addAtomInstance(curFuncInvocation);
        }

        // Assign specular colour.
        if (mPSOutputSpecular.get() != NULL)
        {
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, FFP_PS_COLOUR_BEGIN, internalCounter++);
            curFuncInvocation->pushOperand(psSpecular, Operand::OPS_IN);
            curFuncInvocation->pushOperand(mPSOutputSpecular, Operand::OPS_OUT);		
            psMain->addAtomInstance(curFuncInvocation);
        }

        // Add specular to out colour.
        internalCounter = 0;
        if (mPSOutputDiffuse.get() != NULL && psSpecular.get() != NULL)
        {
            curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ADD, FFP_PS_COLOUR_END, internalCounter++);
            curFuncInvocation->pushOperand(mPSOutputDiffuse, Operand::OPS_IN,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
            curFuncInvocation->pushOperand(psSpecular, Operand::OPS_IN,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
            curFuncInvocation->pushOperand(mPSOutputDiffuse, Operand::OPS_OUT,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
            psMain->addAtomInstance(curFuncInvocation);
        }	

        return true;
    }
    //-----------------------------------------------------------------------
    void FFPColour::write(const SubRenderState & rhs)
    {
        const FFPColour & rhsColour = static_cast<const FFPColour &>(rhs);

        setResolveStageFlags(rhsColour.mResolveStageFlags);
    }
    //-----------------------------------------------------------------------
    bool FFPColour::preAddToRenderState(const RenderState * renderState, 
        ShaderCh * srcPass, ShaderCh * dstPass)
    {
        Nmark trackColour = srcPass->getColour(ShaderCh::SOT_Get)->getColourTracking();

        if(trackColour != 0)			
            addResolveStageMask(FFPColour::SF_VS_INPUT_DIFFUSE);
        
        return true;
    }
    //-----------------------------------------------------------------------
    const String & FFPColourFactory::getType() const
    {
        return FFPColour::Type;
    }
    //-----------------------------------------------------------------------
    SubRenderState * FFPColourFactory::createInstance(ScriptCompiler * compiler, 
        PropertyScriptNode * prop, ShaderCh * pass, SGScriptAnalyze * translator)
    {
        if (prop->name == "colour_stage")
        {
            if(prop->values.size() == 1)
            {
                String modelType;

                if(false == SGScriptAnalyze::get(prop->values.front(), &modelType))
                {
                    compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
                    return NULL;
                }

                if (modelType == "ffp")
                {
                    return createOrRetrieveInstance(translator);
                }
            }		
        }

        return NULL;
    }
    //-----------------------------------------------------------------------
    void FFPColourFactory::writeInstance(MaterialSerializer * ser, 
        SubRenderState * subRenderState, ShaderCh * srcPass, ShaderCh * dstPass)
    {
        ser->writeAttribute(4, "colour_stage");
        ser->writeValue("ffp");
    }
    //-----------------------------------------------------------------------
    SubRenderState*	FFPColourFactory::createInstanceImpl()
    {
        return N_new FFPColour;
    }
}
}

#endif