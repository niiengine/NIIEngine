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

#include "OgreGLES2RenderToVertexBuffer.h"

#include "OgreHardwareBufferManager.h"
#include "OgreGLES2HardwareVertexBuffer.h"
#include "OgreRenderable.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreGLSLESProgramManager.h"
#include "OgreStringConverter.h"
#include "OgreTechnique.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------------
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
        default:
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "GL RenderToVertexBuffer can only output point lists, line lists, or triangle lists");
        }
    }
    //-----------------------------------------------------------------------------
    static GLint getVertexCountPerPrimitive(GeometryRaw::OperationType mType)
    {
        //We can only get points, lines or triangles since they are the only
        //legal R2VB output primitive types
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
    //-----------------------------------------------------------------------------
    GLES2RenderToVertexBuffer::GLES2RenderToVertexBuffer() : mFrontBufferIndex(-1)
    {
        mVertexBuffers[0].reset();
        mVertexBuffers[1].reset();

        // Create query objects
        GLCheck(glGenQueries(1, &mPrimitivesDrawnQuery));
    }
    //-----------------------------------------------------------------------------
    GLES2RenderToVertexBuffer::~GLES2RenderToVertexBuffer()
    {
        GLCheck(glDeleteQueries(1, &mPrimitivesDrawnQuery));
    }
    //-----------------------------------------------------------------------------
    void GLES2RenderToVertexBuffer::getGeometry(GeometryRaw& op)
    {
        op.mType = mType;
        op.useIndexes = false;
        op.vertexData = mVertexData.get();
    }
    //-----------------------------------------------------------------------------
    void GLES2RenderToVertexBuffer::update(SpaceManager* sceneMgr)
    {
        size_t bufSize = mVertexData->vertexDeclaration->getVertexSize(0) * mMaxVertexCount;
        if (!mVertexBuffers[0] || mVertexBuffers[0]->getSizeInBytes() != bufSize)
        {
            // Buffers don't match. Need to reallocate.
            mResetRequested = true;
        }
        
        // Single pass only for now
        Ogre::Pass* r2vbPass = mMaterial->getBestTechnique()->getPass(0);
        // Set pass before binding buffers to activate the GPU programs
        sceneMgr->_setPass(r2vbPass);
        
        bindVerticesOutput(r2vbPass);

        GeometryRaw renderOp;
        size_t targetBufferIndex;
        if (mResetRequested || mResetsEveryUpdate)
        {
            // Use source data to render to first buffer
            mSourceRenderable->getGeometry(renderOp);
            targetBufferIndex = 0;
        }
        else
        {
            // Use current front buffer to render to back buffer
            this->getGeometry(renderOp);
            targetBufferIndex = 1 - mFrontBufferIndex;
        }

        if (!mVertexBuffers[targetBufferIndex] || 
            mVertexBuffers[targetBufferIndex]->getSizeInBytes() != bufSize)
        {
            reallocateBuffer(targetBufferIndex);
        }

        GLES2VertexBuffer * vertexBuffer = static_cast<GLES2VertexBuffer*>(mVertexBuffers[targetBufferIndex].get());
/*        if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(GF_GLSL_SeparateShaderObject))
        {
            GLSLESProgramPipeline* programPipeline =
                GLSLESProgramPipelineManager::getSingleton().getActiveProgramPipeline();
            programPipeline->getVertexArrayObject()->bind();
        }
        else
        {
            GLSLESLinkProgram* linkProgram = GLSLESLinkProgramManager::getSingleton().getActiveProgram();
            linkProgram->getVertexArrayObject()->bind();
        }
        */
        // Bind the target buffer
        GLCheck(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vertexBuffer->getGLBufferId()));

        // Disable rasterization
        GLCheck(glEnable(GL_RASTERIZER_DISCARD));

        RenderSystem* targetRenderSystem = Root::getSingleton().getRenderSystem();
        // Draw the object
        if (r2vbPass->hasVertexProgram())
        {
            targetRenderSystem->on(GPT_VERTEX_PROGRAM, r2vbPass->getVertexProgramParameters(), GPV_ALL);
        }
        if (r2vbPass->hasFragmentProgram())
        {
            targetRenderSystem->on(GPT_FRAGMENT_PROGRAM, r2vbPass->getFragmentProgramParameters(), GPV_ALL);
        }
        if (r2vbPass->hasGeometryProgram())
        {
            targetRenderSystem->on(GPT_GEOMETRY_PROGRAM, r2vbPass->getGeometryProgramParameters(), GPV_ALL);
        }
        GLCheck(glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, mPrimitivesDrawnQuery));

        GLCheck(glBeginTransformFeedback(getR2VBPrimitiveType(mType)));

        targetRenderSystem->_render(renderOp);
        
        GLCheck(glEndTransformFeedback());

        // Finish the query
        GLCheck(glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN));

        GLCheck(glDisable(GL_RASTERIZER_DISCARD));

        // Read back query results
        GLuint primitivesWritten;
        GLCheck(glGetQueryObjectuiv(mPrimitivesDrawnQuery, GL_QUERY_RESULT, &primitivesWritten));
        mVertexData->vertexCount = primitivesWritten * getVertexCountPerPrimitive(mType);

        // Switch the vertex binding if necessary
        if (targetBufferIndex != mFrontBufferIndex)
        {
            mVertexData->vertexBufferBinding->unsetAllBindings();
            mVertexData->vertexBufferBinding->setBinding(0, mVertexBuffers[targetBufferIndex]);
            mFrontBufferIndex = targetBufferIndex;
        }

        // Enable rasterization
        GLCheck(glDisable(GL_RASTERIZER_DISCARD));

        // Clear the reset flag
        mResetRequested = false;
    }
    //-----------------------------------------------------------------------------
    void GLES2RenderToVertexBuffer::reallocateBuffer(size_t index)
    {
        N_assert(index == 0 || index == 1);
        if (mVertexBuffers[index])
        {
            mVertexBuffers[index].reset();
        }
        
        mVertexBuffers[index] = GBufferManager::getSingleton().createVertexBuffer(
            mVertexData->vertexDeclaration->getVertexSize(0), mMaxVertexCount, 
#if OGRE_DEBUG_MODE
            //Allow to read the contents of the buffer in debug mode
            Buffer::HBU_DYNAMIC
#else
            Buffer::HBU_STATIC_WRITE_ONLY
#endif
            );
    }
    //-----------------------------------------------------------------------------
    String GLES2RenderToVertexBuffer::getSemanticVaryingName(VertexType vtype, unsigned short index)
    {
        switch (vtype)
        {
        case VT_Pos:
            return "gl_Position";
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
    //-----------------------------------------------------------------------------
    void GLES2RenderToVertexBuffer::bindVerticesOutput(Pass* pass)
    {
        VertexDeclaration* declaration = mVertexData->vertexDeclaration;
        size_t elemCount = declaration->getElementCount();

        if (elemCount > 0)
        {
            GLuint linkProgramId = 0;
            // Have GLSL shaders, using varying attributes
            GLSLESProgramCommon* linkProgram = GLSLESProgramManager::getSingleton().getActiveProgram();
            linkProgramId = linkProgram->getGLProgramHandle();

            // Note: 64 is the minimum number of interleaved attributes allowed by GL_EXT_transform_feedback
            // So we are using it. Otherwise we could query during rendersystem initialisation and use a dynamic sized array.
            // But that would require C99.
            const GLchar *names[64];
            for (unsigned short e = 0; e < elemCount; e++)
            {
                const VertexElement* element = declaration->getElement(e);
                String varyingName = getSemanticVaryingName(element->getSemantic(), element->getIndex());
                names[e] = varyingName.c_str();
            }

            GLCheck(glTransformFeedbackVaryings(linkProgramId, elemCount, names, GL_INTERLEAVED_ATTRIBS));
            GLCheck(glLinkProgram(linkProgramId));
        }
    }
    //-----------------------------------------------------------------------------
}
