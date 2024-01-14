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

#include "UseCustomCapabilitiesTests.h"
#include "OgreRenderSystem.h"
#include "OgreRenderSystemCapabilities.h"
#include "OgreRenderSystemCapabilitiesSerializer.h"
#include "OgreRenderSystemCapabilitiesManager.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"
#include "OgreLog.h"



// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION( UseCustomCapabilitiesTests );

void UseCustomCapabilitiesTests::setUp()
{
    using namespace Ogre;

	// set up silent logging to not pollute output
	if(LogManager::getSingletonPtr())
		N_delete Ogre::LogManager::getSingletonPtr();
	
	// write cleanup to log
	if(LogManager::getSingletonPtr() == 0)
	{
		LogManager* logManager = N_new LogManager();
		logManager->createLog("testCustomCapabilitiesSetUp.log", true, false);
	}

	if(Ogre::HighLevelGpuProgramManager::getSingletonPtr())
		N_delete Ogre::HighLevelGpuProgramManager::getSingletonPtr();
	if(Ogre::GpuProgramManager::getSingletonPtr())
		N_delete Ogre::GpuProgramManager::getSingletonPtr();
	if(Ogre::CompositorManager::getSingletonPtr())
		N_delete Ogre::CompositorManager::getSingletonPtr();
	if(Ogre::MaterialManager::getSingletonPtr())
		N_delete Ogre::MaterialManager::getSingletonPtr();
	if(Ogre::ResourceSchemeManager::getSingletonPtr())
		N_delete Ogre::ResourceSchemeManager::getSingletonPtr();

	// set up silent logging to not pollute output
	if(LogManager::getSingletonPtr())
		N_delete Ogre::LogManager::getSingletonPtr();
}

void UseCustomCapabilitiesTests::tearDown()
{
	using namespace Ogre;
	// set up silent logging to not pollute output
	if(LogManager::getSingletonPtr())
		N_delete Ogre::LogManager::getSingletonPtr();

}

void checkCaps(const Ogre::RenderSysFeature* caps)
{
    using namespace Ogre;

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_AutoMipmap), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Blend), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Anisotropy), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Dot3), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Cube), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_HW_Stencil), true);

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_HW_Buffer), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Program_Vertex), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Program_Fragment), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_ClipRect), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Stencil_BothSide), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Stencil_Wrap), true);

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_HW_PixelQuery), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_ClipPlanes), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Vertex_UByte4), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Infinite_FarPlane), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Render_To_Texture), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Float), true);

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_NonPow2), false);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_3D), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Point_Sprites), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Point_Param), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Program_VertexTexFetch), false);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_MipMapBias), true);

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Compress), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Fromat_DXT), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_Tex_Fromat_VTC), false);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_GL_FrameBuffer), true);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_GL_FrameBuffer_Arb), false);

    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_GL_FrameBuffer_ATI), false);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_GL_PBuffer), false);
    CPPUNIT_ASSERT_EQUAL(caps->hasCapability(GF_D3D_PerStageConstant), false);

    CPPUNIT_ASSERT(caps->isShaderSupport("arbfp1"));
    CPPUNIT_ASSERT(caps->isShaderSupport("arbvp1"));
    CPPUNIT_ASSERT(caps->isShaderSupport("glsl"));
    CPPUNIT_ASSERT(caps->isShaderSupport("ps_1_1"));
    CPPUNIT_ASSERT(caps->isShaderSupport("ps_1_2"));
    CPPUNIT_ASSERT(caps->isShaderSupport("ps_1_3"));
    CPPUNIT_ASSERT(caps->isShaderSupport("ps_1_4"));

    CPPUNIT_ASSERT_EQUAL(caps->getMaxPointSize(), (NIIf)1024);
    CPPUNIT_ASSERT_EQUAL(caps->isOnlyPow2Texture(), false);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxSpace(), (Ogre::ushort)0);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxTexUnit(), (Ogre::ushort)16);
    CPPUNIT_ASSERT_EQUAL(caps->getStencilBit(), (Ogre::ushort)8);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxMultiFrameObj(), (Ogre::ushort)4);

    CPPUNIT_ASSERT_EQUAL(caps->getMaxVPFloatParamCount(), (Ogre::ushort)256);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxVPIntParamCount(), (Ogre::ushort)0);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxVPBoolParamCount(), (Ogre::ushort)0);

    CPPUNIT_ASSERT_EQUAL(caps->getMaxFPFloatCount(), (Ogre::ushort)64);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxFPIntCount(), (Ogre::ushort)0);
    CPPUNIT_ASSERT_EQUAL(caps->getMaxFPBoolCount(), (Ogre::ushort)0);

    CPPUNIT_ASSERT_EQUAL(caps->getMaxVertexTexUnit(), (Ogre::ushort)0);
    CPPUNIT_ASSERT(caps->isShaderSupport("arbvp1"));
    CPPUNIT_ASSERT(caps->isShaderSupport("arbfp1"));
}

