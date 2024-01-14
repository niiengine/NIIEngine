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

#ifndef _WATER_H_
#define _WATER_H_

#include "SdkSample.h"
#include "OgreParticleCtrlGeo.h"
#include "WaterMesh.h"

#include <iostream>

using namespace Ogre;
using namespace OgreBites;

// Mesh stuff
#define MESH_NAME "WaterMesh"
#define ENTITY_NAME "WaterEntity"
#define MATERIAL_PREFIX "Examples/Water"
#define MATERIAL_NAME "Examples/Water0"
#define COMPLEXITY 64 		// watch out - number of polys is 2*ACCURACY*ACCURACY !
#define PLANE_SIZE 3000.0f
#define CIRCLES_MATERIAL "Examples/Water/Circles"

void prepareCircleMaterial()
{
	char *bmap = new char[256 * 256 * 4] ;
	memset(bmap, 127, 256 * 256 * 4);
	for(int b=0;b<16;b++) {
		int x0 = b % 4 ;
		int y0 = b >> 2 ;
		NIIf radius = 4.0f + 1.4 * (float) b ;
		for(int x = 0;x < 64; x++)
        {
			for(int y = 0;y < 64; y++)
            {
				NIIf dist = Math::sqrt((x-32)*(x-32)+(y-32)*(y-32)); // 0..ca.45
				dist = fabs(dist -radius -2) / 2.0f ;
				dist = dist * 255.0f;
				if (dist>255)
					dist=255 ;
				int colour = 255-(int)dist ;
				colour = (int)( ((NIIf)(15-b))/15.0f * (NIIf) colour );

				bmap[4*(256*(y+64*y0)+x+64*x0)+0]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+1]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+2]=colour ;
				bmap[4*(256*(y+64*y0)+x+64*x0)+3]=colour ;
			}
		}
	}

	DataStream * imgstream = N_new MemDataStream(bmap, 256 * 256 * 4);
	//~ Image img;
	//~ img.loadRawData( imgstream, 256, 256, PF_A8R8G8B8 );
	//~ N_Only(Texture).load(CIRCLES_MATERIAL , img);
	N_Only(Texture).loadRawData(CIRCLES_MATERIAL, GroupDefault, imgstream, 256, 256, PF_A8R8G8B8);
    N_delete imgstream;

	MaterialPtr material = N_Only(Material).create(CIRCLES_MATERIAL, GroupDefault);
	ShaderChTextureUnit * texLayer = material->get(0)->get(0)->getTexture().create(CIRCLES_MATERIAL);
    texLayer->setUAddressing(TAM_CLAMP);
    texLayer->setVAddressing(TAM_CLAMP);
    texLayer->setWAddressing(TAM_CLAMP);
	N_Only(Material).setBlend(material, FBM_ADD);
	N_Only(Material).setDepthWrite(material, false) ;
    material->load();
    // finished with bmap so release the memory
    delete [] bmap;
}

/* =========================================================================*/
/*               WaterCircle class                                          */
/* =========================================================================*/
#define CIRCLE_SIZE 500.0
#define CIRCLE_TIME 0.5f
class WaterCircle
{
private:
	String name ;
	SpaceNode *node ;
	MeshPtr mesh ;
	SubMesh *subMesh ;
	GeometrySpace *entity ;
	NIIf tm ;
    SpaceManager *sceneMgr ;
	static bool first ;
	// some buffers shared by all circles
	static VertexBuffer * posnormVertexBuffer ;
	static IndexBufferSPtr indexBuffer ; // indices for 2 faces
	static VertexBufferSPtr *texcoordsVertexBuffers ;

