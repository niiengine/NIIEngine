/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-3

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

#ifndef _NII_CustomSpaceObj_H_
#define _NII_CustomSpaceObj_H_

#include "NiiPreInclude.h"
#include "NiiShadowVolumeGen.h"
#include "NiiGeometryObj.h"
#include "NiiVertex.h"

namespace NII
{
	class CustomSpaceObjSection;
    /** 自定义几何空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomSpaceObj : public ShadowVolumeGen
    {
    public:
        enum CustomVertexType
        {
            CVT_Pos         = 0x01,
            CVT_Normals     = 0x02,
            CVT_TexCoord_1D = 0x04,
            CVT_TexCoord_2D = 0x08,
            CVT_TexCoord_3D = 0x10,
            CVT_TexCoord_4D = 0x20,
            CVT_Tangent     = 0x40,
            CVT_Diffuse     = 0x80
        };
    public:
        CustomSpaceObj();
        CustomSpaceObj(SpaceID sid);
        virtual ~CustomSpaceObj();

        /** 是否使用最开始的投影空间
        @version NIIEngine 3.0.0
        */
        void setIdentityProj(bool b);

        /** 是否使用最开始的投影空间
        @version NIIEngine 3.0.0
        */
        bool isIdentityProj() const;

        /** 是否使用最开始的视图空间
        @version NIIEngine 3.0.0
        */
        void setIdentityView(bool b);

        /** 是否使用最开始的视图空间
        @version NIIEngine 3.0.0
        */
        bool isIdentityView() const;

        /** 只写入数据
        @remark 只从cpu上传到gpu,不从gpu下载到cpu
        @version NIIEngine 3.0.0
        */
        void setOnlyWrite(bool b);

        /** 只写入数据
        @remark 只从cpu上传到gpu,不从gpu下载到cpu
        @version NIIEngine 3.0.0
        */
        bool isOnlyWrite() const;

        /** 开始定义几何
        @param[in] type 三角饶法
        @param[in] rid 材质资源ID
        @version NIIEngine 3.0.0
        */
        virtual void begin(GeometryRaw::OperationType type, ResourceID rid, GroupID gid = GroupDefault);

        /** 开始定义几何
        @param[in] type 三角饶法
        @param[in] rid 材质资源ID
        @version NIIEngine 3.0.0
        */
        virtual void begin(GeometryRaw::OperationType type, const Material * mat);

		/** 建立新顶点位置
		@version NIIEngine 3.0.0
		*/
		virtual void position(const Vector3f & pos);

        /** 建立新顶点位置
        @version NIIEngine 3.0.0
        */
        virtual void position(NIIf x, NIIf y, NIIf z);

		/** 设置顶点法线
		@version NIIEngine 3.0.0
		*/
		virtual void normals(const Vector3f & nls);

        /** 设置顶点法线
        @version NIIEngine 3.0.0
        */
        virtual void normals(NIIf x, NIIf y, NIIf z);

		/** 设置顶点切线
		@version NIIEngine 3.0.0
		*/
		virtual void tangent(const Vector3f & tang);

        /** 设置顶点切线
        @version NIIEngine 3.0.0
        */
        virtual void tangent(NIIf x, NIIf y, NIIf z);

        /** 设置顶点文理坐标
        @version NIIEngine 3.0.0
        */
        virtual void textureCoord(NIIf u);

		/** 设置顶点文理坐标
		@version NIIEngine 3.0.0
		*/
		virtual void textureCoord(const Vector2f & uv);

        /** 设置顶点文理坐标
        @version NIIEngine 3.0.0
        */
        virtual void textureCoord(NIIf u, NIIf v);

        /** 设置顶点文理坐标
        @version NIIEngine 3.0.0
        */
        virtual void textureCoord(NIIf u, NIIf v, NIIf w);

        /** 设置顶点文理坐标
        @version NIIEngine 3.0.0
        */
        virtual void textureCoord(NIIf x, NIIf y, NIIf z, NIIf w);

		/** 设置顶点颜色
		@version NIIEngine 3.0.0
		*/
		virtual void colour(const Colour & c);

        /** 设置顶点颜色
        @version NIIEngine 3.0.0
        */
        virtual void colour(NIIf r, NIIf g, NIIf b, NIIf a = 1.0f);

