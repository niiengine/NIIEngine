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

#include "OgreMetalTextureGpuWindow.h"
#include "OgreMetalMappings.h"
#include "OgreMetalTextureGpuManager.h"
#include "OgreMetalWindow.h"

#include "OgreTextureGpuListener.h"
#include "OgreTextureBox.h"
#include "OgreVector2.h"

#include "Vao/OgreGBufferManager.h"

#include "OgreException.h"

namespace Ogre
{
    MetalTextureGpuWindow::MetalTextureGpuWindow(
            GpuPageOutStrategy::GpuPageOutStrategy rtype,
            GBufferManager *vaoManager, IdString name, uint32 textureFlags,
            Texture::Type::Texture::Type initialType,
            TextureManager *textureManager, MetalWindow *window ) :
        MetalTextureGpuRenderTarget( rtype, vaoManager, name,
                                     textureFlags, initialType, textureManager ),
        mWindow( window )
    {
        mTextureType = Texture::T_2D;
        mFinalTextureName = 0;
        mDisplayTextureName = 0;
    }
    //-----------------------------------------------------------------------------------
    MetalTextureGpuWindow::~MetalTextureGpuWindow()
    {
        freeInternalImpl();
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::createInternalImpl(void)
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::freeInternalImpl(void)
    {
        _setBackbuffer( 0 );
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::notifyGpuData(void)
    {
        N_assert( mCurrentStorage == StorageType::Resident );
        process( Texture::Process::O_Valid );
    }
    //-----------------------------------------------------------------------------------
    bool MetalTextureGpuWindow::isValidImpl(void) const
    {
        return mCurrentStorage == StorageType::Resident;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::swap(void)
    {
        mWindow->swap();
        //Release strong references
        mFinalTextureName = 0;
        mDisplayTextureName = 0;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::getPlatformData( IdString name, void *pData )
    {
        if( name == "Window" )
            *static_cast<Window**>(pData) = mWindow;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::_setToDisplayDummyTexture(void)
    {
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::poolImpl( const TexturePool *newPool, uint16 slice )
    {
        N_EXCEPT( UnImpl, "",
                     "MetalTextureGpuWindow::poolImpl" );
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::nextDrawable(void)
    {
        mWindow->nextDrawable();
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::_setBackbuffer( id<MTLTexture> backbuffer )
    {
        mFinalTextureName = backbuffer;
        mDisplayTextureName = backbuffer;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::_setMsaaBackbuffer( id<MTLTexture> msaaTex )
    {
        mMsaaFramebufferName = msaaTex;
    }
    //-----------------------------------------------------------------------------------
    void MetalTextureGpuWindow::setTextureType( Texture::Type::Texture::Type textureType )
    {
        N_EXCEPT( UnImpl,
                     "You cannot call setTextureType if isWindow is true",
                     "MetalTextureGpuWindow::setTextureType" );
    }
}
