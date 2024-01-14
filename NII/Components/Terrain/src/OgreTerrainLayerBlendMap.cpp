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
#include "OgreTerrainLayerBlendMap.h"
#include "NiiFrameBuffer.h"
#include "OgreTerrain.h"
#include "OgreImage.h"

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
// we do lots of conversions here, casting them all is tedious & cluttered, we know what we're doing
#   pragma warning (disable : 4244)
#endif

namespace Ogre
{
	//---------------------------------------------------------------------
	TerrainLayerBlendMap::TerrainLayerBlendMap(Terrain* parent, uint8 layerIndex,
		HardwarePixelBuffer* buf)
		: mParent(parent)
		, mLayerIdx(layerIndex)
		, mChannel((layerIndex-1) % 4)
		, mDirty(false)
		, mBuffer(buf)
		, mData(0)
	{
		mData = static_cast<float*>(OGRE_MALLOC(mBuffer->getWidth() * mBuffer->getHeight() * sizeof(float), MEMCATEGORY_RESOURCE));

		// we know which of RGBA we need to look at, now find it in the format
		// because we can't guarantee what precise format the RS gives us
		unsigned char rgbaShift[4];
		PixelFormat fmt = mBuffer->getFormat();
		PixelUtil::getBitShifts(fmt, rgbaShift);
		mChannelOffset = rgbaShift[mChannel] / 8; // /8 to convert to bytes
#if OGRE_ENDIAN == OGRE_ENDIAN_BIG
		// invert (dealing bytewise)
		mChannelOffset = PixelUtil::getNumElemBytes(fmt) - mChannelOffset - 1;
#endif
		download();

	}
	//---------------------------------------------------------------------
	TerrainLayerBlendMap::~TerrainLayerBlendMap()
	{
		OGRE_FREE(mData, MEMCATEGORY_RESOURCE);
		mData = 0;
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::download()
	{
		float* pDst = mData;
		// Download data
		Box box(0, 0, mBuffer->getWidth(), mBuffer->getHeight());
		uint8* pSrc = static_cast<uint8*>(mBuffer->lock(box, Buffer::MM_READ).data);
		pSrc += mChannelOffset;
		size_t srcInc = PixelUtil::getNumElemBytes(mBuffer->getFormat());
		for (size_t y = box.top; y < box.bottom; ++y)
		{
			for (size_t x = box.left; x < box.right; ++x)
			{
				*pDst++ = static_cast<float>(*pSrc) / 255.0f;
				pSrc += srcInc;
			}
		}
		mBuffer->unlock();

	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertWorldToUVSpace(const Vector3 & worldPos, 
        NIIf * outX, NIIf * outY)
	{
		Vector3 terrainSpace;
		mParent->getTerrainPosition(worldPos, &terrainSpace);
		*outX = terrainSpace.x;
		*outY = 1.0f - terrainSpace.y;
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertUVToWorldSpace(NIIf x, NIIf y, Vector3 * outWorldPos)
	{
		mParent->getPos(x, 1.0f - y, 0, outWorldPos);
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertUVToImageSpace(NIIf x, NIIf y, size_t * outX,
        size_t * outY)
	{
		*outX = (unsigned long)(x * (mBuffer->getWidth() - 1));
		*outY = (unsigned long)(y * (mBuffer->getHeight() - 1));
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertImageToUVSpace(size_t x, size_t y, NIIf* outX,
        NIIf * outY)
	{
		*outX = x / (NIIf)(mBuffer->getWidth() - 1);
		*outY = y / (NIIf)(mBuffer->getHeight() - 1);
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertImageToTerrainSpace(size_t x, size_t y, 
        NIIf * outX, NIIf * outY)
	{
		convertImageToUVSpace(x, y, outX, outY);
		*outY = 1.0f - *outY;
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::convertTerrainToImageSpace(NIIf x, NIIf y, 
        size_t * outX, size_t * outY)
	{
		convertUVToImageSpace(x, 1.0f - y, outX, outY);
	}
	//---------------------------------------------------------------------
	float TerrainLayerBlendMap::getBlendValue(size_t x, size_t y)
	{
		return *(mData + y * mBuffer->getWidth() + x);
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::setBlendValue(size_t x, size_t y, float val)
	{
		*(mData + y * mBuffer->getWidth() + x) = val;
		dirtyRect(Rect(x, y, x+1, y+1));

	}
	//---------------------------------------------------------------------
	float* TerrainLayerBlendMap::getBlendPointer()
	{
		return mData;
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::dirty()
	{
		Rect rect;
		rect.top = 0; rect.bottom = mBuffer->getHeight();
		rect.left = 0; rect.right = mBuffer->getWidth();
		dirtyRect(rect);

	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::dirtyRect(const Rect& rect)
	{
		if (mDirty)
		{
			mDirtyBox.left = std::min(mDirtyBox.left, (size_t)rect.left);
			mDirtyBox.top = std::min(mDirtyBox.top, (size_t)rect.top);
			mDirtyBox.right = std::max(mDirtyBox.right, (size_t)rect.right);
			mDirtyBox.bottom = std::max(mDirtyBox.bottom, (size_t)rect.bottom);
		}
		else
		{
			mDirtyBox.left = rect.left;
			mDirtyBox.right = rect.right;
			mDirtyBox.top = rect.top;
			mDirtyBox.bottom = rect.bottom;
			mDirty = true;
		}
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::update()
	{
		if (mData && mDirty)
		{
			// Upload data
			float* pSrcBase = mData + mDirtyBox.top * mBuffer->getWidth() + mDirtyBox.left;
			uint8* pDstBase = static_cast<uint8*>(mBuffer->lock(mDirtyBox, Buffer::MM_READ | Buffer::MM_WRITE).data);
			pDstBase += mChannelOffset;
			size_t dstInc = PixelUtil::getNumElemBytes(mBuffer->getFormat());
			for (size_t y = 0; y < mDirtyBox.getHeight(); ++y)
			{
				float* pSrc = pSrcBase + y * mBuffer->getWidth();
				uint8* pDst = pDstBase + y * mBuffer->getWidth() * dstInc;
				for (size_t x = 0; x < mDirtyBox.getWidth(); ++x)
				{
					*pDst = static_cast<uint8>(*pSrc++ * 255);
					pDst += dstInc;
				}
			}
			mBuffer->unlock();

			mDirty = false;

			// make sure composite map is updated
			// mDirtyBox is in image space, convert to terrain units
			Rect compositeMapRect;
			float blendToTerrain = (float)mParent->getSize() / (float)mBuffer->getWidth();
			compositeMapRect.left = (long)(mDirtyBox.left * blendToTerrain);
			compositeMapRect.right = (long)(mDirtyBox.right * blendToTerrain + 1);
			compositeMapRect.top = (long)((mBuffer->getHeight() - mDirtyBox.bottom) * blendToTerrain);
			compositeMapRect.bottom = (long)((mBuffer->getHeight() - mDirtyBox.top) * blendToTerrain + 1);
			mParent->_dirtyCompositeMapRect(compositeMapRect);
			mParent->updateCompositeMapWithDelay();

		}
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::blit(const PixelBlock & src, const Box & dstBox)
	{
		const PixelBlock* srcBox = &src;

		if (srcBox->getWidth() != dstBox.getWidth() || srcBox->getHeight() != dstBox.getHeight())
		{
			// we need to rescale src to dst size first (also confvert format)
			void* tmpData = OGRE_MALLOC(dstBox.getWidth() * dstBox.getHeight(), MEMCATEGORY_GENERAL);
			srcBox = N_new PixelBlock(dstBox.getWidth(), dstBox.getHeight(), 1, PF_L8ui, tmpData);

			PixelBlock::scale(src, *srcBox);
		}

		// pixel conversion
		PixelBlock dstMemBox(dstBox, PF_L8ui, mData);
		PixelUtil::bulkPixelConversion(*srcBox, dstMemBox);

		if (srcBox != &src)
		{
			// free temp
			OGRE_FREE(srcBox->data, MEMCATEGORY_GENERAL);
			N_delete srcBox;
			srcBox = 0;
		}

		Rect dRect(dstBox.left, dstBox.top, dstBox.right, dstBox.bottom);
		dirtyRect(dRect);

	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::blit(const PixelBlock & src)
	{
		blit(src, Box(0, 0, 0, mBuffer->getWidth(), mBuffer->getHeight(), 1));
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::loadImage(const Image & img)
	{
		blit(img.getPixel());
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::loadImage(DataStream * stream, const String & ext)
	{
		Image img;
		img.load(stream, ext);
		loadImage(img);
	}
	//---------------------------------------------------------------------
	void TerrainLayerBlendMap::loadImage(const String& filename, const String& groupName)
	{
		Image img;
		img.load(filename, groupName);
		loadImage(img);
	}
}