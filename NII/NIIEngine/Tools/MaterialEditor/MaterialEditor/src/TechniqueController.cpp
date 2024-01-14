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
#include "TechniqueController.h"

#include "OgreTechnique.h"

#include "MaterialController.h"
#include "PassController.h"
#include "TechniqueEventArgs.h"

TechniqueController::TechniqueController(ShaderFusion* technique)
: mParentController(NULL), mTechnique(technique)
{
	registerEvents();
}

TechniqueController::TechniqueController(MaterialController* parent, ShaderFusion* technique)
: mParentController(parent), mTechnique(technique)
{
	registerEvents();
}

TechniqueController::~TechniqueController()
{
}

void TechniqueController::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(SchemeChanged);
	registerEvent(LodIndexChanged);
	registerEvent(PassAdded);
	registerEvent(PassRemoved);
}

const ShaderFusion * TechniqueController::getShaderFusion() const
{
	return  mTechnique;
}

MaterialController* TechniqueController::getParentController() const
{
	return mParentController;
}

const PassControllerList* TechniqueController::getPassControllers() const
{
	return &mPassControllers;
}

PassController * TechniqueController::create(void)
{
	ShaderCh * ch = mTechnique->create();

	// Create controller
	PassController* pc = new PassController(this, ch);
	mPassControllers.push_back(pc);

	fireEvent(PassAdded, TechniqueEventArgs(this, pc));

	return pc;
}

PassController* TechniqueController::create(const String& name)
{
	ShaderCh* pass = mTechnique->create();
	pass->setName(name);

	// Create controller
	PassController* pc = new PassController(this, pass);
	mPassControllers.push_back(pc);

	fireEvent(PassAdded, TechniqueEventArgs(this, pc));

	return pc;
}

void TechniqueController::removeAllPasses(void)
{
}

void TechniqueController::removePass(unsigned short index)
{
}

void TechniqueController::setName(const String& name)
{
	mTechnique->setName(name);

	fireEvent(NameChanged, TechniqueEventArgs(this));
}

void TechniqueController::setSchemeID(SchemeID schemeName)
{
	mTechnique->setSchemeID(schemeName);

	fireEvent(SchemeChanged, TechniqueEventArgs(this));
}

void TechniqueController::setLodIndex(unsigned short index)
{
	mTechnique->setLodIndex(index);

	fireEvent(LodIndexChanged, TechniqueEventArgs(this));
}

void TechniqueController::setColour(const ShaderChColour & c)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setColour(c);
}

void TechniqueController::setColourWrite(Nmark mark)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setColourWrite(mark);
}

void TechniqueController::setCullingMode(CullingMode mode)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setCullingMode(mode);
}

void TechniqueController::setDepthCheck(bool b)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setDepthCheck(b);
}

void TechniqueController::setDepthWrite(bool b)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setDepthWrite(b);
}

void TechniqueController::setFog(const ShaderChFog & f)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setFog(f);
}

void TechniqueController::setLight(bool b)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setLight(b);
}

void TechniqueController::setSysCullingMode(SysCullingMode mode)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setSysCullingMode(mode);
}

void TechniqueController::setPoint(const ShaderChPoint & p)
{
	PassControllerList::iterator i, iend = mPassControllers.end();
	for(i = mPassControllers.begin(); i != iend; ++i)
		(*i)->setPoint(p);
}

void TechniqueController::setShadeMode(ShadeMode mode)
{
	PassControllerList::iterator it;
	for(it = mPassControllers.begin(); it != mPassControllers.end(); ++it)
		(*it)->setShadeMode(mode);
}
