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

#ifndef _NII_WIN32_JOYSTICK_FORCEFEEDBACK_H_
#define _NII_WIN32_JOYSTICK_FORCEFEEDBACK_H_

#include "NiiWin32ControlPatternPrereq.h"
#include "NiiJoyDevForceFeedback.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class Win32JoyDevForceFeedback : public JoyDevForceFeedback
    {
    public:
        typedef map<NIIi, LPDIRECTINPUTEFFECT>::type Effects;
    public:
        Win32JoyDevForceFeedback(IDirectInputDevice8 * ipid, const DIDEVCAPS * dcap);
        ~Win32JoyDevForceFeedback();

		/**
			@remarks
			Internal use.. Used during enumeration to build a list of a devices
			support effects.
		*/
		void _addEffectSupport(LPCDIEFFECTINFO pdei);

		/**
			@remarks
			Internal use.. Used during axis enumeration to get number of FF axes
			support effects.
		*/
		void _addFFAxis();

        /// 更新效果
        void upload(GUID guid, DIEFFECT * dif, const JoyStickEffect * eff);

        ///@copydetails JoyDevForceFeedback::setMasterGain
        void setMasterGain(NIIf level);

        ///@copydetails JoyDevForceFeedback::setAutoCenterMode
        void setAutoCenterMode(bool set);

        ///@copydetails JoyDevForceFeedback::upload
        void upload(const JoyStickEffect * effect);

        ///@copydetails JoyDevForceFeedback::modify
        void modify(const JoyStickEffect * effect);

        ///@copydetails JoyDevForceFeedback::remove
        void remove(const JoyStickEffect * effect);

        ///@copydetails JoyDevForceFeedback::getFFAxesNumber
        short getFFAxesNumber();

        ///@copydetails JoyDevForceFeedback::getFFMemoryLoad
        unsigned short getFFMemoryLoad();
    protected:
        Effects mEffects;                       ///<
        NIIi mCurrentNum;                       ///<
		NIIs mFFAxes;                           ///<
		const DIDEVCAPS * mCaps;                ///<
		IDirectInputDevice8 * mJoyStick;        ///< 主控
    };
}
}

#endif