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
#include "NiiFrameObj.h"
#include "NiiTextureManager.h"
#include "NiiStrConv.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiFrameObjListener.h"
#include "NiiEngine.h"
#include "NiiExtFrameBuffer.h"

namespace NII
{
    FrameObjID FrameObj::NextID = 65536;
    //---------------------------------------------------------------------------
    FrameObj::FrameObj(FrameObjID id, const String & name) :
        mName(name),
        mFlushLevel(NII_DEFAULT_RT_GROUP),
        mRasterGroup(GroupInner),
        mExtBuffer(0),
        mFSAA(0),
        mActive(true),
        mStereo(false)
    {
        mID = id ? id : NextID++;
        mStats = N_new FramesPerSecond(N_Engine().getTimer());
        resetFPS();
        mMark = 0;
        mMark |= M_AUTO;
    }
    //---------------------------------------------------------------------------
    FrameObj::~FrameObj()
    {
        Views::iterator i, iend = mViews.end();
        for(i = mViews.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mViews.clear();

        //ExtFrameBuffer keeps track of us, avoid a dangling pointer
        detachExt();
        // Write closing message
        N_Only(Log).stream(LL_Trace)
            << _I18n("Render Target '") << mName << "' "
            << _I18n("Average FPS: ") << mStats->mAvg << " "
            << _I18n("Best FPS: ") << mStats->mBest << " "
            << _I18n("Worst FPS: ") << mStats->mWorst;

        N_delete mStats;
    }
    //-----------------------------------------------------------------------
    void FrameObj::flush(bool swap)
    {
        updateImpl();

        if(swap)
        {
            RenderSys * rsys = N_Engine().getRender();
            FrameObj::swap(rsys->isVSync());
        }
    }
    //------------------------------------------------------------------------
    void FrameObj::swap(bool vsync)
    {
        (void)vsync;
    }
    //---------------------------------------------------------------------------
    Nui32 FrameObj::getWidth() const
    {
        return mWidth;
    }
    //---------------------------------------------------------------------------
    Nui32 FrameObj::getHeight() const
    {
        return mHeight;
    }
    //---------------------------------------------------------------------------
    Nui32 FrameObj::getColourDepth() const
    {
        return mColourDepth;
    }
    //---------------------------------------------------------------------------
    void FrameObj::setRasterGroup(GroupID gid)
    {
        if(mRasterGroup != gid)
        {
            mRasterGroup = gid;
            detachExt();
        }
    }
    //-----------------------------------------------------------------------
    bool FrameObj::attachExt(ExtFrameBuffer * ext)
    {
        if(ext->isSuitable(this))
        {
            detachExt();
            mExtBuffer = ext;
            mExtBuffer->attach(this);
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------------
    void FrameObj::detachExt()
    {
        if(mExtBuffer)
        {
            mExtBuffer->detach(this);
            mExtBuffer = 0;
        }
    }
    //---------------------------------------------------------------------------
    void FrameObj::updateImpl()
    {
        preFlush();

        Viewport * viewport;
        Views::iterator it = mViews.begin();
        while(it != mViews.end())
        {
            viewport = (*it).second;
            if(viewport->isAutoUpdate())
            {
                flushImpl(viewport);
            }
            ++it;
        }

        endFlush();
    }
    //---------------------------------------------------------------------------
    void FrameObj::preFlush()
    {
        FrameObjArgs evt;
        evt.mFrame = this;

        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onPreFlush(&evt);
        }
        mTriCount = 0;
        mBatchCount = 0;
    }
    //---------------------------------------------------------------------------
    void FrameObj::endFlush()
    {
        FrameObjArgs evt;
        evt.mFrame = this;

        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onEndFlush(&evt);
        }
    }
    //---------------------------------------------------------------------------
    void FrameObj::flushImpl(Viewport * viewport)
    {
        N_assert(viewport->getBuffer() == this, "FrameObj::flushImpl the requested viewport is not bound to the rendertarget!");

        FrameViewArgs evt;
        evt.mFrame = this;
        evt.mView = viewport;

        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onPreViewFlush(&evt);
        }

        viewport->update();

        mTriCount += viewport->getTriangleCount();/// ??
        mBatchCount += viewport->getBatchCount(); /// ??

        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onEndViewFlush(&evt);
        }
    }
    //---------------------------------------------------------------------------
    Viewport * FrameObj::createViewport(ViewportID id, Camera * cam, NIIf left, NIIf top,
        NIIf width, NIIf height, NIIi z)
    {
        // Check no existing viewport with this Z-order
        Views::iterator it = mViews.find(z);
        if(it != mViews.end())
        {
            StringStream str;
            str << _I18n("帧对象 ") << mName << _I18n(" 不能创建Z次序 ") << z
                << _I18n(" 的视口,因为已经存在");
            N_EXCEPT(InvalidParam, str.str());
        }
        // Add viewport to list
        // Order based on Z-Order
        Viewport * vp = N_new Viewport(id, cam, this, left, top, width, height, z);
        mViews.insert(Views::value_type(z, vp));
        return vp;
    }
    //-----------------------------------------------------------------------
    void FrameObj::destroyViewport(Nidx index)
    {
        Nidx temp = 0;
        Views::iterator i, iend = mViews.end();
        for(i = mViews.begin(); i != iend; ++i, ++temp)
        {
            if(temp == index)
            {
                N_delete i->second;
                mViews.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void FrameObj::destroyViewport(Viewport * v)
    {
        Views::iterator i, iend = mViews.end();
        for(i = mViews.begin(); i != iend; ++i)
        {
            if(i->second == v)
            {
                N_delete i->second;
                mViews.erase(i);
                break;
            }
        }
    }
    //---------------------------------------------------------------------------
    void FrameObj::destroyAllViewport()
    {
        Views::iterator i, iend = mViews.end();
        for(i = mViews.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mViews.clear();
    }
    //---------------------------------------------------------------------------
    PixelFormat FrameObj::getFormat() const
    {
        return PF_BYTE_RGBA;
    }
    //---------------------------------------------------------------------------
    void FrameObj::resetFPS()
    {
        mTriCount = 0;
        mBatchCount = 0;
        mStats->reset();
    }
    //-----------------------------------------------------------------------
    void FrameObj::add(FrameObjListener * lis)
    {
        mListeners.push_back(lis);
    }
    //-----------------------------------------------------------------------
    void FrameObj::remove(FrameObjListener * lis)
    {
        Listeners::iterator i, iend = mListeners.end();
        for (i = mListeners.begin(); i != iend; ++i)
        {
            if (*i == lis)
            {
                mListeners.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void FrameObj::removeAllListener()
    {
        mListeners.clear();
    }
    //-----------------------------------------------------------------------
    Viewport * FrameObj::getViewport(Nidx index)
    {
        N_assert (index < mViews.size(), "Index out of bounds");

        Views::iterator i = mViews.begin();
        while(index--)
            ++i;
        return i->second;
    }
    //-----------------------------------------------------------------------
    void FrameObj::setActive(bool b)    //ok
    {
        mActive = b;
    }
    //-----------------------------------------------------------------------
    bool FrameObj::isActive() const     //ok
    {
        return mActive;
    }
    //-----------------------------------------------------------------------
    bool FrameObj::isFlipping() const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    bool FrameObj::isStereo() const
    {
        return mStereo;
    }
    //-----------------------------------------------------------------------
    void FrameObj::_notifyDestroy(const Camera * obj)
    {
        Views::iterator i, iend = mViews.end();
        for(i = mViews.begin(); i != iend; ++i)
        {
            Viewport * v = i->second;
            if(v->getCamera() == obj)
            {
                v->setCamera(0);
            }
        }
    }
    //-----------------------------------------------------------------------
    void FrameObj::_notifyDestroy(const Viewport *)
    {
    }
    //-----------------------------------------------------------------------
    bool FrameObj::isFirstWindow() const
    {
        return false;
    }
    //-----------------------------------------------------------------------
    bool FrameObj::isHardwareSRGB() const
    {
        return (mMark & M_SRGB);
    }
    //------------------------------------------------------------------------
}