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
#include "OgreD3D11HardwareOcclusionQuery.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreException.h"
#include "OgreD3D11Device.h"
namespace Ogre {

    /**
    * This is a class that is the DirectX9 implementation of 
    * hardware occlusion testing.
    *
    * @author Lee Sandberg
    *
    * Updated on 12/7/2004 by Chris McGuirk
    * Updated on 4/8/2005 by Tuan Kuranes email: tuan.kuranes@free.fr
    */

    /**
    * Default object constructor
    */
    D3D11HardwareOcclusionQuery::D3D11HardwareOcclusionQuery( D3D11Device & device ) :
        mDevice(device)
    { 
        D3D11_QUERY_DESC queryDesc;
        queryDesc.Query = D3D11_QUERY_OCCLUSION;
        queryDesc.MiscFlags = 0;
        // create the occlusion query
        const HRESULT hr = mDevice->CreateQuery(&queryDesc, mQuery.ReleaseAndGetAddressOf());

        if (FAILED(hr) || mDevice.isError()) 
        {   
            String errorDescription = mDevice.getErrorString(hr);
			N_EXCEPT_EX(Exception::ERR_INTERNAL_ERROR, hr,
                "Cannot allocate a Hardware query. This video card doesn't supports it, sorry.\nError Description:" + errorDescription, 
                "D3D11HardwareOcclusionQuery::D3D11HardwareOcclusionQuery");
        }
    }

    /**
    * Object destructor
    */
    D3D11HardwareOcclusionQuery::~D3D11HardwareOcclusionQuery() 
    { 
    }

    //------------------------------------------------------------------
    // Occlusion query functions (see base class documentation for this)
    //--
    void D3D11HardwareOcclusionQuery::beginOcclusionQuery() 
    {           
        mDevice.GetImmediateContext()->Begin(mQuery.Get());//Issue(D3DISSUE_BEGIN); 
        mIsQueryResultStillOutstanding = true;
        mPixelCount = 0;
    }

    void D3D11HardwareOcclusionQuery::endOcclusionQuery() 
    { 
        mDevice.GetImmediateContext()->End(mQuery.Get());//Issue(D3DISSUE_END); 
    }

    //------------------------------------------------------------------
    bool D3D11HardwareOcclusionQuery::pullOcclusionQuery( unsigned int* NumOfFragments ) 
    {
        // in case you didn't check if query arrived and want the result now.
        if (mIsQueryResultStillOutstanding)
        {
            // Loop until the data becomes available
            UINT64 pixels;
            const size_t dataSize = sizeof( UINT64 );
            while (1)
            {
                const HRESULT hr = mDevice.GetImmediateContext()->GetData(mQuery.Get(), (void *)&pixels, dataSize, 0);//D3DGETDATA_FLUSH

                if  (hr == S_FALSE)
                    continue;
                if  (hr == S_OK)
                {
                    mPixelCount = (unsigned)pixels;
                    *NumOfFragments = (unsigned)pixels;
                    break;
                }
                //in directx10 the device will never be lost
               /* if (hr == D3DERR_DEVICELOST)
                {
                    *NumOfFragments = 100000;
                    mPixelCount = 100000;
                    D3D11_QUERY_DESC queryDesc;
                    queryDesc.Query = D3D11_QUERY_OCCLUSION;
                    mDevice->CreateQuery(%queryDesc, mQuery.ReleaseAndGetAddressOf());
                    break;
                }
                */
            } 
            mIsQueryResultStillOutstanding = false;
        }
        else
        {
            // we already stored result from last frames.
            *NumOfFragments = mPixelCount;
        }       
        return true;
    }
    //------------------------------------------------------------------
    bool D3D11HardwareOcclusionQuery::isStillOutstanding(void)
    {       
        // in case you already asked for this query
        if (!mIsQueryResultStillOutstanding)
            return false;

        UINT64 pixels;
        const HRESULT hr = mDevice.GetImmediateContext()->GetData(mQuery.Get(), (void *) &pixels, sizeof( UINT64 ), 0);

        if (hr  == S_FALSE)
            return true;

       /* if (hr == D3DERR_DEVICELOST)
        {
            mPixelCount = 100000;
            D3D11_QUERY_DESC queryDesc;
            queryDesc.Query = D3D11_QUERY_OCCLUSION;
            mDevice->CreateQuery(&queryDesc, mQuery.ReleaseAndGetAddressOf());
        }
        */
        mPixelCount = (unsigned)pixels;
        mIsQueryResultStillOutstanding = false;
        return false;
    
    } 
}
