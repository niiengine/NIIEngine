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
    enum CopyType
    {
        CT_Clone,
        CT_Ref,
        CT_Struct
    };
    /** 顶点索引
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexData : public BufferAlloc
    {
    public:
        IndexData();
        ~IndexData();

        /** 设置是否自动删除缓存
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b)         { mAutoDestroy = b; }

        /** 获取是否自动删除缓存
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const          { return mAutoDestroy; }

        /** 附加缓存
        @version NIIEngine 3.0.0
        */
        void attach(IndexBuffer * buf);

        /** 分离缓存
        @version NIIEngine 3.0.0
        */
        void detach();

        /** 是否已经附加缓存
        @version NIIEngine 3.0.0
        */
        bool isAttach() const               { return mBuffer != 0; }

        /** 获取附加的缓存
        @version NIIEngine 3.0.0
        */
        IndexBuffer * getAttach() const     { return mBuffer; }

        /** 优化索引,使硬件调用更快
        @remark
            仅用于索引数据(完整三角形链表),在三角形带或其他扇形则无意义
        @version NIIEngine 3.0.0
        */
        void opt();
        
        /** 克隆
        @param[in] ct 数据副本类型
        @param[in] mag 管理器
        @remark 但完成时候,调用者期望 删除和返回 指针
        */
        IndexData * clone(CopyType ct = CT_Clone, BufferModeMark mode = -1, GBufferManager * mag = 0) const;
    protected:
        IndexData(const IndexData & o) {}
        IndexData & operator=(const IndexData &) { return *this; }
    public:
        IndexBuffer * mBuffer;  ///< 缓存主体
        NCount mOffset;         ///< 在缓存中的偏移(单位:单元)
        NCount mCount;          ///< 从开始到结束的数量(顶点索引数量)(单位:单元)
    protected:
        mutable bool mAutoDestroy;
    };

    /** 顶点元素的语义,用于识别顶点缓存的内容意义
    @note 每个slot(bingingpoint)可以绑4个4字节数据,考虑把多个4字节堆成4个4字节数据(不是数组,而是vec4)使用,
        因为gpu软件接口限定顶点成分绑可能只有16个[0, 15]。
    @version NIIEngine 3.0.0
    */
    enum VertexType
    {
        VT_Pos              = 0,    ///< 位置,3个NIIf(固定管线概念)
        VT_Matrix_Weight    = 1,    ///< 混合权重(可编程管线概念)
        VT_Matrix_Index     = 2,    ///< 混合索引(可编程管线概念)
        VT_Normals          = 3,    ///< 法线,3个NIIf(固定管线概念)
        VT_Diffuse          = 4,    ///< 漫反射颜色(固定管线概念)
        VT_Specular         = 5,    ///< 镜面反射颜色(固定管线概念)
        VT_Space            = 6,    ///< 顶点辅助空间(可编程管线概念)(draw buff)
        VT_Tangent          = 7,    ///< 切线 (X轴,当法线是Z时)(可编程管线概念)
        VT_Binormal         = 8,    ///< 副法线(Y轴,当法线是Z时)(可编程管线概念)
        VT_Tex_Coord        = 9,    ///< 纹理坐标(固定管线概念)                   ------texcoord0
        VT_Custom1          = 10,   ///< pre vertex Const/Uniform(可编程管线概念) ------texcoord1
        VT_Custom2          = 11,   ///< pre vertex Const/Uniform(可编程管线概念) ------texcoord2
        VT_Custom3          = 12,   ///< pre vertex Const/Uniform(可编程管线概念) ------texcoord3
        VT_Custom4          = 13,   ///< pre vertex Const/Uniform(可编程管线概念) ------texcoord4
        VT_Custom5          = 14,   ///< pre vertex Const/Uniform(可编程管线概念) ------texcoord5
        VT_Custom6          = 15,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom7          = 16,   ///< pre vertex Const/Uniform(可编程管线概念)-----Gpu api limit
        VT_Custom8          = 17,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom9          = 18,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom10         = 19,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom11         = 20,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom12         = 21,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom13         = 22,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom14         = 23,   ///< pre vertex Const/Uniform(可编程管线概念)
        VT_Custom15         = 24,   ///< pre vertex Const/Uniform(可编程管线概念)
        
        VT_Count            = 25
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

        VertexUnit(DataType dt, VertexType vt, Nui16 bidx, NCount oft, NCount divisor, Nui16 eidx);

        bool operator== (const VertexUnit & o) const;

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const void * in, Nui8 *& out, NCount idxoft) const  { out = (Nui8 *)in + mOffset * (1 + idxoft); }
        inline void getData(const void * in, Nui8 *& out) const                 { out = (Nui8 *)in + mOffset; }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, void *& out, NCount idxoft) const  { out = (void *)(in + mOffset * (1 + idxoft)); }
        inline void getData(const Nui8 * in, void *& out) const                 { out = (void *)(in + mOffset); }

        /** 获取数据
        @param[in] 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui8 *& out, NCount idxoft) const  { out = (Nui8 *)(in + mOffset * (1 + idxoft)); }
        inline void getData(const Nui8 * in, Nui8 *& out) const                 { out = (Nui8 *)(in + mOffset); }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui16 *& out, NCount idxoft) const { out = (Nui16 *)(in + mOffset * (1 + idxoft));}
        inline void getData(const Nui8 * in, Nui16 *& out) const                { out = (Nui16 *)(in + mOffset); }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, Nui32 *& out, NCount idxoft) const { out = (Nui32 *)(in + mOffset * (1 + idxoft));}
        inline void getData(const Nui8 * in, Nui32 *& out) const                { out = (Nui32 *)(in + mOffset); }

        /** 获取数据
        @param[in] in 基础数据
        @param[out] out 实际位置数据
        @version NIIEngine 3.0.0
        */
        inline void getData(const Nui8 * in, NIIf *& out, NCount idxoft) const  { out = (NIIf *)(in + mOffset * (1 + idxoft)); }
        inline void getData(const Nui8 * in, NIIf *& out) const                 { out = (NIIf *)(in + mOffset); }

        /** 获取这个元素的字节大小
        @version NIIEngine 3.0.0
        */
        inline NCount getSize() const                                           { return DataTypeUtil::getSize(mType); }

        /** 转换顶点数据颜色类型
        @param[in] src DT_Colour DT_Colour_ABGR 或 DT_Colour_ARGB.
        @param[in] dst DT_Colour_ABGR 或 DT_Colour_ARGB.
        */
        static void conv(VertexData * obj, DataType src, DataType dest);
    public:
        DataType mType;         ///< 元素类型
        VertexType mVType;      ///< 元素的含义
        NCount mOffset;         ///< 数据偏移(单位:字节)
        NCount mDivisor;        ///< 每个缓存单元能绘制几次图元,0则表示绘制每个顶点单元时都更新数据
        Nui16 mSource;          ///< 所在的缓存
        Nui16 mIndex;           ///< 项的索引,仅适于复合元素,类纹理坐标元素
    };
    typedef vector<VertexUnit>::type VertexUnitList;
    typedef vector<VertexUnitList>::type VertexUnitListList;
    typedef std::pair<OperationType, VertexUnitListList> GeoOperation;
    typedef vector<GeoOperation>::type GeoOperationList;
    
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
        顶点结构上每个顶点位置(三个NIIi或NIIf数据)紧接着法向量(三个NIIf数据)再接着纹理坐标(两个NIIf数据),
        可能还包含漫反射(四个NIIf数据)，镜面反射颜色(四个NIIf数据)，一个顶点结构就是这样的
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

        typedef map<Nui16, VertexUnitList>::type UnitsMap;
        typedef map<Nui16, VertexBuffer *>::type BindingList;
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
        inline const VertexUnit * add(const VertexUnit & unit)
        {
            mUnitList.emplace_back(unit);
            return &(mUnitList.back());
        }

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
        const VertexUnit * insert(Nidx pos, DataType dt, VertexType vt, Nui16 bufidx, NCount oft, Nui16 extidx = 0);

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
        VertexUnit * get(Nidx index) const;

        /** 找出元素
        @param[in] vt 顶点类型
        @param[in] extidx 复合类型索引
        @return 如果没找到则返回 0
        @version NIIEngine 3.0.0
        */
        VertexUnit * get(VertexType vt, Nui16 extidx = 0) const;
        
        /** 找出元素
        @param[in] vt 顶点类型
        @param[in] extidx 复合类型索引
        @return 如果没找到则返回 0
        @version NIIEngine 3.0.0
        */
        VertexUnit * getIndex(VertexType vt, Nidx & globalidx, Nui16 extidx = 0) const;

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
        inline NCount getCount() const              { return mUnitList.size(); }

        /** 获取类型总数
        @param[in] vt
        @version NIIEngine 3.0.0
        */
        NCount16 getCount(VertexType vt) const;
        
        /** 获取单元总数
        @param[in] vt
        @version NIIEngine 3.0.0
        */
        inline NCount getUnitCount() const          { return mBindingList.size() ? mBindingList.begin().second->getUnitCount() : 0; }
        
        /** 给定资源索引获取元素列表.
        @param[in] source 资源索引
        @param[out] out 元素列表.
        @version NIIEngine 3.0.0
        */
        void getUnits(Nui16 source, VertexUnitList & out) const;
        
        /** 给定资源索引获取元素列表.
        @param[in] source 资源索引
        @param[out] out 元素列表.
        @version NIIEngine 3.0.0
        */
        void getUnits(UnitsMap & out) const;
        
        /** 获取元素列表
        @verison NIIEngine 3.0.0 高级api
        */
        const VertexUnitList & getUnits() const              { return mUnitList; }

        /**绑定缓存
        @version NIIEngine 3.0.0
        */
        void attach(const BindingList & list);

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

        /** 获取内存绑定
        @version NIIEngine 3.0.0 高级api
        */
        const BindingList & getAttachs() const      { return mBindingList; }
        
        /** 获取绑定缓存个数
        @version NIIEngine 3.0.0
        */
        NCount getAttachCount() const               { return mBindingList.size(); }

        /** 设置辅助缓存
        @version NIIEngine 3.0.0 高级api
        */
        void setExt(VertexBuffer * vb);

        /** 获取辅助缓存
        @version NIIEngine 3.0.0 高级api
        */
        inline VertexBuffer * getExt() const        { return mExtData; }
        
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
        Nui16 getFreeBufferIndex() const            { return mBufferCount; }

        /** 获取空闲的绑定索引
        @version NIIEngine 3.0.0
        */
        Nui16 getFreeAttachIndex() const            { return mHighIndex++; }

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

        /** 结构数据副本
        @param[in] ct 数据副本类型
        @param[in] mag 产生副本的管理器
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * clone(CopyType ct = CT_Clone, BufferModeMark mode = -1, GBufferManager * mag = 0) const;

        /** 结构副本
        @note 仅结构成分,不含数据副本
        @param[in] swave 是否仅包含cpu可用成分,不含gpu使用成分
        @version NIIEngine 3.0.0 高级api
        */
        VertexData * cloneUnit(bool swave = false, GBufferManager * mag = 0) const;
        
        /** 获取成分总大小
        @version NIIEngine 4.0.0
        */
        static NCount getUnitSize(const VertexUnitList & lt);
        
        /** 获取顶点序列类型
        @version NIIEngine 6.0.0
        */
        static Nidx getVaoType(const GeoOperation & geoOp);
    protected:
        VertexData(const VertexData &) {}
        VertexData & operator=(const VertexData & o) { return *this; }
    public:
        HardwareAnimation * mHWAni; ///< 硬件动画
        NCount mOffset;             ///< 顶点开始位置(单位:单元)
        NCount mCount;              ///< 开始位置到结束数量(顶点数)(单位:单元)
    protected:
        GBufferManager * mMag;
        Nui16 mBufferCount;         ///< 缓存数量
        mutable Nui16 mHighIndex;   ///< 缓存最高索引
        mutable VertexUnitList mUnitList;   ///< 顶点成分
        mutable BindingList mBindingList;   ///< 缓存绑定
        VertexBuffer * mExtData;    ///< 扩展缓存
        Nmark mMark;                ///< 辅助
        static GeoOperationList mGeoLayoutList;
    };
