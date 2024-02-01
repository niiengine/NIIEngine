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
Filename:    StaticPluginLoader.h
Description: Utility class to load plugins statically
-----------------------------------------------------------------------------
*/

#ifndef __StaticPluginLoader_H__
#define __StaticPluginLoader_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
// Static plugin headers
#ifdef OGRE_STATIC_CgProgramManager
#  include "OgreCgPlugin.h"
#endif
#ifdef OGRE_STATIC_OctreeSceneManager
#  include "OgreOctreePlugin.h"
#endif
#ifdef OGRE_STATIC_ParticleFX
#  include "OgreParticleFXPlugin.h"
#endif
#ifdef OGRE_STATIC_BSPSceneManager
#  include "OgreBspSceneManagerPlugin.h"
#endif
#ifdef OGRE_STATIC_GL
#  include "OgreGLPlugin.h"
#endif
#ifdef OGRE_STATIC_GLES
#  include "OgreGLESPlugin.h"
#endif
#ifdef OGRE_STATIC_Direct3D9
#  include "OgreD3D9Plugin.h"
#endif
#ifdef OGRE_STATIC_Direct3D11
#  include "OgreD3D11Plugin.h"
#endif
#ifdef OGRE_STATIC_PCZSceneManager
#  include "OgrePCZPlugin.h"
#endif
#ifdef OGRE_STATIC_OctreeZone
#  include "OgreOctreeZonePlugin.h"
#endif

namespace Ogre
{
	/** Utility class for loading some plugins statically.
	@remarks
		When loading plugins statically, you are limited to loading plugins 
		that are known about at compile time. You should define preprocessor
		symbols depending on which plugins you want to load - the symbol being
		OGRE_STATIC_<pluginname>, with pluginname being the usual name of the
		plugin DLL (no file extension, no debug suffix, and without the Plugin_ 
		or RenderSystem_ prefix.)
	*/
	class StaticPluginLoader
	{
	protected:
#ifdef OGRE_STATIC_CgProgramManager
		CgPlugin* mCgPlugin;
#endif
#ifdef OGRE_STATIC_OctreeSceneManager
		OctreePlugin* mOctreePlugin;
#endif
#ifdef OGRE_STATIC_ParticleFX
		ParticleFXPlugin* mParticleFXPlugin;
#endif
#ifdef OGRE_STATIC_BSPSceneManager
		BspSceneManagerPlugin* mBSPPlugin;
#endif
#ifdef OGRE_STATIC_PCZSceneManager
		PCZPlugin* mPCZPlugin;
#endif
#ifdef OGRE_STATIC_OctreeZone
		OctreeZonePlugin* mOctreeZonePlugin;
#endif
#ifdef OGRE_STATIC_GL
		GLPlugin* mGLPlugin;
#endif
#ifdef OGRE_STATIC_GLES
		GLESPlugin* mGLESPlugin;
#endif
#ifdef OGRE_STATIC_Direct3D9
		D3D9Plugin* mD3D9Plugin;
#endif
#ifdef OGRE_STATIC_Direct3D11
		D3D11Plugin* mD3D11Plugin;
#endif
	public:
		StaticPluginLoader() {}

		/** Load all the enabled plugins against the passed in root object. */
		void load()
		{
			Root& root  = Root::getSingleton();
#ifdef OGRE_STATIC_GL
			mGLPlugin = N_new GLPlugin();
			root.installPlugin(mGLPlugin);
#endif
#ifdef OGRE_STATIC_GLES
			mGLESPlugin = N_new GLESPlugin();
			root.installPlugin(mGLESPlugin);
#endif
#ifdef OGRE_STATIC_Direct3D9
			mD3D9Plugin = N_new D3D9Plugin();
			root.installPlugin(mD3D9Plugin);
#endif
#ifdef OGRE_STATIC_Direct3D11
			mD3D11Plugin = N_new D3D11Plugin();
			root.installPlugin(mD3D11Plugin);
#endif
#ifdef OGRE_STATIC_CgProgramManager
			mCgPlugin = N_new CgPlugin();
			root.installPlugin(mCgPlugin);
#endif
#ifdef OGRE_STATIC_OctreeSceneManager
			mOctreePlugin = N_new OctreePlugin();
			root.installPlugin(mOctreePlugin);
#endif
#ifdef OGRE_STATIC_ParticleFX
			mParticleFXPlugin = N_new ParticleFXPlugin();
			root.installPlugin(mParticleFXPlugin);
#endif
#ifdef OGRE_STATIC_BSPSceneManager
			mBSPPlugin = N_new BspSceneManagerPlugin();
			root.installPlugin(mBSPPlugin);
#endif
#ifdef OGRE_STATIC_PCZSceneManager
			mPCZPlugin = N_new PCZPlugin();
			root.installPlugin(mPCZPlugin);
#endif
#ifdef OGRE_STATIC_OctreeZone
			mOctreeZonePlugin = N_new OctreeZonePlugin();
			root.installPlugin(mOctreeZonePlugin);
#endif
		}

		void unload()
		{
			// don't unload plugins, since Root will have done that. Destroy here.
#ifdef OGRE_STATIC_OctreeZone
			N_delete mOctreeZonePlugin;
#endif
#ifdef OGRE_STATIC_PCZSceneManager
			N_delete mPCZPlugin;
#endif
#ifdef OGRE_STATIC_BSPSceneManager
			N_delete mBSPPlugin;
#endif
#ifdef OGRE_STATIC_ParticleFX
			N_delete mParticleFXPlugin;
#endif
#ifdef OGRE_STATIC_OctreeSceneManager
			N_delete mOctreePlugin;
#endif
#ifdef OGRE_STATIC_CgProgramManager
			N_delete mCgPlugin;
#endif
#ifdef OGRE_STATIC_Direct3D9
			N_delete mD3D9Plugin;
#endif
#ifdef OGRE_STATIC_Direct3D11
			N_delete mD3D11Plugin;
#endif
#ifdef OGRE_STATIC_GL
			N_delete mGLPlugin;
#endif
#ifdef OGRE_STATIC_GLES
			N_delete mGLESPlugin;
#endif

		}

	};

}

#endif