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

#ifndef _NII_GAME_OBJECT_H_
#define _NII_GAME_OBJECT_H_

#include "Nii.h"
#include "NiiGameEffect.h"
#include "NiiGameActionScene.h"

namespace NIIGame
{
    /** 这个对象是描述出现在游戏中的具体的一个事件或物体通过继承这个可以抽象出游戏
        中所有需要的任何物体或事件
    @remark 这是游戏中的最高对象层,这个也是一个游戏中的应用层
    @par
        这个是应用层的对象类,也就是说如操作显卡,声卡,和文件系统这一事件不要出现在这
        里对象里面
    @par 如果要添加具体显卡,声卡,和文件的操作请查看 xxxxSystem.xxx
    @version NIIEngineGame 1.0.0
    */
    class _NIIGameExport GameObj : public CommandObj, public EventObj, public GameAlloc
    {
        friend class ActionScene;
    public:
        typedef list<ActionScene *>::type SceneList;
        typedef map<Nid, GeometryObj *>::type ModeList;
        typedef map<NCount, ControlItem *>::type ControlItems;
        typedef map<NCount, InternetItem *>::type InternetItems;
    public:
        GameObj();
        GameObj(ActionScene * own);
        virtual ~GameObj();

        /** 返回创建这个游戏对象的动作场景
        @version NIIEngineGame 1.0.0
        */
        ActionScene * getParent() const;

        /** 启用纹理技术
        @param mark GameEffect_Texture 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnTextureTech(Nmark mark);

        /** 阴影级应用
        @param mark GameEffect_Shadow 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnShadowTech(Nmark mark);

        /** 光照级应用
        @param mark GameEffect_Light 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnLightTech(Nmark mark);

        /** 模型级应用
        @param mark GameEffect_Model 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnModelTech(Nmark mark);

        /** 事件级应用
        @param mark GameEffect_Event 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnEventTech(Nmark mark);

        /** 声音级应用
        @param mark GameEffect_Voice 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnVoiceTech(Nmark mark);

        /** 输入控制器效果
        @param mark PatternTech_Control 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnControlTech(Nmark mark);

        /** 物理逻辑效果
        @param mark PatternTech_Control 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnPhysicalTech(Nmark mark);

        /** 纹理级应用
        @param mark GameEffect_Texture 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffTextureTech(Nmark mark);

        /** 阴影级应用
        @param mark GameEffect_Shadow 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffShadowTech(Nmark mark);

        /** 光照级应用
        @param mark GameEffect_Light 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffLightTech(Nmark mark);

        /** 模型级应用
        @param mark GameEffect_Model 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffModelTech(Nmark mark);

        /** 事件级应用
        @param mark GameEffect_Event 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffEventTech(Nmark mark);

        /** 声音级应用
        @param mark GameEffect_Voice 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffVoiceTech(Nmark mark);

        /** 输入控制器效果
        @param mark PatternTech_Control 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffControlTech(Nmark mark);

        /** 物理逻辑效果
        @param mark PatternTech_Control 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffPhysicalTech(Nmark mark);

        /** 公共级别
        @param mark GameEffect 单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OnGameEffect(Nmark mark);

        /** 公共级别
        @param mark GameEffect,单个或多个成分组合
        @return 设置成功则返回 true
        */
        virtual bool OffGameEffect(Nmark mark);

        /** 立即执行
        @version NIIEngineGame 1.0.0
        */
        virtual bool action();

        ///@copydetails EventObj::create
        virtual MemberFunctor * create(EventID id) const;
    protected:
        /** 默认更新
        @version NIIEngineGame 1.0.0
        */
        virtual void _update();

        /** 在这个对象创建后触发
        @param target 创建这个场景对象的动作场景
        @version NIIEngineGame 1.0.0
        */
        virtual void onCreated(const EventArgs * arg);

        /** 在这个对象删除时触发
        @param target 删除这个游戏对象的动作场景
        @version NIIEngineGame 1.0.0
        */
        virtual void onDestroyed(const EventArgs * arg);

        /** 在这个对象完全被析构掉的时候触发
        @remark 这个方法告诉用户对象已经完全不存在了,如果要继续使用它必须再次调用初始化方法
        @version NIIEngineGame 1.0.0
        */
        virtual void onDeleted(const EventArgs * arg);

        /** 在对象显示的时候触发
        @remark
            这个方法是在每帧都执行的,所以要注意函数性能问题,这个方法合适分割可见和不可
            见时需要更新的数据方法,从而提升整体性能
        @version NIIEngineGame 1.0.0
        */
        virtual void onShown(const EventArgs * arg);

