#ifndef __TextureArray_H__
#define __TextureArray_H__

#include "SdkSample.h"
#include "OgreImage.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_TextureArray : public SdkSample
{
public:
	Sample_TextureArray()
	{
		mInfo["Title"] = "Texture Array";
		mInfo["Description"] = "Demonstrates texture array support.";
		mInfo["Thumbnail"] = "thumb_texarray.png";
		mInfo["Category"] = "Unsorted";
		mInfo["Help"] = "Top Left: Multi-frame\nTop Right: Scrolling\nBottom Left: Rotation\nBottom Right: Scaling";
        mBoxSky = 0;
    }
    ~Sample_TextureArray()
    {
        if(mBoxSky)
        {
            mSceneMgr->removeSky(mBoxSky);
            N_delete mBoxSky;
            mBoxSky = 0;
        }
    }
protected:

    void testCapabilities( const RenderSysFeature* caps )
    {
        if(!caps->hasCapability(GF_Program_Vertex) || !caps->hasCapability(GF_Program_Fragment))
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex and "
                "fragment programs, so you cannot run this sample. Sorry!");
        }

        if (!GpuProgramManager::getSingleton().isSyntaxSupported("vs_4_0") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
            !GpuProgramManager::getSingleton().isSyntaxSupported("gp4fp"))
        {
            N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support the shader model needed for this sample, "
                        "so you cannot run this sample. Sorry!", "TextureArray::testCapabilities");
        }
    }

	void setupContent()
	{
        mBoxSky = N_new BoxSky(mSceneMgr);
        mBoxSky->setMaterial("Examples/TrippySkyBox");
        mBoxSky->setRenderGroup(RQG_Sky);
		mBoxSky->buildGeo(5000);
        mSceneMgr->addSky(mBoxSky);
		// set our camera to orbit around the origin and show cursor
		mCameraMan->setStyle(CS_ORBIT);
		mTrayMgr->showCursor();

		// the names of the textures we will use (all need to be the same size: 512*512 in our case)
		vector<String>::type texNames;
        texNames.push_back("BeachStones.jpg");
        texNames.push_back("BumpyMetal.jpg");
        texNames.push_back("egyptrockyfull.jpg");
        texNames.push_back("frost.png");
        texNames.push_back("MtlPlat2.jpg");
        texNames.push_back("nskingr.jpg");
        texNames.push_back("Panels_Diffuse.png");
        texNames.push_back("Panels_reflection.png");
        texNames.push_back("RustedMetal.jpg");
        texNames.push_back("spacesky.jpg");
        texNames.push_back("terrain_texture.jpg");
        texNames.push_back("texmap2.jpg");
        texNames.push_back("Water01.jpg");
        texNames.push_back("Water02.jpg");
        texNames.push_back("body.jpg");
        texNames.push_back("stone1.jpg");
        texNames.push_back("wall3.jpg");
        texNames.push_back("sinbad_body.tga");
        texNames.push_back("sinbad_clothes.tga");
        texNames.push_back("stevecube_BK.jpg");

        // create the 2d texture array (the depth is the size of the array - number of textures)
        TexturePtr tex = N_Only(TextureManager).createTexture("TextureArrayTex",
            GroupDefault, TEX_TYPE_2D_ARRAY, 512, 512, texNames.size(), 0, PF_X8R8G8B8);

        // add all the textures to a 2d texture array
		for(size_t i = 0; i < texNames.size(); i++)
		{
            Image terrainTex;
            terrainTex.load(texNames[i], GroupDefault);
            FrameBuffer * pixelBufferBuf = tex->getBuffer(0);
            const PixelBlock & currImage = pixelBufferBuf->lock(Box(0,0,i,terrainTex.getHeight(), terrainTex.getHeight(), i+1), Buffer::MM_WHOLE);
            PixelUtil::bulkPixelConversion(terrainTex.getPixel(), currImage);
            pixelBufferBuf->unlock();
        }

        // create material and set the texture unit to our texture
        MaterialManager & matMgr = MaterialManager::getSingleton();
        MaterialPtr texArrayMat = matMgr.ref("Examples/TextureArray", GroupDefault).first;
        texArrayMat->check();
        ShaderCh * pass = texArrayMat->getLOD()->get(0);
        pass->setLight(false);
		ShaderChTextureUnit * pState = pass->getTexture().create();
        pState->setSrc(tex->getName(), TEX_TYPE_2D_ARRAY);

        // create a plane with float3 tex coord - the third value will be the texture index in our case
	    CustomSpaceObj * textureArrayObject = mSceneMgr->createCustomGeo("TextureAtlasObject");

        // create a quad that uses our material
        int quadSize = 100;
	    textureArrayObject->begin(GeometryRaw::OT_TRIANGLE_LIST, texArrayMat->getName());
	    // triangle 0 of the quad
	    textureArrayObject->position(0, 0, 0);
    	textureArrayObject->textureCoord(0, 0, 0);
	    textureArrayObject->position(quadSize, 0, 0);
    	textureArrayObject->textureCoord(1, 0, 0);
	    textureArrayObject->position(quadSize, quadSize, 0);
    	textureArrayObject->textureCoord(1, 1, texNames.size());

	    // triangle 1 of the quad
	    textureArrayObject->position(0, 0, 0);
    	textureArrayObject->textureCoord(0, 0, 0);
	    textureArrayObject->position(quadSize, quadSize, 0);
    	textureArrayObject->textureCoord(1, 1, texNames.size());
	    textureArrayObject->position(0, quadSize, 0);
    	textureArrayObject->textureCoord(0, 1, texNames.size());

	    textureArrayObject->end();

		// attach it to a node and position appropriately
		SpaceNode* node = mSceneMgr->getOriginNode()->create();
		node->setPos(Vector3f(-quadSize / 2, -quadSize / 2, 0));
		node->attachObject(textureArrayObject);
	}

    void cleanupContent()
    {
        TextureManager::getSingleton().remove("TextureArrayTex");
    }
};

#endif
