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

#include "OgrePCZSceneManager.h"
#include "OgrePCZSpaceQuery.h"
#include "OgrePCZone.h"
#include "OgrePCZCamera.h"
#include "OgrePCZLight.h"
#include "OgrePortal.h"
#include "OgreSceneNode.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreLogManager.h"
#include <OgreRenderSystem.h>
#include "OgreRoot.h"

namespace Ogre
{
	PCZSceneManager::PCZSceneManager(const String & name) :
        SpaceManager(name),
        mDefaultZoneTypeName("ZoneType_Default"),
        mDefaultZoneFileName("none"),
        mLastActiveCamera(0),
        mZoneFactoryManager(0),
        mActiveCameraZone(0),
        mDefaultZone(0),
        mShowPortals(false)
	{
        mType = PCZSceneManagerFactory::FACTORY_TYPE_NAME;;
    }

    PCZSceneManager::~PCZSceneManager()
    {
        // we don't delete the root scene node here because the
        // base scene manager class does that.

		// delete ALL portals
		Portal * p;
		PortalList::iterator i = mPortals.begin();
		for(i = mPortals.begin(); i != mPortals.end(); i++)
		{
			p = *i;
			N_delete p;
		}
		mPortals.clear();

		// delete all the zones
		for (ZoneMap::iterator j = mZones.begin();
			j != mZones.end(); ++j)
		{
			N_delete j->second;
		}
		mZones.clear();
        mDefaultZone = 0;
    }

    void PCZSceneManager::init( const String &defaultZoneTypeName,
								const String &filename)
    {

		// delete ALL portals
		Portal * p;
		PortalList::iterator i = mPortals.begin();
		for (i = mPortals.begin(); i != mPortals.end(); i++)
		{
			p = *i;
			N_delete p;
		}
		mPortals.clear();

		// delete all the zones
		for (ZoneMap::iterator j = mZones.begin();
			j != mZones.end(); ++j)
		{
			N_delete j->second;
		}
		mZones.clear();

        mFrameCount = 0;

		mDefaultZoneTypeName = defaultZoneTypeName;
		mDefaultZoneFileName = filename;

        // create a new default zone
		mZoneFactoryManager = PCZoneFactoryManager::getSingletonPtr();
		mDefaultZone = createZoneFromFile(mDefaultZoneTypeName, "Default_Zone", (PCZSceneNode*)getOriginNode(), mDefaultZoneFileName);
    }

	// Create a portal instance
	Portal* PCZSceneManager::createPortal(const String& name, PortalBase::PORTAL_TYPE type)
	{
		Portal* newPortal = N_new Portal(name, type);
		newPortal->setFactory(Root::getSingleton().getFactory("Portal"));
		newPortal->setManager(this);
		mPortals.push_front(newPortal);
		return newPortal;
	}

	// delete a portal instance by pointer
	void PCZSceneManager::destroyPortal(Portal * p)
	{
		// remove the portal from it's target portal
		Portal * targetPortal = p->getTargetPortal();
		if (targetPortal)
		{
			targetPortal->setTargetPortal(0); // the targetPortal will still have targetZone value, but targetPortal will be invalid
		}
		// remove the Portal from it's home zone
		PCZone * homeZone = p->getCurrentHomeZone();
		if (homeZone)
		{
			// inform zone of portal change. Do here since PCZone is abstract
			homeZone->setPortalsUpdated(true);
			homeZone->_removePortal(p);
		}

		// remove the portal from the master portal list
		PortalList::iterator it = std::find( mPortals.begin(), mPortals.end(), p );
		if (it != mPortals.end())
		{
			mPortals.erase(it);
		}
		// delete the portal instance
		N_delete p;
	}

	// delete a portal instance by pointer
	void PCZSceneManager::destroyPortal(const String & portalName)
	{
		// find the portal from the master portal list
		Portal * p;
		Portal * thePortal = 0;
		PortalList::iterator it = mPortals.begin();
		while (it != mPortals.end())
		{
			p = *it;
			if (p->getName() == portalName)
			{
				thePortal = p;
				// erase entry in the master list
				mPortals.erase(it);
				break;
			}
			it++;
		}
		if (thePortal)
		{
			// remove the portal from it's target portal
			Portal * targetPortal = thePortal->getTargetPortal();
			if (targetPortal)
			{
				targetPortal->setTargetPortal(0); // the targetPortal will still have targetZone value, but targetPortal will be invalid
			}
			// remove the Portal from it's home zone
			PCZone * homeZone = thePortal->getCurrentHomeZone();
			if (homeZone)
			{
				// inform zone of portal change
				homeZone->setPortalsUpdated(true);
				homeZone->_removePortal(thePortal);
			}

			// delete the portal instance
			N_delete thePortal;
		}
	}

	/** Create a new anti portal instance */
	AntiPortal * PCZSceneManager::createAntiPortal(const String& name, PortalBase::PORTAL_TYPE type)
	{
		AntiPortal * newAntiPortal = N_new AntiPortal(name, type);
		newAntiPortal->setFactory(Root::getSingleton().getFactory("AntiPortal"));
		newAntiPortal->setManager(this);
		mAntiPortals.push_front(newAntiPortal);
		return newAntiPortal;
	}

