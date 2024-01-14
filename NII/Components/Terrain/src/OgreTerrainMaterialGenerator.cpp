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
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreSpaceTypeManager.h"
#include "OgreTextureManager.h"
#include "OgreTexture.h"
#include "OgreTerrain.h"
#include "OgreCustomSpaceObj.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgreTextureFrame.h"
#include "NiiFrameBuffer.h"
#include "OgreRenderSystem.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
// we do lots of conversions here, casting them all is tedious & cluttered, we know what we're doing
#   pragma warning (disable : 4244)
#endif

namespace Ogre
{
	//---------------------------------------------------------------------
	TerrainMaterialGenerator::TerrainMaterialGenerator() : 
        mActiveProfile(0), 
        mChangeCounter(0), 
        mDebugLevel(0), 
        mCompositeMapSM(0), 
        mCompositeMapCam(0), 
        mCompositeMapRTT(0), 
        mCompositeMapPlane(0), 
        mCompositeMapLight(0)
	{

	}
	//---------------------------------------------------------------------
	TerrainMaterialGenerator::~TerrainMaterialGenerator()
	{
		for(ProfileList::iterator i = mProfiles.begin(); i != mProfiles.end(); ++i)
			N_delete *i;

		if(mCompositeMapRTT && TextureManager::getSingletonPtr())
		{
			TextureManager::getSingleton().remove(mCompositeMapRTT->getPrc());
			mCompositeMapRTT = 0;
		}

		if(mCompositeMapSM && Root::getSingletonPtr())
		{
			// will also delete cam and objects etc
			Root::getSingleton().destroySceneManager(mCompositeMapSM);
			mCompositeMapSM = 0;
			mCompositeMapCam = 0;
			mCompositeMapPlane = 0;
			mCompositeMapLight = 0;
		}
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGenerator::_renderCompositeMap(size_t size, const Rect & rect, 
        const MaterialPtr & mat, const TexturePtr & destCompositeMap)
	{
		if(!mCompositeMapSM)
		{
			// dedicated SpaceManager
			mCompositeMapSM = Root::getSingleton().createSpaceManager(DefaultSceneManagerFactory::FACTORY_TYPE_NAME);
			float camDist = 100;
			float halfCamDist = camDist * 0.5f;
			mCompositeMapCam = mCompositeMapSM->createCamera("cam");
			mCompositeMapCam->setPos(Vector3f(0, 0, camDist));
			mCompositeMapCam->lookAt(Vector3::ZERO);
			mCompositeMapCam->setProjType(Frustum::T_Orthogonal);
			mCompositeMapCam->setNearClipDistance(10);
			mCompositeMapCam->setFarClipDistance(500);
			mCompositeMapCam->setOrthoAspect(camDist, camDist);

			// Just in case material relies on light auto params
			mCompositeMapLight = mCompositeMapSM->createLight(0);
			mCompositeMapLight->setType(LT_DIRECTIONAL);

			RenderSystem * rSys = Root::getSingleton().getRender();
			NIIf hOffset = rSys->getTexelXOffset() / (NIIf)size;
			NIIf vOffset = rSys->getTexelYOffset() / (NIIf)size;

			// set up scene
			mCompositeMapPlane = mCompositeMapSM->createCustomGeo(0);
			mCompositeMapPlane->begin(GeometryRaw::OT_TRIANGLE_LIST, mat->getOriginID());
			mCompositeMapPlane->position(-halfCamDist, halfCamDist, 0);
			mCompositeMapPlane->textureCoord(0 - hOffset, 0 - vOffset);
			mCompositeMapPlane->position(-halfCamDist, -halfCamDist, 0);
			mCompositeMapPlane->textureCoord(0 - hOffset, 1 - vOffset);
			mCompositeMapPlane->position(halfCamDist, -halfCamDist, 0);
			mCompositeMapPlane->textureCoord(1 - hOffset, 1 - vOffset);
			mCompositeMapPlane->position(halfCamDist, halfCamDist, 0);
			mCompositeMapPlane->textureCoord(1 - hOffset, 0 - vOffset);
			mCompositeMapPlane->quad(0, 1, 2, 3);
			mCompositeMapPlane->end();
			mCompositeMapSM->getOriginNode()->attachObject(mCompositeMapPlane);
		}

		// update
		mCompositeMapPlane->setMaterialName(0, mat->getName());
		TerrainGlobalOptions & globalopts = TerrainGlobalOptions::getSingleton();
		mCompositeMapLight->setDirection(globalopts.getLightMapDirection());
		mCompositeMapLight->setDiffuse(globalopts.getCompositeMapDiffuse());
		mCompositeMapSM->setAmbient(globalopts.getCompositeMapAmbient());

		// check for size change (allow smaller to be reused)
		if(mCompositeMapRTT && size != mCompositeMapRTT->getWidth())
		{
			N_Only(Texture).remove(mCompositeMapRTT->getPrc());
			mCompositeMapRTT = 0;
		}

		if(!mCompositeMapRTT)
		{
			mCompositeMapRTT = N_Only(Texture).createTexture(mCompositeMapSM->getName() + "/compRTT", 
                GroupDefault, TEX_TYPE_2D, PF_BYTE_RGBA, size, size, 0, Texture::MM_FRAME_RGB);
			FrameObj * fo = mCompositeMapRTT->getBuffer()->getBuffer();
			// don't render all the time, only on demand
			fo->setAutoFlush(false);
			Viewport * vp = fo->createViewport(mCompositeMapCam);
			// don't render overlays
			vp->setOverlayEnable(false);
		}

		// calculate the area we need to update
		NIIf vpleft = (NIIf)rect.left / (NIIf)size;
		NIIf vptop = (NIIf)rect.top / (NIIf)size;
		NIIf vpright = (NIIf)rect.right / (NIIf)size;
		NIIf vpbottom = (NIIf)rect.bottom / (NIIf)size;

		FrameObj * rtt = mCompositeMapRTT->getBuffer()->getBuffer();
        mCompositeMapCam->setSubMode(true);
		mCompositeMapCam->setSubView(vpleft, vptop, vpright, vpbottom);

		rtt->flush();

		// We have an RTT, we want to copy the results into a regular texture
		// That's because in non-update scenarios we don't want to keep an RTT
		// around. We use a single RTT to serve all terrain pages which is more
		// efficient.
		Box box(rect.left, rect.top, rect.right, rect.bottom);
		destCompositeMap->getBuffer()->write(mCompositeMapRTT->getBuffer(), box, box);
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	void TerrainMaterialGenerator::Profile::updateCompositeMap(const Terrain* terrain, const Rect& rect)
	{
		// convert point-space rect into image space
		size_t compSize = terrain->getCompositeMap()->getWidth();
		Rect imgRect;
		Vector3 inVec, outVec;
		inVec.x = rect.left;
		inVec.y = rect.bottom - 1; // this is 'top' in image space, also make inclusive
		terrain->convertPosition(Terrain::POINT_SPACE, inVec, Terrain::TERRAIN_SPACE, outVec);
		imgRect.left = outVec.x * compSize;
		imgRect.top = (1.0f - outVec.y) * compSize;
		inVec.x = rect.right - 1;
		inVec.y = rect.top; // this is 'bottom' in image space
		terrain->convertPosition(Terrain::POINT_SPACE, inVec, Terrain::TERRAIN_SPACE, outVec);
		imgRect.right = outVec.x * (NIIf)compSize + 1;
		imgRect.bottom = (1.0 - outVec.y) * compSize + 1;

		imgRect.left = std::max(0L, imgRect.left);
		imgRect.top = std::max(0L, imgRect.top);
		imgRect.right = std::min((long)compSize, imgRect.right);
		imgRect.bottom = std::min((long)compSize, imgRect.bottom);


		mParent->_renderCompositeMap(
			compSize, imgRect,
			terrain->getCompositeMapMaterial(),
			terrain->getCompositeMap());

	}


}

