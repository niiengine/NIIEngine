#include "OISConfig.h"
#ifdef OIS_LIRC_SUPPORT
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
#ifndef OIS_LIRC_H
#define OIS_LIRC_H
#include "OISJoyStick.h"
#include "OISLIRCRingBuffer.h"

namespace OIS
{
	class LIRCFactoryCreator;

	struct RemoteInfo
	{
		RemoteInfo() : buttons(0) {}

		RemoteInfo( const RemoteInfo &other )
		{
			buttons = other.buttons;
			buttonMap = other.buttonMap;
		}

		int buttons;
		std::map<std::string, int> buttonMap;
	};

	//Number of ring buffer events. should be nice sized (the structure is not very big)
	//Will be rounded up to power of two automatically
	#define OIS_LIRC_EVENT_BUFFER 16

	/**	Specialty joystick - Linux Infrared Remote Support */
	class _OISExport LIRCControl : public JoyStick
	{
		friend class LIRCFactoryCreator;
	public:
		LIRCControl(InputManager* creator, int id, bool buffered, LIRCFactoryCreator* local_creator, RemoteInfo &info);
		~LIRCControl();

		//Overrides of Object
		/** copydoc Object::setBuffered */
		void setBuffered(bool buffered);

		/** copydoc Object::capture */
		void capture();

		/** copydoc Object::queryInterface */
		Interface* queryInterface(Interface::IType type);

		/** copydoc Object::_intialize */
		void _init();

	protected:
		//! Internal method used to add a button press to the queue (called from thread)
		void queueButtonPressed(const std::string &id);

		//! The creator who created us
		LIRCFactoryCreator *mLIRCCreator;

		//! Ringbuffer is used to store events from thread and be read from capture
		LIRCRingBuffer mRingBuffer;

		//! Information about remote
		RemoteInfo mInfo;
	};
}
#endif //OIS_LIRC_H
#endif
