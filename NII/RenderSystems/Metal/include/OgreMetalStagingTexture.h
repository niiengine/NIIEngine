/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2017 Torus Knot Software Ltd

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

#ifndef _OgreMetalStagingTexture_H_
#define _OgreMetalStagingTexture_H_

#include "OgreMetalPrerequisites.h"
#include "OgreStagingTextureBufferImpl.h"

#include "Vao/OgreMetalDynamicBuffer.h"

#include "OgreHeaderPrefix.h"

namespace Ogre
{
    class _OgreMetalExport MetalStagingTexture : public MappedTexture
    {
        id<MTLBuffer>   mVboName;
        MetalDevice     *mDevice;
    public:
        MetalStagingTexture( GBufferManager *vaoManager, PixelFormat format, size_t sizeBytes, MetalDevice *device );
        virtual ~MetalStagingTexture();
        
        virtual void unmap(void);

        virtual void read(Texture *dst, NCount dstmipLevel, const PixelBlock * dstBox, const PixelBlock & box, bool host = false);
        
        virtual void write(Texture * src, NCount srcmipmap, const PixelBlock * srcBox, const PixelBlock & box, bool host = false);
    protected:
        PixelBlock mapImpl(const PixelVolume & vol);
        virtual bool belongsToUs( const PixelBlock &box );
    };
}

#include "OgreHeaderSuffix.h"

#endif
