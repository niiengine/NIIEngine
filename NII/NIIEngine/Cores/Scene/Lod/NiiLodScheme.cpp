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
#include "NiiLodScheme.h"
#include "NiiMesh.h"
#include "NiiMaterial.h"

namespace NII
{
    //------------------------------------------------------------------------
    LodScheme::LodScheme(SchemeID sid) :
        mSid(sid),
        mThreshold(0),
        mFactor(1000)
    {
    }
    //------------------------------------------------------------------------
    LodScheme::~LodScheme()
    {
    }
    //------------------------------------------------------------------------
    SchemeID LodScheme::getTypeID() const
    {
        return mSid;
    }
    //------------------------------------------------------------------------
    NIIf LodScheme::getOrigin() const
    {
        return 0.0f;
    }
    //------------------------------------------------------------------------
    Nui32 LodScheme::getDest(NIIf src) const
    {
        return src * mFactor;
    }
    //------------------------------------------------------------------------
    NIIf LodScheme::getBias(NIIf f) const
    {
        return f * mFactor;
    }
    //------------------------------------------------------------------------
    void LodScheme::setFactor(Nui32 f)
    {
        mFactor = f;
    }
    //------------------------------------------------------------------------
    Nui32 LodScheme::getFactor() const
    {
        return mFactor;
    }
    //------------------------------------------------------------------------
    void LodScheme::setThreshold(Nui32 t)
    {
        mThreshold = t;
    }
    //------------------------------------------------------------------------
    Nui32 LodScheme::getThreshold() const
    {
        return mThreshold;
    }
    //------------------------------------------------------------------------
    void LodScheme::setValid(bool s)
    {
        (void)s;
    }
    //------------------------------------------------------------------------
    bool LodScheme::isValid() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    NIIf LodScheme::getValue(const SpaceObj * obj, const Camera * camera) const
    {
        return getValueImpl(obj, camera->getExtLod());
    }
    //------------------------------------------------------------------------
}