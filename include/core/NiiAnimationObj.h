/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-11-3

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

#ifndef _NII_ANIMATION_OBJ_H_
#define _NII_ANIMATION_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiAnimation.h"
#include "NiiAnimationFusionObj.h"

namespace NII
{
    /** 动画容器
    @remarks 仅是个动画容器,不代表一定能演义动画,也就是说它本身是个实例模型,而不是实例
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationObj
    {
    public:
        typedef map<AnimationID, Animation *>::type AnimationList;
    public:
        AnimationObj();
        AnimationObj(AnimationObjID aoid);
        virtual ~AnimationObj();

        /** 获取动画容器ID    
        @version NIIEngine 3.0.0
        */
        AnimationObjID getAniObjID() const;

        /** 创建动画
        @param[in] id 动画ID
        @param[in] dur 动画长度
        @version NIIEngine 3.0.0
        */
        virtual Animation * createAnimation(AnimationID id, TimeDurMS dur);

        /** 获取动画
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        virtual Animation * getAnimation(Nidx index) const;

        /** 获取动画
        @param[in] id 动画ID
        @version NIIEngine 3.0.0
        */
        virtual Animation * getAnimation(AnimationID id) const;

        /** 获取动画并移出管理
        @param[in] id 动画ID
        @version NIIEngine 3.0.0 高级api
        */
        virtual Animation * getAnimationPop(AnimationID id);

        /** 是否存在动画
        @param[in] id 动画ID
        @version NIIEngine 3.0.0
        */
        virtual bool isAnimationExist(AnimationID id) const;

        /** 移去动画
        @param[in] id 动画ID
        @version NIIEngine 3.0.0
        */
        virtual void removeAnimation(AnimationID id);

        /** 移去所有动画
        @version NIIEngine 3.0.0
        */
        virtual void removeAllAnimations();

        /** 获取这个容器中的动画数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getAnimationCount() const;

        /** 是否存在顶点混合
        @remark 可以帮助系统判断是否要创建顶点临时副本
        @version NIIEngine 3.0.0
        */
        virtual bool isVertexFusion() const;

        /** 是否存在顶点法线混合
        @remark 可以帮助系统判断是否要创建顶点临时副本
        @version NIIEngine 3.0.0
        */
        virtual bool isNormlFusion() const;

        /** 是否存在骨骼混合
        @remark 可以帮助系统判断是否要创建顶点临时副本
        @version NIIEngine 3.0.0
        */
        virtual bool isSkeletonFusion() const;

        /** 优化所有动画
        @version NIIEngine 3.0.0
        */
        virtual void optimise();

        /** 注入动画数据关系
        @remark 重至方式
        @param[in] obj 实例对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _init(AnimationFusionObj * obj);

        /** 附加动画数据关系
        @remark 附加方式
        @param[in] obj 实例对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _append(AnimationFusionObj * obj);

        /** 刷新动画数据关系
        @remark 刷新方式
        @param[in] obj 实例对象
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _refresh(AnimationFusionObj * obj);

        /** 刷新一个动画关系
        @remark 更新方式
        @version NIIEngine 3.0.0 高级api
        */
        virtual void _update(AnimationFusionObj * obj);
    protected:
        static Nid mValidID;
        AnimationObjID mAniObjId;
        AnimationList mAnimations;  ///< 动画列表
    };
}

#endif