	/** Delete a anti portal instance by pointer */
	void PCZSceneManager::destroyAntiPortal(AntiPortal * p)
	{
		// remove the Portal from it's home zone
		PCZone * homeZone = p->getCurrentHomeZone();
		if(homeZone)
		{
			// inform zone of portal change. Do here since PCZone is abstract
			homeZone->setPortalsUpdated(true);
			homeZone->_removeAntiPortal(p);
		}

		// remove the portal from the master portal list
		AntiPortalList::iterator it = std::find(mAntiPortals.begin(), mAntiPortals.end(), p);
		if (it != mAntiPortals.end()) mAntiPortals.erase(it);

		// delete the portal instance
		N_delete p;
	}

	/** Delete a anti portal instance by name */
	void PCZSceneManager::destroyAntiPortal(const String& portalName)
	{
		// find the anti portal from the master portal list
		AntiPortal* p;
		AntiPortal* thePortal = 0;
		AntiPortalList::iterator it = mAntiPortals.begin();
		while (it != mAntiPortals.end())
		{
			p = *it;
			if (p->getName() == portalName)
			{
				thePortal = p;
				// erase entry in the master list
				mAntiPortals.erase(it);
				break;
			}
			it++;
		}
		if (thePortal)
		{
			// remove the Portal from it's home zone
			PCZone* homeZone = thePortal->getCurrentHomeZone();
			if (homeZone)
			{
				// inform zone of portal change
				homeZone->setPortalsUpdated(true);
				homeZone->_removeAntiPortal(thePortal);
			}

			// delete the portal instance
			N_delete thePortal;
		}
	}

	/** Create a zone from a file (type of file
		* depends on the zone type
		* ZoneType_Default uses an Ogre Model (.mesh) file
		* ZoneType_Octree uses an Ogre Model (.mesh) file
		* ZoneType_Terrain uses a Terrain.CFG file
		*/
	PCZone * PCZSceneManager::createZoneFromFile(const String &zoneTypeName,
												 const String &zoneName,
												 PCZSceneNode * parentNode,
												 const String &filename)
	{
		PCZone * newZone;

        // create a new default zone
		newZone = mZoneFactoryManager->createPCZone(this, zoneTypeName, zoneName);
		// add to the global list of zones
		mZones[newZone->getName()] = newZone;
		if (filename != "none")
		{
			// set the zone geometry
			newZone->setZoneGeometry(filename, parentNode);
		}

		return newZone;
	}

	/* Get a zone by name */
	PCZone * PCZSceneManager::getZoneByName(const String & zoneName)
	{
        PCZone * zone;
		ZoneMap::iterator i = mZones.find(zoneName);
		if(i != mZones.end())
		{
			zone = i->second;
			return zone;
		}
		return 0; // couldn't find the zone
	}

	void PCZSceneManager::setZoneGeometry(const String & zoneName,
        PCZSceneNode * parentNode, const String & filename)
	{
        PCZone * zone;
		ZoneMap::iterator i = mZones.find(zoneName);
		if(i != mZones.end())
		{
			zone = i->second;
			zone->setZoneGeometry( filename, parentNode );
			return;
		}

	}

	SpaceNode* PCZSceneManager::createNodeImpl(void)
	{
		return N_new PCZSceneNode(this);
	}

	SpaceNode* PCZSceneManager::createNodeImpl(SpaceID sid)
	{
        if(sid == 0)
            return N_new PCZSceneNode(this);
		return N_new PCZSceneNode(this, sid);
	}

