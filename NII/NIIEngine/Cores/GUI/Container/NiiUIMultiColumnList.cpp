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

#include "NiiPreProcess.h"
#include "NiiUIMultiColumnList.h"
#include "NiiUIException.h"
#include "NiiUIScrollbar.h"
#include "NiiUIListHeader.h"
#include "NiiUIListbox.h"
#include "NiiUIWidgetManager.h"
#include "NiiUICursorEventArgs.h"
#include "NiiUIWidgetList.h"
#include "NiiString.h"
#include "NiiXMLSerializer.h"
#include "NiiLogManager.h"

#define ItemRenderingArea               1
#define ItemRenderingAreaHScroll        2
#define ItemRenderingAreaVScroll        3
#define ItemRenderingAreaHVScroll       4
#define ItemRenderingAreaScroll         5

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdColumnsSizable : public PropertyCmd
    {
    public:
        CmdColumnsSizable() : PropertyCmd(N_PropertyUIMultiColumnList_ColumnSizingEnable, _T("columns_sizable"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isColumnSizeEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setColumnSizeEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isColumnSizeEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->setColumnSizeEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColumnsSizable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdColumnsMovable : public PropertyCmd
    {
    public:
        CmdColumnsMovable() : PropertyCmd(N_PropertyUIMultiColumnList_ColumnDraggingEnable, _T("columns_movable"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isColumnDragEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setColumnDragEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isColumnDragEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->setColumnDragEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColumnsMovable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdSortSettingEnabled : public PropertyCmd
    {
    public:
        CmdSortSettingEnabled() : PropertyCmd(N_PropertyUIMultiColumnList_SortEnable, _T("sort_setting_enabled"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isSortEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setSortEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isSortEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->setSortEnable(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSortSettingEnabled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdSortDirection : public PropertyCmd
    {
    public:
        CmdSortDirection() : PropertyCmd(N_PropertyUIMultiColumnList_SortDirection, _T("sort_direction"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ListHeaderItem::SortMode temp = static_cast<const MultiColumnList *>(own)->getSortMode();

            if(temp == ListHeaderItem::SM_None)
            {
                out = _T("None");
            }
            else if(temp == ListHeaderItem::SM_Ascend)
            {
                out = _T("Ascend");
            }
            else if(temp == ListHeaderItem::SM_Descend)
            {
                out = _T("Descend");
            }
            else
                out = _T("SM_Ascend");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ListHeaderItem::SortMode temp;

            if(val == _T("Ascend"))
            {
                temp = ListHeaderItem::SM_Ascend;
            }
            else if(val == _T("Descend"))
            {
                temp = ListHeaderItem::SM_Descend;
            }
            else
            {
                temp = ListHeaderItem::SM_None;
            }
            static_cast<MultiColumnList *>(own)->setSortMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)out;
            *temp = static_cast<const MultiColumnList *>(own)->getSortMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)in;
            static_cast<MultiColumnList *>(own)->setSortMode(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSortDirection();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("None");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUIMultiColumnList_VScrollAlwaysShow, _T("force_vert_scrollbar"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->SetForceVScrollbar(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceVertScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdForceHorzScrollbar : public PropertyCmd
    {
    public:
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUIMultiColumnList_HScrollAlwaysShow, _T("force_horz_scrollbar"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isForceHScrollbar(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->SetForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->SetForceHScrollbar(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceHorzScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdNominatedSelectionColumnID : public PropertyCmd
    {
    public:
        CmdNominatedSelectionColumnID() : PropertyCmd(16, _T("nominated_selection_column_id"),
            _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = N_conv(static_cast<const MultiColumnList *>(own)->getReserveSelectColumnID());
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setReserveSelectColumn(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiColumnList *>(own)->getReserveSelectColumnID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiColumnList *>(own)->setReserveSelectColumn(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNominatedSelectionColumnID();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdNominatedSelectionRow : public PropertyCmd
    {
    public:
        CmdNominatedSelectionRow() : PropertyCmd(17, _T("nominated_selection_row"),
            _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->getReserveSelectRow(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setReserveSelectRow(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiColumnList *>(own)->getReserveSelectRow();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiColumnList *>(own)->setReserveSelectRow(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNominatedSelectionRow();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = String((Nui32)0);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdRowCount : public PropertyCmd
    {
    public:
        CmdRowCount() : PropertyCmd(N_PropertyUIMultiColumnList_RowCount, _T("row_count"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->getRowCount(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            (void)own;
            (void)val;
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiColumnList *>(own)->getRowCount();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            (void)own;
            (void)in;
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRowCount();
        }
        /// @copydetails PropertyCmd::isWritable
        bool isWritable() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdSelectionMode : public PropertyCmd
    {
    public:
        CmdSelectionMode() : PropertyCmd(N_PropertyUIMultiColumnList_SelectionMode, _T("selection_mode"), _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            MultiColumnList::SelectMode val = static_cast<const MultiColumnList *>(own)->getSelectionMode();
            switch(val)
            {
            case MultiColumnList::SM_MultiRow:
                out = _T("SM_MultiRow");
                break;

            case MultiColumnList::SM_SingleColumn:
                out = _T("SM_SingleColumn");
                break;

            case MultiColumnList::SM_MultiColumn:
                out = _T("SM_MultiColumn");
                break;

            case MultiColumnList::SM_SingleCell:
                out = _T("SM_SingleCell");
                break;

            case MultiColumnList::SM_MultiCell:
                out = _T("SM_MultiCell");
                break;

            case MultiColumnList::SM_ReserveSingleColumn:
                out = _T("SM_ReserveSingleColumn");
                break;

            case MultiColumnList::SM_ReserveMultiColumn:
                out = _T("SM_ReserveMultiColumn");
                break;

            case MultiColumnList::SM_ReserveSingleRow:
                out = _T("SM_ReserveSingleRow");
                break;

            case MultiColumnList::SM_ReserveMultiRow:
                out = _T("SM_ReserveMultiRow");
                break;
            default:
                out = _T("SM_SingleRow");
                break;
            }

        }
        void set(PropertyCmdObj * own, const String & val)
        {
            MultiColumnList * obj = static_cast<MultiColumnList *>(own);
            MultiColumnList::SelectMode mode;

            if(val == _T("SM_MultiRow"))
            {
                mode = MultiColumnList::SM_MultiRow;
            }
            else if (val == _T("SM_SingleColumn"))
            {
                mode = MultiColumnList::SM_SingleColumn;
            }
            else if (val == _T("SM_MultiColumn"))
            {
                mode = MultiColumnList::SM_MultiColumn;
            }
            else if (val == _T("SM_SingleCell"))
            {
                mode = MultiColumnList::SM_SingleCell;
            }
            else if (val == _T("SM_MultiCell"))
            {
                mode = MultiColumnList::SM_MultiCell;
            }
            else if (val == _T("SM_ReserveSingleColumn"))
            {
                mode = MultiColumnList::SM_ReserveSingleColumn;
            }
            else if (val == _T("SM_ReserveMultiColumn"))
            {
                mode = MultiColumnList::SM_ReserveMultiColumn;
            }
            else if (val == _T("SM_ReserveSingleRow"))
            {
                mode = MultiColumnList::SM_ReserveSingleRow;
            }
            else if (val == _T("SM_ReserveMultiRow"))
            {
                mode = MultiColumnList::SM_ReserveMultiRow;
            }
            else
            {
                mode = MultiColumnList::SM_SingleRow;
            }
            obj->setSelectionMode(mode);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            MultiColumnList::SelectMode * temp = (MultiColumnList::SelectMode *)out;
            *temp = static_cast<const MultiColumnList *>(own)->getSelectionMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            MultiColumnList::SelectMode * temp = (MultiColumnList::SelectMode *)in;
            static_cast<MultiColumnList *>(own)->setSelectionMode(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("SM_SingleRow");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdAutoSizeColumnUsesHeader : public PropertyCmd
    {
    public:
        CmdAutoSizeColumnUsesHeader() : PropertyCmd(N_PropertyUIMultiColumnList_ColumnAutoSize, _T("auto_size_column_uses_header"),
            _I18n(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiColumnList *>(own)->isColumnAutoSize(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiColumnList *>(own)->setColumnAutoSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiColumnList *>(own)->isColumnAutoSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiColumnList *>(own)->setColumnAutoSize(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoSizeColumnUsesHeader();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdColumnHeader : public PropertyCmd
    {
    public:
        CmdColumnHeader() : PropertyCmd(N_PropertyUIMultiColumnList_addColumn, _T("column_header"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            (void)own;
            (void)out;
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<MultiColumnList *>(own)->addColumn(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            (void)own;
            (void)out;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<MultiColumnList *>(own)->addColumn(*temp);
        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColumnHeader();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
        /// @copydetails PropertyCmd::isSerialize
        virtual bool isSerialize() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::isReadable
        virtual bool isReadable() const
        {
            return false;
        }
    };
    //------------------------------------------------------------------------
    bool descend(const MultiColumnList::Row & a, const MultiColumnList::Row & b)
    {
        return a > b;
    }
    //------------------------------------------------------------------------
    N_EVENT(Container, MultiColumnList, SelectModeEvent,                    0);
    N_EVENT(Container, MultiColumnList, ReserveSelectColumnEvent,           1);
    N_EVENT(Container, MultiColumnList, ReserveSelectRowEvent,              2);
    N_EVENT(Container, MultiColumnList, VScrollModeEvent,                   3);
    N_EVENT(Container, MultiColumnList, HScrollModeEvent,                   4);
    N_EVENT(Container, MultiColumnList, SelectEvent,                        5);
    N_EVENT(Container, MultiColumnList, ItemListEvent,                      6);
    N_EVENT(Container, MultiColumnList, SortColumnEvent,                    7);
    N_EVENT(Container, MultiColumnList, SortModeEvent,                      8);
    N_EVENT(Container, MultiColumnList, ColumnResizeEvent,                  9);
    N_EVENT(Container, MultiColumnList, ColumnMoveEvent,                    10);
    N_EVENT(Container, MultiColumnList, EventCount,                         11);
    //------------------------------------------------------------------------
    const WidgetID MultiColumnList::VScrollbarID(1);    //"__auto_vscrollbar__"
    const WidgetID MultiColumnList::HScrollbarID(2);    //"__auto_hscrollbar__";
    const WidgetID MultiColumnList::ListHeaderID(3);    //"__auto_listheader__"
    //------------------------------------------------------------------------
    MultiColumnList::MultiColumnList(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_MultiColumnList, lid),
        mForceVScroll(false),
        mForceHScroll(false),
        mAutoSizeColumn(false),
        mReserveSelectColumn(0),
        mReserveSelectRow(0),
        mCurrentSelect(0),
        mColumnCount(0)
    {
        notifySignal(MultiColumnList::EventCount);
        setupCmd(N_CmdObj_MultiColumnList);

        // set default selection mode
        mSelectMode = SM_SingleCell;        // hack to ensure call below does what it should.
        setSelectionMode(SM_SingleRow);
    }
    //------------------------------------------------------------------------
    MultiColumnList::~MultiColumnList()
    {
        removeAllImpl();
    }
	//------------------------------------------------------------------------
	void MultiColumnList::setSortEnable(bool set)
	{
		getListHeader()->setSortMode(set ? ListHeaderItem::SM_Descend  : ListHeaderItem::SM_None);
	}
    //------------------------------------------------------------------------
    WidgetID MultiColumnList::getSortColumnID() const
    {
        if (getColumnCount() > 0)
        {
            return getColumnID(getColumnIndex());
        }
        return 0;
    }
    //------------------------------------------------------------------------
    RelPosf MultiColumnList::getColumnTotalWidth() const
    {
        const ListHeader * header = getListHeader();
        RelPosf width(0,0);

        for (Nui32 i = 0; i < getColumnCount(); ++i)
            width += header->getColumnWidth(i);

        return width;
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::getItemRowIndex(const ListboxItem * item) const
    {
        for (Nui32 i = 0; i < getRowCount(); ++i)
        {
            if (isRowExist(item, i))
            {
                return i;
            }
        }

        N_EXCEPT(InvalidRequest, _I18n("the given ListboxItem is not attached to this MultiColumnList."));
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::getItemColumnIndex(const ListboxItem * item) const
    {
        for (NCount i = 0; i < getColumnCount(); ++i)
        {
            if (isColumnExist(item, i))
            {
                return i;
            }
        }
        
        N_EXCEPT(InvalidRequest, _I18n("the given ListboxItem is not attached to this MultiColumnList."));
    }
    //------------------------------------------------------------------------
    GridCellIndx MultiColumnList::getItemGridIndex(const ListboxItem * item) const
    {
        return GridCellIndx(getItemRowIndex(item), getItemColumnIndex(item));
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getItem(const GridCellIndx & grid_ref) const
    {
        if (grid_ref.mColumn >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the column given in the grid reference is out of range."));
        }
        else if (grid_ref.mRow >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the row given in the grid reference is out of range."));
        }
        else
        {
            return mGrid[grid_ref.mRow][grid_ref.mColumn];
        }
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::isColumnExist(const ListboxItem * item, Nidx cidx) const
    {
        if (cidx >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the column index given is out of range."));
        }
        else
        {
            for (NCount i = 0; i < getRowCount(); ++i)
            {
                if (mGrid[i][cidx] == item)
                {
                    return true;
                }

            }
            return false;
        }
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::isRowExist(const ListboxItem * item, Nidx ridx) const
    {
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the row index given is out of range."));
        }
        else
        {
            for (Nui32 i = 0; i < getColumnCount(); ++i)
            {
                if (mGrid[ridx][i] == item)
                {
                    return true;
                }
            }
            return false;
        }
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::isItemExist(const ListboxItem * item) const
    {
        for (NCount i = 0; i < getRowCount(); ++i)
        {
            for (NCount j = 0; j < getColumnCount(); ++j)
            {
                if (mGrid[i][j] == item)
                {
                    return true;
                }
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getColumnItem(const String & text, Nidx cidx, const ListboxItem * begin) const
    {
        if (cidx >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified column index is out of range."));
        }

        Nui32 i = (!begin) ? 0 : getItemRowIndex(begin) + 1;

        for ( ; i < getRowCount(); ++i)
        {
            if (mGrid[i][cidx]->getText() == text)
            {
                return mGrid[i][cidx];
            }

        }

        return 0;
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getRowItem(const String & text, Nidx ridx, const ListboxItem * begin) const
    {
        // ensure row is valid
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified row index is out of range."));
        }

        // find start position for search
        Nui32 i = (!begin) ? 0 : getItemColumnIndex(begin) + 1;

        for ( ; i < getColumnCount(); ++i)
        {
            // does this item match?
            if (mGrid[ridx][i]->getText() == text)
            {
                return mGrid[ridx][i];
            }

        }

        return 0;
    }
    //------------------------------------------------------------------------
    ListboxItem* MultiColumnList::getItem(const String & text, const ListboxItem * begin) const
    {
        GridCellIndx startRef(0, 0);

        if (begin)
        {
            startRef = getItemGridIndex(begin);
            ++startRef.mColumn;
        }

        for (Nui32 i = startRef.mRow; i < getRowCount(); ++i)
        {
            for (Nui32 j = startRef.mColumn; j < getColumnCount(); ++j)
            {
                // does this item match?
                if (mGrid[i][j]->getText() == text)
                {
                    return mGrid[i][j];
                }

            }

        }
        return 0;
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getFirstSelect() const
    {
        return getNextSelect(0);
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getNextSelect(const ListboxItem * begin) const
    {
        GridCellIndx startRef(0, 0);

        // get position of begin if it's not NULL
        if (begin)
        {
            startRef = getItemGridIndex(begin);
            if (++startRef.mColumn == getColumnCount())
            {
                startRef.mColumn = 0;
                ++startRef.mRow;
            }
        }

        // perform the search
        for (Nui32 i = startRef.mRow; i < getRowCount(); ++i)
        {
            for (Nui32 j = startRef.mColumn; j < getColumnCount(); ++j)
            {
                // does this item match?
                ListboxItem* item = mGrid[i][j];

                if ((item != 0) && item->isSelect())
                {
                    return mGrid[i][j];
                }
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    NCount MultiColumnList::getSelectCount() const
    {
        NCount count = 0;
        for(NCount i = 0; i < getRowCount(); ++i)
        {
            for(NCount j = 0; j < getColumnCount(); ++j)
            {
                ListboxItem * item = mGrid[i][j];

                if((item != 0) && item->isSelect())
                {
                    ++count;
                }
            }
        }

        return count;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::isItemSelect(const GridCellIndx & index) const
    {
        ListboxItem * item = getItem(index);
        if (item)
        {
            return item->isSelect();
        }

        return false;
    }
    //------------------------------------------------------------------------
    WidgetID MultiColumnList::getReserveSelectColumnID() const
    {
        if (getColumnCount() > 0)
        {
            return getListHeader()->getColumn(mReserveSelectColumn)->getID();
        }
        return 0;
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::getReserveSelectColumn() const
    {
        return mReserveSelectColumn;
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::getReserveSelectRow() const
    {
        return mReserveSelectRow;
    }
    //------------------------------------------------------------------------
    void MultiColumnList::initChild()
    {
        // get the component sub-widgets
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();
        ListHeader * header       = getListHeader();

        header->bind(ListHeader::ColumnOffsetEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderScrollPrc);
        header->bind(ListHeader::ColumnIndexEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderMovePrc);
        header->bind(ListHeader::ColumnResizeEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderColumnReizePrc);
        header->bind(ListHeader::SortColumnEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderSortColumnPrc);
        header->bind(ListHeader::SortModeEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderSortModePrc);
        header->bind(ListHeader::SplitterDClickEvent, this, (CommandObjFunctor)&MultiColumnList::HeaderItemDClickPrc);
        horzScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&MultiColumnList::HScrollbarPrc);
        vertScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&MultiColumnList::VScrollbarPrc);


        // final initialisation now widget is complete
        setSortMode(ListHeaderItem::SM_None);

        // Perform initial layout
        updateScrollbar();
        layout();
    }
    //------------------------------------------------------------------------
    void MultiColumnList::removeAll()
    {
        if (removeAllImpl())
        {
            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::addColumn(const String & text, WidgetID cid, const RelPosf & width)
    {
        insertColumn(text, cid, width, getColumnCount());
    }
    //------------------------------------------------------------------------
    void MultiColumnList::addColumn(const String & value)
    {
        NCount idstart = value.rfind(_T("id:"));
        NCount wstart = value.rfind(_T("width:"));
        NCount capstart = value.find(_T("text:"));

        // some defaults in case of missing data
        String caption, id(_T("0")), width(_T("{0.33,0}"));

        // extract the caption field
        if (capstart != String::npos)
        {
            capstart = value.find_first_of(_T(":")) + 1;

            if (wstart == String::npos)
                if (idstart == String::npos)
                    caption = value.substr(capstart);
                else
                    caption = value.substr(capstart, idstart - capstart);
            else
                caption = value.substr(capstart, wstart - capstart);

            // trim trailing whitespace
            StrUtil::trimRight(caption, StrUtil::Whitespace);
        }

        // extract the width field
        if (wstart != String::npos)
        {
            width = value.substr(wstart);
            width = width.substr(width.find_first_of(_T("{")));
            width = width.substr(0, width.find_last_of(_T("}")) + 1);
        }

        // extract the id field.
        if (idstart != String::npos)
        {
            id = value.substr(idstart);
            id = id.substr(id.find_first_of(_T(":")) + 1);
        }

        // add the column accordingly
        Nui32 temp1;
        RelPosf temp2;
        StrConv::conv(id, temp1);
        StrConv::conv(width, temp2);
        addColumn(caption, temp1, temp2);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::insertColumn(const String & text, WidgetID cid, const RelPosf & width, Nidx position)
    {
        // if position is out of range, add item to end of current columns.
        if (position > getColumnCount())
        {
            position = getColumnCount();
        }

        // set-up the header for the new column.
        getListHeader()->insertColumn(text, cid, width, position);
        ++mColumnCount;

        // Set the font equal to that of our list
        ListHeaderItem* segment = getColumn(position);
        segment->setFont(mFont);
        // ban properties from segment that we control from here.
        segment->setWrite(N_PropertyUIWidget_ID, false);
        segment->setWrite(N_PropertyUIWidget_Text, false);
        segment->setWrite(N_PropertyUIWidget_Font, false);

        // Insert a blank entry at the appropriate position in each row.
        for (Nui32 i = 0; i < getRowCount(); ++i)
        {
            mGrid[i].mItemList.insert(mGrid[i].mItemList.begin() + position, static_cast<ListboxItem *>(0));
        }

        // update stored nominated selection column if that has changed.
        if ((mReserveSelectColumn >= position) && (getColumnCount() > 1))
        {
            mReserveSelectColumn++;
        }

        WidgetEventArgs args(this);
        onItemList(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::removeColumn(Nidx cidx)
    {
        // ensure index is valid, and throw if not.
        if (cidx >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified column index is out of range."));
        }
        else
        {
            // update stored column index values
            if (mReserveSelectColumn == cidx)
            {
                mReserveSelectColumn = 0;
            }

            // remove the column from each row
            for (Nui32 i = 0; i < getRowCount(); ++i)
            {
                // extract the item pointer.
                ListboxItem* item = mGrid[i][cidx];

                // remove the column entry from the row
                mGrid[i].mItemList.erase(mGrid[i].mItemList.begin() + cidx);

                // delete the ListboxItem as needed.
                if ((item != 0) && item->isAutoDestroy())
                {
                    N_delete item;
                }

            }

            // remove header segment
            getListHeader()->removeColumn(cidx);
            --mColumnCount;

            // signal a change to the list contents
            WidgetEventArgs args(this);
            onItemList(&args);
        }

    }
    //------------------------------------------------------------------------
    void MultiColumnList::removeColumn(WidgetID cid)
    {
        removeColumn(getColumnIndex(cid));
    }
    //------------------------------------------------------------------------
    void MultiColumnList::moveColumn(Nidx cidx, Nidx position)
    {
        // move the segment on the header, events will ensure the items get moved also.
        getListHeader()->moveColumn(cidx, position);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::moveColumn(WidgetID cid, Nidx position)
    {
        moveColumn(getColumnIndex(cid), position);
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::addRow(WidgetID rid)
    {
        return addRow(0, 0, rid);
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::addRow(ListboxItem * item, WidgetID col_id, WidgetID row_id)
    {
        Nidx cidx = 0;

        Row row;
        row.mSortColumn = getColumnIndex();
        row.mItemList.resize(getColumnCount(), 0);
        row.mRowID = row_id;

        if (item)
        {
            cidx = getColumnIndex(col_id);

            // establish item ownership & enter item into column
            item->setOwnerWindow(this);
            row[cidx] = item;
        }

        Nidx pos;

        // if sorting is enabled, insert at an appropriate position
        const ListHeaderItem::SortMode dir = getSortMode();
        if (dir != ListHeaderItem::SM_None)
        {
            // calculate where the row should be inserted
            ListItemGrid::iterator ins_pos = dir == ListHeaderItem::SM_Descend ?
                std::upper_bound(mGrid.begin(), mGrid.end(), row, descend) :
                std::upper_bound(mGrid.begin(), mGrid.end(), row);
            // insert item and get final inserted position.
            ListItemGrid::iterator final_pos = mGrid.insert(ins_pos, row);
            // get final inserted position as an Nui32.
            pos = (Nui32)std::distance(mGrid.begin(), final_pos);
        }
        // not sorted, just stick it on the end.
        else
        {
            pos = getRowCount();
            mGrid.push_back(row);
        }

        // signal a change to the list contents
        WidgetEventArgs args(this);
        onItemList(&args);

        return pos;
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::insertRow(Nidx ridx, WidgetID rid)
    {
        return insertRow(0, 0, ridx, rid);
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::insertRow(ListboxItem * item, WidgetID col_id, Nidx ridx, WidgetID row_id)
    {
        // if sorting is enabled, use add instead of insert
        if (getSortMode() != ListHeaderItem::SM_None)
        {
            return addRow(item, col_id, row_id);
        }
        else
        {
            // Build the new row (empty)
            Row row;
            row.mSortColumn = getColumnIndex();
            row.mItemList.resize(getColumnCount(), 0);
            row.mRowID = row_id;

            // if row index is too big, just insert at end.
            if (ridx > getRowCount())
            {
                ridx = getRowCount();
            }

            mGrid.insert(mGrid.begin() + ridx, row);

            // set the initial item in the new row
            setItem(item, col_id, ridx);

            // signal a change to the list contents
            WidgetEventArgs args(this);
            onItemList(&args);

            return ridx;
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::removeRow(Nidx ridx)
    {
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("The specified row index is out of range."));
        }
        else
        {
            for (Nui32 i = 0; i < getColumnCount(); ++i)
            {
                ListboxItem* item = mGrid[ridx][i];

                if ((item != 0) && item->isAutoDestroy())
                {
                    N_delete item;
                }

            }

            // erase the row from the grid.
            mGrid.erase(mGrid.begin() + ridx);

            // if we have erased the selection row, reset that to 0
            if (mReserveSelectRow == ridx)
            {
                mReserveSelectRow = 0;
            }

            // signal a change to the list contents
            WidgetEventArgs args(this);
            onItemList(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setItem(ListboxItem * item, const GridCellIndx & position)
    {
        if (position.mColumn >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified column index is invalid."));
        }
        else if (position.mRow >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified row index is invalid."));
        }

        // delete old item as required
        ListboxItem* oldItem = mGrid[position.mRow][position.mColumn];

        if ((oldItem != 0) && oldItem->isAutoDestroy())
        {
            N_delete oldItem;
        }

        if (item)
            item->setOwnerWindow(this);

        mGrid[position.mRow][position.mColumn] = item;

        WidgetEventArgs args(this);
        onItemList(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setItem(ListboxItem* item, WidgetID cid, Nidx ridx)
    {
        setItem(item, GridCellIndx(ridx, getColumnIndex(cid)));
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setSelectionMode(MultiColumnList::SelectMode mode)
    {
        if (mSelectMode != mode)
        {
            mSelectMode = mode;

            unselectAll();

            switch(mSelectMode)
            {
            case SM_SingleRow:
                mMultiSelect            = false;
                mSingleRowSelect        = true;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_MultiRow:
                mMultiSelect            = true;
                mSingleRowSelect        = true;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_SingleCell:
                mMultiSelect            = false;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_MultiCell:
                mMultiSelect            = true;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_ReserveSingleColumn:
                mMultiSelect            = false;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = true;
                mReserveSingleRow       = false;
                break;
            case SM_ReserveMultiColumn:
                mMultiSelect            = true;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = true;
                mReserveSingleRow       = false;
                break;
            case SM_SingleColumn:
                mMultiSelect            = false;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = true;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_MultiColumn:
                mMultiSelect            = true;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = true;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = false;
                break;
            case SM_ReserveSingleRow:
                mMultiSelect            = false;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = true;
                break;
            case SM_ReserveMultiRow:
                mMultiSelect            = true;
                mSingleRowSelect        = false;
                mSingleColumnSelect     = false;
                mReserveSingleColumn    = false;
                mReserveSingleRow       = true;
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("invalid or unknown SelectionMode value supplied."));
                break;

            }

            WidgetEventArgs args(this);
            onSelectMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setReserveSelectColumn(WidgetID cid)
    {
        setReserveSelectColumn(getColumnIndex(cid));
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setReserveSelectColumn(Nidx cidx)
    {
        if(mReserveSelectColumn != cidx)
        {
            unselectAll();

            mReserveSelectColumn = cidx;

            WidgetEventArgs args(this);
            onReserveSelectColumn(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setReserveSelectRow(Nidx ridx)
    {
        if(mReserveSelectRow != ridx)
        {
            unselectAll();

            mReserveSelectRow = ridx;

            WidgetEventArgs args(this);
            onReserveSelectRow(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setSortMode(ListHeaderItem::SortMode direction)
    {
        getListHeader()->setSortMode(direction);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setSortColumn(Nidx cidx)
    {
        if(getColumnIndex() != cidx)
        {
            getListHeader()->setSortColumn(cidx);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setSortColumn(WidgetID wid)
    {
        ListHeader * header = getListHeader();
        if(header->getColumn(getColumnIndex())->getID() != wid)
        {
            header->setSortColumn(wid);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::SetForceVScrollbar(bool set)
    {
        if(mForceVScroll != set)
        {
            mForceVScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onVScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::SetForceHScrollbar(bool set)
    {
        if (mForceHScroll != set)
        {
            mForceHScroll = set;

            updateScrollbar();
            WidgetEventArgs args(this);
            onHScrollbarMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::unselectAll()
    {
        // only fire events and update if we actually made any changes
        if (unselectImpl())
        {
            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setItemSelect(ListboxItem * item, bool state)
    {
        setItemSelect(getItemGridIndex(item), state);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setItemSelect(const GridCellIndx & index, bool state)
    {
        if(setCellSelect(index, state))
        {
            WidgetEventArgs args(this);
            onSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::notifyItemUpdate()
    {
        resortList();
        updateScrollbar();
        refresh();
    }
    //------------------------------------------------------------------------
    void MultiColumnList::updateScrollbar()
    {
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();
        NIIf totalHeight = getRowTotalHeight();
        NIIf fullWidth = getListHeader()->getMaxColumnPixelOffset();

        //
        // First show or hide the scroll bars as needed (or requested)
        //
        // show or hide vertical scroll bar as required (or as specified by option)
        if ((totalHeight > getListRenderArea().getHeight()) || mForceVScroll)
        {
            vertScrollbar->show();

            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((fullWidth > getListRenderArea().getWidth()) || mForceHScroll)
            {
                horzScrollbar->show();
            }
            else
            {
                horzScrollbar->hide();
            }
        }
        else
        {
            // show or hide horizontal scroll bar as required (or as specified by option)
            if ((fullWidth > getListRenderArea().getWidth()) || mForceHScroll)
            {
                horzScrollbar->show();

                // show or hide vertical scroll bar as required (or as specified by option)
                if ((totalHeight > getListRenderArea().getHeight()) || mForceVScroll)
                {
                    vertScrollbar->show();
                }
                else
                {
                    vertScrollbar->hide();
                }

            }
            else
            {
                vertScrollbar->hide();
                horzScrollbar->hide();
            }
        }

        //
        // Set up scroll bar values
        //
        Rectf renderArea(getListRenderArea());

        vertScrollbar->setDocSize(totalHeight);
        vertScrollbar->setPageSize(renderArea.getHeight());
        vertScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getHeight() / 10.0f));
        vertScrollbar->setCurrent(vertScrollbar->getCurrent());

        horzScrollbar->setDocSize(fullWidth);
        horzScrollbar->setPageSize(renderArea.getWidth());
        horzScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.getWidth() / 10.0f));
        horzScrollbar->setCurrent(horzScrollbar->getCurrent());
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::selectRange(const GridCellIndx & start, const GridCellIndx & end)
    {
        GridCellIndx tmpStart(start);
        GridCellIndx tmpEnd(end);

        // ensure start is before end
        if (tmpStart.mColumn > tmpEnd.mColumn)
        {
            tmpStart.mColumn = tmpEnd.mColumn;
            tmpEnd.mColumn = start.mColumn;
        }

        if (tmpStart.mRow > tmpEnd.mRow)
        {
            tmpStart.mRow = tmpEnd.mRow;
            tmpEnd.mRow = start.mRow;
        }

        bool modified = false;

        // loop through all items selecting them.
        for (NCount i = tmpStart.mRow; i <= tmpEnd.mRow; ++i)
        {
            for (NCount j = tmpStart.mColumn; j <= tmpEnd.mColumn; ++j)
            {
                ListboxItem * item = mGrid[i][j];
                if (item)
                {
                    modified |= setCellSelect(getItemGridIndex(item), true);
                }
            }
        }

        return modified;
    }
    //------------------------------------------------------------------------
    NIIf MultiColumnList::getRowTotalHeight() const
    {
        NIIf height = 0.0f;

        for (Nidx i = 0; i < getRowCount(); ++i)
        {
            height += getMaxRowHeight(i);
        }

        return height;
    }
    //------------------------------------------------------------------------
    NIIf MultiColumnList::getMaxColumnWidth(Nidx cidx) const
    {
        if (cidx >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified column is out of range."));
        }
        else
        {
            NIIf width = 0.0f;

            // check each item in the column
            for (Nui32 i = 0; i < getRowCount(); ++i)
            {
                ListboxItem* item = mGrid[i][cidx];

                // if the slot has an item in it
                if (item)
                {
                    PlaneSizef sz(item->getPixelSize());

                    // see if this item is wider than the previous widest
                    if (sz.mWidth > width)
                    {
                        // update current widest
                        width = sz.mWidth;
                    }

                }

            }
            // return the widest item.
            return width;
        }
    }
    //------------------------------------------------------------------------
    NIIf MultiColumnList::getMaxRowHeight(Nidx ridx) const
    {
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified row is out of range."));
        }
        else
        {
            NIIf height = 0.0f;
            for (Nui32 i = 0; i < getColumnCount(); ++i)
            {
                ListboxItem* item = mGrid[ridx][i];

                // if the slot has an item in it
                if (item)
                {
                    PlaneSizef sz(item->getPixelSize());

                    // see if this item is higher than the previous highest
                    if (sz.mHeight > height)
                    {
                        // update current highest
                        height = sz.mHeight;
                    }

                }

            }

            // return the hightest item.
            return height;
        }

    }
    //------------------------------------------------------------------------
    bool MultiColumnList::unselectImpl(void)
    {
        bool modified = false;

        for (NCount i = 0; i < getRowCount(); ++i)
        {
            for (NCount j = 0; j < getColumnCount(); ++j)
            {
                ListboxItem* item = mGrid[i][j];

                // if slot has an item, and item is selected
                if ((item != 0) && item->isSelect())
                {
                    // clear selection state and set modified flag
                    item->select(false);
                    modified = true;
                }
            }
        }

        // signal whether or not we did anything.
        return modified;
    }
    //------------------------------------------------------------------------
    ListboxItem * MultiColumnList::getItem(const Vector2f & pt) const
    {
        const ListHeader * header = getListHeader();
        Rectf listArea(getListRenderArea());

        NIIf y = listArea.mTop - getVScrollbar()->getCurrent();
        NIIf x = listArea.mLeft - getHScrollbar()->getCurrent();

        for(Nidx i = 0; i < getRowCount(); ++i)
        {
            y += getMaxRowHeight(i);

            // have we located the row?
            if(pt.y < y)
            {
                // scan across to find column that was clicked
                for(NCount j = 0; j < getColumnCount(); ++j)
                {
                    const ListHeaderItem * seg = header->getColumn(j);
                    x += seg->getWidth().abs(header->getPixelSize().mWidth);

                    // was this the column?
                    if(pt.x < x)
                    {
                        // return contents of grid element that was clicked.
                        return mGrid[i][j];
                    }
                }
            }
        }

        return 0;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::setCellSelect(const GridCellIndx & grid_ref, bool state)
    {
        // validate grid ref
        if (grid_ref.mColumn >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified column index is invalid."));
        }
        else if (grid_ref.mRow >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified row index is invalid."));
        }

        // only do this if the setting is changing
        if (mGrid[grid_ref.mRow][grid_ref.mColumn]->isSelect() != state)
        {
            // if using nominated selection row and/ or column, check that they match.
            if ((!mReserveSingleColumn || (mReserveSelectColumn == grid_ref.mColumn)) &&
                (!mReserveSingleRow || (mReserveSelectRow == grid_ref.mRow)))
            {
                // clear current selection if not multi-select box
                if (state && (!mMultiSelect))
                {
                    unselectImpl();
                }

                // full row?
                if (mSingleRowSelect)
                {
                    // clear selection on all items in the row
                    setRowSelect(grid_ref.mRow, state);
                }
                // full column?
                else if (mSingleColumnSelect)
                {
                    // clear selection on all items in the column
                    setColumnSelect(grid_ref.mColumn, state);

                }
                // single item to be affected
                else
                {
                    mGrid[grid_ref.mRow][grid_ref.mColumn]->select(state);
                }
                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setRowSelect(Nidx ridx, bool state)
    {
        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            ListboxItem* item = mGrid[ridx][i];

            if (item)
            {
                item->select(state);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setColumnSelect(Nidx cidx, bool state)
    {
        for (Nui32 i = 0; i < getRowCount(); ++i)
        {
            ListboxItem * item = mGrid[i][cidx];

            if (item)
            {
                item->select(state);
            }

        }

    }
    //------------------------------------------------------------------------
    void MultiColumnList::moveColumnImpl(Nidx cidx, Nidx position)
    {
        // ensure index is valid, and throw if not.
        if (cidx >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the specified source column index is out of range."));
        }
        else
        {
            // if position is too big, insert at end.
            if (position > getColumnCount())
            {
                position = getColumnCount();
            }

            // update select column index value if needed
            if (mReserveSelectColumn == cidx)
            {
                mReserveSelectColumn = position;
            }
            else if ((cidx < mReserveSelectColumn) && (position >= mReserveSelectColumn))
            {
                mReserveSelectColumn--;
            }
            else if ((cidx > mReserveSelectColumn) && (position <= mReserveSelectColumn))
            {
                mReserveSelectColumn++;
            }

            // move column entry in each row.
            for (Nui32 i = 0; i < getRowCount(); ++i)
            {
                // store entry.
                ListboxItem* item = mGrid[i][cidx];

                // remove the original column for this row.
                mGrid[i].mItemList.erase(mGrid[i].mItemList.begin() + cidx);

                // insert entry at its new position
                mGrid[i].mItemList.insert(mGrid[i].mItemList.begin() + position, item);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onSelectMode(const WidgetEventArgs * arg)
    {
        signal(SelectModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onReserveSelectColumn(const WidgetEventArgs * arg)
    {
        signal(ReserveSelectColumnEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onReserveSelectRow(const WidgetEventArgs * arg)
    {
        signal(ReserveSelectRowEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        signal(VScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        signal(HScrollModeEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiColumnList::onSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onItemList(const WidgetEventArgs * arg)
    {
        updateScrollbar();
        refresh();
        signal(ItemListEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onSortColumn(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SortColumnEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onSortMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onColumnResize(const WidgetEventArgs * arg)
    {
        updateScrollbar();
        refresh();
        signal(ColumnResizeEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onColumnMove(const WidgetEventArgs * arg)
    {
        refresh();
        signal(ColumnMoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onFont(const FontEventArgs * arg)
    {
        for(NCount col = 0; col < getColumnCount(); ++col)
        {
            getColumn(col)->setFont(mFont);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onSize(const WidgetEventArgs * arg)
    {
        // base class handling
        Container::onSize(arg);

        updateScrollbar();
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onButtonDown(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            bool modified = false;

            Vector2f localPos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
            ListboxItem * item = getItem(localPos);

            if(item)
            {
                Nmark wck = mSheet->getWinComKey();
                // clear old selections if no control key is pressed or if multi-select is off
                if(!(wck & WCK_KT_A_Ctrl) || !mMultiSelect)
                {
                    modified = unselectImpl();
                }

                modified = true;

                // select range or item, depending upon keys and last selected item
                if(((wck & WCK_KT_A_Shift) && (mCurrentSelect != 0)) && mMultiSelect)
                {
                    modified |= selectRange(getItemGridIndex(item), getItemGridIndex(mCurrentSelect));
                }
                else
                {
                    modified |= setCellSelect(getItemGridIndex(item), item->isSelect() ^ true);
                }

                // update last selected item
                mCurrentSelect = item->isSelect() ? item : 0;
            }

            // fire event if needed
            if (modified)
            {
                WidgetEventArgs args(this);
                onSelect(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing.
        Widget::onCursorDrag(arg);

        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        if (vertScrollbar->isFinalVisible() && (vertScrollbar->getDocSize() > vertScrollbar->getPageSize()))
        {
            vertScrollbar->setCurrent(vertScrollbar->getCurrent() + vertScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
        else if (horzScrollbar->isFinalVisible() && (horzScrollbar->getDocSize() > horzScrollbar->getPageSize()))
        {
            horzScrollbar->setCurrent(horzScrollbar->getCurrent() + horzScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderScrollPrc(const EventArgs *)
    {
        // grab the header scroll value, convert to pixels, and set the scroll bar to match.
        getHScrollbar()->setCurrent(getListHeader()->getColumnOffset());
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderMovePrc(const EventArgs * e)
    {
        moveColumnImpl(((ColumnEventArgs&)e).mIndex, ((ColumnEventArgs&)e).mNewIndex);

        WidgetEventArgs args(this);
        onColumnMove(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderColumnReizePrc(const EventArgs *)
    {
        updateScrollbar();

        // signal change to our clients
        WidgetEventArgs args(this);
        onColumnResize(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HScrollbarPrc(const EventArgs *)
    {
        // set header offset to match scroll position
        getListHeader()->setColumnOffset(getHScrollbar()->getCurrent());
        refresh();
    }
    //------------------------------------------------------------------------
    void MultiColumnList::VScrollbarPrc(const EventArgs *)
    {
        refresh();
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderSortColumnPrc(const EventArgs *)
    {
        Nidx col = getColumnIndex();
        for (Nui32 i = 0; i < getRowCount(); ++i)
        {
            mGrid[i].mSortColumn = col;
        }

        resortList();

        WidgetEventArgs args(this);
        onSortColumn(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderSortModePrc(const EventArgs *)
    {
        resortList();

        WidgetEventArgs args(this);
        onSortMode(&args);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::HeaderItemDClickPrc(const EventArgs * e)
    {
        // get the column index for the segment that was double-clicked
        Nidx col = getListHeader()->getColumnIndex((ListHeaderItem*)((WidgetEventArgs *)e)->mWidget);

        if (col >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the column index given is out of range."));
        }
        else
        {
            NIIf width = N_MAX(NIIf, getMaxColumnWidth(col), ListHeader::ItemMinWidth);
            setColumnWidth(col, RelPosf(0, width));
        }
    }
    //------------------------------------------------------------------------
    WidgetID MultiColumnList::getRowID(Nidx ridx) const
    {
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the row index given is out of range."));
        }
        else
        {
            return mGrid[ridx].mRowID;
        }
    }
    //------------------------------------------------------------------------
    Nidx MultiColumnList::getRowIndex(WidgetID rid) const
    {
        for (Nidx i = 0; i < getRowCount(); ++i)
        {
            if (mGrid[i].mRowID == rid)
            {
                return i;
            }
        }

        N_EXCEPT(InvalidRequest, _I18n("no row with the requested ID is present."));
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdColumnsSizable());
        dest->add(N_new CmdColumnsMovable());
        dest->add(N_new CmdSortSettingEnabled());
        dest->add(N_new CmdSortDirection());
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        dest->add(N_new CmdNominatedSelectionColumnID());
        dest->add(N_new CmdNominatedSelectionRow());
        dest->add(N_new CmdRowCount());
        dest->add(N_new CmdSelectionMode());
        dest->add(N_new CmdAutoSizeColumnUsesHeader());
        dest->add(N_new CmdColumnHeader());
        return true;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::removeAllImpl(void)
    {
        // just return false if the list is already empty (no rows == empty)
        if (getRowCount() == 0)
        {
            return false;
        }
        // we have items to be removed and possible deleted
        else
        {
            for (Nui32 i = 0; i < getRowCount(); ++i)
            {
                for (Nui32 j = 0; j < getColumnCount(); ++j)
                {
                    ListboxItem* item = mGrid[i][j];

                    // delete item as needed.
                    if ((item != 0) && item->isAutoDestroy())
                    {
                        N_delete item;
                    }

                }

            }

            // clear all items from the grid.
            mGrid.clear();

            // reset other affected fields
            mReserveSelectRow = 0;
            mCurrentSelect = 0;

            return true;
        }

    }
    //------------------------------------------------------------------------
    void MultiColumnList::setRowID(Nui32 ridx, Nui32 row_id)
    {
        // check for invalid index
        if (ridx >= getRowCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("the row index given is out of range."));
        }
        else
        {
            mGrid[ridx].mRowID = row_id;
        }
    }
    //------------------------------------------------------------------------
    Scrollbar* MultiColumnList::getVScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + VScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Scrollbar* MultiColumnList::getHScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + HScrollbarID, false));
    }
    //------------------------------------------------------------------------
    ListHeader * MultiColumnList::getListHeader() const
    {
        return static_cast<ListHeader *>(get((mID << N_Level_Bit_Escape) + ListHeaderID, false));
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::validImpl(const WidgetView * style) const
    {
        return dynamic_cast<const MultiColumnListView *>(style) != 0;
    }
    //------------------------------------------------------------------------
    NCount MultiColumnList::writeImpl(XmlSerializer * dest) const
    {
        // basically this is here to translate the columns in the list into
        // instances of the <ColumnHeader> element.  Because the SortColumnID
        // property requires the column to exist, we also write that out manually.

        // Dump all other properties first
        NCount count = Container::writeImpl(dest);

        // create an dump <ColumnHeader> elements
        for (NCount i = 0; i < getColumnCount(); ++i)
        {
            ListHeaderItem * seg = getColumn(i);

            // column text
            String propString = _T("text:");
            propString += seg->getText();
            // column width
            propString += _T(" width:");
            propString += N_conv(seg->getWidth());
            // column id
            propString += _T(" id:");
            propString += N_conv((Nui32)seg->getID());
            // create the tag
            dest->begin(PropertyCmd::PropertyAttribute)
                .attribute(PropertyCmd::NameAttribute, _T("ColumnHeader"))
                .attribute(PropertyCmd::ValueAttribute, propString)
                .end();
            ++count;
        }

        // write out SortColumnID property, if any(!)
        try
        {
            Nui32 sortColumnID = getColumnIndex(getColumnIndex());
            if (sortColumnID != 0)
            {
                dest->begin(PropertyCmd::PropertyAttribute)
                    .attribute(PropertyCmd::NameAttribute, _T("SortColumnID"))
                    .attribute(PropertyCmd::ValueAttribute,
                        N_conv((Nui32)sortColumnID)).end();
                ++count;
            }
        }
        catch (InvalidRequestException &)
        {
            // This catches error(s) from the MultiLineColumnList for example
            N_Only(Log).log(_I18n("invalid sort column requested. Continuing..."));
        }
        return count;
    }
    //------------------------------------------------------------------------
    Rectf MultiColumnList::getListRenderArea() const
    {
        if (mWidgetView != 0)
        {
            MultiColumnListView* wr = (MultiColumnListView*)mWidgetView;
            return wr->getListRenderArea();
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItem(const ListboxItem * item)
    {
        // NB: throws InvalidRequestException if non-existant item
        visibleItem(getItemGridIndex(item));
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItem(const GridCellIndx & grid_ref)
    {
        visibleItemRow(grid_ref.mRow);
        visibleItemColumn(grid_ref.mColumn);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItemRow(const ListboxItem * item)
    {
        // NB: throws InvalidRequestException if non-existant item
        visibleItemRow(getItemGridIndex(item).mRow);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItemColumn(const ListboxItem * item)
    {
        // NB: throws InvalidRequestException if non-existant item
        visibleItemColumn(getItemGridIndex(item).mColumn);
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItemRow(Nidx ridx)
    {
        Nui32 rows = getRowCount();

        Scrollbar * vertScrollbar = getVScrollbar();

        // handle horizontal scrolling
        // handle simple "scroll to the bottom" case
        if (ridx >= rows)
        {
            vertScrollbar->setCurrent(
                vertScrollbar->getDocSize() - vertScrollbar->getPageSize());
        }
        else
        {
            NIIf bottom;
            NIIf top = 0.0f;
            NIIf listHeight = getListRenderArea().getHeight();

            // get distance to top of item
            Nidx row;
            for (row = 0; row < ridx; ++row)
                top += getMaxRowHeight(row);

            // calculate distance to bottom of item
            bottom = top + getMaxRowHeight(row);

            // account for current scrollbar value
            NIIf currPos = vertScrollbar->getCurrent();
            top      -= currPos;
            bottom   -= currPos;

            // if top edge is above the view area, or if the item is too big to fit
            if ((top < 0.0f) || ((bottom - top) > listHeight))
            {
                // scroll top of item to top of box.
                vertScrollbar->setCurrent(currPos + top);
            }
            // if the bottom edge is below the view area
            else if (bottom >= listHeight)
            {
                // position bottom of item at the bottom of the list
                vertScrollbar->setCurrent(currPos + bottom - listHeight);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::visibleItemColumn(Nidx cidx)
    {
        Nui32 cols = getColumnCount();
        Scrollbar* horzScrollbar = getHScrollbar();

        // handle horizontal scrolling
        // first the simple "scroll to the right edge" case
        if (cidx >= cols)
        {
            horzScrollbar->setCurrent(
                horzScrollbar->getDocSize() - horzScrollbar->getPageSize());
        }
        else
        {
            NIIf right;
            NIIf left = 0.0f;
            NIIf listWidth = getListRenderArea().getWidth();

            // get distance to left edge of item
            Nui32 col;
            for (col = 0; col < cidx; ++col)
                left += getColumnWidth(col).abs(calcParentArea().mWidth);

            // get the distance to the right edge of the item
            right = left + getColumnWidth(col).abs(calcParentArea().mWidth);

            // account for current scrollbar value
            NIIf currPos = horzScrollbar->getCurrent();
            left -= currPos;
            right -= currPos;

            // if the left edge is to the left of the view area, or if the item is
            // too big to fit
            if ((left < 0.0f) || ((right - left) > listWidth))
            {
                // scroll left edge of item to left edge of box.
                horzScrollbar->setCurrent(currPos + left);
            }
            // if right edge is to the right of the view area
            else if (right >= listWidth)
            {
                // position the right edge of the item at the right edge of the list
                horzScrollbar->setCurrent(currPos + right - listWidth);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiColumnList::setColumnAutoSize(bool set)
    {
        mAutoSizeColumn = set;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::isColumnAutoSize() const
    {
        return mAutoSizeColumn;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::Row::operator<(const Row& rhs) const
    {
        ListboxItem* a = mItemList[mSortColumn];
        ListboxItem* b = rhs.mItemList[mSortColumn];

        if (!b)
        {
            return false;
        }
        else if (!a)
        {
            return true;
        }
        return *a < *b;
    }
    //------------------------------------------------------------------------
    bool MultiColumnList::Row::operator>(const Row & rhs) const
    {
        ListboxItem * a = mItemList[mSortColumn];
        ListboxItem * b = rhs.mItemList[mSortColumn];

        if (!a)
        {
            return false;
        }
        else if (!b)
        {
            return true;
        }

        return *a > *b;
    }
    //------------------------------------------------------------------------
    void MultiColumnList::resortList()
    {
        // re-sort list according to direction
        ListHeaderItem::SortMode dir = getSortMode();

        if (dir == ListHeaderItem::SM_Descend)
        {
            std::sort(mGrid.begin(), mGrid.end(), descend);
        }
        else if (dir == ListHeaderItem::SM_Ascend)
        {
            std::sort(mGrid.begin(), mGrid.end());
        }
        // else no (or invalid) direction, so do not sort.
    }
    //------------------------------------------------------------------------
    bool GridCellIndx::operator<=(const GridCellIndx & o) const
    {
        return !operator>(o);
    }
    //------------------------------------------------------------------------
    bool GridCellIndx::operator>(const GridCellIndx & o) const
    {
        return (operator<(o) || operator==(o)) ? false : true;
    }
    //------------------------------------------------------------------------
    bool GridCellIndx::operator>=(const GridCellIndx & o) const
    {
        return !operator<(o);
    }
    //------------------------------------------------------------------------
    /*    Named Areas:
            - ItemRenderingArea         - area where items will be drawn when no scrollbars are visible.
            - ItemRenderingAreaHScroll  - area where items will be drawn when the horizontal scrollbar is visible.
            - ItemRenderingAreaVScroll  - area where items will be drawn when the vertical scrollbar is visible.
            - ItemRenderingAreaHVScroll - area where items will be drawn when both scrollbars are visible.

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"
            ListHeader based widget with name suffix "__auto_listheader__"*/
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MultiColumnListView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    MultiColumnListView::MultiColumnListView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_MultiColumnList)
    {
    }
    //------------------------------------------------------------------------
    Rectf MultiColumnListView::getListRenderArea() const
    {
        MultiColumnList * w = (MultiColumnList *)mSrc;
        // get Style for the assigned look.
        const Style * style = getStyle();
        bool v_visible = w->getVScrollbar()->isVisible();
        bool h_visible = w->getHScrollbar()->isVisible();
        Nid sid;
        // if either of the scrollbars are visible, we might want to use another item rendering area
        if (v_visible || h_visible)
        {
            if (h_visible)
            {
                if (v_visible)
                {
                    sid = ItemRenderingAreaHVScroll;
                }
                else
                {
                    sid = ItemRenderingAreaHScroll;
                }
            }
            else
            {
                if (v_visible)
                {
                    sid = ItemRenderingAreaVScroll;
                }
                else
                {
                    sid = ItemRenderingAreaScroll;
                }
            }

            if (style->isSpecialExist(sid))
            {
                return style->getSpecial(sid).getArea().getPixelRect(*w);
            }
        }
        return style->getSpecial(ItemRenderingArea).getArea().getPixelRect(*w);
    }
    //------------------------------------------------------------------------
    void MultiColumnListView::flush()
    {
        MultiColumnList * w = (MultiColumnList *)mSrc;
        const ListHeader * header = w->getListHeader();
        const Scrollbar * vertScrollbar = w->getVScrollbar();
        const Scrollbar * horzScrollbar = w->getHScrollbar();

        const StyleLayout * ss = &getStyle()->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        ss->render(mSrc);

        //
        // Render list items
        //
        Vector3f itemPos;
        PlaneSizef itemSize;
        Rectf itemClipper, itemRect;

        // calculate position of area we have to render into
        Rectf itemsArea(getListRenderArea());

        // set up initial positional details for items
        itemPos.y = itemsArea.top() - vertScrollbar->getCurrent();
        itemPos.z = 0.0f;

        const NIIf alpha = w->getAlphaEffect();
        // loop through the items
        for (Nidx i = 0; i < w->getRowCount(); ++i)
        {
            // set initial x position for this row.
            itemPos.x = itemsArea.left() - horzScrollbar->getCurrent();

            // calculate height for this row.
            itemSize.mHeight = w->getMaxRowHeight(i);

            // loop through the columns in this row
            for (NCount j = 0; j < w->getColumnCount(); ++j)
            {
                // allow item to use full width of the column
                itemSize.mWidth = header->getColumnWidth(j).abs(header->getPixelSize().mWidth);

                ListboxItem* item = w->getItem(GridCellIndx(i, j));

                // is the item for this column set?
                if (item)
                {
                    // calculate destination area for this item.
                    itemRect.setLeft(itemPos.x);
                    itemRect.setTop(itemPos.y);
                    itemRect.setSize(itemSize);
                    itemClipper = itemRect.intersects(itemsArea);

                    // skip this item if totally clipped
                    if (itemClipper.getWidth() == 0)
                    {
                        itemPos.x += itemSize.mWidth;
                        continue;
                    }

                    // draw this item
                    item->draw(*w->getPixelData(), itemRect, alpha, &itemClipper);
                }

                // update position for next column.
                itemPos.x += itemSize.mWidth;
            }
            // update position ready for next row
            itemPos.y += itemSize.mHeight;
        }
    }
    //------------------------------------------------------------------------
}
}