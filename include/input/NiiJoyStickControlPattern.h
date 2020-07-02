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

#ifndef NII_JOYSTICK_CONTROLPATTERN_H
#define NII_JOYSTICK_CONTROLPATTERN_H

#include "NiiPreInclude.h"
#include "NiiJoyPadControlPattern.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��Ϸҡ���������
    @remark �����ɻ�ҡ�˸���
    @note 
        ���ڴ��ϵͳ�ں˶�ȡҡ�˼�¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI JoyStickControlPattern : public JoyPadControlPattern
    {
    public:
        typedef vector<PovType>::type Povs;
        typedef vector<Vector2<NIIi> >::type Sliders;
    protected:
        JoyStickControlPattern();

        JoyStickControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyStickControlPattern();

        /** ��������
        @remark ����������������¼�
        @param[in] slider ��������
        @param[in] x ���᷽����ֵ
        @param[in] y ���᷽����ֵ
        */
        void slip(Nui8 slider, NIIi x, NIIi y);

        /** ��������
        @remark ����������������¼�,ʹ�����һ�����õ����ݻ���
        @param[in] slider ��������
        */
        void slip(Nui8 slider);

        /** �ƶ�ҡ�˼�
        @remark �����������ҡ�˼��¼�
        @param[in] pov ҡ�˼�����
        @param[in] dir ��������
        */
        void push(Nui8 pov, PovType dir);

        /** �ƶ�ҡ�˼�
        @remark �����������ҡ�˼��¼�,ʹ�����һ�����õ����ݻ���
        @param[in] pov ҡ�˼�����
        */
        void push(Nui32 pov);

        /// ���ð�Ŧҡ������
        void setPovCount(NCount num);

        /// ��ȡ��Ŧҡñ����
        NCount getPovCount() const;

        /// ���û�������
        void setSliderCount(NCount num);

        /// ��ȡ��������
        NCount getSliderCount() const;

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        Povs mPovBuffer;                ///< ҡ�˼�����
        Sliders mSliderBuffer;          ///< ���黺��
        JoyStickControlItem * mTarget;  ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
    };

    /** ���ܿ�����
    @remark ���ڶ��������ԣ���ȫ����
    @version NIIEngine 3.0.0
    */
    class DummyJoyStickControlPattern : public JoyStickControlPattern
    {
    public:
        DummyJoyStickControlPattern() {}
        ///@copydetails ControlPattern::_init
        bool _init() { return true; }

        ///@copydetails ControlPattern::_end
        bool _end() { return true; }

        ///@copydetails ControlPattern::_update
        void _update() {}

        ///@copydetails KeyboardControlPattern::_reset
        void _reset() {}

        ///@copydetails KeyboardControlPattern::_start
        void _start() {}

        ///@copydetails KeyboardControlPattern::_pause
        void _pause() {}

        ///@copydetails KeyboardControlPattern::_stop
        void _stop() {}
    };
}
}
#endif