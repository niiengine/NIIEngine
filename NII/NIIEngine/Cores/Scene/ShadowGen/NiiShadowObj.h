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

#ifndef _NII_ShadowOBJ_H_
#define _NII_ShadowOBJ_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiGeometryObj.h"

namespace NII
{
    /** 阴影容积
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowVolume : public GeometryObj, public ShadowAlloc
    {
        friend class ShadowObj;
    public:
        ShadowVolume();
        virtual ~ShadowVolume();

        /** 设置容积材质
        @version NIIEngine 3.0.0
        */
        inline void setMaterial(Material * m)       { mMaterial = m; }

        /** 获取亮罩
        @version NIIEngine 3.0.0
        */
        inline ShadowVolume * getLightCap() const   { return mLightCap; }

        /** 是否可见
        @version NIIEngine 3.0.0
        */
        virtual bool isVisible() const;

        /** 设置实际使用的索引缓存
        @version NIIEngine 3.0.0
        */
        virtual void setIndex(IndexBuffer * index);

        /// @see GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @see GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;

        /// @see GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera *) const;

        /// @see GeometryObj::getLight
        void getLight(LightList & ll) const;
    protected:
        GeometryRaw mRenderOp;
        Material * mMaterial;
        ShadowVolume * mLightCap;
    };
    
    /** 阴影容积生成,
    @remark 模版级的阴影技术才需要
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowObj : public SpaceObj
    {
    public:
        /** 创建特性
        @version NIIEngine 3.0.0
        */
        enum VolumeType
        {
            VT_Infinity  = 0x01,    ///< 无限挤压
            VT_FrontCap  = 0x02,    ///< 亮罩
            VT_BackCap   = 0x04,    ///< 暗罩
            VT_Prefab    = 0x08,    ///< 预定制
            VT_Software  = 0x10     ///< 软件处理
        };
    public:
        ShadowObj();
        ShadowObj(SpaceID sid);
        virtual ~ShadowObj();

        /** 获取边缘列表
        @remark 确定侧面影像
        @version NIIEngine 3.0.0
        */
        virtual GeoEdget * getEdgeList();

        /** 边缘列表是否存在
        @version NIIEngine 3.0.0
        */
        virtual bool isEdgeExist();

        /** 获取灯亮罩的世界空间边界盒
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getFrontCapAABB() const = 0;

        /** 获取灯暗罩的世界空间边界盒
        @version NIIEngine 3.0.0
        */
        virtual const AABox & getBackCapAABB(const Light & light, NIIf extent) const;

        /** 获取阴影体积列表
        @param[in] light 用于生成阴影的灯光
        @param[in] extent 挤压阴影体积的距离
        @param[in] mark VolumeType 的多个组合
        */
        virtual const ShadowVolumeList & getVolumeList(const Light * light, NIIf extent, Nmark mark, IndexBuffer * index);

        /** 获取挤压距离
        @version NIIEngine 3.0.0
        */
        virtual NIIf getExtrudeDistance(const Light * l) const = 0;

        /** 积压顶点
        @version NIIEngine 3.0.0
        */
        static void extrude(VertexBuffer * data, NCount count, const Vector4f & pos, NIIf extent);
    protected:
        /** 生成容积列表
        @param[in] mark VolumeType 的多个组合
        @version NIIEngine 3.0.0
        */
        void generate(const GeoEdget * src, IndexBuffer * out, const Light * light,  
            Nmark mark, const ShadowVolumeList & srcobj);
    protected:
        mutable AABox mDarkCapAABB;
    };
}
#endif