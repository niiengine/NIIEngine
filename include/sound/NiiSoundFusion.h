/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-17

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

#ifndef _NII_SOUND_FUSION_H_
#define _NII_SOUND_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 节点混合实例
    @remark 设置插值类型配合LOD策略使用能有效的控制运行效率
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundFusion : public AnimationFusion
    {
    public:
        SoundFusion(AnimationObj * obj, Animation * ani,  TimeDurMS pos, TimeDurMS dur, 
            NIIf scale = 1.0, NIIf weight = 1.0, bool enable = false);

        SoundFusion(AnimationFusionObj * own, const SoundFusion & o);

        ~SoundFusion();

        /** 执行应用
        @param[in] factor 混合影响权重(0.0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** 
        @version NIIEngine 4.0.0
        */
        inline void setDirty(bool b) { mDirty = b; }

        /** 
        @version NIIEngine 4.0.0
        */
        inline bool isDirty() const { return mDirty; }
    public:
        /// @copydetails AnimationFusion::reset
        void reset();

        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        SpaceNode * mDst;
        bool mDirty;
    };
}
#endif