#include "SamplePlugin.h"
#include "SkeletalAnimation.h"

using namespace Ogre;
using namespace OgreBites;

#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_SkeletalAnimation;
	sp = N_new SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp); 
	N_delete sp;
	delete s;
}
#endif
