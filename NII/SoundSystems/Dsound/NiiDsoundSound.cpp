
#include "NiiDsoundSound.h"

namespace NII
{
    //----------------------------------------------------------------
    const uint32 DsoundSound::mSecondaryBufferSize = 0x10000;
    const uint32 DsoundSound::_SizeMask = 0xffff;
    const uint32 DsoundSound::_SwapCopySize = 0x8000;
    const uint32 DsoundSound::mUpdateCopySize = 0x4000;
    const uint32 DsoundSound::_XFadeSize = 64;
    const uint DsoundSound::_DefaultChannels = 1;
    const uint DsoundSound::_DefaultSampleRate = 22050;
    const uint DsoundSound::_DefaultSampleSize = 16;
    //----------------------------------------------------------------
    DsoundSound::DsoundSound()
    {
    
    }
    //----------------------------------------------------------------
    DsoundSound::
    
    //----------------------------------------------------------------
    DsoundSound::
    //----------------------------------------------------------------
    DsoundSound::
    //----------------------------------------------------------------
    DsoundSound::
    //----------------------------------------------------------------
    void DsoundSound::release()
    {
#if EAX_AVAILABLE == 1
        if (mEAXSource != 0)
        {
            mEAXSource->Release();
            mEAXSource = 0;
        }
#endif

        if (mSecondaryBuffer != 0)
        {
            mSecondaryBuffer->Stop();
        }

        if (m3DBuffer != 0)
        {
            m3DBuffer->Release();
            m3DBuffer = 0;
        }

        if (mSecondaryBuffer != 0)
        {
            mSecondaryBuffer->Release();
            mSecondaryBuffer = 0;
        }        
    }
    //----------------------------------------------------------------
    Nui32 DsoundSound::getTime()
    {
        if (mMainSample == 0)
            return 0;

        TSampleFormat format;
        uint freq;

        mMainSample->getFormat(format, freq);

        return uint32(1000.0f * (mPlayedOffset+1) / (float)freq);        
    }
    //----------------------------------------------------------------
    void DsoundSound::init(LPDIRECTSOUND directSound, bool useEax)
    {
        // Initialize the buffer format
        WAVEFORMATEX format;

        format.cbSize = sizeof(WAVEFORMATEX);
        format.nChannels = _DefaultChannels;
        format.wBitsPerSample = _DefaultSampleSize;
        format.nSamplesPerSec = _DefaultSampleRate;
        format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
        format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
        format.wFormatTag = WAVE_FORMAT_PCM;


        // Initialize the buffer description

        DSBUFFERDESC desc;

        CSoundDriverDSound* driver = CSoundDriverDSound::instance();


        ZeroMemory(&desc, sizeof(DSBUFFERDESC));
        desc.dwSize = sizeof(DSBUFFERDESC);
        desc.lpwfxFormat = &format;
        desc.dwBufferBytes = mSecondaryBufferSize;
        desc.dwReserved = 0;

        if (driver->countHw3DBuffers() > 0)
        {
            //nldebug("Source: Allocating 3D buffer in hardware");
            desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCHARDWARE | DSBCAPS_GETCURRENTPOSITION2
                            | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_MUTE3DATMAXDISTANCE;
        }
        else
        {
            nldebug("Failed to create a 3D Hardware DirectX secondary buffer. Try 3D software one");

            if (useEax)
            {
                throw ESoundDriver("No 3d hardware sound buffer, but EAX support requested");
            }
            //nldebug("Source: Allocating 3D buffer in software");
            desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2
                            | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_MUTE3DATMAXDISTANCE;
            desc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
            //desc.guid3DAlgorithm = DS3DALG_HRTF_FULL;
        }


        // Allocate the secondary buffer

        if (FAILED(directSound->CreateSoundBuffer(&desc, &mSecondaryBuffer, NULL)))
        {
            if (useEax)
            {
                throw ESoundDriver("Failed to create a 3d hardware sound buffer, but EAX support requested");
            }
            nlwarning("Source: Failed to create a buffer with 3D capabilities.");

            ZeroMemory(&desc, sizeof(DSBUFFERDESC));
            desc.dwSize = sizeof(DSBUFFERDESC);
            desc.lpwfxFormat = &format;
            desc.dwBufferBytes = mSecondaryBufferSize;
            desc.dwReserved = 0;

            if (driver->countHw2DBuffers() > 0)
            {
                //nldebug("Source: Allocating 2D buffer in hardware");
                desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCHARDWARE | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
            }
            else
            {
                //nldebug("Source: Allocating 2D buffer in software");
                desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCSOFTWARE | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
            }

            if (FAILED(directSound->CreateSoundBuffer(&desc, &mSecondaryBuffer, NULL)))
            {
                throw ESoundDriver("Failed to allocate the DirectSound secondary buffer");
            }
        }


        nldebug("Created DirectX secondary buffer @ %p", mSecondaryBuffer);

        // Fill the buffer with silence
        LPVOID ptr;
        DWORD bytes;

        if (FAILED(mSecondaryBuffer->Lock(0, 0, &ptr, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
        {
            throw ESoundDriver("Failed to lock the DirectSound secondary buffer");
        }

        memset(ptr, 0, bytes);

        mSecondaryBuffer->Unlock(ptr, bytes, 0, 0);

        // Allocate the 3D interface, if necessary

        if (FAILED(mSecondaryBuffer->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *) &m3DBuffer)))
        {
           throw ESoundDriver("Failed to allocate the DirectSound 3D buffer");
        }


        if (FAILED(mSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING)))
        {
            throw ESoundDriver("Play failed");
        }        
    }
    //----------------------------------------------------------------
    void DsoundSound::reset()
    {
        setPitch(1.0f);
        setLoops(false);
        setGain(1.0f);
    }
    //----------------------------------------------------------------
    void DsoundSound::setStaticBuffer( const VoiceBuffer *buffer )
    {
        EnterCriticalSection(&mCriticalSection);

        if (mState == source_playing)
        {
            mState = source_swap_pending;
            mMainSample = 0;
            mNextSample = buffer;
            mSampleOffset = 0;
            mPlayedOffset = 0;
        }
        else
        {
            mMainSample = buffer;
            mNextSample = 0;
            mSampleOffset = 0;
            mPlayedOffset = 0;
            mADPCMState.PreviousSample = 0;
            mADPCMState.StepIndex = 0;
            if (buffer)
            {
                buffer->getFormat(mTypeFormat, mSampleFreq);
                switch(mTypeFormat)
                {
                case Mono8:
                    mSampleSize = buffer->getSize();
                    break;
                case Mono16:
                    mSampleSize = buffer->getSize() / 2;
                    break;
                case Mono16ADPCM:
                    mSampleSize = buffer->getSize() * 2;
                    break;
                case Stereo8:
                    mSampleSize = buffer->getSize() / 2;
                    break;
                case Stereo16:
                    mSampleSize = buffer->getSize() / 4;
                    break;
                }
            }
        }
        LeaveCriticalSection(&mCriticalSection);        
    }
    //----------------------------------------------------------------
    VoiceBuffer * DsoundSound::getStaticBuffer()
    {
        if (mState == SS_swap_pending)
            return mNextSample;
        else
            return mMainSample;
    }
    //----------------------------------------------------------------
    void DsoundSound::::getCursors(Cursors &cursors)
    {
        mSecondaryBuffer->GetCurrentPosition((DWORD*)&cursors.PlayCursor, (DWORD*)&cursors.WriteCursor);
        cursors.WriteSize = std::min(mUpdateCopySize, (cursors.PlayCursor - cursors.WriteCursor) & _SizeMask);
    }
    //----------------------------------------------------------------
    void DsoundSound::::fillData(sint16 *dst, uint nbSample)
    {
        nlassert((nbSample & 0xfffffffe) == nbSample);
        if (mMainSample != 0)
        {
            const void *data = ((CBufferDSound*) mMainSample)->getData();
            const sint8 *data8;
            const uint8 *dataAdpcm;
            const sint16 *data16;
            uint	i;

            switch(mTypeFormat)
            {
            case Mono8:
                data8 = (sint8*) data;
                data8 += mSampleOffset;
                for (i=0; i<nbSample; ++i)
                {
                    dst[i] = sint16(data8[i])*256;
                }
                mSampleOffset += nbSample;
                break;
            case Mono16ADPCM:
                dataAdpcm = (uint8*) data;
                dataAdpcm += mSampleOffset/2;
                IBuffer::decodeADPCM(dataAdpcm, dst, nbSample, mADPCMState);
                mSampleOffset += nbSample;
                break;
            case Mono16:
                data16 = (sint16*)data;
                data16 += mSampleOffset;
                CFastMem::memcpy(dst, data16, nbSample*2);
                mSampleOffset += nbSample;
                break;
            case Stereo8:
                data8 = (sint8*) data;
                data8 += mSampleOffset*2;
                for (i=0; i<nbSample; ++i)
                {
                    dst[i] = (sint16(data8[i*2])*128) + (sint16(data8[i*2+1])*128);
                }
                mSampleOffset += nbSample*2;
                break;
            case Stereo16:
                data16 = (sint16*) data;
                data16 += mSampleOffset*2;
                for (i=0; i<nbSample; ++i)
                {
                    dst[i] = (data16[i*2]>>1) + (data16[i*2+1]>>1);
                }
                mSampleOffset += nbSample*2;
                break;
            }

            nlassert(mSampleOffset <= mSampleSize);
        }
        else
        {
            nldebug("Filling : NO DATA from %p to %p (%p sample, %p bytes)", dst, dst+nbSample, nbSample, nbSample*2);

            // write silence in the dst.
            while (nbSample)
            {
                *dst++ = 0;
                --nbSample;
            }
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::fillData(const LockedBufferInfo &lbi, int nbSample)
    {
        nlassert((nbSample & 0x1) == 0);

    	uint size = std::min(uint32(nbSample), lbi.Size1>>1);
        fillData(lbi.Ptr1, size);
        nbSample -= size;

        if (nbSample)
        {
            size = min(uint32(nbSample), lbi.Size2>>1);
            fillData(lbi.Ptr2, size);
            nbSample -= size;
        }
        nlassert(nbSample == 0);    
    }
    //----------------------------------------------------------------
    void DsoundSound::fillSilence(const LockedBufferInfo &lbi, int nbSample)
    {
        uint size = min(uint32(nbSample), lbi.Size1>>1);
        uint tmp = size;
        sint16	*ptr = lbi.Ptr1;

        for (; size != 0; --size)
            *ptr++ = 0;
        nbSample -= tmp;

        if (nbSample)
        {
            size = std::min(uint32(nbSample), lbi.Size2>>1);
            tmp = size;
            ptr = lbi.Ptr2;
            for (; size != 0; --size)
                *ptr++ = 0;
            nbSample -= tmp;
        }
        nlassert(nbSample == 0);        
    }
    //----------------------------------------------------------------
    void DsoundSound::xfade(const LockedBufferInfo &lbi, sint16 *src)
    {
        // do the XFade in integer fixed point arithmetic

        nlassert((_XFadeSize & 0x1) == 0);
        uint	fade = _XFadeSize;
        sint16	*ptr = lbi.Ptr1;
        uint	count = lbi.Size1 /2;
        sint	alpha, invAlpha;

        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            invAlpha = 0x10000 - alpha;
            *ptr = (sint(*ptr)*alpha + sint(*src) * invAlpha) >> 16;
            ++src;
            ++ptr;
            --count;
            --fade;
        }

        ptr = lbi.Ptr2;
        count = lbi.Size2 /2;

        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            invAlpha = 0x10000 - alpha;
            *ptr = (sint(*ptr)*alpha + sint(*src) * invAlpha) >> 16;
            ++src;
            ++ptr;
            --count;
            --fade;
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::fadeOut(const LockedBufferInfo &lbi)
    {
        nlassert((_XFadeSize & 0x1) == 0);
        uint	fade = _XFadeSize;
        sint16	*ptr = lbi.Ptr1;
        uint	count = lbi.Size1/2;
        sint	alpha;

        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            *ptr = (*ptr*alpha) >> 16;
            ++ptr;
            --count;
            --fade;
        }

        ptr = lbi.Ptr2;
        count = lbi.Size2/2;


        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            *ptr = (*ptr*alpha) >> 16;
            ++ptr;
            --count;
            --fade;
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::fadeIn(const LockedBufferInfo &lbi)
    {
        // do the XFade in integer fixed point arithmetic

        nlassert((_XFadeSize & 0x1) == 0);
        uint	fade = _XFadeSize;
        sint16	*ptr = lbi.Ptr1;
        uint	count = lbi.Size1 /2;
        sint	alpha, invAlpha;

        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            invAlpha = 0x10000 - alpha;
            *ptr = (*ptr*invAlpha) >> 16;
            ++ptr;
            --count;
            --fade;
        }

        ptr = lbi.Ptr2;
        count = lbi.Size2 /2;

        while (fade && count)
        {
            alpha = (fade<<16) / _XFadeSize;
            invAlpha = 0x10000 - alpha;
            *ptr = (*ptr*invAlpha) >> 16;
            ++ptr;
            --count;
            --fade;
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::advanceFill(LockedBufferInfo &lbi, uint nbSample)
    {
        uint32 size = nbSample * 2;
        if (lbi.Size1 < size)
        {
            size -= lbi.Size1;
            lbi.Size1 = lbi.Size2;
            lbi.Size2 = 0;
            lbi.Ptr1 = lbi.Ptr2;
            lbi.Ptr2 = 0;
        }

        nlassert(lbi.Size1 >= size);
        lbi.Size1 -= size;
        lbi.Ptr1 += size/2;

        mFillOffset += nbSample*2;
        nlassert(mFillOffset == (mFillOffset & 0xfffffffC));
        mFillOffset &= _SizeMask;
        nlassert(mFillOffset < mSecondaryBufferSize);    
    }
    //----------------------------------------------------------------
    bool DsoundSound::play()
    {

        EnterCriticalSection(&mCriticalSection);

        mSilenceWriten = 0;


        Cursors cursors;
        getCursors(cursors);

        // set a new filling point
        mFillOffset = cursors.WriteCursor;
        mFillOffset = (mFillOffset+3) & 0xfffffffC;
        cursors.WriteCursor = mFillOffset;

        LockedBufferInfo lbi;
        if (lock(mFillOffset, cursors.WriteSize, lbi))
        {
            LockedBufferInfo unlockInfo(lbi);
            // ok, the buffer is locked, write data
            if (mState == source_swap_pending)
            {
                // we swap the buffer.
                mMainSample = mNextSample;
                mNextSample = 0;
                if (mMainSample != 0)
                {
                    mMainSample->getFormat(mTypeFormat, mSampleFreq);
                    switch(mTypeFormat)
                    {
                    case Mono8:
                        mSampleSize = mMainSample->getSize();
                        break;
                    case Mono16:
                        mSampleSize = mMainSample->getSize() / 2;
                        break;
                    case Mono16ADPCM:
                        mSampleSize = mMainSample->getSize() * 2;
                        break;
                    case Stereo8:
                        mSampleSize = mMainSample->getSize() / 2;
                        break;
                    case Stereo16:
                        mSampleSize = mMainSample->getSize() / 4;
                        break;
                    }
                    mState = source_playing;
                }
                else
                {
                    mSampleSize = 0;
                    mState = source_silencing;
                }
            }
            mLastPlayPos = cursors.PlayCursor;
            mSampleOffset = 0;
            mPlayedOffset = 0;
            mADPCMState.PreviousSample = 0;
            mADPCMState.StepIndex = 0;
            // Compute the size of data to write.
            uint dataToFill = std::min(uint(cursors.WriteSize / 2), mSampleSize - mSampleOffset);
            dataToFill &= 0xfffffffe;
            // ok, the buffer is locked, write data
            if (mState == source_playing || mState == source_silencing)
            {
                // we need a little XFade
                sint16	fadeBuffer[_XFadeSize];
                fillData(fadeBuffer, _XFadeSize);
                xfade(lbi, fadeBuffer);
                advanceFill(lbi, _XFadeSize);
                cursors.WriteSize -= _XFadeSize*2;
                dataToFill -= _XFadeSize;
            }
            else
            {
                // we need a little FadeIn
                fillData(lbi, _XFadeSize);
                fadeIn(lbi);
                advanceFill(lbi, _XFadeSize);
                cursors.WriteSize -= _XFadeSize*2;
                dataToFill -= _XFadeSize;
            }
            fillData(lbi, dataToFill);
            cursors.WriteSize -= dataToFill * 2;
            advanceFill(lbi, dataToFill);
            mState = source_playing;
            if (_Loop)
            {
                while (cursors.WriteSize >= 4)
                {
                    if (mSampleOffset == mSampleSize)
                    {
                        // rewind the sample
                        mSampleOffset = 0;
                        mADPCMState.PreviousSample = 0;
                        mADPCMState.StepIndex = 0;
                    }
                    nlassert(mSampleOffset < mSampleSize);
                    dataToFill = std::min(uint(cursors.WriteSize / 2), mSampleSize - mSampleOffset);
                    dataToFill &= 0xfffffffe;
                    fillData(lbi, dataToFill);
                    advanceFill(lbi, dataToFill);
                    cursors.WriteSize -= dataToFill*2;
                }
            }
            else
            {
                if (mSampleOffset == mSampleSize)
                {

                    fillSilence(lbi, cursors.WriteSize/2);
                    advanceFill(lbi, cursors.WriteSize/2);
                    mSilenceWriten = cursors.WriteSize;
                    cursors.WriteSize = 0;
                }

            }


            unlock(unlockInfo);
        }
        else
        {
            nlwarning("Couldn't lock the sound buffer for %u bytes", cursors.WriteSize);
        }

        // set the volume NOW
        CListenerDSound* listener = CListenerDSound::instance();

        updateVolume(listener->getPos());

        LeaveCriticalSection(&mCriticalSection);

        return true;    
    }
    //----------------------------------------------------------------
    void DsoundSound::stop()
    {
        EnterCriticalSection(&mCriticalSection);

        if (mState != source_stopped && mState != source_silencing)
        {
            // retreive the cursors;
            Cursors	cursors;
            getCursors(cursors);

            mFillOffset = cursors.WriteCursor;
            mFillOffset = (mFillOffset+3) & 0xfffffffC;

            LockedBufferInfo lbi;
            if (lock(mFillOffset, cursors.WriteSize, lbi))
            {
                LockedBufferInfo unlockInfo(lbi);

                fadeOut(lbi);
                advanceFill(lbi, _XFadeSize);
                cursors.WriteSize -= _XFadeSize*2;
                fillSilence(lbi, cursors.WriteSize/2);
                advanceFill(lbi, cursors.WriteSize/2);
                mSilenceWriten = cursors.WriteSize;

                mState = source_silencing;

                unlock(unlockInfo);
            }
        }

        LeaveCriticalSection(&mCriticalSection);    
    }
    //----------------------------------------------------------------
    void DsoundSound::pause()
    {
        assert(false);
    }
    //----------------------------------------------------------------
    bool DsoundSound::update()
    {
        bool updateDone = false;

        EnterCriticalSection(&mCriticalSection);

        // fill some data into the buffer
        Cursors cursors;
        getCursors(cursors);
        uint32	writeSize;

        // The total size available for fill(between fillOffset and play cusors)
        uint32 fillSize = (cursors.PlayCursor - mFillOffset) & _SizeMask;
        // The play margin (between play and write cursor)
        uint32 margin = (cursors.WriteCursor - cursors.PlayCursor) & _SizeMask;
        // The number of sample played since previous update
        uint32 samplePlayed = ((cursors.PlayCursor - mLastPlayPos) & _SizeMask) / 2;
        mLastPlayPos = cursors.PlayCursor;

        if (fillSize + margin > mSecondaryBufferSize)
        {
            // arg !
            nlwarning("FillOffset is between play and write cursor (P = %p F = %p W = %p!", cursors.PlayCursor, mFillOffset, cursors.WriteCursor);
            mFillOffset = cursors.WriteCursor;
            mFillOffset = (mFillOffset+3) & 0xfffffffC;
            mSilenceWriten = 0;
            nlassert(mFillOffset < mSecondaryBufferSize);
        }

        // advance of the fill offset over the write cursor
        uint32 advance = (mFillOffset - cursors.WriteCursor) & _SizeMask;

        if (advance > mUpdateCopySize)
        {
            // enougth data wrote, wait until next update
            cursors.WriteSize = 0;
        }

        cursors.WriteCursor = mFillOffset;
        writeSize = cursors.WriteSize;	// compute the real played sample offset

        // update the real number of played sample
        if (mState == source_playing)
            mPlayedOffset += samplePlayed;

        if (writeSize >= mUpdateCopySize)
        {
            writeSize = mUpdateCopySize;
            updateDone = true;
            LockedBufferInfo lbi;
            if (lock(mFillOffset, writeSize, lbi))
            {
                LockedBufferInfo unlockInfo(lbi);
                if (mState == source_playing)
                {
                    nlassert(mSampleOffset <= mSampleSize);
                    uint32	updateSize = min(mSampleSize - mSampleOffset, uint(writeSize/2));
                    updateSize &= 0xfffffffe;
                    fillData(lbi, updateSize);
                    advanceFill(lbi, updateSize);
                    writeSize -= updateSize*2;

                    if (_Loop)
                    {
                        while (mPlayedOffset >= mSampleSize)
                            mPlayedOffset -= mSampleSize;

                        // repeat until we have at least 2 sample to write
                        while (writeSize >= 4)
                        {
                            if (mSampleOffset == mSampleSize)
                            {
                                // rewind the sample
                                mSampleOffset = 0;
                                mADPCMState.PreviousSample = 0;
                                mADPCMState.StepIndex = 0;
                            }

                            updateSize = min(mSampleSize - mSampleOffset, uint(writeSize/2));
                            updateSize &= 0xfffffffe;
                            fillData(lbi, updateSize);
                            advanceFill(lbi, updateSize);
                            writeSize -= updateSize*2;
                        }
                    }
                    else
                    {
                        if (mSampleOffset == mSampleSize)
                        {
                            fillSilence(lbi, writeSize/2);
                            advanceFill(lbi, writeSize/2);
                            mSilenceWriten += writeSize;
                        }
                        if (mPlayedOffset >= mSampleSize)
                        {
                            // all the sample is played, no we are in silencing state !
                            mPlayedOffset = mSampleSize;
                            mState = source_silencing;
                        }
                    }

                }
                else if (mState == source_swap_pending)
                {
                    // the sample has been changed but not replayed yet ? so we 'stop' the old buffer

                    fadeOut(lbi);
                    advanceFill(lbi, _XFadeSize);
                    writeSize -= _XFadeSize*2;
                    fillSilence(lbi, writeSize/2);
                    advanceFill(lbi, writeSize/2);
                    mSilenceWriten = writeSize;
                    mState = source_silencing;
                }
                else if (mState == source_silencing)
                {
                    // write silence into the buffer.
                    uint32 updateSize = min(writeSize, mSecondaryBufferSize - mSilenceWriten);
                    updateSize &= 0xfffffffC;
                    fillSilence(lbi, updateSize/2);
                    advanceFill(lbi, updateSize/2);
                    mSilenceWriten += updateSize;

                    if (mSilenceWriten == mSecondaryBufferSize)
                        mState = source_stopped;
                }
                else
                {
                    nlwarning("Update not needed !");
                }

                unlock(unlockInfo);
            }
        }

        LeaveCriticalSection(&mCriticalSection);

        return updateDone;    
    }
    //----------------------------------------------------------------
    void DsoundSound::updatePos()
    {
        if (m3DBuffer != NULL)
        {
            if (m3DBuffer->SetPosition(mPos.x, mPos.z, mPos.y, deferred ? DS3D_DEFERRED : DS3D_IMMEDIATE) != DS_OK)
            {
                nlwarning ("SetPosition failed");
            }
        }            
    }
    //----------------------------------------------------------------
    void DsoundSound::updateVel()
    {
        if (m3DBuffer != NULL)
        {
            if (m3DBuffer->SetVelocity(mVelocity.x, mVelocity.z, mVelocity.y, deferred ? DS3D_DEFERRED : DS3D_IMMEDIATE) != DS_OK)
            {
                nlwarning ("SetVelocity failed");
            }
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::updateDir()
    {
        if (m3DBuffer != NULL)
        {
            if (m3DBuffer->SetConeOrientation(mDir.x, mDir.z, mDir.y, DS3D_DEFERRED) != DS_OK)
            {
                nlwarning ("SetConeOrientation failed (x=%.2f, y=%.2f, z=%.2f)", dir.x, dir.y, dir.z);
            }
        }
    }
    //----------------------------------------------------------------
    void DsoundSound::updateGen()
    {
        /* convert from linear amplitude to hundredths of decibels */
        _Volume = (uint32)(100.0 * 20.0 * log10(gain));
        clamp(_Volume, DSBVOLUME_MIN, DSBVOLUME_MAX);     
    }
    //----------------------------------------------------------------
    void DsoundSound::updatePit()
    {
        if ((mMainSample != 0) && (mSecondaryBuffer != 0))
        {
            SampleFormat format;
            uint freq;

            mMainSample->getFormat(format, freq);

            mSampleRate = (uint32) (coeff * (float) freq);


            if (mSecondaryBuffer->SetFrequency(mSampleRate) != DS_OK)
            {
                nlwarning("SetFrequency");
            }
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::setNorMode(bool mode)
    {
        if (m3DBuffer != 0)
        {
            HRESULT hr;

            if (mode)
            {
                hr = m3DBuffer->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
            }
            else
            {
                hr = m3DBuffer->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);
            }

            // cache
            if (hr == DS_OK)
                _PosRelative= mode;
            else
                nlwarning("SetMode failed");
        }
        else
        {
            nlwarning("Requested setSourceRelativeMode on a non-3D source");
        }    
    }
    //----------------------------------------------------------------
    void DsoundSound::updateMMD()
    {
        if (m3DBuffer != 0)
        {
            if (m3DBuffer->SetMinDistance(std::max(DS3D_DEFAULTMINDISTANCE, mMinDist), mDeferred ? DS3D_DEFERRED : DS3D_IMMEDIATE) != DS_OK)
            {
                nlwarning("SetMinDistance (%f) failed", mindist);
            }
            if (m3DBuffer->SetMaxDistance(std::min(DS3D_DEFAULTMAXDISTANCE, mMaxDist), mDeferred ? DS3D_DEFERRED : DS3D_IMMEDIATE) != DS_OK)
            {
                nlwarning("SetMaxDistance (%f) failed", maxdist);
            }
        }
        else
        {
            nlwarning("Requested setMinMaxDistances on a non-3D source");
        }        
    }
    //----------------------------------------------------------------
    void DsoundSound::updateVolume(const Vector3f & listener)
    {
        if (!CSoundDriverDSound::instance()->getOption(ISoundDriver::OptionManualRolloff))
        {
            // API controlled rolloff => return (just set the volume)
            mSecondaryBuffer->SetVolume(_Volume);
        }
        else // manual rolloff
        {
            Vector3f pos = getPos();
            // make relative to listener (if not already!)
            if(!_PosRelative)
                pos -= listener;
            float sqrdist = pos.normal();

            float mindist, maxdist;
            mindist=get3DMixDistance();
            maxdist=get3DMaxDistance()
            // attenuate the volume according to distance and alpha
            sint32 volumeDB = Sound::computeManualRollOff(_Volume, DSBVOLUME_MIN, DSBVOLUME_MAX, _Alpha, sqrdist, mindist, maxdist);

            // set attenuated volume
            mSecondaryBuffer->SetVolume(volumeDB);
        }        
    }
    //----------------------------------------------------------------
    void DsoundSound::updateCon()
    {
        if (m3DBuffer != 0)
        {
            // Set the cone angles

            // Convert from radians to degrees
            DWORD inner = (DWORD)(180.0 * mInner / Pi);
            DWORD outer = (DWORD)(180.0 * mOuter / Pi);


            // Sanity check: wrap the angles in the [0,360] interval
            if (outer < inner)
            {
                outer = inner;
            }

            while (inner < DS3D_MINCONEANGLE)
            {
                inner += 360;
            }

            while (inner > DS3D_MAXCONEANGLE)
            {
                inner -= 360;
            }

            while (outer < DS3D_MINCONEANGLE)
            {
                outer += 360;
            }

            while (outer > DS3D_MAXCONEANGLE)
            {
                outer -= 360;
            }

            if (m3DBuffer->SetConeAngles(inner, outer, DS3D_DEFERRED) != DS_OK)
            {
                nlwarning("SetConeAngles failed");
            }

            // Set the outside volume
            if (mOutGain < 0.00001f)
            {
                mOutGain = 0.00001f;
            }

            // convert from linear amplitude to hundredths of decibels
            LONG volume = (LONG)(100.0 * 20.0 * log10(mOutGain));

            if (volume < DSBVOLUME_MIN)
            {
                volume = DSBVOLUME_MIN;
            }
            else if (volume > DSBVOLUME_MAX)
            {
                volume = DSBVOLUME_MAX;
            }

            if (m3DBuffer->SetConeOutsideVolume(volume, DS3D_DEFERRED) != DS_OK)
            {
                nlwarning("SetConeOutsideVolume failed");
            }

        }
        else
        {
            nlwarning("Requested setCone on a non-3D source");
        }    
    }
    //----------------------------------------------------------------
    VoiceBuffer * DsoundSound::getBuffer()
    {
        return mMainSample;
    }
    //----------------------------------------------------------------
    bool DsoundSound::lock(uint32 offset, uint32 size, LockedBufferInfo &lbi)
    {
        HRESULT hr = mSecondaryBuffer->Lock(offset, size, (LPVOID*) &lbi.Ptr1, (DWORD*) &lbi.Size1, (LPVOID*) &lbi.Ptr2, (DWORD*) &lbi.Size2, 0);

        if (hr == DSERR_BUFFERLOST)
        {
            // If the buffer got lost somehow, try to restore it.
            if (FAILED(mSecondaryBuffer->Restore()))
            {
                nlwarning("Lock failed (1)");
                return false;
            }
            if (FAILED(mSecondaryBuffer->Lock(offset, size, (LPVOID*) &lbi.Ptr1, (DWORD*)&lbi.Size1, (LPVOID*) &lbi.Ptr2, (DWORD*)&lbi.Size2, 0)))
            {
                nlwarning("Lock failed (2)");
                return false;
            }
        }
        else if (hr != DS_OK)
        {
            nlwarning("Lock failed (3)");
            return false;
        }

        return true;    
    }
    //----------------------------------------------------------------
    bool DsoundSound::unlock(const LockedBufferInfo &lockedInfo)
    {
        mSecondaryBuffer->Unlock(lbi.Ptr1, lbi.Size1, lbi.Ptr2, lbi.Size2);
        return true;    
    }
    //------------------------------------------------------------
    bool DsoundSound::isPlaying() const
    {
        return mState == source_playing || mState == source_swap_pending;
    }
    //------------------------------------------------------------
    bool DsoundSound::isPaused() const
    {
        //todo
        assert(false);
        return false;
    }
    //------------------------------------------------------------
    bool DsoundSound::isStopped() const
    {
        return mState == source_silencing || mState == source_stopped;
    }
    //------------------------------------------------------------
    bool DsoundSound::needUpdate()const
    {
        return mState == source_silencing || mState == source_playing || mState == source_swap_pending;
    }    
    //----------------------------------------------------------------
    DsoundSound::
    {
    
    }
    //----------------------------------------------------------------
    DsoundSound::
    {
    
    }
}