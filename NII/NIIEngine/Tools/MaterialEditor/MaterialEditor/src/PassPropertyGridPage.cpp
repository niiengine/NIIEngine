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
#pragma warning(disable:4800)

#include "PassPropertyGridPage.h"

#include <wx/propgrid/advprops.h>

#include "OgreBlendMode.h"
#include "OgreCommon.h"
#include "OgrePass.h"
#include "OgrePrerequisites.h"

#include "PassController.h"

using namespace Ogre;

BEGIN_EVENT_TABLE(PassPropertyGridPage, wxPropertyGridPage)
	EVT_PG_CHANGED(-1, PassPropertyGridPage::propertyChanged)
END_EVENT_TABLE()

PassPropertyGridPage::PassPropertyGridPage(PassController* controller)
: mController(controller)
{
}

PassPropertyGridPage::~PassPropertyGridPage()
{
}

void PassPropertyGridPage::populate()
{
	createGeneralCategory();
	createReflectanceCategory();
	createPointCategory();
	createSceneBlendingCategory();
	createDepthCategory();
	createCullingCategory();
	createIlluminationCategory();
	createFogCategory();
	createAlphaRejectionCategory();
	createMiscCategory();
}

void PassPropertyGridPage::createGeneralCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("General")));

	// Name
	mNameId = Append(wxStringProperty(wxT("Name"), wxPG_LABEL, pass->getName()));
	SetPropertyHelpString(mNameId, wxT("Name of this ShaderCh"));
}

void PassPropertyGridPage::createReflectanceCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Reflectance")));

	// Ambient
	Colour ambient = pass->getColour(ShaderCh::SOT_Get)->getAmbient();
	mAmbientId = Append(wxColourProperty(wxT("Ambient"), wxPG_LABEL, wxColour((int)(255 * ambient.r), (int)(255 * ambient.g), (int)(255 * ambient.b))));
	SetPropertyHelpString(mAmbientId, wxT("Ambient colour reflectance"));

	// Diffuse
	Colour diffuse = pass->getColour(ShaderCh::SOT_Get)->getDiffuse();
	mDiffuseId = Append(wxColourProperty(wxT("Diffuse"), wxPG_LABEL, wxColour((int)(255 * diffuse.r), (int)(255 * diffuse.g), (int)(255 * diffuse.b))));
	SetPropertyHelpString(mDiffuseId, wxT("Diffuse colour reflectance"));

	// Specular
	Colour specular = pass->getColour(ShaderCh::SOT_Get)->getSpecular();
	mSpecularId = Append(wxColourProperty(wxT("Specular"), wxPG_LABEL, wxColour((int)(255 * specular.r), (int)(255 * specular.g), (int)(255 * specular.b))));
	SetPropertyHelpString(mSpecularId, wxT("Specular colour reflectance"));

	// Shininess
	mShininessId = Append(wxFloatProperty(wxT("Shininess"), wxPG_LABEL, pass->getColour(ShaderCh::SOT_Get)->getShininess()));
	SetPropertyHelpString(mShininessId, wxT("Shininess, affecting the size of specular highlights"));
}


