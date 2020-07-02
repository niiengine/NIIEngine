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
    /** �������Թ����ؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrame : public UnitKeyFrame
    {
    public:
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode,
            const FusionData & value, PropertyID pid = 0);
        ~PropertyKeyFrame();

        /** ���ö�������ֵΪ��ϵ�ֵ
        @version NIIEngine 3.0.0
        */
        void setPropertyValue(PropertyFusion * obj);

        /** ��ȡ�������Ի��ʹ�õ�ֵ
        @version NIIEngine 3.0.0
        */
        const FusionData & getPropertyValue(PropertyFusion * obj) const;

        /** ����ֵ��Դ�Ķ�������
        @version NIIEngine 3.0.0
        */
        void setProperty(PropertyID pid);

        /** ��ȡֵ��Դ�Ķ�������
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