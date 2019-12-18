/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-17

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

#ifndef _NII_PropertyFusion_H_
#define _NII_PropertyFusion_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"
#include "NiiFusionData.h"

namespace NII
{
    /** 对象属性动画混合
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyFusion : public AnimationFusion
    {
    public:
        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj, Animation * ani, 
            NIIf weight = 1.0, bool enable = false);

        PropertyFusion(PropertyCmdObj * dst, AnimationObj * obj,
            Animation * ani, TimeDurMS pos, TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        PropertyFusion(AnimationFusionObj * own, const PropertyFusion & o);
        
        ~PropertyFusion();

        /** 应用
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** 保存属性当前值
        @remark 属性的值依赖于对象本身
        @version NIIEngine 3.0.0
        */
        void setBaseValue(PropertyID pid);

        /** 获取属性保存值
        @remark 属性的值依赖于对象本身
        @version NIIEngine 3.0.0
        */
        const FusionData & getBaseValue(PropertyID pid);

        /** 设置目标对象
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyCmdObj * dst);

        /** 获取目标对象
        @version NIIEngine 3.0.0
        */
        PropertyCmdObj * getTarget() const;
        
        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        /// @copydetails AnimationFusion::onStart
        void onStart();
    private:
        typedef map<PropertyID, FusionData>::type PropertyValueMap;

        PropertyCmdObj * mDst;
        PropertyValueMap mBaseValue;
    };
}
#endif