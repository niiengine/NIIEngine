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

#include "NiiPreProcess.h"
#include "NiiViewport.h"
#include "NiiMath.h"
#include "NiiViewWindow.h"
#include "NiiMaterialManager.h"
#include "NiiRenderSys.h"
#include "NiiLogManager.h"
#include "NiiFrameObj.h"
#include "NiiCamera.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ViewportListener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ViewportListener::~ViewportListener() 
    {
    }
    //------------------------------------------------------------------------
    void ViewportListener::onInit(Viewport * obj)
    {
    
    }
    //------------------------------------------------------------------------
    void ViewportListener::onDestroy(Viewport * obj)
    {
    }
    //------------------------------------------------------------------------
    void ViewportListener::onCull(Viewport * obj, Camera * target)
    {
    }
    //------------------------------------------------------------------------ 
    void ViewportListener::onRender(Viewport * obj, Camera * target)
    {
    }
    //------------------------------------------------------------------------
    void ViewportListener::onSize(Viewport * obj)
    {
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // Viewport
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    NIIf Viewport::mDefaultDirection = 0;
    //---------------------------------------------------------------------
    Viewport::Viewport(ViewportID id, Texture * buf, const Vector4f & area, const Vector4f & clip, NIIi ZOrder, NCount mipLevel) :
        mID(id),
        mTarget(buf),
        mLeft(area.x),
        mTop(area.y),
        mWidth(area.z),
        mHeight(area.w),
        mClipLeft(clip.x),
        mClipTop(clip.y),
        mPClipWidth(clip.z),
        mCliplHeight(clip.w),
        mZOrder(ZOrder),
        mBgColour(Colour::Black),
        mDepthClearValue(1),
        mClearMark(FBT_COLOUR | FBT_DEPTH),
        mVisableMark(0xFFFFFFFF),
        mLightMark(0xFFFFFFFF),
        mMipMap(mipLevel),
        mMaterial(0)
    {
        mMark = Viewport_SyncRender | Viewport_SyncAspect | Viewport_AutoClear | 
            Viewport_AutoUpdate | Viewport_OverlayEnable | Viewport_SkyEnable |
                Viewport_ShadowEnable | Viewport_FullBuffer | Viewport_NonClip;
#if N_COMPILER != N_CPP_GCCE && N_PLAT != N_PLAT_ANDROID
        N_Only(Log).stream(LL_Trace)
            << _I18n("Creating viewport on target '") << buf->getName() << _T("'")
            << _I18n(", relative dimensions ")    << std::ios::fixed << std::setprecision(2)
            << _T("L: ") << area.x << " T: " << area.y << _T(" W: ") << area.z << _T(" H: ") << area.w
            << _T(" ZOrder: ") << ZOrder;
#endif
        // Set the default orientation mode
        mDirection = mDefaultDirection;

        // Set the default material scheme
        mMaterial = N_Engine().getRender()->getMaterial();

        if(mTarget)
            sync();
    }
    //---------------------------------------------------------------------
    Viewport::~Viewport()
    {
        ViewportListenerList::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onDestroy(this);
        }

        RenderSys * rs = N_Engine().getRender();
        if(rs && (rs->getViewports() == this))
        {
            rs->_setViewport(0);
        }
    }
    //---------------------------------------------------------------------
    void Viewport::syncRenderImpl()
    {
        N_Mark(mMark, Viewport_SyncRender);
    }
    //---------------------------------------------------------------------
    void Viewport::syncAspectImpl()
    {
        N_Mark(mMark, Viewport_SyncAspect);
    }
    //---------------------------------------------------------------------
    void Viewport::sync()
    {
        if(mTarget)
        {
            mTargetHeight = mTarget->getHeight() >> mMipMap;
            mTargetWidth = mTarget->getWidth() >> mMipMap;

            bool nonClip = mLeft == mClipLeft && mTop == mClipTop && mWidth == mClipWidth && mHeight == mCliplHeight;

            N_MarkBool(mMark, Viewport_FullBuffer, mLeft == mTop == 0 && mWidth == mHeight == 1.0 && nonClip);

            /*mPixelLeft = mLeft * mTargetWidth;
            mPixelTop = mTop * mTargetHeight;
            mPixelWidth = mWidth * mTargetWidth;
            mPixelHeight = mHeight * mTargetHeight;

            mClipPixelLeft = mClipLeft * mTargetWidth;
            mClipPixelTop = mClipTop * mTargetHeight;
            mClipPixelWidth = mClipWidth * mTargetWidth;
            mClipPixelHeight = mCliplHeight * mTargetHeight;*/

            // This will check if the cameras isAutoAspectRatio() property is set.
            // If it's true its aspect ratio is fit to the current viewport
            // If it's false the camera remains unchanged.
            // This allows cameras to be used to render to many viewports,
            // which can have their own dimensions and aspect ratios.
            NIIi tpw = getPixelWidth();
            NIIi tph = getPixelHeight();

    #if NII_VIEWPORT_DIR_NONE == 0
            switch((NIIi)mDirection)
            {
            case 0:
            case 180:
                mViewWidth = tpw;
                mViewHeight = tph;
                break;
            case 90:
            case 270:
                mViewWidth = tph;
                mViewHeight = tpw;
                break;
            }
    #else
            mViewWidth = tpw;
            mViewHeight = tph;
    #endif

            N_NonMark(mMark, Viewport_SyncRender);
            N_NonMark(mMark, Viewport_SyncAspect);
            ViewportListenerList::iterator i, iend = mListeners.end();
            for(i = mListeners.begin(); i != iend; ++i)
            {
                (*i)->onSize(this);
            }
        }
        else
        {
            mLeft = 0;
            mTop = 0;
            mWidth = 0;
            mHeight = 0;
            mClipLeft = 0;
            mClipTop = 0;
            mClipWidth = 0;
            mCliplHeight = 0;

            N_Mark(mMark, Viewport_NonClip);
            N_Mark(mMark, Viewport_FullBuffer);
            N_NonMark(mMark, Viewport_SyncRender);
            N_NonMark(mMark, Viewport_SyncAspect);
        }
    }
    //---------------------------------------------------------------------
    void Viewport::setArea(const Vector4f & area, const Vector4f & clip, NCount mipLevel)
    {
        mLeft = area.x;
        mTop = area.y;
        mWidth = area.z;
        mHeight = area.w;

        mClipLeft = clip.x;
        mClipTop = clip.y;
        mClipWidth = clip.z;
        mClipHeight = clip.w;

        mMipMap = mipLevel;
        sync();
    }
    //---------------------------------------------------------------------
    void Viewport::setArea(NIIf left, NIIf top, NIIf width, NIIf height)
    {
        mLeft = left;
        mTop = top;
        mWidth = width;
        mHeight = height;
        sync();
    }
    //---------------------------------------------------------------------
    void Viewport::getPixelArea(NIIi & left, NIIi & top, NIIi & width, NIIi & height) const
    {
        left = getPixelLeft();
        top = getPixelTop();
        width = getPixelWidth();
        height = getPixelHeight();
#if NII_VIEWPORT_DIR_NONE == 0
        switch((NIIi)mDirection)
        {
        case 0:
        case 180:
            mViewWidth = width;
            mViewHeight = height;
            break;
        case 90:
        case 270:
            mViewWidth = height;
            mViewHeight = width;
            break;
        }
#else
        mViewWidth = width;
        mViewHeight = height;
#endif
    }
    //---------------------------------------------------------------------
    void Viewport::setClip(NIIf left, NIIf top, NIIf width, NIIf height)
    {
        mClipLeft = left;
        mClipTop = top;
        mClipWidth = width;
        mClipHeight = height;
        sync();
    }
    //---------------------------------------------------------------------
    void Viewport::getPixelClip(NIIi & left, NIIi & top, NIIi & width, NIIi & height) const
    {
        left = getClipPixelLeft();
        top = getClipPixelTop();
        width = getClipPixelWidth();
        height = getClipPixelHeight();
    }
    //---------------------------------------------------------------------
    void Viewport::update(Camera * target, const RenderQueueFusion * rqfusion)
    {
        if(target)
        {
            float ar = (NIIf) getPixelWidth() / (NIIf) getPixelHeight(); 
            if(target->isAutoAspectRatio() && target->getAspectRatio() != ar)
            {
                target->setAspectRatio(ar);

    #if NII_VIEWPORT_DIR_NONE == 0
                target->setDirection(mDirection);
    #endif
            }
            target->setViewport(this);
            
            target->cull(this, rqfusion);

            ViewportListenerList::iterator i, iend = mListeners.end();
            for(i = mListeners.begin(); i != iend; ++i)
            {
                (*i)->onCull(this, target);
            }

            target->render(this, rqfusion);
            
            ViewportListenerList::iterator i, iend = mListeners.end();
            for(i = mListeners.begin(); i != iend; ++i)
            {
                (*i)->onRender(this, target);
            }
        }
    }
    //---------------------------------------------------------------------
    void Viewport::setDirection(NIIf dir)
    {
#if NII_VIEWPORT_DIR_NONE != 0
        N_EXCEPT(UnImpl, _I18n("Setting Viewport orientation mode is not supported"));
#endif
        mDirection = dir;

        // Update the render system config
#if N_PLAT == N_PLAT_IOS
        RenderSys * rs = N_Engine().getRender();
        if(mDirection == 270)
            rs->setConfig("Orientation", "Landscape Left");
        else if(mDirection == 90)
            rs->setConfig("Orientation", "Landscape Right");
        else if(mDirection == 0)
            rs->setConfig("Orientation", "Portrait");
#endif
        N_NonMark(mMark, Viewport_SyncAspect);
    }
    //---------------------------------------------------------------------
    /*void Viewport::setCustomRender(Nid id)
    { 
        if (id == 0)
            mQueueFusion = 0;
        else
            mQueueFusion = SpaceManager::getQueueFusion(id);
    }*/
    //---------------------------------------------------------------------
    void Viewport::setDefaultDirection(NIIf f)
    {
#if NII_VIEWPORT_DIR_NONE != 0
        N_EXCEPT(UnImpl, _I18n("Setting default Viewport orientation mode is not supported"));
#endif
        mDefaultDirection = f;
    }
    //---------------------------------------------------------------------
    NIIf Viewport::getDefaultDirection()
    {
#if NII_VIEWPORT_DIR_NONE != 0
        N_EXCEPT(UnImpl, _I18n("Getting default Viewport orientation mode is not supported"));
#endif
        return mDefaultDirection;
    }
    //---------------------------------------------------------------------
    void Viewport::onRender(Camera * target)
    {
        ViewportListenerList::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onRender(this, target);
        }
    }
    //---------------------------------------------------------------------
    void Viewport::setAutoClear(bool enable, Nmark32 mark)
    {
        N_MarkBool(mMark, Viewport_AutoClear, enable);
        mClearMark = mark;
    }
    //---------------------------------------------------------------------
    void Viewport::clear(Nmark32 mark, const Colour & col, NIIf depth, Nui16 stencil)
    {
        RenderSys * rs = N_Engine().getRender();
        if(rs)
        {
            Viewport * old = rs->getViewports();
            if(old && old == this)
            {
                rs->clearBuffer(mark, col, depth, stencil);
            }
            else if(old)
            {
                rs->_setViewport(this);
                rs->clearBuffer(mark, col, depth, stencil);
                rs->_setViewport(old);
            }
        }
    }
    //-----------------------------------------------------------------------
    void Viewport::add(ViewportListener * vl)
    {
        if(std::find(mListeners.begin(), mListeners.end(), vl) == mListeners.end())
        {
            mListeners.push_back(vl);
            vl->onInit(this);
        }
    }
    //-----------------------------------------------------------------------
    void Viewport::remove(ViewportListener * vl)
    {
        ViewportListenerList::iterator i = std::find(mListeners.begin(), mListeners.end(), vl);
        if (i != mListeners.end())
        {
            mListeners.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void Viewport::setBuffer(Texture * buf) const
    {
        mTarget = buf;
        sync();
    }
    //-----------------------------------------------------------------------
    void createViewport(ViewportList & out, NCount cnt, Texture * buf,
        const Vector4 * area, const Vector4 * clip, Nui8 mipLevel, bool overlay)
    {
        for(size_t i = 0; i < cnt; ++i)
        {
            Viewport * vp = N_new Viewport(0, buf, area[i], clip[i], 0, mipLevel);
            vp->setOverlayEnable(overlay);
            out.push_back(vp);
        }
    }
    //-----------------------------------------------------------------------
    void deleteViewport(ViewportList & in)
    {
        ViewportList::iterator i, iend = in.size();
        for(i = in.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        in.clear();
    }
    //-----------------------------------------------------------------------
}