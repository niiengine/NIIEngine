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
#include "NiiUISheet.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIManager.h"
#include "NiiUITooltip.h"
#include "NiiUIWindow.h"
#include "NiiViewWindow.h"
#include "NiiFontManager.h"
#include "NiiUISheet.h"
#include "NiiTextureManager.h"
#include "NiiGBufferManager.h"
#include "NiiRenderEffect.h"
#include "NiiGpuBuffer.h"
#include "NiiTextureFrame.h"
#include "NiiRenderSys.h"
#include "NiiCamera.h"
#include "NiiEngine.h"
#include "NiiTimer.h"

#define DEFAULT_SIZE 512

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SheetKeyboardControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SheetKeyboardControlItem::SheetKeyboardControlItem()
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SheetMouseControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SheetMouseControlItem::SheetMouseControlItem()
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SheetJoyPadControlItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SheetJoyPadControlItem::SheetJoyPadControlItem()
    {

    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // InstanceRenderEventArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner InstanceRenderEventArgs : public GeometryLevelEventArgs
    {
    public:
        InstanceRenderEventArgs(GeometryQueue * queue, GeometryLevel gl) :
            mQueue(queue),
            GeometryLevelEventArgs(gl) {}

        GeometryQueue * mQueue;
    };
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, UISubSheet, AttachEvent, 0);
    N_EVENT(CommandObj, UISubSheet, DetachEvent, 1);
    N_EVENT(CommandObj, UISubSheet, PreRenderEvent, 2);
    N_EVENT(CommandObj, UISubSheet, EndRenderEvent, 3);
    N_EVENT(CommandObj, UISubSheet, EventCount, 4);
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // UISubSheet
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    UISubSheet::UISubSheet(RenderSys * rs, UISheet * own, const Vector2f & oft) :
        mSys(rs),
        mParent(own),
        mOft(oft),
        mRel(0),
        mPos(0, 0),
        mSize(0.0f, 0.0f),
        mViewExtent(0),
        mDst(0),
        mView(0),
        mArea(0, 0, 0, 0),
        mRot(Quaternion::IDENTITY),
        mProjectMatrix(Matrix3f::ZERO),
        mValidProjectMatrix(false),
        mValidBuffer(false),
        mValidView(false),
        mRefresh(true)
    {
        notifySignal(UISubSheet::EventCount);
        N_Only(GBuffer).create(mBuffer);
        mBuffer->setBlendMode(PBM_Average);
        setNativeSize(PlaneSizef(DEFAULT_SIZE, DEFAULT_SIZE));
        mRenderQueue.insert(Npair(GL_Overlay, N_new GeometryQueue()));
        mRenderQueue.insert(Npair(GL_Tip, N_new GeometryQueue()));
        mRenderQueue.insert(Npair(GL_Ext, N_new GeometryQueue()));
    }
    //------------------------------------------------------------------------
    UISubSheet::~UISubSheet()
    {
        if (mView)
        {
            N_delete mView;
            mView = 0;
        }
        if (mDst)
        {
            N_Only(Texture).destroy(mDst->getOriginID());
        }

        ChildList::iterator i, iend = mChild.end();
        for (i = mChild.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mChild.clear();
        GeoQueues::iterator r, rend = mRenderQueue.end();
        for (r = mRenderQueue.begin(); r != rend; ++r)
        {
            N_delete r->second;
        }
        mRenderQueue.clear();
        N_Only(GBuffer).destroy(mBuffer);
    }
    //------------------------------------------------------------------------
    void UISubSheet::add(GeometryLevel queue, GeometryPixel * obj)
    {
        mRenderQueue[queue]->add(obj);
    }
    //------------------------------------------------------------------------
    void UISubSheet::remove(GeometryLevel queue, GeometryPixel * obj)
    {
        mRenderQueue[queue]->remove(obj);
    }
    //------------------------------------------------------------------------
    void UISubSheet::clear(GeometryLevel queue)
    {
        mRenderQueue[queue]->clear();
    }
    //------------------------------------------------------------------------
    void UISubSheet::clear()
    {
        GeoQueues::iterator i, iend = mRenderQueue.end();
        for (i = mRenderQueue.begin(); i != iend; ++i)
        {
            i->second->clear();
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::flush()
    {
        if (!mValidBuffer)
        {
            mBuffer->clear();

            RenderEffect * effect = mBuffer->getEffect();

            if (!effect || effect->onRefush(mSys, mBuffer))
            {
                NIIf tu = mSize.mWidth / mDst->getWidth();
                NIIf tv = mSize.mHeight / mDst->getHeight();
                const Rectf tex_rect(false ? Rectf(0, 1, tu, 1 - tv) : Rectf(0, 0, tu, tv));

                const Rectf area(0, 0, mSize.mWidth, mSize.mHeight);
                const Colour c(1, 1, 1, 1);
                Vertex v[6];

                // 顶点 0
                v[0].mPos = Vector3f(area.mLeft, area.mTop, 0.0f);
                v[0].mColour = c;
                v[0].mCoord = Vector2f(tex_rect.mLeft, tex_rect.mTop);

                // 顶点 1
                v[1].mPos = Vector3f(area.mLeft, area.mBottom, 0.0f);
                v[1].mColour = c;
                v[1].mCoord = Vector2f(tex_rect.mLeft, tex_rect.mBottom);

                // 顶点 2
                v[2].mPos = Vector3f(area.mRight, area.mBottom, 0.0f);
                v[2].mColour = c;
                v[2].mCoord = Vector2f(tex_rect.mRight, tex_rect.mBottom);

                // 顶点 3
                v[3].mPos = Vector3f(area.mRight, area.mTop, 0.0f);
                v[3].mColour = c;
                v[3].mCoord = Vector2f(tex_rect.mRight, tex_rect.mTop);

                // 顶点 4
                v[4].mPos = Vector3f(area.mLeft, area.mTop, 0.0f);
                v[4].mColour = c;
                v[4].mCoord = Vector2f(tex_rect.mLeft, tex_rect.mTop);

                // 顶点 5
                v[5].mPos = Vector3f(area.mRight, area.mBottom, 0.0f);
                v[5].mColour = c;
                v[5].mCoord = Vector2f(tex_rect.mRight, tex_rect.mBottom);

                mBuffer->setTexture(mDst);
                mBuffer->add(v, 6);
            }
            mValidBuffer = true;
        }

        if (mRefresh)
        {
            updateMatrix();
            updateViewport();

            UIManager * mag = N_OnlyPtr(UI);
            mSys->_setViewport(mView);
            mag->setProjMatrix(mProjectMatrix);
            mag->setViewMatrix(Matrix4f::IDENTITY);

            InstanceRenderEventArgs arg(0, GL_Unknow);

            GeoQueues::iterator i, iend = mRenderQueue.end();
            for (i = mRenderQueue.begin(); i != iend; ++i)
            {
                arg.mGL = i->first;
                arg.mQueue = i->second;

                signal(PreRenderEvent, &arg);
                arg.mQueue->flush();
                signal(EndRenderEvent, &arg);
            }

            mRefresh = false;
        }

        // 实例表面
        mParent->add(GL_Base, mBuffer);
    }
    //------------------------------------------------------------------------
    void UISubSheet::rebuild()
    {
        mValidBuffer = false;
    }
    //------------------------------------------------------------------------
    void UISubSheet::create(UISubSheet *& out)
    {
        out = N_new UISubSheet(mSys, mParent, mPos);
        out->mRel = this;
        mParent->attach(out);
        mChild.push_back(out);
        refresh();
    }
    //------------------------------------------------------------------------
    void UISubSheet::destroy(UISubSheet * obj)
    {
        if (obj->mParent == mParent)
        {
            mParent->detach(obj);
            ChildList::iterator i = std::find(mChild.begin(), mChild.end(), obj);
            if (i != mChild.end())
            {
                mChild.erase(i);
                refresh();
            }
            N_delete obj;
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::attach(UISubSheet * obj)
    {
        UISheet * parent = obj->mParent;
        if (parent != mParent)
        {
            parent->detach(obj);
            mParent->attach(obj);
            mChild.push_back(obj);
            refresh();
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::detach(UISubSheet * obj)
    {
        ChildList::iterator i = std::find(mChild.begin(), mChild.end(), obj);
        if (i != mChild.end())
        {
            mParent->detach(*i);
            mChild.erase(i);
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::update(TimeDurMS cost)
    {
        RenderEffect * effect = mBuffer->getEffect();

        if (effect)
        {
            mValidBuffer &= effect->onUpdate(cost, mSys);
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::refresh()
    {
        if (!mRefresh)
        {
            mRefresh = true;

            updateViewport();

            Viewport * const pre = mSys->getViewports();

            mSys->_setViewport(mView);
            mSys->clearBuffer(NII::FBT_COLOUR, NII::Colour(0, 0, 0, 0));
            mSys->_setViewport(pre);
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::setClip(const Recti & area)
    {
        Recti tempi;
        tempi.mLeft = area.mLeft - mOft.x;
        tempi.mTop = area.mTop - mOft.y;
        tempi.mRight = area.mRight - mOft.x;
        tempi.mBottom = area.mBottom - mOft.y;
        mBuffer->setClip(tempi);
    }
    //------------------------------------------------------------------------
    void UISubSheet::setArea(const Rectf & area)
    {
        mArea = area;

        if (mView)
        {
            FrameObj * frame = mView->getBuffer();
            if (frame)
            {
                mView->setArea(
                    mArea.left() / frame->getWidth(),
                    mArea.top() / frame->getHeight(),
                    mArea.width() / frame->getWidth(),
                    mArea.height() / frame->getHeight());
            }
        }

        mValidView = false;
        mValidProjectMatrix = false;
    }
    //------------------------------------------------------------------------
    void UISubSheet::setPos(const Vector2f & pos)
    {
        mPos = pos;

        Vector3f trans(mPos.x, mPos.y, 0.0f);
        trans.x -= mOft.x;
        trans.y -= mOft.y;
        mBuffer->setOffset(trans);
        ChildList::iterator i, iend = mChild.end();
        for (i = mChild.begin(); i != iend; ++i)
        {
            (*i)->mOft = mPos;
        }
    }
    //------------------------------------------------------------------------
    void UISubSheet::setSize(const PlaneSizef & size)
    {
        mSize = size;
        mValidBuffer = false;

        setNativeSize(size);
    }
    //------------------------------------------------------------------------
    void UISubSheet::setNativeSize(const PlaneSizef & size)
    {
        if ((mArea.width() >= size.mWidth) && (mArea.height() >= size.mHeight))
            return;

        if (mDst)
        {
            N_Only(Texture).destroy(mDst->getOriginID());
        }

        mDst = N_Only(Texture).createTexture(N_Only(Texture).genValidID(), GroupDefault,
            Texture::T_2D, PF_A8R8G8B8, size.mWidth, size.mHeight, 1, 0, Texture::MM_FRAME_RGB,
            0, 0, false, 0, StrUtil::WBLANK);

        mDst->setAutoDestroy(true);

        TextureFrame * frame = mDst->getBuffer()->getBuffer();
        setArea(Rectf(0.0f, 0.0f, (NIIf)frame->getWidth(), (NIIf)frame->getHeight()));

        if (mView)
        {
            N_delete mView;
            mView = N_new Viewport(N_UI_Viewport, 0, frame, 0, 0, 1, 1, 0);

            if (frame)
            {
                mView->setArea(
                    mArea.left() / frame->getWidth(),
                    mArea.top() / frame->getHeight(),
                    mArea.width() / frame->getWidth(),
                    mArea.height() / frame->getHeight());
            }
        }

        updateViewport();

        Viewport * pre = mSys->getViewports();
        mSys->_setViewport(mView);
        mSys->clearBuffer(NII::FBT_COLOUR, NII::Colour(0, 0, 0, 0));
        mSys->_setViewport(pre);
    }
    //------------------------------------------------------------------------
    void UISubSheet::setRotation(const Quaternion & rot)
    {
        mRot = rot;
        mBuffer->setRotation(mRot);
    }
    //------------------------------------------------------------------------
    void UISubSheet::setOrigin(const Vector3f & org)
    {
        mOrigin = org;
        mBuffer->setOrigin(mOrigin);
    }
    //------------------------------------------------------------------------
    void UISubSheet::updateMatrix()
    {
        if (mValidProjectMatrix)
            return;
        NIIf w = mArea.width();
        NIIf h = mArea.height();
        NIIf aspect = w / h;
        NIIf midx = w * 0.5f;
        mViewExtent = midx / (aspect * 0.267949192431123f);

        NIIf nearZ = mViewExtent * 0.5f;
        NIIf farZ = mViewExtent * 2.0f;
        NIIf nr_sub_far = nearZ - farZ;

        Matrix4f tmp(Matrix4f::ZERO);
        tmp[0][0] = 3.732050808f / aspect;
        tmp[0][3] = -mViewExtent;
        tmp[1][1] = -3.732050808f;
        tmp[1][3] = mViewExtent;
        tmp[2][2] = -((farZ + nearZ) / nr_sub_far);
        tmp[3][2] = 1.0f;
        tmp[3][3] = mViewExtent;
        mSys->getSysProj(tmp, mProjectMatrix);

        mValidProjectMatrix = true;
    }
    //-----------------------------------------------------------------------
    void UISubSheet::updateViewport()
    {
        if (mValidView)
            return;
        mView->sync();
        mValidView = true;
    }
    //------------------------------------------------------------------------
    void UISubSheet::affine(const Vector2f & in, Vector2f & out)
    {
        if ((mRot == Quaternion::IDENTITY))
        {
            out = in;
            return;
        }

        Vector2f _in(in - mOft);

        mParent->affine(&mBuffer->getMatrix(), _in, out);
        out.x += mPos.x;
        out.y += mPos.y;
    }
    //------------------------------------------------------------------------
    void UISubSheet::affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const
    {
        const_cast<UISubSheet *>(this)->updateMatrix();

        NIIf midx = mArea.width() * 0.5f;
        NIIf midy = mArea.height() * 0.5f;

        // viewport matrix
        const Matrix4f vpmat(
            midx, 0, 0, mArea.left() + midx,
            0, -midy, 0, mArea.top() + midy,
            0, 0, 1, 0,
            0, 0, 0, 1);

        // matrices used for projecting and unprojecting points
        const Matrix4f proj(*m * mProjectMatrix * vpmat);
        const Matrix4f unproj(proj.inverse());

        Vector3f _in;

        // unproject the ends of the ray
        _in.x = midx;
        _in.y = midy;
        _in.z = -mViewExtent;
        const Vector3f r1(unproj * _in);
        _in.x = in.x;
        _in.y = in.y;
        _in.z = 0;
        // calculate vector of picking ray
        const Vector3f rv(r1 - unproj * _in);

        // project points to orientate them with GeometryPixel plane
        _in.x = 0.0;
        _in.y = 0.0;
        const Vector3f p1(proj * _in);
        _in.x = 1.0;
        _in.y = 0.0;
        const Vector3f p2(proj * _in);
        _in.x = 0.0;
        _in.y = 1.0;
        const Vector3f p3(proj * _in);

        // calculate the plane normal
        const Vector3f pn((p2 - p1).crossProduct(p3 - p1));
        // calculate distance from origin
        NIIf plen = pn.length();
        NIIf dist = -(p1.x * (pn.x / plen) + p1.y * (pn.y / plen) + p1.z * (pn.z / plen));

        // calculate intersection of ray and plane
        const NIIf pn_dot_rv = pn.dotProduct(rv);

        const NIIf tmp = pn_dot_rv != 0.0f ?
            (pn.dotProduct(r1) + dist) / pn_dot_rv : 0.0f;

        out.x = static_cast<NIIf>(r1.x - rv.x * tmp);
        out.y = static_cast<NIIf>(r1.y - rv.y * tmp);
    }
    //------------------------------------------------------------------------
    void UISubSheet::onAttach(const EventArgs * args)
    {
        signal(AttachEvent, args);
    }
    //------------------------------------------------------------------------
    void UISubSheet::onDetach(const EventArgs * args)
    {
        signal(DetachEvent, args);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CursorMultiTouch
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    struct CursorMultiTouch : public UIAlloc
    {
        CursorMultiTouch() :
            mTouchCount(0),
            mArea(0, 0, 0, 0)
        {
            mTimer = N_Engine().getTimer();
        }

        Widget * mWidget;
        Rectf mArea;
        Timer * mTimer;
        Nul mCost;
        NCount mTouchCount;
    };
    //------------------------------------------------------------------------
    void uisheet_notify(Container * parent, Widget * child, void (Container::*func)(const CursorEventArgs *), CursorEventArgs & args)
    {
        if(!parent || parent == child)
            return;

        Container * p = child->getParent();

        if(p)
        {
            if(p != parent)
            {
                uisheet_notify(parent, p, func, args);
            }

            args.mWidget = child;
            (p->*func)(&args);
        }
    }
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, UISheet, PreRenderEvent,                    0);
    N_EVENT(CommandObj, UISheet, EndRenderEvent,                    1);
    N_EVENT(CommandObj, UISheet, FontEvent,                         2);
    N_EVENT(CommandObj, UISheet, WindowChangeEvent,                 3);
    N_EVENT(CommandObj, UISheet, CursorAreaChangeEvent,             4);
    N_EVENT(CommandObj, UISheet, CursorMoveFactorChangeEvent,       5);
    N_EVENT(CommandObj, UISheet, CursorClickFactorChangeEvent,      6);
    N_EVENT(CommandObj, UISheet, CursorMultiClickFactorChangeEvent, 7);
    N_EVENT(CommandObj, UISheet, EventCount,                        8);
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // UISheet
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    UISheet::UISheet(RenderSys * rs, ViewWindow * win) :
        mCursorArea(PlaneSizef(12.0f, 12.0f)),
        mCursorMultiClickFactor(333),
        mCursorClickFactor(0),
        mCursorMoveFactor(1.0f),
        mWinComKey(0),
        mTooltip(0),
        mActive(0),
        mModal(0),
        mFocus(0),
        mFont(0),
        mSys(rs),
        mView(0),
        mViewExtent(0),
        mValidOverlay(true),
        mOverlay(0),
        mTarget(win),
        mProjectMatrix(Matrix3f::ZERO),
        mGenCursorMultiClick(true),
        mValidProjectMatrix(false),
        mAutoTooltip(false),
        mValidView(false),
        mRedraw(false)
    {
        notifySignal(UISheet::EventCount);
        mCursorTouch = N_new_array_t(CursorMultiTouch, MB_Button4 + 1);
        mCursor = N_new Cursor(Recti(0, 0, mTarget->getWidth(), mTarget->getHeight()));
        setArea(Recti(0, 0, mTarget->getWidth(), mTarget->getHeight()));
    }
    //------------------------------------------------------------------------
    UISheet::~UISheet()
    {
        Childs::iterator i, iend = mChild.end();
        for(i = mChild.begin(); i != iend; ++i)
        {
            N_delete *i;
        }

        if(mTooltip && mAutoTooltip)
        {
            N_Only(Widget).destroy(mTooltip);
            mTooltip = 0;
            mAutoTooltip = false;
        }

        WinList::iterator j, jend = mWinList.end();
        for(j = mWinList.begin(); j != jend; ++j)
        {
            (*j)->setSheet(0);
        }

        if(mView)
        {
            N_delete mView;
            mView = 0;
        }

        N_delete mCursor;
        N_delete_array_t(mCursorTouch, CursorMultiTouch, MB_Button4 + 1);
    }
    //------------------------------------------------------------------------
    void UISheet::apply()
    {
        updateMatrix();
        updateViewport();

        UIManager * mag = N_OnlyPtr(UI);
        mSys->_setViewport(mView);
        mag->setProjMatrix(mProjectMatrix);
        mag->setViewMatrix(Matrix4f::IDENTITY);
    }
    //------------------------------------------------------------------------
    void UISheet::add(GeometryLevel qid, GeometryPixel * obj)
    {
        mRenderQueue[qid].add(obj);
    }
    //------------------------------------------------------------------------
    void UISheet::remove(GeometryLevel qid, GeometryPixel * obj)
    {
        mRenderQueue[qid].remove(obj);
    }
    //------------------------------------------------------------------------
    void UISheet::clear(GeometryLevel qid)
    {
        mRenderQueue[qid].clear();
    }
    //------------------------------------------------------------------------
    void UISheet::clear()
    {
        Queues::iterator i, iend = mRenderQueue.end();
        for(i = mRenderQueue.begin(); i != iend; ++i)
        {
            i->second.clear();
        }
    }
    //------------------------------------------------------------------------
    void UISheet::setArea(const Recti & area)
    {
        mArea = area;

        if(mView)
        {
            FrameObj * rt = mView->getBuffer();
            if(rt)
            {
                mView->setArea(
                    mArea.left() / rt->getWidth(),
                    mArea.top() / rt->getHeight(),
                    mArea.width() / rt->getWidth(),
                    mArea.height() / rt->getHeight());
            }
        }

        mValidView = false;
        mValidProjectMatrix = false;

        mCursor->notifyView(mArea.getSize());

        WinList::iterator i, iend = mWinList.end();
        for(i = mWinList.begin(); i != iend; ++i)
        {
            WidgetEventArgs args(0);
            (*i)->onParentSize(&args);
        }
    }
    //------------------------------------------------------------------------
    Widget * UISheet::getCursorOver()
    {
        if(!mValidOverlay)
        {
            CursorEventArgs arg((Widget *)0);
            Vector2f mouse_pos(mCursor->getPos());
            Widget * const instant = getDest(mouse_pos, true);
            if(instant == mOverlay)
                return false;

            arg.mMouseArg.mScroll = 0;
            arg.mCount = 0;
            arg.mMouseArg.mButton = MB_Unknow;

            Widget * old = mOverlay;
            mOverlay = instant;

            // inform previous window the mouse has left it
            if(old)
            {
                arg.mWidget = old;
                Vector2f pos(old->mParent->getWindow()->affine(mouse_pos));
                arg.mLocalX = pos.x;
                arg.mLocalY = pos.y;
                old->onCursorLeave(&arg);
            }

            // inform window containing mouse that mouse has entered it
            if(mOverlay)
            {
                arg.mWidget = mOverlay;
                Vector2f pos(mOverlay->mParent->getWindow()->affine(mouse_pos));
                arg.mLocalX = pos.x;
                arg.mLocalY = pos.y;
                mOverlay->onCursorEnter(&arg);
            }

            Container * root = getAncestor(old, mOverlay);
            if(old)
            {
                uisheet_notify(root, old, &Container::onCursorLeaveArea, arg);
            }
            if(mOverlay)
            {
                uisheet_notify(root, mOverlay, &Container::onCursorEnterArea, arg);
            }
            mValidOverlay = true;
        }
        return mOverlay;
    }
    //------------------------------------------------------------------------
    void UISheet::create(UISubSheet *& out)
    {
        out = N_new UISubSheet(mSys, this, Vector2f::ZERO);
        mChild.push_back(out);
    }
    //------------------------------------------------------------------------
    void UISheet::destroy(UISubSheet * obj)
    {
        if(obj->getSheet() == this)
        {
            Childs::iterator i = std::find(mChild.begin(), mChild.end(), obj);
            if(i != mChild.end())
            {
                mChild.erase(i);
            }
            N_delete obj;
        }
    }
    //------------------------------------------------------------------------
    void UISheet::attach(UISubSheet * obj)
    {
        if(obj->getSheet() != this)
        {
            obj->getSheet()->detach(obj);
            mChild.push_back(obj);
            obj->mParent = this;
        }
    }
    //------------------------------------------------------------------------
    void UISheet::detach(UISubSheet * obj)
    {
        Childs::iterator i = std::find(mChild.begin(), mChild.end(), obj);
        if(i != mChild.end())
        {
            mChild.erase(i);
        }
        obj->mParent = 0;
    }
    //------------------------------------------------------------------------
    void UISheet::add(Window * obj)
    {
        if(obj)
        {
            UISheet * tmp = obj->getSheet();
            if(tmp == this)
                return;
            else if(tmp != 0)
                tmp->remove(obj);
        }
        else
        {
            return;
        }

        obj->setSheet(this);
        WidgetEventArgs args;
        obj->onParentSize(&args);
        redraw();

        mWinList.push_back(obj);
    }
    //------------------------------------------------------------------------
    void UISheet::remove(Window * obj)
    {
        if(obj->getSheet() == this)
        {
            WinList::iterator i = std::find(mWinList.begin(), mWinList.end(), obj);
            if(mWinList.end() != i)
            {
                mWinList.erase(i);
                obj->setSheet(0);
            }
        }
    }
    //------------------------------------------------------------------------
    void UISheet::setCursorMoveFactor(NIIf f)
    {
        mCursorMoveFactor = f;

        SheetEventArgs args(0, this, 0);
        onCursorMoveFactorChange(&args);
    }
    //------------------------------------------------------------------------
    void UISheet::setCursorClickFactor(TimeDurMS time)
    {
        mCursorClickFactor = time;

        SheetEventArgs args(0, this, 0);
        onCursorClickFactorChange(&args);
    }
    //------------------------------------------------------------------------
    void UISheet::setCursorMultiClickFactor(TimeDurMS time)
    {
        mCursorMultiClickFactor = time;

        SheetEventArgs args(0, this, 0);
        onCursorMultiClickFactorChange(&args);
    }
    //------------------------------------------------------------------------
    void UISheet::setCursorArea(const PlaneSizef & sz)
    {
        mCursorArea = sz;

        SheetEventArgs args(0, this, 0);
        onCursorAreaChange(&args);
    }
    //------------------------------------------------------------------------
    void UISheet::setFont(Font * obj)
    {
        mFont = obj;

        FontEventArgs args(obj);
        onFont(&args);
    }
    //------------------------------------------------------------------------
    Font * UISheet::getFont() const
    {
        if(mFont)
            return mFont;

        return N_Only(Font).getDefault();
    }
    //------------------------------------------------------------------------
    void UISheet::setTooltip(Tooltip * obj, bool autoDestroy)
    {
        if(mTooltip && mAutoTooltip)
        {
            N_Only(Widget).destroy(mTooltip);
            mTooltip = 0;
            mAutoTooltip = false;
        }

        mTooltip = obj;
        mAutoTooltip = autoDestroy;

        if(mTooltip)
            mTooltip->setDataWrite(false);
    }
    //------------------------------------------------------------------------
    void UISheet::setWindow(ViewWindow * obj)
    {
        if(mTarget == obj)
            return;

        ViewWindow * preobj = mTarget;

        if(mView)
        {
            N_delete mView;
            mView = 0;
            mValidView = false;
        }

        mTarget = obj;

        setArea(Recti(0, 0, mTarget->getWidth(), mTarget->getHeight()));

        WinList::iterator i, iend = mWinList.end();
        for(i = mWinList.begin(); i != iend; ++i)
        {
            WidgetEventArgs args(0);
            (*i)->onParentSize(&args);
        }

        RenderSheetEventArgs arg(0, this, 0, preobj);
        onWindowChange(&arg);
    }
    //------------------------------------------------------------------------
    Container * UISheet::getAncestor(Widget * w1, Widget * w2) const
    {
        if(!w2)
            return 0;

        if(w1 == w2 && w1->isContainer())
            return static_cast<Container *>(w1);

        // make sure w1 is always further up
        if(w1 && w2->isContainer() && w1->isAncestor(w2))
            return static_cast<Container *>(w2);

        Container * temp = w1->isContainer() ? static_cast<Container *>(w1) : w1->mParent;
        while(temp)
        {
            if(w2->isAncestor(temp))
                return temp;

            temp = temp->mParent;
        }
        return temp;
    }
    //------------------------------------------------------------------------
    void UISheet::resetCursorOver()
    {
        mValidOverlay = false;
    }
    //------------------------------------------------------------------------
    void UISheet::affine(const Matrix4f * m, const Vector2f & in, Vector2f & out) const
    {
        const_cast<UISheet *>(this)->updateMatrix();

        const NIIf midx = mArea.width() * 0.5f;
        const NIIf midy = mArea.height() * 0.5f;

        // viewport matrix
        const Matrix4f vpmat(
            midx,    0,    0,    mArea.left() + midx,
             0,    -midy,  0,    mArea.top() + midy,
             0,      0,    1,    0,
             0,      0,    0,    1);

        // matrices used for projecting and unprojecting points
        const Matrix4f proj(*m * mProjectMatrix * vpmat);
        const Matrix4f unproj(proj.inverse());

        Vector3f _in;

        // unproject the ends of the ray
        _in.x = midx;
        _in.y = midy;
        _in.z = -mViewExtent;
        const Vector3f r1(unproj * _in);
        _in.x = in.x;
        _in.y = in.y;
        _in.z = 0;
        // calculate vector of picking ray
        const Vector3f rv(r1 - unproj * _in);

        // project points to orientate them with GeometryPixel plane
        _in.x = 0.0;
        _in.y = 0.0;
        const Vector3f p1(proj * _in);
        _in.x = 1.0;
        _in.y = 0.0;
        const Vector3f p2(proj * _in);
        _in.x = 0.0;
        _in.y = 1.0;
        const Vector3f p3(proj * _in);

        // calculate the plane normal
        const Vector3f pn((p2 - p1).crossProduct(p3 - p1));
        // calculate distance from origin
        NIIf plen = pn.length();
        NIIf dist = -(p1.x * (pn.x / plen) + p1.y * (pn.y / plen) + p1.z * (pn.z / plen));

        // calculate intersection of ray and plane
        const NIIf pn_dot_rv = pn.dotProduct(rv);

        const NIIf tmp = pn_dot_rv != 0.0f ? (pn.dotProduct(r1) + dist) / pn_dot_rv : 0.0f;

        out.x = static_cast<NIIf>(r1.x - rv.x * tmp);
        out.y = static_cast<NIIf>(r1.y - rv.y * tmp);
    }
    //------------------------------------------------------------------------
    bool UISheet::onButtonDown(MouseButton button)
    {
        if(button > MB_Button4)
            return false;
        mWinComKey |= (1 << button);
        CursorEventArgs arg((Widget *)0);
        arg.mLocalX = mCursor->getPos().x;
        arg.mLocalY = mCursor->getPos().y;
        arg.mMouseArg.mX = 0;
        arg.mMouseArg.mY = 0;
        arg.mMouseArg.mButton = button;
        arg.mMouseArg.mScroll = 0;
        arg.mWidget = getDest(mCursor->getPos(), false);

        // make mouse position sane for this target window
        if(arg.mWidget)
        {
            Vector2f pos(arg.mWidget->mParent->getWindow()->affine(mCursor->getPos()));
            arg.mLocalX = pos.x;
            arg.mLocalY = pos.y;
        }
        //
        // Handling for multi-click generation
        //
        CursorMultiTouch & tkr = mCursorTouch[button];

        tkr.mTouchCount++;

        // if multi-click requirements are not met
        if(((mCursorMultiClickFactor > 0) &&
            (tkr.mTimer->getCostMS() - tkr.mCost > mCursorMultiClickFactor)) ||
            (!tkr.mArea.intersects(arg.mLocalX, arg.mLocalY)) ||
                (tkr.mWidget != arg.mWidget) || (tkr.mTouchCount > 3))
        {
            // reset to single down event.
            tkr.mTouchCount = 1;

            // build new allowable area for multi-clicks
            tkr.mArea.setPos(Vector2f(arg.mLocalX, arg.mLocalY));
            tkr.mArea.setSize(mCursorArea);
            tkr.mArea -= Vector2f(mCursorArea.mWidth / 2, mCursorArea.mHeight / 2);

            // set target window for click events on this tracker
            tkr.mWidget = arg.mWidget;
        }

        // set click count in the event args
        arg.mCount = tkr.mTouchCount;

        if(arg.mWidget)
        {
            if(mGenCursorMultiClick && arg.mWidget->isCursorMultiClick())
            {
                switch(tkr.mTouchCount)
                {
                case 1:
                    arg.mWidget->onButtonDown(&arg);
                    break;
                default:
                    arg.mWidget->onCursorMultiTouch(&arg);
                    break;
                }
            }
            else
            {
                arg.mWidget->onButtonDown(&arg);
            }
        }

        tkr.mCost = tkr.mTimer->getCostMS();
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onButtonUp(MouseButton button)
    {
        if(button > MB_Button4)
            return false;
        mWinComKey &= ~(1 << button);
        CursorEventArgs arg((Widget *)0);
        arg.mLocalX = mCursor->getPos().x;
        arg.mLocalY = mCursor->getPos().y;
        arg.mMouseArg.mX = 0;
        arg.mMouseArg.mY = 0;
        arg.mMouseArg.mButton = button;
        arg.mMouseArg.mScroll = 0;
        arg.mWidget = getDest(mCursor->getPos(), false);

        // make mouse position sane for this target window
        if(arg.mWidget)
        {
            Vector2f pos(arg.mWidget->mParent->getWindow()->affine(mCursor->getPos()));
            arg.mLocalX = pos.x;
            arg.mLocalY = pos.y;
        }
        // get the tracker that holds the number of down events seen so far for this button
        CursorMultiTouch & tkr = mCursorTouch[button];
        // set click count in the event args
        arg.mCount = tkr.mTouchCount;

        // if there is no window, inputs can not be handled.
        if(!arg.mWidget)
            return false;

        // store original window becase we re-use the event args.
        Widget * const tgt_wnd = arg.mWidget;

        // send 'up' input to the window
        arg.mWidget->onButtonUp(&arg);

        // restore target window (because Widget::on* may have propagated input)
        arg.mWidget = tgt_wnd;

        // send MouseClicked event if the requirements for that were met
        if(mGenCursorMultiClick && ((mCursorClickFactor == 0) ||
            (tkr.mTimer->getCostMS() - tkr.mCost <= mCursorClickFactor)) &&
            (tkr.mArea.intersects(arg.mLocalX, arg.mLocalY)) &&
                (tkr.mWidget == arg.mWidget))
        {
            arg.mWidget->onCursorTouch(&arg);
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onKeyDown(KeyValue key)
    {
        if(key == Key::LShift || key == Key::RShift)
            mWinComKey |= WCK_KT_A_Shift;
        else if (key == Key::LCtrl || key == Key::RCtrl)
            mWinComKey |= WCK_KT_A_Ctrl;
        else if (key == Key::LAlt || key == Key::RAlt)
            mWinComKey |= WCK_KT_A_Alt;

        Widget * f = getForce();

        if(!f)
            return false;

        KeyEventArgs args(f);
        args.mScanCode = key;

        f->onKeyDown(&args);
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onKeyUp(KeyValue key)
    {
        if (key == Key::LShift || key == Key::RShift)
            mWinComKey &= ~WCK_KT_A_Shift;
        else if (key == Key::LCtrl || key == Key::RCtrl)
            mWinComKey &= ~WCK_KT_A_Ctrl;
        else if (key == Key::LAlt || key == Key::RAlt)
            mWinComKey &= ~WCK_KT_A_Alt;

        Widget * f = getForce();

        if(!f)
            return false;

        KeyEventArgs args(f);
        args.mScanCode = key;

        f->onKeyUp(&args);
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onChar(Nui32 chr)
    {
        Widget * f = getForce();

        if(!f)
            return false;

        KeyEventArgs args(f);

        args.mCodePoint = chr;
        args.mWidget->onChar(&args);
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onCursorDrag(NIIi delta)
    {
        CursorEventArgs ma((Widget *)0);
        ma.mLocalX = mCursor->getPos().x;
        ma.mLocalY = mCursor->getPos().y;
        ma.mMouseArg.mX = 0;
        ma.mMouseArg.mY = 0;
        ma.mMouseArg.mButton = MB_Unknow;
        ma.mMouseArg.mScroll = delta;
        ma.mCount = 0;
        ma.mWidget = getDest(mCursor->getPos(), false);

        if(!ma.mWidget)
            return false;

        Vector2f pos(ma.mWidget->mParent->getWindow()->affine(mCursor->getPos()));
        ma.mLocalX = pos.x;
        ma.mLocalY = pos.y;

        ma.mWidget->onCursorDrag(&ma);
        return true;
    }
    //------------------------------------------------------------------------
    bool UISheet::onCursorMove(NIIi x, NIIi y)
    {
        CursorEventArgs arg(0);
        arg.mMouseArg.mX = (Nui32)(mCursorMoveFactor * x);
        arg.mMouseArg.mY = (Nui32)(mCursorMoveFactor * y);

        Vector2f pos = mCursor->getPos() + Vector2f(arg.mMouseArg.mX, arg.mMouseArg.mY);
        mCursor->setPos(pos);

        mValidOverlay = false;
        arg.mWidget = getCursorOver();

        if(arg.mWidget)
        {
            // make mouse position sane for this target window
            pos = arg.mWidget->mParent->getWindow()->affine(pos);
            arg.mLocalX = pos.x;
            arg.mLocalY = pos.y;
            arg.mMouseArg.mScroll = 0;
            arg.mCount = 0;
            arg.mMouseArg.mButton = MB_Unknow;
            arg.mWidget->onCursorMove(&arg);
        }
        return true;
    }
    //------------------------------------------------------------------------
    void UISheet::update(TimeDurMS cost)
    {
        WinList::iterator i, iend = mWinList.end();
        for(i = mWinList.begin(); i != iend; ++i)
        {
            if((*i)->isFinalVisible())
            {
                (*i)->update(cost);
            }
        }
        getCursorOver();
    }
    //------------------------------------------------------------------------
    bool UISheet::onCursorTouch(MouseButton button)
    {
        CursorEventArgs ma((Widget *)0);
        ma.mLocalX = mCursor->getPos().x;
        ma.mLocalY = mCursor->getPos().y;
        ma.mWidget = getDest(mCursor->getPos(), false);

        if(ma.mWidget)
        {
            ma.mMouseArg.mX = 0;
            ma.mMouseArg.mY = 0;
            ma.mMouseArg.mButton = button;
            ma.mMouseArg.mScroll = 0;
            // make mouse position sane for this target window
            Vector2f pos(ma.mWidget->mParent->getWindow()->affine(mCursor->getPos()));
            ma.mLocalX = pos.x;
            ma.mLocalY = pos.y;
            // tell the window about the event.
            ma.mWidget->onCursorTouch(&ma);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool UISheet::onCursorMultiTouch(NII_MEDIA::MouseButton button, NCount count)
    {
        CursorEventArgs arg((Widget *)0);
        arg.mLocalX = mCursor->getPos().x;
        arg.mLocalY = mCursor->getPos().y;
        arg.mWidget = getDest(mCursor->getPos(), false);

        if(arg.mWidget && arg.mWidget->isCursorMultiClick())
        {
            arg.mMouseArg.mX = 0;
            arg.mMouseArg.mY = 0;
            arg.mMouseArg.mButton = button;
            arg.mMouseArg.mScroll = 0;
            Vector2f pos(arg.mWidget->mParent->getWindow()->affine(mCursor->getPos()));
            arg.mLocalX = pos.x;
            arg.mLocalY = pos.y;
            arg.mCount = count;
            // tell the window about the event.
            arg.mWidget->onCursorMultiTouch(&arg);
            return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool UISheet::onObjCopy()
    {
        Widget * source = getForce();
        return source ? source->onCopyData(*N_Engine().getClipboard()) : false;
    }
    //------------------------------------------------------------------------
    bool UISheet::onObjCut()
    {
        Widget * source = getForce();
        return source ? source->onCutData(*N_Engine().getClipboard()) : false;
    }
    //------------------------------------------------------------------------
    bool UISheet::onObjPaste()
    {
        Widget * w = getForce();
        return w ? w->onPasteData(*N_Engine().getClipboard()) : false;
    }
    //------------------------------------------------------------------------
    void UISheet::flush()
    {
        if(mRedraw)
        {
            // 清理像素
            clear();
            WinList::iterator w, wend = mWinList.end();
            for(w = mWinList.begin(); w != wend; ++w)
            {
                UISubSheet * face = (*w)->getRender();
                if(face)
                {
                    face->clear();
                }
            }
            // 排列像素
            for(w = mWinList.begin(); w != wend; ++w)
            {
                (*w)->queuePixelImpl();
            }
            mRedraw = false;
        }
        // 冲刷像素
        GeometryLevelEventArgs arg(GL_Unknow);
        Queues::iterator i, iend = mRenderQueue.end();
        for(i = mRenderQueue.begin(); i != iend; ++i)
        {
            arg.mGL = i->first;

            signal(PreRenderEvent, &arg);
            i->second.flush();
            signal(EndRenderEvent, &arg);
        }
        mCursor->draw();
    }
    //------------------------------------------------------------------------
    void UISheet::FontChangeCB(Widget * w) const
    {
        FontEventArgs arg(w->getFont());

        if(!w->getFont(false))
        {
            w->onFont(&arg);
        }
        if (w->isContainer())
        {
            Container * tc = static_cast<Container *>(w);
            NCount i, count = tc->getCount();
            for (i = 0; i < count; ++i)
            {
                FontChangeCB(tc->get(i));
            }
        }
    }
    //------------------------------------------------------------------------
    Widget * UISheet::getDest(const Vector2f & pos, bool disable) const
    {
        Widget * dst = mActive;

        if(mActive)
        {
            if(mActive->isTransmitCursorEvent())
            {
                Widget * tc = mActive->getTarget(pos, disable);

                if(tc)
                    dst = tc;
            }
        }
        else
        {
            WinList::const_iterator i, iend = mWinList.end();
            for(i = mWinList.begin(); i != iend; ++i)
            {
                if((*i)->isFinalVisible())
                {
                    dst = (*i)->getTarget(pos, disable);
                }
            }
        }

        // modal target overrules
        if(mModal && dst != mModal)
        {
            if(!dst->isAncestor(mModal))
            {
                dst = mModal;
            }
        }
        return dst;
    }
    //------------------------------------------------------------------------
    Widget * UISheet::getForce() const
    {
        if(mFocus)
        {
            return mFocus;
        }

        if(mModal)
        {
            Widget * re = mModal->getActiveChild();
            return re ? re : mModal;
        }

        WinList::const_iterator i, iend = mWinList.end();
        for(i = mWinList.begin(); i != iend; ++i)
        {
            if((*i)->isFinalVisible())
            {
                return (*i)->getActiveChild();
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void UISheet::updateMatrix()
    {
        if(mValidProjectMatrix)
            return;
        NIIf w = mArea.width();
        NIIf h = mArea.height();
        NIIf aspect = w / h;
        NIIf midx = w * 0.5f;
        mViewExtent = midx / (aspect * 0.267949192431123f);

        NIIf nearZ = mViewExtent * 0.5f;
        NIIf farZ = mViewExtent * 2.0f;
        NIIf nr_sub_far = nearZ - farZ;

        Matrix4f tmp(Matrix4f::ZERO);
        tmp[0][0] = 3.732050808f / aspect;
        tmp[0][3] = -mViewExtent;
        tmp[1][1] = -3.732050808f;
        tmp[1][3] = mViewExtent;
        tmp[2][2] = -((farZ + nearZ) / nr_sub_far);
        tmp[3][2] = 1.0f;
        tmp[3][3] = mViewExtent;
        mSys->getSysProj(tmp, mProjectMatrix);

        mValidProjectMatrix = true;
    }
    //-----------------------------------------------------------------------
    void UISheet::updateViewport()
    {
        if(mValidView)
            return;
        if(!mView)
        {
            mView = N_new Viewport(N_UI_Viewport, 0, mTarget, 0, 0, 1, 1, 0);
            if(mTarget)
            {
                mView->setArea(
                    mArea.left() / mTarget->getWidth(),
                    mArea.top() / mTarget->getHeight(),
                    mArea.width() / mTarget->getWidth(),
                    mArea.height() / mTarget->getHeight());
            }
        }

        mView->sync();
        mValidView = true;
    }
    //------------------------------------------------------------------------
    void UISheet::DestroyCB(const Widget * w)
    {
        if(w == getCursorOver())
        {
            mOverlay = 0;
            mValidOverlay = true;
        }
        if(w == mModal)
        {
            mModal = 0;
        }
        if(w == mFocus)
        {
            mFocus = 0;
        }
        if(w == mActive)
        {
            mActive = 0;
        }
        if(w == mTooltip)
        {
            mTooltip = 0;
            mAutoTooltip = false;
        }
    }
    //------------------------------------------------------------------------
    void UISheet::onCursorMoveFactorChange(const SheetEventArgs * arg)
    {
        signal(CursorMoveFactorChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void UISheet::onCursorClickFactorChange(const SheetEventArgs * arg)
    {
        signal(CursorClickFactorChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void UISheet::onCursorMultiClickFactorChange(const SheetEventArgs * arg)
    {
        signal(CursorMultiClickFactorChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void UISheet::onCursorAreaChange(const SheetEventArgs * arg)
    {
        signal(CursorAreaChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void UISheet::onWindowChange(const RenderSheetEventArgs * arg)
    {
        signal(WindowChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void UISheet::onFont(const FontEventArgs * arg)
    {
        WinList::iterator i, iend = mWinList.end();
        for(i = mWinList.begin(); i != iend; ++i)
        {
            FontChangeCB(*i);
        }

        signal(FontEvent, arg);
    }
    //------------------------------------------------------------------------
}
}