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
#include "TerrainTests.h"
#include "OgreTerrain.h"
#include "OgreConfigFile.h"
#include "OgreResourceLoadManager.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TerrainTests);

void TerrainTests::setUp()
{
	mRoot = N_new Root();

	// Load resource paths from config file
	ConfigFile cf;
	cf.load("resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.isNextValid())
	{
		secName = seci.getCurrentKey();
		ConfigFile::SettingsMultiMap *settings = seci.getAndNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			N_Only(ResourceScheme).add(archName, typeName, secName);
		}
	}
	mSceneMgr = mRoot->createSpaceManager(SpaceTypeVerbose::ST_Normal);
}

void TerrainTests::tearDown()
{
	N_delete mRoot;
}

void TerrainTests::testCreate()
{
	Terrain * t = N_new Terrain(mSceneMgr);
	Image img;
	img.load("terrain.png", GroupDefault);

	Terrain::ImportData imp;
	imp.inputImage = &img;
	imp.terrainSize = 513;
	imp.worldSize = 1000;
	imp.minBatchSize = 33;
	imp.maxBatchSize = 65;
	t->prepare(imp);
	// don't load, this requires GPU access
	//t->load();

	N_delete t;
}