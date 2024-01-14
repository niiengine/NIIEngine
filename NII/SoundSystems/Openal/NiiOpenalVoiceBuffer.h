/*
*/

#ifndef NII_OPENAL_VOICE_BUFFER_H_
#define NII_OPENAL_VOICE_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiHardwareBuffer.h"
#include "NiiSharedPtr.h"

#if (NII_PLATFORM == NII_PLATFORM_APPLE)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

namespace NII
{
    class OpenalVoiceBuffer : public VoiceBuffer
    {
    public :
        OpenalVoiceBuffer(ALuint buffername=0 );
        virtual ~OpenalVoiceBuffer();
        
        /// Return the buffer name (as an int)
        inline ALuint bufferName() { return mName; }
        
        /// Set the sample format. (channels = 1, 2, ...; bitsPerSample = 8, 16; frequency = samples per second, 44100, ...)
        virtual void setFormat(VoiceType format, Nui8 channels, Nui8 bitsPerSample, uint32 frequency);
        /// Return the sample format information.
        virtual void getFormat(VoiceType &format, Nui8 &channels, Nui8 &bitsPerSample, uint32 &frequency) const;
            /// Set the storage mode of this buffer, call before filling this buffer. Storage mode is always software if OptionSoftwareBuffer is enabled. Default is auto.
        virtual void setStorageMode(StorageMode storageMode = IBuffer::StorageAuto);
        /// Get the storage mode of this buffer.
        virtual StorageMode getStorageMode();

        /// Get a writable pointer to the buffer of specified size. Use capacity to specify the required bytes. Returns NULL in case of failure. It is only guaranteed that the original data is still available when using StorageSoftware and the specified size is not larger than the size specified in the last lock. Call setStorageMode() and setFormat() first.
        virtual Nui8 *lock(uint capacity);
        /// Notify that you are done writing to this buffer, so it can be copied over to hardware if needed. Set size to the number of bytes actually written to the buffer. Returns true if ok.
        virtual bool unlock(uint size);
        /// Copy the data with specified size into the buffer. A readable local copy is only guaranteed when OptionLocalBufferCopy is set. Returns true if ok.
        virtual bool fill(const Nui8 *src, uint size);
        
        /// Return the size of the buffer, in bytes.
        virtual uint getSize() const;
        /// Return the duration (in ms) of the sample in the buffer.
        virtual float getDuration() const;
        /// Return true if the buffer is stereo (multi-channel), false if mono.
        virtual bool isStereo() const;	
        /// Return true if the buffer is loaded. Used for async load/unload.
        virtual bool isBufferLoaded() const;        
    private:
        /// Buffer name
        ALuint mName;
        /// Sample format
        ALenum mFormat;
        /// Frequency
        ALuint mFreq;
        /// Buffer data (as OpenAL keeps it's own data and doesn't publish it back), aligned 16 byte
        Nui8 *_DataAligned;
        /// The actual pointer used for deletion.
        Nui8 *_DataPtr;
        /// The capacity of the buffer
        uint _Capacity;
        /// The size of the data in the buffer
        uint _Size;
        /// Storage mode
        VoiceBuffer::StorageMode _StorageMode;
        /// Buffer loaded or not
        bool mLoaded;
    };
}
#endif