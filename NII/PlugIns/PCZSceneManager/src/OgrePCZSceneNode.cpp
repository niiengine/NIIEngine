/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
PCZSceneNode.cpp  -  PCZ-specific version of SpaceNode

The PCZSceneNode is an extension used to store zone information and provide
additional functionality for a given Ogre::SpaceNode.  A PCZSceneNode contains a
pointer to the home zone for the node and a list of all zones being visited by
the node.  The PCZSceneManager contains a STD::MAP of PCZSceneNodes which are
keyed by the name of each node.  This allows quick lookup of
a given scenenode's PCZSceneNode by the scene manager.

-----------------------------------------------------------------------------
begin                : Sat Mar 24 2007
author               : Eric Cha
email                : ericc@xenopi.com
TODO				 : None known
-----------------------------------------------------------------------------
*/

#include <OgreRoot.h>
#include "OgrePCZSceneNode.h"
#include "OgreSceneNode.h"
#include "OgrePCZSceneManager.h"
#include "OgrePortal.h"
#include "OgrePCZone.h"
#include "OgrePCZCamera.h"

namespace Ogre
{
	PCZSceneNode::PCZSceneNode( SpaceManager * creator )
		: SpaceNode( creator ),
		mHomeZone(0),
		mAnchored(false),
		mAllowedToVisit(true),
		mLastVisibleFrame(0),
		mLastVisibleFromCamera(0),
		mEnabled(true),
		mMoved(false)
	{
	}

	PCZSceneNode::PCZSceneNode(SpaceManager * creator, const String & name)
		: SpaceNode( creator, name ),
		mHomeZone(0),
		mAnchored(false),
		mAllowedToVisit(true),
		mLastVisibleFrame(0),
		mLastVisibleFromCamera(0),
		mEnabled(true),
		mMoved(false)
	{
	}
    //-----------------------------------------------------------------------
	PCZSceneNode::~PCZSceneNode()
	{
		// clear visiting zones list
		mVisitingZones.clear();

		// delete zone data
		ZoneData * zoneData;
		ZoneDataMap::iterator it = mZoneData.begin();

		while(it != mZoneData.end())
		{
			zoneData = it->second;
			N_delete zoneData;
			++it;
		}
		mZoneData.clear();
	}
    //-----------------------------------------------------------------------
	void PCZSceneNode::update(bool parentHasChanged, bool updateChildren)
	{
		PosNode::update(parentHasChanged, updateChildren);
		if(mParent)
            updateSpaceAABB(); // skip bound update if it's root scene node. Saves a lot of CPU.

		mPrevPosition = mNewPosition;
		mNewPosition = mSpacePos;
	}
    //-----------------------------------------------------------------------
	void PCZSceneNode::updateFromParentImpl() const
	{
		SpaceNode::updateFromParentImpl();
		mMoved = true;
	}
    //-----------------------------------------------------------------------
	SpaceNode * PCZSceneNode::create(const Vector3 & pos,
        const Quaternion & rot)
	{
		PCZSceneNode * c = (PCZSceneNode *)(create(pos, rot));
		if(mHomeZone)
		{
			c->setHomeZone(mHomeZone);
			mHomeZone->_addNode(c);
		}
		return static_cast<SpaceNode*>(c);
	}
    //-----------------------------------------------------------------------
    SpaceNode * PCZSceneNode::create(SpaceID sid, const Vector3 & pos, const Quaternion & rot)
	{
		PCZSceneNode * c = (PCZSceneNode *)(create(sid, pos, rot));
		if(mHomeZone)
		{
			c->setHomeZone(mHomeZone);
			mHomeZone->_addNode(c);
		}
		return static_cast<SpaceNode*>(c);
	}

