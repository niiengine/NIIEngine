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

#ifndef _NII_UI_ListContainer_H_
#define _NII_UI_ListContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIListEntry.h"
#include "NiiUIContainer.h"
#include "NiiUIWidgetList.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListContainerView : public WidgetView
    {
    public:
        ListContainerView(WidgetModelID wmid):
            WidgetView(wmid, N_MODEL_ListContainer){}

        /**
        @version NIIEngine 3.0.0
        */
        virtual Rectf getRenderArea() const = 0;
    };
    
    /** 列表容器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListContainer : public Container
    {
    public:
        /** 排序模式
        @version NIIEngine 3.0.0
        */
        enum SortMode
        {
            SM_None,
            SM_Ascend,
            SM_Descend,
            SM_Custom
        };

        typedef bool (*SortFunc)(const ListItem * a, const ListItem * b);
    public:
        ListContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            ScriptTypeID stid = N_CmdObj_ListContainer, LangID lid = N_PrimaryLang);

        virtual ~ListContainer();

        /** 添加项
        @version NIIEngine 3.0.0
        */
        void addItem(ListItem * obj);

        /** 插入项
        @version NIIEngine 3.0.0
        */
        void insertItem(ListItem * obj, const ListItem * pos);

        /** 移去项
        @version NIIEngine 3.0.0
        */
        void removeItem(ListItem * obj);

        /** 获取项
        @version NIIEngine 3.0.0
        */
        ListItem * getItem(Nidx obj) const;

        /** 是否存在项
        @version NIIEngine 3.0.0
        */
        bool isItemExist(const ListItem * obj) const;

        /** 获取项索引
        @version NIIEngine 3.0.0
        */
        Nidx getItemIndex(const ListItem * obj) const;

        /** 寻找项
        @version NIIEngine 3.0.0
        */
        ListItem * findItem(Nidx index, const String & text);

        /** 获取项数量
        @version NIIEngine 3.0.0
        */
        inline NCount getItemCount() const{ return mItemList.size(); }

        /** 移去所有
        @version NIIEngine 3.0.0
        */
        void removeAllItem();

        /** 设置是否自动容器大小
        @version NIIEngine 3.0.0
        */
        void setAutoResize(bool b);

        /** 获取是否自动容器大小
        @version NIIEngine 3.0.0
        */
        inline bool isAutoResize() const{ return mAutoResize; }

        /** 设置排序模式
        @version NIIEngine 3.0.0
        */
        void setSortMode(SortMode mode);

        /** 获取排序模式
        @version NIIEngine 3.0.0
        */
        inline SortMode getSortMode() const{ return mSortMode; }

        /** 设置自定义排序
        @version NIIEngine 3.0.0
        */
        void setSortFunc(SortFunc func);

        /** 获取自定义排序
        @version NIIEngine 3.0.0
        */
        inline SortFunc getSortFunc() const { return mSortFunc; }

        /** 获取项区域
        @version NIIEngine 3.0.0
        */
        Rectf getItemArea() const;

        /** 获取项容器
        @version NIIEngine 3.0.0
        */
        inline Container * getContentArea() const{ return mContainerArea; }

        /** 排序列表
        @param layout 执行排版
        @version NIIEngine 3.0.0
        */
        void sortList(bool layout = true);

        /** 通知项点击
        @version NIIEngine 3.0.0
        */
        virtual void notifyItemClick(ListItem * obj);

        /** 通知项选中
        @version NIIEngine 3.0.0
        */
        virtual void notifyItemSelect(ListItem * obj, bool select);

        /** 通知项更新
        @version NIIEngine 3.0.0
        */
        virtual void notifyItemUpdate(bool sort = false);

        /// @copydetails Widget::layout()
        void layout();

        /// @copydetails Container::addImpl
        virtual void addImpl(Widget * child);
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);
    protected:
        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * model) const;

        /** 排版内容
        @version NIIEngine 3.0.0
        */
        virtual void layoutContent() = 0;

        /** 内容大小
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getContentSize() const = 0;

        /** 自动容器大小实现
        @version NIIEngine 3.0.0
        */
        virtual void autoResizeImpl();

        /** 清理所有项实现
        @version NIIEngine 3.0.0
        */
        virtual bool removeAllImpl();
        
        /** 移去列表项函数
        @version NIIEngine 3.0.0
        */
        virtual void onItemRmove(const EventArgs * e);

        /** 当列表改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onItemList(const WidgetEventArgs * arg);

        /** 排序模式改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /// @copydetails Widget::onParentSize
        virtual void onParentSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onEndInit
        virtual void onEndInit();
    public:
        /** 当列表改变时触发
        @version NIIEngine 3.0.0
        */
        static const EventID ItemListEvent;

        /** 排序模式改变时触发
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        typedef vector<ListItem *>::type ListEntryList;
    protected:
        ListEntryList mItemList;
        Container * mContainerArea;
        SortMode mSortMode;
        SortFunc mSortFunc;
        bool mAutoResize;
        bool mResort;
    };
}
}
#endif