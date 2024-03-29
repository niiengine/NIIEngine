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
#include "OgreD3D11VideoModeList.h"
#include "OgreException.h"
#include "OgreD3D11Driver.h"
#include "OgreD3D11VideoMode.h"

namespace Ogre 
{
    //---------------------------------------------------------------------
    D3D11VideoModeList::D3D11VideoModeList(IDXGIAdapterN* pAdapter)
    {
        if( NULL == pAdapter)
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "pAdapter parameter is NULL", "D3D11VideoModeList::D3D11VideoModeList" );

        for( int iOutput = 0; ; ++iOutput )
        {
            //AIZTODO: one output for a single monitor ,to be handled for mulimon       
            ComPtr<IDXGIOutput> pOutput;
            HRESULT hr = pAdapter->EnumOutputs( iOutput, pOutput.ReleaseAndGetAddressOf());
            if( DXGI_ERROR_NOT_FOUND == hr )
                break;
            else if (FAILED(hr))
            {
                break;   //Something bad happened.
            }
            else //Success!
            {
				DXGI_OUTPUT_DESC OutputDesc;
				pOutput->GetDesc(&OutputDesc);

				UINT NumModes = 0;
				hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
					0,
					&NumModes,
					NULL);

				// If working over a terminal session, for example using the Simulator for deployment/development, display modes cannot be obtained.
				if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
				{
					DXGI_MODE_DESC fullScreenMode;
					fullScreenMode.Width = OutputDesc.DesktopCoordinates.right - OutputDesc.DesktopCoordinates.left;
					fullScreenMode.Height = OutputDesc.DesktopCoordinates.bottom - OutputDesc.DesktopCoordinates.top;
					fullScreenMode.RefreshRate.Numerator = 60;
					fullScreenMode.RefreshRate.Denominator = 1;
					fullScreenMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					fullScreenMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
					fullScreenMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

					mModeList.push_back(D3D11VideoMode(OutputDesc, fullScreenMode));
				}
				else if (hr == S_OK)
				{
					if (NumModes > 0)
					{
						// Create an array to store Display Mode information
						std::vector<DXGI_MODE_DESC> modes;
						modes.resize(NumModes);

						// Populate our array with information
						hr = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
							0,
							&NumModes,
							modes.data());

						for (UINT m = 0; m < NumModes; m++)
						{
							DXGI_MODE_DESC displayMode = modes[m];
							// Filter out low-resolutions
							if (displayMode.Width < 640 || displayMode.Height < 400)
								continue;

							// Check to see if it is already in the list (to filter out refresh rates)
							BOOL found = FALSE;
							std::vector<D3D11VideoMode>::iterator it;
							for (it = mModeList.begin(); it != mModeList.end(); it++)
							{
								DXGI_OUTPUT_DESC oldOutput = it->getDisplayMode();
								DXGI_MODE_DESC oldDisp = it->getModeDesc();
								if (//oldOutput.Monitor==OutputDesc.Monitor &&
									oldDisp.Width == displayMode.Width &&
									oldDisp.Height == displayMode.Height// &&
									//oldDisp.Format == displayMode.Format
									)
								{
									// Check refresh rate and favour higher if poss
									//if (oldDisp.RefreshRate < displayMode.RefreshRate)
									//  it->increaseRefreshRate(displayMode.RefreshRate);
									found = TRUE;
									break;
								}
							}

							if (!found)
								mModeList.push_back(D3D11VideoMode(OutputDesc, displayMode));

						}
					}
				}
            }
        }
    }
    //---------------------------------------------------------------------
    size_t D3D11VideoModeList::count()
    {
        return mModeList.size();
    }
    //---------------------------------------------------------------------
    D3D11VideoMode* D3D11VideoModeList::item( size_t index )
    {
        std::vector<D3D11VideoMode>::iterator p = mModeList.begin();

        return &p[index];
    }
    //---------------------------------------------------------------------
    D3D11VideoMode* D3D11VideoModeList::item( const String &name )
    {
        std::vector<D3D11VideoMode>::iterator it = mModeList.begin();
        if (it == mModeList.end())
            return NULL;

        for (;it != mModeList.end(); ++it)
        {
            if (it->getDescription() == name)
                return &(*it);
        }

        return NULL;
    }
    //---------------------------------------------------------------------
}
