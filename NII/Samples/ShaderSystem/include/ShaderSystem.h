#ifndef __ShaderSystem_H__
#define __ShaderSystem_H__

#include "SdkSample.h"
#include "OgreShaderExLayeredBlending.h"

using namespace Ogre;
using namespace OgreBites;

class ShaderSystemListener;
class ShaderSystemApplication;

// Lighting models.
enum ShaderSystemLightingModel
{
	SSLM_PerVertexLighting,
	SSLM_PerPixelLighting,
	SSLM_NormalMapLightingTangentSpace,
	SSLM_NormalMapLightingObjectSpace
};

// a hack class to get infinite frustum - needed by instanced viewports demo
// a better solution will be to check the frustums of all the viewports in a similer class
class _OgreSampleClassExport InfiniteFrustum : public Frustum
{
public:
	InfiniteFrustum() : Frustum()
	{
		mFrustumPlanes[Frustum::PT_Left].normal = Vector3::INV_X;
		mFrustumPlanes[Frustum::PT_Left].d = 9999999999999999999.0f;
		mFrustumPlanes[Frustum::PT_Right].normal = Vector3::X;
		mFrustumPlanes[Frustum::PT_Right].d = 9999999999999999999.0f;
		mFrustumPlanes[Frustum::PT_Top].normal = Vector3::INV_Y;
		mFrustumPlanes[Frustum::PT_Top].d = 9999999999999999999.0f;
		mFrustumPlanes[Frustum::PT_Bottom].normal = Vector3::Y;
		mFrustumPlanes[Frustum::PT_Bottom].d = 9999999999999999999.0f;
		mFrustumPlanes[Frustum::PT_Near].normal = Vector3::INV_Z;
		mFrustumPlanes[Frustum::PT_Near].d = 9999999999999999999.0f;
		mFrustumPlanes[Frustum::PT_Far].normal = Vector3::Z;
		mFrustumPlanes[Frustum::PT_Far].d = 9999999999999999999.0f;
	}
	virtual bool isIntersect(const AABox & bound) const {return true;};
	virtual bool isIntersect(const Sphere & bound) const {return true;};
	virtual bool isIntersect(const Vector3 & vert) const {return true;};
	bool project(const Sphere & sphere, Rectf & rect) const 
    {rect.mLeft = rect.mBottom = -1.0f; rect.mRight = rect.mTop = 1.0f; return true;};
	NIIf getNearClipDistance(void) const {return 1.0;};
	NIIf getFarClipDistance(void) const {return 9999999999999.0f;};
	const Plane & getClipPlane(PlaneType plane ) const
    {
        return mFrustumPlanes[plane];
    }
};


// Listener class for frame updates
class _OgreSampleClassExport Sample_ShaderSystem : public SdkSample
{
public:
	Sample_ShaderSystem();
	~Sample_ShaderSystem();
		
	virtual void _shutdown();

	/** @see Sample::checkBoxToggled. */
	void checkBoxToggled(CheckBox* box);

	/** @see Sample::itemSelected. */
	void itemSelected(SelectMenu* menu);

	/** @see Sample::buttonHit. */
	virtual void buttonHit(OgreBites::Button* b);

	/** @see Sample::sliderMoved. */
	virtual void sliderMoved(Slider* slider);

	/** @see Sample::getRequiredPlugins. */
	StringList getRequiredPlugins();

	/** @see Sample::testCapabilities. */
	void testCapabilities(const RenderSysFeature* caps);
	
	/** @see Sample::onRenderCore. */
    bool onRenderCore(const RenderFrameArgs * evt);

	void updateTargetObjInfo();

#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
	/** @see Sample::mousePressed. */
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	/** @see Sample::mouseReleased. */
	bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

	/** @see Sample::mouseMoved. */
	bool mouseMoved(const OIS::MouseEvent& evt);
#endif

protected:

	/** Set the current lighting model. */
	void setCurrentLightingModel(ShaderSystemLightingModel lightingModel);

	/** Return the current lighting model. */
	ShaderSystemLightingModel getCurrentLightingMode() const { return mCurLightingModel; }

	/** Set specular enable state. */
	void setSpecularEnable(bool enable);

	/** Return current specular state. */
	bool getSpecularEnable() const { return mSpecularEnable; }

	/** Set reflection map enable state. */
	void setReflectionMapEnable(bool enable);

	/** Return current reflection map state. */
	bool getReflectionMapEnable() const { return mReflectionMapEnable; }

	/** Set fog per pixel enable state. */
	void setPerPixelFogEnable(bool enable);

	/** Set auto border adjustment mode in texture atlasing. */
	void setAtlasBorderMode( bool enable );

	/** Set instanced viewports enable state. */
	void setInstancedViewportsEnable( bool enable );

	/** Create directional light. */
	void createDirectionalLight();

	/** Create point light. */
	void createPointLight();

	/** Create spot light. */
	void createSpotLight();

	/** Toggle adding of lots of models */
	void updateAddLotsOfModels(bool addThem);
    void addModelToScene(const String &  modelName);

