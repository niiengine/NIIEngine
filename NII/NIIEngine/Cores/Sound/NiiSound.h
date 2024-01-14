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

#ifndef _NII_SOUND_H_
#define _NII_SOUND_H_

#include "NIIPreInclude.h"
#include "NIIVector3.h"

namespace NII
{
    /// 这是是音频的基本对象
    class _EngineAPI Sound : public Resoucre
    {
    public:
        enum SoundState
        {
            SS_PLAYING = 1,
            SS_STOPED = 2,
            SS_PAUSED = 3
        };
        enum SoundMode
        {
            SM_RELATIVE
        };
    public:
        Sound(ResourceID rid, GroupID gid, ResLoadScheme * ls = 0,
            ResResultScheme * rs = 0);

        /** 从一个音频中加载
        @param[in] audio
        */
        virtual void load(const Audio & audio);

        /** 设置这个音频的音量增量
        @remark 这个是增量,实际声音为音频文件中的正常音量加
        @param[in] volume 默认为 1.0
        */
        void setGain(NIIf volume = 1.0f);

        /** 获取这个音频的音量增量
        @remark 这个是增量,实际声音为音频文件中的正常音量加
        @return 一个浮点形增量
        */
        NIIf getGain() const;

        /** 设置这个音频的音量播放速率
        @remark 仅仅是3D模式中使用,立体声模式忽略
        @param[in] vel 一个三维向量
        @param[in] deferred 是否延时,默认为true
        */
        void setVel(const Vector3f & vel = Vector3f::ZERO, bool deferred = true);

        /** 获取这个音频的音量播放速率
        @remark 仅仅是3D模式中使用,立体声模式忽略
        */
        const Vector3f & getVel() const;

        /** 设置这个音频开始衰弱的最最小距离距离(单位).也影响到速率的衰弱
        @remark 仅仅在3D模式中使用
        @param[in] dist 声音最小有效距离
        */
        void setMin(NIIf dist, bool deferred = true);

        /** 获取这个音频开始衰弱的最最小距离距离(单位).也影响到速率的衰弱
        @remark 仅仅在3D模式中使用
        */
        NIIf getMin()const;

        /** 设置这个音频开始衰弱的最最大距离距离(单位).也影响到速率的衰弱
        @remark 仅仅在3D模式中使用
        @param[in] dist 声音最大有效距离
        */
        void setMax(NIIf dist, bool deferred = true);

        /** 获取这个音频开始衰弱的最最大距离距离(单位).也影响到速率的衰弱
        @remark 仅仅在3D模式中使用
        */
        NIIf getMax() const;

        /** 设置这个音频左右平衡度
        @param[in] bal -1.0 左顷 - 0.0 平衡 - 1.0 右顷 - 默认 0.0.
        */
        void setBal(NIIf bal = 0.5f);

        /** 获取这个音频左右平衡度
        @return 返回这个音频的平衡度
        */
        NIIf getBal() const;

        /** 设置这个音频播放的长度
        @remark 播放的时长,单位:秒
        */
        void setLength(NIId len =0);

        /** 获取这个音频播放的长度
        @return 播放的时长,单位:秒
        */
        NIId getLength() const;

        /** 设置这个声音的位置
        @param[in] pos 要设置的位置
        */
        void setPos(const Vector3f & pos);

        /// 获取这个声音的位置
        const Vector3f & getPos();

        /** 设置这个声音的方向
        @param[in] dir 要设置的方向
        */
        void setDir(const Vector3f & dir);

        /// 获取这个声音的方向
        const Vector3f & getDir() const;

        /** 设置这个声音的有效角 单位 radian
        @remark 默认为(2PI, 2PI, 0)
        @param[in] innerAngle为内角
        @param[in] outerAngle为外角
        @param[in] outerGain在[0,1]范围内
        */
        void setCone(NIIf innerAngle, NIIf outerAngle, NIIf outerGain);

        /// 获取发生体内角
        NIIf getInnerCone() const;

