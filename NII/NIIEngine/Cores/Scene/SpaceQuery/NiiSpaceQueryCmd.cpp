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
#include "NiiSpaceQueryCmd.h"
#include "NiiGeometryRaw.h"

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // SpaceQueryCmd
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceQueryCmd::SpaceQueryCmd(SpaceManager * area) :
        mArea(area),
        mMaxResult(999)
    {
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::~SpaceQueryCmd()
    {
        clear();
    }
    //-----------------------------------------------------------------------
    void SpaceQueryCmd::clear()
    {
        QueryList::iterator i, iend = mQueryList.end();
        for(i = mQueryList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mQueryList.clear();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // VertexQueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    VertexQueryDetail::VertexQueryDetail():
        QueryDetail(SpaceQuery::PT_Point)
    {

    }
    //-----------------------------------------------------------------------
    VertexQueryDetail::VertexQueryDetail(const Vector3f & obj):
        QueryDetail(SpaceQuery::PT_Point),
        mPoint(obj)
    {

    }
    //-----------------------------------------------------------------------
    VertexQueryDetail::VertexQueryDetail(const Vector3f & obj, NIIf distance):
        QueryDetail(SpaceQuery::PT_Point, distance),
        mPoint(obj)
    {
    }
    //-----------------------------------------------------------------------
    VertexQueryDetail::~VertexQueryDetail()
    {
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * VertexQueryDetail::clone() const
    {
        return N_new VertexQueryDetail(mPoint, mDistance);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // QueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GeometryRawQueryDetail::GeometryRawQueryDetail() :
        QueryDetail(SpaceQuery::PT_GeoRaw)
    {
    }
    //-----------------------------------------------------------------------
    GeometryRawQueryDetail::GeometryRawQueryDetail(const GeometryRaw * obj):
        QueryDetail(SpaceQuery::PT_GeoRaw),
        mGeoRaw(const_cast<GeometryRaw *>(obj))
    {
    }
    //-----------------------------------------------------------------------
    GeometryRawQueryDetail::GeometryRawQueryDetail(const GeometryRaw * obj, NIIf distance):
        QueryDetail(SpaceQuery::PT_GeoRaw, distance),
        mGeoRaw(const_cast<GeometryRaw *>(obj))
    {

    }
    //-----------------------------------------------------------------------
    GeometryRawQueryDetail::~GeometryRawQueryDetail()
    {

    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * GeometryRawQueryDetail::clone() const
    {
        return N_new GeometryRawQueryDetail(mGeoRaw, mDistance);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // QueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    SpaceObjQueryDetail::SpaceObjQueryDetail():
        QueryDetail(SpaceQuery::PT_SpaceObj)
    {
    }
    //-----------------------------------------------------------------------
    SpaceObjQueryDetail::SpaceObjQueryDetail(const SpaceObj * obj):
        QueryDetail(SpaceQuery::PT_SpaceObj),
        mSpaceObj(const_cast<SpaceObj *>(obj))
    {

    }
    //-----------------------------------------------------------------------
    SpaceObjQueryDetail::SpaceObjQueryDetail(const SpaceObj * obj, NIIf distance):
        QueryDetail(SpaceQuery::PT_SpaceObj, distance),
        mSpaceObj(const_cast<SpaceObj *>(obj))
    {
    }
    //-----------------------------------------------------------------------
    SpaceObjQueryDetail::~SpaceObjQueryDetail()
    {
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * SpaceObjQueryDetail::clone() const
    {
        return N_new SpaceObjQueryDetail(mSpaceObj, mDistance);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // QueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PlaneListQueryDetail::PlaneListQueryDetail() :
        QueryDetail(SpaceQuery::PT_PlaneList)
    {
    }
    //-----------------------------------------------------------------------
    PlaneListQueryDetail::PlaneListQueryDetail(const list<Plane>::type * obj):
        QueryDetail(SpaceQuery::PT_PlaneList),
        mPlaneList(const_cast<list<Plane>::type *>(obj))
    {
    }
    //-----------------------------------------------------------------------
    PlaneListQueryDetail::PlaneListQueryDetail(const list<Plane>::type * obj, NIIf distance):
        QueryDetail(SpaceQuery::PT_PlaneList, distance),
        mPlaneList(const_cast<list<Plane>::type *>(obj))
    {
    }
    //-----------------------------------------------------------------------
    PlaneListQueryDetail::~PlaneListQueryDetail()
    {
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * PlaneListQueryDetail::clone() const
    {
        return N_new PlaneListQueryDetail(mPlaneList, mDistance);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // CollisionQueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    CollisionQueryDetail::CollisionQueryDetail():
        QueryDetail(SpaceQuery::PT_Collision)
    {
    }
    //-----------------------------------------------------------------------
    CollisionQueryDetail::CollisionQueryDetail(const SpaceObj * so1, const SpaceObj * so2):
        QueryDetail(SpaceQuery::PT_Collision),
        mSpaceObj1(const_cast<SpaceObj *>(so1)),
        mSpaceObj2(const_cast<SpaceObj *>(so2))
    {
    }
    //-----------------------------------------------------------------------
    CollisionQueryDetail::~CollisionQueryDetail()
    {
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * CollisionQueryDetail::clone() const
    {
        return N_new CollisionQueryDetail(mSpaceObj1, mSpaceObj2);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // IntersectQueryDetail
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    IntersectQueryDetail::IntersectQueryDetail():
        QueryDetail(SpaceQuery::PT_Intersect),
        mQueryDetail1(0),
        mQueryDetail2(0)
    {

    }
    //-----------------------------------------------------------------------
    IntersectQueryDetail::IntersectQueryDetail(const QueryDetail & qd1, const QueryDetail & qd2):
        QueryDetail(SpaceQuery::PT_Intersect),
        mQueryDetail1(qd1.clone()),
        mQueryDetail2(qd2.clone())
    {
    }
    //-----------------------------------------------------------------------
    IntersectQueryDetail::~IntersectQueryDetail()
    {
        if(mQueryDetail1)
            N_delete mQueryDetail1;
        if(mQueryDetail2)
            N_delete mQueryDetail2;
    }
    //-----------------------------------------------------------------------
    SpaceQueryCmd::QueryDetail * IntersectQueryDetail::clone() const
    {
        return N_new IntersectQueryDetail(*mQueryDetail1, *mQueryDetail2);
    }
    //-----------------------------------------------------------------------
}