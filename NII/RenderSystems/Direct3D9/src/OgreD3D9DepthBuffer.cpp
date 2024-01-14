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
#include "OgreD3D9DepthBuffer.h"
#include "OgreD3D9RenderSystem.h"
#include "OgreRenderTarget.h"

namespace Ogre
{
    D3D9DepthBuffer::D3D9DepthBuffer( uint16 poolId, D3D9RenderSystem *renderSystem,
                                      IDirect3DDevice9 *creator, IDirect3DSurface9 *depthBufferSurf,
                                      D3DFORMAT fmt, uint32 width, uint32 height,
                                      uint32 fsaa, uint32 multiSampleQuality, bool isManual ) :
                DepthBuffer( poolId, 0, width, height, fsaa, "", isManual ),
                mDepthBuffer( depthBufferSurf ),
                mCreator( creator ),
                mMultiSampleQuality( multiSampleQuality ),
                mD3DFormat( fmt ),
                mRenderSys( renderSystem )
    {
        switch( fmt )
        {
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D15S1:
        case D3DFMT_D16:
            mBitDepth = 16;
            break;
        case D3DFMT_D32:
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_D24X4S4:
        case D3DFMT_D32F_LOCKABLE:
        case D3DFMT_D24FS8:
            mBitDepth = 32;
            break;
        }
    }

    D3D9DepthBuffer::~D3D9DepthBuffer()
    {
        if( !mManual )
            mDepthBuffer->Release();
        mDepthBuffer = 0;

        mCreator = 0;
    }
    //---------------------------------------------------------------------
    bool D3D9DepthBuffer::isCompatible( RenderTarget *renderTarget ) const
    {
        IDirect3DSurface9* pBack[NII_MAX_RenderTarget];
        memset( pBack, 0, sizeof(pBack) );
        renderTarget->getPlatformData( "DDBACKBUFFER", &pBack );
        if( !pBack[0] )
            return false;

        D3DSURFACE_DESC srfDesc;
        if( SUCCEEDED(pBack[0]->GetDesc(&srfDesc)) )
        {
            //RenderSystem will determine if bitdepths match (i.e. 32 bit RT don't like 16 bit Depth)
            //This is the same function used to create them. Note results are usually cached so this should
            //be quick
            D3DFORMAT fmt = mRenderSys->_getDepthStencilFormatFor( srfDesc.Format );
            IDirect3DDevice9 *activeDevice = mRenderSys->getActiveD3D9Device();

            if( mCreator == activeDevice &&
                fmt == mD3DFormat &&
                mFsaa == srfDesc.MultiSampleType &&
                mMultiSampleQuality == srfDesc.MultiSampleQuality &&
                this->getWidth() >= renderTarget->getWidth() &&
                this->getHeight() >= renderTarget->getHeight() )
            {
                return true;
            }
        }

        return false;
    }
    //---------------------------------------------------------------------
    IDirect3DDevice9* D3D9DepthBuffer::getDeviceCreator() const
    {
        return mCreator;
    }
    //---------------------------------------------------------------------
    IDirect3DSurface9* D3D9DepthBuffer::getDepthBufferSurface() const
    {
        return mDepthBuffer;
    }
}
