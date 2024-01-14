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
#include "NiiVertexOffset.h"
#include "NiiGBufferManager.h"
#include "NiiCommon.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexOffset::VertexOffset(Nid id, Nui16 tid) :
        mID(id),
        mTarget(tid),
        mBuffer(0)
    {
    }
    //------------------------------------------------------------------------
    VertexOffset::~VertexOffset()
    {
        if(mBuffer != 0)
        {
            N_delete mBuffer;
            mBuffer = 0;
        }
    }
    //------------------------------------------------------------------------
    Nid VertexOffset::getID() const
    {
        return mID;
    }
    //------------------------------------------------------------------------
    Nui16 VertexOffset::getTarget() const
    {
        return mTarget;
    }
    //------------------------------------------------------------------------
    bool VertexOffset::isNormalsValid() const
    {
        return !mNormalsMap.empty();
    }
    //------------------------------------------------------------------------
    void VertexOffset::add(Nidx index, const Vector3f & offset)
    {
        if(!mNormalsMap.empty())
            N_EXCEPT(InvalidParam, _I18n("Inconsistent calls to add, must include normals always or never"));

        if(offset.lengthSquared() < 1e-6f)
        {
            return;
        }

        mVertexOffsetMap[index] = offset;
        if(mBuffer != 0)
        {
            N_delete mBuffer;
            mBuffer = 0;
        }
    }
    //------------------------------------------------------------------------
    void VertexOffset::add(Nidx index, const Vector3f & offset, const Vector3f & normal)
    {
        if(!mVertexOffsetMap.empty() && mNormalsMap.empty())
            N_EXCEPT(InvalidParam, _I18n("Inconsistent calls to add, must include normals always or never"));

        if(offset.lengthSquared() < 1e-6f && normal.lengthSquared() < 1e-6f)
        {
            return;
        }

        mVertexOffsetMap[index] = offset;
        mNormalsMap[index] = normal;
        if(mBuffer != 0)
        {
            N_delete mBuffer;
            mBuffer = 0;
        }
    }
    //------------------------------------------------------------------------
    void VertexOffset::remove(Nidx index)
    {
        OffsetMap::iterator i = mVertexOffsetMap.find(index);
        if (i != mVertexOffsetMap.end())
        {
            mVertexOffsetMap.erase(i);
            if(mBuffer != 0)
            {
                N_delete mBuffer;
                mBuffer = 0;
            }
        }
        NormalsMap::iterator j = mNormalsMap.find(index);
        if (j != mNormalsMap.end())
        {
            mNormalsMap.erase(j);
        }
    }
    //------------------------------------------------------------------------
    void VertexOffset::clear()
    {
        mVertexOffsetMap.clear();
        mNormalsMap.clear();
        if(mBuffer != 0)
        {
            N_delete mBuffer;
            mBuffer = 0;
        }
    }
    //------------------------------------------------------------------------
    const VertexOffset::OffsetMap & VertexOffset::getVertexOffsets() const
    {
        return mVertexOffsetMap;
    }
    //------------------------------------------------------------------------
    const VertexOffset::NormalsMap & VertexOffset::getNormals() const
    {
        return mNormalsMap;
    }
    //------------------------------------------------------------------------
    VertexBuffer * VertexOffset::getFusion(const VertexData * src) const
    {
        NCount numVertices = src->mCount;

        if(mBuffer == 0)
        {
            // Create buffer
            NCount vertexSize = DataTypeUtil::getSize(DT_Float3x);
            bool normals = !mNormalsMap.empty();
            if(normals)
                vertexSize += DataTypeUtil::getSize(DT_Float3x);

            N_Only(GBuffer).create(mBuffer, vertexSize, numVertices, Buffer::M_NORMAL );
            NIIf * data = static_cast<NIIf *>(mBuffer->lock(Buffer::MM_WHOLE));

            // init - these will be the values used where no pose vertex is included
            memset(data, 0, mBuffer->getSize());
            if(normals)
            {
                // zeroes are fine for positions (deltas), but for normals we need the original
                // mesh normals, since delta normals don't work (re-normalisation would
                // always result in a blended normal even with full pose applied)
                const VertexUnit * norl = src->get(VT_Normals, 0);
                N_assert(norl, "error");
                VertexBuffer * nbuffer = src->getAttach(norl->mSource);

                NIIf * dst = data + 3;
                Nui8 * pSrcBase = (Nui8 *)nbuffer->lock(Buffer::MM_READ);
                NIIf * pSrc;
                norl->getData(pSrcBase, pSrc);
                for(NCount v = 0; v < numVertices; ++v)
                {
                    memcpy(dst, pSrc, sizeof(NIIf)*3);

                    dst += 6;
                    pSrc = (NIIf*)(((NIIb*)pSrc) + nbuffer->getUnitSize());
                }
                nbuffer->unlock();
            }
            // Set each vertex
            OffsetMap::const_iterator v = mVertexOffsetMap.begin();
            NormalsMap::const_iterator n = mNormalsMap.begin();

            NCount numFloatsPerVertex = normals ? 6: 3;

            while(v != mVertexOffsetMap.end())
            {
                // Remember, vertex maps are *sparse* so may have missing entries
                // This is why we skip
                NIIf * dst = data + (numFloatsPerVertex * v->first);
                *dst++ = v->second.x;
                *dst++ = v->second.y;
                *dst++ = v->second.z;
                ++v;
                if(normals)
                {
                    *dst++ = n->second.x;
                    *dst++ = n->second.y;
                    *dst++ = n->second.z;
                    ++n;
                }
            }
            mBuffer->unlock();
        }
        return mBuffer;
    }
    //------------------------------------------------------------------------
    VertexOffset * VertexOffset::clone() const
    {
        VertexOffset * re = N_new VertexOffset(mID, mTarget);
        re->mVertexOffsetMap = mVertexOffsetMap;
        re->mNormalsMap = mNormalsMap;
        // Allow buffer to recreate itself, contents may change anyway
        return re;
    }
    //------------------------------------------------------------------------
}