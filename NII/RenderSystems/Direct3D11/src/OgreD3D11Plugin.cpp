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
#include "OgreD3D11Plugin.h"
#include "OgreRoot.h"

namespace Ogre 
{
    const String sPluginName = "D3D11 RenderSystem";
    //---------------------------------------------------------------------
    D3D11Plugin::D3D11Plugin()
        : mRenderSys(0)
    {

    }
    //---------------------------------------------------------------------
    const String& D3D11Plugin::getName() const
    {
        return sPluginName;
    }
    //---------------------------------------------------------------------
    void D3D11Plugin::install()
    {
        // Create the DirectX 11 rendering api
        mRenderSys = new D3D11RenderSystem();
        // Register the render system
        Root::getSingleton().addRenderSystem( mRenderSys );
    }
    //---------------------------------------------------------------------
    void D3D11Plugin::initialise()
    {
        // nothing to do
    }
    //---------------------------------------------------------------------
    void D3D11Plugin::shutdown()
    {
        // nothing to do
    }
    //---------------------------------------------------------------------
    void D3D11Plugin::uninstall()
    {
        delete mRenderSys;
        mRenderSys = 0;
    }


}
