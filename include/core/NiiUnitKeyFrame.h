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

#ifndef _NII_UNIT_KEY_FRAME_H_
#define _NII_UNIT_KEY_FRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiFusionData.h"

namespace NII
{
    /** 数值型的专用关键帧.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrame : public KeyFrame
    {
    public:
        /** 帧过渡模式
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Constant,     ///< 基本插值渡
            M_Point,        ///< 帧点过渡
            M_Square,       ///< 加速形式过渡
            M_SquareRoot    ///< 减速形式过渡
        };

        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode, 
            const FusionData & value);
        virtual ~UnitKeyFrame();

        /** 设置插值函子模式
        @version NIIEngine 3.0.0
        */
        void setMode(Mode p);

        /** 获取插值函子模式
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** 设置这个关键帧的数值
        @remark 所有的关键帧值必须有一致的数据类型
        @version NIIEngine 3.0.0
        */
        void setValue(const FusionData & val);

        /** 获取这个关键帧的数值
        @remark 所有的关键帧值必须有一致的数据类型
        @version NIIEngine 3.0.0
        */
        const FusionData & getValue() const;

        /** 获取帧过渡模式参子
        @param[in] pos[0, 1]
        @return 函子模式结果[0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(NIIf pos);
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Mode mMode;
        FusionData mValue;
    };
}

#endif