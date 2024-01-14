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
#include "NiiViewRectGeo.h"
#include "NiiRenderObj.h"
#include "NiiGBufferManager.h"
#include "NiiMaterialManager.h"
#include "NiiCamera.h"

namespace NII
{
    /**
        RQG_Bg and 'depth_write off' for backdrops, and
        RQG_Overlay and 'depth_check off' for fullscreen quads.
    */
    #define POSITION_BINDING 0
    #define NORMAL_BINDING 1
    #define TEXCOORD_BINDING 2
    //------------------------------------------------------------------------
    RectGeo::RectGeo(bool texcoord, Nmark bufmark) :
        RenderObj()
    {
        buildGeo(texcoord, bufmark);
    }
    //------------------------------------------------------------------------
    RectGeo::RectGeo(SpaceID sid, bool texcoord, Nmark bufmark) :
        RenderObj(sid)
    {
        buildGeo(texcoord, bufmark);
    }
    //------------------------------------------------------------------------
    void RectGeo::buildGeo(bool texcoord, Nmark vBufUsage)
    {
        mGeometryMark |= GeometryObj::M_IdentityProj;
        mGeometryMark |= GeometryObj::M_IdentityView;

        N_Only(GBuffer).create(mRenderOp.mVertex);
        mRenderOp.mIndex = 0;
        mRenderOp.mVertex->mCount = 4;
        mRenderOp.mVertex->mOffset = 0;
        mRenderOp.mType = GeometryRaw::OT_TRIANGLE_STRIP;
        mRenderOp.remove(GRT_Index | GRT_Instancing);

        VertexData * decl = mRenderOp.mVertex;

        decl->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);

        VertexBuffer * vbuf;

        N_Only(GBuffer).create(vbuf, decl->getUnitSize(POSITION_BINDING), mRenderOp.mVertex->mCount, vBufUsage);

        decl->attach(POSITION_BINDING, vbuf);

        decl->add(DT_Float3x, VT_Normals, NORMAL_BINDING, 0);

        N_Only(GBuffer).create(vbuf, decl->getUnitSize(NORMAL_BINDING), mRenderOp.mVertex->mCount, vBufUsage);

        decl->attach(NORMAL_BINDING, vbuf);

        NIIf *pNorm = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        *pNorm++ = 0.0f;
        *pNorm++ = 0.0f;
        *pNorm++ = 1.0f;

        *pNorm++ = 0.0f;
        *pNorm++ = 0.0f;
        *pNorm++ = 1.0f;

        *pNorm++ = 0.0f;
        *pNorm++ = 0.0f;
        *pNorm++ = 1.0f;

        *pNorm++ = 0.0f;
        *pNorm++ = 0.0f;
        *pNorm++ = 1.0f;

        vbuf->unlock();

        if(texcoord)
        {
            decl->add(DT_Float2x, VT_Tex_Coord, TEXCOORD_BINDING, 0);

            VertexBuffer * tvbuf;
            N_Only(GBuffer).create(tvbuf, decl->getUnitSize(TEXCOORD_BINDING), mRenderOp.mVertex->mCount, vBufUsage);

            // Bind buffer
            decl->attach(TEXCOORD_BINDING, tvbuf);

            // Set up basic tex coordinates
            setTexCoord(Vector2f::ZERO, Vector2f::Y, Vector2f::X, Vector2f::UNIT);
        }

        // set basic white material
        this->setMaterial(MaterialManager::WhiteColour->getOriginID());
    }
    //------------------------------------------------------------------------
    RectGeo::~RectGeo()
    {
        N_delete mRenderOp.mVertex;
    }
    //------------------------------------------------------------------------
    void RectGeo::setRect(NIIf left, NIIf top, NIIf right, NIIf bottom, bool updateAABB)
    {
        VertexBuffer * vbuf = mRenderOp.mVertex->getAttach(POSITION_BINDING);
        NIIf * pFloat = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);

        *pFloat++ = left;
        *pFloat++ = top;
        *pFloat++ = -1;

        *pFloat++ = left;
        *pFloat++ = bottom;
        *pFloat++ = -1;

        *pFloat++ = right;
        *pFloat++ = top;
        *pFloat++ = -1;

        *pFloat++ = right;
        *pFloat++ = bottom;
        *pFloat++ = -1;

        vbuf->unlock();

        if(updateAABB)
        {
            mBox.mMinimum.x = std::min(left, right);
            mBox.mMinimum.y = std::min(top, bottom);
            mBox.mMinimum.z = 0;

            mBox.mMaximum.x = std::max(left, right);
            mBox.mMaximum.y = std::max(top, bottom);
            mBox.mMaximum.z = 0;
        }
    }
    //------------------------------------------------------------------------
    void RectGeo::setNormals(const Vector3f & tl, const Vector3f & bl,
        const Vector3f & tr, const Vector3f & br)
    {
        VertexBuffer * vbuf = mRenderOp.mVertex->getAttach(NORMAL_BINDING);
        NIIf * pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

        *pFloat++ = tl.x;
        *pFloat++ = tl.y;
        *pFloat++ = tl.z;

        *pFloat++ = bl.x;
        *pFloat++ = bl.y;
        *pFloat++ = bl.z;

        *pFloat++ = tr.x;
        *pFloat++ = tr.y;
        *pFloat++ = tr.z;

        *pFloat++ = br.x;
        *pFloat++ = br.y;
        *pFloat++ = br.z;

        vbuf->unlock();
    }
    //------------------------------------------------------------------------
    void RectGeo::setTexCoord(const Vector2f & tl, const Vector2f & bl,
        const Vector2f & tr, const Vector2f & br)
    {
        if(mRenderOp.mVertex->getCount() <= TEXCOORD_BINDING)
            return; //Vertex data wasn't built with UV buffer

        VertexBuffer * vbuf = mRenderOp.mVertex->getAttach(TEXCOORD_BINDING);
        NIIf * pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

        *pFloat++ = tl.x;
        *pFloat++ = tl.y;

        *pFloat++ = bl.x;
        *pFloat++ = bl.y;

        *pFloat++ = tr.x;
        *pFloat++ = tr.y;

        *pFloat++ = br.x;
        *pFloat++ = br.y;

        vbuf->unlock();
    }
    //------------------------------------------------------------------------
    NIIf RectGeo::distanceSquared(const Camera * cam) const
    {
        (void)cam; 
        return 0;
    }
    //------------------------------------------------------------------------
    NIIf RectGeo::getCenterRange() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void RectGeo::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //------------------------------------------------------------------------
}