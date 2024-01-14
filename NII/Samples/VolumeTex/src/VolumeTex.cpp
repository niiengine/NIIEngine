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
/**	Generate 3D julia sets and render them as volume texture
	This demonstrates
	   - User generated textures
	   - Procedural volume textures (Julia makes nice dust clouds)
	   - Custom renderables
	@author W.J. van der Laan
*/

#include "SdkSample.h"
#include "SamplePlugin.h"
#include <OgreTexture.h>
#include <NiiFrameBuffer.h>
#include <OgreTextureManager.h>
#include <OgreLogManager.h>
#include <sstream>

#include "VolumeRenderable.h"
#include "ThingRenderable.h"
#include "Julia.h"

using namespace Ogre;
using namespace OgreBites;

TexturePtr ptex;
RenderObj *vrend;
RenderObj *trend;
Overlay* overlay;
float xtime = 0.0f;
SpaceNode *snode,*fnode;
AnimationFusion* mOgreAnimState = 0;

class _OgreSampleClassExport Sample_VolumeTex : public SdkSample
{
public:

    Sample_VolumeTex()
	{
		mInfo["Title"] = "Volume Textures";
		mInfo["Description"] = "Demonstrates the use of volume textures.";
		mInfo["Thumbnail"] = "thumb_voltex.png";
		mInfo["Category"] = "Unsorted";
	}

	void testCapabilities(const RenderSysFeature* caps)
	{       
		if(!caps->hasCapability(GF_Tex_3D))
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, 
                "Your card does not support 3D textures, so cannot run this demo. Sorry!");
        }
	}

