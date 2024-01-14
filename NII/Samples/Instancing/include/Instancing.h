/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/

 Copyright (c) 2000-2012 Torus Knot Software Ltd
 Also see acknowledgements in Readme.html

 You may use this sample code for anything you like, it is not covered by the
 same license as the rest of the engine.
 -----------------------------------------------------------------------------
*/

#include "OgreConfigFile.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "NiiInstanceSpace.h"
#include "SdkSample.h"
#include "SamplePlugin.h"

using namespace Ogre;
using namespace OgreBites;

#define maxObjectsPerBatch 80
#ifndef FLT_MAX
#  define FLT_MAX         3.402823466e+38F        /* max value */
#endif

const size_t numTypeMeshes = 4;

Ogre::String meshes[]=
{
	"razor", //0
	"knot",
	"tudorhouse",
	"WoodPallet"//6
};

enum CurrentGeomOpt{
	INSTANCE_OPT,
	STATIC_OPT,
	ENTITY_OPT
};

class _OgreSampleClassExport  Sample_Instancing : public SdkSample
{
public:

	//-----------------------------------------------------------------------
	Sample_Instancing()
	{
		mInfo["Title"] = "Instancing";
		mInfo["Description"] = "A demo of different methods to handle a large number of objects.";
		mInfo["Thumbnail"] = "thumb_instancing.png";
		mInfo["Category"] = "Geometry";
        mBoxSky = 0;
	}
    ~Sample_Instancing()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
	//-----------------------------------------------------------------------
	bool onRenderCore(const RenderFrameArgs * evt)
	{
		burnCPU();
		return SdkSample::onRenderCore(evt);
	}

protected:

	void setObjectCount(size_t val)
	{
		mNumMeshes=val;
	};
	//-----------------------------------------------------------------------
	void setBurnedTime(double timeBurned)
	{
		mBurnAmount=timeBurned;
	};
	//-----------------------------------------------------------------------
	void changeSelectedMesh(size_t number)
	{
		mSelectedMesh=number;
	}

