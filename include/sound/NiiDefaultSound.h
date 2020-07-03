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

#ifndef NII_DefaultSound_H
#define NII_DefaultSound_H

#include "NiiPreInclude.h"
#include "NiiSound.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI DefaultSound : public Sound
    {
    public:
        DefaultSound();
        virtual ~DefaultSound();
        
        /// @copydetails Sound::setProperty
        void setProperty(PropertyData * param);

        /// @copydetails Sound::getSoundType
        Type getSoundType() const {return T_Unkonw; }

        /**
        @version NIIEngine 4.0.0
        */
        void setBuffer(AudioBuffer * buf);
        
        /**
        @version NIIEngine 4.0.0
        */
        AudioBuffer * getBuffer() const;

        /**
        @version NIIEngine 4.0.0
        */
        inline void setAlpha(NIIf a) { mAlpha = a; }

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getAlpha() const { return mAlpha; }

        /// @copydetails Sound::getSub
        Sound * getSub(Nidx idx) const;

        /// @copydetails Sound::getSubCount
        NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;

        /// @copydetails Resource::write
        virtual void write(const Serializer * in);
    protected:
        /// @copydetails Sound::loadImpl
        virtual void loadImpl();
    private:
        AudioBuffer	* mBuffer;
        NIIf mAlpha;
    };
    
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI DefaultSoundUnit : public SoundUnit
    {
        friend class SoundSys;
    public:
        DefaultSoundUnit(DefaultSound * sound, SpaceNode * node = 0);
        virtual ~DefaultSoundUnit();

        /**
        @version NIIEngine 4.0.0
        */
        virtual AudioBuffer	* getBuffer();

        /// @copydetails SoundUnit::getType
        virtual Type getType() const {return T_Unkonw;}
        
        /// @copydetails SoundUnit::setLoop
        virtual void setLoop(bool l);
        
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

        /** 
        @version NIIEngine 4.0.0
        */
        Vector3f getLocalPos() const;

        /// @copydetails SoundUnit::setVelocity
        virtual void setVelocity(const Vector3f & vel);

        /// @copydetails SoundUnit::setDirection
        virtual void setDirection(const Vector3f & dir);

        /// @copydetails SoundUnit::setGain
        virtual void setGain(NIIf gain);

        /// @copydetails SoundUnit::setRelativeGain
        virtual void setRelativeGain(NIIf gain);

        /// @copydetails SoundUnit::setPitch
        virtual void setPitch(NIIf pitch);

        /// @copydetails SoundUnit::setRelativeMode
        virtual void setRelativeMode(bool mode);

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();
    protected:
        /// @copydetails SoundUnit::onTime
        virtual void onTime();
    protected:
        DefaultSound * mSound;
        SoundTrack * mTrack;
        NIIf mAlpha;
        bool mStop;
    };
}
#endif