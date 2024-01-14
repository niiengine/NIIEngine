#ifndef __NewInstancing_H__
#define __NewInstancing_H__

#include "SdkSample.h"
#include "OgreInstanceGeo.h"
#include "MersenneTwister.h"

using namespace Ogre;
using namespace OgreBites;

#define NUM_TECHNIQUES (((int)InstanceMesh::InstancingTechniquesCount) + 1)

class _OgreSampleClassExport Sample_NewInstancing : public SdkSample
{
public:

	Sample_NewInstancing();

	bool onRenderCore(const RenderFrameArgs * evt);

	bool keyPressed(const OIS::KeyEvent& evt);
protected:
	void setupContent();

	void setupLighting();
	
	void switchInstancingTechnique();

	void switchSkinningTechnique(int index);

	void createEntities();

	void createInstancedEntities();

	void createSceneNodes();
	
	void clearScene();

	void destroyManagers();

	void cleanupContent();

	void animateUnits( float timeSinceLast );

	void moveUnits( float timeSinceLast );

	//Helper function to look towards normDir, where this vector is normalized, with fixed Yaw
	Quaternion lookAt(const Vector3 & normDir);

	void defragmentBatches();

	void setupGUI();

	void itemSelected(SelectMenu* menu);

	void buttonHit( OgreBites::Button* button );

	void checkBoxToggled(CheckBox* box);

	void sliderMoved(Slider* slider);

	void testCapabilities(const RenderSysFeature* caps);

	//The difference between testCapabilities() is that features checked here aren't fatal errors.
	//which means the sample can run (with limited functionality) on those computers
	void checkHardwareSupport();

	//You can also use a union type to switch between GeometrySpace and InstanceGeo almost flawlessly:
	/*
	union FusionEntity
	{
		GeometrySpace			entity
		InstanceGeo	instancedEntity;
	};
	*/
	int NUM_INST_ROW;
	int NUM_INST_COLUMN;
	int								mInstancingTechnique;
	int								mCurrentMesh;
	std::vector<SpaceObj*>		    mEntities;
	std::vector<InstanceGeo*>	    mMovedInstances;
	std::vector<SpaceNode*>			mSceneNodes;
	std::set<AnimationFusion*>		mAnimations;
	InstanceMesh					*mCurrentManager;
	bool							mSupportedTechniques[NUM_TECHNIQUES+1];
	const char**						mCurrentMaterialSet;
	uint16 							mCurrentFlags;

	SelectMenu						*mTechniqueMenu;
	CheckBox						*mMoveInstances;
	CheckBox						*mAnimateInstances;
	SelectMenu						*mSkinningTechniques;
	CheckBox						*mEnableShadows;
	CheckBox						*mSetStatic;
	CheckBox						*mUseSceneNodes;
	OgreBites::Button					*mDefragmentBatches;
	CheckBox						*mDefragmentOptimumCull;
	Slider							*mInstancesSlider;
    BoxSky * mBoxSky;
	MersenneTwister randGenerator;
};

#endif
