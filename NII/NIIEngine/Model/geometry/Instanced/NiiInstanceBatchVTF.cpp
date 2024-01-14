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
#include "NiiInstanceBatchVTF.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiInstanceGeo.h"
#include "NiiMaterialManager.h"
#include "NiiTextureManager.h"
#include "NiiEngine.h"
#include "NiiSkeleton.h"
#include "NiiDualQuaternion.h"

namespace NII
{
    static const Nui16 c_maxTexWidth    = 4096;
    static const Nui16 c_maxTexHeight    = 4096;
    //---------------------------------------------------------------------------
    BaseInstanceBatchVTF::BaseInstanceBatchVTF(InstanceMesh * creator, Mesh * mesh, 
        Material * material, NCount count, const BoneIndexList * bonemap, Nid batchName) :
        InstanceBatch(creator, mesh, material, count, bonemap, batchName),
        mNumWorldMatrices(count),
        mWidthFloatsPadding(0),
        mMaxFloatsPerLine(N_TYPE_MAX(NCount)),
        mRowLength(3), mWeightCount(1),
        mTempTransformsArray3x4(0),
        mUseBoneMatrixLookup(false),
        mMaxLookupTableInstances(16),
        mUseBoneDualQuaternions(false),
        mForceOneWeight(false),
        mUseOneWeight(false)
    {
        cloneMaterial(mMaterial);
    }
    //-------------------------------------------------------------------------
    BaseInstanceBatchVTF::~BaseInstanceBatchVTF()
    {
        //Remove cloned caster materials (if any)
        ShaderFusionList::const_iterator i, iend = mMaterial->getList().end();
        for(i = mMaterial->getList().begin(); i != iend; ++i)
        {
            ShaderFusion * sf = *i;

            if(sf->getShadowCaster() != 0)
                N_Only(Material).remove(sf->getShadowCaster()->getOriginID());
        }

        //Remove cloned material
        N_Only(Material).remove(mMaterial->getOriginID());

        //Remove the VTF texture
        if(mMatrixTexture != 0)
            N_Only(Texture).remove(mMatrixTexture->getOriginID() );

        N_free(mTempTransformsArray3x4);
    }

    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::buildFrom(const SubMesh * baseSubMesh, const GeometryRaw & geo)
    {
        if(useBoneMatrixLookup())
        {
            //when using bone matrix lookup resource are not shared
            //
            //Future implementation: while the instance vertex buffer can't be shared
            //The texture can be.
            //
            build(baseSubMesh);
        }
        else
        {
            createVertexTexture(baseSubMesh);
            InstanceBatch::buildFrom(baseSubMesh, geo);
        }
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::cloneMaterial(const Material * material)
    {
        //Used to track down shadow casters, so the same material caster doesn't get cloned twice
        typedef map<ResourceID, Material *>::type MatMap;
        MatMap clonedMaterials;

        //We need to clone the material so we can have different textures for each batch.
        mMaterial = material->clone(0);

        //Now do the same with the techniques which have a material shadow caster
        ShaderFusionList::const_iterator i, iend = material->getList().end();
        for(i = material->getList().begin(); i != iend; ++i)
        {
            ShaderFusion * sf = *i;

            if(sf->getShadowCaster() != 0)
            {
                const Material * casterMat = sf->getShadowCaster();
                ResourceID casterName = casterMat->getOriginID();

                //Was this material already cloned?
                MatMap::const_iterator itor = clonedMaterials.find(casterName);

                if(itor == clonedMaterials.end())
                {
                    //No? Clone it and track it
                    Material * cloned = casterMat->clone(0);
                    sf->setShadowCaster(cloned);
                    clonedMaterials[casterName] = cloned;
                }
                else
                    sf->setShadowCaster(itor->second); //Reuse the previously cloned mat
            }
        }
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::retrieveBoneIdx(VertexData * vData,
        HWBoneIdxVec & outBoneIdx)
    {
        const VertexUnit * ve = vData->get(VT_Matrix_Index);
        const VertexUnit * veWeights = vData->get(VT_Matrix_Weight);

        VertexBuffer * buff = vData->getAttach(ve->mSource);
        NIIb const * baseBuffer = static_cast<NIIb const *>(buff->lock(Buffer::MM_READ));

        for(NCount i = 0; i < vData->mCount; ++i)
        {
            NIIf const * pWeights = reinterpret_cast<NIIf const *>(baseBuffer + veWeights->mOffset);

            NCount biggestWeightIdx = 0;
            for(NCount j = 1; j < mWeightCount; ++j)
            {
                biggestWeightIdx = pWeights[biggestWeightIdx] < pWeights[j] ? j : biggestWeightIdx;
            }

            Nui8 const * pIndex = reinterpret_cast<Nui8 const *>(baseBuffer + ve->mOffset);
            outBoneIdx[i] = pIndex[biggestWeightIdx];

            baseBuffer += vData->getUnitSize(ve->mSource);
        }
        buff->unlock();
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::retrieveBoneIdxWithWeights(VertexData * vData,
        HWBoneIdxVec & outBoneIdx, HWBoneWgtVec & outBoneWgt)
    {
        const VertexUnit * ve = vData->get(VT_Matrix_Index);
        const VertexUnit * veWeights = vData->get(VT_Matrix_Weight);

        VertexBuffer * buff = vData->getAttach(ve->mSource);

        NIIb const * baseBuffer = (NIIb const*)buff->lock(Buffer::MM_READ);

        for(NCount i = 0; i<vData->mCount * mWeightCount; i += mWeightCount)
        {
            NIIf const * pWeights = reinterpret_cast<NIIf const*>(baseBuffer + veWeights->mOffset);
            Nui8 const * pIndex = reinterpret_cast<Nui8 const*>(baseBuffer + ve->mOffset);

            NIIf weightMagnitude = 0.0f;
            for( NCount j=0; j < mWeightCount; ++j )
            {
                outBoneWgt[i+j] = pWeights[j];
                weightMagnitude += pWeights[j];
                outBoneIdx[i+j] = pIndex[j];
            }

            //Normalize the bone weights so they add to one
            for(NCount j = 0; j < mWeightCount; ++j)
            {
                outBoneWgt[i + j] /= weightMagnitude;
            }

            baseBuffer += vData->getUnitSize(ve->mSource);
        }

        buff->unlock();
    }

    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::setupMaterialToUseVTF(Texture::Type textureType,
        Material * material)
    {
        ShaderFusionList::const_iterator z, zend = material->getList().end();
        for(z = material->getList().begin(); z != zend; ++z)
        {
            ShaderFusion * technique = *z;
            ShaderChList::iterator i, iend = technique->getShaderChList().end();
            for(i = technique->getShaderChList().begin(); i != iend; ++i)
            {
                bool bTexUnitFound = false;
                if(!bTexUnitFound)
                {
                    const ShaderChTexture & shtex = (*i)->getTexture();
                    ShaderChTexture::Textures::const_iterator j, jend = shtex.getTextures().end();
                    for(j = shtex.getTextures().begin(); j != jend; ++j)
                    {
                        ShaderChTextureUnit * texUnit = *j;

                        if(texUnit->getName() == _T("InstancingVTF"))
                        {
                            ResourceIDList ridlist;
                            ridlist.push_back(mMatrixTexture->getOriginID());
                            texUnit->setSrc(ridlist, textureType);
                            texUnit->getSample()->setFiltering(TFM_NONE);
                            texUnit->setFetchType(TDF_VS);
                        }
                    }
                }
            }

            if(technique->getShadowCaster() != 0)
            {
                Material * matCaster = technique->getShadowCaster();
                setupMaterialToUseVTF(textureType, matCaster);
            }
        }
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::createVertexTexture(const SubMesh * baseSubMesh)
    {
        /*
        TODO: Find a way to retrieve max texture resolution,
        http://www.ogre3d.org/forums/viewtopic.php?t=38305

        Currently assuming it's 4096x4096, which is a safe bet for any hardware with decent VTF*/

        NCount uniqueAnimations = mInstancesPerBatch;
        if(useBoneMatrixLookup())
        {
            uniqueAnimations = std::min<NCount>(getMaxLookupTableInstances(), uniqueAnimations);
        }
        mMatricesPerInstance = std::max<NCount>(1, baseSubMesh->getMatrixList().size());

        if(mUseBoneDualQuaternions && !mTempTransformsArray3x4)
        {
            mTempTransformsArray3x4 = N_alloc_t(NIIf, mMatricesPerInstance * 3 * 4);
        }

        mNumWorldMatrices = uniqueAnimations * mMatricesPerInstance;

        //Calculate the width & height required to hold all the matrices. Start by filling the width
        //first (i.e. 4096x1 4096x2 4096x3, etc)

        NCount texWidth = std::min<NCount>(mNumWorldMatrices * mRowLength, c_maxTexWidth);
        NCount maxUsableWidth = texWidth;
        if(matricesToghetherPerRow())
        {
            //The technique requires all matrices from the same instance in the same row
            //i.e. 4094 -> 4095 -> skip 4096 -> 0 (next row) contains data from a new instance
            mWidthFloatsPadding = texWidth % (mMatricesPerInstance * mRowLength);

            if(mWidthFloatsPadding)
            {
                mMaxFloatsPerLine = texWidth - mWidthFloatsPadding;

                maxUsableWidth = mMaxFloatsPerLine;

                //Values are in pixels, convert them to floats (1 pixel = 4 floats)
                mWidthFloatsPadding    *= 4;
                mMaxFloatsPerLine        *= 4;
            }
        }

        NCount texHeight = mNumWorldMatrices * mRowLength / maxUsableWidth;

        if((mNumWorldMatrices * mRowLength) % maxUsableWidth)
            texHeight += 1;

        //Don't use 1D textures, as OGL goes crazy because the shader should be calling texture1D()...
        //Texture::Type texType = texHeight == 1 ? Texture::T_1D : Texture::T_2D;
        Texture::Type texType = Texture::T_2D;

        mMatrixTexture = N_Only(Texture).createTexture(0, mMeshReference->getGroup(), texType,
            PF_R32G32B32A32, texWidth, texHeight, 1, 0, Buffer::M_WRITE | Buffer::M_WHOLE);

        //Set our cloned material to use this custom texture!
        setupMaterialToUseVTF(texType, mMaterial);
    }
    //-----------------------------------------------------------------------
    NCount BaseInstanceBatchVTF::convert3x4MatricesToDualQuaternions(NIIf * matrices,
        NCount numOfMatrices, NIIf * outDualQuaternions)
    {
        DualQuaternion dQuat;
        Matrix4f matrix;
        NCount floatsWritten = 0;

        for (NCount m = 0; m < numOfMatrices; ++m)
        {
            for(NIIi i = 0; i < 3; ++i)
            {
                for(NIIi b = 0; b < 4; ++b)
                {
                    matrix[i][b] = *matrices++;
                }
            }

            matrix[3][0] = 0;
            matrix[3][1] = 0;
            matrix[3][2] = 0;
            matrix[3][3] = 1;

            dQuat.from(matrix);

            //Copy the 2x4 matrix
            for(NIIi i = 0; i < 8; ++i)
            {
                *outDualQuaternions++ = static_cast<NIIf>(dQuat[i]);
                ++floatsWritten;
            }
        }

        return floatsWritten;
    }

    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::updateVertexTexture(void)
    {
        //Now lock the texture and copy the 4x3 matrices!
        mMatrixTexture->getBuffer()->lock(Buffer::MM_WHOLE);
        const PixelBlock & pixelBox = mMatrixTexture->getBuffer()->getMutexData();

        NIIf * pDest = (NIIf *)pixelBox.mData;

        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        NIIf * transforms;

        //If using dual quaternion skinning, write the transforms to a temporary buffer,
        //then convert to dual quaternions, then later write to the pixel buffer
        //Otherwise simply write the transforms to the pixel buffer directly
        if(mUseBoneDualQuaternions)
        {
            transforms = mTempTransformsArray3x4;
        }
        else
        {
            transforms = pDest;
        }

        while(itor != end)
        {
            NCount floatsWritten = (*itor)->getTransforms3x4(transforms);

            if( mManager->isCameraOrigin())
                makeMatrixCameraRelative3x4(transforms, floatsWritten);

            if(mUseBoneDualQuaternions)
            {
                floatsWritten = convert3x4MatricesToDualQuaternions(
                    transforms, floatsWritten / 12, pDest);

                pDest += floatsWritten;
            }
            else
            {
                transforms += floatsWritten;
            }

            ++itor;
        }

        mMatrixTexture->getBuffer()->unlock();
    }
    /* update the lookup numbers for entities with shared transforms */
    void BaseInstanceBatchVTF::updateSharedLookupIndexes()
    {
        if (mTransformSharingDirty)
        {
            if(useBoneMatrixLookup())
            {
                //In each entity update the "transform lookup number" so that:
                // 1. All entities sharing the same transformation will share the same unique number
                // 2. "transform lookup number" will be numbered from 0 up to getMaxLookupTableInstances
                NCount lookupCounter = 0;
                typedef map<Matrix4f *, NCount>::type MapTransformId;
                MapTransformId transformToId;
                InstanceGeoVec::const_iterator it, itend = mInstancedEntities.end();
                for(it = mInstancedEntities.begin(); it != itend ; ++it)
                {
                    if((*it)->isActive())
                    {
                        Matrix4f * transformUniqueId = const_cast<Matrix4f *>((*it)->mSkeleton->getMatrix());
                        MapTransformId::iterator itLu = transformToId.find(transformUniqueId);
                        if (itLu == transformToId.end())
                        {
                            itLu = transformToId.insert(Npair(transformUniqueId, lookupCounter)).first;
                            ++lookupCounter;
                        }
                        (*it)->setTransformLookupNumber(itLu->second);
                    }
                    else
                    {
                        (*it)->setTransformLookupNumber(0);
                    }
                }

                if (lookupCounter > getMaxLookupTableInstances())
                {
                    N_EXCEPT(InvalidState,
                        _I18n("Number of unique bone matrix states exceeds current limitation."));
                }
            }
            mTransformSharingDirty = false;
        }
    }
    //-----------------------------------------------------------------------
    InstanceGeo * BaseInstanceBatchVTF::generateInstanceGeo(NCount num)
    {
        InstanceGeo * sharedTransformEntity = NULL;
        if ((useBoneMatrixLookup()) && (num >= getMaxLookupTableInstances()))
        {
            sharedTransformEntity = mInstancedEntities[num % getMaxLookupTableInstances()];
            if (sharedTransformEntity->mSharedTransformEntity)
            {
                sharedTransformEntity = sharedTransformEntity->mSharedTransformEntity;
            }
        }
        return N_new InstanceGeo( this, num, sharedTransformEntity);
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //-----------------------------------------------------------------------
    NCount BaseInstanceBatchVTF::getMatrixCount() const
    {
        return 1;
    }
    //-----------------------------------------------------------------------
    void BaseInstanceBatchVTF::queue(RenderQueue * queue)
    {
        InstanceBatch::queue(queue);

        if( mBoundsUpdated || mDirtyAnimation || mManager->isCameraOrigin())
            updateVertexTexture();

        mBoundsUpdated = false;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // InstanceBatchVTF
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    InstanceBatchVTF::InstanceBatchVTF(InstanceMesh * creator, Mesh * mesh,
        Material * material, NCount count, const BoneIndexList * bonemap, Nid bid) :
            BaseInstanceBatchVTF(creator, mesh, material, count, bonemap, bid)
    {
    }
    //-----------------------------------------------------------------------
    InstanceBatchVTF::~InstanceBatchVTF()
    {
    }
    //-----------------------------------------------------------------------
    void InstanceBatchVTF::setupVertices(const SubMesh * baseSubMesh)
    {
        N_Only(GBuffer).create(mRenderOperation.mVertex);
        mRemoveOwnVertexData = true; //Raise flag to remove our own vertex data in the end (not always needed)

        VertexData * thisVertexData = mRenderOperation.mVertex;
        VertexData * baseVertexData = baseSubMesh->getVertexData();

        thisVertexData->mOffset = 0;
        thisVertexData->mCount = baseVertexData->mCount * mInstancesPerBatch;
        thisVertexData->copy(baseVertexData);

        HWBoneIdxVec hwBoneIdx;
        HWBoneWgtVec hwBoneWgt;

        //Blend weights may not be present because HW_VTF does not require to be skeletally animated
        const VertexUnit * veWeights = baseVertexData->get(VT_Matrix_Weight);
        if(veWeights)
        {
            //One weight is recommended for VTF
            mWeightCount = (forceOneWeight() || useOneWeight()) ?
                1 : veWeights->getSize() / sizeof(NIIf);
        }
        else
        {
            mWeightCount = 1;
        }

        hwBoneIdx.resize(baseVertexData->mCount * mWeightCount, 0);

        if(mMeshReference->isSkeletonFusion() && mMeshReference->getSkeleton())
        {
            if(mWeightCount > 1)
            {
                hwBoneWgt.resize(baseVertexData->mCount * mWeightCount, 0);
                retrieveBoneIdxWithWeights(baseVertexData, hwBoneIdx, hwBoneWgt);
            }
            else
            {
                retrieveBoneIdx(baseVertexData, hwBoneIdx);
                thisVertexData->remove(VT_Matrix_Index);
                thisVertexData->remove(VT_Matrix_Weight);

                thisVertexData->optUnit();
            }
        }

        for(Nui16 i = 0; i < thisVertexData->getFreeBufferIndex(); ++i)
        {
            //Create our own vertex buffer
            VertexBuffer * tempbuf;
            N_Only(GBuffer).create(tempbuf, thisVertexData->getUnitSize(i), thisVertexData->mCount, Buffer::M_NORMAL );
            thisVertexData->attach(i, tempbuf);

            //Grab the base submesh data
            VertexBuffer * basebuf = baseVertexData->getAttach(i);

            Nui8 * thisBuf = (Nui8*)tempbuf->lock(Buffer::MM_WHOLE);
            Nui8* baseBuf = (Nui8 *)basebuf->lock(Buffer::MM_READ);

            //Copy and repeat
            for(NCount j=0; j<mInstancesPerBatch; ++j)
            {
                NCount sizeOfBuffer = baseVertexData->mCount * baseVertexData->getUnitSize(i);
                memcpy(thisBuf + j * sizeOfBuffer, baseBuf, sizeOfBuffer);
            }

            basebuf->unlock();
            tempbuf->unlock();
        }

        createVertexTexture(baseSubMesh);
        createVertexSemantics(thisVertexData, baseVertexData, hwBoneIdx, hwBoneWgt);
    }
    //-----------------------------------------------------------------------
    void InstanceBatchVTF::setupIndices(const SubMesh * baseSubMesh)
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

        Nui16 * thisBuf16 = static_cast<Nui16*>(buf);
        Nui32 *thisBuf32 = static_cast<Nui32*>(buf);

        for( NCount i=0; i<mInstancesPerBatch; ++i )
        {
            const NCount vertexOffset = i * mRenderOperation.mVertex->mCount / mInstancesPerBatch;

            Nui16 const * initBuf16 = static_cast<Nui16 const *>(baseBuf);
            Nui32 const * initBuf32 = static_cast<Nui32 const *>(baseBuf);

            for(NCount j = 0; j<baseIndexData->mCount; ++j)
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
    void InstanceBatchVTF::createVertexSemantics(VertexData * thisVertexData,
        VertexData * baseVertexData, const HWBoneIdxVec & hwBoneIdx,
            const HWBoneWgtVec & hwBoneWgt)
    {
        const NCount texWidth  = mMatrixTexture->getWidth();
        const NCount texHeight = mMatrixTexture->getHeight();

        //Calculate the texel offsets to correct them offline
        //Akwardly enough, the offset is needed in OpenGL too
        Vector2f texelOffsets;
        texelOffsets.x = /*rsys->getTexelXOffset()*/ -0.5f / (NIIf)texWidth;
        texelOffsets.y = /*rsys->getTexelYOffset()*/ -0.5f / (NIIf)texHeight;

        //Only one weight per vertex is supported. It would not only be complex, but prohibitively slow.
        //Put them in a new buffer, since it's 32 bytes aligned :-)
        Nui16 newSource = thisVertexData->getFreeBufferIndex();
        NCount maxFloatsPerVector = 4;
        NCount offset = 0;

        for(NCount i = 0; i < mWeightCount; i += maxFloatsPerVector / mRowLength)
        {
            offset += thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
            offset += thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
        }

        //Add the weights (supports up to four, which is Ogre's limit)
        if(mWeightCount > 1)
        {
            thisVertexData->add(DT_Float4x, VT_Matrix_Weight, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
        }

        //Create our own vertex buffer
        VertexBuffer * vertexBuffer;
        N_Only(GBuffer).create(vertexBuffer, thisVertexData->getUnitSize(newSource), thisVertexData->mCount, Buffer::M_NORMAL );
        thisVertexData->attach(newSource, vertexBuffer);

        NIIf * thisFloat = (NIIf *)vertexBuffer->lock(Buffer::MM_WHOLE);

        //Copy and repeat
        for(NCount i = 0; i< mInstancesPerBatch; ++i)
        {
            for(NCount j = 0; j<baseVertexData->mCount * mWeightCount; j += mWeightCount)
            {
                NCount numberOfMatricesInLine = 0;

                for(NCount wgtIdx = 0; wgtIdx < mWeightCount; ++wgtIdx)
                {
                    for(NCount k=0; k < mRowLength; ++k)
                    {
                        NCount instanceIdx = (hwBoneIdx[j+wgtIdx] + i * mMatricesPerInstance) * mRowLength + k;
                        //x
                        *thisFloat++ = ((instanceIdx % texWidth) / (NIIf)texWidth) - (NIIf)texelOffsets.x;
                        //y
                        *thisFloat++ = ((instanceIdx / texWidth) / (NIIf)texHeight) - (NIIf)texelOffsets.y;
                    }

                    ++numberOfMatricesInLine;

                    //If another matrix can't be fit, we're on another line, or if this is the last weight
                    if((numberOfMatricesInLine + 1) * mRowLength > maxFloatsPerVector || (wgtIdx+1) == mWeightCount)
                    {
                        //Place zeroes in the remaining coordinates
                        for(NCount k = mRowLength * numberOfMatricesInLine; k < maxFloatsPerVector; ++k)
                        {
                            *thisFloat++ = 0.0f;
                            *thisFloat++ = 0.0f;
                        }

                        numberOfMatricesInLine = 0;
                    }
                }

                //Don't need to write weights if there is only one
                if(mWeightCount > 1)
                {
                    //Write the weights
                    for(NCount wgtIdx = 0; wgtIdx < mWeightCount; ++wgtIdx)
                    {
                        *thisFloat++ = hwBoneWgt[j+wgtIdx];
                    }

                    //Fill the rest of the line with zeros
                    for(NCount wgtIdx = mWeightCount; wgtIdx < maxFloatsPerVector; ++wgtIdx)
                    {
                        *thisFloat++ = 0.0f;
                    }
                }
            }
        }
        vertexBuffer->unlock();
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchVTF::calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const
    {
        NCount retVal = 0;
        const RenderFeature * capabilities = N_Engine().getRender()->getFeature();

        //VTF must be supported
        if(capabilities->isSupport(GF_Program_VertexTexFetch))
        {
            //TODO: Check PF_R32G32B32A32 is supported (should be, since it was the 1st one)
            NCount numBones = std::max<NCount>(1, baseSubMesh->getMatrixList().size());
            retVal = c_maxTexWidth * c_maxTexHeight / mRowLength / numBones;

            if(flags & IM_USE16BIT)
            {
                if(baseSubMesh->getVertexData()->mCount * retVal > 0xFFFF)
                    retVal = 0xFFFF / baseSubMesh->getVertexData()->mCount;
            }

            if(flags & IM_VTFBESTFIT)
            {
                NCount instancesPerBatch = std::min(retVal, mInstancesPerBatch);
                //Do the same as in createVertexTexture()
                NCount numWorldMatrices = instancesPerBatch * numBones;

                NCount texWidth  = std::min<NCount>(numWorldMatrices * mRowLength, c_maxTexWidth);
                NCount texHeight = numWorldMatrices * mRowLength / c_maxTexWidth;

                const NCount remainder = (numWorldMatrices * mRowLength) % c_maxTexWidth;

                if(remainder && texHeight > 0)
                    retVal = texWidth * texHeight / mRowLength / numBones;
            }
        }
        return retVal;
    }
    //--------------------------------------------------------------------------
}