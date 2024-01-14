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

#ifndef _NII_IMAGERE_SAMPLER_H_
#define _NII_IMAGERE_SAMPLER_H_

#include <algorithm>

// 这个文件内联到 NiiImage.cpp!
namespace NII
{
    /** 变量名提示:
         sx_48 = 16/48-bit定点X的位置
         stepx = sx_48邻值的差异
         sx1 = 整数x下限的位置
         sx2 = 整数x上限的位置
         sxf = beween sx1和sx2间的分数权重
         x,y,z 输出像素的目的地

         邻近重新取样,不需要转换格式.
         模板每字节像素,是为了让编译器优化 这样做
         是为简化与memcpy()和取代bitshifts倍增
    */
    template<Nui32 elemsize> struct NearestResampler
    {
        static void scale(const PixelBlock & src, const PixelBlock & dst)
        {
            // srcdata stays at beginning, pdst is a moving pointer
            Nui8* srcdata = (Nui8*)src.getData( 0, 0, 0 );
            Nui8* dstdata = (Nui8*)dst.getData( 0, 0, 0 );

            // sx_48,sy_48,sz_48 represent current position in source
            // using 16/48-bit fixed precision, incremented by steps
            uint64 stepx = ((uint64)src.mWidth << 48) / dst.mWidth;
            uint64 stepy = ((uint64)src.mHeight << 48) / dst.mHeight;
            uint64 stepz = ((uint64)src.mDepth << 48) / dst.mDepth;

            // note: ((stepz>>1) - 1) is an extra half-step increment to adjust
            // for the center of the destination pixel, not the top-left corner
            uint64 sz_48 = (stepz >> 1) - 1;
            for (size_t z = 0; z < dst.getVolume(); z++, sz_48 += stepz) {
                size_t srczoff = (size_t)(sz_48 >> 48) * src.mSliceSize;
                
                uint64 sy_48 = (stepy >> 1) - 1;
                for (size_t y = 0; y < dst.mHeight; y++, sy_48 += stepy) {
                    size_t srcyoff = (size_t)(sy_48 >> 48) * src.mRowSize;
                
                    Nui8 *psrc = srcdata + srcyoff + srczoff;
                    Nui8 *pdst = dstdata + y * dst.mRowSize + z * dst.mSliceSize;

                    uint64 sx_48 = (stepx >> 1) - 1;
                    for (size_t x = 0; x < dst.mWidth; x++, sx_48 += stepx) {
                        memcpy( pdst, psrc + elemsize * ( size_t )( sx_48 >> 48 ), elemsize );
                        pdst += elemsize;
                    }
                }
            }
        }
    };
    //---------------------------------------------------------------------------------
    /// 默认浮点线性重采样,并格式转换
    struct LinearResampler
    {
        static void scale(const PixelBlock & src, const PixelBlock & dst)
        {
            size_t srcelemsize = PixelUtil::getUnitSize( src.mFormat );
            size_t dstelemsize = PixelUtil::getUnitSize( dst.mFormat );

            // srcdata stays at beginning, pdst is a moving pointer
            Nui8* srcdata = (Nui8*)src.getData( 0, 0, 0 );
            Nui8* dstdata = (Nui8*)dst.getData( 0, 0, 0 );
            
            // sx_48,sy_48,sz_48 represent current position in source
            // using 16/48-bit fixed precision, incremented by steps
            uint64 stepx = ((uint64)src.mWidth << 48) / dst.mWidth;
            uint64 stepy = ((uint64)src.mHeight << 48) / dst.mHeight;
            uint64 stepz = ((uint64)src.mDepth << 48) / dst.mDepth;
            
            // note: ((stepz>>1) - 1) is an extra half-step increment to adjust
            // for the center of the destination pixel, not the top-left corner
            uint64 sz_48 = (stepz >> 1) - 1;
            for (size_t z = 0; z < dst.getVolume(); z++, sz_48+=stepz) {
                // temp is 16/16 bit fixed precision, used to adjust a source
                // coordinate (x, y, or z) backwards by half a pixel so that the
                // integer bits represent the first sample (eg, sx1) and the
                // fractional bits are the blend weight of the second sample
                unsigned int temp = static_cast<unsigned int>(sz_48 >> 32);

                temp = (temp > 0x8000)? temp - 0x8000 : 0;
                uint32 sz1 = temp >> 16;                 // src z, sample #1
                uint32 sz2 = std::min(sz1+1,src.mDepth-1);// src z, sample #2
                float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

                uint64 sy_48 = (stepy >> 1) - 1;
                for (size_t y = 0; y < dst.mHeight; y++, sy_48+=stepy) {
                    temp = static_cast<unsigned int>(sy_48 >> 32);
                    temp = (temp > 0x8000)? temp - 0x8000 : 0;
                    uint32 sy1 = temp >> 16;                    // src y #1
                    uint32 sy2 = std::min(sy1+1,src.mHeight-1);// src y #2
                    float syf = (temp & 0xFFFF) / 65536.f; // weight of #2

                    Nui8 *pdst = dstdata + y * dst.mRowSize + z * dst.mSliceSize;

                    uint64 sx_48 = (stepx >> 1) - 1;
                    for (size_t x = 0; x < dst.mWidth; x++, sx_48+=stepx) {
                        temp = static_cast<unsigned int>(sx_48 >> 32);
                        temp = (temp > 0x8000)? temp - 0x8000 : 0;
                        uint32 sx1 = temp >> 16;                    // src x #1
                        uint32 sx2 = std::min(sx1+1,src.mWidth-1);// src x #2
                        float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2
                    
                        Colour x1y1z1, x2y1z1, x1y2z1, x2y2z1;
                        Colour x1y1z2, x2y1z2, x1y2z2, x2y2z2;

    #define UNPACK(dst,x,y,z) PixelUtil::unpackColour(&dst, src.mFormat, srcdata + srcelemsize*(x)+(y)*src.mRowSize+(z)*src.mSliceSize)

                        UNPACK(x1y1z1,sx1,sy1,sz1); UNPACK(x2y1z1,sx2,sy1,sz1);
                        UNPACK(x1y2z1,sx1,sy2,sz1); UNPACK(x2y2z1,sx2,sy2,sz1);
                        UNPACK(x1y1z2,sx1,sy1,sz2); UNPACK(x2y1z2,sx2,sy1,sz2);
                        UNPACK(x1y2z2,sx1,sy2,sz2); UNPACK(x2y2z2,sx2,sy2,sz2);
    #undef UNPACK

                        Colour accum =
                            x1y1z1 * ((1.0f - sxf)*(1.0f - syf)*(1.0f - szf)) +
                            x2y1z1 * (        sxf *(1.0f - syf)*(1.0f - szf)) +
                            x1y2z1 * ((1.0f - sxf)*        syf *(1.0f - szf)) +
                            x2y2z1 * (        sxf *        syf *(1.0f - szf)) +
                            x1y1z2 * ((1.0f - sxf)*(1.0f - syf)*        szf ) +
                            x2y1z2 * (        sxf *(1.0f - syf)*        szf ) +
                            x1y2z2 * ((1.0f - sxf)*        syf *        szf ) +
                            x2y2z2 * (        sxf *        syf *        szf );

                        PixelUtil::packColour( accum, dst.mFormat, pdst );

                        pdst += dstelemsize;
                    }
                }
            }
        }
    };
    //-----------------------------------------------------------------------------
    /** float32线性重采样,仅转换FLOAT32_RGB/FLOAT32_RGBA, 避免像素unpack/repack函数调用开销
    */
    struct LinearResampler_Float32
    {
        static void scale(const PixelBlock & src, const PixelBlock & dst)
        {
            size_t srcelemsize = PixelUtil::getUnitSize( src.mFormat );
            size_t dstelemsize = PixelUtil::getUnitSize( dst.mFormat );
            size_t minchannels = std::min( srcelemsize, dstelemsize ) / sizeof( float );

            // srcdata stays at beginning, pdst is a moving pointer
            Nui8* srcdata = (Nui8*)src.getData( 0, 0, 0 );
            Nui8* dstdata = (Nui8*)dst.getData( 0, 0, 0 );
            
            // sx_48,sy_48,sz_48 represent current position in source
            // using 16/48-bit fixed precision, incremented by steps
            uint64 stepx = ((uint64)src.mWidth << 48) / dst.mWidth;
            uint64 stepy = ((uint64)src.mHeight << 48) / dst.mHeight;
            uint64 stepz = ((uint64)src.mDepth << 48) / dst.mDepth;
            
            // note: ((stepz>>1) - 1) is an extra half-step increment to adjust
            // for the center of the destination pixel, not the top-left corner
            uint64 sz_48 = (stepz >> 1) - 1;
            for (size_t z = 0; z < dst.getVolume(); z++, sz_48+=stepz) {
                // temp is 16/16 bit fixed precision, used to adjust a source
                // coordinate (x, y, or z) backwards by half a pixel so that the
                // integer bits represent the first sample (eg, sx1) and the
                // fractional bits are the blend weight of the second sample
                unsigned int temp = static_cast<unsigned int>(sz_48 >> 32);

                temp = (temp > 0x8000)? temp - 0x8000 : 0;
                uint32 sz1 = temp >> 16;                 // src z, sample #1
                uint32 sz2 = std::min(sz1+1,src.mDepth-1);// src z, sample #2
                float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

                uint64 sy_48 = (stepy >> 1) - 1;
                for (size_t y = 0; y < dst.mHeight; y++, sy_48+=stepy) {
                    temp = static_cast<unsigned int>(sy_48 >> 32);
                    temp = (temp > 0x8000)? temp - 0x8000 : 0;
                    uint32 sy1 = temp >> 16;                    // src y #1
                    uint32 sy2 = std::min(sy1+1,src.mHeight-1);// src y #2
                    float syf = (temp & 0xFFFF) / 65536.f; // weight of #2

                    Nui8 *pdst = (Nui8*)(dstdata + y * dst.mRowSize + z * dst.mSliceSize);

                    uint64 sx_48 = (stepx >> 1) - 1;
                    for (size_t x = 0; x < dst.mWidth; x++, sx_48+=stepx) {
                        temp = static_cast<unsigned int>(sx_48 >> 32);
                        temp = (temp > 0x8000)? temp - 0x8000 : 0;
                        uint32 sx1 = temp >> 16;                    // src x #1
                        uint32 sx2 = std::min(sx1+1,src.mWidth-1);// src x #2
                        float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2
                        
                        // process R,G,B,A simultaneously for cache coherence?
                        float accum[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    #define ACCUM4(x,y,z,factor) \
        { float f = factor; \
        float *psrc = (float*)(srcdata + x * srcelemsize + y * src.mRowSize + z * src.mSliceSize); \
        accum[0] += f*psrc[0]; accum[1] += f*psrc[1]; accum[2] += f*psrc[2]; accum[3] += f*psrc[3]; }

    #define ACCUM3(x,y,z,factor) \
        { float f = factor; \
        float *psrc = (float*)(srcdata + x * srcelemsize + y * src.mRowSize + z * src.mSliceSize); \
        accum[0] += f * psrc[0]; accum[1] += f * psrc[1]; accum[2] += f * psrc[2]; }

    #define ACCUM2(x,y,z,factor) \
        { float f = factor; \
        float *psrc = (float*)(srcdata + x * srcelemsize + y * src.mRowSize + z * src.mSliceSize); \
        accum[0] += f * psrc[0]; accum[1] += f * psrc[1]; }

    #define ACCUM1(x,y,z,factor) \
        { float f = factor; \
        float *psrc = (float*)(srcdata + x * srcelemsize + y * src.mRowSize + z * src.mSliceSize); \
        accum[0] += f * psrc[0]; }

                        switch( minchannels )
                        {
                        case 4: // RGBA
                            ACCUM4(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
                            ACCUM4(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
                            ACCUM4(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
                            ACCUM4(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
                            ACCUM4(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
                            ACCUM4(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
                            ACCUM4(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
                            ACCUM4(sx2,sy2,sz2,      sxf *      syf *      szf );
                            break;
                        case 3: // RGB
                            ACCUM3(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
                            ACCUM3(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
                            ACCUM3(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
                            ACCUM3(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
                            ACCUM3(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
                            ACCUM3(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
                            ACCUM3(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
                            ACCUM3(sx2,sy2,sz2,      sxf *      syf *      szf );
                            break;
                        case 2: // RG
                            ACCUM2(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
                            ACCUM2(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
                            ACCUM2(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
                            ACCUM2(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
                            ACCUM2(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
                            ACCUM2(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
                            ACCUM2(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
                            ACCUM2(sx2,sy2,sz2,      sxf *      syf *      szf );
                            break;
                        case 1: // R
                            ACCUM1(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
                            ACCUM1(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
                            ACCUM1(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
                            ACCUM1(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
                            ACCUM1(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
                            ACCUM1(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
                            ACCUM1(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
                            ACCUM1(sx2,sy2,sz2,      sxf *      syf *      szf );
                            break;
                        }

                        memcpy(pdst, accum, dstelemsize);

    #undef ACCUM4
    #undef ACCUM3
    #undef ACCUM2
    #undef ACCUM1

                        pdst += dstelemsize;
                    }
                }
            }
        }
    };
    //---------------------------------------------------------------------------------
    /** 字节的线性重采样,不做任何格式转换.只处理使用每字节/颜色 通道的像素格
        式.仅2D;削平3D像素盒到默认LinearResampler(慢).字节-每-像素的模板充
        许编译器优化,例如循环展开和替换位位移的倍增
    */
    template<Nui32 channels> struct LinearResampler_Byte
    {
        static void scale(const PixelBlock & src, const PixelBlock & dst)
        {
            // only optimized for 2D
            if (src.getVolume() > 1 || dst.getVolume() > 1) {
                LinearResampler::scale(src, dst);
                return;
            }

            // srcdata stays at beginning of slice, pdst is a moving pointer
            Nui8* srcdata = (Nui8*)src.getData( 0, 0, 0 );
            Nui8* dstdata = (Nui8*)dst.getData( 0, 0, 0 );

            // sx_48,sy_48 represent current position in source
            // using 16/48-bit fixed precision, incremented by steps
            uint64 stepx = ((uint64)src.mWidth << 48) / dst.mWidth;
            uint64 stepy = ((uint64)src.mHeight << 48) / dst.mHeight;
            
            uint64 sy_48 = (stepy >> 1) - 1;
            for (size_t y = 0; y < dst.mHeight; y++, sy_48+=stepy) 
            {
                // bottom 28 bits of temp are 16/12 bit fixed precision, used to
                // adjust a source coordinate backwards by half a pixel so that the
                // integer bits represent the first sample (eg, sx1) and the
                // fractional bits are the blend weight of the second sample
                unsigned int temp = static_cast<unsigned int>(sy_48 >> 36);
                temp = (temp > 0x800)? temp - 0x800: 0;
                unsigned int syf = temp & 0xFFF;
                uint32 sy1 = temp >> 12;
                uint32 sy2 = std::min(sy1+1, src.mHeight-1);
                size_t syoff1 = sy1 * src.mRowSize;
                size_t syoff2 = sy2 * src.mRowSize;

                Nui8 *pdst = (Nui8 *)( dstdata + y * dst.mRowSize );

                uint64 sx_48 = (stepx >> 1) - 1;
                for (size_t x = 0; x < dst.mWidth; x++, sx_48+=stepx) {
                    temp = static_cast<unsigned int>(sx_48 >> 36);
                    temp = (temp > 0x800)? temp - 0x800 : 0;
                    unsigned int sxf = temp & 0xFFF;
                    uint32 sx1 = temp >> 12;
                    uint32 sx2 = std::min(sx1+1, src.mWidth-1);

                    unsigned int sxfsyf = sxf*syf;
                    for (unsigned int k = 0; k < channels; k++) {
                        unsigned int accum =
                            srcdata[syoff1+sx1*channels+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
                            srcdata[syoff1+sx2*channels+k]*((sxf<<12)-sxfsyf) +
                            srcdata[syoff2+sx1*channels+k]*((syf<<12)-sxfsyf) +
                            srcdata[syoff2+sx2*channels+k]*sxfsyf;
                        // accum is computed using 8/24-bit fixed-point math
                        // (maximum is 0xFF000000; rounding will not cause overflow)
                        *pdst++ = static_cast<Nui8>((accum + 0x800000) >> 24);
                    }
                }
            }
        }
    };
}
#endif