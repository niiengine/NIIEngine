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
/***************************************************************************
OgreOctreeSpaceQuery.cpp  -  description
-------------------
begin                : Tues July 20, 2004
copyright            : (C) 2004by Jon Anderson
email                : janders@users.sf.net



***************************************************************************/

#include <OgreOctreeSpaceQuery.h>
#include <OgreOctreeSceneManager.h>
#include <OgreEntity.h>
#include <OgreRoot.h>

namespace Ogre
{

//---------------------------------------------------------------------
OctreeIntersectionSpaceQuery::OctreeIntersectionSpaceQuery(SpaceManager* creator) :
    AreaSpaceQuery(creator)
{
}
//---------------------------------------------------------------------
OctreeIntersectionSpaceQuery::~OctreeIntersectionSpaceQuery()
{
}
//---------------------------------------------------------------------
void OctreeIntersectionSpaceQuery::execute(SpaceQueryCmd * listener)
{
    typedef std::pair<SpaceObj *, SpaceObj *> MovablePair;
    typedef std::set<std::pair<SpaceObj *, SpaceObj *> > MovableSet;

    MovableSet set;

	// Iterate over all movable types
    Engine::SpaceObjFactoryList::iterator ff, ffend = Root::getSingleton().getSpaceObjFactoryList().end();
    for(ff = Root::getSingleton().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
	{
		const SpaceManager::SpaceMap & it = listener->mArea->getSpace(ff->getType());
        SpaceManager::SpaceMap::iterator f, fend = it.end();
        for(f = it.begin(); f != fend; ++f)
		{
			SpaceObj * e = f->second;

			Ogre::list< SpaceNode * >::type list;
			//find the nodes that intersect the AAB
			static_cast<OctreeSceneManager*>( listener->mArea ) -> findNodesIn( e->getSpaceAABB(), list, 0 );
			//grab all moveables from the node that intersect...
			Ogre::list< SpaceNode * >::type::iterator nit = list.begin();
			while( nit != list.end() )
			{
				SpaceNode::AttachList::iterator oit, oitend = (*nit)->getAttachList().end();
                for(oit = (*nit)->getAttachList().begin(); oit != oitend; ++oit)
				{
					SpaceObj * m = oit->second;

					if(m != e && set.find(MovablePair(e,m)) == set.end() &&
                        set.find( MovablePair(m,e)) == set.end() &&
                            (m->getQueryFlags() & mQueryMask) &&
                                (m->getTypeMark() & mQueryTypeMask) && m->isActive() &&
                                    e->getSpaceAABB().intersects(m->getSpaceAABB()))
					{
						listener->onQuery(CollisionQueryDetail(e, m));
						// deal with attached objects, since they are not directly attached to nodes
						if(m->getFactoryID() == N_FACTORY_GEOSPACE)
						{
							GeometrySpace * e2 = static_cast<GeometrySpace *>(m);
							GeometrySpace::Childs i, iend = e2->getAttachList().end();
                            for(i = e2->getAttachList().begin(); i != iend; ++i)
							{
								SpaceObj * c = i->second;
								if(c->getQueryFlags() & mQueryMask &&
									e->getSpaceAABB().intersects(c->getSpaceAABB()))
								{
									listener->onQuery(CollisionQueryDetail(e, c));
								}
							}
						}
					}
					set.insert(MovablePair(e, m));

				}
				++nit;
			}

		}
	}
}
/** Creates a custom Octree AAB query */
OctreeAxisAlignedBoxSpaceQuery::OctreeAxisAlignedBoxSpaceQuery(SpaceManager * creator) :
    AABSpaceQuery(creator)
{
}
/** Deletes the custom Octree query */
OctreeAxisAlignedBoxSpaceQuery::~OctreeAxisAlignedBoxSpaceQuery()
{}

/** Finds any entities that intersect the AAB for the query. */
void OctreeAxisAlignedBoxSpaceQuery::execute(SpaceQueryCmd * listener)
{
    list< SpaceNode * >::type _list;
    //find the nodes that intersect the AAB
    static_cast<OctreeSceneManager*>( listener->mArea ) -> findNodesIn( mAABB, _list, 0 );

    //grab all moveables from the node that intersect...
    list< SpaceNode * >::type::iterator it = _list.begin();
    while(it != _list.end())
    {
        SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
        for(oit = (*it) -> getAttachList().begin(); oit != oitend; ++oit)
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
						SpaceObj * c = i->second;
						if(c->getQueryFlags() & mQueryMask)
						{
							listener->onQuery(SpaceObjQueryDetail(c));
						}
					}
				}
            }

        }

        ++it;
    }

}
//---------------------------------------------------------------------
OctreeRaySpaceQuery::
OctreeRaySpaceQuery(SpaceManager* creator) : RaySpaceQuery(creator)
{
}
//---------------------------------------------------------------------
OctreeRaySpaceQuery::~OctreeRaySpaceQuery()
{}
//---------------------------------------------------------------------
void OctreeRaySpaceQuery::execute(SpaceQueryCmd * listener)
{
    list<SpaceNode *>::type _list;
    //find the nodes that intersect the AAB
    static_cast<OctreeSceneManager *>(listener->mArea) -> findNodesIn(mRay, _list, 0);

    //grab all moveables from the node that intersect...
    list< SpaceNode * >::type::iterator it = _list.begin();
    while(it != _list.end())
    {
        SpaceNode::AttachList::iterator oit, oitend = (*it)->getAttachList().end();
        for(oit = (*it)->getAttachList().begin(); oit != oitend; ++oit)
        {
            SpaceObj * m = oit->second;
            if( (m->getQueryFlags() & mQueryMask) &&
				(m->getTypeMark() & mQueryTypeMask) && m->isActive() )
            {
                std::pair<bool, NIIf> result = mRay.intersects(m->getSpaceAABB());

                if(result.first)
                {
                    listener -> onQuery(SpaceObjQueryDetail(m, result.second));
					// deal with attached objects, since they are not directly attached to nodes
					if(m->getFactoryID() == N_FACTORY_GEOSPACE)
					{
						GeometrySpace * e = static_cast<GeometrySpace *>(m);
						GeometrySpace::Childs i, iend = e->getAttachList().end();
                        for(i = e->getAttachList().begin(); i != iend; ++i)
						{
							SpaceObj * c = i->second;
							if(c->getQueryFlags() & mQueryMask)
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

}


//---------------------------------------------------------------------
OctreeSphereSpaceQuery::
OctreeSphereSpaceQuery(SpaceManager* creator) :
    SphereSpaceQuery(creator)
{
}
//---------------------------------------------------------------------
OctreeSphereSpaceQuery::~OctreeSphereSpaceQuery()
{}
//---------------------------------------------------------------------
void OctreeSphereSpaceQuery::execute(SpaceQueryCmd* listener)
{
    list< SpaceNode * >::type _list;
    //find the nodes that intersect the AAB
    static_cast<OctreeSceneManager*>( listener->mArea ) -> findNodesIn( mSphere, _list, 0 );

    //grab all moveables from the node that intersect...
    list< SpaceNode * >::type::iterator it = _list.begin();
    while( it != _list.end() )
    {
        SpaceNode::AttachList::iterator oit, oitend = (*it) -> getAttachList().end();
        for(oit = (*it) -> getAttachList().begin(); oit != oitend; ++oit)
        {
            SpaceObj * m = oit->second;
            if((m->getQueryFlags() & mQueryMask) && (m->getTypeMark() & mQueryTypeMask) &&
				m->isActive() && mSphere.intersects(m->getSpaceAABB()))
            {
                listener->onQuery(SpaceObjQueryDetail(m));
				// deal with attached objects, since they are not directly attached to nodes
				if (m->getFactoryID() == N_FACTORY_GEOSPACE)
				{
					GeometrySpace* e = static_cast<GeometrySpace *>(m);
					GeometrySpace::Childs i, iend = e->getAttachList().end();
                    for(i = e->getAttachList().begin(); i != iend; ++i)
					{
						SpaceObj * c = i->second;
						if(c->getQueryFlags() & mQueryMask &&
							mSphere.intersects( c->getSpaceAABB()))
						{
							listener->onQuery(SpaceObjQueryDetail(c));
						}
					}
				}
            }
        }

        ++it;
    }
}
//---------------------------------------------------------------------
OctreePlaneSpaceQuery::
OctreePlaneSpaceQuery(SpaceManager * creator) :
    PlaneSpaceQuery(creator)
{

}
//---------------------------------------------------------------------
OctreePlaneSpaceQuery::~OctreePlaneSpaceQuery()
{
}
//---------------------------------------------------------------------
void OctreePlaneSpaceQuery::execute(SpaceQueryCmd * listener)
{
    set<SpaceNode *>::type checkedSceneNodes;

    PlaneSpaceList::iterator pi, piend = mSpaceList.end();
    for(pi = mSpaceList.begin(); pi != piend; ++pi)
    {
        list<SpaceNode *>::type _list;
        //find the nodes that intersect the AAB
        static_cast<OctreeSceneManager*>( listener->mArea ) -> findNodesIn( *pi, _list, 0 );

        //grab all moveables from the node that intersect...
        list<SpaceNode *>::type::iterator it, itend = _list.end();
        for(it = _list.begin(); it != itend; ++it)
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
					if (m->getFactoryID() == N_FACTORY_GEOSPACE)
					{
						GeometrySpace * e = static_cast<GeometrySpace*>(m);
						GeometrySpace::Childs i, iend = e->getAttachList().end();
                        for(i = getAttachList().begin(); i != iend; ++i)
						{
							SpaceObj* c = i->second;
							if(c->getQueryFlags() & mQueryMask && (*pi).intersects( c->getSpaceAABB()))
							{
								listener->onQuery(SpaceObjQueryDetail(c));
							}
						}
					}
                }
            }
        }
    }//for
}


}
