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
#include "NiiDDSCodec.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"
#include "NiiImage.h"
#include "NiiException.h"
#include "NiiStrConv.h"

namespace NII
{
#if N_COMPILER == N_CPP_MSVC
#pragma pack (push, 1)
#else
#pragma pack (1)
#endif
    //----------------------------------------------------------------------
    // Nested structure
    struct DDSPixelFormat
    {
        Nui32 size;
        Nui32 flags;
        Nui32 fourCC;
        Nui32 rgbBits;
        Nui32 redMask;
        Nui32 greenMask;
        Nui32 blueMask;
        Nui32 alphaMask;
    };
    //---------------------------------------------------------------------
    // Nested structure
    struct DDSCaps
    {
        Nui32 caps1;
        Nui32 caps2;
        Nui32 reserved[2];
    };
    //---------------------------------------------------------------------
    // Main header, note preceded by 'DDS '
    struct DDSHeader
    {
        Nui32 size;
        Nui32 flags;
        Nui32 height;
        Nui32 width;
        Nui32 sizeOrPitch;
        Nui32 depth;
        Nui32 mipMapCount;
        Nui32 reserved1[11];
        DDSPixelFormat pixelFormat;
        DDSCaps caps;
        Nui32 reserved2;
    };
    //--------------------------------------------------------------------------------
    // An 8-byte DXT colour block, represents a 4x4 texel area. Used by all DXT formats
    struct DXTColourBlock
    {
        // 2 colour ranges
        Nui16 colour_0;
        Nui16 colour_1;
        // 16 2-bit indexes, each byte here is one row
        Nui8 indexRow[4];
    };
    //--------------------------------------------------------------------------------
    // An 8-byte DXT explicit alpha block, represents a 4x4 texel area. Used by DXT2/3
    struct DXTExplicitAlphaBlock
    {
        // 16 4-bit values, each 16-bit value is one row
        Nui16 alphaRow[4];
    };
    //--------------------------------------------------------------------------------
    // An 8-byte DXT interpolated alpha block, represents a 4x4 texel area. Used by DXT4/5
    struct DXTInterpolatedAlphaBlock
    {
        // 2 alpha ranges
        Nui8 alpha_0;
        Nui8 alpha_1;
        // 16 3-bit indexes. Unfortunately 3 bits doesn't map too well to row bytes
        // so just stored raw
        Nui8 indexes[6];
    };
    //---------------------------------------------------------------------------------
#if N_COMPILER == N_CPP_MSVC
    #pragma pack(pop)
#else
    #pragma pack()
#endif
    //---------------------------------------------------------------------------------
    const Nui32 DDS_MAGIC = NII_MAGIC4('D', 'D', 'S', ' ');
    const Nui32 DDS_PIXELFORMAT_SIZE = 8 * sizeof(Nui32);
    const Nui32 DDS_CAPS_SIZE = 4 * sizeof(Nui32);
    const Nui32 DDS_HEADER_SIZE = 19 * sizeof(Nui32) + DDS_PIXELFORMAT_SIZE + DDS_CAPS_SIZE;

    const Nui32 DDSD_CAPS = 0x00000001;
    const Nui32 DDSD_HEIGHT = 0x00000002;
    const Nui32 DDSD_WIDTH = 0x00000004;
    const Nui32 DDSD_PITCH = 0x00000008;
    const Nui32 DDSD_PIXELFORMAT = 0x00001000;
    const Nui32 DDSD_MIPMAPCOUNT = 0x00020000;
    const Nui32 DDSD_LINEARSIZE = 0x00080000;
    const Nui32 DDSD_DEPTH = 0x00800000;
    const Nui32 DDPF_ALPHAPIXELS = 0x00000001;
    const Nui32 DDPF_FOURCC = 0x00000004;
    const Nui32 DDPF_RGB = 0x00000040;
    const Nui32 DDSCAPS_COMPLEX = 0x00000008;
    const Nui32 DDSCAPS_TEXTURE = 0x00001000;
    const Nui32 DDSCAPS_MIPMAP = 0x00400000;
    const Nui32 DDSCAPS2_CUBEMAP = 0x00000200;
    const Nui32 DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
    const Nui32 DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
    const Nui32 DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
    const Nui32 DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
    const Nui32 DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
    const Nui32 DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
    const Nui32 DDSCAPS2_VOLUME = 0x00200000;

