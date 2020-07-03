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
#include "NiiBone.h"

namespace NII
{
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
            M_Beg = 0,      ///< 第一
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
        void setBlendMode(Mode m);

        /** 获取动画混合模式
        @version NIIEngine 3.0.0
        */
        Mode getBlendMode() const;

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
        NCount getCount() const;

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
        bool isValidBone() const;

        /** 获取各个骨头矩阵
        @param[in] out 有几个骨头就有几个矩阵,而且是以骨头相应的下标存在的
        @version NIIEngine 3.0.0
        */
        void getFusion(Matrix4f * out) const;

        /** 存在自定义骨头
        @version NIIEngine 3.0.0
        */
        bool isExistCustomBone() const;

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
        virtual const ExtList & getExtList() const;

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
}
#endif