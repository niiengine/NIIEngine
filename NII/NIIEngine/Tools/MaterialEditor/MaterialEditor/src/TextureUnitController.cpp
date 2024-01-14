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

#include "TextureUnitController.h"
#include "OgreTextureUnitState.h"
#include "PassController.h"

TextureUnitController::TextureUnitController()
: mParentController(NULL), mTextureUnit(NULL)
{
	registerEvents();
}

TextureUnitController::TextureUnitController(ShaderChTextureUnit * tus)
: mParentController(NULL), mTextureUnit(tus)
{
	registerEvents();
}

TextureUnitController::TextureUnitController(PassController * parent, ShaderChTextureUnit * tus)
: mParentController(parent), mTextureUnit(tus)
{
	registerEvents();
}

TextureUnitController::~TextureUnitController()
{
}

void TextureUnitController::registerEvents()
{
	registerEvent(TextureNameChanged);
	registerEvent(CubicTextureNameChanged);
	registerEvent(AnimatedTextureNameChanged);
	registerEvent(TextureDimensionsChanged);
	registerEvent(CurrentFrameChanged);
	registerEvent(FrameTextureNameChanged);
	registerEvent(FrameTextureNameAdded);
	registerEvent(FrameTextureNameRemoved);
	registerEvent(BindingTypeChanged);
	registerEvent(ContentTypeChanged);
	registerEvent(PixelFormatChanged);
	registerEvent(NumMipmapsChanged);
	registerEvent(AlphaChanged);
	registerEvent(TextureCoordSetChanged);
	registerEvent(TextureTransformChanged);
	registerEvent(TextureScrollChanged);
	registerEvent(TextureUScrollChanged);
	registerEvent(TextureVScrollChanged);
	registerEvent(TextureUScaleChanged);
	registerEvent(TextureVScaleChanged);
	registerEvent(TextureRotateChanged);
	registerEvent(TextureAddressingModeChanged);
	registerEvent(TextureBorderColourChanged);
	registerEvent(ColourOperationChanged);
	registerEvent(ColourOperationExChanged);
	registerEvent(ColourOpMultipassFallbackChanged);
	registerEvent(AlphaOperationChanged);
	registerEvent(EffectAdded);
	registerEvent(EnvironmentMapChanged);
	registerEvent(ScrollAnimationChanged);
	registerEvent(RotateAnimationChanged);
	registerEvent(TransformAnimationChanged);
	registerEvent(ProjectiveTexturingChanged);
	registerEvent(EffectRemoved);
	registerEvent(Blanked);
	registerEvent(TextureFilteringChanged);
	registerEvent(TextureAnisotropyChanged);
	registerEvent(TextureMipMapBiasChanged);
	registerEvent(NameChanged);
	registerEvent(TextureNameAliasChanged);
}


PassController* TextureUnitController::getParentController() const
{
	return mParentController;
}

ShaderChTextureUnit* TextureUnitController::getTextureUnit() const
{
	return mTextureUnit;
}

