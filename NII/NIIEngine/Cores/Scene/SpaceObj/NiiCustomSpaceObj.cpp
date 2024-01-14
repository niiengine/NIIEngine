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
#include "NiiCustomSpaceObj.h"
#include "NiiMaterialManager.h"
#include "NiiEngine.h"
#include "NiiGBufferManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiEdgeListBuilder.h"
#include "NiiMeshManager.h"
#include "NiiGeometryObj.h"
#include "NiiLight.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CustomSpaceObjFactory
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CustomSpaceObjFactory::CustomSpaceObjFactory()
    {
    }
    //------------------------------------------------------------------------
    CustomSpaceObjFactory::~CustomSpaceObjFactory()
    {
    }
    //------------------------------------------------------------------------
    FactoryID CustomSpaceObjFactory::getID() const
    {
        return N_FACTORY_CustomSpace;
    }
    //-----------------------------------------------------------------------------
    SpaceObj * CustomSpaceObjFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        SpaceObj * re = N_new CustomSpaceObj(sid);
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObjFactory::destroy(void * obj)
    {
        SpaceObj * temp = static_cast<SpaceObj *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------------

    /** 内建,可渲染几何的部件
    @version NIIEngine 3.0.0
    */
    class _EngineInner CustomSpaceObjSection : public GeometryObj, public SpaceAlloc
    {
    public:
        CustomSpaceObjSection(CustomSpaceObj * parent, ResourceID mrid,
            GeometryRaw::OperationType type, GroupID gid = GroupDefault);
        virtual ~CustomSpaceObjSection();

        /// Retrieve render operation for manipulation
        inline GeometryRaw * getGeometry(){ return &mRenderOperation; };

        /// Retrieve the material name in use
        inline ResourceID getMaterialName() const { return mMaterialID; }

        /// Retrieve the material group in use
        inline GroupID getMaterialGroup() const { return mGroupName; }

        /// update the material name in use
        void setMaterial(ResourceID rid, GroupID gid = GroupUnknow)
        {
            if(mMaterialID != rid || mGroupName != gid)
            {
                mMaterialID = rid;
                mGroupName = gid;
                if(mMaterial != 0)
                {
                    mMaterial->unref();

                }
                if(mMaterialID != 0)
                {
                    mMaterial = static_cast<Material *>(N_Only(Material).load(mMaterialID, mGroupName));
                    mMaterial->ref(false);
                }
                else
                {
                    mMaterial = 0;
                }
            }
        }

        /// Set whether we need 32-bit indices
        inline void set32BitIndices(bool b) { m32BitIndices = b; }

        /// Get whether we need 32-bit indices
        inline bool get32BitIndices() const { return m32BitIndices; }

        /// @copydetails GeometryObj::getMaterial.
        const Material * getMaterial() const;

        /// @copydetails GeometryObj::getGeometry.
        void getGeometry(GeometryRaw & op) const;

        /// @copydetails GeometryObj::getMatrix.
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::distanceSquared.
        NIIf distanceSquared(const Camera * c) const;

        /// @copydetails GeometryObj::getLight.
        void getLight(LightList & ll) const;
    protected:
        CustomSpaceObj * mParent;
        ResourceID mMaterialID;
        GroupID mGroupName;
        mutable Material * mMaterial;
        GeometryRaw mRenderOperation;
        bool m32BitIndices;
    };
    //-----------------------------------------------------------------------------
    CustomSpaceObjSection::CustomSpaceObjSection(CustomSpaceObj * parent,
        ResourceID mrid, GeometryRaw::OperationType opType, GroupID gid) :
            mParent(parent),
            mMaterialID(mrid),
            mGroupName(gid),
            m32BitIndices(false)
    {
        mMaterial = static_cast<Material *>(N_Only(Material).load(mMaterialID, mGroupName));
        mMaterial->ref(false);

        N_Only(GBuffer).create(mRenderOperation.mVertex);
        mRenderOperation.mVertex->mCount = 0;
        mRenderOperation.mType = opType;
        mRenderOperation.remove(GRT_Index | GRT_Instancing);
    }
    //-----------------------------------------------------------------------------
    CustomSpaceObjSection::~CustomSpaceObjSection()
    {
        if(mMaterial)
        {
            mMaterial->unref();
            mMaterial = 0;
        }
        N_Only(GBuffer).destroy(mRenderOperation.mVertex);
        if(mRenderOperation.mIndex)
        {
            N_Only(GBuffer).destroy(mRenderOperation.mIndex);
        }
    }
    //-----------------------------------------------------------------------------
    const Material * CustomSpaceObjSection::getMaterial() const
    {
        return mMaterial;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObjSection::getGeometry(GeometryRaw & op) const
    {
        op = mRenderOperation;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObjSection::getMatrix(Matrix4f * out, NCount & count) const
    {
        out[0] = mParent->getSpaceMatrix();
        count = 1;
    }
    //-----------------------------------------------------------------------------
    NIIf CustomSpaceObjSection::distanceSquared(const Camera * cam) const
    {
        PosNode * n = mParent->getSpaceNode();
        N_assert(n, "error");
        return n->distanceSquared(cam);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObjSection::getLight(LightList & ll) const
    {
        mParent->query(ll);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // CustomSpaceObjShadowVolume
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    class _EngineInner CustomSpaceObjShadowVolume : public ShadowVolume
    {
    public:
        CustomSpaceObjShadowVolume(CustomSpaceObj * parent,
            IndexBuffer * index, const VertexData * vertex,
                bool createSeparateLightCap, bool isLightCap = false);

        ~CustomSpaceObjShadowVolume()
        {
            N_delete mRenderOp.mIndex;
            N_delete mRenderOp.mVertex;
        }

        inline VertexBuffer * getWBuffer() { return mWBuffer; }

        inline VertexBuffer * getPositionBuffer() { return mPositionBuffer; }

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const
        {
            *out = mParent->getSpaceMatrix();
            count = 1;
        }
    protected:
        CustomSpaceObj * mParent;
        VertexBuffer * mWBuffer;
        VertexBuffer * mPositionBuffer;
    };
    //-----------------------------------------------------------------------------
    CustomSpaceObjShadowVolume::CustomSpaceObjShadowVolume(CustomSpaceObj * parent,
        IndexBuffer * idxbuf, const VertexData * vdata, bool createSeparateLightCap,
        bool isLightCap) :
            mParent(parent)
    {
        // Initialise render op
        mRenderOp.mIndex = N_new IndexData();
        mRenderOp.mIndex->attach(idxbuf);
        mRenderOp.mIndex->mOffset = 0;
        // index start and count are sorted out later

        // Create vertex data which just references position component (and 2 component)
        VertexData * vb;
        N_Only(GBuffer).create(mRenderOp.mVertex);
        vb = mRenderOp.mVertex;
        // Map in position data
        vb->add(DT_Float3x, VT_Pos, 0, 0);
        mPositionBuffer = vdata->getAttach(vdata->get(VT_Pos)->mSource);
        vb->attach(0, mPositionBuffer);
        // Map in w-coord buffer (if present)
        if(vdata->getExt() != 0)
        {
            vb->add(DT_Float1x, VT_Tex_Coord, 1, 0, 0);
            mWBuffer = vdata->getExt();
            vb->attach(1, mWBuffer);
        }
        // Use same vertex start as input
        vb->mOffset = vdata->mOffset;

        if(isLightCap)
        {
            // Use original vertex count, no extrusion
            vb->mCount = vdata->mCount;
        }
        else
        {
            // Vertex count must take into account the doubling of the buffer,
            // because second half of the buffer is the extruded copy
            vb->mCount = vdata->mCount * 2;
            if(createSeparateLightCap)
            {
                // Create child light cap
                mLightCap = N_new CustomSpaceObjShadowVolume(parent,
                    idxbuf, vdata, false, true);
            }
        }
    }
    //-----------------------------------------------------------------------------
    #define TEMP_INITIAL_SIZE 50
    #define TEMP_VERTEXSIZE_GUESS sizeof(NIIf) * 12
    #define TEMP_INITIAL_VERTEX_SIZE TEMP_VERTEXSIZE_GUESS * TEMP_INITIAL_SIZE
    #define TEMP_INITIAL_INDEX_SIZE sizeof(Nui32) * TEMP_INITIAL_SIZE
    //-----------------------------------------------------------------------------
    CustomSpaceObj::CustomSpaceObj() :
        ShadowObj(),
        mOpVertexData(0),
        mOpVertexSize(TEMP_INITIAL_VERTEX_SIZE),
        mOpIndexData(0),
        mOpIndexSize(TEMP_INITIAL_INDEX_SIZE),
        mUnitSize(0),
        mVertexCount(0),
        mIndexCount(0),
        mTexCoordCount(0),
        mRadius(0),
        mEdgeList(0),
        mOpGeo(0),
        mOnlyWrite(false),
        mUnitInput(true),
        mVertexIndex(false),
        mIdentityView(false),
        mCacheUpload(false),
        mIdentityProj(false),
        mIndexOrderRender(false)
    {
    }
    //-----------------------------------------------------------------------------
    CustomSpaceObj::CustomSpaceObj(SpaceID sid) :
        ShadowObj(sid),
        mOpGeo(0),
        mOpVertexData(0),
        mOpVertexSize(TEMP_INITIAL_VERTEX_SIZE),
        mOpIndexData(0),
        mOpIndexSize(TEMP_INITIAL_INDEX_SIZE),
        mUnitSize(0),
        mVertexCount(0),
        mIndexCount(0),
        mTexCoordCount(0),
        mRadius(0),
        mEdgeList(0),
        mOnlyWrite(false),
        mVertexIndex(false),
        mUnitInput(true),
        mIdentityView(false),
        mCacheUpload(false),
        mIdentityProj(false),
        mIndexOrderRender(false)
    {
    }
    //-----------------------------------------------------------------------------
    CustomSpaceObj::~CustomSpaceObj()
    {
        clear();
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::clear()
    {
        resetCacheData();
        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mGeoList.clear();
        N_delete mEdgeList;
        mEdgeList = 0;
        mRadius = 0;
        mVertexIndex = false;
        ShadowVolumeList::iterator s, send = mShadowVolumeList.end();
        for(s = mShadowVolumeList.begin(); s != send; ++s)
        {
            N_delete *s;
        }
        mShadowVolumeList.clear();

        mAABB.setNull();
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::resetCacheData()
    {
        N_free(mOpVertexData);
        N_free(mOpIndexData);
        mOpVertexData = 0;
        mOpIndexData = 0;
        mOpVertexSize = TEMP_INITIAL_VERTEX_SIZE;
        mOpIndexSize = TEMP_INITIAL_INDEX_SIZE;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::cacheVertexCount(NCount numVerts)
    {
        // Calculate byte size
        // Use decl if we know it by now, otherwise default size to pos/norm/texcoord*2
        NCount newSize;
        if(!mUnitInput)
        {
            newSize = mUnitSize * numVerts;
        }
        else
        {
            // estimate - size checks will deal for subsequent verts
            newSize = TEMP_VERTEXSIZE_GUESS * numVerts;
        }

        if(newSize > mOpVertexSize || !mOpVertexData)
        {
            if(!mOpVertexData)
            {
                // init
                newSize = mOpVertexSize;
            }
            else
            {
                // increase to at least NIId current
                newSize = std::max(newSize, mOpVertexSize*2);
            }
            // copy old data
            Nui8 * tmp = mOpVertexData;
            mOpVertexData = N_alloc_t(Nui8, newSize);
            if(tmp)
            {
                memcpy(mOpVertexData, tmp, mOpVertexSize);
                // delete old buffer
                N_free(tmp);
            }
            mOpVertexSize = newSize;
        }
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::cacheIndexCount(NCount numInds)
    {
        NCount newSize = numInds * sizeof(Nui32);
        if(newSize > mOpIndexSize || !mOpIndexData)
        {
            if(!mOpIndexData)
            {
                // init
                newSize = mOpIndexSize;
            }
            else
            {
                // increase to at least NIId current
                newSize = std::max(newSize, mOpIndexSize * 2);
            }
            numInds = newSize / sizeof(Nui32);
            Nui32 * tmp = mOpIndexData;
            mOpIndexData = N_alloc_t(Nui32, numInds);
            if (tmp)
            {
                memcpy(mOpIndexData, tmp, mOpIndexSize);
                N_free(tmp);
            }
            mOpIndexSize = newSize;
        }
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::reserveVertexCount(NCount vcount)
    {
        cacheVertexCount(vcount);
        mVertexCount = vcount;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::reserveIndexCount(NCount icount)
    {
        cacheIndexCount(icount);
        mIndexCount = icount;
    }
    //-----------------------------------------------------------------------------
    /*GeometryObj * CustomSpaceObj::endModify()
    {
        return end();
    }*/
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::begin(GeometryRaw::OperationType type, ResourceID rid, GroupID gid)
    {
        if(mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You cannot call begin() again until after you call end()"));
        }
        mOpGeo = N_new CustomSpaceObjSection(this, rid, type, gid);

        mOpUpdate = false;
        if(mIdentityProj)
            mOpGeo->on(GeometryObj::M_IdentityProj);
        else
            mOpGeo->off(GeometryObj::M_IdentityProj);
        if(mIdentityView)
            mOpGeo->on(GeometryObj::M_IdentityView);
        else
            mOpGeo->off(GeometryObj::M_IdentityView);
        mGeoList.push_back(mOpGeo);
        mUnitInput = true;
        mUnitSize = 0;
        mTexCoordCount = 0;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::begin(GeometryRaw::OperationType type, const Material * mat)
    {
        begin(type, mat->getOriginID(), mat->getGroup());
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::position(const Vector3f & pos)
    {
        position(pos.x, pos.y, pos.z);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::position(NIIf x, NIIf y, NIIf z)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mCacheUpload)
        {
            // bake current vertex
            updateVertexData();
            mUnitInput = false;
        }

        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float3x, VT_Pos, 0, mUnitSize);
            mUnitSize += DataTypeUtil::getSize(DT_Float3x);
        }

        mOpVertex.mPos.x = x;
        mOpVertex.mPos.y = y;
        mOpVertex.mPos.z = z;

        // update bounds
        mAABB.merge(mOpVertex.mPos);
        mRadius = std::max(mRadius, mOpVertex.mPos.length());

        // reset current texture coord
        mTexCoordCount = 0;

        mCacheUpload = true;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::normals(const Vector3f & nls)
    {
        normals(nls.x, nls.y, nls.z);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::normals(NIIf x, NIIf y, NIIf z)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float3x, VT_Normals, 0, mUnitSize);
            mUnitSize += DataTypeUtil::getSize(DT_Float3x);
        }
        mOpVertex.mNorl.x = x;
        mOpVertex.mNorl.y = y;
        mOpVertex.mNorl.z = z;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::tangent(const Vector3f & tang)
    {
        tangent(tang.x, tang.y, tang.z);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::tangent(NIIf x, NIIf y, NIIf z)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float3x, VT_Tangent, 0, mUnitSize);
            mUnitSize += DataTypeUtil::getSize(DT_Float3x);
        }
        mOpVertex.mTangent.x = x;
        mOpVertex.mTangent.y = y;
        mOpVertex.mTangent.z = z;
    }

    //-----------------------------------------------------------------------------
    void CustomSpaceObj::textureCoord(NIIf u)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float1x, VT_Tex_Coord, 0, mUnitSize, mTexCoordCount);
            mUnitSize += DataTypeUtil::getSize(DT_Float1x);
        }
        mOpVertex.mCoordDim[mTexCoordCount] = 1;
        mOpVertex.mCoord[mTexCoordCount].x = u;

        ++mTexCoordCount;

    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::textureCoord(const Vector2f & uv)
    {
        textureCoord(uv.x, uv.y);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::textureCoord(NIIf u, NIIf v)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float2x, VT_Tex_Coord, 0, mUnitSize, mTexCoordCount);
            mUnitSize += DataTypeUtil::getSize(DT_Float2x);
        }
        mOpVertex.mCoordDim[mTexCoordCount] = 2;
        mOpVertex.mCoord[mTexCoordCount].x = u;
        mOpVertex.mCoord[mTexCoordCount].y = v;

        ++mTexCoordCount;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::textureCoord(NIIf u, NIIf v, NIIf w)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float3x, VT_Tex_Coord, 0, mUnitSize, mTexCoordCount);
            mUnitSize += DataTypeUtil::getSize(DT_Float3x);
        }
        mOpVertex.mCoordDim[mTexCoordCount] = 3;
        mOpVertex.mCoord[mTexCoordCount].x = u;
        mOpVertex.mCoord[mTexCoordCount].y = v;
        mOpVertex.mCoord[mTexCoordCount].z = w;

        ++mTexCoordCount;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::textureCoord(NIIf x, NIIf y, NIIf z, NIIf w)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Float4x, VT_Tex_Coord, 0, mUnitSize, mTexCoordCount);
            mUnitSize += DataTypeUtil::getSize(DT_Float4x);
        }
        mOpVertex.mCoordDim[mTexCoordCount] = 4;
        mOpVertex.mCoord[mTexCoordCount].x = x;
        mOpVertex.mCoord[mTexCoordCount].y = y;
        mOpVertex.mCoord[mTexCoordCount].z = z;
        mOpVertex.mCoord[mTexCoordCount].w = w;

        ++mTexCoordCount;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::colour(const Colour & c)
    {
        colour(c.r, c.g, c.b, c.a);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::colour(NIIf r, NIIf g, NIIf b, NIIf a)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        if(mUnitInput && !mOpUpdate)
        {
            // defining declaration
            mOpGeo->getGeometry()->mVertex->add(DT_Colour, VT_Diffuse, 0, mUnitSize);
            mUnitSize += DataTypeUtil::getSize(DT_Colour);
        }
        mOpVertex.mColour.r = r;
        mOpVertex.mColour.g = g;
        mOpVertex.mColour.b = b;
        mOpVertex.mColour.a = a;

    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::index(Nui32 idx)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }
        mVertexIndex = true;
        if(idx >= 65536)
            mOpGeo->set32BitIndices(true);

        // make sure we have index data
        GeometryRaw * gr = mOpGeo->getGeometry();
        if(!gr->mIndex)
        {
            N_Only(GBuffer).create(gr->mIndex);
            gr->mIndex->mCount = 0;
        }
        gr->add(GRT_Index);
        cacheIndexCount(++gr->mIndex->mCount);

        mOpIndexData[gr->mIndex->mCount - 1] = idx;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::triangle(Nui32 i1, Nui32 i2, Nui32 i3)
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You must call begin() before this method"));
        }

        if(mOpGeo->getGeometry()->mType != GeometryRaw::OT_TRIANGLE_LIST)
        {
            N_EXCEPT(InvalidParam, _I18n("This method is only valid on triangle lists"));
        }

        index(i1);
        index(i2);
        index(i3);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::quad(Nui32 i1, Nui32 i2, Nui32 i3, Nui32 i4)
    {
        // first tri
        triangle(i1, i2, i3);
        // second tri
        triangle(i3, i4, i1);
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::beginModify(Nidx idx)
    {
        if(mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You cannot call begin() again until after you call end()"));
        }
        if(idx >= mGeoList.size())
        {
            N_EXCEPT(InvalidParam, _I18n("Invalid section index - out of range."));
        }
        mOpGeo = mGeoList[idx];
        mOpUpdate = true;
        mUnitInput = true;
        mTexCoordCount = 0;
        // reset vertex & index count
        GeometryRaw * gr = mOpGeo->getGeometry();

        gr->mVertex->mCount = 0;
        if(gr->mIndex)
        {
            gr->mIndex->mCount = 0;
        }
        gr->remove(GRT_Index);
        mUnitSize = gr->mVertex->getUnitSize(0);
    }
    //-----------------------------------------------------------------------------
    NCount CustomSpaceObj::getVertexCount() const
    {
        if(!mOpGeo)
            return 0;

        GeometryRaw * gr = mOpGeo->getGeometry();

        // There's an unfinished vertex being defined, so include it in count
        if(mCacheUpload)
            return gr->mVertex->mCount + 1;
        else
            return gr->mVertex->mCount;
    }
    //-----------------------------------------------------------------------------
    NCount CustomSpaceObj::getIndexCount() const
    {
        if(!mOpGeo)
            return 0;

        GeometryRaw * ro = mOpGeo->getGeometry();
        if(ro->mIndex)
            return ro->mIndex->mCount;
        else
            return 0;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::updateVertexData()
    {
        mCacheUpload = false;
        VertexData * vd = mOpGeo->getGeometry()->mVertex;
        if(vd->mCount == 0 && !mOpUpdate)
        {
            // first vertex, autoorganise decl
            vd->build(VertexData::M_None, false);
        }
        cacheVertexCount(++vd->mCount);

        // get base pointer
        Nui8 * pBase = mOpVertexData + (mUnitSize * (vd->mCount - 1));

        VertexUnitList::const_iterator i, iend = vd->getUnits().end();
        for(i = vd->getUnits().begin(); i != iend; ++i)
        {
            NIIf * pFloat = 0;
            RGBA * pRGBA = 0;
            const VertexUnit & elem = *i;
            switch(elem.mType)
            {
            case DT_Float1x:
            case DT_Float2x:
            case DT_Float3x:
            case DT_Float4x:
                elem.getData(pBase, pFloat);
                break;
            case DT_Colour:
            case DT_Colour_ABGR:
            case DT_Colour_ARGB:
                elem.getData(pBase, pRGBA);
                break;
            default:
                // nop ?
                break;
            };

            RenderSys * rs;
            NCount dims;
            switch(elem.mVType)
            {
            case VT_Pos:
                *pFloat++ = mOpVertex.mPos.x;
                *pFloat++ = mOpVertex.mPos.y;
                *pFloat++ = mOpVertex.mPos.z;
                break;
            case VT_Normals:
                *pFloat++ = mOpVertex.mNorl.x;
                *pFloat++ = mOpVertex.mNorl.y;
                *pFloat++ = mOpVertex.mNorl.z;
                break;
            case VT_Tangent:
                *pFloat++ = mOpVertex.mTangent.x;
                *pFloat++ = mOpVertex.mTangent.y;
                *pFloat++ = mOpVertex.mTangent.z;
                break;
            case VT_Tex_Coord:
                dims = DataTypeUtil::getCount(elem.mType);
                for(NCount t = 0; t < dims; ++t)
                    *pFloat++ = mOpVertex.mCoord[elem.mIndex][t];
                break;
            case VT_Diffuse:
                rs = N_Engine().getRender();
                if(rs)
                    rs->convertType(mOpVertex.mColour, pRGBA++);
                else
                    *pRGBA++ = mOpVertex.mColour.getRGBA(); // pick one!
                break;
            default:
                // nop ?
                break;
            };
        }
    }
    //-----------------------------------------------------------------------------
    GeometryObj * CustomSpaceObj::end()
    {
        if(!mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You cannot call end() until after you call begin()"));
        }
        if(mCacheUpload)
        {
            // bake current vertex
            updateVertexData();
        }

        // pointer that will be returned
        CustomSpaceObjSection * result = NULL;

        GeometryRaw * rop = mOpGeo->getGeometry();
        // Check for empty content
        if(rop->mVertex->mCount == 0 || ((rop->getMark() & GRT_Index) && rop->mIndex->mCount == 0))
        {
            // You're wasting my time sonny
            if(mOpUpdate)
            {
                // Can't just undo / remove since may be in the middle
                // Just allow counts to be 0, will not be issued to renderer

                // return the finished section (though it has zero vertices)
                result = mOpGeo;
            }
            else
            {
                // First creation, can really undo
                // Has already been added to section list end, so remove
                mGeoList.pop_back();
                N_delete mOpGeo;
            }
        }
        else // 不是空区域
        {
            VertexBuffer * vbuf = 0;
            // Check buffer sizes
            bool vbufNeedsCreating = true;
            bool ibufNeedsCreating = (rop->getMark() & GRT_Index);
            // Work out if we require 16 or 32-bit index buffers
            NCount mtmp = mOpGeo->get32BitIndices() ? 32 : 16;
            if(mOpUpdate)
            {
                // May be able to reuse buffers, check sizes
                vbuf = rop->mVertex->getAttach(0);
                if(vbuf->getUnitCount() >= rop->mVertex->mCount)
                    vbufNeedsCreating = false;

                if(rop->getMark() & GRT_Index)
                {
                    if((rop->mIndex->mBuffer->getUnitCount() >= rop->mIndex->mCount) &&
                        (mtmp == rop->mIndex->mBuffer->getUnitSize()))
                        ibufNeedsCreating = false;
                }

            }
            if(vbufNeedsCreating)
            {
                // Make the vertex buffer larger if estimated vertex count higher
                // to allow for user-configured growth area
                NCount vertexCount = std::max(rop->mVertex->mCount, mVertexCount);

                N_Only(GBuffer).create(vbuf, mUnitSize, vertexCount, mOnlyWrite ? Buffer::M_HOSTWRITE : Buffer::M_NORMAL);

                rop->mVertex->attach(0, vbuf);
            }
            if(ibufNeedsCreating)
            {
                IndexBuffer * idxbuf;
                // Make the index buffer larger if estimated index count higher
                // to allow for user-configured growth area
                NCount indexCount = std::max(rop->mIndex->mCount, mIndexCount);

                N_Only(GBuffer).create(idxbuf, mtmp, indexCount, mOnlyWrite ? Buffer::M_HOSTWRITE : (Buffer::M_NORMAL ));

                rop->mIndex->attach(idxbuf);
            }
            // Write vertex data
            vbuf->write(mOpVertexData, 0, rop->mVertex->mCount * vbuf->getUnitSize());
            // Write index data
            if(rop->getMark() & GRT_Index)
            {
                if(mtmp == 32)
                {
                    // direct copy from the mOpIndexData
                    rop->mIndex->mBuffer->write(mOpIndexData, 0,
                        rop->mIndex->mCount * rop->mIndex->mBuffer->getUnitSize());
                }
                else //(mtmp == 16)
                {
                    Nui16 * pIdx = static_cast<Nui16 *>(rop->mIndex->mBuffer->lock(Buffer::MM_WHOLE));
                    Nui32 * pSrc = mOpIndexData;
                    for(NCount i = 0; i < rop->mIndex->mCount; i++)
                    {
                        *pIdx++ = static_cast<Nui16>(*pSrc++);
                    }
                    rop->mIndex->mBuffer->unlock();
                }
            }
            // return the finished section
            result = mOpGeo;
        } // empty section check

        mOpGeo = 0;
        resetCacheData();

        if(mSpaceNode)
        {
            mSpaceNode->notify();
        }

        // will return the finished section or NULL if
        // the section was empty (i.e. zero vertices/indices)
        return result;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::setMaterial(Nidx idx, ResourceID rid, GroupID gid)
    {
        if(idx >= mGeoList.size())
        {
            N_EXCEPT(InvalidParam, _I18n("Index out of bounds!"));
        }

        mGeoList[idx]->setMaterial(rid, gid);
    }
    //-----------------------------------------------------------------------------
    Mesh * CustomSpaceObj::createMesh(ResourceID rid, GroupID gid)
    {
        if(mOpGeo)
        {
            N_EXCEPT(InvalidParam, _I18n("You cannot call createMesh() whilst you are in the middle of defining the object; call end() first."));
        }
        if(mGeoList.empty())
        {
            N_EXCEPT(InvalidParam, _I18n("No data defined to convert to a mesh."));
        }
        Mesh * m = N_Only(Mesh).createManual(rid, gid);

        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            CustomSpaceObjSection * sec = *i;
            GeometryRaw * gr = sec->getGeometry();
            SubMesh * sm = m->createSub();
            sm->setSelfVertex(true);
            sm->setGeometryType(gr->mType);
            sm->setMaterial(sec->getMaterialName());
            // Copy vertex data; replicate buffers too
            sm->setVertexData(gr->mVertex->clone(true));
            // Copy index data; replicate buffers too; delete the default, old one to avoid memory leaks

            // check if index data is present
            if(gr->mIndex)
            {
                // Copy index data; replicate buffers too; delete the default, old one to avoid memory leaks
                N_delete sm->getIndexData(0);
                sm->setIndexData(0, gr->mIndex->clone(true));
            }
        }

        m->setAABB(mAABB);
        m->setRangeFactor(0.01f);
        m->setCenterRange(mRadius);

        m->load();

        return m;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::setIndexOrderRender(bool b)
    {
        mIndexOrderRender = b;
    }
    //-----------------------------------------------------------------------------
    bool CustomSpaceObj::isIndexOrderRender() const
    {
        return mIndexOrderRender;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::setIdentityProj(bool b)
    {
        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            if(b)
                (*i)->on(GeometryObj::M_IdentityProj);
            else
                (*i)->off(GeometryObj::M_IdentityProj);
        }
        mIdentityProj = b;
    }
    //-----------------------------------------------------------------------------
    bool CustomSpaceObj::isIdentityProj() const
    {
        return mIdentityProj;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObj::setIdentityView(bool b)
    {
        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            if(b)
                (*i)->on(GeometryObj::M_IdentityView);
            else
                (*i)->off(GeometryObj::M_IdentityView);
        }
        mIdentityView = b;
    }
    //-----------------------------------------------------------------------
    GeometryObj * CustomSpaceObj::getGeo(Nidx index) const
    {
        if(index >= mGeoList.size())
            N_EXCEPT(InvalidParam, _I18n("Index out of bounds."));
        return mGeoList[index];
    }
    //-----------------------------------------------------------------------
    NCount CustomSpaceObj::getGeoCount() const
    {
        return mGeoList.size();
    }
    //-----------------------------------------------------------------------
    FactoryID CustomSpaceObj::getFactoryID() const
    {
        return N_FACTORY_CustomSpace;
    }
    //-----------------------------------------------------------------------
    const AABox & CustomSpaceObj::getAABB() const
    {
        return mAABB;
    }
    //-----------------------------------------------------------------------
    NIIf CustomSpaceObj::getCenterRange() const
    {
        return mRadius;
    }
    //-----------------------------------------------------------------------
    void CustomSpaceObj::queue(RenderQueue * queue)
    {
        // To be used when order of creation must be kept while rendering
        Nui16 priority = queue->getDefaultLevel();

        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            // Skip empty sections (only happens if non-empty first, then updated)
            GeometryRaw * rop = (*i)->getGeometry();
            if(rop->mVertex->mCount == 0 || ((rop->getMark() & GRT_Index) && rop->mIndex->mCount == 0))
                continue;

            if(mRenderLevel != 0)
            {
                queue->add(*i, mRenderGroup, mRenderLevel);
            }
            else if(mRenderGroup != RQG_Unknow)
            {
                queue->add(*i, mRenderGroup, mIndexOrderRender ? priority++ : queue->getDefaultLevel());
            }
            else
            {
                queue->add(*i, queue->getDefaultGroup(), mIndexOrderRender ? priority++ : queue->getDefaultLevel());
            }
        }
    }
    //------------------------------------------------------------------------
    void CustomSpaceObj::query(GeometryQuery * query)
    {
        CustomGeoObjList::iterator i, iend = mGeoList.end();
        for(i = mGeoList.begin(); i != iend; ++i)
        {
            query->query(*i);
        }
    }
    //-----------------------------------------------------------------------------
    GeoEdget * CustomSpaceObj::getEdgeList()
    {
        // Build on demand
        if(!mEdgeList && mVertexIndex)
        {
            EdgeListBuilder eb;
            NCount vdidx = 0;
            bool anyBuilt = false;
            CustomGeoObjList::iterator i, iend = mGeoList.end();
            for(i = mGeoList.begin(); i != iend; ++i)
            {
                GeometryRaw * gr = (*i)->getGeometry();
                // Only indexed triangle geometry supported for stencil shadows
                if((gr->getMark() & GRT_Index) && gr->mIndex->mCount != 0 &&
                    (gr->mType == GeometryRaw::OT_TRIANGLE_FAN ||
                     gr->mType == GeometryRaw::OT_TRIANGLE_LIST ||
                     gr->mType == GeometryRaw::OT_TRIANGLE_STRIP))
                {
                    eb.addVertex(gr->mVertex);
                    eb.addIndex(gr->mIndex, vdidx++);
                    anyBuilt = true;
                }
            }

            if(anyBuilt)
                mEdgeList = eb.build();
        }
        return mEdgeList;
    }
    //---------------------------------------------------------------------
    bool CustomSpaceObj::isEdgeExist()
    {
        return getEdgeList() != 0;
    }
    //---------------------------------------------------------------------
    const AABox & CustomSpaceObj::getFrontCapAABB() const
    {
        return getSpaceAABB();
    }
    //---------------------------------------------------------------------
    const ShadowVolumeList & CustomSpaceObj::getVolumeList(const Light * srclight,
         NIIf destdis, Nmark flags, IndexBuffer * destindex)
    {
        N_assert(destindex, "Only external index buffers are supported right now");

        GeoEdget * edgeList = getEdgeList();

        if(!edgeList)
        {
            return mShadowVolumeList;
        }

        // Calculate the object space light details
        Vector4f lightPos = srclight->getSpace4();
        Matrix4f world2Obj = mSpaceNode->getSpaceMatrix().inverseAffine();
        lightPos = world2Obj.affine(lightPos);

        // Init shadow renderable list if required (only allow indexed)
        bool init = mShadowVolumeList.empty() && mVertexIndex;

        CustomSpaceObjShadowVolume * esr;

        if(init)
            mShadowVolumeList.resize(edgeList->mGeoList.size());

        GeoEdget::EdgeGroupList::iterator egi = edgeList->mGeoList.begin();
        CustomGeoObjList::iterator seci = mGeoList.begin();
        ShadowVolumeList::iterator si, siend = mShadowVolumeList.end();
        for(si = mShadowVolumeList.begin(); si != siend; ++seci)
        {
            // Skip non-indexed geometry
            if(((*seci)->getGeometry()->getMark() & GRT_Index) == 0)
            {
                continue;
            }

            if(init)
            {
                // Create a new renderable, create a separate light cap if
                // we're using a vertex program (either for this model, or
                // for extruding the shadow volume) since otherwise we can
                // get depth-fighting on the light cap
                Material * mat = const_cast<Material *>((*seci)->getMaterial());
                mat->load();
                bool vertexProgram = false;
                ShaderFusion * t = mat->getLOD(*seci, 0);
                for(NCount p = 0; p < t->getCount(); ++p)
                {
                    ShaderCh * pass = t->get(p);
                    if(pass->getProgram().isExist(ShaderChProgram::ST_VS))
                    {
                        vertexProgram = true;
                        break;
                    }
                }
                *si = N_new CustomSpaceObjShadowVolume(this, destindex, egi->mVertex,
                    vertexProgram || ((flags & ShadowObj::VT_Software) == 0));
            }
            // Get shadow renderable
            esr = static_cast<CustomSpaceObjShadowVolume *>(*si);
            VertexBuffer * esrPositionBuffer = esr->getPositionBuffer();
            // Extrude vertices in software if required
            if(flags & ShadowObj::VT_Software)
            {
                extrude(esrPositionBuffer, egi->mVertex->mCount, lightPos, destdis);
            }

            ++si;
            ++egi;
        }
        // Calc triangle light facing
        edgeList->updateView(lightPos);

        // Generate indexes and update renderables
        generate(edgeList, destindex, srclight, flags, mShadowVolumeList);

        return mShadowVolumeList;
    }
    //-----------------------------------------------------------------------------
    NIIf CustomSpaceObj::getExtrudeDistance(const Light * l) const
    {
        if(mSpaceNode)
        {
            Vector3f diff = mSpaceNode->getSpacePos() - l->getSpacePos();
            return l->getRange() - diff.length();
        }

        return 0;
    }
    //-----------------------------------------------------------------------------
}