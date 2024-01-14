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
#include "NiiMaterialMatchListener.h"

namespace NII
{
    //-----------------------------------------------------------------------
    MaterialMatchListener::~MaterialMatchListener()
    {
    }
    //-----------------------------------------------------------------------
    ShaderFusion * MaterialMatchListener::find(SchemeID sid, const Material * mat,
        const GeometryObj * geo, Nidx lod)
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    ShaderFusion * MaterialMatchListener::miss(SchemeID sid, const Material * mat,
        const GeometryObj * geo, Nidx lod)
    {
        return 0;
    }
    //-----------------------------------------------------------------------
}