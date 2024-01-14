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
#include "TextureUnitPropertyGridPage.h"

#include <boost/bind.hpp>

#include <wx/propgrid/advprops.h>

#include "OgreCommon.h"
#include "OgreTextureUnitState.h"

#include "TextureUnitController.h"

using namespace Ogre;

BEGIN_EVENT_TABLE(TextureUnitPropertyGridPage, wxPropertyGridPage)
EVT_PG_CHANGED(-1, TextureUnitPropertyGridPage::propertyChanged)
END_EVENT_TABLE()

TextureUnitPropertyGridPage::TextureUnitPropertyGridPage(TextureUnitController* controller)
: mController(controller)
{
	mController->subscribe(TextureUnitController::TextureNameChanged, boost::bind(&TextureUnitPropertyGridPage::textureNameChanged, this, _1));
	mController->subscribe(TextureUnitController::CubicTextureNameChanged, boost::bind(&TextureUnitPropertyGridPage::cubicTextureNameChanged, this, _1));
	mController->subscribe(TextureUnitController::AnimatedTextureNameChanged, boost::bind(&TextureUnitPropertyGridPage::animatedTextureNameChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureDimensionsChanged, boost::bind(&TextureUnitPropertyGridPage::textureDimensionsChanged, this, _1));
	mController->subscribe(TextureUnitController::CurrentFrameChanged, boost::bind(&TextureUnitPropertyGridPage::currentFrameChanged, this, _1));
	mController->subscribe(TextureUnitController::FrameTextureNameChanged, boost::bind(&TextureUnitPropertyGridPage::frameTextureNameChanged, this, _1));
	mController->subscribe(TextureUnitController::FrameTextureNameAdded, boost::bind(&TextureUnitPropertyGridPage::frameTextureNameAdded, this, _1));
	mController->subscribe(TextureUnitController::FrameTextureNameRemoved, boost::bind(&TextureUnitPropertyGridPage::frameTextureNameRemoved, this, _1));
	mController->subscribe(TextureUnitController::BindingTypeChanged, boost::bind(&TextureUnitPropertyGridPage::bindingTypeChanged, this, _1));
	mController->subscribe(TextureUnitController::ContentTypeChanged, boost::bind(&TextureUnitPropertyGridPage::contentTypeChanged, this, _1));
	mController->subscribe(TextureUnitController::PixelFormatChanged, boost::bind(&TextureUnitPropertyGridPage::pixelFormatChanged, this, _1));
	mController->subscribe(TextureUnitController::NumMipmapsChanged, boost::bind(&TextureUnitPropertyGridPage::numMipmapsChanged, this, _1));
	mController->subscribe(TextureUnitController::AlphaChanged, boost::bind(&TextureUnitPropertyGridPage::alphaChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureCoordSetChanged, boost::bind(&TextureUnitPropertyGridPage::textureCoordSetChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureTransformChanged, boost::bind(&TextureUnitPropertyGridPage::textureTransformChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureScrollChanged, boost::bind(&TextureUnitPropertyGridPage::textureScrollChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureUScrollChanged, boost::bind(&TextureUnitPropertyGridPage::textureUScrollChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureVScrollChanged, boost::bind(&TextureUnitPropertyGridPage::textureVScrollChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureUScaleChanged, boost::bind(&TextureUnitPropertyGridPage::textureUScaleChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureVScaleChanged, boost::bind(&TextureUnitPropertyGridPage::textureVScaleChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureRotateChanged, boost::bind(&TextureUnitPropertyGridPage::textureRotateChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureAddressingModeChanged, boost::bind(&TextureUnitPropertyGridPage::textureAddressingModeChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureBorderColourChanged, boost::bind(&TextureUnitPropertyGridPage::textureBorderColourChanged, this, _1));
	mController->subscribe(TextureUnitController::ColourOperationExChanged, boost::bind(&TextureUnitPropertyGridPage::colourOperationExChanged, this, _1));
	mController->subscribe(TextureUnitController::ColourOpMultipassFallbackChanged, boost::bind(&TextureUnitPropertyGridPage::colourOpMultipassFallbackChanged, this, _1));
	mController->subscribe(TextureUnitController::AlphaOperationChanged, boost::bind(&TextureUnitPropertyGridPage::alphaOperationChanged, this, _1));
	//mController->subscribe(TextureUnitController::EffectAdded, boost::bind(&TextureUnitPropertyGridPage::effectAdded, this, _1));
	mController->subscribe(TextureUnitController::EnvironmentMapChanged, boost::bind(&TextureUnitPropertyGridPage::environmentMapChanged, this, _1));
	mController->subscribe(TextureUnitController::ScrollAnimationChanged, boost::bind(&TextureUnitPropertyGridPage::scrollAnimationChanged, this, _1));
	mController->subscribe(TextureUnitController::RotateAnimationChanged, boost::bind(&TextureUnitPropertyGridPage::rotateAnimationChanged, this, _1));
	mController->subscribe(TextureUnitController::TransformAnimationChanged, boost::bind(&TextureUnitPropertyGridPage::projectiveTexturingChanged, this, _1));
	mController->subscribe(TextureUnitController::ProjectiveTexturingChanged, boost::bind(&TextureUnitPropertyGridPage::projectiveTexturingChanged, this, _1));
	//mController->subscribe(TextureUnitController::EffectRemoved, boost::bind(&TextureUnitPropertyGridPage::effectRemoved, this, _1));
	//mController->subscribe(TextureUnitController::Blanked, boost::bind(&TextureUnitPropertyGridPage::blanked, this, _1));
	mController->subscribe(TextureUnitController::TextureFilteringChanged, boost::bind(&TextureUnitPropertyGridPage::textureFilteringChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureAnisotropyChanged, boost::bind(&TextureUnitPropertyGridPage::textureAnisotropyChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureMipMapBiasChanged, boost::bind(&TextureUnitPropertyGridPage::textureMipMapBiasChanged, this, _1));
	mController->subscribe(TextureUnitController::NameChanged, boost::bind(&TextureUnitPropertyGridPage::nameChanged, this, _1));
	mController->subscribe(TextureUnitController::TextureNameAliasChanged, boost::bind(&TextureUnitPropertyGridPage::textureNameAliasChanged, this, _1));
}

TextureUnitPropertyGridPage::~TextureUnitPropertyGridPage()
{
}

void TextureUnitPropertyGridPage::populate()
{
	const ShaderChTextureUnit * tus = mController->getTextureUnit();

	mTextureNameId = Append(wxStringProperty(wxT("Texture Name"), wxPG_LABEL, tus->getResourceID()));
	mCurrentFrameId = Append(wxIntProperty(wxT("Current Frame"), wxPG_LABEL, tus->getCurrentFrame()));

	wxPGChoices btChoices;
	btChoices.Add(wxT("Fragment"), VDF_FS);
	btChoices.Add(wxT("Vertex"), VDF_VS);
	mBindingTypeId = Append(wxEnumProperty(wxT("Binding Type"), wxPG_LABEL, btChoices, VDF_FS));

	wxPGChoices ctChoices;
	ctChoices.Add(wxT("Named"), ShaderChTexture::T_NORMAL);
	ctChoices.Add(wxT("Shadow"), ShaderChTexture::T_SHADOW);
	mContentTypeId = Append(wxEnumProperty(wxT("Content Type"), wxPG_LABEL, ctChoices, ShaderChTexture::T_NORMAL));

	wxPGChoices pfChoices;
	pfChoices.Add(wxT("Unknown"), PF_UNKNOWN);
	pfChoices.Add(wxT("L8"), PF_L8ui);
	pfChoices.Add(wxT("L16"), PF_L16ui);
	pfChoices.Add(wxT("SHORT_L"), PF_L16ui);
	pfChoices.Add(wxT("A8"), PF_A8UN);
	pfChoices.Add(wxT("BYTE_A"), PF_A8UN);
	pfChoices.Add(wxT("A4L4"), PF_A4L4i);
	pfChoices.Add(wxT("BYTE_LA"), PF_L8A8i);
	pfChoices.Add(wxT("R5G6B5"), PF_R5G6B5);
	pfChoices.Add(wxT("B5G6R5"), PF_B5G6R5);
	pfChoices.Add(wxT("R3G3B2"), PF_R3G3B2);
	pfChoices.Add(wxT("A4R4G4B4"), PF_A4R4G4B4);
	pfChoices.Add(wxT("A1R5G5B5"), PF_A1R5G5B5);
	pfChoices.Add(wxT("R8G8B8UN"), PF_R8G8B8UN);
	pfChoices.Add(wxT("B8G8R8"), PF_B8G8R8UN);
	pfChoices.Add(wxT("A8R8G8B8"), PF_A8R8G8B8);
	pfChoices.Add(wxT("A8B8G8R8"), PF_A8B8G8R8);
	pfChoices.Add(wxT("B8G8R8A8UN"), PF_B8G8R8A8UN);
	pfChoices.Add(wxT("R8G8B8A8"), PF_R8G8B8A8ui);
	pfChoices.Add(wxT("X8R8G8B8"), PF_X8R8G8B8);
	pfChoices.Add(wxT("X8B8G8R8"), PF_X8B8G8R8);
	pfChoices.Add(wxT("BYTE_RGB"), PF_BYTE_RGB);
	pfChoices.Add(wxT("BYTE_BGR"), PF_BYTE_BGR);
	pfChoices.Add(wxT("BYTE_BGRA"), PF_BYTE_BGRA);
	pfChoices.Add(wxT("BYTE_RGBA"), PF_BYTE_RGBA);
	pfChoices.Add(wxT("A2R10G10B10"), PF_A2R10G10B10);
	pfChoices.Add(wxT("A2B10G10R10"), PF_A2B10G10R10);
	pfChoices.Add(wxT("DXT1"), PF_BC1UN);
	pfChoices.Add(wxT("DXT3"), PF_BC2UN);
	pfChoices.Add(wxT("DXT5"), PF_BC3UN);
	pfChoices.Add(wxT("FLOAT16_R"), PF_R16);
	pfChoices.Add(wxT("FLOAT16_RGB"), PF_R16G16B16UN);
	pfChoices.Add(wxT("FLOAT16_RGBA"), PF_R16G16B16A16);
	pfChoices.Add(wxT("FLOAT32_R"), PF_R32);
	pfChoices.Add(wxT("FLOAT32_RGB"), PF_R32G32B32);
	pfChoices.Add(wxT("FLOAT32_RGBA"), PF_R32G32B32A32);
	pfChoices.Add(wxT("FLOAT16_GR"), PF_G16R16);
	pfChoices.Add(wxT("FLOAT32_GR"), PF_G32R32);
	pfChoices.Add(wxT("PF_Depth32"), PF_Depth32);
	pfChoices.Add(wxT("SHORT_RGBA"), PF_R16G16B16A16ui);
	pfChoices.Add(wxT("SHORT_GR"), PF_G16R16ui);
	pfChoices.Add(wxT("SHORT_RGB"), PF_R16G16B16ui);
	pfChoices.Add(wxT("PF_COUNT"), PF_COUNT);
	mPixelFormatId = Append(wxEnumProperty(wxT("Desired Format"), wxPG_LABEL, pfChoices, PF_UNKNOWN));

	mNumMipMapsId = Append(wxIntProperty(wxT("Mip Maps")));
}

void TextureUnitPropertyGridPage::propertyChanged(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();
	if(id == mTextureNameId) {}
	else if(id == mCubicTextureNameId) {}
	else if(id == mAnimatedTextureNameId) {}
	else if(id == mTextureDimensionsId) {}
	else if(id == mCurrentFrameId) {}
	else if(id == mFrameTextureNameId) {}
	else if(id == mBindingTypeId) {}
	else if(id == mContentTypeId) {}
	else if(id == mPixelFormatId) {}
	else if(id == mNumMipMapsId) {}
	else if(id == mAlphaId) {}
	else if(id == mTextureCoordSetId) {}
	else if(id == mTextureTransformId) {}
	else if(id == mTextureScrollId) {}
	else if(id == mTextureUScrollId) {}
	else if(id == mTextureVScrollId) {}
	else if(id == mTextureUScaleId) {}
	else if(id == mTextureVScaleId) {}
	else if(id == mTextureRotateId) {}
	else if(id == mTextureAddressingModeId) {}
	else if(id == mTextureBorderColourId) {}
	else if(id == mColourOperationExId) {}
	else if(id == mColourOpMultipassFallbackId) {}
	else if(id == mAlphaOperationId) {}
	else if(id == mEnvironmentMapId) {}
	else if(id == mScrollAnimationId) {}
	else if(id == mRotateAnimationId) {}
	else if(id == mTransformAnimationId) {}
	else if(id == mProjectiveTexturingId) {}
	else if(id == mTextureFilteringId) {}
	else if(id == mTextureAnisotropyId) {}
	else if(id == mTextureMipMapBiasId) {}
	else if(id == mNameId) {}
	else if(id == mTextureNameAliasId) {}
}

void TextureUnitPropertyGridPage::textureNameChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::cubicTextureNameChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mCubicTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::animatedTextureNameChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mAnimatedTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureDimensionsChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureDimensionsId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::currentFrameChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mCurrentFrameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::frameTextureNameChanged(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mFrameTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::frameTextureNameAdded(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mFrameTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::frameTextureNameRemoved(EventArgs& args)
{
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mFrameTextureNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::bindingTypeChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mBindingTypeId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::contentTypeChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mContentTypeId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::pixelFormatChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mPixelFormatId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::numMipmapsChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mNumMipMapsId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::alphaChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mAlphaId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureCoordSetChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureCoordSetId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureTransformChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureTransformId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureScrollChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureScrollId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureUScrollChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureUScrollId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureVScrollChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureVScrollId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureUScaleChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureUScaleId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureVScaleChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureVScaleId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureRotateChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureRotateId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureAddressingModeChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureAddressingModeId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureBorderColourChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureBorderColourId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::colourOperationExChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mColourOperationExId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::colourOpMultipassFallbackChanged(
		EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mColourOpMultipassFallbackId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::alphaOperationChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mAlphaOperationId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::environmentMapChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mEnvironmentMapId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::scrollAnimationChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mScrollAnimationId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::rotateAnimationChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mRotateAnimationId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::transformAnimationChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTransformAnimationId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::projectiveTexturingChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mProjectiveTexturingId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureFilteringChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureFilteringId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureAnisotropyChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureAnisotropyId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureMipMapBiasChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureMipMapBiasId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::nameChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mNameId);
	if (prop == NULL)
		return;
}

void TextureUnitPropertyGridPage::textureNameAliasChanged(EventArgs& args) {
	TextureUnitEventArgs tuea = dynamic_cast<TextureUnitEventArgs&>(args);
	TextureUnitController* tc = tuea.getController();

	wxPGProperty* prop = GetPropertyPtr(mTextureNameAliasId);
	if (prop == NULL)
		return;
}

