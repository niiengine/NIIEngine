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
#include "OgreTerrainPaging.h"
#include "OgreTerrainPagedWorldSection.h"
#include "OgreTerrainGroup.h"
#include "OgrePagedWorld.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	TerrainPaging::TerrainPaging(PageManager* pageMgr)
		: mManager(pageMgr)
	{
		mManager->addWorldSectionFactory(&mSectionFactory);
	}
	//---------------------------------------------------------------------
	TerrainPaging::~TerrainPaging()
	{
		mManager->removeWorldSectionFactory(&mSectionFactory);
	}
	//---------------------------------------------------------------------
	TerrainPagedWorldSection * TerrainPaging::createWorldSection(
		PagedWorld * world, TerrainGroup * terrainGroup, NIIf loadRadius, NIIf holdRadius, 
        int32 minX, int32 minY, int32 maxX, int32 maxY, const String & sectionName)
	{
		TerrainPagedWorldSection* ret = static_cast<TerrainPagedWorldSection*>(
			world->createSection(terrainGroup->getSceneManager(), SectionFactory::FACTORY_NAME, sectionName));

		ret->init(terrainGroup);
		ret->setLoadRadius(loadRadius);
		ret->setHoldRadius(holdRadius);
		ret->setPageRange(minX, minY, maxX, maxY);

		return ret;

	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	const String TerrainPaging::SectionFactory::FACTORY_NAME("Terrain");

	const String & TerrainPaging::SectionFactory::getName() const
	{
		return FACTORY_NAME;
	}
	//---------------------------------------------------------------------
	PagedWorldSection * TerrainPaging::SectionFactory::createInstance(const String& name,
        PagedWorld * parent, SpaceManager * sm)
	{
		return N_new TerrainPagedWorldSection(name, parent, sm);
	}
	//---------------------------------------------------------------------
	void TerrainPaging::SectionFactory::destroyInstance(PagedWorldSection* s)
	{
		N_delete s;
	}


}

