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

#include "OgrePCZone.h"
#include "OgreSceneNode.h"
#include "OgrePortal.h"
#include "OgrePCZSceneNode.h"
#include "OgrePCZSceneManager.h"

namespace Ogre
{

    PCZone::PCZone( PCZSceneManager * creator, const String& name ) 
    {
        mLastVisibleFrame = 0;
		mLastVisibleFromCamera = 0;
		mName = name;
		mZoneTypeName = "ZoneType_Undefined";
		mEnclosureNode = 0;
		mPCZSM = creator;
		mHasSky = false;
	}

	PCZone::~PCZone()
	{
	}

	/** Remove all nodes from the node reference list and clear it
	*/
	void PCZone::_clearNodeLists(short type)
	{
		if (type & HOME_NODE_LIST)
		{
			mHomeNodeList.clear();
		}
		if (type & VISITOR_NODE_LIST)
		{
			mVisitorNodeList.clear();
		}
	}

	Portal * PCZone::findMatchingPortal(Portal * portal)
	{
		// look through all the portals in zone2 for a match
		Portal* portal2;
		PortalList::iterator pi2, piend2;
		piend2 = mPortals.end();
		for (pi2 = mPortals.begin(); pi2 != piend2; pi2++)
		{
			portal2 = *pi2;
			//portal2->updateDerivedValues();
			if (portal2->getTargetZone() == 0 && portal2->closeTo(portal) &&
				portal2->getSpaceDirection().dotProduct(portal->getSpaceDirection()) < -0.9)
			{
				// found a match!
				return portal2;
			}
		}
		// no match
		return 0;
	}

	/* Add a portal to the zone */
	void PCZone::_addPortal(Portal * newPortal)
	{
		if (newPortal)
		{
			// make sure portal is unique (at least in this zone)
			PortalList::iterator it = std::find(mPortals.begin(), mPortals.end(), newPortal);
			if (it != mPortals.end())
			{
				N_EXCEPT(
					Exception::ERR_DUPLICATE_ITEM,
					"A portal with the name " + newPortal->getName() + " already exists");
			}

			// add portal to portals list
			mPortals.push_back(newPortal);

			// tell the portal which zone it's currently in
			newPortal->setCurrentHomeZone(this);
		}
	}

	/* Remove a portal from the zone (does not erase the portal object, just removes reference) */
	void PCZone::_removePortal(Portal * removePortal)
	{
		if (removePortal)
		{
			mPortals.erase(std::find(mPortals.begin(), mPortals.end(), removePortal));
		}
	}

	/* Add an anti portal to the zone */
	void PCZone::_addAntiPortal(AntiPortal* newAntiPortal)
	{
		if (newAntiPortal)
		{
			// make sure portal is unique (at least in this zone)
			AntiPortalList::iterator it = std::find(mAntiPortals.begin(), mAntiPortals.end(), newAntiPortal);
			if (it != mAntiPortals.end())
			{
				N_EXCEPT(
					Exception::ERR_DUPLICATE_ITEM,
					"An anti portal with the name " + newAntiPortal->getName() + " already exists",
					"PCZone::_addAntiPortal" );
			}

			// add portal to portals list
			mAntiPortals.push_back(newAntiPortal);

			// tell the portal which zone it's currently in
			newAntiPortal->setCurrentHomeZone(this);
		}
	}

	/* Remove an anti portal from the zone */
	void PCZone::_removeAntiPortal(AntiPortal* removeAntiPortal)
	{
		if (removeAntiPortal)
		{
			mAntiPortals.erase(std::find(mAntiPortals.begin(), mAntiPortals.end(), removeAntiPortal));
		}
	}

	/* create node specific zone data if necessary
	*/
	void PCZone::createNodeZoneData(PCZSceneNode *)
	{
	}

	/* get the aabb of the zone - default implementation
	   uses the enclosure node, but there are other perhaps
	   better ways
	*/
	void PCZone::getAABB(AABox & aabb)
	{
		// if there is no node, just return a null box
		if (mEnclosureNode == 0)
		{
			aabb.setNull();
		}
		else
		{
			aabb = mEnclosureNode->getSpaceAABB();
			// since this is the "local" AABB, subtract out any translations
			aabb.setMin(aabb.getMin() - mEnclosureNode->getSpacePos());
			aabb.setMax(aabb.getMax() - mEnclosureNode->getSpacePos());
		}
		return;
	}

	/***********************************************************************\
	ZoneData - Zone-specific Data structure for Scene Nodes
    ************************************************************************/

	ZoneData::ZoneData(PCZSceneNode * node, PCZone * zone)
	{
		mAssociatedZone = zone;
		mAssociatedNode = node;
	}

	ZoneData::~ZoneData()
	{
	}

	void ZoneData::update(void)
	{
	}
}
