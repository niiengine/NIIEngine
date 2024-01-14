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
#include "Project.h"

#include "OgreEntity.h"
#include "OgreLight.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreSceneManager.h"

#include "MaterialController.h"
#include "ProjectEventArgs.h"

using Ogre::GeometrySpace;
using Ogre::Light;
using Ogre::MaterialManager;

Project::Project() : mActiveMaterial(NULL)
{
	registerEvents();
}

Project::Project(const String& name) : mActiveMaterial(NULL), mName(name)
{
	registerEvents();
}

Project::~Project()
{
	MaterialControllerList::iterator it;
	for(it = mMaterialControllers.begin(); it != mMaterialControllers.end(); ++it)
	{
		delete *it;
	}

	mMaterialControllers.clear();
}

void Project::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(MaterialAdded);
	registerEvent(MaterialRemoved);
	registerEvent(ActiveMaterialChanged);
}

const String& Project::getName() const
{
	return mName;
}

void Project::setName(const String& name)
{
	mName = name;

	fireEvent(NameChanged, ProjectEventArgs(this));
}

void Project::addMaterial(MaterialPtr materialPtr)
{
	MaterialController* controller = new MaterialController(materialPtr);
	mMaterialControllers.push_back(controller);

	fireEvent(MaterialAdded, ProjectEventArgs(this, controller));
}

void Project::createMaterial(const String& name)
{
	// TODO: Projects should probably have their own resource groups instead of using the default
	MaterialPtr materialPtr = (MaterialPtr)MaterialManager::getSingletonPtr()->create(name, GroupDefault);

	MaterialController* controller = new MaterialController(materialPtr);
	mMaterialControllers.push_back(controller);

	fireEvent(MaterialAdded, ProjectEventArgs(this, controller));
}

void Project::removeMaterial(MaterialController* controller)
{
	MaterialControllerList::iterator it;
	for(it = mMaterialControllers.begin(); it != mMaterialControllers.end(); ++it)
	{
		if(*it == controller)
		{
			mMaterialControllers.erase(it);
			break;
		}
	}

	// Consider: Should this be fired BEFORE the actual removal?
	fireEvent(MaterialRemoved, ProjectEventArgs(this, controller));
}

void Project::removeMaterial(Material* material)
{
	removeMaterial(getMaterialController(material->getName()));
}

void Project::removeMaterial(const String& name)
{
	removeMaterial(getMaterialController(name));
}

MaterialController* Project::getActiveMaterial() const
{
	return mActiveMaterial;
}

void Project::setActiveMaterial(MaterialController* controller)
{
	assert(controller);

	if(controller == mActiveMaterial) return;

	mActiveMaterial = controller;

	fireEvent(ActiveMaterialChanged, ProjectEventArgs(this));
}

void Project::setActiveMaterial(Material* material)
{
	setActiveMaterial(getMaterialController(material->getName()));
}

void Project::setActiveMaterial(const String& name)
{
	setActiveMaterial(getMaterialController(name));
}

MaterialController* Project::getMaterialController(const String& name)
{
	MaterialController* mc;
	MaterialControllerList::iterator it;
	for(it = mMaterialControllers.begin(); it != mMaterialControllers.end(); ++it)
	{
		mc = (*it);
		if(mc->getMaterial()->getName() == name) return mc;
	}

	return NULL;
}

const MaterialControllerList* Project::getMaterials() const
{
	return &mMaterialControllers;
}

void Project::open()
{
}

void Project::close()
{
}

void Project::generateScene(Ogre::SpaceManager* sceneManager)
{
	sceneManager->setAmbient(Colour(0.5, 0.5, 0.5));

	Light* light = sceneManager->createLight("MainLight");
	light->setPos(Vecotr3f(20, 80, 50));

	GeometrySpace* entity = sceneManager->createGeo("head", "ogrehead.mesh");
	entity->setMaterial(mActiveMaterial->getMaterial()->getName());

	static_cast<SpaceNode *>(sceneManager->getOriginNode()->create())->attachObject(entity);
}

void Project::subscribeTo(RootEventPlugin* plugin)
{
}

void Project::OnRootInitialized(EventArgs& args)
{
}

void Project::OnRootShutdown(EventArgs& args)
{
}