	float *texBufData;
	void _prepareMesh()
	{
		int i,texLvl ;

		mesh = N_Only(Mesh).createManual(name, GroupDefault);
		subMesh = mesh->createSub();
		subMesh->setSelfVertex(true);

		int numVertices = 4 ;

		if(first)
        { // first Circle, create some static common data
			first = false ;

			// static buffer for position and normals
            N_Only(GBuffer).create(posnormVertexBuffer,
                6 * sizeof(float), // size of one vertex data
                4, // number of vertices
                Buffer::Buffer::M_DEV | Buffer::M_WRITE);
			float * posnormBufData = (float*) posnormVertexBuffer->
            lock(Buffer::MM_WHOLE);
			for(i = 0; i < numVertices; ++i)
            {
				posnormBufData[6*i+0]=((NIIf)(i%2)-0.5f)*CIRCLE_SIZE; // pos X
				posnormBufData[6*i+1]=0; // pos Y
				posnormBufData[6*i+2]=((NIIf)(i/2)-0.5f)*CIRCLE_SIZE; // pos Z
				posnormBufData[6*i+3]=0 ; // normal X
				posnormBufData[6*i+4]=1 ; // normal Y
				posnormBufData[6*i+5]=0 ; // normal Z
			}
			posnormVertexBuffer->unlock();

			// static buffers for 16 sets of texture coordinates
			texcoordsVertexBuffers = new VertexBufferSPtr[16];
			for(texLvl = 0; texLvl < 16; texLvl++)
            {
                N_Only(GBuffer).create(texcoordsVertexBuffers[texLvl],
                    2 * sizeof(float), // size of one vertex data
                    numVertices, // number of vertices
                    Buffer::Buffer::M_DEV | Buffer::M_WRITE);
				float * texcoordsBufData = (float*) texcoordsVertexBuffers[texLvl]->
                lock(Buffer::MM_WHOLE);
				float x0 = (NIIf)(texLvl % 4) * 0.25 ;
				float y0 = (NIIf)(texLvl / 4) * 0.25 ;
				y0 = 0.75-y0 ; // upside down
				for(i = 0; i < 4; ++i)
                {
					texcoordsBufData[i * 2 + 0]= x0 + 0.25 * (NIIf)(i%2) ;
					texcoordsBufData[i * 2 + 1]= y0 + 0.25 * (NIIf)(i/2) ;
				}
				texcoordsVertexBuffers[texLvl]->unlock();
			}

			// Index buffer for 2 faces
			unsigned short faces[6] = {2,1,0,  2,3,1};
            N_Only(GBuffer).create(indexBuffer, 16, 6, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
			indexBuffer->write(faces, 0, indexBuffer->getSize());
		}
        VertexData * smvd;
		// Initialize vertex data
        N_Only(GBuffer).create(smvd);
        subMesh->setVertexData(smvd);
        smvd->mOffset = 0;
		smvd->mCount = 4;
		// first, set vertex buffer bindings
		VertexData * vbind = smvd;
		vbind->attach(0, posnormVertexBuffer);
		vbind->attach(1, texcoordsVertexBuffers[0]);
		// now, set vertex buffer declaration
		VertexData * vd = smvd;
		vd->add(DT_Float3x, VT_Pos, 0, 0);
		vd->add(DT_Float3x, VT_Normals, 0, 3 * sizeof(float));
		vd->add(DT_Float2x, VT_Tex_Coord, 1, 0);

		// Initialize index data
		subMesh->getIndexData(0)->mBuffer = indexBuffer;
		subMesh->getIndexData(0)->mOffset = 0;
		subMesh->getIndexData(0)->mCount = 6;

		// set mesh bounds
		AABox circleBounds(-CIRCLE_SIZE/2.0f, 0, -CIRCLE_SIZE/2.0f,
            CIRCLE_SIZE/2.0f, 0, CIRCLE_SIZE/2.0f);
		mesh->setAABB(circleBounds);
        mesh->setRangeFactor(0.01);
        mesh->load();
        mesh->ref();
	}
public:
	int lvl ;
	void setTextureLevel()
	{
		subMesh->getVertexData()->attach(1, texcoordsVertexBuffers[lvl]);
	}
	WaterCircle(SpaceManager *mgr, const String& inName, NIIf x, NIIf y)
	{
        sceneMgr = mgr;
		name = inName ;
		_prepareMesh();
		node = static_cast<SpaceNode*> (sceneMgr->getOriginNode()->create(name));
		node->translate(Vector3f(x*(PLANE_SIZE/COMPLEXITY), 10, y*(PLANE_SIZE/COMPLEXITY)));
		entity = sceneMgr->createGeo(name, name);
		entity->setMaterialName(CIRCLES_MATERIAL);
		node->attachObject(entity);
		tm = 0 ;
		lvl = 0 ;
		setTextureLevel();
	}
	~WaterCircle()
	{
		N_Only(Mesh).remove(mesh->getPrc());
		sceneMgr->destroyGeo(entity->getID());
		static_cast<SpaceNode *> (sceneMgr->getOriginNode())->remove(node->getName());
	}
	void animate(NIIf timeSinceLastFrame)
	{
		int lastlvl = lvl ;
		tm += timeSinceLastFrame ;
		lvl = (int) ((NIIf)(tm)/CIRCLE_TIME * 16);
		if(lvl < 16 && lvl != lastlvl) 
        {
			setTextureLevel();
		}
	}
	static void clearStaticBuffers()
	{
		posnormVertexBuffer = VertexBufferSPtr() ;
		indexBuffer = IndexBufferSPtr() ;
		for(int i = 0; i < 16; i++) 
        {
			texcoordsVertexBuffers[i] = VertexBufferSPtr() ;
		}
		delete [] texcoordsVertexBuffers;
	}
} ;
bool WaterCircle::first = true ;
VertexBufferSPtr WaterCircle::posnormVertexBuffer = VertexBufferSPtr() ;
IndexBufferSPtr WaterCircle::indexBuffer = IndexBufferSPtr() ;
VertexBufferSPtr* WaterCircle::texcoordsVertexBuffers = 0 ;

class _OgreSampleClassExport Sample_Water : public SdkSample
{
public:
    Sample_Water(): waterMesh(0)
    {
		mInfo["Title"] = "Water";
		mInfo["Description"] = "A demo of a simple water effect.";
		mInfo["Thumbnail"] = "thumb_water.png";
		mInfo["Category"] = "Environment";
    }

