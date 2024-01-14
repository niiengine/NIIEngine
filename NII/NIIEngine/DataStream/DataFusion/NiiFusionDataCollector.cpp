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
#include "NiiFusionDataCollector.h"
#include "NiiException.h"

namespace NII
{
    //--------------------------------------------------------------------------
    FusionDataCollector::FusionCollectorList FusionDataCollector::mFusionCollectorList;
    //--------------------------------------------------------------------------
    FusionDataCollector::FusionDataCollector()
    {
    }
    //--------------------------------------------------------------------------
    FusionDataCollector::~FusionDataCollector()
    {
    }
    //--------------------------------------------------------------------------
    FusionData * FusionDataCollector::createFusionData(Nid id)
    {
        N_EXCEPT(NotExist, _I18n("No animable value named '") + N_conv(id) + _I18n("' present."));
    }
    //--------------------------------------------------------------------------
    TypeID FusionDataCollector::getFusionCollectorType() const
    {
        return 0;
    }
    //--------------------------------------------------------------------------
    void FusionDataCollector::setupFusionData() const
    {
        if(mFusionCollectorList.find(getFusionCollectorType()) == mFusionCollectorList.end())
        {
            IDList * vec = &mFusionCollectorList.insert(
                Npair(getFusionCollectorType(), IDList())).first->second;
            initFusionCollector(vec);
        }
    }
    //--------------------------------------------------------------------------
    void FusionDataCollector::initFusionCollector(IDList *) const
    {

    }
    //--------------------------------------------------------------------------
}