        /** 当对焦到这个对象后,鼠标按下的时候触发
        @remark 当鼠标选中了这个对象的时候触发,可以通过设置选中类型是大体或者精细选中
        @param usrdata 调用事件用的额外数据
        @version NIIEngineGame 1.0.0
        */
        virtual void onOverlayPress(const EventArgs * arg);

        /** 当对焦到这个对象后,鼠标放开的时候触发
        @remark
            当鼠标从当前选中的对象中离开并且又单击了鼠标,但选中的是空白或是另一个对象
            的时候触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onOverlayRelease(const EventArgs * arg);

        /** 在鼠标移动到这个对象内时触发
        @remark
            这个方法需要去设置ListenMark,默认是不监听这个事件的,当这个事件有效时,因为
            每帧都需要去执行,存在效率问题
        @version NIIEngineGame 1.0.0
        */
        virtual void onOverlayEnter(const EventArgs * arg);

        /** 在鼠标移动到这个对象外的时候触发
        @remark
            这个方法需要去设置ListenMark,默认是不监听这个事件的,当这个事件有效时,因为
            每帧都需要去执行,存在效率问题
        @version NIIEngineGame 1.0.0
        */
        virtual void onOverlayLeave(const EventArgs * arg);

        /** 当对焦到这个对象后,按下键盘的时候触发
        @remark 对焦后将会监听用户按下的键盘消息
        @version NIIEngineGame 1.0.0
        */
        virtual void onInputPress(const EventArgs * arg);

        /** 当对焦到这个对象后,松开键盘的时候触发
        @remark 对焦后将会监听用户松开的键盘消息
        @version NIIEngineGame 1.0.0
        */
        virtual void onInputLeave(const EventArgs * arg);

        /** 从没有执行动画到执行动画的时候触发
        @remark 在对象第一个动画开始时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onPreAnimate(const EventArgs * arg);

        /** 在执行动画期间触发
        @remark 这个方法是在对象执行动画期间的每帧中执行
        @version NIIEngineGame 1.0.0
        */
        virtual void onAnimating(const EventArgs * arg);

        /** 在一个动画结束后触发
        @remark
            这描述的是一个系列的动画走到最后才发生的,循环动画也是,如此,也就是说每个动
            画轨迹的最后一帧中都会执行这个方法
        @version NIIEngineGame 1.0.0
        */
        virtual void onAnimateEnd(const EventArgs * arg);

        /** 在一个动画状态改变的时候触发
        @remark 在动画开始到结束,或到转到另一个动画的时候会触发这个事件
        @version NIIEngineGame 1.0.0
        */
        virtual void onAnimateStateChange(const EventArgs * arg);

        /** 在一个动画中断的时候触发
        @remark 一般是一个对象在执行动画期间(还没结束),突然执行跳转到0.0时间处的状态
        @version NIIEngineGame 1.0.0
        */
        virtual void onAnimateStateInterrupt(const EventArgs * arg);

        /** 当使用的阴影技术设置改变时触发
        @remark 底层的阴影技术在运行时改变会触发这个事件
        @param apply 所应用的阴影技术
        @par NO 32(16-255)
        */
        virtual void onShadowTechChanged(const EventArgs * arg);

        /** 当使用的纹理技术设置改变时触发
        @remark 运行时的状态改变
        @version NIIEngineGame 1.0.0
        */
        virtual void onTextureTechChanged(const EventArgs * arg);

        /** 当使用的灯光技术设置改变时触发
        @remark 运行时的状态改变
        @version NIIEngineGame 1.0.0
        */
        virtual void onLightTechChanged(const EventArgs * arg);

        /** 当使用的模型技术设置改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onModelTechChanged(const EventArgs * arg);

        /** 当使用的事件技术设置改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onPhysicalTechChanged(const EventArgs * arg);

        /** 当使用的事件技术设置改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onEventTechChanged(const EventArgs * arg);

        /** 当使用的声音技术设置改变时触发
        @remark 运行时的状态改变
        @version NIIEngineGame 1.0.0
        */
        virtual void onVoiceTechChanged(const EventArgs * arg);

        /** 在子对象分离的时候触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onChildDetached(const EventArgs * arg);

        /** 在子对象在运行时附加到对象时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onChildAttached(const EventArgs * arg);

        /** 当对象的位置发生改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onPositionChanged(const EventArgs * arg);

        /** 当对象的方向发生改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onDirectionChanged(const EventArgs * arg);

        /** 当对象的缩放改变时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onScaleChanged(const EventArgs * arg);

        /** 当一个对象的空间发生改变时触发
        @remark 一个对象可以使用局部空间或者全局(世界)空间,当这种状态发生
            改变的时候触发
        @param usrdata 调用事件用的额外数据
        @par NO 43(16-255)
        */
        virtual void onRelationChanged(const EventArgs * arg);

