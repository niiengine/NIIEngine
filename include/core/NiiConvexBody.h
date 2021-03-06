﻿/*
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

#ifndef _NII_ConvexBody_H_
#define _NII_ConvexBody_H_

#include "NiiPreInclude.h"
#include "NiiPolygon.h"

namespace NII
{
    /** 用立体图形描述凸面体
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ConvexBody
    {
    public:
        typedef vector<Polygon *>::type PolygonList;
    public:
        ConvexBody();

        ~ConvexBody();

        ConvexBody(const ConvexBody & o);

        /** 比较
        @version NIIEngine 3.0.0
        */
        bool operator == (const ConvexBody & o) const;

        /** 比较
        @version NIIEngine 3.0.0
        */
        bool operator != (const ConvexBody & o) const { return !(*this == o); }

        /** 数据交换
        @version NIIEngine 3.0.0
        */
        void swap(ConvexBody & o);

        /** 从平截头中建立凸面体
        @version NIIEngine 3.0.0
        */
        void define(const Frustum & obj);

        /** 从AAB中建立凸面体
        @version NIIEngine 3.0.0
        */
        void define(const AABox & obj);

        /** 使用点扩展凸面体
        @note 贝塞尔控点
        @version NIIEngine 3.0.0
        */
        void extend(const Vector3f & pt);

        /** 用平截头剪切凸面体
        @version NIIEngine 3.0.0
        */
        void clip(const Frustum & obj);

        /** 用AABB剪切凸面体
        @version NIIEngine 3.0.0
        */
        void clip(const AABox & obj);

        /** 用凸面体剪切凸面体(本对象)
        @version NIIEngine 3.0.0
        */
        void clip(const ConvexBody & obj);

        /** 用平面空间剪切凸面体
        @version NIIEngine 3.0.0
        */
        void clip(const Plane & obj, bool keepNegative = true);

        /** 清空凸面体的所有多边形
        @version NIIEngine 3.0.0
        */
        void reset();

        /** 获取凸面体多边形数量
        @version NIIEngine 3.0.0
        */
        NCount getPolygonCount() const;

        /** 获取多边形的顶点数量
        @version NIIEngine 3.0.0
        */
        NCount getVertexCount(NCount polyidx) const;

        /** 获取多边形
        @version NIIEngine 3.0.0
        */
        const Polygon & getPolygon(NCount polyidx) const;

        /** 返回指定一个多边形的一个顶点
        @version NIIEngine 3.0.0
        */
        const Vector3f & getVertex(NCount polyidx, NCount vertex) const;

        /** 返回指定多边形法线
        @version NIIEngine 3.0.0
        */
        const Vector3f & getNormal(NCount polyidx);

        /** 返回一个AABB表示
        @version NIIEngine 3.0.0
        */
        AABox getAABB() const;

        /** 凸面体是否属于封闭空间
        @version NIIEngine 3.0.0
        */
        bool hasClosedHull() const;

        /** 合并所有邻多边形到单一多边形
        @version NIIEngine 3.0.0
        */
        void mergePolygons();

        /** 初始化内部多边形池
        @version NIIEngine 3.0.0
        */
        static void setupPool();

        /** 拆毁内部多边形池
        @version NIIEngine 3.0.0
        */
        static void shutdownPool();
    protected:
        /// 缓存池中分配多边形
        static Polygon* allocatePolygon();

        /// 释放多边形回到缓存池
        static void freePolygon(Polygon * polyidx);

        /** 插入多边形到凸面体指定位置
        @version NIIEngine 3.0.0
        */
        void insertPolygon(Polygon * pdata, NCount polyidx);

        /** 插入多边形到凸面体末尾
        @version NIIEngine 3.0.0
        */
        void insertPolygon(Polygon * pdata);

        /** 插入顶点到多边形指定位置
        @version NIIEngine 3.0.0
        */
        void insertVertex(NCount polyidx, NCount veridx, const Vector3f & vertex);

        /** 插入顶点到多边形末尾
        @version NIIEngine 3.0.0
        */
        void insertVertex(NCount polyidx, const Vector3f & vertex);

        /** 删除一个指定的多边形
        @version NIIEngine 3.0.0
        */
        void deletePolygon(NCount polyidx);

        /** 从凸面体移去指定多边形
        @version NIIEngine 3.0.0
        */
        Polygon* popPolygon(NCount polyidx);

        /** 删除指定多边形的指定顶点.
        @version NIIEngine 3.0.0
        */
        void deleteVertex(NCount polyidx, NCount veridx);

        /** 在特定索引中替换一个多边形
        @note 内部删除原来的对象
        @version NIIEngine 3.0.0
        */
        void setPolygon(NCount polyidx, Polygon * obj);

        /** 设置指定多边形指定位置顶点
        @version NIIEngine 3.0.0
        */
        void setVertex(NCount polyidx, NCount veridx, const Vector3f & vertex);
    protected:
        PolygonList mPolygons;
        static PolygonList mFreeList;
        N_static_mutex(msFreePolygonsMutex)
    };

    _EngineAPI std::ostream & operator<< (std::ostream& out, const ConvexBody& obj);
}
#endif