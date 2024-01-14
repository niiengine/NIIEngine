#include "OISConfig.h"
#ifdef OIS_WIN32_WIIMOTE_SUPPORT
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
#ifndef OIS_WiiMoteForceFeedBack_H
#define OIS_WiiMoteForceFeedBack_H

#include "OISPrereqs.h"
#include "OISForceFeedback.h"
#include "wiimote.h"

namespace OIS
{
	class WiiMoteForceFeedback : public ForceFeedback
	{
	public:
		WiiMoteForceFeedback(cWiiMote &wiiMote);
		~WiiMoteForceFeedback();

		/** @copydoc ForceFeedback::upload */
		void upload( const Effect* effect );

		/** @copydoc ForceFeedback::modify */
		void modify( const Effect* effect );

		/** @copydoc ForceFeedback::remove */
		void remove( const Effect* effect );

		/** @copydoc ForceFeedback::setMasterGain */
		void setMasterGain( float level ) {}
		
		/** @copydoc ForceFeedback::setAutoCenterMode */
		void setAutoCenterMode( bool auto_on ) {}

		/** @copydoc ForceFeedback::getFFAxesNumber */
		short getFFAxesNumber() { return 1; }

	protected:
		//! The WiiMote associated with this effect interface
		cWiiMote &mWiiMote;

		//! The handle of the one and only allowed effect
		int mHandle;
	};
}
#endif //OIS_WiiMoteForceFeedBack_H
#endif
