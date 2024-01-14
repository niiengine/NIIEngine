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
#include "NiiVertexIndexIndirect.h"

namespace NII
{
    typedef map<VertexBuffer *, VertexBuffer *>::type SharedVertexBufferMap; 

    struct GeometryRawOperation
    {
        GeometryRawOperation() : mAsync(0), mBuffer(0){}
        GpuBufferAsync * mAsync;
        VertexBuffer * mBuffer;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GeometryRaw
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    GeometryRaw::GeometryRaw() :
        mType(OT_TRIANGLE_LIST),
        mVertex(0),
        mIndex(0),
        mIndirect(0),
        mOffset(0),
        mCount(0),
        mInstanceOffset(0),
        mInstanceCount(1),
        //mGpuData(0),
        mMark(0),
        mVaoId(0),
        mRenderID(0)
    {
    }
    //------------------------------------------------------------------------
    GeometryRaw::GeometryRaw(VertexData * vert, IndexData * idx, IndirectData * idt) :
        mType(OT_TRIANGLE_LIST),
        mVertex(vert),
        mIndex(idx),
        mIndirect(idt),
        mOffset(0),
        mInstanceOffset(0),
        //mGpuData(0),
        mVaoId(0),
        mRenderID(0)
    {
        if (mIndex)
            mCount = mIndex->getUnitCount();
        else if (mVertex)
            mCount = mVertex->getUnitCount();
        else
            mCount = 0;

        if(idt)
        {
            mMark = GRT_Instancing;
            mInstanceCount 
        }
        if(mIndex)
            mMark = GRT_Global | GRT_Index;
    }
    //------------------------------------------------------------------------
    GeometryRaw::GeometryRaw(const GeometryRaw & o) :
        mType(o.mType),
        mVertex(o.mVertex),
        mIndex(o.mIndex),
        mIndirect(o.mIndirect),
        mOffset(o.mOffset),
        mCount(o.mCount),
        mInstanceOffset(o.mInstanceOffset),
        mInstanceCount(o.mInstanceCount),
        mMark(o.mMark),
        //mGpuData(o.mGpuData),
        mVaoId(o.mVaoId),
        mRenderID(o.mRenderID)
    {
    }
    //------------------------------------------------------------------------
    GeometryRaw::GeometryRaw(const GeometryRaw & o, NCount oft, NCount cnt) :
        mType(o.mType),
        mVertex(o.mVertex),
        mIndex(o.mIndex),
        mIndirect(o.mIndirect),
        mOffset(oft),
        mCount(cnt),
        mInstanceOffset(o.mInstanceOffset),
        mInstanceCount(o.mInstanceCount),
        mMark(o.mMark),
        //mGpuData(o.mGpuData),
        mVaoId(o.mVaoId),
        mRenderID(o.mRenderID)
    {
    }
    //------------------------------------------------------------------------
    GeometryRaw & GeometryRaw::operator =(const GeometryRaw & o)
    {
        mType = o.mType;
        mVertex = o.mVertex;
        mIndex = o.mIndex;
        mIndirect = o.mIndirect;
        mOffset = o.mOffset;
        mCount = o.mCount;
        mInstanceCount = o.mInstanceCount;
        mMark = o.mMark;
        //mGpuData = o.mGpuData;
        mVaoId = o.mVaoId;
        mRenderID = o.mRenderID;
        return *this;
    }
    //------------------------------------------------------------------------
    void GeometryRaw::setVertexRange( NCount oft, NCount count)
    {
        size_t limit;
        if( mIndexBuffer )
        {
            limit = mIndexBuffer->getUnitCount();
        }
        else if( mVertex )
        {
            limit = mVertex->getUnitCount();
        }
        else
        {
            limit = std::numeric_limits<uint32>::max();
        }

        if( oft > limit )
        {
            N_EXCEPT(Internal, "oft must be less or equal than " + N_conv( limit ));
        }

        if( oft + count > limit )
        {
            N_EXCEPT(Internal, "oft + count must be less or equal than " + N_conv( limit ) );
        }

        mOffset = oft;
        mCount = count;
    }
    //-----------------------------------------------------------------------------------
    GeometryRaw * GeometryRaw::clone(BufferModeMark vertexmode, BufferModeMark indexmode, BufferModeMark indirect) const
    {
        assert( vertexmode < 0 || (vertexmode >= M_DEVREAD && vertexmode <= M_CoherentMap) );
        assert( indexmode < 0 || (indexmode >= M_DEVREAD && indexmode <= M_CoherentMap) );

        VertexBuffer * vertexbuf = 0;
        while( mVertex )
        {
            vertexbuf = mVertex->clone(CT_Clone, vertexmode);
        }

        IndexBuffer * indexbuf = 0;
        if(mIndexBuffer)
        {
            indexbuf = mIndexBuffer->clone(CT_Clone, indexmode);
        }
        
        IndirectBuffer * indirectbuf = 0;
        if(mIndirect)
        {
            indirectbuf = mIndirect->clone(CT_Clone, indirect);
        }
        
        GeometryRaw * re = N_new GeometryRaw(vertexbuf, indexbuf, indirectbuf);
        re->mType = mType;
        re->mOffset = mOffset;
        re->mCount = mCount;
        re->mVaoType = mVaoType;
        re->mInstanceOffset = mInstanceOffset;
        re->mInstanceCount = mInstanceCount;
        N_Only(GBuffer).setupVAO(re);

        return re;
    }
    //-----------------------------------------------------------------------------------
    void GeometryRaw::lock(RequestList & rlist, NCount unitOft, NCount unitCount, bool shadowbuf)
    {
        set<VertexBuffer*>::type buflist;
        map<VertexBuffer const *, size_t>::type bufmap;
        
        size_t vtypeCnt[VT_Count];
        memset( vtypeCnt, 0, sizeof( vtypeCnt ) );

        RequestList::iterator itor = rlist.begin();
        RequestList::iterator end  = rlist.end();

        while( itor != end )
        {
            size_t bufferIdx;
            const VertexUnit * unit = getIndex(itor->mVType, bufferIdx, vtypeCnt[itor->mVType - 1]);
            if( !unit )
            {
                N_EXCEPT(Internal, "Cannot find vertextype in GeometryRaw" );
            }
            ++vtypeCnt[itor->mVType - 1];

            BindingList::iterator i = mVertex->getAttachs().begin();
            std::advance(i, bufferIdx);
            VertexBuffer * vb = i->second;

            assert(unitOft < vb->getUnitCount());
            assert(unitOft + unitCount <= vb->getUnitCount());

            itor->mType = unit->mType;
            itor->mOffset = unit->mOffset;
            itor->mBuffer = vb;
            
            if(shadowbuf && vb->getHostData())
            {
                itor->mData = vb->getHostData();
                itor->mData += itor->mOffset + unitOft * vb->getUnitSize();
            }
            else if(buflist.find(vb) == buflist.end())
            {
                if(unitCount == 0)
                    unitCount = vb->getUnitCount() - unitOft;

                N_Only(GBuffer).create(itor->mAsync, vb, unitOft * vb->getUnitSize(), unitCount * vb->getUnitSize());

                itor->mData = reinterpret_cast<const Nui8 *>(itor->mAsync->map());
                itor->mData += itor->mOffset;
                bufmap.insert(Npair(itor->mBuffer, itor - rlist.begin()));
                
                buflist.insert(vb);
            }
            else
            {
                map<VertexBuffer const *, size_t>::type::const_iterator it = bufmap.find(itor->mBuffer);

                assert( it != bufmap.end() && "These rlist are invalid or already been unlock" );

                itor->mData = rlist[it->second].mData - rlist[it->second].mOffset + itor->mOffset;
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
    void GeometryRaw::unlock(RequestList & rlist)
    {
        RequestList::iterator itor = rlist.begin();
        RequestList::iterator end  = rlist.end();

        while( itor != end )
        {
            itor->mData = 0;
            if(itor->mAsync )
            {
                itor->mAsync->unmap();
                itor->mAsync = 0;
            }

            ++itor;
        }
    }
    //-----------------------------------------------------------------------------------
}