    virtual void _shutdown()
    {
        // cleanup all allocated circles
		for(unsigned i = 0 ; i < circles.size() ; i++)
        {
		    delete (circles[i]);
		}

        circles.clear();

        SdkSample::_shutdown();
    }

protected:
	WaterMesh * waterMesh ;
	GeometrySpace * waterEntity ;
	AnimationFusion * mAnimState;
    SpaceNode * headNode ;
    Overlay * waterOverlay ;
    ParticleSpace * particleSystem ;
    SpreadParticle * particleEmitter ;
    SpaceManager * sceneMgr ;

    // Just override the mandatory create scene method
    void setupContent(void)
    {
		sceneMgr = mSceneMgr ;
        // Set ambient light
        mSceneMgr->setAmbient(Colour(0.75, 0.75, 0.75));

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        // Accept default settings: point light, white diffuse, just set position
        // NB I could attach the light to a SpaceNode if I wanted it to move automatically with
        //  other objects, but I don't
        l->setPos(200,300,100);

		// Create water mesh and entity
		waterMesh = new WaterMesh(MESH_NAME, PLANE_SIZE, COMPLEXITY);
		waterEntity = mSceneMgr->createGeo(ENTITY_NAME,
                                              MESH_NAME);
		//~ waterEntity->setMaterialName(MATERIAL_NAME);
		SpaceNode *waterNode = mSceneMgr->getOriginNode()->create();
		waterNode->attachObject(waterEntity);

        // Add a head, give it it's own node
        headNode = waterNode->create();
        GeometrySpace *ent = mSceneMgr->createGeo("head", "ogrehead.mesh");
        headNode->attachObject(ent);

		// Create the camera node, set its position & attach camera
        SpaceNode * camNode = mSceneMgr->getOriginNode()->create();
		camNode->translate(Vector3f(0, 500, PLANE_SIZE));
		camNode->yaw(Angle(-45));
        camNode->attachObject(mCamera);

		// Create light node
        SpaceNode * lightNode = mSceneMgr->getOriginNode()->create();
		lightNode->attachObject(l);

        // set up spline animation of light node
        Animation * anim = mSceneMgr->createAnimation("WaterLight", 20);
		NodeKeyFrameTrack * track;
        NodeKeyFrame * key ;
		// create a random spline for light
        track = N_new NodeKeyFrameTrack(anim, 0);
        track->setAssociatedNode(lightNode);
		key = static_cast<NodeKeyFrame *>(track->create(0));
		for(int ff = 1; ff <= 19; ++ff)
        {
			key = static_cast<NodeKeyFrame *>(track->create(ff));
			Vector3 lpos(
                          rand()%(int)PLANE_SIZE , //- PLANE_SIZE/2,
                          rand()%300 + 100,
                          rand()%(int)PLANE_SIZE //- PLANE_SIZE/2
                          );
			key->setTranslate(lpos);
		}
		key = static_cast<NodeKeyFrame *>(track->create(20));
        track->buildSpline();
        // Create a new animation state to track this
        mAnimState = mSceneMgr->createFusion("WaterLight");
        mAnimState->setEnabled(true);

		// Let there be rain
        particleSystem = mSceneMgr->createParticleSpace("rain", "Examples/Water/Rain");
		particleEmitter = particleSystem->getSpread(0);
        SpaceNode* rNode = mSceneMgr->getOriginNode()->create();
        rNode->translate(Vector3f(PLANE_SIZE/2.0f, 3000, PLANE_SIZE/2.0f));
        rNode->attachObject(particleSystem);
        // Fast-forward the rain so it looks more natural
        particleSystem->_forward(20);
		// It can't be set in .particle file, and we need it ;)
		static_cast<ParticleCtrlGeo*>(particleSystem->getRenderer())->setOriginType(OT_Bottom_Center);

		prepareCircleMaterial();

		setupControls();

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		setDragLook(true);
#endif

		timeoutDelay = 0.0f;
	}

#define PANEL_WIDTH 200
	void setupControls()
	{
		mTrayMgr->createLabel(TL_TOPLEFT, "GeneralLabel", "General", PANEL_WIDTH);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "FakeNormalsCB", "Fake normals", PANEL_WIDTH);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "SkyboxCB", "Skybox", PANEL_WIDTH);
		mTrayMgr->createThickSlider(TL_TOPLEFT, "HeadDepthSlider", "Head Depth", PANEL_WIDTH, 80, 1, 3, 50)->setValue(2.0f);
		SelectMenu* waterMaterial = mTrayMgr->createThickSelectMenu(TL_TOPLEFT, "WaterMaterialMenu", "Water material", PANEL_WIDTH, 9);
		for (size_t i = 0; i < 9; i++)
		{
			waterMaterial->addItem(MATERIAL_PREFIX + StrConv::conv(i));
		}
		waterMaterial->selectItem(8);
		mTrayMgr->createLabel(TL_TOPLEFT, "RainLabel", "Rain : [Space]", PANEL_WIDTH);

