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
#include "MaterialEventArgs.h"

#include "MaterialController.h"
#include "TechniqueController.h"

MaterialEventArgs::MaterialEventArgs(MaterialController* mc)
: mMaterialController(mc), mTechniqueController(NULL)
{
}

MaterialEventArgs::MaterialEventArgs(MaterialController* mc, TechniqueController* tc)
: mMaterialController(mc), mTechniqueController(tc)
{
}

MaterialController* MaterialEventArgs::getMaterialController() const
{
	return mMaterialController;
}

TechniqueController* MaterialEventArgs::getTechniqueController() const
{
	return mTechniqueController;
}
