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

#include "NiiGLSLSeparableProgram.h"

namespace Ogre
{
    GLSLSeparableProgram::GLSLSeparableProgram(GLSLShader* vs, GLSLShader* ts, GLSLShader* ds, GLSLShader* gs, GLSLShader* fs, GLSLShader* cs) :
        GLSLProgram(vs, ts, ds, gs, fs, cs)
    {
    }

    GLSLSeparableProgram::~GLSLSeparableProgram()
    {
        GLCheck(glDeleteProgramPipelines(1, &mGLProgramPipelineHandle));
    }

    void GLSLSeparableProgram::compileAndLink()
    {
        // Ensure no monolithic programs are in use.
        GLCheck(glUseProgram(0));
        GLCheck(glGenProgramPipelines(1, &mGLProgramPipelineHandle));

        loadIndividualProgram(getVertexShader());
        loadIndividualProgram(mDomainShader);
        loadIndividualProgram(mHullShader);
        loadIndividualProgram(mGeometryShader);
        loadIndividualProgram(mFragmentShader);
        loadIndividualProgram(mComputeShader);

        if (mLinked)
        {
            if (mVertexShader && getVertexShader()->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_VERTEX_SHADER_BIT, getVertexShader()->getGLProgramHandle()));
            }
            if (mDomainShader && mDomainShader->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_TESS_EVALUATION_SHADER_BIT, mDomainShader->getGLProgramHandle()));
            }
            if (mHullShader && mHullShader->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_TESS_CONTROL_SHADER_BIT, mHullShader->getGLProgramHandle()));
            }
            if (mGeometryShader && mGeometryShader->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_GEOMETRY_SHADER_BIT, mGeometryShader->getGLProgramHandle()));
            }
            if (mFragmentShader && mFragmentShader->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_FRAGMENT_SHADER_BIT, mFragmentShader->getGLProgramHandle()));
            }
            if (mComputeShader && mComputeShader->isLinked())
            {
                GLCheck(glUseProgramStages(mGLProgramPipelineHandle, GL_COMPUTE_SHADER_BIT, mComputeShader->getGLProgramHandle()));
            }

            // Validate pipeline
            GLCheck(glValidateProgramPipeline(mGLProgramPipelineHandle));
            logObjectInfo( getCombinedName() + String("GLSL program pipeline validation result: "), mGLProgramPipelineHandle );

            //            if (getGLSupport()->checkGpuApi("GL_KHR_debug") || gl3wIsSupported(4, 3))
            //                glObjectLabel(GL_PROGRAM_PIPELINE, mGLProgramPipelineHandle, 0,
            //                                 (mVertexShader->getName() + "/" + mFragmentShader->getName()).c_str());
        }
    }

    void GLSLSeparableProgram::loadIndividualProgram(GLSLShader *program)
    {
        if (program)
        {
            if(program->isLinked())
            {
                // This is for the case where all individual programs have been compiled before (in an another separable program).
                // If we don't do that the program will remain unlinked although all individual programs are actually linked.
                mLinked = true;
            }
            else
            {
                GLint linkStatus = 0;

                uint32 hash = program->getHash();

                GLuint programHandle = program->getGLProgramHandle();

                GLCheck(glProgramParameteri(programHandle, GL_PROGRAM_SEPARABLE, GL_TRUE));
                GLCheck(glProgramParameteri(programHandle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE));

                // Use precompiled program if possible.
                MemStream * cache = N_Only(GpuProgram).getCache(hash);
                if (cache)
                {
                    cache->seek(0);

                    GLenum binaryFormat = 0;
                    cache->read(&binaryFormat, sizeof(GLenum));

                    GLint binaryLength = cache->size() - sizeof(GLenum);

                    GLCheck(glProgramBinary(programHandle, binaryFormat, cache->getData() + sizeof(GLenum), binaryLength));

                    GLCheck(glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus));
                    if (!linkStatus)
                        logObjectInfo("Could not use cached binary " + program->getName(), programHandle);
                }

                // Compilation needed if precompiled program is
                // unavailable or failed to link.
                if (!linkStatus)
                {
                    if( program->getType() == ST_VS )
                        bindFixedAttributes( programHandle );

                    program->attachToProgramObject(programHandle);
                    GLCheck(glLinkProgram(programHandle));
                    GLCheck(glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus));

                    // Binary cache needs an update.
                    cache = 0;
                }

                program->setLinked(linkStatus);
                mLinked = linkStatus;

                if(!mLinked)
                	logObjectInfo( getCombinedName() + String("GLSL program result : "), programHandle );

                if (program->getType() == ST_VS)
                    setVertexMatrixFusion(program->isSkeletalAnimationIncluded());

                // Add the microcode to the cache.
                if (!cache && mLinked && N_Only(GpuProgram).isCacheBinary() )
                {
                    GLint binaryLength = 0;
                    GLCheck(glGetProgramiv(programHandle, GL_PROGRAM_BINARY_LENGTH, &binaryLength));

                    MemStream * newMicrocode = N_Only(GpuProgram).createCache(hash, (unsigned long)binaryLength + sizeof(GLenum));

                    GLCheck(glGetProgramBinary(programHandle, binaryLength, NULL, (GLenum *)newMicrocode->getData(), newMicrocode->getData() + sizeof(GLenum)));
                }
            }
        }
    }

    void GLSLSeparableProgram::activate(void)
    {
        if (!mLinked)
        {
            compileAndLink();

            extractLayoutQualifiers();

            buildGLUniformReferences();
        }

        if (mLinked)
        {
            N_Only(GLSL::GLSLProgram).getStateManager()->bindGLProgramPipeline(mGLProgramPipelineHandle);
        }
    }
    
    void GLSLSeparableProgram::inactivate()
    {
        N_Only(GLSL::GLSLProgram).getStateManager()->bindGLProgramPipeline(0);
    }


    void GLSLSeparableProgram::buildGLUniformReferences(void)
    {
        if (mUniformRefsBuilt)
        {
            return;
        }

        // order must match ShaderType
        GLSLShader* shaders[6] = {getVertexShader(), mFragmentShader, mGeometryShader, mDomainShader, mHullShader, mComputeShader};

        for (int i = 0; i < 6; i++)
        {
            if (shaders[i])
            {
                const GpuParamUnitList* params[6] = {NULL};
                params[i] = &(shaders[i]->getParamDef().mDefineList);
                N_Only(GLSL::GLSLProgram).extractUniforms(shaders[i]->getGLProgramHandle(), params, mGLUniformReferences,
                    mGLAtomicCounterReferences, mGLUniformBufferReferences, mSharedParamsBufferMap, mGLCounterBufferReferences);
            }
        }

        mUniformRefsBuilt = true;
    }


    void GLSLSeparableProgram::updateUniforms(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype)
    {
        // determine if we need to transpose matrices when binding
        bool transpose = GL_TRUE;
        if ((stype == ST_FS && mVertexShader && (!getVertexShader()->getColumnMajorMatrices())) ||
            (stype == ST_VS && mFragmentShader && (!mFragmentShader->getColumnMajorMatrices())) ||
            (stype == ST_GS && mGeometryShader && (!mGeometryShader->getColumnMajorMatrices())) ||
            (stype == ST_TS && mHullShader && (!mHullShader->getColumnMajorMatrices())) ||
            (stype == ST_DS && mDomainShader && (!mDomainShader->getColumnMajorMatrices())) ||
            (stype == ST_CS && mComputeShader && (!mComputeShader->getColumnMajorMatrices())))
        {
            transpose = GL_FALSE;
        }

        GLuint progID = 0;
        GLUniformCache * uniformCache = 0;
        if (stype == ST_VS && getVertexShader())
        {
            progID = getVertexShader()->getGLProgramHandle();
            uniformCache = getVertexShader()->getUniformCache();
        }
        else if (stype == ST_FS && mFragmentShader)
        {
            progID = mFragmentShader->getGLProgramHandle();
            uniformCache = mFragmentShader->getUniformCache();
        }
        else if (stype == ST_GS && mGeometryShader)
        {
            progID = mGeometryShader->getGLProgramHandle();
            uniformCache = mGeometryShader->getUniformCache();
        }
        else if (stype == ST_TS && mHullShader)
        {
            progID = mHullShader->getGLProgramHandle();
            uniformCache = mHullShader->getUniformCache();
        }
        else if (stype == ST_DS && mDomainShader)
        {
            progID = mDomainShader->getGLProgramHandle();
            uniformCache = mDomainShader->getUniformCache();
        }
        else if (stype == ST_CS && mComputeShader)
        {
            progID = mComputeShader->getGLProgramHandle();
            uniformCache = mComputeShader->getUniformCache();
        }
        else
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "invalid program type");
        }

        // Iterate through uniform reference list and update uniform values
        GLUniformReferenceIterator currentUniform = mGLUniformReferences.begin();
        GLUniformReferenceIterator endUniform = mGLUniformReferences.end();
        for (; currentUniform != endUniform; ++currentUniform)
        {
            // Only pull values from buffer it's supposed to be in (vertex or fragment)
            // This method will be called once per shader stage.
            if (stype == currentUniform->mSourceProgType)
            {
                const GpuParamUnit* def = currentUniform->mConstantDef;
                if (def->mGBTMark & mask)
                {
                    GLsizei glArraySize = (GLsizei)def->mUnitCount;

                    bool shouldUpdate = true;

                    switch (def->mDataType)
                    {
                        case GDT_Int:
                        case GDT_Int2X:
                        case GDT_Int3X:
                        case GDT_Int4X:
                        case GDT_Sampler1D:
                        case GDT_SamplerShadow1D:
                        case GDT_Sampler2D:
                        case GDT_SamplerShadow2D:
                        case GDT_SamplerArray2D:
                        case GDT_Sampler3D:
                        case GDT_SamplerCube:
                            shouldUpdate = uniformCache->updateUniform(currentUniform->mLocation, params->getBufferData(def->mMemIndex),
                                                                        static_cast<GLsizei>(def->mUnitSize * def->mUnitCount * sizeof(int)));
                            break;
                        default:
                            shouldUpdate = uniformCache->updateUniform(currentUniform->mLocation, params->getBufferData(def->mMemIndex),
                                                                        static_cast<GLsizei>(def->mUnitSize * def->mUnitCount * sizeof(float)));
                            break;

                    }
                    if(!shouldUpdate)
                    {
                        continue;
                    }

                    // Get the index in the parameter real list
                    switch (def->mDataType)
                    {
                    case GDT_Float:
                        GLCheck(glProgramUniform1fv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float2X:
                        GLCheck(glProgramUniform2fv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float3X:
                        GLCheck(glProgramUniform3fv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float4X:
                        GLCheck(glProgramUniform4fv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X2:
                        GLCheck(glProgramUniformMatrix2fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X3:
                        GLCheck(glProgramUniformMatrix3fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X4:
                        GLCheck(glProgramUniformMatrix4fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int:
                        GLCheck(glProgramUniform1iv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int2X:
                        GLCheck(glProgramUniform2iv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int3X:
                        GLCheck(glProgramUniform3iv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int4X:
                        GLCheck(glProgramUniform4iv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X3:
                        GLCheck(glProgramUniformMatrix2x3fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X4:
                        GLCheck(glProgramUniformMatrix2x4fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X4:
                        GLCheck(glProgramUniformMatrix3x2fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X4:
                        GLCheck(glProgramUniformMatrix3x4fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X2:
                        GLCheck(glProgramUniformMatrix4x2fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X3:
                        GLCheck(glProgramUniformMatrix4x3fv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double:
                        GLCheck(glProgramUniform1dv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double2X:
                        GLCheck(glProgramUniform2dv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double3X:
                        GLCheck(glProgramUniform3dv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double4X:
                        GLCheck(glProgramUniform4dv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X2:
                        GLCheck(glProgramUniformMatrix2dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X3:
                        GLCheck(glProgramUniformMatrix3dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X4:
                        GLCheck(glProgramUniformMatrix4dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X3:
                        GLCheck(glProgramUniformMatrix2x3dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X4:
                        GLCheck(glProgramUniformMatrix2x4dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X2:
                        GLCheck(glProgramUniformMatrix3x2dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X4:
                        GLCheck(glProgramUniformMatrix3x4dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X2:
                        GLCheck(glProgramUniformMatrix4x2dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X3:
                        GLCheck(glProgramUniformMatrix4x3dv(progID, currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt:
                    case GDT_Bool:
                        GLCheck(glProgramUniform1uiv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt2X:
                    case GDT_Bool2X:
                        GLCheck(glProgramUniform2uiv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt3X:
                    case GDT_Bool3X:
                        GLCheck(glProgramUniform3uiv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt4X:
                    case GDT_Bool4X:
                        GLCheck(glProgramUniform4uiv(progID, currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;

                    case GDT_Sampler1D:
                    case GDT_SamplerShadow1D:
                    case GDT_Sampler2D:
                    case GDT_SamplerShadow2D:
                    case GDT_SamplerArray2D:
                    case GDT_Sampler3D:
                    case GDT_SamplerCube:
                    case GDT_SamplerRect:
                        // Samplers handled like 1-element ints
                        GLCheck(glProgramUniform1iv(progID, currentUniform->mLocation, 1, params->getBufferData(def->mMemIndex)));
                        break;
                    default:
                        break;

                    } // End switch
                } // Variability & mask
            } // stype == currentUniform->mSourceProgType

        } // End for
    }


    void GLSLSeparableProgram::updateAtomicCounters(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype)
    {
        // Iterate through the list of atomic counter buffers and update them as needed
        // GLAtomicBufferIterator currentBuffer = mGLAtomicBufferReferences.begin();
        // GLAtomicBufferIterator endBuffer = mGLAtomicBufferReferences.end();

        GLAtomicCounterReferenceIterator currentAtomicCounter = mGLAtomicCounterReferences.begin();
        GLAtomicCounterReferenceIterator endAtomicCounter = mGLAtomicCounterReferences.end();

        for (; currentAtomicCounter != endAtomicCounter; ++currentAtomicCounter)
        {
            if (stype == currentAtomicCounter->mSourceProgType)
            {
                const GpuParamUnit* def = currentAtomicCounter->mConstantDef;
                if (def->mGBTMark & mask)
                {
                    GLsizei glArraySize = (GLsizei)def->mUnitCount;

                    // Get the index in the parameter real list
                    //switch (def->mDataType)

                    GLuint glBinding = currentAtomicCounter->mBinding;
                    GLuint glOffset = currentAtomicCounter->mOffset;

                    // Get the buffer this atomic counter belongs to.
                    //TODO exception handling
                    CounterBuffer * atomic_buffer = mGLCounterBufferReferences[glBinding];

                    // Update the value.
                    atomic_buffer->write(params->getBufferData(def->mMemIndex), glOffset, sizeof(GLuint) * glArraySize);
                }
            }
        }

        // GpuProgramParam::ShareSyncList::const_iterator it, end = sharedParams.end();
        // for (it = sharedParams.begin(); it != end; ++it)
        // {
        //     GpuParamValue * value = params->getValue(it);
        //     for (;currentBuffer != endBuffer; ++currentBuffer)
        //     {
        //         GLStructBuffer* hwGlBuffer = static_cast<GLStructBuffer*>(currentBuffer->get());
        //         // Block name is stored in mSharedParams->mName of GpuSharedParamUsageList items
        //         GLint UniformTransform;
        //         GLCheck(UniformTransform = glGetUniformBlockIndex(mGLProgramHandle, it->getName().c_str()));
        //         GLCheck(glUniformBlockBinding(mGLProgramHandle, UniformTransform, hwGlBuffer->getGLBufferBinding()));

        //         hwGlBuffer->writeData(0, hwGlBuffer->getSizeInBytes(), &value->getBufferData().front());
        //     }
        // }
    }

    void GLSLSeparableProgram::updateUniformBlocks(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype)
    {
        //TODO Support uniform block arrays - need to figure how to do this via material.

        // Iterate through the list of uniform blocks and update them as needed.
        SharedParamsBufferMap::const_iterator currentPair = mSharedParamsBufferMap.begin();
        SharedParamsBufferMap::const_iterator endPair = mSharedParamsBufferMap.end();

        for (; currentPair != endPair; ++currentPair)
        {
            // force const call to get*Pointer
            const GpuParamValue* paramsPtr = currentPair->first;

            //FIXME Possible buffer does not exist if no associated uniform block.
            StructBuffer * hwGlBuffer = currentPair->second;

            if (!paramsPtr->isDirty()) continue;

            //FIXME does not check if current progrtype, or if shared param is active

            GpuParamUnitList::iterator parami, iend = paramsPtr->getDefList().end();
            for (parami = paramsPtr->getDefList().begin(); parami != iend; parami++)
            {
                const GpuParamUnit* param = &parami->second;

                //DataType baseType = GpuParamUnit::getBaseType(param->mDataType);

                const void* dataPtr;

                // NOTE: the naming is backward. this is the logical index
                size_t index =  param->mMemIndex;

                //TODO Maybe move to GpuSharedParams?  Otherwise create bool buffer.
                //switch (baseType)
                //{
                //case BCT_FLOAT:
                //case BCT_INT:
                //case BCT_DOUBLE:
                //case BCT_UINT:
                //case BCT_BOOL:
                    dataPtr = paramsPtr->getBufferData(index);
                //    break;
                //case BCT_SAMPLER:
                //case BCT_SUBROUTINE:
                //default:
                //    continue;
                //}

                // NOTE: the naming is backward. this is the physical offset in bytes
                size_t offset = param->mIndex;
                hwGlBuffer->write(dataPtr, offset, param->mUnitCount * param->mUnitSize * 4);
            }
        }
    }


    void GLSLSeparableProgram::updatePassIterationUniforms(GpuProgramParam * params)
    {
        if (params->getRenderCount() != 0)
        {
            size_t index = params->getRenderCount() - &params->getBufferData()[0];

            GLUniformReferenceIterator currentUniform = mGLUniformReferences.begin();
            GLUniformReferenceIterator endUniform = mGLUniformReferences.end();

            // Need to find the uniform that matches the multi pass entry
            for (;currentUniform != endUniform; ++currentUniform)
            {
                // Get the index in the parameter real list
                if (index == currentUniform->mConstantDef->mMemIndex)
                {
                    GLuint progID = 0;
                    if (mVertexShader && currentUniform->mSourceProgType == ST_VS)
                    {
                        progID = getVertexShader()->getGLProgramHandle();
                    }

                    if (mFragmentShader && currentUniform->mSourceProgType == ST_FS)
                    {
                        progID = mFragmentShader->getGLProgramHandle();
                    }

                    if (mGeometryShader && currentUniform->mSourceProgType == ST_GS)
                    {
                        progID = mGeometryShader->getGLProgramHandle();
                    }

                    if (mDomainShader && currentUniform->mSourceProgType == ST_DS)
                    {
                        progID = mDomainShader->getGLProgramHandle();
                    }

                    if (mHullShader && currentUniform->mSourceProgType == ST_TS)
                    {
                        progID = mHullShader->getGLProgramHandle();
                    }

                    if (mComputeShader && currentUniform->mSourceProgType == ST_CS)
                    {
                        progID = mComputeShader->getGLProgramHandle();
                    }

                    GLCheck(glProgramUniform1fv(progID, currentUniform->mLocation, 1, params->getRenderCount()));

                    // There will only be one multipass entry
                    return;
                }
            }
        }
    }
}
