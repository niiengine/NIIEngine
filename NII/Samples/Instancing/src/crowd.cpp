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

/**
    \file
        Crowd.cpp
    \brief
        Shows OGRE's bezier Crowd feature
*/

#include "crowd.h"
inline Ogre::String operator +(const Ogre::String & l,const CEGUI::String & o)
{
	return l + o.c_str();
}
/*
inline CEGUI::String operator +(const CEGUI::String& l,const Ogre::String& o)
{
	return l+o.c_str();
}
*/

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char **argv)
#endif
{
    // Create application object
    CrowdApplication app;

    try {
        app.go();
    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL );
#else
        std::cerr << "An exception has occurred: " << e.getFullDescription();
#endif
    }


    return 0;
}
#ifdef __cplusplus
}
#endif

	CrowdListener::CrowdListener(ViewWindow* win, Camera* cam,CEGUI::Renderer* renderer, CrowdApplication*main)
		: ExampleFrameListener(win, cam,false,true),
		mRequestShutDown(false),
		mLMBDown(false),
		mRMBDown(false),
		mAvgFrameTime(0.1),
		mMain(main),
		mBurnAmount(0)
	{
		const GpuProgramManager::SyntaxCodes &syntaxCodes = GpuProgramManager::getSingleton().getSupportMark();
		for (GpuProgramManager::SyntaxCodes::const_iterator iter = syntaxCodes.begin();iter != syntaxCodes.end();++iter)
		{
				LogManager::getSingleton().logMessage("supported syntax : "+(*iter));
		}
		mGUIRenderer=renderer;
		numMesh = 160;
		numRender = 0;
		meshSelected = 0;
		currentGeomOpt = INSTANCE_OPT;
		createCurrentGeomOpt();

		mMouse->setEventCallback(this);
		mKeyboard->setEventCallback(this);

		mGuiAvg   = CEGUI::WindowManager::getSingleton().getWindow("OPAverageFPS");
		mGuiCurr  = CEGUI::WindowManager::getSingleton().getWindow("OPCurrentFPS");
		mGuiBest  = CEGUI::WindowManager::getSingleton().getWindow("OPBestFPS");
		mGuiWorst = CEGUI::WindowManager::getSingleton().getWindow("OPWorstFPS");
		mGuiTris  = CEGUI::WindowManager::getSingleton().getWindow("OPTriCount");
		mGuiDbg   = CEGUI::WindowManager::getSingleton().getWindow("OPDebugMsg");
		mRoot	  = CEGUI::WindowManager::getSingleton().getWindow("root");
		mDebugOverlay->hide();
		timer = new Ogre::Timer();
		mLastTime = timer->getUS()/1000000.0f;

	}
	//-----------------------------------------------------------------------
	CrowdListener::~CrowdListener()
	{
		destroyCurrentGeomOpt();
		delete timer;
	}
	//-----------------------------------------------------------------------
	bool CrowdListener::onRenderCore(const RenderFrameArgs * evt)
	{
		burnCPU();
		vector <AnimationFusion*>::type::iterator it;
		for(it=animations.begin();it!=animations.end();it++)
		{
			(*it)->addTime(evt->mCurrent);
		}

		updateStats();

		if(mRequestShutDown)
			return false;
		const bool returnValue = ExampleFrameListener::onRenderCore(evt);
		// Call default
		return returnValue;
	}
	//-----------------------------------------------------------------------
	void CrowdListener::burnCPU(void)
	{
	    double mStartTime = timer->getUS()/1000000.0f; //convert into seconds
		double mCurTime =  mStartTime;
		double mStopTime = mLastTime + mBurnAmount;
		double mCPUUsage;

		while( mCurTime < mStopTime )
		{
			mCurTime = timer->getUS()/1000000.0f; //convert into seconds
		}

		if( mCurTime - mLastTime > 0.00001f )
			mCPUUsage = (mCurTime - mStartTime) / (mCurTime - mLastTime) * 100.0f;
		else
			mCPUUsage = FLT_MAX;

		mLastTime = timer->getUS()/1000000.0f; //convert into seconds
		int time = mCPUUsage+0.5f;
		if(mTimeUntilNextToggle<=0)
		{
			//mDebugText="remaining for logic:"+ StrConv::conv(time);
			mTimeUntilNextToggle=1;
		}

	}
	//-----------------------------------------------------------------------
	void CrowdListener::destroyCurrentGeomOpt()
	{
		switch(currentGeomOpt)
		{
			case INSTANCE_OPT:destroyInstanceGeom();break;
			case ENTITY_OPT: destroyEntityGeom ();break;
		}
	}
	//-----------------------------------------------------------------------
	void CrowdListener::createCurrentGeomOpt()
	{
		LogManager::getSingleton().logMessage("geom deleted");
		objectCount=numMesh;
		numRender=1;
		while(objectCount>maxObjectsPerBatch)
		{
			numRender++;
			objectCount-=maxObjectsPerBatch;
		}

		assert (meshSelected < numTypeMeshes);
		MeshPtr m = MeshManager::getSingleton ().getByName ("robot.mesh");
		if (m.isNull ())
		{
			m = N_Only(Mesh).load("robot.mesh",  GroupUnknow);
		}
		NIIf radius = m->getCenterRange();

		// could/should print on screen mesh name,
		//optimisation type,
		//mesh vertices num,
		//32 bit or not,
		//etC..


		switch(currentGeomOpt)
		{
			case INSTANCE_OPT:createInstanceGeom();break;
			case ENTITY_OPT: createEntityGeom ();break;
		}

	}
	//-----------------------------------------------------------------------
	void CrowdListener::createInstanceGeom()
	{
		if (Root::getSingleton ().getRender ()->getFeature ()->hasCapability (GF_Program_Vertex) == false)
		{
			N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Your video card doesn't support batching", "Demo_Instance::createScene");
		}

		GeometrySpace *ent = mCamera->getManager()->createGeo("robot","robot.mesh");
		setupInstancedMaterialToEntity(ent);

		renderInstance.reserve(numRender);
		renderInstance.resize(numRender);

		//Load a mesh to read data from.
		InstanceSpace* batch = new InstanceSpace(mCamera->getManager(),
			"robots" );
		batch->setCastShadow(true);

        size_t i, k;

		batch->setBatchInstanceDimensions (Vector3(1000000, 1000000, 1000000));
		const size_t batchSize = (numMesh > maxObjectsPerBatch) ? maxObjectsPerBatch :numMesh;
		for(i = 0; i < batchSize ; i++)
		{
			batch->addEntity(ent, Vector3::ZERO);
		}
		batch->setOrigin(Vector3::ZERO);

		batch->build();


		for (k = 0; k < numRender-1; k++)
		{
			batch->addBatchInstance();
		}

		i = 0,k = 0;
		InstanceSpace::BatchInstanceIterator regIt = batch->getBatchInstanceIterator();
		size_t baseIndexForBatch = 0;
		//create a RayQuery to get the terrain height
		RaySpaceQuery* raySpaceQuery=mCamera->getManager()->createRayQuery(Ray(Vector3(0,0,0), Vector3::INV_Y));
		RaySpaceQueryResult::iterator it;
		RaySpaceQueryResult& qryResult=raySpaceQuery->execute();
		while (regIt.isNextValid ())
		{

			InstanceSpace::BatchInstance *r = regIt.getAndNext();

			InstanceSpace::BatchInstance::ObjectList bit = r->getObjectList();
			while(bit.isNextValid())
			{
				InstanceSpace::InstancedObject* obj = bit.getAndNext();
				Vector3 position;

				position.x=10*k+500;
				position.z=10*i+500;
				position.y=0;
				raySpaceQuery->setRay (Ray(position, Vector3::Y));
				raySpaceQuery->execute();
                it = qryResult.begin();

                if (it != qryResult.end() && it->worldFragment)
					position.y =  it->worldFragment->mPoint.y;

				obj->setPos(position);
				obj->setScale(Vector3(0.1,0.1,0.1));
				AnimationFusion * anim=obj->getFusion("Walk");
				animations.push_back(anim);
				//offset the animation time, to show that all objects are independently animated.
				anim->setTimePosition(i+k);
				anim->setEnabled(true);
				k++;
			}

			if(k>14)
			{
				k=0;
			    i++;
			}

		}
		batch->setVisible(true);
		renderInstance[0] = batch;
		mCamera->getManager()->destroyQuery(raySpaceQuery);
		mCamera->getManager()->destroyGeo (ent->getID());
	}
	//-----------------------------------------------------------------------
	void CrowdListener::destroyInstanceGeom()
	{
		delete renderInstance[0];
		renderInstance.clear();
		animations.clear();
	}
    
	void CrowdListener::setupInstancedMaterialToEntity(GeometrySpace*ent)
	{
		for (Ogre::uint i = 0; i < ent->getNumSubEntities(); ++i)
		{
			SubEntity* se = ent->getCom(i);
			ResourceID materialName= se->getMaterialID();
			se->setMaterialName(buildCrowdMaterial(materialName, ent->getSkeleton()->getCount()));
		}
	}
    
	String CrowdListener::buildCrowdMaterial(const String &originalMaterialName,int numBones)
	{

		// already instanced ?
		if (StringUtil::endsWith (originalMaterialName, "/instanced"))
			return originalMaterialName;

		MaterialPtr originalMaterial = MaterialManager::getSingleton ().getByName (originalMaterialName);

		// if originalMat doesn't exists use "Instancing" material name
		const String instancedMaterialName (originalMaterial.isNull() ? "Instancing" : originalMaterialName + "/Instanced");
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
                p->getProgram().set(ST_VS, 0, "Crowd");
                p->getProgram().set(ST_VS_SC, 0, "CrowdShadowCaster");
			}
		}
		instancedMaterial->load();
		return instancedMaterialName;
	}
	//-----------------------------------------------------------------------
	void CrowdListener::createEntityGeom()
	{
		size_t k = 0;
		size_t y = 0;
		renderEntity.reserve (numMesh);
		renderEntity.resize (numMesh);
		nodes.reserve (numMesh);
		nodes.resize (numMesh);
		//create a RayQuery to get the terrain height
		RaySpaceQuery * raySpaceQuery=mCamera->getManager()->createRayQuery(Ray(Vector3(0,0,0), Vector3::INV_Y));
		RaySpaceQueryResult::iterator it;
		RaySpaceQueryResult& qryResult=raySpaceQuery->execute();
		for(size_t i = 0; i < numMesh; i++)
		{
			Vector3 position;
			position.x = 10*y+500;
			position.y = 0;
			position.z = 10*k+500;

			raySpaceQuery->setRay (Ray(position, Vector3::Y));
			raySpaceQuery->execute();
            it = qryResult.begin();

            if(it != qryResult.end() && it->worldFragment)
				position.y =  it->worldFragment->mPoint.y;

			nodes[i]=mCamera->getManager()->getOriginNode()->create("node"+StrConv::conv(i));
			LogManager::getSingleton().logMessage(":"+nodes[i]->getName());
			renderEntity[i]=mCamera->getManager()->createGeo("robot"+StrConv::conv(i), "robot.mesh");
	    	nodes[i]->attachObject(renderEntity[i]);
			nodes[i]->setPos(position);
			nodes[i]->setScale(Vector3(0.1,0.1,0.1));
			AnimationFusion*anim=renderEntity[i]->getFusion("Walk");
			animations.push_back(anim);
			anim->setTimePosition(y+k);
			anim->setEnabled(true);
			y++;
			if (y>14)
			{
				y=0;
				k++;
			}
		}
		mCamera->getManager()->destroyQuery(raySpaceQuery);


	}
	//-----------------------------------------------------------------------
	void CrowdListener::destroyEntityGeom()
	{
		size_t i;
		size_t j=0;
		for (i=0;i<numMesh;i++)
		{
			LogManager::getSingleton().logMessage(" " +nodes[i]->getName());
			LogManager::getSingleton().logMessage(StrConv::conv(j)+":"+StrConv::conv(j<numMesh));
			String name=nodes[i]->getName();
			mCamera->getManager()->destroySpaceNode(name);
			mCamera->getManager()->destroyGeo(renderEntity[i]->getID());
			j++;
		}
		animations.clear();



	}
	//-----------------------------------------------------------------------
	bool CrowdListener::mouseMoved ( const OIS::MouseEvent &arg )
	{

		CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
		return true;
	}
	//-----------------------------------------------------------------------
   bool CrowdListener::mousePressed ( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
   {
       CEGUI::System::getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id));
		return true;
   }
	//-----------------------------------------------------------------------
   bool CrowdListener::mouseReleased (const OIS::MouseEvent &arg, OIS::MouseButtonID id)
   {
	   CEGUI::System::getSingleton().injectMouseButtonUp(convertOISMouseButtonToCegui(id));
		return true;
   }

   void CrowdListener::requestShutdown(void)
   {
	   mRequestShutDown=true;
   }
   void CrowdListener::setCurrentGeometryOpt(CurrentGeomOpt opt)
   {
	   currentGeomOpt=opt;
   }
   bool CrowdListener::handleMouseMove(const CEGUI::EventArgs& e)
   {
		using namespace CEGUI;

		if(mLMBDown)
		{
			int a =0;
			// rotate camera
			mRotX += Ogre::Angle(-((const MouseEventArgs&)e).moveDelta.d_x * mAvgFrameTime * 10.0);
			mRotY += Ogre::Angle(-((const MouseEventArgs&)e).moveDelta.d_y * mAvgFrameTime * 10.0);
			mCamera->yaw(mRotX);
			mCamera->pitch(mRotY);
			MouseCursor::getSingleton().setPos( mLastMousePosition );
		}
        return true;
	}
	//--------------------------------------------------------------------------
	bool CrowdListener::handleMouseButtonUp(const CEGUI::EventArgs & e)
	{
		using namespace CEGUI;

		if(((const MouseEventArgs&)e).button == LeftButton )
		{
			mLMBDown = false;
			MouseCursor::getSingleton().setPos(mLastMousePosition);
			CEGUI::MouseCursor::getSingleton().show();
		}
		return true;
	}
	//--------------------------------------------------------------------------
	bool CrowdListener::handleMouseButtonDown(const CEGUI::EventArgs & e)
	{
		using namespace CEGUI;

		if(((const MouseEventArgs&)e).button == LeftButton)
		{
			mLMBDown = true;
			mLastMousePosition=CEGUI::MouseCursor::getSingleton().getPos();
			CEGUI::MouseCursor::getSingleton().hide();
		}
		return true;
	}
	//--------------------------------------------------------------------------
	void CrowdListener::updateStats(void)
	{
		static CEGUI::String currFps = "Current FPS: ";
		static CEGUI::String avgFps = "Average FPS: ";
		static CEGUI::String bestFps = "Best FPS: ";
		static CEGUI::String worstFps = "Worst FPS: ";
		static CEGUI::String tris = "Triangle Count: ";

		const NII::FramesPerSecond & stats = mMain->getRenderWindow()->getFPS();

		mGuiAvg->setText(avgFps + Ogre::StrConv::conv(stats.mAvg));
		mGuiCurr->setText(currFps + Ogre::StrConv::conv(stats.mLast));
		mGuiBest->setText(bestFps + Ogre::StrConv::conv(stats.mBest)
			+ " " + Ogre::StrConv::conv(stats.mBestFrame)+" ms");
		mGuiWorst->setText(worstFps + Ogre::StrConv::conv(stats.mWorst)
			+ " " + Ogre::StrConv::conv(stats.mWorstFrame)+" ms");

		mGuiTris->setText(tris + Ogre::StrConv::conv(mMain->getRenderWindow()->mTriCount));
		mAvgFrameTime = 1.0f / (stats.mAvg + 1.0f);
		if(mAvgFrameTime > 0.1f) 
            mAvgFrameTime = 0.1f;
	}