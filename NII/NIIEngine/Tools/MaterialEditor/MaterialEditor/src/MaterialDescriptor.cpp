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
#include "MaterialDescriptor.h"

#include "MaterialController.h"

MaterialDescriptor::MaterialDescriptor()
: mName(NULL), mMaterialController(NULL)
{
}

MaterialDescriptor::MaterialDescriptor(const String& name)
: mName(name), mMaterialController(NULL)
{
}

MaterialDescriptor::~MaterialDescriptor()
{
}
	
const String& MaterialDescriptor::getName() const
{
	return mName;
}

const String& MaterialDescriptor::getScript() const
{
	return mScript;
}

MaterialController* MaterialDescriptor::getMaterialController()
{
	return mMaterialController;
}

void MaterialDescriptor::OnRootInitialized(EventArgs& args)
{
	if(mMaterialController != NULL)
	{	
		delete mMaterialController;
		mMaterialController = NULL;
	}
}

void MaterialDescriptor::OnRootShutdown(EventArgs& args)
{
}