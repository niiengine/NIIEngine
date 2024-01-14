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
#ifndef OIS_WiiMoteFactoryCreator_H
#define OIS_WiiMoteFactoryCreator_H

#include "OISPrereqs.h"
#include "OISFactoryCreator.h"
#include <deque>

//Forward declare boost classes used
namespace boost
{
	class thread;
	class mutex;
}

namespace OIS
{
	//Forward declare local classes
	class WiiMote;

	//! Max amount of Wiis we will attempt to find
	#define OIS_cWiiMote_MAX_WIIS 4

	/** WiiMote Factory Creator Class */
	class _OISExport WiiMoteFactoryCreator : public FactoryCreator
	{
	public:
		WiiMoteFactoryCreator();
		~WiiMoteFactoryCreator();

		//FactoryCreator Overrides
		/** @copydoc FactoryCreator::deviceList */
		DeviceList freeDeviceList();

		/** @copydoc FactoryCreator::totalDevices */
		int totalDevices(Type iType);

		/** @copydoc FactoryCreator::freeDevices */
		int freeDevices(Type iType);

		/** @copydoc FactoryCreator::vendorExist */
		bool vendorExist(Type iType, const std::string & vendor);

		/** @copydoc FactoryCreator::createObject */
		Object* createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor = "");

		/** @copydoc FactoryCreator::destroyObject */
		void destroyObject(Object* obj);

		//! Local method used to return controller to pool
		void _returnWiiMote(int id);

	protected:
		//! Internal - threaded method
		bool _updateWiiMotesThread();

		//! String name of this vendor
		std::string mVendorName;

		//! queue of open wiimotes (int represents index into hid device)
		std::deque<int> mFreeWiis;

		//! Number of total wiimotes
		int mCount;

		//! Boost thread execution object (only alive when at least 1 wiimote is alive)
		boost::thread *mtThreadHandler;
		
		//! Gaurds access to the Active WiiMote List
		boost::mutex *mtWiiMoteListMutex;

		//! List of created (active) WiiMotes
		std::vector<WiiMote*> mtInUseWiiMotes;

		//! Used to signal thread running or not
		volatile bool mtThreadRunning;
	};
}
#endif //OIS_WiiMoteFactoryCreator_H
#endif
