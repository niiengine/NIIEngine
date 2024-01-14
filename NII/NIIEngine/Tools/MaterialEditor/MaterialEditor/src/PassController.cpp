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
#include "PassController.h"

#include "OgrePass.h"

#include "PassEventArgs.h"
#include "TechniqueController.h"

PassController::PassController(ShaderCh * pass) :
    mParentController(NULL),
    mPass(pass)
{
	registerEvents();
}

PassController::PassController(TechniqueController * parent, ShaderCh * pass) :
    mParentController(parent),
    mPass(pass)
{
	registerEvents();
}

PassController::~PassController()
{
}

void PassController::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(AmbientChanged);
	registerEvent(DiffuseChanged);
	registerEvent(SpecularChanged);
	registerEvent(ShininessChanged);
	registerEvent(SelfIllumChanged);
	registerEvent(VertexColourTrackingChanged);
	registerEvent(PointSizeChanged);
	registerEvent(PointSpritesChanged);
	registerEvent(PointAttenuationChanged);
	registerEvent(PointMinSizeChanged);
	registerEvent(PointMaxSizeChanged);
	registerEvent(SceneBlendingTypeChanged);
	registerEvent(SceneBlendSrcFactorChanged);
	registerEvent(SceneBlendDestFactorChanged);
	registerEvent(DepthCheckChanged);
	registerEvent(DepthWriteChanged);
	registerEvent(DepthFunctionChanged);
	registerEvent(ColourWriteChanged);
	registerEvent(CullingModeChanged);
	registerEvent(ManualCullingModeChanged);
	registerEvent(LightingChanged);
	registerEvent(MaxLightsChanged);
	registerEvent(StartLightChanged);
	registerEvent(ShadingModeChanged);
	registerEvent(PolygonModeChanged);
	registerEvent(FogChanged);
	registerEvent(DepthBiasChanged);
	registerEvent(AlphaRejectionChanged);
	registerEvent(IteratePerLightChanged);
	registerEvent(LightCountPerIterationChanged);
}


TechniqueController* PassController::getParentController() const
{
	return mParentController;
}

ShaderCh* PassController::getPass() const
{
	return mPass;
}

void PassController::setName(const String & name)
{
	mPass->setName(name);

	fireEvent(NameChanged, PassEventArgs(this));
}

void PassController::setColour(const ShaderChColour & c)
{
	mPass->setColour(c);

	fireEvent(AmbientChanged, PassEventArgs(this));
}

void PassController::setColourTracking(Nmark tracking)
{
	mPass->setColour(ShaderCh::SOT_Set)->setColourTracking(tracking);

	fireEvent(VertexColourTrackingChanged, PassEventArgs(this));
}

void PassController::setPoint(const ShaderChPoint & p)
{
	mPass->setPoint(ps);

	fireEvent(PointSizeChanged, PassEventArgs(this));
}

void PassController::setPointSpritesEnabled(bool enabled)
{
	mPass->setPoint(ShaderCh::SOT_Set)->setSpritesEnable(enabled);

	fireEvent(PointSpritesChanged, PassEventArgs(this));
}

void PassController::setPointAttenuation(bool enabled, NIIf constant /* =0.0f */, NIIf linear /* =1.0f */, NIIf quadratic /* =0.0f */)
{
    ShaderChPoint * scp = mPass->setPoint(ShaderCh::SOT_Set);
	scp->setAttenuateEnable(enabled);
    scp->setConstant(constant);
    scp->setLinear(linear);
    scp->setQuadratic(quadratic);

	fireEvent(PointAttenuationChanged, PassEventArgs(this));
}

void PassController::setPointMinSize(NIIf min)
{
	mPass->setPoint(ShaderCh::SOT_Set)->setMinSize(min);

	fireEvent(PointMinSizeChanged, PassEventArgs(this));
}

void PassController::setPointMaxSize(NIIf max)
{
	mPass->setPoint(ShaderCh::SOT_Set)->setMaxSize(max);

	fireEvent(PointMaxSizeChanged, PassEventArgs(this));
}

