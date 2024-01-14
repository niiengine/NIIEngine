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
    /** win32平台的键盘控制器
    @remark 平台专用键盘控制器
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class Win32KeyboardControlPattern : public KeyboardControlPattern
    {
        /// 管理器类概念一般都有友类性质
        friend class Win32ControlPatternManager;
    public:
        /** 通过管理器类可以调用这个方法
        @param[out] input 主控制设备
        @param[out] buff 是否使用缓存模式
        */
        Win32KeyboardControlPattern(Nid id, IDirectInput8 * pDI, DWORD coopSet);

        /// 这个方法把这个实例从输入管理器中移去
        ~Win32KeyboardControlPattern();

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
        Nui32 getTextModeText(KeyCode key);
    private:
		IDirectInput8 * mDirectInput;               ///< 设备输入器
		IDirectInputDevice8 * mKeyboard;            ///< 输入设备
		DWORD mCoopSetting;                         ///< 协调设置
        WCHAR mlastKey;                             ///< 最后一个
    };
}
#endif