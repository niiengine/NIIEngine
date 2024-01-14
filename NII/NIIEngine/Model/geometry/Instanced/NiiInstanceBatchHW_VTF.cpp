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
#include "NiiInstanceBatchHW_VTF.h"
#include "NiiGBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiInstanceGeo.h"
#include "NiiMaterialManager.h"
#include "NiiTextureManager.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    static const Nui16 c_maxTexWidthHW  = 4096;
    static const Nui16 c_maxTexHeightHW = 4096;
    //-----------------------------------------------------------------------
    InstanceBatchHW_VTF::InstanceBatchHW_VTF(InstanceMesh * creator, Mesh * mesh,
        Material * material, NCount count, const BoneIndexList * bonemap, Nid bid) :
            BaseInstanceBatchVTF(creator, mesh, material, count, bonemap, bid),
            mKeepStatic(false)
    {
    }
    //-----------------------------------------------------------------------
    InstanceBatchHW_VTF::~InstanceBatchHW_VTF()
    {
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::setupVertices(const SubMesh * baseSubMesh)
    {
        N_Only(GBuffer).create(mRenderOperation.mVertex);
        mRemoveOwnVertexData = true; //Raise flag to remove our own vertex data in the end (not always needed)

        VertexData * thisVertexData = mRenderOperation.mVertex;
        VertexData * baseVertexData = baseSubMesh->getVertexData();

        thisVertexData->mOffset = 0;
        thisVertexData->mCount = baseVertexData->mCount;
        mRenderOperation.mInstanceCount = mInstancesPerBatch;
        thisVertexData->copy(baseVertexData);

        //Reuse all vertex buffers
        VertexData::BindingList::const_iterator itor = baseVertexData->getAttachs().begin();
        VertexData::BindingList::const_iterator end = baseVertexData->getAttachs().end();
        while(itor != end)
        {
            Nui16 bufferIdx = itor->first;
            VertexBuffer * vBuf = itor->second;
            thisVertexData->attach(bufferIdx, vBuf);
            ++itor;
        }

        //Remove the blend weights & indices
        HWBoneIdxVec hwBoneIdx;
        HWBoneWgtVec hwBoneWgt;

        //Blend weights may not be present because HW_VTF does not require to be skeletally animated
        const VertexUnit * veWeights = baseVertexData->get(VT_Matrix_Weight);
        if(veWeights)
            mWeightCount = forceOneWeight() ? 1 : veWeights->getSize() / sizeof(NIIf);
        else
            mWeightCount = 1;

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
            }

            const VertexUnit * pElement = thisVertexData->get(VT_Matrix_Index);
            if(pElement)
            {
                Nui16 skelDataSource = pElement->mSource;
                thisVertexData->remove(VT_Matrix_Index);
                thisVertexData->remove(VT_Matrix_Weight);
                VertexUnitList temp;
                thisVertexData->getUnits(skelDataSource, temp);
                if (temp.empty())
                {
                    thisVertexData->optUnit();
                    thisVertexData->detach(skelDataSource);
                    thisVertexData->optAttach();
                }
            }
        }

        createVertexTexture(baseSubMesh);
        createVertexSemantics(thisVertexData, baseVertexData, hwBoneIdx, hwBoneWgt);
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::setupIndices(const SubMesh * obj)
    {
        //We could use just a reference, but the InstanceMesh will in the end attampt to delete
        //the pointer, and we can't give it something that doesn't belong to us.
        mRenderOperation.mIndex = obj->getIndexData(0)->clone(true);
        mRemoveOwnIndexData = true;    //Raise flag to remove our own index data in the end (not always needed)
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::createVertexSemantics(VertexData * thisVertexData,
        VertexData * baseVertexData, const HWBoneIdxVec & hwBoneIdx,
            const HWBoneWgtVec & hwBoneWgt)
    {
        const NIIf texWidth  = (NIIf)mMatrixTexture->getWidth();

        //Only one weight per vertex is supported. It would not only be complex, but prohibitively slow.
        //Put them in a new buffer, since it's 16 bytes aligned :-)
        Nui16 newSource = thisVertexData->getFreeBufferIndex();

        NCount offset = 0;

        NCount maxFloatsPerVector = 4;

        //Can fit two dual quaternions in every float4, but only one 3x4 matrix
        for(NCount i = 0; i < mWeightCount; i += maxFloatsPerVector / mRowLength)
        {
            offset += thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
        }

        //Add the weights (supports up to four, which is Ogre's limit)
        if(mWeightCount > 1)
        {
            thisVertexData->add(DT_Float4x, VT_Matrix_Weight, newSource, offset, 0)->getSize();
        }

        //Create our own vertex buffer
        VertexBuffer * vertexBuffer;
        N_Only(GBuffer).create(vertexBuffer, thisVertexData->getUnitSize(newSource), thisVertexData->mCount, Buffer::M_NORMAL );
        thisVertexData->attach(newSource, vertexBuffer);

        NIIf *thisFloat = (NIIf*)vertexBuffer->lock(Buffer::MM_WHOLE);

        //Create the UVs to sample from the right bone/matrix
        for( NCount j=0; j < baseVertexData->mCount * mWeightCount; j += mWeightCount)
        {
            NCount numberOfMatricesInLine = 0;

            //Write the matrices, adding padding as needed
            for(NCount i = 0; i < mWeightCount; ++i)
            {
                //Write the matrix
                for( NCount k=0; k < mRowLength; ++k)
                {
                    //Only calculate U (not V) since all matrices are in the same row. We use the instanced
                    //(repeated) buffer to tell how much U & V we need to offset
                    NCount instanceIdx = hwBoneIdx[j+i] * mRowLength + k;
                    *thisFloat++ = instanceIdx / texWidth;
                }

                ++numberOfMatricesInLine;

                //If another matrix can't be fit, we're on another line, or if this is the last weight
                if((numberOfMatricesInLine + 1) * mRowLength > maxFloatsPerVector || (i+1) == mWeightCount)
                {
                    //Place zeroes in the remaining coordinates
                    for ( NCount k=mRowLength * numberOfMatricesInLine; k < maxFloatsPerVector; ++k)
                    {
                        *thisFloat++ = 0.0f;
                    }

                    numberOfMatricesInLine = 0;
                }
            }

            //Don't need to write weights if there is only one
            if(mWeightCount > 1)
            {
                //Write the weights
                for(NCount i = 0; i < mWeightCount; ++i)
                {
                    *thisFloat++ = hwBoneWgt[j+i];
                }

                //Write the empty space
                for(NCount i = mWeightCount; i < maxFloatsPerVector; ++i)
                {
                    *thisFloat++ = 0.0f;
                }
            }
        }

        vertexBuffer->unlock();

        //Now create the instance buffer that will be incremented per instance, contains UV offsets
        newSource = thisVertexData->getFreeBufferIndex();
        offset = thisVertexData->add(DT_Float2x, VT_Tex_Coord, newSource, 0, 
            thisVertexData->getCount(VT_Tex_Coord))->getSize();
        if (useBoneMatrixLookup())
        {
            //if using bone matrix lookup we will need to add 3 more float4 to contain the matrix. containing
            //the personal world transform of each entity.
            offset += thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
            offset += thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
            thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, 
                thisVertexData->getCount(VT_Tex_Coord))->getSize();
            //Add two floats of padding here? or earlier?
            //If not using bone matrix lookup, is it ok that it is 8 bytes since divides evenly into 16

        }

        //Create our own vertex buffer
        N_Only(GBuffer).create(mInstanceVertexBuffer, thisVertexData->getUnitSize(newSource), mInstancesPerBatch, Buffer::M_NORMAL );

        thisVertexData->attach(newSource, mInstanceVertexBuffer);

        VertexUnitList tempu;
        thisVertexData->getUnits(newSource, tempu);
        VertexUnitList::iterator i, iend = tempu.end();
        for (i = tempu.begin(); i != iend; ++i)
            i->mDivisor = 1;

        updateInstanceDataBuffer(true, NULL);
    }

    //updates the vertex buffer containing the per instance data
    NCount InstanceBatchHW_VTF::updateInstanceDataBuffer(bool isFirstTime, Camera* currentCamera)
    {
        NCount visibleEntityCount = 0;
        bool useMatrixLookup = useBoneMatrixLookup();
        if (isFirstTime ^ useMatrixLookup)
        {
            //update the mTransformLookupNumber value in the entities if needed
            updateSharedLookupIndexes();

            const NIIf texWidth  = static_cast<NIIf>(mMatrixTexture->getWidth());
            const NIIf texHeight = static_cast<NIIf>(mMatrixTexture->getHeight());

            //Calculate the texel offsets to correct them offline
            //Awkwardly enough, the offset is needed in OpenGL too
            Vector2f texelOffsets;
            //RenderSys *rsys = Engine::getOnly().getRender();
            texelOffsets.x = /*rsys->getTexelXOffset()*/ -0.5f / texWidth;
            texelOffsets.y = /*rsys->getTexelXOffset()*/ -0.5f / texHeight;

            NIIf *thisVec = static_cast<NIIf*>(mInstanceVertexBuffer->lock(Buffer::MM_WHOLE));

            const NCount maxPixelsPerLine = std::min(mMatrixTexture->getWidth(), mMaxFloatsPerLine >> 2);

            //Calculate UV offsets, which change per instance
            for( NCount i=0; i<mInstancesPerBatch; ++i )
            {
                InstanceGeo* entity = useMatrixLookup ? mInstancedEntities[i] : NULL;
                if  //Update if we are not using a lookup bone matrix method. In this case the function will
                    //be called only once
                    (!useMatrixLookup ||
                    //Update if we are in the visible range of the camera (for look up bone matrix method
                    //and static mode).
                    (entity->findVisible(currentCamera)))
                {
                    NCount matrixIndex = useMatrixLookup ? entity->mTransformLookupNumber : i;
                    NCount instanceIdx = matrixIndex * mMatricesPerInstance * mRowLength;
                    *thisVec = ((instanceIdx % maxPixelsPerLine) / texWidth) - (NIIf)(texelOffsets.x);
                    *(thisVec + 1) = ((instanceIdx / maxPixelsPerLine) / texHeight) - (NIIf)(texelOffsets.y);
                    thisVec += 2;

                    if (useMatrixLookup)
                    {
                        const Matrix4f & mat = entity->getSpaceMatrix();
                        *(thisVec)     = static_cast<NIIf>( mat[0][0] );
                        *(thisVec + 1) = static_cast<NIIf>( mat[0][1] );
                        *(thisVec + 2) = static_cast<NIIf>( mat[0][2] );
                        *(thisVec + 3) = static_cast<NIIf>( mat[0][3] );
                        *(thisVec + 4) = static_cast<NIIf>( mat[1][0] );
                        *(thisVec + 5) = static_cast<NIIf>( mat[1][1] );
                        *(thisVec + 6) = static_cast<NIIf>( mat[1][2] );
                        *(thisVec + 7) = static_cast<NIIf>( mat[1][3] );
                        *(thisVec + 8) = static_cast<NIIf>( mat[2][0] );
                        *(thisVec + 9) = static_cast<NIIf>( mat[2][1] );
                        *(thisVec + 10)= static_cast<NIIf>( mat[2][2] );
                        *(thisVec + 11)= static_cast<NIIf>( mat[2][3] );
                        if(currentCamera && mManager->isCameraOrigin()) // && useMatrixLookup
                        {
                            const Vector3f & cameraRelativePosition = currentCamera->getSpacePos();
                            *(thisVec + 3) -= static_cast<NIIf>(cameraRelativePosition.x);
                            *(thisVec + 7) -= static_cast<NIIf>(cameraRelativePosition.y);
                            *(thisVec + 11) -=  static_cast<NIIf>(cameraRelativePosition.z);
                        }
                        thisVec += 12;
                    }
                    ++visibleEntityCount;
                }
            }

            mInstanceVertexBuffer->unlock();
        }
        else
        {
            visibleEntityCount = mInstancedEntities.size();
        }
        return visibleEntityCount;
    }
    //-----------------------------------------------------------------------
    bool InstanceBatchHW_VTF::checkSubMeshCompatibility(const SubMesh * obj)
    {
        //Max number of texture coordinates is _usually_ 8, we need at least 2 available
        Nui16 neededTextureCoord = 2;
        if(useBoneMatrixLookup())
        {
            //we need another 3 for the unique world transform of each instanced entity
            neededTextureCoord += 3;
        }
        if(obj->getVertexData()->getCount(VT_Tex_Coord) > 8 - neededTextureCoord)
        {
            N_EXCEPT(UnImpl, _I18n("Given mesh must have at least ") +
                N_conv(neededTextureCoord) + _I18n("free TEXCOORDs"));
        }
        return InstanceBatch::checkSubMeshCompatibility(obj);
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchHW_VTF::calculateMaxNumInstances(const SubMesh * baseSubMesh,
        Nui16 flags) const
    {
        NCount retVal = 0;

        RenderSys * rsys = N_Engine().getRender();
        const RenderFeature * capabilities = rsys->getFeature();

        //VTF & HW Instancing must be supported
        if(capabilities->isSupport(GF_Vertex_Instance) &&
            capabilities->isSupport(GF_Program_VertexTexFetch))
        {
            //TODO: Check PF_R32G32B32A32 is supported (should be, since it was the 1st one)
            NCount numBones = std::max<NCount>(1, baseSubMesh->getMatrixList().size());

            NCount maxUsableWidth = c_maxTexWidthHW - (c_maxTexWidthHW % (numBones * mRowLength));

            //See InstanceBatchHW::calculateMaxNumInstances for the 65535
            retVal = std::min<NCount>( 65535, maxUsableWidth * c_maxTexHeightHW / mRowLength / numBones );

            if( flags & IM_VTFBESTFIT )
            {
                NCount numUsedSkeletons = mInstancesPerBatch;
                if (flags & IM_VTFBONEMATRIXLOOKUP)
                    numUsedSkeletons = std::min<NCount>(getMaxLookupTableInstances(), numUsedSkeletons);
                const NCount instancesPerBatch = std::min( retVal, numUsedSkeletons );
                //Do the same as in createVertexTexture(), but changing c_maxTexWidthHW for maxUsableWidth
                const NCount numWorldMatrices = instancesPerBatch * numBones;

                NCount texWidth  = std::min<NCount>( numWorldMatrices * mRowLength, maxUsableWidth );
                NCount texHeight = numWorldMatrices * mRowLength / maxUsableWidth;

                const NCount remainder = (numWorldMatrices * mRowLength) % maxUsableWidth;

                if( remainder && texHeight > 0 )
                    retVal = static_cast<NCount>(texWidth * texHeight / (NIIf)mRowLength / (NIIf)(numBones));
            }
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchHW_VTF::updateVertexTexture(Camera * currentCamera)
    {
        NCount renderedInstances = 0;
        bool useMatrixLookup = useBoneMatrixLookup();
        if (useMatrixLookup)
        {
            //if we are using bone matrix look up we have to update the instance buffer for the
            //vertex texture to be relevant

            //also note that in this case the number of instances to render comes directly from the
            //updateInstanceDataBuffer() function, not from this function.
            renderedInstances = updateInstanceDataBuffer(false, currentCamera);
        }


        mDirtyAnimation = false;

        //Now lock the texture and copy the 4x3 matrices!
        mMatrixTexture->getBuffer()->lock(Buffer::MM_WHOLE);
        const PixelBlock & pixelBox = mMatrixTexture->getBuffer()->getMutexData();

        NIIf * pSource = (NIIf *)pixelBox.mData;

        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();

        std::vector<bool> writtenPositions(getMaxLookupTableInstances(), false);

        NCount floatPerEntity = mMatricesPerInstance * mRowLength * 4;
        NCount entitiesPerPadding = (NCount)(mMaxFloatsPerLine / floatPerEntity);

        NCount instanceCount = mInstancedEntities.size();
        NCount updatedInstances = 0;

        NIIf* transforms = NULL;
        //If using dual quaternions, write 3x4 matrices to a temporary buffer, then convert to dual quaternions
        if(mUseBoneDualQuaternions)
        {
            transforms = mTempTransformsArray3x4;
        }

        for(NCount i = 0 ; i < instanceCount ; ++i)
        {
            InstanceGeo* entity = mInstancedEntities[i];
            NCount textureLookupPosition = updatedInstances;
            if (useMatrixLookup)
            {
                textureLookupPosition = entity->mTransformLookupNumber;
            }
            //Check that we are not using a lookup matrix or that we have not already written
            //The bone data
            if (((!useMatrixLookup) || !writtenPositions[entity->mTransformLookupNumber]) &&
                //Cull on an individual basis, the less entities are visible, the less instances we draw.
                //No need to use null matrices at all!
                (entity->findVisible( currentCamera )))
            {
                NIIf * pDest = pSource + floatPerEntity * textureLookupPosition +
                    (NCount)(textureLookupPosition / entitiesPerPadding) * mWidthFloatsPadding;

                if(!mUseBoneDualQuaternions)
                {
                    transforms = pDest;
                }

                if(mMeshReference->isSkeletonFusion())
                    mDirtyAnimation |= entity->_updateAnimation();

                NCount floatsWritten = entity->getTransforms3x4(transforms);

                if(!useMatrixLookup && mManager->isCameraOrigin())
                    makeMatrixCameraRelative3x4(transforms, floatsWritten);

                if(mUseBoneDualQuaternions)
                {
                    convert3x4MatricesToDualQuaternions(transforms, floatsWritten / 12, pDest);
                }

                if (useMatrixLookup)
                {
                    writtenPositions[entity->mTransformLookupNumber] = true;
                }
                else
                {
                    ++updatedInstances;
                }
            }

            ++itor;
        }

        if (!useMatrixLookup)
        {
            renderedInstances = updatedInstances;
        }

        mMatrixTexture->getBuffer()->unlock();

        return renderedInstances;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::_boundsDirty(void)
    {
        //Don't update if we're static, but still mark we're dirty
        if(!mBoundsDirty && !mKeepStatic && mCreator)
            mCreator->_addDirtyBatch( this );
        mBoundsDirty = true;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::setStaticAndUpdate(bool bStatic)
    {
        //We were dirty but didn't update bounds. Do it now.
        if(mKeepStatic && mBoundsDirty)
            mCreator->_addDirtyBatch(this);

        mKeepStatic = bStatic;
        if(mKeepStatic)
        {
            //One final update, since there will be none from now on
            //(except further calls to this function). ShaderCh NULL because
            //we want to include only those who were added to the scene
            //but we don't want to perform culling
            mRenderOperation.mInstanceCount = updateVertexTexture(0);
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW_VTF::queue(RenderQueue * queue)
    {
        if(!mKeepStatic)
        {
            //Completely override base functionality, since we don't cull on an "all-or-nothing" basis
            if((mRenderOperation.mInstanceCount = updateVertexTexture(mCurrentCamera)))
                queue->add( this );
        }
        else
        {
            if(mManager->isCameraOrigin())
            {
                N_EXCEPT(InvalidState, _I18n("Camera-relative rendering is incompatible")
                    _I18n(" with Instancing's static batches. Disable at least one of them"));
            }

            //Don't update when we're static
            if(mRenderOperation.mInstanceCount)
                queue->add(this);
        }
    }
    //-----------------------------------------------------------------------
}
