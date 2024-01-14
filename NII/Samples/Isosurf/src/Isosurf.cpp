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
Filename:    IsoSurf.cpp
Description: Demonstrates the use of the geometry shader to tessellate an
	isosurface using marching tetrahedrons. Partial implementation of cg
	Isosurf sample from NVIDIA's OpenGL SDK 10 :
	http://developer.download.nvidia.com/SDK/10/opengl/samples.html
-----------------------------------------------------------------------------
*/

#include "SdkSample.h"
#include "SamplePlugin.h"
#include "ProceduralTools.h"

using namespace Ogre;
using namespace OgreBites;

SamplePlugin* sp;
Sample* s;

class _OgreSampleClassExport Sample_Isosurf : public SdkSample
{
	GeometrySpace* tetrahedra;
    MeshPtr mTetrahedraMesh;
public:

    Sample_Isosurf()
	{
		mInfo["Title"] = "Isosurf";
		mInfo["Description"] = "A demo of procedural geometry manipulation using geometry shaders.";
		mInfo["Thumbnail"] = "thumb_isosurf.png";
		mInfo["Category"] = "Geometry";
    }

    void testCapabilities(const RenderSysFeature* caps)
    {
        if (!caps->hasCapability(GF_Program_Geometry))
        {
			N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your render system / hardware does not support geometry programs, "
                        "so you cannot run this sample. Sorry!",
                        "Sample_Isosurf::testCapabilities");
        }

		Ogre::LogManager::getSingleton().getDefaultLog()->stream() <<
            "Num output vertices per geometry shader run : " << caps->getMaxGeometryOutput();
    }

    // Just override the mandatory create scene method
    void setupContent(void)
    {
		mCamera->setPos(NII::Vector3f(0, 0, -40));
        mCamera->lookAt(NII::Vector3f(0, 0, 0));
		mCamera->setNearClipDistance(0.1);
		mCamera->setFarClipDistance(100);

		mTetrahedraMesh = ProceduralTools::generateTetrahedra();
		//Create tetrahedra and add it to the root scene node
		tetrahedra = mSceneMgr->createGeo("TetrahedraEntity", mTetrahedraMesh->getName());
		//tetraHedra->setDebugMode(true);
		Ogre::SpaceNode* parentNode = mSceneMgr->getOriginNode()->create();
		parentNode->attachObject(tetrahedra);
		parentNode->setScale(Vector3f(10, 10, 10));
    }

    void cleanupContent()
    {
        MeshManager::getSingleton().remove(mTetrahedraMesh->getName());
    }

	bool onRenderCore(const RenderFrameArgs * evt)
	{
		NIIf seconds = (NIIf)(Root::getSingleton().getTimer()->getMS()) / 1000.0;
		Ogre::Pass* renderPass = tetrahedra->getCom(0)->getMaterial()->get(0)->get(0);
		if (renderPass->getProgram().isExist(ST_VS))
		{
			Ogre::Vector4 constParam = Ogre::Vector4(-0.5, 0.0, 0.0, 0.2);
            GpuProgramParam * param;
			renderPass->getProgram().get(0, ST_VS, NULL, param);
            param->set("Metaballs[0]", constParam);

			Ogre::Vector4 timeParam = Ogre::Vector4(
				0.1 + Ogre::Math::Sin(seconds)*0.5, Ogre::Math::Cos(seconds)*0.5, 0.0, 0.1);
			renderPass->getProgram().get(0, ST_VS, NULL, param);
            param->set("Metaballs[1]", timeParam);
		}
		return SdkSample::onRenderCore(evt);
	}
};

#ifndef OGRE_STATIC_LIB

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_Isosurf;
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
