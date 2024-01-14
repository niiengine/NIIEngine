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


#include "NiiPreProcess.h"
#include "NiiRenderFrameListener.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderFrameArgs
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RenderFrameArgs::RenderFrameArgs()
    {
    }
    //------------------------------------------------------------------------
    RenderFrameArgs::~RenderFrameArgs()
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderFrameListener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RenderFrameListener::RenderFrameListener() :
        mEnable(true)
    {
    }
    //------------------------------------------------------------------------
    RenderFrameListener::~RenderFrameListener()
    {
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onPreRender(const RenderFrameArgs *)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onRenderCore(const RenderFrameArgs *)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onRenderSky(const RenderFrameArgs * e)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onRenderTerrain(const RenderFrameArgs * e)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onRenderUI(const RenderFrameArgs * e)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderFrameListener::onEndRender(const RenderFrameArgs *)
    {
        return true;
    }
    //------------------------------------------------------------------------
}