#include "RandomTools.h"
#include "OgreTextureManager.h"
#include "NiiFrameBuffer.h"
#include "OgreRoot.h"

using namespace Ogre;

static const int NUM_RAND_VALUES = 1024;

TexturePtr RandomTools::generateRandomVelocityTexture()
{
	// PPP: Temp workaround for DX 11 which does not seem to like usage dynamic
	Texture::MemMode usage = (Root::getSingletonPtr()->getRender()->getName()=="Direct3D11 Rendering Subsystem") ?
		(Texture::MM_MIPMAP_AUTO | Buffer::Buffer::M_DEV | Buffer::M_WRITE) : (Buffer::M_READ | Buffer::M_WRITE);
	TexturePtr texPtr = N_Only(TextureManager).createTexture("RandomVelocityTexture",
		GroupDefault, TEX_TYPE_1D, 1024, 1, 1, 0, PF_R32G32B32A32, usage);

	FrameBuffer * pixelBuf = texPtr->getBuffer();

	float randomData[NUM_RAND_VALUES * 4];
	for(int i = 0; i < NUM_RAND_VALUES * 4; ++i)
    {
        randomData[i] = float((rand()%10000) - 5000);
    }

	PixelBlock pixelBox(1024, 1, 1, PF_R32G32B32A32, &randomData[0]);
	pixelBuf->write(pixelBox, Box(0, 0, 0, mWidth, mHeight, mDepth));
	return texPtr;
}
