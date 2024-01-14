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

#ifndef _NII_LINUX_JOYSTICK_CONTROLPATTERN_H_
#define _NII_LINUX_JOYSTICK_CONTROLPATTERN_H_

#include "NiiLinuxControlPatternPrereq.h"
#include "NiiJoyStickControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author �ӵõ�
    @mail zelda_ok@163.com
    */
    class LinuxJoyStickControlPattern : public JoyStickControlPattern
    {
    public:
        typedef vector<std::pair<NIIi, NIIi> >::type Rangs;
    public:
        /** ͨ������������Ե����������
        @param[in] id 
        @param[in] main ����
        */
        LinuxJoyStickControlPattern(Nid id, NIIi main);

        ~LinuxJoyStickControlPattern();

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
        void enumCom();
        
        void enumEff();
    private:
        NIIi mJoyStick;                     ///< ����
        LinuxJoyDevForceFeedback * mEff;    ///< Ч��
        Nui32 mButtonOffset;                ///< ƫ��
        Nui32 mAxisOffset;                  ///< ƫ��
        Rangs mRanges;                      ///< ��Χ
    };
}
}
#endif