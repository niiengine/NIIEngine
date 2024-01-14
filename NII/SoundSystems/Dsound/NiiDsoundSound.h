#ifndef NII_DSOUND_AUDIO_H
#define NII_DSOUND_AUDIO_H
#include ""
namespace NII
{
    /** ms中的dsound 接口
    */
    class DsoundSound : public Sound
    {
    public:
        DsoundSound();
        ~DsoundSound(){}
    public:
        uint sourceName() { return _SourceName; }
        
        /// Initialize the DirectSound buffers. Called by the sound driver only.
        void init(LPDIRECTSOUND directSound, bool useEax);
        
        /// Returns the buffer associated with this source.
        VoiceBuffer *getBuffer();
        
        /// Reset the source before reuse
        void reset();

        /// Update the source (e.g. continue to stream the data in)
        bool update();
        
        /// Update the source's volume according to its distance and fade out curve.
        /// It takes the current position of the listener as argument.
        void updateVolume(const Vector3f & listener);
        ///
        void setNorMode(bool mode);
        
        /// 接口来源 -> Sound
        Nui32 getTime();
        /// 接口来源 -> Sound
        bool isPlaying() const; 
        /// 接口来源 -> Sound
        bool isStoped() const;
        /// 接口来源 -> Sound
        bool isPaused() const;
        /// 接口来源 -> Sound
        bool needUpdate() const;
        ///声音播放状态的枚举
        enum SourceState{
            SS_stopped = Sound::SS_STOPED,
            SS_playing = Sound::SS_PLAYING,
            SS_silencing,
            SS_swap_pending
        };
    private:
        ///更新
        void updatePos();
        ///更新
        void updateVel();
        ///更新
        void updateDir();
        ///更新
        void updateGen();
        ///更新
        void updatePit();
        ///更新
        void updateMMD();
        ///更新
        void updateCon();    
        
        void copySampleTo16BitsTrack(void *dst, void *src, uint nbSample, TSampleFormat sourceFormat);
        
        /// Release all DirectSound resources
        void release();
        
        /// The minimum size of available space in the DS buffer for update
        static const uint32 mUpdateCopySize;
        /// The size of the samples that are copied when buffers are swapped
        static const uint32 _SwapCopySize;
        /// The number of channels
        static const uint   _DefaultChannels;
        /// The default sample rate
        static const uint   _DefaultSampleRate;
        /// The default sample size
        static const uint   _DefaultSampleSize;
        /// The length of the crossfade, in samples
        static const uint32 _XFadeSize;
        
        /// The play and write cursors
        struct Cursors
        {
            uint32  PlayCursor;
            uint32  WriteCursor;
            uint32  WriteSize;
        };
        /// A locked buffer info.
        struct LockedBufferInfo
        {
            /// First locked part.
            sint16  *Ptr1;
            uint32  Size1;

            /// second locked part (or 0 if none)
            sint16  *Ptr2;
            uint32  Size2;
        };
        
        /// Utility function that locks the DirectSound buffer and restores it if it was lost.
        bool lock(uint32 writePos, uint32 size, LockedBufferInfo &lockedInfo);
	
        /// Utility function that unlocks the DirectSound buffer

        bool unlock(const LockedBufferInfo &lockedInfo);
        
        void etCursors(Cursors &cursors);
        uint32 checkFillCursor();
        
        
        void fillData(const LockedBufferInfo &lbi, int nbSample);
        void fillData(sint16 *dst, uint nbSample);
        void fillSilence(const LockedBufferInfo &lbi, int nbSample);
        
        void xfade(const LockedBufferInfo &lbi, sint16 *src);
        void fadeOut(const LockedBufferInfo &lbi);
        void fadeIn(const LockedBufferInfo &lbi);
        
        void advanceFill(LockedBufferInfo &lbi, uint nbSample);
        
        /// Replace the current buffer with the swap buffer
        void swap();
        
        /// Fill the buffer with fresh samples. Should be called inside the critical zone.
        bool fill();
        /// Fill the buffer with sparkling silence. Should be called inside the critical zone.
        bool silence();
        /** Do a cross fade between the current buffer and the buffer stored in the _SwapBuffer
            variable. Call inside the critical zone.
        */
        void crossFade();
        /// Fade out the current buffer. Call inside the critical zone.
        void fadeOut();
        /// Fade in the current buffer. Call inside the critical zone.
        void adeIn();   
    private:
        /// Source name
        uint mName;

        SourceState mState;
	
        VoiceBuffer * mMainSample;
        /// Size of the buffer in sample
        uint mSampleSize;
        /// Position in the buffer in sample.
        uint mSampleOffset;
        /// The number of sample realy played (depend on play cursor).
        uint32 mPlayedOffset;
        ///
        SampleFormat mTypeFormat;
        ///
        uint mSampleFreq;
	
        /// The sample rate of the source (= _Freq * _Buffer sample rate)
        uint32 mSampleRate;
	
	
        VoiceBuffer *mNextSample;
        
        uint32 mLastPlayPos;
        uint32 mFillOffset;
        uint32 mSilenceWriten;
	

        /// DirectSound 第二缓存，主缓存是自动创建的
        LPDIRECTSOUNDBUFFER mSecondaryBuffer;
        /// The byte size of the DirectSound secondary buffers
        static const uint32 mSecondaryBufferSize;
        /// The mask for the buffer size
        static const uint32 _SizeMask;
        /// 3D interface of the secondary buffer.
        LPDIRECTSOUND3DBUFFER m3DBuffer;
        /// The critial section object to protect the swap and update functions
        CRITICAL_SECTION mCriticalSection;
	
        /// The state for ADPCM decompression.
        VoiceBuffer::ADPCMState	mADPCMState;

#if EAX_AVAILABLE == 1
        LPKSPROPERTYSET mEAXSource;
#endif        
    };
}
#endif