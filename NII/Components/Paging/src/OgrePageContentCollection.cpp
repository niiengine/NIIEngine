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
#include "OgrePageContentCollection.h"
#include "OgrePageContentCollectionFactory.h"
#include "OgreStreamSerialiser.h"
#include "OgrePageContent.h"
#include "OgrePage.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	const uint32 PageContentCollection::CHUNK_ID = N_MAGIC4('P','G','C','C);
	const uint16 PageContentCollection::CHUNK_VERSION = 1;
	//---------------------------------------------------------------------
	PageContentCollection::PageContentCollection(PageContentCollectionFactory * creator) : 
        mCreator(creator),
        mParent(0)
	{
	}
	//---------------------------------------------------------------------
	PageContentCollection::~PageContentCollection()
	{
		// don't call destroy(), we're not the final subclass
	}
	//---------------------------------------------------------------------
	PageManager* PageContentCollection::getManager() const
	{
		return mParent->getManager();
	}
	//---------------------------------------------------------------------
	const String& PageContentCollection::getType() const
	{
		return mCreator->getName();
	}
	//---------------------------------------------------------------------
	void PageContentCollection::_notify(Page* parent)
	{
		mParent = parent;
	}
	//---------------------------------------------------------------------
	SpaceManager * PageContentCollection::getSceneManager() const
	{
		return mParent->getSceneManager();
	}
}