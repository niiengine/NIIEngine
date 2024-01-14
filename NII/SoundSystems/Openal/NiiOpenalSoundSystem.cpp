#include "NiiPreProcess.h"
#include "NiiOpenalSoundSystem.h"

namespace NII
{
    OpenalSoundSystem::OpenalSoundSystem()
    {
        alExtInit();        
    }
    //--------------------------------------------------------
    virtual OpenalSoundSystem::~OpenalSoundSystem()
    {
        // Release internal resources of all remaining IMusicChannel instances
        if (_MusicChannels.size())
        {
            nlwarning("AL: _MusicChannels.size(): '%u'", (uint32)_MusicChannels.size());
            set<CMusicChannelAL *>::iterator it(_MusicChannels.begin()), end(_MusicChannels.end());
            for (; it != end; ++it) delete *it;
            _MusicChannels.clear();
        }
        // Remove the allocated (but not exported) source and buffer names-
        // Release internal resources of all remaining ISource instances
        if (_Sources.size())
        {
            nlwarning("AL: _Sources.size(): '%u'", (uint32)_Sources.size());
            set<CSourceAL *>::iterator it(_Sources.begin()), end(_Sources.end());
            for (; it != end; ++it) delete *it;
            _Sources.clear();
        }
        if (!_Buffers.empty()) alDeleteBuffers(compactAliveNames(_Buffers, alIsBuffer), &*_Buffers.begin());	
        // Release internal resources of all remaining IEffect instances
        if (_Effects.size())
        {
            nlwarning("AL: _Effects.size(): '%u'", (uint32)_Effects.size());
            set<CEffectAL *>::iterator it(_Effects.begin()), end(_Effects.end());
            for (; it != end; ++it) delete *it;
            _Effects.clear();
        }

        // OpenAL exit
        if (_AlContext) 
        { 
            alcDestroyContext(_AlContext); 
            _AlContext = NULL; 
        }
        if (_AlDevice) 
        { 
            alcCloseDevice(_AlDevice); 
            _AlDevice = NULL; 
        }        
    }
    //--------------------------------------------------------
    OpenalSoundSystem::getDevices(vector<String> &devices)
    {
        devices.push_back(""); // empty

        if (AlEnumerateAllExt)
        {	
            const ALchar* deviceNames = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
            // const ALchar* defaultDevice = NULL;
            if(!strlen(deviceNames))
            {
                nldebug("AL: No audio devices");
            }
            else
            {
                nldebug("AL: Listing devices: ");
                while(deviceNames && *deviceNames)
                {
                    nldebug("AL:   - %s", deviceNames);
                    devices.push_back(deviceNames);
                    deviceNames += strlen(deviceNames) + 1;
                }
            }
        }
        else
        {
            nldebug("AL: ALC_ENUMERATE_ALL_EXT not present");
        }        
    }
    //--------------------------------------------------------
    static const ALchar *getDeviceInternal(const String &device)
    {
        if (device.empty()) return NULL;
        if (AlEnumerateAllExt)
        {	
            const ALchar* deviceNames = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
            // const ALchar* defaultDevice = NULL;
            if(!strlen(deviceNames))
            {
                nldebug("AL: No audio devices");
            }
            else
            {
                while (deviceNames && *deviceNames)
                {
                    if (!strcmp(deviceNames, device.c_str()))
                        return deviceNames;
                    deviceNames += strlen(deviceNames) + 1;
                }
            }
        }
        else
        {
            nldebug("AL: ALC_ENUMERATE_ALL_EXT not present");
        }
        nldebug("AL: Device '%s' not found", device.c_str());
        return NULL;
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::initDevice(const String &device, ISoundDriver::TSoundOptions options)
    {
        // list of supported options in this driver
        // no adpcm, no manual rolloff (for now)
        const sint supportedOptions = 
            OptionEnvironmentEffects
            | OptionSoftwareBuffer
            | OptionManualRolloff
            | OptionLocalBufferCopy
            | OptionHasBufferStreaming;

        // list of forced options in this driver
        const sint forcedOptions = 0;

        // set the options
        _Options = (TSoundOptions)(((sint)options & supportedOptions) | forcedOptions);
        
        /* TODO: multichannel */

        // OpenAL initialization
        const ALchar *dev = getDeviceInternal(device);
        nldebug("AL: Opening device: '%s'", dev == NULL ? "NULL" : dev);
        _AlDevice = alcOpenDevice(dev);
        if (!_AlDevice) throw ESoundDriver("AL: Failed to open device");
        nldebug("AL: ALC_DEVICE_SPECIFIER: '%s'", alcGetString(_AlDevice, ALC_DEVICE_SPECIFIER));
        //int attrlist[] = { ALC_FREQUENCY, 48000,
        //                   ALC_MONO_SOURCES, 12, 
        //                   ALC_STEREO_SOURCES, 4, 
        //                   ALC_INVALID };
        _AlContext = alcCreateContext(_AlDevice, NULL); // attrlist);
        if (!_AlContext) { alcCloseDevice(_AlDevice); throw ESoundDriver("AL: Failed to create context"); }
        alcMakeContextCurrent(_AlContext);
        alTestError();

        // Display version information
        const ALchar *alversion, *alrenderer, *alvendor, *alext;
        alversion = alGetString(AL_VERSION);
        alrenderer = alGetString(AL_RENDERER);
        alvendor = alGetString(AL_VENDOR);
        alext = alGetString(AL_EXTENSIONS);
        alTestError();
        nldebug("AL: AL_VERSION: '%s', AL_RENDERER: '%s', AL_VENDOR: '%s'", alversion, alrenderer, alvendor);
        nldebug("AL: AL_EXTENSIONS: %s", alext);

        // Load and display extensions
        alExtInitDevice(_AlDevice);
    #if EAX_AVAILABLE
        nlinfo("AL: EAX: %s, EAX-RAM: %s, ALC_EXT_EFX: %s", 
            AlExtEax ? "Present" : "Not available", 
            AlExtXRam ? "Present" : "Not available", 
            AlExtEfx ? "Present" : "Not available");
    #else
        nldebug("AL: EAX-RAM: %s, ALC_EXT_EFX: %s", 
            AlExtXRam ? "Present" : "Not available", 
            AlExtEfx ? "Present" : "Not available");
    #endif
        alTestError();

        nldebug("AL: Max. sources: %u, Max. effects: %u", (uint32)countMaxSources(), (uint32)countMaxEffects());

        if (getOption(OptionEnvironmentEffects)) 
        {
            if (!AlExtEfx)
            {
                nlwarning("AL: ALC_EXT_EFX is required, environment effects disabled");
                _Options = (TSoundOptions)((uint)_Options & ~OptionEnvironmentEffects);
            }
            else if (!countMaxEffects())
            {		
                nlwarning("AL: No effects available, environment effects disabled");
                _Options = (TSoundOptions)((uint)_Options & ~OptionEnvironmentEffects);
            }
        }

        // Choose the I3DL2 model (same as DirectSound3D if not manual)
        if (getOption(OptionManualRolloff)) alDistanceModel(AL_NONE);
        else alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
        alTestError();

        // Initial buffers and sources allocation
        allocateNewItems(alGenBuffers, alIsBuffer, _Buffers, _NbExpBuffers, INITIAL_BUFFERS);
        alTestError();        
    }
    //--------------------------------------------------------
    SoundSystem::SoundOptions OpenalSoundSystem::getOptions()
    {
        return _Options;
    }
    //--------------------------------------------------------
    bool OpenalSoundSystem::getOption(ISoundDriver::TSoundOptions option)
    {
        return ((uint)_Options & (uint)option) == (uint)option;
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::allocateNewItems(TGenFunctionAL algenfunc, TTestFunctionAL altestfunc,
        vector<ALuint>& names, uint index, uint nb )
    {
        nlassert( index == names.size() );
        names.resize( index + nb );
        // FIXME assumption about inner workings of vector;
        // &(names[...]) only works with "names.size() - nbalive == 1"
        generateItems( algenfunc, altestfunc, nb, &(names[index]) );        
    }
    //--------------------------------------------------------
    void ThrowGenException( TGenFunctionAL algenfunc )
    {
        if ( algenfunc == alGenBuffers )
            throw ESoundDriverGenBuf();
        else if ( algenfunc == alGenSources )
            throw ESoundDriverGenSrc();
        else
            nlstop;
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::generateItems( TGenFunctionAL algenfunc, TTestFunctionAL altestfunc, uint nb, ALuint *array )
    {
    	// array is actually a vector element address!
        algenfunc( nb, array );

        // Error handling
        if ( alGetError() != AL_NO_ERROR )
        {
            ThrowGenException( algenfunc );
        }

        // Check buffers
        uint i;
        for ( i=0; i!=nb; i++ )
        {
            if ( ! altestfunc( array[i] ) )
            {
                ThrowGenException( algenfunc );
            }
        }    
    }
    //--------------------------------------------------------
    VoiceBuffer * OpenalSoundSystem::createBuffer()
    {
        CBufferAL *buffer = new CBufferAL(createItem(alGenBuffers, alIsBuffer, _Buffers, _NbExpBuffers, BUFFER_ALLOC_RATE));
        return buffer;        
    }
    //--------------------------------------------------------
    Sound* OpenalSoundSystem::createSource()
    {
        CSourceAL *sourceAl = new CSourceAL(this);
        _Sources.insert(sourceAl);
        return sourceAl;
    }
    //--------------------------------------------------------
    SoundEffer * OpenalSoundSystem::createReverbEffect()
    {
        IReverbEffect *ieffect = NULL;
        CEffectAL *effectal = NULL;
        
        ALuint slot = AL_NONE;
        alGenAuxiliaryEffectSlots(1, &slot);
        if (alGetError() != AL_NO_ERROR)
        {
            nlwarning("AL: alGenAuxiliaryEffectSlots failed");
            return NULL;
        }
        
        ALuint effect = AL_NONE;
        alGenEffects(1, &effect);
        if (alGetError() != AL_NO_ERROR)
        {
            nlwarning("AL: alGenEffects failed");
            alDeleteAuxiliaryEffectSlots(1, &slot);
            return NULL; /* createEffect */
        }

    #if EFX_CREATIVE_AVAILABLE
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
        if (alGetError() != AL_NO_ERROR)
        {
            nlinfo("AL: Creative Reverb Effect not supported, falling back to standard Reverb Effect");
        }
        else
        {
            alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect); alTestError();
            alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE); alTestError(); // auto only for reverb!
            CCreativeReverbEffectAL *eff = new CCreativeReverbEffectAL(this, effect, slot);
            ieffect = static_cast<IReverbEffect *>(eff);
            effectal = static_cast<CEffectAL *>(eff);
            nlassert(ieffect); nlassert(effectal);
            _Effects.insert(effectal);
            return ieffect;
        }
    #endif		

        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        if (alGetError() != AL_NO_ERROR)
        {
            nlwarning("AL: Reverb Effect not supported");
            alDeleteAuxiliaryEffectSlots(1, &slot);
            alDeleteEffects(1, &effect);
            return NULL; /* createEffect */
        }
        else
        {
            alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect); alTestError();
            alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, AL_TRUE); alTestError(); // auto only for reverb!
            CStandardReverbEffectAL *eff = new CStandardReverbEffectAL(this, effect, slot);
            ieffect = static_cast<IReverbEffect *>(eff);
            effectal = static_cast<CEffectAL *>(eff);
            nlassert(ieffect); nlassert(effectal);
            _Effects.insert(effectal);
            return ieffect;
        }
    }
    //--------------------------------------------------------
    static uint getMaxNumSourcesInternal()
    {
        ALuint sources[256];
        memset(sources, 0, sizeofarray(sources));
        uint sourceCount = 0;
        
        alGetError();
        
        for (; sourceCount < 256; ++sourceCount)
        {
            alGenSources(1, &sources[sourceCount]);
            if (alGetError() != AL_NO_ERROR)
                break;
        }
        
        alDeleteSources(sourceCount, sources);
        if (alGetError() != AL_NO_ERROR)
        {
            for (uint i = 0; i < 256; i++)
            {
                alDeleteSources(1, &sources[i]);
            }
        }

        alGetError();

        return sourceCount;
    }
    //--------------------------------------------------------
    uint OpenalSoundSystem::countMaxSources()
    {
        // ALC_MONO_SOURCES
        // software allows 256 sources (software audio ftw!)
        // cheap openal cards 32, expensive openal cards 128
        // trying to go too high is safely handled anyways
        return getMaxNumSourcesInternal() + (uint)_Sources.size();        
    }
    //--------------------------------------------------------
    uint OpenalSoundSystem::countMaxEffects()
    {
        if (!getOption(OptionEnvironmentEffects)) return 0;
        if (!AlExtEfx) return 0;
        ALCint max_auxiliary_sends;
        alcGetIntegerv(_AlDevice, ALC_MAX_AUXILIARY_SENDS, 1, &max_auxiliary_sends);
        return (uint)max_auxiliary_sends;        
    }
    //--------------------------------------------------------
    ALuint OpenalSoundSystem::createItem(TGenFunctionAL algenfunc, TTestFunctionAL altestfunc,
        vector<ALuint>& names, uint& index, uint allocrate)
    {
        nlassert( index <= names.size() );
        if ( index == names.size() )
        {
            // Generate new items
            uint nbalive = compactAliveNames( names, altestfunc );
            if ( nbalive == names.size() )
            {
                // Extend vector of names
                // FIXME? assumption about inner workings of vector
                allocateNewItems( algenfunc, altestfunc, names, index, allocrate );
            }
            else
            {
                // Take the room of the deleted names
                nlassert(nbalive < names.size());
                index = nbalive;
                // FIXME assumption about inner workings of vector;
                // &(names[...]) only works with "names.size() - nbalive == 1"
                generateItems(algenfunc, altestfunc, (uint)names.size() - nbalive, &(names[nbalive]));
            }
        }

        // Return the name of the item
        nlassert( index < names.size() );
        ALuint itemname = names[index];
        index++;
        return itemname;        
    }
    //--------------------------------------------------------
    uint OpenalSoundSystem::compactAliveNames( vector<ALuint>& names, TTestFunctionAL altestfunc )
    {
        vector<ALuint>::iterator iball, ibcompacted;
        for ( iball=names.begin(), ibcompacted=names.begin(); iball!=names.end(); ++iball )
        {
            // iball is incremented every iteration
            // ibcompacted is not incremented if a buffer is not valid anymore
            if ( altestfunc( *iball ) )
            {
                *ibcompacted = *iball;
                ++ibcompacted;
            }
        }
        nlassert( ibcompacted <= names.end() );
        return (uint)(ibcompacted - names.begin());
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::commit3DChanges()
    {
        // Sync up sources & listener 3d position.
        if (getOption(OptionManualRolloff))
        {
            set<CSourceAL*>::iterator it = _Sources.begin(), iend = _Sources.end();
            while(it != iend)
            {
                (*it)->updateManualRolloff();
                ++it;
            }
        }

        // update the music (XFade etc...)
        updateMusic();        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::writeProfile(String& out)
    {
        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::startBench()
    {
        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::endBench()
    {
        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::displayBench(NLMISC::CLog *log)
    {
        
    }
    //--------------------------------------------------------
    bool OpenalSoundSystem::getMusicInfo(const String &filepath, String &artist, String &title)
    {
        // add support for additional non-standard music file types info here
        return IMusicBuffer::getInfo(filepath, artist, title);        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::updateMusic()
    {
        set<CMusicChannelAL *>::iterator it(_MusicChannels.begin()), end(_MusicChannels.end());
        for (; it != end; ++it) (*it)->update();        
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::removeBuffer(CBufferAL *buffer)
    {
        nlassert(buffer != NULL);
        if (!deleteItem( buffer->bufferName(), alDeleteBuffers, _Buffers))
		nlwarning("AL: Deleting buffer: name not found");
    }
    //--------------------------------------------------------
    void OpenalSoundSystem::removeSource(CSourceAL *source)
    {
        if (_Sources.find(source) != _Sources.end()) _Sources.erase(source);
        else nlwarning("AL: removeSource already called");
    }
    //-------------------------------------------------------- 
    void OpenalSoundSystem::remove(CEffectAL *effect)
    {
        if (_Effects.find(effect) != _Effects.end()) _Effects.erase(effect);
        else nlwarning("AL: remove already called");
    }
    //-------------------------------------------------------- 
    SoundChannel * OpenalSoundSystem::createMusicChannel()
    {
        CMusicChannelAL *music_channel = new CMusicChannelAL(this);
        _MusicChannels.insert(music_channel);
        return static_cast<IMusicChannel *>(music_channel);
    }
    //-------------------------------------------------------- 
    void OpenalSoundSystem::removeMusicChannel(SoundChannel *musicChannel)
    {
        if (_MusicChannels.find(musicChannel) != _MusicChannels.end()) _MusicChannels.erase(musicChannel);
        else nlwarning("AL: removeMusicChannel already called");
    }
    //-------------------------------------------------------- 
    void OpenalSoundSystem::setGain( float gain )
    {
    	clamp(gain, 0.f, 1.f);
        _MasterGain= gain;
        // TODO: update all sources in not using manual rollof ?
    }
    //-------------------------------------------------------- 
    NIIf OpenalSoundSystem::getGain()
    {
        return _MasterGain;
    }
    //-------------------------------------------------------- 
    bool OpenalSoundSystem::deleteItem( ALuint name, TDeleteFunctionAL aldeletefunc, vector<ALuint>& names )
    {
        vector<ALuint>::iterator ibn = find( names.begin(), names.end(), name );
        if ( ibn == names.end() )
        {
            return false;
        }
        aldeletefunc( 1, &*ibn );
        *ibn = AL_NONE;
        alTestError();
        return true;        
    }
    //-------------------------------------------------------- 
    SoundListen * OpenalSoundSystem::createListener()
    {
    	nlassert(!CListenerAL::isInitialized());
        return new CListenerAL();
    }
    //-------------------------------------------------------- 
    void OpenalSoundSystem::applyRolloffFactor(float f)
    {
        _RolloffFactor = f;
        if(!getOption(OptionManualRolloff))
        {
            set<CSourceAL *>::iterator it(_Sources.begin()), end(_Sources.end());
            for(; it != end; ++it) 
                alSourcef((*it)->getSource(), AL_ROLLOFF_FACTOR, _RolloffFactor);
            alTestError();
        }        
    }
    //-------------------------------------------------------- 
    SampleFormat OpenalSoundSystem::ALtoNLSoundFormat(ALenum alformat)
    {
        switch ( alformat )
        {
        case AL_FORMAT_MONO8 : return Mono8;
        case AL_FORMAT_MONO16 : return Mono16;
        case AL_FORMAT_STEREO8 : return Stereo8;
        case AL_FORMAT_STEREO16 : return Stereo16;
        default : nlstop; return Mono8;
        }        
    }
 
}