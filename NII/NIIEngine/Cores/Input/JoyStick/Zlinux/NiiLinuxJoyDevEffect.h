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

#ifndef _NII_LINUX_JOYSTICK_RAMP_EFFECT_H_
#define _NII_LINUX_JOYSTICK_RAMP_EFFECT_H_

#include "NiiLinuxControlPatternPrereq.h"
#include "NiiJoyStickRampEffect.h"

namespace NII
{
namespace NII_MEDIA
{
    class JoyDevForceFeedback;

    /**
    @version NIIEngine 3.0.0
    */
    class LinuxJoyStickRampEffect : public JoyStickRampEffect
    {
    public:
        /// @copydetails JoyStickConstantEffect::apply
        void apply(JoyDevForceFeedback * dest);    
    };

    /**
    @version NIIEngine 3.0.0
    */
    class LinuxJoyStickPeriodicEffect : public JoyStickPeriodicEffect
    {
    public:
        /// @copydetails JoyStickConstantEffect::apply
        void apply(JoyDevForceFeedback * dest);
    };

    /**
    @version NIIEngine 3.0.0
    */
    class LinuxJoyStickConstantEffect : public JoyStickConstantEffect
    {
    public:
        /// @copydetails JoyStickConstantEffect::apply
        void apply(JoyDevForceFeedback * dest);
    };

    /**
    @version NIIEngine 3.0.0
    */
    class LinuxJoyStickConditionalEffect : public JoyStickConditionalEffect
    {
    public:
        LinuxJoyStickConditionalEffect();

        /// @copydetails JoyStickConstantEffect::apply
        void apply(JoyDevForceFeedback * dest);

        NIIs mCenter;
    };
}
}
#endif