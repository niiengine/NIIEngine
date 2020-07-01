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

#ifndef NII_SoundSource_h_
#define NII_SoundSource_h_

#include "NiiPreInclude.h"
#include "NiiSoundSys.h"

namespace NII
{
    /** 
    @version NIIEngine 4.0.0
    */
    class SoundSource
    {
    public:
        SoundSource();
        virtual ~SoundSource();

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setBuffer(AudioBuffer * buffer) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual AudioBuffer * getBuffer() = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setAlpha(NIIf a) {}

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setLoop(bool l) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool isLoop() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool play() = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void stop() = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void pause() = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool isPlay() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool isStop() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool update() = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual TimeDurMS getPlayTime() const;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setPos(const Vector3f & pos) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual const Vector3f & getPos() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setVelocity(const Vector3f & vel) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void getVelocity(Vector3f & vel) const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setDirection(const Vector3f & dir) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void getDirection(Vector3f & dir) const = 0;
        
        /**
        @verison NIIEngine 4.0.0
        */
        inline void setDeferMode(bool set) { mDeferMode = set; }
        
        /**
        @verison NIIEngine 4.0.0
        */
        inline bool isDeferMode() const { return mDeferMode; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setGain(float gain) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual float getGain() const = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setPitch(float pitch) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual float getPitch() const = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setRelativeMode(bool mode) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual bool getRelativeMode() const = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setMinDistance(float dist) = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void setMaxDistance(float dist) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual float getMinDistance() const = 0;

        /**
        @verison NIIEngine 4.0.0
        */
        virtual float getMaxDistance() const = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setCone(const Radian & inner, const Radian & outer, NIIf outergain) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void getCone(Radian & inner, Radian & outer, NIIf & outergain) const = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setOption(Nui32 prop, void * value, NCount size) = 0;
    protected:
        Ni32 getRollOff(Ni32 db, Ni32 dbmin, Ni32 dbmax, NIIf squaredist) const;
    protected:
        AudioBuffer * mBuffer;
        bool mDeferMode;
    };
}
#endif