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

#include "NiiGLSLMonolithicProgram.h"

namespace NII 
{
    //-----------------------------------------------------------------------
    static GLint getGLGeometryInputPrimitiveType(GeometryRaw::OperationType optype)
    {
        switch (optype)
        {
        case GeometryRaw::OT_POINT_LIST:
            return GL_POINTS;
        case GeometryRaw::OT_LINE_LIST:
        case GeometryRaw::OT_LINE_STRIP:
			return GL_LINES;
        case GeometryRaw::OT_LINE_LIST_ADJ:
        case GeometryRaw::OT_LINE_STRIP_ADJ:
			return GL_LINES_ADJACENCY;
        case GeometryRaw::OT_TRIANGLE_LIST_ADJ:
        case GeometryRaw::OT_TRIANGLE_STRIP_ADJ:
            return GL_TRIANGLES_ADJACENCY;
        default:
        case GeometryRaw::OT_TRIANGLE_LIST:
        case GeometryRaw::OT_TRIANGLE_STRIP:
        case GeometryRaw::OT_TRIANGLE_FAN:
            return GL_TRIANGLES;
		}
    }
    //-----------------------------------------------------------------------
    static GLint getGLGeometryOutputPrimitiveType(GeometryRaw::OperationType optype)
    {
        switch (optype)
        {
        case GeometryRaw::OT_POINT_LIST:
            return GL_POINTS;
        case GeometryRaw::OT_LINE_STRIP:
            return GL_LINE_STRIP;
        default:
        case GeometryRaw::OT_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLSLMonolithicProgram
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLSLMonolithicProgram::GLSLMonolithicProgram(GLSLShader* vs, GLSLShader* ts, GLSLShader* ds, GLSLShader* gs, GLSLShader* fs, GLSLShader* cs)
        : GLSLProgram(vs, ts, ds, gs, fs, cs)
    {
    }
    //-----------------------------------------------------------------------
    GLSLMonolithicProgram::~GLSLMonolithicProgram(void)
    {
        GLCheck(glDeleteProgram(mGLProgramHandle));
        
        delete mUniformCache;
        mUniformCache = 0;
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::activate(void)
    {
        if (!mLinked)
        {
            uint32 hash = getCombinedHash();

            GLCheck(mGLProgramHandle = glCreateProgram());

            if ( N_Engine().getRender()->getFeature()->isSupport(GF_Shader_Binary_Get) && N_Only(GpuProgram).isCacheExist(hash) )
            {
                getCache(hash);
            }
            else
            {
                compileAndLink();
            }

            extractLayoutQualifiers();
            buildGLUniformReferences();
        }

        if (mLinked)
        {
            GLCheck(glUseProgram(mGLProgramHandle));
        }
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::inactivate()
    {
        GLCheck(glUseProgram(0));
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::compileAndLink()
    {
        // attach Vertex Program
        if (mVertexShader)
        {
            getVertexShader()->attachToProgramObject(mGLProgramHandle);
            setVertexMatrixFusion(mVertexShader->isSkeletalAnimationIncluded());
            
            // Some drivers (e.g. OS X on nvidia) incorrectly determine the attribute binding automatically

            // and end up aliasing existing built-ins. So avoid! 
            // Bind all used attribs - not all possible ones otherwise we'll get 
            // lots of warnings in the log, and also may end up aliasing names used
            // as varyings by accident
            // Because we can't ask GL whether an attribute is used in the shader
            // until it is linked (chicken and egg!) we have to parse the source

            size_t numAttribs = sizeof(DefaultAttributeLayout)/sizeof(CustomAttribute);
            const VString & vpSource = mVertexShader->getProgramCode();
            for (size_t i = 0; i < numAttribs; ++i)
            {
                const CustomAttribute& a = DefaultAttributeLayout[i];

                // we're looking for either: 
                //   attribute vec<n> <semantic_name>
                //   in vec<n> <semantic_name>
                // The latter is recommended in GLSL 1.3 onwards 
                // be slightly flexible about formatting
                VString::size_type pos = vpSource.find(a.name);
                bool foundAttr = false;
                while (pos != String::npos && !foundAttr)
                {
                    String::size_type startpos = vpSource.find("attribute", pos < 20 ? 0 : pos-20);
                    if (startpos == String::npos)
                        startpos = vpSource.find("in", pos-20);
                    if (startpos != String::npos && startpos < pos)
                    {
                        // final check 
                        VString expr = vpSource.substr(startpos, pos + strlen(a.name) - startpos);
                        VStringList vec;
                        StrUtil::split(expr, vec);
                        if ((vec[0] == "in" || vec[0] == "attribute") && vec[2] == a.name)
                        {
                            glBindAttribLocation(mGLProgramHandle, a.attrib, a.name);
                            foundAttr = true;
                        }
                    }
                    // Find the position of the next occurrence if needed
                    pos = vpSource.find(a.name, pos + strlen(a.name));
                }
            }
        }
        
        if(mGeometryShader)
        {
            mGeometryShader->attachToProgramObject(mGLProgramHandle);
            
            glProgramParameteriEXT(mGLProgramHandle, GL_GEOMETRY_INPUT_TYPE, getGLGeometryInputPrimitiveType(mGeometryShader->getInputOperationType()));
            glProgramParameteriEXT(mGLProgramHandle, GL_GEOMETRY_OUTPUT_TYPE, getGLGeometryOutputPrimitiveType(mGeometryShader->getOutputOperationType()));
            glProgramParameteriEXT(mGLProgramHandle, GL_GEOMETRY_VERTICES_OUT, mGeometryShader->getMaxOutputVertices());
        }

        // attach remaining Programs
        for (auto shader : {mFragmentShader, mHullShader, mDomainShader, mComputeShader})
        {
        	if(shader)
            {
                shader->attachToProgramObject(mGLProgramHandle);
            }
        }

        bindFixedAttributes(mGLProgramHandle);

        // the link
        GLCheck(glLinkProgram( mGLProgramHandle ));
        GLCheck(glGetProgramiv( mGLProgramHandle, GL_LINK_STATUS, &mLinked ));

        logObjectInfo( getCombinedName() + String(" GLSL link result : "), mGLProgramHandle );

        if(glIsProgram(mGLProgramHandle))
        {
            GLCheck(glValidateProgram(mGLProgramHandle));
        }
        logObjectInfo( getCombinedName() + String(" GLSL validation result : "), mGLProgramHandle );

        if(mLinked)
        {
            if ( N_Only(GpuProgram).isCacheBinary() )
            {
                GLint binaryLength = 0;
                GLCheck(glGetProgramiv(mGLProgramHandle, GL_PROGRAM_BINARY_LENGTH, &binaryLength));

                MemStream * nMicro = N_Only(GpuProgram).createCache(getCombinedHash(), binaryLength + sizeof(GLenum));

                GLCheck(glGetProgramBinary(mGLProgramHandle, binaryLength, NULL, (GLenum *)nMicro->getData(), nMicro->getData() + sizeof(GLenum)));
            }
        }
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::buildGLUniformReferences(void)
    {
        if (mUniformRefsBuilt)
        {
            return;
        }

        // order must match ShaderType
        GLSLShader* shaders[6] = {getVertexShader(), mFragmentShader, mGeometryShader, mDomainShader, mHullShader, mComputeShader};
        const GpuParamUnitList* params[6] = { NULL };

        for (int i = 0; i < 6; i++)
        {
            if (shaders[i])
            {
                params[i] = &(shaders[i]->getParamDef().mDefineList);
            }
        }

        // Do we know how many shared params there are yet? Or if there are any blocks defined?
        N_Only(GLSL::GLSLProgram).extractUniforms(
            mGLProgramHandle, params, mGLUniformReferences, mGLAtomicCounterReferences,
            mGLUniformBufferReferences, mSharedParamsBufferMap, mGLCounterBufferReferences);

        mUniformRefsBuilt = true;
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::updateUniforms(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype)
    {
        // Iterate through uniform reference list and update uniform values
        GLUniformReferenceIterator currentUniform = mGLUniformReferences.begin();
        GLUniformReferenceIterator endUniform = mGLUniformReferences.end();

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

        for (;currentUniform != endUniform; ++currentUniform)
        {
            // Only pull values from buffer it's supposed to be in (vertex or fragment)
            // This method will be called twice, once for vertex program params,
            // and once for fragment program params.
            if (stype == currentUniform->mSourceProgType)
            {
                const GpuParamUnit * def = currentUniform->mConstantDef;
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
                        default:
                            shouldUpdate = mUniformCache->updateUniform(currentUniform->mLocation,
                                params->getBufferData(def->mMemIndex),
                                static_cast<GLsizei>(def->mUnitSize * def->mUnitCount * sizeof(float)));
                            break;

                    }

                    if(!shouldUpdate)
                        continue;
                    
                    // Get the index in the parameter real list
                    switch (def->constType)
                    {
                    case GDT_Float:
                        GLCheck(glUniform1fv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float2X:
                        GLCheck(glUniform2fv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float3X:
                        GLCheck(glUniform3fv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Float4X:
                        GLCheck(glUniform4fv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double:
                        GLCheck(glUniform1dv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double2X:
                        GLCheck(glUniform2dv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double3X:
                        GLCheck(glUniform3dv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Double4X:
                        GLCheck(glUniform4dv(currentUniform->mLocation, glArraySize, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X2:
                        GLCheck(glUniformMatrix2dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X3:
                        GLCheck(glUniformMatrix2x3dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd2X4:
                        GLCheck(glUniformMatrix2x4dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X2:
                        GLCheck(glUniformMatrix3x2dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X3:
                        GLCheck(glUniformMatrix3dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd3X4:
                        GLCheck(glUniformMatrix3x4dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X2:
                        GLCheck(glUniformMatrix4x2dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X3:
                        GLCheck(glUniformMatrix4x3dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrixd4X4:
                        GLCheck(glUniformMatrix4dv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X2:
                        GLCheck(glUniformMatrix2fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X3:
                        GLCheck(glUniformMatrix2x3fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix2X4:
                        GLCheck(glUniformMatrix2x4fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X4:
                        GLCheck(glUniformMatrix3x2fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X3:
                        GLCheck(glUniformMatrix3fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix3X4:
                        GLCheck(glUniformMatrix3x4fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X2:
                        GLCheck(glUniformMatrix4x2fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X3:
                        GLCheck(glUniformMatrix4x3fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Matrix4X4:
                        GLCheck(glUniformMatrix4fv(currentUniform->mLocation, glArraySize, transpose, params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int:
                        GLCheck(glUniform1iv(currentUniform->mLocation, glArraySize, (GLint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int2X:
                        GLCheck(glUniform2iv(currentUniform->mLocation, glArraySize, (GLint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int3X:
                        GLCheck(glUniform3iv(currentUniform->mLocation, glArraySize, (GLint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_Int4X:
                        GLCheck(glUniform4iv(currentUniform->mLocation, glArraySize, (GLint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt:
                    case GDT_Bool:
                        GLCheck(glUniform1uiv(currentUniform->mLocation, glArraySize, (GLuint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt2X:
                    case GDT_Bool2X:
                        GLCheck(glUniform2uiv(currentUniform->mLocation, glArraySize, (GLuint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt3X:
                    case GDT_Bool3X:
                        GLCheck(glUniform3uiv(currentUniform->mLocation, glArraySize, (GLuint*)params->getBufferData(def->mMemIndex)));
                        break;
                    case GDT_UInt4X:
                    case GDT_Bool4X:
                        GLCheck(glUniform4uiv(currentUniform->mLocation, glArraySize, (GLuint*)params->getBufferData(def->mMemIndex)));
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
                        GLCheck(glUniform1iv(currentUniform->mLocation, 1, (GLint*)params->getBufferData(def->mMemIndex)));
                        break;
                    default:
                        break;

                    } // End switch

                } // Variability & mask
            } // stype == currentUniform->mSourceProgType

        } // End for
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::updateUniformBlocks(GpuProgramParam * params, uint16 mask, GpuProgram::ShaderType stype)
    {
        // Iterate through the list of uniform buffers and update them as needed
        GLUniformBufferIterator currentBuffer = mGLUniformBufferReferences.begin();
        GLUniformBufferIterator endBuffer = mGLUniformBufferReferences.end();

        NCount it, end = params->getValueCount();
        for (it = 0; it < end; ++it)
        {
            GpuParamValue * value = params->getValue(it);
            for (;currentBuffer != endBuffer; ++currentBuffer)
            {
                GLStructBuffer* buf = static_cast<GLStructBuffer*>(*currentBuffer);

                // Block name is stored in mSharedParams->mName of GpuSharedParamUsageList items
                GLint UniformTransform;
                GLCheck(UniformTransform = glGetUniformBlockIndex(mGLProgramHandle, value->getName().c_str()));
                GLCheck(glUniformBlockBinding(mGLProgramHandle, UniformTransform, buf->getGLBufferBinding()));

                buf->write(value->getBufferData(0), 0, buf->getSize());
            }
        }
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::updatePassIterationUniforms(GpuProgramParam * params)
    {
        if (params->getRenderCount() != 0)
        {
            Nidx index = params->getRenderCount() - &params->getBufferData()[0];

            GLUniformReferenceIterator currentUniform = mGLUniformReferences.begin();
            GLUniformReferenceIterator endUniform = mGLUniformReferences.end();

            // Need to find the uniform that matches the multi pass entry
            for (;currentUniform != endUniform; ++currentUniform)
            {
                if (index == currentUniform->mConstantDef->mMemIndex)
                {
                    GLCheck(glUniform1fv(currentUniform->mLocation, 1, params->getRenderCount()));
                    return;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void GLSLMonolithicProgram::getCache(Nui32 id)
    {
        MemStream * cacheMicrocode = N_Only(GpuProgram).getCache(id);

        cacheMicrocode->seek(0);

        // Turns out we need this param when loading.
        GLenum binaryFormat = 0;
        cacheMicrocode->read(&binaryFormat, sizeof(GLenum));

        // Get size of binary.
        GLint binaryLength = static_cast<GLint>(cacheMicrocode->size() - sizeof(GLenum));

        // Load binary.
        GLCheck(glProgramBinary(mGLProgramHandle, binaryFormat, cacheMicrocode->getPos(), binaryLength));

        GLint success = 0;
        GLCheck(glGetProgramiv(mGLProgramHandle, GL_LINK_STATUS, &success));

        if(success)
        {
            mLinked = true;
            return;
        }

        logObjectInfo("could not load from cache " + getCombinedName(), mGLProgramHandle);
        // Something must have changed since the program binaries
        // were cached away. Fallback to source shader loading path,
        // and then retrieve and cache new program binaries once again.
        compileAndLink();
    }
    //-----------------------------------------------------------------------
}