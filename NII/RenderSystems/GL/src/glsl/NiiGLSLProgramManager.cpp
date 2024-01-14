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

#include "NiiGLSLLinkProgramManager.h"
#include "NiiGLSLGpuProgram.h"
#include "NiiGLSLShader.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"

namespace NII 
{
    template<> GLSL::GLSLProgramManager* Singleton<GLSL::GLSLProgramManager>::mOnly = 0;
namespace GLSL 
{
    //-----------------------------------------------------------------------
    GLSLProgramManager::GLSLProgramManager(GLRenderSystem * sys) : 
        mActiveVS(NULL),
        mActiveTS(NULL),
        mActiveDS(NULL),
        mActiveGS(NULL),
        mActiveFS(NULL),
        mActiveCS(NULL),
        mActiveProgram(NULL),
        mRenderSys(sys)
    {
        // Fill in the relationship between type names and enums
        mTypeEnumMap.emplace("float", GL_FLOAT);
        mTypeEnumMap.emplace("vec2", GL_FLOAT_VEC2);
        mTypeEnumMap.emplace("vec3", GL_FLOAT_VEC3);
        mTypeEnumMap.emplace("vec4", GL_FLOAT_VEC4);
        mTypeEnumMap.emplace("sampler1D", GL_SAMPLER_1D);
        mTypeEnumMap.emplace("sampler2D", GL_SAMPLER_2D);
        mTypeEnumMap.emplace("sampler3D", GL_SAMPLER_3D);
        mTypeEnumMap.emplace("samplerCube", GL_SAMPLER_CUBE);
        mTypeEnumMap.emplace("sampler1DShadow", GL_SAMPLER_1D_SHADOW);
        mTypeEnumMap.emplace("sampler2DShadow", GL_SAMPLER_2D_SHADOW);
        mTypeEnumMap.emplace("int", GL_INT);
        mTypeEnumMap.emplace("ivec2", GL_INT_VEC2);
        mTypeEnumMap.emplace("ivec3", GL_INT_VEC3);
        mTypeEnumMap.emplace("ivec4", GL_INT_VEC4);
        mTypeEnumMap.emplace("bool", GL_BOOL);
        mTypeEnumMap.emplace("bvec2", GL_BOOL_VEC2);
        mTypeEnumMap.emplace("bvec3", GL_BOOL_VEC3);
        mTypeEnumMap.emplace("bvec4", GL_BOOL_VEC4);
        mTypeEnumMap.emplace("mat2", GL_FLOAT_MAT2);
        mTypeEnumMap.emplace("mat3", GL_FLOAT_MAT3);
        mTypeEnumMap.emplace("mat4", GL_FLOAT_MAT4);

        // GL 2.1
        mTypeEnumMap.emplace("mat2x2", GL_FLOAT_MAT2);
        mTypeEnumMap.emplace("mat3x3", GL_FLOAT_MAT3);
        mTypeEnumMap.emplace("mat4x4", GL_FLOAT_MAT4);
        mTypeEnumMap.emplace("mat2x3", GL_FLOAT_MAT2x3);
        mTypeEnumMap.emplace("mat3x2", GL_FLOAT_MAT3x2);
        mTypeEnumMap.emplace("mat3x4", GL_FLOAT_MAT3x4);
        mTypeEnumMap.emplace("mat4x3", GL_FLOAT_MAT4x3);
        mTypeEnumMap.emplace("mat2x4", GL_FLOAT_MAT2x4);
        mTypeEnumMap.emplace("mat4x2", GL_FLOAT_MAT4x2);

        // GL 3.0
        mTypeEnumMap.emplace("uint", GL_UNSIGNED_INT);
        mTypeEnumMap.emplace("uvec2", GL_UNSIGNED_INT_VEC2);
        mTypeEnumMap.emplace("uvec3", GL_UNSIGNED_INT_VEC3);
        mTypeEnumMap.emplace("uvec4", GL_UNSIGNED_INT_VEC4);
        mTypeEnumMap.emplace("samplerCubeShadow", GL_SAMPLER_CUBE_SHADOW);
        mTypeEnumMap.emplace("sampler1DArray", GL_SAMPLER_1D_ARRAY);
        mTypeEnumMap.emplace("sampler2DArray", GL_SAMPLER_2D_ARRAY);
        mTypeEnumMap.emplace("sampler1DArrayShadow", GL_SAMPLER_1D_ARRAY_SHADOW);
        mTypeEnumMap.emplace("sampler2DArrayShadow", GL_SAMPLER_2D_ARRAY_SHADOW);
        mTypeEnumMap.emplace("isampler1D", GL_INT_SAMPLER_1D);
        mTypeEnumMap.emplace("isampler2D", GL_INT_SAMPLER_2D);
        mTypeEnumMap.emplace("isampler3D", GL_INT_SAMPLER_3D);
        mTypeEnumMap.emplace("isamplerCube", GL_INT_SAMPLER_CUBE);
        mTypeEnumMap.emplace("isampler1DArray", GL_INT_SAMPLER_1D_ARRAY);
        mTypeEnumMap.emplace("isampler2DArray", GL_INT_SAMPLER_2D_ARRAY);
        mTypeEnumMap.emplace("usampler1D", GL_UNSIGNED_INT_SAMPLER_1D);
        mTypeEnumMap.emplace("usampler2D", GL_UNSIGNED_INT_SAMPLER_2D);
        mTypeEnumMap.emplace("usampler3D", GL_UNSIGNED_INT_SAMPLER_3D);
        mTypeEnumMap.emplace("usamplerCube", GL_UNSIGNED_INT_SAMPLER_CUBE);
        mTypeEnumMap.emplace("usampler1DArray", GL_UNSIGNED_INT_SAMPLER_1D_ARRAY);
        mTypeEnumMap.emplace("usampler2DArray", GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);

        // GL 3.1
        mTypeEnumMap.emplace("sampler2DRect", GL_SAMPLER_2D_RECT);
        mTypeEnumMap.emplace("sampler2DRectShadow", GL_SAMPLER_2D_RECT_SHADOW);
        mTypeEnumMap.emplace("isampler2DRect", GL_INT_SAMPLER_2D_RECT);
        mTypeEnumMap.emplace("usampler2DRect", GL_UNSIGNED_INT_SAMPLER_2D_RECT);
        mTypeEnumMap.emplace("samplerBuffer", GL_SAMPLER_BUFFER);
        mTypeEnumMap.emplace("isamplerBuffer", GL_INT_SAMPLER_BUFFER);
        mTypeEnumMap.emplace("usamplerBuffer", GL_UNSIGNED_INT_SAMPLER_BUFFER);

        // GL 3.2
        mTypeEnumMap.emplace("sampler2DMS", GL_SAMPLER_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("isampler2DMS", GL_INT_SAMPLER_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("usampler2DMS", GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("sampler2DMSArray", GL_SAMPLER_2D_MULTISAMPLE_ARRAY);
        mTypeEnumMap.emplace("isampler2DMSArray", GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);
        mTypeEnumMap.emplace("usampler2DMSArray", GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY);

        // GL 4.0
        mTypeEnumMap.emplace("double", GL_DOUBLE);
        mTypeEnumMap.emplace("dmat2", GL_DOUBLE_MAT2);
        mTypeEnumMap.emplace("dmat3", GL_DOUBLE_MAT3);
        mTypeEnumMap.emplace("dmat4", GL_DOUBLE_MAT4);
        mTypeEnumMap.emplace("dmat2x2", GL_DOUBLE_MAT2);
        mTypeEnumMap.emplace("dmat3x3", GL_DOUBLE_MAT3);
        mTypeEnumMap.emplace("dmat4x4", GL_DOUBLE_MAT4);
        mTypeEnumMap.emplace("dmat2x3", GL_DOUBLE_MAT2x3);
        mTypeEnumMap.emplace("dmat3x2", GL_DOUBLE_MAT3x2);
        mTypeEnumMap.emplace("dmat3x4", GL_DOUBLE_MAT3x4);
        mTypeEnumMap.emplace("dmat4x3", GL_DOUBLE_MAT4x3);
        mTypeEnumMap.emplace("dmat2x4", GL_DOUBLE_MAT2x4);
        mTypeEnumMap.emplace("dmat4x2", GL_DOUBLE_MAT4x2);
        mTypeEnumMap.emplace("dvec2", GL_DOUBLE_VEC2);
        mTypeEnumMap.emplace("dvec3", GL_DOUBLE_VEC3);
        mTypeEnumMap.emplace("dvec4", GL_DOUBLE_VEC4);
        mTypeEnumMap.emplace("samplerCubeArray", GL_SAMPLER_CUBE_MAP_ARRAY);
        mTypeEnumMap.emplace("samplerCubeArrayShadow", GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW);
        mTypeEnumMap.emplace("isamplerCubeArray", GL_INT_SAMPLER_CUBE_MAP_ARRAY);
        mTypeEnumMap.emplace("usamplerCubeArray", GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY);

        mTypeEnumMap.emplace("image1D", GL_IMAGE_1D);
        mTypeEnumMap.emplace("iimage1D", GL_INT_IMAGE_1D);
        mTypeEnumMap.emplace("uimage1D", GL_UNSIGNED_INT_IMAGE_1D);
        mTypeEnumMap.emplace("image2D", GL_IMAGE_2D);
        mTypeEnumMap.emplace("iimage2D", GL_INT_IMAGE_2D);
        mTypeEnumMap.emplace("uimage2D", GL_UNSIGNED_INT_IMAGE_2D);
        mTypeEnumMap.emplace("image3D", GL_IMAGE_3D);
        mTypeEnumMap.emplace("iimage3D", GL_INT_IMAGE_3D);
        mTypeEnumMap.emplace("uimage3D", GL_UNSIGNED_INT_IMAGE_3D);
        mTypeEnumMap.emplace("image2DRect", GL_IMAGE_2D_RECT);
        mTypeEnumMap.emplace("iimage2DRect", GL_INT_IMAGE_2D_RECT);
        mTypeEnumMap.emplace("uimage2DRect", GL_UNSIGNED_INT_IMAGE_2D_RECT);
        mTypeEnumMap.emplace("imageCube", GL_IMAGE_CUBE);
        mTypeEnumMap.emplace("iimageCube", GL_INT_IMAGE_CUBE);
        mTypeEnumMap.emplace("uimageCube", GL_UNSIGNED_INT_IMAGE_CUBE);
        mTypeEnumMap.emplace("imageBuffer", GL_IMAGE_BUFFER);
        mTypeEnumMap.emplace("iimageBuffer", GL_INT_IMAGE_BUFFER);
        mTypeEnumMap.emplace("uimageBuffer", GL_UNSIGNED_INT_IMAGE_BUFFER);
        mTypeEnumMap.emplace("image1DArray", GL_IMAGE_1D_ARRAY);
        mTypeEnumMap.emplace("iimage1DArray", GL_INT_IMAGE_1D_ARRAY);
        mTypeEnumMap.emplace("uimage1DArray", GL_UNSIGNED_INT_IMAGE_1D_ARRAY);
        mTypeEnumMap.emplace("image2DArray", GL_IMAGE_2D_ARRAY);
        mTypeEnumMap.emplace("iimage2DArray", GL_INT_IMAGE_2D_ARRAY);
        mTypeEnumMap.emplace("uimage2DArray", GL_UNSIGNED_INT_IMAGE_2D_ARRAY);
        mTypeEnumMap.emplace("imageCubeArray", GL_IMAGE_CUBE_MAP_ARRAY);
        mTypeEnumMap.emplace("iimageCubeArray", GL_INT_IMAGE_CUBE_MAP_ARRAY);
        mTypeEnumMap.emplace("uimageCubeArray", GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY);
        mTypeEnumMap.emplace("image2DMS", GL_IMAGE_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("iimage2DMS", GL_INT_IMAGE_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("uimage2DMS", GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE);
        mTypeEnumMap.emplace("image2DMSArray", GL_IMAGE_2D_MULTISAMPLE_ARRAY);
        mTypeEnumMap.emplace("iimage2DMSArray", GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY);
        mTypeEnumMap.emplace("uimage2DMSArray", GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY);

        // GL 4.2
        mTypeEnumMap.emplace("atomic_uint", GL_UNSIGNED_INT_ATOMIC_COUNTER);
    }
    //-----------------------------------------------------------------------
    GLSLProgramManager::~GLSLProgramManager() {}
    //-----------------------------------------------------------------------
    GLSLProgram * GLSLProgramManager::getActiveProgram()
    {
        // if there is an active link program then return it
        if (mActiveProgram)
            return mActiveProgram;

        // no active link program so find one or make a new one
        // is there an active key?
        Nui32 activeKey = 0;
        if (mActiveVS)
        {
            activeKey = FastHash(activeKey, mActiveVS->getShaderID());
        }
        if (mActiveDS)
        {
            activeKey = FastHash(activeKey, mActiveDS->getShaderID());
        }
        if (mActiveTS)
        {
            activeKey = FastHash(activeKey, mActiveTS->getShaderID());
        }
        if (mActiveGS)
        {
            activeKey = FastHash(activeKey, mActiveGS->getShaderID());
        }
        if (mActiveFS)
        {
            activeKey = FastHash(activeKey, mActiveFS->getShaderID());
        }
        if (mActiveCS)
        {
            // overwrite as compute shaders are not part of the pipeline
            activeKey = FastHash(0, mActiveCS->getShaderID());
        }

        // Only return a link program object if a program exists.
        if (activeKey > 0)
        {
            // Find the key in the hash map.
            ProgramIterator programFound = mPrograms.find(activeKey);
            if (programFound == mPrograms.end())
            {
                if (mRenderSys->getFeature()->isSupport(GF_GLSL_SeparateShaderObject))
                {
                    mActiveProgram = new GLSLSeparableProgram(mActiveVS, mActiveTS, mActiveDS, mActiveGS, mActiveFS, mActiveCS);
                }
                else
                {
                    mActiveProgram = new GLSLMonolithicProgram(mActiveVS, mActiveTS, mActiveDS, mActiveGS, mActiveFS, mActiveCS);
                }

                mPrograms[activeKey] = mActiveProgram;
            }
            else
            {
                // Found a link program in map container so make it active.
                mActiveProgram = static_cast<GLSLProgram*>(programFound->second);
            }
        }

        // Make the program object active.
        //if(mRenderSys->getFeature()->isSupport(GF_GLSL_SeparateShaderObject) && mActiveProgram)
        if(mActiveProgram)
            mActiveProgram->activate();

        return mActiveProgram;
    }
    //-----------------------------------------------------------------------
    void GLSLProgramManager::setActiveFragmentShader(GLSLShader* fp)
    {
        if (fp != mActiveFS)
        {
            mActiveFS = fp;
            // ActiveLinkProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void GLSLProgramManager::setActiveVertexShader(GLSLShader* vp)
    {
        if (vp != mActiveVS)
        {
            mActiveVS = vp;
            // ActiveLinkProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void GLSLProgramManager::setActiveGeometryShader(GLSLShader* gp)
    {
        if (gp != mActiveGS)
        {
            mActiveGS = gp;
            // ActiveLinkProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void GLSL::GLSLProgramManager::setActiveHullShader(GLSLShader* tp)
    {
        if (tp != mActiveTS)
        {
            mActiveTS = tp;
            // ActiveMonolithicProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void GLSL::GLSLProgramManager::setActiveDomainShader(GLSLShader* dp)
    {
        if (dp != mActiveDS)
        {
            mActiveDS = dp;
            // ActiveMonolithicProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //-----------------------------------------------------------------------
    void GLSL::GLSLProgramManager::setActiveComputeShader(GLSLShader* cp)
    {
        if (cp != mActiveCS)
        {
            mActiveCS = cp;
            // ActiveMonolithicProgram is no longer valid
            mActiveProgram = NULL;
        }
    }
    //---------------------------------------------------------------------
    bool GLSL::GLSLProgramManager::isAttributeValid(VertexType type, Ni32 index)
    {
        if(mActiveProgram)
        {
            return mActiveProgram->isAttributeValid(type, index);
        }
        return false;
    }
    //---------------------------------------------------------------------
    NIIi GLSL::GLSLProgramManager::getAttributeIndex(VertexType type, Ni32 index)
    {
        if(mActiveProgram)
        {
            return mActiveProgram->getAttributeIndex(type, index);
        }
        return -1;
    }
    //---------------------------------------------------------------------
    void GLSLProgramManager::convertGLUniformtoOgreType(GLenum gltype, GpuParamUnit & unit)
    {
        // Note GLSL never packs rows into float4's (from an API perspective anyway)
        // therefore all values are tight in the buffer.
        //TODO Should the rest of the above enum types be included here?
        switch (gltype)
        {
        case GL_FLOAT:
            unit.mDataType = GDT_Float;
            break;
        case GL_FLOAT_VEC2:
            unit.mDataType = GDT_Float2X;
            break;
        case GL_FLOAT_VEC3:
            unit.mDataType = GDT_Float3X;
            break;
        case GL_FLOAT_VEC4:
            unit.mDataType = GDT_Float4X;
            break;
        case GL_IMAGE_1D: //TODO should be its own type?
        case GL_SAMPLER_1D:
        case GL_SAMPLER_1D_ARRAY:
        case GL_INT_SAMPLER_1D:
        case GL_INT_SAMPLER_1D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_1D:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            // need to record samplers for GLSL
            unit.mDataType = GDT_Sampler1D;
            break;
        case GL_IMAGE_2D: //TODO should be its own type?
        case GL_IMAGE_2D_RECT:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_2D_RECT:    // TODO: Move these to a new type??
        case GL_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        case GL_SAMPLER_2D_ARRAY:
        case GL_INT_SAMPLER_2D:
        case GL_INT_SAMPLER_2D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D:
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
            unit.mDataType = GDT_Sampler2D;
            break;
        case GL_IMAGE_3D: //TODO should be its own type?
        case GL_SAMPLER_3D:
        case GL_INT_SAMPLER_3D:
        case GL_UNSIGNED_INT_SAMPLER_3D:
            unit.mDataType = GDT_Sampler3D;
            break;
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_CUBE_SHADOW:
        case GL_INT_SAMPLER_CUBE:
        case GL_UNSIGNED_INT_SAMPLER_CUBE:
            unit.mDataType = GDT_SamplerCube;
            break;
        case GL_SAMPLER_1D_SHADOW:
        case GL_SAMPLER_1D_ARRAY_SHADOW:
            unit.mDataType = GDT_SamplerShadow1D;
            break;
        case GL_SAMPLER_2D_SHADOW:
        case GL_SAMPLER_2D_RECT_SHADOW:
        case GL_SAMPLER_2D_ARRAY_SHADOW:
            unit.mDataType = GDT_SamplerShadow2D;
            break;
        case GL_INT:
            unit.mDataType = GDT_Int;
            break;
        case GL_INT_VEC2:
            unit.mDataType = GDT_Int2X;
            break;
        case GL_INT_VEC3:
            unit.mDataType = GDT_Int3X;
            break;
        case GL_INT_VEC4:
            unit.mDataType = GDT_Int4X;
            break;
        case GL_FLOAT_MAT2:
            unit.mDataType = GDT_Matrix2X2;
            break;
        case GL_FLOAT_MAT3:
            unit.mDataType = GDT_Matrix3X3;
            break;
        case GL_FLOAT_MAT4:
            unit.mDataType = GDT_Matrix4X4;
            break;
        case GL_FLOAT_MAT2x3:
            unit.mDataType = GDT_Matrix2X3;
            break;
        case GL_FLOAT_MAT3x2:
            unit.mDataType = GDT_Matrix3X2;
            break;
        case GL_FLOAT_MAT2x4:
            unit.mDataType = GDT_Matrix2X4;
            break;
        case GL_FLOAT_MAT4x2:
            unit.mDataType = GDT_Matrix4X2;
            break;
        case GL_FLOAT_MAT3x4:
            unit.mDataType = GDT_Matrix3X4;
            break;
        case GL_FLOAT_MAT4x3:
            unit.mDataType = GDT_Matrix4X3;
            break;
        case GL_DOUBLE:
            unit.mDataType = GDT_Double;
            break;
        case GL_DOUBLE_VEC2:
            unit.mDataType = GDT_Double2X;
            break;
        case GL_DOUBLE_VEC3:
            unit.mDataType = GDT_Double3X;
            break;
        case GL_DOUBLE_VEC4:
            unit.mDataType = GDT_Double4X;
            break;
        case GL_DOUBLE_MAT2:
            unit.mDataType = GDT_Matrixd2X2;
            break;
        case GL_DOUBLE_MAT3:
            unit.mDataType = GDT_Matrixd3X3;
            break;
        case GL_DOUBLE_MAT4:
            unit.mDataType = GDT_Matrixd4X4;
            break;
        case GL_DOUBLE_MAT2x3:
            unit.mDataType = GDT_Matrixd2X3;
            break;
        case GL_DOUBLE_MAT3x2:
            unit.mDataType = GDT_Matrixd3X2;
            break;
        case GL_DOUBLE_MAT2x4:
            unit.mDataType = GDT_Matrixd2X4;
            break;
        case GL_DOUBLE_MAT4x2:
            unit.mDataType = GDT_Matrixd4X2;
            break;
        case GL_DOUBLE_MAT3x4:
            unit.mDataType = GDT_Matrixd3X4;
            break;
        case GL_DOUBLE_MAT4x3:
            unit.mDataType = GDT_Matrixd4X3;
            break;
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_ATOMIC_COUNTER: //TODO should be its own type?
            unit.mDataType = GDT_UInt;
            break;
        case GL_UNSIGNED_INT_VEC2:
            unit.mDataType = GDT_UInt2X;
            break;
        case GL_UNSIGNED_INT_VEC3:
            unit.mDataType = GDT_UInt3X;
            break;
        case GL_UNSIGNED_INT_VEC4:
            unit.mDataType = GDT_UInt4X;
            break;
        case GL_BOOL:
            unit.mDataType = GDT_Bool;
            break;
        case GL_BOOL_VEC2:
            unit.mDataType = GDT_Bool2X;
            break;
        case GL_BOOL_VEC3:
            unit.mDataType = GDT_Bool3X;
            break;
        case GL_BOOL_VEC4:
            unit.mDataType = GDT_Bool4X;
            break;
        default:
            unit.mDataType = GDT_Unknow;
            break;
        }

        // GL doesn't pad
        unit.mUnitSize = GpuProgramParam::get32bCount(unit.mDataType, false);
    }
    //---------------------------------------------------------------------
    bool GLSL::GLSLProgramManager::findUniformDataSource(const String & paramName,
        const GpuParamUnitList* (&constantDefs)[6], GLUniformReference& refToUpdate)
    {
        for(int i = 0; i < 6; i++) {
            if (constantDefs[i])
            {
                GpuParamUnitList::const_iterator parami = constantDefs[i]->find(paramName);
                if (parami != constantDefs[i]->end())
                {
                    refToUpdate.mSourceProgType = static_cast<GpuProgramType>(i);
                    refToUpdate.mConstantDef = &(parami->second);
                    return true;
                }
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    //FIXME This is code bloat...either template or unify UniformReference
    // and AtomicCounterReference
    bool GLSLProgramManager::findAtomicCounterDataSource(const String & paramName, 
        const GpuParamUnitList* (&constantDefs)[6], GLAtomicCounterReference & refToUpdate)
    {
        if (constantDefs[i])
        {
            GpuParamUnitList::const_iterator parami = constantDefs[i]->find(paramName);
            if (parami != constantDefs[i]->end())
            {
                refToUpdate.mSourceProgType = static_cast<GpuProgramType>(i);
                refToUpdate.mConstantDef = &(parami->second);
                return true;
            }
        }
        return false;
    }
    //---------------------------------------------------------------------
    void GLSL::GLSLProgramManager::extractUniforms(
        GLuint programObject,
        const GpuParamUnitList * (&constantDefs)[6],
        GLUniformReferenceList & uniformList,
        GLAtomicCounterReferenceList & counterList,
        GLStructBufferList & uniformBufferList,
        SharedParamsBufferMap & sharedParamsBufferMap,
        GLCounterBufferList & counterBufferList)
    {
        // Scan through the active uniforms and add them to the reference list.
        GLint uniformCount = 0;
#define uniformLength 200
        // GLint uniformLength = 0;
        // glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformLength);

        char uniformName[uniformLength];
        GLUniformReference newGLUniformReference;
        GLAtomicCounterReference newGLAtomicCounterReference;

        // Get the number of active uniforms, including atomic
        // counters and uniforms contained in uniform blocks.
        GLCheck(glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &uniformCount));

        // Loop over each active uniform and add it to the reference
        // container.
        for (GLuint index = 0; index < (GLuint)uniformCount; ++index)
        {
            GLint arraySize;
            GLenum glType;
            GLCheck(glGetActiveUniform(programObject, index, uniformLength, NULL, &arraySize, &glType, uniformName));

            // Don't add built in uniforms, atomic counters, or uniform block parameters.
            GLCheck(newGLUniformReference.mLocation = glGetUniformLocation(programObject, uniformName));
            if (newGLUniformReference.mLocation >= 0)
            {
                // User defined uniform found, add it to the reference list.
                String paramName = String(uniformName);

                // Current ATI drivers (Catalyst 7.2 and earlier) and
                // older NVidia drivers will include all array
                // elements as uniforms but we only want the root
                // array name and location. Also note that ATI Catalyst
                // 6.8 to 7.2 there is a bug with glUniform that does
                // not allow you to update a uniform array past the
                // first uniform array element ie you can't start
                // updating an array starting at element 1, must
                // always be element 0.

                // If the uniform name has a "[" in it then its an array element uniform.
                String::size_type arrayStart = paramName.find('[');
                if (arrayStart != String::npos)
                {
                    // if not the first array element then skip it and continue to the next uniform
                    if (paramName.compare(arrayStart, paramName.size() - 1, "[0]") != 0) continue;
                    paramName = paramName.substr(0, arrayStart);
                }

                // Find out which params object this comes from
                bool foundSource = findUniformDataSource(paramName, constantDefs, newGLUniformReference);

                // Only add this parameter if we found the source
                if (foundSource)
                {
                    N_assert(size_t (arraySize) == newGLUniformReference.mConstantDef->arraySize && "GL doesn't agree with our array size!");
                    uniformList.push_back(newGLUniformReference);
                }

                // Don't bother adding individual array params, they will be
                // picked up in the 'parent' parameter can copied all at once
                // anyway, individual indexes are only needed for lookup from
                // user params
            } // end if
            // Handle atomic counters. Currently atomic counters
            // cannot be in uniform blocks and are always unsigned
            // integers.
            else if (glType == GL_UNSIGNED_INT_ATOMIC_COUNTER)
            {
                String paramName = String(uniformName);

                GLint binding, offset;
                GLCheck(glGetActiveUniformsiv(programObject, 1, &index, GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX, &binding));
                GLCheck(glGetActiveUniformsiv(programObject, 1, &index, GL_UNIFORM_OFFSET, &offset));

                newGLAtomicCounterReference.mBinding = binding;
                newGLAtomicCounterReference.mOffset = offset;

                // increment the total number of atomic counters
                // including size of array if applicable
                //atomicCounterCount += arraySize;
                // actually, this should not be necessary since
                // parameters are processed one by one

                // If the uniform name has a "[" in it then its an array element uniform.
                String::size_type arrayStart = paramName.find('[');
                if (arrayStart != String::npos)
                {
                    // if not the first array element then skip it and continue to the next uniform
                    if (paramName.compare(arrayStart, paramName.size() - 1, "[0]") != 0) 
                        continue;
                    paramName = paramName.substr(0, arrayStart);
                }

                printf("ATOMIC COUNTER FOUND: %s %d", paramName.c_str(), arraySize);

                // Find out which params object this comes from
                bool foundSource = findAtomicCounterDataSource(paramName, constantDefs, newGLAtomicCounterReference);

                // Only add this parameter if we found the source
                if (foundSource)
                {
                    // size_t adjustedArraySize = 0;
                    // if (arraySize == 2 && newGLAtomicCounterReference.mConstantDef->arraySize == 1) {
                    //     adjustedArraySize = 1;
                    // }
                    // else {
                    //     adjustedArraySize = (size_t) arraySize;
                    // }

                    //FIXME On Linux AMD Catalyst 13.4, OpenGL reports
                    // a single atomic counter as having size 2.  Bug
                    // or feature?
                    // N_assert((size_t)arraySize == newGLAtomicCounterReference.mConstantDef->arraySize
                    //        && "GL doesn't agree with our array size!");

                    counterList.push_back(newGLAtomicCounterReference);
                    printf("ATOMIC COUNTER SOURCE FOUND\n");
                }
            }
        } // end for


        //FIXME uniform buffers need to be created during material script parsing of shared params
        // StructBuffer * newStructBuffer;
        // N_Only(GBuffer).create(newStructBuffer, blockSize, Buffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false, uniformName);
        // GLStructBuffer * hwGlBuffer = static_cast<GLStructBuffer **>(newStructBuffer);
        // hwGlBuffer->setGLBufferBinding(blockBinding);
        // GpuParamValue * blockSharedParams = N_Only(GpuProgram).getShare(uniformName);
        // uniformBufferList.push_back(uniformBuffer);

        //FIXME Ogre materials need a new shared param that is associated with an entity.
        // This could be impemented as a switch-like statement inside shared_params:

        GLint blockCount = 0;

        // Now deal with uniform blocks

        GLCheck(glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount));

        for (int index = 0; index < blockCount; ++index)
        {
            GLCheck(glGetActiveUniformBlockName(programObject, index, uniformLength, NULL, uniformName));

            // Map uniform block to binding point of GL buffer of
            // shared param bearing the same name.

            GpuParamValue * blockSharedParams = N_Only(GpuProgram).getShare(uniformName);
            GLStructBuffer * hwGlBuffer;
            SharedParamsBufferMap::const_iterator bufferMapi = sharedParamsBufferMap.find(blockSharedParams);
            if (bufferMapi != sharedParamsBufferMap.end())
            {
                hwGlBuffer = static_cast<GLStructBuffer *>(bufferMapi->second);
            }
            else
            {
                // Create buffer and add entry to buffer map.
                GLint blockSize;
                GLCheck(glGetActiveUniformBlockiv(programObject, index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize));
                N_Only(GBuffer).create(hwGlBuffer, blockSize, Buffer::M_DEVWRITE | Buffer::M_WHOLE);
                GLint bufferBinding = sharedParamsBufferMap.size();
                hwGlBuffer->setGLBufferBinding(bufferBinding);
                sharedParamsBufferMap.insert(Npair(blockSharedParams, hwGlBuffer));

                // Get active block parameter properties.
                GpuParamUnitList::iterator j, jend = blockSharedParams->getDefList().end();
                std::vector<const char*> sharedParamNames;
                for (j = blockSharedParams->getDefList().begin(); j != jend; ++j)
                {
                    sharedParamNames.push_back(j->first.c_str());
                }

                std::vector<GLuint> uniformParamIndices(sharedParamNames.size());
                std::vector<GLint> uniformParamOffsets(sharedParamNames.size());

                GLCheck(glGetUniformIndices(programObject, sharedParamNames.size(), &sharedParamNames[0], &uniformParamIndices[0]));
                //FIXME debug this (see stdout)
                GLCheck(glGetActiveUniformsiv(programObject, uniformParamIndices.size(), &uniformParamIndices[0], GL_UNIFORM_OFFSET, &uniformParamOffsets[0]));
                //TODO handle uniform arrays
                //GL_UNIFORM_ARRAY_STRIDE
                //TODO handle matrices
                //GL_UNIFORM_MATRIX_STRIDE

                for (size_t i = 0, j = blockSharedParams->getDefList().begin(); j != jend; ++j, ++i)
                {
                    // NOTE: the naming in GL3Plus is backward. mIndex is actually the physical index of the parameter
                    // while the physicalIndex is the logical array offset..
                    j->second.mIndex = uniformParamOffsets[i];
                }
            }

            GLint bufferBinding = hwGlBuffer->getGLBufferBinding();
            GLCheck(glUniformBlockBinding(programObject, index, bufferBinding));
        }

        // Now deal with shader storage blocks
        if (mRenderSys->hasMinGLVersion(4, 3))
        {
            GLCheck(glGetProgramInterfaceiv(programObject, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &blockCount));

            //TODO error if GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS > # shader_storage_blocks
            // do same for other buffers

            for (int index = 0; index < blockCount; ++index)
            {
                GLCheck(glGetProgramResourceName(programObject, GL_SHADER_STORAGE_BLOCK, index, uniformLength, NULL, uniformName));

                // Map uniform block to binding point of GL buffer of
                // shared param bearing the same name.

                GpuParamValue * blockSharedParams = N_Only(GpuProgram).getShare(uniformName);
                GLStructBuffer * hwGlBuffer;
                SharedParamsBufferMap::const_iterator bufferMapi = sharedParamsBufferMap.find(blockSharedParams);
                if (bufferMapi != sharedParamsBufferMap.end())
                {
                    hwGlBuffer = static_cast<GLStructBuffer*>(bufferMapi->second);
                }
                else
                {
                    // Create buffer and add entry to buffer map.
                    GLint blockSize;
                    GLenum properties[] = {GL_BUFFER_DATA_SIZE};
                    GLCheck(glGetProgramResourceiv(programObject, GL_SHADER_STORAGE_BLOCK, index, 1, properties, 1, NULL, &blockSize));
                    //TODO Implement shared param access param in materials (R, W, R+W)
                    N_Only(GBuffer).create(hwGlBuffer, blockSize, Buffer::M_NORMAL);

                    hwGlBuffer->setGLBufferBinding(sharedParamsBufferMap.size());

                    sharedParamsBufferMap.insert(Npair(blockSharedParams, hwGlBuffer));

                    // Get active block parameter properties.
                    properties[0] = GL_OFFSET;
                    GpuParamUnitList::iterator j, jend = blockSharedParams->getDefList().end();
                    for (j = blockSharedParams->getDefList().begin(); j != jend; ++j) 
                    {
                        GLuint varIndex = glGetProgramResourceIndex(programObject, GL_BUFFER_VARIABLE, j->first.c_str());
                        GLint offset;
                        glGetProgramResourceiv(programObject, GL_BUFFER_VARIABLE, varIndex, 1, properties, 1, NULL, &offset);
                        j->second.mIndex = offset;
                    }
                }

                GLint bufferBinding = hwGlBuffer->getGLBufferBinding();

                GLCheck(glShaderStorageBlockBinding(programObject, index, bufferBinding));
            }
        }
        if (mRenderSys->hasMinGLVersion(4, 2))
        {
            // Now deal with atomic counters buffers
            GLCheck(glGetProgramiv(programObject, GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, &blockCount));

            for (int index = 0; index < blockCount; index++)
            {
                //TODO Is this necessary?
                //GpuParamValue * blockSharedParams = N_Only(GpuProgram).getShare(uniformName);

                //TODO We could build list of atomic counters here or above,
                // whichever is most efficient.
                // GLint * active_indices;
                // GLCheck(glGetActiveAtomicCounterBufferiv(programObject, index, GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES, active_indices));

                GLint bufferSize, bufferBinding;
                GLCheck(glGetActiveAtomicCounterBufferiv(programObject, index, GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE, &bufferSize));
                GLCheck(glGetActiveAtomicCounterBufferiv(programObject, index, GL_ATOMIC_COUNTER_BUFFER_BINDING, &bufferBinding));
                //TODO check parameters of this GL call
                GLCounterBuffer * hwGlBuffer;
                N_Only(GBuffer).create(hwGlBuffer, bufferSize, Buffer::M_DEVWRITE | Buffer::M_WHOLE);

                hwGlBuffer->setGLBufferBinding(bufferBinding);
                counterBufferList.push_back(hwGlBuffer);
            }
        }
    }
    //---------------------------------------------------------------------
}
}

