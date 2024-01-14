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
#ifndef _TEXTUREUNITCONTROLLER_H_
#define _TEXTUREUNITCONTROLLER_H_

#include <list>

#include <boost/signal.hpp>

#include "OgreTexture.h"
#include "OgreTextureUnitState.h"

#include "EventArgs.h"
#include "EventContainer.h"

class PassController;

using namespace Ogre;

class TextureUnitController;

class TextureUnitEventArgs : public EventArgs
{
public:
	TextureUnitEventArgs(TextureUnitController* tuc) : mController(tuc) {}
	~TextureUnitEventArgs() {}

	TextureUnitController* getController() { return mController; }

protected:
	TextureUnitController* mController;
};

class TextureUnitController : public EventContainer
{
public:
	enum TextureUnitEvent
	{
		TextureNameChanged,
		CubicTextureNameChanged,
		AnimatedTextureNameChanged,
		TextureDimensionsChanged,
		CurrentFrameChanged,
		FrameTextureNameChanged,
		FrameTextureNameAdded,
		FrameTextureNameRemoved,
		BindingTypeChanged,
		ContentTypeChanged,
		PixelFormatChanged,
		NumMipmapsChanged,
		AlphaChanged,
		TextureCoordSetChanged,
		TextureTransformChanged,
		TextureScrollChanged,
		TextureUScrollChanged,
		TextureVScrollChanged,
		TextureUScaleChanged,
		TextureVScaleChanged,
		TextureRotateChanged,
		TextureAddressingModeChanged,
		TextureBorderColourChanged,
		ColourOperationChanged,
		ColourOperationExChanged,
		ColourOpMultipassFallbackChanged,
		AlphaOperationChanged,
		EffectAdded,
		EnvironmentMapChanged,
		ScrollAnimationChanged,
		RotateAnimationChanged,
		TransformAnimationChanged,
		ProjectiveTexturingChanged,
		EffectRemoved,
		Blanked,
		TextureFilteringChanged,
		TextureAnisotropyChanged,
		TextureMipMapBiasChanged,
		NameChanged,
		TextureNameAliasChanged
	};

	TextureUnitController();
	TextureUnitController(ShaderChTextureUnit * textureUnit);
	TextureUnitController(PassController * parent, ShaderChTextureUnit * textureUnit);
	virtual ~TextureUnitController();

	ShaderChTextureUnit * getTextureUnit() const;
	void getTextureUnit(ShaderChTextureUnit * tus);

	PassController * getParentController() const;

	void setSrc(ResourceID rid, Texture::Type ttype);
	void setSrc(ResourceID * rid, bool forUVW = false);
	void setAni(const String & name, NCount count, NIIf duration = 0);
	void setAni(const String * const names, NCount count, NIIf duration = 0);
	void setCurrentFrame(unsigned int frameNumber);
	void setFrame(const String & name, unsigned int frameNumber);
	void addFrame(const String & name);
	void removeFrame(const size_t frameNumber);
	void setFetchType(TextureDataFetch bt);
	void setContentType(ShaderChTextureUnit::Type ct);
	void setFormat(PixelFormat pf);
	void setMipmapCount(int numMipmaps);
	void setTextureCoordSet(unsigned int set);
	void setMatrix(const Matrix4 & x);
	void setUOft(NIIf value);
	void setUScale(NIIf value);
	void setVScale(NIIf value);
	void setRotate(const Radian & angle);
    void setUAddressing(TextureAddressingMode tam);
    void setVAddressing(TextureAddressingMode tam);
    void setWAddressing(TextureAddressingMode tam);
    void setTextureBorderColour(const Colour & colour);
	void add(ShaderChTextureUnit::TextureEffect & effect);
    void setEnvMap(bool b); 
    void setEnvMapType(ShaderChTextureUnit::EnvMapMode type);
	void setScrollAnimation(NIIf uSpeed, NIIf vSpeed);
	void setRotateAnimation(NIIf speed);
    void setProjTexture(bool b); 
    void setProjFrustum(const Frustum * proj);
	void removeAllEffects(void);
	void remove(ShaderChTextureUnit::TextureTransformType type);
	void setTextureMipmapBias(float bias);
	void setName(const String & name);
protected:
	void registerEvents();

	PassController * mParentController;
	ShaderChTextureUnit * mTextureUnit;
};
#endif // _TEXTUREUNITCONTROLLER_H_