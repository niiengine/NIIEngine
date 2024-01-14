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
OgrePCZPlugin.cpp  -  Portal Connected Zone Scene Manager Plugin class
-----------------------------------------------------------------------------
begin                : Mon Feb 19 2007
author               : Eric Cha
email                : ericc@xenopi.com
Code Style Update	 :
-----------------------------------------------------------------------------
*/

#include "OgrePCZPlugin.h"
#include <OgreRoot.h>

namespace Ogre 
{
	const String sPluginName = "Portal Connected Zone Scene Manager";
	//---------------------------------------------------------------------
	PCZPlugin::PCZPlugin()
		:mPCZSMFactory(0)
	{
	}
	//---------------------------------------------------------------------
	const String& PCZPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void PCZPlugin::install()
	{
		// Create objects
		mPCZSMFactory = N_new PCZSceneManagerFactory();
		mPCZoneFactoryManager = N_new PCZoneFactoryManager();
		mPCZLightFactory = N_new PCZLightFactory();
		mPortalFactory = N_new PortalFactory();
		mAntiPortalFactory = N_new AntiPortalFactory();
	}
	//---------------------------------------------------------------------
	void PCZPlugin::init()
	{
		// Register
		Root::getSingleton().addSceneManagerFactory(mPCZSMFactory);
		Root::getSingleton().addFactory(mPCZLightFactory);
		Root::getSingleton().addFactory(mPortalFactory);
		Root::getSingleton().addFactory(mAntiPortalFactory);

		// set type flags to static member variable for fast access.
		PortalFactory::FACTORY_TYPE_FLAG = mPortalFactory->getTypeMark();
		AntiPortalFactory::FACTORY_TYPE_FLAG = mAntiPortalFactory->getTypeMark();
	}
	//---------------------------------------------------------------------
	void PCZPlugin::shutdown()
	{
		// Unregister
		Root::getSingleton().removeSceneManagerFactory(mPCZSMFactory);
		Root::getSingleton().removeFactory(mPCZLightFactory);
		Root::getSingleton().removeFactory(mPortalFactory);
		Root::getSingleton().removeFactory(mAntiPortalFactory);
	}
	//---------------------------------------------------------------------
	void PCZPlugin::uninstall()
	{
		// destroy 
		N_delete mPCZSMFactory;
		mPCZSMFactory = 0;
		N_delete mPCZoneFactoryManager;
		mPCZoneFactoryManager = 0;
		N_delete mPCZLightFactory;
		mPCZLightFactory = 0;
		N_delete mPortalFactory;
		mPortalFactory = 0;
		N_delete mAntiPortalFactory;
		mAntiPortalFactory = 0;
	}


}
