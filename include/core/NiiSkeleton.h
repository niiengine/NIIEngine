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

#ifndef _NII_SKELETON_H_
#define _NII_SKELETON_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiAnimationObj.h"
#include "NiiPosNode.h"

namespace NII
{
    /** 骨骼顶点分配
    @version NIIEngine 3.0.0
    */
    typedef struct _SkeletonVertex
    {
        Nui32 mVertexIndex; ///< 顶点索引
        Nui16 mBoneIndex;   ///< 骨头索引
        NIIf mWeight;       ///< 权重

    } SkeletonVertex;
    typedef multimap<Nidx, SkeletonVertex>::type SkeletonVertexList;

    /** 骨骼里的骨头 .
    @remark 如果还在地球尝试使用三轴传感器完成骨头动画的录制
    @note id机制是局限在一个骨骼里的
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Bone : public PosNode
    {
        friend class Skeleton;
    public:
        Bone(SpaceID id, Skeleton * own);
        ~Bone();

        /** 设置辅助名字
        @version NIIEngine 3.0.0 高级api
        */
        void setName(const String & name)       { mName = name; }

        /** 获取辅助名字
        @version NIIEngine 3.0.0 高级api
        */
        const String & getName() const          { return mName; }

        /** 创建子骨头
        @param[in] id 同个体系(骨骼)中唯一
        @param[in] pos 相对位置
        @param[in] rot 相对方向
        @version NIIEngine 3.0.0
        */
        Bone * create(SpaceID id, const Vector3f & pos = Vector3f::ZERO,
            const Quaternion & rot = Quaternion::IDENTITY);

        /** 设立原始点
        @version NIIEngine 3.0.0
        */
        void origin();

        /** 重至位置
        @remark 重至到设立的原始点
        @version NIIEngine 3.0.0
        */
        void reset();

        /** 设置是否手动控制.
        @version NIIEngine 3.0.0
        */
        void setManual(bool b);

        /** 获取是否手动控制.
        @version NIIEngine 3.0.0
        */
        bool isManual() const           { return mManual; }

        /** 获取初始化位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getOriginPos() const;

        /** 获取初始化方向
        @version NIIEngine 3.0.0
        */
        virtual const Quaternion & getOriginOri() const;

        /** 获取初始化缩放
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getOriginScale() const;

        /** 副本对象
        @remark 复制到指定的骨骼中
        @version NIIEngine 3.0.0 高级api
        */
        Bone * cloneTo(Skeleton * dst) const;

        /// @copydetails PosNode::notify
        void notify(bool force = false);
    protected:
        /// @copydetails PosNode::createImpl
        PosNode * createImpl(SpaceID id = 0);

        /// @copydetails PosNode::destroyImpl
        void destroyImpl(SpaceID id);