        /** 当对象的子对象发生状态改变的时候触发
        @remark 在对象第一个动画开始时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onChildStateChanged(const EventArgs * arg);

        /** 从没有执行声音播放到执行声音播放的时候触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onPreVoice(const EventArgs * arg);

        /** 在执行播放声音期间触发
        @remark 在声音播放期间每帧都执行的回调
        @version NIIEngineGame 1.0.0
        */
        virtual void onVoicing(const EventArgs * arg);

        /** 在声音播放完后触发
        @remark 对象的发出的声音结束,而且下一刻后不继续播放
        @version NIIEngineGame 1.0.0
        */
        virtual void onVoiceEnd(const EventArgs * arg);

        /** 在一个播放中的声音状态改变的时候触发
        @remark 在声音播放开始到结束,或到转到另一个动画的时候会触发这个事件
        @version NIIEngineGame 1.0.0
        */
        virtual void onVoiceStateChanged(const EventArgs * arg);

        /** 在一个播放中的声音中断的时候触发
        @remark 一般是一个对象在播放声音期间(还没结束),突然执行跳转到0.0时
            间处的状态
        @version NIIEngineGame 1.0.0
        */
        virtual void onVoicedInterrupted(const EventArgs * arg);

        /** 物体空间冲突前触发
        @remark 通过这个函数可以配置物理引擎
        @version NIIEngineGame 1.0.0
        */
        virtual void onPrePhysicalEvent(const EventArgs * arg);

        /** 物体空间冲突后触发
        @remark
        @version NIIEngineGame 1.0.0
        */
        virtual void onPhysicalEventEnd(const EventArgs * arg);

        /** 对象从宿主对象中分离时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onDetached(const EventArgs * arg);

        /** 对象附加到宿主对象中时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onAttached(const EventArgs * arg);

        /** 对象被摄象机对焦的时触发
        @remark
            对焦事件驱动一般都是由游戏对象产生的,当重写这个方法的时候,尽量继承父类的
            这个方法,从而激活内部的控制驱动
        @version NIIEngineGame 1.0.0
        */
        virtual void onFocused(const EventArgs * arg);

        /** 对象在摄象机中释放焦点时触发
        @remark
            对焦事件驱动一般都是由游戏对象产生的,当重写这个方法的时候,尽量继承父类的
            这个方法,从而释放内部的控制驱动
        @version NIIEngineGame 1.0.0
        */
        virtual void onReleased(const EventArgs * arg);

        /** 当对象接收来自外网数据时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onInternetIn(const EventArgs * arg);

        /** 当对象某些数据发送到外网时触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onInternetOut(const EventArgs * arg);

        /** 在动画数据更新期间中触发
        @remark 在执行每个动画数据成份更新前触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onAnimateUpdating(const EventArgs * arg);

        /** 在动画数据更新前触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onPreUpdateAnimate(const EventArgs * arg);

        /** 在动画数据更新完后触发
        @version NIIEngineGame 1.0.0
        */
        virtual void onUpdateAnimateEnd(const EventArgs * arg);

        /** 渲染前触发(批次级技术不触发)
        @version NIIEngineGame 1.0.0
        */
        virtual void onPreRender(const EventArgs * arg);

        /** 渲染期间触发(每个模型触发一次,批次级技术不触发)
        @version NIIEngineGame 1.0.0
        */
        virtual void onRendering(const EventArgs * arg);

        /** 渲染结束触发(批次级技术不触发)
        @version NIIEngineGame 1.0.0
        */
        virtual void onRenderEnd(const EventArgs * arg);
    private:
        /** 效验这个对象的完整性
        @param force
            强制补这个对象的全完整性,除非使用了完整的补全策略,不然尽量不要为true
        @param rel 效验这个对象的关系对象是否完整性
        */
        virtual bool check(bool force = false, bool rel = false);
    public:
        /** 这个游戏对象在创建后的第一瞬间触发
        @remark 对象都应该由动作场景去创建
        @version NIIEngine 
        */
        static const EventID CreatedEvent;

        /** 在这个游戏对象从所有动作场景中移去的时候触发
        @remark
            但此时可能还有其他的场景正在引用它,此时并没有真正的把对象析构掉,但这个方
            法可能会把这个游戏对象完全蒸发掉,这个依赖于具体的设计
        @
        */
        static const EventID DestroyedEvent;

