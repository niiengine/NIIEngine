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

#ifndef _NII_LINUX_JOY_STICK_FORCE_FEEDBACK_H_
#define _NII_LINUX_JOY_STICK_FORCE_FEEDBACK_H_

#include "NiiLinuxControlPatternPrereq.h"
#include "NiiJoyDevForceFeedback.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author É”µ√µØ
    @mail zelda_ok@163.com
    */
    class LinuxJoyDevForceFeedback : public JoyDevForceFeedback
    {
    public:
        typedef map<NIIi, struct ff_effect *>::type Effects;
    public:
        LinuxJoyDevForceFeedback(NIIi devID);
        ~LinuxJoyDevForceFeedback();

		void upload(struct ff_effect * ffeffect, const Effect * effect);

		void stop(NIIi id);

		void start(NIIi id);

		void unload(NIIi id);

        ///@copydetails JoyDevForceFeedback::setMasterGain
        void setMasterGain(NIIf level);

        ///@copydetails JoyDevForceFeedback::setAutoCenterMode
        void setAutoCenterMode(bool auto_on);

        ///@copydetails JoyDevForceFeedback::upload
        void upload(const JoyStickEffect * eff);

        ///@copydetails JoyDevForceFeedback::modify
        void modify(const JoyStickEffect * eff);

        ///@copydetails JoyDevForceFeedback::remove
        void remove(const JoyStickEffect * eff);

        ///@copydetails JoyDevForceFeedback::getFFAxesNumber
        short getFFAxesNumber();

        ///@copydetails JoyDevForceFeedback::getFFMemoryLoad
        unsigned short getFFMemoryLoad();
    protected:
        Effects mEffects;           ///<
        NIIi mJoyStick;             ///< ÷˜øÿ
    };
}
}

#endif