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
#include "PageCoreTests.h"
#include "OgrePaging.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PageCoreTests );

void PageCoreTests::setUp()
{
	mRoot = N_new Root();
	mPageManager = N_new PageManager();

	mRoot->add("./", "FileSystem");

	mSceneMgr = mRoot->createSpaceManager(SpaceTypeVerbose::ST_Normal);
}

void PageCoreTests::tearDown()
{
	N_delete mPageManager;
	N_delete mRoot;
}


void PageCoreTests::testSimpleCreateSaveLoadWorld()
{
	String worldName = "MyWorld";
	String filename = "myworld.world";
	String sectionName1 = "Section1";
	String sectionName2 = "Section2";
	PagedWorld* world = mPageManager->createWorld(worldName);
	PagedWorldSection* section = world->createSection("Grid2D", mSceneMgr, sectionName1);
	section = world->createSection("Grid2D", mSceneMgr, sectionName2);

	// Create a page
	Page* p = section->loadOrCreatePage(Vector3::ZERO);

	SimplePageContentCollection* coll = static_cast<SimplePageContentCollection*>(
		p->createContentCollection("Simple"));


	world->save(filename);

	mPageManager->destroyWorld(world);
	world = 0;

	world = mPageManager->loadWorld(filename);

	CPPUNIT_ASSERT_EQUAL(worldName, world->getName());
	CPPUNIT_ASSERT_EQUAL((size_t)2, world->getSectionCount());

	section = world->getSection(sectionName1);
	CPPUNIT_ASSERT(section != 0);
	section = world->getSection(sectionName2);
	CPPUNIT_ASSERT(section != 0);



}

