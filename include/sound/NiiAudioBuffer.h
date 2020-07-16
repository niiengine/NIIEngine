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

#ifndef NII_AUDIOBUFFER_H
#define NII_AUDIOBUFFER_H

#include "NiiPreInclude.h"
#include "NiiBuffer.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class AudioBuffer : public Buffer
    {
        friend class SoundSys;
    public:
        enum ExtMode
        {
            EM_STREAM       = M_EXT1,
            EM_ADPCM        = M_EXT2,
            EM_STEREO       = M_EXT3,
            EM_MONO         = M_EXT4,
            EM_8BIT         = M_EXT5,
            EM_16BIT        = M_EXT6
        };
        
        enum ExtMuteMode
        {
            EMM_ADPCM,
            EMM_Mono16
        };
        
        AudioBuffer(BufferManager * mag, Nmark mode);
        virtual ~AudioBuffer() {}

        /** 
        @version NIIEngine 4.0.0 adv
        */
        inline void setID(BufferID bid)             { mID = bid; }
        
        /** 
        @version NIIEngine 4.0.0
        */
        inline BufferID getID() const               { return mID; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool isStreamMode() const            { return mMode & EM_STREAM; }

        /** 
        @version NIIEngine 4.0.0
        */
        virtual bool isStereo() const = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setFormat(SampleFormat format);

        /** 
        @version NIIEngine 4.0.0
        */
        inline SampleFormat getFormat() const       { return mFormat; }
        
        /**
        @version NIIEngine 4.0.0
        */
        NCount getConvertSample(ExtMuteMode mode, NCount size) const;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRate(NCount rate);

        /**
        @version NIIEngine 4.0.0
        */
        inline NCount getRate() const               { return mRate; }
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual TimeDurMS getDuration() const = 0;
        
        /** 
        @version NIIEngine 4.0.0
        */
        virtual bool isValid() = 0;

        /**
        @version NIIEngine 4.0.0
        */
        static void encodeADPCM(Ni16 * in, Nui8 * out, NCount count, Ni16 & lastSample, Nui16 & lastStep);

        /**
        @version NIIEngine 4.0.0
        */
        static void decodeADPCM(Nui8 * in, Ni16 * out, NCount count, Ni16 & lastSample, Nui16 & lastStep);
    protected:
        AudioBuffer(){}

        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark m, NCount oft, NCount size);
    private:
        BufferID mID;
        Nmark mOpMark;
        NCount mRate;
        SampleFormat mFormat;
    };
}
#endif