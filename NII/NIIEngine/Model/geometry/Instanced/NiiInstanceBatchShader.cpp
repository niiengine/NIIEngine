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
#include "NiiInstanceBatchShader.h"
#include "NiiGBufferManager.h"
#include "NiiInstanceGeo.h"
#include "NiiMaterial.h"
#include "NiiVertexIndexIndirect.h"
#include "NiiLogManager.h"
#include "NiiInstanceMesh.h"

namespace NII
{
    //-----------------------------------------------------------------------
    InstanceBatchShader::InstanceBatchShader(InstanceMesh * creator, Mesh * mesh, 
        Material * material, NCount count, const BoneIndexList * bonemap, Nid bid) :
            InstanceBatch(creator, mesh, material, count, bonemap, bid),
            mNumWorldMatrices(count)
    {
    }
    //-----------------------------------------------------------------------
    InstanceBatchShader::~InstanceBatchShader()
    {
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchShader::calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const
    {
        NCount numBones = std::max<NCount>(1, baseSubMesh->getMatrixList().size());

        mMaterial->load();
        ShaderFusion * technique = mMaterial->getLOD();
        if(technique)
        {
            GpuProgramParam * vertexParam;
            technique->get(0)->getProgram().getParam(ShaderChProgram::ST_VS, vertexParam);
            GpuParamUnitList::const_iterator i, iend = vertexParam->getDefineList().end();
            for(i = vertexParam->getDefineList().begin(); i != iend; ++i)
            {
                const GpuParamUnit & constDef = i->second;
                if((constDef.mDataType == GDT_Matrix3X4 ||
                    constDef.mDataType == GDT_Matrix4X3 ||        //OGL GLSL bitches without this
                    constDef.mDataType == GDT_Matrix2X4 ||
                    constDef.mDataType == GDT_Float4X)            //OGL GLSL bitches without this
                    && (constDef.isFloat()))
                {
                    const GpuParamBlock * entry = vertexParam->_getSyncParamBlock(constDef.mMemIndex);
                    if(entry && (entry->mSyncParam == GSP_Matrix3x4_Space_Array || entry->mSyncParam == GSP_DualQuaternion_Space))
                    {
                        //Material is correctly done!
                        NCount arraySize = constDef.mUnitCount;

                        //Deal with GL "hacky" way of doing 4x3 matrices
                        if(entry->mSyncParam == GSP_Matrix3x4_Space_Array && constDef.mDataType == GDT_Float4X)
                            arraySize /= 3;
                        else if(entry->mSyncParam == GSP_DualQuaternion_Space && constDef.mDataType == GDT_Float4X)
                            arraySize /= 2;

                        //Check the num of arrays
                        NCount retVal = arraySize / numBones;

                        if(flags & IM_USE16BIT)
                        {
                            if(baseSubMesh->getVertexData()->mCount * retVal > 0xFFFF)
                                retVal = 0xFFFF / baseSubMesh->getVertexData()->mCount;
                        }

                        if((retVal < 3 && entry->mSyncParam == GSP_Matrix3x4_Space_Array) ||
                            (retVal < 2 && entry->mSyncParam == GSP_DualQuaternion_Space))
                        {
                            N_Only(Log).log(
                                _I18n("InstanceBatchShader: Mesh ") + N_conv(mMeshReference->getOriginID()) +
                                _I18n(" using material ") + N_conv(mMaterial->getOriginID()) +
                                _I18n(" contains many bones. The amount of "
                                "instances per batch is very low. Performance benefits will "
                                "be minimal, if any. It might be even slower!"), LL_Info);
                        }
                        return retVal;
                    }
                }
            }

            //Reaching here means material is supported, but malformed
            N_EXCEPT(InvalidParam, _I18n("Material '") + N_conv(mMaterial->getOriginID()) +
                _I18n("' is malformed for this instancing technique"));
        }

        //Reaching here the material is just unsupported.

        return 0;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & ro)
    {
        if(mMeshReference->isSkeletonFusion() && mMeshReference->getSkeleton())
            mNumWorldMatrices = mInstancesPerBatch * baseSubMesh->getMatrixList().size();
        InstanceBatch::buildFrom(baseSubMesh, ro);
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupVertices(const SubMesh * baseSubMesh)
    {
        N_Only(GBuffer).create(mRenderOperation.mVertex);
        mRemoveOwnVertexData = true; //Raise flag to remove our own vertex data in the end (not always needed)

        VertexData * thisVertexData = mRenderOperation.mVertex;
        VertexData * baseVertexData = baseSubMesh->getVertexData();

        thisVertexData->mOffset = 0;
        thisVertexData->mCount = baseVertexData->mCount * mInstancesPerBatch;
        thisVertexData->copy(baseVertexData);

        if(mMeshReference->isSkeletonFusion() && mMeshReference->getSkeleton())
        {
            //Building hw skinned batches follow a different path
            setupHardwareSkinned(baseSubMesh, thisVertexData, baseVertexData);
            return;
        }

        //TODO: Can't we, instead of using another source, put the index ID in the same source?
        thisVertexData->add(DT_UByte4x, VT_Matrix_Index, thisVertexData->getFreeBufferIndex(), 0);

        for(Nui16 i = 0; i < thisVertexData->getFreeBufferIndex() - 1; ++i)
        {
            //Create our own vertex buffer
            VertexBuffer * vBuffer;
            N_Only(GBuffer).create(vBuffer, thisVertexData->getUnitSize(i), thisVertexData->mCount, Buffer::M_NORMAL );

            thisVertexData->attach(i, vBuffer);

            //Grab the base submesh data
            VertexBuffer * bvBuffer = baseVertexData->getAttach(i);

            Nui8 * thisBuf = (Nui8*)vBuffer->lock(Buffer::MM_WHOLE);
            Nui8 * baseBuf = (Nui8*)bvBuffer->lock(Buffer::MM_READ);

            //Copy and repeat
            for(NCount j = 0; j < mInstancesPerBatch; ++j)
            {
                NCount sizeOfBuffer = baseVertexData->mCount * baseVertexData->getUnitSize(i);
                memcpy(thisBuf + j * sizeOfBuffer, baseBuf, sizeOfBuffer );
            }

            bvBuffer->unlock();
            vBuffer->unlock();
        }

        {
            //Now create the vertices "index ID" to individualize each instance
            Nui16 lastSource = thisVertexData->getFreeBufferIndex() - 1;
            VertexBuffer * vBuffer;
            N_Only(GBuffer).create(vBuffer, thisVertexData->getUnitSize(lastSource), thisVertexData->mCount, Buffer::M_NORMAL );

            thisVertexData->attach(lastSource, vBuffer);

            Nui8 * thisBuf = (Nui8*)vBuffer->lock(Buffer::MM_WHOLE);
            for(NCount j = 0; j < mInstancesPerBatch; ++j)
            {
                for(NCount k = 0; k < baseVertexData->mCount; ++k)
                {
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                    *thisBuf++ = j;
                }
            }
            vBuffer->unlock();
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupIndices(const SubMesh * baseSubMesh)
    {
        mRenderOperation.mIndex = N_new IndexData();
        mRemoveOwnIndexData = true;    //Raise flag to remove our own index data in the end (not always needed)

        IndexData * thisIndexData = mRenderOperation.mIndex;
        IndexData * baseIndexData = baseSubMesh->getIndexData(0);

        thisIndexData->mOffset = 0;
        thisIndexData->mCount = baseIndexData->mCount * mInstancesPerBatch;

        //TODO: Check numVertices is below max supported by GPU
        NCount mtmp = 16;
        if(mRenderOperation.mVertex->mCount > 65535)
            mtmp = 32;
        N_Only(GBuffer).create(thisIndexData->mBuffer, mtmp, thisIndexData->mCount, Buffer::M_NORMAL );

        void * buf = thisIndexData->mBuffer->lock(Buffer::MM_WHOLE);
        void const * baseBuf = baseIndexData->mBuffer->lock(Buffer::MM_READ);

        Nui16 * thisBuf16 = (Nui16 *)buf;
        Nui32 * thisBuf32 = (Nui32 *)buf;

        for(NCount i=0; i<mInstancesPerBatch; ++i)
        {
            const NCount vertexOffset = i * mRenderOperation.mVertex->mCount / mInstancesPerBatch;

            Nui16 const * initBuf16 = static_cast<Nui16 const *>(baseBuf);
            Nui32 const * initBuf32 = static_cast<Nui32 const *>(baseBuf);

            for(NCount j = 0; j < baseIndexData->mCount; ++j)
            {
                Nui32 originalVal;
                if(baseSubMesh->getIndexData(0)->mBuffer->getUnitSize() == 16)
                    originalVal = *initBuf16++;
                else
                    originalVal = *initBuf32++;

                if(mtmp == 16)
                    *thisBuf16++ = (Nui16)(originalVal + vertexOffset);
                else
                    *thisBuf32++ = originalVal + vertexOffset;
            }
        }

        baseIndexData->mBuffer->unlock();
        thisIndexData->mBuffer->unlock();
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::setupHardwareSkinned(const SubMesh * baseSubMesh,
        VertexData * thisVertexData, VertexData * baseVertexData)
    {
        NCount numBones = baseSubMesh->getMatrixList().size();
        mNumWorldMatrices = mInstancesPerBatch * numBones;

        for(Nui16 i = 0; i < thisVertexData->getFreeBufferIndex(); ++i)
        {
            //Create our own vertex buffer
            VertexBuffer * vBuffer;
            N_Only(GBuffer).create(vBuffer, thisVertexData->getUnitSize(i), thisVertexData->mCount, Buffer::M_NORMAL );
            thisVertexData->attach(i, vBuffer);

            VertexUnitList veList;
            thisVertexData->getUnits(i, veList);

            //Grab the base submesh data
            VertexBuffer * bvBuffer = baseVertexData->getAttach(i);

            Nui8 * thisBuf = (Nui8 *)vBuffer->lock(Buffer::MM_WHOLE);
            Nui8 * baseBuf = (Nui8 *)bvBuffer->lock(Buffer::MM_READ);
            Nui8 *startBuf = baseBuf;

            //Copy and repeat
            for(NCount j = 0; j < mInstancesPerBatch; ++j)
            {
                //Repeat source
                baseBuf = startBuf;

                for(NCount k = 0; k < baseVertexData->mCount; ++k)
                {
                    VertexUnitList::const_iterator it = veList.begin();
                    VertexUnitList::const_iterator en = veList.end();

                    while(it != en)
                    {
                        switch(it->mVType)
                        {
                        case VT_Matrix_Index:
                        {
                            *(thisBuf + it->mOffset + 0) = *(baseBuf + it->mOffset + 0) + j * numBones;
                            *(thisBuf + it->mOffset + 1) = *(baseBuf + it->mOffset + 1) + j * numBones;
                            *(thisBuf + it->mOffset + 2) = *(baseBuf + it->mOffset + 2) + j * numBones;
                            *(thisBuf + it->mOffset + 3) = *(baseBuf + it->mOffset + 3) + j * numBones;
                        }
                        break;
                        default:
                        {
                            memcpy(thisBuf + it->mOffset, baseBuf + it->mOffset, it->getSize());
                        }
                        break;
                        }
                        ++it;
                    }
                    thisBuf += baseVertexData->getUnitSize(i);
                    baseBuf += baseVertexData->getUnitSize(i);
                }
            }

            bvBuffer->unlock();
            vBuffer->unlock();
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchShader::getMatrix(Matrix4f * out, NCount & count) const
    {
        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end)
        {
            out += (*itor)->getTransforms(out);
            ++itor;
        }
        count = mNumWorldMatrices;
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchShader::getMatrixCount() const
    {
        return mNumWorldMatrices;
    }
    //-----------------------------------------------------------------------
}
