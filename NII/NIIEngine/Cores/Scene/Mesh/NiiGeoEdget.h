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

#ifndef _NII_GEOEDGET_H_
#define _NII_GEOEDGET_H_

#include "NiiPreInclude.h"
#include "NiiGeometryRaw.h"
#include "NiiVector4.h"

namespace NII
{
    /** 网格边缘
    @note 同种饶向的三角形
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GeoEdget : public GeoEdgetAlloc
    {
    public:
        /** 更新视图点位置
        @remark 通过视图点相应的辐射出边缘
        @version NIIEngine 3.0.0
        */
        void updateView(const Vector4f & pos);
        
        /** 更新这个基于顶点位置的边缘列表的面法线,对于动画对象很有用
        @param[in] vdidx 顶点集
        @param[in] pos
        @version NIIEngine 3.0.0
        */
        void updateNormals(NCount vdidx, const VertexBuffer * pos);

        /** 获取边缘几何
        @remark 相对当前视点
        @version NIIEngine 3.0.0
        */
        void getEdget(GeometryRaw & out);
    public:
        /** 三角形结构
        @version NIIEngine 3.0.0
        */
        struct Triangle
        {
            Triangle() : mIndexDataIndex(0), mVertexDataIndex(0) {}
            NCount mLocalVertIndex[3];
            NCount mVertexDataIndex;
            NCount mIndexDataIndex;
            NCount mVertIndex[3];   ///< 原始缓存的
        };
        
        /** 边缘数据
        @version NIIEngine 3.0.0
        */
        struct Edge
        {
            NCount mLocalVertIndex[2];            
            NCount mTriIndex[2];
            NCount mVertIndex[2];   ///< 原始缓存的
            bool mAlone; 
        };

        typedef vector<Edge>::type EdgeList;

        /** 边缘组
        @version NIIEngine 3.0.0
        */
        struct EdgeGroup
        {
            Nidx mVertexDataIndex;
            VertexData * mVertex;
            EdgeList mEdgeList;
            NCount mOffset;
            NCount mCount;
        };

        typedef vector<EdgeGroup>::type EdgeGroupList;
        typedef vector<Triangle>::type TriangleList;
        typedef vector<NIIb>::type ViewPosList;
        typedef vector<Vector4f, StlAlloc<Vector4f, N_AlignAlloc(16)> >::type NormalsList;
    public:
        EdgeGroupList mGeoList;
        TriangleList mTriangleList;
        NormalsList mNormalsList;
        ViewPosList mViewPosList;
        bool mCloseUp;
    };
    typedef vector<GeoEdget *>::type GeoEdgetList;
}
#endif