        /** 获取骨头当前空间的变换
        @param[out] m 空间的变换
        @version NIIEngine 3.0.0 高级api
        */
        void getFusion(Matrix4f * m) const;
    protected:
        Skeleton * mCreator;
        Vector3f mSrcPosOrg;
        Vector3f mSrcScaleOrg;
        Quaternion mSrcOriOrg;
        Vector3f mSrcPos;
        Vector3f mSrcScale;
        Quaternion mSrcOri;
        String mName;
        bool mManual;
    };

    /** 骨骼
    @remark
        骨骼属于资源级对象,极有可能在多个对象中使用,故然不应该包含当前状态这个概念,
        而保持公共概念.
    @par 使用多个三轴传感器可以构建完整的骨骼
    @par ID 0x05
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Skeleton : public AnimationObj, public Resource
    {
    public:
        /** 骨骼骨头计算的混合类型
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Beg = 0,      ///< 首次
            M_AVG = 2,      ///< 平均
            M_End = 1,      ///< 最后
            M_MUL = 3       ///< 累计
        };
        typedef NII::set<Bone *>::type BoneSet;

        /** 扩展骨骼
        @version NIIEngine 3.0.0
        */
        class Ext : public ResourceAlloc
        {
        public:
            Ext(ResourceID id, NIIf scale) :
                mSkeletonID(id),
                mScale(scale),
                mSkeleton(0){}

            Ext(ResourceID id, NIIf scale, Skeleton * obj) :
                mSkeletonID(id),
                mScale(scale),
                mSkeleton(obj){}
        public:
            ResourceID mSkeletonID;
            Skeleton * mSkeleton;
            NIIf mScale;
        };
        typedef vector<Ext>::type ExtList;
    public:
        /** 构造函数
        @version NIIEngine 3.0.0 高级api
        */
        Skeleton(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);

        virtual ~Skeleton();

        /** 更新骨骼推导的所有变换
        @version NIIEngine 3.0.0
        */
        virtual void _update();

        /** 构建骨骼原始状态
        @version NIIEngine 3.0.0
        */
        virtual void build();

        /** 重至骨骼原始状态
        @version NIIEngine 3.0.0
        */
        virtual void reset(bool all = false);

        /** 设置动画混合模式
        @version NIIEngine 3.0.0
        */
        void setBlendMode(Mode mode)                    { mBlendMode = mode; }

        /** 获取动画混合模式
        @version NIIEngine 3.0.0
        */
        Mode getBlendMode() const                       { return mBlendMode; }

        /** 创建骨头
        @param[in] id 标识
        @version NIIEngine 3.0.0
        */
        Bone * create(Nidx id);

        /** 删除骨头
        @param[in] id 标识
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx id);

        /** 获取骨头
        @version NIIEngine 3.0.0
        */
        Bone * get(Nidx index) const;

        /** 获取骨头数目
        @version NIIEngine 3.0.0
        */
        NCount getCount() const                         { return mBoneList.size(); }

        /** 获取根骨头
        @verison NIIEngine 3.0.0
        */
        Bone * getRoot() const;

        /** 获取根骨头列表
        @verison NIIEngine 3.0.0
        */
        const BoneList & getRootList() const;

        /** 骨头是否处于有效状态
        @version NIIEngine 3.0.0
        */
        bool isValidBone() const                        { return mValidBone; }

        /** 获取各个骨头矩阵
        @param[in] out 有几个骨头就有几个矩阵,而且是以骨头相应的下标存在的
        @version NIIEngine 3.0.0
        */
        void getFusion(Matrix4f * out) const;

        /** 存在自定义骨头
        @version NIIEngine 3.0.0
        */
        bool isCustomBoneExist() const                  { return !mManualBones.empty(); }

        /** 通知自定义骨头位置更新了
        @remark 资源级
        @version NIIEngine 3.0.0
        */
        virtual void _notifyManualUpdate(Bone * obj);

        /** 通知骨头自定义状态变化
        @remark 资源级
        @version NIIEngine 3.0.0
        */
        virtual void _notifyManualChange(Bone * obj);

        /** 获取动画
        @param[in] id 动画的id
        @param[in] ext 是否存在于从扩展骨骼中的
        */
        virtual Animation * getAnimation(AnimationID id, const Ext *& ext) const;

        /** 添加扩展骨骼
        @version NIIEngine 3.0.0
        */
        virtual void addExt(ResourceID rid, NIIf scale = 1.0f);

        /** 移去所有扩展骨骼
        @version NIIEngine 3.0.0
        */
        virtual void removeAllExt();

        /** 获取扩展骨骼
        @version NIIEngine 3.0.0
        */
        virtual const ExtList & getExtList() const      { return mExtList; }

        /// @copydetails AnimationObj::createAnimation
        virtual Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /// @copydetails AnimationObj::getAnimation
        virtual Animation * getAnimation(AnimationID id) const;

        /// @copydetails AnimationObj::_init
        virtual void _init(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_append
        virtual void _append(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_refresh
        virtual void _refresh(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::_update
        virtual void _update(AnimationFusionObj * obj);

        /// @copydetails AnimationObj::optimise
        virtual void optimise();

        /// @copydetails Resource::getWorkFactor
        virtual NCount getWorkFactor() const;
    protected:
        Skeleton(LangID lid = N_PrimaryLang);

        /** 获取根骨头
        @version NIIEngine 3.0.0 高级api
        */
        Bone * getRootBoneImpl() const;

        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    protected:
        Mode mBlendMode;
        ExtList mExtList;
        BoneList mBoneList;
        BoneSet mManualBones;
        mutable BoneList mRootBones;
        bool mValidBone;
    };

    /** 骨骼混合
    @note 这个一个实例对象,所以要保持当前状态这个概念,公共属性概念尽量不要出现
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SkeletonFusion : public Skeleton
    {
    public:
        SkeletonFusion(Skeleton * obj, LangID lid = N_PrimaryLang);
        ~SkeletonFusion();

        /** 更新
        @version NIIEngine 3.0.0
        */
        void update();

        /** 建立连接到骨头的标签节点
        @param[in] obj 需要连接的骨头(属于这个实例的骨头)
        @param[in] pos 连接位置偏量
        @param[in] ori 连接方向偏量
        @version NIIEngine 3.0.0
        */
        TagBone * attach(Bone * obj, const Vector3f & pos = Vector3f::ZERO,
            const Quaternion & ori = Quaternion::IDENTITY);

        /** 解除连接
        @param[in] obj 已经连接的标签节点
        @version NIIEngine 3.0.0
        */
        void detach(TagBone * obj);

        /** 获取骨头的矩阵
        @note 局部空间变换
        @param 骨头有几个就数组就有多大
        @version NIIEngine 3.0.0
        */
        const Matrix4f * getMatrix() const              { return mBoneSpace; }

        /// @copydetails AnimationObj::createAnimation
        Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /// @copydetails AnimationObj::getAnimation
        Animation * getAnimation(Nidx index) const;

        /// @copydetails AnimationObj::getAnimation
        Animation * getAnimation(AnimationID id, const Ext *& ext) const;

        /// @copydetails AnimationObj::removeAnimation
        void removeAnimation(AnimationID id);

        /// @copydetails AnimationObj::getAnimationCount
        NCount getAnimationCount() const;

        /// @see Skeleton::addExt
        void addExt(ResourceID rid, NIIf scale = 1.0f);

        /// @see Skeleton::removeAllExt
        void removeAllExt();

        /// @see Skeleton::getExtList
        const ExtList & getExtList() const;

        /// @see Skeleton::_init
        void _init(AnimationFusionObj * obj);

        /// @see Skeleton::_update
        void _update(AnimationFusionObj * obj);

        /// @see Resource::getOriginID
        ResourceID getOriginID() const;

        /// @see Resource::getPrc
        PrcID getPrc() const;

        /// @see Resource::getGroup
        GroupID getGroup();
    protected:
        /// @copydetails Skeleton::loadImpl
        void loadImpl();

        /// @copydetails Skeleton::unloadImpl
        void unloadImpl();
    protected:
        typedef list<TagBone *>::type TagBoneList;
    protected:
        Skeleton * mSkeleton;
        Matrix4f * mBoneSpace;
        TagBoneList mTagList;
        TagBoneList mFreeTagList;
        Nidx mFreeTagIndex;
    };
}
#endif