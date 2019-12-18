/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_RENDEROBJ_H_
#define _NII_RENDEROBJ_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiGeometryObj.h"
#include "NiiAABox.h"

namespace NII
{
    /** 渲染基础对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderObj : public SpaceObj, public GeometryObj
    {
    public:
        RenderObj();
        RenderObj(SpaceID sid);
        virtual ~RenderObj();

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 设置渲染操作
        @version NIIEngine 3.0.0
        */
        virtual void setGeometry(const GeometryRaw & raw);

        /** 设置边界盒子
        @note 局部级
        @version NIIEngine 3.0.0
        */
        void setAABB(const AABox & box);

        /** 设置变换
        @note 局部级
        @version NIIEngine 3.0.0
        */
        void setMatrix(const Matrix4f & local);

        using SpaceObj::query;

        /// @copydetails GeometryObj::getGeometry
        virtual void getGeometry(GeometryRaw & raw) const;

        /// @copydetails GeometryObj:: getMaterial
        virtual const Material * getMaterial() const;

        /// @copydetails GeometryObj::getMatrix
        virtual void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::_notify
        virtual void _notify(Camera * cam);

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @copydetails SpaceObj::getAABB
        virtual const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;
        
        /// @copydetails SpaceObj::_update
        virtual void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::getFactoryID
        virtual FactoryID getFactoryID() const;
    protected:
        AABox mBox;
        NIIf mRadius;
        Matrix4f mMatrix;
        GeometryRaw mRenderOp;
        ResourceID mMaterialID;
        Material * mMaterial;
        Camera * mCamera;
    };
}
#endif