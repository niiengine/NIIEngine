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

#ifndef _NII_VERTEX_OFFSET_H_
#define _NII_VERTEX_OFFSET_H_

#include "NiiPreInclude.h"
#include "NiiGpuBuffer.h"
#include "NiiVector3.h"

namespace NII
{
    /** 顶点偏移集
    @remark
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffset : public AnimableAlloc
    {
    public:
        typedef map<Nidx, Vector3f>::type OffsetMap;
        typedef map<Nidx, Vector3f>::type NormalsMap;
    public:
        /** 构造函数
        @version NIIEngine 3.0.0
        */
        VertexOffset(Nid id, Nui16 tid);
        virtual ~VertexOffset();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nid getID() const;

        /** 获取目标ID
        @version NIIEngine 3.0.0
        */
        Nui16 getTarget() const;

        /** 是否包含法线的偏移
        @version NIIEngine 3.0.0
        */
        bool isNormalsValid() const;

        /** 添加偏移
        @param[in] index 顶点索引
        @param[in] offset 这个姿态的位置偏移量
        @version NIIEngine 3.0.0
        */
        void add(Nidx index, const Vector3f & offset);

        /** 添加偏移
        @param[in] index 顶点索引
        @param[in] offset 顶点偏移
        @version NIIEngine 3.0.0
        */
        void add(Nidx index, const Vector3f & offset, const Vector3f & normal);

        /** 移去偏移
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /*** 移去所有偏移
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 位置偏移集
        @version NIIEngine 3.0.0 高级api
        */
        const OffsetMap & getVertexOffsets() const;

        /** 法线偏移集
        @version NIIEngine 3.0.0 高级api
        */
        const NormalsMap & getNormals() const;

        /** 获取顶点偏移量的一个硬件顶点缓存版本
        @version NIIEngine 3.0.0 高级api
        */
        VertexBuffer * getFusion(const VertexData * src) const;

        /** 副本
        @version NIIEngine 3.0.0
        */
        VertexOffset * clone() const;
    protected:
        Nid mID;                        ///<
        Nui16 mTarget;                  ///< 目标
        NormalsMap mNormalsMap;         ///< 法线
        OffsetMap mVertexOffsetMap;     ///< 顶点
        mutable VertexBuffer * mBuffer; ///< 结果副本
    };
}
#endif