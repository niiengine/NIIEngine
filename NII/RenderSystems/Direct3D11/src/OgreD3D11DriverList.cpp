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
#include "OgreD3D11DriverList.h"
#include "OgreD3D11Device.h"
#include "OgreD3D11Driver.h"
#include "OgreException.h"
#include "OgreLogManager.h"

namespace Ogre 
{
    //-----------------------------------------------------------------------
    D3D11DriverList::D3D11DriverList() 
    {
        mHiddenDriversCount = 0;
    }
    //-----------------------------------------------------------------------
    D3D11DriverList::~D3D11DriverList(void)
    {
    }
    //-----------------------------------------------------------------------
    void D3D11DriverList::clear()
    {
        mHiddenDriversCount = 0;
        mDriverList.clear();
    }
    //-----------------------------------------------------------------------
    void D3D11DriverList::refresh()
    {
        clear();
        std::map<std::wstring, unsigned> sameNameCounter;

        LogManager::getSingleton().logMessage( "D3D11: Driver Detection Starts" );

        // We need fresh IDXGIFactory to get fresh driver list
        ComPtr<IDXGIFactoryN> pDXGIFactory;
        HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactoryN), (void**)pDXGIFactory.GetAddressOf());
        if( FAILED(hr) )
        {
            N_EXCEPT_EX(RenderingAPI, hr,
                "Failed to create Direct3D11 DXGIFactory1",
                "D3D11DriverList::refresh");
        }

        for( UINT iAdapter=0; ; iAdapter++ )
        {
            ComPtr<IDXGIAdapterN> pDXGIAdapter;
            HRESULT hr = pDXGIFactory->EnumAdapters1(iAdapter, pDXGIAdapter.GetAddressOf());
            if( DXGI_ERROR_NOT_FOUND == hr )
            {
                hr = S_OK;
                break;
            }
            if( FAILED(hr) )
            {
                N_EXCEPT_EX(RenderingAPI, hr,
                    "Failed to enum adapters",
                    "D3D11DriverList::refresh");
            }

            DXGI_ADAPTER_DESC1 desc1 = {0};
            hr = pDXGIAdapter->GetDesc1(&desc1);
            if( FAILED(hr) )
            {
                N_EXCEPT_EX(RenderingAPI, hr,
                    "Failed to get adapter description",
                    "D3D11DriverList::refresh");
            }

            unsigned sameNameIndex = sameNameCounter[std::wstring(desc1.Description)]++;

            SharedPtr<D3D11Driver> driver(OGRE_NEW_T(D3D11Driver, MEMCATEGORY_GENERAL)(pDXGIAdapter.Get(), desc1, sameNameIndex), SPFM_DELETE_T);

            LogManager::getSingleton().logMessage("D3D11: \"" + driver->DriverDescription() + "\"");

            // we don't want NVIDIA PerfHUD in the list, so place it to the hidden part of drivers list
            const bool isHidden = wcscmp(driver->getAdapterIdentifier().Description, L"NVIDIA PerfHUD") == 0;
            if(isHidden)
            {
                mDriverList.push_back(driver);
                ++mHiddenDriversCount;
            }
            else
            {
                mDriverList.insert(mDriverList.end() - mHiddenDriversCount, driver);
            }
        }

        LogManager::getSingleton().logMessage( "D3D11: Driver Detection Ends" );
    }
    //-----------------------------------------------------------------------
    size_t D3D11DriverList::count() const 
    {
        return mDriverList.size() - mHiddenDriversCount;
    }
    //-----------------------------------------------------------------------
    D3D11Driver* D3D11DriverList::item( size_t index )
    {
        return mDriverList.at( index ).get();
    }
    //-----------------------------------------------------------------------
    D3D11Driver* D3D11DriverList::item( const String &name )
    {
        for(std::vector<SharedPtr<D3D11Driver> >::iterator it = mDriverList.begin(), it_end = mDriverList.end(); it != mDriverList.end(); ++it)
        {
            if((*it)->DriverDescription() == name)
                return (*it).get();
        }

        return NULL;
    }
    //-----------------------------------------------------------------------
    D3D11Driver* D3D11DriverList::findByName(const String &name)
    {
        // return requested driver
        if(!name.empty())
            if(D3D11Driver* driver = item(name))
                return driver;

        // return default driver
        if(!mDriverList.empty())
            return mDriverList[0].get();

        return NULL;
    }
    //-----------------------------------------------------------------------
}