        /** 设置顶点索引
        @note 和 position 调用对应
        @version NIIEngine 3.0.0
        */
        virtual void index(Nui32 idx);

        /** 设置顶点索引
        @version NIIEngine 3.0.0
        */
        virtual void triangle(Nui32 idx1, Nui32 idx2, Nui32 idx3);

        /** 设置顶点索引
        @version NIIEngine 3.0.0
        */
        virtual void quad(Nui32 idx1, Nui32 idx2, Nui32 idx3, Nui32 idx4);

        /** 完成几何定义
        @version NIIEngine 3.0.0
        */
        virtual GeometryObj * end();

        /** 清除所有定义
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 更新几何定义
        @note 不更新顶点定义,仅更新顶点数据
        @version NIIEngine 3.0.0
        */
        void beginModify(Nidx index);

        /** 预留顶点位置数量
        @version NIIEngine 3.0.0
        */
        void reserveVertexCount(NCount vcount);

        /** 预留顶点索引数量
        @version NIIEngine 3.0.0
        */
        void reserveIndexCount(NCount icount);

        /** 结束更新几何定义
        @note 对应的是 beginModify 操作
        @version NIIEngine 3.0.0
        */
        GeometryObj * endModify();

        /** 获取顶点数量
        @version NIIEngine 3.0.0
        */
        NCount getVertexCount() const;

        /** 获取索引数量
        @version NIIEngine 3.0.0
        */
        NCount getIndexCount() const;

        /** 设置材质
        @note 开始时已经定义了,这方法一般用于重新设置
        @version NIIEngine 3.0.0
        */
        void setMaterial(Nidx geoidx, ResourceID rid, GroupID gid = GroupDefault);

        /** 几何为基础创建网格
        @version NIIEngine 3.0.0
        */
		Mesh * createMesh(ResourceID rid, GroupID gid = GroupDefault);

        /** 设置是否索引顺序渲染
        @version NIIEngine 3.0.0
        */
        void setIndexOrderRender(bool b);

        /** 获取是否索引顺序渲染
        @version NIIEngine 3.0.0
        */
        bool isIndexOrderRender() const;

        /** 获取几何
        @version NIIEngine 3.0.0
        */
        GeometryObj * getGeo(Nidx index) const;

        /** 获取几何数量
        @version NIIEngine 3.0.0
        */
        NCount getGeoCount() const;

        /** 设置边界盒
        @version NIIEngine 3.0.0 高级api
        */
        void setAABB(const AABox & box);

		using SpaceObj::query;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::getAABB.
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange.
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::queue.
        void queue(RenderQueue * queue);

        /// @copydetails ShadowVolumeGen::getEdgeList
        GeoEdget * getEdgeList();

        /// @copydetails ShadowVolumeGen::isEdgeExist
        bool isEdgeExist();

        /// @copydetails ShadowVolumeGen::getLightCapAABB
        const AABox & getLightCapAABB() const;

        /// @copydetails ShadowVolumeGen::getVolumeList
        const ShadowVolumeList & getVolumeList(const Light * light, NIIf extent,
            Nmark mark, IndexBuffer * index);

        /// @copydetails ShadowVolumeGen::getExtrudeDistance
        NIIf getExtrudeDistance(const Light * l) const;

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);
    protected:
        virtual void resetCacheData();
        virtual void updateVertexData();
        virtual void cacheVertexCount(NCount count);
        virtual void cacheIndexCount(NCount count);
    protected:
		typedef vector<CustomSpaceObjSection *>::type CustomGeoObjList;

        AABox mAABB;
        NIIf mRadius;
		CustomGeoObjList mGeoList;
        ShadowVolumeList mShadowVolumeList;
		CustomSpaceObjSection * mOpGeo;
        GeoEdget * mEdgeList;
        ComplexVertex mOpVertex;
        Nui8 * mOpVertexData;
        Nui32 * mOpIndexData;
        NCount mOpVertexSize;
        NCount mOpIndexSize;
        NCount mUnitSize;
        NCount mVertexCount;
        NCount mIndexCount;
        Nui8 mTexCoordCount;
        bool mOnlyWrite;
        bool mVertexIndex;
        bool mUnitInput;
        bool mOpUpdate;
        bool mIndexOrderRender;
        bool mCacheUpload;
        bool mIdentityProj;
        bool mIdentityView;

    };
}
#endif
