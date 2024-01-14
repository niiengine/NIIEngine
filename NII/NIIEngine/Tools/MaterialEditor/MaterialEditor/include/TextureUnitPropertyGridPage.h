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
#ifndef _TEXTUREUNITPROPERTYGRIDPAGE_H_
#define _TEXTUREUNITPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>

class TextureUnitController;
class EventArgs;

class TextureUnitPropertyGridPage : public wxPropertyGridPage
{
public:
	TextureUnitPropertyGridPage(TextureUnitController* controller);
	virtual ~TextureUnitPropertyGridPage();

	virtual void populate();

	void textureNameChanged(EventArgs& args);
	void cubicTextureNameChanged(EventArgs& args);
	void animatedTextureNameChanged(EventArgs& args);
	void textureDimensionsChanged(EventArgs& args);
	void currentFrameChanged(EventArgs& args);
	void frameTextureNameChanged(EventArgs& args);
	void frameTextureNameAdded(EventArgs& args);
	void frameTextureNameRemoved(EventArgs& args);
	void bindingTypeChanged(EventArgs& args);
	void contentTypeChanged(EventArgs& args);
	void pixelFormatChanged(EventArgs& args);
	void numMipmapsChanged(EventArgs& args);
	void alphaChanged(EventArgs& args);
	void textureCoordSetChanged(EventArgs& args);
	void textureTransformChanged(EventArgs& args);
	void textureScrollChanged(EventArgs& args);
	void textureUScrollChanged(EventArgs& args);
	void textureVScrollChanged(EventArgs& args);
	void textureUScaleChanged(EventArgs& args);
	void textureVScaleChanged(EventArgs& args);
	void textureRotateChanged(EventArgs& args);
	void textureAddressingModeChanged(EventArgs& args);
	void textureBorderColourChanged(EventArgs& args);
	void colourOperationExChanged(EventArgs& args);
	void colourOpMultipassFallbackChanged(EventArgs& args);
	void alphaOperationChanged(EventArgs& args);
	void environmentMapChanged(EventArgs& args);
	void scrollAnimationChanged(EventArgs& args);
	void rotateAnimationChanged(EventArgs& args);
	void transformAnimationChanged(EventArgs& args);
	void projectiveTexturingChanged(EventArgs& args);
	void textureFilteringChanged(EventArgs& args);
	void textureAnisotropyChanged(EventArgs& args);
	void textureMipMapBiasChanged(EventArgs& args);
	void nameChanged(EventArgs& args);
	void textureNameAliasChanged(EventArgs& args);

protected:
	virtual void propertyChanged(wxPropertyGridEvent& event);

	TextureUnitController* mController;

	wxPGId mTextureNameId;
    wxPGId mCubicTextureNameId;
    wxPGId mAnimatedTextureNameId;
    wxPGId mTextureDimensionsId;
    wxPGId mCurrentFrameId;
    wxPGId mFrameTextureNameId;
    wxPGId mBindingTypeId;
    wxPGId mContentTypeId;
    wxPGId mPixelFormatId;
    wxPGId mNumMipMapsId;
    wxPGId mAlphaId;
    wxPGId mTextureCoordSetId;
    wxPGId mTextureTransformId;
    wxPGId mTextureScrollId;
    wxPGId mTextureUScrollId;
    wxPGId mTextureVScrollId;
    wxPGId mTextureUScaleId;
    wxPGId mTextureVScaleId;
    wxPGId mTextureRotateId;
    wxPGId mTextureAddressingModeId;
    wxPGId mTextureBorderColourId;
    wxPGId mColourOperationExId;
    wxPGId mColourOpMultipassFallbackId;
    wxPGId mAlphaOperationId;
    wxPGId mEnvironmentMapId;
    wxPGId mScrollAnimationId;
    wxPGId mRotateAnimationId;
    wxPGId mTransformAnimationId;
    wxPGId mProjectiveTexturingId;
    wxPGId mTextureFilteringId;
    wxPGId mTextureAnisotropyId;
    wxPGId mTextureMipMapBiasId;
    wxPGId mNameId;
    wxPGId mTextureNameAliasId;


	DECLARE_EVENT_TABLE();
};

#endif // _TEXTUREUNITPROPERTYGRIDPAGE_H_
