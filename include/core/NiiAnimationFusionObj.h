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

#ifndef _NII_ANIMATION_FUSION_OBJ_H_
#define _NII_ANIMATION_FUSION_OBJ_H_

#include "NiiPreInclude.h"

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
        Nmark getFrameMark() const;

        /** 获取所有的动画实际混合(实例)
        @version NIIEngine 3.0.0
        */
        AnimationFusions & getFusions(Nid id = 0) const;

        /** 获取有效的动画实际混合(实例)
        @version NIIEngine 3.0.0
        */
        const FusionList & getValidFusions() const;

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
}
#endif