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

#ifndef _NII_ANIMATION_FUSION_H_
#define _NII_ANIMATION_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiCommandObj.h"
#include "NiiAnimation.h"
#include "NiiDataEquation.h"

using namespace NII::NII_COMMAND;

namespace NII
{
    /** 动画进度控制器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationDataValue : public DataValue<NIIf>
    {
    public:
        AnimationDataValue(AnimationFusion * target);

        ~AnimationDataValue();

        /** 获取进度值
        @version NIIEngine 3.0.0
        */
        const NIIf getValue() const;

        /** 设置进度值
        @version NIIEngine 3.0.0
        */
        void setValue(const NIIf & value);
    protected:
        AnimationFusion * mTargetFusion;
    };

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
        inline bool isEnable() const { return mEnable;  }

        /** 获取动画对象体
        @version NIIEngine 3.0.0
        */
        inline AnimationFusionObj * getParent() const { return mParent;  }

        /** 获取使用的动画
        @version NIIEngine 3.0.0
        */
        inline Animation * getAnimation() const { return mAnimation;  }

        /** 获取使用的动画容器
        @version NIIEngine 3.0.0
        */
        inline AnimationObj * getAnimationObj() const { return mAnimationObj;  }

        /** 设置时间因子
        @param[in] f f(y) = x * time
        @version NIIEngine 3.0.0
        */
        inline void setTimeFactor(NIIf f) { mTimeFactor = f;  }

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getTimeFactor() const { return mTimeFactor;  }

        /** 设置播放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        inline void setPlayMode(Animation::PlayMode mode) { mPlayMode = mode;  }

        /** 获取播放模式
        @version NIIEngine 3.0.0
        */
        inline Animation::PlayMode getPlayMode() const { return mPlayMode;  }

        /** 设置是否自动开始动画
        @param[in] b
        @version NIIEngine 3.0.0
        */
        inline void setAutoStart(bool b) { mAutoStart = b;  }

        /** 获取是否自动开始动画
        @version NIIEngine 3.0.0
        */
        inline bool isAutoStart() const { return mAutoStart;  }

        /** 设置是否自动更新动画
        @version NIIEngine 3.0.0
        */
        inline void setAutoUpdate(bool b) { mAutoUpdate = b;  }

        /** 获取是否自动更新动画
        @version NIIEngine 3.0.0
        */
        inline bool isAutoUpdate() const { return mAutoUpdate;  }

        /** 设置最大时间跨度
        version NIIEngine 3.0.0
        */
        inline void setMaxCostLimit(TimeDurMS max) { mMaxCostLimit = max;  }

        /** 获取最大时间跨度
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMaxCostLimit() const { return mMaxCostLimit;  }

        /** 设置最小时间跨度
        @version NIIEngine 3.0.0
        */
        inline void setMinCostLimit(TimeDurMS min) { mMinCostLimit = min; }

        /** 获取最小时间跨度
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMinCostLimit() const { return mMinCostLimit;  }

        /** 设置是否跳过下一帧更新(只跳一次)
        @remark 主要是用于当开始动画时,避免由于刚开启跨度时间大,导致跳帧.需要跳过下一帧更新
        @version NIIEngine 3.0.0
        */
        inline void setSkipStep(bool set) { mSkipStep = set;  }

        /** 获取是否跳过下一帧更新(只跳一次)
        @remark 主要是用于当开始动画时,避免由于刚开启跨度时间大,导致跳帧.需要跳过下一帧更新
        @version NIIEngine 3.0.0
        */
        inline bool getSkipStep() const { return mSkipStep; }

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

        /** 设置动画实例的总长度(可能比整个动画短)
        @version NIIEngine 3.0.0
        */
        inline void setDur(TimeDurMS dur) { mLength = dur;  }

        /** 获取动画实例的总长度(可能比整个动画短)
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getDur() const { return mLength;  }

        /** 设置动画实例的影响(权重)
        @version NIIEngine 3.0.0
        */
        void setWeight(NIIf f);

        /** 获取动画实例的影响(权重)
        @version NIIEngine 3.0.0
        */
        inline NIIf getWeight() const { return mWeight;  }

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