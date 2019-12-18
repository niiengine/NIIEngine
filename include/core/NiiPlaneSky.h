/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-3

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

#ifndef _NII_PLANE_SKY_H_
#define _NII_PLANE_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** 面天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSky : public Sky
    {
    public:
        PlaneSky();
        PlaneSky(const SpaceManager * gm);
        virtual ~PlaneSky();

        /** 设置材质
        @param[in] rid 资源ID
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf xtiling = 10,
            NIIf ytiling = 10, NIIi xsegments = 10, NIIi ysegments = 10, NIIf bow = 0);

        /** 天空面的大小因子
        @return 正比
        @version NIIEngine 3.0.0
        */
        NIIf getScale() const;

        /** 天空面的弯曲因子
        @return 整数存在
        @version NIIEngine 3.0.0
        */
        NIIf getBow() const;

        /** 获取天空平面的定义
        @version NIIEngine 3.0.0
        */
        const Plane & getPlane() const;
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
        Plane mPlane;
        NIIf mScale;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
        NIIf mBow;
        ResourceID mMaterial;
        ResourceID mMesh;
        GeometrySpace * mGeo;
    };
}
#endif