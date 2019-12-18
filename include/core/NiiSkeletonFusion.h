/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-9

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