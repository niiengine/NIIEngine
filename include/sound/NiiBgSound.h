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

#ifndef _NII_BGSOUND_H_
#define _NII_BGSOUND_H_

#include "NiiPreInclude.h"
#include "NiiSound.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BgSound : public Sound
    {
    public:
        struct Info
        {
            ReourceID mResourceID;
            Nui32 mMark;
        };
        typedef vector<Info>::type InfoList;
    public:
        BgSound();
        ~BgSound();
        
        /// @copydetails Sound::setProperty
        void setProperty(PropertyData * param);

        /// @copydetails Sound::getSoundType
        Type getSoundType() const               { return T_Bg; }

        /// @copydetails Sound::getDuration
        virtual TimeDurMS getDuration() const;

        /// @copydetails Sound::getSub
        Sound * getSub(Nidx idx) const;
        
        /// @copydetails Sound::getSubCount
        virtual NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;
        
        /// @copydetails Resource::write
        virtual void write(const Serializer * in);

        /**
        @version NIIEngine 4.0.0
        */
        const InfoList & getSounds() const      { return mSoundList; }
    protected:
        /// @copydetails Resource::loadImpl
        virtual void loadImpl();
    private:
        InfoList mSoundList;
    };
    
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI BgSoundUnit : public SoundUnit
    {
    public:
        BgSoundUnit(BgSound * sound, SpaceNode * node = 0);
        ~BgSoundUnit();

        /// @copydetails SoundUnit::getType
        Type getType() const {return T_Bg;}
        
        /// @copydetails SoundUnit::play
        virtual void play();

        /// @copydetails SoundUnit::stop
        virtual void stop();

        /// @copydetails SoundUnit::setGain
        void setGain(NIIf gain);

        /// @copydetails SoundUnit::setRelativeGain
        void setRelativeGain(NIIf gain);

        /// @copydetails SoundUnit::setPos
        void setPos(const Vector3f & pos);

        /// @copydetails SoundUnit::setVelocity
        void setVelocity(const Vector3f & vel);

        /// @copydetails SoundUnit::setDirection
        void setDirection(const Vector3f & dir);

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();

        /**
        @version NIIEngine 4.0.0
        */
        void setGainFilter(vector<NIIf> & value);
    private:
        /// @copydetails SoundUnit::onUpdate
        void onUpdate();
    protected:
        struct Unit
        {
            SoundUnit * mUnit;
            Nui32 mState;
            Nui32 mMark;
        };
        typedef vector<Unit>::type UnitList;
    private:
        BgSound * mSound;
        UnitList mUnitList;
    };
}
#endif