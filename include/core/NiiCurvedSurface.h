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

#ifndef _NII_CurvedSurface_H_
#define _NII_CurvedSurface_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CurvedSurface : public GeoAlloc
    {
    public:
        CurvedSurface();
        ~CurvedSurface();

        enum Type
        {
            T_Bezier,     ///< 贝塞尔
            T_Fluid       ///< 流体
        };

        /** 定义面
        @version NIIEngine 3.0.0
        */
        void define(VertexData * ctrlpoint, NCount width, NCount height, Type pType = T_Bezier, 
            NCount uMaxSubLevel = -1, NCount vMaxSubLevel = -1, FaceSideType visibleSide = FST_Front);
                
        /** 构建面
        @version NIIEngine 3.0.0
        */
        void build(VertexBuffer * dstvb, NCount vbbegin, IndexBuffer * dstib, NCount ibbegin);

        /** 获取顶点数量
        @version NIIEngine 3.0.0
        */
        inline NCount getVertexCount() const                {  return mVertexCount; }

        /** 获取索引数量
        @version NIIEngine 3.0.0
        */
        inline NCount getIndexCount() const                 { return mIndexCount; }
        
        /** 设置细分因子.
        @version NIIEngine 3.0.0
        */
        void setSubdivisionFactor(NIIf factor);

        /** 获取细分因子.
        @version NIIEngine 3.0.0
        */
        NIIf getSubdivisionFactor() const                   {return mFactor;}

        /** 获取当前索引数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCurrentVertexCount() const          { return mFVertexCount; }
        
        /** 获取当前索引数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCurrentIndexCount() const          { return mFIndexCount; }

        /** 获取索引偏移量
        @version NIIEngine 3.0.0
        */
        inline NCount getIndexOffset() const                { return mIndexOffset; }

        /** 获取顶点偏移量
        @version NIIEngine 3.0.0
        */
        inline NCount getVertexOffset() const               { return mVertexOffset; }

        /** 获取边界
        @note defineSurface函数后
        @version NIIEngine 3.0.0
        */
        inline const AABox & getAABB() const                { return mAABB; }

        /** 获取中心半径
        @note defineSurface函数后
        @version NIIEngine 3.0.0
        */
        inline NIIf getCenterRange() const                  { return mBounding; }

        /** 获取数据
        @version NIIEngine 3.0.0
        */
        inline VertexBuffer * getData() const               { return mData->getAttach(0); }
    protected:
        void genControlPoint();
        void subdivideCurve(NCount idx, NCount stepSize, NCount cnt, NCount iterations);
        void interpolateVertex(NCount leftIndex, NCount rightIndex, NCount destIndex);
        void makeTriangles(void);

        NCount getAutoULevel(bool forMax = false);
        NCount getAutoVLevel(bool forMax = false);
    protected:
        vector<Vector3f >::type mCtlList;
        VertexData * mData;
        void * mDataRaw;
        Type mType;
        NCount mCtlWidth;
        NCount mCtlHeight;
        NCount mCtlCount;
        NCount mULevel;
        NCount mVLevel;
        NCount mMaxULevel;
        NCount mMaxVLevel;
        NCount mMeshWidth;
        NCount mMeshHeight;
        FaceSideType mVSide;

        NIIf mFactor;

        VertexBuffer * mVertexBuffer;
        IndexBuffer * mIndexBuffer;
        NCount mVertexOffset;
        NCount mIndexOffset;
        NCount mVertexCount;
        NCount mIndexCount;
        NCount mFIndexCount;
        NCount mFVertexCount;

        AABox mAABB;
        NIIf mBounding;
    };
}
#endif