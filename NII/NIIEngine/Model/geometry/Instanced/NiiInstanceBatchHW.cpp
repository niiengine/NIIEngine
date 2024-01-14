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
#include "NiiInstanceBatchHW.h"
#include "NiiGBufferManager.h"
#include "NiiInstanceGeo.h"
#include "NiiMaterial.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    InstanceBatchHW::InstanceBatchHW(InstanceMesh * creator, Mesh * mesh, Material * material, 
        NCount count, const BoneIndexList * bonemap, Nid bid) :
            InstanceBatch(creator, mesh, material, count, bonemap, bid),
            mKeepStatic(false)
    {
        //Override defaults, so that InstancedEntities don't create a skeleton instance
        mTechnSupportsSkeletal = false;
    }
    //-----------------------------------------------------------------------
    InstanceBatchHW::~InstanceBatchHW()
    {
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchHW::calculateMaxNumInstances(const SubMesh * baseSubMesh, Nui16 flags) const
    {
        NCount retVal = 0;

        RenderSys * rsys = N_Engine().getRender();
        const RenderFeature * capabilities = rsys->getFeature();

        if(capabilities->isSupport(GF_Vertex_Instance))
        {
            //This value is arbitrary (theorical max is 2^30 for D3D9) but is big enough and safe
            retVal = 65535;
        }

        return retVal;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::buildFrom(const SubMesh * obj, const GeometryRaw & geo)
    {
        InstanceBatch::buildFrom(obj, geo);

        //We need to clone the VertexData (but just reference all buffers, except the last one)
        //because last buffer contains data specific to this batch, we need a different binding
        mRenderOperation.mVertex = mRenderOperation.mVertex->clone(false);
        VertexData * vd = mRenderOperation.mVertex;
        const Nui16 lastSource = vd->getFreeBufferIndex() - 1;
        VertexBuffer * vertexBuffer;
        N_Only(GBuffer).create(vertexBuffer, vd->getUnitSize(lastSource), mInstancesPerBatch, Buffer::M_NORMAL );
        vd->attach(lastSource, vertexBuffer);
        VertexUnitList tempu;
        vd->getUnits(lastSource, tempu);
        VertexUnitList::iterator i, iend = tempu.end();
        for (i = tempu.begin(); i != iend; ++i)
            i->mDivisor = 1;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::setupVertices(const SubMesh * baseSubMesh)
    {
        mRenderOperation.mVertex = baseSubMesh->getVertexData()->clone();
        mRemoveOwnVertexData = true; //Raise flag to remove our own vertex data in the end (not always needed)

        VertexData * thisVertexData = mRenderOperation.mVertex;

        //No skeletal animation support in this technique, sorry
        removeBlendData();

        //Modify the declaration so it contains an extra source, where we can put the per instance data
        NCount offset = 0;
        Nui16 nextTexCoord = thisVertexData->getCount(VT_Tex_Coord);
        const Nui16 newSource = thisVertexData->getFreeBufferIndex();
        for(NIIi i = 0; i < 3; ++i)
        {
            thisVertexData->add(DT_Float4x, VT_Tex_Coord, newSource, offset, nextTexCoord++);
            offset = thisVertexData->getUnitSize(newSource);
        }

        //Create the vertex buffer containing per instance data
        VertexBuffer * vertexBuffer;
        N_Only(GBuffer).create(vertexBuffer, thisVertexData->getUnitSize(newSource), mInstancesPerBatch, Buffer::M_NORMAL );
        thisVertexData->attach(newSource, vertexBuffer);
        VertexUnitList tempu;
        thisVertexData->getUnits(newSource, tempu);
        VertexUnitList::iterator i, iend = tempu.end();
        for (i = tempu.begin(); i != iend; ++i)
            i->mDivisor = 1;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::setupIndices(const SubMesh * obj)
    {
        //We could use just a reference, but the InstanceMesh will in the end attampt to delete
        //the pointer, and we can't give it something that doesn't belong to us.
        mRenderOperation.mIndex = obj->getIndexData(0)->clone(true);
        mRemoveOwnIndexData = true;    //Raise flag to remove our own index data in the end (not always needed)
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::removeBlendData()
    {
        VertexData * thisVertexData = mRenderOperation.mVertex;

        Nui16 safeSource = 0xFFFF;
        const VertexUnit * blendIndexElem = thisVertexData->get(VT_Matrix_Index);
        if(blendIndexElem)
        {
            //save the source in order to prevent the next stage from unbinding it.
            safeSource = blendIndexElem->mSource;
            // Remove buffer reference
            thisVertexData->detach(blendIndexElem->mSource);
        }
        // Remove blend weights
        const VertexUnit * blendWeightElem = thisVertexData->get(VT_Matrix_Weight);
        if(blendWeightElem && blendWeightElem->mSource != safeSource)
        {
            // Remove buffer reference
            thisVertexData->detach(blendWeightElem->mSource);
        }

        thisVertexData->remove(VT_Matrix_Index);
        thisVertexData->remove(VT_Matrix_Weight);
        thisVertexData->opt();
    }
    //-----------------------------------------------------------------------
    bool InstanceBatchHW::checkSubMeshCompatibility(const SubMesh * obj)
    {
        //Max number of texture coordinates is _usually_ 8, we need at least 3 available
        if(obj->getVertexData()->getCount(VT_Tex_Coord) > 8 - 2)
        {
            N_EXCEPT(UnImpl, _I18n("Given mesh must have at least 3 free TEXCOORDs"));
        }

        return InstanceBatch::checkSubMeshCompatibility(obj);
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchHW::updateVertexBuffer(Camera * currentCamera)
    {
        NCount retVal = 0;

        //Now lock the vertex buffer and copy the 4x3 matrices, only those who need it!
        Nui16 bufferIdx = mRenderOperation.mVertex->getAttachCount()-1;
        NIIf * pDest = (NIIf *)mRenderOperation.mVertex->getAttach(bufferIdx)->lock(Buffer::MM_WHOLE);

        InstanceGeoVec::const_iterator itor = mInstancedEntities.begin();
        InstanceGeoVec::const_iterator end  = mInstancedEntities.end();

        while(itor != end)
        {
            //Cull on an individual basis, the less entities are visible, the less instances we draw.
            //No need to use null matrices at all!
            if( (*itor)->findVisible( currentCamera ) )
            {
                const NCount floatsWritten = (*itor)->getTransforms3x4(pDest);

                if( mManager->isCameraOrigin())
                    makeMatrixCameraRelative3x4(pDest, floatsWritten);

                pDest += floatsWritten;

                ++retVal;
            }
            ++itor;
        }

        mRenderOperation.mVertex->getAttach(bufferIdx)->unlock();

        return retVal;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::_boundsDirty(void)
    {
        //Don't update if we're static, but still mark we're dirty
        if(!mBoundsDirty && !mKeepStatic)
            mCreator->_addDirtyBatch(this);
        mBoundsDirty = true;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::setStaticAndUpdate(bool bStatic)
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
            mRenderOperation.mInstanceCount = updateVertexBuffer(0);
        }
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = Matrix4f::IDENTITY;
        count = 1;
    }
    //-----------------------------------------------------------------------
    NCount InstanceBatchHW::getMatrixCount() const
    {
        return 1;
    }
    //-----------------------------------------------------------------------
    void InstanceBatchHW::queue(RenderQueue * queue)
    {
        if(!mKeepStatic)
        {
            //Completely override base functionality, since we don't cull on an "all-or-nothing" basis
            //and we don't support skeletal animation
            if((mRenderOperation.mInstanceCount = updateVertexBuffer(mCurrentCamera)))
                queue->add(this);
        }
        else
        {
            if(mManager->isCameraOrigin())
            {
                N_EXCEPT(InvalidState,
                    _I18n("Camera-relative rendering is incompatible"
                    " with Instancing's static batches. Disable at least one of them"));
            }

            //Don't update when we're static
            if(mRenderOperation.mInstanceCount)
                queue->add(this);
        }
    }
    //-----------------------------------------------------------------------
}