void PassController::setDepthCheck(bool b)
{
	mPass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(b);

	fireEvent(DepthCheckChanged, PassEventArgs(this));
}

void PassController::setDepthWrite(bool b)
{
	mPass->setDepthWrite(b);

	fireEvent(DepthWriteChanged, PassEventArgs(this));
}

void PassController::setColourWrite(Nmark mark)
{
	mPass->setColourWrite(mark);

	fireEvent(ColourWriteChanged, PassEventArgs(this));
}

void PassController::setCullingMode(CullingMode mode)
{
	mPass->setCullingMode(mode);

	fireEvent(CullingModeChanged, PassEventArgs(this));
}

void PassController::setSysCullingMode(SysCullingMode mode)
{
	mPass->setSysCullingMode(mode);

	fireEvent(ManualCullingModeChanged, PassEventArgs(this));
}

void PassController::setLight(bool b)
{
	mPass->setLight(b);

	fireEvent(LightingChanged, PassEventArgs(this));
}

void PassController::setLightMax(NCount count)
{
	mPass->setLightMax(count);

	fireEvent(MaxLightsChanged, PassEventArgs(this));
}

void PassController::setLightBegin(unsigned short startLight)
{
	mPass->setLightBegin(startLight);

	fireEvent(StartLightChanged, PassEventArgs(this));
}

void PassController::setShadeMode(ShadeMode mode)
{
	mPass->setShadeMode(mode);

	fireEvent(ShadingModeChanged, PassEventArgs(this));
}

void PassController::setPolygonMode(PolygonMode mode)
{
	mPass->setPolygonMode(mode);

	fireEvent(PolygonModeChanged, PassEventArgs(this));
}

void PassController::setFog(const ShaderChFog & f)
{
	mPass->setFog(f);

	fireEvent(FogChanged, PassEventArgs(this));
}

void PassController::setAlphaRejectSettings(CmpMode func, unsigned char value)
{
    mPass->setColour(ShaderCh::SOT_Set)->setAlphaReject(func);
    mPass->setColour(ShaderCh::SOT_Set)->setValue(value);
    mPass->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
	fireEvent(AlphaRejectionChanged, PassEventArgs(this));
}

void PassController::setAlphaReject(CmpMode func)
{
	mPass->setColour(ShaderCh::SOT_Set)->setAlphaReject(func);

	fireEvent(AlphaRejectionChanged, PassEventArgs(this));
}

void PassController::setAlphaRejectValue(Nui8 val)
{
	mPass->setColour(ShaderCh::SOT_Set)->setValue(val);

	fireEvent(AlphaRejectionChanged, PassEventArgs(this));
}

void PassController::setIteratePerLight(bool enabled, LightType lightType)
{
    mPass->setEachLight(enabled);
    mPass->setLightTypeLimit(lightType);
	fireEvent(IteratePerLightChanged, PassEventArgs(this));
}

void PassController::setLightRenderLimit(unsigned short c)
{
	mPass->setLightRenderLimit(c);

	fireEvent(LightCountPerIterationChanged, PassEventArgs(this));
}

void PassController::setVertexProgram(GpuProgramID gpid, bool resetParams /* =true */)
{
    mPass->getProgram().set(ST_VS, 0, gpid);
	// TODO: Fire event
}

void PassController::setShadowCasterVertexProgram(const String& name)
{
	mPass->getProgram().set(ST_VS_SC, 0, name, params);

	// TODO: Fire event
}

void PassController::setShadowReceiverVertexProgram(const String& name, GpuProgramParam * params)
{
	mPass->getProgram().set(ST_VS_SR, 0, name, params);

	// TODO: Fire event
}

void PassController::setShadowReceiverFragmentProgram(const String& name, GpuProgramParam * params)
{
	mPass->getProgram().set(ST_FS_SR, 0, name, params);

	// TODO: Fire event
}

void PassController::setFragmentProgram(const String& name, GpuProgramParam * params)
{
	mPass->getProgram().set(ST_FS, 0, name, params);

	// TODO: Fire event
}