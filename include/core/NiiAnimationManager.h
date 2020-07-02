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

#ifndef _NII_ANIMATION_MANAGER_H__
#define _NII_ANIMATION_MANAGER_H__

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 动画管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationManager : public Singleton<AnimationManager>, public GeometryAlloc
    {
    public:
        AnimationManager();
        ~AnimationManager();

        /** 更新时间机制
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 获取最后一帧时间控制值
        @version NIIEngine 3.0.0
        */
        DataValue<TimeDurMS> * getLastTimeValue() const;

        /** 获取简单的时间控制函数.
        @version NIIEngine 3.0.0
        */
        DataFunc<TimeDurMS, TimeDurMS> * getNormalTimeFunc() const;

        /** 创建动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        Animation * createAnimation(AnimationObj * obj, AnimationID id, TimeDurMS time, AnimationMode mode);

        /** 获取动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        Animation * getAnimation(AnimationObj * obj, AnimationID id) const;

        /** 获取动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        Animation * getAnimation(AnimationObj * obj, Nidx index) const;

        /** 是否存在动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        bool isAnimationExist(AnimationObj * obj, AnimationID id) const;

        /** 删除动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        void destroyAnimation(AnimationObj * obj, AnimationID id);

        /** 删除所有动画(全局)
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        void destroyAllAnimations();

        /** 获取所有动画
        @note AnimationObj 中很可能已经存在动画, 这个函数是控制动态附加动画
        @version NIIEngine 3.0.0 高级api
        */
        NCount getCount(AnimationObj * obj) const;

        /** 加入混合
        @note 只有把混合实例加入到管理才会更新自动更新内部时间
        @version NIIEngine 3.0.0
        */
        void addFusion(AnimationFusion * fusion);

        /** 删除混合
        @note 只有把混合实例加入到管理才会更新自动更新内部时间
        @version NIIEngine 3.0.0
        */
        void removeFusion(AnimationFusion * fusion);

        /** 删除混合
        @note 只有把混合实例加入到管理才会更新自动更新内部时间
        @version NIIEngine 3.0.0
        */
        void removeAllFusion(AnimationFusionObj * obj);

        /** 删除所有混合
        @note 只有把混合实例加入到这里才会更新自动更新内部时间
        @version NIIEngine 3.0.0
        */
        void removeAllFusion();

        /** 获取混合数量
        @version NIIEngine 3.0.0
        */
        NCount getFusionCount(AnimationFusionObj * obj) const;

        /** 设置时间控制器的相对速度时间.
        @version NIIEngine 3.0.0
        */
        void setTimeFactor(NIIf f);

        /** 返回时间控制器的相对速度时间.
        @version NIIEngine 3.0.0
        */
        NIIf getTimeFactor() const;

        /** 设置一个常量的帧速率.
        @version NIIEngine 3.0.0
        */
        void setFixFrame(TimeDurMS f);

        /** 获取每帧间流逝的时间常量.
        @version NIIEngine 3.0.0
        */
        TimeDurMS getFixFrame() const;

        /** 设置所用的时间.
        @version NIIEngine 3.0.0
        */
        void setCostTime(TimeDurMS cost);

        /** 返回所用的时间.
        @version NIIEngine 3.0.0
        */
        TimeDurMS getCostTime() const;

        /** 设置资源群组
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid);

        /** 获取资源群组
        @version NIIEngine 3.0.0
        */
        GroupID getGroup();

        /** 生成有效ID
        @version NIIEngine 3.0.0
        */
        Nid genValidID();
    private:
        typedef vector<AnimationID>::type AnimationList;
        typedef map<AnimationObj *, AnimationList>::type AniObjList;
        typedef multimap<AnimationObjID, AnimationFusion *>::type AniFusionList;
        typedef map<AnimationFusionObj *, AniFusionList>::type ObjFusionList;
    private:
        GroupID mGroupID;
        Nid mValidID;
        N_mutex(mValidIDMutex)
        N_mutable_mutex(mAnimationsMutex)
        AniObjList mAniObjList;
        ObjFusionList mObjFusionList;
        DataValue<TimeDurMS> * mFrameTimeValue;                
        DataFunc<TimeDurMS, TimeDurMS> * mNormalTimeFunc;
        Nul mLastFrameNumber;
    };
}
#endif