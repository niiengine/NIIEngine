#include "OgreDemoApp.h"

DemoApp::DemoApp()
{
	m_pCubeNode		= 0;
	m_pCubeEntity   = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::~DemoApp()
{
#ifdef USE_RTSHADER_SYSTEM
    mShaderGenerator->removeSceneManager(OgreFramework::getSingletonPtr()->m_pSceneMgr);

    finalizeRTShaderSystem();
#endif

    delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#ifdef USE_RTSHADER_SYSTEM

/*-----------------------------------------------------------------------------
 | Initialize the RT Shader system.
 -----------------------------------------------------------------------------*/
bool DemoApp::initializeRTShaderSystem(Ogre::SpaceManager* sceneMgr)
{
    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
        mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

        mShaderGenerator->addSceneManager(sceneMgr);

        // Setup core libraries and shader cache path.
        Ogre::GroupIDList groupVector = N_Only(ResourceScheme).getGroups();
        Ogre::GroupIDList::iterator itGroup, itGroupEnd = groupVector.end();
        Ogre::String shaderCoreLibsPath;
        Ogre::String shaderCachePath;

        for(itGroup = groupVector.begin(); itGroup != itGroupEnd; ++itGroup)
        {
            bool coreLibsFound = false;

            // Try to find the location of the core shader lib functions and use it
            // as shader cache path as well - this will reduce the number of generated files
            // when running from different directories.
            VFS * vfs = N_Only(ResourceManager).getVFS("RTShaderLib", *itGroup);
            if(vfs)
            {
                shaderCoreLibsPath = vfs->getName() + "/";
                shaderCachePath = shaderCoreLibsPath;
                coreLibsFound = true;
                break;
            }
            // Core libs path found in the current group.
            if (coreLibsFound)
                break;
        }

        // Core shader libs not found -> shader generating will fail.
        if(shaderCoreLibsPath.empty())
            return false;

        // Create and register the material manager listener.
        mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
        Ogre::MaterialManager::getSingleton().add(mMaterialMgrListener);
    }
    return true;
}
/*-----------------------------------------------------------------------------
 | Finalize the RT Shader system.
 -----------------------------------------------------------------------------*/
void DemoApp::finalizeRTShaderSystem()
{
    // Restore default scheme.
    NII::N_Only(Material).setCurrent(0);

    // Unregister the material manager listener.
    if(mMaterialMgrListener != NULL)
    {
        Ogre::MaterialManager::getSingleton().remove(mMaterialMgrListener);
        delete mMaterialMgrListener;
        mMaterialMgrListener = NULL;
    }

    // Finalize RTShader system.
    if(mShaderGenerator != NULL)
    {
        Ogre::RTShader::ShaderGenerator::finalize();
        mShaderGenerator = NULL;
    }
}
#endif // USE_RTSHADER_SYSTEM

void DemoApp::startDemo()
{
	new OgreFramework();
	if(!OgreFramework::getSingletonPtr()->initOgre("DemoApp v1.0", this, 0))
		return;

	m_bShutdown = false;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

#ifdef USE_RTSHADER_SYSTEM
    initializeRTShaderSystem(OgreFramework::getSingletonPtr()->m_pSceneMgr);
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", NII::GroupInner);
    baseWhite->setLight(false);
    mShaderGenerator->createShaderBasedTechnique("BaseWhite",
        0, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
        "BaseWhite");
    GpuProgram * prog;
    baseWhite->get(1)->get(0)->getProgram().get(0, ST_VS, prog);
    baseWhite->get(0)->get(0)->getProgram().set(ST_VS, 0, prog->getOriginID());
    baseWhite->get(1)->get(0)->getProgram().get(0, ST_FS, prog);
    baseWhite->get(0)->get(0)->getProgram().set(ST_FS, 0, prog->getOriginID());

    // creates shaders for base material MaterialManager::WhiteColour using the RTSS
    mShaderGenerator->createShaderBasedTechnique(MaterialManager::WhiteColour->getOriginID(),
        0, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
        MaterialManager::WhiteColour->getOriginID());
    Ogre::Material * baseWhiteNoLighting = MaterialManager::WhiteColour;
    baseWhiteNoLighting->get(1)->get(0)->getProgram().get(0, ST_VS, prog);
    baseWhiteNoLighting->get(0)->get(0)->getProgram().set(ST_VS, 0, prog->getOriginID());
    baseWhiteNoLighting->get(1)->get(0)->getProgram().get(0, ST_FS, prog);
    baseWhiteNoLighting->get(0)->get(0)->getProgram().set(ST_FS, 0, prog->getOriginID());
#endif

	setupDemoScene();
#if !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	runDemo();
#endif
}
//|||||||||||||||||||||||||||||||||||||||||||||||
void DemoApp::setupDemoScene()
{
    BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
    mBoxSky->setMaterial();
    mBoxSky->setRenderGroup(RQG_Sky);
	OgreFramework::getSingletonPtr()->m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("Light")->setPos(Vecotr3f(75, 75, 75));

	m_pCubeEntity = OgreFramework::getSingletonPtr()->m_pSceneMgr->createGeo("Cube", "ogrehead.mesh");
	m_pCubeNode = OgreFramework::getSingletonPtr()->m_pSceneMgr->getOriginNode()->create("CubeNode");
	m_pCubeNode->attachObject(m_pCubeEntity);
}
//|||||||||||||||||||||||||||||||||||||||||||||||
void DemoApp::runDemo()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");

	double timeSinceLastFrame = 0;
	double startTime = 0;

    OgreFramework::getSingletonPtr()->m_pRenderWnd->resetFPS();

#if (!defined(OGRE_IS_IOS)) && !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
	while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown())
	{
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		Ogre::WindowUtil::messagePump();
#endif
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->m_pTimer->getCostMS();

#if !OGRE_IS_IOS
			OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
#endif
			OgreFramework::getSingletonPtr()->m_pMouse->capture();

			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->m_pRoot->renderFrame();

			timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getCostMS() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}
#endif

#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoApp::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F))
	{
        //do something
	}
#endif
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoApp::keyReleased(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
#endif

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
