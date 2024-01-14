#ifndef __Dot3Bump_H__
#define __Dot3Bump_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Dot3Bump : public SdkSample
{
public:
	Sample_Dot3Bump()
		: mMoveLights (true)
	{
		mInfo["Title"] = "Bump Mapping";
		mInfo["Description"] = "Shows how to use the dot product blending operation and normalization cube map "
			"to achieve a bump mapping effect. Tangent space computations made through the guide of the tutorial "
			"on bump mapping from http://users.ox.ac.uk/~univ1234 by paul.baker@univ.ox.ac.uk.";
		mInfo["Thumbnail"] = "thumb_bump.png";
		mInfo["Category"] = "Lighting";
		mInfo["Help"] = "Left click and drag anywhere in the scene to look around. Let go again to show "
			"cursor and access widgets. Use WASD keys to move.";
	}

	StringList getRequiredPlugins()
	{
		StringList names;
        if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
            names.push_back("Cg Program Manager");
		return names;
	}

	void testCapabilities(const RenderSysFeature* caps)
	{
		if (!caps->hasCapability(GF_Program_Vertex) || !(caps->hasCapability(GF_Program_Fragment)))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and fragment programs, "
				"so you cannot run this sample. Sorry!", "Dot3BumpSample::testCapabilities");
        }

		if (!GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1") &&
			!GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
			!GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0") &&
			!GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
		{
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support the shader model needed for this sample, "
				"so you cannot run this sample. Sorry!", "Dot3BumpSample::testCapabilities");
		}
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		if (mMoveLights)
		{
			// rotate the light pivots
			mLightPivot1->roll(Angle(evt->mCurrent * 30));
			mLightPivot2->roll(Angle(evt->mCurrent * 10));
		}

		return SdkSample::onRenderCore(evt);  // don't forget the parent class updates!
	}

	void itemSelected(SelectMenu* menu)
	{
		if (menu == mMeshMenu)
		{
			// change to the selected entity
			mObjectNode->detachAll();
			mObjectNode->attachObject(mSceneMgr->getGeo(mMeshMenu->getSelectItem()));

			// remember which material is currently selected
			int index = std::max<int>(0, mMaterialMenu->getSelectionIndex());

			// update the material menu's options
			mMaterialMenu->setItems(mPossibilities[mMeshMenu->getSelectItem()]);

			mMaterialMenu->selectItem(index);   // select the material with the saved index
		}
		else
		{
			// set the selected material for the active mesh
			((GeometrySpace *)mObjectNode->getAttach(0))->setMaterialName(menu->getSelectItem());
		}
	}

	void checkBoxToggled(CheckBox * box)
	{
		if(StringUtil::beginsWith(box->getName(), "Light", false))
		{
			// get the light pivot that corresponds to this checkbox
			SpaceNode * pivot = box->getName() == "Light1" ? mLightPivot1 : mLightPivot2;
			SpaceNode::AttachList::iterator it, itend = pivot->getAttachList().end();
            for(it = pivot->getAttachList().begin(); it != itend; ++i)
			{
				it->second->setVisible(box->isChecked());
			}

		}
		else if (box->getName() == "MoveLights")
		{
			mMoveLights = !mMoveLights;
		}
	}

