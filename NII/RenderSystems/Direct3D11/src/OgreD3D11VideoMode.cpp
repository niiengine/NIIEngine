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
#include "OgreD3D11VideoMode.h"

namespace Ogre 
{
    //---------------------------------------------------------------------
    String D3D11VideoMode::getDescription() const
    {
        unsigned int colourDepth = 16;
        if( mModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_TYPELESS ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_UINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_SINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_TYPELESS ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_UINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_SINT 
            )
            colourDepth = 32;

        return StringUtil::format("%d x %d @ %d-bit colour", mModeDesc.Width, mModeDesc.Height, colourDepth);
    }
    //---------------------------------------------------------------------
    unsigned int D3D11VideoMode::getColourDepth() const
    {
        unsigned int colourDepth = 16;
        if( mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_TYPELESS ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_UINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_SINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_TYPELESS ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_UINT ||
            mModeDesc.Format == DXGI_FORMAT_R32G32B32_SINT 
            )
            colourDepth = 32;

        return colourDepth;
    }
    //---------------------------------------------------------------------
    unsigned int D3D11VideoMode::getWidth() const
    {
        return mModeDesc.Width;
    }
    //---------------------------------------------------------------------
    unsigned int D3D11VideoMode::getHeight() const
    {
        return mModeDesc.Height;
    }
    //---------------------------------------------------------------------
    DXGI_FORMAT D3D11VideoMode::getFormat() const
    {
        return mModeDesc.Format;
    }
    //---------------------------------------------------------------------
    DXGI_RATIONAL D3D11VideoMode::getRefreshRate() const
    {
        return mModeDesc.RefreshRate;
    }
    //---------------------------------------------------------------------
    DXGI_OUTPUT_DESC D3D11VideoMode::getDisplayMode() const
    {
        return mDisplayMode;
    }
    //---------------------------------------------------------------------
    DXGI_MODE_DESC D3D11VideoMode::getModeDesc() const
    {
        return mModeDesc;
    }
    //---------------------------------------------------------------------
    void D3D11VideoMode::increaseRefreshRate( DXGI_RATIONAL rr )
    {
        mModeDesc.RefreshRate = rr;
    }
    //---------------------------------------------------------------------
    D3D11VideoMode::D3D11VideoMode( DXGI_OUTPUT_DESC d3ddm,DXGI_MODE_DESC ModeDesc )
    {
        modeNumber = ++modeCount; mDisplayMode = d3ddm;mModeDesc=ModeDesc;
    }
    //---------------------------------------------------------------------
    D3D11VideoMode::D3D11VideoMode( const D3D11VideoMode &ob )
    {
        modeNumber = ++modeCount; mDisplayMode = ob.mDisplayMode;mModeDesc=ob.mModeDesc;
    }
    //---------------------------------------------------------------------
    D3D11VideoMode::D3D11VideoMode()
    {
        modeNumber = ++modeCount; ZeroMemory( &mDisplayMode, sizeof(DXGI_OUTPUT_DESC) );ZeroMemory( &mModeDesc, sizeof(DXGI_MODE_DESC) );
    }
    //---------------------------------------------------------------------
    D3D11VideoMode::~D3D11VideoMode()
    {
        modeCount--;
    }
    //---------------------------------------------------------------------
}
