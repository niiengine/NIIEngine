/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-8

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

#ifndef _NII_JOYDEV_CONTROL_ARGS_H_
#define _NII_JOYDEV_CONTROL_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    enum JoyDevControlType
    {
        JDCT_Button,
        JDCT_Slider,
        JDCT_Pov,
        JDCT_Vector,
        JDCT_Axis,
        JDCT_Wheel
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevControlArgs : public EventArgs
    {
    public:
        JoyDevControlArgs(JoyDevControlType t) : mType(t) {}
        virtual ~JoyDevControlArgs() {}
        
        JoyDevControlType mType;   ///< 具体类型,不要直接修改它
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyHandleControlVectorArgs : public JoyDevControlArgs
    {
    public:
        JoyHandleControlVectorArgs(NIIi v, NIIf x, NIIf y, NIIf z) :
            JoyDevControlArgs(JDCT_Vector),
            mVector(v),
            mX(x),
            mY(y),
            mZ(z) {}

        NIIi mVector;   ///< vector索引
        NIIf mX;        ///< x坐标
        NIIf mY;        ///< y坐标
        NIIf mZ;        ///< z坐标
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlAxisArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlAxisArgs(Nui8 a, NIIi rel) :
            JoyDevControlArgs(JDCT_Axis),
            mAxis(a),
            mRel(rel) {}

        Nui8 mAxis;                         ///< 摇杆的索引
        NIIi mRel;                          ///< 摇动的数量
    };

    /** 手柄控制参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlButtonArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlButtonArgs(Nui8 b) :
            JoyDevControlArgs(JDCT_Button),
            mButton(b) {}

        Nui8 mButton;                       ///< 按键的索引
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlPovArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlPovArgs(Nui8 pov, PovType dir) :
            JoyDevControlArgs(JDCT_Pov),
            mPov(pov),
            mDirection(dir) {}

        Nui8 mPov;                      ///< 索引
        PovType mDirection;             ///< pov方向
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlSliderArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlSliderArgs(Nui8 s, const Vector2<NIIi> & data) :
            JoyDevControlArgs(JDCT_Slider),
            mSlider(s),
            mData(data) {}

        const Vector2<NIIi> & mData;        ///< 横纵轴数据
        Nui8 mSlider;                       ///< slider索引
    };

    /** 摇轮
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyWheelControlArgs : public JoyDevControlArgs
    {
    public:
        JoyWheelControlArgs(NIIf w) :
            JoyDevControlArgs(JDCT_Wheel),
            mWheel(w) {}

        NIIf mWheel;
    };
}
}
#endif