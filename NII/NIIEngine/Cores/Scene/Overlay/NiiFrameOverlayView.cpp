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
#include "NiiFrameOverlayView.h"
#include "NiiMaterialManager.h"
#include "NiiOverlayManager.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiRenderQueue.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiOverlayTypeList.h"
#include "NiiViewport.h"
#include "NiiGeometryObj.h"

namespace NII
{
    /** 边界几何
    @version NIIEngine 3.0.0
    */
    class _EngineInner FrameBorderGeo : public GeometryObj, public OverlayAlloc
    {
        friend class FrameOverlayView;
    public:
        FrameBorderGeo(FrameOverlayView * parent):
            mParent(parent)
        {
            mGeometryMark |= GeometryObj::M_IdentityProj;
            mGeometryMark |= GeometryObj::M_IdentityView;
        }

        ~FrameBorderGeo()
        {
            if(mGeo.mVertex)
                N_Only(GBuffer).destroy(mGeo.mVertex);
            if(mGeo.mIndex)
                N_Only(GBuffer).destroy(mGeo.mIndex);
        }

        GeometryRaw * getGeometryPtr() { return &mGeo; }

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const { return mParent->mMaterial; }

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const { op = mGeo; }

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const { mParent->getMatrix(out, count); }

        /// @copydetails GeometryObj::getMatrixCount
        NCount getMatrixCount() const { return 1; }

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const { return mParent->distanceSquared(cam); }

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const { ll.clear(); }
    protected:
        FrameOverlayView * mParent;        ///< 父对象
        GeometryRaw mGeo;
    };

