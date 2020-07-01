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

#ifndef NII_COLLECTSound_H
#define NII_COLLECTSound_H

#include "NiiPreInclude.h"
#include "NiiSound.h"

#define CSArgCount 10

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class CollectSound : public Sound
    {
    public:
        class Info
        {
        public:
            Info() : mPosition(Vector3f::Null), mPreValue(100)
            {
                for(uint i = 0; i < CSArgCount; i++)
                    mArgIndex[i] = -1;
            }
        public:
            Ni32 mArgIndex[CSArgCount];
            Nui32 mPreValue;
            Vector3f mPosition;
        };
        class Collecter
        {
        public:
            virtual ~Collecter() {}
            virtual void init(uint * arg) = 0;
            virtual void addSound(Sound * sound, const String & name) = 0;
            virtual Sound * getSound(Info & info, uint32 value) = 0;
            virtual Sound * getSub(Nidx idx) const =0;
            virtual NCount getSoundCount() const = 0;
            virtual TimeDurMS getDuration() const;
            virtual NIIf getMaxDistance() const =0;
        };
    public:
        CollectSound();
        ~CollectSound();

        void init();
        
        Sound * getSound(Info & info);
        
        /// @copydetails Sound::getSoundType
        Type getSoundType() const { return T_Collect; }

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

        /// @copydetails Sound::getMaxDistance
        NIIf getMaxDistance() const;
    protected:
        /// @copydetails Sound::loadImpl
        virtual void loadImpl();
    private:
        Collecter * mContainer;
        String _PatternName;
        String _BaseName;
        Nui32 mValue;
    };
}
#endif