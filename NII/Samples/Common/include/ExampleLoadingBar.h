/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleLoadingBar.h
Description: Defines an example loading progress bar which you can use during
startup, level changes etc to display loading progress.
IMPORTANT: Note that this progress bar relies on you having the OgreCore.zip
package already added to a resource group called 'Bootstrap' - this provides
the basic resources required for the progress bar and will be loaded automatically.
*/
#include "OgreResourceLoadManager.h"
#include "OgreException.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreRenderWindow.h"

using namespace Ogre;

/** Defines an example loading progress bar which you can use during
	startup, level changes etc to display loading progress.
@remarks
	Basically you just need to create an instance of this class, call start()
	before loading and finish() afterwards. You may also need to stop areas of
	your scene rendering in between since this method will call
	ViewWindow::flush() to update the display of the bar - we advise using
	SpaceManager's 'special case render queues' for this, see
	SpaceManager::addExcludeRender for details.
@note
	This progress bar relies on you having the OgreCore.zip package already
	added to a resource group called 'Bootstrap' - this provides the basic
	resources required for the progress bar and will be loaded automatically.
*/
class ExampleLoadingBar : public ResourceGroupListener
{
public:
	ExampleLoadingBar() {}
	virtual ~ExampleLoadingBar(){}

	/** Show the loading bar and start listening.
	@param window The window to update
	@param numGroupsInit The number of groups you're going to be initialising
	@param numGroupsLoad The number of groups you're going to be loading
	@param initProportion The proportion of the progress which will be taken
		up by initialisation (ie script parsing etc). Defaults to 0.7 since
		script parsing can often take the majority of the time.
	*/
	virtual void start(ViewWindow* window,
		unsigned short numGroupsInit = 1,
		unsigned short numGroupsLoad = 1,
		NIIf initProportion = 0.70f)
	{
		mWindow = window;
		mNumGroupsInit = numGroupsInit;
		mNumGroupsLoad = numGroupsLoad;
		mInitProportion = initProportion;
		// We need to pre-init the 'Bootstrap' group so we can use
		// the basic contents in the loading screen
		ResourceSchemeManager::getSingleton().setup("Bootstrap");

		OverlayManager& omgr = OverlayManager::getSingleton();
		mLoadOverlay = (Overlay*)omgr.getByName("Core/LoadOverlay");
		if (!mLoadOverlay)
		{
			N_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
				"Cannot find loading overlay", "ExampleLoadingBar::start");
		}
		mLoadOverlay->show();

		// Save links to the bar and to the loading text, for updates as we go
		mLoadingBarElement = omgr.getGeo("Core/LoadPanel/Bar/Progress");
		mLoadingCommentElement = omgr.getGeo("Core/LoadPanel/Comment");
		mLoadingDescriptionElement = omgr.getGeo("Core/LoadPanel/Description");

		OverlayGeo* barContainer = omgr.getGeo("Core/LoadPanel/Bar");
		mProgressBarMaxSize = barContainer->getWidth();
		mLoadingBarElement->setWidth(0);

		// self is listener
		ResourceSchemeManager::getSingleton().addResourceGroupListener(this);
	}

	/** Hide the loading bar and stop listening.
	*/
	virtual void finish(void)
	{
		// hide loading screen
		mLoadOverlay->hide();

		// Unregister listener
		ResourceSchemeManager::getSingleton().removeResourceGroupListener(this);
	}

	// ResourceGroupListener callbacks
	void resourceGroupScriptingStarted(const String& groupName, size_t scriptCount)
	{
		assert(mNumGroupsInit > 0 && "You stated you were not going to init "
			"any groups, but you did! Divide by zero would follow...");
		// Lets assume script loading is 70%
		mProgressBarInc = mProgressBarMaxSize * mInitProportion / (NIIf)scriptCount;
		mProgressBarInc /= mNumGroupsInit;
		mLoadingDescriptionElement->setCaption("Parsing scripts...");
		mWindow->flush();
	}

	void scriptParseStarted(const String& scriptName, bool &skipThisScript)
	{
		mLoadingCommentElement->setCaption(scriptName);
		mWindow->flush();
	}

	void scriptParseEnded(const String& scriptName, bool skipped)
	{
		mLoadingBarElement->setWidth(
			mLoadingBarElement->getWidth() + mProgressBarInc);
		mWindow->flush();
	}

	void resourceGroupScriptingEnded(const String & groupName)
	{

	}

	void resourceGroupLoadStarted(const String & groupName, size_t resourceCount)
	{
		assert(mNumGroupsLoad > 0 && "You stated you were not going to load "
			"any groups, but you did! Divide by zero would follow...");
		mProgressBarInc = mProgressBarMaxSize * (1-mInitProportion) /
			(NIIf)resourceCount;
		mProgressBarInc /= mNumGroupsLoad;
		mLoadingDescriptionElement->setCaption("Loading resources...");
		mWindow->flush();
	}

	void resourceLoadStarted(const ResourcePtr & resource)
	{
		mLoadingCommentElement->setCaption(resource->getName());
		mWindow->flush();
	}

	void resourceLoadEnded(void)
	{
	}

	void worldGeometryStageStarted(const String & description)
	{
		mLoadingCommentElement->setCaption(description);
		mWindow->flush();
	}

	void worldGeometryStageEnded(void)
	{
		mLoadingBarElement->setWidth(
			mLoadingBarElement->getWidth() + mProgressBarInc);
		mWindow->flush();
	}

	void resourceGroupLoadEnded(const String & groupName)
	{
	}

	Ogre::DataStream * resourceLoading(const String &name, const String &group, Resource *resource)
	{
		return 0;
	}

	bool resourceCollision(ResourcePtr &resource, ResourceManager *resourceManager)
	{
		return false;
	}
protected:
	ViewWindow * mWindow;
	Overlay * mLoadOverlay;
	NIIf mInitProportion;
	unsigned short mNumGroupsInit;
	unsigned short mNumGroupsLoad;
	NIIf mProgressBarMaxSize;
	NIIf mProgressBarScriptSize;
	NIIf mProgressBarInc;
	OverlayGeo* mLoadingBarElement;
	OverlayGeo* mLoadingDescriptionElement;
	OverlayGeo* mLoadingCommentElement;
};