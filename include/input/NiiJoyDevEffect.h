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
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevForceFeedback : public PatternAlloc
    {
    public:
        typedef multimap<JoyStickEffect::ForceEffectType, JoyStickEffect::ForceEffectFactorType>::type SupportedEffectList;
    public:
        JoyDevForceFeedback();
        virtual ~JoyDevForceFeedback();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void _setGainSupport(bool set);

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void _setAutoCenterSupport(bool set);

        /**
        @remarks
        @param level
        @version NIIEngine 3.0.0
        */
        virtual void setMasterGain(float level) = 0;

        /**
        @remarks
        @param set
        @version NIIEngine 3.0.0
        */
        virtual void setAutoCenterMode(bool set) = 0;

        /**
        @remarks
        @version NIIEngine 3.0.0
        */
        virtual void upload(const JoyStickEffect * effect) = 0;

        /**
        @remarks 
        @version NIIEngine 3.0.0
        */
        virtual void modify(const JoyStickEffect * effect) = 0;

        /**
        @remarks 
        @version NIIEngine 3.0.0
        */
        virtual void remove(const JoyStickEffect * effect) = 0;

        /**
        @remarks 
        @version NIIEngine 3.0.0
        */
        virtual short getFFAxesNumber() = 0;

        /**
        @remarks 
        @version NIIEngine 3.0.0
        */
        virtual unsigned short getFFMemoryLoad() = 0;
        
        /**
        @remarks
        @version NIIEngine 3.0.0
        */
        const SupportedEffectList& getSupportedEffects() const;

        /**
        @remarks
        @version NIIEngine 3.0.0
        */
        bool supportsEffect(JoyStickEffect::ForceEffectType force, JoyStickEffect::ForceEffectFactorType type) const;

        void _addEffectTypes(JoyStickEffect::ForceEffectType force, JoyStickEffect::ForceEffectFactorType type);
    protected:
        SupportedEffectList mSupportList;
        bool mSetGainSupport;
        bool mSetAutoCenterSupport;
    };
}
}
#endif