void setUpGLRenderSystemOptions(Ogre::RenderSystem* rs)
{
	using namespace Ogre;
	SysOptionList options = rs->getConfig();
	// set default options
	// this should work on every semi-normal system
	rs->setConfig(String("Colour Depth"), String("32"));
	rs->setConfig(String("FSAA"), String("0"));
	rs->setConfig(String("Full Screen"), String("No"));
	rs->setConfig(String("VSync"), String("No"));
	rs->setConfig(String("Video Mode"), String("800 x 600"));
	
	// use the best RTT
	SysOption optionRTT = options["RenderTarget Mode"];
	
	if(find(optionRTT.mReserve.begin(), optionRTT.mReserve.end(), "FBO") != optionRTT.mReserve.end())
	{
		rs->setConfig(String("RenderTarget Mode"), String("FBO"));
	}
	else if(find(optionRTT.mReserve.begin(), optionRTT.mReserve.end(), "PBuffer") != optionRTT.mReserve.end())
	{
		rs->setConfig(String("RenderTarget Mode"), String("PBuffer"));
	}
	else
		rs->setConfig(String("RenderTarget Mode"), String("Copy"));
}

void UseCustomCapabilitiesTests::testCustomCapabilitiesGL()
{
    using namespace Ogre;

	// set up silent logging to not pollute output
	if(LogManager::getSingletonPtr())
		N_delete Ogre::LogManager::getSingletonPtr();
	
	if(LogManager::getSingletonPtr() == 0)
	{
		LogManager* logManager = N_new LogManager();
		logManager->createLog("testCustomCapabilitiesGL.log", true, false);
	}

	Root* root = N_new Root("plugins.cfg");
	RenderSystem* rs = root->getRenderSystemByName("OpenGL Rendering Subsystem");
	if(rs == 0)
	{
		CPPUNIT_ASSERT_ASSERTION_PASS("This test is irrelevant because GL RenderSystem is not available");
	}
	else
	{
		try 
        {
			setUpGLRenderSystemOptions(rs);
			root->set(rs);
			root->init(true, "OGRE testCustomCapabilitiesGL Window",
											"../../../Media/CustomCapabilities/customCapabilitiesTest.cfg");

			const RenderSysFeature* caps = rs->getFeature();

			checkCaps(caps);
		}
		// clean up root, in case of error, and let cppunit to handle the exception
		catch(...)
		{
		}
	}
    N_delete root;
}

void setUpD3D9RenderSystemOptions(Ogre::RenderSystem * rs)
{
	using namespace Ogre;
	SysOptionList options = rs->getConfig();
	// set default options
	// this should work on every semi-normal system
	rs->setConfig(String("Anti aliasing"), String("None"));
	rs->setConfig(String("Full Screen"), String("No"));
	rs->setConfig(String("VSync"), String("No"));
	rs->setConfig(String("Video Mode"), String("800 x 600 @ 32-bit colour"));
	
	// pick first available device
	SysOption optionDevice = options["Rendering Device"];

	rs->setConfig(optionDevice.name, optionDevice.mValue);
}

void UseCustomCapabilitiesTests::testCustomCapabilitiesD3D9()
{
	// set up silent logging to not pollute output
	if(LogManager::getSingletonPtr())
		N_delete Ogre::LogManager::getSingletonPtr();
	
	if(LogManager::getSingletonPtr() == 0)
	{
		LogManager* logManager = N_new LogManager();
		logManager->createLog("testCustomCapabilitiesD3D9.log", true, false);
	}

    Root* root = N_new Root("plugins.cfg");
	RenderSystem* rs = root->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	if(rs == 0)
	{
		CPPUNIT_ASSERT_ASSERTION_PASS("This test is irrelevant because D3D9 RenderSystem is not available");
	}
	else
	{	
		try 
        {
			setUpD3D9RenderSystemOptions(rs);
			root->set(rs);
			root->init(true, "OGRE testCustomCapabilitiesD3D9 Window",
											"../../../Media/CustomCapabilities/customCapabilitiesTest.cfg");
		
			const RenderSysFeature* caps = rs->getFeature();

			checkCaps(caps);
		}
		// clean up root, in case of error, and let cppunit to handle the exception
		catch(...)
		{
		}
	}

    N_delete root;
}


