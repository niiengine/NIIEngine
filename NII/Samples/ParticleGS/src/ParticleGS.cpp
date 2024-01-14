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
/*
-----------------------------------------------------------------------------
Filename:    ParticleGS.cpp
Description: Demonstrates the use of the geometry shader and render to vertex
	buffer to create a particle system that is entirely calculated on the GPU.
	Partial implementation of ParticlesGS example from Microsoft's DirectX 10
	SDK : http://msdn.microsoft.com/en-us/library/ee416421.aspx
-----------------------------------------------------------------------------
*/

#include "ProceduralCustomSpaceObj.h"
#include "OgreRenderToVertexBuffer.h"
#include "RandomTools.h"
#include "SamplePlugin.h"
#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

//#define LOG_GENERATED_BUFFER
const Vector3 GRAVITY_VECTOR = Vector3(0, -9.8, 0);

#ifdef LOG_GENERATED_BUFFER
struct FireworkParticle 
{
	float pos[3];
	float timer;
	float type;
	float vel[3];
};
#endif

class _OgreSampleClassExport Sample_ParticleGS : public SdkSample
{
public:
    Sample_ParticleGS() 
	{
		mInfo["Title"] = "Geometry Shader Particle System";
		mInfo["Description"] = "A demo of particle systems using geometry shaders and render to vertex buffers.";
		mInfo["Thumbnail"] = "thumb_particlegs.png";
		mInfo["Category"] = "Effects";
    }

protected:

	void createProceduralParticleSpace()
	{
		mParticleSpace = static_cast<ProceduralCustomSpaceObj*>
			(mSceneMgr->createSpace("ParticleGSEntity", ProceduralCustomSpaceObjFactory::FACTORY_TYPE_NAME));
		mParticleSpace->setMaterial("Ogre/ParticleGS/Display");

		//Generate the geometry that will seed the particle system
		CustomSpaceObj* particleSystemSeed = mSceneMgr->createCustomGeo("ParticleSeed");
		//This needs to be the initial launcher particle
		particleSystemSeed->begin(GeometryRaw::OT_POINT_LIST, "Ogre/ParticleGS/Display");
		particleSystemSeed->position(0,0,0); //Position
		particleSystemSeed->textureCoord(1); //Timer
		particleSystemSeed->textureCoord(0); //Type
		particleSystemSeed->textureCoord(0,0,0); //Velocity
		particleSystemSeed->end();

		//Generate the RenderToBufferObject
		DisplayBuffer * r2vbObject;
        N_Only(GBuffer).create(r2vbObject);
		r2vbObject->setMaterialID("Ogre/ParticleGS/Generate");
		
		//Apply the random texture
		TexturePtr randomTexture = RandomTools::generateRandomVelocityTexture();
		r2vbObject->getMaterial()->getLOD()->get(0)->
			getTexture().get("RandomTexture")->setSrc(
                randomTexture->getName(), randomTexture->getType());

		r2vbObject->setGeoOp(GeometryRaw::OT_POINT_LIST);
		r2vbObject->setMaxVertexCount(16000);
		r2vbObject->setAutoFlush(false);
		VertexData * vd = r2vbObject->getData();
		size_t offset = 0;
		offset += vd->add(DT_Float3x, VT_Pos, 0, offset)->getSize(); //Position
		offset += vd->add(DT_Float1x, VT_Tex_Coord, 0, offset, 0)->getSize(); //Timer
		offset += vd->add(DT_Float1x, VT_Tex_Coord, 0, offset, 1)->getSize(); //Type
		vd->add(DT_Float3x, VT_Tex_Coord, 0, offset, 2)->getSize(); //Velocity
		
		//Bind the two together
		mParticleSpace->setRenderToVertexBuffer(r2vbObject);
		mParticleSpace->setCustomSpaceObj(particleSystemSeed);

		//Set bounds
		AABox aabb;
		aabb.setMin(-100,-100,-100);
		aabb.setMax(100,100,100);
		mParticleSpace->setAABB(aabb);
	}

