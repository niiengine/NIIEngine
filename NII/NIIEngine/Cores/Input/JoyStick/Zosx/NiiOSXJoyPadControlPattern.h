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

#infdef _NII_OSX_JOYPAD_CONTROL_PATTERN_H_
#define _NII_OSX_JOYPAD_CONTROL_PATTERN_H_

#include "NiiOSXControlPatternPrereq.h"
#include "NiiJoyPadControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class OSXJoyPadControlPattern : public JoyPadControlPattern
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
    public:
        /** ͨ������������Ե����������
        @param[in] input �������豸
        @param[in] buff �Ƿ�ʹ�û���ģʽ
        */
        OSXJoyPadControlPattern(Nid id, IOHIDDeviceInterface ** interface);

        ~OSXJoyPadControlPattern();

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
        /// ö�ٳɷ�
        void enumCom();

        /// ����һ���¼�����
		IOHIDQueueInterface ** create(Nui32 depth = 8);
    private:
		AxisCookies mAxisCookies;
		ButtonCookies mButtonCookies;
		IOHIDQueueInterface ** mQueue;
		IOHIDDeviceInterface ** mInterface;
    };
}
}