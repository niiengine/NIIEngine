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

#ifndef NII_SOUND_H
#define NII_SOUND_H

#include "NiiPreInclude.h"
#include "NiiSoundUnit.h"

#define SoundContextNbArgs 10

namespace NII
{
    /** 音频资源
    @version NIIEngine 4.0.0
     */
    class _EngineAPI Sound : public Resource, public ExtData
    {
        friend class SoundSys;
    public:
        enum Type
        {
            T_Unkonw,
            T_Bg,
            T_Music,
            T_Fusion
        };

        Sound(ScriptTypeID stid, ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        virtual ~Sound();

        bool operator<(const Sound & o) const;
        
        bool operator == (const Sound & o) const;

        /**
        @version NIIEngine 4.0.0
        */
        inline Type getSoundType() const { return mType; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setLoop(bool b) { mLoop = b; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool getLoop() const { return mLoop; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setGain(NIIf g) const { mGain = g; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getGain() const { return mGain; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setPitch(NIIf p) { mPitch = p; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getPitch() const { return mPitch; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setPriority(PriorityLevel pl) { mPriority = pl; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline PriorityLevel getPriority() const { return mPriority; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool isConeEnable() const { return mConeEnable; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setInnerAngle(NIIf a) { mInnerAngle = a; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getInnerAngle() const { return mInnerAngle; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline void setOuterAngle(NIIf a) { mOuterAngle = a; }

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getOuterAngle() const { return mOuterAngle; }

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getOuterGain() const { return mOuterGain; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getMinDistance() const { return mMinDistance; }
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getMaxDistance() const{ return mMaxDistance; }

        /**
        @version NIIEngine 4.0.0
        */
        const Vector3f & getDirection()const{ return mDirection; }

        /**
        @version NIIEngine 4.0.0
        */
        inline bool isFadeInEnable() const { return mFadeInEnable; }

        /**
        @version NIIEngine 4.0.0
        */
        inline bool isFadeOutEnable() const { return mFadeOutEnable; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline TimeDurMS getFadeInTime() const {return mFadeInTime; }

        /**
        @version NIIEngine 4.0.0
        */
        inline TimeDurMS getFadeOutTime() const {return mFadeOutTime; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual TimeDurMS getDuration() const;

        /**
        @version NIIEngine 4.0.0
        */
        virtual Sound * getSub(Nindex idx) const = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NCount getSubCount() const;

        /// @copydetails Resource::read
        virtual void read(Serializer * out) const;
        
        /// @copydetails Resource::write
        virtual void write(const Serializer * in);
    protected:
        /// @copydetails Resource::loadImpl
        virtual void loadImpl();
    protected:
        Type mType;
        PriorityLevel mPriority;
        Vector3f mDirection;
        NIIf mInnerAngle;
        NIIf mOuterAngle;
        NIIf mOuterGain;
        NIIf mGain;             ///< [0,1]
        NIIf mPitch;            ///< ]0,1]
        NIIf mMinDistance;
        NIIf mMaxDistance;
        TimeDurMS mFadeInTime;
        TimeDurMS mFadeOutTime;
        TimeDurMS mDuration;
        bool mFadeInEnable;
        bool mFadeOutEnable;
        bool mConeEnable;
        bool mLoop;
    };
    typedef vector<Sound *>::type SoundList;

    /**
    @version NIIEngine 4.0.0
    */
    enum PriorityLevel
    {
        HighestPri          = 0,
        HighPri             = 1,
        MidPri              = 2,
        LowPri              = 3,
        NbSoundPriorities   = 4
    };

    /**
    @version NIIEngine 4.0.0
    */
    class _EngineAPI SoundUnit
    {
        friend class SoundSys;
    public:
        /**
        @version NIIEngine 4.0.0
        */
        enum Type
        {
            T_Unkonw,
            T_Bg,
            T_Fusion,
            T_Music
        };
        
        /**
        @version NIIEngine 4.0.0
        */
        class Listener
        {
        public:
            virtual ~Listener(){}
            
            /**
            @version NIIEngine 4.0.0
            */
            void onCreate(SoundUnit * unit){}
            
            /**
            @version NIIEngine 4.0.0
            */
            void onDestroy(SoundUnit * unit){}
        };
    public:
        SoundUnit(Sound * sound, SpaceNode * node);
        virtual ~SoundUnit();

        /** 获取使用的声音
        @version NIIEngine 4.0.0
        */
        virtual Sound * getSound() = 0;

        /** 获取声音类型
        @version NIIEngine 4.0.0
        */
        virtual Type getType() const = 0;

        /** 播放声音
        @version NIIEngine 4.0.0
        */
        virtual void play();

        /** 停止播放
        @version NIIEngine 4.0.0
        */
        virtual void stop();

        /** 是否处于播放中
        @version NIIEngine 4.0.0
        */
        virtual bool isPlaying() { return mPlay; }

        /**
        @version NIIEngine 4.0.0
        */
        inline void setListener(Listener * obj) { mListener = obj; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline Listener * getListener() const { return mListener; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline void setPriority(PriorityLevel pr) {  mPriority = pr; }

        /**
        @version NIIEngine 4.0.0
        */
        inline PriorityLevel getPriority() const { return mPriority; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setLoop(bool loop);

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool getLoop() const;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setPos(const Vector3f & pos);

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setPosRef(const Vector3f * pos);

        /**
        @version NIIEngine 4.0.0
        */
        virtual const Vector3f & getPos() const;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setVelocity(const Vector3f & vel);

        /**
        @version NIIEngine 4.0.0
        */
        inline const Vector3f & getVelocity() const { return mVelocity; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setDirection(const Vector3f & dir);

        /**
        @version NIIEngine 4.0.0
        */
        inline const Vector3f & getDirection() const { return mDirection; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setPitch(NIIf pitch);

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getPitch() const{ return mPitch; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setGain(NIIf gain);

        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getGain() const{ return mGain; }

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRelativeMode(bool mode);

        /**
        @version NIIEngine 4.0.0
        */
        virtual bool getRelativeMode() const { return mRelMode; }
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRelativeGain(NIIf gain);

        /**
        @version NIIEngine 4.0.0
        */
        inline NIIf getRelativeGain() const { return mSrcGain == 0.0f ? 0.0f : mGain / mSrcGain; }

        /**
        @version NIIEngine 4.0.0
        */
        inline SpaceNode * getNode() const { return mNode; }
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual TimeDurMS getPlayTime();
    protected:
        SoundUnit();
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void onUpdate();
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void onTime();
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual void onSwap();
    protected:
        Vector3f mPosition;
        Vector3f mVelocity;
        Vector3f mDirection;
        Vector3f * mRefPosition;
        PriorityLevel mPriority;
        NIIf mGain;
        NIIf mPitch;
        NIIf mSrcGain;
        TimeDurMS mLastPlay;
        Listener * mListener;
        SpaceNode * mNode;
        const ExtData * mExtData;

        bool mPlay;
        bool mLoop;
        bool mRelMode;
    };
    typedef vector<SoundUnit *>::type SoundUnitList;
}
#endif