	/** Toggle instanced viewports */
	void updateInstancedViewports(bool enabled);

	/** Toggle light visibility. */
	void updateLightState(const String& lightName, bool visible);

	/** Update runtime generated shaders of the target entities in this demo. */
	void updateSystemShaders();

	/** Export a given material including RTSS extended attributes.*/
	void exportRTShaderSystemMaterial(const String& fileName, const String& materialName);

	/** Create shaders based techniques using the given entity based on its sub entities material set. */
	void generateShaders(GeometrySpace* entity);

	/** @see Sample::setupView. */
	virtual void setupView();

	/** @see Sample::setupContent. */
	virtual void setupContent();

	/** Setup the UI for the sample. */
	void setupUI();
	
	/** @see Sample::setupContent. */
	virtual void cleanupContent();

	/** @see Sample::loadResources. */
	void loadResources();

	/** Create private resource group. */
	void createPrivateResourceGroup();
	
	/** @see Sample::unloadResources. */
	void unloadResources();

	void createInstancedViewports();
	void destroyInstancedViewports();
	void destroyInstancedViewportsFactory();

	/** Destroy private resource group. */
	void destroyPrivateResourceGroup();

	/** Pick the target object. */
	void pickTargetObject( const OIS::MouseEvent &evt );

	/** Apply shadow type from the given shadow menu selected index. */
	void applyShadowType(int menuIndex);

	/** Change the current texture layer blend mode. */
	void changeTextureLayerBlendMode();

	/** Update layer blend caption. */
	void updateLayerBlendingCaption( RTShader::LayeredBlending::BlendMode nextBlendMode );

	CustomSpaceObj* createTextureAtlasObject();
	void createMaterialForTexture( const String & texName, bool isTextureAtlasTexture );
	// Types.
protected:
	typedef vector<GeometrySpace*>::type	EntityList;
	typedef EntityList::iterator	EntityListIterator;

	typedef map<String, bool>::type  StringMap;
	typedef StringMap::iterator		 StringMapIterator;

protected:
	EntityList							mTargetEntities;		// Target entities that will use runtime shader generated materials.	
	ShaderSystemLightingModel			mCurLightingModel;		// The current lighting model.
	SelectMenu*							mLightingModelMenu;		// The lighting model menu.
	SelectMenu*							mFogModeMenu;			// The fog mode menu.
	SelectMenu*							mLanguageMenu;			// The shading language menu.
	SelectMenu*							mShadowMenu;			// The shadow type menu.
	bool								mPerPixelFogEnable;		// When true the RTSS will do per pixel fog calculations.
	bool								mSpecularEnable;		// The current specular state.	
	RTShader::SubRenderStateFactory*	mReflectionMapFactory;	// The custom reflection map shader extension factory.
	RTShader::SubRenderState*			mInstancedViewportsSubRenderState;// todo - doc
	bool								mInstancedViewportsEnable;		// todo - doc
	InfiniteFrustum 					mInfiniteFrustum;				// todo - doc
	ParticleCtrl*						mBbsFlare;						// todo - doc
	bool								mAddedLotsOfModels;		        // todo - doc
    vector<GeometrySpace *>::type              mLotsOfModelsEntities;          // todo - doc       
    vector<SpaceNode *>::type           mLotsOfModelsNodes;             // todo - doc  
    int                                 mNumberOfModelsAdded;           // todo - doc   
    RTShader::SubRenderStateFactory *   mInstancedViewportsFactory;     // todo - doc

	RTShader::SubRenderState*			mReflectionMapSubRS;	// The reflection map sub render state.
	RTShader::LayeredBlending*			mLayerBlendSubRS;		// The layer blending sub render state.
	Label*								mLayerBlendLabel;		// The layer blending label.
	Slider*								mReflectionPowerSlider;	// The reflection power controller slider.
	bool								mReflectionMapEnable;	// The current reflection map effect state.
	Slider*								mModifierValueSlider;   // The value of the modifier for the layered blend controller slider.
	GeometrySpace*								mLayeredBlendingEntity; // GeometrySpace used to show layered blending SRS
	SpaceNode*							mPointLightNode;		// Point light scene node.
	SpaceNode*							mDirectionalLightNode;	// Directional light scene node.		
	RaySpaceQuery*						mRayQuery;				// The ray scene query.
	SpaceObj*						mTargetObj;				// The current picked target object.
	Label*								mTargetObjMatName;		// The target object material name label.
	Label*								mTargetObjVS;			// The target object vertex shader label.
	Label*								mTargetObjFS;			// The target object fragment shader label.
	CheckBox*							mDirLightCheckBox;		// The directional light check box.
	CheckBox*							mPointLightCheckBox;	// The point light check box.
	CheckBox*							mSpotLightCheckBox;		// The spot light check box.
	String								mExportMaterialPath;	// The path of the export material.
	CheckBox*							mInstancedViewportsCheckBox; // The instanced viewports check box.
	CheckBox*							mAddLotsOfModels; // The "add lots of models" check box.				
    BoxSky * mBoxSky;
};

#endif
