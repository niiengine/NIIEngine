/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-4

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
#ifndef _NII_DISPLAY_BUFFER_H_
#define _NII_DISPLAY_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"

namespace NII
{
    /** 显示缓存
    @remark 可以多次重复调用渲染的几何对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DisplayBuffer: public GPUAlloc
    {
    public:
        DisplayBuffer();
        virtual ~DisplayBuffer();

        /** 重新冲刷顶点
        @verison NIIEngine 3.0.0
        */
        virtual void reflush();

        /** 冲刷顶点
        @version NIIEngine 3.0.0
        */
        virtual void flush(SpaceManager * mag) = 0;

        /** 设置用于渲染几何到顶点缓存的材质名字
        @version NIIEngine 3.0.0
        */
        void setMaterialID(ResourceID rid);

        /** 获取用于渲染几何到顶点缓存的材质
        @version NIIEngine 3.0.0
        */
        const Material * getMaterial();

        /** 设置几何类型
        @note 不同渲染系统,可能会被局限
        @version NIIEngine 3.0.0
        */
        void setGeoOp(GeometryRaw::OperationType type);

        /** 获取几何类型
        @note 不同渲染系统,可能会被局限
        @version NIIEngine 3.0.0
        */
        GeometryRaw::OperationType getGeoOp() const;

        /** 获取顶点来源
        @version NIIEngine 3.0.0
        */
        void setGeo(GeometryObj * src);

        /** 获取顶点来源
        @version NIIEngine 3.0.0
        */
        const GeometryObj * getGeo() const;

        /** 设置缓存顶点最大数量
        @version NIIEngine 3.0.0
        */
        void setMaxVertexCount(NCount cnt);

        /** 获取缓存顶点最大数量
        @version NIIEngine 3.0.0
        */
        NCount getMaxVertexCount() const;

        /** 设置是否自动刷新顶点
        @version NIIEngine 3.0.0
        */
        void setAutoFlush(bool b);

        /** 获取是否自动刷新顶点
        @version NIIEngine 3.0.0
        */
        bool isAutoFlush() const;

        /** 获取数据
        @remark 获取目标数据
        @version NIIEngine 3.0.0
        */
        VertexData * getData();

        /** 获取渲染操作
        @version NIIEngine 3.0.0
        */
        virtual void getGeometry(GeometryRaw & op) const = 0;
    protected:
        GeometryRaw::OperationType mGeoOp;
        Material * mMaterial;
        GeometryObj * mGeo;
        VertexData * mVertexData;
        NCount mMaxVertexCount;
        bool mRequestFlush;
        bool mAutoFlush;
    };
}
#endif