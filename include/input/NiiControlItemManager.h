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