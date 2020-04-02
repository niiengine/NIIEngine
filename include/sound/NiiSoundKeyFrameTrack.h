/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-1

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

#ifndef _NII_SOUND_KEY_FRAME_TRACK_H_
#define _NII_SOUND_KEY_FRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** 声音专用KeyFrameTrack
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundKeyFrameTrack : public KeyFrameTrack
    {
    public:
        SoundKeyFrameTrack(const Animation * own, KeyFrameTrackID id);
        SoundKeyFrameTrack(const Animation * own, KeyFrameTrackID id, SpaceNode * target);

        virtual ~SoundKeyFrameTrack();

        /** 设置目标对象
        @param node
        @version NIIEngine 4.0.0
        */
        virtual void setTarget(SpaceNode * node);

        /** 获取目标对象
        @version NIIEngine 4.0.0
        */
        virtual SpaceNode * getTarget() const;

        /**
        @version NIIEngine 4.0.0
        */
        inline void setPos(const Vector3f & pos) { mPos = pos; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline const Vector3f & getPos() const { return mPos; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline void setRelativeGain(bool set) { mRelativeGain = set; }
        
        /**
        @version NIIEngine 4.0.0
        */
        inline bool getRelativeGain() const { return mRelativeGain; }
        
        /** 应用混合
        @version NIIEngine 4.0.0
        */
        virtual void apply(AnimationFusion * dst, SpaceNode * com, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::isValid
        virtual bool isValid() const;

        /// @copydetails KeyFrameTrack::optimise
        virtual void optimise();

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani,  NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS time);

        /// @copydetails KeyFrameTrack::notifyImpl
        void notifyImpl() const;
    protected:
        SpaceNode * mTarget;
        Vector3f mPos;
        bool mRelativeGain;
    };
}
#endif