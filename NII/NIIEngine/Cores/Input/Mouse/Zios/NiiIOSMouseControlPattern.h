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
    /** 这是一个专门为NII引擎包装的Win32键盘部分
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class Win32MouseControlPattern : public MouseControlPattern
    {
    public:
        /// 防止滥用
        Win32MouseControlPattern();

        /** 通过管理器类可以调用这个方法
        @param[in] main 主控制窗体(需要它的大小)
        @param[in] input 主控制设备
        @param[in] buff 是否使用缓存模式
        */
        Win32MouseControlPattern(Nid id, const ViewWindow * own,
            IDirectInput8 * pdi, bool coop);

        /// 这个方法把这个实例从输入管理器中移去
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
        HWND mHwin;                         ///< 窗体句柄(加速映射关系)
		IDirectInput8 * mDirectInput;       ///< 设备接口
		IDirectInputDevice8 * mMouse;       ///< 实例
		DWORD mCoopSetting;                 ///< 协调设置
    };
}
#endif