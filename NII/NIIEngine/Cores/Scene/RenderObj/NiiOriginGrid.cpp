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
#include "NiiOriginGrid.h"
#include "NiiGBufferManager.h"

#define POSITION_BINDING 0

namespace NII
{
    //------------------------------------------------------------------------
    OriginGrid::OriginGrid(NCount x, NCount y, Nmark bufmark) :
        RenderObj(),
        mXGrid(x),
        mYGrid(y)
    {
        buildGeo(x, y, bufmark);
    }
    //------------------------------------------------------------------------
    OriginGrid::OriginGrid(SpaceID sid, NCount x, NCount y, Nmark bufmark) :
        RenderObj(sid),
        mXGrid(x),
        mYGrid(y)
    {
        buildGeo(x, y, bufmark);
    }
    //------------------------------------------------------------------------
    OriginGrid::~OriginGrid()
    {
        N_delete mRenderOp.mVertex;
    }
    //------------------------------------------------------------------------
    NIIf OriginGrid::distanceSquared(const Camera * cam) const
    {
        (void)cam; return 0;
    }
    //------------------------------------------------------------------------
    NIIf OriginGrid::getCenterRange() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void OriginGrid::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //------------------------------------------------------------------------
    void OriginGrid::buildGeo(NCount x, NCount y, Nmark vBufUsage)
    {
        mGeometryMark |= GeometryObj::M_IdentityProj;
        mGeometryMark |= GeometryObj::M_IdentityView;

        N_Only(GBuffer).create(mRenderOp.mVertex);
        mRenderOp.mIndex = 0;
        mRenderOp.mVertex->mCount = 4;
        mRenderOp.mVertex->mOffset = 0;
        mRenderOp.mType = GeometryRaw::OT_QUADS_STRIP;
        mRenderOp.remove(GRT_Index | GRT_Instancing);

        VertexData * decl = mRenderOp.mVertex;

        decl->add(DT_Float3x, VT_Pos, POSITION_BINDING, 0);

        VertexBuffer * vbuf;

        N_Only(GBuffer).create(vbuf, decl->getUnitSize(POSITION_BINDING), mRenderOp.mVertex->mCount, vBufUsage);

        decl->attach(POSITION_BINDING, vbuf);
    }
    //------------------------------------------------------------------------
}