	PCZone* PCZSceneNode::getHomeZone(void)
	{
		return mHomeZone;
	}
	void PCZSceneNode::setHomeZone(PCZone * zone)
	{
		// if the new home zone is different than the current, remove
		// the node from the current home zone's list of home nodes first
		if (zone != mHomeZone && mHomeZone)
		{
			mHomeZone->removeNode(this);
		}
		mHomeZone = zone;
	}
	void PCZSceneNode::anchorToHomeZone(PCZone * zone)
	{
		mHomeZone = zone;
		if (zone)
		{
			mAnchored = true;
		}
		else
		{
			mAnchored = false;
		}
	}
	void PCZSceneNode::addZoneToVisitingZonesMap(PCZone * zone)
	{
		mVisitingZones[zone->getName()] = zone;
	}
	void PCZSceneNode::clearVisitingZonesMap(void)
	{
		mVisitingZones.clear();
	}
	/* The following function does the following:
	 * 1) Remove references to the node from zones the node is visiting
	 * 2) Clear the node's list of zones it is visiting
	 */
	void PCZSceneNode::clearNodeFromVisitedZones( void )
	{
		if (mVisitingZones.size() > 0)
		{
			// first go through the list of zones this node is visiting
			// and remove references to this node
			PCZone* zone;
			ZoneMap::iterator it = mVisitingZones.begin();

			while ( it != mVisitingZones.end() )
			{
				zone = it->second;
				zone->removeNode(this);
				++it;
			}

			// second, clear the visiting zones list
			mVisitingZones.clear();

		}
	}

	/* Remove all references that the node has to the given zone
	*/
	void PCZSceneNode::removeReferencesToZone(PCZone * zone)
	{
		if (mHomeZone == zone)
		{
			mHomeZone = 0;
		}
		// search the map of visiting zones and remove
		ZoneMap::iterator i;
		i = mVisitingZones.find(zone->getName());
		if (i != mVisitingZones.end())
		{
			mVisitingZones.erase(i);
		}
	}

	/* returns true if zone is in the node's visiting zones map
	   false otherwise.
	*/
	bool PCZSceneNode::isVisitingZone(PCZone * zone)
	{
		ZoneMap::iterator i;
		i = mVisitingZones.find(zone->getName());
		if(i != mVisitingZones.end())
		{
			return true;
		}
		return false;
	}

    /** Adds the attached objects of this PCZSceneNode into the queue. */
    void PCZSceneNode::_addToRenderQueue(Camera * cam, RenderQueue * queue,
        bool onlyShadowCasters, RenderBound * visibleBounds)
    {
		AttachList::iterator mit = mAttachList.begin();

		while(mit != mAttachList.end())
		{
			SpaceObj * mo = mit->second;

			mo->_notify(cam);
			if(mo->isVisible() && (!onlyShadowCasters || mo->isCastShadow()))
			{
				mo -> _update(queue);

			    if(cam)
			    {
				    cam->expandBound(mo->getSpaceAABB(true), mo->getSpaceBS(true));
			    }
            }
			++mit;
        }
    }

	/** Save the node's current position as the previous position
	*/
	void PCZSceneNode::savePrevPosition(void)
	{
		mPrevPosition = getSpacePos();
	}

	void PCZSceneNode::setZoneData(PCZone * zone, ZoneData * zoneData)
	{

		// first make sure that the data doesn't already exist
		if (mZoneData.find(zone->getName()) != mZoneData.end())
		{
			N_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "A ZoneData associated with zone " +
                zone->getName() + " already exists");
		}
		mZoneData[zone->getName()] = zoneData;
	}

	// get zone data for this node for given zone
	// NOTE: This routine assumes that the zone data is present!
	ZoneData* PCZSceneNode::getZoneData(PCZone * zone)
	{
		return mZoneData[zone->getName()];
	}

	// update zone-specific data for any zone that the node is touching
	void PCZSceneNode::updateZoneData(void)
	{
		ZoneData* zoneData;
		PCZone * zone;
		// make sure home zone data is updated
		zone = mHomeZone;
		if (zone->requiresZoneSpecificNodeData())
		{
			zoneData = getZoneData(zone);
			zoneData->update();
		}
		// update zone data for any zones visited
		ZoneMap::iterator it = mVisitingZones.begin();
		while ( it != mVisitingZones.end() )
		{
			zone = it->second;
			if (zone->requiresZoneSpecificNodeData())
			{
				zoneData = getZoneData(zone);
				zoneData->update();
			}
			++it;
		}
	}
}
