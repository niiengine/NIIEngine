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

#include "OgreParticleFXPlugin.h"
#include "OgreRoot.h"
#include "OgreParticleSpaceManager.h"

#include "OgrePointEmitterFactory.h"
#include "OgreBoxEmitterFactory.h"
#include "OgreEllipsoidEmitterFactory.h"
#include "OgreHollowEllipsoidEmitterFactory.h"
#include "OgreRingEmitterFactory.h"
#include "OgreCylinderEmitterFactory.h"
#include "OgreLinearForceAffectorFactory.h"
#include "OgreColourFaderAffectorFactory.h"
#include "OgreColourFaderAffectorFactory2.h"
#include "OgreColourImageAffectorFactory.h"
#include "OgreColourInterpolatorAffectorFactory.h"
#include "OgreScaleAffectorFactory.h"
#include "OgreRotationAffectorFactory.h"
#include "OgreDirectionRandomiserAffectorFactory.h"
#include "OgreDeflectorPlaneAffectorFactory.h"

namespace Ogre 
{
	const String sPluginName = "ParticleFX";
	//---------------------------------------------------------------------
	ParticleFXPlugin::ParticleFXPlugin()
	{

	}
	//---------------------------------------------------------------------
	const String & ParticleFXPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void ParticleFXPlugin::install()
	{
		// -- Create all new particle emitter factories --
		SpreadParticleFactory* pEmitFact;

		// PointEmitter
		pEmitFact = N_new PointEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// BoxEmitter
		pEmitFact = N_new BoxEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// EllipsoidEmitter
		pEmitFact = N_new EllipsoidEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// CylinderEmitter
		pEmitFact = N_new CylinderEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// RingEmitter
		pEmitFact = N_new RingEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// HollowEllipsoidEmitter
		pEmitFact = N_new HollowEllipsoidEmitterFactory();
		ParticleSpaceManager::getSingleton().addEmitterFactory(pEmitFact);
		mEmitterFactories.push_back(pEmitFact);

		// -- Create all new particle affector factories --
		ParticleEffectFactory* pAffFact;

		// LinearForceAffector
		pAffFact = N_new LinearForceAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// ColourFaderAffector
		pAffFact = N_new ColourFaderAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// ColourFaderAffector2
		pAffFact = N_new ColourFaderAffectorFactory2();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// ColourImageAffector
		pAffFact = N_new ColourImageAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// ColourInterpolatorAffector
		pAffFact = N_new ColourInterpolatorAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// ScaleAffector
		pAffFact = N_new ScaleAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// RotationAffector
		pAffFact = N_new RotationAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);


		// DirectionRandomiserAffector
		pAffFact = N_new DirectionRandomiserAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);

		// DeflectorPlaneAffector
		pAffFact = N_new DeflectorPlaneAffectorFactory();
		ParticleSpaceManager::getSingleton().addAffectorFactory(pAffFact);
		mAffectorFactories.push_back(pAffFact);
	}
	//---------------------------------------------------------------------
	void ParticleFXPlugin::init()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void ParticleFXPlugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void ParticleFXPlugin::uninstall()
	{
		// destroy 
		vector<SpreadParticleFactory*>::type::iterator ei;
		vector<ParticleEffectFactory*>::type::iterator ai;

		for (ei = mEmitterFactories.begin(); ei != mEmitterFactories.end(); ++ei)
		{
			N_delete (*ei);
		}

		for (ai = mAffectorFactories.begin(); ai != mAffectorFactories.end(); ++ai)
		{
			N_delete (*ai);
		}
	}
}