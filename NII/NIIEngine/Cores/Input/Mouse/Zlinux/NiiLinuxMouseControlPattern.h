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

#ifndef _NII_LINUX_CONTROLPATTERN_H_
#define _NII_LINUX_CONTROLPATTERN_H_

#include "NiiLinuxControlPatternPrereq.h"
#include "NiiMouseControlPattern.h"

namespace NII
{
    /** 这是一个专门为NII引擎包装的linux键盘部分
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class LinuxMouseControlPattern : public MouseControlPattern
    {
    public:
        /// 防止滥用
        LinuxMouseControlPattern();

        /** 通过管理器类可以调用这个方法
        @param[in] id
        @param[in] main
        */
        LinuxMouseControlPattern(Nid id, NIIi main);

        /// 这个方法把这个实例从输入管理器中移去
        ~LinuxMouseControlPattern();
        
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
        NIIi mMouse;                            ///< 主控
    };
}
#endif