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

#ifndef _NII_BGSOUND_H_
#define _NII_BGSOUND_H_

#include "NiiPreInclude.h"
#include "NiiSound.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BgSound : public Sound
    {
    public:
        struct Info
        {
            String mResourceID;
            Nui32 mMark;
        };
        typedef vector<Info>::type InfoList;
    public:
        BgSound();
        ~BgSound();

        /// @copydetails Sound::getSoundType
        Type getSoundType() const { return T_Bg; }

        /// @copydetails Sound::getDuration
        virtual TimeDurMS getDuration() const;

        /// @copydetails Sound::getSub
        Sound * getSub(Nindex idx) const;
        
        /// @copydetails Sound::getSubCount
        virtual NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;
        
        /// @copydetails Resource::write
        virtual void write(const Serializer * in);

        /**
        @version NIIEngine 4.0.0
        */
        const InfoList & getSounds() const { return mSoundList; }
    protected:
        /// @copydetails Resource::loadImpl
        virtual void loadImpl();
    private:
        InfoList mSoundList;
    };
    
    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI BgSoundUnit : public SoundUnit
    {
    public:
        BgSoundUnit(BgSound * sound, SpaceNode * node = 0);
        ~BgSoundUnit();

        /// @copydetails SoundUnit::getType
        Type getType() const {return T_Bg;}
        
        /// @copydetails SoundUnit::play
        virtual void play();

        /// @copydetails SoundUnit::stop
        virtual void stop();

        /// @copydetails SoundUnit::setGain
        void setGain(NIIf gain);

        /// @copydetails SoundUnit::setRelativeGain
        void setRelativeGain(NIIf gain);

        /// @copydetails SoundUnit::setPos
        void setPos(const Vector3f & pos);

        /// @copydetails SoundUnit::setVelocity
        void setVelocity(const Vector3f & vel);

        /// @copydetails SoundUnit::setDirection
        void setDirection(const Vector3f & dir);

        /// @copydetails SoundUnit::getSound
        virtual Sound * getSound();

        /**
        @version NIIEngine 4.0.0
        */
        void setGainFilter(vector<NIIf> & value);
    private:
        /// @copydetails SoundUnit::onUpdate
        void onUpdate();
    protected:
        struct Unit
        {
            SoundUnit * mUnit;
            Nui32 mState;
            Nui32 mMark;
        };
        typedef vector<Unit>::type UnitList;
    private:
        BgSound * mSound;
        UnitList mUnitList;
    };
}
#endif