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

#ifndef _NII_GEOMETRYRAW_H_
#define _NII_GEOMETRYRAW_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 几何样本预制
    @version NIIEngine 3.0.0
    */
    enum GeometryPrefab
    {
        GSP_None    = 0,
        GSP_Plane   = 1,
        GSP_Cube    = 2,
        GSP_Sphere  = 3,
        GSP_Teapot  = 4,
        GSP_Custom  = 5,
        GSP_Founder = 6
    };

    /** 几何成分类型
    @version NIIEngine 3.0.0
    */
    enum GeometryRawType
    {
        GRT_Normal      = 0x01,
        GRT_Global      = 0x02,
        GRT_Shadow      = 0x04,
        GRT_Index       = 0x08,     ///< 顶点索引
        GRT_Instancing  = 0x10,     ///< 实例顶点
        GRT_AutoVertex  = 0x20,
        GRT_AutoIndex   = 0x40,
        GRT_Edge_XY     = 0x80,     ///< 边缘辅助
        GRT_Edge_XZ     = 0x100,    ///< 边缘辅助
        GRT_Edge_YZ     = 0x200,    ///< 边缘辅助
        GRT_Edge_YX     = 0x400,    ///< 边缘辅助
        GRT_Edge_ZY     = 0x800,    ///< 边缘辅助
        GRT_Edge_ZX     = 0x1000,   ///< 边缘辅助
        GRT_UI          = 0x2000,   ///< UI状态
        GRT_Level_0     = 0x40000,  ///< lod辅助
        GRT_Level_1     = 0x80000,  ///< lod辅助
        GRT_Level_2     = 0x100000, ///< lod辅助
        GRT_Level_3     = 0x200000, ///< lod辅助
        GRT_Level_4     = 0x400000, ///< lod辅助
        GRT_Level_5     = 0x800000, ///< lod辅助
        GRT_Level_6     = 0x1000000,///< lod辅助
        GRT_Level_7     = 0x2000000,///< lod辅助
        GRT_Level_8     = 0x4000000,///< lod辅助
        GRT_Level_9     = 0x8000000 ///< lod辅助(说不定就是个上纹理的四边形)
    };
    typedef Nmark GeoRawTypeMark;

    #define AdjOperationTypeMark 0x100000

    /** 顶点数组几何渲染
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI GeometryRaw : public BufferAlloc
    {
        friend class RenderSys;
    public:
        /** 执行的渲染操作类型
        @version NIIEngine 3.0.0
        */
        enum OperationType
        {
            OT_POINT_LIST       = 1,
            OT_LINE_LIST        = 2,
            OT_LINE_STRIP       = 3,
            OT_LINE_LOOP        = 4,
            OT_QUADS            = 5,
            OT_QUADS_STRIP      = 6,
            OT_POLYGON          = 7,
            OT_TRIANGLE_LIST    = 8,
            OT_TRIANGLE_STRIP   = 9,
            OT_TRIANGLE_FAN     = 10,
            OT_LINE_LIST_ADJ    = OT_LINE_LIST | AdjOperationTypeMark,
            OT_TRIANGLE_LIST_ADJ = OT_TRIANGLE_LIST | AdjOperationTypeMark,
            OT_LINE_STRIP_ADJ   = OT_LINE_STRIP | AdjOperationTypeMark,
            OT_TRIANGLE_STRIP_ADJ = OT_TRIANGLE_STRIP | AdjOperationTypeMark
        };
    public:
        GeometryRaw();
        GeometryRaw(const GeometryRaw & o);
        GeometryRaw(const GeometryRaw & o, NCount oft, NCount cnt);

        GeometryRaw & operator =(const GeometryRaw & o);

        /** 添加附加类型
        @param[in] type 单个或取多个枚举
        @version NIIEngine 3.0.0 高级api
        */
        void add(GeoRawTypeMark type);

        /** 移去附加类型
        @param[in] type 单个或取多个枚举
        @version NIIEngine 3.0.0 高级api
        */
        void remove(GeoRawTypeMark type);

        /** 附加类型码
        @version NIIEngine 3.0.0 高级api
        */
        GeoRawTypeMark getRawMark() const;

        /** 获取顶点数量
        @version NIIEngine 3.0.0
        */
        NCount getVertexCount() const;
    public:
        OperationType mType;        ///< 操作类型
        VertexData * mVertex;       ///< 顶点数据
        IndexData * mIndex;         ///< 索引数据(mMark & GRT_Index)
        NCount mOffset;             ///< 开始绘制处(顶点级偏移)(mMark & GRT_OffsetCount)
        NCount mCount;              ///< 开始绘制数(顶点级数量)(mMark & GRT_OffsetCount)
        NCount mInstanceCount;      ///< 实例数量,着色程序才可控
    private:
        GeoRawTypeMark mMark;///< GeometryRawType 集合
    };
    typedef vector<GeometryRaw *>::type GeometryRawList;
}
#endif