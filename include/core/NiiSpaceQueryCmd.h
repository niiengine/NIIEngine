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
    /** 空间查询
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI SpaceQueryCmd
    {
    public:
        /** 查询细节
        @version NIIEngine 3.0.0
        */
        class _EngineAPI QueryDetail : public SpaceAlloc
        {
        public:
            QueryDetail(){}
            QueryDetail(SpaceQuery::PrimitivesType type) :mType(type) {}
            QueryDetail(SpaceQuery::PrimitivesType type, NIIf dis) :mType(type),mDistance(dis){}
            virtual ~QueryDetail(){}

            /** 副本
            @note 使用 N_delete 删除
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

        /** 设置结果最大数量
        @version NIIEngine 3.0.0
        */
        inline void setMaxResult(NCount c)              { mMaxResult = c; }

        /** 结果最大数量
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxResult() const              {return mMaxResult;}

        /** 执行查询
        @version NIIEngine 3.0.0
        */
        virtual void execute() = 0;

        /** 查询返回
        @version NIIEngine 3.0.0
        */
        virtual bool onQuery(const QueryDetail & result) = 0;

        /** 获取上次查询结果
        @version NIIEngine 3.0.0
        */
        inline const QueryList & getQuery() const       {return mQueryList;}

        /** 获取作用区域
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * getEffectArea() const     { return mArea; }

        /** 清理结果
        @version NIIEngine 3.0.0
        */
        void clear();
    protected:
        SpaceManager * mArea;
        QueryList mQueryList;
        NCount mMaxResult;
    };

    /** 顶点查询细节
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

    /** 几何元查询细节
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

    /** 空间对象查询细节
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

    /** 面容积查询细节
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

    /** 碰撞查询细节
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

    /** 片元交叉查询细节
    @note 复杂型
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