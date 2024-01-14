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

#ifndef _NII_PROPERTYID_KEYFRAMETRACK_H_
#define _NII_PROPERTYID_KEYFRAMETRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** 属性值关键帧轨迹
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrameTrack : public KeyFrameTrack
    {
    public:
        /** 运算模式
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_ABS,        ///< 绝对
            M_REL,        ///< 相对
            M_MulREL    ///< 叠加型相对
        };
    public:
        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id);

        PropertyKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            PropertyID target, Mode mode = M_ABS);

        /** 设置运算模式
        @version NIIEngine 3.0.0
        */
        void setMode(Mode mode);

        /** 获取混算模式
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** 设置属性
        @note 独立轨迹使用
        @version NIIEngine 3.0.0
        */
        void setTarget(PropertyID pid);

        /** 获取属性
        @note 独立轨迹使用
        @version NIIEngine 3.0.0
        */
        PropertyID getTarget() const;

        /** 设置运算值
        @note 从对象中设置值
        @version NIIEngine 3.0.0
        */
        void setValue(PropertyFusion * obj);

        /** 应用混合
        @version NIIEngine 3.0.0
        */
        void apply(AnimationFusion * dst, PropertyID target, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::apply
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const;

        /// @copydetails KeyFrameTrack::getFusion
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * kf) const;

        /// @copydetails KeyFrameTrack::createFusion
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const;

        /// @copydetails KeyFrameTrack::clone
        KeyFrameTrack * clone(const Animation * o) const;
    protected:
        /// @copydetails KeyFrameTrack::createImpl
        KeyFrame * createImpl(TimeDurMS pos);
    protected:
        Mode mMode;
        PropertyID mTarget; /// PropertyCmd
    };
}

#endif