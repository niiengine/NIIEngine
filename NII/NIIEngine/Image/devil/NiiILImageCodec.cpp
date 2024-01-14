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
#include "NiiILImageCodec.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiImage.h"
#include "NiiException.h"
#include "NiiILUtil.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

namespace NII
{
    bool ILImageCodec::_is_initialised = false;
    //---------------------------------------------------------------------
    ILImageCodec::ILImageCodec(CodecID cid, const String & hint, ILenum iltype):
        ImageCodec(cid, hint),
        mIlType(ilType)
    {
        initialiseIL();
    }
    //---------------------------------------------------------------------
    ILImageCodec::~ILImageCodec()
    {
    }
    //---------------------------------------------------------------------
    void ILImageCodec::initialiseIL(void)
    {
        if(!_is_initialised)
        {
            ilInit();
            iluInit();
            ilEnable(IL_FILE_OVERWRITE);
            _is_initialised = true;
        }
    }
    //---------------------------------------------------------------------
    CodecResult ILImageCodec::encode(const Image * in, const String & out, const String & hint) const
    {
        ILuint ImageName;

        ilGenImages(1, &ImageName);
        ilBindImage(ImageName);

        PixelBlock src(in->mWidth, in->mHeight, in->mDepth, in->mFormat, in->mBuffer);

        // Convert image from NII to current IL image
        ILUtil::from(src);

        iluFlipImage();

        // Implicitly pick DevIL codec
        ilSave(mIlType, out.c_str());

        // Check if everything was ok
        ILenum PossibleError = ilGetError() ;
        if(PossibleError != IL_NO_ERROR)
        {
            ilDeleteImages(1, &ImageName);
            return CodecResult_Error;
        }

        ilDeleteImages(1, &ImageName);
        return CodecResult_Success;
    }
    //---------------------------------------------------------------------
    CodecResult ILImageCodec::decode(DataStream * in, Image * out) const
    {
        // DevIL variables
        ILuint ImageName;
        ILint ImageFormat, BytesPerPixel, ImageType;

        // Load the image
        ilGenImages(1, &ImageName);
        ilBindImage(ImageName);

        // Put it right side up
        ilEnable(IL_ORIGIN_SET);
        ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);

