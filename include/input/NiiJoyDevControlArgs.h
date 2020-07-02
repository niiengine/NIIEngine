/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
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