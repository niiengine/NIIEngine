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
#include "OgreTerrainPagedWorldSection.h"
#include "OgreTerrainGroup.h"
#include "OgreGrid2DPageStrategy.h"
#include "OgrePagedWorld.h"
#include "OgrePageManager.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	TerrainPagedWorldSection::TerrainPagedWorldSection(const String& name, 
        PagedWorld * parent, SpaceManager * sm) : 
            PagedWorldSection(name, parent, sm), 
            mTerrainGroup(0)
	{
		// we always use a grid strategy
		setStrategy(parent->getManager()->getStrategy("Grid2D"));
	}
	//---------------------------------------------------------------------
	TerrainPagedWorldSection::~TerrainPagedWorldSection()
	{
		N_delete mTerrainGroup;
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::init(TerrainGroup* grp)
	{
		if (mTerrainGroup == grp)
			return;

		if (mTerrainGroup)
			N_delete mTerrainGroup;

		mTerrainGroup = grp;
		syncSettings();

		// Unload all existing terrain pages, because we want the paging system
		// to be in charge of this
		mTerrainGroup->removeAllTerrains();
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::syncSettings()
	{

		// Base grid on terrain settings
		Grid2DPageStrategyData* gridData = getGridStrategyData();
		switch (mTerrainGroup->getAlignment())
		{
		case Terrain::ALIGN_X_Y:
			gridData->setMode(G2D_X_Y);
			break;
		case Terrain::ALIGN_X_Z:
			gridData->setMode(G2D_X_Z);
			break;
		case Terrain::ALIGN_Y_Z:
			gridData->setMode(G2D_Y_Z);
			break;
		}
		gridData->setOrigin(mTerrainGroup->getOrigin());

		gridData->setCellSize(mTerrainGroup->getTerrainWorldSize());

	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setLoadRadius(NIIf sz)
	{
		getGridStrategyData()->setLoadRadius(sz);
	}
	//---------------------------------------------------------------------
	NIIf TerrainPagedWorldSection::getLoadRadius() const
	{
		return getGridStrategyData()->getLoadRadius();
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setHoldRadius(NIIf sz)
	{
		getGridStrategyData()->setHoldRadius(sz);
	}
	//---------------------------------------------------------------------
	NIIf TerrainPagedWorldSection::getHoldRadius()
	{
		return getGridStrategyData()->getHoldRadius();
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setPageRange(int32 minX, int32 minY, int32 maxX, int32 maxY)
	{
		getGridStrategyData()->setCellRange(minX, minY, maxX, maxY);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setPageRangeMinX(int32 minX)
	{
		getGridStrategyData()->setCellRangeMinX(minX);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setPageRangeMinY(int32 minY)
	{
		getGridStrategyData()->setCellRangeMinY(minY);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setPageRangeMaxX(int32 maxX)
	{
		getGridStrategyData()->setCellRangeMaxX(maxX);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::setPageRangeMaxY(int32 maxY)
	{
		getGridStrategyData()->setCellRangeMaxY(maxY);
	}
	//---------------------------------------------------------------------
	int32 TerrainPagedWorldSection::getPageRangeMinX() const
	{
		return getGridStrategyData()->getCellRangeMinX();
	}
	//---------------------------------------------------------------------
	int32 TerrainPagedWorldSection::getPageRangeMinY() const
	{
		return getGridStrategyData()->getCellRangeMinY();
	}
	//---------------------------------------------------------------------
	int32 TerrainPagedWorldSection::getPageRangeMaxX() const
	{
		return getGridStrategyData()->getCellRangeMaxX();
	}
	//---------------------------------------------------------------------
	int32 TerrainPagedWorldSection::getPageRangeMaxY() const
	{
		return getGridStrategyData()->getCellRangeMaxY();
	}
	//---------------------------------------------------------------------
	Grid2DPageStrategy* TerrainPagedWorldSection::getGridStrategy() const
	{
		return static_cast<Grid2DPageStrategy *>(this->getStrategy());
	}
	//---------------------------------------------------------------------
	Grid2DPageStrategyData * TerrainPagedWorldSection::getGridStrategyData() const
	{
		return static_cast<Grid2DPageStrategyData *>(mStrategyData);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::loadSubtypeData(FileStreamSerializer& ser)
	{
		// we load the TerrainGroup information from here
		if(!mTerrainGroup)
			mTerrainGroup = N_new TerrainGroup(getSceneManager());

		mTerrainGroup->loadGroupDefinition(ser);

		// params that are in the Grid2DStrategyData will have already been loaded
		// as part of the main load() routine

		syncSettings();
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::saveSubtypeData(FileStreamSerializer & ser)
	{
		mTerrainGroup->saveGroupDefinition(ser);

		// params that are in the Grid2DStrategyData will have already been saved
		// as part of the main save() routine

	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::loadPage(PageID pageID, bool forceSynchronous)
	{
		if (!mParent->getManager()->getPagingOperationsEnabled())
			return;

		PageMap::iterator i = mPages.find(pageID);
		if (i == mPages.end())
		{
			// trigger terrain load
			long x, y;
			// pageID is the same as a packed index
			mTerrainGroup->unpackIndex(pageID, &x, &y);
			mTerrainGroup->defineTerrain(x, y);
			mTerrainGroup->loadTerrain(x, y, forceSynchronous);
		}

		PagedWorldSection::loadPage(pageID, forceSynchronous);
	}
	//---------------------------------------------------------------------
	void TerrainPagedWorldSection::unloadPage(PageID pageID, bool forceSynchronous)
	{
		if (!mParent->getManager()->getPagingOperationsEnabled())
			return;

		PagedWorldSection::unloadPage(pageID, forceSynchronous);


		// trigger terrain unload
		long x, y;
		// pageID is the same as a packed index
		mTerrainGroup->unpackIndex(pageID, &x, &y);
		mTerrainGroup->unloadTerrain(x, y);


	}




}

