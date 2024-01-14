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
#include "OgreD3D11DepthBuffer.h"
#include "OgreD3D11HardwarePixelBuffer.h"
#include "OgreD3D11Texture.h"
#include "OgreD3D11Mappings.h"
#include "OgreD3D11RenderTarget.h"

namespace Ogre
{
    D3D11DepthBuffer::D3D11DepthBuffer( uint16 poolId, D3D11RenderSystem *renderSystem,
                                        ID3D11DepthStencilView *depthBufferView,
                                        uint32 width, uint32 height,
                                        uint32 fsaa, uint32 multiSampleQuality, bool isManual ) :
                ExtFrameBuffer( poolId, width, height, 0, fsaa, "", isManual ),
                mDepthStencilView( depthBufferView ),
                mMultiSampleQuality( multiSampleQuality),
                mRenderSys(renderSystem)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC pDesc;
        mDepthStencilView->GetDesc( &pDesc );
        // Unknown PixelFormat at the moment
        PixelFormat format = D3D11Mappings::_getPF(pDesc.Format);
        mBitDepth = static_cast<uint16>(PixelUtil::getNumElemBytes(format) * 8);
    }

    D3D11DepthBuffer::~D3D11DepthBuffer()
    {
        if( !mManual )
            mDepthStencilView->Release();
        mDepthStencilView = 0;
    }
    //---------------------------------------------------------------------
    bool D3D11DepthBuffer::isCompatible( RenderTarget *renderTarget ) const
    {
        D3D11_TEXTURE2D_DESC BBDesc;

        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(renderTarget);
        if (!d3d11RenderTarget)
        {
            return false;
        }

        ID3D11Texture2D* d3d11texture = d3d11RenderTarget->getSurface();
        if (!d3d11texture)
        {
            return false;
        }

        d3d11texture->GetDesc(&BBDesc);

        //RenderSystem will determine if bitdepths match (i.e. 32 bit RT don't like 16 bit Depth)
        //This is the same function used to create them. Note results are usually cached so this should
        //be quick

        //TODO: Needs to check format too!
        if( mFsaa == BBDesc.SampleDesc.Count &&
            mMultiSampleQuality == BBDesc.SampleDesc.Quality &&
            this->getWidth() == renderTarget->getWidth() &&
            this->getHeight() == renderTarget->getHeight() )
        {
            return true;
        }

        return false;
    }
    //---------------------------------------------------------------------
    ID3D11DepthStencilView* D3D11DepthBuffer::getDepthStencilView() const
    {
        return mDepthStencilView;
    }
    //---------------------------------------------------------------------
    void D3D11DepthBuffer::_resized(ID3D11DepthStencilView *depthBufferView, uint32 width, uint32 height)
    {
        mHeight = height;
        mWidth = width;

        mDepthStencilView = depthBufferView;
    }
}
