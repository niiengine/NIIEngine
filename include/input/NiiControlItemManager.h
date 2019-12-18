/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

        /// @copydetails Singleton::getOnly
        static ControlItemManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static ControlItemManager * getOnlyPtr();
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