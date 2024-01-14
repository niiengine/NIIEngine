#ifndef _PASSCONTROLLER_H_
#define _PASSCONTROLLER_H_

#include <list>

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgreLight.h"
#include "OgrePrerequisites.h"

#include "EventContainer.h"

class TechniqueController;

using namespace Ogre;

class PassController : public EventContainer
{
public:
	enum PassEvent
	{
		NameChanged,
		AmbientChanged,
		DiffuseChanged,
		SpecularChanged,
		ShininessChanged,
		SelfIllumChanged,
		VertexColourTrackingChanged,
		PointSizeChanged,
		PointSpritesChanged,
		PointAttenuationChanged,
		PointMinSizeChanged,
		PointMaxSizeChanged,
		SceneBlendingTypeChanged,
		SceneBlendSrcFactorChanged,
		SceneBlendDestFactorChanged,
		DepthCheckChanged,
		DepthWriteChanged,
		DepthFunctionChanged,
		ColourWriteChanged,
		CullingModeChanged,
		ManualCullingModeChanged,
		LightingChanged,
		MaxLightsChanged,
		StartLightChanged,
		ShadingModeChanged,
		PolygonModeChanged,
		FogChanged,
		DepthBiasChanged,
		AlphaRejectionChanged,
		IteratePerLightChanged,
		LightCountPerIterationChanged,
	};

	PassController(ShaderCh* pass);
	PassController(TechniqueController* parent, ShaderCh* pass);
	virtual ~PassController();

	TechniqueController * getParentController() const;
	ShaderCh * getPass() const;

	void registerEvents();

	void setName(const String & name);
	void setColour(const ShaderChColour & c);

	void setColourTracking(Nmark tracking);
	void setPoint(const ShaderChPoint & p);
	void setPointSpritesEnabled(bool enabled);
	void setPointAttenuation(bool enabled, NIIf constant=0.0f, NIIf linear=1.0f, NIIf quadratic=0.0f);
	void setPointMinSize(NIIf min);
	void setPointMaxSize(NIIf max);
	void setDepthCheck(bool b);
	void setDepthWrite(bool b);
	void setColourWrite(Nmark mark);
	void setCullingMode(CullingMode mode);
	void setSysCullingMode(SysCullingMode mode);
	void setLight(bool b);
	void setLightMax(NCount count);
	void setLightBegin(unsigned short startLight);
	void setShadeMode(ShadeMode mode);
	void setPolygonMode(PolygonMode mode);
	void setFog(const ShaderChFog & f);
	void setAlphaRejectSettings(CmpMode func, Nui8 value);
	void setAlphaReject(CmpMode func);
	void setAlphaRejectValue(Nui8 val);
	void setIteratePerLight(bool enabled, LightType lightType=LT_POINT);
	void setLightRenderLimit(unsigned short c);
	void setVertexProgram(const String & name, GpuProgramParam * params);
    void setFragmentProgram(const String & name, GpuProgramParam * params);
	void setShadowCasterVertexProgram(const String & name, GpuProgramParam * params);
	void setShadowReceiverVertexProgram(const String & name, GpuProgramParam * params);
	void setShadowReceiverFragmentProgram(const String & name, GpuProgramParam * params);
protected:
	ShaderCh * mPass;
	TechniqueController * mParentController;
};

#endif // _PASSCONTROLLER_H_