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

#ifndef _NII_WIN32_TOUCH_CONTROLPATTERN_H_
#define _NII_WIN32_TOUCH_CONTROLPATTERN_H_

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiTouchControlPattern.h"

namespace NII
{
    /** ����һ��ר��ΪNII�����װ��Win32��������
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class _EngineAPI Win32TouchControlPattern : public TouchControlPattern
    {
    public:
        Win32TouchControlPattern(Nid id);

        ~Win32TouchControlPattern();

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const { return 255; }

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const { return 255; }
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
        PatternID getPID() const { return 0; }
    private:

    };
}
#endif