void PassPropertyGridPage::createPointCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Point"), wxPG_LABEL));

	// Point Size
	wxPGId pointSize = Append(wxParentProperty(wxT("Size"), wxPG_LABEL));
	mPointSizeId = AppendIn(pointSize, wxFloatProperty(wxT("Size"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getSize()));
	SetPropertyHelpString(mPointSizeId, wxT("Point size, affecting the size of points when rendering a point list, or a list of point sprites"));

	// Point Sprites
	mPointSpritesId = Append(wxBoolProperty(wxT("Point Sprites"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->isSpritesEnable()));

	// Point Attenuation
	mAttenuationId = Append(wxParentProperty(wxT("Attenuation"),wxPG_LABEL));
	SetPropertyHelpString(mAttenuationId, wxT("Determines how points are attenuated with distance"));
	mPointAttenuationId = AppendIn(mAttenuationId, wxBoolProperty(wxT("Enabled"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->isAttenuateEnable()));
	mPointMinSizeId = AppendIn(mAttenuationId, wxFloatProperty(wxT("Min"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getMinSize()));
	SetPropertyHelpString(mPointMinSizeId, wxT("Minimum point size, when point attenuation is in use"));
	mPointMaxSizeId = AppendIn(mAttenuationId, wxFloatProperty(wxT("Max"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getMaxSize()));
	SetPropertyHelpString(mAttenuationId, wxT("Maximum point size, when point attenuation is in use"));
	mPointAttenuationConstantId = AppendIn(mAttenuationId, wxFloatProperty(wxT("Constant"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getConstant()));
	SetPropertyHelpString(mPointAttenuationConstantId, wxT("Constant coefficient of the point attenuation"));
	mPointAttenuationLinearId = AppendIn(mAttenuationId, wxFloatProperty(wxT("Linear"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getLinear()));
	SetPropertyHelpString(mPointAttenuationLinearId, wxT("Linear coefficient of the point attenuation"));
	mPointAttenuationQuadraticId = AppendIn(mAttenuationId, wxFloatProperty(wxT("Quadratic"), wxPG_LABEL, pass->getPoint(ShaderCh::SOT_Get)->getQuadratic()));
	SetPropertyHelpString(mPointAttenuationQuadraticId, wxT("Quadratic coefficient of the point attenuation"));
}


void PassPropertyGridPage::createSceneBlendingCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Scene Blending")));

	wxPGChoices sbtChoices;
	sbtChoices.Add(wxT("N/A") -1);
	sbtChoices.Add(wxT("Transparent Alpha"), FBM_INV_ALPHA);
	sbtChoices.Add(wxT("Transparent Colour"), FBM_INV_COLOUR);
	sbtChoices.Add(wxT("Add"), FBM_ADD);
	sbtChoices.Add(wxT("Modulate"), FBM_MODULATE);
	sbtChoices.Add(wxT("Replace"), FBM_REPLACE);

	wxPGChoices sbfChoices;
	sbfChoices.Add(wxT("One"), CF_ONE);
	sbfChoices.Add(wxT("Zero"), CF_ZERO);
	sbfChoices.Add(wxT("Dest Colour"), CF_DST);
	sbfChoices.Add(wxT("Src Colour"), CF_SRC);
	sbfChoices.Add(wxT("One Minus Dest Colour"), CF_INV_DST);
	sbfChoices.Add(wxT("One Minus Src Colour"), CF_INV_SRC);
	sbfChoices.Add(wxT("Dest Alpha"), CF_DST_ALPHA);
	sbfChoices.Add(wxT("Source Alpha"), CF_SRC_ALPHA);
	sbfChoices.Add(wxT("One Minus Dest Alpha"), CF_INV_DST_ALPHA);
	sbfChoices.Add(wxT("One Minus Source Alpha"), CF_INV_SRC_ALPHA);

	// Scene Blend Type
	bool type = true;
	FrameBlendMode blendType;
	ColourFactor srcFactor = pass->getBlend(ShaderCh::SOT_Get)->mSrcFactor;
	ColourFactor destFactor = pass->getBlend(ShaderCh::SOT_Get)->mDstFactor;
	if(srcFactor == CF_SRC_ALPHA && destFactor == CF_INV_SRC_ALPHA)
		blendType = FBM_INV_ALPHA;
	else if(srcFactor == CF_SRC && destFactor == CF_INV_SRC)
		blendType = FBM_INV_COLOUR;
	else if(srcFactor == CF_DST && destFactor == CF_ZERO)
		blendType = FBM_MODULATE;
	else if(srcFactor == CF_ONE && destFactor == CF_ONE)
		blendType = FBM_ADD;
	else if(srcFactor == CF_ONE && destFactor == CF_ZERO)
		blendType= FBM_REPLACE;
	else type = false;

	mSceneBlendTypeId = Append(wxEnumProperty(wxT("Type"), wxPG_LABEL, sbtChoices, (type) ? blendType : 0));

	// Source Scene Blend Type
	mSrcSceneBlendTypeId = Append(wxEnumProperty(wxT("Src Factor"), wxPG_LABEL, sbfChoices, srcFactor));

	// Destination Scene Blend Type
	mDestSceneBlendTypeId = Append(wxEnumProperty(wxT("Dest Factor"), wxPG_LABEL, sbfChoices, destFactor));
}


void PassPropertyGridPage::createDepthCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Depth")));

	// Depth Check
	mDepthCheckId = Append(wxBoolProperty(wxT("Depth Check"), wxPG_LABEL, pass->getStencil(ShaderCh::SOT_Get)->isDepthEnable()));

	// Depth Write
	mDepthWriteId = Append(wxBoolProperty(wxT("Depth Write"), wxPG_LABEL, pass->getStencil(ShaderCh::SOT_Get)->isDepthWrite()));

	//  Depth Function
	wxPGChoices compareFuncChoices;
	compareFuncChoices.Add(wxT("Fail"), CM_ALWAYS_FAIL);
	compareFuncChoices.Add(wxT("ShaderCh"), CM_ALWAYS_PASS);
	compareFuncChoices.Add(wxT("<"), CM_LESS);
	compareFuncChoices.Add(wxT("<="), CM_LESS_EQUAL);
	compareFuncChoices.Add(wxT("=="), CM_EQUAL);
	compareFuncChoices.Add(wxT("!="), CM_NOT_EQUAL);
	compareFuncChoices.Add(wxT(">="), CM_GREATER_EQUAL);
	compareFuncChoices.Add(wxT(">"), CM_GREATER);

	mDepthFunctionId = Append(wxEnumProperty(wxT("Depth Function"), wxPG_LABEL, compareFuncChoices, pass->getStencil(ShaderCh::SOT_Get)->getDepthCompareFunc()));

	mDepthBiasId = Append(wxParentProperty(wxT("Depth Bias"), wxPG_LABEL));

	// Constant Bias
	mDepthBiasConstantId = AppendIn(mDepthBiasId, wxFloatProperty(wxT("Constant"), wxPG_LABEL, pass->getStencil(ShaderCh::SOT_Get)->getBiasConstant()));

	// Slope Bias
	mDepthBiasSlopeId = AppendIn(mDepthBiasId, wxFloatProperty(wxT("Slope Scale"), wxPG_LABEL, pass->getStencil(ShaderCh::SOT_Get)->getBiasSlopeScale()));
}

void PassPropertyGridPage::createCullingCategory()
{
	const ShaderCh * ch = mController->getPass();

	Append(wxPropertyCategory(wxT("Culling")));

	// Culling Mode
	wxPGChoices cullingModeChoices;
	cullingModeChoices.Add(wxT("None"), CM_None);
	cullingModeChoices.Add(wxT("Clockwise"), CM_Clockwise);
	cullingModeChoices.Add(wxT("Counterclockwise"), CM_Anticlockwise);

	Append(wxEnumProperty(wxT("Culling Mode"), wxPG_LABEL, cullingModeChoices,
        ch->getStencil(ShaderCh::SOT_Get)->getDepthCompareFunc()));

	// Manual Culling Mode
	wxPGChoices manualCullingModeChoices;
	manualCullingModeChoices.Add(wxT("None"), SCM_NONE);
	manualCullingModeChoices.Add(wxT("Back"), SCM_BACK);
	manualCullingModeChoices.Add(wxT("Front"), SCM_FRONT);

	mManualCullingModeId = Append(wxEnumProperty(wxT("Manual Culling Mode"),
        wxPG_LABEL, manualCullingModeChoices, ch->getSysCullingMode()));
}

void PassPropertyGridPage::createIlluminationCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Illumination")));

	mLightingId = Append(wxBoolProperty(wxT("Lighting"), wxPG_LABEL, pass->isLight()));

	// Max Simultaneous Lights
	mMaxLightsId = Append(wxIntProperty(wxT("Max Lights"), wxPG_LABEL, pass->getLightMax()));

	// Start Light
	mStartLightId = Append(wxIntProperty(wxT("Start Light"), wxPG_LABEL, pass->getLightBegin()));

	// Light Iteration
	mIterationId = Append(wxBoolProperty(wxT("Iteration"), wxPG_LABEL, pass->isEachLight()));

	// Shading Mode
	wxPGChoices shadingModeChoices;
	shadingModeChoices.Add(wxT("Flat"), SM_FLAT);
	shadingModeChoices.Add(wxT("Gouraud"), SM_GOURAUD);
	shadingModeChoices.Add(wxT("Phong"), SM_PHONG);

	mShadingModeId = Append(wxEnumProperty(wxT("Shading Mode"), wxPG_LABEL, shadingModeChoices, pass->getShaderMode()));

	// Self Illumination
	Colour selfIllum = pass->getColour(ShaderCh::SOT_Get)->getEmissive();
	mSelfIlluminationId = Append(wxColourProperty(wxT("Self Illumination"), wxPG_LABEL, wxColour((int)(255 * selfIllum.r), (int)(255 * selfIllum.g), (int)(255 * selfIllum.b))));
}

void PassPropertyGridPage::createFogCategory()
{
	const ShaderCh * pass = mController->getPass();

	mOverrideSceneId = Append(wxPropertyCategory(wxT("Fog")));

	// Fog Enabled
	mFogOverrideId = Append(wxBoolProperty(wxT("Override Scene"), wxPG_LABEL, pass->getFog(ShaderCh::SOT_Get)->mMode ! = FM_NONE));

	// Fog Mode
	wxPGChoices fogModeChoices;
	fogModeChoices.Add(wxT("None"), FM_NONE);
	fogModeChoices.Add(wxT("EXP"), FM_EXP);
	fogModeChoices.Add(wxT("EXP2"), FM_EXP2);
	fogModeChoices.Add(wxT("Linear"), FM_LINEAR);

	mFogModeId = Append(wxEnumProperty(wxT("Fog Mode"), wxPG_LABEL, fogModeChoices, pass->getFog(ShaderCh::SOT_Get)->mMode));
}

// Possibly better as a wxParentProperty within Misc?
void PassPropertyGridPage::createAlphaRejectionCategory()
{
	const ShaderCh* pass = mController->getPass();
	Append(wxPropertyCategory(wxT("Alpha Rejection"), wxPG_LABEL));

	// Alpha Reject Func
	wxPGChoices compareFuncChoices;
	compareFuncChoices.Add(wxT("Fail"), CM_ALWAYS_FAIL);
	compareFuncChoices.Add(wxT("ShaderCh"), CM_ALWAYS_PASS);
	compareFuncChoices.Add(wxT("<"), CM_LESS);
	compareFuncChoices.Add(wxT("<="), CM_LESS_EQUAL);
	compareFuncChoices.Add(wxT("=="), CM_EQUAL);
	compareFuncChoices.Add(wxT("!="), CM_NOT_EQUAL);
	compareFuncChoices.Add(wxT(">="), CM_GREATER_EQUAL);
	compareFuncChoices.Add(wxT(">"), CM_GREATER);
	mAlphaRejectFuncId = Append(wxEnumProperty(wxT("Function"), wxPG_LABEL, compareFuncChoices, pass->getColour(ShaderCh::SOT_Get)->getAlphaReject()));

	// Alpha Reject Value
	mAlphaRejectValueId = Append(wxIntProperty(wxT("Value"), wxPG_LABEL, pass->getColour(ShaderCh::SOT_Get)->getValue()));
}

void PassPropertyGridPage::createMiscCategory()
{
	const ShaderCh* pass = mController->getPass();

	Append(wxPropertyCategory(wxT("Misc")));

	// Colour Write
	mColourWriteId = Append(wxIntProperty(wxT("Colour Write"), wxPG_LABEL, pass->getColourWrite()));

	// Polygon Mode
	wxPGChoices polygonModeChoices;
	polygonModeChoices.Add(wxT("Points"), PM_POINTS);
	polygonModeChoices.Add(wxT("Wireframe"), PM_LINE);
	polygonModeChoices.Add(wxT("Solid"), PM_SOLID);

	mPolygonModeId = Append(wxEnumProperty(wxT("Polygon Mode"), wxPG_LABEL, polygonModeChoices, pass->getPolygonMode()));

	// Track Vertex Colour Type
	wxPGChoices vertexColourTypeChoices;
	vertexColourTypeChoices.Add(wxT("None"), CM_T_NONE);
	vertexColourTypeChoices.Add(wxT("Ambient"), CM_T_AMBIENT);
	vertexColourTypeChoices.Add(wxT("Diffuse"), CM_T_DIFFUSE);
	vertexColourTypeChoices.Add(wxT("Specular"), CM_T_SPECULAR);
	vertexColourTypeChoices.Add(wxT("Emissive"), CM_T_EMISSIVE);

	mTrackVertexColourTypeId = Append(wxEnumProperty(wxT("Track Vertex Colour Type"), wxPG_LABEL, vertexColourTypeChoices, pass->getColour(ShaderCh::SOT_Get)->getColourTracking()));
}

void PassPropertyGridPage::propertyChanged(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();
	if(id == mNameId)
	{
		mController->setName(event.GetPropertyValueAsString().c_str());
	}
	else if(id == mAmbientId)
	{
		// TODO
	}
	else if(id == mDiffuseId)
	{
		// TODO
	}
	else if(id == mSpecularId)
	{
		// TODO
	}
	else if(id == mShininessId)
	{

	}
	else if(id == mPointSizeId)
	{
		mController->setPoint((NIIf)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointSpritesId)
	{
		mController->setPointSpritesEnabled(event.GetPropertyValueAsBool());
	}
	//else if(id == mAttenuationId)
	//{
	//}
	else if(id == mPointAttenuationId)
	{
		const ShaderCh* pass = mController->getPass();
		mController->setPointAttenuation(event.GetPropertyValueAsBool(),
			pass->getPoint(ShaderCh::SOT_Get)->getConstant(), pass->getPoint(ShaderCh::SOT_Get)->getLinear(),
			pass->getPoint(ShaderCh::SOT_Get)->getQuadratic());
	}
	else if(id == mPointMinSizeId)
	{
		mController->setPointMinSize((NIIf)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointMaxSizeId)
	{
		mController->setPointMaxSize((NIIf)event.GetPropertyValueAsDouble());
	}
	else if(id == mPointAttenuationConstantId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setPointAttenuation(pass->getPoint(ShaderCh::SOT_Get)->isAttenuateEnable(),
			(NIIf)event.GetPropertyValueAsDouble(), pass->getPoint(ShaderCh::SOT_Get)->getLinear(),
			pass->getPoint(ShaderCh::SOT_Get)->getQuadratic());
	}
	else if(id == mPointAttenuationLinearId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setPointAttenuation(pass->getPoint(ShaderCh::SOT_Get)->isAttenuateEnable(),
			pass->getPoint(ShaderCh::SOT_Get)->getConstant(), (NIIf)event.GetPropertyValueAsDouble(),
			pass->getPoint(ShaderCh::SOT_Get)->getQuadratic());
	}
	else if(id == mPointAttenuationQuadraticId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setPointAttenuation(pass->getPoint(ShaderCh::SOT_Get)->isAttenuateEnable(),
			pass->getPoint(ShaderCh::SOT_Get)->getConstant(), pass->getPoint(ShaderCh::SOT_Get)->getLinear(),
			(NIIf)event.GetPropertyValueAsDouble());
	}
	else if(id == mDepthCheckId)
	{
		mController->setDepthCheck(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthWriteId)
	{
		mController->setDepthWrite(event.GetPropertyValueAsBool());
	}
	else if(id == mDepthFunctionId)
	{

	}
	//else if(id == mDepthBiasId)
	//{
	//}
	else if(id == mDepthBiasConstantId)
	{

	}
	else if(id == mDepthBiasSlopeId)
	{

	}
	else if(id == mManualCullingModeId)
	{
		mController->setSysCullingMode((SysCullingMode)event.GetPropertyValueAsInt());
	}
	else if(id == mLightingId)
	{
		mController->setLight(event.GetPropertyValueAsBool());
	}
	else if(id == mMaxLightsId)
	{
		mController->setLightMax(event.GetPropertyValueAsInt());
	}
	else if(id == mStartLightId)
	{
		mController->setLightBegin(event.GetPropertyValueAsInt());
	}
	else if(id == mIterationId)
	{
		mController->setLightRenderLimit(event.GetPropertyValueAsInt());
	}
	else if(id == mShadingModeId)
	{
		mController->setShadeMode((ShadeMode)event.GetPropertyValueAsInt());
	}
	else if(id == mSelfIlluminationId)
	{
		// TODO
	}
	else if(id == mOverrideSceneId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setFog(pass->getFog(ShaderCh::SOT_Get));
	}
	else if(id == mFogOverrideId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setFog(pass->getFog(ShaderCh::SOT_Get));
	}
	else if(id == mFogModeId)
	{
		const ShaderCh * pass = mController->getPass();
		mController->setFog(ShaderChFog((FogMode)event.GetPropertyValueAsInt(),
            pass->getFog->mColour, pass->getFog.mDensity, pass->getFog.mBegin,
                pass->getFog.mEnd));
	}
	else if(id == mAlphaRejectFuncId)
	{
		mController->setAlphaReject((CmpMode)event.GetPropertyValueAsInt());
	}
	else if(id == mAlphaRejectValueId)
	{
		mController->setAlphaRejectValue(event.GetPropertyValueAsInt());
	}
	else if(id == mColourWriteId)
	{
		mController->setColourWrite(event.GetPropertyValueAsInt());
	}
	else if(id == mPolygonModeId)
	{
		mController->setPolygonMode((PolygonMode)event.GetPropertyValueAsInt());
	}
	else if(id == mTrackVertexColourTypeId)
	{
		mController->setColourTracking((Nmark)event.GetPropertyValueAsInt());
	}
}