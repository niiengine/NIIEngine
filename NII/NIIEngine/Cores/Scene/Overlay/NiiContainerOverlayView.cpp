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
#include "NiiContainerOverlayView.h"
#include "NiiMaterial.h"
#include "NiiStrConv.h"
#include "NiiGBufferManager.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiOverlayTypeList.h"

namespace NII
{
    //------------------------------------------------------------------------
    class _EngineInner CmdSelfView : public PropertyCmd
    {
    public:
        CmdSelfView() : PropertyCmd(N_PropertyContainerOverlayView_SelfView, _T("transparent"),
            _T("Sets whether the panel is transparent, i.e. invisible itself "
                "but it's contents are still displayed."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ContainerOverlayView *>(own)->isSelfView(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ContainerOverlayView *>(own)->setSelfView(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ContainerOverlayView *>(own)->isSelfView();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ContainerOverlayView *>(own)->setSelfView(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelfView();
        }
        /// @copydetails PropertyCmd::set
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdUVCoords : public PropertyCmd
    {
    public:
        CmdUVCoords() : PropertyCmd(N_PropertyContainerOverlayView_UVCoords, _T("uv_coords"),
            _T("The texture coordinates for the texture. 1 set of uv values."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            NIIf u1, v1, u2, v2;

            static_cast<const ContainerOverlayView *>(own)->getUV(u1, v1, u2, v2);
            out = N_conv(u1) + _T(" ") + N_conv(v1) + _T(" ") +
                N_conv(u2) + _T(" ") + N_conv(v2);

        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList vec;
            StrUtil::split(val, vec);
            NIIf t1, t2, t3, t4;
            StrConv::conv(vec[0], t1);
            StrConv::conv(vec[1], t2);
            StrConv::conv(vec[2], t3);
            StrConv::conv(vec[3], t4);
            static_cast<ContainerOverlayView *>(own)->setUV(t1, t2, t3, t4);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            static_cast<const ContainerOverlayView *>(own)->getUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ContainerOverlayView *>(own)->setUV(*temp, *++temp, *++temp, *++temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdUVCoords();
        }
        /// @copydetails PropertyCmd::set
        void getDefault(String & out) const
        {
            out = _T("0.0 0.0 1.0 1.0");
        }
    };
    //------------------------------------------------------------------------
    // vertex buffer bindings, set at compile time (we could look these up but no point)
    #define POSITION_BINDING 0
    #define TEXCOORD_BINDING 1
    //------------------------------------------------------------------------
    ContainerOverlayView::ContainerOverlayView(OverlayGeoID id, ScriptTypeID stid, LangID lid) :
        OverlayView(id, stid, lid),
        // Defer creation of texcoord buffer until we know how big it needs to be
        mTexCoorCount(0),
        mU1(0.0),
        mV1(0.0),
        mU2(1.0),
        mV2(1.0),
        mSelfView(true)
    {
        setupCmd(N_CmdObj_ContainerOverlayView);
    }
    //------------------------------------------------------------------------
    ContainerOverlayView::~ContainerOverlayView()
    {
        if(mInit)
        {
            N_Only(GBuffer).destroy(mGeo.mVertex);
        }
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::init()
    {
        if(!mInit)
        {
            OverlayView::init();
            // Setup render op in advance
            N_Only(GBuffer).create(mGeo.mVertex);
            mGeo.mType = GeometryRaw::OT_TRIANGLE_STRIP;
            mGeo.remove(GRT_Index | GRT_Instancing);

            // Vertex declaration: 1 position, add texcoords later depending on #layers
            // Create as separate buffers so we can lock & discard separately
            VertexData * decl = mGeo.mVertex;
            decl->mOffset = 0;
            decl->mCount = 4;
            decl->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);

            VertexBuffer * vbuf;
            N_Only(GBuffer).create(vbuf, decl->getUnitSize(POSITION_BINDING), decl->mCount, Buffer::M_NORMAL );
            decl->attach(POSITION_BINDING, vbuf);

            mInit = true;
        }
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::setSelfView(bool b)
    {
        mSelfView = b;
    }
    //------------------------------------------------------------------------
    bool ContainerOverlayView::isSelfView() const
    {
        return mSelfView;
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::setUV(NIIf u1, NIIf v1, NIIf u2, NIIf v2)
    {
        mU1 = u1;
        mU2 = u2;
        mV1 = v1;
        mV2 = v2;
        mGeoTexDirty = true;
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::getUV(NIIf & u1, NIIf & v1, NIIf & u2, NIIf & v2) const
    {
        u1 = mU1;
        u2 = mU2;
        v1 = mV1;
        v2 = mV2;
    }
    //------------------------------------------------------------------------
    OverlayGeoType ContainerOverlayView::getTypeID() const
    {
        return N_OverlayType_Container;
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::getGeometry(GeometryRaw & op) const
    {
        op = mGeo;
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::queue(RenderQueue * queue)
    {
        if(mVisible)
        {
            if(mSelfView && mMaterial != 0)
            {
                OverlayGeo::queue(queue);
            }

            ChildMap::iterator i, iend = mChildren.end();
            for(i = mChildren.begin(); i != iend; ++i)
            {
                i->second->queue(queue);
            }
        }
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::updateGeoPos()
    {
        /*
            0-----2
            |    /|
            |  /  |
            |/    |
            1-----3
        */
        NIIf left, right, top, bottom;

        /* Convert positions into -1, 1 coordinate space (homogenous clip space).
            - Left / right is simple range conversion
            - Top / bottom also need inverting since y is upside down - this means
            that top will end up greater than bottom and when computing texture
            coordinates, we have to flip the v-axis (ie. subtract the value from
            1.0 to get the actual correct value).
        */
        left = getEnvLeft() * 2 - 1;
        right = left + (mWidth * 2);
        top = -((getEnvTop() * 2) - 1);
        bottom = top - (mHeight * 2);

        VertexBuffer * vbuf = mGeo.mVertex->getAttach(POSITION_BINDING);
        NIIf * pPos = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

        // Use the furthest away depth value, since materials should have depth-check off
        // This initialised the depth buffer for any 3D objects in front
        NIIf zValue = N_Engine().getRender()->getMaxDepth();
        *pPos++ = left;
        *pPos++ = top;
        *pPos++ = zValue;

        *pPos++ = left;
        *pPos++ = bottom;
        *pPos++ = zValue;

        *pPos++ = right;
        *pPos++ = top;
        *pPos++ = zValue;

        *pPos++ = right;
        *pPos++ = bottom;
        *pPos++ = zValue;

        vbuf->unlock();
    }
    //------------------------------------------------------------------------
    void ContainerOverlayView::updateGeoTex()
    {
        // Generate for as many texture layers as there are in material
        if(mMaterial != 0 && mInit)
        {
            // Assume one technique and pass for the moment
            NCount numLayers = mMaterial->get(0)->get(0)->getTexture().getCount(ShaderChTexture::T_NORMAL);

            VertexData * decl = mGeo.mVertex;
            // Check the number of texcoords we have in our buffer now
            if(mTexCoorCount > numLayers)
            {
                // remove extras
                for(NCount i = mTexCoorCount; i > numLayers; --i)
                {
                    decl->remove(VT_Tex_Coord, static_cast<Nui16>(i));
                }
            }
            else if(mTexCoorCount < numLayers)
            {
                // Add extra texcoord elements
                NCount offset = DataTypeUtil::getSize(DT_Float2x) * mTexCoorCount;
                for (NCount i = mTexCoorCount; i < numLayers; ++i)
                {
                    decl->add(DT_Float2x, VT_Tex_Coord, TEXCOORD_BINDING, offset, static_cast<Nui16>(i));
                    offset += DataTypeUtil::getSize(DT_Float2x);
                }
            }

            // if number of layers changed at all, we'll need to reallocate buffer
            if(mTexCoorCount != numLayers)
            {
                VertexBuffer * newbuf;
                N_Only(GBuffer).create(newbuf, decl->getUnitSize(TEXCOORD_BINDING), decl->mCount, Buffer::M_NORMAL );
                // Bind buffer, note this will unbind the old one and destroy the buffer it had
                decl->attach(TEXCOORD_BINDING, newbuf);
                // Set num tex coords in use now
                mTexCoorCount = numLayers;
            }

            // Get the tcoord buffer & lock
            if(mTexCoorCount)
            {
                VertexBuffer * vbuf = decl->getAttach(TEXCOORD_BINDING);
                NIIf * pVBStart = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

                NCount uvSize = DataTypeUtil::getSize(DT_Float2x) / sizeof(NIIf);
                NCount vertexSize = decl->getUnitSize(TEXCOORD_BINDING) / sizeof(NIIf);
                for(Nidx i = 0; i < numLayers; ++i)
                {
                    /*
                        0-----2
                        |    /|
                        |  /  |
                        |/    |
                        1-----3
                    */
                    // Find start offset for this set
                    NIIf * pTex = pVBStart + (i * uvSize);

                    pTex[0] = mU1;
                    pTex[1] = mV1;

                    pTex += vertexSize; // jump by 1 vertex stride
                    pTex[0] = mU1;
                    pTex[1] = mV2;

                    pTex += vertexSize;
                    pTex[0] = mU2;
                    pTex[1] = mV1;

                    pTex += vertexSize;
                    pTex[0] = mU2;
                    pTex[1] = mV2;
                }
                vbuf->unlock();
            }
        }
    }
    //-----------------------------------------------------------------------
    bool ContainerOverlayView::initCmd(PropertyCmdSet * dest)
    {
        OverlayView::initCmd(dest);

        dest->add(N_new CmdSelfView());
        dest->add(N_new CmdUVCoords());
        return true;
    }
    //------------------------------------------------------------------------
}