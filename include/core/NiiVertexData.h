/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_VERTEXDATA_H_
#define _NII_VERTEXDATA_H_

#include "NiiPreInclude.h"
#include "NiiVertexBuffer.h"
#include "NiiVertexCom.h"
#include "NiiQuaternion.h"
#include "NiiAABox.h"

namespace NII
{
    /** 临时顶点缓存
    @version NIIEngine 3.0.0
    */
    class _EngineInner AniTempBuffer : public TempBufferCtl, public BufferAlloc
    {
    public:
        AniTempBuffer(bool autoDsy = true);
        ~AniTempBuffer();

        /** 设置映射源
        @param[in] src 源缓存
        @version NIIEngine 3.0.0
        */
        void map(const VertexData * src);

        /** 分配副本
        @version NIIEngine 3.0.0
        */
        void alloc(bool pos = true, bool normal = true);

        /** 续定临时缓存
        @version NIIEngine 3.0.0
        */
        bool refill(bool pos = true, bool normal = true) const;

        /** 绑定副本到指定顶点数据中
        @param[in] dst 指定顶点数据
        @param[in] syn 是否同步前后缓存
        @version NIIEngine 3.0.0
        */
        void bind(VertexData * dst, bool syn);

        /// @copydetails TempBufferCtl::expire
        void expire(Buffer * buffer);
    private:
        VertexBuffer * mSrcPos;     ///< 源头指针(位置)
        VertexBuffer * mSrcNrls;    ///< 源头指针(法线)
        VertexBuffer * mDstPos;     ///< 副本指针(位置)
        VertexBuffer * mDstNrls;    ///< 副本指针(法线)
        Nui16 mPosIndex;
        Nui16 mNrlsIndex;
        Nui8 mMark;
    };
    
    /** 硬件动画
    @version NIIEngine 3.0.0
    */
    class _EngineInner HardwareAnimation : public BufferAlloc
    {
    public:
        /** 硬件顶点数据
        @version NIIEngine 3.0.0
        */
        struct Fusion
        {
            NIIf mFactor;
            Nui16 mBufferIndex;
        };
        typedef vector<Fusion>::type FusionList;
    public:
        HardwareAnimation();
        ~HardwareAnimation();

        /** 分配成分
        @version NIIEngine 3.0.0
        */
        NCount alloc(VertexData * dst, NCount cnt, bool nrls);

        /** 释放成分
        @version NIIEngine 3.0.0
        */
        void free(VertexData * dst, NCount cnt, bool nrls);
    public:
        FusionList mFusionData;
        NCount mFusionIndex;
    };

    /** 顶点数据
    @remark
        顶点内部的结构并非一定能用上,而且如果把它们重组也可能带来效率问题,或许在标准
        的顶点结构上每个顶点位置(三个NIIi或NIIf数据)紧接着法向量(三个NIIf数据)再接
        着纹理坐标(两个NIIf数据),一个顶点结构就是这样的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexData : public VertexDataAlloc
    {
        friend class GBufferManager;
    public:
        /** 顶点数据成分
        @version NIIEngine 3.0.0 高级api
        */
        enum Mode
        {
            M_None      = 0,
            M_Animation = 0x01,
            M_ShadowVol = 0x02,
            M_Skeleton  = 0x04,
            M_Normals   = 0x08,
            M_Space     = 0x10
        };

        typedef list<VertexCom>::type Units;
        typedef map<Nui16, VertexBuffer *>::type Attachs;
    public:
        VertexData(GBufferManager * mag = 0);
        ~VertexData();

        /** 数据类型是否相同
        @version NIIEngine 3.0.0
        */
        bool operator== (const VertexData & o) const;

        /** 数据类型是否不同
        @version NIIEngine 3.0.0
        */
        bool operator!= (const VertexData & o) const;

