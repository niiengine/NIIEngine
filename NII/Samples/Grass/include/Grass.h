#ifndef __Grass_H__
#define __Grass_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Grass : public SdkSample
{
public:
	Sample_Grass() : GRASS_WIDTH(40), GRASS_HEIGHT(40)
	{
		mInfo["Title"] = "Grass";
		mInfo["Description"] = "Demonstrates how to use the StaticSpace class to create 'baked' "
			"instances of many meshes, to create effects like grass efficiently.";
		mInfo["Thumbnail"] = "thumb_grass.png";
		mInfo["Category"] = "Environment";
		mInfo["Help"] = "Press B to toggle bounding boxes.";
        mBoxSky = 0;
	}
    ~Sample_Grass()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
    bool onRenderCore(const RenderFrameArgs * evt)
    {
		mLightAnimState->addTime(evt->mCurrent);   // move the light around
		waveGrass(evt->mCurrent);                  // wave the grass around slowly to simulate wind
		return SdkSample::onRenderCore(evt);        // don't forget the parent class updates!
    }

	bool keyPressed(const OIS::KeyEvent& evt)
	{
		// toggle bounding boxes with B key unless the help dialog is visible
		if (evt.key == OIS::KC_B && !mTrayMgr->isDialogVisible())
			mSceneMgr->setAABBView(!mSceneMgr->isAABBView());
		return SdkSample::keyPressed(evt);
	}

protected:

	/*=============================================================================
	// This class will be used to pulsate the light and billboard.
	=============================================================================*/
	class LightPulse : public DataValue<NIIf>
	{
	public:
		LightPulse(Light * light, SyncParticle * billboard, const Colour & maxColour, NIIf maxSize)
		{
			mLight = light;
			mBillboard = billboard;
			mMaxColour = maxColour;
			mMaxSize = maxSize;
		}

		NIIf getValue () const
		{
			return mIntensity;
		}

		void setValue(NIIf value)
		{
			mIntensity = value;

			// calculate new colour and apply it to the light and billboard
			Colour newColour = mMaxColour * mIntensity;
			mLight->setDiffuse(newColour);
			mBillboard->setColour(newColour);

			// calculate new billboard size and apply it
			NIIf newSize = mMaxSize * mIntensity;
			mBillboard->setSize(newSize, newSize);
		}
	protected:
		Light * mLight;
		SyncParticle * mBillboard;
		Colour mMaxColour;
		NIIf mMaxSize;
		NIIf mIntensity;
	};

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(push, 1)
#endif
    struct GrassVertex
	{
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(pop)
#endif

	void setupContent()
	{
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/SpaceSkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		mBoxSky->buildGeo(5000);
        mSceneMgr->addSky(mBoxSky);
		// create a mesh for our ground
		MeshManager::getSingleton().createPlane("ground", GroupDefault,
			Plane(Vector3::Y, 0), 1000, 1000, 20, 20, true, 1, 6, 6, Vector3::Z);

		// create a ground entity from our mesh and attach it to the origin
		GeometrySpace* ground = mSceneMgr->createGeo("Ground", "ground");
		ground->setMaterialName("Examples/GrassFloor");
		ground->setCastShadow(false);
		mSceneMgr->getOriginNode()->attachObject(ground);

		// create our grass mesh, and create a grass entity from it
		createGrassMesh();
		GeometrySpace* grass = mSceneMgr->createGeo("Grass", "grass");

		// create a static geometry field, which we will populate with grass
		mField = mSceneMgr->createStaticSpace("Field");
		mField->setRegionDimensions(Vector3(140, 140, 140));
		mField->setOrigin(Vector3(70, 70, 70));

		// add grass uniformly throughout the field, with some random variations
		for (int x = -280; x < 280; x += 20)
		{
			for (int z = -280; z < 280; z += 20)
			{
				Vector3 pos(x + Math::random(-7, 7), 0, z + Math::random(-7, 7));
				Quaternion ori(Angle(Math::random(0, 359)), Vector3::Y);
				Vector3 scale(1, Math::random(0.85, 1.15), 1);

				mField->addEntity(grass, pos, ori, scale);
			}
		}

		mField->build();  // build our static geometry (bake the grass into it)

        unsigned short src, dest;
		// build tangent vectors for the ogre head mesh
		MeshPtr headMesh = N_Only(Mesh).load("ogrehead.mesh", GroupDefault);
		if(!headMesh->checkTangentBuffer(false))
			headMesh->buildTangentBuffer(VT_Tangent);

		// put an ogre head in the middle of the field
		GeometrySpace * head = mSceneMgr->createGeo("Head", "ogrehead.mesh");
		head->setMaterialName("Examples/OffsetMapping/Specular");
		static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0, 30, 0)))->attachObject(head);

		setupLighting();

		// set initial camera position and speed
		mCamera->setPos(Vector3f(0, 50, 100));
	}

	void createGrassMesh()
	{
		MeshPtr mesh = N_Only(Mesh).createManual("grass", GroupDefault);

		// create a submesh with the grass material
		SubMesh * sm = mesh->createSub();
		sm->setMaterialName("Examples/GrassBlades");
		sm->setSelfVertex(true);
        VertexData * smvd;
        sm->setVertexData(smvd);
        N_Only(GBuffer).create(smvd);
		smvd->mOffset = 0;
		smvd->mCount = 12;
		sm->getIndexData(0)->mCount = 18;

		// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
		VertexData * decl = smvd;
        decl->add(DT_Float3x, VT_Pos, 0, 0);
        decl->add(DT_Float3x, VT_Normals, 0, sizeof(float) * 3);
        decl->add(DT_Float2x, VT_Tex_Coord, 0, sizeof(float) * 6, 0);

		// create a vertex buffer
		VertexBuffer * vb;
        N_Only(GBuffer).create(vb, decl->getUnitSize(0), smvd->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

		GrassVertex * verts = (GrassVertex*)vb->lock(Buffer::MM_WHOLE);  // start filling in vertex data

		for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			// planes intersect along the Y axis with 60 degrees between them
			NIIf x = Math::Cos(Angle(i * 60)) * GRASS_WIDTH / 2;
			NIIf z = Math::Sin(Angle(i * 60)) * GRASS_WIDTH / 2;

			for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
			{
				GrassVertex& vert = verts[i * 4 + j];

				vert.x = j < 2 ? -x : x;
				vert.y = j % 2 ? 0 : GRASS_HEIGHT;
				vert.z = j < 2 ? -z : z;

				// all normals point straight up
				vert.nx = 0;
				vert.ny = 1;
				vert.nz = 0;

				vert.u = j < 2 ? 0 : 1;
				vert.v = j % 2;
			}
		}

		vb->unlock();  // commit vertex changes

		smvd->attach(0, vb);  // bind vertex buffer to our submesh

		// create an index buffer
		N_Only(GBuffer).create(sm->getIndexData(0)->mBuffer, 16,
            sm->getIndexData(0)->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

		// start filling in index data
		Ogre::uint16* indices = (Ogre::uint16*)sm->getIndexData(0)->mBuffer->lock(Buffer::MM_WHOLE);

		for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			unsigned int off = i * 4;  // each plane consists of 2 triangles

			*indices++ = 0 + off;
			*indices++ = 3 + off;
			*indices++ = 1 + off;

			*indices++ = 0 + off;
			*indices++ = 2 + off;
			*indices++ = 3 + off;
		}

		sm->getIndexData(0)->mBuffer->unlock();  // commit index changes
	}

	void setupLighting()
	{
		mSceneMgr->setAmbient(Colour::Black);  // turn off ambient light

		Colour lightColour(1, 1, 0.3);

		// create a light
		Light * light = mSceneMgr->createLight(0);
		light->setDiffuse(lightColour);
		light->setSpecular(1, 1, 0.3);
        light->setRange(1500);
        light->setAttenuationConstant(1);
        light->setAttenuationLinear(0.0005);
        light->setAttenuationQuadric(0);
		// create a flare
		ParticleCtrl* bbs = mSceneMgr->createParticleCtrl(0, 1);
		bbs->setMaterialName("Examples/Flare");
		SyncParticle* bb = bbs->create(Vector3f(0, 0, 0), lightColour);

		// create a controller for the light intensity, using our LightPulsator class
		DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(Ogre::WFT_SINE, 0.5, 0.5, 0, 0.5);
		DataValue<NIIf> * dest = N_new LightPulse(light, bb, lightColour, 15);
		EquationManager& cm = EquationManager::getSingleton();
		mLightController = cm.createController(cm.getFrameTimeSource(), dest, func);

		// create a light node and attach the light and flare to it
		SpaceNode* lightNode = mSceneMgr->getOriginNode()->create();
		lightNode->attachObject(light);
		lightNode->attachObject(bbs);

		// set up a 20 second animation for our light, using spline interpolation for nice curves
        Animation* anim = mSceneMgr->createAnimation("LightTrack", 20);
        anim->setInterpolationMode(NodeFusion::M_SPLINE);

		// create a track to animate the camera's node
        NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, 0);
        track->setAssociatedNode(lightNode);

        // create keyframes for our track
        static_cast<NodeKeyFrame *>(track->create(0))->setTranslate(Vector3(42, 77, -42));
        static_cast<NodeKeyFrame *>(track->create(2))->setTranslate(Vector3(21, 84, -35));
        static_cast<NodeKeyFrame *>(track->create(4))->setTranslate(Vector3(-21, 91, -14));
        static_cast<NodeKeyFrame *>(track->create(6))->setTranslate(Vector3(-56, 70, -28));
        static_cast<NodeKeyFrame *>(track->create(8))->setTranslate(Vector3(-28, 70, -56));
        static_cast<NodeKeyFrame *>(track->create(10))->setTranslate(Vector3(-14, 63, -28));
        static_cast<NodeKeyFrame *>(track->create(12))->setTranslate(Vector3(-14, 56, 28));
        static_cast<NodeKeyFrame *>(track->create(14))->setTranslate(Vector3(0, 35, 84));
        static_cast<NodeKeyFrame *>(track->create(16))->setTranslate(Vector3(14, 35, 14));
        static_cast<NodeKeyFrame *>(track->create(18))->setTranslate(Vector3(35, 84, 0));
        static_cast<NodeKeyFrame *>(track->create(20))->setTranslate(Vector3(42, 77, -42));

        track->buildSpline();

		lightNode->setPos(static_cast<NodeKeyFrame *>(track->get(0))->getTranslate());

		// create a new animation state to track this
		mLightAnimState = mSceneMgr->createFusion("LightTrack");
		mLightAnimState->setEnabled(true);
	}

	void waveGrass(TimeDurMS timeElapsed)
	{
		static NIIf xinc = Math::PI * 0.3;
		static NIIf zinc = Math::PI * 0.44;
		static NIIf xpos = Math::random(-Math::PI, Math::PI);
		static NIIf zpos = Math::random(-Math::PI, Math::PI);
		static Vector4 offset(0, 0, 0, 0);

		xpos += xinc * timeElapsed;
		zpos += zinc * timeElapsed;

		// update vertex program parameters by binding a value to each renderable
		StaticSpace::RegionIterator regs =  mField->getRegionIterator();
		while (regs.isNextValid())
		{
			StaticSpace::Region* reg = regs.getAndNext();

			// a little randomness
			xpos += reg->getCentre().x * 0.001;
			zpos += reg->getCentre().z * 0.001;
			offset.x = Math::Sin(xpos) * 4;
			offset.z = Math::Sin(zpos) * 4;

			StaticSpace::Region::LODIterator lods = reg->getLODIterator();
			while(lods.isNextValid())
			{
				StaticSpace::LODBucket::MaterialIterator mats = lods.getAndNext()->getMaterialIterator();
				while(mats.isNextValid())
				{
					StaticSpace::MaterialBucket::GeometryIterator geoms = mats.getAndNext()->getGeometryIterator();
					while(geoms.isNextValid()) 
                        geoms.getAndNext()->setCustonParam(999, offset);
				}
			}
		}
	}

	void cleanupContent()
	{
		EquationManager::getSingleton().destroyController(mLightController);
		MeshManager::getSingleton().remove("ground");
		MeshManager::getSingleton().remove("grass");
	}

	const NIIf GRASS_WIDTH;
	const NIIf GRASS_HEIGHT;
	StaticSpace * mField;
	AnimationFusion * mLightAnimState;
	DataEquation<NIIf> * mLightController;
    BoxSky * mBoxSky;
};

#endif