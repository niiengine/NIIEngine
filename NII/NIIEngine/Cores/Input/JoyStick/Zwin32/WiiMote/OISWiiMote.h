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
#ifndef OIS_WiiMote_H
#define OIS_WiiMote_H
#include "OISJoyStick.h"
#include "OISWiiMoteRingBuffer.h"
#include "wiimote.h"

namespace OIS
{
	class WiiMoteFactoryCreator;
	class WiiMoteForceFeedback;

	//Number of ring buffer events. should be nice sized (the structure is not very big)
	//Will be rounded up to power of two automatically
	#define OIS_WII_EVENT_BUFFER 32

	/**	Specialty joystick - WiiMote controller */
	class _OISExport WiiMote : public JoyStick
	{
	public:
		WiiMote(InputManager* creator, int id, bool buffered, WiiMoteFactoryCreator* local_creator);
		~WiiMote();

		//Overrides of Object
		void setBuffered(bool buffered);

		void capture();

		Interface* queryInterface(Interface::IType type);

		void _init();

		void _threadUpdate();

	protected:
		void _doButtonCheck(bool new_state, int ois_button, unsigned int &pushed, unsigned int &released);
		bool _doPOVCheck(const cWiiMote::tButtonStatus &bState, unsigned int &newPosition);

		//! The creator who created us
		WiiMoteFactoryCreator *mWiiCreator;

		//! Actual WiiMote HID device
		cWiiMote mWiiMote;

		//! Used to signal thread that remote is ready
		volatile bool mtInitialized;

		//! Ringbuffer is used to store events from thread and be read from capture
		WiiMoteRingBuffer mRingBuffer;

		//Following variables are used entirely within threaded context
		int mtLastButtonStates;
		unsigned int mtLastPOVState;
		float mtLastX, mtLastY, mtLastZ;
		float mtLastNunChuckX, mtLastNunChuckY, mtLastNunChuckZ;
		int mLastNunChuckXAxis, mLastNunChuckYAxis;

		//Small workaround for slow calibration of wiimote data
		int _mWiiMoteMotionDelay;

		//Simple rumble force
		WiiMoteForceFeedback *mRumble;
	};
}
#endif //OIS_WiiMote_H
#endif