	//-----------------------------------------------------------------------
	void burnCPU()
	{
		double mStartTime = mTimer->getUS() / 1000000.0f; //convert into seconds
		double mCurTime =  mStartTime;
		double mStopTime = mLastTime + mBurnAmount;

		while(mCurTime < mStopTime)
		{
			mCurTime = mTimer->getUS() / 1000000.0f; //convert into seconds
		}

		mLastTime = mTimer->getUS() / 1000000.0f; //convert into seconds
	}
	//-----------------------------------------------------------------------
	void destroyCurrentGeomOpt()
	{
		switch(mCurrentGeomOpt)
		{
		case INSTANCE_OPT: destroyInstanceGeom(); break;
		case STATIC_OPT: destroyStaticGeom(); break;
		case ENTITY_OPT: destroyEntityGeom(); break;
		}

		assert (mNumRendered == posMatrices.size ());
		for (size_t i = 0; i < mNumRendered; i++)
		{
			delete [] posMatrices[i];
		}

		posMatrices.clear();
	}
	//-----------------------------------------------------------------------
	void createCurrentGeomOpt()
	{
		objectCount = mNumMeshes;
		mNumRendered = 1;

		while(objectCount>maxObjectsPerBatch)
		{
			mNumRendered++;
			objectCount-=maxObjectsPerBatch;
		}

		assert(mSelectedMesh < numTypeMeshes);
		MeshPtr m = MeshManager::getSingleton ().getByName (meshes[mSelectedMesh] + ".mesh");
		if (m.isNull ())
		{
			m = N_Only(Mesh).load(meshes[mSelectedMesh] + ".mesh", GroupUnknow);
		}
		const NIIf radius = m->getCenterRange();

		// could/should print on screen mesh name,
		//optimisation type,
		//mesh vertices num,
		//32 bit or not,
		//etC..

		posMatrices.resize (mNumRendered);
		posMatrices.reserve (mNumRendered);

		vector <Vector3 *>::type posMatCurr;
		posMatCurr.resize (mNumRendered);
		posMatCurr.reserve (mNumRendered);
		for (size_t i = 0; i < mNumRendered; i++)
		{
			posMatrices[i] = new Vector3[mNumMeshes];
			posMatCurr[i] = posMatrices[i];
		}

		size_t i = 0, j = 0;
		for (size_t p = 0; p < mNumMeshes; p++)
		{
			for (size_t k = 0; k < mNumRendered; k++)
			{
				posMatCurr[k]->x = radius*i;
				posMatCurr[k]->y = k*radius;

				posMatCurr[k]->z = radius*j;
				posMatCurr[k]++;
			}
			if (++j== 10)
			{
				j = 0;
				i++;
			}
		}
		posMatCurr.clear ();

		switch(mCurrentGeomOpt)
		{
		case INSTANCE_OPT:createInstanceGeom();break;
		case STATIC_OPT:createStaticGeom ();break;
		case ENTITY_OPT: createEntityGeom ();break;
		}
	}
	//-----------------------------------------------------------------------
	void createInstanceGeom()
	{
		if (Root::getSingleton ().getRender ()->getFeature()->hasCapability (GF_Program_Vertex) == false)
		{
			N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Your video card doesn't support batching", "Demo_Instance::createScene");
		}

		GeometrySpace *ent = mSceneMgr->createGeo(meshes[mSelectedMesh], meshes[mSelectedMesh] + ".mesh");


		renderInstance.reserve(mNumRendered);
		renderInstance.resize(mNumRendered);

		//Load a mesh to read data from.
		InstanceSpace* batch = mSceneMgr->createInstanceSpace(meshes[mSelectedMesh] + "s" );
		batch->setCastShadow(true);

		batch->setBatchInstanceDimensions (Vector3(1000000, 1000000, 1000000));
		const size_t batchSize = (mNumMeshes > maxObjectsPerBatch) ? maxObjectsPerBatch :mNumMeshes;
		setupInstancedMaterialToEntity(ent);
		for(size_t i = 0; i < batchSize ; i++)
		{
			batch->addEntity(ent, Vector3::ZERO);
		}
		batch->setOrigin(Vector3::ZERO);

		batch->build();

		for (size_t k = 0; k < mNumRendered-1; k++)
		{
			batch->addBatchInstance();
		}
		InstanceSpace::BatchInstanceIterator regIt = batch->getBatchInstanceIterator();
		size_t k = 0;
		while (regIt.isNextValid ())
		{
			InstanceSpace::BatchInstance * r = regIt.getAndNext();

			InstanceSpace::BatchInstance::ObjectList bit = r->getObjectList();
			int j = 0;
			while(bit.isNextValid())
			{
				InstanceSpace::InstancedObject * obj = bit.getAndNext();

				const Vector3 position (posMatrices[k][j]);
				obj->setPos(position);
				++j;

			}
			k++;
		}
		batch->setVisible(true);
		renderInstance[0] = batch;

		mSceneMgr->destroyGeo (ent->getID());
	}

	void setupInstancedMaterialToEntity(GeometrySpace * ent)
	{
		for(Ogre::uint i = 0; i < ent->getNumSubEntities(); ++i)
		{
			SubEntity * se = ent->getCom(i);
			ResourceID materialName= se->getMaterialID();
			se->setMaterialName(buildInstancedMaterial(materialName));
		}
	}

