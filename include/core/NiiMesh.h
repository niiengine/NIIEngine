/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-10-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_MESH_H_
#define _NII_MESH_H_

#include "NiiPreInclude.h"
#include "NiiAnimationObj.h"
#include "NiiResource.h"
#include "NiiDataStream.h"
#include "NiiSubMesh.h"
#include "NiiAABox.h"
#include "NiiGeoEdget.h"
#include "NiiIDManager.h"

namespace NII
{
    class MeshLodScheme;

    /** 网格
    @remark
        这个类直接和模型格式切切相关,它的设计直接关系到以后的模型从blend3d softimage 3dmax
        等软件中导出,而且还有存在关于扩展格式的(.3ds, .md5, .mdl等)的直接使用,和以后的工作
        效率关系大.还有一点就是录制骨骼最好使用三轴传感器(集)设备(Triaxial sensor)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Mesh : public AnimationObj, public Resource
    {
        friend class GeometrySpace;
        friend class SubMesh;
        friend class VertexOffsetFusion;
        friend class VertexUnitFusion;
        friend class MeshSerializer;
    public:
        /** 网格中顶点的成分类型
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_None          = 0,
            M_Edge          = 0x01,
            M_Skeleton      = 0x02,
            M_ShadowVol     = 0x04,
            M_ObjSpace      = 0x08,
            M_TangentSpace  = 0x10,
            M_NormalsSpace  = 0x20,
            M_ShareVertex   = 0x40,
            M_AutoLod       = 0x80
        };
    public:
        Mesh(ResourceID rid, GroupID gid, ResourceLoadScheme * rls = 0, 
			ResourceResultScheme * rrs = 0, ScriptTypeID stid = N_CMD_Mesh, LangID lid = N_PrimaryLang);

        virtual ~Mesh();

        using Resource::remove;

        /** 构建网格扩展模式
        @param[in] mode 扩展模式
        @version NIIEngine 3.0.0
        */
        void build(Mode mode);

        /** 移去网格扩展模式
        @version NIIEngine 3.0.0
        */
        void remove(Mode mode);

        /** 是否存在网格扩展模式
        @version NIIEngine 3.0.0
        */
        bool isExist(Mode mode);

        /** 顶点内存模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setVertexMode(Nmark mode);

        /** 顶点内存模式
        @version NIIEngine 3.0.0
        */
        Nmark getVertexMode() const;

        /** 顶点索引内存模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setIndexMode(Nmark mode);

        /** 顶点索引内存模式.
        @version NIIEngine 3.0.0
        */
        Nmark getIndexMode() const;

        /** 手动设置这个网格的AABB.
        @version NIIEngine 3.0.0
        */
        void setAABB(const AABox & aab);

        /** 获取这个网格的AABB.
        @version NIIEngine 3.0.0
        */
        const AABox & getAABB() const;

        /** 手动设置边界半径.
        @param[in] f 半径
        @version NIIEngine 3.0.0
        */
        void setCenterRange(NIIf r);

        /** 获取围绕这个网格的边界球
        @version NIIEngine 3.0.0
        */
        NIIf getCenterRange() const;

        /** 设置范围因子
        @param[in] f
        @verion NIIEngine 3.0.0
        */
        void setRangeFactor(NIIf f);

        /** 获取范围因子
        @version NIIEngine 3.0.0
        */
        NIIf getRangeFactor() const;

        /** 设置预制类型
        @note 仅在网格加载前有效
        @version NIIEngine 3.0.0
        */
        void setPrefabType(GeometryPrefab type);

        /** 获取预制类型
        @note 仅在网格加载前有效
        @version NIIEngine 3.0.0
        */
        GeometryPrefab getPrefabType() const;

        /** 设置是否自动构建Lod
        @version NIIEngine 3.0.0
        */
        void setAutoLod(bool b);

        /** 获取是否自动构建Lod
        @version NIIEngine 3.0.0
        */
        bool getAutoLod() const;

