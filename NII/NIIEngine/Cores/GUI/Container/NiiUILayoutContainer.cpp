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
#include "NiiUILayoutContainer.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include <algorithm>

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LayoutCell
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LayoutCell::LayoutCell(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_LayoutCell, lid),
        mChildInnerArea(this, (CalcMethod)&LayoutCell::getChildAreaImpl)
    {
        // cell should take the whole window by default I think
        setSize(RelPlaneSizef(RelPosf(1, 0), RelPosf(1, 0)));
    }
    //------------------------------------------------------------------------
    LayoutCell::~LayoutCell()
    {
    }
    //------------------------------------------------------------------------
    const CalcRect & LayoutCell::getChildInnerArea() const
    {
        if (!mParent)
        {
            return Container::getChildInnerArea();
        }
        else
        {
            return mChildInnerArea;
        }
    }
    //------------------------------------------------------------------------
    void LayoutCell::notifyPosChange()
    {
        mChildInnerArea.resetCached();

        Container::notifyPosChange();
    }
    //------------------------------------------------------------------------
    Rectf LayoutCell::getInnerAreaImpl(bool pixelAlign) const
    {
        return mParent ? (pixelAlign ? mParent->getInnerArea().getIntCache() :
            mParent->getInnerArea().getFloatCache()) :
            Container::getInnerAreaImpl(pixelAlign);
    }
    //------------------------------------------------------------------------
    Rectf LayoutCell::getChildAreaImpl(bool pixelAlign) const
    {
        if (!mParent)
        {
            return pixelAlign ? Container::getChildInnerArea().getIntCache() :
                Container::getChildInnerArea().getFloatCache();
        }
        else
        {
            return pixelAlign ?
                Rectf(getArea().getIntCache().getPos(), mParent->getInnerArea().getIntCache().getSize()) :
                Rectf(getArea().getFloatCache().getPos(), mParent->getInnerArea().getFloatCache().getSize());
        }
    }
    //------------------------------------------------------------------------
    void LayoutCell::addImpl(Widget * sub)
    {
        if (getCount() > 0)
        {
            N_EXCEPT(InvalidRequest, _I18n("You can't add more than one widget to a layout cell!"));
        }
        Container::addImpl(sub);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LayoutContainer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LayoutContainer::LayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own,
        const String & name, LangID lid):
        Container(wid, fid, own, name, stid, lid),
        mLayout(false),
        mChildInnerArea(this, (CalcMethod)&LayoutContainer::getChildAreaImpl)
    {
        // layoutImpl should take the whole window by default I think
        setSize(RelPlaneSizef(RelPosf(1, 0), RelPosf(1, 0)));
        bind(Container::AddEvent, this, (CommandObjFunctor)&LayoutContainer::LayoutPrc);
        bind(Container::RemoveEvent, this, (CommandObjFunctor)&LayoutContainer::LayoutPrc);
    }
    //------------------------------------------------------------------------
    LayoutContainer::~LayoutContainer()
    {

    }
    //------------------------------------------------------------------------
    void LayoutContainer::update(TimeDurMS cost)
    {
        Container::update(cost);

        if(mLayout)
        {
            layoutImpl();

            mLayout = false;
        }
    }
    //------------------------------------------------------------------------
    const CalcRect & LayoutContainer::getChildInnerArea() const
    {
        return mChildInnerArea;
    }
    //------------------------------------------------------------------------
    void LayoutContainer::notifyPosChange()
    {
        Container::notifyPosChange();

        mChildInnerArea.resetCached();
    }
    //------------------------------------------------------------------------
    Rectf LayoutContainer::getInnerAreaImpl(bool pixelAlign) const
    {
        return mParent ? (pixelAlign ? mParent->getInnerArea().getIntCache():
            mParent->getInnerArea().getFloatCache()) : Widget::getInnerAreaImpl(pixelAlign);
    }
    //------------------------------------------------------------------------
    Rectf LayoutContainer::getChildAreaImpl(bool pixelAlign) const
    {
        if(!mParent)
        {
            return pixelAlign ? Container::getChildInnerArea().getIntCache() :
                Container::getChildInnerArea().getFloatCache();
        }
        else
        {
            return pixelAlign ?
                Rectf(getArea().getIntCache().getPos(), mParent->getInnerArea().getIntCache().getSize()) :
                Rectf(getArea().getFloatCache().getPos(), mParent->getInnerArea().getFloatCache().getSize());
        }
    }
    //------------------------------------------------------------------------
    void LayoutContainer::addImpl(Widget * widget)
    {
        Container::addImpl(widget);

        mSignalList.insert(Npair(widget, widget->bind(Widget::SizeEvent, this, (CommandObjFunctor)&LayoutContainer::LayoutPrc)));
        mSignalList.insert(Npair(widget, widget->bind(Widget::MarginModeEvent, this, (CommandObjFunctor)&LayoutContainer::LayoutPrc)));
    }
    //------------------------------------------------------------------------
    void LayoutContainer::removeImpl(Widget * sub)
    {
        // we want to get rid of the subscription, because the child window could
        // get removed and added somewhere else, we would be wastefully updating
        // layouts if it was sized inside other Widget
        SignalList::iterator conn, iend = mSignalList.end();
        while((conn = mSignalList.find(sub)) != iend)
        {
            conn->second->disconnect();
            mSignalList.erase(conn);
        }

        Container::removeImpl(sub);
    }
    //------------------------------------------------------------------------
    void LayoutContainer::LayoutPrc(const EventArgs *)
    {
        mLayout = true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // IndexLayoutContainer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    N_EVENT(Container, IndexLayoutContainer, ChildOrderChangeEvent,    0);
    N_EVENT(Container, IndexLayoutContainer, EventCount,               1);
    //------------------------------------------------------------------------
    IndexLayoutContainer::IndexLayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid,
        Container * own = 0, const String & name, LangID lid):
            LayoutContainer(stid, wid, fid, own, name, lid)
    {
        notifySignal(IndexLayoutContainer::EventCount);
    }
    //------------------------------------------------------------------------
    IndexLayoutContainer::~IndexLayoutContainer()
    {
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::swap(Nidx wnd1, Nidx wnd2)
    {
        if(wnd1 < mChilds.size() && wnd2 < mChilds.size())
        {
            std::swap(mChilds[wnd1], mChilds[wnd2]);

            WidgetEventArgs args(this);
            onChildOrderChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::swap(Widget * wnd1, Widget * wnd2)
    {
        if (isExist(wnd1) && isExist(wnd2))
        {
            swap(getIndex(wnd1), getIndex(wnd2));
        }
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::move(Widget * wnd, Nidx position)
    {
        if(!isExist(wnd))
            return;

        position = std::min(position, mChilds.size() - 1);

        const NCount oldPosition = getIndex(wnd);

        if (oldPosition == position)
        {
            return;
        }

        // we get the iterator of the old position
        WidgetList::iterator it = mChilds.begin();
        std::advance(it, oldPosition);

        // we are the window from it's old position
        mChilds.erase(it);

        // if the window comes before the point we want to insert to,
        // we have to decrement the position
        if (oldPosition < position)
        {
            --position;
        }

        // find iterator of the new position
        it = mChilds.begin();
        std::advance(it, position);
        // and insert the window there
        mChilds.insert(it, wnd);

        WidgetEventArgs args(this);
        onChildOrderChange(&args);
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::forward(Widget * window, NCount delta)
    {
        NCount oldPosition = getIndex(window);
        NCount newPosition = oldPosition + delta;
        newPosition = N_MAX(NCount, newPosition, 0);
        // this is handled by move itself
        //newPosition = std::min(newPosition, (int)(mChilds.size() - 1));

        move(window, newPosition);
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::add(Widget * w, Nidx index)
    {
        add(w);

        move(w, index);
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::remove(Nidx index)
    {
        remove(get(index));
    }
    //------------------------------------------------------------------------
    void IndexLayoutContainer::onChildOrderChange(const WidgetEventArgs * arg)
    {
        mLayout = true;
        signal(ChildOrderChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // VerticalLayoutContainer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    VerticalLayoutContainer::VerticalLayoutContainer(WidgetID wid, FactoryID fid, Container * own,
        const String & name, LangID lid) :
            IndexLayoutContainer(N_CmdObj_VerticalLayoutContainer, wid, fid, own, name, lid)
    {
    }
    //------------------------------------------------------------------------
    VerticalLayoutContainer::~VerticalLayoutContainer()
    {
    }
    //------------------------------------------------------------------------
    void VerticalLayoutContainer::layoutImpl()
    {
        // used to compare UDims
        NIIf absWidth = getChildArea(true).getIntCache().getWidth();

        // this is where we store the top offset
        // we continually increase this number as we go through the windows
        RelPosf topOffset(0, 0);
        RelPosf layoutWidth(0, 0);

        WidgetList::iterator it, itend = mChilds.end();
        for (it = mChilds.begin(); it != itend; ++it)
        {
            Widget * widget = *it;

            const RelVector2f & offset = widget->getMarginOft();
            const RelVector2f & boundingSize = widget->getMarginSize();

            // full child widget width, including margins
            const RelPosf & childWidth = boundingSize.x;

            if(layoutWidth.abs(absWidth) < childWidth.abs(absWidth))
            {
                layoutWidth = childWidth;
            }

            widget->setPos(offset + RelVector2f(RelPosf(0, 0), topOffset));
            topOffset += boundingSize.y;
        }

        setSize(RelPlaneSizef(layoutWidth, topOffset));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // HorizontalLayoutContainer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    HorizontalLayoutContainer::HorizontalLayoutContainer(WidgetID wid, FactoryID fid,
        Container * own, const String & name, LangID lid) :
            IndexLayoutContainer(N_CmdObj_HorizontalLayoutContainer, wid, fid, own, name, lid)
    {
    }
    //------------------------------------------------------------------------
    HorizontalLayoutContainer::~HorizontalLayoutContainer()
    {
    }
    //------------------------------------------------------------------------
    void HorizontalLayoutContainer::layoutImpl()
    {
        // used to compare UDims
        const NIIf absHeight = getChildArea(true).getIntCache().getHeight();

        // this is where we store the left offset
        // we continually increase this number as we go through the windows
        RelPosf leftOffset(0, 0);
        RelPosf layoutHeight(0, 0);
        WidgetList::iterator it, itend = mChilds.end();
        for(it = mChilds.begin(); it != itend; ++it)
        {
            Widget * widget = *it;

            const RelVector2f & offset = widget->getMarginOft();
            const RelVector2f & boundingSize = widget->getMarginSize();

            // full child widget width, including margins
            const RelPosf & childHeight = boundingSize.y;

            if (layoutHeight.abs(absHeight) < childHeight.abs(absHeight))
            {
                layoutHeight = childHeight;
            }

            widget->setPos(offset + RelVector2f(leftOffset, RelPosf(0, 0)));
            leftOffset += boundingSize.x;
        }

        setSize(RelPlaneSizef(leftOffset, layoutHeight));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // GridLayoutContainer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdGridSize : public PropertyCmd
    {
    public:
        CmdGridSize() : PropertyCmd(N_PropertyUIGridLayoutContainer_GridSize, _T("grid_size"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            PlaneSizei val = static_cast<const GridLayoutContainer *>(own)->getGrid();
            Ntchar buff[128];
            Nsntprintf(buff, sizeof(buff), _T("%i %i"), val.mWidth, val.mHeight);
            out = buff;
        }
        void set(PropertyCmdObj * own, const String & in)
        {
            PlaneSizei val(0, 0);
            Nsscanf(in.c_str(), _T("%i %i"), &val.mWidth, &val.mHeight);
            static_cast<GridLayoutContainer *>(own)->setGrid(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            PlaneSizei * temp = (PlaneSizei *)out;
            *temp = static_cast<const GridLayoutContainer *>(own)->getGrid();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            PlaneSizei * temp = (PlaneSizei *)in;
            static_cast<GridLayoutContainer *>(own)->setGrid(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdGridSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0 0");
        }
    };

    class _EngineInner CmdAutoPositioning : public PropertyCmd
    {
    public:
        CmdAutoPositioning() : PropertyCmd(N_PropertyUIGridLayoutContainer_LayoutType, _T("auto_positioning"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            GridLayoutContainer::LayoutType val = static_cast<const GridLayoutContainer *>(own)->getLayoutType();

            if(val == GridLayoutContainer::LT_LeftToRight)
            {
                out = _T("Left to Right");
            }
            else if(val == GridLayoutContainer::LT_None)
            {
                out = _T("Disabled");
            }
            else if(val == GridLayoutContainer::LT_TopToBottom)
            {
                out = _T("Top to Bottom");
            }
            else
                out = _T("Left to Right");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            GridLayoutContainer::LayoutType re;

            if(val == _T("Disabled"))
            {
                re = GridLayoutContainer::LT_None;
            }
            else if(val == _T("Top to Bottom"))
            {
                re = GridLayoutContainer::LT_TopToBottom;
            }
            else
            {
                re = GridLayoutContainer::LT_LeftToRight;
            }
            static_cast<GridLayoutContainer *>(own)->setLayoutType(re);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            GridLayoutContainer::LayoutType * temp = (GridLayoutContainer::LayoutType *)out;
            *temp = static_cast<const GridLayoutContainer *>(own)->getLayoutType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            GridLayoutContainer::LayoutType * temp = (GridLayoutContainer::LayoutType *)in;
            static_cast<GridLayoutContainer *>(own)->setLayoutType(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoPositioning();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("Left to Right");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, GridLayoutContainer, ChildOrderChangeEvent,    0);
    N_EVENT(Container, GridLayoutContainer, EventCount,               1);
    //------------------------------------------------------------------------
    GridLayoutContainer::GridLayoutContainer(WidgetID wid, FactoryID fid, Container * own,
        const String & name, LangID lid) :
        LayoutContainer(N_CmdObj_GridLayoutContainer, wid, fid, own, name, lid),
        mLayoutType(LT_LeftToRight),
        mGridWidth(0),
        mGridHeight(0),
        mNextIndex(0),
        mNextGridX(std::numeric_limits<NCount>::max()),
        mNextGridY(std::numeric_limits<NCount>::max())
    {
        notifySignal(GridLayoutContainer::EventCount);
        // grid size is 0x0 that means 0 child windows,
        // no need to populate mChilds with dummies

        setupCmd(N_CmdObj_GridLayoutContainer);
    }
    //------------------------------------------------------------------------
    GridLayoutContainer::~GridLayoutContainer()
    {

    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::setGrid(NCount width, NCount height)
    {
        // copy the old children list
        WidgetList oldChildren = mChilds;

        // remove all child windows
        while(getCount() != 0)
        {
            remove(mChilds[0]);
        }

        // we simply fill the grid with dummies to ensure everything works smoothly
        // when something is added to the grid, it simply replaces the dummy
        for(NCount i = 0; i < width * height; ++i)
        {
            Widget * dummy = createDummy();
            add(dummy);
        }

        const NCount oldWidth = mGridWidth;

        const NCount oldHeight = mGridHeight;

        const LayoutType oldAO = mLayoutType;

        mGridWidth = width;

        mGridHeight = height;

        // now we have to map oldChildren to new children
        for (NCount y = 0; y < height; ++y)
        {
            for (NCount x = 0; x < width; ++x)
            {
                // we have to skip if we are out of the old grid
                if (x >= oldWidth || y >= oldHeight)
                    continue;

                const NCount oldIdx = getGridIndex(x, y, oldWidth, oldHeight);
                Widget * previous = oldChildren[oldIdx];

                if(previous->isAutoWindow())
                {
                    N_Only(Widget).destroy(previous);
                }
                else
                {
                    add(previous, x, y);
                }

                oldChildren[oldIdx] = 0;
            }
        }

        setLayoutType(oldAO);
        // oldAOIdx could mean something completely different now!
        // todo: perhaps convert oldAOOdx to new AOIdx?
        setNextLayoutIndex(0);

        // we have to destroy windows that don't fit the new grid if they are set
        // to be destroyed by parent
        for (NCount i = 0; i < oldChildren.size(); ++i)
        {
            if (oldChildren[i] && oldChildren[i]->isAutoDestroy())
            {
                N_Only(Widget).destroy(oldChildren[i]);
            }
        }
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::setGrid(const PlaneSizei & size)
    {
        setGrid(N_MAX(NCount, 0, size.mWidth), N_MAX(NCount, 0, size.mHeight));
    }
    //------------------------------------------------------------------------
    PlaneSizei GridLayoutContainer::getGrid() const
    {
        return PlaneSizei(getGridWidth(), getGridHeight());
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::add(Widget * sub, NCount x, NCount y)
    {
        // when user starts to add windows to specific locations, AO has to be disabled
        LayoutType old = mLayoutType;
        setLayoutType(LT_None);
        mNextGridX = x;
        mNextGridY = y;

        LayoutContainer::add(sub);
        setLayoutType(old);
    }
    //------------------------------------------------------------------------
    Widget * GridLayoutContainer::get(Nidx x, Nidx y)
    {
        N_assert(x < mGridWidth, "out of bounds");
        N_assert(y < mGridHeight, "out of bounds");

        return get(getGridIndex(x, y, mGridWidth, mGridHeight));
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::remove(Nidx x, Nidx y)
    {
        remove(get(x, y));
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::swap(Nidx sub1, Nidx sub2)
    {
        if(sub1 < mChilds.size() && sub2 < mChilds.size())
        {
            std::swap(mChilds[sub1], mChilds[sub2]);

            WidgetEventArgs args(this);
            onChildOrderChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::swap(NCount x1, NCount y1, NCount x2, NCount y2)
    {
        swap(getGridIndex(x1, y1, mGridWidth, mGridHeight),
            getGridIndex(x2, y2, mGridWidth, mGridHeight));
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::swap(Widget * sub1, Widget * sub2)
    {
        swap(getIndex(sub1), getIndex(sub2));
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::move(Widget * wnd, NCount x, NCount y)
    {
        remove(wnd);
        add(wnd, x, y);
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::layoutImpl()
    {
        std::vector<RelPosf> colSizes(mGridWidth, RelPosf(0, 0));
        std::vector<RelPosf> rowSizes(mGridHeight, RelPosf(0, 0));

        // used to compare UDims
        NIIf absWidth = getChildArea(true).getIntCache().getWidth();
        NIIf absHeight = getChildArea(true).getIntCache().getHeight();

        // first, we need to determine rowSizes and colSizes, this is needed before
        // any layouting work takes place
        for(NCount y = 0; y < mGridHeight; ++y)
        {
            for(NCount x = 0; x < mGridWidth; ++x)
            {
                // x and y is the position of window in the grid
                const NCount childIdx = getGridIndex(x, y, mGridWidth, mGridHeight);

                Widget * window = get(childIdx);
                const RelVector2f size = window->getMarginSize();

                if(colSizes[x].abs(absWidth) < size.x.abs(absWidth))
                {
                    colSizes[x] = size.x;
                }

                if(rowSizes[y].abs(absHeight) < size.y.abs(absHeight))
                {
                    rowSizes[y] = size.y;
                }
            }
        }

        // OK, now in rowSizes[y] is the height of y-th row
        //         in colSizes[x] is the width of x-th column

        // second layouting phase starts now
        for(NCount y = 0; y < mGridHeight; ++y)
        {
            for(NCount x = 0; x < mGridWidth; ++x)
            {
                // x and y is the position of window in the grid
                const NCount childIdx = getGridIndex(x, y, mGridWidth, mGridHeight);
                Widget * window = get(childIdx);
                const RelVector2f & offset = window->getMarginOft();
                RelVector2f gridCellOffset = RelVector2f(RelPosf(0, 0), RelPosf(0, 0));

                for(NCount i = 0; i < x; ++i)
                {
                    gridCellOffset.x += colSizes[i];
                }

                for(NCount i = 0; i < y; ++i)
                {
                    gridCellOffset.y += rowSizes[i];
                }

                window->setPos(gridCellOffset + offset);
            }
        }

        // now we just need to determine the total width and height and set it
        RelPlaneSizef ret(RelPosf(0, 0), RelPosf(0, 0));
        for(NCount i = 0; i < colSizes.size(); ++i)
        {
            ret.mWidth += colSizes[i];
        }
        for(NCount i = 0; i < rowSizes.size(); ++i)
        {
            ret.mHeight += rowSizes[i];
        }
        setSize(ret);
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::onChildOrderChange(const WidgetEventArgs * arg)
    {
        mLayout = true;
        signal(ChildOrderChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    NCount GridLayoutContainer::getGridIndex(NCount x, NCount y, NCount gridWidth, NCount gridHeight) const
    {
        // example:
        // mChilds = {1, 2, 3, 4, 5, 6}
        // grid is 3x2
        // 1 2 3
        // 4 5 6

        N_assert(x < gridWidth, "error logic");
        N_assert(y < gridHeight, "error logic");

        return y * gridWidth + x;
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::getGridPos(Nidx idx, NCount & gridX, NCount & gridY, NCount gridWidth, NCount gridHeight) const
    {
        gridX = 0;
        gridY = 0;

        while(idx >= gridWidth)
        {
            idx -= gridWidth;
            ++gridY;
        }

        N_assert(gridY < gridHeight, "error logic");

        gridX = idx;
    }
    //------------------------------------------------------------------------
    Nidx GridLayoutContainer::getGridIndex(Nidx APIdx) const
    {
        // todo: more auto positioning variants? will someone use them?

        if(mLayoutType == LT_None)
        {
            N_assert(0, "error logic");
        }
        else if(mLayoutType == LT_LeftToRight)
        {
            // this is the same positioning as implementation
            return APIdx;
        }
        else if(mLayoutType == LT_TopToBottom)
        {
            // we want
            // 1 3 5
            // 2 4 6

            NCount x, y;
            bool done = false;

            for(x = 0; x < mGridWidth; ++x)
            {
                for(y = 0; y < mGridHeight; ++y)
                {
                    if(APIdx == 0)
                    {
                        done = true;
                        break;
                    }

                    --APIdx;
                }

                if(done)
                {
                    break;
                }
            }

            N_assert(APIdx == 0, "error logic");
            return getGridIndex(x, y, mGridWidth, mGridHeight);
        }

        // should never happen
        N_assert(0, "error logic");
        return APIdx;
    }
    //------------------------------------------------------------------------
    Widget * GridLayoutContainer::createDummy()
    {
        Widget * dummy = N_Only(Widget).create(N_WIDGET_DefaultWindow, 0, 0);

        dummy->setAutoWindow(true);
        dummy->setVisible(false);
        dummy->setSize(RelPlaneSizef(RelPosf(0, 0), RelPosf(0, 0)));
        dummy->setAutoDestroy(true);

        return dummy;
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::addImpl(Widget * widget)
    {
        if(widget->isAutoWindow())
        {
            LayoutContainer::addImpl(widget);
        }
        else
        {
            LayoutContainer::addImpl(widget);

            // OK, widget is already in mChilds

            // idx is the future index of the child that's being added
            NCount idx;

            if(mLayoutType == LT_None)
            {
                if((mNextGridX == std::numeric_limits<NCount>::max()) &&
                    (mNextGridY == std::numeric_limits<NCount>::max()))
                {
                    N_EXCEPT(InvalidRequest, _I18n("错误排版容器设置"));
                }

                idx = getGridIndex(mNextGridX, mNextGridY, mGridWidth, mGridHeight);
                // reset location to sentinel values.
                mNextGridX = mNextGridY = std::numeric_limits<NCount>::max();
            }
            else
            {
                idx = getGridIndex(mNextIndex);
                ++mNextIndex;
            }

            // we swap the dummy and the added child
            // this essentially places the added child to it's right position and
            // puts the dummy at the end of mChilds it will soon get removed from
            std::swap(mChilds[idx], mChilds[mChilds.size() - 1]);

            Widget * toBeRemoved = mChilds[mChilds.size() - 1];
            remove(toBeRemoved);

            if(toBeRemoved->isAutoDestroy())
            {
                N_Only(Widget).destroy(toBeRemoved);
            }
        }
    }
    //------------------------------------------------------------------------
    void GridLayoutContainer::removeImpl(Widget * sub)
    {
        if(!sub->isAutoWindow())
        {
            // before we remove the child, we must add new dummy and place it
            // instead of the removed child
            Widget * dummy = createDummy();
            add(dummy);

            NCount i = getIndex(sub);
            std::swap(mChilds[i], mChilds[mChilds.size() - 1]);
        }

        LayoutContainer::removeImpl(sub);
    }
    //------------------------------------------------------------------------
    bool GridLayoutContainer::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdGridSize());
        dest->add(N_new CmdAutoPositioning());
        return true;
    }
    //------------------------------------------------------------------------
}
}