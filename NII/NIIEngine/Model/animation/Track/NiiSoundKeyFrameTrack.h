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