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

#ifndef _NII_MeshSerializer_H_
#define _NII_MeshSerializer_H_

#include "NiiPreInclude.h"
#include "NiiFileSerializer.h"
#include "NiiGeoEdget.h"
#include "NiiSkeleton.h"

namespace NII
{
    /** 读取网格成分文件实现
    @version NIIEngine 3.0.0
    */
    class _EngineInner MeshSerializer : public FileSerializer
    {
        friend class Mesh;
    public:
        MeshSerializer(const DataStream * src);
        MeshSerializer(const FileSerializer & obj);
        virtual ~MeshSerializer();
    protected:
        using FileSerializer::write;
        using FileSerializer::read;

        /** 独立流导出
        @version NIIEngine 3.0.0
        */
        void write(const Mesh * in, const String & file, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        /** 独立流导出
        @version NIIEngine 3.0.0
        */
        void write(const Mesh * in, DataStream * stream, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        /** 独立流导入
        @version NIIEngine 3.0.0
        */
        void read(DataStream * in, Mesh * out);

        /** 独立流导入
        @version NIIEngine 3.0.0
        */
        void read(const String & file, Mesh * out);

        /// @copydetails FileSerializer::read
        virtual void readUnit(void * out);

        /// @copydetails FileSerializer::write
        virtual void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native);

        /// @copydetails FileSerializer::isVersion
        bool isVersion(SerialVer ver) const;

        /// @copydetails FileSerializer::getUnit
        SerializerID getUnit() const;

        /// @copydetails FileSerializer::getGroup
        GroupID getGroup() const;

        virtual void writeMesh(const Mesh * in);
        virtual void writeSkeleton(ResourceID rid);
        virtual void writeSkeletonAssign(const SkeletonVertex & assign);
        virtual void writeSubMesh(const SubMesh * in);
        virtual void writeSubSkeletonAssign(const SkeletonVertex & assign);
        virtual void writeLod(const Mesh * in);
        virtual void writeEdgeList(const Mesh * in);
        virtual void writeAnimationList(const Mesh * in);
        virtual void writeAnimation(const Animation * in);
        virtual void writeKeyFrameTrack(const VertexKeyFrameTrack * in);
        virtual void writeVertexUnitKeyFrame(const VertexKeyFrameTrack * track, const VertexUnitKeyFrame * in);
        virtual void writeVertexOffsetKeyFrame(const VertexKeyFrameTrack * track, const VertexOffsetKeyFrame * in);
        virtual void writeVertexOffsetList(const Mesh * in);
        virtual void writeVertexOffset(const VertexOffset * in);
        virtual void writeEdgePoint(const Mesh * in);
        virtual void writeAABB(const Mesh * in);
        virtual void writeVertexData(const VertexData * in);

        virtual void readMesh(Mesh * out);
        virtual void readSkeleton(Mesh * out);
        virtual void readSkeletonAssign(Mesh * out);
        virtual void readSubMesh(Mesh * out);
        virtual void readSubSkeletonAssign(SubMesh * out);
        virtual void readLod(Mesh * out);
        virtual void readEdgeList(Mesh * out);
        virtual void readAnimationList(Mesh * out);
        virtual void readAnimation(Mesh * out);
        virtual void readKeyFrameTrack(Mesh * out, Animation * target);
        virtual void readVertexUnitKeyFrame(VertexKeyFrameTrack * out);
        virtual void readVertexOffsetKeyFrame(VertexKeyFrameTrack * out);
        virtual void readVertexOffsetList(Mesh * out);
        virtual void readVertexOffset(Mesh * out);
        virtual void readEdgePoint(Mesh * out);
        virtual void readAABB(Mesh * out);
        virtual void readVertexData(Mesh * out, VertexData * target);

        virtual NCount getMeshSize(const Mesh * obj);
        virtual NCount getSubMeshSize(const SubMesh * obj);
        virtual NCount getGeometrySize(const VertexData * obj);
        virtual NCount getEdgeListSize(const Mesh * obj);
        virtual NCount getLodEdgeListSize(const GeoEdget * obj, bool relation);
        virtual NCount getEdgeGroupSize(const GeoEdget::EdgeGroup & group);
        virtual NCount getAnimationListSize(const Mesh * obj);
        virtual NCount getAnimationSize(const Animation * obj);
        virtual NCount getKeyFrameTrackSize(const VertexKeyFrameTrack * obj);
        virtual NCount getVertexUnitKeyFrameSize(const VertexUnitKeyFrame * obj, NCount vertexCount);
        virtual NCount getVertexOffsetKeyFrameSize(const VertexOffsetKeyFrame * obj);
        virtual NCount getVertexOffsetListSize(const Mesh * obj);
        virtual NCount getVertexOffsetSize(const VertexOffset * obj);
    protected:
        Mesh * mCurrentObj;
    };
}
#endif