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

#ifndef NII_MUSICSOUND_H
#define NII_MUSICSOUND_H

#include "NiiPreInclude.h"
#include "NiiSound.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI MusicSound : public Sound
    {
    public:
        MusicSound();
        virtual ~MusicSound();
        
        /**
        @version NIIEngine 4.0.0
        */
        inline TimeDurMS getReplayDelay() const {return mReplayDelay; }
        
        /// @copydetails Sound::setProperty
        void setProperty(PropertyData * param);

        /// @copydetails Sound::getSoundType
        virtual Type getSoundType() const {return T_Music;}

        /// @copydetails Sound::getDuration
        virtual TimeDurMS getDuration() const;

        /// @copydetails Sound::getSub
        virtual Sound * getSub(Nidx idx) const;
        
        /// @copydetails Sound::getSubCount
        virtual NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;
        
        /// @copydetails Resource::write
        virtual void write(const Serializer * in);
    protected:
        /// @copydetails Sound::loadImpl
        virtual void loadImpl();
    protected:
        TimeDurMS mReplayDelay;
    };

    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI MusicSoundUnit : public SoundUnit
    {
    public:
        MusicSoundUnit(MusicSound * sound, SpaceNode * node = 0);
        ~MusicSoundUnit();

        /// @copydetails SoundUnit::play
        virtual void play();

        /// @copydetails SoundUnit::stop
        virtual void stop();

        /// @copydetails SoundUnit::getType
        Type getType() const{return T_Music; }

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();
    protected:
        MusicSound * mSound;
        TimeDurMS mLastTime;
    };
}
}
#endif