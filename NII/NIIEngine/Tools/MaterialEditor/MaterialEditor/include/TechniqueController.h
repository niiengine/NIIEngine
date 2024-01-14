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
#ifndef _TECHNIQUECONTROLLER_H_
#define _TECHNIQUECONTROLLER_H_

#include <list>

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgrePrerequisites.h"

#include "EventContainer.h"

class MaterialController;
class PassController;

using namespace Ogre;

typedef std::list<PassController*> PassControllerList;

class TechniqueController : public EventContainer
{
public:
	enum TechniqueEvent
	{
		NameChanged,
		SchemeChanged,
		LodIndexChanged,
		PassAdded,
		PassRemoved
	};

	TechniqueController(ShaderFusion* technique);
	TechniqueController(MaterialController* parent, ShaderFusion* technique);
	virtual ~TechniqueController();

	MaterialController* getParentController() const;
	const ShaderFusion * getShaderFusion() const;
	const PassControllerList * getPassControllers() const;

	PassController * create(void);
	PassController * create(const String & name);
	void removeAllPasses(void);
	void removePass(unsigned short index);

	void setName(const String & name);
	void setSchemeID(SchemeID schemeName);
	void setLodIndex(unsigned short index);

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

	MaterialController* mParentController;
	ShaderFusion* mTechnique;
	PassControllerList mPassControllers;
};
#endif // _TECHNIQUECONTROLLER_H_
