#ifndef NII_OPENALSOUNDSYSTEM_H_
#define NII_OPENALSOUNDSYSTEM_H_
namespace NII
{

#if !FINAL_VERSION
void alTestWarning(const char *src);
#else
#define alTestWarning(__src)
#endif

#ifdef NL_DEBUG
void alTestError();
#else
#define alTestError() alTestWarning("alTestError")
#endif

    class OpenalSoundSystem
    {
    public:
        /// Constructor
        OpenalSoundSystem();
        /// Destructor
        virtual ~OpenalSoundSystem();

        /// Return a list of available devices for the user. The value at index 0 is empty, and is used for automatic device selection.
        virtual void getDevices(vector<String> & devices);
        /// Initialize the driver with a user selected device. If device.empty(), the default or most appropriate device is used.
        virtual void initDevice(const String & device, SoundOptions options);

        /// Return options that are enabled (including those that cannot be disabled on this driver).
        virtual SoundOptions getOptions();
        /// Return if an option is enabled (including those that cannot be disabled on this driver).
        virtual bool getOption(SoundOptions option);

        /// Commit all the changes made to 3D settings of listener and sources
        virtual void commit3DChanges();

        /// Create the listener instance
        virtual	SoundListener *createListener();
        /// Create a source, destroy with delete
        virtual	Sound *createSource();
        /// Create a sound buffer, destroy with delete
        virtual	VoiceBuffer *createBuffer();
        /// Create a reverb effect
        virtual SoundEffect *createReverbEffect();
        /// Return the maximum number of sources that can created
        virtual uint countMaxSources();
        /// Return the maximum number of effects that can be created
        virtual uint countMaxEffects();

        /// Write information about the driver to the output stream.
        virtual void writeProfile(String& /* out */);

        virtual void startBench();
        virtual void endBench();
        virtual void displayBench(NLMISC::CLog * /* log */);

        /// Create a music channel, destroy with destroyMusicChannel.
        virtual SoundChannel *createMusicChannel();

        /** Get music info. Returns false if the song is not found or the function is not implemented.
         *  \param filepath path to file, CPath::lookup done by driver
         *  \param artist returns the song artist (empty if not available)
         *  \param title returns the title (empty if not available)
         */
        virtual bool getMusicInfo(const String &filepath, String &artist, String &title);

        /// Get audio/container extensions that are supported natively by the driver implementation.
        virtual void getMusicExtensions(vector<String> & /* extensions */) const { }
        /// Return if a music extension is supported by the driver's music channel.
        virtual bool isMusicExtensionSupported(const String & /* extension */) const { return false; }

        ALCdevice *getAlDevice() { return _AlDevice; }
        ALCcontext *getAlContext() { return _AlContext; }
        float getRolloffFactor() { return _RolloffFactor; }

        /// Change the rolloff factor and apply to all sources
        void applyRolloffFactor(float f);

        /// Remove a buffer
        void removeBuffer(CBufferAL *buffer);
        /// Remove a source
        void removeSource(CSourceAL *source);
        /// Remove an effect
        void remove(CEffectAL *effect);
        /// (Internal) Remove music channel (should be called by the destructor of the music channel class).
        void removeMusicChannel(CMusicChannelAL *musicChannel);	

        /** Set the gain (volume value inside [0 , 1]). (default: 1)
         * 0.0 -> silence
         * 0.5 -> -6dB
         * 1.0 -> no attenuation
         * values > 1 (amplification) not supported by most drivers
         */
        void setGain( float gain );

        /// Get the gain
        float getGain();
    protected:
        void updateMusic();

        /// Allocate nb new buffers or sources
        void    allocateNewItems( TGenFunctionAL algenfunc, TTestFunctionAL altestfunc,
                                                  vector<ALuint>& names, uint index, uint nb );
        
        /// Generate nb buffers
        void    generateItems( TGenFunctionAL algenfunc, TTestFunctionAL altestfunc, uint nb, ALuint *array );

        /// Remove names of deleted items and return the number of valid items
        uint    compactAliveNames( vector<ALuint>& names, TTestFunctionAL testfunc );

        /// Create a sound buffer or a sound source
        ALuint  createItem( TGenFunctionAL algenfunc, TTestFunctionAL altestfunc,
                                            vector<ALuint>& names, uint& index, uint allocrate );

        /// Delete a buffer or a source
        bool    deleteItem( ALuint name, TDeleteFunctionAL aldeletefunc, vector<ALuint>& names );

        /// Master Volume [0,1]
        float _MasterGain;        
    private:
        // outside pointers
        /// The string mapper provided by client code.
        IStringMapperProvider *_StringMapper;

        ALCdevice *_AlDevice;
        ALCcontext *_AlContext;

        vector<ALuint> _Buffers;
        set<CSourceAL *> _Sources;
        set<CEffectAL *> _Effects;
        /// Array with the allocated music channels created by client code.
        set<CMusicChannelAL *> _MusicChannels;
        // Number of exported buffers (including any deleted buffers)
        uint _NbExpBuffers;
        // Number of exported sources (including any deleted sources)
        uint _NbExpSources;

        float _RolloffFactor; // ***todo*** move
        /// Driver options
        SoundOptions _Options;        
    }
}
#endif