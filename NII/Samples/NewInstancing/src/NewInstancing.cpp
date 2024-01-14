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

#include "SamplePlugin.h"
#include "NewInstancing.h"

using namespace Ogre;
using namespace OgreBites;

#ifndef OGRE_STATIC_LIB

SamplePlugin * sp;
Sample * s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_NewInstancing;
	sp = N_new SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp);
	N_delete sp;
	delete s;
}

#endif

static const char *c_instancingTechniques[] =
{
	"Shader Based",
	"Vertex Texture Fetch (VTF)",
	"Hardware Instancing Basic",
	"Hardware Instancing + VTF",
	"Limited Animation - Hardware Instancing + VTF",
	"No Instancing"
};

static const char *c_materialsTechniques[] =
{
	"Examples/Instancing/ShaderBased/Robot",
	"Examples/Instancing/VTF/Robot",
	"Examples/Instancing/HWBasic/Robot",
	"Examples/Instancing/VTF/HW/Robot",
	"Examples/Instancing/VTF/HW/LUT/Robot",
	"Examples/Instancing/ShaderBased/Robot"
};

static const char *c_materialsTechniques_dq[] =
{
	"Examples/Instancing/ShaderBased/Robot_dq",
	"Examples/Instancing/VTF/Robot_dq",
	"Examples/Instancing/HWBasic/Robot",
	"Examples/Instancing/VTF/HW/Robot_dq",
	"Examples/Instancing/VTF/HW/LUT/Robot_dq",
	"Examples/Instancing/ShaderBased/Robot_dq"
};

static const char *c_materialsTechniques_dq_two_weights[] =
{
	"Examples/Instancing/ShaderBased/spine_dq_two_weights",
	"Examples/Instancing/VTF/spine_dq_two_weights",
	"Examples/Instancing/HWBasic/spine",
	"Examples/Instancing/VTF/HW/spine_dq_two_weights",
	"Examples/Instancing/VTF/HW/LUT/spine_dq_two_weights",
	"Examples/Instancing/ShaderBased/spine_dq_two_weights"
};

