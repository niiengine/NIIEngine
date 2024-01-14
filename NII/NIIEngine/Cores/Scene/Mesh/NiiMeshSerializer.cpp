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
#include "NiiMeshSerializer.h"
#include "NiiMeshFileFormat.h"
#include "NiiMaterial.h"
#include "NiiMesh.h"
#include "NiiLogManager.h"
#include "NiiSkeleton.h"
#include "NiiDefaultStream.h"
#include "NiiFileSerializerList.h"
#include "NiiGBufferManager.h"
#include "NiiMeshLodScheme.h"
#include "NiiLodUnitList.h"
#include "NiiVertexOffsetKeyFrameTrack.h"
#include "NiiVertexUnitKeyFrameTrack.h"
#include "NiiVertexOffsetKeyFrame.h"
#include "NiiVertexUnitKeyFrame.h"
#include "NiiEngine.h"

#if N_COMPILER == N_CPP_MSVC
    // Disable conversion warnings, we do a lot of them, intentionally
    #pragma warning (disable : 4267)
#endif

namespace NII
{
    //------------------------------------------------------------------------
    void __meshflip(Nui8 * pBase, const VertexUnitList & elems, NCount vertCnt,
        NCount vertexSize)
    {
        for(NCount v = 0; v < vertCnt; ++v)
        {
            VertexUnitList::const_iterator i, iend = elems.end();
            for(i = elems.begin(); i != iend; ++i)
            {
                Nui8 * pElem;
                // re-base pointer to the element
                i->getData(pBase, pElem);
                // Flip the endian based on the type
                NCount typeSize = 0;
                NCount typeCount = 0;
                switch(DataTypeUtil::getBaseType(i->mType, typeCount))
                {
                case DT_Float1x:
                    typeSize = sizeof(NIIf);
                    break;
                case DT_Short2x:
                    typeSize = sizeof(NIIs) * 2;
                    break;
                case DT_Colour:
                case DT_Colour_ABGR:
                case DT_Colour_ARGB:
                    typeSize = sizeof(RGBA);
                    break;
                case DT_UByte4x:
                    typeSize = 0; // NO FLIPPING
                    break;
                default:
                    N_assert(false, "error"); // Should never happen
                };
                Serializer::flip(pElem, typeSize, typeCount);
            }
            pBase += vertexSize;
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MeshSerializer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MeshSerializer::MeshSerializer(const DataStream * src) :
        FileSerializer(src)
    {
        mVersion = 30000;
    }
    //------------------------------------------------------------------------
    MeshSerializer::MeshSerializer(const FileSerializer & obj) :
        FileSerializer(obj)
    {
        mVersion = 30000;
    }
    //------------------------------------------------------------------------
    MeshSerializer::~MeshSerializer()
    {
    }
    //------------------------------------------------------------------------
    void MeshSerializer::write(const Mesh * in, const String & file, SerialVer ver,
        SerialFormat sf, Endian emode)
    {
        std::fstream * f = N_new_t(std::fstream)();
        f->open(file.c_str(), std::ios::binary | std::ios::out);
        DataStream * stream = N_new FileDataStream(f);

        write(in, stream, ver, sf, emode);

        N_delete stream;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::write(const Mesh * in, DataStream * stream, SerialVer ver,
        SerialFormat sf, Endian emode)
    {
        DataStream * old = mStream;
        mStream = stream;

        writeFileHeader();
        writeHeader();
        write(in, ver, sf, emode);
        reviseWrite();

        mStream = old;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::read(const String & file, Mesh * out)
    {
        std::fstream * f = N_new_t(std::fstream)();
        f->open(file.c_str(), std::ios::binary | std::ios::in);
        DataStream * stream = N_new FileDataStream(f);

        read(stream, out);

        N_delete stream;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::read(DataStream * in, Mesh * out)
    {
        DataStream * old = mStream;
        mStream = in;

        readFileHeader();
        readHeader();
        readUnit(out);

        mStream = old;
    }
    //------------------------------------------------------------------------
    bool MeshSerializer::isVersion(SerialVer ver) const
    {
        if(ver == 30000)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    SerializerID MeshSerializer::getUnit() const
    {
        return N_Serializer_Mesh_30000;
    }
    //------------------------------------------------------------------------
    GroupID MeshSerializer::getGroup() const
    {
        return GroupUnknow;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readUnit(void * out)
    {
        mCurrentObj = static_cast<Mesh *>(out);
        Nui16 chunkid;
        while(!mStream->end())
        {
            read(&chunkid, 1);
            read(&mChunkSize, 1);

            switch(chunkid)
            {
            case M_MESH:
                readMesh(mCurrentObj);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeUnit(const void * in, SerialVer ver, SerialFormat sf, Endian emode)
    {
        N_Only(Log).log(_I18n("MeshSerializer writing mesh data to stream :") + mStream->getName());

        check(emode);
        setFormat(sf);

        const Mesh * src = static_cast<const Mesh *>(in);

        if(src->getAABB().isNull() || src->getCenterRange() == 0.0f)
        {
            N_EXCEPT(InvalidParam, _I18n("The Mesh you have supplied does not have its bounds completely defined. Define them first before exporting."));
        }

        if(!mStream->isWriteable())
        {
            N_EXCEPT(InvalidParam, _I18n("不能写入数据流,名字: ") + mStream->getName());
        }

        N_Only(Log).log(_I18n("Writing mesh data..."));
        writeMesh(src);
        N_Only(Log).log(_I18n("Mesh data exported."));

        N_Only(Log).log(_I18n("MeshSerializer export successful."));
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeMesh(const Mesh * mesh)
    {
        // Header
        Nui16 t1 = M_MESH;
        Nui32 t2 = getMeshSize(mesh);
        // bool skelAnim
        bool skelAnim = mesh->isSkeletonFusion();

        write(&t1, 1);
        write(&t2, 1);
        write(&skelAnim, 1);

        // Write main geometry
        if(mesh->getVertexData())
            writeVertexData(mesh->getVertexData());

        // Write Submeshes
        for(NCount i = 0; i < mesh->getSubCount(); ++i)
        {
            N_Only(Log).log(_I18n("Writing submesh..."));
            writeSubMesh(mesh->getSub(i));
            N_Only(Log).log(_I18n("Submesh exported."));
        }

        // Write skeleton info if required
        if(mesh->isSkeletonFusion())
        {
            N_Only(Log).log(_I18n("Exporting skeleton link..."));
            // Write skeleton link
            writeSkeleton(mesh->getSkeletonID());
            N_Only(Log).log(_I18n("Skeleton link exported."));

            // Write bone assignments
            if(!mesh->getSkeletonList().empty())
            {
                N_Only(Log).log(_I18n("Exporting shared geometry bone assignments..."));

                SkeletonVertexList::const_iterator vi, viend = mesh->getSkeletonList().end();
                for(vi = mesh->getSkeletonList().begin(); vi != viend; ++vi)
                {
                    writeSkeletonAssign(vi->second);
                }

                N_Only(Log).log(_I18n("Shared geometry bone assignments exported."));
            }
        }

        // Write LOD data if any
        if(mesh->getLodCount() > 1)
        {
            N_Only(Log).log(_I18n("Exporting LOD information...."));
            writeLod(mesh);
            N_Only(Log).log(_I18n("LOD information exported."));

        }
        // Write bounds information
        N_Only(Log).log(_I18n("Exporting bounds information...."));
        writeAABB(mesh);
        N_Only(Log).log(_I18n("Bounds information exported."));

        // Write edge lists
        if(mesh->getLodScheme()->isValid())
        {
            N_Only(Log).log(_I18n("Exporting edge lists..."));
            writeEdgeList(mesh);
            N_Only(Log).log(_I18n("Edge lists exported"));
        }

        // Write morph animation
        writeVertexOffsetList(mesh);
        if(mesh->isVertexFusion())
        {
            writeAnimationList(mesh);
        }

        // Write submesh extremes
        writeEdgePoint(mesh);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeSubMesh(const SubMesh * sm)
    {
        // Header
        Nui16 t1 = M_SUBMESH;
        Nui32 t2 = getSubMeshSize(sm);
        Nui32 idxcnt = sm->getIndexData(0)->mCount;
        Nui16 geoType = static_cast<Nui16>(sm->getGeometryType());
        Nui32 mid = sm->getMaterial();
        bool svertex = sm->isSelfVertex();

        write(&t1);
        write(&t2);
        // Nui32 materialrid
        write(&mid);
        // Nui16 GeometryType
        write(&geoType);
        // String meshname
        write(sm->getName());
        // bool selfVertex
        write(&svertex);
        // Nui32 indexCount
        write(&idxcnt, 1);

        // bool Idx32
        bool idx32 = (sm->getIndexData(0)->getAttach() != 0 &&
            sm->getIndexData(0)->getAttach()->getUnitSize() == 32);
        write(&idx32, 1);

        if(idxcnt > 0)
        {
            // Nui16 * faceVertexIndices ((idxcnt)
            IndexBuffer * ibuf = sm->getIndexData(0)->getAttach();
            void * pIdx = ibuf->lock(Buffer::MM_READ);
            if(idx32)
            {
                Nui32 * pIdx32 = (Nui32 *)pIdx;
                write(pIdx32, sm->getIndexData(0)->mCount);
            }
            else
            {
                Nui16 * pIdx16 = (Nui16 *)pIdx;
                write(pIdx16, sm->getIndexData(0)->mCount);
            }
            ibuf->unlock();
        }

        // M_GEOMETRY stream (Optional: present only if mSelfVertex = true)
        if(sm->isSelfVertex())
        {
            writeVertexData(sm->getVertexData());
        }
        // end of sub mesh chunk

        // Bone assignments
        if(!sm->getSkeletonList().empty())
        {
            N_Only(Log).log(_I18n("Exporting dedicated geometry bone assignments..."));

            SkeletonVertexList::const_iterator i, iend = sm->getSkeletonList().end();
            for(i = sm->getSkeletonList().begin(); i != iend; ++i)
            {
                writeSubSkeletonAssign(i->second);
            }

            N_Only(Log).log(_I18n("Dedicated geometry bone assignments exported."));
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeEdgePoint(const Mesh * mesh)
    {
        bool has_extremes = false;
        for(NCount idx = 0; idx < mesh->getSubCount(); ++idx)
        {
            SubMesh * subm = mesh->getSub(idx);
            if(subm->getEdgePoints().empty())
                continue;

            if(!has_extremes)
            {
                has_extremes = true;
                N_Only(Log).log(_I18n("Writing submesh extremes..."));
            }

            Nui16 t1 = M_TABLE_EXTREMES;
            Nui32 chunkSize = ChunkHeaderSize + sizeof (Nui16) +
                subm->getEdgePoints().size () * sizeof (NIIf) * 3;

            write(&t1, 1);
            write(&chunkSize, 1);
            write(&idx, 1);

            NIIf * vertices = N_alloc_t(NIIf, subm->getEdgePoints().size() * 3);

            NIIf * data = vertices;
            vector<Vector3f >::type::const_iterator i, iend = subm->getEdgePoints().end();
            for(i = subm->getEdgePoints().begin(); i != iend; ++i)
            {
                *data++ = i->x;
                *data++ = i->y;
                *data++ = i->z;
            }
            write(vertices, subm->getEdgePoints().size () * 3);

            N_free(vertices);
        }
        if(has_extremes)
            N_Only(Log).log(_I18n("Extremes exported."));
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeVertexData(const VertexData * vertexData)
    {
        // calc size
        const VertexUnitList & elemList = vertexData->getUnits();
        const VertexData::BindingList & attachs = vertexData->getAttachs();
        VertexData::BindingList::const_iterator vbi, vbiend = attachs.end();

        // Header
        Nui16 t1 = M_GEOMETRY;
        Nui32 size = ChunkHeaderSize + sizeof(Nui32) + // base
            (ChunkHeaderSize + elemList.size() * (ChunkHeaderSize + sizeof(Nui16) * 5)); // elements

        for(vbi = attachs.begin(); vbi != vbiend; ++vbi)
        {
            size += (ChunkHeaderSize * 2) + (sizeof(Nui16) * 2) + vbi->second->getSize();
        }
        Nui32 vertcnt = vertexData->mCount;

        write(&t1, 1);
        write(&size, 1);
        write(&vertcnt, 1);

        // Vertex declaration
        size = ChunkHeaderSize + elemList.size() * (ChunkHeaderSize + sizeof(Nui16) * 5);
        t1 = M_GEOMETRY_VERTEX_DECLARATION;
        Nui16 tmp;

        write(&t1, 1);
        write(&size, 1);
        VertexUnitList::const_iterator vei, veiend = elemList.end();
        t1 = M_GEOMETRY_VERTEX_ELEMENT;
        size = ChunkHeaderSize + sizeof(Nui16) * 5;
        for(vei = elemList.begin(); vei != veiend; ++vei)
        {
            const VertexUnit & elem = *vei;
            write(&t1, 1);
            write(&size, 1);
            // Nui16 source;      // buffer bind source
            tmp = elem.mSource;
            write(&tmp, 1);
            // Nui16 type;        // DataType
            tmp = static_cast<Nui16>(elem.mType);
            write(&tmp, 1);
            // Nui16 semantic; // VertexType
            tmp = static_cast<Nui16>(elem.mVType);
            write(&tmp, 1);
            // Nui16 offset;    // start offset in buffer in bytes
            tmp = static_cast<Nui16>(elem.mOffset);
            write(&tmp, 1);
            // Nui16 index;    // index of the semantic (for colours and texture coords)
            tmp = elem.mIndex;
            write(&tmp, 1);

        }

        // Buffers and attachs
        vbiend = attachs.end();
        t1 = M_GEOMETRY_VERTEX_BUFFER;
        for(vbi = attachs.begin(); vbi != vbiend; ++vbi)
        {
            VertexBuffer * vbuf = vbi->second;
            size = (ChunkHeaderSize * 2) + (sizeof(Nui16) * 2) + vbuf->getSize();

            write(&t1);
            write(&size);
            // Nui16 bindIndex;    // Index to bind this buffer to
            tmp = vbi->first;
            write(&tmp);
            // Nui16 vertexSize;    // Per-vertex size, must agree with declaration at this index
            tmp = (Nui16)vbuf->getUnitSize();
            write(&tmp);

            // Data
            size = ChunkHeaderSize + vbuf->getSize();
            t1 = M_GEOMETRY_VERTEX_BUFFER_DATA;
            write(&t1, 1);
            write(&size, 1);
            void * pBuf = vbuf->lock(Buffer::MM_READ);

            if(mFlipEndian)
            {
                // endian conversion
                // Copy data
                Nui8 * temp = N_alloc_t(Nui8, vbuf->getSize());
                memcpy(temp, pBuf, vbuf->getSize());

                VertexUnitList unit;
                vertexData->getUnits(vbi->first, unit);
                __meshflip(temp, unit, vertexData->mCount, vbuf->getUnitSize());

                write(temp, vbuf->getUnitSize(), vertexData->mCount);
                N_free(temp);
            }
            else
            {
                write(pBuf, vbuf->getUnitSize(), vertexData->mCount);
            }
            vbuf->unlock();
        }
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getMeshSize(const Mesh * mesh)
    {
        NCount size = ChunkHeaderSize;

        // Num shared vertices
        size += sizeof(Nui32);

        // Geometry
        if(mesh->getVertexData() && mesh->getVertexData()->mCount > 0)
        {
            size += getGeometrySize(mesh->getVertexData());
        }

        // Submeshes
        for(NCount i = 0; i < mesh->getSubCount(); ++i)
        {
            size += getSubMeshSize(mesh->getSub(i));
        }

        // Skeleton link
        if(mesh->isSkeletonFusion())
        {
            size += ChunkHeaderSize + sizeof(Nui32);
        }

        // Edge list
        if(mesh->getLodScheme()->isValid())
        {
            size += getEdgeListSize(mesh);
        }

        // Animations
        for(Nidx a = 0; a < mesh->getAnimationCount(); ++a)
        {
            Animation * anim = mesh->getAnimation(a);
            size += getAnimationSize(anim);
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getSubMeshSize(const SubMesh * pSub)
    {
        NCount size = ChunkHeaderSize;

        IndexBuffer * idxbuf = pSub->getIndexData(0)->getAttach();
        bool idx32 = (idxbuf != 0 && idxbuf->getUnitSize() == 32);

        // Nui32 Material rid
        size += sizeof(Nui32);
        // Nui16 GeoType
        size += sizeof(Nui16);
        // String meshName;
        size += getSize(pSub->getName());
        // bool mSelfVertex
        size += sizeof(bool);
        // Nui32 idxcnt
        size += sizeof(Nui32);
        // bool indexes32bit
        size += sizeof(bool);
        // Nui32* / Nui16* faceVertexIndices
        if(idx32)
            size += sizeof(Nui32) * pSub->getIndexData(0)->mCount;
        else
            size += sizeof(Nui16) * pSub->getIndexData(0)->mCount;

        // Geometry
        if(pSub->isSelfVertex())
        {
            size += getGeometrySize(pSub->getVertexData());
        }

        // Bone assignments
        if(!pSub->getSkeletonList().empty())
        {
            SkeletonVertexList::const_iterator vi, viend = pSub->getSkeletonList().end();
            for(vi = pSub->getSkeletonList().begin(); vi != viend; ++vi)
            {
                size += (ChunkHeaderSize + sizeof(Nui32) + sizeof(Nui16) + sizeof(NIIf));
            }
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getGeometrySize(const VertexData * obj)
    {
        NCount size = ChunkHeaderSize;

        // Num vertices
        size += sizeof(Nui32);

        VertexUnitList::const_iterator i, iend = obj->getUnits().end();
        for(i = obj->getUnits().begin(); i != iend; ++i)
        {
            const VertexUnit & elem = *i;
            size += DataTypeUtil::getSize(elem.mType) * obj->mCount;
        }
        return size;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readVertexData(Mesh * out, VertexData * dest)
    {
        Nui32 vertcnt;
        read(&vertcnt);

        dest->mCount = vertcnt;
        dest->mOffset = 0;

        // Find optional geometry streams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && (streamID == M_GEOMETRY_VERTEX_DECLARATION ||
                streamID == M_GEOMETRY_VERTEX_BUFFER ))
            {
                switch(streamID)
                {
                case M_GEOMETRY_VERTEX_DECLARATION:
                {
                    // Find optional geometry streams
                    if(!mStream->end())
                    {
                        Nui16 streamID;
                        read(&streamID, 1);
                        read(&mChunkSize, 1);

                        while(!mStream->end() && (streamID == M_GEOMETRY_VERTEX_ELEMENT))
                        {
                            Nui16 source, offset, index, tmp;
                            DataType vType;
                            VertexType vSemantic;
                            // Nui16 source;      // buffer bind source
                            read(&source, 1);
                            // Nui16 type;        // DataType
                            read(&tmp, 1);
                            vType = static_cast<DataType>(tmp);
                            // Nui16 semantic; // VertexType
                            read(&tmp, 1);
                            vSemantic = static_cast<VertexType>(tmp);
                            // Nui16 offset;    // start offset in buffer in bytes
                            read(&offset, 1);
                            // Nui16 index;    // index of the semantic
                            read(&index, 1);

                            dest->add(vType, vSemantic, source, offset, index);

                            if(vType == DT_Colour)
                            {
                                N_Only(Log).stream()
                                    << _I18n("Warning: DT_Colour element type is deprecated,")
                                    << _I18n("you should use one of the more specific types to indicate the byte order.")
                                    << _I18n("Use NiiMeshUpgrade on ")<< out->getOriginID() << _I18n(" as soon as possible.");
                            }

                            // Get next stream
                            if(!mStream->end())
                            {
                                Nui16 streamID;
                                read(&streamID, 1);
                                read(&mChunkSize, 1);
                            }
                        }
                        if(!mStream->end())
                        {
                            // Backpedal back to start of non-submesh stream
                            mStream->skip(-ChunkHeaderSize);
                        }
                    }
                }
                break;
                case M_GEOMETRY_VERTEX_BUFFER:
                {
                    Nui16 bindIndex, vertexSize;
                    Nui16 headerID;

                    // Nui16 bindIndex;    // Index to bind this buffer to
                    read(&bindIndex, 1);
                    // Nui16 vertexSize;    // Per-vertex size, must agree with declaration at this index
                    read(&vertexSize, 1);

                    // Check for vertex data header
                    read(&headerID, 1);
                    read(&mChunkSize, 1);

                    if(headerID != M_GEOMETRY_VERTEX_BUFFER_DATA)
                    {
                        N_EXCEPT(NotExist, _I18n("Can't find vertex buffer data area"));
                    }
                    // Check that vertex size agrees
                    if(dest->getUnitSize(bindIndex) != vertexSize)
                    {
                        N_EXCEPT(Internal, _I18n("Buffer vertex size does not agree with vertex declaration"));
                    }
                    // Create / populate vertex buffer
                    VertexBuffer * vbuf;
                    N_Only(GBuffer).create(vbuf, vertexSize, dest->mCount, out->mVertexMode);

                    void * pBuf = vbuf->lock(Buffer::MM_WHOLE);
                    mStream->read(pBuf, dest->mCount * vertexSize);

                    // endian conversion for OSX
                    if(mFlipEndian)
                    {
                        VertexUnitList unit;
                        dest->getUnits(bindIndex, unit);
                        __meshflip((Nui8 *)pBuf, unit, dest->mCount, vertexSize);
                    }
                    vbuf->unlock();

                    // Set binding
                    dest->attach(bindIndex, vbuf);
                }
                break;
                }
                // Get next stream
                if(!mStream->end())
                {
                    Nui16 streamID;
                    read(&streamID, 1);
                    read(&mChunkSize, 1);
                }
            }
            if(!mStream->end())
            {
                // Backpedal back to start of non-submesh stream
                mStream->skip(-ChunkHeaderSize);
            }
        }

        // Perform any necessary colour conversion for an active rendersystem
        RenderSys * rsys = N_Engine().getRender();
        if(N_EnginePtr() && rsys)
        {
            // We don't know the source type if it's DT_Colour, but assume ARGB
            // since that's the most common. Won't get used unless the mesh is
            // ambiguous anyway, which will have been warned about in the log
            VertexUnit::conv(dest, DT_Colour_ARGB, N_Engine().getColourFormat());
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readMesh(Mesh * out)
    {
        // Never automatically build edge lists for this version
        // expect them in the file or not at all
        out->mMark &= ~Mesh::M_AutoLod;

        // bool skelAnim
        bool skelAnim;
        read(&skelAnim);

        // Find all substreams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() &&
                (streamID == M_GEOMETRY ||
                 streamID == M_SUBMESH ||
                 streamID == M_MESH_SKELETON_LINK ||
                 streamID == M_MESH_BONE_ASSIGNMENT ||
                 streamID == M_MESH_LOD ||
                 streamID == M_MESH_BOUNDS ||
                 streamID == M_EDGE_LISTS ||
                 streamID == M_POSES ||
                 streamID == M_ANIMATIONS ||
                 streamID == M_TABLE_EXTREMES))
            {
                switch(streamID)
                {
                case M_GEOMETRY:
                    VertexData * vdata;
                    N_Only(GBuffer).create(vdata);
                    out->setVertexData(vdata);
                    try
                    {
                        readVertexData(out, vdata);
                    }
                    catch(Exception & e)
                    {
                        if (typeid(&e) == typeid(class NotExistException))
                        {
                            // duff geometry data entry with 0 vertices
                            N_Only(GBuffer).destroy(vdata);
                            out->setVertexData(0);
                            // Skip this stream (pointer will have been returned to just after header)
                            mStream->skip(mChunkSize - ChunkHeaderSize);
                        }
                        else
                        {
                            throw;
                        }
                    }
                    break;
                case M_SUBMESH:
                    readSubMesh(out);
                    break;
                case M_MESH_SKELETON_LINK:
                    readSkeleton(out);
                    break;
                case M_MESH_BONE_ASSIGNMENT:
                    readSkeletonAssign(out);
                    break;
                case M_MESH_LOD:
                    readLod(out);
                    break;
                case M_MESH_BOUNDS:
                    readAABB(out);
                    break;
                case M_EDGE_LISTS:
                    readEdgeList(out);
                    break;
                case M_POSES:
                    readVertexOffsetList(out);
                    break;
                case M_ANIMATIONS:
                    readAnimationList(out);
                    break;
                case M_TABLE_EXTREMES:
                    readEdgePoint(out);
                    break;
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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readSubMesh(Mesh * out)
    {
        Nui16 streamID;
        // Nui32 matrid
        Nui32 matrid;
        // Nui16 GeometryType
        Nui16 geoType;
        // String meshName;
        String meshname;
        // bool selfvertex
        bool selfvertex;
        // Nui32 indexCount
        Nui32 idxcnt;
        // bool indexes32Bit
        bool idx32;

        read(&matrid, 1);
        read(&geoType, 1);
        read(meshname);
        read(&selfvertex, 1);
        read(&idxcnt, 1);
        read(&idx32, 1);

        SubMesh * sm = out->createSub();
        sm->setName(meshname);
        sm->setMaterial(matrid);
        sm->setGeometryType((GeometryRaw::OperationType)geoType);
        sm->setSelfVertex(selfvertex);

        IndexBuffer * ibuf = 0;
        sm->getIndexData(0)->mOffset = 0;
        sm->getIndexData(0)->mCount = idxcnt;

        if(idxcnt > 0)
        {
            if(idx32)
            {
                N_Only(GBuffer).create(ibuf, 32, sm->getIndexData(0)->mCount, out->mIndexMode);
                // Nui32* faceVertexIndices
                Nui32 * pIdx = static_cast<Nui32 *>(ibuf->lock(Buffer::MM_WHOLE));
                read(pIdx, sm->getIndexData(0)->mCount);
                ibuf->unlock();
            }
            else // 16-bit
            {
                N_Only(GBuffer).create(ibuf, 16, sm->getIndexData(0)->mCount, out->mIndexMode);
                // Nui16* faceVertexIndices
                Nui16 * pIdx = static_cast<Nui16 *>(ibuf->lock(Buffer::MM_WHOLE));
                read(pIdx, sm->getIndexData(0)->mCount);
                ibuf->unlock();
            }
        }
        sm->getIndexData(0)->attach(ibuf);

        // M_GEOMETRY stream (Optional: present only if mSelfVertex = true)
        if(sm->isSelfVertex())
        {
            read(&streamID, 1);
            read(&mChunkSize, 1);

            if(streamID != M_GEOMETRY)
            {
                N_EXCEPT(Internal, _I18n("Missing geometry data in mesh file"));
            }
            VertexData * smvd;
            N_Only(GBuffer).create(smvd);
            sm->setVertexData(smvd);
            readVertexData(out, smvd);
        }

        // Find all bone assignments, submesh operation, and texture aliases (if present)
        if(!mStream->end())
        {
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && (streamID == M_SUBMESH_BONE_ASSIGNMENT || streamID == M_SUBMESH_TEXTURE_ALIAS))
            {
                switch(streamID)
                {
                case M_SUBMESH_BONE_ASSIGNMENT:
                    readSubSkeletonAssign(sm);
                    break;
                case M_SUBMESH_TEXTURE_ALIAS:
                    //readSubMeshTextureAlias(sm);
                    break;
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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeSkeleton(ResourceID rid)
    {
        Nui16 t1 = M_MESH_SKELETON_LINK;
        Nui32 t2 = ChunkHeaderSize + sizeof(Nui32);

        write(&t1, 1);
        write(&t2, 1);
        write((Nui32 *)&rid, 1);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readSkeleton(Mesh * out)
    {
        Nui32 skelName;
        read(&skelName, 1);

        out->setSkeletonID(skelName);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeSkeletonAssign(const SkeletonVertex & assign)
    {
        Nui16 t1 = M_MESH_BONE_ASSIGNMENT;
        Nui32 t2 = ChunkHeaderSize + sizeof(Nui32) + sizeof(Nui16) + sizeof(NIIf);

        write(&t1, 1);
        write(&t2, 1);
        // Nui32 mVertexIndex;
        write(&assign.mVertexIndex, 1);
        // Nui16 mBoneIndex;
        write(&assign.mBoneIndex, 1);
        // NIIf mWeight;
        write(&assign.mWeight, 1);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeSubSkeletonAssign(const SkeletonVertex & assign)
    {
        Nui16 t1 = M_SUBMESH_BONE_ASSIGNMENT;
        Nui32 t2 = ChunkHeaderSize + sizeof(Nui32) + sizeof(Nui16) + sizeof(NIIf);

        write(&t1, 1);
        write(&t2, 1);
        // Nui32 mVertexIndex;
        write(&assign.mVertexIndex, 1);
        // Nui16 mBoneIndex;
        write(&assign.mBoneIndex, 1);
        // NIIf mWeight;
        write(&assign.mWeight, 1);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readSkeletonAssign(Mesh * out)
    {
        SkeletonVertex assign;

        // Nui32 mVertexIndex;
        read(&assign.mVertexIndex, 1);
        // Nui16 mBoneIndex;
        read(&assign.mBoneIndex, 1);
        // NIIf mWeight;
        read(&assign.mWeight, 1);

        out->addSkeleton(assign);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readSubSkeletonAssign(SubMesh * out)
    {
        SkeletonVertex assign;

        // Nui32 mVertexIndex;
        read(&assign.mVertexIndex, 1);
        // Nui16 mBoneIndex;
        read(&assign.mBoneIndex, 1);
        // NIIf mWeight;
        read(&assign.mWeight, 1);

        out->addSkeleton(assign);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeLod(const Mesh * mesh)
    {
        MeshLodScheme * scheme = mesh->getLodScheme();
        NCount numLods = mesh->getLodCount();

        Nui16 t1 = M_MESH_LOD;
        Nui32 size = ChunkHeaderSize;

        // Nui32 schemeid
        size += sizeof(Nui32);
        // Nui32 threshold
        size += sizeof(Nui32);
        // Nui32 factor
        size += sizeof(Nui32);
        // Nui16 lodcnt;
        size += sizeof(Nui16);

        write(&t1, 1);
        write(&size, 1);
        Nui32 tempk = scheme->getTypeID();
        // Nui32 schemeid;
        write(&tempk);
        tempk = scheme->getThreshold();
        // Nui32 threshold
        write(&tempk);
        tempk = scheme->getFactor();
        // Nui32 factor
        write(&tempk);
        // Nui16 lodcnt;
        write((Nui16 *)&numLods);

        // Loop from LOD 1 (not 0, this is full detail)
        for(Nidx lodidx = 1; lodidx < numLods; ++lodidx)
        {
            const MeshLodScheme::MeshLod & usage = mesh->getLodScheme()->getInfo(lodidx);
            bool relation = scheme->isRelation(lodidx);
            if(relation)
            {
                // Usage Header
                size = ChunkHeaderSize;
                // NIIf lodsrc;
                size += sizeof(NIIf);
                // Nui32 loddst;
                size += sizeof(Nui32);
                // bool relation
                size += sizeof(bool);
                // Nui32 lodrid
                size += sizeof(Nui32);
                // Nui32 lodgid
                size += sizeof(Nui32);
                // Nui32 lodidxbuf
                size += sizeof(Nui32);

                Nui16 subidx;
                // Calc generated SubMesh sections size
                for(subidx = 0; subidx < mesh->getSubCount(); ++subidx)
                {
                    // header
                    size += ChunkHeaderSize;
                    // Nui32 numFaces;
                    size += sizeof(Nui32);
                    // bool is32Bit
                    size += sizeof(bool);

                    const IndexData * idxdata = mesh->getSub(subidx)->getIndexData(usage.mIdxBuf);
                    if(idxdata->getAttach() != 0 && idxdata->getAttach()->getUnitSize() == 32)
                    {
                        size += static_cast<Nul>(sizeof(Nui32) * idxdata->mCount);
                    }
                    else
                    {
                        size += static_cast<Nul>(sizeof(Nui16) * idxdata->mCount);
                    }
                }

                t1 = M_MESH_LOD_USAGE;
                write(&t1, 1);
                write(&size, 1);
                write(&usage.mSrc, 1);
                write(&usage.mDest, 1);
                write(&relation, 1);
                write((Nui32 *)&usage.mRid, 1);
                write((Nui32 *)&usage.mGid, 1);
                write((Nui32 *)&usage.mIdxBuf, 1);

                // Now write sections
                // Calc generated SubMesh sections size
                for(subidx = 0; subidx < mesh->getSubCount(); ++subidx)
                {
                    size = ChunkHeaderSize;
                    // Nui32 numFaces;
                    size += sizeof(Nui32);
                    // bool indexes32Bit
                    size += sizeof(bool);

                    const IndexData * idxdata = mesh->getSub(subidx)->getIndexData(usage.mIdxBuf);
                    IndexBuffer * ibuf = idxdata->getAttach();
                    // bool indexes32bit
                    bool idx32 = (ibuf != 0 && ibuf->getUnitSize() == 32);
                    if(idx32)
                    {
                        size += static_cast<Nul>(sizeof(Nui32) * idxdata->mCount);
                    }
                    else
                    {
                        size += static_cast<Nul>(sizeof(Nui16) * idxdata->mCount);
                    }

                    t1 = M_MESH_LOD_GENERATED;
                    Nui32 idxCount = static_cast<Nui32>(idxdata->mCount);

                    write(&t1, 1);
                    write(&size, 1);
                    write(&idxCount, 1);
                    write(&idx32, 1);

                    if(idxCount > 0)
                    {
                        if(idx32)
                        {
                            Nui32 * pIdx = static_cast<Nui32 *>(ibuf->lock(Buffer::MM_READ));
                            write(pIdx, idxdata->mCount);
                            ibuf->unlock();
                        }
                        else
                        {
                            Nui16 * pIdx = static_cast<Nui16 *>(ibuf->lock(Buffer::MM_READ));
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
                Nui32 manualSize = ChunkHeaderSize;
                // NIIf  lodsrc;
                size += sizeof(NIIf);
                // Nui32 loddst;
                size += sizeof(Nui32);
                // bool  relation
                size += sizeof(bool);

                // Manual part size
                // Nui32 logrid;
                manualSize += sizeof(Nui32);
                // Nui32 lodgid;
                manualSize += sizeof(Nui32);
                size += manualSize;

                t1 = M_MESH_LOD_USAGE;
                write(&t1, 1);
                write(&size, 1);
                write(&usage.mSrc, 1);
                write(&usage.mDest, 1);
                write(&relation, 1);

                t1 = M_MESH_LOD_MANUAL;
                write(&t1, 1);
                write(&manualSize, 1);
                write((Nui32 *)&usage.mRid, 1);
                write((Nui32 *)&usage.mGid, 1);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeAABB(const Mesh * mesh)
    {
        // Usage Header
        Nui32 size = ChunkHeaderSize;
        Nui16 t1 = M_MESH_BOUNDS;
        size += sizeof(NIIf) * 7;
        write(&t1, 1);
        write(&size, 1);
        // NIIf minx, miny, minz
        const Vector3f & min = mesh->mAABB.getMin();
        const Vector3f & max = mesh->mAABB.getMax();
        write(&min.x, 1);
        write(&min.y, 1);
        write(&min.z, 1);
        // NIIf maxx, maxy, maxz
        write(&max.x, 1);
        write(&max.y, 1);
        write(&max.z, 1);
        // NIIf radius
        write(&mesh->mCenterRange, 1);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readAABB(Mesh * out)
    {
        Vector3f min, max;
        NIIf radius;

        // NIIf minx, miny, minz
        read(&min.x, 1);
        read(&min.y, 1);
        read(&min.z, 1);
        // NIIf maxx, maxy, maxz
        read(&max.x, 1);
        read(&max.y, 1);
        read(&max.z, 1);

        AABox box(min, max);
        out->setAABB(box);
        out->setRangeFactor(0.01f);

        // NIIf radius
        read(&radius, 1);
        out->setCenterRange(radius);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readLod(Mesh * out)
    {
        Nui16 streamID;

        Nui32 schemeid;
        Nui32 threshold;
        Nui32 factor;
        Nui16 lodcount;

        // Nui32 schemeid;
        read(&schemeid, 1);
        // Nui32 threshold;
        read(&threshold, 1);
        // Nui32 factor
        read(&factor, 1);
        // Nui16 lodcount
        read(&lodcount, 1);

        MeshLodScheme * scheme = static_cast<MeshLodScheme *>(N_Only(LodScheme).create(N_LOD_Mesh, schemeid));
        scheme->setFactor(factor);
        scheme->setThreshold(threshold);
        out->setLodScheme(scheme);
        out->setLodCount(lodcount);

        NCount relcnt = out->getLodScheme()->getRelationCount();
        if(relcnt)
        {
            NCount numsubs = out->getSubCount();
            for(NCount sub = 0; sub < numsubs; ++sub)
            {
                out->getSub(sub)->setLodCount(relcnt);
            }
        }

        NCount lodidx;
        // Loop from 1 rather than 0 (full detail index is not in file)
        for(lodidx = 1; lodidx < lodcount; ++lodidx)
        {
            read(&streamID, 1);
            read(&mChunkSize, 1);

            if(streamID != M_MESH_LOD_USAGE)
            {
                N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_USAGE stream in ") + out->getOriginID());
            }
            // NIIf lodsrc
            NIIf lodsrc;
            // Nui32 loddst
            Nui32 loddst;
            // bool lodrelation
            bool relation;

            read(&lodsrc, 1);
            read(&loddst, 1);
            read(&relation, 1);

            if(relation)
            {
                Nui32 lodrid;
                Nui32 lodgid;
                Nui32 lodidxbuf;
                read(&lodrid);
                read(&lodgid);
                read(&lodidxbuf);

                MeshLodScheme::MeshLod usage(lodsrc, loddst, lodrid, lodgid, lodidxbuf, true);

                // Get one set of detail per SubMesh
                NCount i, numSubs  = out->getSubCount();
                for(i = 0; i < numSubs; ++i)
                {
                    read(&streamID, 1);
                    read(&mChunkSize, 1);

                    if(streamID != M_MESH_LOD_GENERATED)
                    {
                        N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_GENERATED stream in ") + out->getOriginID());
                    }

                    SubMesh * sm = out->getSub(i);
                    IndexData * indexData = N_new IndexData();
                    sm->setIndexData(lodidxbuf, indexData);
                    // Nui32 numIndexes
                    Nui32 numIndexes;
                    // bool idx32Bit
                    bool idx32Bit;

                    read(&numIndexes, 1);
                    read(&idx32Bit, 1);

                    indexData->mCount = (NCount)numIndexes;
                    IndexBuffer * buf;
                    // Nui16*/NIIi* faceIndexes;  ((v1, v2, v3) * numFaces)
                    if(idx32Bit)
                    {
                        N_Only(GBuffer).create(buf, 32, indexData->mCount, out->mIndexMode);
                        Nui32 * pIdx = static_cast<Nui32 *>(buf->lock(0, buf->getSize(), Buffer::MM_WHOLE));

                        read(pIdx, indexData->mCount);
                    }
                    else
                    {
                        N_Only(GBuffer).create(buf, 16, indexData->mCount, out->mIndexMode);
                        Nui16 * pIdx = static_cast<Nui16 *>(buf->lock(0, buf->getSize(), Buffer::MM_WHOLE));

                        read(pIdx, indexData->mCount);
                    }
                    buf->unlock();
                    indexData->attach(buf);
                }
                out->getLodScheme()->add(usage);
            }
            else
            {
                // Read detail stream
                read(&streamID, 1);
                read(&mChunkSize, 1);

                if(streamID != M_MESH_LOD_MANUAL)
                {
                    N_EXCEPT(NotExist, _I18n("Missing M_MESH_LOD_MANUAL stream in ") + out->getOriginID());
                }

                Nui32 lodrid;
                Nui32 lodgid;

                read(&lodrid, 1);
                read(&lodgid, 1);

                MeshLodScheme::MeshLod usage(lodsrc, loddst, lodrid, lodgid, 0, false);

                out->getLodScheme()->add(usage);
            }
        }
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getEdgeListSize(const Mesh * obj)
    {
        NCount size = ChunkHeaderSize;
        for(Nui16 i = 0; i < obj->getLodCount(); ++i)
        {
            const GeoEdget * edg = obj->getEdgeList(i);
            bool relation = obj->getLodScheme()->isRelation(i);

            size += getLodEdgeListSize(edg, relation);
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getLodEdgeListSize(const GeoEdget * edg, bool relation)
    {
        NCount size = ChunkHeaderSize;
        // Nui16 lodIndex
        size += sizeof(Nui16);
        // Nui16 locallodIndex
        size += sizeof(Nui16);
        // bool relation            // If manual, no edge data here, loaded from manual mesh
        size += sizeof(bool);

        if(relation)
        {
            // bool mCloseUp
            size += sizeof(bool);
            // Nul numTriangles
            size += sizeof(Nui32);
            // Nul numEdgeGroups
            size += sizeof(Nui32);
            // Triangle * triangleList
            NCount triSize = 0;
            // Nul mIndexDataIndex
            // Nul mVertexDataIndex
            // Nul mVertIndex[3]
            // Nul mLocalVertIndex[3]
            // NIIf normal[4]
            triSize += sizeof(Nui32) * 8 + sizeof(NIIf) * 4;

            size += triSize * edg->mTriangleList.size();
            // Write the groups
            GeoEdget::EdgeGroupList::const_iterator i, iend = edg->mGeoList.end();
            for(i = edg->mGeoList.begin(); i != iend; ++i)
            {
                size += getEdgeGroupSize(*i);
            }
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getEdgeGroupSize(const GeoEdget::EdgeGroup & group)
    {
        NCount size = ChunkHeaderSize;

        // Nul mVertexDataIndex
        size += sizeof(Nui32);
        // Nul mOffset
        size += sizeof(Nui32);
        // Nul mCount
        size += sizeof(Nui32);
        // Nul numEdges
        size += sizeof(Nui32);
        // Edge * edgeList
        NCount edgeSize = 0;
        // Nul  mTriIndex[2]
        // Nul  mVertIndex[2]
        // Nul  mLocalVertIndex[2]
        // bool mAlone
        edgeSize += sizeof(Nui32) * 6 + sizeof(bool);
        size += edgeSize * group.mEdgeList.size();
        return size;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeEdgeList(const Mesh * mesh)
    {
        Nui16 t1 = M_EDGE_LISTS;
        Nui32 t2 = getEdgeListSize(mesh);

        write(&t1, 1);
        write(&t2, 1);
        Nui16 iend = mesh->getLodCount();
        for(Nui16 i = 0; i < iend; ++i)
        {
            const GeoEdget * edg = mesh->getEdgeList(i);
            bool relation = mesh->getLodScheme()->isRelation(i);
            Nui16 localidx = mesh->getLodScheme()->getRelationIndex(i);
            t1 = M_EDGE_LIST_LOD;
            t2 = getLodEdgeListSize(edg, relation);

            write(&t1, 1);
            write(&t2, 1);
            // Nui16 lodIndex
            write(&i, 1);
            // Nui16 localidx
            write(&localidx, 1);
            // bool relation            // If manual, no edge data here, loaded from manual mesh
            write(&relation, 1);

            if(relation)
            {
                // bool mCloseUp
                write(&edg->mCloseUp, 1);
                // Nul  numTriangles
                Nui32 count = edg->mTriangleList.size();
                write(&count, 1);
                // Nul numEdgeGroups
                count = edg->mGeoList.size();
                write(&count, 1);
                // Triangle* triangleList
                // Iterate rather than writing en-masse to allow endian conversion
                GeoEdget::TriangleList::const_iterator t = edg->mTriangleList.begin();
                GeoEdget::NormalsList::const_iterator fni = edg->mNormalsList.begin();
                for ( ; t != edg->mTriangleList.end(); ++t, ++fni)
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
                GeoEdget::EdgeGroupList::const_iterator gi, giend = edg->mGeoList.end();
                for(gi = edg->mGeoList.begin(); gi != giend; ++gi)
                {
                    const GeoEdget::EdgeGroup & edgeGroup = *gi;
                    t1 = M_EDGE_GROUP;
                    write(&t1, 1);
                    t2 = getEdgeGroupSize(edgeGroup);
                    write(&t2, 1);
                    // Nul mVertexDataIndex
                    Nui32 vertexSet = edgeGroup.mVertexDataIndex;
                    write(&vertexSet, 1);
                    // Nul mOffset
                    Nui32 triStart = edgeGroup.mOffset;
                    write(&triStart, 1);
                    // Nul mCount
                    Nui32 triCount = edgeGroup.mCount;
                    write(&triCount, 1);
                    // Nul numEdges
                    count = edgeGroup.mEdgeList.size();
                    write(&count, 1);
                    // Edge* edgeList
                    // Iterate rather than writing en-masse to allow endian conversion
                    GeoEdget::EdgeList::const_iterator ei, eiend = edgeGroup.mEdgeList.end();
                    for(ei = edgeGroup.mEdgeList.begin(); ei != eiend; ++ei)
                    {
                        const GeoEdget::Edge & edge = *ei;
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
    void MeshSerializer::readEdgeList(Mesh * out)
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
                // Nui16 locallodIndex;
                Nui16 locallodIndex;
                // bool relation            // If manual, no edge data here, loaded from manual mesh
                bool relation;

                read(&lodIndex, 1);
                read(&locallodIndex, 1);
                read(&relation, 1);
                // Only load in non-manual levels; others will be connected up by Mesh on demand
                if(relation)
                {
                    GeoEdget * edg = N_new GeoEdget();

                    // Read detail information of the edge list
                    // bool mCloseUp
                    read(&edg->mCloseUp, 1);
                    // Nul numTriangles
                    Nui32 numTriangles;
                    read(&numTriangles, 1);
                    // Allocate correct amount of memory
                    edg->mTriangleList.resize(numTriangles);
                    edg->mNormalsList.resize(numTriangles);
                    edg->mViewPosList.resize(numTriangles);
                    // Nul numEdgeGroups
                    Nui32 numEdgeGroups;
                    read(&numEdgeGroups, 1);
                    // Allocate correct amount of memory
                    edg->mGeoList.resize(numEdgeGroups);
                    // Triangle* triangleList
                    Nui32 tmp[3];
                    for(NCount t = 0; t < numTriangles; ++t)
                    {
                        GeoEdget::Triangle & tri = edg->mTriangleList[t];
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
                        read(&(edg->mNormalsList[t].x), 4);
                    }

                    for (Nui32 eg = 0; eg < numEdgeGroups; ++eg)
                    {
                        read(&streamID, 1);
                        read(&mChunkSize, 1);

                        if(streamID != M_EDGE_GROUP)
                        {
                            N_EXCEPT(Internal, _I18n("Missing M_EDGE_GROUP stream"));
                        }
                        GeoEdget::EdgeGroup & edgeGroup = edg->mGeoList[eg];

                        // Nul mVertexDataIndex
                        read(tmp, 1);
                        edgeGroup.mVertexDataIndex = tmp[0];
                        // Nul triStart
                        read(tmp, 1);
                        edgeGroup.mOffset = tmp[0];
                        // Nul mCount
                        read(tmp, 1);
                        edgeGroup.mCount = tmp[0];
                        // Nul numEdges
                        Nui32 numEdges;
                        read(&numEdges, 1);
                        edgeGroup.mEdgeList.resize(numEdges);
                        // Edge* edgeList
                        for (Nui32 e = 0; e < numEdges; ++e)
                        {
                            GeoEdget::Edge& edge = edgeGroup.mEdgeList[e];
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
                        }
                    }

                    // Postprocessing edge groups
                    GeoEdget::EdgeGroupList::iterator egi, egend = edg->mGeoList.end();
                    for(egi = edg->mGeoList.begin(); egi != egend; ++egi)
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
                                    (Nui16)edgeGroup.mVertexDataIndex-1)->getVertexData();
                            }
                        }
                        else
                        {
                            edgeGroup.mVertex = out->getSub(
                                (Nui16)edgeGroup.mVertexDataIndex)->getVertexData();
                        }
                    }
                    out->setEdgeList(locallodIndex, edg);
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
    NCount MeshSerializer::getAnimationListSize(const Mesh * mesh)
    {
        NCount size = ChunkHeaderSize;

        for(Nidx a = 0; a < mesh->getAnimationCount(); ++a)
        {
            Animation* anim = mesh->getAnimation(a);
            size += getAnimationSize(anim);
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getAnimationSize(const Animation * anim)
    {
        NCount size = ChunkHeaderSize;
        // Nui32 aniID
        size += sizeof(Nui32);
        // Nui32 length
        size += sizeof(Nui32);

        Animation::TrackList::const_iterator i, iend = anim->getTrack().end();
        for(i = anim->getTrack().begin(); i != iend; ++i)
        {
            size += getKeyFrameTrackSize(static_cast<VertexKeyFrameTrack *>(i->second));
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getKeyFrameTrackSize(const VertexKeyFrameTrack * track)
    {
        NCount size = ChunkHeaderSize;
        // Nui16 type
        size += sizeof(Nui16);
        // Nui16 target        // 0 for shared geometry,
        size += sizeof(Nui16);

        if(typeid(*track) == typeid(VertexUnitKeyFrameTrack))
        {
            for(NCount i = 0; i < track->getCount(); ++i)
            {
                VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(track->get(i));
                size += getVertexUnitKeyFrameSize(kf, track->getTarget()->mCount);
            }
        }
        else
        {
            for(NCount i = 0; i < track->getCount(); ++i)
            {
                VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(track->get(i));
                size += getVertexOffsetKeyFrameSize(kf);
            }
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getVertexUnitKeyFrameSize(const VertexUnitKeyFrame * kf,
        NCount vertCnt)
    {
        NCount size = ChunkHeaderSize;
        // NIIf time
        size += sizeof(NIIf);
        // NIIf x,y,z[,nx,ny,nz]
        bool normals = kf->getValue()->getUnitSize() > (sizeof(NIIf) * 3);
        size += sizeof(NIIf) * (normals ? 6 : 3) * vertCnt;

        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getVertexOffsetKeyFrameSize(const VertexOffsetKeyFrame * kf)
    {
        NCount size = ChunkHeaderSize;

        // NIIf time
        size += sizeof(NIIf);
                                     // Nui16 mTargetIndex   NIIf mWeight
        size += (ChunkHeaderSize + sizeof(Nui16) + sizeof(NIIf)) * kf->getValue().size();

        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getVertexOffsetListSize(const Mesh * mesh)
    {
        NCount size = ChunkHeaderSize;

        VertexOffsetList::const_iterator i, iend = mesh->getOffsetList().end();
        for(i = mesh->getOffsetList().begin(); i != iend; ++i)
        {
            size += getVertexOffsetSize(*i);
        }
        return size;
    }
    //------------------------------------------------------------------------
    NCount MeshSerializer::getVertexOffsetSize(const VertexOffset * pose)
    {
        NCount size = ChunkHeaderSize;

        // Nui32 id (may be blank)
        size += sizeof(Nui32);
        // Nui16 target
        size += sizeof(Nui16);
        // bool normals
        size += sizeof(bool);
        // vertex offsets
        size += pose->getVertexOffsets().size() * (ChunkHeaderSize + sizeof(Nui32) +
            sizeof(NIIf) * 3 + (pose->getNormals().empty() ? 0 : size += sizeof(NIIf) * 3));

        return size;
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeVertexOffsetList(const Mesh * mesh)
    {
        VertexOffsetList::const_iterator i, iend = mesh->getOffsetList().end();
        i = mesh->getOffsetList().begin();
        if(i != iend)
        {
            Nui16 t1 = M_POSES;
            Nui32 t2 = getVertexOffsetListSize(mesh);

            write(&t1, 1);
            write(&t2, 1);

            for(; i != iend; ++i)
            {
                writeVertexOffset(*i);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeVertexOffset(const VertexOffset * pose)
    {
        Nui16 t1 = M_POSE;
        Nui32 t2 = getVertexOffsetSize(pose);
        Nui16 val = pose->getTarget();
        Nui32 pid = pose->getID();
        bool normals = !pose->getNormals().empty();

        write(&t1, 1);
        write(&t2, 1);
        // Nui32 id
        write(&pid);
        // Nui16 target
        write(&val, 1);
        // bool normals
        write(&normals, 1);

        t2 = ChunkHeaderSize + sizeof(Nui32) + sizeof(NIIf) * 3 +
            (pose->getNormals().empty() ? 0 : sizeof(NIIf) * 3);
        VertexOffset::OffsetMap::const_iterator i, iend = pose->getVertexOffsets().end();
        VertexOffset::NormalsMap::const_iterator j, jend = pose->getNormals().end();
        for(i = pose->getVertexOffsets().begin(), j = pose->getNormals().begin();
            i != iend; ++i, ++j)
        {
            Nui32 vertexIndex = (Nui32)i->first;
            const Vector3f & offset = i->second;
            t1 = M_POSE_VERTEX;
            write(&t1, 1);
            write(&t2, 1);
            // Nul vertexIndex
            write(&vertexIndex, 1);
            // NIIf xoffset, yoffset, zoffset
            write(&offset.x, 3);
            if(normals)
            {
                const Vector3f & normal = j->second;
                // NIIf xnormal, ynormal, znormal
                write(&normal.x, 3);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeAnimationList(const Mesh * mesh)
    {
        Nui16 t1 = M_ANIMATIONS;
        Nui32 t2 = getAnimationListSize(mesh);
        write(&t1);
        write(&t2);

        for(Nidx a = 0; a < mesh->getAnimationCount(); ++a)
        {
            Animation * anim = mesh->getAnimation(a);
            N_Only(Log).log(_I18n("Exporting animation ") + anim->getID());
            writeAnimation(anim);
            N_Only(Log).log(_I18n("Animation exported."));
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeAnimation(const Animation * obj)
    {
        Nui16 t1 = M_ANIMATION;
        Nui32 t2 = getAnimationSize(obj);
        Nui32 id = obj->getID();
        Nui32 length = obj->getDur();
        write(&t1);
        write(&t2);
        // Nui32 id
        write(&id);
        // Nui32 length
        write(&length);

        // tracks
        Animation::TrackList::const_iterator i, iend = obj->getTrack().end();
        for(i = obj->getTrack().begin(); i != iend; ++i)
        {
            writeKeyFrameTrack(static_cast<VertexKeyFrameTrack *>(i->second));
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeKeyFrameTrack(const VertexKeyFrameTrack * track)
    {
        Nui16 t1 = M_ANIMATION_TRACK;
        Nui32 t2 = getKeyFrameTrackSize(track);
        Nui16 animType;
        Nui16 target = track->getID();

        if (typeid(*track) == typeid(VertexUnitKeyFrameTrack))
            animType = ANIM_VertexUnit;
        else
            animType = ANIM_VertexOffset;
        write(&t1, 1);
        write(&t2, 1);
        // Nui16 type    // 1 == morph, 2 == pose
        write(&animType, 1);
        // Nui16 target
        write(&target, 1);

        if(animType == ANIM_VertexUnit)
        {
            for(Nui16 i = 0; i < track->getCount(); ++i)
            {
                VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(track->get(i));
                writeVertexUnitKeyFrame(track, kf);
            }
        }
        else // ANIM_VertexOffset
        {
            for(Nui16 i = 0; i < track->getCount(); ++i)
            {
                VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(track->get(i));
                writeVertexOffsetKeyFrame(track, kf);
            }
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeVertexUnitKeyFrame(const VertexKeyFrameTrack * track,
        const VertexUnitKeyFrame * kf)
    {
        Nui16 t1 = M_ANIMATION_MORPH_KEYFRAME;
        NCount vertCnt = track->getTarget()->mCount;
        Nui32 t2 = getVertexUnitKeyFrameSize(kf, vertCnt);
        write(&t1, 1);
        write(&t2, 1);
        // NIIf time
        TimeDurMS timePos = kf->getTime();
        write(&timePos);
        // bool includeNormals
        bool includeNormals = kf->getValue()->getUnitSize() > (sizeof(NIIf) * 3);
        write(&includeNormals);
        // NIIf x,y,z            // repeat by number of vertices in original geometry
        NIIf * pSrc = static_cast<NIIf *>(kf->getValue()->lock(Buffer::MM_READ));
        write(pSrc, vertCnt * (includeNormals ? 6 : 3));
        kf->getValue()->unlock();
    }
    //------------------------------------------------------------------------
    void MeshSerializer::writeVertexOffsetKeyFrame(const VertexKeyFrameTrack * track,
        const VertexOffsetKeyFrame * kf)
    {
        Nui16 t1 = M_ANIMATION_POSE_KEYFRAME;
        Nui32 t2 = getVertexOffsetKeyFrameSize(kf);
        TimeDurMS timePos = kf->getTime();

        write(&t1);
        write(&t2);
        // NIIf time
        write(&timePos);

        // pose references
        VertexOffsetKeyFrame::Offsets::const_iterator i, iend = kf->getValue().end();
        t1 = M_ANIMATION_POSE_REF;
                            // Nui16 mTargetIndex  NIIf mWeight
        t2 = ChunkHeaderSize + sizeof(Nui16) + sizeof(NIIf);
        for(i = kf->getValue().begin(); i != iend; ++i)
        {
            const VertexOffsetKeyFrame::VertexOffset & oft = *i;

            write(&t1);
            write(&t2);
            // Nui16 mTargetIndex
            write(&oft.mTargetIndex);
            // NIIf mWeight
            write(&oft.mWeight);
        }
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readVertexOffsetList(Mesh * out)
    {
        // Find all substreams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && (streamID == M_POSE))
            {
                switch(streamID)
                {
                case M_POSE:
                    readVertexOffset(out);
                    break;
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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readVertexOffset(Mesh * out)
    {
        // Nui32 name (may be blank)
        Nui32 name;
        // Nui16 target
        Nui16 target;
        // bool normals
        bool normals;

        read(&name, 1);
        read(&target, 1);
        read(&normals, 1);

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
                    Vector3f offset, normal;
                    // Nul vertexIndex
                    read(&vertIndex, 1);
                    // NIIf xoffset, yoffset, zoffset
                    read(&offset.x, 3);

                    if(normals)
                    {
                        read(&normal.x, 3);
                        pose->add(vertIndex, offset, normal);
                    }
                    else
                    {
                        pose->add(vertIndex, offset);
                    }

                    break;

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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readAnimationList(Mesh * out)
    {
        // Find all substreams
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && streamID == M_ANIMATION)
            {
                readAnimation(out);

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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readAnimation(Mesh * out)
    {
        // Nui32 name
        Nui32 name;
        // Nui32 length
        Nui32 dur;

        read(&name);
        read(&dur);

        Animation * anim = out->createAnimation(name, dur);
        // tracks
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && streamID == M_ANIMATION_TRACK)
            {
                readKeyFrameTrack(out, anim);

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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readKeyFrameTrack(Mesh * out, Animation * anim)
    {
        Nui16 inAnimType;
        Nui16 target;

        // Nui16 type
        read(&inAnimType);
        // Nui16 target
        read(&target);
        VertexKeyFrameTrack * track;
        if(inAnimType == ANIM_VertexUnit)
        {
            track = N_new VertexUnitKeyFrameTrack(anim, target);
            if(target == 0)
            {
                static_cast<VertexUnitKeyFrameTrack *>(track)->setTarget(out->getVertexData());
            }
            else
            {
                static_cast<VertexUnitKeyFrameTrack *>(track)->setTarget(out->getSub(target - 1)->getVertexData());
            }
        }
        else
        {
            track = N_new VertexOffsetKeyFrameTrack(anim, target);
            if(target == 0)
            {
                static_cast<VertexOffsetKeyFrameTrack *>(track)->setTarget(out->getVertexData());
            }
            else
            {
                static_cast<VertexOffsetKeyFrameTrack *>(track)->setTarget(out->getSub(target - 1)->getVertexData());
            }
        }
        // keyframes
        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && (streamID == M_ANIMATION_MORPH_KEYFRAME ||
                streamID == M_ANIMATION_POSE_KEYFRAME))
            {
                switch(streamID)
                {
                case M_ANIMATION_MORPH_KEYFRAME:
                    readVertexUnitKeyFrame(track);
                    break;
                case M_ANIMATION_POSE_KEYFRAME:
                    readVertexOffsetKeyFrame(track);
                    break;
                };

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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readVertexUnitKeyFrame(VertexKeyFrameTrack * out)
    {
        TimeDurMS timePos;
        bool normals;
        // NIIf time
        read(&timePos);
        // bool normals
        read(&normals);

        VertexUnitKeyFrame * kf = static_cast<VertexUnitKeyFrame *>(out->create(timePos));

        // Create buffer, allow read and use shadow buffer
        NCount vertcnt = out->getTarget()->mCount;
        NCount vertexSize = sizeof(NIIf) * (normals ? 6 : 3);
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, vertexSize, vertcnt, Buffer::M_DEV | Buffer::M_HOST);
        // NIIf x,y,z            // repeat by number of vertices in original geometry
        NIIf * pDst = (NIIf *)vbuf->lock(Buffer::MM_WHOLE);
        read(pDst, vertcnt * (normals ? 6 : 3));
        vbuf->unlock();
        kf->setValue(vbuf);
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readVertexOffsetKeyFrame(VertexKeyFrameTrack * out)
    {
        // NIIf time
        TimeDurMS timePos;
        read(&timePos, 1);

        // Create keyframe
        VertexOffsetKeyFrame * kf = static_cast<VertexOffsetKeyFrame *>(out->create(timePos));

        if(!mStream->end())
        {
            Nui16 streamID;
            read(&streamID, 1);
            read(&mChunkSize, 1);

            while(!mStream->end() && streamID == M_ANIMATION_POSE_REF)
            {
                Nui16 oftIndex;
                NIIf weight;
                // Nui16 mTargetIndex
                read(&oftIndex, 1);
                // NIIf mWeight
                read(&weight, 1);

                kf->add(oftIndex, weight);

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
    }
    //------------------------------------------------------------------------
    void MeshSerializer::readEdgePoint(Mesh * out)
    {
        Nui16 idx;
        read(&idx, 1);

        SubMesh * sm = out->getSub(idx);
        NCount vertcnt = (mChunkSize - ChunkHeaderSize - sizeof(Nui16)) / sizeof(NIIf);

        N_assert ((vertcnt % 3) == 0, "error");

        NIIf * vert = N_alloc_t(NIIf, vertcnt);
        read(vert, vertcnt);

        for(NCount i = 0; i < vertcnt; i += 3)
            sm->addEdgePoint(Vector3f(vert[i], vert[i + 1], vert[i + 2]));

        N_free(vert);
    }
    //------------------------------------------------------------------------
}