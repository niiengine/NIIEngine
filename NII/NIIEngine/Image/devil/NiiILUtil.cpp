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

#include <NiiPreProcess.h>
#include "NiiILUtil.h"
#include <NiiPixelUtil.h>
#include <NiiBitwise.h>
#include <NiiColour.h>
#include <NiiException.h>

#include <IL/il.h>
#include <IL/ilu.h>

namespace NII
{
    //----------------------------------------------------------------------------
    // IL ÌØ¶¨º¯Êýspecific functions
    PixelFormat ILUtil::ilFormat2NiiFormat(NIIi ImageFormat, NIIi ImageType)
    {
        PixelFormat fmt = PF_UNKNOWN;
        switch(ImageFormat)
        {
        /* Compressed formats -- ignore type */
        case IL_DXT1:
            fmt = PF_BC1UN;
            break;
        //case IL_DXT2:
        //    fmt = PF_BC2UN;
            break;
        case IL_DXT3:
            fmt = PF_BC2UN;
            break;
        //case IL_DXT4:
        //    fmt = PF_DXT4;
        //    break;
        case IL_DXT5:
            fmt = PF_BC3UN;
            break;
        /* Normal formats */
        case IL_RGB:
            switch(ImageType)
            {
            case IL_FLOAT:
                fmt = PF_R32G32B32;
                break;
            case IL_UNSIGNED_SHORT:
            case IL_SHORT:
                fmt = PF_R16G16B16A16ui;
                break;
            default:
                fmt = PF_BYTE_RGB;
                break;
            }
            break;
        case IL_BGR:
            switch(ImageType)
            {
            case IL_FLOAT:
                fmt = PF_R32G32B32;
                break;
            case IL_UNSIGNED_SHORT:
            case IL_SHORT:
                fmt = PF_R16G16B16A16ui;
                break;
            default:
                fmt = PF_BYTE_BGR;
                break;
            }
            break;
        case IL_RGBA:
            switch(ImageType)
            {
            case IL_FLOAT:
                fmt = PF_R32G32B32A32;
                break;
            case IL_UNSIGNED_SHORT:
            case IL_SHORT:
                fmt = PF_R16G16B16A16ui;
                break;
            default:
                fmt = PF_BYTE_RGBA;
                break;
            }
            break;
        case IL_BGRA:
            switch(ImageType)
            {
            case IL_FLOAT:
                fmt = PF_R32G32B32A32;
                break;
            case IL_UNSIGNED_SHORT:
            case IL_SHORT:
                fmt = PF_R16G16B16A16ui;
                break;
            default:
                fmt = PF_BYTE_BGRA;
                break;
            }
            break;
        case IL_LUMINANCE:
            switch(ImageType)
            {
            case IL_BYTE:
            case IL_UNSIGNED_BYTE:
                fmt = PF_L8ui;
                break;
            default:
                fmt = PF_L16ui;
            }
            break;
        case IL_LUMINANCE_ALPHA:
            fmt = PF_L8A8i;
            break;
        }
        return fmt;
    }
    //-----------------------------------------------------------------------
    ILUtil::ILFormat ILUtil::NiiFormat2ilFormat(PixelFormat pf)
    {
        switch(pf)
        {
            case PF_L8ui:
                return ILFormat(1, IL_LUMINANCE, IL_UNSIGNED_BYTE);
            case PF_A8UN:
                return ILFormat(1, IL_LUMINANCE, IL_UNSIGNED_BYTE);
            case PF_L16ui:
                return ILFormat(1, IL_LUMINANCE, IL_UNSIGNED_SHORT);
            case PF_L8A8i:
                return ILFormat(2, IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
            case PF_BYTE_RGB:
                return ILFormat(3, IL_RGB, IL_UNSIGNED_BYTE);
            case PF_BYTE_RGBA:
                return ILFormat(4, IL_RGBA, IL_UNSIGNED_BYTE);
            case PF_BYTE_BGR:
                return ILFormat(3, IL_BGR, IL_UNSIGNED_BYTE);
            case PF_BYTE_BGRA:
                return ILFormat(4, IL_BGRA, IL_UNSIGNED_BYTE);
            case PF_R16G16B16A16ui:
                return ILFormat(4, IL_RGBA, IL_UNSIGNED_SHORT);
            case PF_R32G32B32:
                return ILFormat(3, IL_RGB, IL_FLOAT);
            case PF_R32G32B32A32:
                return ILFormat(4, IL_RGBA, IL_FLOAT);
            case PF_BC1UN:
                return ILFormat(0, IL_DXT1);
            //case PF_BC2UN:
            //    return ILFormat(0, IL_DXT2);
            case PF_BC2UN:
                return ILFormat(0, IL_DXT3);
            //case PF_DXT4:
            //    return ILFormat(0, IL_DXT4);
            case PF_BC3UN:
                return ILFormat(0, IL_DXT5);
            default:
                return ILFormat();
        }
    }
    //-----------------------------------------------------------------------
    inline void packI(Nui8 r, Nui8 g, Nui8 b, Nui8 a, PixelFormat pf, Nui8 * dest)
    {
        PixelUtil::packColour(r, g, b, a, pf, dest);
    }
    //-----------------------------------------------------------------------
    inline void packI(Nui16 r, Nui16 g, Nui16 b, Nui16 a, PixelFormat pf, Nui8 * dest)
    {
        PixelUtil::packColour((NIIf)r/65535.0f, (NIIf)g/65535.0f,
            (NIIf)b/65535.0f, (NIIf)a/65535.0f, pf, dest);
    }
    //-----------------------------------------------------------------------
    inline void packI(NIIf r, NIIf g, NIIf b, NIIf a, PixelFormat pf, Nui8 * dest)
    {
        PixelUtil::packColour(r, g, b, a, pf, dest);
    }
    //-----------------------------------------------------------------------
    template <typename T> void ilToNiiInternal(Nui8 * tar, PixelFormat niifmt, T r, T g, T b, T a)
    {
        const NIIi ilfmt = ilGetInteger(IL_IMAGE_FORMAT);
        T *src = (T*)ilGetData();
        T *srcend = (T*)((Nui8*)ilGetData() + ilGetInteger(IL_IMAGE_SIZE_OF_DATA));
        const NCount elemSize = PixelUtil::getNumElemBytes(niifmt);
        while(src < srcend)
        {
            switch(ilfmt)
            {
            case IL_RGB:
                r = src[0];
                g = src[1];
                b = src[2];
                src += 3;
                break;
            case IL_BGR:
                b = src[0];
                g = src[1];
                r = src[2];
                src += 3;
                break;
            case IL_LUMINANCE:
                r = src[0];
                g = src[0];
                b = src[0];
                src += 1;
                break;
            case IL_LUMINANCE_ALPHA:
                r = src[0];
                g = src[0];
                b = src[0];
                a = src[1];
                src += 2;
                break;
            case IL_RGBA:
                r = src[0];
                g = src[1];
                b = src[2];
                a = src[3];
                src += 4;
                break;
            case IL_BGRA:
                b = src[0];
                g = src[1];
                r = src[2];
                a = src[3];
                src += 4;
                break;
            default:
                return;
            }
            packI(r, g, b, a, niifmt, tar);
            tar += elemSize;
        }

    }
    //-----------------------------------------------------------------------
    // Utility function to convert IL data types to UNSIGNED_
    ILenum ILabs(ILenum in)
    {
        switch(in)
        {
        case IL_INT: return IL_UNSIGNED_INT;
        case IL_BYTE: return IL_UNSIGNED_BYTE;
        case IL_SHORT: return IL_UNSIGNED_SHORT;
        default: return in;
        }
    }
    //-----------------------------------------------------------------------
    void ILUtil::to(const PixelBlock &dst)
    {
        if(!dst.isWholeSlice())
            N_EXCEPT( UnImpl,
                _T("Destination must currently be consecutive")) ;
        if(dst.getWidth() != static_cast<NCount>(ilGetInteger( IL_IMAGE_WIDTH )) ||
            dst.getHeight() != static_cast<NCount>(ilGetInteger( IL_IMAGE_HEIGHT )) ||
                dst.getDepth() != static_cast<NCount>(ilGetInteger( IL_IMAGE_DEPTH )))
            N_EXCEPT( InvalidParam,
                _T("Destination dimensions must equal IL dimension"));

        NIIi ilfmt = ilGetInteger( IL_IMAGE_FORMAT );
        NIIi iltp = ilGetInteger( IL_IMAGE_TYPE );

        // Check if in-memory format just matches
        // If yes, we can just copy it and save conversion
        ILFormat ifmt = NiiFormat2ilFormat( dst.format );
        if(ifmt.format == ilfmt && ILabs(ifmt.type) == ILabs(iltp)) {
            memcpy(dst.data, ilGetData(), ilGetInteger( IL_IMAGE_SIZE_OF_DATA ));
            return;
        }
        // Try if buffer is in a known NII format so we can use NII its
        // conversion routines
        PixelFormat bufFmt = ilFormat2NiiFormat((NIIi)ilfmt, (NIIi)iltp);

        ifmt = NiiFormat2ilFormat( bufFmt );
        if(ifmt.format == ilfmt && ILabs(ifmt.type) == ILabs(iltp))
        {
            // IL format matches another NII format
            PixelBlock src(dst.getWidth(), dst.getHeight(), dst.getDepth(), bufFmt, ilGetData());
            PixelUtil::bulkPixelConversion(src, dst);
            return;
        }

        // Thee extremely slow method
        if(iltp == IL_UNSIGNED_BYTE || iltp == IL_BYTE)
        {
            ilToNiiInternal(static_cast<Nui8*>(dst.data), dst.format, (Nui8)0x00,(Nui8)0x00,(Nui8)0x00,(Nui8)0xFF);
        }
        else if(iltp == IL_FLOAT)
        {
            ilToNiiInternal(static_cast<Nui8*>(dst.data), dst.format, 0.0f,0.0f,0.0f,1.0f);
        }
        else if(iltp == IL_SHORT || iltp == IL_UNSIGNED_SHORT)
        {
            ilToNiiInternal(static_cast<Nui8*>(dst.data), dst.format,
                (Nui16)0x0000,(Nui16)0x0000,(Nui16)0x0000,(Nui16)0xFFFF);
        }
        else
        {
            N_EXCEPT( UnImpl, _T("Cannot convert this DevIL type")) ;
        }
    }
    //-----------------------------------------------------------------------
    void ILUtil::from(const PixelBlock & src)
    {
        // ilTexImage http://openil.sourceforge.net/docs/il/f00059.htm
        ILFormat ifmt = NiiFormat2ilFormat( src.mFormat );
        if(src.isWholeSlice() && ifmt.isValid())
        {
            // The easy case, the buffer is laid out in memory just like
            // we want it to be and is in a format DevIL can understand directly
            // We could even save the copy if DevIL would let us
            ilTexImage(static_cast<ILuint>(src.getWidth()),
                static_cast<ILuint>(src.getHeight()),
                static_cast<ILuint>(src.getDepth()), ifmt.numberOfChannels,
                ifmt.format, ifmt.type, src.mData);
        }
        else if(ifmt.isValid())
        {
            // The format can be understood directly by DevIL. The only
            // problem is that ilTexImage expects our image data consecutively
            // so we cannot use that directly.

            // Let DevIL allocate the memory for us, and copy the data consecutively
            // to its memory
            ilTexImage(static_cast<ILuint>(src.getWidth()), static_cast<ILuint>(src.getHeight()),
                static_cast<ILuint>(src.getDepth()), ifmt.numberOfChannels, ifmt.format, ifmt.type, 0);
            PixelBlock dst(src.getWidth(), src.getHeight(), src.getDepth(), src.mFormat, ilGetData());
            PixelUtil::bulkPixelConversion(src, dst);
        }
        else
        {
            // Here it gets ugly. We're stuck with a pixel format that DevIL
            // can't do anything with. We will do a bulk pixel conversion and
            // then feed it to DevIL anyway. The problem is finding the best
            // format to convert to.

            // most general format supported by NII and DevIL
            PixelFormat fmt = PixelUtil::hasAlpha(src.mFormat) ? PF_R32G32B32A32:PF_R32G32B32;

            // Make up a pixel format
            // We don't have to consider luminance formats as they have
            // straight conversions to DevIL, just weird permutations of RGBA an LA
            NIIi depths[4];
            PixelUtil::getBitDepths(src.mFormat, depths);

            // Native endian format with all bit depths<8 can safely and quickly be
            // converted to 24/32 bit
            if(PixelUtil::isNativeEndian(src.mFormat) &&
                depths[0]<=8 && depths[1]<=8 && depths[2]<=8 && depths[3]<=8)
            {
                if(PixelUtil::hasAlpha(src.mFormat))
                {
                    fmt = PF_A8R8G8B8;
                }
                else
                {
                    fmt = PF_R8G8B8UN;
                }
            }

            // Let DevIL allocate the memory for us, then do the conversion ourselves
            ifmt = NiiFormat2ilFormat( fmt );
            ilTexImage(static_cast<ILuint>(src.getWidth()), static_cast<ILuint>(src.getHeight()),
                static_cast<ILuint>(src.getDepth()),  ifmt.numberOfChannels, ifmt.format, ifmt.type, 0);
            PixelBlock dst(src.getWidth(), src.getHeight(), src.getDepth(), fmt, ilGetData());
            PixelUtil::bulkPixelConversion(src, dst);
        }
    }
    //--------------------------------------------------------------------------------
}