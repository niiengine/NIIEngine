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
#include "OgreD3D9Driver.h"
#include "OgreD3D9VideoModeList.h"
#include "OgreD3D9VideoMode.h"

namespace Ogre
{   
    D3D9Driver::D3D9Driver()
    {                       
        mAdapterNumber  = 0;
        ZeroMemory( &mD3D9DeviceCaps, sizeof(mD3D9DeviceCaps) );
        ZeroMemory( &mAdapterIdentifier, sizeof(mAdapterIdentifier) );
        ZeroMemory( &mDesktopDisplayMode, sizeof(mDesktopDisplayMode) );        
        mVideoModeList = NULL;              
    }

    D3D9Driver::D3D9Driver( const D3D9Driver &ob )
    {           
        mAdapterNumber      = ob.mAdapterNumber;
        mD3D9DeviceCaps     = ob.mD3D9DeviceCaps;
        mAdapterIdentifier  = ob.mAdapterIdentifier;
        mDesktopDisplayMode = ob.mDesktopDisplayMode;
        mVideoModeList      = NULL;             
    }

    D3D9Driver::D3D9Driver( unsigned int adapterNumber,
        const D3DCAPS9& deviceCaps,
        const D3DADAPTER_IDENTIFIER9& adapterIdentifier, 
        const D3DDISPLAYMODE& desktopDisplayMode )
    {               
        mAdapterNumber      = adapterNumber;
        mD3D9DeviceCaps     = deviceCaps;
        mAdapterIdentifier  = adapterIdentifier;
        mDesktopDisplayMode = desktopDisplayMode;
        mVideoModeList      = NULL;         
    }

    D3D9Driver::~D3D9Driver()
    {
        if (mVideoModeList != NULL)
        {
            N_delete mVideoModeList;
            mVideoModeList = NULL;
        }
    }

    String D3D9Driver::DriverName() const
    {
        return String(mAdapterIdentifier.Driver);
    }

    String D3D9Driver::DriverDescription() const
    {       
        StringStream str;
        str << "Monitor-" << (mAdapterNumber+1) << "-" << mAdapterIdentifier.Description;
        String driverDescription(str.str());
        StringUtil::trim(driverDescription);

        return  driverDescription;
    }

    D3D9VideoModeList* D3D9Driver::getVideoModeList()
    {
        if( !mVideoModeList )
            mVideoModeList = N_new D3D9VideoModeList( this );

        return mVideoModeList;
    }   
}
