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
#include "OgreD3D11Device.h"
#include "OgreD3D11HardwareBufferManager.h"
#include "OgreD3D11RenderToVertexBuffer.h"
#include "OgreHardwareBufferManager.h"
#include "OgreD3D11HardwareVertexBuffer.h"
#include "OgreRenderable.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreTechnique.h"
#include "OgreStringConverter.h"
#include "OgreD3D11RenderSystem.h"
#include "OgreD3D11HLSLProgram.h"

namespace Ogre {

    D3D11RenderToVertexBuffer::D3D11RenderToVertexBuffer(D3D11Device & device, 
                                                         D3D11HardwareBufferManagerBase * bufManager) 
        : mDevice(device)
        ,  mFrontBufferIndex(-1)
        , mBufManager(bufManager)
        , mpGeometryShader(0)
    {
        mVertexBuffers[0].reset();
        mVertexBuffers[1].reset();
    }

    D3D11RenderToVertexBuffer::~D3D11RenderToVertexBuffer(void)
    {
    }

    void D3D11RenderToVertexBuffer::getGeometry(GeometryRaw& op)
    {
        op.mType = mType;
        op.useIndexes = false;
        op.useGlobalInstancingVertexBufferIsAvailable = false;
        op.vertexData = mVertexData.get();
    }

    void D3D11RenderToVertexBuffer::setupGeometryShaderLinkageToStreamOut(Pass* pass)
    {
        static bool done =  false;

        if (done)
            return;

        N_assert(pass->hasGeometryProgram());
        const GpuProgram * program = pass->getGeometryProgram();

        D3D11HLSLProgram* dx11Program = 0;
        if (program->getSyntaxCode()=="unified")
        {
            dx11Program = static_cast<D3D11HLSLProgram*>(program->getMain());
        }
        else
        {
            dx11Program = static_cast<D3D11HLSLProgram*>(program.get());
        }
        dx11Program->reinterpretGSForStreamOut();

        done = true;
        
    }
    void D3D11RenderToVertexBuffer::update(SpaceManager* sceneMgr)
    {
        //Single pass only for now
        Ogre::Pass* r2vbPass = mMaterial->getBestTechnique()->getPass(0);

        setupGeometryShaderLinkageToStreamOut(r2vbPass);

        size_t bufSize = mVertexData->vertexDeclaration->getVertexSize(0) * mMaxVertexCount;
        if (!mVertexBuffers[0] || mVertexBuffers[0]->getSizeInBytes() != bufSize)
        {
            //Buffers don't match. Need to reallocate.
            mResetRequested = true;
        }

        //Set pass before binding buffers to activate the GPU programs
        sceneMgr->_setPass(r2vbPass);

        GeometryRaw renderOp;
        size_t targetBufferIndex;
        if (mResetRequested || mResetsEveryUpdate)
        {
            //Use source data to render to first buffer
            mSourceRenderable->getGeometry(renderOp);
            targetBufferIndex = 0;
        }
        else
        {
            //Use current front buffer to render to back buffer
            renderOp.mType = mType;
            renderOp.useIndexes = false;
            renderOp.vertexData = mVertexData.get();
            targetBufferIndex = 1 - mFrontBufferIndex;
        }

        if (!mVertexBuffers[targetBufferIndex] || 
            mVertexBuffers[targetBufferIndex]->getSizeInBytes() != bufSize)
        {
            reallocateBuffer(targetBufferIndex);
        }

        RenderSystem* targetRenderSystem = Root::getSingleton().getRenderSystem();

        //Draw the object
        D3D11VertexBuffer* vertexBuffer = static_cast<D3D11VertexBuffer*>(mVertexBuffers[targetBufferIndex].get());
    
        UINT offset[1] = { 0 };
        ID3D11Buffer* iBuffer[1];
        iBuffer[0] = vertexBuffer->getD3DVertexBuffer();
        mDevice.GetImmediateContext()->SOSetTargets( 1, iBuffer, offset );

        if (r2vbPass->hasVertexProgram())
        {
            targetRenderSystem->on(GPT_VERTEX_PROGRAM, 
                r2vbPass->getVertexProgramParameters(), GPV_ALL);
        }
        if (r2vbPass->hasGeometryProgram())
        {
            targetRenderSystem->on(GPT_GEOMETRY_PROGRAM,
                r2vbPass->getGeometryProgramParameters(), GPV_ALL);
        }

        // Remove fragment program
        mDevice.GetImmediateContext()->PSSetShader(NULL, NULL, 0);

        targetRenderSystem->_render(renderOp);  

        //Switch the vertex binding if necessary
        if (targetBufferIndex != mFrontBufferIndex)
        {
            mVertexData->vertexBufferBinding->unsetAllBindings();
            mVertexData->vertexBufferBinding->setBinding(0, mVertexBuffers[targetBufferIndex]);
            mFrontBufferIndex = targetBufferIndex;
        }

        // Remove stream output buffer 
        iBuffer[0]=NULL;
        mDevice.GetImmediateContext()->SOSetTargets( 1, iBuffer, offset );
        //Clear the reset flag
        mResetRequested = false;

        // Enable DrawAuto
        mVertexData->vertexCount = -1;
    }
    //-----------------------------------------------------------------------------
    void D3D11RenderToVertexBuffer::reallocateBuffer(size_t index)
    {
        N_assert(index == 0 || index == 1);
        if (mVertexBuffers[index])
        {
            mVertexBuffers[index].reset();
        }

        mVertexBuffers[index] = mBufManager->createStreamOutputVertexBuffer(
            mVertexData->vertexDeclaration->getVertexSize(0), mMaxVertexCount, 
            Buffer::HBU_STATIC_WRITE_ONLY
            );
    }
}
