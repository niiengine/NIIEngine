/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgrePageManager.h"
#include "OgrePagedWorld.h"
#include "OgrePageStrategy.h"
#include "OgrePageContentCollectionFactory.h"
#include "OgrePagedWorldSection.h"
#include "OgrePageContentFactory.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgrePagedWorldSection.h"
#include "OgrePagedWorld.h"
#include "OgreGrid2DPageStrategy.h"
#include "OgreGrid3DPageStrategy.h"
#include "OgreSimplePageContentCollection.h"
#include "OgreStreamSerialiser.h"
#include "OgreRoot.h"
#include "OgrePageContent.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	PageManager::PageManager()
		: mWorldNameGenerator("World")
		, mPageProvider(0)
		, mPageResourceGroup(GroupDefault)
		, mDebugDisplayLvl(0)
		, mPagingEnabled(true)
		, mGrid2DPageStrategy(0)
		, mGrid3DPageStrategy(0)
		, mSimpleCollectionFactory(0)
	{

		mEventRouter.pManager = this;
		mEventRouter.pWorldMap = &mWorlds;
		mEventRouter.pCameraList = &mCameraList;

		Root::getSingleton().addFrameListener(&mEventRouter);

		createStandardStrategies();
		createStandardContentFactories();

	}
	//---------------------------------------------------------------------
	PageManager::~PageManager()
	{
		Root::getSingleton().removeFrameListener(&mEventRouter);

		N_delete mGrid3DPageStrategy;
		N_delete mGrid2DPageStrategy;
		N_delete mSimpleCollectionFactory;
	}
	//---------------------------------------------------------------------
	void PageManager::createStandardStrategies()
	{
		mGrid2DPageStrategy = N_new Grid2DPageStrategy(this);
		addStrategy(mGrid2DPageStrategy);

		mGrid3DPageStrategy = N_new Grid3DPageStrategy(this);
		addStrategy(mGrid3DPageStrategy);
	}
	//---------------------------------------------------------------------
	void PageManager::createStandardContentFactories()
	{
		// collections
		mSimpleCollectionFactory = N_new SimplePageContentCollectionFactory();
		addContentCollectionFactory(mSimpleCollectionFactory);

	}
	//---------------------------------------------------------------------
	PagedWorld * PageManager::createWorld(const String& name)
	{
		String theName = name;
		if (theName.empty())
		{
			do
			{
				theName = mWorldNameGenerator.generate();
			} while (mWorlds.find(theName) != mWorlds.end());
		}
		else if(mWorlds.find(theName) != mWorlds.end())
		{
			N_EXCEPT(Exception::ERR_DUPLICATE_ITEM,
				"World named '" + theName + "' already exists!");
		}

		PagedWorld * ret = N_new PagedWorld(theName, this);
		mWorlds[theName] = ret;

		return ret;
	}
	//---------------------------------------------------------------------
	void PageManager::destroyWorld(const String& name)
	{
		WorldMap::iterator i = mWorlds.find(name);
		if (i != mWorlds.end())
		{
			N_delete i->second;
			mWorlds.erase(i);
		}
	}
	//---------------------------------------------------------------------
	void PageManager::destroyWorld(PagedWorld* world)
	{
		destroyWorld(world->getName());
	}
	//---------------------------------------------------------------------
	PagedWorld * PageManager::loadWorld(const String& filename, const String& name)
	{
		PagedWorld * ret = createWorld(name);

		FileStreamSerializer * ser = _readWorldStream(filename);
		ret->load(*ser);
		N_delete ser;

		return ret;
	}
	//---------------------------------------------------------------------
	PagedWorld* PageManager::loadWorld(const DataStream * stream, const String& name)
	{
		PagedWorld* ret = createWorld(name);

		ret->load(stream);

		return ret;
	}
	//---------------------------------------------------------------------
	void PageManager::saveWorld(PagedWorld* world, const String& filename)
	{
		world->save(filename);
	}
	//---------------------------------------------------------------------
	void PageManager::saveWorld(PagedWorld* world, const DataStream * stream)
	{
		world->save(stream);
	}
	//---------------------------------------------------------------------
	PagedWorld* PageManager::getWorld(const String& name)
	{
		WorldMap::iterator i = mWorlds.find(name);
		if (i != mWorlds.end())
			return i->second;
		else
			return 0;
	}
	//---------------------------------------------------------------------
	void PageManager::addStrategy(PageStrategy * obj)
	{
		// note - deliberately allowing overriding
		mStrategies[obj->getName()] = obj;
	}
	//---------------------------------------------------------------------
	void PageManager::removeStrategy(PageStrategy * obj)
	{
		StrategyMap::iterator i = mStrategies.find(obj->getName());
		if (i != mStrategies.end() && i->second == obj)
		{
			mStrategies.erase(i);
		}
	}
	//---------------------------------------------------------------------
	PageStrategy* PageManager::getStrategy(const String& name)
	{
		StrategyMap::iterator i = mStrategies.find(name);
		if (i != mStrategies.end())
			return i->second;
		else
			return 0;

	}
	//---------------------------------------------------------------------
	const PageManager::StrategyMap& PageManager::getStrategies() const
	{
		return mStrategies;
	}
	//---------------------------------------------------------------------
	void PageManager::addContentCollectionFactory(PageContentCollectionFactory* f)
	{
		// note - deliberately allowing overriding
		mContentCollectionFactories[f->getName()] = f;
	}
	//---------------------------------------------------------------------
	void PageManager::removeContentCollectionFactory(PageContentCollectionFactory* f)
	{
		ContentCollectionFactoryMap::iterator i = mContentCollectionFactories.find(f->getName());
		if (i != mContentCollectionFactories.end() && i->second == f)
		{
			mContentCollectionFactories.erase(i);
		}
	}
	//---------------------------------------------------------------------
	PageContentCollectionFactory* PageManager::getContentCollectionFactory(const String& name)
	{
		ContentCollectionFactoryMap::iterator i = mContentCollectionFactories.find(name);
		if (i != mContentCollectionFactories.end())
			return i->second;
		else
			return 0;

	}
	//---------------------------------------------------------------------
	const PageManager::ContentCollectionFactoryMap& PageManager::getContentCollectionFactories() const
	{
		return mContentCollectionFactories;
	}
	//---------------------------------------------------------------------
	PageContentCollection* PageManager::createContentCollection(const String& typeName)
	{
		PageContentCollectionFactory* fact = getContentCollectionFactory(typeName);
		if (!fact)
			N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
				typeName + " is not the name of a valid PageContentCollectionFactory",
				"PageManager::createContentCollection");

		return fact->createInstance();
	}
	//---------------------------------------------------------------------
	void PageManager::destroyContentCollection(PageContentCollection* coll)
	{
		PageContentCollectionFactory* fact = getContentCollectionFactory(coll->getType());
		if (fact)
			fact->destroyInstance(coll);
		else
			N_delete coll; // normally a safe fallback
	}
	//---------------------------------------------------------------------
	void PageManager::addContentFactory(PageContentFactory* f)
	{
		// note - deliberately allowing overriding
		mContentFactories[f->getName()] = f;
	}
	//---------------------------------------------------------------------
	void PageManager::removeContentFactory(PageContentFactory* f)
	{
		ContentFactoryMap::iterator i = mContentFactories.find(f->getName());
		if (i != mContentFactories.end() && i->second == f)
		{
			mContentFactories.erase(i);
		}
	}
	//---------------------------------------------------------------------
	PageContentFactory* PageManager::getContentFactory(const String& name)
	{
		ContentFactoryMap::iterator i = mContentFactories.find(name);
		if (i != mContentFactories.end())
			return i->second;
		else
			return 0;

	}
	//---------------------------------------------------------------------
	const PageManager::ContentFactoryMap& PageManager::getContentFactories() const
	{
		return mContentFactories;
	}
	//---------------------------------------------------------------------
	PageContent* PageManager::createContent(const String& typeName)
	{
		PageContentFactory* fact = getContentFactory(typeName);
		if (!fact)
			N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			typeName + " is not the name of a valid PageContentFactory",
			"PageManager::createContent");

		return fact->createInstance();
	}
	//---------------------------------------------------------------------
	void PageManager::destroyContent(PageContent* c)
	{
		PageContentFactory* fact = getContentFactory(c->getType());
		if (fact)
			fact->destroyInstance(c);
		else
			N_delete c; // normally a safe fallback
	}
	//---------------------------------------------------------------------
	void PageManager::addWorldSectionFactory(PagedWorldSectionFactory* f)
	{
		// note - deliberately allowing overriding
		mWorldSectionFactories[f->getName()] = f;
	}
	//---------------------------------------------------------------------
	void PageManager::removeWorldSectionFactory(PagedWorldSectionFactory* f)
	{
		WorldSectionFactoryMap::iterator i = mWorldSectionFactories.find(f->getName());
		if (i != mWorldSectionFactories.end() && i->second == f)
		{
			mWorldSectionFactories.erase(i);
		}
	}
	//---------------------------------------------------------------------
	PagedWorldSectionFactory* PageManager::getWorldSectionFactory(const String& name)
	{
		WorldSectionFactoryMap::iterator i = mWorldSectionFactories.find(name);
		if (i != mWorldSectionFactories.end())
			return i->second;
		else
			return 0;

	}
	//---------------------------------------------------------------------
	const PageManager::WorldSectionFactoryMap& PageManager::getWorldSectionFactories() const
	{
		return mWorldSectionFactories;
	}
	//---------------------------------------------------------------------
	PagedWorldSection * PageManager::createWorldSection(const String & typeName,
		const String & name, PagedWorld * parent, SpaceManager * sm)
	{
		PagedWorldSectionFactory * fact = getWorldSectionFactory(typeName);
		if(!fact)
            N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
                typeName + " is not the name of a valid PagedWorldSectionFactory");

		return fact->createInstance(name, parent, sm);
	}
	//---------------------------------------------------------------------
	void PageManager::destroyWorldSection(PagedWorldSection * coll)
	{
		PagedWorldSectionFactory* fact = getWorldSectionFactory(coll->getType());
		if (fact)
			fact->destroyInstance(coll);
		else
			N_delete coll; // normally a safe fallback
	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PageManager::_readPageStream(PageID pageID, PagedWorldSection * section)
	{
		FileStreamSerializer* ser = 0;
		if(mPageProvider)
			ser = mPageProvider->readPageStream(pageID, section);
		if(!ser)
		{
			// use default implementation
			StringUtil::StrStreamType nameStr;
			nameStr << section->getWorld()->getName() << "_" << section->getName()
				<< "_" << pageID << ".page";
			DataStream * stream = ResourceSchemeManager::getSingleton().openResource(nameStr.str());

			ser = N_new FileStreamSerializer(stream);
            N_delete stream;
		}

		return ser;
	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PageManager::_writePageStream(PageID pageID, PagedWorldSection* section)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->writePageStream(pageID, section);
		if (!ser)
		{
			// use default implementation
			StringUtil::StrStreamType nameStr;
			nameStr << section->getWorld()->getName() << "_" << section->getName()
				<< "_" << pageID << ".page";

			// create file, overwrite if necessary
			DataStream * stream = N_Only(ResourceScheme).createSrc(
				nameStr.str(), mPageResourceGroup, true);

			ser = N_new FileStreamSerializer(stream);
            N_delete stream;
		}

		return ser;
	}
	//---------------------------------------------------------------------
	FileStreamSerializer * PageManager::_readWorldStream(const String & filename)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->readWorldStream(filename);
		if (!ser)
		{
			// use default implementation
			DataStream * stream = ResourceSchemeManager::getSingleton().openResource(filename);

			ser = N_new FileStreamSerializer(stream);
            N_delete stream;
		}
		return ser;
	}
	//---------------------------------------------------------------------
	FileStreamSerializer* PageManager::_writeWorldStream(const String & filename)
	{
		FileStreamSerializer* ser = 0;
		if (mPageProvider)
			ser = mPageProvider->writeWorldStream(filename);
		if (!ser)
		{
			// use default implementation
			// create file, overwrite if necessary
			DataStream * stream = N_Only(ResourceScheme).createSrc(
				filename, mPageResourceGroup, true);

			ser = N_new FileStreamSerializer(stream);
            N_delete stream;
		}

		return ser;
	}
	//---------------------------------------------------------------------
	bool PageManager::_prepareProceduralPage(Page * page, PagedWorldSection * section)
	{
		bool generated = false;
		if(mPageProvider)
			generated = mPageProvider->prepareProceduralPage(page, section);

		return generated;
	}
	//---------------------------------------------------------------------
	bool PageManager::_loadProceduralPage(Page * page, PagedWorldSection * section)
	{
		bool generated = false;
		if(mPageProvider)
			generated = mPageProvider->loadProceduralPage(page, section);

		return generated;
	}
	//---------------------------------------------------------------------
	bool PageManager::_unprepareProceduralPage(Page * page, PagedWorldSection * section)
	{
		bool generated = false;
		if(mPageProvider)
			generated = mPageProvider->unprepareProceduralPage(page, section);

		return generated;
	}
	//---------------------------------------------------------------------
	bool PageManager::_unloadProceduralPage(Page * page, PagedWorldSection * section)
	{
		bool generated = false;
		if(mPageProvider)
			generated = mPageProvider->unloadProceduralPage(page, section);

		return generated;
	}
	//---------------------------------------------------------------------
	void PageManager::addCamera(Camera * c)
	{
		if(std::find(mCameraList.begin(), mCameraList.end(), c) == mCameraList.end())
		{
			mCameraList.push_back(c);
			c->add(&mEventRouter);
		}
	}
	//---------------------------------------------------------------------
	void PageManager::removeCamera(Camera * c)
	{
		CameraList::iterator i = std::find(mCameraList.begin(), mCameraList.end(), c);
		if(i != mCameraList.end())
		{
			c->remove(&mEventRouter);
			mCameraList.erase(i);
		}
	}
	//---------------------------------------------------------------------
	bool PageManager::isCameraExist(Camera * c) const
	{
		return std::find(mCameraList.begin(), mCameraList.end(), c) != mCameraList.end();
	}
	//---------------------------------------------------------------------
	const CameraList & PageManager::getCameraList() const
	{
		return mCameraList;
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	void PageManager::EventRouter::onCull(Camera * obj)
	{
	}
	//---------------------------------------------------------------------
	void PageManager::EventRouter::onDestroy(Camera * obj)
	{
		pManager->removeCamera(obj);
	}
	//---------------------------------------------------------------------
	bool PageManager::EventRouter::onPreRender(const RenderFrameArgs * evt)
	{
		for(WorldMap::iterator i = pWorldMap->begin(); i != pWorldMap->end(); ++i)
		{
			i->second->frameStart(evt->mCurrent);
			// Notify of all active cameras
			// Previously we did this in onPreRender, but that had the effect
			// of causing unnecessary unloading of pages if a camera was rendered
			// intermittently, so we assume that all cameras we're told to watch are 'active'
			for(CameraList::iterator c = pCameraList->begin(); c != pCameraList->end(); ++c)
			{
				i->second->notifyCamera(*c);
			}
		}
		return true;
	}
	//---------------------------------------------------------------------
	bool PageManager::EventRouter::onEndRender(const RenderFrameArgs * evt)
	{
		for(WorldMap::iterator i = pWorldMap->begin(); i != pWorldMap->end(); ++i)
			i->second->frameEnd(evt->mCurrent);

		return true;
	}
}