        /** 设置是否构建几何边缘
        @version NIIEngine 3.0.0
        */
        void setGenEdget(bool b);

        /** 获取是否构建几何边缘
        @version NIIEngine 3.0.0
        */
        bool isGenEdget() const;

        /** 设置骨骼资源ID
        @version NIIEngine 3.0.0
        */
        void setSkeletonID(SkeletonID sid);

        /** 获取骨骼资源ID
        @version NIIEngine 3.0.0
        */
        SkeletonID getSkeletonID() const;

        /** 设置骨骼
        @version NIIEngine 3.0.0
        */
        void setSkeleton(Skeleton * obj);

        /** 获取骨骼
        @version NIIEngine 3.0.0
        */
        Skeleton * getSkeleton() const;

        /** 设置顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @note 设置后内存将由这个网格控制
        @version NIIEngine 3.0.0 顶级api
        */
        void setVertexData(VertexData * vd);

        /** 获取顶点数据
        @remark 这是个复杂的体系,包含了法线,切线,骨骼混合等
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * getVertexData() const;

        /** 创建子网格
        @version NIIEngine 3.0.0
        */
        SubMesh * createSub();

        /** 删除子网格
        @version NIIEngine 3.0.0
        */
        void destroySub(Nidx index);

        /** 获取子网格
        @version NIIEngine 3.0.0
        */
        SubMesh * getSub(Nidx index) const;

        /** 获取子网格数量
        @version NIIEngine 3.0.0
        */
        NCount getSubCount() const;

        /** 获取子网格列表
        @version NIIEngine 3.0.0
        */
        const SubMeshList & getSubList() const;

        /** 加入骨骼顶点
        @version NIIEngine 3.0.0
        */
        void addSkeleton(const SkeletonVertex & vba);

        /** 移去骨骼顶点
        @param[in] vindex 顶点索引
        @version NIIEngine 3.0.0
        */
        void removeSkeleton(Nidx vindex);

        /** 移去所有骨骼顶点
        @param[in]
        @version NIIEngine 3.0.0
        */
        void clearSkeleton();

        /** 获取距阵映射
        @version NIIEngine 3.0.0
        */
        const BoneIndexList & getMatrixList() const;

        /** 获取到骨头分配列表
        @version NIIEngine 3.0.0 高级api
        */
        const SkeletonVertexList & getSkeletonList() const;

        /** 创建顶点偏移集
        @param[in] id 偏移的ID
        @param[in] vertidx 使用偏移的顶点
        @version NIIEngine 3.0.0
        */
        VertexOffset * createOffset(Nid id, Nui16 vertidx);

        /** 获取顶点偏移集
        @param[in] id
        @version NIIEngine 3.0.0
        */
        VertexOffset * getOffset(Nid id);

        /** 获取顶点偏移集
        @param[in] index
        @version NIIEngine 3.0.0
        */
        VertexOffset * getOffset(Nidx index);

        /** 移去顶点偏移集
        @param[in] id
        @version NIIEngine 3.0.0
        */
        void removeOffset(Nid id);

        /** 移去顶点偏移集
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void removeOffset(Nidx index);

        /** 移去所有顶点偏移集
        @version NIIEngine 3.0.0
        */
        void removeAllOffset();

        /** 获取顶点偏移集数量
        @version NIIEngine 3.0.0
        */
        NCount getOffsetCount() const;

        /** 获取偏移集
        @version NIIEngine 3.0.0
        */
        const VertexOffsetList & getOffsetList() const;

        /** 设置LOD数量
        @version NIIEngine 3.0.0
        */
        void setLodCount(NCount c);

        /** 获取LOD数量
        @remark 这个数量包含原始模型
        */
        NCount getLodCount() const;

        /** 移去所有Lod
        @version NIIEngine 3.0.0
        */
        void removeAllLod();

        /** 设置这个网格使用的LOD策略.
        @param[in] ls LOD策略
        @version NIIEngine 3.0.0
        */
        void setLodScheme(MeshLodScheme * ls);

