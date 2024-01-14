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

#include "OgrePortal.h"
#include "OgrePCZSceneNode.h"
#include "OgreSphere.h"
#include "OgreSegment.h"
#include "OgreRay.h"
#include "OgrePCZone.h"   // need access to real zone class

using namespace NII;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Portal
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Portal::Portal(SpaceID sid, const PORTAL_TYPE type) :
        PortalBase(sid, type),
        mTargetZone(0),
        mTargetPortal(0)
    {
    }
    //-----------------------------------------------------------------------
    Portal::~Portal()
    {
    }
    //-----------------------------------------------------------------------
    // Set the 1st Zone the Portal connects to
    void Portal::setTargetZone(PCZone * zone)
    {
        mTargetZone = zone;
    }
    //-----------------------------------------------------------------------
    PCZone * Portal::getTargetZone() 
    {
        return mTargetZone;
    }
    //-----------------------------------------------------------------------
    // Set the Portal the Portal connects to
    void Portal::setTargetPortal(Portal * portal)
    {
        mTargetPortal = portal;
    }
    //-----------------------------------------------------------------------
    Portal * Portal::getTargetPortal() 
    {
        return mTargetPortal;
    }
    //-----------------------------------------------------------------------
    /// @copydoc SpaceObj::getFactoryID
    FactoryID Portal::getFactoryID() const
    {
        return "Portal";
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PortalFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    unsigned long PortalFactory::FACTORY_TYPE_FLAG = 0xFFFFFFFF;
    //-----------------------------------------------------------------------
    PortalFactory::PortalFactory()
    {
    }
    //-----------------------------------------------------------------------
    PortalFactory::~PortalFactory()
    {
    }
    //-----------------------------------------------------------------------
    SpaceObj * PortalFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        SpaceObj * re = N_new Portal(sid, getPortalType(params));
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------
    void PortalFactory::destroy(SpaceObj * obj)
    {
        N_delete obj;
    }
    //-----------------------------------------------------------------------