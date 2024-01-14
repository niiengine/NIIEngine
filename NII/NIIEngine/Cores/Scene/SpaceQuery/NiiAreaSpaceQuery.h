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

#ifndef _NII_AREA_SPACEQUERY_H_
#define _NII_AREA_SPACEQUERY_H_

#include "NiiPreInclude.h"
#include "NiiSpaceQuery.h"
#include "NiiSpaceQueryCmd.h"
#include "NiiAABox.h"
#include "NiiRay.h"
#include "NiiSphere.h"
#include "NiiPlaneSpace.h"

namespace NII
{
    /** ����ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AreaSpaceQuery : public SpaceQuery, public SpaceQueryCmd
    {
    public:
        AreaSpaceQuery(SpaceManager * mag);
        virtual ~AreaSpaceQuery();

        using SpaceQuery::execute;

        /// @copydetails SpaceQuery::execute
        void execute();
        
        /// @copydetails SpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);

        /// @copydetails SpaceQueryCmd::onQuery
        bool onQuery(const QueryDetail & result);
    };
    
    /** AABB�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AABSpaceQuery : public AreaSpaceQuery
    {
    public:
        AABSpaceQuery(SpaceManager * mag);
        virtual ~AABSpaceQuery();

        /** ���������
        @version NIIEngine 3.0.0
        */
        inline void setBox(const AABox & area)      { mAABB = area;  }

        /** ��ȡ�����
        @version NIIEngine 3.0.0
        */
        inline const AABox & getBox() const         { return mAABB; }

        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        AABox mAABB;
    };
    
    /** ���߲�ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RaySpaceQuery : public AreaSpaceQuery
    {
    public:
        RaySpaceQuery(SpaceManager * mag);
        virtual ~RaySpaceQuery();

        /** ���ò�ѯ����
        @version NIIEngine 3.0.0
        */
        inline void setRay(const Ray & ray)         { mRay = ray; }

        /** ��ȡ��ѯ����
        @version NIIEngine 3.0.0
        */
        inline const Ray & getRay() const           { return mRay;  }

        /** �����Ƿ��������
        @version NIIEngine 3.0.0
        */
        inline void setSortDistance(bool set)       { mSortDistance = set; }

        /** �Ƿ��������
        @version NIIEngine 3.0.0
        */
        inline bool isSortDistance() const          { return mSortDistance; }

        /// @copydetails SpaceQueryCmd::execute
        virtual void execute();
        
        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        Ray mRay;
        bool mSortDistance;
    };
    
    /** Sphere�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SphereSpaceQuery : public AreaSpaceQuery
    {
    public:
        SphereSpaceQuery(SpaceManager * mag);
        virtual ~SphereSpaceQuery();

        /** ����������.
        @version NIIEngine 3.0.0
        */
        void setSphere(const Sphere & area);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        const Sphere & getSphere() const;
        
        /// @copydetails SpaceQuery::execute
        virtual void execute(SpaceQueryCmd * cmd);
    protected:
        Sphere mSphere;
    };
    
    /** Plane�ռ��ѯ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSpaceQuery : public AreaSpaceQuery
    {
    public:
        PlaneSpaceQuery(SpaceManager * mag);
        virtual ~PlaneSpaceQuery();

        /** ���ÿռ�
        @version NIIEngine 3.0.0
        */
        inline void setSpaceList(const PlaneSpaceList & list)  { mSpaceList = list; }

        /** ��ȡ�ռ�
        @version NIIEngine 3.0.0
        */
        inline const PlaneSpaceList & getSpaceList() const     {return mSpaceList;}
        
        /// @copydetails SpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);
    protected:
        PlaneSpaceList mSpaceList;
    };
}
#endif