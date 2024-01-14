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
        virtual ~SizeContainer();

        /** 设置自动大小
        @version NIIEngine 3.0.0
        */
        void setAutoSize(bool b);

        /** 是否自动大小
        @version NIIEngine 3.0.0
        */
        bool isAutoSize() const                 { return mAutoSize; }

        /** 设置内容区域大小
        @version NIIEngine 3.0.0
        */
        void setContentArea(const Rectf & area);

        /** 获取内容区域大小
        @version NIIEngine 3.0.0
        */
        const Rectf & getContentArea() const    { return mChildArea; }

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
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 获取子对象区域
        @version NIIEngine 3.0.0
        */
        Rectf getChildAreaImpl(bool pixelAlign) const;

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
        void setAreaImpl(const RelVector2f & pos, const RelPlaneSizef & size, bool event = true);

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