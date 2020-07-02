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

#ifndef _NII_PredefinedControllers_H__
#define _NII_PredefinedControllers_H__

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiDataEquation.h"
#include "NiiRenderFrameListener.h"
#include "NiiGpuProgram.h"

namespace NII
{
    /// 预定义控制器函数,处理动画.
    class _EngineAPI AnimationControllerFunction : public DataFunc<TimeDurMS, NIIf>
    {
    public:
        AnimationControllerFunction(bool autoDsy, TimeDurMS dur, TimeDurMS oft = 0) :
            DataFunc<TimeDurMS, NIIf>(autoDsy)
        {
            mSeqTime = dur;
            mTime = oft;
        }

        ///@copydetails DataFunc::func
        NIIf func(const TimeDurMS & source)
        {
            mTime += source;
            // Wrap
            while (mTime >= mSeqTime)
                mTime -= mSeqTime;
            while (mTime < 0)
                mTime += mSeqTime;

            // Return parametric
            return (NIIf)mTime / mSeqTime;
        }

        /// 设置当前时间
        void setTime(TimeDurMS timeVal)
        {
            mTime = timeVal;
        }

        // 动画实际长度
        void setSequenceTime(TimeDurMS seqVal)
        {
            mSeqTime = seqVal;
        }
    protected:
        TimeDurMS mSeqTime;
        TimeDurMS mTime;
    };

    /** 预定义控制器值,获取/设置 纹理坐标修改 (缩放 和 变换).
    @remark
        Effects can be applied to the scale or the offset of the u or v coordinates, or both. If separate
        modifications are required to u and v then 2 instances are required to control both independently, or 4
        if you ant separate u and v scales as well as separate u and v offsets.
    @par
        Because of the nature of this value, it can accept values outside the 0..1 parametric range.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TexCoordModifierControllerValue : public DataValue<NIIf>
    {
    public:
        /** Constructor.
        @param[in] t ShaderChTextureUnit to apply the modification to.
        @param[in] translateU If true, the u coordinates will be translated by the modification.
        @param[in] translateV If true, the v coordinates will be translated by the modification.
        @param[in] scaleU If true, the u coordinates will be scaled by the modification.
        @param[in] scaleV If true, the v coordinates will be scaled by the modification.
        @param[in] rotate If true, the texture will be rotated by the modification.
        */
        TexCoordModifierControllerValue(bool autoDsy, ShaderChTextureUnit* t,
            bool translateU = false, bool translateV = false, bool scaleU = false,
                bool scaleV = false, bool rotate = false);

        /// @copydetails DataValue::getValue
        const NIIf getValue() const;

        /// @copydetails DataValue::setValue
        void setValue(const NIIf & value);
    protected:
        ShaderChTextureUnit * mTextureLayer;
        bool mTransU, mTransV;
        bool mScaleU, mScaleV;
        bool mRotate;
    };

    /// 预定义控制器函数,简单把输入值缩放到输出值.
    class _EngineAPI ScaleControllerFunction : public DataFunc<TimeDurMS, NIIf>
    {
    public:
        /** Constructor, requires a scale factor.
        @param[in] scalefactor The multiplier applied to the input to produce the output.
        @param[in] deltaInput If true, signifies that the input will be a delta value such that the function should
            add it to an internal counter before calculating the output.
        */
        ScaleControllerFunction(bool autoDsy, NIIf scalefactor);

        ///@copydetails DataFunc::func
        NIIf func(const TimeDurMS & source);
    protected:
        NIIf mScale;
        NIIf mTemp;
    };

    /** 波浪类型枚举
    @version NIIEngine 3.0.0
    */
    enum WaveformType
    {
        WFT_SINE,               ///< 标准正弦波,从低到高平滑变化,然后回到原来
        WFT_TRIANGLE,           ///< 一个成角的波,一个以点峰的常量级 上升/下降速
        WFT_SQUARE,             ///< 一半时间使用最小值,另一半时间瞬间过渡到最大值
        WFT_SAWTOOTH,           ///< 从最小到最大期间逐渐的稳定增长,返回结束时片刻的最小
        WFT_INVERSE_SAWTOOTH,   ///< 从最小到最大期间逐渐的稳定减小, 返回结束时片刻的最大
        /** 震动宽度调制,像WFT_SQUARE一样,不包括高到低过渡,通
            过工作周期控制,50% (0.5) 的工作周期与WFT_SQUARE一样的输出
        */
        WFT_PWM
    };
        
    /** 预定义控制器函数,基于波形.
    @remark A waveform function translates parametric input to parametric output based on a wave. The factors
        affecting the function are:
            - wave type - the shape of the wave
            - base - the base value of the output from the wave
            - frequency - the speed of the wave in cycles per second
            - phase - the offset of the start of the wave, e.g. 0.5 to start half-way through the wave
            - amplitude - scales the output so that instead of lying within [0,1] it lies within [0,1] * amplitude
            - duty cycle - the active width of a PWM signal
    @par Note that for simplicity of integration with the rest of the controller insfrastructure, the output of
        the wave is parametric i.e. 0..1, rather than the typical wave output of [-1,1]. To compensate for this, the
        traditional output of the wave is scaled by the following function before output:
    @par output = (waveoutput + 1) * 0.5
    @par Hence a wave output of -1 becomes 0, a wave ouput of 1 becomes 1, and a wave output of 0 becomes 0.5.
    */
    class _EngineAPI WaveformControllerFunction : public DataFunc<TimeDurMS, NIIf>
    {
    public:
        /** Default constructor, requires at least a wave type, other parameters can be defaulted unless required.
        @param[in] deltaInput If true, signifies that the input will be a delta value such that the function should
            add it to an internal counter before calculating the output.
        @param[in] dutyCycle Used in PWM mode to specify the pulse width.
        */
        WaveformControllerFunction(bool autoDsy, WaveformType wType, NIIf base = 0, 
            NIIf frequency = 1, NIIf phase = 0, NIIf amplitude = 1, bool filter = true, 
            NIIf dutyCycle = 0.5);

        ///@copydetails DataFunc::func
        NIIf func(const TimeDurMS & source);
        
        ///@copydetails DataFunc::isValid
        bool isValid() const;
    protected:
        WaveformType mWaveType;
        NIIf mBase;
        NIIf mFrequency;
        NIIf mPhase;
        NIIf mAmplitude;
        NIIf mDutyCycle;
        NIIf mTemp;
        bool mDeltaInput;   ///< 如果真,函数增加值会到一起,而且控制在1.0间
    };
}
#endif