    // Special FourCC codes
    const Nui32 D3DFMT_R16F            = 111;
    const Nui32 D3DFMT_G16R16F         = 112;
    const Nui32 D3DFMT_A16B16G16R16F   = 113;
    const Nui32 D3DFMT_R32F            = 114;
    const Nui32 D3DFMT_G32R32F         = 115;
    const Nui32 D3DFMT_A32B32G32R32F   = 116;

    //---------------------------------------------------------------------
    DDSCodec * DDSCodec::msInstance = 0;
    //---------------------------------------------------------------------
    DDSCodec::DDSCodec() :
        ImageCodec(N_IMAGE_DDS, "dds")
    {
    }
    //---------------------------------------------------------------------
    DDSCodec::~DDSCodec()
    {
    }
    //---------------------------------------------------------------------
    CodecResult DDSCodec::encode(const Image * in, const String & out, const String & hint)
    {
        NCount width = in->getWidth();
        NCount height = in->getHeight();
        NCount depth = in->getDepth();
        NCount size = in->getSize();
        PixelFormat format = in->getFormat();
        // Check size for cube map faces
        bool isCubeMap = (size == PixelUtil::getSize(width, height, depth, 6, format, in->getMipmapCount()));

        // Establish texture attributes
        bool isVolume = (depth > 1);
        bool isFloat32r = (format == PF_R32);
        bool hasAlpha = false;
        bool notImplemented = false;
        String notImplementedString = "";

        // Check for all the 'not implemented' conditions
        if (dest->getMipmapCount() != 0)
        {
            // No mip map functionality yet
            notImplemented = true;
            notImplementedString += " mipmaps";
        }

        if ((isVolume == true)&&(width != height))
        {
            // Square textures only
            notImplemented = true;
            notImplementedString += _T(" non square textures");
        }

        Nui32 temp = 1;
        while (temp < width)
        {
            temp <<= 1;
        }
        if (temp != width)
        {
            // Power two textures only
            notImplemented = true;
            notImplementedString += _T(" non power two textures");
        }

        switch(format)
        {
        case PF_A8R8G8B8:
        case PF_X8R8G8B8:
        case PF_R8G8B8UN:
        case PF_R32:
            break;
        default:
            // No crazy NII_MAGIC4 or 565 et al. file formats at this stage
            notImplemented = true;
            notImplementedString = _T(" unsupported pixel format");
            break;
        }

        // Except if any 'not implemented' conditions were met
        if(notImplemented)
        {
            return CodecResult_NoImpl;
        }
        else
        {
            // Build header and write to disk

            // Variables for some DDS header flags
            Nui32 ddsHeaderFlags = 0;
            Nui32 ddsHeaderRgbBits = 0;
            Nui32 ddsHeaderSizeOrPitch = 0;
            Nui32 ddsHeaderCaps1 = 0;
            Nui32 ddsHeaderCaps2 = 0;
            Nui32 ddsMagic = DDS_MAGIC;

            // Initalise the header flags
            ddsHeaderFlags = (isVolume) ?
                DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_DEPTH|DDSD_PIXELFORMAT :
                DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;

            // Initalise the rgbBits flags
            switch(format)
            {
            case PF_A8R8G8B8:
                ddsHeaderRgbBits = 8 * 4;
                hasAlpha = true;
                break;
            case PF_X8R8G8B8:
                ddsHeaderRgbBits = 8 * 4;
                break;
            case PF_R8G8B8UN:
                ddsHeaderRgbBits = 8 * 3;
                break;
            case PF_R32:
                ddsHeaderRgbBits = 32;
                break;
            default:
                ddsHeaderRgbBits = 0;
                break;
            }

            // Initalise the SizeOrPitch flags (power two textures for now)
            ddsHeaderSizeOrPitch = ddsHeaderRgbBits * width;

            // Initalise the caps flags
            ddsHeaderCaps1 = (isVolume||isCubeMap) ? DDSCAPS_COMPLEX|DDSCAPS_TEXTURE : DDSCAPS_TEXTURE;
            if (isVolume)
            {
                ddsHeaderCaps2 = DDSCAPS2_VOLUME;
            }
            else if (isCubeMap)
            {
                ddsHeaderCaps2 = DDSCAPS2_CUBEMAP|
                DDSCAPS2_CUBEMAP_POSITIVEX|DDSCAPS2_CUBEMAP_NEGATIVEX|
                DDSCAPS2_CUBEMAP_POSITIVEY|DDSCAPS2_CUBEMAP_NEGATIVEY|
                DDSCAPS2_CUBEMAP_POSITIVEZ|DDSCAPS2_CUBEMAP_NEGATIVEZ;
            }

            // Populate the DDS header information
            DDSHeader ddsHeader;
            ddsHeader.size = DDS_HEADER_SIZE;
            ddsHeader.flags = ddsHeaderFlags;
            ddsHeader.width = (Nui32)width;
            ddsHeader.height = (Nui32)height;
            ddsHeader.depth = (Nui32)(isVolume ? depth : 0);
            ddsHeader.depth = (Nui32)(isCubeMap ? 6 : ddsHeader.depth);
            ddsHeader.mipMapCount = 0;
            ddsHeader.sizeOrPitch = ddsHeaderSizeOrPitch;
            for (Nui32 reserved1=0; reserved1<11; reserved1++) // XXX nasty constant 11
            {
                ddsHeader.reserved1[reserved1] = 0;
            }
            ddsHeader.reserved2 = 0;

            ddsHeader.pixelFormat.size = DDS_PIXELFORMAT_SIZE;
            ddsHeader.pixelFormat.flags = (hasAlpha) ? DDPF_RGB|DDPF_ALPHAPIXELS : DDPF_RGB;
            ddsHeader.pixelFormat.flags = (isFloat32r) ? DDPF_FOURCC : ddsHeader.pixelFormat.flags;
            ddsHeader.pixelFormat.fourCC = (isFloat32r) ? D3DFMT_R32F : 0;
            ddsHeader.pixelFormat.rgbBits = ddsHeaderRgbBits;

            ddsHeader.pixelFormat.alphaMask = (hasAlpha)   ? 0xFF000000 : 0x00000000;
            ddsHeader.pixelFormat.alphaMask = (isFloat32r) ? 0x00000000 : ddsHeader.pixelFormat.alphaMask;
            ddsHeader.pixelFormat.redMask   = (isFloat32r) ? 0xFFFFFFFF :0x00FF0000;
            ddsHeader.pixelFormat.greenMask = (isFloat32r) ? 0x00000000 :0x0000FF00;
            ddsHeader.pixelFormat.blueMask  = (isFloat32r) ? 0x00000000 :0x000000FF;

            ddsHeader.caps.caps1 = ddsHeaderCaps1;
            ddsHeader.caps.caps2 = ddsHeaderCaps2;
            ddsHeader.caps.reserved[0] = 0;
            ddsHeader.caps.reserved[1] = 0;

            // Swap endian
            flipL(&ddsMagic, sizeof(Nui32), 1);
            flipL(&ddsHeader, 4, sizeof(DDSHeader) / 4);

            // Write the file
            std::ofstream of;
            of.open(out.c_str(), std::ios_base::binary|std::ios_base::out);
            of.write((const NIIb *)&ddsMagic, sizeof(Nui32));
            of.write((const NIIb *)&ddsHeader, DDS_HEADER_SIZE);
            // XXX flipL on each pixel chunk written unless isFloat32r ?
            of.write((const NIIb *)in->getData(), (Nui32)size);
            of.close();
            return CodecResult_Success;
        }
        return CodecResult_NotMatch;
    }
    //---------------------------------------------------------------------
    PixelFormat DDSCodec::convertFourCCFormat(Nui32 fourcc) const
    {
        // convert dxt pixel format
        switch(fourcc)
        {
        case NII_MAGIC4('D','X','T','1'):
            return PF_BC1UN;
        //case NII_MAGIC4('D','X','T','2'):
        //    return PF_BC2UN;
        case NII_MAGIC4('D','X','T','3'):
            return PF_BC2UN;
        //case NII_MAGIC4('D','X','T','4'):
        //    return PF_DXT4;
        case NII_MAGIC4('D','X','T','5'):
            return PF_BC3UN;
        case D3DFMT_R16F:
            return PF_R16;
        case D3DFMT_G16R16F:
            return PF_G16R16;
        case D3DFMT_A16B16G16R16F:
            return PF_R16G16B16A16;
        case D3DFMT_R32F:
            return PF_R32;
        case D3DFMT_G32R32F:
            return PF_G32R32;
        case D3DFMT_A32B32G32R32F:
            return PF_R32G32B32A32;
        // We could support 3Dc here, but only ATI cards support it, not nVidia
        default:
            N_EXCEPT(NoExist,
                _T("Unsupported FourCC format found in DDS file"));
        };
    }
    //---------------------------------------------------------------------
    PixelFormat DDSCodec::convertPixelFormat(Nmark rgbBits, Nmark r, Nmark g,
        Nmark b, Nmark a) const
    {
        // General search through pixel formats
        for(NIIi i = PF_UNKNOWN + 1; i < PF_COUNT; ++i)
        {
            PixelFormat pf = static_cast<PixelFormat>(i);
            if(PixelUtil::getNumElemBits(pf) == rgbBits)
            {
                Nui32 testMasks[4];
                PixelUtil::getBitMasks(pf, testMasks);
                NIIi testBits[4];
                PixelUtil::getBitDepths(pf, testBits);
                if(testMasks[0] == r && testMasks[1] == g && testMasks[2] == b &&
                    // for alpha, deal with 'X8' formats by checking bit counts
                    (testMasks[3] == a || (a == 0 && testBits[3] == 0)))
                {
                    return pf;
                }
            }

        }

        N_EXCEPT(NoExist, _T("Cannot determine pixel format"));
    }
    //---------------------------------------------------------------------
    void DDSCodec::unpackDXTColour(PixelFormat pf, const DXTColourBlock & block,
        Colour * c) const
    {
        // Note - we assume all values have already been endian swapped

        // Colour lookup table
        Colour destc[4];

        if (pf == PF_BC1UN && block.colour_0 <= block.colour_1)
        {
            // 1-bit alpha
            PixelUtil::unpackColour(&(destc[0]), PF_R5G6B5, (Nui8 *)&(block.colour_0));
            PixelUtil::unpackColour(&(destc[1]), PF_R5G6B5, (Nui8 *)&(block.colour_1));
            // one intermediate colour, half way between the other two
            destc[2] = (destc[0] + destc[1]) / 2;
            // transparent colour
            destc[3] = Colour::ZERO;
        }
        else
        {
            PixelUtil::unpackColour(&(destc[0]), PF_R5G6B5, (Nui8 *)&(block.colour_0));
            PixelUtil::unpackColour(&(destc[1]), PF_R5G6B5, (Nui8 *)&(block.colour_1));
            // first interpolated colour, 1/3 of the way along
            destc[2] = (2 * destc[0] + destc[1]) / 3;
            // second interpolated colour, 2/3 of the way along
            destc[3] = (destc[0] + 2 * destc[1]) / 3;
        }

        // Process 4x4 block of texels
        for (NCount row = 0; row < 4; ++row)
        {
            for (NCount x = 0; x < 4; ++x)
            {
                // LSB come first
                Nui8 colIdx = static_cast<Nui8>(block.indexRow[row] >> (x * 2) & 0x3);
                if (pf == PF_BC1UN)
                {
                    // Overwrite entire colour
                    c[(row * 4) + x] = destc[colIdx];
                }
                else
                {
                    // alpha has already been read (alpha precedes colour)
                    Colour & col = c[(row * 4) + x];
                    col.r = destc[colIdx].r;
                    col.g = destc[colIdx].g;
                    col.b = destc[colIdx].b;
                }
            }
        }
    }
    //---------------------------------------------------------------------
    void DDSCodec::unpackDXTAlpha(const DXTExplicitAlphaBlock & block, Colour * c) const
    {
        // Note - we assume all values have already been endian swapped

        // This is an explicit alpha block, 4 bits per pixel, LSB first
        for (NCount row = 0; row < 4; ++row)
        {
            for (NCount x = 0; x < 4; ++x)
            {
                // Shift and mask off to 4 bits
                Nui8 val = static_cast<Nui8>(block.alphaRow[row] >> (x * 4) & 0xF);
                // Convert to [0,1]
                c->a = (NIIf)val / (NIIf)0xF;
                c++;
            }
        }
    }
    //---------------------------------------------------------------------
    void DDSCodec::unpackDXTAlpha(const DXTInterpolatedAlphaBlock & block, Colour * pCol) const
    {
        // 8 derived alpha values to be indexed
        NIIf derivedAlphas[8];

        // Explicit extremes
        derivedAlphas[0] = block.alpha_0 / (NIIf)0xFF;
        derivedAlphas[1] = block.alpha_1 / (NIIf)0xFF;


        if (block.alpha_0 <= block.alpha_1)
        {
            // 4 interpolated alphas, plus zero and one
            // full range including extremes at [0] and [5]
            // we want to fill in [1] through [4] at weights ranging
            // from 1/5 to 4/5
            NIIf denom = 1.0f / 5.0f;
            for (NCount i = 0; i < 4; ++i)
            {
                NIIf factor0 = (4 - i) * denom;
                NIIf factor1 = (i + 1) * denom;
                derivedAlphas[i + 2] =
                    (factor0 * block.alpha_0) + (factor1 * block.alpha_1);
            }
            derivedAlphas[6] = 0.0f;
            derivedAlphas[7] = 1.0f;
        }
        else
        {
            // 6 interpolated alphas
            // full range including extremes at [0] and [7]
            // we want to fill in [1] through [6] at weights ranging
            // from 1/7 to 6/7
            NIIf denom = 1.0f / 7.0f;
            for (NCount i = 0; i < 6; ++i)
            {
                NIIf factor0 = (6 - i) * denom;
                NIIf factor1 = (i + 1) * denom;
                derivedAlphas[i + 2] =
                    (factor0 * block.alpha_0) + (factor1 * block.alpha_1);
            }
        }
        // Ok, now we've built the reference values, process the indexes
        for (NCount i = 0; i < 16; ++i)
        {
            NCount baseByte = (i * 3) / 8;
            NCount baseBit = (i * 3) % 8;
            Nui8 bits = static_cast<Nui8>(block.indexes[baseByte] >> baseBit & 0x7);
            // do we need to stitch in next byte too?
            if (baseBit > 5)
            {
                Nui8 extraBits = static_cast<Nui8>(
                    (block.indexes[baseByte+1] << (8 - baseBit)) & 0xFF);
                bits |= extraBits & 0x7;
            }
            pCol[i].a = derivedAlphas[bits];
        }
    }
    //---------------------------------------------------------------------
    CodecResult DDSCodec::decode(DataStream * in, Image * out) const
    {
		// Read 4 character code
		Nui32 fileType;
		in->read(&fileType, sizeof(Nui32));
		flipL(&fileType, sizeof(Nui32), 1);

		if(NII_MAGIC4('D', 'D', 'S', ' ') != fileType)
		{
			return CodecResult_NotMatch;
		}

		// Read header in full
		DDSHeader header;
		in->read(&header, sizeof(DDSHeader));

		// Endian flipL if required, all 32-bit values
		flipL(&header, 4, sizeof(DDSHeader) / 4);

		// Check some sizes
		if (header.size != DDS_HEADER_SIZE)
		{
            return CodecResult_Error;
		}
		if (header.pixelFormat.size != DDS_PIXELFORMAT_SIZE)
		{
			return CodecResult_Error;
		}

		out->mDepth = 1; // (deal with volume later)
		out->mWidth = header.width;
		out->mHeight = header.height;
		NCount numFaces = 1; // assume one face until we know otherwise

		if (header.caps.caps1 & DDSCAPS_MIPMAP)
		{
	        out->mMipmapCount = static_cast<Nui16>(header.mipMapCount - 1);
		}
		else
		{
			out->mMipmapCount = 0;
		}
		out->mFlags = 0;

		bool decompressDXT = false;
		// Figure out basic image type
		if (header.caps.caps2 & DDSCAPS2_CUBEMAP)
		{
			out->mFlags |= M_Cube;
			numFaces = 6;
		}
		else if (header.caps.caps2 & DDSCAPS2_VOLUME)
		{
			out->mFlags |= M_3D;
			out->mDepth = header.depth;
		}
		// Pixel format
		PixelFormat sourceFormat = PF_UNKNOWN;

		if (header.pixelFormat.flags & DDPF_FOURCC)
		{
			sourceFormat = convertFourCCFormat(header.pixelFormat.fourCC);
		}
		else
		{
			sourceFormat = convertPixelFormat(header.pixelFormat.rgbBits,
				header.pixelFormat.redMask, header.pixelFormat.greenMask,
				header.pixelFormat.blueMask,
				header.pixelFormat.flags & DDPF_ALPHAPIXELS ?
				header.pixelFormat.alphaMask : 0);
		}

		if (PixelUtil::isCompress(sourceFormat))
		{
            RenderSys * tmp;
            N_Only().get(tmp);
			if(tmp == NULL || tmp->getFeature()->isSupport(GF_Tex_Fromat_DXT) == false)
			{
				// We'll need to decompress
				decompressDXT = true;
				// Convert format
				switch (sourceFormat)
				{
				case PF_BC1UN:
					// source can be either 565 or 5551 depending on whether alpha present
					// unfortunately you have to read a block to figure out which
					// Note that we upgrade to 32-bit pixel formats here, even
					// though the source is 16-bit; this is because the interpolated
					// values will benefit from the 32-bit results, and the source
					// from which the 16-bit samples are calculated may have been
					// 32-bit so can benefit from this.
					DXTColourBlock block;
					in->read(&block, sizeof(DXTColourBlock));
					flipL(&(block.colour_0), sizeof(Nui16), 1);
					flipL(&(block.colour_1), sizeof(Nui16), 1);
					// skip back since we'll need to read this again
					in->skip(0 - (long)sizeof(DXTColourBlock));
					// colour_0 <= colour_1 means transparency in DXT1
					if (block.colour_0 <= block.colour_1)
					{
						out->mFormat = PF_BYTE_RGBA;
					}
					else
					{
						out->mFormat = PF_BYTE_RGB;
					}
					break;
				case PF_BC2UN:
				case PF_BC3UN:
					// full alpha present, formats vary only in encoding
					out->mFormat = PF_BYTE_RGBA;
					break;
                default:
                    // all other cases need no special format handling
                    break;
				}
			}
			else
			{
				// Use original format
				out->mFormat = sourceFormat;
				// Keep DXT data compressed
				out->mFlags |= M_Compress;
			}
		}
		else // not compressed
		{
			// Don't test against DDPF_RGB since greyscale DDS doesn't set this
			// just derive any other kind of format
			out->mFormat = sourceFormat;
		}

		// Calculate total size from number of mipmaps, faces and size
		out->mSize = PixelUtil::getSize(out->mWidth, out->mHeight, out->mDepth,  numFaces, out->mFormat, out->mMipmapCount);

        out->mBuffer = N_alloc_t(Nui8, out->mSize);

		// Now deal with the data
		Nui8 * destPtr = out->mBuffer;

		// all mips for a face, then each face
		for(NCount i = 0; i < numFaces; ++i)
		{
			NCount width = out->mWidth;
			NCount height = out->mheight;
			NCount depth = out->mDepth;

			for(NCount mip = 0; mip <= out->mMipmapCount; ++mip)
			{
				NCount dstPitch = width * PixelUtil::getNumElemBytes(out->mFormat);

				if(PixelUtil::isCompress(sourceFormat))
				{
					// Compressed data
					if(decompressDXT)
					{
						DXTColourBlock col;
						DXTInterpolatedAlphaBlock iAlpha;
						DXTExplicitAlphaBlock eAlpha;
						// 4x4 block of decompressed colour
						Colour tempColours[16];
						NCount destBpp = PixelUtil::getNumElemBytes(out->mFormat);
						NCount sx = std::min(width, 4);
						NCount sy = std::min(height, 4);
						NCount destPitchMinus4 = dstPitch - destBpp * sx;
						// slices are done individually
						for(NCount z = 0; z < depth; ++z)
						{
							// 4x4 blocks in x/y
							for(NCount y = 0; y < height; y += 4)
							{
								for(NCount x = 0; x < width; x += 4)
								{
									if(sourceFormat == PF_BC2UN)
									{
										// explicit alpha
										in->read(&eAlpha, sizeof(DXTExplicitAlphaBlock));
										flipL(eAlpha.alphaRow, sizeof(Nui16), 4);
										unpackDXTAlpha(eAlpha, tempColours) ;
									}
									else if(sourceFormat == PF_BC3UN)
									{
										// interpolated alpha
										in->read(&iAlpha, sizeof(DXTInterpolatedAlphaBlock));
										flipL(&(iAlpha.alpha_0), sizeof(Nui16), 1);
										flipL(&(iAlpha.alpha_1), sizeof(Nui16), 1);
										unpackDXTAlpha(iAlpha, tempColours) ;
									}
									// always read colour
									in->read(&col, sizeof(DXTColourBlock));
									flipL(&(col.colour_0), sizeof(Nui16), 1);
									flipL(&(col.colour_1), sizeof(Nui16), 1);
									unpackDXTColour(sourceFormat, col, tempColours);

									// write 4x4 block to uncompressed version
									for(NCount by = 0; by < sy; ++by)
									{
										for(NCount bx = 0; bx < sx; ++bx)
										{
											PixelUtil::packColour(tempColours[by * 4 + bx], out->mFormat, destPtr);
											destPtr = destPtr + destBpp;
										}
										// advance to next row
										destPtr = destPtr + destPitchMinus4;
									}
									// next block. Our dest pointer is 4 lines down
									// from where it started
									if(x + 4 >= width)
									{
										// Jump back to the start of the line
										destPtr = destPtr - destPitchMinus4;
									}
									else
									{
										// Jump back up 4 rows and 4 pixels to the
										// right to be at the next block to the right
										destPtr = destPtr - dstPitch * sy + destBpp * sx);
									}
								}
							}
						}

					}
					else
					{
						// load directly
						// DDS format lies! sizeOrPitch is not always set for DXT!!
						NCount dxtSize = PixelUtil::getSize(width, height, depth, 1, out->mFormat);
						in->read(destPtr, dxtSize);
						destPtr = destPtr + dxtSize);
					}
				}
				else
				{
					// Final data - trim incoming pitch
					NCount srcPitch;
					if(header.flags & DDSD_PITCH)
					{
						srcPitch = header.sizeOrPitch / std::max(1, mip * 2);
					}
					else
					{
						// assume same as final pitch
						srcPitch = dstPitch;
					}
					assert (dstPitch <= srcPitch);
					NIIl srcAdvance = srcPitch - dstPitch;

					for(NCount z = 0; z < out->mDepth; ++z)
					{
						for(NCount y = 0; y < out->mHeight; ++y)
						{
							in->read(destPtr, dstPitch);
							if(srcAdvance > 0)
								in->skip(srcAdvance);

							destPtr = destPtr + dstPitch;
						}
					}

				}
				if(width != 1)
                    width /= 2;
				if(height != 1)
                    height /= 2;
				if(depth != 1)
                    depth /= 2;
			}

		}
		return CodecResult_Success;
    }
    //---------------------------------------------------------------------
    bool DDSCodec::match(const String & hint) const
    {
        return false;
    }
    //---------------------------------------------------------------------
    String DDSCodec::match(const Nui8 * magic, NCount valid) const
    {
        if(valid >= sizeof(Nui32))
        {
            Nui32 fileType;
            memcpy(&fileType, magic, sizeof(Nui32));
            flipL(&fileType, sizeof(Nui32), 1);

            if (DDS_MAGIC == fileType)
            {
                return String("dds");
            }
        }

        return StrUtil::BLANK;
    }
    //--------------------------------------------------------------------
}