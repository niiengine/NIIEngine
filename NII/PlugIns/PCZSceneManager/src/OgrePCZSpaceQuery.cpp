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

#include <OgreEntity.h>
#include <OgreRoot.h>

#include "OgrePCZSpaceQuery.h"
#include "OgrePCZSceneManager.h"

namespace Ogre
{
    //---------------------------------------------------------------------
    PCZIntersectionSpaceQuery::PCZIntersectionSpaceQuery(SpaceManager* creator) :
        AreaSpaceQuery(creator)
    {

    }
    //---------------------------------------------------------------------
    PCZIntersectionSpaceQuery::~PCZIntersectionSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void PCZIntersectionSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        typedef std::pair<SpaceObj *, SpaceObj *> MovablePair;
        typedef std::set<std::pair<SpaceObj *, SpaceObj *> > MovableSet;

        MovableSet set;

        Engine::SpaceObjFactoryList::iterator ff, ffend = Root::getSingleton().getSpaceObjFactoryList().end();
        for(ff = Root::getSingleton().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
		    const SpaceManager::SpaceMap & it = listener->mArea->getSpace(ff->getType());
            SpaceMap::iterator sm, smend = it.end();
		    for(sm = it.begin(); sm != smend; ++sm)
		    {
			    SpaceObj * e = sm->second;
                PCZone * zone = ((PCZSceneNode*)(e->getSceneNode()))->getHomeZone();
			    PCZSceneNodeList list;
			    //find the nodes that intersect the AAB
			    static_cast<PCZSceneManager *>(listener->mArea) -> findNodesIn(e->getSpaceAABB(), list, zone, 0);
			    //grab all moveables from the node that intersect...
			    PCZSceneNodeList::iterator nit = list.begin();
			    while( nit != list.end() )
			    {
				    SpaceNode::AttachList::iterator oit, oitend = (*nit)->getAttachList().end();
                    for(oit = (*nit)->getAttachList().begin(); oit != oitend; ++oit)
				    {
					    SpaceObj * m = oit->second;

					    if(m != e &&
							set.find(MovablePair(e,m)) == set.end() &&
							set.find(MovablePair(m,e)) == set.end() &&
							(m->getQueryFlags() & mQueryMask) &&
							(m->getTypeMark() & mQueryTypeMask) && m->isActive() &&
							e->getSpaceAABB().intersects(m->getSpaceAABB()))
					    {
						    listener->onQuery(CollisionQueryDetail(e, m));
						    // deal with attached objects, since they are not directly attached to nodes
						    if (m->getFactoryID() == N_FACTORY_GEOSPACE)
						    {
							    GeometrySpace * e2 = static_cast<GeometrySpace *>(m);
							    GeometrySpace::Childs i, iend = e2->getAttachList().end();
                                for(i = e2->getAttachList().begin(); i != iend; ++i)
							    {
								    SpaceObj* c = i->second;
								    if (c->getQueryFlags() & mQueryMask &&
									    e->getSpaceAABB().intersects( c->getSpaceAABB() ))
								    {
									    listener->onQuery(CollisionQueryDetail(e, c));
								    }
							    }
						    }
					    }
					    set.insert( MovablePair(e,m) );

				    }
				    ++nit;
			    }

		    }
	    }
    }
    /** Creates a custom PCZ AAB query */
    PCZAxisAlignedBoxSpaceQuery::PCZAxisAlignedBoxSpaceQuery(SpaceManager * creator) :
        AABSpaceQuery(creator)
    {
		mStartZone = 0;
        mExcludeNode = 0;
    }
    /** Deletes the custom PCZ query */
    PCZAxisAlignedBoxSpaceQuery::~PCZAxisAlignedBoxSpaceQuery()
    {
    }
    /** Finds any entities that intersect the AAB for the query. */
    void PCZAxisAlignedBoxSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        PCZSceneNodeList list;
        //find the nodes that intersect the AAB
        static_cast<PCZSceneManager *>(listener->mArea)->findNodesIn(mAABB, list, mStartZone, (PCZSceneNode*)mExcludeNode);

        //grab all moveables from the node that intersect...
        PCZSceneNodeList::iterator it = list.begin();
        while(it != list.end())
        {
            SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
            for(oit = (*it)->getAttachList().begin(); oit != oitend; ++oit)
            {
                SpaceObj * m = oit->second;
                if((m->getQueryFlags() & mQueryMask) && (m->getTypeMark() & mQueryTypeMask) &&
				    m->isActive() && mAABB.intersects(m->getSpaceAABB()))
                {
                    listener -> onQuery(SpaceObjQueryDetail(m));
				    // deal with attached objects, since they are not directly attached to nodes
				    if (m->getFactoryID() == N_FACTORY_GEOSPACE)
				    {
					    GeometrySpace* e = static_cast<GeometrySpace*>(m);
					    GeometrySpace::Childs i, iend = e->getAttachList().end();
                        for(i = e->getAttachList().begin(); i != iend; ++i)
					    {
						    SpaceObj* c = i->second;
						    if (c->getQueryFlags() & mQueryMask)
						    {
							    listener->onQuery(SpaceObjQueryDetail(c));
						    }
					    }
				    }
                }

            }
            ++it;
        }
        // reset startzone and exclude node
        mStartZone = 0;
        mExcludeNode = 0;
    }
    //---------------------------------------------------------------------
    PCZRaySpaceQuery::
    PCZRaySpaceQuery(SpaceManager* creator) : RaySpaceQuery(creator)
    {
		mStartZone = 0;
        mExcludeNode = 0;
    }
    //---------------------------------------------------------------------
    PCZRaySpaceQuery::~PCZRaySpaceQuery()
    {}
    //---------------------------------------------------------------------
    void PCZRaySpaceQuery::execute(SpaceQueryCmd* listener)
    {
        PCZSceneNodeList list;
        //find the nodes that intersect the Ray
        static_cast<PCZSceneManager*>( listener->mArea ) -> findNodesIn( mRay, list, mStartZone, (PCZSceneNode*)mExcludeNode );

        //grab all moveables from the node that intersect...
        PCZSceneNodeList::iterator it = list.begin();
        while( it != list.end() )
        {
            SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
            for(oit = (*it)->getAttachList().begin(); oit != oitend; ++oit)
            {
                SpaceObj * m = oit->second;
                if( (m->getQueryFlags() & mQueryMask) &&
				    (m->getTypeMark() & mQueryTypeMask) && m->isActive() )
                {
                    std::pair<bool, NIIf> result = mRay.intersects(m->getSpaceAABB());

                    if( result.first )
                    {
                        listener -> onQuery(SpaceObjQueryDetail(m, result.second));
					    // deal with attached objects, since they are not directly attached to nodes
					    if (m->getFactoryID() == N_FACTORY_GEOSPACE)
					    {
						    GeometrySpace* e = static_cast<GeometrySpace*>(m);
						    GeometrySpace::Childs i, iend = e->getAttachList().end();
                            for(i = e->getAttachList().begin(); i != iend; ++i)
						    {
							    SpaceObj* c = i->second;
							    if (c->getQueryFlags() & mQueryMask)
							    {
								    result = mRay.intersects(c->getSpaceAABB());
								    if (result.first)
								    {
									    listener->onQuery(SpaceObjQueryDetail(c, result.second));
								    }
							    }
						    }
					    }
                    }
                }
            }
            ++it;
        }
        // reset startzone and exclude node
        mStartZone = 0;
        mExcludeNode = 0;
    }


    //---------------------------------------------------------------------
    PCZSphereSpaceQuery::PCZSphereSpaceQuery(SpaceManager * creator) :
        SphereSpaceQuery(creator)
    {
		mStartZone = 0;
        mExcludeNode = 0;
    }
    //---------------------------------------------------------------------
    PCZSphereSpaceQuery::~PCZSphereSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void PCZSphereSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        PCZSceneNodeList list;
        //find the nodes that intersect the Sphere
        static_cast<PCZSceneManager *>(listener->mArea)->findNodesIn(mSphere, list, mStartZone, (PCZSceneNode*)mExcludeNode );

        //grab all moveables from the node that intersect...
        PCZSceneNodeList::iterator it = list.begin();
        while(it != list.end())
        {
            SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
            for(oit = (*it)->getAttachList().begin(); oit != oitend; ++oit)
            {
                SpaceObj * m = oit->second;
                if((m->getQueryFlags() & mQueryMask) && (m->getTypeMark() & mQueryTypeMask) &&
				    m->isActive() && mSphere.intersects(m->getSpaceAABB()))
                {
                    listener->onQuery(SpaceObjQueryDetail(m));
				    // deal with attached objects, since they are not directly attached to nodes
				    if(m->getFactoryID() == N_FACTORY_GEOSPACE)
				    {
					    GeometrySpace * e = static_cast<GeometrySpace*>(m);
					    GeometrySpace::Childs i, iend = e->getAttachList().end();
                        for(i = e->getAttachList().begin(); i != iend; ++i)
					    {
						    SpaceObj * c = i->second;
						    if(c->getQueryFlags() & mQueryMask &&
                                mSphere.intersects(c->getSpaceAABB()))
						    {
							    listener->onQuery(SpaceObjQueryDetail(c));
						    }
					    }
				    }
                }
            }
            ++it;
        }
        // reset startzone and exclude node
        mStartZone = 0;
        mExcludeNode = 0;
    }
    //---------------------------------------------------------------------
    PCZPlaneSpaceQuery::
    PCZPlaneSpaceQuery(SpaceManager * creator) :
        PlaneSpaceQuery(creator)
    {
		mStartZone = 0;
        mExcludeNode = 0;
    }
    //---------------------------------------------------------------------
    PCZPlaneSpaceQuery::~PCZPlaneSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void PCZPlaneSpaceQuery::execute(SpaceQueryCmd* listener)
    {
        set<SpaceNode*>::type checkedSceneNodes;

        PlaneSpaceList::iterator pi, piend = mSpaceList.end();
        for (pi = mSpaceList.begin(); pi != piend; ++pi)
        {
            PCZSceneNodeList list;
            //find the nodes that intersect the Plane bounded Volume
            static_cast<PCZSceneManager*>( listener->mArea ) -> findNodesIn( *pi, list, mStartZone, (PCZSceneNode*)mExcludeNode );

            //grab all moveables from the node that intersect...
            PCZSceneNodeList::iterator it, itend = list.end();
            for(it = list.begin(); it != itend; ++it)
            {
                // avoid double-check same scene node
                if(!checkedSceneNodes.insert(*it).second)
                    continue;
                SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
                for(oit = (*it)->getAttachList().begin(); oit != oitend; ++oit)
                {
                    SpaceObj * m = oit->second;
                    if((m->getQueryFlags() & mQueryMask) && (m->getTypeMark() & mQueryTypeMask) &&
					    m->isActive() && (*pi).intersects(m->getSpaceAABB()))
                    {
                        listener -> onQuery(SpaceObjQueryDetail(m));
					    // deal with attached objects, since they are not directly attached to nodes
					    if(m->getFactoryID() == N_FACTORY_GEOSPACE)
					    {
						    GeometrySpace * e = static_cast<GeometrySpace *>(m);
						    GeometrySpace::Childs i, iend = e->getAttachList().end();
                            for(i = e->getAttachList().begin(); i != iend; ++i)
						    {
							    SpaceObj* c = i->second;
							    if(c->getQueryFlags() & mQueryMask &&
								    (*pi).intersects( c->getSpaceAABB()))
							    {
								    listener->onQuery(SpaceObjQueryDetail(c));
							    }
						    }
					    }
                    }
                }
            }
        }//for
        // reset startzone and exclude node
        mStartZone = 0;
        mExcludeNode = 0;
    }
}