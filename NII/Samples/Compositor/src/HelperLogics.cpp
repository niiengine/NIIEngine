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

#include "HelperLogics.h"
#include "Ogre.h"

//---------------------------------------------------------------------------
class HeatVisionListener: public Ogre::CompositorInstance::Listener
{
public:
    HeatVisionListener();
    virtual ~HeatVisionListener();
    virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
protected:
    Ogre::GpuProgramParam * fpParams;
    float start, end, curr;
    Ogre::Timer *timer;
};
//---------------------------------------------------------------------------
class HDRListener: public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	int mBloomSize;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	HDRListener();
	virtual ~HDRListener();
	void notifyViewportSize(int width, int height);
	void notifyCompositor(Ogre::CompositorInstance* instance);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};
//---------------------------------------------------------------------------
class GaussianListener : public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth, mVpHeight;
	// Array params - have to pack in groups of 4 since this is how Cg generates them
	// also prevents dependent texture read problems if ops don't require swizzle
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexOffsetsVert[15][4];
public:
	GaussianListener();
	virtual ~GaussianListener();
	void notifyViewportSize(int width, int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};
//---------------------------------------------------------------------------
Ogre::CompositorInstance::Listener* HDRLogic::createListener(Ogre::CompositorInstance* instance)
{
	HDRListener* listener = new HDRListener;
	Ogre::Viewport* vp = instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getPixelWidth(), vp->getPixelHeight());
	listener->notifyCompositor(instance);
	return listener;
}
//---------------------------------------------------------------------------
Ogre::CompositorInstance::Listener* HeatVisionLogic::createListener(Ogre::CompositorInstance* instance)
{
	return new HeatVisionListener;
}
//---------------------------------------------------------------------------
Ogre::CompositorInstance::Listener* GaussianBlurLogic::createListener(Ogre::CompositorInstance* instance)
{
	GaussianListener* listener = new GaussianListener;
	Ogre::Viewport* vp = instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getPixelWidth(), vp->getPixelHeight());
	return listener;
}
/*************************************************************************
	                    HeatVisionListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
HeatVisionListener::HeatVisionListener()
{
	timer = new Ogre::Timer();
    start = end = curr = 0.0f;
}
//---------------------------------------------------------------------------
HeatVisionListener::~HeatVisionListener()
{
   delete timer;
}
//---------------------------------------------------------------------------
void HeatVisionListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    if(pass_id == 0xDEADBABE)
    {
        timer->reset();
        mat->get(0)->get(0)->getProgram().get(0, ST_FS, NULL, fpParams);
    }
}
//---------------------------------------------------------------------------
void HeatVisionListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
    if(pass_id == 0xDEADBABE)
    {
        // "random_fractions" parameter
        fpParams->set("random_fractions", Ogre::Vector4(Ogre::Math::random(0.0, 1.0), Ogre::Math::random(0, 1.0), 0, 0));

        // "depth_modulator" parameter
        float inc = ((float)timer->getMS())/1000.0f;
        if ( (fabs(curr-end) <= 0.001) ) {
            // take a new value to reach
            end = Ogre::Math::random(0.95, 1.0);
            start = curr;
        } else {
            if (curr > end) curr -= inc;
            else curr += inc;
        }
        timer->reset();

        fpParams->set("depth_modulator", Ogre::Vector4(curr, 0, 0, 0));
    }
}
//-------------------------------------------------------------------
NIIf normalDistribution(NIIf x, NIIf offset, NIIf scale)
{
    NIIf nom = Math::Exp(-Math::Sqr(x - offset) / (2 * Math::Sqr(scale)));
    NIIf denom = scale * Math::sqrt(2 * Math::PI);

    return nom / denom;
}
//---------------------------------------------------------------------------

/*************************************************************************
HDRListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
HDRListener::HDRListener()
{
}
//---------------------------------------------------------------------------
HDRListener::~HDRListener()
{
}
//---------------------------------------------------------------------------
void HDRListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
}
//---------------------------------------------------------------------------
void HDRListener::notifyCompositor(Ogre::CompositorInstance* instance)
{
	// Get some RTT dimensions for later calculations
	Ogre::CompositionTechnique::TextureDefinitionIterator defIter =
		instance->getShaderFusion()->getTextureDefinitionIterator();
	while (defIter.isNextValid())
	{
		Ogre::CompositionTechnique::FrameDefine* def =
			defIter.getAndNext();
		if(def->name == "rt_bloom0")
		{
			mBloomSize = (int)def->width; // should be square
			// Calculate gaussian texture offsets & weights
			float deviation = 3.0f;
			float texelSize = 1.0f / (float)mBloomSize;

			// central sample, no offset
			mBloomTexOffsetsHorz[0][0] = 0.0f;
			mBloomTexOffsetsHorz[0][1] = 0.0f;
			mBloomTexOffsetsVert[0][0] = 0.0f;
			mBloomTexOffsetsVert[0][1] = 0.0f;
			mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
				mBloomTexWeights[0][2] = normalDistribution(0, 0, deviation);
			mBloomTexWeights[0][3] = 1.0f;

			// 'pre' samples
			for(int i = 1; i < 8; ++i)
			{
				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
					mBloomTexWeights[i][2] = 1.25f * normalDistribution(i, 0, deviation);
				mBloomTexWeights[i][3] = 1.0f;
				mBloomTexOffsetsHorz[i][0] = i * texelSize;
				mBloomTexOffsetsHorz[i][1] = 0.0f;
				mBloomTexOffsetsVert[i][0] = 0.0f;
				mBloomTexOffsetsVert[i][1] = i * texelSize;
			}
			// 'post' samples
			for(int i = 8; i < 15; ++i)
			{
				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
					mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
				mBloomTexWeights[i][3] = 1.0f;

				mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
				mBloomTexOffsetsHorz[i][1] = 0.0f;
				mBloomTexOffsetsVert[i][0] = 0.0f;
				mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
			}

		}
	}
}
//---------------------------------------------------------------------------
void HDRListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	//case 994: // rt_lum4
	case 993: // rt_lum3
	case 992: // rt_lum2
	case 991: // rt_lum1
	case 990: // rt_lum0
		break;
	case 800: // rt_brightpass
		break;
	case 701: // rt_bloom1
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParam * fparams;
            mat->getLOD()->get(0)->getProgram().get(0, ST_FS, NULL, fparams);
			fparams->set("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
			fparams->set("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	case 700: // rt_bloom0
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParam * fparams;
            mat->get(0)->get(0)->getProgram().get(0, ST_FS, NULL, fparams);
			fparams->set("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->set("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	}
}
//---------------------------------------------------------------------------
void HDRListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}
//---------------------------------------------------------------------------
/*************************************************************************
GaussianListener Methods
*************************************************************************/
//---------------------------------------------------------------------------
GaussianListener::GaussianListener()
{
}
//---------------------------------------------------------------------------
GaussianListener::~GaussianListener()
{
}
//---------------------------------------------------------------------------
void GaussianListener::notifyViewportSize(int width, int height)
{
	mVpWidth = width;
	mVpHeight = height;
	// Calculate gaussian texture offsets & weights
	float deviation = 3.0f;
	float texelSize = 1.0f / (float)std::min(mVpWidth, mVpHeight);

	// central sample, no offset
	mBloomTexOffsetsHorz[0][0] = 0.0f;
	mBloomTexOffsetsHorz[0][1] = 0.0f;
	mBloomTexOffsetsVert[0][0] = 0.0f;
	mBloomTexOffsetsVert[0][1] = 0.0f;
	mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
		mBloomTexWeights[0][2] = Ogre::Math::normalDistribution(0, 0, deviation);
	mBloomTexWeights[0][3] = 1.0f;

	// 'pre' samples
	for(int i = 1; i < 8; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = Ogre::Math::normalDistribution(i, 0, deviation);
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = i * texelSize;
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for(int i = 8; i < 15; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
		mBloomTexWeights[i][3] = 1.0f;

		mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	// Prepare the fragment params offsets
	switch(pass_id)
	{
	case 701: // blur horz
		{
			// horizontal bloom
			mat->load();
			Ogre::GpuProgramParam * fparams;
			mat->getLOD()->get(0)->getProgram().get(0, ST_FS, NULL, fparams);
			fparams->set("sampleOffsets", mBloomTexOffsetsHorz[0], 15);
			fparams->set("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	case 700: // blur vert
		{
			// vertical bloom
			mat->load();
			Ogre::GpuProgramParam * fparams;
            mat->get(0)->get(0)->getProgram().get(0, ST_FS, NULL, fparams);
			fparams->set("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->set("sampleWeights", mBloomTexWeights[0], 15);

			break;
		}
	}
}
//---------------------------------------------------------------------------
void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}
//---------------------------------------------------------------------------