        /** 在这个对象完全被析构掉的时候触发
        @remark
            这个方法告诉用户对象已经完全不存在了,如果要继续使用它必须再次调用初始化方
            法
        @par NO 18(16-255)
        */
        static const EventID DeletedEvent;

        /** 在对象显示的时候触发
        @remark
            这个方法是在每帧都执行的,所以要注意函数性能问题,这个方法合适分割可见和不可
            见时需要更新的数据方法,从而提升整体性能
        @par NO 21(16-255)
        */
        static const EventID ShownEvent;

        /** 当对焦到这个对象后,鼠标按下的时候触发
        @remark 当鼠标选中了这个对象的时候触发,可以通过设置选中类型是大体或者精细选中
        @par NO 19(16-255)
        */
        static const EventID OverlayPressEvent;

        /** 当对焦到这个对象后,鼠标放开的时候触发
        @remark
            当鼠标从当前选中的对象中离开并且又单击了鼠标,但选中的是空白或是另一个对象
            的时候触发
        @par NO 20(16-255)
        */
        static const EventID OverlayReleaseEvent;

        /** 在到这个对象内时触发
        @remark
            这个方法需要去设置ListenMark,默认是不监听这个事件的,当这个事件有效时,因为
            每帧都需要去执行,存在效率问题
        @par NO 23(16-255)
        */
        static const EventID OverlayEnterEvent;

        /** 在到这个对象外的时候触发
        @remark
            这个方法需要去设置ListenMark,默认是不监听这个事件的,当这个事件有效时,因为
            每帧都需要去执行,存在效率问题
        @par NO 24(16-255)
        */
        static const EventID OverlayLeaveEvent;

        /** 当对焦到这个对象后,按下键盘的时候触发
        @remark 对焦后将会监听用户按下的键盘消息
        @par NO 25(16-255)
        */
        static const EventID InputPressEvent;

        /** 当对焦到这个对象后,松开键盘的时候触发
        @remark 对焦后将会监听用户松开的键盘消息
        @par NO 26(16-255)
        */
        static const EventID InputReleaseEvent;

        /** 从没有执行动画到执行动画的时候触发
        @remark 在对象第一个动画开始时触发
        @par NO 27(16-255)
        */
        static const EventID PreAnimateEvent;

        /** 在执行动画期间触发
        @remark 这个方法是在对象执行动画期间的每帧中执行
        @par NO 28(16-255)
        */
        static const EventID AnimatingEvent;

        /** 在一个动画结束后触发
        @remark
            这描述的是一个系列的动画走到最后才发生的,循环动画也是,如此,也就是说每个动
            画轨迹的最后一帧中都会执行这个方法
        @par NO 29(16-255)
        */
        static const EventID AnimateEndEvent;

        /** 在一个动画状态改变的时候触发
        @remark 在动画开始到结束,或到转到另一个动画的时候会触发这个事件
        @par NO 30(16-255)
        */
        static const EventID AnimateStateChangeEvent;

        /** 在一个动画中断的时候触发
        @remark 一般是一个对象在执行动画期间(还没结束),突然执行跳转到0.0时间处的状态
        @par NO 31(16-255)
        */
        static const EventID AnimateStateInterruptEvent;

        /** 当使用的阴影技术设置改变时触发
        @remark 底层的阴影技术在运行时改变会触发这个事件
        @par NO 32(16-255)
        */
        static const EventID ShadowTechChangedEvent;

        /** 当使用的纹理技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 33(16-255)
        */
        static const EventID TextureTechChangedEvent;

        /** 当使用的灯光技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 34(16-255)
        */
        static const EventID LightTechChangedEvent;

        /** 当使用的模型技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 35(16-255)
        */
        static const EventID PhysicalTechChangedEvent;

        /** 当使用的模型技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 35(16-255)
        */
        static const EventID ModelTechChangedEvent;

        /** 当使用的事件技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 36(16-255)
        */
        static const EventID EventTechChangedEvent;

        /** 当使用的声音技术设置改变时触发
        @remark 运行时的状态改变
        @par NO 37(16-255)
        */
        static const EventID VoiceTechChangedEvent;

        /** 在一个子对象分离的时候触发
        @par NO 38(16-255)
        */
        static const EventID ChildDetachedEvent;

        /** 在一个子对象在运行时附加到对象时触发
        @par NO 39(16-255)
        */
        static const EventID ChildAttachedEvent;

