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
#include "PlayPen.h"
#include "PlayPenSamples.h"
#include "OgreResourceLoadManager.h"

//---------------------------------------------------------------------
PlayPenPlugin::PlayPenPlugin() :
    SamplePlugin("PlayPenPlugin")
{
	// add the samples
	addSample(new PlayPen_testManualBlend());
	addSample(new PlayPen_testProjectSphere());
	addSample(new PlayPen_testCameraSetDirection());
	addSample(new PlayPen_testManualLOD());
	addSample(new PlayPen_testManualLODFromFile());
	addSample(new PlayPen_testFullScreenSwitch());
	addSample(new PlayPen_testMorphAnimationWithNormals());
	addSample(new PlayPen_testMorphAnimationWithoutNormals());
	addSample(new PlayPen_testPoseAnimationWithNormals());
	addSample(new PlayPen_testPoseAnimationWithoutNormals());
}
//---------------------------------------------------------------------
PlayPenPlugin::~PlayPenPlugin()
{
	for (SampleSet::iterator i = mSamples.begin(); i != mSamples.end(); ++i)
		delete *i;
	mSamples.clear();
}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
GroupID PlayPenBase::TRANSIENT_RESOURCE_GROUP = "PlayPenTransient";
PlayPenBase::PlayPenBase()
{
	mInfo["Thumbnail"] = "thumb_playpen.png";
	mInfo["Category"] = "Tests";

	ResourceSchemeManager & rgm = ResourceSchemeManager::getSingleton();
	if(!rgm.isExist(TRANSIENT_RESOURCE_GROUP))
		rgm.create(TRANSIENT_RESOURCE_GROUP);
}
//---------------------------------------------------------------------
void PlayPenBase::unloadResources()
{
	ResourceSchemeManager::getSingleton().clearResourceGroup(TRANSIENT_RESOURCE_GROUP);

	SdkSample::unloadResources();
}
//---------------------------------------------------------------------
bool PlayPenBase::onPreRender(const RenderFrameArgs * evt)
{
	for(AnimationStateList::iterator animi = mAnimStateList.begin(); animi != mAnimStateList.end(); ++animi)
	{
		(*animi)->addTime(evt->mCurrent);
	}

	return true;
}

#ifndef OGRE_STATIC_LIB

PlayPenPlugin * playPenPlugin = 0;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	playPenPlugin = N_new PlayPenPlugin();
	Root::getSingleton().installPlugin(playPenPlugin);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(playPenPlugin);
	N_delete playPenPlugin;
}
#endif