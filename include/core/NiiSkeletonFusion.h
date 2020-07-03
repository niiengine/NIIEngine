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

#ifndef _NII_SKELETON_FUSION_H_
#define _NII_SKELETON_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiSkeleton.h"

namespace NII
{
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
        const Matrix4f * getMatrix() const;

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