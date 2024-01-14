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
#include "OgreD3D9RenderSystem.h"
#include "OgreD3D9HardwareBufferManager.h"
#include "OgreD3D9HardwareVertexBuffer.h"
#include "OgreD3D9HardwareIndexBuffer.h"
#include "OgreD3D9VertexDeclaration.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

namespace Ogre {
    //-----------------------------------------------------------------------
    D3D9HardwareBufferManager::D3D9HardwareBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
    {
        destroyAllDeclarations();
        destroyAllBindings();
    }
    //-----------------------------------------------------------------------
    HardwareVertexBufferSharedPtr 
    D3D9HardwareBufferManager::
    createVertexBuffer(size_t vertexSize, size_t numVerts, Buffer::Usage usage,
        bool useShadowBuffer)
    {
        N_assert (numVerts > 0);
#if OGRE_D3D_MANAGE_BUFFERS
        // Override shadow buffer setting; managed buffers are automatically
        // backed by system memory
        // Don't override shadow buffer if discardable, since then we use
        // unmanaged buffers for speed (avoids write-through overhead)
        // Don't override if we use directX9EX, since then we don't have managed
        // pool. And creating non-write only default pool causes a performance warning. 
        if (useShadowBuffer && !(usage & Buffer::HBU_DISCARDABLE) &&
            !D3D9RenderSystem::isDirectX9Ex())
        {
            useShadowBuffer = false;
            // Also drop any WRITE_ONLY so we can read direct
            if (usage == Buffer::HBU_DYNAMIC_WRITE_ONLY)
            {
                usage = Buffer::HBU_DYNAMIC;
            }
            else if (usage == Buffer::HBU_STATIC_WRITE_ONLY)
            {
                usage = Buffer::HBU_STATIC;
            }
        }
        //If we have write only buffers in DirectX9Ex we will turn on the discardable flag.
        //Otherwise Ogre will operates in far less framerate
        if (D3D9RenderSystem::isDirectX9Ex() && (usage & Buffer::HBU_WRITE_ONLY))
        {
            usage = (Buffer::Usage)
                ((unsigned int)usage | (unsigned int)Buffer::HBU_DISCARDABLE);
        }
#endif
        D3D9HardwareVertexBuffer* vbuf = N_new D3D9HardwareVertexBuffer(
            this, vertexSize, numVerts, usage, false, useShadowBuffer);
        {
                    OGRE_LOCK_MUTEX(mVertexBuffersMutex);
            mVertexBuffers.insert(vbuf);
        }
        return HardwareVertexBufferSharedPtr(vbuf);
    }
    //-----------------------------------------------------------------------
    HardwareIndexBufferSharedPtr 
    D3D9HardwareBufferManager::
    createIndexBuffer(IndexBuffer::IndexType itype, size_t numIndexes, 
        Buffer::Usage usage, bool useShadowBuffer)
    {
        N_assert (numIndexes > 0);
#if OGRE_D3D_MANAGE_BUFFERS
        // Override shadow buffer setting; managed buffers are automatically
        // backed by system memory
        // Don't override shadow buffer if discardable, since then we use
        // unmanaged buffers for speed (avoids write-through overhead)
        // Don't override if we use directX9EX, since then we don't have managed
        // pool. And creating non-write only default pool causes a performance warning. 
        if (useShadowBuffer && !(usage & Buffer::HBU_DISCARDABLE) &&
            !D3D9RenderSystem::isDirectX9Ex())
        {
            useShadowBuffer = false;
            // Also drop any WRITE_ONLY so we can read direct
            if (usage == Buffer::HBU_DYNAMIC_WRITE_ONLY)
            {
                usage = Buffer::HBU_DYNAMIC;
            }
            else if (usage == Buffer::HBU_STATIC_WRITE_ONLY)
            {
                usage = Buffer::HBU_STATIC;
            }
        }
        //If we have write only buffers in DirectX9Ex we will turn on the discardable flag.
        //Otherwise Ogre will operates in far less framerate
        if (D3D9RenderSystem::isDirectX9Ex() && (usage & Buffer::HBU_WRITE_ONLY))
        {
            usage = (Buffer::Usage)
                ((unsigned int)usage | (unsigned int)Buffer::HBU_DISCARDABLE);
        }
#endif
        D3D9HardwareIndexBuffer* idx = N_new D3D9HardwareIndexBuffer(
            this, itype, numIndexes, usage, false, useShadowBuffer);
        {
                    OGRE_LOCK_MUTEX(mIndexBuffersMutex);
            mIndexBuffers.insert(idx);
        }
        return HardwareIndexBufferSharedPtr(idx);
            
    }
    //-----------------------------------------------------------------------
    RenderToVertexBufferSharedPtr 
        D3D9HardwareBufferManager::createRenderToVertexBuffer()
    {
        N_EXCEPT(RenderingAPI, 
            "Direct3D9 does not support render to vertex buffer objects", 
            "D3D9HardwareBufferManager::createRenderToVertexBuffer");
    }
    //---------------------------------------------------------------------
    StructBuffer * 
        D3D9HardwareBufferManager::createUniformBuffer(size_t sizeBytes, Buffer::Usage usage, bool useShadowBuffer, const String& name)
    {
        N_EXCEPT(RenderingAPI, 
                "Uniform buffer not supported in Direct3D 9 RenderSystem.",
                "D3D9HardwareBufferManager::createUniformBuffer");
    }
    //-----------------------------------------------------------------------
    CounterBuffer *
    D3D9HardwareBufferManager::createCounterBuffer(size_t sizeBytes,
                                                          Buffer::Usage usage, bool useShadowBuffer, const String& name)
    {
        N_EXCEPT(RenderingAPI,
                    "D3D9 does not support atomic counter buffers",
                    "D3D9HardwareBufferManager::createCounterBuffer");
    }
    //-----------------------------------------------------------------------
    VertexDeclaration* D3D9HardwareBufferManager::createVertexDeclarationImpl(void)
    {
        return N_new D3D9VertexDeclaration();
    }
    //-----------------------------------------------------------------------
    void D3D9HardwareBufferManager::destroyVertexDeclarationImpl(VertexDeclaration* decl)
    {
        N_delete decl;
    }
}
