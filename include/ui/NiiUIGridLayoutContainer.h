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

#ifndef _NII_UI_GridLayoutContainer_H_
#define _NII_UI_GridLayoutContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUILayoutContainer.h"

namespace NII
{
namespace UI
{
    /** 网格排版
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GridLayoutContainer : public LayoutContainer
    {
    public:
        /** 自动排序类型
        @version NIIEngine 3.0.0
        */
        enum LayoutType
        {
            LT_None,
            LT_TopToBottom,
            LT_LeftToRight
        };
    public:
        GridLayoutContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~GridLayoutContainer();

        /** 设置自动排序类型
        @version NIIEngine 3.0.0
        */
        void setLayoutType(LayoutType set);

        /** 获取自动排序类型
        @version NIIEngine 3.0.0
        */
        LayoutType getLayoutType() const;

        /** 设置网格数量
        @version NIIEngine 3.0.0
        */
        void setGrid(NCount width, NCount height);

        /** 设置网格宽数量
        @version NIIEngine 3.0.0
        */
        NCount getGridWidth() const;

        /** 设置网格高数量
        @version NIIEngine 3.0.0
        */
        NCount getGridHeight() const;

        /** 设置网格数量
        @version NIIEngine 3.0.0
        */
        void setGrid(const PlaneSizei & size);

        /** 获取网格数量
        @version NIIEngine 3.0.0
        */
        PlaneSizei getGrid() const;
        
        using Container::add;

        using Container::remove;

        using Container::get;

        /** 添加子元素
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub, NCount x, NCount y);

        /** 获取子元素
        @version NIIEngine 3.0.0
        */
        Widget * get(Nidx x, Nidx y);

        /** 移去子元素
        @version NIIEngine 3.0.0
        */
        void remove(Nidx x, Nidx y);

        /** 子元素换位
        @version NIIEngine 3.0.0
        */
        void swap(Nidx sub1, Nidx sub2);

        /** 子元素换位
        @version NIIEngine 3.0.0
        */
        void swap(NCount x1, NCount y1, NCount x2, NCount y2);

        /** 子元素换位
        @version NIIEngine 3.0.0
        */
        void swap(Widget * sub1, Widget * sub2);

        /** 移去动子元素
        @version NIIEngine 3.0.0
        */
        void move(Widget * wnd, NCount x1, NCount y1);

        /** 设置下一个排版索引
        @version NIIEngine 3.0.0
        */
        void setNextLayoutIndex(Nidx idx);

        /** 获取下一个排版索引
        @version NIIEngine 3.0.0
        */
        Nidx getNextLayoutIndex() const;

        /// @copydoc LayoutContainer::layoutImpl
        virtual void layoutImpl();

        /// @copydetails Container::addImpl
        virtual void addImpl(Widget * sub);

        /// @copydetails Container::removeImpl
        virtual void removeImpl(Widget * sub);
    public:
        /** 子元素排序变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID ChildOrderChangeEvent;

        /// 事件数量
        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 子元素排序变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onChildOrderChange(const WidgetEventArgs * arg);
    protected:
        /** 占位使用的元素
        @version NIIEngine 3.0.0
        */
        Widget * createDummy();

        /** 获取位置所在的网格索引
        @version NIIEngien 3.0.0 内部api
        */
        Nidx getGridIndex(NCount x, NCount y, NCount gridW, NCount gridH) const;

        /** 获取实际的网格索引
        @version NIIEngien 3.0.0 内部api
        */
        Nidx getGridIndex(Nidx index) const;

        /** 获取索引所在的网格位置
        @version NIIEngien 3.0.0 内部api
        */
        void getGridPos(Nidx index, NCount & outx, NCount & outy, NCount gridW, NCount gridH) const;
    protected:
        NCount mGridWidth;
        NCount mGridHeight;
        LayoutType mLayoutType;
        Nidx mNextIndex;
        NCount mNextGridX;
        NCount mNextGridY;
    };
}
}
#endif