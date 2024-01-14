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
#include "NiiFreeImageCodec.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiImage.h"
#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"

// freeimage 3.9.1~3.11.0 interoperability fix
#ifndef FREEIMAGE_COLORORDER
/* we have freeimage 3.9.1, define these symbols in such way as 3.9.1 really work
(do not use 3.11.0 definition, as color order was changed between these two versions
 on Apple systems)
 */
#define FREEIMAGE_COLORORDER_BGR    0
#define FREEIMAGE_COLORORDER_RGB    1
#if defined(FREEIMAGE_BIGENDIAN)
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB
#else
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_BGR
#endif
#endif

// Color order is actually RGB on iPhone
#if N_PLAT == N_PLAT_IOS
#undef FREEIMAGE_COLORORDER
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB
#endif

namespace NII
{
    //---------------------------------------------------------------------
    void FreeImageSaveErrorHandler(FREE_IMAGE_FORMAT fif, const NIIb * message)
    {
        // Callback method as required by FreeImage to report problems
        N_EXCEPT(IO, message)
    }
    //---------------------------------------------------------------------
    FreeImageCodec::FreeImageCodec(CodecID cid, const String & hint, FREE_IMAGE_FORMAT fiType) :
        ImageCodec(cid, hint),
        mOwnType(fiType)
    {
    }
    //---------------------------------------------------------------------
    FIBITMAP * FreeImageCodec::_encode(const Image * in) const
    {
        FIBITMAP * ret = 0;

        NCount srcw = in->mWidth;
        NCount srch = in->mHeight;
        PixelFormat srcpf = in->mFormat;

        // The required format, which will adjust to the format
        // actually supported by FreeImage.
        PixelFormat dstpf = srcpf;

        // determine the settings
        FREE_IMAGE_TYPE imageType;
        PixelFormat determiningFormat = srcpf;

        switch(determiningFormat)
        {
        case PF_R5G6B5:
        case PF_B5G6R5:
        case PF_R8G8B8UN:
        case PF_B8G8R8UN:
        case PF_A8R8G8B8:
        case PF_X8R8G8B8:
        case PF_A8B8G8R8:
        case PF_X8B8G8R8:
        case PF_B8G8R8A8UN:
        case PF_R8G8B8A8ui:
        case PF_A4L4i:
        case PF_L8A8i:
        case PF_R3G3B2:
        case PF_A4R4G4B4:
        case PF_A1R5G5B5:
        case PF_A2R10G10B10:
        case PF_A2B10G10R10:
            // I'd like to be able to use r/g/b masks to get FreeImage to load the data
            // in it's existing format, but that doesn't work, FreeImage needs to have
            // data in RGB[A] (big endian) and BGR[A] (little endian), always.
            if(PixelUtil::hasAlpha(determiningFormat))
            {
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                dstpf = PF_BYTE_RGBA;
#else
                dstpf = PF_BYTE_BGRA;
#endif
            }
            else
            {
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                dstpf = PF_BYTE_RGB;
#else
                dstpf = PF_BYTE_BGR;
#endif
            }
            // fall through
        case PF_L8ui:
        case PF_A8UN:
            imageType = FIT_BITMAP;
            break;
        case PF_L16ui:
            imageType = FIT_UINT16;
            break;
        case PF_G16R16ui:
            dstpf = PF_R16G16B16ui;
            // fall through
        case PF_R16G16B16ui:
            imageType = FIT_RGB16;
            break;
        case PF_R16G16B16A16ui:
            imageType = FIT_RGBA16;
            break;
        case PF_R16:
            dstpf = PF_R32;
            // fall through
        case PF_R32:
            imageType = FIT_FLOAT;
            break;
        case PF_G16R16:
        case PF_R16G16B16ui:
        case PF_G32R32:
            dstpf = PF_R32G32B32;
            // fall through
        case PF_R32G32B32:
            imageType = FIT_RGBF;
            break;
        case PF_R16G16B16A16:
            dstpf = PF_R32G32B32A32;
            // fall through
        case PF_R32G32B32A32:
            imageType = FIT_RGBAF;
            break;
        default:
            N_EXCEPT(NoExist, _T("Invalid image format"));
        };

        // Check support for this image type & bit depth
        if(!FreeImage_FIFSupportsExportType(mOwnType, imageType) ||
            !FreeImage_FIFSupportsExportBPP(mOwnType, (NIIi)PixelUtil::getNumElemBits(dstpf)))
        {
            // Ok, need to allocate a fallback
            // Only deal with RGBA -> RGB for now
            switch(dstpf)
            {
            case PF_BYTE_RGBA:
                dstpf = PF_BYTE_RGB;
                break;
            case PF_BYTE_BGRA:
                dstpf = PF_BYTE_BGR;
                break;
            default:
                break;
            };

        }

        bool conversionRequired = false;
        Nui8 * srcData = in->mBuffer;

        // Check BPP
        unsigned bpp = static_cast<unsigned>(PixelUtil::getNumElemBits(dstpf));
        if(!FreeImage_FIFSupportsExportBPP(mOwnType, (NIIi)bpp))
        {
            if(bpp == 32 && PixelUtil::hasAlpha(srcpf) && FreeImage_FIFSupportsExportBPP(mOwnType, 24))
            {
                // drop to 24 bit (lose alpha)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                dstpf = PF_BYTE_RGB;
#else
                dstpf = PF_BYTE_BGR;
#endif
                bpp = 24;
            }
            else if(bpp == 128 && PixelUtil::hasAlpha(srcpf) && FreeImage_FIFSupportsExportBPP(mOwnType, 96))
            {
                // drop to 96-bit floating point
                dstpf = PF_R32G32B32;
            }
        }

        PixelBlock convBox(srcw, srch, 1, dstpf);
        if(dstpf != srcpf)
        {
            conversionRequired = true;
            // Allocate memory
            convBox.mData = N_alloc_t(Nui8, convBox.getWholeSliceSize());
            // perform conversion and reassign source
            PixelBlock newSrc(srcw, srch, 1, srcpf, in->mBuffer);
            PixelUtil::bulkPixelConversion(newSrc, convBox);
            srcData = convBox.mData;

        }

        ret = FreeImage_AllocateT(imageType, static_cast<int>(srcw), static_cast<int>(srch), bpp);

        if(!ret)
        {
            if (conversionRequired)
                N_free(convBox.mData);

            return CodecResult_Suspend;
        }

        if (dstpf == PF_L8ui || dstpf == PF_A8UN)
        {
            // Must explicitly tell FreeImage that this is greyscale by setting
            // a "grey" palette (otherwise it will save as a normal RGB
            // palettized image).
            FIBITMAP * tmp = FreeImage_ConvertToGreyscale(ret);
            FreeImage_Unload(ret);
            ret = tmp;
        }

        NCount dstPitch = FreeImage_GetPitch(ret);
        NCount srcPitch = srcw * PixelUtil::getNumElemBytes(dstpf);

        // Copy data, invert scanlines and respect FreeImage pitch
        Nui8 * pSrc;
        Nui8 * pDst = FreeImage_GetBits(ret);
        for(NCount y = 0; y < srch; ++y)
        {
            pSrc = srcData + (srch - y - 1) * srcPitch;
            memcpy(pDst, pSrc, srcPitch);
            pDst += dstPitch;
        }

        if(conversionRequired)
        {
            // delete temporary conversion area
            N_free(convBox.mData);
        }
        return ret;
    }
    //---------------------------------------------------------------------
    CodecResult FreeImageCodec::_decode(FIBITMAP * in, Image * out) const
    {
        out->mWidth = FreeImage_GetWidth(in);
        out->mHeight = FreeImage_GetHeight(in);
        out->mDepth  = 1;// only 2D formats handled by this codec
        out->mMipmapCount = 0;// no mipmaps in non-DDS
        out->mFlags = 0;

        // Must derive format first, this may perform conversions
        FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(in);
        FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(in);
        unsigned bpp = FreeImage_GetBPP(in);

        switch(imageType)
        {
        case FIT_UNKNOWN:
        case FIT_COMPLEX:
        case FIT_UINT32:
        case FIT_INT32:
        case FIT_DOUBLE:
        default:
            return CodecResult_NotMatch;
            break;
        case FIT_BITMAP:
            // Standard image type
            // Perform any colour conversions for greyscale
            if(colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
            {
                FIBITMAP * newBitmap = FreeImage_ConvertToGreyscale(in);
                // free old bitmap and replace
                FreeImage_Unload(in);
                in = newBitmap;
                // get new formats
                bpp = FreeImage_GetBPP(in);
            }
            // Perform any colour conversions for RGB
            else if(bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
            {
                FIBITMAP * newBitmap =  NULL;
                if (FreeImage_IsTransparent(in))
                {
                    // convert to 32 bit to preserve the transparency
                    // (the alpha byte will be 0 if pixel is transparent)
                    newBitmap = FreeImage_ConvertTo32Bits(in);
                }
                else
                {
                    // no transparency - only 3 bytes are needed
                    newBitmap = FreeImage_ConvertTo24Bits(in);
                }
                // free old bitmap and replace
                FreeImage_Unload(in);
                in = newBitmap;
                // get new formats
                bpp = FreeImage_GetBPP(in);
            }

            // by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
            switch(bpp)
            {
            case 8:
                out->mFormat = PF_L8ui;
                break;
            case 16:
                // Determine 555 or 565 from green mask
                // cannot be 16-bit greyscale since that's FIT_UINT16
                if(FreeImage_GetGreenMask(in) == FI16_565_GREEN_MASK)
                {
                    out->mFormat = PF_R5G6B5;
                }
                else
                {
                    // FreeImage doesn't support 4444 format so must be 1555
                    out->mFormat = PF_A1R5G5B5;
                }
                break;
            case 24:
                // FreeImage differs per platform
                //     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
                //     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                out->mFormat = PF_BYTE_RGB;
#else
                out->mFormat = PF_BYTE_BGR;
#endif
                break;
            case 32:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                out->mFormat = PF_BYTE_RGBA;
#else
                out->mFormat = PF_BYTE_BGRA;
#endif
                break;
            };
            break;
        case FIT_UINT16:
        case FIT_INT16:
            // 16-bit greyscale
            out->mFormat = PF_L16ui;
            break;
        case FIT_FLOAT:
            // Single-component floating point data
            out->mFormat = PF_R32;
            break;
        case FIT_RGB16:
            out->mFormat = PF_R16G16B16ui;
            break;
        case FIT_RGBA16:
            out->mFormat = PF_R16G16B16A16ui;
            break;
        case FIT_RGBF:
            out->mFormat = PF_R32G32B32;
            break;
        case FIT_RGBAF:
            out->mFormat = PF_R32G32B32A32;
            break;
        };

        Nui8 * srcData = FreeImage_GetBits(in);
        NCount srcPitch = FreeImage_GetPitch(in);

        // Final data - invert image and trim pitch at the same time
        NCount dstPitch = out->mWidth * PixelUtil::getNumElemBytes(out->mFormat);
        out->mSize = dstPitch * out->mHeight;
        out->mBuffer = N_alloc_t(Nui8, out->mSize);
        NCount height = out->mHeight;

        Nui8 * pDst = out->mBuffer;
        Nui8 * pSrc;

        for(NCount y = 0; y < height; ++y)
        {
            pSrc = srcData + (height - 1 - y) * srcPitch;
            memcpy(pDst, pSrc, dstPitch);
            pDst += dstPitch;
        }

        FreeImage_Unload(in);
    }
    //---------------------------------------------------------------------
    CodecResult FreeImageCodec::encode(const Image * in, const String & out, const String & hint) const
    {
        FIBITMAP * fiBitmap = _encode(ej);
        FreeImage_Save(mOwnType, fiBitmap, out.c_str());
        FreeImage_Unload(fiBitmap);
    }
    //---------------------------------------------------------------------
    CodecResult FreeImageCodec::encode(const Image * in, DataStream *& out, const String & hint) const
    {
        FIBITMAP * fiBitmap = _encode(in);
        // open memory chunk allocated by FreeImage
        FIMEMORY * mem = FreeImage_OpenMemory();
        // write data into memory
        FreeImage_SaveToMemory(mOwnType, fiBitmap, mem);
        // Grab data information
        BYTE * data;
        DWORD size;
        FreeImage_AcquireMemory(mem, &data, &size);

        out = N_new MemStream(size);
        out->write(data, size);

        // Now free FreeImage memory buffers
        FreeImage_CloseMemory(mem);
        // Unload bitmap
        FreeImage_Unload(fiBitmap);
    }
    //---------------------------------------------------------------------
    CodecResult FreeImageCodec::decode(const String & in, Image * out) const
    {
        FIBITMAP * fiBitmap = NULL;

        fiBitmap = FreeImage_Load(mOwnType, in.c_str());

        if(!fiBitmap)
        {
            return CodecResult_Error;
        }
        return _decode(fiBitmap, out);
    }
    //---------------------------------------------------------------------
    CodecResult FreeImageCodec::decode(DataStream * in, Image * out) const
    {
        FIBITMAP * fiBitmap = NULL;

        MemStream * mds = static_cast<MemStream *>(in);

        FIMEMORY * fiMem = FreeImage_OpenMemory(mds->getData(), static_cast<DWORD>(mds->size()));
        fiBitmap = FreeImage_LoadFromMemory(mOwnType, fiMem);

        if(!fiBitmap)
        {
            return CodecResult_Error;
        }

        FreeImage_CloseMemory(fiMem);

        return _decode(fiBitmap, out);
    }
    //---------------------------------------------------------------------
    bool FreeImageCodec::match(const String & hint) const
    {
        return false;
    }
    //---------------------------------------------------------------------
    String FreeImageCodec::match(const Nui8 * magic, NCount valid) const
    {
        FIMEMORY * fiMem = FreeImage_OpenMemory((BYTE *)magic, static_cast<DWORD>(valid));

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, (NIIi)valid);
        FreeImage_CloseMemory(fiMem);

        if(fif != FIF_UNKNOWN)
        {
            String ext(FreeImage_GetFormatFromFIF(fif));
            StrUtil::toLowerCase(ext);
            return ext;
        }
        else
        {
            return StrUtil::BLANK;
        }
    }
    //----------------------------------------------------------------------
}