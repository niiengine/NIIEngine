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
#include "NiiMeshSerializerExt.h"
#include "NiiMeshFileFormat.h"
#include "NiiMesh.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiSkeleton.h"
#include "NiiGBufferManager.h"
#include "NiiVertexOffset.h"
#include "NiiMaterial.h"
#include "NiiVertexKeyFrameTrack.h"
#include "NiiVertexUnitKeyFrame.h"
#include "NiiMeshLodScheme.h"
#include "NiiLodSchemeManager.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer_v1_41
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer_v1_41::MeshSerializer_v1_41(const DataStream * src) :
        MeshSerializer(src)
    {
        mVersion = 17000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_41::MeshSerializer_v1_41(const FileSerializer & obj) :
        MeshSerializer(obj)
    {
        mVersion = 17000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_41::~MeshSerializer_v1_41()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_41::writeVertexUnitKeyFrame(const VertexKeyFrameTrack * track,
        const VertexUnitKeyFrame * kf)
    {
        Nui16 t1 = M_ANIMATION_MORPH_KEYFRAME;
        NCount vertexCount = track->getTarget()->mCount;
        Nui32 t2 = getVertexUnitKeyFrameSize(kf, vertexCount);
        write(&t1);
        write(&t2);
        // NIIf time
        TimeDurMS timePos = kf->getTime();
        write(&timePos);
        // NIIf x,y,z            // repeat by number of vertices in original geometry
        NIIf * pSrc = (NIIf *)kf->getValue()->lock(Buffer::MM_READ);
        write(pSrc, vertexCount * 3);
        kf->getValue()->unlock();
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_41::readVertexUnitKeyFrame(VertexKeyFrameTrack * out)
    {
        // NIIf time
        TimeDurMS timePos;
        read(&timePos, 1);

        VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(out->create(timePos));

        // Create buffer, allow read and use shadow buffer
        NCount vertexCount = out->getTarget()->mCount;
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, DataTypeUtil::getSize(DT_Float3x), vertexCount, M_NORMAL );
        // NIIf x,y,z            // repeat by number of vertices in original geometry
        NIIf * pDst = static_cast<NIIf*>(vbuf->lock(Buffer::MM_WHOLE));
        read(pDst, vertexCount * 3);
        vbuf->unlock();
        kf->setValue(vbuf);
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_41::writeVertexOffset(const VertexOffset * pose)
    {
        Nui16 t1 = M_POSE;
        Nui32 t2 = getVertexOffsetSize(pose);
        write(&t1);
        write(&t2);
        t2 = pose->getID();
        // NIIb* name (may be blank)
        write(&t2);

        // Nui16 target
        t1 = pose->getTarget();
        write(&t1, 1);

        // Nul vertexIndex NIIf xoffset, yoffset, zoffset
        t2 = ChunkHeaderSize + sizeof(Nui32) + sizeof(NIIf) * 3;

        VertexOffset::OffsetMap::const_iterator i, iend = pose->getVertexOffsets().end();
        for(i = pose->getVertexOffsets().begin(); i != iend; ++i)
        {
            Nui32 vertexIndex = (Nui32)i->first;
            Vector3f offset = i->second;
            t1 = M_POSE_VERTEX;
            write(&t1, 1);
            write(&t2, 1);
            // Nul vertexIndex
            write(&vertexIndex, 1);
            // NIIf xoffset, yoffset, zoffset
            write(&offset.x, 3);
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_41::readVertexOffset(Mesh * out)
    {
        // Nui32 name (may be blank)
        Nui32 name;
        // Nui16 target
        Nui16 target;

        read(&name, 1);
        read(&target, 1);

        VertexOffset * pose = out->createOffset(name, target);

        // Find all substreams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && (streamID == M_POSE_VERTEX))
            {
                switch(streamID)
                {
                case M_POSE_VERTEX:
                    // create vertex offset
                    Nui32 vertIndex;
                    Vector3f offset;
                    // Nul vertexIndex
                    read(&vertIndex, 1);
                    // NIIf xoffset, yoffset, zoffset
                    read(&offset.x, 3);

                    pose->add(vertIndex, offset);
                    break;

                }

                if (!mStream->end())
                {
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }

            }
            if (!mStream->end())
            {
                // Backpedal back to start of stream
                mStream->skip(-ChunkHeaderSize);
            }
        }
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer_v1_41::getVertexOffsetSize(const VertexOffset * pose)
    {
        NCount size = ChunkHeaderSize;

        // NIIb* name (may be blank)
        //size += pose->getName().length() + 1;
        size += sizeof(Nui32);
        // Nui16 target
        size += sizeof(Nui16);

        // vertex offsets
        // Nul vertexIndex NIIf xoffset, yoffset, zoffset
        size += pose->getVertexOffsets().size() * (ChunkHeaderSize + sizeof(Nui32) + sizeof(NIIf) * 3);

        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer_v1_41::getVertexUnitKeyFrameSize(const VertexUnitKeyFrame * kf,
        NCount vertexCount)
    {
        NCount size = ChunkHeaderSize;
        // NIIf time
        size += sizeof(NIIf);
        // NIIf x,y,z
        size += sizeof(NIIf) * 3 * vertexCount;

        return size;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer_v1_4
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer_v1_4::MeshSerializer_v1_4(const DataStream * src):
        MeshSerializer_v1_41(src)
    {
        mVersion = 14000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_4::MeshSerializer_v1_4(const FileSerializer & obj):
        MeshSerializer_v1_41(obj)
    {
        mVersion = 14000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_4::~MeshSerializer_v1_4()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_4::writeLod(const Mesh * mesh)
    {
        MeshLodScheme * scheme = mesh->getLodScheme();
        NCount numLods = scheme->getCount();

        // Header
        Nui16 t1 = M_MESH_LOD;
        Nui32 size = ChunkHeaderSize;
        // Nui32 id
        size += sizeof(Nui32);
        // Nui32 threshold
        size += sizeof(Nui32);
        // Nui32 factor
        size += sizeof(Nui32);
        // Nui16 numLevels;
        size += sizeof(Nui16);
        // Nui16 numrelLevels;
        size += sizeof(Nui16);

        write(t1);
        write(size);

        // Nui32 id
        write(scheme->getTypeID());
        // Nui32 threshold
        write(scheme->getThreshold());
        // Nui32 factor
        write(scheme->getFactor());
        // Nui16 numLevels;
        write((Nui16)numLods);
        // Nui16 numLevels;
        write((Nui16)scheme->getRelationCount());

        // Loop from LOD 1 (not 0, this is full detail)
        for(Nidx lodidx = 1; lodidx < numLods; ++lodidx)
        {
            const MeshLodScheme::MeshLod & usage = scheme->getInfo(lodidx); // 需要改变
            bool relat = scheme->isRelation(lodidx);
            if(relat)
            {
                // Usage Header
                size = ChunkHeaderSize;
                // NIIf fromDepthSquared;
                size += sizeof(NIIf);
                // bool rellod
                size += sizeof(bool);

                NCount subidx, subidxend = mesh->getSubCount();
                // Calc generated SubMesh sections size
                for(subidx = 0; subidx < subidxend; ++subidx)
                {
                    // header
                    size += ChunkHeaderSize;
                    // Nui32 numFaces;
                    size += sizeof(Nui32);
                    // bool indexes32Bit
                    size += sizeof(bool);

                    const IndexData * idxdata = mesh->getSub(subidx)->getIndexData(lodidx);

                    if(idxdata->mBuffer != 0 && idxdata->mBuffer->getUnitSize() == 32)
                    {
                        size += sizeof(Nui32) * idxdata->mCount;
                    }
                    else
                    {
                        size += sizeof(Nui16) * idxdata->mCount;
                    }
                }
                t1 = M_MESH_LOD_USAGE;
                write(t1);
                write(size);
                // Main difference to later version here is that we use 'value' (squared depth)
                // rather than 'mSrc' which is just depth
                write(usage.mSrc);
                write(true);

                // Now write sections
                // Calc generated SubMesh sections size
                for(subidx = 0; subidx < subidxend; ++subidx)
                {
                    size = ChunkHeaderSize;
                    // Nui32 numFaces;
                    size += sizeof(Nui32);
                    // bool idx32
                    size += sizeof(bool);

                    const IndexData * idxdata = mesh->getSub(subidx)->getIndexData(lodidx);
                    IndexBuffer * ibuf = idxdata->getAttach();

                    bool idx32 = (ibuf != 0 && ibuf->getUnitSize() == 32);
                    if(idx32)
                    {
                        size += sizeof(Nui32) * idxdata->mCount;
                    }
                    else
                    {
                        size += sizeof(Nui16) * idxdata->mCount;
                    }

                    Nui32 idxCount = idxdata->mCount;
                    t1 = M_MESH_LOD_GENERATED;
                    write(t1);
                    write(size);
                    write(idxCount);
                    write(idx32);

                    if(idxCount > 0)
                    {
                        if(idx32)
                        {
                            Nui32 * pIdx = (Nui32 *)ibuf->lock(Buffer::MM_READ);
                            write(pIdx, idxdata->mCount);
                            ibuf->unlock();
                        }
                        else
                        {
                            Nui16 * pIdx = (Nui16 *)ibuf->lock(Buffer::MM_READ);
                            write(pIdx, idxdata->mCount);
                            ibuf->unlock();
                        }
                    }
                }
            }
            else
            {
                // Header
                size = ChunkHeaderSize;
                // NIIf fromDepthSquared;
                size += sizeof(NIIf);
                // bool rellod
                size += sizeof(bool);

                // Manual part size
                Nui32 manualSize = ChunkHeaderSize;
                // ResourceID mRid;
                manualSize += sizeof(usage.mRid);
                size += manualSize;
                t1 = M_MESH_LOD_USAGE;

                write(t1);
                write(size);
                // Main difference to later version here is that we use 'value' (squared depth)
                // rather than 'mSrc' which is just depth
                write(usage.mSrc);
                write(false);

                t1 = M_MESH_LOD_MANUAL;
                write(t1);
                write(manualSize);
                write(usage.mRid);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_4::readLod(Mesh * out)
    {
        SchemeID sid;
        read(&sid);
        // Use the old strategy for this mesh
        MeshLodScheme * scheme = static_cast<MeshLodScheme *>(N_Only(LodScheme).create(N_LOD_Mesh, sid));
        out->setLodScheme(scheme);

        Nui16 streamID;

        Nui32 threshold;
        Nui32 factor;
        Nui16 lodcount;
        Nui16 rellodcount;

        read(&threshold);
        read(&factor);
        read(&lodcount);
        read(&rellodcount);

        scheme->setThreshold(threshold);
        scheme->setFactor(factor);
        scheme->setCount(lodcount);
        // Preallocate submesh lod face data if not manual
        NCount lodidx, numsubs = out->getSubCount();
        for(lodidx = 0; lodidx < numsubs; ++lodidx)
        {
            out->getSub(lodidx)->setLodCount(rellodcount);
        }
        bool rellod;
        Nui32 usagedst;
        NIIf usagesrc;
        // Loop from 1 rather than 0 (full detail index is not in file)
        for(lodidx = 1; lodidx < lodcount; ++lodidx)
        {
            read(&streamID);
            read(&mChunkSize);

            if(streamID != M_MESH_LOD_USAGE)
            {
                N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_USAGE stream in ") + 
                    N_conv(out->getOriginID()));
            }
            // Read depth
            read(&usagesrc);
            read(&rellod);
            usagedst = scheme->getDest(usagesrc);

            MeshLodScheme::MeshLod usage(usagesrc, usagedst, rellod);
            if(rellod)
            {
                usage.mRid = 0;
                usage.mMesh = NULL;

                // Get one set of detail per SubMesh
                for(NCount i = 0; i < numsubs; ++i)
                {
                    read(&streamID, 1);
                    read(&mChunkSize, 1);

                    if(streamID != M_MESH_LOD_GENERATED)
                    {
                        N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_GENERATED stream in ") + 
                            N_conv(out->getOriginID()));
                    }

                    SubMesh * sm = out->getSub(i);
                    IndexData * indexData = N_new IndexData();
                    sm->setIndexData(lodidx, indexData);
                    // Nui32 numIndexes
                    Nui32 numIndexes;
                    // bool idx32Bit
                    bool idx32Bit;

                    read(&numIndexes, 1);
                    read(&idx32Bit, 1);

                    indexData->mCount = numIndexes;
                    IndexBuffer * buf;
                    // Nui16*/NIIi* faceIndexes;  ((v1, v2, v3) * numFaces)
                    if(idx32Bit)
                    {
                        N_Only(GBuffer).create(buf, 32, indexData->mCount, out->getIndexMode());
                        Nui32 * pIdx = (Nui32 *)buf->lock(0, buf->getSize(), Buffer::MM_WHOLE);

                        read(pIdx, indexData->mCount);
                    }
                    else
                    {
                        N_Only(GBuffer).create(buf, 16, indexData->mCount, out->getIndexMode());
                        Nui16 * pIdx = (Nui16 *)buf->lock(0, buf->getSize(), Buffer::MM_WHOLE);

                        read(pIdx, indexData->mCount);

                    }
                    buf->unlock();
                    indexData->attach(buf);
                }
            }
            else
            {
                // Read detail stream
                Nui16 streamID;
                read(&streamID, 1);
                read(&mChunkSize, 1);

                if(streamID != M_MESH_LOD_MANUAL)
                {
                    N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_MANUAL stream in ") + 
                        N_conv(out->getOriginID()));
                }

                read(&usage.mRid, 1);
                usage.mMesh = NULL; // will trigger load later
            }

            // Save usage
            scheme->add(usage);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer_v1_3
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer_v1_3::MeshSerializer_v1_3(const DataStream * src):
        MeshSerializer_v1_4(src)
    {
        mVersion = 13000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_3::MeshSerializer_v1_3(const FileSerializer & obj):
        MeshSerializer_v1_4(obj)
    {
        mVersion = 13000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_3::~MeshSerializer_v1_3()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_3::readEdgeList(Mesh * out)
    {
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && streamID == M_EDGE_LIST_LOD)
            {
                // Process single LOD

                // Nui16 lodIndex
                Nui16 lodIndex;
                read(&lodIndex, 1);
                // bool relation            // If manual, no edge data here, loaded from manual mesh
                bool relation;
                read(&relation, 1);
                // Only load in non-manual levels; others will be connected up by Mesh on demand
                if(relation)
                {
                    GeoEdget * ed = N_new GeoEdget();

                    // Read detail information of the edge list
                    // Nul numTriangles
                    Nui32 numTriangles;
                    read(&numTriangles, 1);
                    // Allocate correct amount of memory
                    ed->mTriangleList.resize(numTriangles);
                    ed->mNormalsList.resize(numTriangles);
                    ed->mViewPosList.resize(numTriangles);
                    // Nul numEdgeGroups
                    Nui32 numEdgeGroups;
                    read(&numEdgeGroups, 1);
                    // Allocate correct amount of memory
                    ed->mGeoList.resize(numEdgeGroups);
                    // Triangle* triangleList
                    Nui32 tmp[3];
                    for(NCount t = 0; t < numTriangles; ++t)
                    {
                        GeoEdget::Triangle& tri = ed->mTriangleList[t];
                        // Nul mIndexDataIndex
                        read(tmp, 1);
                        tri.mIndexDataIndex = tmp[0];
                        // Nul vertexSet
                        read(tmp, 1);
                        tri.mVertexDataIndex = tmp[0];
                        // Nul mVertIndex[3]
                        read(tmp, 3);
                        tri.mVertIndex[0] = tmp[0];
                        tri.mVertIndex[1] = tmp[1];
                        tri.mVertIndex[2] = tmp[2];
                        // Nul mLocalVertIndex[3]
                        read(tmp, 3);
                        tri.mLocalVertIndex[0] = tmp[0];
                        tri.mLocalVertIndex[1] = tmp[1];
                        tri.mLocalVertIndex[2] = tmp[2];
                        // NIIf normal[4]
                        read(&(ed->mNormalsList[t].x), 4);

                    }

                    // Assume the mesh is closed, it will update later
                    ed->mCloseUp = true;

                    for(Nui32 eg = 0; eg < numEdgeGroups; ++eg)
                    {
                        Nui16 streamID;
                        read(&streamID, 1);
                        read(&mChunkSize, 1);

                        if (streamID != M_EDGE_GROUP)
                        {
                            N_EXCEPT(Internal, _I18n("Missing M_EDGE_GROUP stream"));
                        }
                        GeoEdget::EdgeGroup & edgeGroup = ed->mGeoList[eg];

                        // Nul mVertexDataIndex
                        read(tmp, 1);
                        edgeGroup.mVertexDataIndex = tmp[0];
                        // Nul numEdges
                        Nui32 numEdges;
                        read(&numEdges, 1);
                        edgeGroup.mEdgeList.resize(numEdges);
                        // Edge* edgeList
                        for(Nui32 e = 0; e < numEdges; ++e)
                        {
                            GeoEdget::Edge & edge = edgeGroup.mEdgeList[e];
                            // Nul  mTriIndex[2]
                            read(tmp, 2);
                            edge.mTriIndex[0] = tmp[0];
                            edge.mTriIndex[1] = tmp[1];
                            // Nul  mVertIndex[2]
                            read(tmp, 2);
                            edge.mVertIndex[0] = tmp[0];
                            edge.mVertIndex[1] = tmp[1];
                            // Nul  mLocalVertIndex[2]
                            read(tmp, 2);
                            edge.mLocalVertIndex[0] = tmp[0];
                            edge.mLocalVertIndex[1] = tmp[1];
                            // bool mAlone
                            read(&(edge.mAlone), 1);

                            // The mesh is closed only if no mAlone edge here
                            if (edge.mAlone)
                            {
                                ed->mCloseUp = false;
                            }
                        }
                    }
                    reorganiseTriangles(ed);

                    // Postprocessing edge groups
                    GeoEdget::EdgeGroupList::iterator egi, egend = ed->mGeoList.end();
                    for(egi = ed->mGeoList.begin(); egi != egend; ++egi)
                    {
                        GeoEdget::EdgeGroup & edgeGroup = *egi;
                        // Populate edgeGroup.vertexData pointers
                        // If there is shared vertex data, mVertexDataIndex 0 is that,
                        // otherwise 0 is first dedicated
                        if(out->getVertexData())
                        {
                            if(edgeGroup.mVertexDataIndex == 0)
                            {
                                edgeGroup.mVertex = out->getVertexData();
                            }
                            else
                            {
                                edgeGroup.mVertex = out->getSub(
                                    edgeGroup.mVertexDataIndex-1)->getVertexData();
                            }
                        }
                        else
                        {
                            edgeGroup.mVertex = out->getSub(
                                edgeGroup.mVertexDataIndex)->getVertexData();
                        }
                    }
                    out->setEdgeList(lodIndex, ed);
                }

                if(!mStream->end())
                {
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }
            }
            if(!mStream->end())
            {
                // Backpedal back to start of stream
                mStream->skip(-ChunkHeaderSize);
            }
        }

        out->getLodScheme()->setValid(true);
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_3::reorganiseTriangles(GeoEdget * ed)
    {
        NCount numTriangles = ed->mTriangleList.size();

        if(ed->mGeoList.size() == 1)
        {
            // Special case for only one edge group in the edge list, which occuring
            // most time. In this case, all triangles belongs to that group.
            ed->mGeoList.front().mOffset = 0;
            ed->mGeoList.front().mCount = numTriangles;
        }
        else
        {
            GeoEdget::EdgeGroupList::iterator egi, egend = ed->mGeoList.end();

            // Calculate number of triangles for edge groups
            for(egi = ed->mGeoList.begin(); egi != egend; ++egi)
            {
                egi->mOffset = 0;
                egi->mCount = 0;
            }

            bool isGrouped = true;
            GeoEdget::EdgeGroup * lastEdgeGroup = 0;
            for(NCount t = 0; t < numTriangles; ++t)
            {
                // Gets the edge group that the triangle belongs to
                const GeoEdget::Triangle & tri = ed->mTriangleList[t];
                GeoEdget::EdgeGroup * edgeGroup = &ed->mGeoList[tri.mVertexDataIndex];

                // Does edge group changes from last edge group?
                if (isGrouped && edgeGroup != lastEdgeGroup)
                {
                    // Remember last edge group
                    lastEdgeGroup = edgeGroup;

                    // Is't first time encounter this edge group?
                    if (!edgeGroup->mCount && !edgeGroup->mOffset)
                    {
                        // setup first triangle of this edge group
                        edgeGroup->mOffset = t;
                    }
                    else
                    {
                        // original triangles doesn't grouping by edge group
                        isGrouped = false;
                    }
                }

                // Count number of triangles for this edge group
                if(edgeGroup)
                    ++edgeGroup->mCount;
            }

            //
            // Note that triangles has been sorted by vertex set for a NIIl time,
            // but never stored to old version mesh file.
            //
            // Adopt this fact to avoid remap triangles here.
            //

            // Does triangles grouped by vertex set?
            if (!isGrouped)
            {
                // Ok, the triangles of this edge list isn't grouped by vertex set
                // perfectly, seems ancient mesh file.
                //
                // We need work hardly to group triangles by vertex set.
                //

                // Calculate mOffset and reset mCount to zero for each edge group first
                NCount triStart = 0;
                for(egi = ed->mGeoList.begin(); egi != egend; ++egi)
                {
                    egi->mOffset = triStart;
                    triStart += egi->mCount;
                    egi->mCount = 0;
                }

                // The map used to mapping original triangle index to new index
                typedef vector<NCount>::type TriangleIndexRemap;
                TriangleIndexRemap triangleIndexRemap(numTriangles);

                // New triangles information that should be group by vertex set.
                GeoEdget::TriangleList newTriangles(numTriangles);
                GeoEdget::NormalsList newTriangleFaceNormals(numTriangles);

                // Calculate triangle index map and organise triangles information
                for(NCount t = 0; t < numTriangles; ++t)
                {
                    // Gets the edge group that the triangle belongs to
                    const GeoEdget::Triangle & tri = ed->mTriangleList[t];
                    GeoEdget::EdgeGroup & edgeGroup = ed->mGeoList[tri.mVertexDataIndex];

                    // Calculate new index
                    NCount newIndex = edgeGroup.mOffset + edgeGroup.mCount;
                    ++edgeGroup.mCount;

                    // Setup triangle index mapping entry
                    triangleIndexRemap[t] = newIndex;

                    // Copy triangle info to new placement
                    newTriangles[newIndex] = tri;
                    newTriangleFaceNormals[newIndex] = ed->mNormalsList[t];
                }

                // Replace with new triangles information
                ed->mTriangleList.swap(newTriangles);
                ed->mNormalsList.swap(newTriangleFaceNormals);

                // Now, update old triangle indices to new index
                for(egi = ed->mGeoList.begin(); egi != egend; ++egi)
                {
                    GeoEdget::EdgeList::iterator ei, eend;
                    eend = egi->mEdgeList.end();
                    for (ei = egi->mEdgeList.begin(); ei != eend; ++ei)
                    {
                        ei->mTriIndex[0] = triangleIndexRemap[ei->mTriIndex[0]];
                        if (!ei->mAlone)
                        {
                            ei->mTriIndex[1] = triangleIndexRemap[ei->mTriIndex[1]];
                        }
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_3::writeEdgeList(const Mesh * pMesh)
    {
        Nui16 t1 = M_EDGE_LISTS;
        Nui32 t2 = getEdgeListSize(pMesh);
        write(&t1);
        write(&t2);
        for(Nui16 i = 0; i < pMesh->getLodCount(); ++i)
        {
            const GeoEdget * ed = pMesh->getEdgeList(i);
            bool isManual = !pMesh->getLodScheme()->isRelation(i) && (i > 0);
            t1 = M_EDGE_LIST_LOD;
            t2 = getLodEdgeListSize(ed, isManual);

            write(&t1, 1);
            write(&t2, 1);
            // Nui16 lodIndex
            write(&i, 1);
            // bool isManual            // If manual, no edge data here, loaded from manual mesh
            write(&isManual, 1);

            if (!isManual)
            {
                // Nul  numTriangles
                Nui32 count = ed->mTriangleList.size();
                write(&count, 1);
                // Nul numEdgeGroups
                count = ed->mGeoList.size();
                write(&count, 1);
                // Triangle* triangleList
                // Iterate rather than writing en-masse to allow endian conversion
                GeoEdget::TriangleList::const_iterator t = ed->mTriangleList.begin();
                GeoEdget::NormalsList::const_iterator fni = ed->mNormalsList.begin();
                for ( ; t != ed->mTriangleList.end(); ++t, ++fni)
                {
                    const GeoEdget::Triangle& tri = *t;
                    // Nul mIndexDataIndex;
                    Nui32 tmp[3];
                    tmp[0] = tri.mIndexDataIndex;
                    write(tmp, 1);
                    // Nul vertexSet;
                    tmp[0] = tri.mVertexDataIndex;
                    write(tmp, 1);
                    // Nul mVertIndex[3];
                    tmp[0] = tri.mVertIndex[0];
                    tmp[1] = tri.mVertIndex[1];
                    tmp[2] = tri.mVertIndex[2];
                    write(tmp, 3);
                    // Nul mLocalVertIndex[3];
                    tmp[0] = tri.mLocalVertIndex[0];
                    tmp[1] = tri.mLocalVertIndex[1];
                    tmp[2] = tri.mLocalVertIndex[2];
                    write(tmp, 3);
                    // NIIf normal[4];
                    write(&(fni->x), 4);
                }
                // Write the groups
                for (GeoEdget::EdgeGroupList::const_iterator gi = ed->mGeoList.begin();
                    gi != ed->mGeoList.end(); ++gi)
                {
                    const GeoEdget::EdgeGroup & edgeGroup = *gi;
                    t1 = M_EDGE_GROUP;
                    t2 = getEdgeGroupSize(edgeGroup);
                    write(&t1, 1);
                    write(&t2, 1);
                    // Nul mVertexDataIndex
                    Nui32 vertexSet = (Nui32)edgeGroup.mVertexDataIndex;
                    write(&vertexSet, 1);
                    // Nul numEdges
                    count = (Nui32)edgeGroup.mEdgeList.size();
                    write(&count, 1);
                    // Edge* edgeList
                    // Iterate rather than writing en-masse to allow endian conversion
                    for (GeoEdget::EdgeList::const_iterator ei = edgeGroup.mEdgeList.begin();
                        ei != edgeGroup.mEdgeList.end(); ++ei)
                    {
                        const GeoEdget::Edge& edge = *ei;
                        Nui32 tmp[2];
                        // Nul  mTriIndex[2]
                        tmp[0] = edge.mTriIndex[0];
                        tmp[1] = edge.mTriIndex[1];
                        write(tmp, 2);
                        // Nul  mVertIndex[2]
                        tmp[0] = edge.mVertIndex[0];
                        tmp[1] = edge.mVertIndex[1];
                        write(tmp, 2);
                        // Nul  mLocalVertIndex[2]
                        tmp[0] = edge.mLocalVertIndex[0];
                        tmp[1] = edge.mLocalVertIndex[1];
                        write(tmp, 2);
                        // bool mAlone
                        write(&(edge.mAlone), 1);
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer_v1_2
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer_v1_2::MeshSerializer_v1_2(const DataStream * src):
        MeshSerializer_v1_3(src)
    {
        mVersion = 12000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_2::MeshSerializer_v1_2(const FileSerializer & obj):
        MeshSerializer_v1_3(obj)
    {
        mVersion = 12000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_2::~MeshSerializer_v1_2()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_2::readMesh(Mesh * out)
    {
        MeshSerializer::readMesh(out);
        // Always automatically build edge lists for this version
        out->build(Mesh::M_AutoLod);
    }
    //---------------------------------------------------------------------
    void MeshSerializer_v1_2::readVertexData(Mesh * out, VertexData * dest)
    {
        Nui16 bindIdx = 0;

        dest->mOffset = 0;

        Nui32 vertexCount = 0;
        read(&vertexCount, 1);
        dest->mCount = vertexCount;

        // Vertex buffers

        readGeometryPositions(bindIdx, out, dest);
        ++bindIdx;

        // Find optional geometry streams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            Nui16 texCoordSet = 0;
            while(!mStream->end() &&
                (streamID == M_GEOMETRY_NORMALS ||
                 streamID == M_GEOMETRY_COLOURS ||
                 streamID == M_GEOMETRY_TEXCOORDS ))
            {
                switch (streamID)
                {
                case M_GEOMETRY_NORMALS:
                    readGeometryNormals(bindIdx++, out, dest);
                    break;
                case M_GEOMETRY_COLOURS:
                    readGeometryColours(bindIdx++, out, dest);
                    break;
                case M_GEOMETRY_TEXCOORDS:
                    readGeometryTexCoords(bindIdx++, out, dest, texCoordSet++);
                    break;
                }
                // Get next stream
                if(!mStream->end())
                {
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }
            }
            if (!mStream->end())
            {
                // Backpedal back to start of non-submesh stream
                mStream->skip(-ChunkHeaderSize);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_2::readGeometryPositions(Nui16 bindIdx, Mesh * pMesh, 
        VertexData * dest)
    {
        NIIf * pFloat = 0;
        VertexBuffer * vbuf;
        // NIIf* pVertices (x, y, z order x numVertices)
        dest->add(DT_Float3x, VT_Pos, bindIdx, 0);

        N_Only(GBuffer).create(vbuf, dest->getUnitSize(bindIdx), dest->mCount, pMesh->getVertexMode());

        pFloat = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);
        read(pFloat, dest->mCount * 3);
        vbuf->unlock();
        dest->attach(bindIdx, vbuf);
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_2::readGeometryNormals(Nui16 bindIdx, Mesh * pMesh, 
        VertexData * dest)
    {
        NIIf * pFloat = 0;
        VertexBuffer * vbuf;
        // NIIf* pNormals (x, y, z order x numVertices)
        dest->add(DT_Float3x, VT_Normals, bindIdx, 0);

        N_Only(GBuffer).create(vbuf, dest->getUnitSize(bindIdx), dest->mCount, pMesh->getVertexMode());

        pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        read(pFloat, dest->mCount * 3);
        vbuf->unlock();
        dest->attach(bindIdx, vbuf);
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_2::readGeometryColours(Nui16 bindIdx, Mesh * out, 
        VertexData * dest)
    {
        RGBA * pRGBA = 0;
        VertexBuffer * vbuf;
        // Nul* pColours (RGBA 8888 format x numVertices)
        dest->add(DT_Colour, VT_Diffuse, bindIdx, 0);

        N_Only(GBuffer).create(vbuf, dest->getUnitSize(bindIdx), dest->mCount, out->getVertexMode());

        pRGBA = (RGBA *)vbuf->lock(Buffer::MM_WHOLE);
        read(pRGBA, dest->mCount);
        vbuf->unlock();
        dest->attach(bindIdx, vbuf);
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_2::readGeometryTexCoords(Nui16 bindIdx, Mesh * out, 
        VertexData * dest, Nui16 texCoordSet)
    {
        NIIf * pFloat = 0;
        VertexBuffer * vbuf;
        // Nui16 dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
        Nui16 dim;
        read(&dim, 1);
        // NIIf* pTexCoords  (u [v] [w] order, dimensions x numVertices)
        dest->add(DataTypeUtil::getMultiType(DT_Float1x, dim), VT_Tex_Coord,
            bindIdx, 0, texCoordSet);

        N_Only(GBuffer).create(vbuf ,dest->getUnitSize(bindIdx), dest->mCount, out->getVertexMode());

        pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        read(pFloat, dest->mCount * dim);
        vbuf->unlock();
        dest->attach(bindIdx, vbuf);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer_v1_1
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer_v1_1::MeshSerializer_v1_1(const DataStream * src) :
        MeshSerializer_v1_2(src)
    {
        mVersion = 11000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_1::MeshSerializer_v1_1(const FileSerializer & obj) :
        MeshSerializer_v1_2(obj)
    {
        mVersion = 11000;
    }
    //------------------------------------------------------------------------
    MeshSerializer_v1_1::~MeshSerializer_v1_1()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer_v1_1::readGeometryTexCoords(Nui16 bindIdx, Mesh * out, 
        VertexData * dest, Nui16 texCoordSet)
    {
        NIIf * pFloat = 0;
        VertexBuffer * vbuf;
        // Nui16 dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
        Nui16 dim;
        read(&dim, 1);
        // NIIf* pTexCoords  (u [v] [w] order, dimensions x numVertices)
        dest->add(DataTypeUtil::getMultiType(DT_Float1x, dim), VT_Tex_Coord,
            bindIdx, 0, texCoordSet);

        N_Only(GBuffer).create(vbuf, dest->getUnitSize(bindIdx), dest->mCount, out->getVertexMode());

        pFloat = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        read(pFloat, dest->mCount * dim);

        // Adjust individual v values to (1 - v)
        if(dim == 2)
        {
            for(NCount i = 0; i < dest->mCount; ++i)
            {
                ++pFloat; // skip u
                *pFloat = 1.0f - *pFloat; // v = 1 - v
                ++pFloat;
            }

        }
        vbuf->unlock();
        dest->attach(bindIdx, vbuf);
    }
    //------------------------------------------------------------------------
}