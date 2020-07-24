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
    /** 控制单元管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ControlItemManager : public Singleton<ControlItemManager>, public EventAlloc
    {
    public:
        ControlItemManager();
        ~ControlItemManager();

        /// 获取虚拟鼠标控制单元
        NII_MEDIA::MouseControlItem * getDummyMouse() const{ return mMouse; }

        /// 获取虚拟键盘控制单元
        NII_MEDIA::KeyboardControlItem * getDummyKeyboard(){ return mKeyboard; }

        /// 获取虚拟手柄控制单元
        NII_MEDIA::JoyPadControlItem * getDummyJoyPad(){ return mJoyPad; }

        /// 获取虚拟摇杆控制单元
        NII_MEDIA::JoyStickControlItem * getDummyJoyStick(){ return mJoyStick; }

        /// 获取虚拟手把控制单元
        NII_MEDIA::JoyHandleControlItem * getDummyJoyHandle(){ return mJoyHandle; }

        /// 获取虚拟手盘控制单元
        NII_MEDIA::JoyWheelControlItem * getDummyJoyWheel(){ return mJoyWheel; }

        /// 获取虚拟触屏控制单元
        NII_MEDIA::TouchControlItem * getDummyTouch() { return mTouch; }
    protected:
        NII_MEDIA::MouseControlItem * mMouse;                    ///< 虚拟鼠标
        NII_MEDIA::KeyboardControlItem * mKeyboard;                ///< 虚拟键盘
        NII_MEDIA::JoyPadControlItem * mJoyPad;                    ///< 虚拟手柄
        NII_MEDIA::JoyStickControlItem * mJoyStick;                ///< 虚拟摇杆
        NII_MEDIA::JoyHandleControlItem * mJoyHandle;            ///<
        NII_MEDIA::JoyWheelControlItem * mJoyWheel;                ///<
        NII_MEDIA::TouchControlItem * mTouch;                    ///<
    };
}
#endif