	String buildInstancedMaterial(const String &originalMaterialName)
	{
		// already instanced ?
		if(StringUtil::endsWith (originalMaterialName, "/instanced"))
			return originalMaterialName;

		Material * originalMaterial = N_Only(Material).getByName(originalMaterialName);

		// if originalMat doesn't exists use "Instancing" material name
		const String instancedMaterialName (originalMaterial == 0 ? "Instancing" : originalMaterialName + "/Instanced");
		Material * instancedMaterial = MaterialManager::getSingleton ().getByName (instancedMaterialName);

		// already exists ?
		if(instancedMaterial == 0)
		{
			instancedMaterial = originalMaterial->clone(instancedMaterialName);
			instancedMaterial->load();
			ShaderChList::iterator i, iend = instancedMaterial->getLOD()->getShaderChList().end();
			for(i = instancedMaterial->getLOD()->getShaderChList().begin(); i != iend; ++i)
			{
				Pass * p = *i;
                p->getProgram().set(ST_VS, 0, "Instancing");
                p->getProgram().set(ST_VS_SC, 0,"InstancingShadowCaster");
			}
		}
		instancedMaterial->load();
		return instancedMaterialName;
	}
	//-----------------------------------------------------------------------
	void destroyInstanceGeom()
	{
        mSceneMgr->destroyAllInstanceSpace();
		renderInstance.clear();
	}
	//-----------------------------------------------------------------------
	void createStaticGeom()
	{
		GeometrySpace *ent = mSceneMgr->createGeo(meshes[mSelectedMesh], meshes[mSelectedMesh] + ".mesh");

		renderStatic.reserve (mNumRendered);
		renderStatic.resize (mNumRendered);

		StaticSpace* geom = mSceneMgr->createStaticSpace(meshes[mSelectedMesh] + "s");

		geom->setRegionDimensions (Vector3(1000000, 1000000, 1000000));
		size_t k = 0;
		size_t y = 0;
		for (size_t i = 0; i < mNumMeshes; i++)
		{
			if (y==maxObjectsPerBatch)
			{
				y=0;
				k++;
			}
			geom->addEntity (ent, posMatrices[k][y]);
			y++;
		}
		geom->setCastShadow(true);
		geom->build ();
		renderStatic[0] = geom;
		mSceneMgr->destroyGeo (ent->getID());
	}
	//-----------------------------------------------------------------------
	void destroyStaticGeom()
	{
        mSceneMgr->destroyAllStaticSpace();
		renderStatic.clear();
	}
	//-----------------------------------------------------------------------
	void createEntityGeom()
	{
		size_t k = 0;
		size_t y = 0;
		renderEntity.reserve (mNumMeshes);
		renderEntity.resize (mNumMeshes);
		nodes.reserve (mNumMeshes);
		nodes.resize (mNumMeshes);

		for (size_t i = 0; i < mNumMeshes; i++)
		{
			if (y==maxObjectsPerBatch)
			{
				y=0;
				k++;
			}

			nodes[i]=mSceneMgr->getOriginNode()->create("node"+StrConv::conv(i));
			renderEntity[i]=mSceneMgr->createGeo(meshes[mSelectedMesh]+StrConv::conv(i), meshes[mSelectedMesh] + ".mesh");
			nodes[i]->attachObject(renderEntity[i]);
			nodes[i]->setPos(posMatrices[k][y]);

			y++;
		}
	}
	//-----------------------------------------------------------------------
	void destroyEntityGeom()
	{
		size_t i;
		size_t j = 0;
		for(i = 0; i < mNumMeshes; i++)
		{
			String name = nodes[i]->getName();
			mSceneMgr->destroySpaceNode(name);
			mSceneMgr->destroyGeo(renderEntity[i]->getID());
			j++;
		}
	}
    //-----------------------------------------------------------------------
	void setCurrentGeometryOpt(CurrentGeomOpt opt)
	{
		mCurrentGeomOpt = opt;
	}
	//-----------------------------------------------------------------------
	// Just override the mandatory create scene method
	void setupContent()
	{
		// Set ambient light
		mSceneMgr->setAmbient(Colour(0.2, 0.2, 0.2));
		Light * l = mSceneMgr->createLight("MainLight");
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/MorningSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		//add a skybox
		mBoxSky->buildGeo(1000);
        mBoxSky->addSky(mBoxSky);
		//setup the light
		l->setType(LT_DIRECTIONAL);
		l->setDirection(Vector3f(-0.5, -0.5, 0));

		mCamera->setPos(NII::Vector3f(500, 500, 1500));
		mCamera->lookAt(NII::Vector3f(0, 0, 0));
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
        setDragLook(true);
#endif

		Plane plane;
        plane.normal = Vector3::Y;
        plane.d = 100;
        MeshManager::getSingleton().createPlane("Myplane", GroupDefault, plane,
            1500, 1500, 20, 20, true, 1, 5, 5, Vector3::Z);
        GeometrySpace* pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
        pPlaneEnt->setMaterialName("Examples/Rockwall");
        pPlaneEnt->setCastShadow(false);
        static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(pPlaneEnt);

		CompositorManager::getSingleton().addCompositor(mViewport,"Bloom");

		setupControls();

		const GpuProgramManager::SyntaxCodes &syntaxCodes = GpuProgramManager::getSingleton().getSupportMark();
		for (GpuProgramManager::SyntaxCodes::const_iterator iter = syntaxCodes.begin();iter != syntaxCodes.end();++iter)
		{
			LogManager::getSingleton().logMessage("supported syntax : "+(*iter));
		}

		mNumMeshes = 160;
		mNumRendered = 0;
		mSelectedMesh = 0;
		mBurnAmount = 0;
		mCurrentGeomOpt = INSTANCE_OPT;
		createCurrentGeomOpt();

		mTimer = new Ogre::Timer();
		mLastTime = mTimer->getUS() / 1000000.0f;
	}

