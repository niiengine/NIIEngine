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
    /** android平台的键盘控制器
    @remark 平台专用键盘控制器
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class AndroidKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        /** 通过管理器类可以调用这个方法
        @param[out] input 主控制设备
        @param[out] buff 是否使用缓存模式
        */
        AndroidKeyboardControlPattern(Nid id);

        /// 这个方法把这个实例从输入管理器中移去
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