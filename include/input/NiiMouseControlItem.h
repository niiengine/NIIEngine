/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-23

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

#ifndef _NII_MOUSE_CONTROLITEM_H_
#define _NII_MOUSE_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    enum MouseButton
    {
        MB_Left = 0,
        MB_Right = 1,
        MB_Middle = 2,
        MB_Button3 = 3,
        MB_Button4 = 4,
        MB_Button5 = 5,
        MB_Button6 = 6,
        MB_Button7 = 7,
        MB_Unknow = 8
    };

    /** 鼠标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseMoveBaseArgs : public EventAlloc
    {
    public:
        MouseMoveBaseArgs();
        MouseMoveBaseArgs(const MouseMoveBaseArgs & o);
        MouseMoveBaseArgs(const MouseMoveArgs * o);
        MouseMoveBaseArgs(const MouseControlPattern * src);
        MouseMoveBaseArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);

        const MouseControlPattern * mSrc;   ///< 参数来源
        Ni32 mX;                            ///< x坐标移动数量
        Ni32 mY;                            ///< y坐标移动数量
        Ni32 mScroll;                       ///< wheel滚动数量
    };
    /** 鼠标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseMoveArgs : public EventArgs
    {
    public:
        MouseMoveArgs();
        MouseMoveArgs(const MouseMoveBaseArgs * o);
        MouseMoveArgs(const MouseMoveArgs & o);
        MouseMoveArgs(const MouseControlPattern * src);
        MouseMoveArgs(const MouseControlPattern * src, Ni32 x, Ni32 y, Ni32 scroll);

        const MouseControlPattern * mSrc;   ///< 参数来源
        Ni32 mX;                            ///< x坐标移动数量
        Ni32 mY;                            ///< y坐标移动数量
        Ni32 mScroll;                       ///< wheel滚动数量
    };

    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI MousePressBaseArgs : public EventAlloc
    {
    public:
        MousePressBaseArgs();
        MousePressBaseArgs(const MousePressBaseArgs & o);
        MousePressBaseArgs(const MousePressArgs * o);
        MousePressBaseArgs(const MouseControlPattern * src);
        MousePressBaseArgs(const MouseControlPattern * src, MouseButton b);

        const MouseControlPattern * mSrc;   ///< 参数来源
        MouseButton mButton;                ///< 按键编码
    };
    /**
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI MousePressArgs : public EventArgs
    {
    public:
        MousePressArgs();
        MousePressArgs(const MousePressBaseArgs * o);
        MousePressArgs(const MousePressArgs & o);
        MousePressArgs(const MouseControlPattern * src);
        MousePressArgs(const MouseControlPattern * src, MouseButton b);

        const MouseControlPattern * mSrc;   ///< 参数来源
        MouseButton mButton;                ///< 按键编码
    };

    typedef void(MouseControlItem::*MouseMethod)(const MousePressArgs *);
    /** 鼠标控制单元
    @remark 游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多
        的处理性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public ControlItem
    {
        friend class MouseControlPattern;
    public:
        MouseControlItem();

        virtual ~MouseControlItem();

        /** 设置鼠标单元的增量因子
        @remark 使用这个函数可以加快鼠标移动
        @par 因子级别,倍数关系
        @version NIIEngine 3.0.0
        */
        void setGain(NIIf factor);

        /** 获取鼠标单元的增量因子
        @remark 使用这个函数可以加快鼠标移动
        @par 因子级别,倍数关系
        @version NIIEngine 3.0.0
        */
        NIIf getGain() const;

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    public:
        /** 当鼠标左键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressLeft(const MousePressArgs * arg);

        /** 当鼠标左键被按下后释放时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseLeft(const MousePressArgs * arg);

        /** 当鼠标右键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressRight(const MousePressArgs * arg);

        /** 当鼠标右键被按下后释放时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseRight(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressMiddle(const MousePressArgs * arg);

        /** 当鼠标中键被按下后释放时触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseMiddle(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton3(const MousePressArgs * arg);

        /** 当鼠标中键被按下后释放时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton3(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton4(const MousePressArgs * arg);

        /** 当鼠标中键被按下后释放时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton4(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton5(const MousePressArgs * arg);

        /** 当鼠标中键被按下后释放时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton5(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton6(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton6(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onPressButton7(const MousePressArgs * arg);

        /** 当鼠标中键被按下时触发,鼠标的额外扩展(一般不要用)
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onReleaseButton7(const MousePressArgs * arg);

        /** 当鼠标移动的时候触发
        @param[in] arg 鼠标事件参数
        @version NIIEngine 3.0.0
        */
        virtual void onMove(const MouseMoveArgs * arg);
    protected:
        /** 初始化映射表
        @remark 至少调用一次
        */
        static void init();

        /** 当鼠标按键按下时触发
        @param[in] arg
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPress(const MousePressArgs * arg);

        /** 当鼠标按键释放时触发
        @param[in] arg
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onRelease(const MousePressArgs * arg);
    protected:
        NIIf mFactor;                        ///< 加速因子
    };

    /**
    @version NIIEngine 3.0.0
    */
    class DummyMouseControlItem : public MouseControlItem, public ControlAlloc
    {
    public:
        DummyMouseControlItem() {}
    };
}
}
#endif