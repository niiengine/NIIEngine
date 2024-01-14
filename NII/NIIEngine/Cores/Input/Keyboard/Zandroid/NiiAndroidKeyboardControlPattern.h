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

#ifndef NII_WIN32_CONTROL_PATTERN_H
#define NII_WIN32_CONTROL_PATTERN_H

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiKeyboardControlPattern.h"

namespace NII
{
    /** androidƽ̨�ļ��̿�����
    @remark ƽ̨ר�ü��̿�����
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class AndroidKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        /** ͨ������������Ե����������
        @param[out] input �������豸
        @param[out] buff �Ƿ�ʹ�û���ģʽ
        */
        AndroidKeyboardControlPattern(Nid id);

        /// ������������ʵ�����������������ȥ
        ~AndroidKeyboardControlPattern();

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const;

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const;

        /// @copydetails KeyboardControlPattern::getKeyText
        const String & getKeyText(KeyCode key) const;
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

        /// @copydetails KeyboardControlPattern::syn
        void syn() const;
    private:
    };
}
#endif