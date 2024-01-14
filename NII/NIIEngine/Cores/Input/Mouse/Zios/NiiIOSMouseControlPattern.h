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

#ifndef _NII_WIN32_CONTROLPATTERN_H_
#define _NII_WIN32_CONTROLPATTERN_H_

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiMouseControlPattern.h"

namespace NII
{
    /** ����һ��ר��ΪNII�����װ��Win32���̲���
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class Win32MouseControlPattern : public MouseControlPattern
    {
    public:
        /// ��ֹ����
        Win32MouseControlPattern();

        /** ͨ������������Ե����������
        @param[in] main �����ƴ���(��Ҫ���Ĵ�С)
        @param[in] input �������豸
        @param[in] buff �Ƿ�ʹ�û���ģʽ
        */
        Win32MouseControlPattern(Nid id, const ViewWindow * own,
            IDirectInput8 * pdi, bool coop);

        /// ������������ʵ�����������������ȥ
        ~Win32MouseControlPattern();
        
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
        HWND mHwin;                         ///< ������(����ӳ���ϵ)
		IDirectInput8 * mDirectInput;       ///< �豸�ӿ�
		IDirectInputDevice8 * mMouse;       ///< ʵ��
		DWORD mCoopSetting;                 ///< Э������
    };
}
#endif