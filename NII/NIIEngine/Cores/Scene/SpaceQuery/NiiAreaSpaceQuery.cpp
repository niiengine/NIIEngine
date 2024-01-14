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
#include "NiiAreaSpaceQuery.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    AreaSpaceQuery::AreaSpaceQuery(SpaceManager * mag) :
        SpaceQueryCmd(mag),
        SpaceQuery()
    {
        mSupportPrimitivesMark &= SpaceQuery::PT_None;
    }
    //-----------------------------------------------------------------------
    AreaSpaceQuery::~AreaSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void AreaSpaceQuery::execute()
    {
        clear();
        execute(this);
    }
    //---------------------------------------------------------------------
    void AreaSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        Engine::SpaceObjFactoryList::const_iterator ff, ffend = N_Engine().getSpaceObjFactoryList().end();
        for(ff = N_Engine().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
            const SpaceManager::SpaceMap & objItA = listener->getEffectArea()->getSpace(ff->second->getID());
            SpaceManager::SpaceMap::const_iterator h, hend = objItA.end();
            for(h = objItA.begin(); h != hend; ++h)
            {
                SpaceObj * a = h->second;
                // skip entire section if type doesn't match
                if(!(a->getTypeMark() & mQueryTypeMask))
                    break;

                // Skip if a does not pass the mask
                if(!(a->getIntersectMark() & mQueryMask) || !a->isActive())
                    continue;

                // Check against later objects in the same group
                SpaceManager::SpaceMap::const_iterator j, jend = objItA.end();
                for(j = objItA.begin(); j != jend; ++j)
                {
                    SpaceObj * b = j->second;
                    // Apply mask to b (both must pass)
                    if((b->getIntersectMark() & mQueryMask) && b->isActive())
                    {
                        const AABox & box1 = a->getSpaceAABB();
                        const AABox & box2 = b->getSpaceAABB();

                        if(box1.intersects(box2))
                        {
                            if(!listener->onQuery(CollisionQueryDetail(a, b)))
                                return;
                        }
                    }
                }
                // Check  against later groups
                Engine::SpaceObjFactoryList::const_iterator fff;
                for(fff = ff; fff != ffend; ++fff)
                {
                    const SpaceManager::SpaceMap & objItC = listener->getEffectArea()->getSpace(fff->second->getID());
                    SpaceManager::SpaceMap::const_iterator k, kend = objItC.end();
                    for(k = objItC.begin(); k != kend; ++k)
                    {
                        SpaceObj * c = k->second;
                        // skip entire section if type doesn't match
                        if(!(c->getTypeMark() & mQueryTypeMask))
                            break;

                        // Apply mask to c (both must pass)
                        if((c->getIntersectMark() & mQueryMask) && c->isActive())
                        {
                            const AABox & box1 = a->getSpaceAABB();
                            const AABox & box2 = c->getSpaceAABB();

                            if(box1.intersects(box2))
                            {
                                if(!listener->onQuery(CollisionQueryDetail(a, c)))
                                    return;
                            }
                        }
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    bool AreaSpaceQuery::onQuery(const QueryDetail & result)
    {
        mQueryList.push_back(result.clone());
        return true;
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // AABSpaceQuery
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    AABSpaceQuery::AABSpaceQuery(SpaceManager * mag) :
        AreaSpaceQuery(mag)
    {
        mSupportPrimitivesMark &= SpaceQuery::PT_None;
    }
    //---------------------------------------------------------------------
    AABSpaceQuery::~AABSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void AABSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        // 遍历所有可移动类型
        NCount count = 0;
        Engine::SpaceObjFactoryList::const_iterator ff, ffend = N_Engine().getSpaceObjFactoryList().end();
        for(ff = N_Engine().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
            const SpaceManager::SpaceMap & objItA = listener->getEffectArea()->getSpace(ff->second->getID());
            SpaceManager::SpaceMap::const_iterator i, iend = objItA.end();
            for(i = objItA.begin(); i != iend; ++i)
            {
                SpaceObj * a = i->second;
                if(!(a->getTypeMark() & mQueryTypeMask))
                    break;

                if((a->getIntersectMark() & mQueryMask) && a->isActive() &&
                    mAABB.intersects(a->getSpaceAABB()))
                {
                    ++count;
                    if(!listener->onQuery(SpaceObjQueryDetail(a)))
                        return;
                    if(count == mMaxResult)
                        return;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // RaySpaceQuery
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    bool QueryResultSortLess(const SpaceQueryCmd::QueryDetail * a, const SpaceQueryCmd::QueryDetail * b)
    {
        return a->mDistance < b->mDistance;
    }
    //---------------------------------------------------------------------
    RaySpaceQuery::RaySpaceQuery(SpaceManager * mag) :
        AreaSpaceQuery(mag)
    {
        // No world geometry results supported
        mSupportPrimitivesMark &= SpaceQuery::PT_None;
        mSortDistance = false;
    }
    //-----------------------------------------------------------------------
    RaySpaceQuery::~RaySpaceQuery()
    {
    }
    //-----------------------------------------------------------------------
    void RaySpaceQuery::execute()
    {
        clear();
        execute(this);

        if(mSortDistance && mQueryList.size())
        {
            mQueryList.sort(QueryResultSortLess);
        }
    }
    //---------------------------------------------------------------------
    void RaySpaceQuery::execute(SpaceQueryCmd * listener)
    {
        // Note that becuase we have no scene partitioning, we actually
        // perform a complete scene search even if restricted results are
        // requested; smarter scene manager queries can utilise the paritioning
        // of the scene in order to reduce the number of intersection tests
        // required to fulfil the query

        NCount count = 0;
        Engine::SpaceObjFactoryList::const_iterator ff, ffend = N_Engine().getSpaceObjFactoryList().end();
        for(ff = N_Engine().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
            const SpaceManager::SpaceMap & objItA = listener->getEffectArea()->getSpace(ff->second->getID());
            SpaceManager::SpaceMap::const_iterator i, iend = objItA.end();
            for(i = objItA.begin(); i != iend; ++i)
            {
                SpaceObj * a = i->second;
                // 跳过整个组,如果类型不匹配
                if(!(a->getTypeMark() & mQueryTypeMask))
                    break;

                if((a->getIntersectMark() & mQueryMask) && a->isActive())
                {
                    // Do ray / box test
                    std::pair<bool, NIIf> result;
                    result.first = mRay.intersects(a->getSpaceAABB(), result.second);

                    if(result.first)
                    {
                        ++count;
                        if(!listener->onQuery(SpaceObjQueryDetail(a, result.second)))
                            return;
                        if(count == mMaxResult)
                            return;
                    }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // PlaneSpaceQuery
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PlaneSpaceQuery::PlaneSpaceQuery(SpaceManager * mag) :
        AreaSpaceQuery(mag)
    {
        // No world geometry results supported
        mSupportPrimitivesMark &= SpaceQuery::PT_None;
    }
    //-----------------------------------------------------------------------
    PlaneSpaceQuery::~PlaneSpaceQuery()
    {
    }
    //---------------------------------------------------------------------
    void PlaneSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        // 遍历所有可移动类型
        NCount count = 0;
        Engine::SpaceObjFactoryList::const_iterator ff, ffend = N_Engine().getSpaceObjFactoryList().end();
        for(ff = N_Engine().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
            const SpaceManager::SpaceMap & objItA = listener->getEffectArea()->getSpace(ff->second->getID());
            SpaceManager::SpaceMap::const_iterator i, iend = objItA.end();
            for(i = objItA.begin(); i != iend; ++i)
            {
                SpaceObj* a = i->second;
                // 跳过整个组,如果类型不匹配
                if (!(a->getTypeMark() & mQueryTypeMask))
                    break;

                PlaneSpaceList::iterator pi, piend = mSpaceList.end();
                for (pi = mSpaceList.begin(); pi != piend; ++pi)
                {
                    PlaneSpace& vol = *pi;
                    // 处理 AABB /平面容积测试
                    if((a->getIntersectMark() & mQueryMask) &&
                        a->isActive() && vol.intersects(a->getSpaceAABB()))
                    {
                        ++count;
                        if(!listener->onQuery(SpaceObjQueryDetail(a)))
                            return;
                        if(count == mMaxResult)
                            return;
                        break;
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SphereSpaceQuery
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SphereSpaceQuery::SphereSpaceQuery(SpaceManager * mag) : 
        AreaSpaceQuery(mag)
    {
        // No world geometry results supported
        mSupportPrimitivesMark &= SpaceQuery::PT_None;
    }
    //-----------------------------------------------------------------------
    SphereSpaceQuery::~SphereSpaceQuery()
    {
    }
    //-----------------------------------------------------------------------
    void SphereSpaceQuery::setSphere(const Sphere & sphere)
    {
        mSphere = sphere;
    }
    //-----------------------------------------------------------------------
    const Sphere & SphereSpaceQuery::getSphere() const
    {
        return mSphere;
    }
    //---------------------------------------------------------------------
    void SphereSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        Sphere testSphere;

        // 遍历所有可移动类型
        NCount count = 0;
        Engine::SpaceObjFactoryList::const_iterator ff, ffend = N_Engine().getSpaceObjFactoryList().end();
        for(ff = N_Engine().getSpaceObjFactoryList().begin(); ff != ffend; ++ff)
        {
            const SpaceManager::SpaceMap & objItA = listener->getEffectArea()->getSpace(ff->second->getID());
            SpaceManager::SpaceMap::const_iterator m, mend = objItA.end();
            for(m = objItA.begin(); m != mend; ++m)
            {
                SpaceObj * a = m->second;
                // 跳过整个组,如果类型不匹配
                if(!(a->getTypeMark() & mQueryTypeMask))
                    break;
                // 跳过未附加的
                if(!a->isActive() || !(a->getIntersectMark() & mQueryMask))
                    continue;

                // Do sphere / sphere test
                testSphere.setCenter(a->getSpaceNode()->getSpacePos());
                testSphere.setRadius(a->getCenterRange());
                if(mSphere.intersects(testSphere))
                {
                    ++count;
                    if(!listener->onQuery(SpaceObjQueryDetail(a)))
                        return;
                    if(count == mMaxResult)
                        return;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
}