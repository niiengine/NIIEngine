/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-4

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

#ifndef _NII_DOME_SKY_H_
#define _NII_DOME_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** 穹天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DomeSky : public Sky
    {
    public:
        DomeSky();
        DomeSky(const SpaceManager * gm);
        virtual ~DomeSky();

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 构建几何
        @version NIIEngine 3.0.0
        */
        void buildGeo(NIIf cuvfactor = 10, NIIf range = 1000, NIIf xtiling = 20,
            NIIf ytiling = 20, NIIi xsegments = 20, NIIi ysegments = 20,
                const Quaternion & ori_oft = Quaternion::IDENTITY);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);

        /** 创建一个面
        @version NIIEngine 3.0.0
        */
        Mesh * createPlaneImpl(ResourceID & rid, GroupID gid, PlaneType bp,
            NIIf cuvfactor, NIIf range, const Quaternion & ori_oft,
                NIIf xtiling, NIIf ytiling, NIIi xsegments, NIIi ysegments) const;
    protected:
        SpaceNode * mNode;
        GeometrySpace * mGeo[5];
        ResourceID mResource[5];
        ResourceID mMaterial;
        NIIf mRange;
        NIIf mCurveFactor;
        NIIf mXTiling;
        NIIf mYTiling;
        NIIi mXSegment;
        NIIi mYSegment;
    };
}
#endif