    //------------------------------------------------------------------------
    class _EngineInner CmdBorderSize : public PropertyCmd
    {
    public:
        CmdBorderSize() : PropertyCmd(N_PropertyFrameOverlayView_BorderSize, _T("border_size"),
            _T("The sizes of the borders relative to the screen size, in the order "
            "left, right, top, bottom."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const FrameOverlayView * obj = static_cast<const FrameOverlayView *>(own);

            StrConv::conv(Vector4f(obj->getLeftBorderSize(), obj->getRightBorderSize(),
                obj->getTopBorderSize(), obj->getBottomBorderSize()), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setBorderSize(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            const FrameOverlayView * obj = static_cast<const FrameOverlayView *>(own);
            NIIf * temp = (NIIf *)out;
            *temp = obj->getLeftBorderSize();
            *++temp = obj->getRightBorderSize();
            *++temp = obj->getTopBorderSize();
            *++temp = obj->getBottomBorderSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setBorderSize(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBorderSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 0.0 0.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdLeftUV : public PropertyCmd
    {
    public:
        CmdLeftUV() : PropertyCmd(N_PropertyFrameOverlayView_LeftUV, _T("border_left_uv"),
            _T("The texture coordinates for the left edge border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getLeftCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setLeftCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdLeftUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTopUV : public PropertyCmd
    {
    public:
        CmdTopUV() : PropertyCmd(N_PropertyFrameOverlayView_TopUV, _T("border_top_uv"),
            _T("The texture coordinates for the top edge border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getTopCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setTopCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getTopCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setTopCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTopUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdRightUV : public PropertyCmd
    {
    public:
        CmdRightUV() : PropertyCmd(N_PropertyFrameOverlayView_RightUV, _T("border_right_uv"),
            _T("The texture coordinates for the right edge border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getRightCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setRightCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRightUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdBorderUV : public PropertyCmd
    {
    public:
        CmdBorderUV() : PropertyCmd(N_PropertyFrameOverlayView_BorderUV, _T("border_bottom_uv"),
            _T("The texture coordinates for the bottom edge border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getBottomCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setBottomCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getBottomCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setBottomCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBorderUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdCentreUV : public PropertyCmd
    {
    public:
        CmdCentreUV() : PropertyCmd(N_PropertyFrameOverlayView_CentreUV, _T("uv_coords"),
            _T("The texture coordinates for the texture. 1 set of uv values."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;
            static_cast<const FrameOverlayView *>(own)->getCentreCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setCentreCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getCentreCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setCentreCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCentreUV();
        }
        /// @copydetails PropertyCmd::set
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 1.0 1.0");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTopLeftUV : public PropertyCmd
    {
    public:
        CmdTopLeftUV() : PropertyCmd(N_PropertyFrameOverlayView_TopLeftUV, _T("border_topleft_uv"),
            _T("The texture coordinates for the top-left corner border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;
            static_cast<const FrameOverlayView *>(own)->getTopLeftCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setTopLeftCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getTopLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setTopLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTopLeftUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdBottomLeftUV : public PropertyCmd
    {
    public:
        CmdBottomLeftUV() : PropertyCmd(N_PropertyFrameOverlayView_BottomLeftUV, _T("border_bottomleft_uv"),
            _T("The texture coordinates for the bottom-left corner border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getBottomLeftCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setBottomLeftCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getBottomLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setBottomLeftCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBottomLeftUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdBottomRightUV : public PropertyCmd
    {
    public:
        CmdBottomRightUV() : PropertyCmd(N_PropertyFrameOverlayView_BottomRightUV, _T("border_bottomright_uv"),
            _T("The texture coordinates for the bottom-right corner border texture. 2 sets of uv values, "
            "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const FrameOverlayView *>(own)->getBottomRightCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *> (own)->setBottomRightCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getBottomRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setBottomRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBottomRightUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTopRightUV : public PropertyCmd
    {
    public:
        CmdTopRightUV() : PropertyCmd(N_PropertyFrameOverlayView_TopRightUV, _T("border_topright_uv"),
            _T("The texture coordinates for the top-right corner border texture. 2 sets of uv values, "
                "one for the top-left corner, the other for the bottom-right corner."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;
            static_cast<const FrameOverlayView *>(own)->getTopRightCellUV(u1, v1, u2, v2);
            StrConv::conv(Vector4f(u1 ,v1, u2, v2), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Vector4f temp;
            StrConv::conv(val, temp);
            static_cast<FrameOverlayView *>(own)->setTopRightCellUV(temp.x, temp.y, temp.z, temp.w);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const FrameOverlayView *>(own)->getTopRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<FrameOverlayView *>(own)->setTopRightCellUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTopRightUV();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //---------------------------------------------------------------------
    #define BCELL_UV(x) (x * 4 * 2)
    #define POSITION_BINDING 0
    #define TEXCOORD_BINDING 1
    //---------------------------------------------------------------------
    FrameOverlayView::FrameOverlayView(OverlayGeoID id, LangID lid):
        ContainerOverlayView(id, N_CmdObj_FrameOverlayView, lid),
        mLeftBorderSize(0),
        mRightBorderSize(0),
        mTopBorderSize(0),
        mBottomBorderSize(0),
        mPixelLeftBorderSize(0),
        mPixelRightBorderSize(0),
        mPixelTopBorderSize(0),
        mPixelBottomBorderSize(0),
        mFrameGeo(0)
    {
        setupCmd(N_CmdObj_FrameOverlayView);
    }
    //---------------------------------------------------------------------
    FrameOverlayView::~FrameOverlayView()
    {
        if(mInit)
        {
            N_delete mFrameGeo;
            mFrameGeo = 0;
        }
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::init()
    {
        if(!mInit)
        {
            ContainerOverlayView::init();

            mFrameGeo = N_new FrameBorderGeo(this);
            GeometryRaw * georaw = mFrameGeo->getGeometryPtr();
            georaw->mType = GeometryRaw::OT_TRIANGLE_LIST;
            georaw->add(GRT_Index);
            georaw->remove(GRT_Instancing);

            N_Only(GBuffer).create(georaw->mVertex);
            VertexData * vdata = georaw->mVertex;
            vdata->mCount = 4 * 8;
            vdata->mOffset = 0;

            // Position and texture coords each have their own buffers to allow
            // each to be edited separately with the discard flag
            vdata->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);
            vdata->add(DT_Float2x, VT_Tex_Coord, TEXCOORD_BINDING, 0, 0);

            VertexBuffer * vbuf = 0;
            N_Only(GBuffer).create(vbuf, vdata->getUnitSize(POSITION_BINDING), vdata->mCount, Buffer::M_NORMAL );
            vdata->attach(POSITION_BINDING, vbuf);

            N_Only(GBuffer).create(vbuf, vdata->getUnitSize(TEXCOORD_BINDING), vdata->mCount, Buffer::M_NORMAL);
            vdata->attach(TEXCOORD_BINDING, vbuf);

            N_Only(GBuffer).create(georaw->mIndex);
            IndexData * idxd = georaw->mIndex;
            idxd->mCount = 8 * 6;
            idxd->mOffset = 0;

            /* Each cell is
                0-----2
                |    /|
                |  /  |
                |/    |
                1-----3
            */
            N_Only(GBuffer).create(idxd->mBuffer, 16, idxd->mCount, Buffer::M_NORMAL );
            Nui16 * pIdx = static_cast<Nui16 *>(idxd->mBuffer->lock(0, idxd->mBuffer->getSize(), Buffer::MM_WHOLE));
            for(Nui16 cell = 0; cell < 8; ++cell)
            {
                Nui16 base = cell * 4;
                *pIdx++ = base;
                *pIdx++ = base + 1;
                *pIdx++ = base + 2;

                *pIdx++ = base + 2;
                *pIdx++ = base + 1;
                *pIdx++ = base + 3;
            }
            idxd->mBuffer->unlock();

            mInit = true;
        }
    }
    //---------------------------------------------------------------------
    OverlayGeoType FrameOverlayView::getTypeID() const
    {
        return N_OverlayType_Frame;
    }
    //---------------------------------------------------------------------
    bool FrameOverlayView::initCmd(PropertyCmdSet * dest)
    {
        ContainerOverlayView::initCmd(dest);
        dest->add(N_new CmdBorderSize());
        dest->add(N_new CmdLeftUV());
        dest->add(N_new CmdTopUV());
        dest->add(N_new CmdBorderUV());
        dest->add(N_new CmdCentreUV());
        dest->add(N_new CmdRightUV());
        dest->add(N_new CmdTopLeftUV());
        dest->add(N_new CmdBottomLeftUV());
        dest->add(N_new CmdTopRightUV());
        dest->add(N_new CmdBottomRightUV());
        return true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBorderSize(NIIf size)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelLeftBorderSize = mPixelRightBorderSize =
                mPixelTopBorderSize = mPixelBottomBorderSize = static_cast<Nui16>(size);
        }
        else
        {
            mLeftBorderSize = mRightBorderSize = mTopBorderSize = mBottomBorderSize = size;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setLeftBorderSize(NIIf size)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelLeftBorderSize = static_cast<Nui16>(size);
        }
        else
        {
            mLeftBorderSize = size;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setRightBorderSize(NIIf size)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelRightBorderSize = static_cast<Nui16>(size);
        }
        else
        {
            mRightBorderSize = size;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setTopBorderSize(NIIf size)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelTopBorderSize = static_cast<Nui16>(size);
        }
        else
        {
            mTopBorderSize = size;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBottomBorderSize(NIIf size)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelBottomBorderSize = static_cast<Nui16>(size);
        }
        else
        {
            mBottomBorderSize = size;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBorderSize(NIIf left, NIIf right, NIIf top, NIIf bottom)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelLeftBorderSize = static_cast<Nui16>(left);
            mPixelTopBorderSize = static_cast<Nui16>(top);
            mPixelRightBorderSize = static_cast<Nui16>(right);
            mPixelBottomBorderSize = static_cast<Nui16>(bottom);
        }
        else
        {
            mLeftBorderSize = left;
            mRightBorderSize = right;
            mTopBorderSize = top;
            mBottomBorderSize = bottom;
        }
        mGeoPosDirty = true;
    }
    //---------------------------------------------------------------------
    NIIf FrameOverlayView::getLeftBorderSize() const
    {
        if(mMetricsMode == SM_Abs)
        {
            return mPixelLeftBorderSize;
        }
        else
        {
            return mLeftBorderSize;
        }
    }
    //---------------------------------------------------------------------
    NIIf FrameOverlayView::getRightBorderSize() const
    {
        if (mMetricsMode == SM_Abs)
        {
            return mPixelRightBorderSize;
        }
        else
        {
            return mRightBorderSize;
        }
    }
    //---------------------------------------------------------------------
    NIIf FrameOverlayView::getTopBorderSize() const
    {
        if (mMetricsMode == SM_Abs)
        {
            return mPixelTopBorderSize;
        }
        else
        {
            return mTopBorderSize;
        }
    }
    //---------------------------------------------------------------------
    NIIf FrameOverlayView::getBottomBorderSize() const
    {
        if(mMetricsMode == SM_Abs)
        {
            return mPixelBottomBorderSize;
        }
        else
        {
            return mBottomBorderSize;
        }
    }
    //------------------------------------------------------------------------
    void FrameOverlayView::setCentreCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mU1 = u1;
        mV1 = v1;
        mU2 = u2;
        mV2 = v2;
        mGeoTexDirty = true;
    }
    //------------------------------------------------------------------------
    void FrameOverlayView::getCentreCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mU1;
        v1 = mV1;
        u2 = mU2;
        v2 = mV2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::updateGeoTex()
    {
        ContainerOverlayView::updateGeoTex();
        /* Each cell is
            0-----2
            |    /|
            |  /  |
            |/    |
            1-----3
        */
        // No choice but to lock / unlock each time here, but lock only small sections

        VertexBuffer * vbuf = mFrameGeo->getGeometryPtr()->mVertex->getAttach(TEXCOORD_BINDING);
        NIIf * pUV = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);
        for(NCount i = 0; i < 8; ++i)
        {
            *pUV++ = mBorderUV[i].u1;
            *pUV++ = mBorderUV[i].v1;
            *pUV++ = mBorderUV[i].u1;
            *pUV++ = mBorderUV[i].v2;

            *pUV++ = mBorderUV[i].u2;
            *pUV++ = mBorderUV[i].v1;
            *pUV++ = mBorderUV[i].u2;
            *pUV++ = mBorderUV[i].v2;
        }

        vbuf->unlock();
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Left].u1 = u1;
        mBorderUV[CT_Left].u2 = u2;
        mBorderUV[CT_Left].v1 = v1;
        mBorderUV[CT_Left].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Right].u1 = u1;
        mBorderUV[CT_Right].u2 = u2;
        mBorderUV[CT_Right].v1 = v1;
        mBorderUV[CT_Right].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setTopCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Top].u1 = u1;
        mBorderUV[CT_Top].u2 = u2;
        mBorderUV[CT_Top].v1 = v1;
        mBorderUV[CT_Top].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBottomCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Bottom].u1 = u1;
        mBorderUV[CT_Bottom].u2 = u2;
        mBorderUV[CT_Bottom].v1 = v1;
        mBorderUV[CT_Bottom].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setTopLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Top_Left].u1 = u1;
        mBorderUV[CT_Top_Left].u2 = u2;
        mBorderUV[CT_Top_Left].v1 = v1;
        mBorderUV[CT_Top_Left].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setTopRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Top_Right].u1 = u1;
        mBorderUV[CT_Top_Right].u2 = u2;
        mBorderUV[CT_Top_Right].v1 = v1;
        mBorderUV[CT_Top_Right].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBottomLeftCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Bottom_Left].u1 = u1;
        mBorderUV[CT_Bottom_Left].u2 = u2;
        mBorderUV[CT_Bottom_Left].v1 = v1;
        mBorderUV[CT_Bottom_Left].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::setBottomRightCellUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mBorderUV[CT_Bottom_Right].u1 = u1;
        mBorderUV[CT_Bottom_Right].u2 = u2;
        mBorderUV[CT_Bottom_Right].v1 = v1;
        mBorderUV[CT_Bottom_Right].v2 = v2;
        mGeoTexDirty = true;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Left].u1;
        v1 = mBorderUV[CT_Left].v1;
        u2 = mBorderUV[CT_Left].u2;
        v2 = mBorderUV[CT_Left].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Right].u1;
        v1 = mBorderUV[CT_Right].v1;
        u2 = mBorderUV[CT_Right].u2;
        v2 = mBorderUV[CT_Right].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getTopCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Top].u1;
        v1 = mBorderUV[CT_Top].v1;
        u2 = mBorderUV[CT_Top].u2;
        v2 = mBorderUV[CT_Top].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getBottomCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Bottom].u1;
        v1 = mBorderUV[CT_Bottom].v1;
        u2 = mBorderUV[CT_Bottom].u2;
        v2 = mBorderUV[CT_Bottom].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getTopLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Top_Left].u1;
        v1 = mBorderUV[CT_Top_Left].v1;
        u2 = mBorderUV[CT_Top_Left].u2;
        v2 = mBorderUV[CT_Top_Left].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getTopRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Top_Right].u1;
        v1 = mBorderUV[CT_Top_Right].v1;
        u2 = mBorderUV[CT_Top_Right].u2;
        v2 = mBorderUV[CT_Top_Right].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getBottomLeftCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Bottom_Left].u1;
        v1 = mBorderUV[CT_Bottom_Left].v1;
        u2 = mBorderUV[CT_Bottom_Left].u2;
        v2 = mBorderUV[CT_Bottom_Left].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::getBottomRightCellUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mBorderUV[CT_Bottom_Right].u1;
        v1 = mBorderUV[CT_Bottom_Right].v1;
        u2 = mBorderUV[CT_Bottom_Right].u2;
        v2 = mBorderUV[CT_Bottom_Right].v2;
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::updateGeoPos()
    {
        /*
        Grid is like this:
        +--+---------------+--+
        |0 |       1       |2 |
        +--+---------------+--+
        |  |               |  |
        |  |               |  |
        |3 |    center     |4 |
        |  |               |  |
        +--+---------------+--+
        |5 |       6       |7 |
        +--+---------------+--+
        */
        // Convert positions into -1, 1 coordinate space (homogenous clip space)
        // Top / bottom also need inverting since y is upside down
        NIIf left[8], right[8], top[8], bottom[8];
        // Horizontal
        left[0] = left[3] = left[5] = getEnvLeft() * 2 - 1;
        left[1] = left[6] = right[0] = right[3] = right[5] = left[0] + (mLeftBorderSize * 2);
        right[2] = right[4] = right[7] = left[0] + (mWidth * 2);
        left[2] = left[4] = left[7] = right[1] = right[6] = right[2] - (mRightBorderSize * 2);
        // Vertical
        top[0] = top[1] = top[2] = -((getEnvTop() * 2) - 1);
        top[3] = top[4] = bottom[0] = bottom[1] = bottom[2] = top[0] - (mTopBorderSize * 2);
        bottom[5] = bottom[6] = bottom[7] = top[0] -  (mHeight * 2);
        top[5] = top[6] = top[7] = bottom[3] = bottom[4] = bottom[5] + (mBottomBorderSize * 2);

        // Lock the whole position buffer in discard mode
        VertexBuffer * vbuf = mFrameGeo->getGeometryPtr()->mVertex->getAttach(POSITION_BINDING);
        NIIf * pPos = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);
        // Use the furthest away depth value, since materials should have depth-check off
        // This initialised the depth buffer for any 3D objects in front
        NIIf zValue = N_Engine().getRender()->getMaxDepth();
        for(Nui16 cell = 0; cell < 8; ++cell)
        {
            /*
                0-----2
                |    /|
                |  /  |
                |/    |
                1-----3
            */
            *pPos++ = left[cell];
            *pPos++ = top[cell];
            *pPos++ = zValue;

            *pPos++ = left[cell];
            *pPos++ = bottom[cell];
            *pPos++ = zValue;

            *pPos++ = right[cell];
            *pPos++ = top[cell];
            *pPos++ = zValue;

            *pPos++ = right[cell];
            *pPos++ = bottom[cell];
            *pPos++ = zValue;

        }
        vbuf->unlock();

        // Also update center geometry
        // NB don't use superclass because we need to make it smaller because of border
        vbuf = mGeo.mVertex->getAttach(POSITION_BINDING);
        pPos = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);
        // Use cell 1 and 3 to determine positions
        *pPos++ = left[1];
        *pPos++ = top[3];
        *pPos++ = zValue;

        *pPos++ = left[1];
        *pPos++ = bottom[3];
        *pPos++ = zValue;

        *pPos++ = right[1];
        *pPos++ = top[3];
        *pPos++ = zValue;

        *pPos++ = right[1];
        *pPos++ = bottom[3];
        *pPos++ = zValue;

        vbuf->unlock();
    }
    //---------------------------------------------------------------------
    void FrameOverlayView::queue(RenderQueue * queue)
    {
        // Add self twice to the queue
        // Have to do this to allow 2 materials
        if(mVisible)
        {
            queue->add(mFrameGeo, RQG_Overlay, mZOrder);
            ContainerOverlayView::queue(queue);
        }
    }
    //-----------------------------------------------------------------------
    void FrameOverlayView::setSizeMode(SizeMode gmm)
    {
        ContainerOverlayView::setSizeMode(gmm);
        if(gmm != SM_Unit)
        {
            mPixelLeftBorderSize = static_cast<Nui16>(mLeftBorderSize);
            mPixelTopBorderSize = static_cast<Nui16>(mTopBorderSize);
            mPixelRightBorderSize = static_cast<Nui16>(mRightBorderSize);
            mPixelBottomBorderSize = static_cast<Nui16>(mBottomBorderSize);
        }
    }
    //-----------------------------------------------------------------------
    void FrameOverlayView::_update()
    {
        if(mMetricsMode != SM_Unit && (!mViewport->isSyncAspect() || mGeoPosDirty))
        {
            mLeftBorderSize = mPixelLeftBorderSize * mPixelScaleX;
            mTopBorderSize = mPixelTopBorderSize * mPixelScaleY;
            mRightBorderSize = mPixelRightBorderSize * mPixelScaleX;
            mBottomBorderSize = mPixelBottomBorderSize * mPixelScaleY;
            mGeoPosDirty = true;
        }
        ContainerOverlayView::_update();
    }
    //---------------------------------------------------------------------
}