		mTrayMgr->createLabel(TL_TOPRIGHT, "AdvancedLabel", "Advanced", PANEL_WIDTH);
		mTrayMgr->createThickSlider(TL_TOPRIGHT, "RippleSpeedSlider", "Ripple Speed", PANEL_WIDTH, 80, 0, 2, 50)->setValue(0.3, false);
		mTrayMgr->createThickSlider(TL_TOPRIGHT, "DistanceSlider", "Distance", PANEL_WIDTH, 80, 0.1, 5.0, 50)->setValue(0.4, false);
		mTrayMgr->createThickSlider(TL_TOPRIGHT, "ViscositySlider", "Viscosity", PANEL_WIDTH, 80, 0, 1, 50)->setValue(0.05, false);
		mTrayMgr->createThickSlider(TL_TOPRIGHT, "FrameTimeSlider", "FrameTime", PANEL_WIDTH, 80, 0, 1, 61)->setValue(0.13, false);

		mTrayMgr->showCursor();
	}

	void cleanupContent()
	{
		// If when you finish the application is still raining there
 		// are water circles that are still being processed
 		unsigned int activeCircles = this->circles.size ();

 		// Kill the active water circles
 		for (unsigned int i = 0; i < activeCircles; i++)
 			delete (this->circles[i]);

		delete waterMesh;
		waterMesh = 0;
	}

protected:
	NIIf timeoutDelay ;

