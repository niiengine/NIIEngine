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

#ifndef _NII_VERTEXDATA_H_
#define _NII_VERTEXDATA_H_

#include "NiiPreInclude.h"
#include "NiiBufferManager.h"
#include "NiiGpuBuffer.h"
#include "NiiQuaternion.h"
#include "NiiAABox.h"
#include "NiiColour.h"
#include "NiiDataType.h"

namespace NII
{
    /** 顶点元素的语义,用于识别顶点缓存的内容意义
    @version NIIEngine 3.0.0
    */
    enum VertexType
    {
        VT_Pos              = 1,    ///< 位置,3个NIIf
        VT_Matrix_Weight    = 2,    ///< 混合权重
        VT_Matrix_Index     = 3,    ///< 混合索引
        VT_Normals          = 4,    ///< 法线,3个NIIf
        VT_Diffuse          = 5,    ///< 漫反射颜色
        VT_Specular         = 6,    ///< 镜面反射颜色
        VT_Tex_Coord        = 7,    ///< 纹理坐标
        VT_Space            = 8,    ///< 顶点辅助空间
        VT_Binormal         = 9,    ///< 副法线(Y轴,当法线是Z时)
        VT_Tangent          = 10,   ///< 切线 (X轴,当法线是Z时)
        VT_Count            = 11
    };

    /**
    @remark
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexUnit : public VertexDataAlloc
    {
        friend class VertexData;
    public:
        VertexUnit();

        VertexUnit(DataType dt, VertexType vt, Nui16 bidx, NCount oft);

        VertexUnit(DataType dt, VertexType vt, Nui16 bidx, NCount oft, Nui16 eidx);

        bool operator== (const VertexUnit & o) const;

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const void * in, Nui8 *& out) const { out = (Nui8 *)in + mOffset; }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, void *& out) const { out = (void *)(in + mOffset); }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui8 *& out) const { out = (Nui8 *)(in + mOffset); }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui16 *& out) const { out = (Nui16 *)(in + mOffset);}

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui32 *& out) const { out = (Nui32 *)(in + mOffset);}

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, NIIf *& out) const { out = (NIIf *)(in + mOffset); }

        /** 获取这个元素的字节大小
        @version NIIEngine 3.0.0
        */
        NCount getSize() const;

        /** 转换顶点数据颜色类型
        @param[in] src DT_Colour DT_Colour_ABGR 或 DT_Colour_ARGB.
        @param[in] dst DT_Colour_ABGR 或 DT_Colour_ARGB.
        */
        static void conv(VertexData * obj, DataType src, DataType dest);
    public:
        DataType mType;         ///< 元素类型
        VertexType mVType;      ///< 元素的含义
        NCount mOffset;         ///< 数据偏移
        Nui16 mSource;          ///< 所在的缓存
        Nui16 mIndex;           ///< 项的索引,仅适于复合元素,类纹理坐标元素
    };
    
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

        typedef list<VertexUnit>::type Units;
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
        const VertexUnit * add(DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

        /** 插入元素
        @param[in] idx 插入位置
        @param[in] dt 数据类型
        @param[in] vt 顶点类型
        @param[in] bufidx 缓存索引
        @param[in] oft 缓存中的偏移
        @param[in] extidx 复合项索引
        @version NIIEngine 3.0.0
        */
        const VertexUnit * insert(Nidx idx, DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

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
        const VertexUnit * get(Nidx index) const;

        /** 找出元素
        @param[in] vt 顶点类型
        @param[in] extidx 复合类型索引
        @return 如果没找到则返回 0
        @version NIIEngine 3.0.0
        */
        const VertexUnit * get(VertexType vt, Nui16 extidx = 0) const;

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
        inline NCount getCount() const{ return mUnits.size(); }

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
        inline VertexBuffer * getExt() const{ return mExtData; }
        
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
        NCount allocHWAnimate(NCount count, bool normal);
        
        /** 分配切线
        @version NIIEngine 3.0.0
        */
        void allocTangent(VertexType type, Nui16 index, Nui16 texcoord);

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