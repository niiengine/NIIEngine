/**/

// Nii includes
#include "NiiPreProcess.h"
#include "NiiOpenalVoiceBuffer.h"
namespace NII
{
    OpenalVoiceBuffer::OpenalVoiceBuffer(ALuint buffername) :
        HardwareVoiceBuffer(), mName(buffername), mFormat(AL_INVALID), mFreq(0),
        _DataAligned(NULL), _DataPtr(NULL), _Capacity(0), _Size(0), _StorageMode(HardwareVoiceBuffer::StorageAuto), mLoaded(false)
    {
        
    }
    //--------------------------------------------------------------------------------------
    OpenalVoiceBuffer::~OpenalVoiceBuffer()
    {
        // delete local copy
        if (_DataPtr != NULL)
        {
            delete[] _DataPtr;
            _DataAligned = NULL;
            _DataPtr = NULL;
        }

        // delete OpenAL copy
        CSoundDriverAL *sdal = CSoundDriverAL::getInstance();
        //nlinfo("AL: Deleting buffer (name %u)", mName );
        sdal->removeBuffer( this );
    }
    //--------------------------------------------------------------------------------------
    /// Set the sample format. (channels = 1, 2, ...; bitsPerSample = 8, 16; frequency = samples per second, 44100, ...)
    void OpenalVoiceBuffer::setFormat(VoiceType format, Nui8 channels, Nui8 bitsPerSample, uint32 frequency)
    {
        SampleFormat sampleFormat;
        bufferFormatToSampleFormat(format, channels, bitsPerSample, sampleFormat);
        switch (sampleFormat) 
        {
            case SF_Mono8: mFormat = AL_FORMAT_MONO8; break;
            case SF_Mono16: mFormat = AL_FORMAT_MONO16; break;
            case SF_Stereo8: mFormat = AL_FORMAT_STEREO8; break;
            case SF_Stereo16: mFormat = AL_FORMAT_STEREO16; break;
            default: nlstop; mFormat = AL_INVALID;
        }
        mFreq = frequency;
    }
    //--------------------------------------------------------------------------------------
    /// Get a writable pointer to the buffer of specified size. Returns NULL in case of failure. It is only guaranteed that the original data is still available when using StorageSoftware and the specified size is not larger than the available data. Call setStorageMode() and setFormat() first.
    Nui8 *OpenalVoiceBuffer::lock(uint capacity)
    {
        nlassert((mFormat != AL_INVALID) && (mFreq != 0));

        mLoaded = false;

        if (_DataPtr)
        {
            if (capacity > _Capacity) 
            {
                delete[] _DataPtr;
                _DataAligned = NULL;
                _DataPtr = NULL;
            }
        }
        
        if (!_DataPtr) _DataPtr = new Nui8[capacity + 15];
        _DataAligned = (Nui8 *)((size_t)_DataPtr + ((16 - ((size_t)_DataPtr % 16)) % 16));
        if (_Size > capacity) _Size = capacity;
        _Capacity = capacity;
        
        return _DataAligned;
    }
    //---------------------------------------------------------------------------
    /// Notify that you are done writing to this buffer, so it can be copied over to hardware if needed. Returns true if ok.
    bool OpenalVoiceBuffer::unlock(uint size)
    {
        if (size > _Capacity) 
        {
            _Size = _Capacity;
            return false;
        }
        
        // Fill buffer (OpenAL one)
        _Size = size;
        alBufferData(mName, mFormat, _DataAligned, _Size, mFreq);
        
        if (_StorageMode != HardwareVoiceBuffer::StorageSoftware && !CSoundDriverAL::getInstance()->getOption(ISoundDriver::OptionLocalBufferCopy))
        {
            delete[] _DataPtr;
            _DataAligned = NULL;
            _DataPtr = NULL;
            _Capacity = 0;
        }
        
        // Error handling
        if (alGetError() == AL_NO_ERROR)
            mLoaded = true;
        
        return mLoaded;
    }
    //--------------------------------------------------------------
    /// Copy the data with specified size into the buffer. A readable local copy is only guaranteed when OptionLocalBufferCopy is set. Returns true if ok.
    bool OpenalVoiceBuffer::fill(const Nui8 *src, uint size)
    {
        nlassert((mFormat != AL_INVALID) && (mFreq != 0));

        bool localBufferCopy = CSoundDriverAL::getInstance()->getOption(ISoundDriver::OptionLocalBufferCopy);

        if (_DataPtr)
        {
            if ((!localBufferCopy) || (size > _Capacity)) 
            {
                delete[] _DataPtr;
                _DataAligned = NULL;
                _DataPtr = NULL;
                _Capacity = 0;
            }
        }
        
        _Size = size;
        
        if (localBufferCopy)
        {
            // Force a local copy of the buffer
            if (!_DataPtr) 
            {
                _DataPtr = new Nui8[size + 15];
                _DataAligned = (Nui8 *)((size_t)_DataPtr + ((16 - ((size_t)_DataPtr % 16)) % 16));
                _Capacity = size;
            }
            CFastMem::memcpy(_DataAligned, src, size);
        }
        
        // Fill buffer (OpenAL one)
        alBufferData(mName, mFormat, src, size, mFreq);
        
        // Error handling
        if (alGetError() == AL_NO_ERROR)
            mLoaded = true;
        
        return mLoaded;
    }
    //----------------------------------------------------------------------
    /// Return the sample format information.
    void OpenalVoiceBuffer::getFormat(VoiceType &format, Nui8 &channels, Nui8 &bitsPerSample, uint32 &frequency) const
    {
        SampleFormat sampleFormat;
        switch (mFormat)
        {
            case AL_FORMAT_MONO8: sampleFormat = Mono8; break;
            case AL_FORMAT_MONO16: sampleFormat = Mono16; break;
            case AL_FORMAT_STEREO8: sampleFormat = Stereo8; break;
            case AL_FORMAT_STEREO16: sampleFormat = Stereo16; break;
            default: sampleFormat = SampleFormatUnknown;
        }
        sampleFormatToBufferFormat(sampleFormat, format, channels, bitsPerSample);
        frequency = mFreq;
    }
    //---------------------------------------------------------------------
    /*
     * Return the size of the buffer, in bytes
     */
    uint32 OpenalVoiceBuffer::getSize() const
    {
        return _Size;
        /*ALint value;
        alGetBufferi(mName, AL_SIZE, &value);
        nlassert(alGetError() == AL_NO_ERROR);
        return value;*/
    }
    //---------------------------------------------------------------------
    /*
     * Return the duration (in ms) of the sample in the buffer
     */
    float OpenalVoiceBuffer::getDuration() const
    {
        if ( mFreq == 0 )
            return 0;

        uint32 bytespersample;
        switch ( mFormat ) {
            case AL_FORMAT_MONO8:
                bytespersample = 1;
                break;

            case AL_FORMAT_MONO16:
            case AL_FORMAT_STEREO8:
                bytespersample = 2;
                break;

            case AL_FORMAT_STEREO16:
                bytespersample = 4;
                break;

            default:
                return 0;
        }

        return (float)(getSize()) * 1000.0f / (float)mFreq / (float)bytespersample;
    }
    //-------------------------------------------------------------------------
    /*
     * Return true if the buffer is stereo, false if mono
     */
    bool OpenalVoiceBuffer::isStereo() const
    {
        return (mFormat==AL_FORMAT_STEREO8) || (mFormat==AL_FORMAT_STEREO16);
    }
    //-------------------------------------------------------------------------
    bool OpenalVoiceBuffer::isBufferLoaded() const
    {
        return mLoaded;
    }
    //-------------------------------------------------------------------------    
    /// Set the storage mode of this buffer, call before filling this buffer. Storage mode is always software if OptionSoftwareBuffer is enabled. Default is auto.
    void OpenalVoiceBuffer::setStorageMode(StorageMode storageMode)
    {
        _StorageMode = storageMode;
    }
    //-------------------------------------------------------------------------
    VoiceBuffer::StorageMode OpenalVoiceBuffer::getStorageMode()
    {
        return _StorageMode;
    }
    //-------------------------------------------------------------------------
}