        /** 当一个对象的位置发生改变时触发
        @par NO 40(16-255)
        */
        static const EventID PositionChangedEvent;

        /** 当一个对象的方向发生改变时触发
        @par NO 41(16-255)
        */
        static const EventID DirectionChangedEvent;

        /** 当一个对象的缩放改变时触发
        @par NO 42(16-255)
        */
        static const EventID ScaleChangedEvent;

        /** 当一个对象的空间发生改变时触发
        @remark
            一个对象可以使用局部空间或者全局(世界)空间,当这种状态发生改变的时候触发
        @par NO 43(16-255)
        */
        static const EventID RelationChangedEvent;

        /** 当一个对象的子对象发生状态改变的时候触发
        @remark 在对象第一个动画开始时触发
        @par NO 44(16-255)
        */
        static const EventID ChildStateChangedEvent;

        /** 从没有执行声音播放到执行声音播放的时候触发
        @par NO 45(16-255)
        */
        static const EventID PreVoiceEvent;

        /** 在执行播放声音期间触发
        @remark 在声音播放期间每帧都执行的回调
        @par NO 46(16-255)
        */
        static const EventID VoicingEvent;

        /** 在声音播放完后触发
        @remark 对象的发出的声音结束,而且下一刻后不继续播放
        @par NO 47(16-255)
        */
        static const EventID VoiceEndEvent;

        /** 在一个播放中的声音状态改变的时候触发
        @remark 在声音播放开始到结束,或到转到另一个动画的时候会触发这个事件
        @par NO 48(16-255)
        */
        static const EventID VoiceStateChangedEvent;

        /** 在一个播放中的声音中断的时候触发
        @remark 一般是一个对象在播放声音期间(还没结束),突然执行跳转到0.0时
            间处的状态
        @par NO 49(16-255)
        */
        static const EventID VoicedInterruptedEvent;

        /** 物体空间冲突前触发
        @remark 通过这个函数可以配置物理引擎
        @par NO 50(16-255)
        */
        static const EventID PrePhysicalEvent;

        /** 物体空间冲突后触发
        @par NO 51(16-255)
        */
        static const EventID PhysicalEndEvent;

        /** 对象从宿主对象中分离时触发
        @par NO 52(16-255)
        */
        static const EventID DetachedEvent;

        /** 对象附加到宿主对象中时触发
        @par NO 53(16-255)
        */
        static const EventID AttachedEvent;

        /** 对象被摄象机对焦的时触发
        @par NO 54(16-255)
        */
        static const EventID FocusedEvent;

        /** 对象在摄象机中释放焦点时触发
        @par NO 55(16-255)
        */
        static const EventID ReleasedEvent;

        /** 当对象接收网络数据时触发
        @par NO 56(16-255)
        */
        static const EventID InternetInEvent;

        /** 当对象发送网络数据时触发
        @par NO 57(16-255)
        */
        static const EventID InternetOutEvent;
        
        /** 在动画数据更新前触发
        @par NO 58(16-255)
        */
        static const EventID PreUpdateAnimateEvent;
        
        /** 在动画数据更新期间中触发
        @par NO 59(16-255)
        */
        static const EventID AnimateUpdatingEvent;

        /** 在动画数据更新完后触发
        @par NO 60(16-255)
        */
        static const EventID UpdateAnimateEndEvent;

        /** 从不渲染到渲染前触发
        @par NO 61(16-255)
        */
        static const EventID PreRenderEvent;

        /** 渲染期间触发
        @par NO 62(16-255)
        */
        static const EventID RenderingEvent;

        /** 对象的渲染结束触发
        @par NO 63(16-255)
        */
        static const EventID RenderEndEvent;

        ///
        static const EventID EventCount;
    protected:
        ActionScene * mParent;              ///< 这个对象的父对象的句柄(仅充许一个父对象)
        Nid mId;                            ///< 这个对象的唯一句柄,有这个句柄可编写外部脚本控制
        ModeList mModes;
        SceneList mScenes;
        NIIi mShadowTechMark; 
        NIIi mTextureTechMark;
        NIIi mLightTechMark;
        NIIi mModelTechMark;
        NIIi mEventTechMark;
        NIIi mVoiceTechMark;
        NIIi mPhysicalTechMark;
        NIIi mControlEffectMark;
        NIIi mInternetEffectMark;
        NIIi mGameEffectMark;
        ControlItems mContorlItems;
        ControlItems mValidContorlItems;
        InternetItems mInternetItems;
        InternetItems mValidInternetItems;
    };
}
#endif