#if N_PLAT == N_PLAT_WIN32
#pragma pack(push, 4)
#endif
    /** 顶点数组绘制结构体
    @version NIIEngine 5.0.0
    */
    typedef struct VertexIndirect
    {
        VertexIndirect() {}
        VertexIndirect(NCount count, NCount instanceCount, NCount oft, NCount instanceOft) :
            mCount(count), mInstanceCount(instanceCount), mOft(oft), mInstanceOft(instanceOft) {}
        N_ALIGNED_DECL(Nui32, mCount, 4);
        N_ALIGNED_DECL(Nui32, mInstanceCount, 4);
        N_ALIGNED_DECL(Nui32, mOft, 4);
        N_ALIGNED_DECL(Nui32, mInstanceOft, 4);
    }N_ALIGNED_STRUCT(4) VertexIndirect;

    /**顶点索引数组绘制结构体
    @version NIIEngine 5.0.0
    */
    typedef struct IndexIndirect
    {
        IndexIndirect() {}
        IndexIndirect(NCount count, NCount instanceCount, NCount oft, NCount vertexOft, NCount instanceOft) :
            mCount(count), mInstanceCount(instanceCount), mOft(oft), mVertexOft(vertexOft), mInstanceOft(instanceOft) {}
        N_ALIGNED_DECL(Nui32, mCount, 4);
        N_ALIGNED_DECL(Nui32, mInstanceCount, 4);
        N_ALIGNED_DECL(Nui32, mOft, 4);
        N_ALIGNED_DECL(Nui32, mVertexOft, 4);
        N_ALIGNED_DECL(Nui32, mInstanceOft, 4);
    }N_ALIGNED_STRUCT(4) IndexIndirect;
