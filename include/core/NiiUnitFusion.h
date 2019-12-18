/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-8

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

#ifndef _NII_UNIT_FUSION_H_
#define _NII_UNIT_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 单元混合实例
    @remark
        可以用于混合 Vector2 Vector3 等简单的数字型(线性)动画,如UI窗体/单元从大到小,从小
        到大的缩放过程动画
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitFusion : public AnimationFusion
    {
    public:
        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani,
            NIIf weight = 1.0, bool enable = false);

        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani, TimeDurMS pos,
            TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        UnitFusion(AnimationFusionObj * own, const UnitFusion & o);

        ~UnitFusion();

        /** 应用
        @remark
        @param[in] factor 混合影响权重
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0f);
    public:
        /// @copydetails AnimationFusion::reset
        void reset();
        
        /// @copydetails AnimationFusion::
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        FusionData * mDst;
    };
}
#endif