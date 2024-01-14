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

#ifndef _GRID_SECTORIZER_H
#define _GRID_SECTORIZER_H

//#define _USE_ORDERED_LIST

#include "NiiNetPreInclude.h"

#ifdef _USE_ORDERED_LIST
#include "DS_OrderedList.h"
#else
#include "DS_List.h"
#endif

class GridSectorizer
{
public:
	GridSectorizer();
	~GridSectorizer();

	// _cellWidth, _cellHeight is the width and height of each cell in world units
	// minX, minY, maxX, maxY are the world dimensions (can be changed to dynamically allocate later if needed)
	void Init(const NIIf _maxCellWidth, const NIIf _maxCellHeight, const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY);

	// Adds a pointer to the grid with bounding rectangle dimensions
	void AddEntry(void *entry, const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY);

#ifdef _USE_ORDERED_LIST

	// Removes a pointer, as above
	void RemoveEntry(void *entry, const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY);

	// Adds and removes in one pass, more efficient than calling both functions consecutively
	void MoveEntry(void *entry, const NIIf sourceMinX, const NIIf sourceMinY, const NIIf sourceMaxX, const NIIf sourceMaxY,
		const NIIf destMinX, const NIIf destMinY, const NIIf destMaxX, const NIIf destMaxY);

#endif

	// Adds to intersectionList all entries in a certain radius
	void GetEntries(DataStructures::List<void*>& intersectionList, const NIIf minX, const NIIf minY, const NIIf maxX, const NIIf maxY);

	void Clear(void);

protected:
	int WorldToCellX(const NIIf input) const;
	int WorldToCellY(const NIIf input) const;
	int WorldToCellXOffsetAndClamped(const NIIf input) const;
	int WorldToCellYOffsetAndClamped(const NIIf input) const;

	// Returns true or false if a position crosses cells in the grid.  If false, you don't need to move entries
	bool PositionCrossesCells(const NIIf originX, const NIIf originY, const NIIf destinationX, const NIIf destinationY) const;

	NIIf cellOriginX, cellOriginY;
	NIIf cellWidth, cellHeight;
	NIIf invCellWidth, invCellHeight;
	NIIf gridWidth, gridHeight;
	int gridCellWidthCount, gridCellHeightCount;
#ifdef _USE_ORDERED_LIST
	DataStructures::OrderedList<void*, void*>* grid;
#else
	DataStructures::List<void*>* grid;
#endif
};

#endif
