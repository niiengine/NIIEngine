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

#ifndef _NII_UNIT_KEY_FRAME_H_
#define _NII_UNIT_KEY_FRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiFusionData.h"

namespace NII
{
    /** ��ֵ�͵�ר�ùؼ�֡.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrame : public KeyFrame
    {
    public:
        /** ֡����ģʽ
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Constant,     ///< ������ֵ��
            M_Point,        ///< ֡�����
            M_Square,       ///< ������ʽ����
            M_SquareRoot    ///< ������ʽ����
        };

        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode, 
            const FusionData & value);
        virtual ~UnitKeyFrame();

        /** ���ò�ֵ����ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(Mode p);

        /** ��ȡ��ֵ����ģʽ
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** ��������ؼ�֡����ֵ
        @remark ���еĹؼ�ֵ֡������һ�µ���������
        @version NIIEngine 3.0.0
        */
        void setValue(const FusionData & val);

        /** ��ȡ����ؼ�֡����ֵ
        @remark ���еĹؼ�ֵ֡������һ�µ���������
        @version NIIEngine 3.0.0
        */
        const FusionData & getValue() const;

        /** ��ȡ֡����ģʽ����
        @param[in] pos[0, 1]
        @return ����ģʽ���[0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(NIIf pos);
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Mode mMode;
        FusionData mValue;
    };
}

#endif