protected:

	void setupContent()
	{
		// create our main node to attach our entities to
		mObjectNode = mSceneMgr->getOriginNode()->create();

		setupModels();
		setupLights();
		setupControls();

		mCamera->setPos(Vector3f(0, 0, 500));

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		setDragLook(true);
#endif
	}

	void loadResources()
	{
#ifdef USE_RTSHADER_SYSTEM
		Ogre::GroupIDList groupVector = Ogre::ResourceSchemeManager::getSingleton().getGroups();
		Ogre::GroupIDList::iterator itGroup, itGroupEnd = groupVector.end();
		Ogre::String shaderCoreLibsPath;

		for(itGroup = groupVector.begin(); itGroup != itGroupEnd; ++itGroup)
		{
			bool coreLibsFound = false;

			// Find the location of the core shader libs
            VFS * vfs = N_Only(ResourceScheme).getVFS("RTShaderLib", *itGroup);
            if(vfs)
            {
                shaderCoreLibsPath = vfs->getName() + "/";
                coreLibsFound = true;
                break;
			}

			// Core libs path found in the current group.
			if (coreLibsFound)
				break;
		}
#endif
	}

	void unloadResources()
	{

	}

	void setupModels()
	{
		StringList matNames;

		matNames.push_back("Examples/BumpMapping/MultiLight");
		matNames.push_back("Examples/BumpMapping/MultiLightSpecular");
		matNames.push_back("Examples/OffsetMapping/Specular");
		matNames.push_back("Examples/ShowUV");
		matNames.push_back("Examples/ShowNormals");
		matNames.push_back("Examples/ShowTangents");

#ifdef USE_RTSHADER_SYSTEM
		matNames.push_back("RTSS/NormalMapping_SinglePass");
		matNames.push_back("RTSS/NormalMapping_MultiPass");
#endif


		mPossibilities["ogrehead.mesh"] = matNames;
		mPossibilities["knot.mesh"] = matNames;

		matNames.clear();
		matNames.push_back("Examples/Athene/NormalMapped");
		matNames.push_back("Examples/Athene/NormalMappedSpecular");
		matNames.push_back("Examples/Athene/NormalMappedSpecular");
		matNames.push_back("Examples/ShowUV");
		matNames.push_back("Examples/ShowNormals");
		matNames.push_back("Examples/ShowTangents");
#ifdef USE_RTSHADER_SYSTEM
		matNames.push_back("RTSS/Athene/NormalMapping_SinglePass");
		matNames.push_back("RTSS/Athene/NormalMapping_MultiPass");
#endif

		mPossibilities["athene.mesh"] = matNames;

		for(std::map<String, StringList>::iterator it = mPossibilities.begin(); it != mPossibilities.end(); it++)
		{
			// load each mesh with non-default hardware buffer usage options
			MeshPtr mesh = N_Only(Mesh).load(it->first, GroupDefault, Buffer::M_WRITE);

            // build tangent vectors for our mesh
            unsigned short src, dest;
            if(!mesh->checkTangentBuffer(false))
            {
                mesh->buildTangentBuffer(VT_Tangent);
            }

			// create an entity from the mesh and set the first available material
			GeometrySpace* ent = mSceneMgr->createGeo(mesh->getName(), mesh->getName());
			ent->setMaterialName(it->second.front());
		}
	}

	void setupLights()
	{
		mSceneMgr->setAmbient(Colour::Black);   // disable ambient lighting

		// create pivot nodes
		mLightPivot1 = mSceneMgr->getOriginNode()->create();
		mLightPivot2 = mSceneMgr->getOriginNode()->create();

		Light* l;
		ParticleCtrl* bbs;

		// create white light
		l = mSceneMgr->createLight(0);
		l->setPos(200, 0, 0);
		l->setDiffuse(1, 1, 1);
		l->setSpecular(1, 1, 1);
		// create white flare
		bbs = mSceneMgr->createParticleCtrl(0, 20);
		bbs->setMaterialName("Examples/Flare");
		bbs->create(Vector3f(200, 0, 0))->setColour(Colour::White);

		mLightPivot1->attachObject(l);
		mLightPivot1->attachObject(bbs);

		// create red light
		l = mSceneMgr->createLight(0);
		l->setPos(40, 200, 50);
		l->setDiffuse(1, 0, 0);
		l->setSpecular(1, 0.8, 0.8);
		// create white flare
		bbs = mSceneMgr->createParticleCtrl(0, 20);
		bbs->setMaterialName("Examples/Flare");
		bbs->create(Vector3f(50, 200, 50))->setColour(Colour::Red);

		mLightPivot2->attachObject(l);
		mLightPivot2->attachObject(bbs);
	}

	void setupControls()
	{
		mTrayMgr->showCursor();

		// make room for the controls
		mTrayMgr->showLogo(TL_TOPRIGHT);
		mTrayMgr->showFrameStats(TL_TOPRIGHT);
		mTrayMgr->toggleAdvancedFrameStats();

		// create a menu to choose the model displayed
		mMeshMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "Mesh", "Mesh", 370, 290, 10);
		for (std::map<String, StringList>::iterator it = mPossibilities.begin(); it != mPossibilities.end(); it++)
			mMeshMenu->addItem(it->first);

		// create a menu to choose the material used by the model
		mMaterialMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "Material", "Material", 370, 290, 10);

		// create checkboxes to toggle lights
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Light1", "Light A")->setChecked(true, false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Light2", "Light B")->setChecked(true, false);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "MoveLights", "Move Lights")->setChecked(true, false);

		// a friendly reminder
		StringList names;
		names.push_back("Help");
		mTrayMgr->createParamsPanel(TL_TOPLEFT, "Help", 100, names)->setParamValue(0, "H/F1");

		mMeshMenu->selectItem(0);  // select first mesh
	}

	void cleanupContent()
	{
		// clean up properly to avoid interfering with subsequent samples
		for (std::map<String, StringList>::iterator it = mPossibilities.begin(); it != mPossibilities.end(); it++)
			MeshManager::getSingleton().unload(it->first);
		mPossibilities.clear();
	}

	std::map<String, StringList> mPossibilities;
	SpaceNode* mObjectNode;
	SpaceNode* mLightPivot1;
	SpaceNode* mLightPivot2;
	bool mMoveLights;
	SelectMenu* mMeshMenu;
	SelectMenu* mMaterialMenu;
};

#endif
