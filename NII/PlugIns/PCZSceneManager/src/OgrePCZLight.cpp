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

#include "OgreStableHeaders.h"
#include "OgrePCZLight.h"
#include "OgrePCZone.h" // need for testing affected zone
#include "OgreException.h"
#include "OgrePCZSceneNode.h"
#include "OgrePCZCamera.h"
#include "OgrePCZFrustum.h"
#include "OgrePCZSceneManager.h"

namespace Ogre
{
    //-----------------------------------------------------------------------
    PCZLight::PCZLight() :
        Light()
    {
		mNeedsUpdate = true;   // need to update the first time, regardless of attachment or movement
    }
    //-----------------------------------------------------------------------
	PCZLight::PCZLight(SpaceID sid) :
        Light(sid)
    {
		mNeedsUpdate = true;   // need to update the first time, regardless of attachment or movement
    }
    //-----------------------------------------------------------------------
    PCZLight::~PCZLight()
    {
        affectedZonesList.clear();
    }
    //-----------------------------------------------------------------------
    FactoryID PCZLight::getFactoryID() const
    {
		return "PCZLight";
    }
    //-----------------------------------------------------------------------
    /** Clear the affectedZonesList
    */
    void PCZLight::clearAffectedZones(void)
    {
        affectedZonesList.clear();
    }
    //-----------------------------------------------------------------------
    /** Add a zone to the zones affected list
    */
    void PCZLight::addZoneToAffectedZonesList(PCZone * zone)
    {
        affectedZonesList.push_back(zone);
    }
    //-----------------------------------------------------------------------
    /** check if a zone is in the list of zones affected by the light
    */
    bool PCZLight::affectsZone(PCZone * zone)
    {
        ZoneList::iterator it = std::find(affectedZonesList.begin(), affectedZonesList.end(), zone);
        if(it == affectedZonesList.end())
        {
            // not in the affectedZonesList
            return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    void PCZLight::updateZones(PCZone * defaultZone, unsigned long frameCount)
    {
        //update the zones this light affects
        PCZone * homeZone;
        affectedZonesList.clear();
        mAffectsVisibleZone = false;
        PCZSceneNode * sn = (PCZSceneNode*)(this->getSceneNode());
        if (sn)
        {
            // start with the zone the light is in
            homeZone = sn->getHomeZone();
            if (homeZone)
            {
                affectedZonesList.push_back(homeZone);
                if (homeZone->getLastVisibleFrame() == frameCount)
                {
                    mAffectsVisibleZone = true;
                }
            }
            else
            {
                // error - scene node has no homezone!
                // just say it affects the default zone and leave it at that.
                affectedZonesList.push_back(defaultZone);
                if (defaultZone->getLastVisibleFrame() == frameCount)
                {
                    mAffectsVisibleZone = true;
                }
                return;
            }
        }
        else
        {
            // ERROR! not connected to a scene node,
            // just say it affects the default zone and leave it at that.
            affectedZonesList.push_back(defaultZone);
            if (defaultZone->getLastVisibleFrame() == frameCount)
            {
                mAffectsVisibleZone = true;
            }
            return;
        }

        // now check visibility of each portal in the home zone.  If visible to
        // the light then add the target zone of the portal to the list of
        // affected zones and recurse into the target zone
        static PCZFrustum portalFrustum;
        Vector3 v = getSpacePos();
        portalFrustum.setOrigin(v);
        homeZone->_checkLightAgainstPortals(this, frameCount, &portalFrustum, 0);
    }
	//-----------------------------------------------------------------------
	void PCZLight::removeZoneFromAffectedZonesList(PCZone * zone)
	{
		ZoneList::iterator it = std::find(affectedZonesList.begin(), affectedZonesList.end(), zone);

		if (it != affectedZonesList.end())
		{
			affectedZonesList.erase( it );   // zone is in list, erase it.
		}
	}
	//-----------------------------------------------------------------------
	void PCZLight::_notify(void)
	{
		Light::_notify();   // inform ogre Light of movement

		mNeedsUpdate = true;   // set need update flag
	}
	//-----------------------------------------------------------------------
	bool PCZLight::getNeedsUpdate(void)
	{
		if(mNeedsUpdate)   // if this light has moved, return true immediately
			return true;

		// if any zones affected by this light have updated portals, then this light needs updating too
        ZoneList::iterator i, iend = affectedZonesList.end();
		for(ZoneList::iterator i = affectedZonesList.begin() ; i != iend; ++i)
		{
			if((*i)->getPortalsUpdated())
                return true;   // return immediately to prevent further iterating
		}

		return false;   // light hasn't moved, and no zones have updated portals. no light update.
	}
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PCZLightFactory
    //-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	FactoryID PCZLightFactory::getID() const
	{
		return "PCZLight";
	}
	//-----------------------------------------------------------------------
	SpaceObj * PCZLightFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0)
	{
        SpaceObj * re = N_new PCZLight(sid);
        re->setFactory(this);
        re->setManager(mag);
		return re;
	}
	//-----------------------------------------------------------------------
	void PCZLightFactory::destroy(SpaceObj * obj)
	{
		N_delete obj;
	}
    //-----------------------------------------------------------------------

} // Namespace
