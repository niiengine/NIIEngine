/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef NII_FUSIONSOUND_H
#define NII_FUSIONSOUND_H

#include "NiiPreInclude.h"
#include "NiiSound.h"
#include "audio_mixer_user.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionSound : public Sound
    {
    public:
        enum Mode
        {
            M_Unkonw,
            M_Order,
            M_TimeOrder,
            M_Fusion
        };
    public:
        FusionSound();
        virtual ~FusionSound();

        /**
        @version NIIEngine 4.0.0
        */
        TimeDurMS getDuration();

        /**
        @version NIIEngine 4.0.0
        */
        inline void setMode(Mode mode){ mMode = mode;}

        /**
        @version NIIEngine 4.0.0
        */
        inline Mode getMode(){ return mMode;}

        /**
        @version NIIEngine 4.0.0
        */
        inline Nui32 getSoundIndex(Nindex idx) const{ return mIndexList[idx]; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NCount getSoundIndexCount() const{ return mIndexList.size(); }

        /**
        @version NIIEngine 4.0.0
        */
        inline Nui32 getDelayIndex(Nindex idx) const{ return mTimeList[idx]; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NCount getDelayIndexCount() const{ return mTimeList.size(); }

        /**
        @version NIIEngine 4.0.0
        */
        const String & getSound(Nindex idx) const;

        /**
        @version NIIEngine 4.0.0
        */
        inline const SoundList & getSounds() const { return mSoundList; }

        /**
        @version NIIEngine 4.0.0
        */
        inline TimeDurMS getFadeTime() const { return mFadeTime; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setTimeFactor(NIIf factor){ mTimeFactor = factor; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getTimeFactor() const { return mTimeFactor; }

        /// @copydetails Sound::getSoundType
        Type getSoundType() const { return T_Fusion; }

        /// @copydetails Sound::getSub
        Sound * getSub(Nindex idx) const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;
        
        /// @copydetails Resource::write
        virtual void write(const Serializer * in);
    private:
        /// @copydetails Sound::getMaxDistance
        virtual NIIf getMaxDistance() const;
        
        /// @copydetails Sound::loadImpl
        virtual void loadImpl();
    private:
        Mode mMode;
        std::vector<String> mSoundList;
        std::vector<Nindex>	mIndexList;
        std::vector<Nindex>	mTimeList;
        TimeDurMS mFadeTime;
        NIIf mTimeFactor;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionSoundUnit : public SoundUnit
    {
    public:
        FusionSoundUnit(FusionSound * sound, SpaceNode * node = 0);
        ~FusionSoundUnit();

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();

        /// @copydetails SoundUnit::getType
        virtual Type getType() const {return T_Fusion; }
                
        /// @copydetails SoundUnit::play
        virtual void play();

        /// @copydetails SoundUnit::stop
        virtual void stop();

        /// @copydetails SoundUnit::isPlaying
        virtual bool isPlaying();

        /// @copydetails SoundUnit::getPlayTime
        virtual TimeDurMS getPlayTime();

        /// @copydetails SoundUnit::setPos
        virtual void setPos(const Vector3f & pos);

        /// @copydetails SoundUnit::setVelocity
        virtual void setVelocity(const Vector3f & vel);

        /// @copydetails SoundUnit::setDirection
        virtual void setDirection(const Vector3f & dir);

        /// @copydetails SoundUnit::setGain
        virtual void setGain(NIIf gain);

        /// @copydetails SoundUnit::setRelativeGain
        virtual void setRelativeGain(NIIf gain);
    private:
        /// @copydetails SoundUnit::onUpdate
        void onUpdate();

        /// @copydetails SoundUnit::onTime
        void onTime();

        /// @copydetails SoundUnit::onSwap
        void onSwap();

        void playStuf();
    private:
        SoundUnitList mUnitList;
        FusionSound * mSound;
        SoundUnit * mUnit;
        SoundUnit * mSwap;
        TimeDurMS mUnitTime;
        TimeDurMS mSwapTime;

        uint32 mFadeTime;
        NIIf mTimeFactor;
        NIIf mFadeFactor;
        Nindex mCurrentIndex;
        Nindex mDelayIndex;
        
        bool mStop;
        bool mStopTime;
    };
}
#endif