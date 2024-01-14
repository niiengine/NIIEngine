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
#include "NiiOverlay.h"
#include "NiiEngine.h"
#include "NiiSpaceManager.h"
#include "NiiOverlayManager.h"
#include "NiiViewport.h"
#include "NiiEulerAngle.h"
#include "NiiMaterialManager.h"
#include "NiiException.h"
#include "NiiRenderQueue.h"

namespace NII
{
    //---------------------------------------------------------------------
    Overlay::Overlay(OverlayID id, Viewport * vp) :
        mID(id),
        mRotate(0.0f),
        mScrollX(0.0f),
        mScrollY(0.0f),
        mScaleX(1.0f),
        mScaleY(1.0f),
        mZOrder(100),
        mViewport(vp),
        mSpaceValid(false),
        mVisible(false),
        mInit(false)
    {
        mRootNode = N_new SpaceNode(NULL);
    }
    //---------------------------------------------------------------------
    Overlay::~Overlay()
    {
        N_delete mRootNode;
        ViewList::iterator i, iend = mViewList.end();
        for (i = mViewList.begin(); i != iend; ++i)
        {
            (*i)->_notifyParent(0, 0);
        }
    }
    //---------------------------------------------------------------------
    void Overlay::setZOrder(Nidx16 level)
    {
        // Limit to 650 since this is multiplied by 100 to pad out for containers
        N_assert(level <= 650, "Overlay ZOrder cannot be greater than 650!");

        mZOrder = level;

        Nidx16 zorder = mZOrder * 100;
        ViewList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            zorder = (*i)->_notifyZOrder(zorder);
        }
    }
    //---------------------------------------------------------------------
    void Overlay::show()
    {
        mVisible = true;
        if(!mInit)
        {
            init();
        }
    }
    //---------------------------------------------------------------------
    void Overlay::init()
    {
        ViewList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != mViewList.end(); ++i)
        {
            (*i)->init();
        }
        mInit = true;
    }
    //---------------------------------------------------------------------
    void Overlay::add(OverlayView * obj)
    {
        mViewList.push_back(obj);
        obj->_notifyParent(0, this);

        Nidx zorder = mZOrder * 100;
        ViewList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            zorder = (*i)->_notifyZOrder(zorder);
        }
        obj->_notifyViewport();
    }
    //---------------------------------------------------------------------
    void Overlay::remove(OverlayView * obj)
    {
        mViewList.remove(obj);
        obj->_notifyParent(0, 0);
        Nidx zorder = mZOrder * 100;
        ViewList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            zorder = (*i)->_notifyZOrder(zorder);
        }
    }
    //---------------------------------------------------------------------
    void Overlay::clearAll()
    {
        mRootNode->removeAll();
        mViewList.clear();
        // Note no deallocation, memory handled by OverlayManager & SpaceManager
    }
    //---------------------------------------------------------------------
    void Overlay::setOffset(NIIf x, NIIf y)
    {
        mScrollX = x;
        mScrollY = y;
        mSpaceValid = false;
    }
    //---------------------------------------------------------------------
    OverlayView * Overlay::get(OverlayGeoID id)
    {
        ViewList::iterator i, iend = mViewList.end();
        for(i = mViewList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == id)
            {
                return *i;

            }
        }
        return NULL;
    }
    //---------------------------------------------------------------------
    void Overlay::offset(NIIf xoff, NIIf yoff)
    {
        mScrollX += xoff;
        mScrollY += yoff;
        mSpaceValid = false;
    }
    //---------------------------------------------------------------------
    void Overlay::setRotate(const Radian & angle)
    {
        mRotate = angle;
        mSpaceValid = false;
    }
    //---------------------------------------------------------------------
    void Overlay::setScale(NIIf x, NIIf y)
    {
        mScaleX = x;
        mScaleY = y;
        mSpaceValid = false;
    }
    //---------------------------------------------------------------------
    void Overlay::getSpaceMatrix(Matrix4f * xform) const
    {
        if(!mSpaceValid)
        {
            // Ordering:
            //    1. Scale
            //    2. Rotate
            //    3. Translate

            Radian ori = Radian(0.0f);

#if NII_VIEWPORT_DIR_NONE == 0
            ori = Radian(mViewport->getDirection() * Math::PI_2);
#endif

            Matrix3f rot3x3, scale3x3;
            eulerXYZ(&rot3x3, Radian(0), Radian(0), mRotate + ori);
            scale3x3 = Matrix3f::ZERO;
            scale3x3[0][0] = mScaleX;
            scale3x3[1][1] = mScaleY;
            scale3x3[2][2] = 1.0f;

            Overlay * temp = const_cast<Overlay *>(this);
            temp->mSpace = Matrix4f::IDENTITY;
            temp->mSpace = rot3x3 * scale3x3;
            temp->mSpace.setTrans(Vector3f(mScrollX, mScrollY, 0));

            temp->mSpaceValid = true;
        }
        *xform = mSpace;
    }
    //---------------------------------------------------------------------
    void Overlay::renderImpl(Camera * cam, RenderQueue * queue)
    {
        ViewList::iterator i, iend = mViewList.end();
        if(!mViewport->isSyncAspect())
        {
            for(i = mViewList.begin(); i != iend; ++i)
            {
                (*i)->_notifyViewport();
            }
        }

        if(mVisible)
        {
            // Add 3D elements
            mRootNode->setPos(cam->getSpacePos());
            mRootNode->setOri(cam->getSpaceOri());
            mRootNode->update(false, true);
            // Set up the default queue group for the objects about to be added
            Nui8 oldgrp = queue->getDefaultGroup();
            Nui16 oldPriority = queue->getDefaultLevel();
            queue->setDefaultGroup(RQG_Overlay);
            queue->setDefaultLevel(static_cast<Nui16>((mZOrder * 100) - 1));
            cam->setExpandMode(false);
            mRootNode->queue(queue, cam, true, false);
            cam->setExpandMode(true);
            // Reset the group
            queue->setDefaultGroup(oldgrp);
            queue->setDefaultLevel(oldPriority);

            for(i = mViewList.begin(); i != iend; ++i)
            {
                (*i)->_update();

                (*i)->queue(queue);
            }
        }
    }
    //---------------------------------------------------------------------
    OverlayGeo * Overlay::getGeo(NIIf x, NIIf y)
    {
        OverlayGeo * ret = NULL;
        NIIi currZ = -1;
        ViewList::iterator i, iend = mViewList.end();
        for (i = mViewList.begin(); i != iend; ++i)
        {
            NIIi z = (*i)->getZOrder();
            if(z > currZ)
            {
                OverlayGeo * found = (*i)->getGeo(x,y);
                if(found)
                {
                    currZ = found->getZOrder();
                    ret = found;
                }
            }
        }
        return ret;
    }
    //---------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdLeft : public PropertyCmd
    {
    public:
        CmdLeft() : PropertyCmd(N_PropertyOverlayGeo_Left, _T("left"),
            _T("The position of the left border of the gui element."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const OverlayGeo *>(own)->getLeft(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf r;
            StrConv::conv(val, r);
            static_cast<OverlayGeo *>(own)->setLeft(r);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getLeft();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<OverlayGeo *>(own)->setLeft(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdLeft();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTop : public PropertyCmd
    {
    public:
        CmdTop() : PropertyCmd(N_PropertyOverlayGeo_Top, _T("top"),
            _T("The position of the top border of the gui element."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const OverlayGeo *>(own)->getTop(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf r;
            StrConv::conv(val, r);
            static_cast<OverlayGeo *>(own)->setTop(r);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getTop();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<OverlayGeo *>(own)->setTop(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTop();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdWidth : public PropertyCmd
    {
    public:
        CmdWidth() : PropertyCmd(N_PropertyOverlayGeo_Width, _T("width"), _T("The width of the element."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const OverlayGeo *>(own)->getWidth(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf r;
            StrConv::conv(val, r);
            static_cast<OverlayGeo *>(own)->setWidth(r);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getWidth();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<OverlayGeo *>(own)->setWidth(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdWidth();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(1.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdHeight : public PropertyCmd
    {
    public:
        CmdHeight() : PropertyCmd(N_PropertyOverlayGeo_Height, _T("height"), _T("The height of the element."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const OverlayGeo *>(own)->getHeight(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf r;
            StrConv::conv(val, r);
            static_cast<OverlayGeo *>(own)->setHeight(r);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getHeight();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<OverlayGeo *>(own)->setHeight(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHeight();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(1.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdMaterial : public PropertyCmd
    {
    public:
        CmdMaterial() : PropertyCmd(N_PropertyOverlayGeo_Material, _T("material"), _T("The name of the material to use."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const OverlayGeo*>(own)->getMaterialID(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 id;
            StrConv::conv(val, id);
            static_cast<OverlayGeo *>(own)->setMaterial(id);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const OverlayGeo*>(own)->getMaterialID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<OverlayGeo *>(own)->setMaterial(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaterial();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdCaption : public PropertyCmd
    {
    public:
        CmdCaption() : PropertyCmd(N_PropertyOverlayGeo_Caption, _T("caption"), _T("The element caption, if supported."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const OverlayGeo *>(own)->getCaption();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<OverlayGeo *>(own)->setCaption(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getCaption();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            String * temp = (String *)in;
            static_cast<OverlayGeo *>(own)->setCaption(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCaption();
        }

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdMetricsMode : public PropertyCmd
    {
    public:
        CmdMetricsMode() : PropertyCmd(N_PropertyOverlayGeo_MetricsMode, _T("metrics_mode"),
            _T("The type of metrics to use, either 'relative' to the screen, 'pixels' or 'relative_aspect_adjusted'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            SizeMode gmm = static_cast<const OverlayGeo *>(own)->getSizeMode();

            switch(gmm)
            {
            case SM_Abs :
                out = _T("pixels");
                break;
            case SM_Rel :
                out = _T("relative_aspect_adjusted");
                break;
            default :
                out = _T("relative");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            OverlayGeo * obj = static_cast<OverlayGeo *>(own);
            if(val == _T("pixels"))
            {
                obj->setSizeMode(SM_Abs);
            }
            else if(val == _T("relative_aspect_adjusted"))
            {
                obj->setSizeMode(SM_Rel);
            }
            else
            {
                obj->setSizeMode(SM_Unit);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            SizeMode * temp = (SizeMode *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getSizeMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            SizeMode * temp = (SizeMode *)in;
            static_cast<OverlayGeo *>(own)->setSizeMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMetricsMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("relative");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdHorizontalAlign : public PropertyCmd
    {
    public:
        CmdHorizontalAlign() : PropertyCmd(N_PropertyOverlayGeo_HorizontalAlign, _T("horz_align"),
            _T("The horizontal alignment, 'left', 'right' or 'center'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            HAlign gha = static_cast<const OverlayGeo *>(own)->getXAlign();
            switch(gha)
            {
            case HA_LEFT:
                out = _T("left");
                break;
            case HA_RIGHT:
                out = _T("right");
                break;
            case HA_CENTRE:
                out = _T("center");
                break;
            default :
                out = _T("center");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & in)
        {
            if(in == _T("left"))
            {
                static_cast<OverlayGeo *>(own)->setXAlign(HA_LEFT);
            }
            else if(in == _T("right"))
            {
                static_cast<OverlayGeo *>(own)->setXAlign(HA_RIGHT);
            }
            else
            {
                static_cast<OverlayGeo *>(own)->setXAlign(HA_CENTRE);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            HAlign * temp = (HAlign *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getXAlign();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            HAlign * temp = (HAlign *)in;
            static_cast<OverlayGeo *>(own)->setXAlign(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHorizontalAlign();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("left");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdVerticalAlign : public PropertyCmd
    {
    public:
        CmdVerticalAlign() : PropertyCmd(N_PropertyOverlayGeo_VerticalAlign, _T("vert_align"),
            _T("The vertical alignment, 'top', 'bottom' or 'center'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            VAlign gva = static_cast<const OverlayGeo *>(own)->getYAlign();
            switch(gva)
            {
            case VA_TOP:
                out = _T("top");
                break;
            case VA_BOTTOM:
                out = _T("bottom");
                break;
            case VA_CENTRE:
                out = _T("center");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            OverlayGeo * obj = static_cast<OverlayGeo *>(own);
            if(val == _T("top"))
            {
                obj->setYAlign(VA_TOP);
            }
            else if(val == _T("bottom"))
            {
                obj->setYAlign(VA_BOTTOM);
            }
            else
            {
                obj->setYAlign(VA_CENTRE);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            VAlign * temp = (VAlign *)out;
            *temp = static_cast<const OverlayGeo *>(own)->getYAlign();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            VAlign * temp = (VAlign *)in;
            static_cast<OverlayGeo *>(own)->setYAlign(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVerticalAlign();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("top");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdVisible : public PropertyCmd
    {
    public:
        CmdVisible() : PropertyCmd(N_PropertyOverlayGeo_Visible, _T("visible"),
            _T("Initial visibility of element, either 'true' or 'false' (default true)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            bool visible = static_cast<const OverlayGeo *>(own)->isVisible();
            switch(visible)
            {
            case true:
                out = _T("true");
                break;
            case false:
                out = _T("false");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            if(val == _T("true"))
            {
                static_cast<OverlayGeo *>(own)->show();
            }
            else if (val == _T("false"))
            {
                static_cast<OverlayGeo *>(own)->hide();
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const OverlayGeo *>(own)->isVisible();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool *temp = (bool *)in;
            if(*temp == true)
            {
                static_cast<OverlayGeo *>(own)->show();
            }
            else
            {
                static_cast<OverlayGeo *>(own)->hide();
            }
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdVisible();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("true");
        }
    };
    //---------------------------------------------------------------------
    OverlayGeo::OverlayGeo(OverlayGeoID id, ScriptTypeID stid, LangID lid):
        PropertyCmdObj(stid, lid),
        mID(id),
        mLeft(0.0f),
        mTop(0.0f),
        mWidth(1.0f),
        mHeight(1.0f),
        mMetricsMode(SM_Unit),
        mHorzAlign(HA_LEFT),
        mVertAlign(VA_TOP),
        mPixelTop(0.0),
        mPixelLeft(0.0),
        mPixelWidth(1.0),
        mPixelHeight(1.0),
        mPixelScaleX(1.0),
        mPixelScaleY(1.0),
        mParent(0),
        mViewport(0),
        mOverlay(0),
        mZOrder(0),
        mMaterial(0),
        mSrcTemplate(0),
        mInit(false),
        mEnabled(true),
        mVisible(true),
        mTemplate(true),
        mNeedUpdate(true),
        mGeoPosDirty(true),
        mGeoTexDirty(true)
    {
        mGeometryMark |= GeometryObj::M_IdentityProj;
        mGeometryMark |= GeometryObj::M_IdentityView;
    }
    //---------------------------------------------------------------------
    OverlayGeo::~OverlayGeo()
    {
        if(mParent)
        {
            mParent->remove(mID);
            mParent = 0;
        }
        if(mMaterial)
        {
            mMaterial->unref();
            mMaterial = 0;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::show()
    {
        mVisible = true;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::hide()
    {
        mVisible = false;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setEnable(bool b)
    {
        mEnabled = b;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setSize(NIIf width, NIIf height)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelWidth = width;
            mPixelHeight = height;
        }
        else
        {
            mWidth = width;
            mHeight = height;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setPos(NIIf left, NIIf top)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelLeft = left;
            mPixelTop = top;
        }
        else
        {
            mLeft = left;
            mTop = top;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setWidth(NIIf width)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelWidth = width;
        }
        else
        {
            mWidth = width;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getWidth() const
    {
        if(mMetricsMode != SM_Unit)
        {
            return mPixelWidth;
        }
        else
        {
            return mWidth;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setHeight(NIIf height)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelHeight = height;
        }
        else
        {
            mHeight = height;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getHeight() const
    {
        if(mMetricsMode != SM_Unit)
        {
            return mPixelHeight;
        }
        else
        {
            return mHeight;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setLeft(NIIf left)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelLeft = left;
        }
        else
        {
            mLeft = left;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getLeft() const
    {
        if(mMetricsMode != SM_Unit)
        {
            return mPixelLeft;
        }
        else
        {
            return mLeft;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setTop(NIIf top)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelTop = top;
        }
        else
        {
            mTop = top;
        }
        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getTop() const
    {
        if(mMetricsMode != SM_Unit)
        {
            return mPixelTop;
        }
        else
        {
            return mTop;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitLeft(NIIf left)
    {
        mLeft = left;
        mPixelLeft = left / mPixelScaleX;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitTop(NIIf top)
    {
        mTop = top;
        mPixelTop = top / mPixelScaleY;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitWidth(NIIf width)
    {
        mWidth = width;
        mPixelWidth = width / mPixelScaleX;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitHeight(NIIf height)
    {
        mHeight = height;
        mPixelHeight = height / mPixelScaleY;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitPos(NIIf left, NIIf top)
    {
        mLeft = left;
        mTop  = top;
        mPixelLeft = left / mPixelScaleX;
        mPixelTop  = top / mPixelScaleY;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setUnitSize(NIIf width, NIIf height)
    {
        mWidth  = width;
        mHeight = height;
        mPixelWidth  = width / mPixelScaleX;
        mPixelHeight = height / mPixelScaleY;

        mNeedUpdate = true;
        _notifyPos();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::setMaterial(ResourceID rid)
    {
        if(mMaterialID == rid)
        {
            return;
        }
        else
        {
            mMaterialID = rid;
            if(rid != 0)
            {
                if(mMaterial)
                    mMaterial->unref();

                mMaterial = static_cast<Material *>(N_Only(Material).get(rid));
                if(mMaterial == 0)
                {
                    N_EXCEPT(NotExist, _I18n("Could not find material ") + rid);
                }
                mMaterial->setLight(false);
                N_Only(Material).setDepthCheck(mMaterial, false);

                mMaterial->ref(true);
            }
            else
            {
                if(mMaterial)
                {
                    mMaterial->unref();
                    mMaterial = 0;
                }
            }
        }
    }
    //---------------------------------------------------------------------
    const Material * OverlayGeo::getMaterial() const
    {
        return mMaterial;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::getMatrix(Matrix4f * out, NCount & count) const
    {
        mOverlay->getSpaceMatrix(out);
        count = 1;
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::distanceSquared(const Camera * cam) const
    {
        (void)cam;
        return 10000.0f - (NIIf)getZOrder();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::getLight(LightList & ll) const
    {
        ll.clear();
    }
    //---------------------------------------------------------------------
    void OverlayGeo::_notifyPos(void)
    {
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::_update()
    {
        // Check size if pixel-based
        switch(mMetricsMode)
        {
        case SM_Abs :
            if(!mViewport->isSyncAspect() || mGeoPosDirty)
            {
                NIIf vpWidth, vpHeight;
                vpWidth = (NIIf)(mViewport->getViewWidth());
                vpHeight = (NIIf)(mViewport->getViewHeight());

                mPixelScaleX = 1.0f / vpWidth;
                mPixelScaleY = 1.0f / vpHeight;

                mLeft = mPixelLeft * mPixelScaleX;
                mTop = mPixelTop * mPixelScaleY;
                mWidth = mPixelWidth * mPixelScaleX;
                mHeight = mPixelHeight * mPixelScaleY;
            }
            break;
        case SM_Rel :
            if(!mViewport->isSyncAspect() || mGeoPosDirty)
            {
                NIIf vpWidth, vpHeight;

                vpWidth = (NIIf)(mViewport->getViewWidth());
                vpHeight = (NIIf) (mViewport->getViewHeight());

                mPixelScaleX = 1.0f / (10000.0f * (vpWidth / vpHeight));
                mPixelScaleY = 1.0f /  10000.0f;

                mLeft = mPixelLeft * mPixelScaleX;
                mTop = mPixelTop * mPixelScaleY;
                mWidth = mPixelWidth * mPixelScaleX;
                mHeight = mPixelHeight * mPixelScaleY;
            }
            break;
        default:
            break;
        }

        updateImpl();
        // NB container subclasses will update children too

        // Tell self to update own position geometry
        if(mGeoPosDirty && mInit)
        {
            updateGeoPos();
            mGeoPosDirty = false;
        }
        // Tell self to update own texture geometry
        if(mGeoTexDirty && mInit)
        {
            updateGeoTex();
            mGeoTexDirty = false;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::updateImpl()
    {
        NIIf parentLeft = 0, parentTop = 0, parentBottom = 0, parentRight = 0;

        if(mParent)
        {
            parentLeft = mParent->getEnvLeft();
            parentTop = mParent->getEnvTop();
            if(mHorzAlign == HA_CENTRE || mHorzAlign == HA_RIGHT)
            {
                parentRight = parentLeft + mParent->getEnvWidth();
            }
            if(mVertAlign == VA_CENTRE || mVertAlign == VA_BOTTOM)
            {
                parentBottom = parentTop + mParent->getEnvHeight();
            }
        }
        else
        {
            RenderSys * rs = N_Engine().getRender();
            OverlayManager & oMgr = N_Only(Overlay);

            // Calculate offsets required for mapping texel origins to pixel origins in the
            // current rendersystem
            NIIf hOffset = rs->getTexelXOffset() / mViewport->getViewWidth();
            NIIf vOffset = rs->getTexelYOffset() / mViewport->getViewHeight();

            parentLeft = 0.0f + hOffset;
            parentTop = 0.0f + vOffset;
            parentRight = 1.0f + hOffset;
            parentBottom = 1.0f + vOffset;
        }

        // Sort out position based on alignment
        // NB all we do is derived the origin, we don't automatically sort out the position
        // This is more flexible than forcing absolute right & middle
        switch(mHorzAlign)
        {
        case HA_CENTRE:
            mEnvLeft = ((parentLeft + parentRight) * 0.5f) + mLeft;
            break;
        case HA_LEFT:
            mEnvLeft = parentLeft + mLeft;
            break;
        case HA_RIGHT:
            mEnvLeft = parentRight + mLeft;
            break;
        };

        switch(mVertAlign)
        {
        case VA_CENTRE:
            mEnvTop = ((parentTop + parentBottom) * 0.5f) + mTop;
            break;
        case VA_TOP:
            mEnvTop = parentTop + mTop;
            break;
        case VA_BOTTOM:
            mEnvTop = parentBottom + mTop;
            break;
        };

        mNeedUpdate = false;

        if(mParent != 0)
        {
            Rectf parent;
            Rectf child;

            mParent->getClipArea(parent);

            child.mLeft = mEnvLeft;
            child.mTop  = mEnvTop;
            child.mRight  = mEnvLeft + mWidth;
            child.mBottom = mEnvTop + mHeight;

            mClipArea = parent.intersects(child);
        }
        else
        {
            mClipArea.mLeft = mEnvLeft;
            mClipArea.mTop  = mEnvTop;
            mClipArea.mRight  = mEnvLeft + mWidth;
            mClipArea.mBottom = mEnvTop + mHeight;
        }
    }
    //---------------------------------------------------------------------
    void OverlayGeo::_notifyParent(OverlayView * parent, Overlay * overlay)
    {
        mParent = parent;
        mOverlay = overlay;

        if(mOverlay)
        {
            if(mOverlay->isInit() && !mInit)
            {
                init();
            }
            mViewport = mOverlay->getViewport();
        }

        mNeedUpdate = true;
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getEnvLeft()
    {
        if(mNeedUpdate)
        {
            updateImpl();
        }
        return mEnvLeft;
    }
    //---------------------------------------------------------------------
    NIIf OverlayGeo::getEnvTop()
    {
        if(mNeedUpdate)
        {
            updateImpl();
        }
        return mEnvTop;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::getClipArea(Rectf & rect)
    {
        if(mNeedUpdate)
        {
            updateImpl();
        }
        rect = mClipArea;
    }
    //---------------------------------------------------------------------
    Nidx16 OverlayGeo::_notifyZOrder(Nidx16 newZOrder)
    {
        mZOrder = newZOrder;
        return mZOrder + 1;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::_notifyViewport()
    {
        switch(mMetricsMode)
        {
        case SM_Abs :
            {
                NIIf vpWidth, vpHeight;
                vpWidth = (NIIf)(mViewport->getViewWidth());
                vpHeight = (NIIf)(mViewport->getViewHeight());

                mPixelScaleX = 1.0f / vpWidth;
                mPixelScaleY = 1.0f / vpHeight;
            }
            break;
        case SM_Rel :
            {
                NIIf vpWidth, vpHeight;
                vpWidth = (NIIf)(mViewport->getViewWidth());
                vpHeight = (NIIf)(mViewport->getViewHeight());

                mPixelScaleX = 1.0f / (10000.0f * (vpWidth / vpHeight));
                mPixelScaleY = 1.0f /  10000.0f;
            }
            break;
        case SM_Unit :
            mPixelScaleX = 1.0;
            mPixelScaleY = 1.0;
            mPixelLeft = mLeft;
            mPixelTop = mTop;
            mPixelWidth = mWidth;
            mPixelHeight = mHeight;
            break;
        }

        mLeft = mPixelLeft * mPixelScaleX;
        mTop = mPixelTop * mPixelScaleY;
        mWidth = mPixelWidth * mPixelScaleX;
        mHeight = mPixelHeight * mPixelScaleY;

        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void OverlayGeo::queue(RenderQueue * queue)
    {
        if(mVisible)
        {
            queue->add(this, RQG_Overlay, mZOrder);
        }
    }
    //-----------------------------------------------------------------------
    bool OverlayGeo::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdLeft());
        dest->add(N_new CmdTop());
        dest->add(N_new CmdWidth());
        dest->add(N_new CmdHeight());
        dest->add(N_new CmdMaterial());
        dest->add(N_new CmdCaption());
        dest->add(N_new CmdMetricsMode());
        dest->add(N_new CmdHorizontalAlign());
        dest->add(N_new CmdVerticalAlign());
        dest->add(N_new CmdVisible());
        return true;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setCaption(const ViewString & caption)
    {
        mCaption = caption;
        _notifyPos();
    }
    //-----------------------------------------------------------------------
    const ViewString & OverlayGeo::getCaption() const
    {
        return mCaption;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setColour(const Colour & col)
    {
        mColour = col;
    }
    //-----------------------------------------------------------------------
    const Colour & OverlayGeo::getColour() const
    {
        return mColour;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setSizeMode(SizeMode gmm)
    {
        switch(gmm)
        {
        case SM_Abs :
        {
            NIIf vpWidth, vpHeight;
            vpWidth = (NIIf)(mViewport->getViewWidth());
            vpHeight = (NIIf)(mViewport->getViewHeight());

            // cope with temporarily zero dimensions, avoid divide by zero
            vpWidth = vpWidth == 0.0f ? 1.0f : vpWidth;
            vpHeight = vpHeight == 0.0f ? 1.0f : vpHeight;

            mPixelScaleX = 1.0f / vpWidth;
            mPixelScaleY = 1.0f / vpHeight;

            if (mMetricsMode == SM_Unit)
            {
                mPixelLeft = mLeft;
                mPixelTop = mTop;
                mPixelWidth = mWidth;
                mPixelHeight = mHeight;
            }
        }
        break;
        case SM_Rel :
        {
            NIIf vpWidth, vpHeight;
            vpWidth = (NIIf)(mViewport->getViewWidth());
            vpHeight = (NIIf)(mViewport->getViewHeight());

            mPixelScaleX = 1.0f / (10000.0f * (vpWidth / vpHeight));
            mPixelScaleY = 1.0f / 10000.0f;

            if (mMetricsMode == SM_Unit)
            {
                mPixelLeft = mLeft;
                mPixelTop = mTop;
                mPixelWidth = mWidth;
                mPixelHeight = mHeight;
            }
        }
        break;
        case SM_Unit :
        {
            mPixelScaleX = 1.0;
            mPixelScaleY = 1.0;
            mPixelLeft = mLeft;
            mPixelTop = mTop;
            mPixelWidth = mWidth;
            mPixelHeight = mHeight;
        }
        break;
        }

        mLeft = mPixelLeft * mPixelScaleX;
        mTop = mPixelTop * mPixelScaleY;
        mWidth = mPixelWidth * mPixelScaleX;
        mHeight = mPixelHeight * mPixelScaleY;

        mMetricsMode = gmm;
        mNeedUpdate = true;
        _notifyPos();
    }
    //-----------------------------------------------------------------------
    SizeMode OverlayGeo::getSizeMode() const
    {
        return mMetricsMode;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setXAlign(HAlign align)
    {
        mHorzAlign = align;
        _notifyPos();
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setYAlign(VAlign align)
    {
        mVertAlign = align;
        _notifyPos();
    }
    //-----------------------------------------------------------------------
    OverlayGeo * OverlayGeo::getGeo(NIIf x, NIIf y) const
    {
        if(mClipArea.intersects(x, y))
        {
            return const_cast<OverlayGeo *>(this);
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    NIIi OverlayGeo::getScreenWidth() const
    {
        return mViewport->getViewWidth();
    }
    //-----------------------------------------------------------------------
    NIIi OverlayGeo::getScreenHeight() const
    {
        return mViewport->getViewHeight();
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setParent(OverlayView * parent)
    {
        mParent = parent;
    }
    //-----------------------------------------------------------------------
    void OverlayGeo::setTemplate(OverlayGeo * obj)
    {
        this->copyProperty(obj);
        mSrcTemplate = obj ;
        return;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * OverlayGeo::clone(OverlayGeoID newid)
    {
        OverlayGeo * c = N_Only(Overlay).createGeo(getTypeID(), newid);
        c->copyProperty(this);

        return c;
    }
    //-----------------------------------------------------------------------
    //---------------------------------------------------------------------
    // OverlayView
    //---------------------------------------------------------------------    
    //---------------------------------------------------------------------
    OverlayView::OverlayView(OverlayGeoID id, ScriptTypeID stid, LangID lid) :
        OverlayGeo(id, stid, lid),
        mChildPosCheck(true)
    {
    }
    //---------------------------------------------------------------------
    OverlayView::~OverlayView()
    {
        if(mOverlay && !mParent)
        {
            mOverlay->remove(this);
            mViewport = 0;
        }

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            OverlayGeo * child = i->second;
            child->_notifyParent(0, 0);
        }
    }
    //---------------------------------------------------------------------
    void OverlayView::add(OverlayGeo * c)
    {
        OverlayGeoID name = c->getID();
        ChildMap::iterator i = mChildren.find(name);
        if(i != mChildren.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("子元素: ") + N_conv(name) + _I18n(" 已经存在"));
        }

        mChildren.insert(ChildMap::value_type(name, c));
        // tell child about parent & ZOrder
        c->_notifyParent(this, mOverlay);
        c->_notifyZOrder(mZOrder + 1);
        c->_notifyViewport();
    }
    //---------------------------------------------------------------------
    void OverlayView::remove(OverlayGeoID id)
    {
        ChildMap::iterator i = mChildren.find(id);
        if(i != mChildren.end())
        {        
            OverlayGeo * element = i->second;
            mChildren.erase(i);
            element->setParent(0);
        }
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayView::get(OverlayGeoID id)
    {
        ChildMap::iterator i = mChildren.find(id);
        if(i == mChildren.end())
        {
            N_EXCEPT(NotExist, _I18n("子元素: ") + N_conv(id) + _I18n(" 不存在"));
        }
        return i->second;
    }
    //---------------------------------------------------------------------
    void OverlayView::init()
    {
        ChildMap::iterator ci, ciend = mChildren.end();
        for(ci =  mChildren.begin(); ci != ciend; ++ci)
        {
            ci->second->init();
        }
    }
    //---------------------------------------------------------------------
    void OverlayView::_notifyPos()
    {
        OverlayGeo::_notifyPos();

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            i->second->_notifyPos();
        }
    }
    //---------------------------------------------------------------------
    void OverlayView::_update()
    {
        OverlayGeo::_update();

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            i->second->_update();
        }
    }
    //---------------------------------------------------------------------
    Nidx16 OverlayView::_notifyZOrder(Nidx16 level)
    {
        OverlayGeo::_notifyZOrder(level);
        level++;

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            level = i->second->_notifyZOrder(level);
        }

        return level;
    }
    //---------------------------------------------------------------------
    void OverlayView::_notifyViewport()
    {
        OverlayGeo::_notifyViewport();

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            i->second->_notifyViewport();
        }
    }
    //---------------------------------------------------------------------
    void OverlayView::_notifyParent(OverlayView * parent, Overlay * overlay)
    {
        OverlayGeo::_notifyParent(parent, overlay);

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            i->second->_notifyParent(this, overlay);
        }
    }
    //---------------------------------------------------------------------
    void OverlayView::queue(RenderQueue * queue)
    {
        if(mVisible)
        {
            OverlayGeo::queue(queue);

            ChildMap::iterator i, iend = mChildren.end();
            for(i = mChildren.begin(); i != iend; ++i)
            {
                // Give children ZOrder 1 higher than this
                i->second->queue(queue);
            }
        }
    }
    //---------------------------------------------------------------------
    OverlayGeo * OverlayView::getGeo(NIIf x, NIIf y) const
    {
        OverlayGeo * ret = NULL;
        NIIi currZ = -1;

        if(mVisible)
        {
            ret = OverlayGeo::getGeo(x, y);
            if(ret && mChildPosCheck)
            {
                ChildMap::const_iterator i, iend = mChildren.end();
                for(i = mChildren.begin(); i != iend; ++i)
                {
                    OverlayGeo * r = i->second;
                    if(r->isVisible() && r->isEnabled())
                    {
                        NIIi z = r->getZOrder();
                        if(z > currZ)
                        {
                            OverlayGeo * c = r->getGeo(x, y);
                            if(c)
                            {
                                currZ = z;
                                ret = c;
                            }
                        }
                    }
                }
            }
        }
        return ret;
    }
    //------------------------------------------------------------------------
    void OverlayView::setTemplate(OverlayGeo * obj)
    {
        OverlayGeo::setTemplate(obj);
        OverlayView * o = dynamic_cast<OverlayView *>(obj);
        if(o)
        {
            ChildMap::iterator i, iend = o->mChildren.end();
            for(i = o->mChildren.begin(); i != iend; ++i)
            {
                OverlayGeo * oldChild = i->second;
                if(oldChild->isTemplate())
                {
                    OverlayGeo * newChild = N_Only(Overlay).createGeo(oldChild->getTypeID());
                    newChild->setTemplate(oldChild);
                    add(newChild);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    OverlayGeo * OverlayView::clone(OverlayGeoID newid)
    {
        OverlayView * newview = static_cast<OverlayView *>(OverlayGeo::clone(newid));

        ChildMap::iterator i, iend = mChildren.end();
        for(i = mChildren.begin(); i != iend; ++i)
        {
            OverlayGeo * thisChild = i->second;
            if(thisChild->isTemplate())
            {
                OverlayGeo * newChild = thisChild->clone(0);
                newview->add(newChild);
            }
        }

        return newview;
    }
    //------------------------------------------------------------------------
}