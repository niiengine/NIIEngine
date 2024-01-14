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
#include "OgreD3D9VideoModeList.h"
#include "OgreException.h"
#include "OgreD3D9RenderSystem.h"

namespace Ogre 
{
    D3D9VideoModeList::D3D9VideoModeList( D3D9Driver* pDriver )
    {
        if( NULL == pDriver )
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "pDriver parameter is NULL", "D3D9VideoModeList::D3D9VideoModeList" );

        mDriver = pDriver;
        enumerate();
    }

    D3D9VideoModeList::~D3D9VideoModeList()
    {
        mDriver = NULL;
        mModeList.clear();
    }

    BOOL D3D9VideoModeList::enumerate()
    {
        UINT iMode;
        IDirect3D9* pD3D = D3D9RenderSystem::getDirect3D9();
        UINT adapter = mDriver->getAdapterNumber();

        for( iMode=0; iMode < pD3D->GetAdapterModeCount( adapter, D3DFMT_R5G6B5 ); iMode++ )
        {
            D3DDISPLAYMODE displayMode;
            pD3D->EnumAdapterModes( adapter, D3DFMT_R5G6B5, iMode, &displayMode );

            // Filter out low-resolutions
            if( displayMode.Width < 640 || displayMode.Height < 400 )
                continue;

            // Check to see if it is already in the list (to filter out refresh rates)
            BOOL found = FALSE;
            std::vector<D3D9VideoMode>::iterator it;
            for( it = mModeList.begin(); it != mModeList.end(); ++it )
            {
                D3DDISPLAYMODE oldDisp = it->getDisplayMode();
                if( oldDisp.Width == displayMode.Width &&
                    oldDisp.Height == displayMode.Height &&
                    oldDisp.Format == displayMode.Format )
                {
                    // Check refresh rate and favour higher if poss
                    if (oldDisp.RefreshRate < displayMode.RefreshRate)
                        it->increaseRefreshRate(displayMode.RefreshRate);
                    found = TRUE;
                    break;
                }
            }

            if( !found )
                mModeList.push_back( D3D9VideoMode( displayMode ) );
        }

        for( iMode=0; iMode < pD3D->GetAdapterModeCount( adapter, D3DFMT_X8R8G8B8 ); iMode++ )
        {
            D3DDISPLAYMODE displayMode;
            pD3D->EnumAdapterModes( adapter, D3DFMT_X8R8G8B8, iMode, &displayMode );

            // Filter out low-resolutions
            if( displayMode.Width < 640 || displayMode.Height < 400 )
                continue;

            // Check to see if it is already in the list (to filter out refresh rates)
            BOOL found = FALSE;
            std::vector<D3D9VideoMode>::iterator it;
            for( it = mModeList.begin(); it != mModeList.end(); ++it )
            {
                D3DDISPLAYMODE oldDisp = it->getDisplayMode();
                if( oldDisp.Width == displayMode.Width &&
                    oldDisp.Height == displayMode.Height &&
                    oldDisp.Format == displayMode.Format )
                {
                    // Check refresh rate and favour higher if poss
                    if (oldDisp.RefreshRate < displayMode.RefreshRate)
                        it->increaseRefreshRate(displayMode.RefreshRate);
                    found = TRUE;
                    break;
                }
            }

            if( !found )
                mModeList.push_back( D3D9VideoMode( displayMode ) );
        }

        return TRUE;
    }

    size_t D3D9VideoModeList::count()
    {
        return mModeList.size();
    }

    D3D9VideoMode* D3D9VideoModeList::item( size_t index )
    {
        std::vector<D3D9VideoMode>::iterator p = mModeList.begin();

        return &p[index];
    }

    D3D9VideoMode* D3D9VideoModeList::item( const String &name )
    {
        std::vector<D3D9VideoMode>::iterator it = mModeList.begin();
        if (it == mModeList.end())
            return NULL;

        for (;it != mModeList.end(); ++it)
        {
            if (it->getDescription() == name)
                return &(*it);
        }

        return NULL;
    }
}