        /** 获取网格LOD
        @version NIIEngine 3.0.0
        */
        MeshLodScheme * getLodScheme() const;

        /** 设置LOD网格边缘
        @note 参数2内存将交由本类管理
        @version NIIEngine 3.0.0 高级api
        */
        void setEdgeList(Nidx lod, GeoEdget * edg);

        /** 获取LOD网格边缘
        @version NIIEngine 3.0.0
        */
        GeoEdget * getEdgeList(Nidx lod = 0) const;

        /** 构建切线
        @remark 用于顶点和片段着色程序中
        @param[in] dtype 存储切线实际的类型
        @param[in] build4D 是否分割顶点
        @param[in] mirrorUV 是否对称UV
        @param[in] rotationUV 是否旋转UV
        @version NIIEngine 3.0.0 高级api
        */
        void buildTangentBuffer(VertexType dtype = VT_Tangent, bool build4D = false,
            bool mirrorUV = false, bool rotationUV = false);

        /** 检测切线
        @param[in] coordtype 是否使用纹理坐标类型存储切线
        @version NIIEngine 3.0.0 高级api
        */
        bool checkTangentBuffer(bool coordtype);

        /** 副本
        @param[in] rid 新资源ID
        @param[in] gid 新群主ID
        @version NIIEngine 3.0.0 高级
        */
        Mesh * clone(ResourceID rid, GroupID gid = GroupUnknow);

        /** 计算切线空间向量
        @version NIIEngine 3.0.0
        */
        static Vector3f calcTangent(const Vector3f & p1, const Vector3f & p2,
            const Vector3f & p3, NIIf u1, NIIf v1, NIIf u2, NIIf v2, NIIf u3, NIIf v3);

        /// @copydetails AnimationObj::createAnimation
        virtual Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /// @copydetails AnimationObj::removeAnimation
        virtual void removeAnimation(AnimationID id);

        /// @copydetails AnimationObj::removeAllAnimations
        virtual void removeAllAnimations();

        /// @copydetails AnimationObj::_init
        virtual void _init(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_append
        virtual void _append(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_update
        virtual void _update(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::isVertexFusion
        virtual bool isVertexFusion() const;

        /// @copydetails AnimationObj::isSkeletonFusion
        virtual bool isSkeletonFusion() const;
    protected:
        /** 构建边缘列表
        @version NIIEngine 3.0.0 内部api
        */
        void buildEdgeList(Nidx lod = 0);

        /// @copydetails Resource::planImpl
        void planImpl();

        /// @copydetails Resource::unplanImpl
        void unplanImpl();

        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::loadEndImpl
        void loadEndImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    protected:
        DataStream * mSrc;                          ///< 网格数据流
        Skeleton * mSkeleton;                       ///< 骨骼
        MeshLodScheme * mLodScheme;                 ///< LOD策略
        VertexData * mVertex;                       ///< 顶点数据
        SkeletonID mSkeletonID;                     ///< 骨骼资源ID
        GeometryPrefab mPrefabType;                 ///< 预制类型
        Nmark mVertexMode;                          ///< 顶点缓存模式
        Nmark mIndexMode;                           ///< 索引缓存模式
        BoneIndexList mMatrixs;                     ///< 骨骼矩阵
        SubMeshList mSubMeshList;                   ///< 子网格
        GeoEdgetList mEdgetList;                    ///< 边缘列表
        VertexOffsetList mVertexOffsetList;         ///< 顶点偏移
        SkeletonVertexList mSkeletonVertexList;     ///< 骨骼顶点分配
        Nidx mCoordIndex;							///< 纹理位置索引
        Nidx mTangentIndex;							///< 切线位置索引
        NIIf mCenterRange;                          ///< 范围半径
        NIIf mRangeFactor;                          ///< 范围缩放因子
        AABox mAABB;                                ///< 轴对称盒
        Nmark mMark;
    };
}
#endif