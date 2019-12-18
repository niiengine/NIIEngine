/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-21

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

#ifndef _NII_PROPERTY_ID_KEYFRAME_H_
#define _NII_PROPERTY_ID_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiUnitKeyFrame.h"

namespace NII
{
    /** 对象属性关联关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrame : public UnitKeyFrame
    {
    public:
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode,
            const FusionData & value, PropertyID pid = 0);
        ~PropertyKeyFrame();

        /** 设置对象属性值为混合的值
        @version NIIEngine 3.0.0
        */
        void setPropertyValue(PropertyFusion * obj);

        /** 获取对象属性混合使用的值
        @version NIIEngine 3.0.0
        */
        const FusionData & getPropertyValue(PropertyFusion * obj) const;

        /** 设置值来源的对象属性
        @version NIIEngine 3.0.0
        */
        void setProperty(PropertyID pid);

        /** 获取值来源的对象属性
        @version NIIEngine 3.0.0
        */
        PropertyID getProperty() const;
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        PropertyID mProperty;
    };
}
#endif