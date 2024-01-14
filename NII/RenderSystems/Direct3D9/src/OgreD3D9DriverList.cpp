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
#include "OgreD3D9DriverList.h"
#include "OgreLogManager.h"
#include "OgreException.h"
#include "OgreD3D9RenderSystem.h"

namespace Ogre 
{
    D3D9DriverList::D3D9DriverList()
    {
        enumerate();
    }

    D3D9DriverList::~D3D9DriverList(void)
    {
        mDriverList.clear();
    }

    BOOL D3D9DriverList::enumerate()
    {
        IDirect3D9* lpD3D9 = D3D9RenderSystem::getDirect3D9();

        LogManager::getSingleton().logMessage( "D3D9: Driver Detection Starts" );
        for( UINT iAdapter=0; iAdapter < lpD3D9->GetAdapterCount(); ++iAdapter )
        {
            D3DADAPTER_IDENTIFIER9 adapterIdentifier;
            D3DDISPLAYMODE d3ddm;
            D3DCAPS9 d3dcaps9;
            
            lpD3D9->GetAdapterIdentifier( iAdapter, 0, &adapterIdentifier );
            lpD3D9->GetAdapterDisplayMode( iAdapter, &d3ddm );
            lpD3D9->GetDeviceCaps( iAdapter, D3DDEVTYPE_HAL, &d3dcaps9 );

            mDriverList.push_back( D3D9Driver( iAdapter, d3dcaps9, adapterIdentifier, d3ddm ) );
        }

        LogManager::getSingleton().logMessage( "D3D9: Driver Detection Ends" );

        return TRUE;
    }

    size_t D3D9DriverList::count() const 
    {
        return mDriverList.size();
    }

    D3D9Driver* D3D9DriverList::item( size_t index )
    {
        return &mDriverList.at( index );
    }

    D3D9Driver* D3D9DriverList::item( const String &name )
    {
        std::vector<D3D9Driver>::iterator it = mDriverList.begin();
        if (it == mDriverList.end())
            return NULL;

        for (;it != mDriverList.end(); ++it)
        {
            if (it->DriverDescription() == name)
                return &(*it);
        }

        return NULL;
    }
}
