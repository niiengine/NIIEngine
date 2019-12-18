/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-4-2

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
#ifndef _NII_VERTEXFUSION_H_
#define _NII_VERTEXFUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 顶点混合
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexFusion : public AnimationFusion
    {
    public:
        VertexFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);
            
        VertexFusion(AnimationFusionObj * own, const VertexFusion & o);
        
        virtual ~VertexFusion();

        /** 应用到目标对象
        @remark 硬件混合不能获取当前状态,部分操作如计算阴影体积需要软件混合
        @param[in] sw 执行软件顶点数据
        @param[in] hw 执行硬件顶点数据
        @version NIIEngine 3.0.0
        */
        virtual void apply(GeometrySpace * dst, bool sw, bool hw) = 0;
        
        /** 应用到顶点数组
        @version NIIEngine 3.0.0
        */
        virtual void apply(VertexData * data) = 0;
    };
}
#endif