    SpaceNode * PCZSceneManager::createSpaceNode(SpaceID name)
    {
        // Check name not used
        if(mSceneNodes.find(name) != mSceneNodes.end())
        {
            N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
                "A scene node with the name " + name + " already exists");
        }
        SpaceNode * on = createNodeImpl( name );
        mSceneNodes[ on->getID() ] = on;
		// create any zone-specific data necessary
		createZoneSpecificNodeData((PCZSceneNode*)on);
		// return pointer to the node
        return on;
    }

    // Create a camera for the scene
    Camera * PCZSceneManager::createCamera(const String & name)
    {
		// Check name not used
		if (mCameraList.find(name) != mCameraList.end())
		{
			N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
				"A camera with the name " + name + " already exists");
		}

        Camera * c = N_new PCZCamera(name, this);
        mCameraList.insert(CameraMap::value_type(name, c));

		// tell all the zones about the new camera
		ZoneMap::iterator i, iend = mZones.end();
		PCZone * zone;
		for(i = mZones.begin(); i != iend; i++)
		{
			zone = i->second;
	        zone->notifyCameraCreated(c);
		}

        return c;
    }

    // Destroy a Scene PosNode by name.
    void PCZSceneManager::destroySpaceNode(SpaceID name)
    {
        SpaceNode * on = (getSpaceNode(name));

        if(on != 0)
		{
            // destroy the node
            if(on != 0)
            {
                // remove references to the node from zones
                removeSceneNode(on);

                // destroy the node
                SpaceManager::destroySpaceNode(on->getID());
            }
		}
    }
	//-----------------------------------------------------------------------
	void PCZSceneManager::destroyAllCreateObj(void)
	{
		destroyAllStaticSpace();
		destroyAllSpace();

		// Clear root node of all children
		getOriginNode()->removeAll();
		getOriginNode()->detachAll();

		// Delete all SceneNodes, except root that is
		for (SceneNodeList::iterator i = mSceneNodes.begin();
			i != mSceneNodes.end(); ++i)
		{
			N_delete i->second;
		}
		mSceneNodes.clear();
		mAutoFocusList.clear();

		// delete all the zones
		for (ZoneMap::iterator j = mZones.begin();
			j != mZones.end(); ++j)
		{
			N_delete j->second;
		}
		mZones.clear();
	    mDefaultZone = 0;

		// Clear animations
		destroyAllAnimations();

		// Remove sky nodes since they've been deleted
		mSkyBoxNode = mSkyPlaneNode = mSkyDomeNode = 0;
		mSkyBoxEnabled = mSkyPlaneEnabled = mSkyDomeEnabled = false;

		// Clear render queue, empty completely
		if (mRenderQueue)
			mRenderQueue->clear(true);

		// re-initialize
        init(mDefaultZoneTypeName, mDefaultZoneFileName);
	}

	/** Overridden from SpaceManager */
	void PCZSceneManager::setSceneGeoRenderGroup(uint8 qid)
	{
		// tell all the zones about the new WorldGeometryRenderQueue
		PCZone * zone;
        ZoneMap::iterator i;
		for (i = mZones.begin(); i != mZones.end(); i++)
		{
			zone = i->second;
	        zone->notifyWorldGeometryRenderQueue( qid );
		}
		// call the regular scene manager version
		SpaceManager::setSceneGeoRenderGroup(qid);
	}

	/** Overridden from SpaceManager */
    void PCZSceneManager::renderSpace(Camera * cam, Viewport * vp)
    {
		// notify all the zones that a scene render is starting
        PCZone * zone;
		ZoneMap::iterator i, iend = mZones.end();
		for(i = mZones.begin(); i != iend; ++i)
		{
			zone = i->second;
	        zone->notifyBeginRenderScene();
		}
		// do the regular renderSpace
        SpaceManager::renderSpace(cam, vp);
    }

	/* enable/disable sky rendering */
	void PCZSceneManager::enableSky(bool onoff)
	{
		if (mSkyBoxNode)
		{
			mSkyBoxEnabled = onoff;
		}
		else if (mSkyDomeNode)
		{
			mSkyDomeEnabled = onoff;
		}
		else if (mSkyPlaneNode)
		{
			mSkyPlaneEnabled = onoff;
		}
	}

	/* Set the zone which contains the sky node */
	void PCZSceneManager::setSkyZone(PCZone * zone)
	{
		if (zone == 0)
		{
			// if no zone specified, use default zone
			zone = mDefaultZone;
		}
		if (mSkyBoxNode)
		{
			((PCZSceneNode*)mSkyBoxNode)->setHomeZone(zone);
			((PCZSceneNode*)mSkyBoxNode)->anchorToHomeZone(zone);
			zone->setHasSky(true);
		}
		if (mSkyDomeNode)
		{
			((PCZSceneNode*)mSkyDomeNode)->setHomeZone(zone);
			((PCZSceneNode*)mSkyDomeNode)->anchorToHomeZone(zone);
			zone->setHasSky(true);
		}
		if (mSkyPlaneNode)
		{
			((PCZSceneNode*)mSkyPlaneNode)->setHomeZone(zone);
			((PCZSceneNode*)mSkyPlaneNode)->anchorToHomeZone(zone);
			zone->setHasSky(true);
		}

	}

	//-----------------------------------------------------------------------
	// THIS IS THE MAIN LOOP OF THE MANAGER
	//-----------------------------------------------------------------------
	/* updateSpace does several things now:
	   1) standard scene graph update (transform all nodes in the node tree)
	   2) update the spatial data for all zones (& portals in the zones)
	   3) Update the PCZSNMap entry for every scene node
	*/
	void PCZSceneManager::updateSpace( Camera * cam )
	{
		// First do the standard scene graph update
		SpaceManager::updateSpace( cam );
		// check for portal zone-related changes (portals intersecting other portals)
		_updatePortalZoneData();
		// mark nodes dirty base on portals that changed.
		_dirtyNodeByMovingPortals();
		// update all scene nodes
		_updatePCZSceneNodes();
		// calculate zones affected by each light
		_calcZonesAffectedByLights(cam);
		// clear update flags at end so user triggered updated are
		// not cleared prematurely
		_clearAllZonesPortalUpdateFlag();
	}

	/** Update the zone data for every zone portal in the scene */

	void PCZSceneManager::_updatePortalZoneData(void)
	{
		PCZone * zone;
	    ZoneMap::iterator zit = mZones.begin();

	    while ( zit != mZones.end() )
	    {
		    zone = zit->second;
			// this callchecks for portal zone changes & applies zone data changes as necessary
			zone->updatePortalsZoneData();
			// proceed to next zone in the list
		    ++zit;
	    }
	}

	/** Mark nodes dirty for every zone with moving portal in the scene */
	void PCZSceneManager::_dirtyNodeByMovingPortals(void)
	{
		PCZone * zone;
		ZoneMap::iterator zit = mZones.begin();

		while ( zit != mZones.end() )
		{
			zone = zit->second;
			// this call mark nodes dirty base on moving portals
			zone->dirtyNodeByMovingPortals();
			// proceed to next zone in the list
			++zit;
		}
	}

	/* Update all PCZSceneNodes.
	*/
	void PCZSceneManager::_updatePCZSceneNodes(void)
	{
		SceneNodeList::iterator it = mSceneNodes.begin();
		PCZSceneNode * pczsn;

		while ( it != mSceneNodes.end() )
		{
			pczsn = (PCZSceneNode*)(it->second);
			if (pczsn->isMoved() && pczsn->isEnabled())
			{
				// Update a single entry
				_updatePCZSceneNode(pczsn);

				// reset moved state.
				pczsn->setMoved(false);
			}
			// proceed to next entry in the list
			++it;
		}
	}

    /*
    */
    void PCZSceneManager::_calcZonesAffectedByLights(Camera * cam)
    {
        SpaceMap & lights = getSpaceList(PCZLightFactory::FACTORY_TYPE_NAME);
	    {
            SpaceMap::iterator i, iend = lights.end();
            for(i = lights.begin(); i != iend; ++i)
		    {
			    PCZLight* l = static_cast<PCZLight*>(i->second);
				if(l->getNeedsUpdate())
				{
					// only update if necessary
					l->updateZones(((PCZSceneNode*)(cam->getSceneNode()))->getHomeZone(), mFrameCount);
				}
				// clear update flag
				l->clearNeedsUpdate();
            }
        }
    }

	//-----------------------------------------------------------------------
    // Update all the zone info for a given node.  This function
	// makes sure the home zone of the node is correct, and references
	// to any zones it is visiting are added and a reference to the
	// node is added to the visitor lists of any zone it is visiting.
	//
	void PCZSceneManager::_updatePCZSceneNode(PCZSceneNode * pczsn)
    {
	    // Skip if root Zone has been destroyed (shutdown conditions)
	    if (!mDefaultZone)
		    return;

		// Skip if the node is the sceneroot node
		if (pczsn == getOriginNode())
			return;

		// clear all references to visiting zones
		pczsn->clearNodeFromVisitedZones();

        // Find the current home zone of the node associated with the pczsn entry.
		_updateHomeZone( pczsn, false );

		/* The following function does the following:
		* 1) Check all portals in the home zone - if the node is touching the portal
		*    then add the node to the connected zone as a visitor
		* 2) Recurse into visited zones in case the node spans several zones
		*/
		// (recursively) check each portal of home zone to see if the node is touching
		if (pczsn->getHomeZone() &&
			pczsn->allowedToVisit() == true)
		{
			pczsn->getHomeZone()->_checkNodeAgainstPortals(pczsn, 0);
		}

		// update zone-specific data for the node for any zones that require it
		pczsn->updateZoneData();
    }

    /** Removes all references to the node from every zone in the scene.
    */
    void PCZSceneManager::removeSceneNode( SpaceNode * sn )
    {
	    // Skip if mDefaultZone has been destroyed (shutdown conditions)
	    if (!mDefaultZone)
		    return;

		PCZSceneNode * pczsn = (PCZSceneNode*)sn;

		// clear all references to the node in visited zones
		pczsn->clearNodeFromVisitedZones();

        // tell the node it's not in a zone
        pczsn->setHomeZone(0);
    }

	// Set the home zone for a node
	void PCZSceneManager::addPCZSceneNode(PCZSceneNode * sn, PCZone * homeZone)
	{
		// set the home zone
		sn->setHomeZone(homeZone);
		// add the node
		homeZone->_addNode(sn);
	}

	//-----------------------------------------------------------------------
	/* Create a zone with the given name and parent zone */
	PCZone * PCZSceneManager::createZone(const String& zoneType, const String& instanceName)
	{
		if (mZones.find(instanceName) != mZones.end())
		{
			N_EXCEPT(
				Exception::ERR_DUPLICATE_ITEM,
				"A zone with the name " + instanceName + " already exists",
				"PCZSceneManager::createZone" );
		}
		PCZone * newZone = mZoneFactoryManager->createPCZone(this, zoneType, instanceName);
		if (newZone)
		{
			// add to the global list of zones
			mZones[instanceName] = newZone;
		}
		if (newZone->requiresZoneSpecificNodeData())
		{
			createZoneSpecificNodeData(newZone);
		}
		return newZone;
	}

	/* destroy an existing zone within the scene */
	/* if destroySceneNodes is true, then all nodes which have the destroyed
	   zone as their homezone are desroyed too.  If destroySceneNodes is false
	   then all scene nodes which have the zone as their homezone will have
	   their homezone pointer set to 0, which will allow them to be re-assigned
	   either by the user or via the automatic re-assignment routine */
	void PCZSceneManager::destroyZone(PCZone * zone, bool destroySceneNodes)
	{
		// need to remove this zone from all lights affected zones list,
		// otherwise next frame _calcZonesAffectedByLights will call PCZLight::getNeedsUpdate()
		// which will try to access the zone pointer and will cause an access violation
		SpaceMap & lights = getSpaceList(PCZLightFactory::FACTORY_TYPE_NAME);
		{
            SpaceMap::iterator m, mend = lights.end();
            for(m = lights.begin(); m != mend; ++m)
			{
				PCZLight * l = static_cast<PCZLight*>(m->second);
				if(l)
				{
					// no need to check, this function does that anyway. if exists, is erased.
					l->removeZoneFromAffectedZonesList(zone);
				}
			}
		}
		// if not destroying scene nodes, then make sure any nodes who have
		// this zone as homezone are set to have 0 for a homezone
		for (SceneNodeList::iterator i = mSceneNodes.begin();
			i != mSceneNodes.end(); ++i)
		{
			PCZSceneNode * pczsn = (PCZSceneNode*)(i->second);
			if (!destroySceneNodes)
			{
				if (pczsn->getHomeZone() == zone)
				{
					pczsn->setHomeZone(0);
				}
			}
			// reset all node visitor lists
			// note, it might be more efficient to only do this to nodes which
			// are actually visiting the zone being destroyed, but visitor lists
			// get cleared every frame anyway, so it's not THAT big a deal.
			pczsn->clearNodeFromVisitedZones();
		}

		ZoneMap::iterator it;
		it = mZones.find(zone->getName());
		if (it != mZones.end())
		{
			mZones.erase(zone->getName());
		}
		N_delete zone;
	}

    /* The following function checks if a node has left it's current home zone.
	* This is done by checking each portal in the zone.  If the node has crossed
	* the portal, then the current zone is no longer the home zone of the node.  The
	* function then recurses into the connected zones.  Once a zone is found where
	* the node does NOT cross out through a portal, that zone is the new home zone.
    * When this function is done, the node should have the correct home zone already
	* set.  A pointer is returned to this zone as well.
	*
	* NOTE: If the node does not have a home zone when this function is called on it,
	*       the function will do its best to find the proper zone for the node using
	*       bounding box volume testing.  This CAN fail to find the correct zone in
	*		some scenarios, so it is best for the user to EXPLICITLY set the home
	*		zone of the node when the node is added to the scene using
	*       PCZSceneNode::setHomeZone()
	*/
    void PCZSceneManager::_updateHomeZone( PCZSceneNode * pczsn, bool allowBackTouches )
    {
	    // Skip if root PCZoneTree has been destroyed (shutdown conditions)
	    if (!mDefaultZone)
		    return;

		PCZone * startzone;
		PCZone * newHomeZone;

		// start with current home zone of the node
		startzone = pczsn->getHomeZone();

		if (startzone)
		{
			if (!pczsn->isAnchored())
			{
				newHomeZone = startzone->updateNodeHomeZone(pczsn, false);
			}
			else
			{
				newHomeZone = startzone;
			}

			if (newHomeZone != startzone)
			{
				// add the node to the home zone
				newHomeZone->_addNode(pczsn);
			}
		}
		else
		{
			// the node hasn't had it's home zone set yet, so do our best to
			// find the home zone using volume testing.
			Vector3 nodeCenter = pczsn->getSpacePos();
			PCZone * bestZone = findZoneForPoint(nodeCenter);
			// set the best zone as the node's home zone
			pczsn->setHomeZone(bestZone);
			// add the node to the zone
			bestZone->_addNode(pczsn);
		}

		return;

    }

	/* Find the best (smallest) zone that contains a point
	*/
	PCZone * PCZSceneManager::findZoneForPoint(Vector3 & point)
	{
		PCZone * zone;
		PCZone * bestZone = mDefaultZone;
		NIIf bestVolume = Ogre::Math::POS_INFINITY;

		ZoneMap::iterator zit = mZones.begin();

		while ( zit != mZones.end() )
		{
			zone = zit->second;
			AABox aabb;
			zone->getAABB(aabb);
			SpaceNode * enclosureNode = zone->getEnclosureNode();
			if (enclosureNode != 0)
			{
				// since this is the "local" AABB, add in world translation of the enclosure node
				aabb.setMin(aabb.getMin() + enclosureNode->getSpacePos());
				aabb.setMax(aabb.getMax() + enclosureNode->getSpacePos());
			}
			if (aabb.intersects(point))
			{
				if (aabb.volume() < bestVolume)
				{
					// this zone is "smaller" than the current best zone, so make it
					// the new best zone
					bestZone = zone;
					bestVolume = aabb.volume();
				}
			}
			// proceed to next zone in the list
			++zit;
		}
		return bestZone;
	}

	// create any zone-specific data necessary for all zones for the given node
	void PCZSceneManager::createZoneSpecificNodeData(PCZSceneNode * node)
	{
		ZoneMap::iterator i;
		PCZone * zone;
		for (i = mZones.begin(); i != mZones.end(); i++)
		{
			zone = i->second;
			if (zone->requiresZoneSpecificNodeData())
			{
				zone->createNodeZoneData(node);
			}
		}
	}

	// create any zone-specific data necessary for all nodes for the given zone
	void PCZSceneManager::createZoneSpecificNodeData(PCZone * zone)
	{
		SceneNodeList::iterator it = mSceneNodes.begin();
		PCZSceneNode * pczsn;
		if (zone->requiresZoneSpecificNodeData())
		{
			while ( it != mSceneNodes.end() )
			{
				pczsn = (PCZSceneNode*)(it->second);
				// create zone specific data for the node
				zone->createNodeZoneData(pczsn);
				// proceed to next entry in the list
				++it;
			}
		}
	}

	// set the home zone for a scene node
	void PCZSceneManager::setNodeHomeZone(SpaceNode *node, PCZone *zone)
	{
		// cast the Ogre::SpaceNode to a PCZSceneNode
		PCZSceneNode * pczsn = (PCZSceneNode*)node;
		pczsn->setHomeZone(zone);
	}

	// (optional) post processing for any scene node found visible for the frame
    void PCZSceneManager::_alertVisibleObjects( void )
    {
        N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED,
            "Function doesn't do as advertised",
            "PCZSceneManager::_alertVisibleObjects" );