void TextureUnitController::setSrc(ResourceID rid, Texture::Type ttype)
{
	mTextureUnit->setSrc(rid, ttype);

	fireEvent(TextureNameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setSrc(ResourceID * rid, Texture::Type ttype)
{
	mTextureUnit->setSrc(rid, ttype);

	fireEvent(CubicTextureNameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setAni(const String &name, NCount numFrames, NIIf duration /* = 0 */)
{
	mTextureUnit->setAni(&name, numFrames, duration);

	fireEvent(AnimatedTextureNameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setAni(const String *const names, NCount numFrames, NIIf duration /* = 0 */)
{
	mTextureUnit->setAni(names, numFrames, duration);

	fireEvent(AnimatedTextureNameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setCurrentFrame(unsigned int frameNumber)
{
	mTextureUnit->setCurrentFrame(frameNumber);

	fireEvent(CurrentFrameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setFrame(const String &name, unsigned int frameNumber)
{
	mTextureUnit->setFrame(name, frameNumber);

	fireEvent(FrameTextureNameChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::addFrame(const String &name)
{
	mTextureUnit->addFrame(name);

	fireEvent(FrameTextureNameAdded, TextureUnitEventArgs(this));
}

void TextureUnitController::removeFrame(const size_t frameNumber)
{
	mTextureUnit->removeFrame(frameNumber);

	fireEvent(FrameTextureNameRemoved, TextureUnitEventArgs(this));
}

void TextureUnitController::setFetchType(TextureDataFetch bt)
{
	mTextureUnit->setFetchType(bt);

	fireEvent(BindingTypeChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setContentType(ShaderChTextureUnit::Type ct)
{
	mTextureUnit->setContentType(ct);

	fireEvent(ContentTypeChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setFormat(PixelFormat pf)
{
	mTextureUnit->setFormat(pf);

	fireEvent(PixelFormatChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setMipmapCount(int numMipmaps)
{
	mTextureUnit->setMipmapCount(numMipmaps);

	fireEvent(NumMipmapsChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setTextureCoordSet(unsigned int set)
{
	mTextureUnit->setTextureCoordSet(set);

	fireEvent(TextureCoordSetChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setMatrix(const Matrix4 & xform)
{
	mTextureUnit->setMatrix(xform);

	fireEvent(TextureTransformChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setUOft(NIIf value)
{
	mTextureUnit->setUOft(value);

	fireEvent(TextureUScrollChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setUScale(NIIf value)
{
	mTextureUnit->setUScale(value);

	fireEvent(TextureUScaleChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setVScale(NIIf value)
{
	mTextureUnit->setVScale(value);

	fireEvent(TextureVScaleChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setRotate(const Radian & angle)
{
	mTextureUnit->setRotate(angle);

	fireEvent(TextureRotateChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setUAddressing(TextureAddressingMode tam)
{
	mTextureUnit->setUAddressing(tam);

	fireEvent(TextureAddressingModeChanged, TextureUnitEventArgs(this));
}
void TextureUnitController::setVAddressing(TextureAddressingMode u)
{
	mTextureUnit->setVAddressing(tam);

	fireEvent(TextureAddressingModeChanged, TextureUnitEventArgs(this));
}
void TextureUnitController::setWAddressing(TextureAddressingMode u)
{
	mTextureUnit->setWAddressing(tam);

	fireEvent(TextureAddressingModeChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setTextureBorderColour(const Colour &colour)
{
	mTextureUnit->setTextureBorderColour(colour);

	fireEvent(TextureBorderColourChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::add(ShaderChTextureUnit::TextureEffect & effect)
{
	mTextureUnit->add(effect);

	fireEvent(EffectAdded, TextureUnitEventArgs(this));
}

void TextureUnitController::setEnvMap(bool enable)
{
	mTextureUnit->setEnvMap(enable);

	fireEvent(EnvironmentMapChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setEnvMapType(ShaderChTextureUnit::EnvMapMode type)
{
	mTextureUnit->setEnvMapType(type);

	fireEvent(EnvironmentMapChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setScrollAnimation(NIIf uSpeed, NIIf vSpeed)
{
	mTextureUnit->setScrollAnimation(uSpeed, vSpeed);

	fireEvent(ScrollAnimationChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setRotateAnimation(NIIf speed)
{
	mTextureUnit->setRotateAnimation(speed);

	fireEvent(AlphaOperationChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setProjTexture(bool b)
{
    mTextureUnit->setProjTexture(b);
    fireEvent(ProjectiveTexturingChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setProjFrustum(const Frustum * proj)
{
    mTextureUnit->setProjFrustum(proj);
    fireEvent(ProjectiveTexturingChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::removeAllEffects(void)
{
	mTextureUnit->removeAllEffects();

	fireEvent(EffectRemoved, TextureUnitEventArgs(this));
}

void TextureUnitController::remove(ShaderChTextureUnit::TextureTransformType type)
{
	mTextureUnit->remove(type);

	fireEvent(EffectRemoved, TextureUnitEventArgs(this));
}

void TextureUnitController::setTextureMipmapBias(float bias)
{
	mTextureUnit->setMipmapBias(bias);

	fireEvent(TextureMipMapBiasChanged, TextureUnitEventArgs(this));
}

void TextureUnitController::setName(const String &name)
{
	mTextureUnit->setName(name);

	fireEvent(NameChanged, TextureUnitEventArgs(this));
}
