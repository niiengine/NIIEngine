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
#ifndef _MATERIALDESCRIPTOR_H_
#define _MATERIALDESCRIPTOR_H_

#include "OgreString.h"

class EventArgs;
class MaterialController;
class MaterialEventArgs;
class PassEventArgs;
class TechniqueEventArgs;

using Ogre::MaterialPtr;
using Ogre::String;

class MaterialDescriptor
{
public:
	MaterialDescriptor();
	MaterialDescriptor(const String& name);
	virtual ~MaterialDescriptor();
	
	const String& getName() const;
	const String& getScript() const;
	MaterialController* getMaterialController();
	MaterialPtr& getMaterial();
	void setMaterial(MaterialPtr& mp);
		
	void OnRootInitialized(EventArgs& args);
	void OnRootShutdown(EventArgs& args);
	
protected:
	String mName;
	String mScript;
	MaterialController* mMaterialController;
};

#endif _MATERIALDESCRIPTOR_H_