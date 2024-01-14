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

#ifndef NII_OSX_KEYBOARD_CONTROL_PATTERN_H
#define NII_OSX_KEYBOARD_CONTROL_PATTERN_H

#include "NiiOSXControlPatternPrereq.h"
#include "NiiKeyboardControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** OSXƽ̨�ļ��̿�����
    @remark ƽ̨ר�ü��̿�����
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class OSXKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        enum OSXKeyboardEventType
        {
            MKET_KEYUP,
            MKET_KEYDOWN,
            MKET_KEYREPEAT,
        };
        class OSXKeyStackEvent
        {
        public:
            OSXKeyStackEvent(KeyValue key, OSXKeyboardEventType type) :
                mKey(key),
                mType(type) {}

            OSXKeyboardEventType mType;
            KeyValue mKey;                          ///< �����ı���
        };
        typedef list<OSXKeyStackEvent>::type Events;
    public:
        /** ͨ������������Ե����������
        @param[in] id �������豸
        @param[in] own �Ƿ�ʹ�û���ģʽ
        @param[in] repeat �����Զ��ظ�����ģʽ
        */
        OSXKeyboardControlPattern(Nid id);

        /// ������������ʵ�����������������ȥ
        ~OSXKeyboardControlPattern();

        /** ���������µ�ʱ�򴥷�
        @remark ����������״̬
        @param[in] event �ص��¼�
        */
        void onKeyDown(EventRef event);

        /** �������ͷŵ�ʱ�򴥷�
        @remark ����������״̬
        @param[in] event �ص��¼�
        */
        void onKeyUp(EventRef event);

        /** ������״̬�仯ʱ�򴥷�
        @remark ֻ��������״̬
        @param[in] event �ص��¼�
        */
        void onKeyMod(EventRef event);

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const;

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const;

        /// @copydetails KeyboardControlPattern::syn
        void syn(KeyboardAlterLockType type) const;
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
        /** ������һ���豸�ӿ�
        @remark osxר��
        */
        void setupHid();

        /** ����capslock numlock scrlock��λ��
        @remark osxר��
        */
        void findCookie();
    private:
        IOHIDDeviceInterface ** mMain;      ///< �������
        IOHIDElementCookie mCaps;           ///< capslock�Ļ���
        IOHIDElementCookie mNum;            ///< numlock�Ļ���
        IOHIDElementCookie mScr;            ///< scrlock�Ļ���
        UInt32 mPrevModMask;                ///< ǰһ��
        EventHandlerUPP mKeyDown;           ///< ���»ص�
        EventHandlerUPP mKeyUp;             ///< �ͷŻص�
        EventHandlerUPP mKeyMod;            ///< ���ֻص�
        EventHandlerRef mKeyDownEventRef;   ///< �ص�����
        EventHandlerRef mKeyUpEventRef;     ///< �ص�����
        EventHandlerRef mKeyModEventRef;    ///< �ص�����
        Events mEvents;                     ///< �¼�����
    };
}
}
#endif