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
#include "NiiUIListHeader.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include "NiiUIWindow.h"
#include "NiiPixelManager.h"
#include "NiiUICursor.h"

namespace NII
{
namespace UI
{
    class _EngineInner CmdColumnsSizable : public PropertyCmd
    {
    public:
        CmdColumnsSizable() : PropertyCmd(N_PropertyUIListHeader_ColumnSizingEnable, _T("columns_sizable"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeader *>(own)->isColumnSizeEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListHeader *>(own)->setColumnSizeEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListHeader *>(own)->isColumnSizeEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListHeader *>(own)->setColumnSizeEnable(*temp);
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

    class _EngineInner CmdColumnsMovable : public PropertyCmd
    {
    public:
        CmdColumnsMovable() : PropertyCmd(N_PropertyUIListHeader_ColumnDraggingEnable, _T("columns_movable"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeader *>(own)->isColumnDragEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListHeader *>(own)->setColumnDragEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListHeader *>(own)->isColumnDragEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListHeader *>(own)->setColumnDragEnable(*temp);
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

    class _EngineInner CmdSortColumnID : public PropertyCmd
    {
    public:
        CmdSortColumnID() : PropertyCmd(N_PropertyUIListHeader_SortSegmentID, _T("sort_column_id"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeader *>(own)->getSortColumnID(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<ListHeader *>(own)->getSortColumn(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const ListHeader *>(own)->getSortColumnID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<ListHeader *>(own)->getSortColumn(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSortColumnID();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };

    class _EngineInner CmdSortDirection : public PropertyCmd
    {
    public:
        CmdSortDirection() : PropertyCmd(N_PropertyUIListHeader_SortDirection, _T("sort_direction"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            ListHeaderItem::SortMode val = static_cast<const ListHeader *>(own)->getSortMode();

            if(val == ListHeaderItem::SM_None)
            {
                out = _T("None");
            }
            else if(val == ListHeaderItem::SM_Ascend)
            {
                out = _T("Ascend");
            }
            else if(val == ListHeaderItem::SM_Descend)
            {
                out = _T("Descend");
            }
            else
                out = _T("Ascend");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            ListHeaderItem::SortMode re;
            if(val == _T("Ascend"))
            {
                re = ListHeaderItem::SM_Ascend;
            }
            else if(val == _T("Descend"))
            {
                re = ListHeaderItem::SM_Descend;
            }
            else
            {
                re = ListHeaderItem::SM_None;
            }
            static_cast<ListHeader *>(own)->setSortMode(re);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)out;
            *temp = static_cast<const ListHeader *>(own)->getSortMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)in;
            static_cast<ListHeader *>(own)->setSortMode(*temp);
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
    //------------------------------------------------------------------------
    class _EngineInner CmdSizable : public PropertyCmd
    {
    public:
        CmdSizable() : PropertyCmd(N_PropertyUIListHeaderSegment_SizingEnable, _T("sizable"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeaderItem *>(own)->isDragResize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListHeaderItem *>(own)->setDragResize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListHeaderItem *>(own)->isDragResize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListHeaderItem *>(own)->setDragResize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSizable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdClickable : public PropertyCmd
    {
    public:
        CmdClickable() : PropertyCmd(N_PropertyUIListHeaderSegment_ClickEnable, _T("clickable"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeaderItem *>(own)->isClickEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListHeaderItem *>(own)->setClickEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListHeaderItem *>(own)->isClickEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListHeaderItem *>(own)->setClickEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdClickable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdDragable : public PropertyCmd
    {
    public:
        CmdDragable() : PropertyCmd(N_PropertyUIListHeaderSegment_DragMovingEnable, _T("dragable"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ListHeaderItem *>(own)->isDragMove(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ListHeaderItem *>(own)->setDragMove(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ListHeaderItem *>(own)->isDragMove();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ListHeaderItem *>(own)->setDragMove(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDragable();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdHSortDirection : public PropertyCmd
    {
    public:
        CmdHSortDirection() : PropertyCmd(N_PropertyUIListHeaderSegment_SortDirection, _T("sort_direction"),
            _T("Property to get/set the sort direction setting of the header segment.  Value is the text of one of the SortMode enumerated value names."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ListHeaderItem::SortMode val = static_cast<const ListHeaderItem *>(own)->getSortMode();
            if (val == ListHeaderItem::SM_None)
            {
                out = _T("None");
            }
            else if (val == ListHeaderItem::SM_Ascend)
            {
                out = _T("Ascend");
            }
            else if (val == ListHeaderItem::SM_Descend)
            {
                out = _T("Descend");
            }
            else
                out = _T("SM_Ascend");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & str)
        {
            ListHeaderItem::SortMode val;
            if (str == _T("Ascend"))
            {
                val = ListHeaderItem::SM_Ascend;
            }
            else if (str == _T("Descend"))
            {
                val = ListHeaderItem::SM_Descend;
            }
            else
            {
                val = ListHeaderItem::SM_None;
            }
            static_cast<ListHeaderItem *>(own)->setSortMode(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)out;
            *temp = static_cast<const ListHeaderItem *>(own)->getSortMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ListHeaderItem::SortMode * temp = (ListHeaderItem::SortMode *)in;
            static_cast<ListHeaderItem *>(own)->setSortMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHSortDirection();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("None");
        }
    };

    class _EngineInner CmdSizingCursorImage : public PropertyCmd
    {
    public:
        CmdSizingCursorImage() : PropertyCmd(N_PropertyUIListHeaderSegment_SizingCursorImage, _T("sizing_cursor_image"),
            _T("Property to get/set the sizing cursor image for the List Header Segment.  Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const PixelBuffer * val = static_cast<const ListHeaderItem *>(own)->getSizingView();
            out = val ? N_conv(val->getGroupID()) + _T("|") + N_conv(val->getID()) : _T("0|0");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<ListHeaderItem *>(own)->setSizingView(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            const PixelBuffer * val = static_cast<const ListHeaderItem *>(own)->getSizingView();
            *temp = val ? ((Nui64)val->getGroupID() << 32) | val->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<ListHeaderItem *>(own)->setSizingView(N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSizingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdMovingCursorImage : public PropertyCmd
    {
    public:
        CmdMovingCursorImage() : PropertyCmd(N_PropertyUIListHeaderSegment_MovingCursorImage, _T("moving_cursor_image"),
            _T("Property to get/set the moving cursor image for the List Header Segment.  Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            PixelBuffer * val = static_cast<const ListHeaderItem *>(own)->getMovingView();
            out = val ? N_conv(val->getGroupID()) + _T("|") + N_conv(val->getID()) : _T("0|0");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<ListHeaderItem *>(own)->setMovingView(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            PixelBuffer * val = static_cast<const ListHeaderItem *>(own)->getMovingView();
            *temp = val ? ((Nui64)val->getGroupID()) | val->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<ListHeaderItem *>(own)->setMovingView(N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMovingCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListHeaderItem
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Widget, ListHeaderItem, ItemClickEvent, 0);
    N_EVENT(Widget, ListHeaderItem, SplitterDClickEvent, 1);
    N_EVENT(Widget, ListHeaderItem, ResizeEnableEvent, 2);
    N_EVENT(Widget, ListHeaderItem, SortModeEvent, 3);
    N_EVENT(Widget, ListHeaderItem, MoveModeEvent, 4);
    N_EVENT(Widget, ListHeaderItem, ClickModeEvent, 5);
    N_EVENT(Widget, ListHeaderItem, ItemDragBeginEvent, 6);
    N_EVENT(Widget, ListHeaderItem, ItemDragEndEvent, 7);
    N_EVENT(Widget, ListHeaderItem, ItemDragMoveEvent, 8);
    N_EVENT(Widget, ListHeaderItem, ItemResizeEvent, 9);
    N_EVENT(Widget, ListHeaderItem, EventCount, 10);
    //------------------------------------------------------------------------
    #define DefaultSizingArea       8.0f
    #define SegmentMoveThreshold    12.0f
    //------------------------------------------------------------------------
    ListHeaderItem::ListHeaderItem(WidgetID wid, FactoryID fid, Container * own,
        const String & name, LangID lid) :
        Widget(N_CmdObj_ListHeaderSegment, wid, fid, own, name, lid),
        mSizingView(0),
        mMovingView(0),
        mSplitterSize(DefaultSizingArea),
        mSplitterHover(false),
        mDragSizing(false),
        mSortMode(SM_None),
        mItemHover(false),
        mItemClick(false),
        mResizeEnable(true),
        mDragMove(true),
        mDragMoving(false),
        mClickEnable(true)
    {
        notifySignal(ListHeaderItem::EventCount);
        setupCmd(N_CmdObj_ListHeaderSegment);
    }
    //------------------------------------------------------------------------
    ListHeaderItem::~ListHeaderItem()
    {
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::setDragResize(bool b)
    {
        if (mResizeEnable != b)
        {
            mResizeEnable = b;

            // if sizing is now disabled, ensure sizing operation is cancelled
            if (!mResizeEnable && mDragSizing)
            {
                lost();
            }

            WidgetEventArgs args(this);
            onResizeMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::setSortMode(SortMode mode)
    {
        if (mSortMode != mode)
        {
            mSortMode = mode;

            WidgetEventArgs args(this);
            onSortMode(&args);

            refresh();
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::setDragMove(bool b)
    {
        if (mDragMove != b)
        {
            mDragMove = b;
            WidgetEventArgs args(this);
            onMoveMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::setClickEnable(bool b)
    {
        if (mClickEnable != b)
        {
            mClickEnable = b;
            WidgetEventArgs args(this);
            onClickMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onItemClick(const WidgetEventArgs * arg)
    {
        signal(ItemClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onSplitterDClick(const WidgetEventArgs * arg)
    {
        signal(SplitterDClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onResizeMode(const WidgetEventArgs * arg)
    {
        signal(ResizeEnableEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onSortMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onMoveMode(const WidgetEventArgs * arg)
    {
        signal(MoveModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onItemDragBegin(const WidgetEventArgs * arg)
    {
        signal(ItemDragBeginEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onItemDragEnd(const WidgetEventArgs * arg)
    {
        signal(ItemDragEndEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onItemDragMove(const WidgetEventArgs * arg)
    {
        refresh();
        signal(ItemDragMoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onItemResize(const WidgetEventArgs * arg)
    {
        refresh();
        signal(ItemResizeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onClickMode(const WidgetEventArgs * arg)
    {
        signal(ClickModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onCursorMove(const CursorEventArgs * arg)
    {
        Widget::onCursorMove(arg);

        const Vector2f localMousePos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
        if (mDragSizing)
        {
            NIIf delta = localMousePos.x - mDragPos.x;

            // store this so we can work out how much size actually changed
            NIIf orgWidth = mPixelSize.mWidth;

            // ensure that we only size to the set constraints.
            //
            // NB: We are required to do this here due to our virtually unique sizing nature; the
            // normal system for limiting the window size is unable to supply the information we
            // require for updating our internal state used to manage the dragging, etc.
            NIIf maxWidth(mMaxSize.mWidth.abs(getRenderSize().mWidth));
            NIIf minWidth(mMinSize.mWidth.abs(getRenderSize().mWidth));
            NIIf newWidth = orgWidth + delta;

            if (maxWidth != 0.0f && newWidth > maxWidth)
                delta = maxWidth - orgWidth;
            else if (newWidth < minWidth)
                delta = minWidth - orgWidth;

            // update segment area rect
            // URGENT FIXME: The pixel alignment will be done automatically again, right? Why is it done here? setAreaImpl will do it!
            RelRectf area(mArea.mLeft, mArea.mTop, mArea.mRight + RelPosf(0,/*PixelAligned(*/delta/*)*/), mArea.mBottom);
            setAreaImpl(RelVector2f(area.mLeft, area.mTop), area.getSize());

            // move the dragging point so mouse remains 'attached' to edge of segment
            mDragPos.x += mPixelSize.mWidth - orgWidth;

            WidgetEventArgs args(this);
            onItemResize(&args);
        }
        else if (mDragMoving)
        {
            // calculate movement deltas.
            NIIf deltaX = localMousePos.x - mDragPos.x;
            NIIf deltaY = localMousePos.y - mDragPos.y;

            // update 'ghost' position
            mDragOffset.x += deltaX;
            mDragOffset.y += deltaY;

            // update drag point.
            mDragPos.x += deltaX;
            mDragPos.y += deltaY;

            WidgetEventArgs args(this);
            onItemDragMove(&args);
        }
        // not sizing, is mouse in the widget area?
        else if (intersects(Vector2f(arg->mLocalX, arg->mLocalY)))
        {
            // mouse in sizing area & sizing is enabled
            if ((localMousePos.x > (mPixelSize.mWidth - mSplitterSize)) && mResizeEnable)
            {
                // only react if settings are changing.
                if (!mSplitterHover && !mItemClick)
                {
                    mSplitterHover = true;

                    // change the mouse cursor.
                    mSheet->getCursor()->setImage(mSizingView);

                    // trigger redraw so 'sizing' area can be highlighted if needed.
                    refresh();
                }

                // reset segment hover as needed.
                if (mItemHover)
                {
                    mItemHover = false;
                    refresh();
                }
            }
            // mouse not in sizing area and/or sizing not enabled
            else
            {
                // reset sizing area hover state if needed.
                if (mSplitterHover)
                {
                    mSplitterHover = false;
                    mSheet->getCursor()->setImage(mAide1->getMouseCursor());
                    refresh();
                }

                // set segment hover state if not already set.
                if ((!mItemHover) && isClickEnable())
                {
                    mItemHover = true;
                    refresh();
                }

                // if we are pushed but not yet drag moving
                if (mItemClick && !mDragMoving)
                {
                    NIIf deltaX = localMousePos.x - mDragPos.x;
                    NIIf deltaY = localMousePos.y - mDragPos.y;

                    if ((deltaX > SegmentMoveThreshold) || (deltaX < -SegmentMoveThreshold) ||
                        (deltaY > SegmentMoveThreshold) || (deltaY < -SegmentMoveThreshold))
                    {
                        if (mDragMove)
                        {
                            // initialise drag moving state
                            mDragMoving = true;
                            mItemClick = false;
                            mItemHover = false;
                            mDragOffset.x = 0.0f;
                            mDragOffset.y = 0.0f;

                            // setup new cursor
                            mSheet->getCursor()->setImage(mMovingView);

                            // Trigger the event
                            WidgetEventArgs args(this);
                            onItemDragBegin(&args);
                        }
                    }
                }
            }
        }
        // mouse is no longer within the widget area...
        else
        {
            // only change settings if change is required
            if (mSplitterHover)
            {
                mSplitterHover = false;
                mSheet->getCursor()->setImage(mAide1->getMouseCursor());
                refresh();
            }

            // reset segment hover state if not already done.
            if (mItemHover)
            {
                mItemHover = false;
                refresh();
            }
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onButtonDown(const CursorEventArgs * arg)
    {
        Widget::onButtonDown(arg);

        if (arg->mMouseArg.mButton == MB_Left)
        {
            // ensure all inputs come to us for now
            if (focus())
            {
                // get position of mouse as co-ordinates local to this window.
                Vector2f localPos(arg->mLocalX - getActualPosX(), arg->mLocalY - getActualPosY());
                // store drag point for possible sizing or moving operation.
                mDragPos = localPos;

                // if the mouse is in the sizing area
                if (mSplitterHover)
                {
                    if (isDragResize())
                    {
                        // setup the 'dragging' state variables
                        mDragSizing = true;
                    }
                }
                else
                {
                    mItemClick = true;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onButtonUp(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonUp(arg);

        if (arg->mMouseArg.mButton == MB_Left)
        {
            // if we were pushed and mouse was released within our segment area
            if (mItemClick && mItemHover)
            {
                WidgetEventArgs args(this);
                onItemClick(&args);
            }
            else if (mDragMoving)
            {
                mSheet->getCursor()->setImage(mAide1->getMouseCursor());

                WidgetEventArgs args(this);
                onItemDragEnd(&args);
            }
            // release our capture on the input data
            lost();
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        Widget::onCursorMultiTouch(arg);

        if ((arg->mMouseArg.mButton == MB_Left) && mSplitterHover)
        {
            WidgetEventArgs args(this);
            onSplitterDClick(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onCursorLeave(const CursorEventArgs * arg)
    {
        Widget::onCursorLeave(arg);

        mSplitterHover = false;
        mDragSizing = false;
        mItemHover = false;
        refresh();
    }
    //------------------------------------------------------------------------
    void ListHeaderItem::onLost(const WidgetEventArgs * arg)
    {
        Widget::onLost(arg);

        mDragSizing = false;
        mItemClick = false;
        mDragMoving = false;
    }
    //------------------------------------------------------------------------
    bool ListHeaderItem::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdSizable());
        dest->add(N_new CmdClickable());
        dest->add(N_new CmdDragable());
        dest->add(N_new CmdHSortDirection());
        dest->add(N_new CmdSizingCursorImage());
        dest->add(N_new CmdMovingCursorImage());
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListHeaderItemView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_STATE(WidgetView, ListHeaderItemView, HoverState, 0);
    N_STATE(WidgetView, ListHeaderItemView, SplitterHoverState, 1);
    N_STATE(WidgetView, ListHeaderItemView, DragState, 2);
    N_STATE(WidgetView, ListHeaderItemView, AscIconState, 3);
    N_STATE(WidgetView, ListHeaderItemView, DescIconState, 4);
    N_STATE(WidgetView, ListHeaderItemView, DragAscIconState, 5);
    N_STATE(WidgetView, ListHeaderItemView, DragDescIconState, 6);
    N_STATE(WidgetView, ListHeaderItemView, StateCount, 7);
    //--------------------------------------------------------------------------
    ListHeaderItemView::ListHeaderItemView(
        WidgetViewlID type) :
        WidgetView(type, N_VIEW_ListHeaderSegment)
    {
    }
    //--------------------------------------------------------------------------
    void ListHeaderItemView::flush()
    {
        ListHeaderItem * w = (ListHeaderItem *)mSrc;

        const Style * style = getStyle();
        const StyleLayout * mode;
        if (w->isFinalDisable())
        {
            mode = &style->getLayout(DisableState);
        }
        else if ((w->isItemHover() != w->isItemClick()) && !w->isSplitterHover() && w->isClickEnable())
        {
            mode = &style->getLayout(HoverState);
        }
        else if (w->isSplitterHover())
        {
            mode = &style->getLayout(SplitterHoverState);
        }
        else
        {
            mode = &style->getLayout(NormalState);
        }

        mode->render(w);
        ListHeaderItem::SortMode sort_dir = w->getSortMode();
        if (sort_dir == ListHeaderItem::SM_Ascend)
        {
            mode = &style->getLayout(AscIconState);
            mode->render(w);
        }
        else if (sort_dir == ListHeaderItem::SM_Descend)
        {
            mode = &style->getLayout(DescIconState);
            mode->render(w);
        }

        // draw ghost copy if the segment is being dragged.
        if (w->isDragMoving())
        {
            PlaneSizef pixel_size = w->getPixelSize();
            Rectf targetArea(0, 0, pixel_size.mWidth, pixel_size.mHeight);
            targetArea += w->getDragOffset();
            mode = &style->getLayout(DragState);
            mode->render(w, targetArea);

            if (sort_dir == ListHeaderItem::SM_Ascend)
            {
                mode = &style->getLayout(DragAscIconState);
                mode->render(w, targetArea);
            }
            else if (sort_dir == ListHeaderItem::SM_Descend)
            {
                mode = &style->getLayout(DragDescIconState);
                mode->render(w, targetArea);
            }
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListHeader
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Container, ListHeader, SortColumnEvent,                     0);
    N_EVENT(Container, ListHeader, SortModeEvent,                       1);
    N_EVENT(Container, ListHeader, ColumnResizeEvent,                   2);
    N_EVENT(Container, ListHeader, ColumnClickEvent,                    3);
    N_EVENT(Container, ListHeader, SplitterDClickEvent,                 4);
    N_EVENT(Container, ListHeader, ColumnIndexEvent,                    5);
    N_EVENT(Container, ListHeader, ColumnAddEvent,                      6);
    N_EVENT(Container, ListHeader, ColumnRemoveEvent,                   7);
    N_EVENT(Container, ListHeader, ColumnMoveModeEvent,                 8);
    N_EVENT(Container, ListHeader, ColumnSizeModeEvent,                 9);
    N_EVENT(Container, ListHeader, ColumnOffsetEvent,                   10);
    N_EVENT(Container, ListHeader, EventCount,                          11);
    const NIIf ListHeader::ScrollSpeed = 8.0f;
    const NIIf ListHeader::ItemMinWidth = 20.0f;
    //------------------------------------------------------------------------
    //const String ListHeader::SegmentNameSuffix("__auto_seg_");
    //------------------------------------------------------------------------
    ListHeader::ListHeader(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_ListHeader, lid),
        mSortItem(0),
        mResizeEnable(true),
        mDragMove(true),
        mColumnOffset(0.0f),
        mSortMode(ListHeaderItem::SM_None)
    {
        notifySignal(ListHeader::EventCount);
        setupCmd(N_CmdObj_ListHeader);
    }
    //------------------------------------------------------------------------
    ListHeader::~ListHeader()
    {
    }
    //------------------------------------------------------------------------
    ListHeaderItem * ListHeader::getColumn(WidgetID id) const
    {
        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            if (mItemList[i]->getID() == id)
            {
                return mItemList[i];
            }
        }
        N_EXCEPT(InvalidRequest, _I18n("no segment with the requested ID is attached to this ListHeader."));
    }
    //------------------------------------------------------------------------
    Nidx ListHeader::getColumnIndex(const ListHeaderItem * segment) const
    {
        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            if (mItemList[i] == segment)
            {
                return i;
            }

        }
        N_EXCEPT(InvalidRequest, _I18n("the given ListHeaderItem is not attached to this ListHeader."));
    }
    //------------------------------------------------------------------------
    Nidx ListHeader::getColumnIndex(WidgetID id) const
    {
        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            if (mItemList[i]->getID() == id)
            {
                return i;
            }

        }

        N_EXCEPT(InvalidRequest, _I18n("no column with the requested ID is available on this ListHeader."));
    }
    //------------------------------------------------------------------------
	Nidx ListHeader::getColumnIndex(const String& text) const
    {
        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            if (mItemList[i]->getText() == text)
            {
                return i;
            }

        }

        N_EXCEPT(InvalidRequest, _I18n("no column with the text '") + text + _I18n("' is attached to this ListHeader."));
    }
    //------------------------------------------------------------------------
    NIIf ListHeader::getColumnPixelOffset(const ListHeaderItem & item) const
    {
        NIIf offset = 0.0f;

        for (Nui32 i = 0; i < getColumnCount(); ++i)
        {
            if (mItemList[i] == &item)
            {
                return offset;
            }

            offset += mItemList[i]->getPixelSize().mWidth;
        }

        N_EXCEPT(InvalidRequest,  _I18n("the given ListHeaderItem is not attached to this ListHeader."));
    }
    //------------------------------------------------------------------------
    NIIf ListHeader::getColumnPixelOffset(Nidx column) const
    {
        if (column >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("requested column index is out of range for this ListHeader."));
        }
        else
        {
            NIIf offset = 0.0f;

            for (Nidx i = 0; i < column; ++i)
            {
                offset += mItemList[i]->getPixelSize().mWidth;
            }
            return offset;
        }
    }
    //------------------------------------------------------------------------
    NIIf ListHeader::getMaxColumnPixelOffset() const
    {
        NIIf extent = 0.0f;
        for (NCount i = 0; i < getColumnCount(); ++i)
        {
            extent += mItemList[i]->getPixelSize().mWidth;
        }
        return extent;
    }
    //------------------------------------------------------------------------
    RelPosf ListHeader::getColumnWidth(Nidx column) const
    {
        if (column >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("requested column index is out of range for this ListHeader."));
        }
        else
        {
            return mItemList[column]->getWidth();
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::setSortMode(ListHeaderItem::SortMode mode)
    {
        if (mSortMode != mode)
        {
            mSortMode = mode;

            if(mSortMode != ListHeaderItem::SM_None)
            {
                for (NCount i = 0; i <getColumnCount(); ++i)
                {
                    mItemList[i]->setClickEnable(true);
                }
            }
            
            if (mSortItem)
            {
                mSortItem->setSortMode(mode);
            }

            WidgetEventArgs args(this);
            onSortMode(&args);
        }

    }
    //------------------------------------------------------------------------
    void ListHeader::setSortColumn(Nidx column)
    {
        if (column >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified column index is out of range for this ListHeader."));
        }
        else
        {
            if (mSortItem != mItemList[column])
            {
                if (mSortItem)
                {
                    mSortItem->setSortMode(ListHeaderItem::SM_None);
                }

                mSortItem = mItemList[column];
                mSortItem->setSortMode(mSortMode);

                WidgetEventArgs args(this);
                onSortColumn(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::getSortColumn(WidgetID id)
    {
        setSortColumn(getColumn(id));
    }
    //------------------------------------------------------------------------
    void ListHeader::setColumnSizeEnable(bool set)
    {
        if (mResizeEnable != set)
        {
            mResizeEnable = set;

            // make the set change for all component segments.
            for (Nui32 i = 0; i <getColumnCount(); ++i)
            {
                mItemList[i]->setDragResize(mResizeEnable);
            }

            // Fire set changed event.
            WidgetEventArgs args(this);
            onColumnSizeMode(&args);
        }

    }
    //------------------------------------------------------------------------
    void ListHeader::setColumnDragEnable(bool set)
    {
        if (mDragMove != set)
        {
            mDragMove = set;
            for (NCount i = 0; i <getColumnCount(); ++i)
            {
                mItemList[i]->setDragMove(mDragMove);
            }

            WidgetEventArgs args(this);
            onColumnMoveMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::addColumn(const String & text, WidgetID id, const RelPosf & width)
    {
        insertColumn(text, id, width, getColumnCount());
    }
    //------------------------------------------------------------------------
    void ListHeader::insertColumn(const String & text, WidgetID id, const RelPosf & width, Nui32 pos)
    {
        if (pos > getColumnCount())
        {
            pos = getColumnCount();
        }

        ListHeaderItem * item = createItem(N_Only(Widget).genValidWidgetID());

        item->setSize(RelPlaneSizef(width, RelPosf(1.0f, 0)));
        item->setMinSize(RelPlaneSizef(RelPosf(0, ItemMinWidth), RelPosf(0, 0)));
        item->setText(text);
        item->setID(id);
        item->setDragResize(mResizeEnable);
        item->setDragMove(mDragMove);
        item->setClickEnable(mSortMode != ListHeaderItem::SM_None);

        item->bind(ListHeaderItem::ItemResizeEvent, this, (CommandObjFunctor)&ListHeader::ItemResizePrc);
        item->bind(ListHeaderItem::ItemDragEndEvent, this, (CommandObjFunctor)&ListHeader::ItemMovePrc);
        item->bind(ListHeaderItem::ItemClickEvent, this, (CommandObjFunctor)&ListHeader::ItemClickPrc);
        item->bind(ListHeaderItem::SplitterDClickEvent, this, (CommandObjFunctor)&ListHeader::ItemDClickPrc);
        item->bind(ListHeaderItem::ItemDragMoveEvent, this, (CommandObjFunctor)&ListHeader::ItemDragPrc);
        
        mItemList.insert((mItemList.begin() + pos), item);
        add(item);

        layoutItem();

        WidgetEventArgs args(this);
        onColumnAdd(&args);

        if (!mSortItem)
        {
            setSortColumn(pos);
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::removeColumn(Nidx column)
    {
        if (column < getColumnCount())
        {
            ListHeaderItem * item = mItemList[column];

            // remove from the list of segments
            mItemList.erase(mItemList.begin() + column);

            if (mSortItem == item)
            {
                if (getColumnCount() > 0)
                {
                    mSortMode = ListHeaderItem::SM_None;
                    setSortColumn(Nidx(0));
                }
                else
                {
                    mSortItem = 0;
                }

            }
            remove(item);

            // destroy the segment (done in derived class, since that's where it was created).
            destroyItem(item);

            layoutItem();

            WidgetEventArgs args(this);
            onColumnRemove(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::moveColumn(Nidx column, Nidx pos)
    {
        if (column >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified column index is out of range for this ListHeader."));
        }
        else
        {
            // if pos is too big, move to end.
            if (pos >= getColumnCount())
            {
                pos = getColumnCount() - 1;
            }

            ListHeaderItem* item = mItemList[column];

            // remove original copy of segment
            mItemList.erase(mItemList.begin() + column);

            // insert the segment at it's new pos
            mItemList.insert(mItemList.begin() + pos, item);

            // Fire sequence changed event
            ColumnEventArgs args(this, column, pos);
            onColumnIndex(&args);

            layoutItem();
        }

    }
    //------------------------------------------------------------------------
    void ListHeader::insertColumn(const String & text, WidgetID id, const RelPosf & width, const ListHeaderItem * pos)
    {
        insertColumn(text, id, width, getColumnIndex(pos));
    }
    //------------------------------------------------------------------------
    void ListHeader::removeColumn(const ListHeaderItem * item)
    {
        removeColumn(getColumnIndex(item));
    }
    //------------------------------------------------------------------------
    void ListHeader::moveColumn(Nidx column, const ListHeaderItem * pos)
    {
        moveColumn(column, getColumnIndex(pos));
    }
    //------------------------------------------------------------------------
    void ListHeader::moveColumn(const ListHeaderItem * item, Nidx pos)
    {
        moveColumn(getColumnIndex(item), pos);
    }
    //------------------------------------------------------------------------
    void ListHeader::moveColumn(const ListHeaderItem * item, const ListHeaderItem * pos)
    {
        moveColumn(getColumnIndex(item), getColumnIndex(pos));
    }
    //------------------------------------------------------------------------
    void ListHeader::setColumnOffset(NIIf offset)
    {
        if(mColumnOffset != offset)
        {
            mColumnOffset = offset;
            layoutItem();
            refresh();

            WidgetEventArgs args(this);
            onColumnOffset(&args);
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::setColumnWidth(Nidx column, const RelPosf & width)
    {
        if(column >= getColumnCount())
        {
            N_EXCEPT(InvalidRequest, _I18n("specified column index is out of range for this ListHeader."));
        }
        else
        {
            mItemList[column]->setSize(RelPlaneSizef(width, mItemList[column]->getSize().mHeight));

            layoutItem();

            WidgetEventArgs args(mItemList[column]);
            onColumnResize(&args);
        }

    }
    //------------------------------------------------------------------------
    void ListHeader::layoutItem()
    {
        RelVector2f pos(RelPosf(0, -mColumnOffset), RelPosf(0, 0.0f));
        for(NCount i = 0; i < getColumnCount(); ++i)
        {
            mItemList[i]->setPos(pos);
            pos.x += mItemList[i]->getWidth();
        }
    }
    //------------------------------------------------------------------------
    bool ListHeader::validImpl(const WidgetView * view) const
    {
        return dynamic_cast<const ListHeaderView *>(view) != 0;
    }
    //------------------------------------------------------------------------
    void ListHeader::onSortColumn(const WidgetEventArgs * arg)
    {
        signal(SortColumnEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onSortMode(const WidgetEventArgs * arg)
    {
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnResize(const WidgetEventArgs * arg)
    {
        signal(ColumnResizeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnClick(const WidgetEventArgs * arg)
    {
        signal(ColumnClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onSplitterDClick(const WidgetEventArgs * arg)
    {
        signal(SplitterDClickEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnIndex(const WidgetEventArgs * arg)
    {
        signal(ColumnIndexEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnAdd(const WidgetEventArgs * arg)
    {
        signal(ColumnAddEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnRemove(const WidgetEventArgs * arg)
    {
        signal(ColumnRemoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnMoveMode(const WidgetEventArgs * arg)
    {
        signal(ColumnMoveModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnSizeMode(const WidgetEventArgs * arg)
    {
        signal(ColumnSizeModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::onColumnOffset(const WidgetEventArgs * arg)
    {
        signal(ColumnOffsetEvent, arg);
    }
    //------------------------------------------------------------------------
    void ListHeader::ItemResizePrc(const EventArgs * arg)
    {
        layoutItem();

        WidgetEventArgs args(static_cast<const WidgetEventArgs *>(arg)->mWidget);
        onColumnResize(&args);
    }
    //------------------------------------------------------------------------
    void ListHeader::ItemMovePrc(const EventArgs * e)
    {
        const Vector2f mousePos(mParent->getWindow()->affine(mSheet->getCursor()->getPos()));
        if(intersects(mousePos))
        {
            // get mouse pos as something local
            Vector2f localMousePos(mousePos.x - getActualPosX(), mousePos.y - getActualPosY());
            // set up to allow for current offsets
            NIIf currwidth = -mColumnOffset;

            // calculate column where dragged segment was dropped
            Nui32 col;
            for(col = 0; col < getColumnCount(); ++col)
            {
                currwidth += mItemList[col]->getPixelSize().mWidth;

                if(localMousePos.x < currwidth)
                {
                    // this is the column, exit loop early
                    break;
                }

            }

            // find original column for dragged segment.
            ListHeaderItem * item = ((ListHeaderItem*)((WidgetEventArgs&)e).mWidget);
            Nidx curcol = getColumnIndex(item);

            // move column
            moveColumn(curcol, col);
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::ItemClickPrc(const EventArgs * arg)
    {
        if (mSortMode != ListHeaderItem::SM_None)
        {
            ListHeaderItem * item = static_cast<ListHeaderItem *>(static_cast<const WidgetEventArgs *>(arg)->mWidget);
            if (mSortItem != item)
            {
                mSortMode = ListHeaderItem::SM_Descend;
                setSortColumn(item);
            }
            else if (mSortItem)
            {
                switch (mSortItem->getSortMode())
                {
                case ListHeaderItem::SM_None:
                    setSortMode(ListHeaderItem::SM_Descend);
                    break;
                case ListHeaderItem::SM_Ascend:
                    setSortMode(ListHeaderItem::SM_Descend);
                    break;
                case ListHeaderItem::SM_Descend:
                    setSortMode(ListHeaderItem::SM_Ascend);
                    break;
                }
            }
            onColumnClick(static_cast<const WidgetEventArgs *>(arg));
        }
    }
    //------------------------------------------------------------------------
    void ListHeader::ItemDClickPrc(const EventArgs * arg)
    {
        WidgetEventArgs args(static_cast<const WidgetEventArgs *>(arg)->mWidget);
        onSplitterDClick(&args);
    }
    //------------------------------------------------------------------------
    void ListHeader::ItemDragPrc(const EventArgs *)
    {
        // what we do here is monitor the pos and scroll if we can when mouse is outside area.
        const Vector2f localMousePos(mParent->getWindow()->affine(mSheet->getCursor()->getPos())
            - Vector2f(getActualPosX(), getActualPosY()));
        // scroll left?
        if (localMousePos.x < 0.0f)
        {
            if (mColumnOffset > 0.0f)
            {
                setColumnOffset(N_MAX(NIIf, 0.0f, mColumnOffset - ScrollSpeed));
            }
        }
        // scroll right?
        else if (localMousePos.x >= mPixelSize.mWidth)
        {
            NIIf maxOffset = N_MAX(NIIf, 0.0f, getMaxColumnPixelOffset() - mPixelSize.mWidth);

            // if we have not scrolled to the limit
            if (mColumnOffset < maxOffset)
            {
                // scroll, but never beyond the limit
                setColumnOffset(N_MIN(NIIf, maxOffset, mColumnOffset + ScrollSpeed));
            }
        }
    }
    //------------------------------------------------------------------------
    bool ListHeader::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdColumnsSizable());
        dest->add(N_new CmdColumnsMovable());
        dest->add(N_new CmdSortColumnID());
        dest->add(N_new CmdSortDirection());
        return true;
    }
    //------------------------------------------------------------------------
    ListHeaderItem * ListHeader::createItem(WidgetID name) const
    {
        if (mWidgetView != 0)
        {
            ListHeaderView* wr = static_cast<ListHeaderView*>(mWidgetView);
            return wr->createItem(name);
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void ListHeader::destroyItem(ListHeaderItem * segment) const
    {
        if (mWidgetView != 0)
        {
            ListHeaderView * wr = static_cast<ListHeaderView *>(mWidgetView);
            wr->destroyItem(segment);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ListHeaderView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdSegmentWidgetType : public PropertyCmd
    {
    public:
        CmdSegmentWidgetType() : PropertyCmd(N_PropertyUIListHeader_SegmentWidgetType, _T("SegmentWidgetType"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ListHeaderView * obj = static_cast<ListHeaderView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv((Nui32)obj->getItemType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            ListHeaderView * obj = static_cast<ListHeaderView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 sid;
            StrConv::conv(val, sid);
            obj->setItemType(sid);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            ListHeaderView * obj = static_cast<ListHeaderView *>(static_cast<const Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)out;
            *temp = obj->getItemType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            ListHeaderView * obj = static_cast<ListHeaderView *>(static_cast<const Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)in;
            obj->setItemType(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSegmentWidgetType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    ListHeaderView::ListHeaderView(WidgetViewlID type) :
        WidgetView(type, N_VIEW_ListHeader)
    {
        add(N_new CmdSegmentWidgetType());
    }
    //------------------------------------------------------------------------
    void ListHeaderView::flush()
    {
        const StyleLayout * ss = &getStyle()->getLayout(mSrc->isFinalDisable() ? DisableState : NormalState);
        ss->render(mSrc);
    }
    //------------------------------------------------------------------------
    ListHeaderItem * ListHeaderView::createItem(WidgetID wid) const
    {
        if (mItemType == 0)
        {
            N_EXCEPT(InvalidRequest, _I18n("Segment widget type has not been set!"));
        }

        Widget * com = N_Only(Widget).create(mItemType, wid, 0);
        com->setAutoWindow(true);
        return static_cast<ListHeaderItem *>(com);
    }
    //------------------------------------------------------------------------
    void ListHeaderView::destroyItem(ListHeaderItem * com) const
    {
        N_Only(Widget).destroy(com);
    }
    //------------------------------------------------------------------------
}
}
