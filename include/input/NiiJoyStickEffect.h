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

#ifndef _NII_JOY_STICK_EFFECT_H_
#define _NII_JOY_STICK_EFFECT_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_MEDIA
{
    class JoyDevForceFeedback;
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickEffect : public PatternAlloc
    {
        friend class JoyStickControlPattern;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        enum ForceEffectType
        {
            EFT_Unknown,                    ///< 无效的
            EFT_Constant,                   ///< 常量级
            EFT_Ramp,                       ///<
            EFT_Periodic,                   ///< 定期级
            EFT_Conditional,                ///< 条件级
            EFT_Custom                      ///< 自定义
        };

        /**
        @version NIIEngine 3.0.0
        */
        enum ForceEffectFactorType
        {
            FEFT_Unknown,                   ///< 无效的
            FEFT_Constant,                  ///< ConstantForce
            FEFT_Ramp,                      ///< RampForce
            FEFT_Square,                    ///< PeriodicForce
            FEFT_Triangle,                  ///< PeriodicForce
            FEFT_Sine,                      ///< PeriodicForce
            FEFT_SawToothUp,                ///< PeriodicForce
            FEFT_SawToothDown,              ///< PeriodicForce
            FEFT_Friction,                  ///< ConditionalForce
            FEFT_Damper,                    ///< ConditionalForce
            FEFT_Inertia,                   ///< ConditionalForce
            FEFT_Spring,                    ///< ConditionalForce
            FEFT_Custom                     ///< 自定义
        };

        enum ForceEffectDirection
        {
            FED_NorthWest,                  ///<
            FED_North,                      ///<
            FED_NorthEast,                  ///<
            FED_East,                       ///<
            FED_SouthEast,                  ///<
            FED_South,                      ///<
            FED_SouthWest,                  ///<
            FED_West                        ///<
        };

        class Envelope : public PatternAlloc
        {
        public:
            Envelope ();

            Nui32 mAttackLength;              ///<
            Nui16 mAttackLevel;               ///<
            Nui32 mFadeLength;                ///<
            Nui16 mFadeLevel;                 ///<
        };
    protected:
        JoyStickEffect();

        JoyStickEffect(ForceEffectType type, ForceEffectFactorType factor);
    public:
        virtual ~JoyStickEffect();

        /** 应用效果
        @version NIIEngine 3.0.0
        */
        virtual void apply(JoyDevForceFeedback * dest) = 0;
    protected:
        NIIi mHandle;                       ///<
        ForceEffectType mType;              ///<
        ForceEffectDirection mDirection;    ///<
        ForceEffectFactorType mFactorType;  ///<
        Nui16 mTriggerButton;               ///<
        Nui32 mTriggerInterval;             ///<
        Nui32 mReplayLength;                ///<
        Nui32 mReplayDelay;                 ///<
        NIIs mAxes;                         ///< 多少个摇杆影响这个效果
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickConstantEffect : public JoyStickEffect
    {
    public:
        JoyStickConstantEffect() :
            mLevel(5000) {}

        Envelope mEnvelope;     ///< 基础成分
        NIIs mLevel;            ///< -10K 到 +10k
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickPeriodicEffect : public JoyStickEffect
    {
    public:
        JoyStickPeriodicEffect() :
            mMagnitude(0),
            mOffset(0),
            mPhase(0),
            mPeriod(0) {}

        Envelope mEnvelope;     ///< 基础成分
        Nui16 mMagnitude;       ///< 0 到 10,0000
        NIIs mOffset;
        Nui16 mPhase;           ///< Position at which playback begins 0 到 35,999
        Nui32 mPeriod;          ///< Period of effect (microseconds)
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickRampEffect : public JoyStickEffect
    {
    public:
        JoyStickRampEffect() :
            mStartLevel(0),
            mEndLevel(0) {}

        Envelope mEnvelope;         ///< 基础成分
        Nui16 mStartLevel;          ///< -10K 到 +10k
        Nui16 mEndLevel;            ///< -10K 到 +10k
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickConditionalEffect : public JoyStickEffect
    {
    public:
        JoyStickConditionalEffect() :
            mRightCoeff(0),
            mLeftCoeff(0),
            mRightSaturation(0),
            mLeftSaturation(0),
            mDeadband(0) {}

        NIIs mRightCoeff;
        NIIs mLeftCoeff;
        Nui16 mRightSaturation;
        Nui16 mLeftSaturation;
        Nui16 mDeadband;
    };
}
}
#endif