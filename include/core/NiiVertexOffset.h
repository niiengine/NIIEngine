/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-2

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_VERTEX_OFFSET_H_
#define _NII_VERTEX_OFFSET_H_

#include "NiiPreInclude.h"
#include "NiiVertexBuffer.h"
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