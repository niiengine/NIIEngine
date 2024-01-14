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
#include "OgrePagedWorld.h"
#include "OgreResourceLoadManager.h"

#include "OgrePageManager.h"
#include "OgrePagedWorldSection.h"
#include "OgreStreamSerialiser.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	const uint32 PagedWorld::CHUNK_ID = N_MAGIC4('P','W','L','D');
	const uint32 PagedWorld::CHUNK_SECTIONDECLARATION_ID = N_MAGIC4('P','W','L','S');
	const uint16 PagedWorld::CHUNK_VERSION = 1;
	//---------------------------------------------------------------------
	PagedWorld::PagedWorld(const String& name, PageManager* manager)
		:mName(name), mManager(manager), mPageProvider(0), mSectionNameGenerator("Section")
	{

	}
	//---------------------------------------------------------------------
	PagedWorld::~PagedWorld()
	{
		destroyAllSections();
	}
	//---------------------------------------------------------------------
	void PagedWorld::load(const String& filename)
	{
		FileStreamSerializer* ser = mManager->_readWorldStream(filename);
		load(*ser);
		N_delete ser;
	}
	//---------------------------------------------------------------------
	void PagedWorld::load(const DataStream *  stream)
	{
		FileStreamSerializer ser(stream);
		load(ser);
	}
	//---------------------------------------------------------------------
	bool PagedWorld::load(FileStreamSerializer& ser)
	{
		if (!ser.readChunkBegin(CHUNK_ID, CHUNK_VERSION, "PagedWorld"))
			return false;

		// Name
		ser.read(&mName);
		// Sections
		while(ser.peekNextChunkID() == CHUNK_SECTIONDECLARATION_ID)
		{
			ser.readChunkBegin();
			String sectionType, sectionName;
			ser.read(&sectionType);
			ser.read(&sectionName);
			ser.readChunkEnd(CHUNK_SECTIONDECLARATION_ID);
			// Scene manager will be loaded
			PagedWorldSection* sec = createSection(0, sectionType, sectionName);
			bool sectionsOk = sec->load(ser);
			if (!sectionsOk)
				destroySection(sec);
		}

		ser.readChunkEnd(CHUNK_ID);

		return true;

	}
	//---------------------------------------------------------------------
	void PagedWorld::save(const String& filename)
	{
		FileStreamSerializer* ser = mManager->_writeWorldStream(filename);
		save(*ser);
		N_delete ser;
	}
	//---------------------------------------------------------------------
	void PagedWorld::save(const DataStream * stream)
	{
		FileStreamSerializer ser(stream);
		save(ser);
	}
	//---------------------------------------------------------------------
	void PagedWorld::save(FileStreamSerializer& ser)
	{
		ser.writeChunkBegin(CHUNK_ID, CHUNK_VERSION);

		// Name
		ser.write(&mName);
		// Sections
		for (SectionMap::iterator i = mSections.begin(); i != mSections.end(); ++i)
		{
			PagedWorldSection* sec = i->second;
			// declaration
			ser.writeChunkBegin(CHUNK_SECTIONDECLARATION_ID);
			ser.write(&sec->getType());
			ser.write(&sec->getName());
			ser.writeChunkEnd(CHUNK_SECTIONDECLARATION_ID);
			// data
			i->second->save(ser);
		}

		ser.writeChunkEnd(CHUNK_ID);
	}
	//---------------------------------------------------------------------
	PagedWorldSection * PagedWorld::createSection(SpaceManager * sceneMgr,
		const String & typeName,
		const String & sectionName /*= StringUtil::BLANK*/)
	{
		String theName = sectionName;
		if(theName.empty())
		{
			do 
			{
				theName = mSectionNameGenerator.generate();
			} while (mSections.find(theName) != mSections.end());
		}
		else if(mSections.find(theName) != mSections.end())
		{
			N_EXCEPT(Exception::ERR_DUPLICATE_ITEM, 
				"World section named '" + theName + "' already exists!");
		}

		PagedWorldSection* ret = 0;
		if (typeName == "General")
			ret = N_new PagedWorldSection(theName, this, sceneMgr);
		else
		{
			PagedWorldSectionFactory* fact = getManager()->getWorldSectionFactory(typeName);
			if (!fact)
			{
				N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
					"World section type '" + typeName + "' does not exist!");
			}

			ret = fact->createInstance(theName, this, sceneMgr);

		}
		mSections[theName] = ret;

		return ret;
	}
	//---------------------------------------------------------------------
	PagedWorldSection* PagedWorld::createSection(const String& strategyName, SpaceManager* sceneMgr,
		const String& sectionName)
	{
		// get the strategy
		PageStrategy* strategy = mManager->getStrategy(strategyName);

		return createSection(strategy, sceneMgr, sectionName);
		
	}
	//---------------------------------------------------------------------
	PagedWorldSection* PagedWorld::createSection(PageStrategy* strategy, SpaceManager* sceneMgr, 
		const String& sectionName)
	{
		PagedWorldSection* ret = createSection(sceneMgr, "General", sectionName);
		ret->setStrategy(strategy);

		return ret;
	}

	//---------------------------------------------------------------------
	void PagedWorld::destroySection(const String& name)
	{
		SectionMap::iterator i = mSections.find(name);
		if (i != mSections.end())
		{
			N_delete i->second;
			mSections.erase(i);
		}
	}
	//---------------------------------------------------------------------
	void PagedWorld::destroySection(PagedWorldSection* sec)
	{
		destroySection(sec->getName());
	}
	//---------------------------------------------------------------------
	void PagedWorld::destroyAllSections()
	{
		for (SectionMap::iterator i = mSections.begin(); i != mSections.end(); ++i)
			N_delete i->second;
		mSections.clear();
	}
	//---------------------------------------------------------------------
	PagedWorldSection* PagedWorld::getSection(const String& name)
	{
		SectionMap::iterator i = mSections.find(name);
		if (i != mSections.end())
			return i->second;
		else
			return 0;

	}
	//---------------------------------------------------------------------
	bool PagedWorld::_prepareProceduralPage(Page* page, PagedWorldSection* section)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->prepareProceduralPage(page, section);
		if (!generated)
			generated = mManager->_prepareProceduralPage(page, section);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorld::_loadProceduralPage(Page* page, PagedWorldSection* section)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->loadProceduralPage(page, section);
		if (!generated)
			generated = mManager->_loadProceduralPage(page, section);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorld::_unprepareProceduralPage(Page* page, PagedWorldSection* section)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->unprepareProceduralPage(page, section);
		if (!generated)
			generated = mManager->_unprepareProceduralPage(page, section);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorld::_unloadProceduralPage(Page* page, PagedWorldSection* section)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->unloadProceduralPage(page, section);
		if (!generated)
			generated = mManager->_unloadProceduralPage(page, section);
		return generated;

	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PagedWorld::_readPageStream(PageID pageID, PagedWorldSection* section)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->readPageStream(pageID, section);
		if (!ser)
			ser = mManager->_readPageStream(pageID, section);
		return ser;

	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PagedWorld::_writePageStream(PageID pageID, PagedWorldSection* section)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->writePageStream(pageID, section);
		if (!ser)
			ser = mManager->_writePageStream(pageID, section);
		return ser;

	}
	//---------------------------------------------------------------------
	void PagedWorld::frameStart(NIIf t)
	{
		for (SectionMap::iterator i = mSections.begin(); i != mSections.end(); ++i)
		{
			i->second->frameStart(t);
		}
	}
	//---------------------------------------------------------------------
	void PagedWorld::frameEnd(NIIf t)
	{
		for (SectionMap::iterator i = mSections.begin(); i != mSections.end(); ++i)
		{
			i->second->frameEnd(t);
		}
	}
	//---------------------------------------------------------------------
	void PagedWorld::notifyCamera(Camera* cam)
	{
		for (SectionMap::iterator i = mSections.begin(); i != mSections.end(); ++i)
		{
			i->second->notifyCamera(cam);
		}
	}
	//---------------------------------------------------------------------
	std::ostream& operator <<( std::ostream& o, const PagedWorld& p )
	{
		o << "PagedWorld(" << p.getName() << ")";
		return o;
	}




}

