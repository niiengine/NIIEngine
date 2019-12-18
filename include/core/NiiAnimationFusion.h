/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-9-17

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

#ifndef _NII_ANIMATION_FUSION_H_
#define _NII_ANIMATION_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiCommandObj.h"
#include "NiiEventFunctor.h"
#include "NiiAnimation.h"

using namespace NII::NII_COMMAND;

namespace NII
{
    /** 动画对象的动画实际混合(实例)
    @remark 这个用于描述具体对象的具体动画状态
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationFusion : public CommandObj, public AnimableAlloc
    {
        friend class AnimationFusionObj;
    public:
        /** 构造函数
        @param[in] obj 动画容器
        @param[in] ani 动画
        @param[in] pos 开始时间位置(单位秒)
        @param[in] dur 持续时间长度,可能比实际的动画短(单位秒)
        @param[in] weight 这个动画应用时的权重.(并非所有类型动画都有权重概念)
        @version NIIEngine 3.0.0
        */
        AnimationFusion(AnimationObj * obj, Animation * ani, 
            TimeDurMS pos, TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        /** 构造函数
        @version NIIEngine 3.0.0 高级api
        */
        AnimationFusion(AnimationFusionObj * own, const AnimationFusion & o);

        virtual ~AnimationFusion();

        /** 设置时间状态
        @param[in] o 状态源,同个动画
        @version NIIEngine 3.0.0
        */
        void setState(const AnimationFusion & o);

        /** 设置动画是否启用
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** 获取动画是否启用
        @version NIIEngine 3.0.0
        */
        bool isEnable() const;

        /** 获取动画对象体
        @version NIIEngine 3.0.0
        */
        AnimationFusionObj * getParent() const;

        /** 获取使用的动画
        @version NIIEngine 3.0.0
        */
        Animation * getAnimation() const;

        /** 获取使用的动画容器
        @version NIIEngine 3.0.0
        */
        AnimationObj * getAnimationObj() const;

        /** 设置时间因子
        @param[in] f f(y) = x * time
        @version NIIEngine 3.0.0
        */
        void setTimeFactor(NIIf f);

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        NIIf getTimeFactor() const;

        /** 设置播放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setPlayMode(Animation::PlayMode mode);

        /** 获取播放模式
        @version NIIEngine 3.0.0
        */
        Animation::PlayMode getPlayMode() const;

        /** 设置是否自动开始动画
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setAutoStart(bool b);

        /** 获取是否自动开始动画
        @version NIIEngine 3.0.0
        */
        bool isAutoStart() const;

        /** 设置是否自动更新动画
        @version NIIEngine 3.0.0
        */
        void setAutoUpdate(bool b);

        /** 获取是否自动更新动画
        @version NIIEngine 3.0.0
        */
        bool isAutoUpdate() const;

        /** 设置最大时间跨度
        version NIIEngine 3.0.0
        */
        void setMaxCostLimit(TimeDurMS max);

        /** 获取最大时间跨度
        @version NIIEngine 3.0.0
        */
        TimeDurMS getMaxCostLimit() const;

        /** 设置最小时间跨度
        @version NIIEngine 3.0.0
        */
        void setMinCostLimit(TimeDurMS min);

        /** 获取最小时间跨度
        @version NIIEngine 3.0.0
        */
        TimeDurMS getMinCostLimit() const;

        /** 设置是否跳过下一帧更新(只跳一次)
        @remark 主要是用于当开始动画时,避免由于刚开启跨度时间大,导致跳帧.需要跳过下一帧更新
        @version NIIEngine 3.0.0
        */
        void setSkipStep(bool b);

        /** 获取是否跳过下一帧更新(只跳一次)
        @remark 主要是用于当开始动画时,避免由于刚开启跨度时间大,导致跳帧.需要跳过下一帧更新
        @version NIIEngine 3.0.0
        */
        bool getSkipStep() const;

        /** 添加触发信号
        @version NIIEngine 3.0.0
        */
        void addSignal(SignalPtr conn);

        /** 移去所有触发信号
        @version NIIEngine 3.0.0
        */
        void removeSignal();

        /** 动画快进
        @remark 从当前开始快进
        @param[in] oft 毫秒
        @version NIIEngine 3.0.0
        */
        void skip(TimeDurMS oft);

        /** 动画快进
        @remark 从动画头开始快进
        @param[in] pos 毫秒
        @version NIIEngine 3.0.0
        */
        void seek(TimeDurMS pos);

        /** 当前时间位置
        @return 毫秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS tell() const;

        /** 动画是否已经结束,
        @remark 非 RM_Loop 模式
        @version NIIEngine 3.0.0
        */
        bool isEnd() const;

        /** 设置基础状态使用的动画时间点
        @param[in] ms 毫秒
        @version NIIEngine 3.0.0
        */
        void setBeginTime(TimeDurMS ms);

        /** 获取基础状态使用的动画时间点
        @version NIIEngine 3.0.0
        */
        TimeDurMS getBeginTime() const;

        /** 获取动画实例的总长度(可能比整个动画短)
        @version NIIEngine 3.0.0
        */
        TimeDurMS getDur() const;

        /** 设置动画实例的总长度(可能比整个动画短)
        @version NIIEngine 3.0.0
        */
        void setDur(TimeDurMS len);

        /** 设置动画实例的影响(权重)
        @version NIIEngine 3.0.0
        */
        void setWeight(NIIf f);

        /** 获取动画实例的影响(权重)
        @version NIIEngine 3.0.0
        */
        NIIf getWeight() const;

        /** 副本对象
        @version NIIEngine 3.0.0
        */
        virtual AnimationFusion * clone(AnimationFusionObj * own) const = 0;
    public:
        /** 重至混合数据到原状态
        @note
            有部分混合属于过程性混合,也就是说要根据一个公共的对象去计算混合结果,这个每次运算的时候
            都应该回到最当初的状态.就需要继承这个函数
        @version NIIEngine 3.0.0
        */
        virtual void reset();

        /** 更新动画
        @remark 使用实际调用线程执行,或许仅仅更新时间
        @param[in] cost 上次和这次度过的时间
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 开始动画
        @remark 使用实际调用线程执行
        @version NIIEngine 3.0.0
        */
        void start(bool skip = true);

        /** 结束动画
        @remark 使用实际调用线程执行
        @version NIIEngine 3.0.0
        */
        void stop();

        /** 停止动画
        @remark 使用实际调用线程执行
        @version NIIEngine 3.0.0
        */
        void pause();

        /** 继续动画
        @remark 使用实际调用线程执行
        @version NIIEngine 3.0.0
        */
        void resume(bool skip = true);
    public:
        /** 动画开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationStartEvent;

        /** 动画停止事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationStopEvent;

        /** 动画暂停事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationPauseEvent;

        /** 动画恢复事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationResumeEvent;

        /** 动画结束事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationEndEvent;

        /** 动画循环事件
        @version NIIEngine 3.0.0
        */
        static const EventID AnimationLoopEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        /** 开始动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStart();

        /** 停止动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStop();

        /** 暂停动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onPause();

        /** 恢复动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onResume();

        /** 结束动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onEnd();

        /** 循环动画时触发
        @version NIIEngine 3.0.0
        */
        virtual void onLoop();
    public:
        /** 外部控制器
        @version NIIEngine 3.0.0 高级api
        */
        void startCB(const EventArgs * e);

        /** 外部控制器
        @version NIIEngine 3.0.0 高级api
        */
        void stopCB(const EventArgs * e);

        /** 外部控制器
        @version NIIEngine 3.0.0 高级api
        */
        void pauseCB(const EventArgs * e);

        /** 外部控制器
        @version NIIEngine 3.0.0 高级api
        */
        void resumeCB(const EventArgs * e);
    protected:
        AnimationFusionObj * mParent;
        AnimationObj * mAnimationObj;
        Animation * mAnimation;
        TimeDurMS mBegin;               ///< 开始位置(毫秒)
        TimeDurMS mTimePos;             ///< 当前位置(毫秒)
        TimeDurMS mLength;              ///< 持续长度(毫秒)
        TimeDurMS mMaxCostLimit;
        TimeDurMS mMinCostLimit;
        NIIf mTimeFactor;
        NIIf mWeight;
        Animation::PlayMode mPlayMode;
        SignalPtrs mEvents;
        bool mEnable;
        bool mReplayback;
        bool mAutoUpdate;
        bool mSkipStep;
        bool mAutoStart;
    };
}
#endif