    void testCapabilities(const RenderSysFeature * caps)
    {
        if(!caps->hasCapability(GF_Program_Geometry))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your render system / hardware does not support geometry programs, "
                "so you cannot run this sample. Sorry!");
        }
		if(!caps->hasCapability(GF_Render_To_Vertex))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your render system / hardware does not support render to vertex buffers, "
                "so you cannot run this sample. Sorry!");
        }
    }
    
	void setupContent(void)
    {
        demoTime = 0;

        mProceduralCustomSpaceObjFactory = N_new ProceduralCustomSpaceObjFactory();
        Root::getSingleton().addFactory(mProceduralCustomSpaceObjFactory);

		createProceduralParticleSpace();

		static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create())->attachObject(mParticleSpace);
		mCamera->setPos(NII::Vector3f(0, 35, -100));
		mCamera->lookAt(NII::Vector3f(0, 35, 0));
		
		//Add an ogre head to the scene
		SpaceNode* ogreHeadSN = mSceneMgr->getOriginNode()->create();
		GeometrySpace *ogreHead = mSceneMgr->createGeo("head", "ogrehead.mesh");        
		ogreHeadSN->scale(Vector3f(0.1,0.1,0.1));
		ogreHeadSN->yaw(Angle(180));
		ogreHeadSN->attachObject(ogreHead);
		
		//Add a plane to the scene
		Plane plane;
		plane.normal = Vector3::Y;
		plane.d = 100;
		MeshManager::getSingleton().createPlane("Myplane", GroupDefault, plane,
			1500,1500,20,20,true,1,60,60,Vector3::Z);
		GeometrySpace * pPlaneEnt = mSceneMgr->createGeo( "plane", "Myplane" );
		pPlaneEnt->setMaterialName("Examples/Rockwall");
		pPlaneEnt->setCastShadow(false);
		static_cast<SpaceNode *>(mSceneMgr->getOriginNode()->create(Vector3(0,95,0)))->attachObject(pPlaneEnt);
    }

    void cleanupContent()
    {
        Root::getSingleton().removeFactory(mProceduralCustomSpaceObjFactory);
        N_delete mProceduralCustomSpaceObjFactory;
        mProceduralCustomSpaceObjFactory = 0;

        MeshManager::getSingleton().remove("Myplane");
    }

	bool onPreRender(const RenderFrameArgs * evt) 
	{ 
		//Set shader parameters
		GpuProgramParam * geomParams;
        mParticleSpace->getRenderToVertexBuffer()->getMaterial()->
            getLOD()->get(0)->getProgram().get(0, ST_GS, NULL, geomParams);
        if(geomParams->getDefine() != 0)
        {
            getDefine()->get(name);
            if(geomParams->getDefine()->get("elapsedTime"))
            {
                geomParams->set("elapsedTime", evt->mCurrent);
            }		
            demoTime += evt->mCurrent;
            if(geomParams->getDefine()->get("globalTime"))
            {
                geomParams->set("globalTime", demoTime);
            }
            if(geomParams->getDefine()->get("frameGravity"))
            {
                geomParams->set("frameGravity", GRAVITY_VECTOR * evt->mCurrent);
            }
        }
		return SdkSample::onPreRender(evt); 
	}

#ifdef LOG_GENERATED_BUFFER
	bool onEndRender(const RenderFrameArgs * evt) 
	{ 
		//This will only work if the vertex buffer usage is dynamic (see R2VB implementation)
		LogManager::getSingleton().getDefaultLog()->stream() << 
			"Particle system for frame " <<	Root::getSingleton().getCurrentFrame();
		GeometryRaw renderOp;
		mParticleSpace->getRenderToVertexBuffer()->getGeometry(renderOp);
		const VertexBufferSPtr & vertexBuffer = renderOp.mVertex->getBuffer(0);
		
		assert(vertexBuffer->getUnitSize() == sizeof(FireworkParticle));
		FireworkParticle * particles = static_cast<FireworkParticle *>
			(vertexBuffer->lock(Buffer::MM_READ));
		
		for(size_t i = 0; i < renderOp.mVertex->mCount; ++i)
		{
			FireworkParticle& p = particles[i];
			LogManager::getSingleton().getDefaultLog()->stream() <<
				"FireworkParticle " << i+1 << " : " <<
				"Position : " << p.pos[0] << " " << p.pos[1] << " " << p.pos[2] << " , " <<
				"Timer : " << p.timer << " , " <<
				"Type : " << p.type << " , " <<
				"Velocity : " << p.vel[0] << " " << p.vel[1] << " " << p.vel[2];
		}
		
		vertexBuffer->unlock();
		return SdkSample::onEndRender(evt); 
	}
#endif
    
    NIIf demoTime;
    ProceduralCustomSpaceObj* mParticleSpace;
    ProceduralCustomSpaceObjFactory *mProceduralCustomSpaceObjFactory;
};

#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_ParticleGS;
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

