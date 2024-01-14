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
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <list>

#include <boost/signal.hpp>

#include "OgreMaterial.h"
#include "OgreString.h"

#include "EventContainer.h"

namespace Ogre
{
	class SpaceManager;
}

class MaterialController;
class Project;
class RootEventPlugin;

using Ogre::Material;
using Ogre::MaterialPtr;
using Ogre::String;

typedef std::list<MaterialController*> MaterialControllerList;

class Project : public EventContainer
{
public:
	enum ProjectEvent
	{
		NameChanged,
		MaterialAdded,
		MaterialRemoved,
		ActiveMaterialChanged
	};

	Project();
	Project(const String& name);
	virtual ~Project();

	void registerEvents();

	const String& getName() const;
	void setName(const String& name);

	void addMaterial(MaterialPtr materialPtr);
	void createMaterial(const String& name);

	void removeMaterial(MaterialController* controller);
	void removeMaterial(Material* material);
	void removeMaterial(const String& name);
	
	MaterialController* getActiveMaterial() const;
	void setActiveMaterial(MaterialController* controller);
	void setActiveMaterial(Material* material);
	void setActiveMaterial(const String& name);
	
	MaterialController* getMaterialController(const String& name);
	
	const MaterialControllerList* getMaterials() const;

	void open();
	void close();

	bool isOpen();
	bool isClosed();

	void generateScene(Ogre::SpaceManager* sceneManager);
	
	void OnRootInitialized(EventArgs& args);
	void OnRootShutdown(EventArgs& args);

protected:
	String mName;
	bool mOpen;
	MaterialController* mActiveMaterial;
	MaterialControllerList mMaterialControllers;
	
	void subscribeTo(RootEventPlugin* plugin);
};

#endif // _PROJECT_H_