static const char *c_meshNames[] =
{
	"robot.mesh",
	"spine.mesh"
};
//------------------------------------------------------------------------------
Sample_NewInstancing::Sample_NewInstancing() : NUM_INST_ROW(50), NUM_INST_COLUMN(50), mCurrentManager(0), mCurrentMaterialSet(c_materialsTechniques), mCurrentFlags(0), mSkinningTechniques(NULL)
{
	mInfo["Title"] = "New Instancing";
	mInfo["Description"] = "Demonstrates how to use the new InstancedManager to setup many dynamic"
		" instances of the same mesh with much less performance impact";
	mInfo["Thumbnail"] = "thumb_newinstancing.png";
	mInfo["Category"] = "Environment";
	mInfo["Help"] = "Press Space to switch Instancing Techniques.\n"
		"Press B to toggle bounding boxes.\n\n"
		"Changes in the slider take effect after switching instancing technique\n"
		"Different batch sizes give different results depending on CPU culling"
		" and instance numbers on the scene.\n\n"
		"If performance is too slow, try defragmenting batches once in a while";
    mBoxSky = 0;
}
//------------------------------------------------------------------------------
~Sample_NewInstancing()
{
    if(mBoxSky)
    {
        N_delete mBoxSky;
        mSceneMgr->removeSky(mBoxSky);
        mBoxSky = 0;
    }
}
//------------------------------------------------------------------------------
bool Sample_NewInstancing::onRenderCore(const RenderFrameArgs * evt)
{
	if(mAnimateInstances->isChecked())
		animateUnits(evt->mCurrent);

	if(mMoveInstances->isChecked())
		moveUnits(evt->mCurrent);

	return SdkSample::onRenderCore(evt);        // don't forget the parent class updates!
}
//------------------------------------------------------------------------------
bool Sample_NewInstancing::keyPressed(const OIS::KeyEvent & evt)
{
	//Toggle bounding boxes with B key unless the help dialog is visible
	if(evt.key == OIS::KC_B && !mTrayMgr->isDialogVisible() && mCurrentManager)
	{
		bool oldShow = mCurrentManager->getSetting(InstanceMesh::SHOW_BOUNDINGBOX,
			mCurrentMaterialSet[mInstancingTechnique]);
		mCurrentManager->setSetting( InstanceMesh::SHOW_BOUNDINGBOX, !oldShow);
	}

	//Switch to next instancing technique with space bar
	if(evt.key == OIS::KC_SPACE && !mTrayMgr->isDialogVisible())
		mTechniqueMenu->selectItem((mTechniqueMenu->getSelectionIndex() + 1) % (NUM_TECHNIQUES+1));

	return SdkSample::keyPressed(evt);
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::setupContent()
{
	//Initialize the techniques and current mesh variables
	mInstancingTechnique = 0;
	mCurrentMesh = 0;
	mCurrentManager = 0;

	checkHardwareSupport();

	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	mSceneMgr->getRenderPattern()->setTexture(0, 2048, 2048, PF_R32);
	mSceneMgr->setSelfShadow(true);
	mSceneMgr->setBackFaceInCast(true);

	//LiSPSMShadowSetup *shadowCameraSetup = new LiSPSMShadowSetup();
	FocusedShadowSetup * shadowCameraSetup = N_new FocusedShadowSetup();
	//PlaneOptimalShadowGenCamera *shadowCameraSetup = new PlaneOptimalShadowGenCamera();

	mSceneMgr->setShadowGenCamera(shadowCameraSetup);

	mEntities.reserve(NUM_INST_ROW * NUM_INST_COLUMN);
	mSceneNodes.reserve(NUM_INST_ROW * NUM_INST_COLUMN);
    mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial("Examples/CloudyNoonSkyBox");
    mBoxSky->setRenderGroup(RQG_Sky);
	mBoxSky->buildGeo();
    mSceneMgr->addSky(mBoxSky);
	// create a mesh for our ground
	MeshManager::getSingleton().createPlane("ground", GroupDefault,
		Plane(Vector3::Y, 0), 10000, 10000, 20, 20, true, 1, 6, 6, Vector3::Z);

	// create a ground entity from our mesh and attach it to the origin
	GeometrySpace * ground = mSceneMgr->createGeo("Ground", "ground");
	ground->setMaterialName("Examples/Instancing/Misc/Grass");
	ground->setCastShadow(false);
	mSceneMgr->getOriginNode()->attachObject(ground);

	setupLighting();

	// set initial camera position and speed
	mCamera->setPos(NII::Vector3f(0, 120, 100));

	setupGUI();

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
	setDragLook(true);
#endif

	switchInstancingTechnique();
}
//------------------------------------------------------------------------------
void Sample_NewInstancing::setupLighting()
{
	mSceneMgr->setAmbient(Colour(0.40f, 0.40f, 0.40f));

	Colour lightColour(1, 0.5, 0.3);
    Vector3f norl;
    (Vector3::UNIT * -1.0f).normalise(norl);
	//Create main (point) light
	Light * light = mSceneMgr->createLight(0);
	light->setDiffuse(lightColour);
	light->setPos( 0.0f, 25.0f, 0.0f );
	light->setSpecular( 0.6, 0.82, 1.0 );
    light->setRange(3500);
    light->setAttenuationConstant(0.085);
    light->setAttenuationLinear(0.00008);
    light->setAttenuationQuadric(0.00006);
	light->setCastShadow( false );

	//Create a dummy spot light for shadows
	light = mSceneMgr->createLight(0);
	light->setType(LT_SPOT);
	light->setDiffuse(Colour(0.15f, 0.35f, 0.44f));
	light->setPos(250.0f, 200.0f, 250.0f);
	light->setDirection(norl);
	light->setSpecular(0.2, 0.12, 0.11);
    light->setRange(3500);
    light->setAttenuationConstant(0.005);
    light->setAttenuationLinear(0.00002);
    light->setAttenuationQuadric(0.00001);
    light->setSpotlightInner(Angle(80));
    light->setSpotlightOuter(Angle(90));
    light->setSpotlightFalloff(1.0);
	light->setCastShadow( true );
	light->setLightMask( 0x00000000 );
}
//------------------------------------------------------------------------------
void Sample_NewInstancing::switchInstancingTechnique()
{
	randGenerator.randomize();
	//mInstancingTechnique = (mInstancingTechnique+1) % (NUM_TECHNIQUES+1);
	mInstancingTechnique = mTechniqueMenu->getSelectionIndex();

	if( mCurrentManager )
		mSceneMgr->destroyInstanceMesh(mCurrentManager->getID());

	if( !mSupportedTechniques[mInstancingTechnique] )
	{
		//Hide GUI features available only to instancing
		mCurrentManager = 0;
		mDefragmentBatches->hide();
		mDefragmentOptimumCull->hide();
		return;
	}

	if(mInstancingTechnique < NUM_TECHNIQUES)
	{
		//Instancing

		//Create the manager if we haven't already (i.e. first time)
		//Because we use IM_USEALL as flags, the actual num of instances per batch might be much lower
		//If you're not bandwidth limited, you may want to lift IM_VTFBESTFIT flag away

		InstanceMeshType technique = InstanceMesh::ShaderBased;

		switch( mInstancingTechnique )
		{
		case 0: technique = InstanceMesh::ShaderBased; break;
		case 1: technique = InstanceMesh::TextureVTF; break;
		case 2: technique = InstanceMesh::HWInstancingBasic; break;
		case 3:
		case 4: technique = InstanceMesh::HWInstancingVTF; break;
		}

		uint16 flags = IM_USEALL;
		flags |= mCurrentFlags;

		if (mInstancingTechnique == 4)
		{
			flags |= IM_VTFBONEMATRIXLOOKUP;
		}
		//Only one weight is recommended for the VTF technique, but force the use of more for the demo
		if(mInstancingTechnique == 1 && (flags & IM_USEBONEDUALQUATERNIONS))
		{
			flags &= ~IM_USEONEWEIGHT;
		}

		mCurrentManager = N_Engine().createInstanceMesh(mSceneMgr,
			"InstanceMgr" + StrConv::conv(mInstancingTechnique), c_meshNames[mCurrentMesh],
			ResourceSchemeManager::GroupUnknow, 0, NUM_INST_ROW * NUM_INST_COLUMN, technique, flags);

		createInstancedEntities();

		//Show GUI features available only to instancing
		mDefragmentBatches->show();
		mDefragmentOptimumCull->show();
	}
	else
	{
		//Non-instancing
		createEntities();

		//Hide GUI features available only to instancing
		mCurrentManager = 0;
		mDefragmentBatches->hide();
		mDefragmentOptimumCull->hide();
	}

	createSceneNodes();

	//Show/hide "static" button, and restore config. Do this _after_ createSceneNodes()
	if( mInstancingTechnique == InstanceMesh::HWInstancingBasic ||
		mInstancingTechnique == InstanceMesh::HWInstancingVTF ||
		mInstancingTechnique == InstanceMesh::HWInstancingVTF + 1) // instancing with lookup
	{
		if( mSetStatic->isChecked() )
			mCurrentManager->setBatchesAsStaticAndUpdate( mSetStatic->isChecked() );
		mSetStatic->show();
	}
	else
		mSetStatic->hide();
	if( mInstancingTechnique < NUM_TECHNIQUES)
	{
		mUseSceneNodes->show();
	}
	else
	{
		mUseSceneNodes->hide();
	}
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::switchSkinningTechnique(int index)
{
	switch(index)
	{
		default:
		//Linear Skinning
		case 0:
			mCurrentMesh = 0;
			mCurrentMaterialSet = c_materialsTechniques;
			mCurrentFlags = 0;
			break;
		//Dual Quaternion Skinning
		case 1:
			mCurrentMesh = 0;
			mCurrentMaterialSet = c_materialsTechniques_dq;
			mCurrentFlags = IM_USEBONEDUALQUATERNIONS;
			break;
		//Dual Quaternion Skinning with two weights
		case 2:
			mCurrentMesh = 1;
			mCurrentMaterialSet = c_materialsTechniques_dq_two_weights;
			mCurrentFlags = IM_USEBONEDUALQUATERNIONS;
			break;
	};
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::createEntities()
{
	for(int i=0; i<NUM_INST_ROW * NUM_INST_COLUMN; ++i)
	{
		//Create the non-instanced entity. Use the same shader as shader-based because:
		//a. To prove we can (runs without modification! :-) )
		//b. Make a fair comparison
		GeometrySpace * ent = mSceneMgr->createGeo(0, c_meshNames[mCurrentMesh]);
		ent->setMaterialName(mCurrentMaterialSet[NUM_TECHNIQUES]);
		mEntities.push_back(ent);

		//Get the animation
		AnimationFusion *anim = ent->getFusion("Walk");
		if(mAnimations.insert(anim).second)
		{
			anim->setEnabled(true);
			anim->addTime(randGenerator.nextFloat() * 10); //Random start offset
		}
	}
}
//------------------------------------------------------------------------------
void Sample_NewInstancing::createInstancedEntities()
{
	for(int i = 0; i<NUM_INST_ROW; ++i)
	{
		for(int j = 0; j<NUM_INST_COLUMN; ++j)
		{
			//Create the instanced entity
            InstanceGeo * ent = N_Engine().createInstanceGeo(mCurrentManager, mCurrentMaterialSet[mInstancingTechnique]);
                mEntities.push_back(ent);

			//HWInstancingBasic is the only technique without animation support
			if(mInstancingTechnique != InstanceMesh::HWInstancingBasic)
			{
				//Get the animation
				AnimationFusion * anim = ent->getFusion("Walk");
				anim->setEnabled(true);
				anim->addTime(randGenerator.nextFloat() * 10); //Random start offset
				mAnimations.insert(anim);
			}

			if((mInstancingTechnique < NUM_TECHNIQUES) && (!mUseSceneNodes->isChecked()))
			{
                Quaternion orient;
                orient = from(Radian(randGenerator.nextFloat() * 10 * 3.14159265359f), Vector3::Y)
				mMovedInstances.push_back(ent);
				ent->setOri(orient);
				ent->setPos(NII::Vector3(mEntities[0]->getCenterRange() * (i - NUM_INST_ROW * 0.5f), 0,
					mEntities[0]->getCenterRange() * (j - NUM_INST_COLUMN * 0.5f)) );
			}
		}
	}
}
//------------------------------------------------------------------------------
void Sample_NewInstancing::createSceneNodes()
{
	//Here the SceneNodes are created. Since InstancedEntities derive from SpaceObj,
	//they behave like regular Entities on this.
	SpaceNode * rootNode = mSceneMgr->getOriginNode();

	for(int i = 0; i<NUM_INST_ROW; ++i)
	{
		for(int j = 0; j<NUM_INST_COLUMN; ++j)
		{
			int idx = i * NUM_INST_COLUMN + j;
			if((mInstancingTechnique >= NUM_TECHNIQUES) || (mUseSceneNodes->isChecked()))
			{
				SpaceNode *sceneNode = rootNode->create();
				sceneNode->attachObject( mEntities[idx] );
				sceneNode->yaw( Radian( randGenerator.nextFloat() * 10 * 3.14159265359f )); //Random orientation
				sceneNode->setPos(Vector3f(mEntities[idx]->getCenterRange() * (i - NUM_INST_ROW * 0.5f), 0,
					mEntities[idx]->getCenterRange() * (j - NUM_INST_COLUMN * 0.5f) ));
				mSceneNodes.push_back( sceneNode );
			}

		}
	}
}
//------------------------------------------------------------------------------
void Sample_NewInstancing::clearScene()
{
	std::vector<SpaceObj*>::const_iterator itor = mEntities.begin();
	std::vector<SpaceObj*>::const_iterator end  = mEntities.end();

	//Note: Destroying the instance manager automatically destroys all instanced entities
	//created by this manager (beware of not leaving reference to those pointers)
	while( itor != end )
	{
		SpaceNode *sceneNode = (*itor)->getSceneNode();
		if (sceneNode)
		{
			sceneNode->detachAll();
			static_cast<SpaceNode *>(sceneNode->getParent())->destroy( sceneNode->getID() );
		}
		if( mInstancingTechnique == NUM_TECHNIQUES )
			mSceneMgr->destroyGeo( (*itor)->getID() );
		else
			mSceneMgr->destroyInstanceGeo( static_cast<InstanceGeo*>(*itor) );

		++itor;
	}

	//Free some memory, but don't destroy the manager so when we switch this technique
	//back again it doesn't take too long
	if( mCurrentManager )
		mCurrentManager->cleanupEmptyBatches();

	mEntities.clear();
	mMovedInstances.clear();
	mSceneNodes.clear();
	mAnimations.clear();
}
//-----------------------------------------------------------------------------------
void Sample_NewInstancing::destroyManagers()
{
	mSceneMgr->destroyInstanceMesh(mCurrentManager->getID());;
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::cleanupContent()
{
    MeshManager::getSingleton().remove("ground");
	clearScene();
	destroyManagers();
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::animateUnits( float timeSinceLast )
{
	//Iterates through all AnimationSets and updates the animation being played. Demonstrates the
	//animation is unique and independent to each instance
	std::set<AnimationFusion*>::const_iterator itor = mAnimations.begin();
	std::set<AnimationFusion*>::const_iterator end  = mAnimations.end();

	while( itor != end )
	{
		(*itor)->addTime( timeSinceLast );
		++itor;
	}
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::moveUnits( float timeSinceLast )
{
	NIIf fMovSpeed = 1.0f;

	if( !mEntities.empty() )
		fMovSpeed = mEntities[0]->getCenterRange() * 0.30f;

	if (!mSceneNodes.empty())
	{
		//Randomly move the units along their normal, bouncing around invisible walls
		std::vector<SpaceNode*>::const_iterator itor = mSceneNodes.begin();
		std::vector<SpaceNode*>::const_iterator end  = mSceneNodes.end();

		while( itor != end )
		{
			//Calculate bounces
			Vector3 entityPos = (*itor)->getPos();
			Vector3 planeNormal = Vector3::ZERO;
			if( (*itor)->getPos().x < -5000.0f )
			{
				planeNormal = Vector3::X;
				entityPos.x = -4999.0f;
			}
			else if( (*itor)->getPos().x > 5000.0f )
			{
				planeNormal = Vector3::INV_X;
				entityPos.x = 4999.0f;
			}
			else if( (*itor)->getPos().z < -5000.0f )
			{
				planeNormal = Vector3::Z;
				entityPos.z = -4999.0f;
			}
			else if( (*itor)->getPos().z > 5000.0f )
			{
				planeNormal = Vector3::INV_Z;
				entityPos.z = 4999.0f;
			}

			if( planeNormal != Vector3::ZERO )
			{
				Vector3 vDir, pnorl;
                (*itor)->getOri().getXAxis().normalise(vDir);
                planeNormal.reflect(vDir).normalise(pnorl);
				(*itor)->setOri(lookAt(pnorl));
				(*itor)->setPos(entityPos);
			}

			//Move along the direction we're looking to
			(*itor)->translate(Vector3::X * timeSinceLast * fMovSpeed, PosNode::SM_Local );
			++itor;
		}
	}
	else
	{
		//No scene nodes (instanced entities only)
		//Update instanced entities directly

		//Randomly move the units along their normal, bouncing around invisible walls
		std::vector<InstanceGeo*>::const_iterator itor = mMovedInstances.begin();
		std::vector<InstanceGeo*>::const_iterator end  = mMovedInstances.end();

		while( itor != end )
		{
			//Calculate bounces
			InstanceGeo* pEnt = *itor;
			Vector3 entityPos = pEnt->getPos();
			Vector3 planeNormal = Vector3::ZERO;
			if( pEnt->getPos().x < -5000.0f )
			{
				planeNormal = Vector3::X;
				entityPos.x = -4999.0f;
			}
			else if( pEnt->getPos().x > 5000.0f )
			{
				planeNormal = Vector3::INV_X;
				entityPos.x = 4999.0f;
			}
			else if( pEnt->getPos().z < -5000.0f )
			{
				planeNormal = Vector3::Z;
				entityPos.z = -4999.0f;
			}
			else if( pEnt->getPos().z > 5000.0f )
			{
				planeNormal = Vector3::INV_Z;
				entityPos.z = 4999.0f;
			}

			if( planeNormal != Vector3::ZERO )
			{
				Vector3 vDir, pnorl;
                pEnt->getOri().getXAxis().normalise(vDir);
                planeNormal.reflect(vDir).normalise(pnorl);
				pEnt->setOri(lookAt(pnorl), false);
				pEnt->setPos(entityPos, false);
			}

			//Move along the direction we're looking to
			Vector3 transAmount = Vector3::X * timeSinceLast * fMovSpeed;
			pEnt->setPos( pEnt->getPos() + pEnt->getOri() * transAmount );
			++itor;
		}
	}
}

//------------------------------------------------------------------------------
Quaternion Sample_NewInstancing::lookAt(const Vector3 & normDir)
{
	Quaternion retVal;
	Vector3 xVec = Vector3::Y.crossProduct(normDir);
	xVec.normalise();

	Vector3 yVec = normDir.crossProduct(xVec);
	yVec.normalise();

	retVal.from(xVec, yVec, normDir);

	return retVal;
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::defragmentBatches()
{
	//Defragment batches is used after many InstancedEntities were removed (and you won't
	//be requesting more). However, then the optimize cull option is on, it can cause
	//quite a perf. boost on large batches (i.e. VTF) even if not a single instance was ever removed.
	if( mCurrentManager )
		mCurrentManager->defragmentBatches( mDefragmentOptimumCull->isChecked() );
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::setupGUI()
{
	mTechniqueMenu = mTrayMgr->createLongSelectMenu(
		TL_TOPLEFT, "TechniqueSelectMenu", "Technique", 450, 350, 5);
	for( int i=0; i<NUM_TECHNIQUES+1; ++i )
	{
		String text = c_instancingTechniques[i];
		if( !mSupportedTechniques[i] )
			text = "Unsupported: " + text;
		mTechniqueMenu->addItem( text );
	}
	//Check box to enable dual quaternion skinning
	mSkinningTechniques = mTrayMgr->createLongSelectMenu(TL_TOPLEFT, "SkinningTechnique", "Skinning Technique", 450, 285, 5);
	mSkinningTechniques->addItem("Linear Skinning");
	mSkinningTechniques->addItem("Dual Quaternion Skinning");
	mSkinningTechniques->addItem("Dual Quaternion Skinning (2 wgts)");

	//Check box to move the units
	mMoveInstances = mTrayMgr->createCheckBox(TL_TOPRIGHT, "MoveInstances", "Move Instances", 175);
	mMoveInstances->setChecked(false);

	//Check box to animate the units
	mAnimateInstances = mTrayMgr->createCheckBox(TL_TOPRIGHT, "AnimateInstances",
		"Animate Instances", 175);
	mAnimateInstances->setChecked(false);

	//Checkbox to toggle shadows
	mEnableShadows = mTrayMgr->createCheckBox(TL_TOPRIGHT, "EnableShadows",
		"Enable Shadows", 175);
	mEnableShadows->setChecked(true);

	//Check box to make instances static (where supported)
	mSetStatic = mTrayMgr->createCheckBox(TL_TOPRIGHT, "SetStatic", "Set Static", 175);
	mSetStatic->setChecked(false);

	//Checkbox to toggle use of scene nodes
	mUseSceneNodes = mTrayMgr->createCheckBox(TL_TOPRIGHT, "UseSceneNodes",
		"Use Scene Nodes", 175);
	mUseSceneNodes->setChecked(true, false);

	//Controls to control batch defragmentation on the fly
	mDefragmentBatches =  mTrayMgr->createButton(TL_RIGHT, "DefragmentBatches",
		"Defragment Batches", 175);
	mDefragmentOptimumCull = mTrayMgr->createCheckBox(TL_RIGHT, "DefragmentOptimumCull",
		"Optimum Cull", 175);
	mDefragmentOptimumCull->setChecked(true);

	//Slider to control max number of instances
	mInstancesSlider = mTrayMgr->createThickSlider( TL_TOPLEFT, "InstancesSlider", "Instances (NxN)",
		300, 50, 4, 100, 97 );
	mInstancesSlider->setValue( NUM_INST_ROW );

	mTrayMgr->showCursor();
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::itemSelected( SelectMenu* menu )
{
	if (menu == mTechniqueMenu)
	{
		clearScene();
		switchInstancingTechnique();
	}
	else if(menu == mSkinningTechniques)
	{
		clearScene();
		switchSkinningTechnique(menu->getSelectionIndex());
		switchInstancingTechnique();
	}
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::buttonHit( OgreBites::Button* button )
{
	if( button == mDefragmentBatches ) defragmentBatches();
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::checkBoxToggled( CheckBox* box )
{
	if( box == mEnableShadows )
	{
		mSceneMgr->setShadowTechnique( mEnableShadows->isChecked() ?
			SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED : SHADOWTYPE_NONE );
	}
	else if( box == mSetStatic && mCurrentManager )
	{
		mCurrentManager->setBatchesAsStaticAndUpdate( mSetStatic->isChecked() );
	}
	else if (box == mUseSceneNodes)
	{
		clearScene();
		switchInstancingTechnique();
	}
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::sliderMoved( Slider* slider )
{
	if( slider == mInstancesSlider ) NUM_INST_ROW = static_cast<int>(mInstancesSlider->getValue());
	NUM_INST_COLUMN = static_cast<int>(mInstancesSlider->getValue());
}

//------------------------------------------------------------------------------
void Sample_NewInstancing::testCapabilities( const RenderSysFeature* caps )
{
	if (!caps->hasCapability(GF_Program_Vertex) || !caps->hasCapability(GF_Program_Fragment))
	{
		N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and "
			"fragment programs, so you cannot run this sample. Sorry!");
	}

	if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("fp40") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0") )
	{
        N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support the shader model needed for this sample, "
                    "so you cannot run this sample. Sorry!", "NewInstancing::testCapabilities");
	}
}
//------------------------------------------------------------------------------

void Sample_NewInstancing::checkHardwareSupport()
{
	//Check Technique support
	for(int i = 0; i<NUM_TECHNIQUES; ++i)
	{
		InstanceMeshType technique;
		switch( i )
		{
		case 0: technique = InstanceMesh::ShaderBased; break;
		case 1: technique = InstanceMesh::TextureVTF; break;
		case 2: technique = InstanceMesh::HWInstancingBasic; break;
		case 3:
		case 4: technique = InstanceMesh::HWInstancingVTF; break;
		}

		uint16 flags = IM_USEALL;
		if(i == 4)
		{
			flags |= IM_VTFBONEMATRIXLOOKUP;
		}

        InstanceMesh tmpMgr("TmpInstanceManager", c_meshNames[mCurrentMesh], GroupUnknow,
            technique, flags, NUM_INST_ROW * NUM_INST_COLUMN, 0);

        size_t numInstances = tmpMgr.getMaxOrBestNumInstancesPerBatch(mCurrentMaterialSet[i], numInstancesPerBatch, flags);

		mSupportedTechniques[i] = numInstances > 0;
	}

	//Non instancing is always supported
	mSupportedTechniques[NUM_TECHNIQUES] = true;
}