        /** 添加元素
        @param[in] dt 数据类型
        @param[in] vt 顶点类型
        @param[in] bufidx 缓存索引
        @param[in] oft 缓存中的偏移
        @param[in] extidx 复合项索引
        @version NIIEngine 3.0.0
        */
        const VertexCom * add(DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

        /** 插入元素
        @param[in] idx 插入位置
        @param[in] dt 数据类型
        @param[in] vt 顶点类型
        @param[in] bufidx 缓存索引
        @param[in] oft 缓存中的偏移
        @param[in] extidx 复合项索引
        @version NIIEngine 3.0.0
        */
        const VertexCom * insert(Nidx idx, DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

        /** 修改一个元素,位置,和参数像add那样
        @param[in] idx 索引位置
        @param[in] dt 数据
        @param[in] vt 顶点类型
        @param[in] bufidx 缓存索引
        @param[in] oft 缓存中的偏移
        @param[in] extidx 复合类型索引
        @version NIIEngine 3.0.0
        */
        void modify(Nidx idx, DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

        /** 获取单一元素
        @param[in] idx 索引位置
        @version NIIEngine 3.0.0
        */
        const VertexCom * get(Nidx index) const;

        /** 找出元素
        @param[in] vt 顶点类型
        @param[in] extidx 复合类型索引
        @return 如果没找到则返回 0
        @version NIIEngine 3.0.0
        */
        const VertexCom * get(VertexType vt, Nui16 extidx = 0) const;

        /** 给定资源索引获取元素列表.
        @param[in] source 资源索引
        @param[out] out 元素列表.
        @version NIIEngine 3.0.0
        */
        void find(Nui16 source, Units & out) const;

        /** 克隆这个声明
        @version NIIEngine 3.0.0
        */
        void copy(const VertexData * src) const;

        /** 给定索引,移去这个声明中的元素
        @param[in] idx 元素下标
        @version NIIEngine 3.0.0
        */
        virtual void remove(Nidx idx);

        /** 移去给定语义和使用的索引的元素.
        @remark
            在这种情况下,`索引`指的是重复元素的使用的索引例如 纹理坐标.对于其他元
            素这将永远是0,并不是指在载体中索引
        @param[in] vt 数据含义
        @param[in] extidx 一般只有纹理这类数据时才有用,其他数据只是0
        @version NIIEngine 3.0.0
        */
        virtual void remove(VertexType vt, Nui16 extidx = 0);

        /** 移去所有元素
        @version NIIEngine 3.0.0
        */
        virtual void removeAll();

        /** 获取成分个数
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取类型总数
        @param[in] vt
        @version NIIEngine 3.0.0
        */
        NCount getCount(VertexType vt) const;

        /** 获取元素列表
        @verison NIIEngine 3.0.0 高级api
        */
        Units & getUnits() const;

        /** 绑定缓存
        @param[in] bufidx 缓存索引
        @param[in] vb 需要附加的缓存
        @version NIIEngine 3.0.0
        */
        void attach(Nui16 bufidx, VertexBuffer * vb);

        /** 分离已经绑定的缓存
        @param[in] bufidx 缓存索引
        @version NIIEngine 3.0.0
        */
        void detach(Nui16 bufidx);

        /** 给定索引是否绑定了缓存
        @param[in] bufidx 缓存索引
        @version NIIEngine 3.0.0
        */
        bool isAttach(Nui16 bufidx) const;

        /** 获取缓存索引绑定的缓存
        @param[in] bufidx 缓存索引
        @version NIIEngine 3.0.0
        */
        VertexBuffer * getAttach(Nui16 bufidx) const;

        /** 解除所有绑定的缓存
        @version NIIEngine 3.0.0
        */
        void detachAll();

        /** 获取绑定缓存个数
        @version NIIEngine 3.0.0
        */
        NCount getAttachCount() const;

        /** 获取内存绑定
        @version NIIEngine 3.0.0 高级api
        */
        Attachs & getAttachs() const;

        /** 设置辅助缓存
        @version NIIEngine 3.0.0 高级api
        */
        void setExt(VertexBuffer * vb);

        /** 获取辅助缓存
        @version NIIEngine 3.0.0 高级api
        */
        VertexBuffer * getExt() const;
        
        /** 优化排序
        @version NIIEngine 3.0.0 高级api
        */
        void sort();

        /** 优化内存绑定
        @version NIIEngine 3.0.0 高级api
        */
        void opt();

        /** 优化顶点单元
        @remark 仅在内存并没实际绑定时,优化状态(前)
        @note 这将导致顶点声明重新排序
        @version NIIEngine 3.0.0 高级api
        */
        void optUnit();

        /** 优化内存绑定
        @remark 仅在内存并没实际绑定时,优化状态(前)
        @version NIIEngine 3.0.0 高级api
        */
        void optAttach();

        /** 优化有效的绑定
        @version NIIEngine 3.0.0
        */
        void optValidAttach();

        /** 获取空闲的缓存索引
        @version NIIEngine 3.0.0
        */
        Nui16 getFreeBufferIndex() const;

        /** 获取空闲的绑定索引
        @version NIIEngine 3.0.0
        */
        Nui16 getFreeAttachIndex() const;

        /** 构建结构
        @param[in] mode 顶点数据用途
        @param[in] sync 同步之前的数据
        @version NIIEngine 3.0.0
        */
        void build(Nmark mode, bool sync);

        /** 构建结构
        @param[in] mode 顶点数据用途
        @param[in] bmode 缓存使用方式,个数必须等于 mBufferCount.
        @param[in] sync 同步之前的数据
        @version NIIEngine 3.0.0
        */
        void build(Nmark mode, Nmark * bmode, bool sync);

        /** 重新构建结构
        @remark 保留原始数据
        @version NIIEngine 3.0.0
        */
        void rebuild(Nmark mode);

        /** 初始化硬件动画
        @version NIIEngine 3.0.0
        */
        NCount allocHWAni(NCount count, bool normal);

        /** 缓存单元大小
        @param[in] bidx 缓存索引
        @version NIIEngine 3.0.0
        */
        NCount getUnitSize(Nui16 bidx) const;

        /** 计算边界
        @version NIIEngine 3.0.0
        */
        void calcAABB(const Vector3f & pos, const Quaternion & rot, const Vector3f & scale , AABox & out);

        /** 是否有实例顶点
        @remark
            实例顶点也就是同一顶点数据在某一可变(着色程序控制)或不可变的状态执行多次
            绘制(渲染)
        @version NIIEngine 3.0.0
        */
        bool isInstanceValid() const;

        /** 结构数据副本
        @param[in] data 数据副本
        @param[in] mag 产生副本的管理器
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * clone(bool data = true, GBufferManager * mag = 0) const;

        /** 结构副本
        @note 仅结构成分,不含数据副本
        @param[in] swave 是否仅包含cpu可用成分,不含gpu使用成分
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * cloneCom(bool swave = false, GBufferManager * mag = 0) const;
    protected:
        VertexData(const VertexData & o);
        VertexData & operator=(const VertexData & o);
    public:
        NCount mOffset;             ///< 开始顶点
        NCount mCount;              ///< 顶点数量
        HardwareAnimation * mHWAni; ///< 硬件动画
    protected:
        GBufferManager * mMag;      ///< 管理者
        Nui16 mBufferCount;         ///< 缓存数量
        mutable Nui16 mHighIndex;   ///< 缓存最高索引
        mutable Units mUnits;       ///< 顶点成分
        mutable Attachs mAttachs;   ///< 缓存绑定
        VertexBuffer * mExtData;    ///< 扩展缓存
        Nmark mMark;                ///< 辅助
    };
}
#endif