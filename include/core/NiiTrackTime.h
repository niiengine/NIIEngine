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

#ifndef _NII_TRACK_TIME_H_
#define _NII_TRACK_TIME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 关键帧时间索引
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TrackTime
    {
    public:
        TrackTime(TimeDurMS pos);
        TrackTime(TimeDurMS pos, Nidx index, Nidx lindex);

        /** 是否有效
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 获取时间
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimePos() const;

        /** 获取动画开始关键帧
        @version NIIEngine 3.0.0
        */
        Nidx getKeyIndex() const;
        
        /** 设置当前时间关键帧
        @version NIIEngine 3.0.0
        */
        void setLocalKeyIndex(Nidx idx);
        
        /** 获取当前时间关键帧
        @version NIIEngine 3.0.0
        */
        Nidx getLocalKeyIndex() const;
    protected:
        TimeDurMS mTimePos;
        Nidx mKeyIndex;
        Nidx mLocalKeyIndex;
    };
}

#endif