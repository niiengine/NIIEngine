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
#include "NiiCustomSpaceObjFactory.h"
#include "NiiCustomSpaceObj.h"
#include "NiiFactoryList.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CustomSpaceObjFactory
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CustomSpaceObjFactory::CustomSpaceObjFactory()
    {
    }
    //------------------------------------------------------------------------
    CustomSpaceObjFactory::~CustomSpaceObjFactory()
    {
    }
    //------------------------------------------------------------------------
    FactoryID CustomSpaceObjFactory::getID() const
    {
        return N_FACTORY_CustomSpace;
    }
    //-----------------------------------------------------------------------------
    SpaceObj * CustomSpaceObjFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        SpaceObj * re = N_new CustomSpaceObj(sid);
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------------
    void CustomSpaceObjFactory::destroy(void * obj)
    {
        SpaceObj * temp = static_cast<SpaceObj *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------------
}