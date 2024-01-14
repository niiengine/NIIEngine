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

#infdef _NII_OSX_JOYSTICK_CONTROL_PATTERN_H_
#define _NII_OSX_JOYSTICK_CONTROL_PATTERN_H_

#include "NiiOSXControlPatternPrereq.h"
#include "NiiJoyStickControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class OSXJoyStickControlPattern : public JoyStickControlPattern
    {
        friend class OSXControlPatternManager;
    public:
        class Info
        {
        public:
            Info(int scale, int offset) : mScale(scale), mOffset(offset)
            {
            }

            NIIf mScale;
            NIIi mOffset;
        };

        typedef map<IOHIDElementCookie, Info>::type AxisCookies;
        typedef vector<IOHIDElementCookie>::type ButtonCookies;
        typedef vector<IOHIDElementCookie>::type HatCookies;
        typedef vector<IOHIDElementCookie>::type SliderCookies;
    public:
        /** 通过管理器类可以调用这个方法
        @param[in] input 主控制设备
        @param[in] buff 是否使用缓存模式
        */
        OSXJoyStickControlPattern(Nid id, IOHIDDeviceInterface ** interface);

        ~OSXJoyStickControlPattern();

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const;

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const;
    protected:
        /// @copydetails Pattern::_init
        bool _init();

        /// @copydetails Pattern::_update
        void _update();

        /// @copydetails Pattern::_end
        bool _end(){}

        /// @copydetials ControlPattern::_reset
        void _reset();

        /// @copydetails ControlPattern::_start
        void _start();

        /// @copydetails ControlPattern::_pause
        void _pause();

        /// @copyetails ControlPattern::_stop
        void _stop();

        /// @copydetails Pattern::getPID
        PatternID getPID() const;
    private:
        /// 枚举成分
        void enumCom();

        /// 创建一个事件队列
		IOHIDQueueInterface ** create(Nui32 depth = 8);

        /// hat
        void push(Nui8 pov, Nui32 value);
    private:
		AxisCookies mAxisCookies;
		ButtonCookies mButtonCookies;
        HatCookies mHatCookies;
        SliderCookies mSliderCookies;
		IOHIDQueueInterface ** mQueue;
		IOHIDDeviceInterface ** mInterface;
    };
}
}