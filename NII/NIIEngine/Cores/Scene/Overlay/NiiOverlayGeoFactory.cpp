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
#include "NiiOverlayGeoFactory.h"
#include "NiiOverlayTypeList.h"
#include "NiiRectOverlayGeo.h"

namespace NII
{
    //-----------------------------------------------------------------------
    void OverlayGeoFactory::destroyGeo(OverlayGeo * geo)
    {
        N_delete geo;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * RectOverlayGeoFactory::createGeo(OverlayGeoID id)
    {
        return N_new RectOverlayGeo(id);
    }
    //-----------------------------------------------------------------------
    OverlayGeoType RectOverlayGeoFactory::getTypeID() const
    {
        return N_OverlayType_Rect;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * ContainerOverlayGeoFactory::createGeo(OverlayGeoID id)
    {
        return N_new ContainerOverlayView(id);
    }
    //-----------------------------------------------------------------------
    OverlayGeoType ContainerOverlayGeoFactory::getTypeID() const
    {
        return N_OverlayType_Container;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * FrameOverlayGeoFactory::createGeo(OverlayGeoID id)
    {
        return N_new FrameOverlayView(id);
    }
    //-----------------------------------------------------------------------
    OverlayGeoType FrameOverlayGeoFactory::getTypeID() const
    {
        return N_OverlayType_Frame;
    }
    //-----------------------------------------------------------------------
    OverlayGeo * TextOverlayGeoFactory::createGeo(OverlayGeoID id)
    {
        return N_new TextOverlayGeo(id);
    }
    //-----------------------------------------------------------------------
    OverlayGeoType TextOverlayGeoFactory::getTypeID() const
    {
        return N_OverlayType_Text;
    }
    //-----------------------------------------------------------------------
}