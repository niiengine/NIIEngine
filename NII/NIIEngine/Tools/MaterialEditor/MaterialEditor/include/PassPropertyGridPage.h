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
#ifndef _PASSPROPERTYGRIDPAGE_H_
#define _PASSPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>

class PassController;

class PassPropertyGridPage : public wxPropertyGridPage
{
public:
	PassPropertyGridPage(PassController* controller);
	virtual ~PassPropertyGridPage();

	virtual void populate();

protected:
	virtual void createGeneralCategory();
	virtual void createReflectanceCategory();
	virtual void createPointCategory();
	virtual void createSceneBlendingCategory();
	virtual void createDepthCategory();
	virtual void createCullingCategory();
	virtual void createIlluminationCategory();
	virtual void createFogCategory();
	virtual void createAlphaRejectionCategory();
	virtual void createMiscCategory();

	virtual void propertyChanged(wxPropertyGridEvent& event);
	
	PassController* mController;

	wxPGId mNameId;

	wxPGId mAmbientId;
	wxPGId mDiffuseId;
	wxPGId mSpecularId;
	wxPGId mShininessId;

	wxPGId mPointSizeId;
	wxPGId mPointSpritesId;
	wxPGId mAttenuationId;
	wxPGId mPointAttenuationId;
	wxPGId mPointMinSizeId;
	wxPGId mPointMaxSizeId;
	wxPGId mPointAttenuationConstantId;
	wxPGId mPointAttenuationLinearId;
	wxPGId mPointAttenuationQuadraticId;

	wxPGId mSceneBlendTypeId;
	wxPGId mSrcSceneBlendTypeId;
	wxPGId mDestSceneBlendTypeId;

	wxPGId mDepthCheckId;
	wxPGId mDepthWriteId;
	wxPGId mDepthFunctionId;
	wxPGId mDepthBiasId;
	wxPGId mDepthBiasConstantId;
	wxPGId mDepthBiasSlopeId;

	wxPGId mManualCullingModeId;

	wxPGId mLightingId;
	wxPGId mMaxLightsId;
	wxPGId mStartLightId;
	wxPGId mIterationId;
	wxPGId mShadingModeId;
	wxPGId mSelfIlluminationId;

	wxPGId mOverrideSceneId;
	wxPGId mFogOverrideId;
	wxPGId mFogModeId;

	wxPGId mAlphaRejectFuncId;
	wxPGId mAlphaRejectValueId;

	wxPGId mColourWriteId;
	wxPGId mPolygonModeId;
	wxPGId mTrackVertexColourTypeId;

	DECLARE_EVENT_TABLE();
};

#endif // _PASSPROPERTYGRIDPAGE_H_