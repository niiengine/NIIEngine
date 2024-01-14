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

#ifndef _NII_POLYGON_H_
#define _NII_POLYGON_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"

namespace NII
{
    /** 一个在三维空间描述多边形的类
    @remark 多个点描述的逆时针顺序多边形,并且所有点在某个平面上
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Polygon
    {
    public:
        typedef vector<Vector3f>::type VertexList;
        typedef std::pair<Vector3f, Vector3f> Edge;
        typedef list<Edge>::type EdgeMap;
    public:
        Polygon();
        ~Polygon();

        Polygon(const Polygon & o);

        /** 比较相等
        @version NIIEngine 3.0.0
        */
        bool operator == (const Polygon & o) const;

        /** 比较不相等
        @version NIIEngine 3.0.0
        */
        bool operator != (const Polygon & o) const
        {
            return !(*this == o);
        }

        /** 插入一个顶点到一个指定位置
        @note 顶点必须共面
        @version NIIEngine 3.0.0
        */
        void insertVertex(const Vector3f & vertex, NCount idx);

        /** 插入一个顶点到多边形末尾
        @note 顶点必须共面
        @version NIIEngine 3.0.0
        */
        void insertVertex(const Vector3f & vertex);

        /** 删除特定的顶点
        @version NIIEngine 3.0.0
        */
        void deleteVertex(NCount vertex);

        /** 返回指定顶点
        @version NIIEngine 3.0.0
        */
        const Vector3f & getVertex(NCount idx) const;

        /** 设置一个多边形的一个指定顶点
        @note 顶点必须共面
        @version NIIEngine 3.0.0
        */
        void setVertex(const Vector3f & vertex, NCount idx);

        /** 顶点数量
        @version NIIEngine 3.0.0
        */
        NCount getVertexCount() const;

        /** 返回多边形法线
        @version NIIEngine 3.0.0
        */
        const Vector3f & getNormal() const;

        /** 确定点是否在多边形里面
        @remark 
            凸面的和平面的多边形，存在一个点在多边形的平面上,且在多边形的边界里
        @version NIIEngine 3.0.0
        */
        bool isPointInside(const Vector3f & point) const;

        /** 获取多边形边缘
        @par 1-2 2-3 3-4 4-1,逆时针顺序ccw
        @version NIIEngine 3.0.0
        */
        void getEdge(EdgeMap * list) const;

        /** 获取多边形边缘
        @par 1-4 4-3 3-2 2-1,顺时针顺序cw
        @version NIIEngine 3.0.0
        */
        void getClockwiseEdge(EdgeMap * list) const;

        /** 从多边形中移去重复顶点
        @version NIIEngine 3.0.0
        */
        void opt();

        /** 重新设置对象
        @version NIIEngine 3.0.0
        */
        void cleanAll();
    protected:
        /**更新法线
        @version NIIEngine 3.0.0
        */
        void updateNormal() const;
    protected:
        VertexList          mVertexList;
        mutable Vector3f    mNormal;
        mutable bool        mIsNormalSet;
    };

    /** 输出顶点数据
    @version NIIEngine 3.0.0
    */
    _EngineAPI Nostream & operator<< (Nostream & strm, const Polygon & poly);
}
#endif