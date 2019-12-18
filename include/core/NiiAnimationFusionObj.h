/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-15

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