	//-----------------------------------------------------------------------
	void setupControls()
	{
		SelectMenu* technique = mTrayMgr->createThickSelectMenu(TL_TOPLEFT, "TechniqueType", "Instancing Technique", 200, 3);
		technique->addItem("Instancing");
		technique->addItem("Static Geometry");
		technique->addItem("Independent Entities");

		SelectMenu* objectType = mTrayMgr->createThickSelectMenu(TL_TOPLEFT, "ObjectType", "Object : ", 200, 4);
		objectType->addItem("razor");
		objectType->addItem("knot");
		objectType->addItem("tudorhouse");
		objectType->addItem("woodpallet");

		mTrayMgr->createThickSlider(TL_TOPLEFT, "ObjectCountSlider", "Object count", 200, 50, 0, 1000, 101)->setValue(160, false);

		mTrayMgr->createThickSlider(TL_TOPLEFT, "CPUOccupationSlider", "CPU Load (ms)", 200, 75, 0, 1000.0f / 60, 20);

		mTrayMgr->createCheckBox(TL_TOPLEFT, "ShadowCheckBox", "Shadows", 200);

		mTrayMgr->createCheckBox(TL_TOPLEFT, "PostEffectCheckBox", "Post Effect", 200);

		mTrayMgr->showCursor();
	}

	void cleanupContent()
	{
        mSceneMgr->destroyAllInstanceSpace();
        mSceneMgr->destroyAllStaticSpace();
        MeshManager::getSingleton().remove("Myplane");
		destroyCurrentGeomOpt();
		delete mTimer;
	}

	void sliderMoved(Slider* slider)
	{
		if (slider->getName() == "ObjectCountSlider")
		{
			destroyCurrentGeomOpt();
			setObjectCount((size_t)slider->getValue());
			createCurrentGeomOpt();
		}
		else if (slider->getName() == "CPUOccupationSlider")
		{
			setBurnedTime(slider->getValue() / 1000.0f);
		}
	}

	void itemSelected(SelectMenu* menu)
	{
		if (menu->getName() == "TechniqueType")
		{
			//Menu items are synchronized with enum
			CurrentGeomOpt selectedOption = (CurrentGeomOpt)menu->getSelectionIndex();
			destroyCurrentGeomOpt();
			setCurrentGeometryOpt(selectedOption);
			createCurrentGeomOpt();
		}
		else if (menu->getName() == "ObjectType")
		{
			destroyCurrentGeomOpt();
			changeSelectedMesh(menu->getSelectionIndex());
			createCurrentGeomOpt();
		}
	}

	void checkBoxToggled(CheckBox* box)
	{
		if (box->getName() == "ShadowCheckBox")
		{
			if (box->isChecked())
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
			}
			else
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
			}
		}
		else if (box->getName() == "PostEffectCheckBox")
		{
			CompositorManager::getSingleton().setCompositorEnabled(mViewport,"Bloom",box->isChecked());
		}
	}

	double mAvgFrameTime;
	size_t mSelectedMesh;
	size_t mNumMeshes;
	size_t objectCount;
	String mDebugText;
	CurrentGeomOpt mCurrentGeomOpt;

	size_t mNumRendered;

	Ogre::Timer*mTimer;
	double mLastTime, mBurnAmount;

	vector <InstanceSpace *>::type renderInstance;
	vector <StaticSpace *>::type renderStatic;
	vector <GeometrySpace *>::type renderEntity;
	vector <SpaceNode *>::type nodes;
	vector <Vector3 *>::type posMatrices;
    BoxSky * mBoxSky;
};
