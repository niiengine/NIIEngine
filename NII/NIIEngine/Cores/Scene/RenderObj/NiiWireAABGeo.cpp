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
#include "NiiWireAABGeo.h"
#include "NiiRenderObj.h"
#include "NiiGBufferManager.h"
#include "NiiMaterialManager.h"
#include "NiiCamera.h"

namespace NII
{
    #define POSITION_BINDING 0
    //--------------------------------------------------------------------------
    WireAABGeo::WireAABGeo() :
        RenderObj()
    {
        N_Only(GBuffer).create(mRenderOp.mVertex);
        mRenderOp.mIndex = 0;
        mRenderOp.mVertex->mCount = 24;
        mRenderOp.mVertex->mOffset = 0;
        mRenderOp.mType = GeometryRaw::OT_LINE_LIST;
        mRenderOp.remove(GRT_Index | GRT_Instancing);

        VertexData * decl = mRenderOp.mVertex;

        decl->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);

        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, decl->getUnitSize(POSITION_BINDING), mRenderOp.mVertex->mCount, Buffer::M_NORMAL );
        decl->attach(POSITION_BINDING, vbuf);

        // set basic white material
        setMaterial(MaterialManager::WhiteColour->getOriginID());
    }
    //--------------------------------------------------------------------------
    WireAABGeo::WireAABGeo(SpaceID sid) :
        RenderObj(sid)
    {
        N_Only(GBuffer).create(mRenderOp.mVertex);
        mRenderOp.mIndex = 0;
        mRenderOp.mVertex->mCount = 24;
        mRenderOp.mVertex->mOffset = 0;
        mRenderOp.mType = GeometryRaw::OT_LINE_LIST;
        mRenderOp.remove(GRT_Index | GRT_Instancing);

        VertexData * decl = mRenderOp.mVertex;

        decl->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);

        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, decl->getUnitSize(POSITION_BINDING), mRenderOp.mVertex->mCount, Buffer::M_NORMAL );
        decl->attach(POSITION_BINDING, vbuf);

        // set basic white material
        this->setMaterial(MaterialManager::WhiteColour->getOriginID());
    }
    //--------------------------------------------------------------------------
    WireAABGeo::~WireAABGeo()
    {
        N_delete mRenderOp.mVertex;
    }
    //------------------------------------------------------------------------
    void WireAABGeo::setAABBGeo(const AABox & aabb)
    {
        // init the vertices to the aabb
        Vector3f vmax = aabb.getMax();
        Vector3f vmin = aabb.getMin();

        NIIf sqLen = std::max(vmax.lengthSquared(), vmin.lengthSquared());
        mRadius = Math::sqrt(sqLen);

        NIIf maxx = vmax.x;
        NIIf maxy = vmax.y;
        NIIf maxz = vmax.z;

        NIIf minx = vmin.x;
        NIIf miny = vmin.y;
        NIIf minz = vmin.z;

        // fill in the Vertex buffer: 12 lines with 2 endpoints each make up a box
        VertexBuffer * vbuf = mRenderOp.mVertex->getAttach(POSITION_BINDING);

        NIIf * pPos = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));

        // line 0
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = minz;
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = minz;
        // line 1
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = minz;
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = maxz;
        // line 2
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = minz;
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = minz;
        // line 3
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = minz;
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        // line 4
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = minz;
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = minz;
        // line 5
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = minz;
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = maxz;
        // line 6
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = minz;
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = minz;
        // line 7
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        // line 8
        *pPos++ = minx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = maxz;
        // line 9
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = minz;
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        // line 10
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = maxz;
        *pPos++ = maxx;
        *pPos++ = maxy;
        *pPos++ = maxz;
        // line 11
        *pPos++ = minx;
        *pPos++ = miny;
        *pPos++ = maxz;
        *pPos++ = maxx;
        *pPos++ = miny;
        *pPos++ = maxz;
        vbuf->unlock();

        setAABB(aabb);
    }
    //------------------------------------------------------------------------
    // Override this method to prevent parent transforms (rotation,translation,scale)
    void WireAABGeo::getMatrix(Matrix4f * out, NCount & count) const
    {
        // return identity matrix to prevent parent transforms
        *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //-----------------------------------------------------------------------
    NIIf WireAABGeo::distanceSquared(const Camera * cam) const
    {
        Vector3f min, max, mid, dist;
        min = mBox.getMin();
        max = mBox.getMax();
        mid = ((max - min) * 0.5) + min;
        dist = cam->getSpacePos() - mid;

        return dist.lengthSquared();
    }
    //-----------------------------------------------------------------------
}