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
#ifndef _NII_UI_ScrollContainer_H_
#define _NII_UI_ScrollContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 滚动窗体容器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollContainer : public Container
    {
    public:
        ScrollContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        ~ScrollContainer();

        /** 设置内容区域自动大小
        @version NIIEngine 3.0.0
        */
        void setContentAutoSize(bool b);

        /** 获取内容区域自动大小
        @version NIIEngine 3.0.0
        */
        bool isContentAutoSize() const;

        /** 设置内容区域大小
        @version NIIEngine 3.0.0
        */
        void setContentArea(const Rectf & area);

        /** 获取内容区域大小
        @version NIIEngine 3.0.0
        */
        const Rectf & getContentArea() const;

        /** 设置是否始终显示y轴滚动条
        @version NIIEngine 3.0.0
        */
        void setForceVScrollbar(bool b);

        /** 获取是否始终显示y轴滚动条
        @version NIIEngine 3.0.0
        */
        bool isForceVScrollbar() const;

        /** 设置是否始终显示x轴滚动条
        @version NIIEngine 3.0.0
        */
        void setForceHScrollbar(bool b);

        /** 获取是否始终显示x轴滚动条
        @version NIIEngine 3.0.0
        */
        bool isForceHScrollbar() const;

        /** 设置x轴滚动条步进
        @version NIIEngine 3.0.0
        */
        void setHStepSize(NIIf step);

        /** 获取x轴滚动条步进
        @version NIIEngine 3.0.0
        */
        NIIf getHStepSize() const;

        /** 设置x轴滚动条页余
        @version NIIEngine 3.0.0
        */
        void setHPageRemain(NIIf overlap);

        /** 获取x轴滚动条页余
        @version NIIEngine 3.0.0
        */
        NIIf getHPageRemain() const;

        /** 设置y轴滚动条步进
        @version NIIEngine 3.0.0
        */
        void setVStepSize(NIIf step);

        /** 设置y轴滚动条步进
        @version NIIEngine 3.0.0
        */
        NIIf getVStepSize() const;

        /** 设置y轴滚动条页余
        @version NIIEngine 3.0.0
        */
        void setVPageRemain(NIIf overlap);

        /** 设置y轴滚动条页余
        @version NIIEngine 3.0.0
        */
        NIIf getVPageRemain() const;

        /** 设置x轴滚动条位置
        @note 百分级
        @version NIIEngine 3.0.0
        */
        void setHScrollPos(NIIf pos);

        /** 获取x轴滚动条位置
        @note 百分级
        @version NIIEngine 3.0.0
        */
        NIIf getHScrollPos() const;

        /** 设置y轴滚动条位置
        @note 百分级
        @version NIIEngine 3.0.0
        */
        void setVScrollPos(NIIf pos);

        /** 获取y轴滚动条位置
        @note 百分级
        @version NIIEngine 3.0.0
        */
        NIIf getVScrollPos() const;

        /** 获取y轴滚动条
        @version NIIEngine 3.0.0
        */
        Scrollbar * getVScrollbar() const;

        /** 获取x轴滚动条
        @version NIIEngine 3.0.0
        */
        Scrollbar * getHScrollbar() const;
        
        /** 获取内容容器
        @version NIIEngine 3.0.0
        */
        SizeContainer * getContentContainer() const;

        /// @copydetails Widget::destroy
        void destroy();

        /// @copydetails Container::addImpl
        void addImpl(Widget * sub);

        /// @copydetails Container::removeImpl
        void removeImpl(Widget * sub);
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 更新滚动条
        @version NIIEngine 3.0.0
        */
        void updateScrollbar();

        /** 更新内容区域
        @version NIIEngine 3.0.0
        */
        void updateContentArea();

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;
    protected:
        void ContentAreaScrollMFunc(const EventArgs * arg);
        void AutoSizeChangeMFunc(const EventArgs * arg);

        /** 当内容区域改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaChange(const WidgetEventArgs * arg);

        /** 当内容区域滚动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaScroll(const WidgetEventArgs * arg);
        
        /** 当自动大小设置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAutoSizeChange(const WidgetEventArgs * arg);
        
        /** 当y轴滚动设置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarSetChange(const WidgetEventArgs * arg);

        /** 当x轴滚动设置变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarSetChange(const WidgetEventArgs * arg);

        /// @copydetails Container::onSize
        void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::getImpl
        Widget * getImpl(const String & name, bool r = false) const;

        /** 内容区域函数
        @version NIIEngine 3.0.0
        */
        void ContentAreaMFunc(const EventArgs * e);
    public:
        /** 内容区域改变时
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaChangeEvent;

        /** y轴滚动设置变化时
        @version NIIEngine 3.0.0
        */
        static const EventID VScrollbarSetEvent;

        /** x轴滚动设置变化时
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollbarSetEvent;

        /** 自动大小设置变化时
        @version NIIEngine 3.0.0
        */
        static const EventID AutoSizeChangeEvent;

        /** 内容区域滚动时
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaScrollEvent;

        /** 数量总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        SizeContainer * mContentContainer;
        Scrollbar * mXScrollbar;
        Scrollbar * mYScrollbar;
        Rectf mContentRect;
        NIIf mVScrollStep;
        NIIf mHScrollStep;
        NIIf mVScrollRemain;
        NIIf mHScrollRemain;
        SignalPtr mContentChangeSingal;
        SignalPtr mAutoSizeChangeSingal;
        bool mForceVScroll;
        bool mForceHScroll;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollContainerView : public WidgetView
    {
    public:
        ScrollContainerView(WidgetModelID wsid);

        virtual Rectf getViewableArea() const;

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::getArea
        Rectf getArea() const;
    protected:
        /// @copydetails WidgetView::onStyleAttach
        void onStyleAttach();

        /// @copydetails WidgetView::onStyleDetach
        void onStyleDetach();

    private:
        bool mStyleValid;
    };
}
}
#endif