#if N_PLAT == N_PLAT_WIN32
#pragma pack(pop) 
#endif

    typedef vector<VertexIndirect>::type VertexIndirectList;
    typedef vector<IndexIndirect>::type IndexIndirectList;

    /** 绘制索引
    @remark IndirectData
    @version NIIEngine 5.0.0
    */
    class _EngineAPI IndirectData : public BufferAlloc
    {
    public:
        IndirectData(IndirectBuffer::Type type);
        ~IndirectData();

        /** 设置是否自动删除缓存
        @version NIIEngine 5.0.0
        */
        void setAutoDestroy(bool b)                 { mAutoDestroy = b; }

        /** 获取是否自动删除缓存
        @version NIIEngine 5.0.0
        */
        bool isAutoDestroy() const                  { return mAutoDestroy; }

        /** 获取类型
        @version NIIEngine 5.0.0
        */
        IndirectBuffer::Type getType() const        { return mType; }

        /** 获取单元大小
        @version NIIEngine 5.0.0
        */
        NCount getUnitSize() const                  { return mType == T_Vertex ? sizeof(VertexIndirect) : sizeof(IndexIndirect); }
        
        /** 获取实例总数量
        @version NIIEngine 5.0.0
        */
        NCount getInstanceCount() const;

        /** 添加
        @note 在IndirectData中 addVertex 和 addIndex 仅能使用其一
        @version NIIEngine 5.0.0
        */
        void addVertex(NCount count, NCount instanceCount, NCount oft, NCount instanceOft);

        /** 移去
        @version NIIEngine 5.0.0
        */
        void removeVertex(Nidx idx);

        /** 移去所有
        @note 在IndirectData中 addVertex 和 addIndex 仅能使用其一
        @version NIIEngine 5.0.0
        */
        void clearAllVertex();

        /** 添加
        @note 在IndirectData中 addVertex 和 addIndex 仅能使用其一
        @version NIIEngine 5.0.0
        */
        void addIndex(NCount count, NCount instanceCount, NCount oft, NCount vertexOft, NCount instanceOft);

        /** 移去
        @version NIIEngine 5.0.0
        */
        void removeIndex(Nidx idx);

        /** 移去所有
        @note IndirectData中 addVertex 和 addIndex 仅能使用其一
        @version NIIEngine 5.0.0
        */
        void clearAllIndex();
        
        /** 附加的缓存
        @version NIIEngine 5.0.0
        */
        void attach(IndirectBuffer * buf);

        /** 分离缓存
        @version NIIEngine 5.0.0
        */
        void detach();

        /** 是否已经附加缓存
        @version NIIEngine 5.0.0
        */
        bool isAttach() const                       { return mBuffer != 0; }

        /** 获取附加的缓存
        @version NIIEngine 5.0.0
        */
        IndirectBuffer * getAttach() const          { return mBuffer; }

        /** 写入数据到附加的缓存(单位字节)
        @note 基于原有的 mOffset
        @version NIIEngine 5.0.0
        */
        void write(NCount oft, NCount srcbufoft, NCount cnt);

        /** 写入数据到附加的缓存(单位单元)
        @note 基于原有的 mOffset
        @version NIIEngine 5.0.0
        */
        void writeUnit(NCount unitoft, NCount srcBufUnitoft, NCount unitcnt)
        {
            NCount unitsize = getUnitSize();
            write(unitoft * unitsize, srcBufUnitoft * unitsize, unitcnt * unitsize);
        }

        /** 从附加缓存中读取数据 (单位字节)
        @note 基于原有的 mOffset
        @version NIIEngine 5.0.0
        */
        void read(NCount oft, NCount dstbufoft, NCount cnt);

        /** 从附加缓存中读取数据(单位单元)
        @note 基于原有的 mOffset
        @version NIIEngine 5.0.0
        */
        void readUnit(NCount unitoft, NCount dstBufUnitoft, NCount unitcnt)
        {
            NCount unitsize = getUnitSize();
            read(unitoft * unitsize, dstBufUnitoft * unitsize, unitcnt * unitsize);
        }
        
        /** 克隆
        @param[in] ct 数据副本类型
        @param[in] mag 管理器
        @remark 但完成时候,调用者期望 删除和返回 指针
        */
        IndirectData * clone(CopyType ct = CT_Clone, BufferModeMark mode = -1, GBufferManager * mag = 0) const;
    protected:
        IndirectData();
        IndirectData(const IndirectData & o) {}
        IndirectData & operator=(const IndirectData &) { return *this; }
    public:
        IndirectBuffer * mBuffer;   ///< 缓存主体(targetBuf)
        IndirectBuffer::Type mType;
        NCount mOffset;             ///< 在缓存中的偏移(单位:单元)
        NCount mCount;              ///< 在缓存中的数量(单位:单元)
    protected:
        VertexIndirectList mVertexIndirectList;
        IndexIndirectList mIndexIndirectList;
        mutable bool mAutoDestroy;
    };
}
#endif