protected:

	float global_real, global_imag, global_theta;

    void setupView(void)
	{
		SdkSample::setupView();

		// Create the camera
        mCamera->setPos(Vector3f(220, -2, 176));
        mCamera->lookAt(Vector3f(0, 0, 0));
        mCamera->setNearClipDistance(5);
	}

    void setupContent(void)
    {
        // Create dynamic texture
		ptex = N_Only(TextureManager).createTexture("DynaTex", "General", TEX_TYPE_3D, 
            64, 64, 64, 0, PF_A8R8G8B8);

		// Set ambient light
        mSceneMgr->setAmbient(Colour(0.6, 0.6, 0.6));
        BoxSky * mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial();
        mBoxSky->setRenderGroup(RQG_Sky);
		mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox", 50 );

        //mRoot->getRender()->clearBuffer(FBT_COLOUR, Colour(255,255,255,0));

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        l->setDiffuse(0.75, 0.75, 0.80);
		l->setSpecular(0.9, 0.9, 1);
        l->setPos(-100,80,50);
		mSceneMgr->getOriginNode()->attachObject(l);
		
		// Create volume renderable
		snode = mSceneMgr->getOriginNode()->create(Vector3(0,0,0));      
		
        vrend = new VolumeRenderable(32, 750.0f, "DynaTex");
        snode->attachObject( vrend );
		
		trend = new ThingRenderable(90.0f, 32, 7.5f);
		trend->setMaterial("Examples/VTDarkStuff");
		snode->attachObject(trend);
		
		// Ogre head node
		fnode = mSceneMgr->getOriginNode()->create(Vector3(0,0,0));
		// Load ogre head
		GeometrySpace * head = mSceneMgr->createGeo("head", "ogrehead.mesh");
		fnode->attachObject(head);
		
		// Animation for ogre head
		// Create a track for the light
        Animation * anim = mSceneMgr->createAnimation("OgreTrack", 10);
        // Spline it for nice curves
        anim->setInterpolationMode(NodeFusion::M_SPLINE);
        // Create a track to animate the camera's node
        NodeKeyFrameTrack * track = N_new NodeKeyFrameTrack(anim, 0);
        track->setAssociatedNode(fnode);
        // Setup keyframes
        NodeKeyFrame * key = static_cast<NodeKeyFrame *>(track->create(0)); // A startposition
        key->setTranslate(Vector3(0.0f, -15.0f, 0.0f));
        key = static_cast<NodeKeyFrame *>(track->create(5));//B
        key->setTranslate(Vector3(0.0f, 15.0f, 0.0f));
        key = static_cast<NodeKeyFrame *>(track->create(10));//C
        key->setTranslate(Vector3(0.0f, -15.0f, 0.0f));
        track->buildSpline();
        // Create a new animation state to track this
        mOgreAnimState = mSceneMgr->createFusion("OgreTrack");
        mOgreAnimState->setEnabled(true);
     
        //mFountainNode->attachObject(pSys2);

		//Setup defaults
		global_real = 0.4f;
		global_imag = 0.6f;
		global_theta = 0.0f;

		// show GUI
		createControls();

		setDragLook(true);

		generate();
    }

	bool onRenderCore(const RenderFrameArgs * evt)
    {
		xtime += evt->mCurrent;
		xtime = fmod(xtime, 10.0f);
		//snode->roll(Angle(evt->mCurrent * 20.0f));
		//fnode->roll(Angle(evt->mCurrent * 20.0f));
		static_cast<ThingRenderable*>(trend)->addTime(evt->mCurrent * 0.05f);
		mOgreAnimState->addTime(evt->mCurrent);
		return SdkSample::onRenderCore(evt);
    }

	void cleanupContent(void)
	{
		TextureManager::getSingleton().remove("DynaTex");
		delete vrend;
		delete trend;
	}

	void createControls()
	{
		mTrayMgr->createLabel(TL_TOPLEFT, "JuliaParamLabel", "Julia Parameters", 200);
		mTrayMgr->createThickSlider(TL_TOPLEFT, "RealSlider", "NIIf", 200, 80, -1, 1, 50)->setValue(global_real, false);
		mTrayMgr->createThickSlider(TL_TOPLEFT, "ImagSlider", "Imag", 200, 80, -1, 1, 50)->setValue(global_imag, false);
		mTrayMgr->createThickSlider(TL_TOPLEFT, "ThetaSlider", "Theta", 200, 80, -1, 1, 50)->setValue(global_theta, false);
		mTrayMgr->showCursor();
	}

	void sliderMoved(Slider* slider)
	{
		if (slider->getName() == "RealSlider")
		{
			global_real = slider->getValue();
		}
		else if (slider->getName() == "ImagSlider")
		{
			global_imag = slider->getValue();
		} 
		else if (slider->getName() == "ThetaSlider")
		{
			global_theta = slider->getValue();
		}
		generate();
	}

	void generate()
	{
		/* Evaluate julia fractal for each point */
		Julia julia(global_real, global_imag, global_theta);
		const float scale = 2.5;
		const float vcut = 29.0f;
		const float vscale = 1.0f/vcut;
		
		FrameBuffer * buffer = ptex->getBuffer(0);
		Ogre::StringStream d;
		d << "FrameBuffer " << buffer->getWidth() << " " << buffer->getHeight() << " " << buffer->getDepth();
		LogManager::getSingleton().logMessage(d.str());
		
		buffer->lock(Buffer::MM_READ | Buffer::MM_WRITE);
		const PixelBlock &pb = buffer->getMutexData();
		d.str("");
		d << "PixelBlock " << pb.getWidth() << " " << pb.getHeight() << " " << pb.getDepth() << " " << pb.rowPitch << " " << pb.slicePitch << " " << pb.data << " " << PixelUtil::getFormatName(pb.format);
		LogManager::getSingleton().logMessage(d.str());
		
		Ogre::uint32 *pbptr = static_cast<Ogre::uint32*>(pb.data);
		for(size_t z=pb.front; z<pb.back; z++) 
        {
            for(size_t y=pb.top; y<pb.bottom; y++)
            {
                for(size_t x=pb.left; x<pb.right; x++)
                {
                    if(z==pb.front || z==(pb.back-1) || y==pb.top|| y==(pb.bottom-1) ||
						x==pb.left || x==(pb.right-1))
					{
						// On border, must be zero
						pbptr[x] = 0;
                    } 
					else
					{
						float val = julia.eval(((float)x/pb.getWidth()-0.5f) * scale, 
								((float)y/pb.getHeight()-0.5f) * scale, 
								((float)z/pb.getDepth()-0.5f) * scale);
						if(val > vcut)
							val = vcut;
						
						PixelUtil::packColour((float)x/pb.getWidth(), (float)y/pb.getHeight(), (float)z/pb.getDepth(), (1.0f-(val*vscale))*0.7f, PF_A8R8G8B8, &pbptr[x]);
						
					}	
                }
                pbptr += pb.rowPitch;
            }
            pbptr += pb.getSliceOft() / pb.mPixelSize;
        }
		buffer->unlock();
	}
};

#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;

extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_VolumeTex;
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
