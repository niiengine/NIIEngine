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
#include "OgrePageContent.h"
#include "OgrePageContentFactory.h"
#include "OgrePageContentCollection.h"

namespace Ogre
{
	//---------------------------------------------------------------------
	PageContent::PageContent(PageContentFactory* creator) : 
        mCreator(creator),
        mParent(0)
	{

	}
	//---------------------------------------------------------------------
	PageContent::~PageContent()
	{
		// don't call destroy(), we're not the final subclass
	}
	//---------------------------------------------------------------------
	PageManager * PageContent::getManager() const
	{
		return mParent->getManager();
	}
	//---------------------------------------------------------------------
	SpaceManager * PageContent::getSceneManager() const
	{
		return mParent->getSceneManager();
	}
	//---------------------------------------------------------------------
	const String & PageContent::getType() const
	{
		return mCreator->getName();
	}
	//---------------------------------------------------------------------
	void PageContent::_notify(PageContentCollection* parent)
	{
		mParent = parent;
	}

}

