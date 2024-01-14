/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2016 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef _Ogre_MetalAsyncTicket_H_
#define _Ogre_MetalAsyncTicket_H_

#include "OgreMetalPrerequisites.h"

#include "Vao/OgreAsyncTicket.h"
#include "OgreAsyncTextureTicket.h"
#include "OgreTextureBox.h"
#include <dispatch/dispatch.h>

namespace Ogre
{
    class _OgreMetalExport MetalAsyncTicket : public GpuBufferAsync
    {
    public:
        MetalAsyncTicket( GpuBuffer *creator, MappedBuffer * mapbuf, NCount oft, NCount size, MetalDevice *device );
        virtual ~MetalAsyncTicket();

        virtual OperationReason isDataValid() const;
    protected:
        virtual const void* mapImpl(void);
        
        dispatch_semaphore_t    mFenceName;
        MetalDevice             *mDevice;
    };
    
    /** 
    @version NIIEngine 5.0.0
    */
    class _OgreMetalExport MetalAsyncTextureTicket : public TextureAsync
    {
    public:
        MetalAsyncTextureTicket( NCount width, NCount height, NCount depth, NCount array, Texture::Type textureType,
                                 PixelFormat pf, MetalGBufferManager *vaoManager, MetalDevice *device );
        virtual ~MetalAsyncTextureTicket();

        virtual bool isDataValid(void);
    protected:
        virtual PixelBlock mapImpl(NCount depthIdx, NCount arrayIdx = 0, NCount oft = 0);

        virtual void unmapImpl();

        void waitForDownloadToFinish();

        virtual void writeImpl( Texture * src, NCount srcmipmap, PixelBlock *srcBox, NCount oft = 0, bool notify = false);
    protected:
        id<MTLBuffer>   mVboName;

        uint32 mDownloadFrame;
        dispatch_semaphore_t mAccurateFence;
        MetalGBufferManager * mMag;
        MetalDevice * mDevice;
    };
}

#endif
