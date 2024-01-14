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
#include "OgreD3D11Driver.h"
#include "OgreD3D11VideoModeList.h"
#include "OgreD3D11VideoMode.h"
#include "OgreD3D11Device.h"
#include "OgreString.h"
#include "OgreStringConverter.h"

#ifndef DXGI_ADAPTER_FLAG_SOFTWARE
#define DXGI_ADAPTER_FLAG_SOFTWARE 2 // unavailable in June 2010 SDK
#endif

namespace Ogre
{
    //---------------------------------------------------------------------
    D3D11Driver::D3D11Driver() 
    {
        ZeroMemory( &mAdapterIdentifier, sizeof(mAdapterIdentifier) );
        mSameNameAdapterIndex = 0;
    }
    //---------------------------------------------------------------------
    D3D11Driver::D3D11Driver(IDXGIAdapterN* pDXGIAdapter, const DXGI_ADAPTER_DESC1& desc, unsigned sameNameIndex)
    {
        mDXGIAdapter = pDXGIAdapter;
        mAdapterIdentifier = desc;
        mSameNameAdapterIndex = sameNameIndex;
    }
    //---------------------------------------------------------------------
    D3D11Driver::~D3D11Driver()
    {
    }
    //---------------------------------------------------------------------
    String D3D11Driver::DriverDescription() const
    {
        char str[sizeof(mAdapterIdentifier.Description) + 1];
        wcstombs(str, mAdapterIdentifier.Description, sizeof(str) - 1);
        str[sizeof(str) - 1] = '\0';
        String driverDescription=str;
        StringUtil::trim(driverDescription);
        if(mAdapterIdentifier.VendorId == 0x1414 && mAdapterIdentifier.DeviceId == 0x8c && (mAdapterIdentifier.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
            driverDescription += " (software)"; // It`s software only variation of "Microsoft Basic Render Driver", always present since Win8
        else if(mSameNameAdapterIndex != 0)
            driverDescription += " (" + Ogre::StrConv::conv(mSameNameAdapterIndex + 1) + ")";

        return driverDescription;
    }
    //---------------------------------------------------------------------
    D3D11VideoModeList* D3D11Driver::getVideoModeList()
    {
        if(!mVideoModeList)
            mVideoModeList = SharedPtr<D3D11VideoModeList>(OGRE_NEW_T(D3D11VideoModeList, MEMCATEGORY_GENERAL)(getDeviceAdapter()), SPFM_DELETE_T);

        return mVideoModeList.get();
    }
    //---------------------------------------------------------------------
    const DXGI_ADAPTER_DESC1& D3D11Driver::getAdapterIdentifier() const
    {
        return mAdapterIdentifier;
    }
    //---------------------------------------------------------------------
    IDXGIAdapterN* D3D11Driver::getDeviceAdapter() const
    {
        return mDXGIAdapter.Get();
    }
    //---------------------------------------------------------------------
}
