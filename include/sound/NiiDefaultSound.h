/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef NII_DefaultSound_H
#define NII_DefaultSound_H

#include "NiiPreInclude.h"
#include "NiiSound.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI DefaultSound : public Sound
    {
    public:
        DefaultSound();
        virtual ~DefaultSound();

        /// @copydetails Sound::getSoundType
        Type getSoundType() const {return T_Unkonw; }

        /**
        @version NIIEngine 4.0.0
        */
        void setBuffer(AudioBuffer * buf);
        
        /**
        @version NIIEngine 4.0.0
        */
        AudioBuffer * getBuffer() const;

        /**
        @version NIIEngine 4.0.0
        */
        inline void setAlpha(NIIf a) { mAlpha = a; }

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getAlpha() const { return mAlpha; }

        /// @copydetails Sound::getSub
        Sound * getSub(Nindex idx) const;

        /// @copydetails Sound::getSubCount
        NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;

        /// @copydetails Resource::write
        virtual void write(const Serializer * in);
    protected:
        /// @copydetails Sound::loadImpl
        virtual void loadImpl();
    private:
        AudioBuffer	* mBuffer;
        NIIf mAlpha;
    };
    
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI DefaultSoundUnit : public SoundUnit, public SoundSys::IMixerEvent
    {
        friend class SoundSys;
    public:
        DefaultSoundUnit(DefaultSound * sound, SpaceNode * node = 0);
        virtual ~DefaultSoundUnit();
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual AudioBuffer	* getBuffer();

        /// @copydetails SoundUnit::getType
        virtual Type getType() const {return T_Unkonw;}
        
        /// @copydetails SoundUnit::setLoop
        virtual void setLoop(bool l);
        
        /// @copydetails SoundUnit::play
        virtual void play();

        /// @copydetails SoundUnit::stop
        virtual void stop();

        /// @copydetails SoundUnit::isPlaying
        virtual bool isPlaying();

        /// @copydetails SoundUnit::getPlayTime
        virtual uint32 getPlayTime();

        /// @copydetails SoundUnit::setPos
        virtual void setPos(const Vector3f & pos);

        /** 
        @version NIIEngine 4.0.0
        */
        Vector3f getLocalPos() const;

        /// @copydetails SoundUnit::setVelocity
        virtual void setVelocity(const Vector3f & vel);

        /// @copydetails SoundUnit::setDirection
        virtual void setDirection(const Vector3f & dir);

        /// @copydetails SoundUnit::setGain
        virtual void setGain(NIIf gain);

        /// @copydetails SoundUnit::setRelativeGain
        virtual void setRelativeGain(NIIf gain);

        /// @copydetails SoundUnit::setPitch
        virtual void setPitch(NIIf pitch);

        /// @copydetails SoundUnit::setRelativeMode
        virtual void setRelativeMode(bool mode);

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();
    protected:
        /// @copydetails IMixerEvent::onTime
        virtual void onTime();
    protected:
        DefaultSound * mSound;
        SoundTrack * mTrack;
        NIIf mAlpha;
        bool mStop;
    };
}
#endif