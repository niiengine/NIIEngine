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
#include "NiiUICursor.h"
#include "NiiUIManager.h"
#include "NiiUICommon.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiPixelManager.h"
#include "NiiGBufferManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    N_EVENT(NII, Cursor,    ImageChangeEvent,           0);
    N_EVENT(NII, Cursor,    DefaultImageChangeEvent,    1);
    N_EVENT(NII, Cursor,    EventCount,                 2);
    //------------------------------------------------------------------------
    Cursor::Cursor() :
        mView(0),
        mNormalView(0),
        mPos(0.0f, 0.0f),
        mSize(0.0f, 0.0f),
        mArea(0, 0, N_Only(UI).getDisplaySize(N_PRIMARY_Viewport)),
        mBufferValid(false),
        mVisible(true)
    {
        notifySignal(Cursor::EventCount);
        N_Only(GBuffer).create(mBuffer);
        mBuffer->setClip(mArea);
        setMoveArea(&mArea);
        setPos(N_Only(UI).getCursorInitPos());
    }
    //------------------------------------------------------------------------
    Cursor::Cursor(const Recti & area) :
        mView(0),
        mNormalView(0),
        mArea(area),
        mPos(0.0f, 0.0f),
        mSize(0.0f, 0.0f),
        mBufferValid(false),
        mVisible(true)
    {
        N_Only(GBuffer).create(mBuffer);
        mBuffer->setClip(mArea);
        setMoveArea(&mArea);
        setPos(N_Only(UI).getCursorInitPos());
    }
    //------------------------------------------------------------------------
    Cursor::~Cursor()
    {
        N_Only(GBuffer).destroy(mBuffer);
    }
    //------------------------------------------------------------------------
    void Cursor::setImage(PixelBuffer * pb)
    {
        if(pb == mView)
            return;

        mView = pb;
        mBufferValid = false;

        CursorViewArgs args(this);
        args.mImage = pb;
        onImageChange(&args);
    }
    //------------------------------------------------------------------------
    void Cursor::setDefaultImage(PixelBuffer * pb)
    {
        if(pb == mNormalView)
            return;

        mNormalView = pb;
        mBufferValid = mView != 0;

        CursorViewArgs args(this);
        args.mImage = pb;
        onDefaultImageChange(&args);
    }
    //------------------------------------------------------------------------
    void Cursor::draw()
    {
        if(!mVisible || !mView)
            return;

        if(!mBufferValid)
        {
            mBufferValid = true;
            mBuffer->clear();

            if(!mView)
                return;

            if(mSize.mWidth != 0.0f || mSize.mHeight != 0.0f)
            {
                const PlaneSizef & sz = mView->getSize();
                const Vector2f & oft = mView->getOffset();
                Vector2f temp;
                temp.x = mSize.mWidth / sz.mWidth * oft.x - oft.x;
                temp.y = mSize.mHeight / sz.mHeight * oft.y - oft.y;
                const ColourArea c(0XFFFFFFFF);
                mBuffer->add(*mView, Rectf(temp, mSize), 0, c);
            }
            else
            {
                const ColourArea c(0XFFFFFFFF);
                mBuffer->add(*mView, Rectf(Vector2f(0, 0), mView->getSize()), 0, c);
            }

        }

        mBuffer->flush();
    }
    //------------------------------------------------------------------------
    void Cursor::setPos(const Vector2f & pos)
    {
        mPos = pos;
        clampMoveArea();

        mBuffer->setOffset(Vector3f(mPos.x, mPos.y, 0));
    }
    //------------------------------------------------------------------------
    void Cursor::clampMoveArea()
    {
        Recti absarea(abs(mMoveArea, mArea.getSize()));

        if(mPos.x >= absarea.mRight)
            mPos.x = absarea.mRight -1;

        if(mPos.y >= absarea.mBottom)
            mPos.y = absarea.mBottom -1;

        if(mPos.y < absarea.mTop)
            mPos.y = absarea.mTop;

        if(mPos.x < absarea.mLeft)
            mPos.x = absarea.mLeft;
    }
    //------------------------------------------------------------------------
    void Cursor::setMoveArea(const Recti * area)
    {
        if(!area)
        {
            mMoveArea.mLeft = RelPosi(mArea.mLeft / mArea.getWidth(), 0);
            mMoveArea.mTop = RelPosi(mArea.mTop / mArea.getHeight(), 0);
            mMoveArea.mRight = RelPosi(mArea.mRight / mArea.getWidth(), 0);
            mMoveArea.mBottom = RelPosi(mArea.mBottom / mArea.getHeight(), 0);
        }
        else
        {
            Recti finalArea(area->intersects(mArea));
            mMoveArea.mLeft = RelPosi(finalArea.mLeft / mArea.getWidth(), 0);
            mMoveArea.mTop = RelPosi(finalArea.mTop / mArea.getHeight(), 0);
            mMoveArea.mRight = RelPosi(finalArea.mRight / mArea.getWidth(), 0);
            mMoveArea.mBottom = RelPosi(finalArea.mBottom / mArea.getHeight(), 0);
        }

        clampMoveArea();
    }
    //------------------------------------------------------------------------
    Recti Cursor::getMoveArea() const
    {
        return Recti(abs(mMoveArea, mArea.getSize()));
    }
    //------------------------------------------------------------------------
    void Cursor::setEvnMoveArea(const RelRecti * area)
    {
        if(area)
        {
            mMoveArea = *area;
        }
        else
        {
            mMoveArea.mLeft = RelPosi(mArea.mLeft / mArea.getWidth(), 0);
            mMoveArea.mTop = RelPosi(mArea.mTop / mArea.getHeight(), 0);
            mMoveArea.mRight = RelPosi(mArea.mRight / mArea.getWidth(), 0);
            mMoveArea.mBottom = RelPosi(mArea.mBottom / mArea.getHeight(), 0);
        }

        clampMoveArea();
    }
    //------------------------------------------------------------------------
    void Cursor::notifyView(const PlaneSizei & nsize)
    {
        const Recti screenArea(0, 0, nsize);
        mBuffer->setClip(screenArea);
        mBufferValid = false;
    }
    //------------------------------------------------------------------------
    void Cursor::setRenderSize(const PlaneSizef & size)
    {
        mSize = size;
        mBufferValid = false;
    }
    //------------------------------------------------------------------------
    void Cursor::refresh()
    {
        mBufferValid = false;
    }
    //------------------------------------------------------------------------
    void Cursor::onImageChange(const CursorViewArgs * arg)
    {
        signal(ImageChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Cursor::onDefaultImageChange(const CursorViewArgs * arg)
    {
        signal(DefaultImageChangeEvent, arg);
    }
    //------------------------------------------------------------------------
}
}