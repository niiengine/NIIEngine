/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-1

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

#ifndef _NII_KEYFRAME_H_
#define _NII_KEYFRAME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 到动画序列中的关键帧.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyFrame : public AnimableAlloc
    {
        friend class KeyFrameTrack;
    public:
        /** 构造函数
        @param[in] own 所属动画轨道
        @param[in] pos 所属动画时间
        @version NIIEngine 3.0.0
        */
        KeyFrame(const KeyFrameTrack * own, TimeDurMS pos);

        virtual ~KeyFrame();

        /** 设置动画轨道中的关键帧时间
        @return 毫秒,时间是相对的
        @version NIIEngine 3.0.0
        */
        void setTime(TimeDurMS time);

        /** 获取动画轨道中的关键帧时间
        @return 毫秒,时间是相对的
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTime() const;
    protected:
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        TimeDurMS mTime;                ///< 关键帧的时间点
        mutable KeyFrameTrack * mTrack; ///< 关键帧所属的动画轨道
    };
}
#endif