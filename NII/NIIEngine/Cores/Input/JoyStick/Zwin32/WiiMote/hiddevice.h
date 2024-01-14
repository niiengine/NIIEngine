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
#include "OISConfig.h"
#ifdef OIS_WIN32_WIIMOTE_SUPPORT
//cWiimote 0.2 by Kevin Forbes (http://simulatedcomicproduct.com)
//This code is public domain, and comes with no warranty. The user takes full responsibility for anything that happens as a result from using this code.

#ifndef HIDDEVICE_H
#define HIDDEVICE_H

#include <windows.h>

class cHIDDevice
{
public:
	cHIDDevice();
	~cHIDDevice();
	bool Disconnect();
	bool Connect(unsigned short device_id, unsigned short vendor_id, int index=0);
	bool IsConnected() const {return mConnected;}

	bool WriteToDevice(unsigned const char * OutputReport, int num_bytes);
	bool ReadFromDevice(unsigned const char * buffer, int max_bytes, int & bytes_read, int timeout=50);
private:

	//bool OpenDevice(int index, unsigned short device_id, unsigned short vendor_id);
	bool OpenDevice(unsigned short device_id, unsigned short vendor_id, int index);
	//bool FindWiimote();
	void GetCapabilities();
	void PrepareForOverlappedTransfer();
	
	HANDLE mHandle;
	HANDLE mEvent;
	HANDLE WriteHandle;
	HANDLE ReadHandle;
	OVERLAPPED mOverlapped;
	OVERLAPPED HIDOverlapped;
//	HIDP_CAPS Capabilities;
	bool mConnected;

};
#endif
#endif
