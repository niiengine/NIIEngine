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

#include "NiiOpenalSound.h"

namespace NII
{
    //------------------------------------------------------------------------
    OpenalSound::OpenalSound(ResourceManager * creator, const String & name,
        ResourceHandle handle, const String & group, bool isManual,
            ManualResourceLoader * loader) :
                Sound(creator, name, handle, group, isManual, loader),
                mTarget(AL_NONE),
                mDirectFilter(AL_FILTER_NULL),
                mEffectFilter(AL_FILTER_NULL),
    {

    }
    //------------------------------------------------------------------------
    OpenalSound::~OpenalSound()
    {

    }
    //------------------------------------------------------------------------
    void OpenalSound::loadImpl()
    {

    }
    //------------------------------------------------------------------------
    void OpenalSound::unloadImpl()
    {
        if (mTarget != AL_NONE)
        {
            alDeleteSources(1, &mTarget);
            mTarget = AL_NONE;
        }

        if (mDirectFilter != AL_FILTER_NULL)
        {
            alDeleteFilters(1, &mDirectFilter);
            mDirectFilter = AL_FILTER_NULL;
        }

        if (mEffectFilter != AL_FILTER_NULL)
        {
            alDeleteFilters(1, &mEffectFilter);
            mEffectFilter = AL_FILTER_NULL;
        }
    }
    //------------------------------------------------------------------------
    void OpenalSound::setupImpl();
    {
        // 基本设置
        alGenSources(1, &mTarget);
        getError();

        // configure rolloff
        if (soundDriver->getOption(ISoundDriver::OptionManualRolloff))
        {
            alSourcef(mTarget, AL_ROLLOFF_FACTOR, 0);
            getError();
        }
        else
        {
            alSourcef(mTarget, AL_ROLLOFF_FACTOR, soundDriver->getRolloffFactor());
            getError();
        }

        // create filters
        if (soundDriver->getOption(ISoundDriver::OptionEnvironmentEffects))
        {
            alGenFilters(1, &mDirectFilter);
            alFilteri(mDirectFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
            alFilterf(mDirectFilter, AL_LOWPASS_GAIN, 1.0f);
            alFilterf(mDirectFilter, AL_LOWPASS_GAINHF, 1.0f);
            getError();
            alGenFilters(1, &mEffectFilter);
            alFilteri(mEffectFilter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
            alFilterf(mEffectFilter, AL_LOWPASS_GAIN, 1.0f);
            alFilterf(mEffectFilter, AL_LOWPASS_GAINHF, 1.0f);
            getError();
        }

        //
        
    }
    //------------------------------------------------------------------------
    void OpenalSound::playImpl()
    {

    }
    //------------------------------------------------------------------------
    void OpenalSound::stopImpl()
    {

    }
    //------------------------------------------------------------------------
    void OpenalSound::pauseImpl()
    {

    }
    //------------------------------------------------------------------------
    void OpenalSound::bindImpl()
    {
        alSourceStop(mTarget);
        alTestError();

        // Set buffer
        if (mBuffer == NULL)
        {
            alSourcei(mTarget, AL_BUFFER, AL_NONE );
            alTestError();
        }
        else
        {
            CBufferAL * bufferAL = static_cast<CBufferAL *>(mBuffer);
            alSourcei(_Source, AL_BUFFER, bufferAL->bufferName());
            alTestError();

            // Set relative mode if the buffer is stereo
            mStereo = bufferAL->isStereo();
        }    
    }
    //------------------------------------------------------------------------
    void OpenalSound::getError() const
    {
    	ALuint errcode = alGetError();
        switch (errcode)
        {
        case AL_NO_ERROR : break;
        case AL_INVALID_NAME:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Invalid name"));
        case AL_INVALID_ENUM:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Invalid enum"));
        case AL_INVALID_VALUE:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Invalid value"));
        case AL_INVALID_OPERATION:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Invalid operation"));
        case AL_OUT_OF_MEMORY:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Out of memory"));
        default:
            NII_EXCEPT(Exception::ERR_INVALIDPARAMS, NIIX("Unknown"));
        }
    }
    //------------------------------------------------------------------------
    void OpenalSound::updateManualRolloff()
    {
        CVector distanceVector = mStereo ? mPos : (mPos - CListenerAL::getInstance()->getPos());
        float distanceSquare = distanceVector.sqrnorm();
        float rolloff = Sound::computeRolloff(mFade, distanceSquare, mMinDist, mMaxDist);
        alSourcef(mTarget, AL_GAIN, mGain * rolloff);
        getError();  
    }
    //------------------------------------------------------------------------
    void OpenalSound::addPartner(VoiceBuffer * partner)
    {
        CBufferAL *bufferAL = static_cast<CBufferAL *>(partner);
        ALuint bufferName = bufferAL->bufferName();
        nlassert(bufferName);
        alSourceQueueBuffers(_Source, 1, &bufferName);
        alTestError();
        _QueuedBuffers.push(bufferAL);
        
        // Resume playback if the internal OpenAL source stopped due to buffer underrun.
        ALint srcstate;
        alGetSourcei(_Source, AL_SOURCE_STATE, &srcstate);
        alTestError();
        if (_IsPlaying && (srcstate == AL_STOPPED || srcstate == AL_INITIAL))
        {
            nlwarning("AL: Streaming buffer underrun, resuming playback.");
            play();
        }        
    }
    //------------------------------------------------------------------------
    Nui OpenalSound::getPartnerCount() const
    {
        // a bit ugly here, but makes a much easier/simpler implementation on both drivers
        ALint buffersProcessed;
        alGetSourcei(mTarget, AL_BUFFERS_PROCESSED, &buffersProcessed);
        while (buffersProcessed)
        {
            ALuint bufferName = _QueuedBuffers.front()->bufferName();
            alSourceUnqueueBuffers(_Source, 1, &bufferName);
            alTestError();
            const_cast<std::queue<CBufferAL *> &>(_QueuedBuffers).pop();
            --buffersProcessed;
        }
        // return how many are left in the queue
        //ALint buffersQueued;
        //alGetSourcei(_SourceName, AL_BUFFERS_QUEUED, &buffersQueued);
        //alTestError();
        //return (uint)buffersQueued;
        return (uint)_QueuedBuffers.size();        
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
}