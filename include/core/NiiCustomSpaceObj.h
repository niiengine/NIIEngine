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

#ifndef _NII_CustomSpaceObj_H_
#define _NII_CustomSpaceObj_H_

#include "NiiPreInclude.h"
#include "NiiShadowVolumeObj.h"
#include "NiiGeometryObj.h"
#include "NiiVertex.h"

namespace NII
{
    class CustomSpaceObjSection;
    /** 自定义几何空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomSpaceObj : public ShadowVolumeObj
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

        /// @copydetails ShadowVolumeObj::getEdgeList
        GeoEdget * getEdgeList();

        /// @copydetails ShadowVolumeObj::isEdgeExist
        bool isEdgeExist();

        /// @copydetails ShadowVolumeObj::getFrontCapAABB
        const AABox & getFrontCapAABB() const;

        /// @copydetails ShadowVolumeObj::getVolumeList
        const ShadowVolumeList & getVolumeList(const Light * light, NIIf extent,
            Nmark mark, IndexBuffer * index);

        /// @copydetails ShadowVolumeObj::getExtrudeDistance
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