#define RAIN_HEIGHT_RANDOM 5
#define RAIN_HEIGHT_CONSTANT 5


	typedef vector<WaterCircle*>::type WaterCircles ;
	WaterCircles circles ;

	void processCircles(NIIf timeSinceLastFrame)
	{
		for(unsigned int i=0;i<circles.size();i++) 
        {
			circles[i]->animate(timeSinceLastFrame);
		}
		bool found ;
		do {
			found = false ;
			for(WaterCircles::iterator it = circles.begin() ;
                it != circles.end();
                ++it) {
				if ((*it)->lvl>=16) {
					delete (*it);
					circles.erase(it);
					found = true ;
					break ;
				}
			}
		} while (found) ;
	}

	void processParticles()
	{
		static int pindex = 0;
        ParticleList::iterator i, iend = particleSystem->getList().end();
		for(i = particleSystem->getList().begin(); i != iend; ++i) 
        {
			Particle * particle = *i;
			Vector3 ppos = particle->position;
			if (ppos.y<=0 && particle->mRemainTTL>0) { // hits the water!
				// delete particle
				particle->mRemainTTL = 0.0f;
				// push the water
				float x = ppos.x / PLANE_SIZE * COMPLEXITY ;
				float y = ppos.z / PLANE_SIZE * COMPLEXITY ;
				float h = rand() % RAIN_HEIGHT_RANDOM + RAIN_HEIGHT_CONSTANT ;
				if (x<1) x=1 ;
				if (x>COMPLEXITY-1) x=COMPLEXITY-1;
				if (y<1) y=1 ;
				if (y>COMPLEXITY-1) y=COMPLEXITY-1;
				waterMesh->push(x,y,-h) ;
				WaterCircle *circle = new WaterCircle(mSceneMgr,
                                                      "Circle#"+StrConv::conv(pindex++),
                                                      x, y);
				circles.push_back(circle);
			}
		}
	}

	void sliderMoved(Slider* slider)
	{
		if (slider->getName() == "HeadDepthSlider")
		{
			headDepth = slider->getValue();
		}
		else if (slider->getName() == "RippleSpeedSlider")
		{
			waterMesh->PARAM_C = slider->getValue();
		}
		else if (slider->getName() == "DistanceSlider")
		{
			waterMesh->PARAM_D = slider->getValue();
		}
		else if (slider->getName() == "ViscositySlider")
		{
			waterMesh->PARAM_U = slider->getValue();
		}
		else if (slider->getName() == "FrameTimeSlider")
		{
			waterMesh->PARAM_T = slider->getValue();
		}
	}

	void checkBoxToggled(CheckBox* checkBox)
	{
		if (checkBox->getName() == "FakeNormalsCB")
		{
			waterMesh->useFakeNormals = checkBox->isChecked();
		}
		else if (checkBox->getName() == "SkyboxCB")
		{
            BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
            mBoxSky->setMaterial();
            mBoxSky->setRenderGroup(RQG_Sky);
			sceneMgr->setSkyBox(checkBox->isChecked(), "Examples/SceneSkyBox2");
		}
	}

	void itemSelected(SelectMenu* menu)
	{
		//Only one menu in this demo
		const String& materialName = menu->getSelectItem();
		MaterialPtr material = MaterialManager::getSingleton().getByName(materialName);
		if (material.isNull())
        {
			N_EXCEPT(Exception::ERR_INTERNAL_ERROR,
                        "Material "+materialName+"doesn't exist!",
                        "WaterListener::updateMaterial");
		}
		waterEntity->setMaterialName(materialName);
	}

	/** Head animation */
	NIIf headDepth ;
	void animateHead(NIIf timeSinceLastFrame)
	{
		// sine track? :)
		static double sines[4] = {0,100,200,300};
		static const double adds[4] = {0.3,-1.6,1.1,0.5};
		static Vector3 oldPos = Vector3::Z;
		for(int i = 0; i < 4; i++) 
        {
			sines[i]+=adds[i]*timeSinceLastFrame;
		}
		NIIf tx = ((sin(sines[0]) + sin(sines[1])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
		NIIf ty = ((sin(sines[2]) + sin(sines[3])) / 4 + 0.5 ) * (float)(COMPLEXITY-2) + 1 ;
		waterMesh->push(tx,ty, -headDepth);
		NIIf step = PLANE_SIZE / COMPLEXITY;
        headNode->set(Vector3f::ZERO, Vector3f::UNIT, Quaternion::IDENTITY);
		headNode->scale(Vector3f(3, 3, 3));
		Vector3 newPos = Vector3(step*tx, headDepth, step*ty);
		Vector3 diffPos = newPos - oldPos ;
		Quaternion headRotation = Vector3::Z.getRotation(diffPos);
		oldPos = newPos ;
		headNode->translate(newPos);
		headNode->rotate(headRotation);
	}
public:
    bool onRenderCore(const RenderFrameArgs * evt)
    {
		if( SdkSample::onRenderCore(evt) == false )
		{
			// check if we are exiting, if so, clear static HardwareBuffers to avoid segfault
			WaterCircle::clearStaticBuffers();
			return false;
		}

        mAnimState->addTime(evt->mCurrent);

		// rain
		processCircles(evt->mCurrent);
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
		if (mKeyboard->isKeyDown(OIS::KC_SPACE)) {
			particleEmitter->setSpreadRate(20.0f);
		} else {
			particleEmitter->setSpreadRate(0.0f);
		}
#endif
		processParticles();


		timeoutDelay-=evt->mCurrent ;
		if (timeoutDelay<=0)
			timeoutDelay = 0;

		animateHead(evt->mCurrent);

		waterMesh->updateMesh(evt->mCurrent);

		return true;
    }
};
#endif