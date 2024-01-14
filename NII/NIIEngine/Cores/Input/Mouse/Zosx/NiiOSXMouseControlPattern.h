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

#ifndef _NII_OSX_MOUSE_CONTROLPATTERN_H_
#define _NII_OSX_MOUSE_CONTROLPATTERN_H_

#include "NiiOSXControlPatternPrereq.h"
#include "NiiMouseControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ����һ��ר��ΪNII�����װ��Object Oriented Input System���̲���
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class OSXMouseControlPattern : public MouseControlPattern
    {
    public:
        /// ��ֹ����
        OSXMouseControlPattern();

        /** ͨ������������Ե����������
        @param[in] main �����ƴ���(��Ҫ���Ĵ�С)
        @param[in] input �������豸
        @param[in] buff �Ƿ�ʹ�û���ģʽ
        */
        OSXMouseControlPattern(Nid id);

        /// ������������ʵ�����������������ȥ
        ~OSXMouseControlPattern();

        /// ����ƶ��¼�����
        void onMouseMove(EventRef event);

        /// ��갴�������¼�����
        void onMousePress(EventRef event);

        /// ��갴���ͷ��¼�����
        void onMouseRelease(EventRef event);

        /// ���������¼�����
        void onMouseWheel(EventRef event);

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
        Ni32 mTempX;                                ///<
        Ni32 mTempY;                                ///<
        Ni32 mTempZ;                                ///<
        WindowRef mWin;                             ///<
        EventHandlerUPP mMouseMove;                 ///<
        EventHandlerUPP mMouseRelease;              ///<
        EventHandlerUPP mMousePress;                ///<
        EventHandlerUPP mMouseWheel;                ///<
		EventHandlerUPP mWindowFocus;               ///<
        EventHandlerRef mMouseMoveRef;              ///<
        EventHandlerRef mMousePressRef;             ///<
        EventHandlerRef mMouseReleaseRef;           ///<
        EventHandlerRef mMouseWheelRef;             ///<
		EventHandlerRef mWindowFocusEventRef;       ///<

		bool mFocusChange;                          ///<
		bool mRelationChange;                       ///<
    };
}
}
#endif