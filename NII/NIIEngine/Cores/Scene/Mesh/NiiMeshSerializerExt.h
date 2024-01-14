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

#ifndef _NII_MeshSerializerExt_H_
#define _NII_MeshSerializerExt_H_

#include "NiiPreInclude.h"
#include "NiiMeshSerializer.h"

namespace NII
{
    /** Class for providing backwards-compatibility for loading version 1.41 of the .mesh format.
    This mesh format was used from Ogre v1.7.
    */
    class _EngineInner MeshSerializer_v1_41 : public MeshSerializer
    {
    public:
        MeshSerializer_v1_41(const DataStream * src);
        MeshSerializer_v1_41(const FileSerializer & obj);
        ~MeshSerializer_v1_41();
    protected:
        void writeVertexUnitKeyFrame(const VertexKeyFrameTrack * track, const VertexUnitKeyFrame * kf);
        void readVertexUnitKeyFrame(VertexKeyFrameTrack * out);
        void writeVertexOffset(const VertexOffset * pose);
        void readVertexOffset(Mesh * out);
        NCount getVertexUnitKeyFrameSize(const VertexUnitKeyFrame * kf, NCount vertexCount);
        NCount getVertexOffsetSize(const VertexOffset * pose);
    };

    /** Class for providing backwards-compatibility for loading version 1.4 of the .mesh format.
    This mesh format was used from Ogre v1.4.
    */
    class _EngineInner MeshSerializer_v1_4 : public MeshSerializer_v1_41
    {
    public:
        MeshSerializer_v1_4(const DataStream * src);
        MeshSerializer_v1_4(const FileSerializer & obj);
        ~MeshSerializer_v1_4();
    protected:
        virtual void writeLod(const Mesh * in);
        virtual void readLod(Mesh * out);
    };

    /// 类提供 为加载1.3版本.mesh格式的向后-兼容,这个网格格式是使用 NII v1.0 (and some pre-releases)
    class _EngineInner MeshSerializer_v1_3 : public MeshSerializer_v1_4
    {
    public:
        MeshSerializer_v1_3(const DataStream * src);
        MeshSerializer_v1_3(const FileSerializer & obj);
        ~MeshSerializer_v1_3();
    protected:
        virtual void readEdgeList(Mesh * out);

        virtual void writeEdgeList(const Mesh * in);

        /// 重新组织边缘列表三角型到顶点集分组
        virtual void reorganiseTriangles(GeoEdget * ed);
    };

    /// 类提供 为加载1.2版本.mesh格式的向后-兼容This is a LEGACY FORMAT that pre-dates version NII 1.0
    class _EngineInner MeshSerializer_v1_2 : public MeshSerializer_v1_3
    {
    public:
        MeshSerializer_v1_2(const DataStream * src);
        MeshSerializer_v1_2(const FileSerializer & obj);
        ~MeshSerializer_v1_2();
    protected:
        virtual void readMesh(Mesh * out);
        virtual void readVertexData(Mesh * out, VertexData * target);
        virtual void readGeometryPositions(Nui16 bindIdx, Mesh * out, VertexData * target);
        virtual void readGeometryNormals(Nui16 bindIdx, Mesh * out, VertexData * target);
        virtual void readGeometryColours(Nui16 bindIdx, Mesh * out, VertexData * target);
        virtual void readGeometryTexCoords(Nui16 bindIdx, Mesh * out, VertexData * target,
            Nui16 set);
    };

    /// 类提供 为加载1.1版本.mesh格式的向后-兼容This is a LEGACY FORMAT that pre-dates version NII 1.0
    class _EngineInner MeshSerializer_v1_1 : public MeshSerializer_v1_2
    {
    public:
        MeshSerializer_v1_1(const DataStream * src);
        MeshSerializer_v1_1(const FileSerializer & obj);
        ~MeshSerializer_v1_1();
    protected:
        void readGeometryTexCoords(Nui16 bindIdx, Mesh * out, VertexData * target, Nui16 set);
    };
}
#endif