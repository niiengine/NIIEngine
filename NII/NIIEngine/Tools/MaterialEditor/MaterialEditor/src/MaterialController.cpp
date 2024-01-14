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
#include "MaterialController.h"

#include "OgreColourValue.h"
#include "OgreTechnique.h"

#include "MaterialEventArgs.h"
#include "TechniqueController.h"

MaterialController::MaterialController()
: mMaterialPtr(NULL)
{
	registerEvents();
}

MaterialController::MaterialController(MaterialPtr materialPtr)
: mMaterialPtr(materialPtr)
{
	registerEvents();
}

MaterialController::~MaterialController()
{
}

void MaterialController::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(TechniqueAdded);
	registerEvent(TechniqueRemoved);
}

MaterialPtr MaterialController::getMaterial() const
{
	return mMaterialPtr;
}

void MaterialController::setMaterial(MaterialPtr mp)
{
	mMaterialPtr = mp;
}

TechniqueController * MaterialController::getTechniqueController(const String & name)
{
	TechniqueController * tc;
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
	{
		tc = (*i);
		if(tc->getShaderFusion()->getName() == name) 
            return tc;
	}

	return NULL;
}

const TechniqueControllerList * MaterialController::getTechniqueControllers() const
{
	return &mTechniqueControllers;
}

void MaterialController::setName(const String & name)
{
	// ???
}

void MaterialController::setReceiveShadow(bool b)
{
	mMaterialPtr->setReceiveShadow(b);
}

void MaterialController::setAlphaShadowCast(bool b)
{
	mMaterialPtr->setAlphaShadowCast(b);
}

TechniqueController * MaterialController::createTechnique(void)
{
	ShaderFusion * t;
    mMaterialPtr->create(t);

	// Create controller
	TechniqueController* tc = new TechniqueController(t);
	mTechniqueControllers.push_back(tc);

	fireEvent(TechniqueAdded, MaterialEventArgs(this, tc));

	return tc;
}

TechniqueController * MaterialController::createTechnique(const String & name)
{
	ShaderFusion * t;
    mMaterialPtr->create(t);
	t->setName(name);

	// Create controller
	TechniqueController * tc = new TechniqueController(t);
	mTechniqueControllers.push_back(tc);

	fireEvent(TechniqueAdded, MaterialEventArgs(this, tc));

	return tc;
}

void MaterialController::removeTechnique(unsigned short index)
{
	mMaterialPtr->destroy(index);

	fireEvent(TechniqueRemoved, MaterialEventArgs(this));
}

void MaterialController::removeAllTechniques(void)
{
	mMaterialPtr->destroyAll();

	fireEvent(TechniqueRemoved, MaterialEventArgs(this));
}

void MaterialController::setColour(const ShaderChColour & c)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setColour(c);
}

void MaterialController::setColourWrite(Nmark mark)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setColourWrite(mark);
}

void MaterialController::setCullingMode(CullingMode mode)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setCullingMode(mode);
}

void MaterialController::setDepthCheck(bool b)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setDepthCheck(b);
}

void MaterialController::setDepthWrite(bool b)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setDepthWrite(b);
}

void MaterialController::setFog(const ShaderChFog & f)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setFog(f);
}

void MaterialController::setLight(bool b)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != mTechniqueControllers.end(); ++i)
		(*i)->setLight(b);
}

void MaterialController::setSysCullingMode(SysCullingMode mode)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setSysCullingMode(mode);
}

void MaterialController::setPoint(const ShaderChPoint & p)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
    {
		(*i)->setPoint(p);
    }
}

void MaterialController::setShadeMode(ShadeMode mode)
{
	TechniqueControllerList::iterator i, iend = mTechniqueControllers.end();
	for(i = mTechniqueControllers.begin(); i != iend; ++i)
		(*i)->setShadeMode(mode);
}
