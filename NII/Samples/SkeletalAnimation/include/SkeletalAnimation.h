#ifndef __SkeletalAnimation_H__
#define __SkeletalAnimation_H__

#include "SdkSample.h"

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderExHardwareSkinning.h"
#endif

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_SkeletalAnimation : public SdkSample
{
public:
	Sample_SkeletalAnimation() : NUM_MODELS(6), ANIM_CHOP(8)
#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
		, mSrsHardwareSkinning(0)
#endif
	{
		mInfo["Title"] = "Skeletal Animation";
		mInfo["Description"] = "A demo of the skeletal animation feature, including spline animation.";
		mInfo["Thumbnail"] = "thumb_skelanim.png";
		mInfo["Category"] = "Animation";
	}

    bool onRenderCore(const RenderFrameArgs * evt)
    {
        for (unsigned int i = 0; i < NUM_MODELS; i++)
        {
			// update sneaking animation based on speed
			mAnimStates[i]->addTime(mAnimSpeeds[i] * evt->mCurrent);

			if (mAnimStates[i]->getTimePosition() >= ANIM_CHOP)   // when it's time to loop...
			{
				/* We need reposition the scene node origin, since the animation includes translation.
				Position is calculated from an offset to the end position, and rotation is calculated
				from how much the animation turns the character. */

				Quaternion rot(Angle(-60), Vector3::Y);   // how much the animation turns the character

				// find current end position and the offset
				Vector3 currEnd = mModelNodes[i]->getOri() * mSneakEndPos + mModelNodes[i]->getPos();
				Vector3 offset = rot * mModelNodes[i]->getOri() * -mSneakStartPos;

				mModelNodes[i]->setPos(currEnd + offset);
				mModelNodes[i]->rotate(rot);

				mAnimStates[i]->setTimePosition(0);   // reset animation time
			}
        }

		return SdkSample::onRenderCore(evt);
    }


protected:

	void setupContent()
	{

/*#if defined(USE_RTSHADER_SYSTEM) && defined(RTSHADER_SYSTEM_BUILD_EXT_SHADERS)
		//To make glsles work the program will need to be provided with proper
		//shadow caster materials
		if (mShaderGenerator->getTargetLanguage() != "glsles")
		{
			//Add the hardware skinning to the shader generator default render state
			mSrsHardwareSkinning = mShaderGenerator->createSubRenderState(Ogre::RTShader::HardwareSkinning::Type);
			Ogre::RTShader::RenderState* renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
			renderState->addTemplateSubRenderState(mSrsHardwareSkinning);

			Ogre::MaterialPtr pCast1 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_1weight");
			Ogre::MaterialPtr pCast2 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_2weight");
			Ogre::MaterialPtr pCast3 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_3weight");
			Ogre::MaterialPtr pCast4 = Ogre::MaterialManager::getSingleton().getByName("Ogre/RTShader/shadow_caster_dq_skinning_4weight");

			Ogre::RTShader::HardwareSkinningFactory::getSingleton().setCustomShadowCasterMaterials(
				Ogre::RTShader::ST_DUAL_QUATERNION, pCast1, pCast2, pCast3, pCast4);
		}
#endif*/
		// set shadow properties
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
		mSceneMgr->getRenderPattern()->setTextureDim(512);
		mSceneMgr->setShadowColour(Colour(0.6, 0.6, 0.6));
		mSceneMgr->getRenderPattern()->setTextureCount(2);

		// add a little ambient lighting
		mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));

		SpaceNode * lightsBbsNode = mSceneMgr->getOriginNode()->create();
		ParticleCtrl * bbs;

		// Create billboard set for lights .
		bbs = mSceneMgr->createParticleCtrl(0, 20);
		bbs->setMaterialName("Examples/Flare");
		lightsBbsNode->attachObject(bbs);

		// add a blue spotlight
		Light * l = mSceneMgr->createLight(0);
		Vector3 dir;
		l->setType(LT_SPOT);
		l->setPos(-40, 180, -10);
		dir = -l->getPos();
		dir.normalise();
		l->setDirection(dir);
		l->setDiffuse(0.0, 0.0, 0.5);
		bbs->create(l->getPos())->setColour(l->getDiffuse());

		// add a green spotlight.
		l = mSceneMgr->createLight(0);
		l->setType(LT_SPOT);
		l->setPos(0, 150, -100);
		dir = -l->getPos();
		dir.normalise();
		l->setDirection(dir);
		l->setDiffuse(0.0, 0.5, 0.0);
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
		tweakSneakAnim();

		SpaceNode* sn = NULL;
		GeometrySpace* ent = NULL;
		AnimationFusion* as = NULL;

		for (unsigned int i = 0; i < NUM_MODELS; i++)
		{
			// create scene nodes for the models at regular angular intervals
			sn = mSceneMgr->getOriginNode()->create();
			sn->yaw(Radian(Math::PI2X * (float)i / (float)NUM_MODELS));
			sn->translate(Vector3f(0, 0, -20), PosNode::SM_Local);
			mModelNodes.push_back(sn);

			// create and attach a jaiqua entity
			ent = mSceneMgr->createGeo("Jaiqua" + StrConv::conv(i + 1), "jaiqua.mesh");

/*#ifdef USE_RTSHADER_SYSTEM
			if (mShaderGenerator->getTargetLanguage() == "glsles")*/
            if (mShaderGenerator->getTargetLanguage() == "glsles")
            {
                MaterialPtr mat = MaterialManager::getSingleton().getByName("jaiqua");
                mat->get(0)->get(0)->getProgram().set(ST_FS_SC, 0, "Ogre/BasicFragmentPrograms/PassthroughFpGLSLES");
            }
                ent->setMaterialName("jaiqua");
/*            else
#endif
                ent->setMaterialName("jaiquaDualQuatTest");*/
			sn->attachObject(ent);

/*#if defined(USE_RTSHADER_SYSTEM) && defined(RTSHADER_SYSTEM_BUILD_EXT_SHADERS)
			//To make glsles work the program will need to be provided with proper
			//shadow caster materials
			if (mShaderGenerator->getTargetLanguage() != "glsles")
			{
				//In case the system uses the RTSS, the following line will ensure
				//that the entity is using hardware animation in RTSS as well.
				RTShader::HardwareSkinningFactory::getSingleton().prepareEntityForSkinning(ent, Ogre::RTShader::ST_DUAL_QUATERNION, true, false);

				//The following line is needed only because the Jaiqua model material has shaders and
				//as such is not automatically reflected in the RTSS system
				RTShader::ShaderGenerator::getSingleton().createShaderBasedTechnique(
					ent->getCom(0)->getMaterialID(),
					0,
					Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
					true);
			}
#endif*/

			// enable the entity's sneaking animation at a random speed and loop it manually since translation is involved
			as = ent->getFusion("Sneak");
			as->setEnabled(true);
			as->setPlayMode(RM_Once);
			mAnimSpeeds.push_back(Math::random(0.5, 1.5));
			mAnimStates.push_back(as);
		}

		// create name and value for skinning mode
		StringList names;
		names.push_back("Skinning");
		String value = "Software";

		// change the value if hardware skinning is enabled
		MaterialPtr entityMaterial = ent->getCom(0)->getMaterial();
		if(!entityMaterial.isNull())
		{
			ShaderFusion * sf = entityMaterial->getLOD();
			if(sf)
			{
				Pass * pass = sf->get(0);
                GpuProgram * prog;
				if (pass && pass->getProgram().isExist(ST_VS))
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

	/*-----------------------------------------------------------------------------
	| The jaiqua sneak animation doesn't loop properly. This method tweaks the
	| animation to loop properly by altering the Spineroot bone track.
	-----------------------------------------------------------------------------*/
	void tweakSneakAnim()
	{
		// get the skeleton, animation, and the node track iterator
		SkeletonPtr skel = N_Only(Skeleton).load("jaiqua.skeleton", GroupDefault);
		Animation * anim = skel->getAnimation("Sneak");
		Animation::TrackList tracks = anim->getNodeTrackIterator();

		while(tracks.isNextValid())   // for every node track...
		{
			NodeKeyFrameTrack * track = tracks.getAndNext();

			// get the keyframe at the chopping point
			NodeKeyFrame oldKf(0, 0);
			track->getFusion(ANIM_CHOP, &oldKf);

			// drop all keyframes after the chopping point
			while(track->getKeyFrame(track->getNumKeyFrames()-1)->getTime() >= ANIM_CHOP - 0.3f)
				track->removeKeyFrame(track->getNumKeyFrames()-1);

			// create a new keyframe at chopping point, and get the first keyframe
			NodeKeyFrame * newKf = static_cast<NodeKeyFrame *>(track->create(ANIM_CHOP));
			NodeKeyFrame * startKf = static_cast<NodeKeyFrame *>(track->get(0));

			Bone * bone = skel->get(track->getID());

			if(bone->getName() == "Spineroot")   // adjust spine root relative to new location
			{
				mSneakStartPos = startKf->getTranslate() + bone->getOriginPos();
				mSneakEndPos = oldKf.getTranslate() + bone->getOriginPos();
				mSneakStartPos.y = mSneakEndPos.y;

				newKf->setTranslate(oldKf.getTranslate());
				newKf->setRotation(oldKf.getRotation());
				newKf->setScale(oldKf.getScale());
			}
			else   // make all other bones loop back
			{
				newKf->setTranslate(startKf->getTranslate());
				newKf->setRotation(startKf->getRotation());
				newKf->setScale(startKf->getScale());
			}
            track->buildSpline();
		}
	}

	void cleanupContent()
	{
		mModelNodes.clear();
		mAnimStates.clear();
		mAnimSpeeds.clear();
		MeshManager::getSingleton().remove("floor");

/*#if defined(USE_RTSHADER_SYSTEM) && defined(RTSHADER_SYSTEM_BUILD_EXT_SHADERS)
		//To make glsles work the program will need to be provided with proper
		//shadow caster materials
		if (mShaderGenerator->getTargetLanguage() != "glsles")
		{
			Ogre::RTShader::RenderState* renderState = mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
			renderState->removeTemplateSubRenderState(mSrsHardwareSkinning);
		}
#endif*/
	}

	const unsigned int NUM_MODELS;
	const NIIf ANIM_CHOP;

	std::vector<SpaceNode*> mModelNodes;
	std::vector<AnimationFusion*> mAnimStates;
	std::vector<NIIf> mAnimSpeeds;

	Vector3 mSneakStartPos;
	Vector3 mSneakEndPos;

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
	RTShader::SubRenderState* mSrsHardwareSkinning;
#endif
};

#endif
