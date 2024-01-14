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
#include "OgrePagedWorldSection.h"
#include "OgrePageStrategy.h"
#include "OgreStreamSerialiser.h"
#include "OgreException.h"
#include "OgrePagedWorld.h"
#include "OgrePageManager.h"
#include "OgrePage.h"
#include "OgreLogManager.h"
#include "OgreRoot.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	const uint32 PagedWorldSection::CHUNK_ID = N_MAGIC4('P','W','S','C');
	const uint16 PagedWorldSection::CHUNK_VERSION = 1;
	//---------------------------------------------------------------------
	PagedWorldSection::PagedWorldSection(const String & name, PagedWorld * parent,
        SpaceManager * sm) : 
            mName(name),
            mParent(parent),
            mStrategy(0),
            mPageProvider(0),
            mSceneMgr(sm)
	{
	}
	//---------------------------------------------------------------------
	PagedWorldSection::~PagedWorldSection()
	{
		if (mStrategy)
		{
			mStrategy->destroyData(mStrategyData);
			mStrategyData = 0;
		}

		removeAllPages();
	}
	//---------------------------------------------------------------------
	PageManager* PagedWorldSection::getManager() const
	{
		return mParent->getManager();
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::setAABB(const AABox & box)
	{
		mAABB = box;
	}
	//---------------------------------------------------------------------
	const AABox & PagedWorldSection::getAABB() const
	{
		return mAABB;
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::setStrategy(PageStrategy* strat)
	{
		if (strat != mStrategy)
		{
			if (mStrategy)
			{
				mStrategy->destroyData(mStrategyData);
				mStrategy = 0;
				mStrategyData = 0;
			}

			mStrategy = strat;
			if (mStrategy)
				mStrategyData = mStrategy->createData();

			removeAllPages();
		}
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::setStrategy(const String & stratName)
	{
		setStrategy(getManager()->getStrategy(stratName));
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::setSceneManager(SpaceManager * sm)
	{
		if(sm != mSceneMgr)
		{
			mSceneMgr = sm;
			removeAllPages();
		}
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::setSceneManager(const String & smName)
	{
		setSceneManager(Root::getSingleton().getSpaceManager(smName));
	}
	//---------------------------------------------------------------------
	bool PagedWorldSection::load(FileStreamSerializer& ser)
	{
		if (!ser.readChunkBegin(CHUNK_ID, CHUNK_VERSION, "PagedWorldSection"))
			return false;

		// Name
		ser.read(&mName);
		// AABB
		ser.read(&mAABB);
		// SpaceManager type
		String smType, smInstanceName;
		SpaceManager* sm = 0;
		ser.read(&smType);
		ser.read(&smInstanceName);
		Root& root = Root::getSingleton();
		if (root.isSpaceManagerExist(smInstanceName))
			sm = root.getSpaceManager(smInstanceName);
		else
			sm = root.createSpaceManager(smType, smInstanceName);
		setSceneManager(sm);
		// Page Strategy Name
		String stratname;
		ser.read(&stratname);
		setStrategy(stratname);
		// Page Strategy Data
		bool strategyDataOk = mStrategyData->load(ser);
		if (!strategyDataOk)
			LogManager::getSingleton().stream() << "Error: PageStrategyData for section '"
			<< mName << "' was not loaded correctly, check file contents";

		/// Load any data specific to a subtype of this class
		loadSubtypeData(ser);

		ser.readChunkEnd(CHUNK_ID);

		return true;

	}
	//---------------------------------------------------------------------
	void PagedWorldSection::save(FileStreamSerializer& ser)
	{
		ser.writeChunkBegin(CHUNK_ID, CHUNK_VERSION);

		// Name
		ser.write(&mName);
		// AABB
		ser.write(&mAABB);
		// SpaceManager type & name
		ser.write(&mSceneMgr->getTypeID());
		ser.write(&mSceneMgr->getName());
		// Page Strategy Name
		ser.write(&mStrategy->getName());
		// Page Strategy Data
		mStrategyData->save(ser);

		/// Save any data specific to a subtype of this class
		saveSubtypeData(ser);

		ser.writeChunkEnd(CHUNK_ID);

		// save all pages (in separate files)
		for (PageMap::iterator i = mPages.begin(); i != mPages.end(); ++i)
		{
			i->second->save();
		}


	}
	//---------------------------------------------------------------------
	PageID PagedWorldSection::getPageID(const Vector3& worldPos)
	{
		return mStrategy->getPageID(worldPos, this);
	}
	//---------------------------------------------------------------------
	Page* PagedWorldSection::loadOrCreatePage(const Vector3& worldPos)
	{
		PageID id = getPageID(worldPos);
		// this will create a Page instance no matter what, even if load fails
		// we force the load attempt to happen immediately (forceSynchronous)
		loadPage(id, true);
		return getPage(id);
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::loadPage(PageID pageID, bool sync)
	{
		if (!mParent->getManager()->getPagingOperationsEnabled())
			return;

		PageMap::iterator i = mPages.find(pageID);
		if (i == mPages.end())
		{
			Page* page = N_new Page(pageID, this);
			// try to insert
			std::pair<PageMap::iterator, bool> ret = mPages.insert(
				PageMap::value_type(page->getID(), page));

			if (!ret.second)
			{
				// page with this ID already in map
				if (ret.first->second != page)
				{
					// replacing a page, delete the old one
					N_delete ret.first->second;
					ret.first->second = page;
				}
			}
			page->load(sync);
		}
		else
			i->second->ref();
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::unloadPage(PageID pageID, bool sync)
	{
		if (!mParent->getManager()->getPagingOperationsEnabled())
			return;

		PageMap::iterator i = mPages.find(pageID);
		if (i != mPages.end())
		{
			Page* page = i->second;
			mPages.erase(i);

			page->unload();

			N_delete page;
			
		}
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::unloadPage(Page* p, bool sync)
	{
		unloadPage(p->getID(), sync);
	}
	//---------------------------------------------------------------------
	bool PagedWorldSection::_prepareProceduralPage(Page* page)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->prepareProceduralPage(page, this);
		if (!generated)
			generated = mParent->_prepareProceduralPage(page, this);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorldSection::_loadProceduralPage(Page* page)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->loadProceduralPage(page, this);
		if (!generated)
			generated = mParent->_loadProceduralPage(page, this);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorldSection::_unloadProceduralPage(Page* page)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->unloadProceduralPage(page, this);
		if (!generated)
			generated = mParent->_unloadProceduralPage(page, this);
		return generated;

	}
	//---------------------------------------------------------------------
	bool PagedWorldSection::_unprepareProceduralPage(Page* page)
	{
		bool generated = false;
		if (mPageProvider)
			generated = mPageProvider->unprepareProceduralPage(page, this);
		if (!generated)
			generated = mParent->_unprepareProceduralPage(page, this);
		return generated;

	}
	//---------------------------------------------------------------------
	void PagedWorldSection::holdPage(PageID pageID)
	{
		PageMap::iterator i = mPages.find(pageID);
		if (i != mPages.end())
			i->second->ref();
	}
	//---------------------------------------------------------------------
	Page* PagedWorldSection::getPage(PageID pageID)
	{
		PageMap::iterator i = mPages.find(pageID);
		if (i != mPages.end())
			return i->second;
		else
			return 0;
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::removeAllPages()
	{
		if(!mParent->getManager()->getPagingOperationsEnabled())
			return;

		for(PageMap::iterator i= mPages.begin(); i != mPages.end(); ++i)
		{
			N_delete i->second;
		}
		mPages.clear();
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::frameStart(NIIf timeSinceLastFrame)
	{
		mStrategy->frameStart(timeSinceLastFrame, this);

		for (PageMap::iterator i = mPages.begin(); i != mPages.end(); ++i)
			i->second->frameStart(timeSinceLastFrame);
	}
	//---------------------------------------------------------------------
	void PagedWorldSection::frameEnd(NIIf timeElapsed)
	{
		mStrategy->frameEnd(timeElapsed, this);

		for (PageMap::iterator i = mPages.begin(); i != mPages.end(); )
		{
			// if this page wasn't used, unload
			Page* p = i->second;
			// pre-increment since unloading will remove it
			++i;
			if (!p->isHeld())
				unloadPage(p);
			else
				p->frameEnd(timeElapsed);
		}

	}
	//---------------------------------------------------------------------
	void PagedWorldSection::notifyCamera(Camera* cam)
	{
		mStrategy->notifyCamera(cam, this);

		for (PageMap::iterator i = mPages.begin(); i != mPages.end(); ++i)
			i->second->notifyCamera(cam);
	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PagedWorldSection::_readPageStream(PageID pageID)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->readPageStream(pageID, this);
		if (!ser)
			ser = mParent->_readPageStream(pageID, this);
		return ser;

	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PagedWorldSection::_writePageStream(PageID pageID)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->writePageStream(pageID, this);
		if (!ser)
			ser = mParent->_writePageStream(pageID, this);
		return ser;

	}
	//---------------------------------------------------------------------
	const String& PagedWorldSection::getType()
	{
		static const String stype("General");
		return stype;
	}
	//---------------------------------------------------------------------
	std::ostream& operator <<( std::ostream& o, const PagedWorldSection& p )
	{
		o << "PagedWorldSection(" << p.getName() << ", world:" << p.getWorld()->getName() << ")";
		return o;
	}





}

