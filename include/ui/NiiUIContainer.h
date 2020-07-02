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

#ifndef _NII_UI_CONTAINER_H_
#define _NII_UI_CONTAINER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** UI容器单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Container : public Widget
    {
        friend class UISheet;
        friend class Window;
        friend class Widget;
    public:
        Container(WidgetID wid, FactoryID sid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_Container, LangID lid = N_PrimaryLang);
        virtual ~Container();

        using PropertyCmdObj::get;

        /** 创建一个子对象并把它附加到这个对象中
        @param[in] wid UI单元id
        @param[in] fid UI单元类型
        @param[in] name 辅助名字
        @version NIIEngine 3.0.0
        */
        Widget * create(WidgetID wid, FactoryID fid, const String & name = _T(""));

        /** 删除这个对象的子对象
        @param sub 子对象
        @version NIIEngine 3.0.0
        */
        void destroy(Widget * sub);

        /** 删除这个对象的指定路径子对象
        @param path 子对象路径(包含自身的名字)
        @version NIIEngine 3.0.0
        */
        void destroy(const String & path);

        /** 添加指定子对象
        @remark 自动分离子对象原关系
        @param[in] sub 子对象
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub);

        /** 移去指定子对象
        @param[in] sub 子对象
        @version NIIEngine 3.0.0
        */
        void remove(Widget * sub);

        /** 移去指定id的子对象
        @param id 子对象id
        @version NIIEngine 3.0.0
        */
        void remove(WidgetID id);

        /** 移去指定路径的子对象
        @param path 子对象路径(包含自身的名字)
        @version NIIEngine 3.0.0
        */
        void remove(const String & path);

        /** 获取指定下标的子UI单元对象
        @remark 这个函数比较内部
        @param index 下标
        @return 指定下标的子UI单元对象
        @version NIIEngine 3.0.0
        */
        Widget * get(Nidx index) const;

        /** 获取指定id子级对象
        @param[in] 对象的ID
        @param[in] r 递归
        @version NIIEngine 3.0.0
        */
        Widget * get(WidgetID id, bool r) const;

        /** 返回指定路径的子对象
        @param str 子对象的名字或总路径
        @param n_path 第一参数是否为路径
        @version NIIEngine 3.0.0
        */
        Widget * get(const String & str, bool nonpath = false) const;

        /** 指定id子级对象是否存在
        @param[in] id 子对象id
        @param[in] r 递归
        @version NIIEngine 3.0.0
        */
        bool isExist(WidgetID id, bool r = false) const;

        /** 是否存在子对象
        @param[in] sub 子对象
        @version NIIEngine 3.0.0
        */
        bool isExist(const Widget * sub) const;

        /** 指定路径子对象是否存在
        @param[in] str 元素名字或路径
        @param[in] n_path 第一参数是否为路径
        @version NIIEngine 3.0.0
        */
        bool isExist(const String & str, bool n_path = false) const;

        /** 把子单元移到最前面
        @version NIIEngine 3.0.0
        */
        void moveChildFront(Widget * sub);

        /** 把子单元移到最后面
        @version NIIEngine 3.0.0
        */
        void moveChildBack(Widget * sub);

        /** 获取子对象的索引
        @version NIIEngine 3.0.0
        */
        NCount getIndex(Widget * sub) const;

        /** 子对象是否被对焦
        @version NIIEngine 3.0.0
        */
        bool isChildFocus() const;

        /** 游标是否在这个容器内部
        @version NIIEngine 3.0.0
        */
        bool isCursorEnter() const;

        /** 子对象的个数
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取与给定像素位置相交子UI单元
        @param pos 指定像素位置
        @return 与像素位置相交的UI单元,不存在则返回NULL(0)
        @version NIIEngine 3.0.0
        */
        Widget * get(const Vector2f & pos) const;

        /** 获取指定位置的子对象
        @param pos 位置
        @param disable 是否可以处于禁用状态
        @version NIIEngine 3.0.0
        */
        Widget * getTarget(const Vector2f & pos, bool disable = false) const;

        /** 获取活动的子对象
        @version NIIEngine 3.0.0
        */
        Widget * getActiveChild() const;

        /** 设置是否下发游标事件
        @version NIIEngine 3.0.0
        */
        void setTransmitCursorEvent(bool b);

        /** 获取是否下发游标事件
        @version NIIEngine 3.0.0
        */
        bool isTransmitCursorEvent() const;

        /** 获取内容整体区域
        @version NIIEngine 3.0.0
        */
        virtual const CalcRect & getChildArea() const;

        /** 获取内容内区域
        @version NIIEngine 3.0.0
        */
        virtual const CalcRect & getChildInnerArea() const;

        /** 内容区域
        @param content
        @version NIIEngine 3.0.0
        */
        const CalcRect & getChildArea(bool content) const;

        /** 设置所在的窗体
        @version NIIEngine 3.0.0 高级api
        */
        void setWindow(Window * win);

        /** 获取所在的窗体
        @version NIIEngine 3.0.0 高级api
        */
        Window * getWindow() const;

        /** 添加子UI元素
        @remark 添加子UI元素的具体实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void addImpl(Widget * e);

        /** 移去子UI元素
        @remark 移去子UI元素的具体实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void removeImpl(Widget * e);

        /** 获取子命名UI元素
        @remark 获取子命名UI元素的具体实现
        @param[in] str 元素名字或路径
        @param[in] nonpath 第一参数是否为路径
        @version NIIEngine 3.0.0 高级api
        */
        virtual Widget * getImpl(const String & str, bool nonpath = false) const;

        /// @copydetails Widget::update
        virtual void update(TimeDurMS cost);

        /// @copydetails Widget::layout
        virtual void layout();

        /// @copydetails Widget::isContainer
        virtual bool isContainer() const;
        
        /// @copydetails Widget::setRestorePreFocus
        virtual void setRestorePreFocus(bool set);
    public:
        /** 添加子对象时
        @version NIIEngine 3.0.0
        */
        static const EventID AddEvent;

        /** 移去子对象时
        @version NIIEngine 3.0.0
        */
        static const EventID RemoveEvent;

        /** 子对移动时触发
        @version NIIEngine 3.0.0
        */
        static const EventID ChildMoveEvent;

        /** 游标进入时
        @version NIIEngine 3.0.0
        */
        static const EventID CursorEnterAreaEvent;

        /** 游标移去时
        @version NIIEngine 3.0.0
        */
        static const EventID CursorLeaveAreaEvent;

        /** 对象事件总类数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Widget::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onEnable
        virtual void onEnable(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDisable
        virtual void onDisable(const WidgetEventArgs * arg);

        /// @copydetails Widget::onAlpha
        virtual void onAlpha(const WidgetEventArgs * arg);

        /// @copydetails Widget::onInactivate
        virtual void onInactivate(const ActivationEventArgs * arg);

        /// @copydetails Widget::onStyleAttach
        virtual void onStyleAttach(const StyleEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onSheet
        virtual void onSheet(const SheetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /** 添加一个子UI单元对象到这个UI单元对象(后)触发
        @note 注意触发的前后顺序
        @version NIIEngine 3.0.0
        */
        virtual void onAdd(const WidgetEventArgs * arg);

        /** 从这个UI单元对象中移去一个子UI单元对象(后)触发
        @note 注意触发的前后顺序
        @version NIIEngine 3.0.0
        */
        virtual void onRemove(const WidgetEventArgs * arg);

        /** 子对移动时触发
        @param[in] arg
        @version NIIEngine 3.0.0
        */
        virtual void onChildMove(const WidgetEventArgs * arg);

        /** 游标进入后触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorEnterArea(const CursorEventArgs * arg);

        /** 游标移出后触发
        @version NIIEngine 3.0.0
        */
        virtual void onCursorLeaveArea(const CursorEventArgs * arg);
    protected:
        /** 初始化子UI单元
        @remark 一般是指表面的可见的结构
        @version NIIEngine 3.0.0 高级api
        */
        virtual void initChild();

        /** 清理子UI单元
        @remark 一般是指表面的可见的结构
        @version NIIEngine 3.0.0 高级api
        */
        virtual void clearChild();

        /** 获取子对象
        @param[in] pos 位置
        @param[in] f 测试函数
        @param[in] disable 是否可以处于禁用状态
        @version NIIEngine 3.0.0 高级api
        */
        Widget * get(const Vector2f & pos, intersect f, bool disable = false) const;

        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::queuePixelImpl
        virtual void queuePixelImpl();

        /// @copydetails Widget::refreshImpl
        virtual void refreshImpl(bool r);

        /// @copydetails Widget::cloneImpl
        virtual void cloneImpl(Widget * dest, bool r) const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();

        /// @copydetails Widget::notifyClipChange
        virtual void notifyClipChange();

        /// @copydetails Widget::writeImpl
        virtual NCount writeImpl(XmlSerializer * dest) const;
    protected:
        bool writeChild(XmlSerializer * dest) const;
    protected:
        Window * mWindow;
        WidgetList mChilds;
        bool mCursorEnter;          ///< 游标是否进入
        bool mTransmitCursorEvent;  ///< 下发游标事件
    };
}
}
#endif