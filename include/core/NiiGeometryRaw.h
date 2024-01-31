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
#include "NiiVertexIndexIndirect.h"

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
        GRT_Instancing  = 0x08,         ///< 实例顶点
        GRT_Index       = 0x10,         ///< 顶点索引
        GRT_Indirect    = 0x20,         ///< 绘制索引
        GRT_AutoVertex  = 0x40,
        GRT_AutoIndex   = 0x80,
        GRT_ArrayObject = 0x100,        ///< VAO对象
        GRT_Edge_XY     = 0x200,        ///< 边缘辅助
        GRT_Edge_XZ     = 0x400,        ///< 边缘辅助
        GRT_Edge_YZ     = 0x800,        ///< 边缘辅助
        GRT_Edge_YX     = 0x1000,       ///< 边缘辅助
        GRT_Edge_ZY     = 0x2000,       ///< 边缘辅助
        GRT_Edge_ZX     = 0x4000,       ///< 边缘辅助
        GRT_UI          = 0x8000,       ///< UI状态
        GRT_Level_0     = 0x400000,     ///< lod辅助
        GRT_Level_1     = 0x800000,     ///< lod辅助
        GRT_Level_2     = 0x1000000,    ///< lod辅助
        GRT_Level_3     = 0x2000000,    ///< lod辅助
        GRT_Level_4     = 0x4000000,    ///< lod辅助
        GRT_Level_5     = 0x8000000,    ///< lod辅助
        GRT_Level_6     = 0x10000000,   ///< lod辅助
        GRT_Level_7     = 0x20000000,   ///< lod辅助
        GRT_Level_8     = 0x40000000,   ///< lod辅助
        GRT_Level_9     = 0x80000000    ///< lod辅助(说不定就是个纹理的四边形)
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
            OT_TRIANGLE_LIST    = 4,
            OT_TRIANGLE_STRIP   = 5,
            OT_TRIANGLE_FAN     = 6,
            OT_QUADS            = 7,
            OT_QUADS_STRIP      = 8,
            OT_LINE_LIST_ADJ    = OT_LINE_LIST | AdjOperationTypeMark,
            OT_TRIANGLE_LIST_ADJ = OT_TRIANGLE_LIST | AdjOperationTypeMark,
            OT_LINE_STRIP_ADJ   = OT_LINE_STRIP | AdjOperationTypeMark,
            OT_TRIANGLE_STRIP_ADJ = OT_TRIANGLE_STRIP | AdjOperationTypeMark
        };
    public:
        GeometryRaw();
        GeometryRaw(VertexData * vert, IndexData * idx, IndirectData * idt);
        GeometryRaw(const GeometryRaw & o);
        GeometryRaw(const GeometryRaw & o, NCount oft, NCount cnt);

        GeometryRaw & operator =(const GeometryRaw & o);
        
        /** 顶点数据
        @version NIIEngine 5.0.0
        */
        inline VertexData * getVertex() const       { return mVertex; }
        
        /** 顶点索引数据
        @version NIIEngine 5.0.0
        */
        inline IndexData * getIndex() const         { return mIndex; }
        
        /** 绘制调用索引数据
        @version NIIEngine 5.0.0
        */
        inline IndirectData * getIndirect() const   { return mIndirect; }

        /** 添加附加类型
        @param[in] type 单个或取多个枚举
        @version NIIEngine 3.0.0 高级api
        */
        inline void add(GeoRawTypeMark type)        { mMark |= type; }

        /** 移去附加类型
        @param[in] type 单个或取多个枚举
        @version NIIEngine 3.0.0 高级api
        */
        inline void remove(GeoRawTypeMark type)     { mMark &= ~type; }

        /** 附加类型码
        @version NIIEngine 3.0.0 高级api
        */
        inline GeoRawTypeMark getMark() const       { return mMark; }
        
        /** 设置顶点范围
        @note单位
        @version NIIEngine 3.0.0
        */
        void setVertexRange(NCount oft, NCount count);
        
        /** 获取顶点偏量
        @version NIIEngine 3.0.0
        */
        inline NCount getVertexOffset() const       { return (mMark & GRT_Index) ? mIndex->mOffset + mOffset : mOffset; }

        /** 获取顶点数量
        @version NIIEngine 3.0.0
        */
        inline NCount getVertexCount() const        { return mCount ? mCount : ((mMark & GRT_Index) ? mIndex->mCount : mVertex->mCount); }
        
        /** 获取绘制类型
        @version NIIEngine 3.0.0
        */
        inline OperationType getType() const        { return mType; }

        /**
        @version NIIEngine 3.0.0
        */
        class Operation : public BufferAlloc
        {
            friend class GeometryRaw;
        public:
            enum Type
            {
                Type_Unknow,
                Type_Read,
                Type_Write,
                Type_Compute
            };
            
            Operation() :
                mVType(VT_Count), mType(DT_Float1x), mOpType(Type_Unknow), mData(0), mOffset(0), mBuffer(0),mGpuData(0) {}
                
            Operation(VertexType vtype, Type type) :
                mVType(vtype), mType(DT_Float1x), mOpType(type), mData(0), mOffset(0), mBuffer(0),mGpuData(0) {}

            Type mOpType;
            DataType mType;
            VertexType mVType;
            Nui8 * mData;
            NCount mOffset;
            void * mGpuData;
        };

        typedef vector<Operation>::type RequestList;

        /** 操作锁定
        @version NIIEngine 3.0.0
        */
        static void lock(RequestList & rlist, NCount unitOft = 0, NCount unitCount = 0, bool shadowbuf = false);

        /** 操作解锁
        @version NIIEngine 3.0.0
        */
        static void unlock(RequestList & rlist);
        
        /** 复制
        @version NIIEngine 3.0.0
        */
        GeometryRaw * clone(BufferModeMark vertex = -1, BufferModeMark index = -1, BufferModeMark indirect = -1) const;
    public:
        VertexData * mVertex;       ///< 顶点数据
        IndexData * mIndex;         ///< 索引数据((mMark & GRT_Index) == true)
        IndirectData * mIndirect;   ///< 绘制数据((mMark & GRT_Indirect) == true)
        Nid mVaoId;                 ///< 顶点数组对象((mMark & GRT_ArrayObject) == true)
        Nmark mVaoType;             ///< 顶点数组类型相同代表数据模型相同 ((mMark & GRT_ArrayObject) == true)
        Nmark mRenderID;            ///< 渲染ID相同代表潜在优化渲染 ((mMark & GRT_ArrayObject) == true)
        OperationType mType;        ///< 操作类型
        NCount mOffset;             ///< 开始绘制处(顶点级偏移)(((mMark & GRT_OffsetCount) == true) && ~((mMark & GRT_Indirect) == true))
        NCount mCount;              ///< 从开始到结束绘制数(顶点级数量)((mMark & GRT_OffsetCount) == true) && ((~(mMark & GRT_Indirect)) == true)
        NCount mInstanceOffset;     ///< 实例开始((mMark & GRT_Instancing) == true) && ((~(mMark & GRT_Indirect)) == true)
        NCount mInstanceCount;      ///< 实例数量,着色程序才可控 ((mMark & GRT_Instancing) == true) && ((~(mMark & GRT_Indirect)) == true)
    private:
        GeoRawTypeMark mMark;
    };
    typedef vector<GeometryRaw *>::type GeometryRawList;
}
#endif