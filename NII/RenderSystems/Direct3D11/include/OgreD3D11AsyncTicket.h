/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2014 Torus Knot Software Ltd

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

#ifndef _Ogre_D3D11AsyncTicket_H_
#define _Ogre_D3D11AsyncTicket_H_

#include "OgreD3D11Prerequisites.h"

#include "Vao/OgreAsyncTicket.h"
#include "OgreTextureBox.h"

namespace Ogre
{
    class _OgreD3D11Export D3D11AsyncTicket : public GpuBufferAsync
    {
    public:
        D3D11AsyncTicket( GpuBuffer * creator, MappedBuffer * buffer, NCount oft, NCount size, D3D11Device &device );
        virtual ~D3D11AsyncTicket();

        virtual OperationReason isDataValid() const;
    protected:
        ComPtr<ID3D11Query> mFenceName;
        D3D11Device &mDevice;

        virtual const void* mapImpl(void);
    };
    
    /** See TextureAsync
    */
    class _OgreD3D11Export D3D11AsyncTextureTicket : public TextureAsync
    {
    public:
        D3D11AsyncTextureTicket( NCount width, NCount height, NCount depth, NCount array, Texture::Type textureType, PixelFormat pf, D3D11GBufferManager *vaoManager );
        virtual ~D3D11AsyncTextureTicket();

        virtual bool isDataValid(void);
        
    protected:
        ComPtr<ID3D11Resource> mStagingTexture;

        uint32          mDownloadFrame;
        ComPtr<ID3D11Query> mAccurateFence;
        D3D11GBufferManager *mMag;
        uint32          mMappedSlice;
        bool            mIsArray2DTexture;

        virtual PixelBlock mapImpl(NCount depthIdx, NCount arrayIdx = 0, NCount oft = 0);
        virtual void unmapImpl(void);

        virtual bool getArray(void) const;

        void waitForDownloadToFinish(void);

        virtual void writeImpl( Texture * src, NCount srcmipmap, PixelBlock * srcBox, NCount oft, bool notify = false);
    };
}

#endif
