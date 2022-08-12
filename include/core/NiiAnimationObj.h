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

#ifndef _NII_ANIMATION_OBJ_H_
#define _NII_ANIMATION_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiAnimation.h"

namespace NII
{
    /** 可动画对象
    @remark 包含多个动画实际混合(实例)
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationFusionObj : public AnimableAlloc
    {
        friend class AnimationManager;
    public:
        typedef map<AnimationID, AnimationFusion *>::type AnimationFusions;
        typedef map<Nid, AnimationFusions>::type ObjFusions;
        typedef vector<AnimationFusion *>::type FusionList;
    public:
        AnimationFusionObj();
        AnimationFusionObj(const AnimationFusionObj & o);
        ~AnimationFusionObj();

        /** 是否存在动画混合
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 添加动画混合
        @note 内存交由本类管理
        @param[in] subid
        @param[in] enable 是否启用动画
        @version NIIEngine 3.0.0
        */
        void addFusion(AnimationFusion * fusion, Nid subid = 0, bool enable = false);

        /** 获取动画混合
        @param[in] aid
        @param[in] subid
        @version NIIEngine 3.0.0
        */
        AnimationFusion * getFusion(AnimationID aid, Nid subid = 0) const;

        /** 是否存在动画混合
        @param[in] aid
        @param[in] id
        @version NIIEngine 3.0.0
        */
        bool isFusionExist(AnimationID aid, Nid id = 0) const;

        /** 移去动画状态
        @param[in] aid
        @param[in] id
        @version NIIEngine 3.0.0
        */
        void removeFusion(AnimationID aid, Nid id = 0);

        /** 移去所有动画状态
        @version NIIEngine 3.0.0
        */
        void removeAllFusion(Nid id);

        /** 移去所有动画状态
        @version NIIEngine 3.0.0
        */
        void removeAllFusion();

        /** 通知动画启用变化
        @version NIIEngine 3.0.0 高级api
        */
        void _notifyDisable(AnimationObjID aid);

        /** 通知动画启用变化
        @version NIIEngine 3.0.0 高级api
        */
        void _notifyEnable(AnimationFusion * obj);

        /** 通知内容或当前帧变化
        @version NIIEngine 3.0.0 高级api
        */
        void _notifyFrame();

        /** 获得当前帧码
        @version NIIEngine 3.0.0 高级api
        */
        Nmark getFrameMark() const { return mFrameMark; }

        /** 获取所有的动画实际混合(实例)
        @version NIIEngine 3.0.0
        */
        AnimationFusions & getFusions(Nid id = 0) const;

        /** 获取有效的动画实际混合(实例)
        @version NIIEngine 3.0.0
        */
        const FusionList & getValidFusions() const { return mValidFusions; }

        /** 从另一个对象中复制状态
        @note 考虑使用同一个对象
        @param[in] src 数据来源
        @version NIIEngine 3.0.0
        */
        void copyState(const AnimationFusionObj * src);
    protected:
        /** 通知禁用所有动画
        @version NIIEngine 3.0.0 高级api
        */
        void _notifyDisable();
    private:
        static AnimationFusions Empty;
    protected:
        N_mutex1
        mutable ObjFusions mFusions;
        FusionList mValidFusions;
        Nmark mFrameMark;
    };

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