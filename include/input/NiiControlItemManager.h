/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_CONTROLITEM_MANAGER_H_
#define _NII_CONTROLITEM_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"

namespace NII
{
    /** ���Ƶ�Ԫ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlItemManager : public Singleton<ControlItemManager>, public EventAlloc
    {
    public:
        ControlItemManager();
        ~ControlItemManager();

        /// ��ȡ���������Ƶ�Ԫ
        NII_MEDIA::MouseControlItem * getDummyMouse() const{ return mMouse; }

        /// ��ȡ������̿��Ƶ�Ԫ
        NII_MEDIA::KeyboardControlItem * getDummyKeyboard(){ return mKeyboard; }

        /// ��ȡ�����ֱ����Ƶ�Ԫ
        NII_MEDIA::JoyPadControlItem * getDummyJoyPad(){ return mJoyPad; }

        /// ��ȡ����ҡ�˿��Ƶ�Ԫ
        NII_MEDIA::JoyStickControlItem * getDummyJoyStick(){ return mJoyStick; }

        /// ��ȡ�����ְѿ��Ƶ�Ԫ
        NII_MEDIA::JoyHandleControlItem * getDummyJoyHandle(){ return mJoyHandle; }

        /// ��ȡ�������̿��Ƶ�Ԫ
        NII_MEDIA::JoyWheelControlItem * getDummyJoyWheel(){ return mJoyWheel; }

        /// ��ȡ���ⴥ�����Ƶ�Ԫ
        NII_MEDIA::TouchControlItem * getDummyTouch() { return mTouch; }

        /// @copydetails Singleton::getOnly
        static ControlItemManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static ControlItemManager * getOnlyPtr();
    protected:
        NII_MEDIA::MouseControlItem * mMouse;                    ///< �������
        NII_MEDIA::KeyboardControlItem * mKeyboard;                ///< �������
        NII_MEDIA::JoyPadControlItem * mJoyPad;                    ///< �����ֱ�
        NII_MEDIA::JoyStickControlItem * mJoyStick;                ///< ����ҡ��
        NII_MEDIA::JoyHandleControlItem * mJoyHandle;            ///<
        NII_MEDIA::JoyWheelControlItem * mJoyWheel;                ///<
        NII_MEDIA::TouchControlItem * mTouch;                    ///<
    };
}
#endif