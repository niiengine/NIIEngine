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

#include "NiiPreProcess.h"
#include "NiiImage.h"
#include "NiiImageCodec.h"
#include "NiiTextureManager.h"
#include "NiiVFSManager.h"
#include "NiiException.h"
#include "NiiColour.h"
#include "NiiMath.h"
#include "NiiImageResampler.h"
#include "NiiImageDownsampler.h"

namespace NII
{
    //------------------------------------------------------------------------
    Image::Image():
        mWidth(0),
        mHeight(0),
        mDepth(1),
        mArray(1),
        mSize(0),
        mMipmapCount(1),
        mFlags(0),
        mBuffer(0),
        mFormat(PF_UNKNOWN),
        mAutoDelete(true)
    {
    }
    //------------------------------------------------------------------------
    Image::Image(const Image & o) :
        mBuffer(0),
        mAutoDelete(true)
    {
        *this = o;
    }
    //------------------------------------------------------------------------
    Image::~Image()
    {
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
            mBuffer = NULL;
        }
    }
    //------------------------------------------------------------------------
    Image & Image::operator = (const Image & o)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
        }
        mWidth = o.mWidth;
        mHeight = o.mHeight;
        mDepth = o.mDepth;
        mArray = o.mArray;
        mFormat = o.mFormat;
        mSize = o.mSize;
        mFlags = o.mFlags;
        mPixelSize = o.mPixelSize;
        mMipmapCount = o.mMipmapCount;
        mAutoDelete = o.mAutoDelete;

        if(mAutoDelete)
        {
            mBuffer = N_alloc_simd(Nui8, mSize);
            memcpy(mBuffer, o.mBuffer, mSize);
        }
        else
        {
            mBuffer = o.mBuffer;
        }
        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::create(NCount width, NCount height, NCount depth, NCount array, PixelFormat pf, NCount mipmap)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
        }
        mMipmapCount = std::min(PixelUtil::getMaxMipmap(width, height, depth), mipmap);
        mSize = PixelUtil::getSize(width, height, depth, array, pf, mMipmapCount, 4);
        mBuffer = N_alloc_simd(mSize);
        mWidth = width;
        mHeight = height;
        mDepth = depth;
        mArray = array;
        mFormat = pf;
        mFlags = 0;
        mAutoDelete = true;
        mPixelSize = (Nui8)PixelUtil::getUnitSize(mFormat);

        if(PixelUtil::isCompress(pf))
            mFlags |= M_Compress;
        if(mDepth != 1)
            mFlags |= M_3D;
        if(mArray == 6)
            mFlags |= M_Cube;

        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::load(Nui8 * data, NCount w, NCount h, NCount d, PixelFormat pf, NCount faces, NCount mipmap, bool autoDelete)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
        }

        mBuffer = data;
        mWidth = w;
        mHeight = h;
        mDepth = d;
        mArray = faces;
        mFormat = pf;
        mFlags = 0;
        mAutoDelete = autoDelete;
        mMipmapCount = mipmap;
        mPixelSize = (Nui8)PixelUtil::getUnitSize(mFormat);

        if(PixelUtil::isCompress(pf))
            mFlags |= M_Compress;
        if(mDepth != 1)
            mFlags |= M_3D;
        if(mArray == 6)
            mFlags |= M_Cube;

        mSize = PixelUtil::getSize(w, h, d, faces, pf, mMipmapCount);

        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::load(DataStream * data, NCount w, NCount h, NCount d, PixelFormat pf, NCount faces, NCount mipmap)
    {
        NCount size = PixelUtil::getSize(w, h, d, faces, pf, mipmap);
        if(size != data->size())
        {
            N_EXCEPT(InvalidParam, _I18n("Stream size does not match calculated image size"));
        }

        Nui8 * dst = N_alloc_t(Nui8, size);
        data->read(dst, size);

        return load(dst, w, h, d, pf, faces, mipmap, true);
    }
    //------------------------------------------------------------------------
    Image & Image::load(const String & file, GroupID gid)
    {
        String ext;
        NCount pos = file.find_last_of(_T("."));
        if(pos != String::npos && pos < (file.length() - 1))
        {
            ext = file.substr(pos + 1);
        }

        DataStream * encoded = N_Only(ResourceScheme).open(file, gid);
        load(encoded, ext);
        N_delete encoded;

        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::load(DataStream * data, const String & hint)
    {
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
            mBuffer = NULL;
        }

        Codec * codec = 0;
        if(hint.empty())
        {
            // 从magic数字/32字节
            NCount magicLen = N_MIN(NCount, data->size(), 32);
            
            Nui8 magicBuf[32];
            data->read(magicBuf, magicLen);
            data->seek(0);
            
            // 从流的第一个32字节和文件大小获取编解码器
            codec = Codec::getCodec(magicBuf, magicLen);
        }
        else
        {
            codec = Codec::getCodec(hint);
        }

        if(!codec)
            N_EXCEPT(InvalidParam, _I18n("无法辨别图片格式: ") + data->getName());

        static_cast<ImageCodec *>(codec)->decode(data, this);

        mPixelSize = (Nui8)PixelUtil::getUnitSize(mFormat);
        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::horizontalFlip(NCount array, NCount mipmap)
    {
        if(mBuffer == 0)
            return *this;

        NCount mipcnt = (mipmap == -1 ? mMipmapCount : 1);
        NCount mipfirst = (mipmap == -1 ? 0 : mipmap);
        for(NCount i = 0; i < mipcnt; ++i)
        {
            PixelBlock box = getPixel(array, i + mipfirst);

            switch(box.mPixelSize)
            {
            case 1:
                for(NCount z = 0; z < box.getVolume(); ++z)
                {
                    for(NCount y = 0; y < box.mHeight; ++y)
                    {
                        NCount tempw = box.mWidth >> 1u;
                        Nui8 * src = box.getOriginData(0, y, z);
                        Nui8 * dst = box.getOriginData(box.mWidth - 1u, y, z);
                        for(NCount x = 0; x < tempw; ++x)
                        {
                            std::swap(src, dst);
                            ++src;
                            --dst;
                        }
                    }
                }
                break;
            case 2:
                for(NCount z = 0; z < box.getVolume(); ++z)
                {
                    for(NCount y = 0; y < box.mHeight; ++y)
                    {
                        NCount tempw = box.mWidth >> 1u;
                        Nui16 * src = reinterpret_cast<Nui16 *>(box.getOriginData(0, y, z));
                        Nui16 * dst = reinterpret_cast<Nui16 *>(box.getOriginData(box.mWidth - 1u, y, z));
                        for(NCount x = 0; x < tempw; ++x)
                        {
                            std::swap(src, dst);
                            ++src;
                            --dst;
                        }
                    }
                }
                break;
            case 4:
                for(NCount z = 0; z < box.getVolume(); ++z)
                {
                    for(NCount y = 0; y < box.mHeight; ++y)
                    {
                        NCount tempw = box.mWidth >> 1u;
                        Nui32 * src = reinterpret_cast<Nui32 *>(box.getOriginData(0, y, z));
                        Nui32 * dst = reinterpret_cast<Nui32 *>(box.getOriginData(box.mWidth - 1u, y, z));
                        for(NCount x = 0; x < tempw; ++x)
                        {
                            std::swap(src, dst);
                            ++src;
                            --dst;
                        }
                    }
                }
                break;
            default:
                for(NCount z = 0; z < box.getVolume(); ++z)
                {
                    for(NCount y = 0; y < box.mHeight; ++y)
                    {
                        NCount tempw = box.mWidth >> 1u;
                        Nui8 * src = box.getOriginData(0, y, z);
                        Nui8 * dst = box.getOriginData(box.mWidth - 1u, y, z);
                        for(NCount x = 0; x < tempw; ++x)
                        {
                            for(NCount i = 0; i < box.mPixelSize; ++i)
                            {
                                std::swap(src, dst);
                                ++src;
                                ++dst;
                            }

                            dst -= box.mPixelSize << 1u;
                        }
                    }
                }
                break;
            }
        }
        
        return *this;
    }
    //------------------------------------------------------------------------
    Image & Image::verticalFlip(NCount array, NCount mipmap)
    {
        if(mBuffer == 0)
            return *this;
        
        NCount mipcnt = (mipmap == -1 ? mMipmapCount : 1);
        NCount mipfirst = (mipmap == -1 ? 0 : mipmap);
        void * pTempBuffer = N_alloc_simd(getRowSize(0));
        for(NCount i = 0; i < mipcnt; ++i)
        {
            if(PixelUtil::isCompress(mFormat))
            {
                PixelBlock box = getPixel(array, i + mipfirst);
            
                for( NCount z = 0; z < box.getVolume(); ++z )
                {
                    for( NCount y = 0; y < (box.mHeight >> 1u); ++y )
                    {
                        void * row0 = box.getOriginData( 0, y, z );
                        void * rowN = box.getOriginData( 0, box.mHeight - y - 1u, z );
                        memcpy( pTempBuffer, rowN, box.mRowSize );
                        memcpy( rowN, row0, box.mRowSize );
                        memcpy( row0, pTempBuffer, box.mRowSize );
                    }
                }
            }
            else
                getPixel(array, mipmap).verticalFlip();
        }
        N_free_simd(pTempBuffer);

        return *this;
    }
    //-----------------------------------------------------------------------------------
    bool Image::genMipmap(bool srgb, PixelBlock::Filter filter)
    {
        //OgreProfileExhaustive( "Image2::genMipmap" );

        // resizing dynamic images is not supported
        assert(mAutoDelete);
        assert((mTextureType == Texture::Type2D || mTextureType == Texture::T_CUBE) && "Texture type not supported" );

        ImageDownsampler2D * downsampler2DFunc      = 0;
        ImageDownsamplerCube * downsamplerCubeFunc  = 0;
        ImageBlur2D * separableBlur2DFunc           = 0;

        srgb |= PixelUtil::isSRGB(mFormat);

        bool canGenerateMipmaps = getDownsampler(mFormat, downsampler2DFunc, downsamplerCubeFunc, separableBlur2DFunc,
            srgb, mDepth, mArray, mTextureType, filter);

        if( !canGenerateMipmaps )
            return false;

        // Allocate new buffer
        NCount reqMipmap = PixelUtil::getMaxMipmap(mWidth, mHeight, getDepth());
        Image tmpImage0;
        Nui8 * tmpBuffer1 = 0;
        if(reqMipmap != mNumMipmaps || filter == F_Gaussian_Bilinear)
        {
            tmpImage0.load(mBuffer, mWidth, mHeight, mDepth, mArray, mFormat, mNumMipmaps, true);

            mBuffer = N_alloc_simd(PixelUtil::getSize(mWidth, mHeight, getDepth(), getArray(), mFormat, reqMipmap, 4u));

            PixelBlock srcBox = tmpImage0.getPixel(-1, 0);
            PixelBlock dstBox = getPixel(-1, 0);
            memcpy(dstBox.mData, srcBox.mData, srcBox.mSliceSize * srcBox.mArray);

            mNumMipmaps = reqMipmap;

            //Free memory now, it's not needed anymore. If we still
            //need it, it will be freed when function returns
            if(filter == F_Gaussian_Bilinear)
                tmpBuffer1 = reinterpret_cast<Nui8*>(N_alloc_simd(getSliceSize(0)));
            //else
            //    tmpImage0.freeMemory();
        }

        NCount dstWidth  = mWidth;
        NCount dstHeight = mHeight;

        int filterIdx = 1;

        switch( filter )
        {
        case FILTER_NEAREST:
            filterIdx = 0; break;
        case FILTER_LINEAR:
        case FILTER_BILINEAR:
            filterIdx = 1; break;
        case F_Gaussian:
            filterIdx = 2; break;
        case F_Gaussian_Bilinear:
            filterIdx = 1; break;
        default: // Keep compiler happy
            break;
        }

        const FilterKernel & chosenFilter = c_filterKernels[filterIdx];

        for(NCount i = 1u; i < mNumMipmaps; ++i)
        {
            NCount srcWidth = dstWidth;
            NCount srcHeight = dstHeight;
            dstWidth = std::max<NCount>(1u, dstWidth >> 1u);
            dstHeight = std::max<NCount>(1u, dstHeight >> 1u);

            PixelBlock box0 = getPixel(-1, i - 1u);
            PixelBlock box1 = getPixel(-1, i);

            if( mTextureType == Texture::T_CUBE )
            {
                Nui8 const * upFaces[6];
                upFaces[0] = box0.getOriginData(0, 0, 0);
                upFaces[1] = box0.getOriginData(0, 0, 1);
                upFaces[2] = box0.getOriginData(0, 0, 2);
                upFaces[3] = box0.getOriginData(0, 0, 3);
                upFaces[4] = box0.getOriginData(0, 0, 4);
                upFaces[5] = box0.getOriginData(0, 0, 5);

                for( size_t j=0; j<6; ++j )
                {
                    Nui8 *downFace = box1.getOriginData(0, 0, j);
                    (*downsamplerCubeFunc)( downFace, upFaces,
                                            dstWidth, dstHeight, box1.mRowSize,
                                            srcWidth, srcHeight, box0.mRowSize,
                                            chosenFilter.kernel,
                                            chosenFilter.kernelStartX, chosenFilter.kernelEndX,
                                            chosenFilter.kernelStartY, chosenFilter.kernelEndY,
                                            j );
                }
            }
            else
            {
                if( filter != F_Gaussian_Bilinear )
                {
                    (*downsampler2DFunc)( box1.mData, box0.mData,
                                          dstWidth, dstHeight, box1.mRowSize,
                                          srcWidth, box0.mRowSize,
                                          chosenFilter.kernel,
                                          chosenFilter.kernelStartX, chosenFilter.kernelEndX,
                                          chosenFilter.kernelStartY, chosenFilter.kernelEndY );
                }
                else
                {
                    //tmpImage0 should contain one or more mips (from mip 0), and tmpBuffer1 should
                    //be large enough to contain mip 0. This assert should never trigger.
                    assert( tmpImage0.getSize() >= box0.getSize() );

                    //Copy box0 to tmpImage0
                    memcpy( tmpImage0.mBuffer, box0.mData, box0.getSize() );

                    //The image right now is in both box0 and tmpImage0. We can't ref box0,
                    //So we blur tmpImage0, and use tmpBuffer1 to store intermediate results
                    const FilterSeparableKernel &separableKernel = c_filterSeparableKernels[0];
                    (*separableBlur2DFunc)( tmpBuffer1,
                                            tmpImage0.mBuffer,
                                            srcWidth, srcHeight, box0.mRowSize,
                                            separableKernel.kernel,
                                            separableKernel.kernelStart, separableKernel.kernelEnd );
                    //Filter again...
                    (*separableBlur2DFunc)( tmpBuffer1,
                                            tmpImage0.mBuffer,
                                            srcWidth, srcHeight, box0.mRowSize,
                                            separableKernel.kernel,
                                            separableKernel.kernelStart, separableKernel.kernelEnd );

                    //Now that tmpImage0 is blurred, bilinear downsample its contents into box1.
                    (*downsampler2DFunc)( box1.mData,
                                          tmpImage0.mBuffer,
                                          dstWidth, dstHeight, box1.mRowSize,
                                          srcWidth, box0.mRowSize,
                                          chosenFilter.kernel,
                                          chosenFilter.kernelStartX, chosenFilter.kernelEndX,
                                          chosenFilter.kernelStartY, chosenFilter.kernelEndY );
                }
            }
        }

        if( tmpBuffer1 )
        {
            N_free_simd(tmpBuffer1);
            tmpBuffer1 = 0;
        }

        return true;
    }
    //------------------------------------------------------------------------
    void Image::save(const String & file, CodecID cid, NCount mipBegin, NCount mipCount)
    {
        if(mBuffer)
        {
            String ext;
            NCount pos = file.find_last_of(_T("."));
            if(pos == String::npos)
                N_EXCEPT(InvalidParam, _I18n("Unable to save image file '") + file + _I18n("' - invalid extension."));

            while(pos != file.length() - 1)
                ext += file[++pos];

            Codec * codec = Codec::getCodec(ext);
            if(codec == 0)
            {
                codec = Codec::getCodec(cid);
            }
            if(codec != 0)
            {
                if(mipCount == -1 || mipCount + mipBegin > mMipmapCount)
                {
                    mipCount = mMipmapCount - mipBegin;
                }
                static_cast<ImageCodec *>(codec)->encode(this, file, ext, mipBegin, mipCount);
            }
        }
    }
    //------------------------------------------------------------------------
    DataStream * Image::encode(const String & ext, NCount mipBegin, NCount mipCount) const
    {
        if(mBuffer)
        {
            Codec * codec = Codec::getCodec(ext);

            if(!codec)
                N_EXCEPT(InvalidParam, _I18n("Unable to encode image data as '") + ext + _I18n("' - invalid extension."));

            DataStream * dest = 0;

            if(mipCount == -1 || mipCount + mipBegin > mMipmapCount)
            {
                mipCount = mMipmapCount - mipBegin;
            }
            static_cast<ImageCodec *>(codec)->encode(this, dest, ext, mipBegin, mipCount);

            return dest;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    bool Image::isAlpha() const
    {
        return PixelUtil::getExentMark(mFormat) & PFF_Alpha;
    }
    //------------------------------------------------------------------------
    void Image::resize(NCount w, NCount h, PixelBlock::Filter filter)
    {
        // 动态图像不支持重新调整大小
        N_assert(mAutoDelete, "error");
        N_assert(mDepth == 1, "error");

        // 重新分配temp图像,确保自动删除为真
        Image temp;
        temp.load(mBuffer, mWidth, mHeight, mArray, mFormat, mMipmapCount, true);

        mMipmapCount = 1;
        mWidth = w;
        mHeight = h;
        mSize = PixelUtil::getSize(mWidth, mHeight, mDepth, mArray, mFormat, mMipmapCount, 4);
        mBuffer = N_alloc_simd(Nui8, mSize);

        PixelBlock::scale(temp.getPixel(-1), getPixel(-1), filter);
    }
    //------------------------------------------------------------------------
    void Image::getColour(Colour & out, NCount x, NCount y, NCount z, NCount mipmap) const
    {
        PixelBlock block = getPixel(-1, mipLevel);
        block.getColour(out, x, y, z);
    }
    //------------------------------------------------------------------------
    void Image::setColour(const Colour & c, NCount x, NCount y, NCount z, NCount mipmap)
    {
        PixelBlock textureBox = getPixel(-1, mipLevel);
        textureBox.setColour(c, x, y, z);
    }
    //------------------------------------------------------------------------
    NCount Image::getRowSize(NCount mipmap) const
    {
        assert(mipmap < mMipmapCount);
        return PixelUtil::getSize(std::max(mWidth >> mipmap, 1u), 1u, 1u, 1u, mFormat, 4u );
    }
    //-----------------------------------------------------------------------------------
    NCount Image::getSliceSize(NCount mipmap ) const
    {
        assert(mipmap < mMipmapCount);
        return PixelUtil::getSize(std::max(mWidth >> mipmap, 1u), std::max( mHeight >> mipmap, 1u ), 1u, 1u, mFormat, 4u );
    }
    //------------------------------------------------------------------------
    PixelBlock Image::getPixel(NCount face, NCount mipmap) const
    {
        ////////////////////////////////////////////////
        // 图像数据的布置为:                        ////
        // face 0, top level (mip 0)               ////
        // face 0, mip 1                           ///
        // face 0, mip 2                          ///
        // face 1, top level (mip 0)             ///
        // face 1, mip 1                         //
        // face 1, mip 2                        //
        // etc                                 //
        ////////////////////////////////////////
        /*if(mipmap > mMipmapCount)
            N_EXCEPT(UnImpl, _I18n("Mipmap index out of range")) ;
        if(face >= mArray)
            N_EXCEPT(InvalidParam, _I18n("Face index out of range"));

        Nui8 * offset = mBuffer;
        NCount width = getWidth();
        NCount height = getHeight();
        NCount depth = getDepth();
        NCount numMips = getMipmapCount();

        NCount fullFaceSize = 0;
        NCount finalFaceSize = 0;
        NCount finalWidth = 0, finalHeight = 0, finalDepth = 0;
        for(NCount mip = 0; mip <= numMips; ++mip)
        {
            if(mip == mipmap)
            {
                finalFaceSize = fullFaceSize;
                finalWidth = width;
                finalHeight = height;
                finalDepth = depth;
            }
            fullFaceSize += PixelUtil::getSize(width, height, depth, getFormat());
            if(width != 1) width /= 2;
            if(height != 1) height /= 2;
            if(depth != 1) depth /= 2;
        }
        offset += face * fullFaceSize;
        offset += finalFaceSize;
        PixelBlock src(finalWidth, finalHeight, finalDepth, mFormat, offset);*/
        
        N_assert(mipmap < mMipmapCount, "error");
        
        NCount dface = (face == -1 ? mArray : face + 1);
        
        N_assert(face <= mArray, "error");
        NCount array = (face == -1 ? mArray : 1);

        void * data = PixelUtil::getData(mBuffer, mWidth, mHeight, mDepth, dface, mipmap, mFormat);

        PixelBlock re(std::max(1u, mWidth >> mipmap), std::max(1u, mHeight >> mipmap), std::max(1u, mDepth >> mipmap), 
            array, PixelUtil::getUnitSize(mFormat), getRowSize(mipmap), getSliceSize(mipmap), mFormat, data);

        return re;
    }
    //------------------------------------------------------------------------
    Image & Image::combine(const String & rgb, const String & a, PixelFormat pf, GroupID gid)
    {
        Image irgb, ia;

        irgb.load(rgb, gid);
        ia.load(a, gid);

        return combine(irgb, ia, pf);
    }
    //------------------------------------------------------------------------
    Image & Image::combine(DataStream * rgb, DataStream * a, PixelFormat pf, const String & rgbhint, const String & ahint)
    {
        Image irgb, ia;

        irgb.load(rgb, rgbhint);
        ia.load(a, ahint);

        return combine(irgb, ia, pf);
    }
    //------------------------------------------------------------------------
    Image & Image::combine(const Image & rgb, const Image & a, PixelFormat pf)
    {
        // 图像应该是相同的大小,有相同数量的mipmap
        if(rgb.mWidth != a.mWidth || rgb.mHeight != a.mHeight || rgb.mDepth != a.mDepth)
        {
            N_EXCEPT(InvalidParam, _I18n("ImageList must be the same dimensions"));
        }

        if(rgb.mMipmapCount != a.mMipmapCount || rgb.getArray() != a.getArray())
        {
            N_EXCEPT(InvalidParam, _I18n("ImageList must have the same number of surfaces (faces & mipmaps)"));
        }

        // 格式检测
        if(PixelUtil::getDataCount(pf) != 4)
        {
            N_EXCEPT(InvalidParam, _I18n("Target format must have 4 components"));
        }

        if(PixelUtil::isCompress(pf) || PixelUtil::isCompress(rgb.getFormat())
            || PixelUtil::isCompress(a.getFormat()))
        {
            N_EXCEPT(InvalidParam, _I18n("Compressed formats are not supported in this method"));
        }
        if(mBuffer && mAutoDelete)
        {
            N_free_simd(mBuffer);
            mBuffer = NULL;
        }

        mWidth = rgb.mWidth;
        mHeight = rgb.mHeight;
        mDepth = rgb.mDepth;
        mFormat = pf;
        mMipmapCount = rgb.mMipmapCount;
        NCount numFaces = rgb.getArray();
        mArray = numFaces;

        mFlags = 0;
        if(mDepth != 1)
            mFlags |= M_3D;
        if(numFaces == 6)
            mFlags |= M_Cube;

        mSize = PixelUtil::getSize(mWidth, mHeight, mDepth, numFaces, mFormat, mMipmapCount);
        mPixelSize = (Nui8)PixelUtil::getUnitSize(mFormat);
        mBuffer = N_alloc_simd(Nui8, mSize);
        mAutoDelete = true;

        for(NCount face = 0; face < numFaces; ++face)
        {
            for(NCount mip = 0; mip <= mMipmapCount; ++mip)
            {
                // 先转换RGB
                PixelBlock srcRGB = rgb.getPixel(face, mip);
                PixelBlock dst = getPixel(face, mip);
                // 大批转换
                PixelUtil::conv(srcRGB, dst);
                Nui8 * pdst = dst.mData;

                // 现在选择加入透明度通道
                PixelBlock srcAlpha = a.getPixel(face, mip);
                Nui8 * psrcAlpha = srcAlpha.mData;

                for(NCount d = 0; d < mDepth; ++d)
                {
                    for(NCount y = 0; y < mHeight; ++y)
                    {
                        for(NCount x = 0; x < mWidth; ++x)
                        {
                            Colour colRGBA, colA;
                            PixelUtil::unpackColour(&colRGBA, mFormat, pdst);
                            PixelUtil::unpackColour(&colA, a.mFormat, psrcAlpha);
                            colRGBA.a = (colA.r + colA.g + colA.b) / 3.0f;

                            PixelUtil::packColour(colRGBA, mFormat, pdst);

                            psrcAlpha += PixelUtil::getUnitSize(a.getFormat());
                            pdst += PixelUtil::getUnitSize(mFormat);

                        }
                    }
                }
            }
        }
        return *this;
    }
    //------------------------------------------------------------------------
}