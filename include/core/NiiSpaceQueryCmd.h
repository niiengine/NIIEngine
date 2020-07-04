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

#ifndef _NII_SpaceQueryCmd_H_
#define _NII_SpaceQueryCmd_H_

#include "NiiPreInclude.h"
#include "NiiSpaceQuery.h"

namespace NII
{
    /** �ռ��ѯ
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI SpaceQueryCmd
    {
    public:
        /** ��ѯϸ��
        @version NIIEngine 3.0.0
        */
        class _EngineAPI QueryDetail : public SpaceAlloc
        {
        public:
            QueryDetail(){}
            QueryDetail(SpaceQuery::PrimitivesType type) :mType(type) {}
            QueryDetail(SpaceQuery::PrimitivesType type, NIIf dis) :mType(type),mDistance(dis){}
            virtual ~QueryDetail(){}

            /** ����
            @note ʹ�� N_delete ɾ��
            @version NIIEngine 3.0.0
            */
            virtual QueryDetail * clone() const = 0;
        public:
            SpaceQuery::PrimitivesType mType;
            NIIf mDistance;
        };
        typedef list<QueryDetail *>::type QueryList;
    public:
        SpaceQueryCmd(SpaceManager * space);
        virtual ~SpaceQueryCmd();

        /** ���ý���������
        @version NIIEngine 3.0.0
        */
        inline void setMaxResult(NCount c)              { mMaxResult = c; }

        /** ����������
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxResult() const              {return mMaxResult;}

        /** ִ�в�ѯ
        @version NIIEngine 3.0.0
        */
        virtual void execute() = 0;

        /** ��ѯ����
        @version NIIEngine 3.0.0
        */
        virtual bool onQuery(const QueryDetail & result) = 0;

        /** ��ȡ�ϴβ�ѯ���
        @version NIIEngine 3.0.0
        */
        inline const QueryList & getQuery() const       {return mQueryList;}

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * getEffectArea() const     { return mArea; }

        /** ������
        @version NIIEngine 3.0.0
        */
        void clear();
    protected:
        SpaceManager * mArea;
        QueryList mQueryList;
        NCount mMaxResult;
    };

    /** �����ѯϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        VertexQueryDetail();
        ~VertexQueryDetail();
        VertexQueryDetail(const Vector3f & obj);
        VertexQueryDetail(const Vector3f & obj, NIIf distance);

        /// @copydetails QueryDetail::clone
        QueryDetail * clone() const;
    public:
        Vector3f mPoint;
    };

    /** ����Ԫ��ѯϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeometryRawQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        GeometryRawQueryDetail();
        GeometryRawQueryDetail(const GeometryRaw * obj);
        GeometryRawQueryDetail(const GeometryRaw * obj, NIIf distance);
        ~GeometryRawQueryDetail();

        /// @copydetails QueryDetail::clone
        QueryDetail * clone() const;
    public:
        GeometryRaw * mGeoRaw;
    };

    /** �ռ�����ѯϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceObjQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        SpaceObjQueryDetail();
        SpaceObjQueryDetail(const SpaceObj * obj);
        SpaceObjQueryDetail(const SpaceObj * obj, NIIf distance);
        ~SpaceObjQueryDetail();

        /// @copydetails QueryDetail::clone
        QueryDetail * clone() const;
    public:
        SpaceObj * mSpaceObj;
    };

    /** ���ݻ���ѯϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneListQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        PlaneListQueryDetail();
        PlaneListQueryDetail(const list<Plane>::type * obj);
        PlaneListQueryDetail(const list<Plane>::type * obj, NIIf distance);
        ~PlaneListQueryDetail();

        QueryDetail * clone() const;
    public:
        list<Plane>::type * mPlaneList;
    };

    /** ��ײ��ѯϸ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CollisionQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        CollisionQueryDetail();
        CollisionQueryDetail(const SpaceObj * so1, const SpaceObj * so2);
        ~CollisionQueryDetail();

        /// @copydetails QueryDetail::clone
        QueryDetail * clone() const;
    public:
        SpaceObj * mSpaceObj1;
        SpaceObj * mSpaceObj2;
    };

    /** ƬԪ�����ѯϸ��
    @note ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IntersectQueryDetail : public SpaceQueryCmd::QueryDetail
    {
    public:
        IntersectQueryDetail();
        IntersectQueryDetail(const QueryDetail & qd1, const QueryDetail & qd2);
        ~IntersectQueryDetail();

        /// @copydetails QueryDetail::clone
        QueryDetail * clone() const;
    public:
        QueryDetail * mQueryDetail1;
        QueryDetail * mQueryDetail2;
    };
}

#endif