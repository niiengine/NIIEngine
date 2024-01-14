#ifndef __DualQuaternion_Sample_H__
#define __DualQuaternion_Sample_H__

#include "SdkSample.h"

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderExHardwareSkinning.h"
#endif

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_DualQuaternion : public SdkSample
{
public:
	Sample_DualQuaternion() : 
        ent(0), 
        entDQ(0), 
        totalTime(0)
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
		, mSrsHardwareSkinning(0)
#endif
	{
		mInfo["Title"] = "Dual Quaternion Skinning";
		mInfo["Description"] = "A demo of the dual quaternion skinning feature in conjunction with the linear skinning feature.";
		mInfo["Thumbnail"] = "thumb_dualquaternionskinning.png";
		mInfo["Category"] = "Animation";
	}

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		const NIIf start = 30;
		const NIIf range = 145;
		const NIIf speed = 1;
		Vector3 vec;
        Vector3(1,0.3,0).normalise(vec);
		totalTime += evt->mCurrent;
		Quaternion orient;
        orient.from(Angle(start + Ogre::Math::Sin(totalTime * speed) * range), vec);
		ent->getSkeleton()->get("Bone02")->setOri(orient);
		entDQ->getSkeleton()->get("Bone02")->setOri(orient);

		return SdkSample::onRenderCore(evt);
	}
protected:

	void setupContent()
	{
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
        //Add the hardware skinning to the shader generator default render state
        mSrsHardwareSkinning = mShaderGenerator->createSubRenderState(Ogre::RTShader::HardwareSkinning::Type);
        Ogre::RTShader::RenderState* renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        renderState->addTemplateSubRenderState(mSrsHardwareSkinning);

        Ogre::MaterialPtr pCast1 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_1weight_twophase");
        Ogre::MaterialPtr pCast2 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_2weight_twophase");
        Ogre::MaterialPtr pCast3 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_3weight_twophase");
        Ogre::MaterialPtr pCast4 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_4weight_twophase");

        Ogre::RTShader::HardwareSkinningFactory::getSingleton().setCustomShadowCasterMaterials(RTShader::ST_DUAL_QUATERNION, pCast1, pCast2, pCast3, pCast4);

        Ogre::MaterialPtr pCast1l = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_skinning_1weight");
        Ogre::MaterialPtr pCast2l = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_skinning_2weight");
        Ogre::MaterialPtr pCast3l = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_skinning_3weight");
        Ogre::MaterialPtr pCast4l = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_skinning_4weight");

        Ogre::RTShader::HardwareSkinningFactory::getSingleton().setCustomShadowCasterMaterials(RTShader::ST_LINEAR, pCast1l, pCast2l, pCast3l, pCast4l);
#endif
		// set shadow properties
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
		mSceneMgr->getRenderPattern()->setTextureDim(2048);
		mSceneMgr->setShadowColour(Colour(0.6, 0.6, 0.6));
		mSceneMgr->getRenderPattern()->setTextureCount(1);

		// add a little ambient lighting
		mSceneMgr->setAmbient(Colour(0.2, 0.2, 0.2));

		SpaceNode* lightsBbsNode = mSceneMgr->getOriginNode()->create();
		ParticleCtrl* bbs;

		// Create billboard set for lights .
		bbs = mSceneMgr->createParticleCtrl(0, 20);
		bbs->setMaterialName("Examples/Flare");
		lightsBbsNode->attachObject(bbs);

 		Light * l = mSceneMgr->createLight(0);
		Vector3 dir;
 		l->setType(LT_POINT);
 		l->setPos(30, 70, 40);
 		dir = -l->getPos();
 		dir.normalise();
 		l->setDirection(dir);
 		l->setDiffuse(1, 1, 1);
 		bbs->create(l->getPos())->setColour(l->getDiffuse());

		// create a floor mesh resource
		MeshManager::getSingleton().createPlane("floor", GroupDefault,
			Plane(Vector3::Y, -1), 250, 250, 25, 25, true, 1, 15, 15, Vector3::Z);

		// add a floor to our scene using the floor mesh we created
		GeometrySpace* floor = mSceneMgr->createGeo("Floor", "floor");
		floor->setMaterialName("Examples/Rockwall");
		floor->setCastShadow(false);
		mSceneMgr->getOriginNode()->attachObject(floor);

		// set camera initial transform and speed
		mCamera->setPos(NII::Vector3f(100, 20, 0));
		mCamera->lookAt(NII::Vector3f(0, 10, 0));
		mCameraMan->setTopSpeed(50);

		setupModels();
	}

	void setupModels()
	{
		SpaceNode * sn = mSceneMgr->getOriginNode()->create();
		sn->translate(Vector3f(0, 0, 20), PosNode::SM_Local);

		//Create and attach a spine entity with standard skinning
		ent = mSceneMgr->createGeo("Spine", "spine.mesh");
		ent->setMaterialName("spine");
		ent->getSkeleton()->get("Bone02")->setManual(true);
		sn->attachObject(ent);
		sn->scale(Vector3(0.2,0.2,0.2));

		sn = mSceneMgr->getOriginNode()->create();
		sn->translate(Vector3f(0, 0, -20), PosNode::SM_Local);

		//Create and attach a spine entity with dual quaternion skinning
		entDQ = mSceneMgr->createGeo("SpineDQ", "spine.mesh");
		entDQ->setMaterialName("spineDualQuat");
		entDQ->getSkeleton()->get("Bone02")->setManual(true);
		sn->attachObject(entDQ);
		sn->scale(Vector3(0.2,0.2,0.2));

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
        //In case the system uses the RTSS, the following line will ensure
        //that the entity is using hardware animation in RTSS as well.
        RTShader::HardwareSkinningFactory::getSingleton().prepareEntityForSkinning(ent);
        RTShader::HardwareSkinningFactory::getSingleton().prepareEntityForSkinning(entDQ, RTShader::ST_DUAL_QUATERNION, false, true);

        //The following line is needed only because the spine models' materials have shaders and
        //as such is not automatically reflected in the RTSS system
        RTShader::ShaderGenerator::getSingleton().createShaderBasedTechnique(
            ent->getCom(0)->getMaterialName(),
            0,
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
            true);

        RTShader::ShaderGenerator::getSingleton().createShaderBasedTechnique(
            entDQ->getCom(0)->getMaterialName(),
            0,
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
            true);
#endif

		// create name and value for skinning mode
		StringList names;
		names.push_back("Skinning");
		String value = "Software";

		// change the value if hardware skinning is enabled
		MaterialPtr dqMat = ent->getCom(0)->getMaterial();
		if(!dqMat.isNull())
		{
			ShaderFusion * bestTechnique = dqMat->getLOD();
			if(bestTechnique)
			{
				ShaderCh * pass = bestTechnique->get(0);
                GpuProgram * prog;

				if(pass && pass->getProgram().isExist(ST_VS)
                {
                    pass->getProgram().get(0, ST_VS, prog);
                    if(prog->isVertexMatrixFusion())
                    {
                        value = "Hardware";
                    }
                }
			}
		}

		// create a params panel to display the skinning mode
		mTrayMgr->createParamsPanel(TL_TOPLEFT, "Skinning", 170, names)->setParamValue(0, value);
	}

	void cleanupContent()
	{
		MeshManager::getSingleton().remove("floor");

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
        Ogre::RTShader::RenderState* renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        renderState->removeTemplateSubRenderState(mSrsHardwareSkinning);
#endif
	}

	GeometrySpace* ent;
	GeometrySpace* entDQ;

	NIIf totalTime;

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	RTShader::SubRenderState* mSrsHardwareSkinning;
#endif
};

#endif
