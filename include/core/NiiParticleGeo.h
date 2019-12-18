/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_ParticleGeo_H_
#define _NII_ParticleGeo_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiParticleCtrl.h"
#include "NiiParticle.h"

namespace NII
{
    /** 粒子几何
    @note 仅控制活动粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleGeo : public PropertyCmdObj, public ParticleAlloc
    {
    public:
        ParticleGeo(ScriptTypeID stid = N_CMD_ParticleGeo, LangID lid = N_PrimaryLang) :
			PropertyCmdObj(stid, lid){}
        virtual ~ParticleGeo(){}

        /** 获取类型
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getType() const = 0;

        /** 排列粒子
        @version NIIEngine 3.0.0
        */
        virtual void queue(RenderQueue * queue, ParticleList & list) = 0;

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid) = 0;

        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        virtual void setRenderGroup(Nui16 rqid) = 0;

        /** 设置是否独立裁剪
        @version NIIEngine 3.0.0
        */
        virtual void setCullItself(bool b) = 0;
        
        /** 设置是否使用本地模式
        @version NIIEngine 3.0.0
        */
        virtual void setLocalSpace(bool b) = 0;

        /** 获取排序模式
        @version NIIEngine 3.0.0
        */
        virtual ParticleCtrl::SortMode getSortMode() const = 0;

        /** 通知摄象机机改变
        @version NIIEngine 3.0.0
        */
        virtual void _notify(Camera * cam) = 0;
        
        /** 执行几何查询
        @version NIIEngine 3.0.0
        */
        virtual void query(GeometryQuery * query) = 0;

        /** 附加到节点上
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * node, bool tag) = 0;
    public:
        /** 当粒子旋转时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleRotate(){}

        /** 当粒子大小变化时
        @version NIIEngine 3.0.0
        */
        virtual void onParticleResize(){}

        /** 当控制器大小变化时
        @version NIIEngine 3.0.0
        */
        virtual void onCtrlResize(NIIf width, NIIf height){}
        
        /** 当粒子池变化时
        @version NIIEngine 3.0.0
        */
        virtual void onPoolSizeChange(NCount count){}
    };
}
#endif