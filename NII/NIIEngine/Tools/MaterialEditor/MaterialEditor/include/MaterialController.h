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
#ifndef _MATERIALCONTROLLER_H_
#define _MATERIALCONTROLLER_H_

#include <list>

#include <boost/signal.hpp>

#include "OgreMaterial.h"

#include "EventContainer.h"

class TechniqueController;

using namespace Ogre;

typedef std::list<TechniqueController *> TechniqueControllerList;

class MaterialController : public EventContainer
{
public:
	enum MaterialEvent
	{
		NameChanged,
		TechniqueAdded,
		TechniqueRemoved
	};

	MaterialController();
	MaterialController(MaterialPtr material);
	virtual ~MaterialController();

	MaterialPtr getMaterial() const;
	void setMaterial(MaterialPtr mp);

	TechniqueController* getTechniqueController(const String& name);
	const TechniqueControllerList* getTechniqueControllers() const;

	void setName(const String & name);
	void setReceiveShadow(bool b);
	void setAlphaShadowCast(bool b);
	TechniqueController* createTechnique(void);
	TechniqueController* createTechnique(const String& name);
	void removeTechnique(unsigned short index);
	void removeAllTechniques(void);

	void setColour(const ShaderChColour & c);
    
	void setColourWrite(Nmark mark);
	void setCullingMode(CullingMode mode);

	void setDepthCheck(bool b);
	void setDepthWrite(bool b);
	
	void setFog(const ShaderChFog & f);
    
	void setLight(bool b);
	void setSysCullingMode(SysCullingMode mode);
	void setPoint(const ShaderChPoint & p);
	void setShadeMode(ShadeMode mode);
protected:
	void registerEvents();

	MaterialPtr mMaterialPtr;

	TechniqueControllerList mTechniqueControllers;
};

#endif // _MATERIALCONTROLLER_H_