//        NodeList::iterator it = mVisible.begin();
//
//        while ( it != mVisible.end() )
//        {
//            SpaceNode * node = *it;
//            // this is where you would do whatever you wanted to the visible node
//            // but right now, it does nothing.
//            ++it;
//        }
    }

    //-----------------------------------------------------------------------
    Light* PCZSceneManager::createLight(SpaceID sid)
    {
	    return static_cast<Light *>(
		    createSpace(sid, PCZLightFactory::FACTORY_TYPE_NAME));
    }
	//-----------------------------------------------------------------------
	Light* PCZSceneManager::getLight(SpaceID sid) const
	{
		return static_cast<Light *>(
			getSpace(sid, PCZLightFactory::FACTORY_TYPE_NAME));
	}
	//-----------------------------------------------------------------------
	bool PCZSceneManager::isLightExist(SpaceID sid) const
	{
		return isSpaceExist(sid, PCZLightFactory::FACTORY_TYPE_NAME);
	}
	//-----------------------------------------------------------------------
	void PCZSceneManager::destroyLight(SpaceID sid)
	{
		destroySpace(sid, PCZLightFactory::FACTORY_TYPE_NAME);
	}
	//-----------------------------------------------------------------------
	void PCZSceneManager::destroyAllLight()
	{
		destroyAllSpace(PCZLightFactory::FACTORY_TYPE_NAME);
	}
	//---------------------------------------------------------------------
	void PCZSceneManager::findLightsAffectingFrustum(const Camera * camera)
	{
		// Similar to the basic SpaceManager, we iterate through
		// lights to see which ones affect the frustum.  However,
		// since we have camera & lights partitioned by zones,
		// we can check only those lights which either affect the
		// zone the camera is in, or affect zones which are visible to
		// the camera
        Nui32 lhash = mValidLights.getHash();
		SpaceMap & lights = getSpaceList(PCZLightFactory::FACTORY_TYPE_NAME);
		{
			// Pre-allocate memory
			mValidLights.clear();
			mValidLights.reserve(lights.size());

            SpaceMap::iterator m, mend = lights.end();
			for(m = lights.begin(); m != mend; ++m)
			{
				PCZLight * l = static_cast<PCZLight *>(m->second);
				if(l->isVisible() && l->affectsVisibleZone())
				{
					if(l->getType() == LT_DIRECTIONAL)
					{
						// Always visible
						mValidLights.push_back(l);
					}
					else
					{
						// NB treating spotlight as point for simplicity
						// Just see if the lights attenuation range is within the frustum
						Sphere sphere(l->getSpacePos(), l->getRange());
						if (camera->isIntersect(sphere))
						{
							mValidLights.push_back(l);
						}
					}
				}
			}
		} // release lock on lights collection

		// from here on down this function is same as Ogre::SpaceManager

		// Update lights affecting frustum if changed
		if(mValidLights.getHash() != lhash)
		{
			LightList::iterator j, jend = mValidLights.begin();
			for(j = mValidLights.begin(); j != jend; ++j)
			{
				// add cam distance for sorting if texture shadows
				if (isShadowTechniqueTextureBased())
				{
					(*j)->setSortBase(camera->getSpacePos());
				}
			}

			// Sort the lights if using texture shadows, since the first 'n' will be
			// used to generate shadow textures and we should pick the most appropriate
			if (isShadowTechniqueTextureBased())
			{
                // default sort (stable to preserve directional light ordering
                std::stable_sort(mValidLights.begin(), mValidLights.end(),
                    ShadowMappingPattern::LightShadowLess());

			}
            lhash = mValidLights.getHash();
			// notify light dirty, so all movable objects will re-populate
			// their light list next time
			notifyLight();
		}
	}
	//---------------------------------------------------------------------
	void PCZSceneManager::ensureShadowTexturesCreated()
	{
		bool shadowTextureConfigDirty = !ShadowMappingPattern::mTextureValid;
		SpaceManager::ensureShadowTexturesCreated();
		if(!shadowTextureConfigDirty) return;

		size_t count = mShadowTextureCameras.size();
		for(size_t i = 0; i < count; ++i)
		{
			PCZSceneNode * node = (PCZSceneNode *)mOriginNode->create(
				mShadowTextureCameras[i]->getName());
			node->attach(mShadowTextureCameras[i]);
			addPCZSceneNode(node, mDefaultZone);
		}
	}
	//---------------------------------------------------------------------
	void PCZSceneManager::destroyShadowTextures(void)
	{
		size_t count = mShadowTextureCameras.size();
		for (size_t i = 0; i < count; ++i)
		{
			SpaceNode* node = mShadowTextureCameras[i]->getSceneNode();
			mOriginNode->destroy(node->getID());
		}
		SpaceManager::destroyShadowTextures();
	}
	//---------------------------------------------------------------------
	void PCZSceneManager::onCast(Light * light, Camera * camera)
	{
		PCZSceneNode * camNode = (PCZSceneNode *)camera->getSceneNode();

		if(light->getType() == LT_DIRECTIONAL)
		{
			if(camNode->getHomeZone() != mActiveCameraZone)
				addPCZSceneNode(camNode, mActiveCameraZone);
		}
		else
		{
			PCZSceneNode* lightNode = (PCZSceneNode*)light->getSceneNode();
			assert(lightNode);
			PCZone* lightZone = lightNode->getHomeZone();
			if (camNode->getHomeZone() != lightZone)
				addPCZSceneNode(camNode, lightZone);
		}
	}

	/* Attempt to automatically connect unconnected portals to proper target zones
		* by looking for matching portals in other zones which are at the same location
		*/
	void PCZSceneManager::connectPortalsToTargetZonesByLocation(void)
	{
		// go through every zone to find portals
		ZoneMap::iterator i, iend;
		PCZone* zone;
		iend = mZones.end();
		bool foundMatch;
		for (i = mZones.begin(); i != iend; i++)
		{
			zone = i->second;
			// go through all the portals in the zone
			Portal* portal;
			PortalList::iterator pi, piend;
			piend = zone->mPortals.end();
			for (pi = zone->mPortals.begin(); pi != piend; pi++)
			{
				portal = *pi;
				//portal->updateDerivedValues();
				if (portal->getTargetZone() == 0)
				{
					// this is a portal without a connected zone - look for
					// a matching portal in another zone
					PCZone* zone2;
					ZoneMap::iterator j= mZones.begin();
					foundMatch = false;
					while (!foundMatch && j != mZones.end())
					{
						zone2 = j->second;
						if (zone2 != zone) // make sure we don't look in the same zone
						{
							Portal * portal2 = zone2->findMatchingPortal(portal);
							if (portal2)
							{
								// found a match!
								Ogre::LogManager::getSingletonPtr()->logMessage("Connecting portal "+portal->getName()+" to portal "+portal2->getName());
								foundMatch = true;
								portal->setTargetZone(zone2);
								portal->setTargetPortal(portal2);
								portal2->setTargetZone(zone);
								portal2->setTargetPortal(portal);
							}
						}
						j++;
					}
					if (foundMatch == false)
					{
						// error, didn't find a matching portal!
						N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
							"Could not find matching portal for portal " + portal->getName(),
							"PCZSceneManager::connectPortalsToTargetZonesByLocation");

					}
				}
			}
		}
	}

	// main visibility determination & render queue filling routine
	// over-ridden from base/default scene manager.  This is *the*
	// main call.
	void PCZSceneManager::findSpace(Camera * cam,
        RenderBound * visibleBounds, bool onlyShadowCasters)
	{
		// clear the render queue
		getRenderQueue()->clear();

		// if we are re-rendering the scene again with the same camera, we can just use the cache.
		// this helps post processing compositors.
		unsigned long frameCount = Root::getSingleton().getCurrentFrame();
		if (mLastActiveCamera == cam && mFrameCount == frameCount)
		{
			RenderQueue* queue = getRenderQueue();
			size_t count = mVisible.size();
			for (size_t i = 0; i < count; ++i)
			{
				((PCZSceneNode*)mVisible[i])->_addToRenderQueue(
					cam, queue, onlyShadowCasters, visibleBounds);
			}
			return;
		}

		// increment the visibility frame counter
		mFrameCount = frameCount;
		mLastActiveCamera = cam;

		// clear the list of visible nodes
		mVisible.clear();

		// turn off sky
		enableSky(false);

		// remove all extra culling planes
		((PCZCamera*)cam)->removeAllExtraCullingPlanes();

		// update the camera
		((PCZCamera*)cam)->update();

		// get the home zone of the camera
		PCZone* cameraHomeZone = ((PCZSceneNode*)(cam->getSceneNode()))->getHomeZone();

		// walk the zones, starting from the camera home zone,
		// adding all visible scene nodes to the mVisibles list
		cameraHomeZone->setLastVisibleFrame(mFrameCount);
		cameraHomeZone->findVisibleNodes((PCZCamera*)cam,
										  mVisible,
										  getRenderQueue(),
										  visibleBounds,
										  onlyShadowCasters,
										  mNodeView,
										  mShowBoundingBoxes);
	}

    void PCZSceneManager::findNodesIn( const AABox &box,
                                       PCZSceneNodeList &list,
                                       PCZone * startZone,
                                       PCZSceneNode *exclude )
    {
        PortalList visitedPortals;
		if (startZone)
		{
			// start in startzone, and recurse through portals if necessary
			startZone->_findNodes(box, list, visitedPortals, true, true, exclude);
		}
		else
		{
			// no start zone specified, so check all zones
			ZoneMap::iterator i;
			PCZone * zone;
			for (i = mZones.begin(); i != mZones.end(); i++)
			{
				zone = i->second;
				zone->_findNodes( box, list, visitedPortals, false, false, exclude );
			}
		}
    }

    void PCZSceneManager::findNodesIn( const Sphere &sphere,
                                       PCZSceneNodeList &list,
                                       PCZone * startZone,
                                       PCZSceneNode *exclude  )
    {
        PortalList visitedPortals;
		if (startZone)
		{
			// start in startzone, and recurse through portals if necessary
			startZone->_findNodes(sphere, list, visitedPortals, true, true, exclude);
		}
		else
		{
			// no start zone specified, so check all zones
			ZoneMap::iterator i;
			PCZone * zone;
			for (i = mZones.begin(); i != mZones.end(); i++)
			{
				zone = i->second;
				zone->_findNodes( sphere, list, visitedPortals, false, false, exclude );
			}
		}
    }

    void PCZSceneManager::findNodesIn(const PlaneSpace & volume,
                                       PCZSceneNodeList & list,
                                       PCZone * startZone,
                                       PCZSceneNode *exclude )
    {
        PortalList visitedPortals;
		if (startZone)
		{
			// start in startzone, and recurse through portals if necessary
			startZone->_findNodes(volume, list, visitedPortals, true, true, exclude);
		}
		else
		{
			// no start zone specified, so check all zones
			ZoneMap::iterator i;
			PCZone * zone;
			for (i = mZones.begin(); i != mZones.end(); i++)
			{
				zone = i->second;
				zone->_findNodes( volume, list, visitedPortals, false, false, exclude );
			}
		}
    }

    void PCZSceneManager::findNodesIn( const Ray &r,
                                       PCZSceneNodeList &list,
                                       PCZone * startZone,
                                       PCZSceneNode *exclude  )
    {
        PortalList visitedPortals;
		if (startZone)
		{
			// start in startzone, and recurse through portals if necessary
			startZone->_findNodes(r, list, visitedPortals, true, true, exclude);
		}
		else
		{
			ZoneMap::iterator i;
			PCZone * zone;
			for (i = mZones.begin(); i != mZones.end(); i++)
			{
				zone = i->second;
				zone->_findNodes( r, list, visitedPortals, false, false, exclude );
			}
		}
    }

    // get option keys (base along with PCZ-specific)
    bool PCZSceneManager::getPropertyList(PropertyArray * refKeys)
    {
        SpaceManager::getPropertyList(refKeys);
        refKeys->push_back( "ShowBoundingBoxes" );
        refKeys->push_back( "ShowPortals" );

        return true;
    }

    bool PCZSceneManager::setProperty(PropertyID key, const void * val)
    {
        if ( key == "ShowBoundingBoxes" )
        {
            mShowBoundingBoxes = * static_cast < const bool * > ( val );
            return true;
        }

        else if ( key == "ShowPortals" )
        {
            mShowPortals = * static_cast < const bool * > ( val );
            return true;
        }
		// send option to each zone
		ZoneMap::iterator i;
		PCZone * zone;
		for (i = mZones.begin(); i != mZones.end(); i++)
		{
			zone = i->second;
	         if (zone->setProperty(key, val ) == true)
			 {
				 return true;
			 }
		}

		// try regular scenemanager option
        return SpaceManager::setProperty( key, val );
    }

    bool PCZSceneManager::getProperty(getProperty key, void * val) const
    {
        if(key == "ShowBoundingBoxes")
        {

            * static_cast < bool * > ( val ) = mShowBoundingBoxes;
            return true;
        }
        if(key == "ShowPortals")
        {

            * static_cast < bool * > ( val ) = mShowPortals;
            return true;
        }
        return SpaceManager::getProperty( key, val );

    }

    //---------------------------------------------------------------------
    AABSpaceQuery*
    PCZSceneManager::createAABBQuery(const AABox& box, Nmark mask)
    {
        PCZAxisAlignedBoxSpaceQuery* q = N_new PCZAxisAlignedBoxSpaceQuery(this);
        q->setBox(box);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    SphereSpaceQuery*
    PCZSceneManager::createSphereQuery(const Sphere& sphere, Nmark mask)
    {
        PCZSphereSpaceQuery* q = N_new PCZSphereSpaceQuery(this);
        q->setSphere(sphere);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    PlaneSpaceQuery * PCZSceneManager::createPlaneSpaceQuery(
        const PlaneSpaceList & volumes, Nmark mask)
    {
        PCZPlaneSpaceQuery* q = N_new PCZPlaneSpaceQuery(this);
        q->setSpaceList(volumes);
        q->setQueryMask(mask);
        return q;
    }

    //---------------------------------------------------------------------
    RaySpaceQuery * PCZSceneManager::createRayQuery(const Ray & ray, Nmark mask)
    {
        PCZRaySpaceQuery* q = N_new PCZRaySpaceQuery(this);
        q->setRay(ray);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
    AreaSpaceQuery * PCZSceneManager::createIntersectQuery(Nmark mask)
    {

        PCZIntersectionSpaceQuery* q = N_new PCZIntersectionSpaceQuery(this);
        q->setQueryMask(mask);
        return q;
    }
    //---------------------------------------------------------------------
	// clear portal update flag from all zones
	void PCZSceneManager::_clearAllZonesPortalUpdateFlag(void)
	{
		ZoneMap::iterator zoneIterator = mZones.begin();

		while(zoneIterator != mZones.end())
		{
			(zoneIterator->second)->setPortalsUpdated(false);
			zoneIterator++;
		}
	}
	//---------------------------------------------------------------------
	void PCZSceneManager::prepareSTs(Camera * cam, Viewport * vp, const LightList * lightList)
	{
		mActiveCameraZone = ((PCZSceneNode*)cam->getSceneNode())->getHomeZone();
        if(lightList != 0)
            SpaceManager::renderShadow(cam, vp, *lightList);
        else
            SpaceManager::renderShadow(cam, vp, LightList());
	}

    //-----------------------------------------------------------------------
    const String PCZSceneManagerFactory::FACTORY_TYPE_NAME = "PCZSceneManager";
    //-----------------------------------------------------------------------
    const SpaceTypeVerbose & PCZSceneManagerFactory::getVerboseImpl() const
    {
	    mVerbose.mTypeID = FACTORY_TYPE_NAME;
	    mVerbose.mVerbose = "Scene manager organising the scene using Portal Connected Zones.";
	    mVerbose.mMark = SpaceTypeVerbose::ST_Normal | SpaceTypeVerbose::ST_Intersect; // support all types
    }
    //-----------------------------------------------------------------------
    SpaceManager* PCZSceneManagerFactory::create(const String & name)
    {
	    return N_new PCZSceneManager(name);
    }
    //-----------------------------------------------------------------------
    void PCZSceneManagerFactory::destroyInstance(SpaceManager * obj)
    {
	    N_delete obj;
    }


}
