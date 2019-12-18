/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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
#ifndef _NII_UI_SIZEContainer_H_
#define _NII_UI_SIZEContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 大小容器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SizeContainer : public Container
    {
    public:
        SizeContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        ~SizeContainer();

        /** 设置自动大小
        @version NIIEngine 3.0.0
        */
        void setAutoSize(bool b);

        /** 是否自动大小
        @version NIIEngine 3.0.0
        */
        bool isAutoSize() const;

        /** 设置内容区域大小
        @version NIIEngine 3.0.0
        */
        void setContentArea(const Rectf & area);

        /** 获取内容区域大小
        @version NIIEngine 3.0.0
        */
        const Rectf & getContentArea() const;

        /** 获取自动内容区域大小
        @version NIIEngine 3.0.0
        */
        Rectf getAutoContentArea() const;

        /// @copydetails Widget::getChildInnerArea
        virtual const CalcRect & getChildInnerArea() const;

        /// @copydetails Widget::getChildArea
        virtual const CalcRect & getChildArea() const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 获取客户区域实现
        @version NIIEngine 3.0.0
        */
        Rectf getClientAreaImpl(bool pixelAlign) const;

        /** 子对象大小函数
        @version NIIEngine 3.0.0
        */
        void ChildSizeMFunc(const EventArgs * e);

        /** 子对象位置函数
        @version NIIEngine 3.0.0
        */
		void ChildMoveMFunc(const EventArgs * e);

        /// @copydetails Widget::queuePixelImpl
        void queuePixelImpl();

        /// @copydetails Widget::getClipInnerAreaImpl
        Rectf getClipInnerAreaImpl() const;

        /// @copydetails Widget::getInnerAreaImpl
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;
        
        /// @copydetails Widget::setAreaImpl
        void setAreaImpl(const RelVector2 & pos, const RelPlaneSize & size, bool event = true);

        /// @copydetails Widget::getIntersectAreaImpl
        Rectf getIntersectAreaImpl() const;

        /// @copydetails Container::onAdd
        void onAdd(const WidgetEventArgs * arg);

        /// @copydetails Container::onRemove
        void onRemove(const WidgetEventArgs * arg);

        /// @copydetails Widget::onParentSize
        void onParentSize(const WidgetEventArgs * arg);
        
        /** 当自动大小设置改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onAutoSizeChange(const WidgetEventArgs * arg);
        
        /** 当内容区域改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaChange(const WidgetEventArgs * arg);
    public:
        /** 设置自动大小改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID AutoSizeChangeEvent;
        
        /** 内容区域改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        typedef multimap<Widget *, SignalPtr>::type SignalList;
    protected:
        Rectf mChildArea;
        CalcRect mClientArea;
        SignalList mSignalList;
        bool mAutoSize;        
    };
}
}
#endif