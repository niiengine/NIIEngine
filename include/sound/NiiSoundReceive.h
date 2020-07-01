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

#ifndef NII_SoundReceive_H
#define NII_SoundReceive_H

#include "NiiPreInclude.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class SoundReceive
    {
    public:
        virtual	~SoundReceive();

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
        virtual void setOrientation(const Vector3f & front, const Vector3f & up) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void getOrientation(Vector3f & front, Vector3f & up) const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setGain(NIIf gain) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getGain() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setDopplerFactor(NIIf f) = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getDopplerFactor() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRolloffFactor(NIIf f) = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getRolloffFactor() const = 0;

        /** 
        @VERSION niieNGINE 4.0.0
        */
        virtual void setOption(Nui32 opt, void * value, NCount size) = 0;
    protected:
        SoundReceive(){}
    };
}
#endif