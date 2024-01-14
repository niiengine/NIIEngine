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
#include "NiiTangentSpace.h"
#include "NiiGBufferManager.h"
#include "NiiLogManager.h"
#include "NiiException.h"

namespace NII
{
    template <typename T> void _remapIndexes(T * ibuf, NCount indexSet, TangentSpace::Result & res)
    {
        for (TangentSpace::IndexRemapList::iterator i = res.indexesRemapped.begin();
            i != res.indexesRemapped.end(); ++i)
        {
            TangentSpace::IndexRemap & remap = *i;

            // Note that because this is a vertex split situation, and vertex
            // split is only for some faces, it's not a case of replacing all
            // instances of vertex index A with vertex index B
            // It actually matters which triangle we're talking about, so drive
            // the update from the face index

            if (remap.indexSet == indexSet)
            {
                T * pBuf;
                pBuf = ibuf + remap.faceIndex * 3;

                for (NIIi v = 0; v < 3; ++v, ++pBuf)
                {
                    if (*pBuf == remap.splitVertex.first)
                    {
                        *pBuf = (T)remap.splitVertex.second;
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    TangentSpace::TangentSpace() :
        mVData(0),
        mSplitMirrored(false),
        mSplitRotated(false),
        mStoreParityInW(false)
    {
    }
    //---------------------------------------------------------------------
    TangentSpace::~TangentSpace()
    {
    }
    //---------------------------------------------------------------------
    void TangentSpace::clear()
    {
        mIDataList.clear();
        mOpTypes.clear();
        mVData = 0;
    }
    //---------------------------------------------------------------------
    void TangentSpace::setVertexData(VertexData * in)
    {
        mVData = in;
    }
    //---------------------------------------------------------------------
    void TangentSpace::addIndexData(IndexData * i_in, GeometryRaw::OperationType op)
    {
        if(op != GeometryRaw::OT_TRIANGLE_FAN &&
            op != GeometryRaw::OT_TRIANGLE_LIST &&
            op != GeometryRaw::OT_TRIANGLE_STRIP)
        {
            N_EXCEPT(InvalidParam, _I18n("Only indexed triangle (list, strip, fan) render operations are supported."));
        }
        mIDataList.push_back(i_in);
        mOpTypes.push_back(op);
    }
    //---------------------------------------------------------------------
    TangentSpace::Result TangentSpace::build(VertexType type, Nui16 index,
        Nui16 texcoordSet)
    {
        Result res;

        // Pull out all the vertex components we'll need
        populateVertexArray(texcoordSet);

        // Now process the faces and calculate / add their contributions
        processFaces(res);

        // Now normalise & orthogonalise
        normaliseVertices();

        // Create new final geometry
        // First extend existing buffers to cope with new vertices
        extendBuffers(res.vertexSplits);

        // Alter indexes
        remapIndexes(res);

        // Create / identify target & write tangents
        insertTangents(res, type, texcoordSet, index);

        return res;
    }
    //---------------------------------------------------------------------
    void TangentSpace::extendBuffers(VertexSplits & vertexSplits)
    {
        if(!vertexSplits.empty())
        {
            // ok, need to increase the vertex buffer size, and alter some indexes

            // vertex buffers first
            VertexData::BindingList newbind;
            const VertexData::BindingList & bindmap = mVData->getAttachs();
            VertexData::BindingList::const_iterator i, iend = bindmap.end();
            for(i = bindmap.begin(); i != iend; ++i)
            {
                VertexBuffer * srcbuf = i->second;
                // Derive vertex count from buffer not vertex data, in case using
                // the mOffset option in vertex data
                NCount newVertexCount = srcbuf->getUnitCount() + vertexSplits.size();
                // Create new buffer & bind
                VertexBuffer * newBuf;
                N_Only(GBuffer).create(newBuf, srcbuf->getUnitSize(), newVertexCount, srcbuf->getModeMark());
                newbind.insert(Npair(i->first, newBuf));

                // Copy existing contents (again, entire buffer, not just elements referenced)
                newBuf->write(srcbuf, 0, 0, srcbuf->getUnitCount() * srcbuf->getUnitSize());

                // Split vertices, read / write from new buffer
                Nui8 * pBase = (Nui8 *)newBuf->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                VertexSplits::iterator spliti, splitiend = vertexSplits.end();
                for(spliti = vertexSplits.begin(); spliti != splitiend; ++spliti)
                {
                    const Nui8 * pSrcBase = pBase + spliti->first * newBuf->getUnitSize();
                    Nui8 * pDstBase = pBase + spliti->second * newBuf->getUnitSize();
                    memcpy(pDstBase, pSrcBase, newBuf->getUnitSize());
                }
                newBuf->unlock();

            }

            // Update vertex data
            // Increase vertex count according to num splits
            mVData->mCount += vertexSplits.size();
            mVData->detachAll();
            mVData->attach(newbind);

            // If vertex size requires 32bit index buffer
            if(mVData->mCount > 65536)
            {
                for(NCount i = 0; i < mIDataList.size(); ++i)
                {
                    // check index size
                    IndexData * idata = mIDataList[i];
                    IndexBuffer * srcbuf = idata->mBuffer;
                    if(srcbuf->getUnitSize() == 16)
                    {
                        NCount indexCount = srcbuf->getUnitCount();

                        // convert index buffer to 32bit.
                        IndexBuffer * newBuf;
                        N_Only(GBuffer).create(newBuf, 32, indexCount, srcbuf->getModeMark());

                        Nui16 * pSrcBase = (Nui16 *)srcbuf->lock(Buffer::MM_READ);
                        Nui32 * pBase = (Nui32 *)newBuf->lock(Buffer::MM_WRITE);

                        NCount j = 0;
                        while(j < indexCount)
                        {
                            *pBase++ = *pSrcBase++;
                            ++j;
                        }

                        srcbuf->unlock();
                        newBuf->unlock();

                        // assign new index buffer.
                        idata->mBuffer = newBuf;
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void TangentSpace::remapIndexes(Result & res)
    {
        for(NCount i = 0; i < mIDataList.size(); ++i)
        {
            IndexData * idata = mIDataList[i];
            // Now do index data
            // no new buffer required, same size but some triangles remapped
            if(idata->mBuffer->getUnitSize() == 32)
            {
                Nui32 * p32 = (Nui32 *)idata->mBuffer->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                _remapIndexes(p32, i, res);
            }
            else
            {
                Nui16 * p16 = (Nui16 *)idata->mBuffer->lock(Buffer::MM_READ | Buffer::MM_WRITE);
                _remapIndexes(p16, i, res);
            }
            idata->mBuffer->unlock();
        }
    }
    //---------------------------------------------------------------------
    void TangentSpace::normaliseVertices()
    {
        // Just run through our complete (possibly augmented) list of vertices
        // Normalise the tangents & binormals
        for (VertexInfoArray::iterator i = mVertexArray.begin(); i != mVertexArray.end(); ++i)
        {
            VertexInfo& v = *i;

            v.tangent.normalise();
            v.binormal.normalise();

            // Orthogonalise with the vertex normal since it's currently
            // orthogonal with the face normals, but will be close to ortho
            // Apply Gram-Schmidt orthogonalise
            Vector3f temp = v.tangent;
            v.tangent = temp - (v.norm * v.norm.dotProduct(temp));

            temp = v.binormal;
            v.binormal = temp - (v.norm * v.norm.dotProduct(temp));

            // renormalize
            v.tangent.normalise();
            v.binormal.normalise();
        }
    }
    //---------------------------------------------------------------------
    void TangentSpace::processFaces(Result& result)
    {
        // Quick pre-check for triangle strips / fans
        for (OpTypeList::iterator ot = mOpTypes.begin(); ot != mOpTypes.end(); ++ot)
        {
            if (*ot != GeometryRaw::OT_TRIANGLE_LIST)
            {
                // Can't split strips / fans
                setSplitMirrored(false);
                setSplitRotated(false);
            }
        }

        for(NCount i = 0; i < mIDataList.size(); ++i)
        {
            IndexData * i_in = mIDataList[i];
            GeometryRaw::OperationType opType = mOpTypes[i];

            // Read data from buffers
            Nui16 * p16 = 0;
            Nui32 * p32 = 0;

            IndexBuffer * ibuf = i_in->mBuffer;
            if(ibuf->getUnitSize() == 32)
            {
                p32 = (Nui32 *)ibuf->lock(Buffer::MM_READ);
                // offset by index start
                p32 += i_in->mOffset;
            }
            else
            {
                p16 = (Nui16*)ibuf->lock(Buffer::MM_READ);
                // offset by index start
                p16 += i_in->mOffset;
            }
            // current triangle
            NCount vertInd[3] = { 0, 0, 0 };
            // loop through all faces to calculate the tangents and normals
            NCount faceCount = opType == GeometryRaw::OT_TRIANGLE_LIST ?
                i_in->mCount / 3 : i_in->mCount - 2;
            for (NCount f = 0; f < faceCount; ++f)
            {
                bool invertOrdering = false;
                // Read 1 or 3 indexes depending on type
                if (f == 0 || opType == GeometryRaw::OT_TRIANGLE_LIST)
                {
                    vertInd[0] = p32? *p32++ : *p16++;
                    vertInd[1] = p32? *p32++ : *p16++;
                    vertInd[2] = p32? *p32++ : *p16++;
                }
                else if (opType == GeometryRaw::OT_TRIANGLE_FAN)
                {
                    // Element 0 always remains the same
                    // Element 2 becomes element 1
                    vertInd[1] = vertInd[2];
                    // read new into element 2
                    vertInd[2] = p32? *p32++ : *p16++;
                }
                else if (opType == GeometryRaw::OT_TRIANGLE_STRIP)
                {
                    // Shunt everything down one, but also invert the ordering on
                    // odd numbered triangles (== even numbered i's)
                    // we interpret front as anticlockwise all the time but strips alternate
                    if (f & 0x1)
                    {
                        // odd tris (index starts at 3, 5, 7)
                        invertOrdering = true;
                    }
                    vertInd[0] = vertInd[1];
                    vertInd[1] = vertInd[2];
                    vertInd[2] = p32? *p32++ : *p16++;
                }

                // deal with strip inversion of winding
                NCount localVertInd[3];
                localVertInd[0] = vertInd[0];
                if (invertOrdering)
                {
                    localVertInd[1] = vertInd[2];
                    localVertInd[2] = vertInd[1];
                }
                else
                {
                    localVertInd[1] = vertInd[1];
                    localVertInd[2] = vertInd[2];
                }

                // For each triangle
                //   Calculate tangent & binormal per triangle
                //   Note these are not normalised, are weighted by UV area
                Vector3f faceTsU, faceTsV, faceNorm;
                calculateFaceTangentSpace(localVertInd, faceTsU, faceTsV, faceNorm);

                // Skip invalid UV space triangles
                NIIf ftusl = faceTsU.lengthSquared();
                NIIf ftvsl = faceTsV.lengthSquared();
                if(ftusl < 0.000000000001 || ftvsl < 0.000000000001)
                    continue;

                addFaceTangentSpaceToVertices(i, f, localVertInd, faceTsU, faceTsV, faceNorm, result);
            }
            ibuf->unlock();
        }
    }
    //---------------------------------------------------------------------
    void TangentSpace::addFaceTangentSpaceToVertices(NCount indexSet, NCount faceIndex,
        NCount * localVertInd, const Vector3f & faceTsU, const Vector3f & faceTsV,
        const Vector3f & faceNorm, Result & result)
    {
        // Calculate parity for this triangle
        NIIi faceParity = calculateParity(faceTsU, faceTsV, faceNorm);
        // Now add these to each vertex referenced by the face
        for(NIIi v = 0; v < 3; ++v)
        {
            // index 0 is vertex we're calculating, 1 and 2 are the others

            // We want to re-weight these by the angle the face makes with the vertex
            // in order to obtain tesselation-independent results
            NIIf angleWeight = calculateAngleWeight(localVertInd[v],
                localVertInd[(v+1)%3], localVertInd[(v+2)%3]);

            VertexInfo * vertex = &(mVertexArray[localVertInd[v]]);

            // check parity (0 means not set)
            // Locate parity-version of vertex index, or create if doesn't exist
            // If parity-version of vertex index was different, record alteration
            // in triangle remap
            // in vertex split list
            bool splitVertex = false;
            NCount reusedOppositeParity = 0;
            bool splitBecauseOfParity = false;
            bool newVertex = false;
            if(!vertex->parity)
            {
                // init
                vertex->parity = faceParity;
                newVertex = true;
            }
            if(mSplitMirrored)
            {
                if(!newVertex && faceParity != calculateParity(vertex->tangent,
                    vertex->binormal, vertex->norm))//vertex->parity != faceParity)
                {
                    // Check for existing alternative parity
                    if(vertex->oppositeParityIndex)
                    {
                        // Ok, have already split this vertex because of parity
                        // Use the same one again
                        reusedOppositeParity = vertex->oppositeParityIndex;
                        vertex = &(mVertexArray[reusedOppositeParity]);
                    }
                    else
                    {
                        splitVertex = true;
                        splitBecauseOfParity = true;

                        N_Only(Log).stream(LL_Trace)
                            << "TSC parity split - Vpar: " << vertex->parity
                            << " Fpar: " << faceParity
                            << " faceTsU: " << faceTsU
                            << " faceTsV: " << faceTsV
                            << " faceNorm: " << faceNorm
                            << " vertTsU:" << vertex->tangent
                            << " vertTsV:" << vertex->binormal
                            << " vertNorm:" << vertex->norm;
                    }
                }
            }
            if(mSplitRotated)
            {
                // deal with excessive tangent space rotations as well as mirroring
                // same kind of split behaviour appropriate
                if(!newVertex && !splitVertex)
                {
                    // If more than 90 degrees, split
                    Vector3f uvCurrent = vertex->tangent + vertex->binormal;

                    // project down to the plane (plane normal = face normal)
                    Vector3f vRotHalf = uvCurrent - faceNorm;
                    vRotHalf *= faceNorm.dotProduct(uvCurrent);

                    if((faceTsU + faceTsV).dotProduct(vRotHalf) < 0.0f)
                    {
                        splitVertex = true;
                    }
                }
            }
            if(splitVertex)
            {
                NCount newVertexIndex = mVertexArray.size();
                VertexSplit splitInfo(localVertInd[v], newVertexIndex);
                result.vertexSplits.push_back(splitInfo);
                // re-point opposite parity
                if(splitBecauseOfParity)
                {
                    vertex->oppositeParityIndex = newVertexIndex;
                }
                // copy old values but reset tangent space
                VertexInfo newVertex = *vertex;
                newVertex.tangent = Vector3f::ZERO;
                newVertex.binormal = Vector3f::ZERO;
                newVertex.parity = faceParity;
                mVertexArray.push_back(newVertex);
                result.indexesRemapped.push_back(IndexRemap(indexSet, faceIndex, splitInfo));

                vertex = &(mVertexArray[newVertexIndex]);
            }
            else if (reusedOppositeParity)
            {
                // didn't split again, but we do need to record the re-used remapping
                VertexSplit splitInfo(localVertInd[v], reusedOppositeParity);
                result.indexesRemapped.push_back(IndexRemap(indexSet, faceIndex, splitInfo));

            }
            // Add weighted tangent & binormal
            vertex->tangent += (faceTsU * angleWeight);
            vertex->binormal += (faceTsV * angleWeight);
        }
    }
    //---------------------------------------------------------------------
    NIIi TangentSpace::calculateParity(const Vector3f & u, const Vector3f & v,
        const Vector3f & n)
    {
        // Note that this parity is the reverse of what you'd expect - this is
        // because the 'V' texture coordinate is actually left handed
        if (u.crossProduct(v).dotProduct(n) >= 0.0f)
            return -1;
        else
            return 1;
    }
    //---------------------------------------------------------------------
    void TangentSpace::calculateFaceTangentSpace(const NCount * vertInd, 
        Vector3f & tsU, Vector3f & tsV, Vector3f & tsN)
    {
        const VertexInfo& v0 = mVertexArray[vertInd[0]];
        const VertexInfo& v1 = mVertexArray[vertInd[1]];
        const VertexInfo& v2 = mVertexArray[vertInd[2]];
        Vector2f deltaUV1 = v1.uv - v0.uv;
        Vector2f deltaUV2 = v2.uv - v0.uv;
        Vector3f deltaPos1 = v1.pos - v0.pos;
        Vector3f deltaPos2 = v2.pos - v0.pos;

        // face normal
        tsN = deltaPos1.crossProduct(deltaPos2);
        tsN.normalise();

        NIIf uvarea = deltaUV1.crossProduct(deltaUV2) * 0.5f;
        if (Math::isEqual(uvarea, 0.0f))
        {
            // no tangent, null uv area
            tsU = tsV = Vector3f::ZERO;
        }
        else
        {
            // Normalise by uvarea
            NIIf a = deltaUV2.y / uvarea;
            NIIf b = -deltaUV1.y / uvarea;
            NIIf c = -deltaUV2.x / uvarea;
            NIIf d = deltaUV1.x / uvarea;

            tsU = (deltaPos1 * a) + (deltaPos2 * b);
            tsU.normalise();

            tsV = (deltaPos1 * c) + (deltaPos2 * d);
            tsV.normalise();

            NIIf abs_uvarea = Math::Abs(uvarea);
            tsU *= abs_uvarea;
            tsV *= abs_uvarea;

            // tangent (tsU) and binormal (tsV) are now weighted by uv area
        }
    }
    //---------------------------------------------------------------------
    NIIf TangentSpace::calculateAngleWeight(NCount vidx0, NCount vidx1, NCount vidx2)
    {
        const VertexInfo & v0 = mVertexArray[vidx0];
        const VertexInfo & v1 = mVertexArray[vidx1];
        const VertexInfo & v2 = mVertexArray[vidx2];

        Vector3f diff0 = v1.pos - v0.pos;
        Vector3f diff1 = v2.pos - v1.pos;

        // Weight is just the angle - larger == better
        return diff0.angle(diff1).getValue();
    }
    //---------------------------------------------------------------------
    void TangentSpace::populateVertexArray(Nui16 sourceTexCoordSet)
    {
        // Get the incoming UV element
        const VertexUnit * uvElem = mVData->get(VT_Tex_Coord, sourceTexCoordSet);

        if(!uvElem || uvElem->mType != DT_Float2x)
        {
            N_EXCEPT(InvalidParam, _I18n("计算切线需要二维纹理坐标."));
        }

        VertexBuffer * uvBuf = 0, *posBuf = 0, *normBuf = 0;
        Nui8 *pUvBase, *pPosBase, *pNormBase;
        NCount uvInc, posInc, normInc;

        uvBuf = mVData->getAttach(uvElem->mSource);
        pUvBase = (Nui8 *)uvBuf->lock(Buffer::MM_READ);
        uvInc = uvBuf->getUnitSize();
        // offset for vertex start
        pUvBase += mVData->mOffset * uvInc;

        // find position
        const VertexUnit * posElem = mVData->get(VT_Pos);
        if(posElem->mSource == uvElem->mSource)
        {
            pPosBase = pUvBase;
            posInc = uvInc;
        }
        else
        {
            // A different buffer
            posBuf = mVData->getAttach(posElem->mSource);
            pPosBase = (Nui8 *)posBuf->lock(Buffer::MM_READ);
            posInc = posBuf->getUnitSize();
            // offset for vertex start
            pPosBase += mVData->mOffset * posInc;
        }
        // find a normal buffer
        const VertexUnit * normElem = mVData->get(VT_Normals);
        if(!normElem)
            N_EXCEPT(NotExist, _I18n("No vertex normals found"));

        if(normElem->mSource == uvElem->mSource)
        {
            pNormBase = pUvBase;
            normInc = uvInc;
        }
        else if (normElem->mSource == posElem->mSource)
        {
            // normals are in the same buffer as position
            // this condition arises when an animated(skeleton) mesh is not built with
            // an edge list buffer ie no shadows being used.
            pNormBase = pPosBase;
            normInc = posInc;
        }
        else
        {
            // A different buffer
            normBuf = mVData->getAttach(normElem->mSource);
            pNormBase = (Nui8 *)normBuf->lock(Buffer::MM_READ);
            normInc = normBuf->getUnitSize();
            // offset for vertex start
            pNormBase += mVData->mOffset * normInc;
        }

        // Preinitialise vertex info
        mVertexArray.clear();
        mVertexArray.resize(mVData->mCount);

        NIIf * pFloat;
        VertexInfo * vInfo = &(mVertexArray[0]);
        for(NCount v = 0; v < mVData->mCount; ++v, ++vInfo)
        {
            posElem->getData(pPosBase, pFloat);
            vInfo->pos.x = *pFloat++;
            vInfo->pos.y = *pFloat++;
            vInfo->pos.z = *pFloat++;
            pPosBase += posInc;

            normElem->getData(pNormBase, pFloat);
            vInfo->norm.x = *pFloat++;
            vInfo->norm.y = *pFloat++;
            vInfo->norm.z = *pFloat++;
            pNormBase += normInc;

            uvElem->getData(pUvBase, pFloat);
            vInfo->uv.x = *pFloat++;
            vInfo->uv.y = *pFloat++;
            pUvBase += uvInc;
        }
        // unlock buffers
        uvBuf->unlock();
        if(posBuf)
        {
            posBuf->unlock();
        }
        if(normBuf)
        {
            normBuf->unlock();
        }
    }
    //---------------------------------------------------------------------
    void TangentSpace::insertTangents(Result & res, VertexType targetSemantic,
        Nui16 sourceTexCoordSet, Nui16 index)
    {
        const VertexUnit * tangentsElem = mVData->get(targetSemantic, index);
        bool needsToBeCreated = false;
        DataType tangentsType = mStoreParityInW ? DT_Float4x : DT_Float3x;

        if(!tangentsElem)
        { // no tex coords with index 1
            needsToBeCreated = true ;
        }
        else if(tangentsElem->mType != tangentsType)
        {
            N_EXCEPT(InvalidParam,
                _I18n("Target semantic set already exists but is not of the right size, therefore ")
                    _I18n("cannot contain tangents. You should delete this existing entry first. "));
        }

        VertexBuffer * targetBuffer, *origBuffer;
        Nui8 * pSrc = NULL;

        if(needsToBeCreated)
        {
            // To be most efficient with our vertex streams,
            // tack the new tangents onto the same buffer as the
            // source texture coord set
            const VertexUnit * prevTexCoordElem = mVData->get(VT_Tex_Coord, sourceTexCoordSet);
            if(!prevTexCoordElem)
            {
                N_EXCEPT(NotExist,
                    _I18n("Cannot locate the first texture coordinate element to ")
                        _I18n("which to append the new tangents."));
            }
            // Find the buffer associated with  this element
            origBuffer = mVData->getAttach(prevTexCoordElem->mSource);
            // Now create a new buffer, which includes the previous contents
            // plus extra space for the 3D coords

            N_Only(GBuffer).create(targetBuffer,
                origBuffer->getUnitSize() + DataTypeUtil::getSize(tangentsType),
                    origBuffer->getUnitCount(), origBuffer->getModeMark());
            // Add the new element
            tangentsElem = mVData->add(tangentsType, targetSemantic, 
                prevTexCoordElem->mSource, origBuffer->getUnitSize(), index);
            // Set up the source pointer
            pSrc = (Nui8 *)origBuffer->lock(Buffer::MM_READ);
            // Rebind the new buffer
            mVData->attach(prevTexCoordElem->mSource, targetBuffer);
        }
        else
        {
            origBuffer = mVData->getAttach(tangentsElem->mSource);
            targetBuffer = origBuffer;
        }

        Nui8 * pDest = (Nui8 *)targetBuffer->lock(Buffer::MM_WHOLE);
        NCount origVertSize = origBuffer->getUnitSize();
        NCount newVertSize = targetBuffer->getUnitSize();
        for (NCount v = 0; v < origBuffer->getUnitCount(); ++v)
        {
            if (needsToBeCreated)
            {
                // Copy original vertex data as well
                memcpy(pDest, pSrc, origVertSize);
                pSrc += origVertSize;
            }
            // Write in the tangent
            NIIf * pTangent;
            tangentsElem->getData(pDest, pTangent);
            VertexInfo & vertInfo = mVertexArray[v];
            *pTangent++ = vertInfo.tangent.x;
            *pTangent++ = vertInfo.tangent.y;
            *pTangent++ = vertInfo.tangent.z;
            if(mStoreParityInW)
                *pTangent++ = (NIIf)vertInfo.parity;

            // Next target vertex
            pDest += newVertSize;
        }
        targetBuffer->unlock();

        if(needsToBeCreated)
        {
            origBuffer->unlock();
        }
    }
    //---------------------------------------------------------------------
}