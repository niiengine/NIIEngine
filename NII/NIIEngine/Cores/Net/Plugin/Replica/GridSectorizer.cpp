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
#include "GridSectorizer.h"
#include <math.h>

namespace NII
{
namespace NII_NET
{
    //------------------------------------------------------------------------
    GridSectorizer::GridSectorizer()
    {
        grid=0;
    }
    //------------------------------------------------------------------------
    GridSectorizer::~GridSectorizer()
    {
        if (grid)
            N_free(grid);
    }
    //------------------------------------------------------------------------
    void GridSectorizer::Init(const NIIf _maxCellWidth, const NIIf _maxCellHeight,
        const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY)
    {
        N_assert(_maxCellWidth > 0.0f && _maxCellHeight > 0.0f);
        if (grid)
            N_free(grid);

        cellOriginX=minX;
        cellOriginY=minY;
        gridWidth=maxX-minX;
        gridHeight=maxY-minY;
        gridCellWidthCount=(int) ceil(gridWidth/_maxCellWidth);
        gridCellHeightCount=(int) ceil(gridHeight/_maxCellHeight);
        // Make the cells slightly smaller, so we allocate an extra unneeded cell if on the edge.  This way we don't go outside the array on rounding errors.
        cellWidth=gridWidth/gridCellWidthCount;
        cellHeight=gridHeight/gridCellHeightCount;
        invCellWidth = 1.0f / cellWidth;
        invCellHeight = 1.0f / cellHeight;

        grid = N_alloc_t(DataStructures::List<void*>, gridCellWidthCount*gridCellHeightCount);
    }
    //------------------------------------------------------------------------
    void GridSectorizer::AddEntry(void *entry, const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY)
    {
        N_assert(cellWidth>0.0f);
        N_assert(minX < maxX && minY < maxY);

        int xStart, yStart, xEnd, yEnd, xCur, yCur;
        xStart=WorldToCellXOffsetAndClamped(minX);
        yStart=WorldToCellYOffsetAndClamped(minY);
        xEnd=WorldToCellXOffsetAndClamped(maxX);
        yEnd=WorldToCellYOffsetAndClamped(maxY);

        for (xCur=xStart; xCur <= xEnd; ++xCur)
        {
            for (yCur=yStart; yCur <= yEnd; ++yCur)
            {
    #ifdef _USE_ORDERED_LIST
                grid[yCur*gridCellWidthCount+xCur].Insert(entry,entry, true);
    #else
                grid[yCur*gridCellWidthCount+xCur].Insert(entry);
    #endif
            }
        }
    }
    //------------------------------------------------------------------------
    #ifdef _USE_ORDERED_LIST
    void GridSectorizer::RemoveEntry(void *entry, const NIIf minX, const NIIf minY,
        const NIIf maxX, const NIIf maxY)
    {
        N_assert(cellWidth>0.0f);
        N_assert(minX <= maxX && minY <= maxY);

        int xStart, yStart, xEnd, yEnd, xCur, yCur;
        xStart=WorldToCellXOffsetAndClamped(minX);
        yStart=WorldToCellYOffsetAndClamped(minY);
        xEnd=WorldToCellXOffsetAndClamped(maxX);
        yEnd=WorldToCellYOffsetAndClamped(maxY);

        for (xCur=xStart; xCur <= xEnd; ++xCur)
        {
            for (yCur=yStart; yCur <= yEnd; ++yCur)
            {
                grid[yCur*gridCellWidthCount+xCur].RemoveIfExists(entry);
            }
        }
    }
    //------------------------------------------------------------------------
    void GridSectorizer::MoveEntry(void * entry, const NIIf sourceMinX,
        const NIIf sourceMinY, const NIIf sourceMaxX, const NIIf sourceMaxY,
            const NIIf destMinX, const NIIf destMinY, const NIIf destMaxX,
                const NIIf destMaxY)
    {
        N_assert(cellWidth>0.0f);
        N_assert(sourceMinX < sourceMaxX && sourceMinY < sourceMaxY);
        N_assert(destMinX < destMaxX && destMinY < destMaxY);

        if (PositionCrossesCells(sourceMinX, sourceMinY, destMinX, destMinY)==false &&
            PositionCrossesCells(destMinX, destMinY, destMinX, destMinY)==false)
            return;

        int xStartSource, yStartSource, xEndSource, yEndSource;
        int xStartDest, yStartDest, xEndDest, yEndDest;
        int xCur, yCur;
        xStartSource=WorldToCellXOffsetAndClamped(sourceMinX);
        yStartSource=WorldToCellYOffsetAndClamped(sourceMinY);
        xEndSource=WorldToCellXOffsetAndClamped(sourceMaxX);
        yEndSource=WorldToCellYOffsetAndClamped(sourceMaxY);

        xStartDest=WorldToCellXOffsetAndClamped(destMinX);
        yStartDest=WorldToCellYOffsetAndClamped(destMinY);
        xEndDest=WorldToCellXOffsetAndClamped(destMaxX);
        yEndDest=WorldToCellYOffsetAndClamped(destMaxY);

        // Remove source that is not in dest
        for (xCur=xStartSource; xCur <= xEndSource; ++xCur)
        {
            for (yCur=yStartSource; yCur <= yEndSource; ++yCur)
            {
                if (xCur < xStartDest || xCur > xEndDest ||
                    yCur < yStartDest || yCur > yEndDest)
                {
                    grid[yCur*gridCellWidthCount+xCur].RemoveIfExists(entry);
                }
            }
        }

        // Add dest that is not in source
        for (xCur=xStartDest; xCur <= xEndDest; ++xCur)
        {
            for (yCur=yStartDest; yCur <= yEndDest; ++yCur)
            {
                if (xCur < xStartSource || xCur > xEndSource ||
                    yCur < yStartSource || yCur > yEndSource)
                {
                    grid[yCur*gridCellWidthCount+xCur].Insert(entry,entry, true);
                }
            }
        }
    }
    #endif
    //------------------------------------------------------------------------
    void GridSectorizer::GetEntries(DataStructures::List<void *> & intersectionList, 
        const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY)
    {
    #ifdef _USE_ORDERED_LIST
        DataStructures::OrderedList<void*, void*>* cell;
    #else
        DataStructures::List<void*>* cell;
    #endif
        int xStart, yStart, xEnd, yEnd, xCur, yCur;
        Nui32 index;
        xStart=WorldToCellXOffsetAndClamped(minX);
        yStart=WorldToCellYOffsetAndClamped(minY);
        xEnd=WorldToCellXOffsetAndClamped(maxX);
        yEnd=WorldToCellYOffsetAndClamped(maxY);

        intersectionList.Clear(true);
        for (xCur=xStart; xCur <= xEnd; ++xCur)
        {
            for (yCur=yStart; yCur <= yEnd; ++yCur)
            {
                cell = grid+yCur*gridCellWidthCount+xCur;
                for (index=0; index < cell->Size(); ++index)
                    intersectionList.Insert(cell->operator [](index));
            }
        }
    }
    //------------------------------------------------------------------------
    bool GridSectorizer::PositionCrossesCells(const NIIf originX, const NIIf originY,
        const NIIf destinationX, const NIIf destinationY) const
    {
        return originX/cellWidth!=destinationX/cellWidth || originY/cellHeight!=destinationY/cellHeight;
    }
    //------------------------------------------------------------------------
    int GridSectorizer::WorldToCellX(const NIIf input) const
    {
        return (int)((input-cellOriginX)*invCellWidth);
    }
    //------------------------------------------------------------------------
    int GridSectorizer::WorldToCellY(const NIIf input) const
    {
        return (int)((input-cellOriginY)*invCellHeight);
    }
    //------------------------------------------------------------------------
    int GridSectorizer::WorldToCellXOffsetAndClamped(const NIIf input) const
    {
        int cell=WorldToCellX(input);
        cell = cell > 0 ? cell : 0; // __max(cell,0);
        cell = gridCellWidthCount-1 < cell ? gridCellWidthCount-1 : cell; // __min(gridCellWidthCount-1, cell);
        return cell;
    }
    //------------------------------------------------------------------------
    int GridSectorizer::WorldToCellYOffsetAndClamped(const NIIf input) const
    {
        int cell=WorldToCellY(input);
        cell = cell > 0 ? cell : 0; // __max(cell,0);
        cell = gridCellHeightCount-1 < cell ? gridCellHeightCount-1 : cell; // __min(gridCellHeightCount-1, cell);
        return cell;
    }
    //------------------------------------------------------------------------
    void GridSectorizer::Clear(void)
    {
        int cur;
        int count = gridCellWidthCount*gridCellHeightCount;
        for (cur=0; cur<count;cur++)
            grid[cur].Clear(true);
    }
    //------------------------------------------------------------------------
}
}