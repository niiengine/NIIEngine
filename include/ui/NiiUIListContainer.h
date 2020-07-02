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
    
    /** �б�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ListContainer : public Container
    {
    public:
        /** ����ģʽ
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

        /** �����
        @version NIIEngine 3.0.0
        */
        void addItem(ListItem * obj);

        /** ������
        @version NIIEngine 3.0.0
        */
        void insertItem(ListItem * obj, const ListItem * pos);

        /** ��ȥ��
        @version NIIEngine 3.0.0
        */
        void removeItem(ListItem * obj);

        /** ��ȡ��
        @version NIIEngine 3.0.0
        */
        ListItem * getItem(Nidx obj) const;

        /** �Ƿ������
        @version NIIEngine 3.0.0
        */
        bool isItemExist(const ListItem * obj) const;

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        Nidx getItemIndex(const ListItem * obj) const;

        /** Ѱ����
        @version NIIEngine 3.0.0
        */
        ListItem * findItem(Nidx index, const String & text);

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        inline NCount getItemCount() const{ return mItemList.size(); }

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void removeAllItem();

        /** �����Ƿ��Զ�������С
        @version NIIEngine 3.0.0
        */
        void setAutoResize(bool b);

        /** ��ȡ�Ƿ��Զ�������С
        @version NIIEngine 3.0.0
        */
        inline bool isAutoResize() const{ return mAutoResize; }

        /** ��������ģʽ
        @version NIIEngine 3.0.0
        */
        void setSortMode(SortMode mode);

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        inline SortMode getSortMode() const{ return mSortMode; }

        /** �����Զ�������
        @version NIIEngine 3.0.0
        */
        void setSortFunc(SortFunc func);

        /** ��ȡ�Զ�������
        @version NIIEngine 3.0.0
        */
        inline SortFunc getSortFunc() const { return mSortFunc; }

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        Rectf getItemArea() const;

        /** ��ȡ������
        @version NIIEngine 3.0.0
        */
        inline Container * getContentArea() const{ return mContainerArea; }

        /** �����б�
        @param layout ִ���Ű�
        @version NIIEngine 3.0.0
        */
        void sortList(bool layout = true);

        /** ֪ͨ����
        @version NIIEngine 3.0.0
        */
        virtual void notifyItemClick(ListItem * obj);

        /** ֪ͨ��ѡ��
        @version NIIEngine 3.0.0
        */
        virtual void notifyItemSelect(ListItem * obj, bool select);

        /** ֪ͨ�����
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

        /** �Ű�����
        @version NIIEngine 3.0.0
        */
        virtual void layoutContent() = 0;

        /** ���ݴ�С
        @version NIIEngine 3.0.0
        */
        virtual PlaneSizef getContentSize() const = 0;

        /** �Զ�������Сʵ��
        @version NIIEngine 3.0.0
        */
        virtual void autoResizeImpl();

        /** ����������ʵ��
        @version NIIEngine 3.0.0
        */
        virtual bool removeAllImpl();
        
        /** ��ȥ�б����
        @version NIIEngine 3.0.0
        */
        virtual void onItemRmove(const EventArgs * e);

        /** ���б�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onItemList(const WidgetEventArgs * arg);

        /** ����ģʽ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /// @copydetails Widget::onParentSize
        virtual void onParentSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onEndInit
        virtual void onEndInit();
    public:
        /** ���б�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID ItemListEvent;

        /** ����ģʽ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;

        /// �¼�����
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