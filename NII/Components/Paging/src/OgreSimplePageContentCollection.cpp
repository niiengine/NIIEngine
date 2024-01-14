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
#include "OgreSimplePageContentCollection.h"
#include "OgrePageContent.h"
#include "OgreStreamSerialiser.h"
#include "OgrePageManager.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	const uint32 SimplePageContentCollection::SUBCLASS_CHUNK_ID = N_MAGIC4('S','P','C','D');
	const uint16 SimplePageContentCollection::SUBCLASS_CHUNK_VERSION = 1;
	//---------------------------------------------------------------------
	SimplePageContentCollection::SimplePageContentCollection(PageContentCollectionFactory* creator)
		: PageContentCollection(creator)
	{

	}
	//---------------------------------------------------------------------
	SimplePageContentCollection::~SimplePageContentCollection()
	{
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			delete *i;
		mContentList.clear();
	}
	//---------------------------------------------------------------------
	PageContent* SimplePageContentCollection::createContent(const String& typeName)
	{
		PageContent* c = getManager()->createContent(typeName);
		mContentList.push_back(c);
		return c;
	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::destroyContent(PageContent* c)
	{
		ContentList::iterator i = std::find(mContentList.begin(), mContentList.end(), c);
		if (i != mContentList.end())
			mContentList.erase(i);
		getManager()->destroyContent(c);
	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::save(FileStreamSerializer& stream)
	{
		stream.writeChunkBegin(SUBCLASS_CHUNK_ID, SUBCLASS_CHUNK_VERSION);

		for(ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->save(stream);

		stream.writeChunkEnd(SUBCLASS_CHUNK_ID);
	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::frameStart(NIIf timeSinceLastFrame)
	{
		for(ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->frameStart(timeSinceLastFrame);

	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::frameEnd(NIIf timeElapsed)
	{
		for(ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->frameEnd(timeElapsed);
	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::notifyCamera(Camera* cam)
	{
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->notifyCamera(cam);
	}
	//---------------------------------------------------------------------
	bool SimplePageContentCollection::prepare(FileStreamSerializer& stream)
	{
		if (!stream.readChunkBegin(SUBCLASS_CHUNK_ID, SUBCLASS_CHUNK_VERSION, "SimplePageContentCollection"))
			return false;

		bool ret = true;
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			ret = (*i)->prepare(stream) && ret;


		stream.readChunkEnd(SUBCLASS_CHUNK_ID);

		return ret;

	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::load()
	{
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->load();

	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::unload()
	{
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->unload();

	}
	//---------------------------------------------------------------------
	void SimplePageContentCollection::unprepare()
	{
		for (ContentList::iterator i = mContentList.begin(); i != mContentList.end(); ++i)
			(*i)->unprepare();
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	String SimplePageContentCollectionFactory::FACTORY_NAME = "Simple";
	//---------------------------------------------------------------------



}

