/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_JOY_STICK_FORCE_FEEDBACK_H_
#define _NII_JOY_STICK_FORCE_FEEDBACK_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickEffect.h"

namespace NII
{
namespace NII_MEDIA
{
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
		bool supportsEffect(JoyStickEffect::ForceEffectType force,
            JoyStickEffect::ForceEffectFactorType type) const;

		void _addEffectTypes(JoyStickEffect::ForceEffectType force,
            JoyStickEffect::ForceEffectFactorType type);
    protected:
		SupportedEffectList mSupportedEffects;
		bool mSetGainSupport;
		bool mSetAutoCenterSupport;
    };
}
}
#endif
