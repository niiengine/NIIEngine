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

#ifndef _NII_WIN32_JOYSTICK_CONTROL_PATTERN_H_
#define _NII_WIN32_JOYSTICK_CONTROL_PATTERN_H_

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiJoyStickControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class Win32JoyStickControlPattern : public JoyStickControlPattern
    {
    public:
        /** ͨ������������Ե����������
        @param[in] input �������豸
        @param[in] buff �Ƿ�ʹ�û���ģʽ
        */
        Win32JoyStickControlPattern(Nid id, IDirectInput8 * pdi, GUID guid);

        /// ������������ʵ�����������������ȥ
        ~Win32JoyStickControlPattern();

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
		/// �����豸�е�Ԫ��
        void enumCom();

        /// �ƶ�ҡ�˼�
        void push(Nui8 pov, DIDEVICEOBJECTDATA & di);

		//! Enumerate axis callback
		static BOOL CALLBACK enumDev(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID p);

        //! Enumerate Force Feedback callback
		static BOOL CALLBACK enumEff(LPCDIEFFECTINFO pdei, LPVOID p);
    private:
        DIDEVCAPS mCaps;                        ///<
        GUID mGuidInstance;                     ///<
		IDirectInput8 * mDirectInput;           ///<
		IDirectInputDevice8 * mJoyStick;        ///<
        Win32JoyDevForceFeedback * mFfeedback;  ///<
        NCount mCurrentAxis;                    ///<
    };
}
}
#endif