        /// 获取发生体外角
        NIIf getOuterCone() const;

        /// 获取发生体外角增幅
        NIIf getConeGain() const;

        /** 设置频率调动 一般为1.0,每减少50%等于一个八度音调切换
        @version NIIEngine 3.0.0
        */
        void setPitch(NIIf pitch = 1.0f);

        /** 设置频率调动 一般为1.0,每减少50%等于一个八度音调切换
        @version NIIEngine 3.0.0
        */
        NIIf getPitch() const;

        /// 获取播放到现在的持续时间(豪秒)
        Nul getTime() const;

        /// 设置播放模式(立体声)
        void setMode(SoundMode mode);

        /// 获取播放模式(立体声)
        SoundMode getMode() const;

        /** 设置Fade为音量距离曲线.
        @remark 仅在OptionManualRolloff中才使用 .
        @param[in] f 范围-1.0到1.0,默认为0.0
        @brief 设置音量距离曲线的alpha 值
        */
        void setFade(NIIf f = 0.0);

        /** 获取这个声音需要的缓存
        @param[in] region
        */
        virtual HardwareVoiceBufferSharedPtr getBuffer(AudioRaw::Dur region) = 0 ;

        /** 把当前的声音缓存转换成一个音频
        @param[in] src 音频对象,声音的来源
        */
        virtual void convert(Audio & src);

        /// 添加格外的辅助数据源,子类必须实现它
        virtual void add(VoiceBuffer * partner) = 0;

        NCount getCount() const = 0;
    protected:
        /// 建立内部的设置,子类必须实现
        virtual void setupImpl() = 0;

        /// 播放具体具体实现,子类必须实现
        virtual void playImpl() = 0;

        /// 停止播放具体实现,子类必须实现
        virtual void stopImpl() = 0;

        /// 暂停播放的具体实现,子类必须实现
        virtual void pauseImpl() = 0;

        /// 绑定数据源的具体实现,子类必须实现它
        virtual void bindImpl() = 0;

        /// 添加格外的辅助数据源,子类必须实现它
        virtual void addPartner(VoiceBuffer * partner) = 0;

        /** cpu计算衰减
        @remark 如果硬件支持计算衰减,尽量使用硬件支持
        @param[in] vol 当前音量
        @param[in] min 最小音量
        @param[in] max 最大音量
        @param[in] fade 消退系数
        @param[in] dist 当前距离(次方)
        @param[in] dmix 最小距离
        @param[in] dmax 最大距离
        */
        static Ni32 computeRollOff(Ni32 vol, Ni32 min, Ni32 max, NIIf fade, NIIf dist,
            NIIf dmin, NIIf dmax);

        /** cpu计算衰减
        @remark 如果硬件支持计算衰减,尽量使用硬件支持
        @param[in] fade 消退系数
        @param[in] dist 当前距离(次方)
        @param[in] dmin 最小距离
        @param[in] dmax 最大距离
        */
        static NIIf computeRolloff(NIIf fade, NIIf dist, NIIf dmin, NIIf dmax);
    protected:
        NIIf mGain;                     ///< 音量
        NIIf mBalance;                  ///< 平衡度
        Vector3f mVelocity;             ///< 播放速率
        NIIf mMinDist;                  ///< 最小范围
        NIIf mMaxDist;                  ///< 最大范围
        NIId mLength;                   ///< 播放的长度
        NIIf mFade;                     ///< 消退系数
        NIIf mPitch;                    ///< 频率的调动
        NIIf mInner;                    ///< 内有效角
        NIIf mOuter;                    ///< 外有效角
        NIIf mOutGain;                  ///< 内外交的过度参数
        bool mDeferred;                 ///< 是否延迟
        bool mStereo;                   ///< 是否立体声
        VoiceBuffer * mBuffer;          ///< 当前绑定的数据源(主对象)
        Vector3f mPos;                  ///< 这个声音的位置
        Vector3f mDir;                  ///< 这个声音的方向
        Nul mTime;                      ///< 声音持续时间
    };
}
#endif