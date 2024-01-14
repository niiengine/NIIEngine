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
#include "NiiSpaceQuery.h"
#include "NiiException.h"
#include "NiiSpaceManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    SpaceQuery::SpaceQuery() :
        mQueryMask(0xFFFFFFFF),
        mPrimitivesMark(SpaceQuery::PT_None)
    {
        // default type mask to everything except lights & fx (previous behaviour)
        mQueryTypeMask = (0xFFFFFFFF & ~SpaceManager::ParticleSpaceMark)
            & ~SpaceManager::LightSpaceMark;
    }
    //-----------------------------------------------------------------------
    SpaceQuery::~SpaceQuery()
    {
    }
    //-----------------------------------------------------------------------
    void SpaceQuery::setQueryMask(Nmark mask)
    {
        mQueryMask = mask;
    }
    //-----------------------------------------------------------------------
    Nmark SpaceQuery::getQueryMask() const
    {
        return mQueryMask;
    }
    //-----------------------------------------------------------------------
    void SpaceQuery::setQueryTypeMask(Nmark mask)
    {
        mQueryTypeMask = mask;
    }
    //-----------------------------------------------------------------------
    Nmark SpaceQuery::getQueryTypeMask() const
    {
        return mQueryTypeMask;
    }
    //-----------------------------------------------------------------------
    void SpaceQuery::setPrimitivesMark(Nmark type)
    {
        mPrimitivesMark = mSupportPrimitivesMark & type;
    }
    //-----------------------------------------------------------------------
    Nmark SpaceQuery::getPrimitivesMark() const
    {
        return mPrimitivesMark;
    }
    //-----------------------------------------------------------------------
    void SpaceQuery::addSupportPrimitives(PrimitivesType type)
    {

    }
    //-----------------------------------------------------------------------
    Nmark SpaceQuery::getSupportPrimitivesMark() const
    {
        return mSupportPrimitivesMark;
    }
    //-----------------------------------------------------------------------
}