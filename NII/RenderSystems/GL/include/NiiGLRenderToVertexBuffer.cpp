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

#include "OgreGL3PlusRenderToVertexBuffer.h"
#include "OgreHardwareBufferManager.h"
#include "OgreGL3PlusHardwareVertexBuffer.h"
#include "OgreRenderable.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreGLSLProgramManager.h"
#include "OgreGLSLShader.h"
#include "OgreStringConverter.h"
#include "OgreTechnique.h"

namespace Ogre 
{
    GLRenderToVertexBuffer::GLRenderToVertexBuffer()
        : mTargetBufferIndex(0)
        , mFirstUpdate(true)
    {
        mVertexBuffers[0] = 0;
        mVertexBuffers[1] = 0;

        // Create query objects.
        GLCheck(glGenQueries(1, &mPrimitivesDrawnQuery));

        //TODO GL4+
        // glGenTransformFeedbacks(1, mFeedbackObject);
    }


    GLRenderToVertexBuffer::~GLRenderToVertexBuffer()
    {
        GLCheck(glDeleteQueries(1, &mPrimitivesDrawnQuery));
    }


    static GLint getR2VBPrimitiveType(GeometryRaw::OperationType mType)
    {
        switch (mType)
        {
        case GeometryRaw::OT_POINT_LIST:
            return GL_POINTS;
        case GeometryRaw::OT_LINE_LIST:
            return GL_LINES;
        case GeometryRaw::OT_TRIANGLE_LIST:
            return GL_TRIANGLES;
            //TODO Add other GeometryRaw allowed when no GS present.
        default:
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "GL RenderToVertexBuffer can only output point lists, line lists, or triangle lists");
        }
    }


    static GLint getVertexCountPerPrimitive(GeometryRaw::OperationType mType)
    {
        // We can only get points, lines or triangles since they are the only
        // legal R2VB output primitive types.
        switch (mType)
        {
        case GeometryRaw::OT_POINT_LIST:
            return 1;
        case GeometryRaw::OT_LINE_LIST:
            return 2;
        default:
        case GeometryRaw::OT_TRIANGLE_LIST:
            return 3;
        }
    }


    void GLRenderToVertexBuffer::getGeometry(GeometryRaw & op)
    {
        op.mType = mGeoOp;
        op.remove(GRT_Index);
        op.mVertex = mVertexData;
    }


    void GLRenderToVertexBuffer::bindVerticesOutput(ShaderCh * pass)
    {
        size_t unitcnt = mVertexData->getCount();

        if (unitcnt == 0)
            return;

        // Store the output in a buffer.  The buffer has the same
        // structure as the shader output vertex data.
        // Note: 64 is the minimum number of interleaved
        // attributes allowed by GL_EXT_transform_feedback so we
        // are using it. Otherwise we could query during
        // rendersystem initialisation and use a dynamic sized
        // array.  But that would require C99.
        size_t sourceBufferIndex = mTargetBufferIndex == 0 ? 1 : 0;

        // Bind and fill vertex arrays + buffers.
        reallocateBuffer(sourceBufferIndex);
        reallocateBuffer(mTargetBufferIndex);
        // GL3PlusHardwareVertexBuffer* sourceVertexBuffer = static_cast<GL3PlusHardwareVertexBuffer*>(mVertexBuffers[mSourceBufferIndex].get());
        // GL3PlusHardwareVertexBuffer* targetVertexBuffer = static_cast<GL3PlusHardwareVertexBuffer*>(mVertexBuffers[mTargetBufferIndex].get());

        //TODO GL4+ glBindTransformFeedback

        // Dynamically determine shader output variable names.
        std::vector<String> nameStrings;

        for (uint e = 0; e < unitcnt; e++)
        {
            const VertexUnit * unit = mVertexData->get(e);
            String name = getSemanticVaryingName(unit->mVType, unit->mIndex);
            nameStrings.push_back(name);
        }

        GLSLProgram* program = N_Only(GLSL::GLSLProgram).getActiveProgram();

        program->setTransformFeedbackVaryings(nameStrings);
    }


    void GLRenderToVertexBuffer::flush(SpaceManager* sceneMgr)
    {
        //     size_t bufSize = mVertexData->vertexDeclaration->getVertexSize(0) * mMaxVertexCount;
        //     if (!mVertexBuffers[0] || mVertexBuffers[0]->getSizeInBytes() != bufSize)
        //     {
        //         // Buffers don't match. Need to reallocate.
        //         mRequestFlush = true;
        //     }

        //     if (mRequestFlush || mAutoFlush)
        //     {
        //         // Use source data to render to first buffer.
        //         mGeo->getGeometry(renderOp);
        //         targetBufferIndex = 0;
        //     }
        //     else
        //     {
        //         // Use current front buffer to render to back buffer.
        //         this->getGeometry(renderOp);
        //         targetBufferIndex = 1 - mSourceBufferIndex;
        //     }

        //     if (!mVertexBuffers[targetBufferIndex] ||
        //         mVertexBuffers[targetBufferIndex]->getSizeInBytes() != bufSize)
        //     {
        //         reallocateBuffer(targetBufferIndex);
        //     }

        // Single pass only for now.
        ShaderCh * r2vbPass = mMaterial->getLOD()->get(0);

        // Set pass before binding buffers to activate the GPU programs.
        sceneMgr->applyCh(r2vbPass, false, true);
        if (mFirstUpdate)
        {
            bindVerticesOutput(r2vbPass);
            mFirstUpdate = false;
        }

        // size_t targetBufferIndex = mSourceBufferIndex == 0 ? 0 : 1;

        // Disable rasterization.
        GLCheck(glEnable(GL_RASTERIZER_DISCARD));

        // Bind shader parameters.
        RenderSys * targetRenderSystem = Engine::getOnly().getRender();
        if (r2vbPass->getProgram().isExist(ShaderChProgram::ST_VS))
        {
            r2vbPass->getProgram().getParam(ShaderChProgram::ST_VS, param);
            targetRenderSystem->on(GpuProgram::ST_VS, param, GPT_Unknow);
        }
        if (r2vbPass->getProgram().isExist(ShaderChProgram::ST_FS))
        {
            r2vbPass->getProgram().getParam(ShaderChProgram::ST_FS, param);
            targetRenderSystem->on(GpuProgram::ST_FS, param, GPT_Unknow);
        }
        if (r2vbPass->getProgram().isExist(ShaderChProgram::ST_GS))
        {
            r2vbPass->getProgram().getParam(ShaderChProgram::ST_GS, param);
            targetRenderSystem->on(GpuProgram::ST_GS, param, GPT_Unknow);
        }
        //TODO add tessellation stages

        // Bind source vertex array + target tranform feedback buffer.
        GLVertexBuffer * targetVertexBuffer = static_cast<GLVertexBuffer *>(mVertexBuffers[mTargetBufferIndex]);
        // GLCheck(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VertexBuffer[mTargetBufferIndex]));
        GLCheck(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, targetVertexBuffer->getGLBufferId()));
        // GLCheck(glBindVertexArray(VertexArray[mSourceBufferIndex]));
        if (Engine::getOnly().getRender()->isSupport(GF_GLSL_SeparateShaderObject))
        {
            GLSLProgram* separableProgram = N_Only(GLSL::GLSLProgram).getActiveProgram();
            separableProgram->activate();
        }

        // 'Render' data to the transform buffer.
        GLCheck(glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mPrimitivesDrawnQuery));
        GLCheck(glBeginTransformFeedback(getR2VBPrimitiveType(mGeoOp)));

        GeometryRaw renderOp;
        if (mRequestFlush || mAutoFlush)
        {
            // Use source data to render to first buffer.
            mGeo->getGeometry(renderOp);
        }
        else
        {
            // Use current front buffer to render to back buffer.
            this->getGeometry(renderOp);
        }
        targetRenderSystem->_render(renderOp);
        // GLCheck(glDrawArrays(GL_POINTS, 0, 1));

        //TODO GL4+
        //glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mFeedbackObject);
        //glDrawTransformFeedback(getR2VBPrimitiveType(mGeoOp), mFeedbackObject);

        GLCheck(glEndTransformFeedback());
        GLCheck(glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN));

        // Read back query results.
        GLuint primitivesWritten;
        GLCheck(glGetQueryObjectuiv(mPrimitivesDrawnQuery, GL_QUERY_RESULT, &primitivesWritten));
        mVertexData->vertexCount = primitivesWritten * getVertexCountPerPrimitive(mGeoOp);

        // Switch the vertex binding.
        mVertexData->detachAll();
        mVertexData->attach(0, mVertexBuffers[mTargetBufferIndex]);
        mTargetBufferIndex = mTargetBufferIndex == 0 ? 1 : 0;

        // Enable rasterization.
        GLCheck(glDisable(GL_RASTERIZER_DISCARD));

        // Clear the reset flag.
        mRequestFlush = false;
    }

    void GLRenderToVertexBuffer::reallocateBuffer(size_t index)
    {
        N_assert(index == 0 || index == 1);
        if(mVertexBuffers[index])
        {
			GBufferManager::getOnly().destroy(mVertexBuffers[index]);
        }

        GBufferManager::getOnly().create(mVertexBuffers[index],
            mVertexData->getUnitSize(0), mMaxVertexCount,
#if NII_DEBUG_MODE
            //Allow to read the contents of the buffer in debug mode
            (Buffer::M_READ | Buffer::M_WRITE));
#else
            Buffer::Buffer::M_DEV | Buffer::M_WRITE);
#endif
    }


    String GLRenderToVertexBuffer::getSemanticVaryingName(VertexType vtype, unsigned short index)
    {
        switch (vtype)
        {
        case VT_Pos:
            // Since type of gl_Position cannot be redefined, it is
            // better to use a custom variable name.
            // return "gl_Position";
            return "oPos";
        case VT_Normals:
            return "oNormal";
        case VT_Tex_Coord:
            return String("oUv") + StrConv::conv(index);
        case VT_Diffuse:
            return "oColour";
        case VT_Specular:
            return "oSecColour";
            //TODO : Implement more?
        default:
            N_EXCEPT(RenderingAPI, "Unsupported vertex element sematic in render to vertex buffer");
        }
    }
}
