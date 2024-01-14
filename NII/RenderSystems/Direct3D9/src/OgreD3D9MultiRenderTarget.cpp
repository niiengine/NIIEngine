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
#include "OgreD3D9MultiRenderTarget.h"
#include "OgreD3D9HardwarePixelBuffer.h"
#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreBitwise.h"
#include "OgreD3D9RenderSystem.h"
#include "OgreRoot.h"
#include "OgreD3D9Device.h"
#include "OgreD3D9DeviceManager.h"

namespace Ogre 
{
    D3D9MultiRenderTarget::D3D9MultiRenderTarget(const String &name):
        MultiRenderTarget(name)
    {
        /// Clear targets
        for(size_t x=0; x<NII_MAX_RenderTarget; ++x)
        {
            mRenderTargets[x] = 0;
        }
    }
    D3D9MultiRenderTarget::~D3D9MultiRenderTarget()
    {
    }

    void D3D9MultiRenderTarget::bindSurfaceImpl(size_t attachment, RenderTexture *target)
    {
        N_assert(attachment<NII_MAX_RenderTarget);
        /// Get buffer and surface to bind to
        D3D9HardwarePixelBuffer *buffer = 0;
        target->getPlatformData("BUFFER", &buffer);
        N_assert(buffer);

        /// Find first non-null target
        int y;
        for(y=0; y<NII_MAX_RenderTarget && !mRenderTargets[y]; ++y) ;
        
        if(y!=NII_MAX_RenderTarget)
        {
            /// If there is another target bound, compare sizes
            if (mRenderTargets[y]->getWidth() != buffer->getWidth() ||
                mRenderTargets[y]->getHeight() != buffer->getHeight())
            {
                N_EXCEPT(
                    Exception::ERR_INVALIDPARAMS, 
                    "MultiRenderTarget surfaces are not of same size", 
                    "D3D9MultiRenderTarget::bindSurface");
            }

            if (!Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS)
                && (PixelUtil::getNumElemBits(mRenderTargets[y]->getFormat()) != 
                    PixelUtil::getNumElemBits(buffer->getFormat())))
            {
                N_EXCEPT(
                    Exception::ERR_INVALIDPARAMS, 
                    "MultiRenderTarget surfaces are not of same bit depth and hardware requires it", 
                    "D3D9MultiRenderTarget::bindSurface"
                );
            }
        }

        mRenderTargets[attachment] = buffer;
        checkAndUpdate();
    }

    void D3D9MultiRenderTarget::unbindSurfaceImpl(size_t attachment)
    {
        N_assert(attachment<NII_MAX_RenderTarget);
        mRenderTargets[attachment] = 0;
        checkAndUpdate();
    }

    void D3D9MultiRenderTarget::update(bool _swap)
    {     
        D3D9DeviceManager* deviceManager = D3D9RenderSystem::getDeviceManager();        
        D3D9Device* currRenderWindowDevice = deviceManager->getActiveRenderTargetDevice();

        if (currRenderWindowDevice != NULL)
        {
            if (currRenderWindowDevice->isDeviceLost() == false)
                MultiRenderTarget::update(_swap);
        }
        else
        {
            for (UINT i=0; i < deviceManager->getDeviceCount(); ++i)
            {
                D3D9Device* device = deviceManager->getDevice(i);

                if (device->isDeviceLost() == false)
                {
                    deviceManager->setActiveRenderTargetDevice(device);
                    MultiRenderTarget::update(_swap);
                    deviceManager->setActiveRenderTargetDevice(NULL);
                }                               
            }
        }       
    }

    void D3D9MultiRenderTarget::getPlatformData(const String& name, void *pData)
    {
        if(name == "DDBACKBUFFER")
        {
            IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
            /// Transfer surfaces
            for(size_t x=0; x<NII_MAX_RenderTarget; ++x)
            {
                if(mRenderTargets[x] != NULL)                               
                    pSurf[x] = mRenderTargets[x]->getSurface(D3D9RenderSystem::getActiveD3D9Device());          
            }
        }
    }

    void D3D9MultiRenderTarget::checkAndUpdate()
    {
        if(mRenderTargets[0])
        {
            mWidth  = (unsigned int)mRenderTargets[0]->getWidth();
            mHeight = (unsigned int)mRenderTargets[0]->getHeight();
        }
        else
        {
            mWidth = 0;
            mHeight = 0;
        }
    }
}

