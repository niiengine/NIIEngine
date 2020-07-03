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
        
        /// @copydetails Sound::setProperty
        void setProperty(PropertyData * param);

        /**
        @version NIIEngine 4.0.0
        */
        TimeDurMS getDuration();

        /**
        @version NIIEngine 4.0.0
        */
        inline void setMode(Mode mode)  { mMode = mode;}

        /**
        @version NIIEngine 4.0.0
        */
        inline Mode getMode() const     { return mMode;}

        /**
        @version NIIEngine 4.0.0
        */
        inline Nui32 getSoundIndex(Nidx idx) const{ return mIndexList[idx]; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NCount getSoundIndexCount() const{ return mIndexList.size(); }

        /**
        @version NIIEngine 4.0.0
        */
        inline Nui32 getDelayIndex(Nidx idx) const{ return mTimeList[idx]; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NCount getDelayIndexCount() const{ return mTimeList.size(); }

        /**
        @version NIIEngine 4.0.0
        */
        const String & getSound(Nidx idx) const;

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
        Sound * getSub(Nidx idx) const;

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
        std::vector<Nidx>	mIndexList;
        std::vector<Nidx>	mTimeList;
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

        /// @copydetails SoundUnit::isPlay
        virtual bool isPlay();

        /// @copydetails SoundUnit::getPlayTime
        virtual TimeDurMS getPlayTime() const;

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
        Nidx mCurrentIndex;
        Nidx mDelayIndex;
        
        bool mStop;
        bool mStopTime;
    };
}
#endif