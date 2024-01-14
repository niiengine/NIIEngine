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
#include "OgreD3D11MultiRenderTarget.h"
#include "OgreD3D11RenderSystem.h"
#include "OgreRoot.h"
#include "OgreD3D11Texture.h"
#include "OgreD3D11HardwarePixelBuffer.h"

namespace Ogre 
{
    //---------------------------------------------------------------------
    D3D11MultiRenderTarget::D3D11MultiRenderTarget(const String &name) :
    //---------------------------------------------------------------------
    MultiRenderTarget(name), mNumberOfViews(0)
    {
        /// Clear targets
        for(size_t x=0; x<NII_MAX_RenderTarget; ++x)
        {
            mRenderTargetViews[x] = 0;
            mRenderTargets[x] = 0;
        }
    }
    //---------------------------------------------------------------------
    D3D11MultiRenderTarget::~D3D11MultiRenderTarget()
    {
    }
    //---------------------------------------------------------------------
    void D3D11MultiRenderTarget::bindSurfaceImpl(size_t attachment, RenderTexture *target)
    {
        N_assert(attachment<NII_MAX_RenderTarget);

        D3D11RenderTarget* d3d11RenderTarget = dynamic_cast<D3D11RenderTarget*>(target);

        /// Find first non-null target
        int y;
        for(y=0; y<NII_MAX_RenderTarget && !mRenderTargets[y]; ++y) ;

        if(y!=NII_MAX_RenderTarget)
        {
            /// If there is another target bound, compare sizes
            if(mRenderTargets[y]->getWidth() != target->getWidth() ||
                mRenderTargets[y]->getHeight() != target->getHeight())
            {
				N_EXCEPT(Exception::ERR_INVALIDPARAMS, 
                    "MultiRenderTarget surfaces are not of same size", 
                    "D3D11MultiRenderTarget::bindSurface"
                    );
            }
        }

        mRenderTargets[attachment] = target;
        mRenderTargetViews[attachment] =
            d3d11RenderTarget ? d3d11RenderTarget->getRenderTargetView() : NULL;

        if(mNumberOfViews < NII_MAX_RenderTarget)
            mNumberOfViews++;

        checkAndUpdate();
    }
    //---------------------------------------------------------------------
    void D3D11MultiRenderTarget::unbindSurfaceImpl(size_t attachment)
    {
        N_assert(attachment<NII_MAX_RenderTarget);
        mRenderTargetViews[attachment] = 0;

        if(mNumberOfViews > 0)
            mNumberOfViews--;

        checkAndUpdate();
    }
    //---------------------------------------------------------------------
    void D3D11MultiRenderTarget::update(void)
    {
        //D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(Root::getSingleton().getRenderSystem());

        MultiRenderTarget::update();
    }

    uint D3D11MultiRenderTarget::getNumberOfViews() const { return mNumberOfViews; }

    ID3D11Texture2D* D3D11MultiRenderTarget::getSurface(uint index) const
    {
        D3D11RenderTarget* renderTarget = dynamic_cast<D3D11RenderTarget*>(mRenderTargets[index]);
        return renderTarget ? renderTarget->getSurface() : NULL;
    }

    ID3D11RenderTargetView* D3D11MultiRenderTarget::getRenderTargetView(uint index) const
    {
        D3D11RenderTarget* renderTarget = dynamic_cast<D3D11RenderTarget*>(mRenderTargets[index]);
        return renderTarget ? renderTarget->getRenderTargetView() : NULL;
    }

    void D3D11MultiRenderTarget::checkAndUpdate()
    {
        if(mRenderTargets[0])
        {
            mWidth = static_cast<unsigned int>(mRenderTargets[0]->getWidth());
            mHeight = static_cast<unsigned int>(mRenderTargets[0]->getHeight());
        }
        else
        {
            mWidth = 0;
            mHeight = 0;
        }
    }
    //---------------------------------------------------------------------


}

