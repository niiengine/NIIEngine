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

#ifndef _NII_ANIMATION_H_
#define _NII_ANIMATION_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
	class _EngineAPI AnimationProperty
	{
	public:
		static const PropertyID KeyFrameTrackProperty;
		static const PropertyID KeyFrameProperty;
		static const PropertyID AnimationEventProperty;
		static const PropertyID DurationProperty;
		static const PropertyID ReplayModeAttribute;
		static const PropertyID ReplayModeOnce;
		static const PropertyID ReplayModeLoop;
		static const PropertyID ReplayModeBounce;
		static const PropertyID AutoStartProperty;
		static const PropertyID TargetPropertyAttribute;
		static const PropertyID DataTypeProperty;
		static const PropertyID CalcModeProperty;
		static const PropertyID ValueMethodAbsProperty;
		static const PropertyID ValueMethodRelProperty;
		static const PropertyID ValueMethodMulRelProperty;
		static const PropertyID PositionProperty;
		static const PropertyID ValuePropertySrcProperty;
		static const PropertyID FactorConstantProperty;
		static const PropertyID FactorPointProperty;
		static const PropertyID FactorSquareProperty;
		static const PropertyID FactorSquareRootProperty;
		static const PropertyID EventAttribute;
		static const PropertyID ActionAttribute;
		static const PropertyID PropertyCount;
	};

    /** 处理动画模式
    @version NIIEngine 3.0.0
    */
    enum AnimationMode
    {
        ANIM_None         = 0,
        ANIM_Unit         = 1,
        ANIM_VertexUnit   = 2,
        ANIM_VertexOffset = 3,
        ANIM_PropertyID   = 4,
        ANIM_PosNode      = 5,
        ANIM_Ext1         = 6,
        ANIM_Ext2         = 7,
        ANIM_Ext3         = 8,
        ANIM_Ext4         = 9,
        ANIM_Ext5         = 10,
        ANIM_Ext6         = 11,
        ANIM_Ext7         = 12,
        ANIM_Ext8         = 13,
        ANIM_Ext9         = 14,
        ANIM_Ext10        = 15
    };

    /** 一个动画.
    @remark 由一个或多个关键帧轨迹组合而成
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Animation : public AnimableAlloc
    {
        friend class AnimationFusion;
    public:
        /** 播放模式
        @version NIIEngine 3.0.0
        */
        enum PlayMode
        {
            RM_Once,    ///< 只播放一次
            RM_Loop,    ///< 循环模式
            RM_Bounce   ///< 循环(前进/后退交替)模式
        };

        /** 动作类型
        @version NIIEngine 3.0.0
        */
        enum ActionType
        {
            AT_Start,       ///< 开始
            AT_Stop,        ///< 停止
            AT_Pause,       ///< 暂停
            AT_Resume       ///< 恢复
        };

        typedef map<KeyFrameTrackID, KeyFrameTrack *>::type TrackList;
        typedef multimap<EventID, ActionType>::type Actions;
    public:
        /** 构造函数
        @param[in] id 动画的ID,在父对象里是唯一的
        @param[in] dur 动画的长度
        */
        Animation(AnimationID id, TimeDurMS dur, AnimationMode mode);
        ~Animation();

        /** 获取动画对象的ID
        @version NIIEngine 3.0.0
        */
        AnimationID getID() const;

        /** 获取动画模式
        @version NIIEngine 3.0.0
        */
        AnimationMode getMode() const;

        /** 播放模式
        @param[in] mode 模式
        @version NIIEngine 3.0.0
        */
        void setPlayMode(PlayMode mode);

        /** 播放模式
        @version NIIEngine 3.0.0
        */
        PlayMode getPlayMode() const;

        /** 设置是否自动开始动画
        @param[in] b
        @version NIIEngine 3.0.0
        */
        void setAutoStart(bool b);

        /** 获取是否自动开始动画
        @version NIIEngine 3.0.0
        */
        bool isAutoStart() const;

        /** 设置基础状态使用的动画
        @note 所有动画轨迹加入完成后调用,仅适合同型动画
        @version NIIEngine 3.0.0
        */
        void setBase(AnimationID id, TimeDurMS ms);

        /** 获取基础状态使用的动画
        @note 所有动画轨迹加入完成后调用,仅适合同型动画
        @version NIIEngine 3.0.0
        */
        AnimationID getBaseID() const;

        /** 获取基础状态使用的动画时间点
        @note 所有动画轨迹加入完成后调用,仅适合同型动画
        @version NIIEngine 3.0.0
        */
        TimeDurMS getBaseTime() const;

        /** 设置动画所属
        @param[in] obj 容器对象(非状态对象)
        @version NIIEngine 3.0.0
        */
        inline void setObj(AnimationObj * obj){ mContainer = obj; }

        /** 获取动画所属
        @param[out] obj 容器对象(非状态对象)
        @version NIIEngine 3.0.0
        */
        inline AnimationObj * getObj() const { return mContainer; }

        /** 添加动画轨迹
        @param[in] obj 动画轨迹
        @note 参数的内存将交由本类管理
        @version NIIEngine 3.0.0
        */
        void add(KeyFrameTrack * obj);

        /** 移去动画轨迹
        @param[in] obj 动画轨迹
        @note 确定使用 remove 而不使用 destroy
        @version NIIEngine 3.0.0 高级api
        */
        void remove(KeyFrameTrackID id);

        /** 删除动画轨迹
        @param[in] id 轨迹的ID
        @version NIIEngine 3.0.0
        */
        void destroy(KeyFrameTrackID id);

        /** 获取动画轨迹
        @param[in] id 轨迹的id
        @version NIIEngine 3.0.0
        */
        KeyFrameTrack * get(KeyFrameTrackID id) const;

        /** 动画轨迹是否存在
        @param[in] id 轨迹的id
        @version NIIEngine 3.0.0
        */
        bool isExist(KeyFrameTrackID id) const;

        /** 获取动画轨迹基础元数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取帧时间
        @remark 定点时间
        @version NIIEngine 3.0.0
        */
        TrackTime getTime(TimeDurMS pos) const;

        /** 对象动画机制,添加事件
        @param[in] eid 事件ID
        @param[in] type 动作类型
        @version NIIEngine 3.0.0
        */
        void add(EventID eid, ActionType type);

        /** 对象动画机制,移去事件
        @param[in] eid 事件ID
        @param[in] type 动作类型
        @version NIIEngine 3.0.0
        */
        void remove(EventID eid, ActionType type);

        /** 对象动画机制,连接事件
        @version NIIEngine 3.0.0
        */
        void connect(AnimationFusion * obj);

        /** 对象动画机制,切断事件
        @version NIIEngine 3.0.0
        */
        void disconnect(AnimationFusion * obj);

        /** 设置动画的总长度
        @param[in] dur
        @note 改变一个动画的长度,会使用 AnimationFusion 实例无效,需要重新创建
        @version NIIEngine 3.0.0
        */
        void setDur(TimeDurMS dur);

        /** 获取动画的总长度.
        @remark
        @version NIIEngine 3.0.0
        */
        TimeDurMS getDur() const;

        /** 应用动画到具体混合中
        @remark 主要用于已经指定目标量的轨迹
        @param[in] pos 应用到动画中的时间.
        @param[in] weight 影响力权重,1.0为完全
        @param[in] scale 仅适合空间级的缩放
        @version NIIEngine 3.0.0 高级api
        */
        void apply(AnimationFusion * fusion, TimeDurMS pos, NIIf weight = 1.0, NIIf scale = 1.0f);

        /** 获取动画轨迹
        @version NIIEngine 3.0.0
        */
        const TrackList & getTrack() const;

        /** 获取事件列表
        @version NIIEngine 3.0.0
        */
        const Actions & getEvent() const;

        /** 优化内部结构
        @param non 是否删除无效果的关键帧
        @version NIIEngine 3.0.0
        */
        void optimise(bool non = true);

        /** 创建动画混合实例
        @version NIIEngine 3.0.0
        */
        AnimationFusion * createFusion(AnimationObj * obj, NIIf weight = 1.0f, bool enable = false);

        /** 复制
        @param[in] id 复制出来的动画对象的ID
        @version NIIEngine 3.0.0
        */
        Animation * clone(AnimationID id) const;
    protected:
        AnimationObj * mContainer;  ///< 主容器(非状态对象)
        TrackList mTracks;
        AnimationID mID;
        AnimationID mBaseID;
        AnimationID mBaseTime;
        AnimationMode mMode;
        PlayMode mPlayMode;
        Actions mEvents;
        TimeDurMS mLength;
        bool mAutoStart;
    };
}
#endif