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

#ifndef _NII_PROPERTY_ID_KEYFRAME_H_
#define _NII_PROPERTY_ID_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiUnitKeyFrame.h"

namespace NII
{
    /** 对象属性关联关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrame : public UnitKeyFrame
    {
    public:
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode,
            const FusionData & value, PropertyID pid = 0);
        ~PropertyKeyFrame();

        /** 设置对象属性值为混合的值
        @version NIIEngine 3.0.0
        */
        void setPropertyValue(PropertyFusion * obj);

        /** 获取对象属性混合使用的值
        @version NIIEngine 3.0.0
        */
        const FusionData & getPropertyValue(PropertyFusion * obj) const;

        /** 设置值来源的对象属性
        @version NIIEngine 3.0.0
        */
        void setProperty(PropertyID pid);

        /** 获取值来源的对象属性
        @version NIIEngine 3.0.0
        */
        PropertyID getProperty() const;
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        PropertyID mProperty;
    };
}
#endif