        // Keep DXTC(compressed) data if present
        ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);

        // Load image from stream, cache into memory
        ilLoadL(mIlType, in->getPtr(), static_cast<ILuint>(in->size()));

        // Check if everything was ok
        ILenum PossibleError = ilGetError() ;
        if(PossibleError != IL_NO_ERROR)
        {
            return CodecResult_Error;
        }

        ImageFormat = ilGetInteger(IL_IMAGE_FORMAT);
        ImageType = ilGetInteger(IL_IMAGE_TYPE);

        // Convert image if ImageType is incompatible with us (NIId or NIIl)
        if(ImageType != IL_BYTE && ImageType != IL_UNSIGNED_BYTE &&
            ImageType != IL_FLOAT && ImageType != IL_UNSIGNED_SHORT &&
                ImageType != IL_SHORT)
        {
            ilConvertImage(ImageFormat, IL_FLOAT);
            ImageType = IL_FLOAT;
        }
        // Converted paletted images
        if(ImageFormat == IL_COLOUR_INDEX)
        {
            ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
            ImageFormat = IL_BGRA;
            ImageType = IL_UNSIGNED_BYTE;
        }

        // Now sets some variables
        BytesPerPixel = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

        out->mFormat = ILUtil::ilFormat2NiiFormat(ImageFormat, ImageType);
        out->mWidth = ilGetInteger(IL_IMAGE_WIDTH);
        out->mHeight = ilGetInteger(IL_IMAGE_HEIGHT);
        out->mDepth = ilGetInteger(IL_IMAGE_DEPTH);
        out->mMipmapCount = ilGetInteger(IL_NUM_MIPMAPS);
        out->mFlags = 0;

        if(out->mFormat == PF_UNKNOWN)
        {
            std::stringstream err;
            err << _T("Unsupported devil format ImageFormat=") << std::hex <<
                ImageFormat << " ImageType="<< ImageType << std::dec;

            ilDeleteImages(1, &ImageName);

            return CodecResult_NoImpl;
        }

        // Check for cubemap
        //ILuint cubeflags = ilGetInteger ( IL_IMAGE_CUBEFLAGS );
        NCount numFaces = ilGetInteger (IL_NUM_IMAGES) + 1;
        if(numFaces == 6)
            out->mFlags |= M_Cube;
        else
            numFaces = 1; // Support only 1 or 6 face images for now

        // Keep DXT data (if present at all and the GPU supports it)
        ILuint dxtFormat = ilGetInteger(IL_DXTC_DATA_FORMAT);
        RenderSys * rsys;
        N_Engine().get(rsys);
        if(dxtFormat != IL_DXT_NO_COMP &&
            rsys->getFeature()->isSupport(GF_Tex_Fromat_DXT))
        {
            out->mFormat = ILUtil::ilFormat2NiiFormat(dxtFormat, ImageType);
            out->mFlags |= M_Compress;

            // Validate that this devil version saves DXT mipmaps
            if(out->mMipmapCount > 0)
            {
                ilBindImage(ImageName);
                ilActiveMipmap(1);
                if((NCount)ilGetInteger(IL_DXTC_DATA_FORMAT) != dxtFormat)
                {
                    out->mMipmapCount = 0;
                    N_Only(Log).log(
                        _T("Warning: Custom mipmaps for compressed image ") + in->getName() +
                            _T(" were ignored because they are not loaded by this DevIL version"));
                }
            }
        }

        NCount width = out->mWidth;
        NCount height = out->mHeight;
        NCount depth = out->mDepth;
        NCount mipmap = out->mMipmapCount;
        PixelFormat format = out->mFormat;

        // Calculate total size from number of mipmaps, faces and size
        out->mSize = PixelUtil::getSize(width, height, depth, numFaces, format, mipmap);
        out->mBuffer = N_alloc_t(Nui8, out->mSize);
        Nui8 * data = out->mBuffer;
        NCount offset = 0;
        // Dimensions of current mipmap

        // Transfer data
        for(NCount mip = 0; mip <= mipmap; ++mip)
        {
            for(NCount i = 0; i < numFaces; ++i)
            {
                ilBindImage(ImageName);
                if(numFaces > 1)
                    ilActiveImage(i);
                if(mipmap > 0)
                    ilActiveMipmap(mip);
                // Size of this face
                NCount imageSize = PixelUtil::getSize(width, height, depth, 1, format);
                if(out->haveExt(M_Compress))
                {
                    // Compare DXT size returned by DevIL with our idea of the compressed size
                    if(imageSize == ilGetDXTCData(NULL, 0, dxtFormat))
                    {
                        // Retrieve data from DevIL
                        ilGetDXTCData(data + offset, imageSize, dxtFormat);
                    }
                    else
                    {
                        N_Only(Log).log(_T("Warning: compressed image ") +
                            in->getName() + " size mismatch, devilsize=" +
                            StrConv::conv(ilGetDXTCData(NULL, 0, dxtFormat)) + " oursize="+
                            StrConv::conv(imageSize));
                    }
                }
                else
                {
                    // Retrieve data from DevIL
                    PixelBlock dst(width, height, depth, format, data + offset);
                    ILUtil::to(dst);
                }
                offset += imageSize;
            }
            // Next mip
            if(width != 1)
                width /= 2;
            if(height != 1)
                height /= 2;
            if(depth != 1)
                depth /= 2;
        }

        // Restore IL state
        ilDisable(IL_ORIGIN_SET);
        ilDisable(IL_FORMAT_SET);

        ilDeleteImages(1, &ImageName);
        return CodecResult_Success;
    }
    //---------------------------------------------------------------------
    bool ILImageCodec::match(const String & hint) const
    {
        return false;
    }
    //---------------------------------------------------------------------
    String ILImageCodec::match(const Nui8 * magic, NCount valid) const
    {
        // DevIL uses magic internally to determine file types when
        // necessary but does not expose the code in its API.
        // This makes it difficult to implement this function, but also
        // reduces its importance. Just for now, here is a kludge to
        // get Nii to build and ensure that it always tries to load files
        // that DevIL might be able to load.
        return String("jpg");
    }
    //---------------------------------------------------------------------
}
