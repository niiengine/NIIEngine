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
#ifndef _NII_ImageDownsampler_H_
#define _NII_ImageDownsampler_H_

namespace NII
{
    /** Range is [kernelStart; kernelEnd]
    @param dstPtr
    @param srcPtr
    @param width
    @param height
    @param kernel
    @param kernelStartX
    @param kernelEndX
    @param kernelStartY
    @param kernelEndY
     */
    typedef void (ImageDownsampler2D)( Nui8 * dstPtr, Nui8 const * srcPtr,
                                       int32 dstWidth, int32 dstHeight, int32 dstBytesPerRow,
                                       int32 srcWidth, int32 srcBytesPerRow,
                                       const Nui8 kernel[5][5],
                                       const int8 kernelStartX, const int8 kernelEndX,
                                       const int8 kernelStartY, const int8 kernelEndY );

    ImageDownsampler2D downscale2x_XXXA8888;
    ImageDownsampler2D downscale2x_XXX888;
    ImageDownsampler2D downscale2x_XX88;
    ImageDownsampler2D downscale2x_X8;
    ImageDownsampler2D downscale2x_A8;
    ImageDownsampler2D downscale2x_XA88;

    //
    //  CUBEMAP versions
    //

    typedef void (ImageDownsamplerCube)(Nui8 * dstPtr, Nui8 const ** srcPtr,
                                       int32 dstWidth, int32 dstHeight, int32 dstBytesPerRow,
                                       int32 srcWidth, int32 srcHeight, int32 srcBytesPerRow,
                                       const Nui8 kernel[5][5],
                                       const int8 kernelStartX, const int8 kernelEndX,
                                       const int8 kernelStartY, const int8 kernelEndY,
                                       Nui8 currentFace );

    ImageDownsamplerCube downscale2x_XXXA8888_cube;
    ImageDownsamplerCube downscale2x_XXX888_cube;
    ImageDownsamplerCube downscale2x_XX88_cube;
    ImageDownsamplerCube downscale2x_X8_cube;
    ImageDownsamplerCube downscale2x_A8_cube;
    ImageDownsamplerCube downscale2x_XA88_cube;

    /** Range is [kernelStart; kernelEnd]
    @param _tmpPtr
        Temporary buffer. Must be able to hold a copy of _srcDstPtr
    @param _srcDstPtr
        Input image to blur, output image blurred
    @param width
    @param height
    @param kernel
    @param kernelStart
    @param kernelEnd
     */
    typedef void (ImageBlur2D)( Nui8 *_tmpPtr, Nui8 *_srcDstPtr,
                                int32 width, int32 height, int32 mRowSize,
                                const Nui8 kernel[5],
                                const int8 kernelStart, const int8 kernelEnd );

    ImageBlur2D separableBlur_XXXA8888;
    ImageBlur2D separableBlur_XXX888;
    ImageBlur2D separableBlur_XX88;
    ImageBlur2D separableBlur_X8;
    ImageBlur2D separableBlur_A8;
    ImageBlur2D separableBlur_XA88;

    //-----------------------------------------------------------------------------------
    //Signed versions
    //-----------------------------------------------------------------------------------


    ImageDownsampler2D downscale2x_Signed_XXXA8888;
    ImageDownsampler2D downscale2x_Signed_XXX888;
    ImageDownsampler2D downscale2x_Signed_XX88;
    ImageDownsampler2D downscale2x_Signed_X8;
    ImageDownsampler2D downscale2x_Signed_A8;
    ImageDownsampler2D downscale2x_Signed_XA88;

    //
    //  CUBEMAP Signed versions
    //

    ImageDownsamplerCube downscale2x_Signed_XXXA8888_cube;
    ImageDownsamplerCube downscale2x_Signed_XXX888_cube;
    ImageDownsamplerCube downscale2x_Signed_XX88_cube;
    ImageDownsamplerCube downscale2x_Signed_X8_cube;
    ImageDownsamplerCube downscale2x_Signed_A8_cube;
    ImageDownsamplerCube downscale2x_Signed_XA88_cube;

    //
    //  Blur Signed versions
    //

    ImageBlur2D separableBlur_Signed_XXXA8888;
    ImageBlur2D separableBlur_Signed_XXX888;
    ImageBlur2D separableBlur_Signed_XX88;
    ImageBlur2D separableBlur_Signed_X8;
    ImageBlur2D separableBlur_Signed_A8;
    ImageBlur2D separableBlur_Signed_XA88;

    //-----------------------------------------------------------------------------------
    //Float32 versions
    //-----------------------------------------------------------------------------------


    ImageDownsampler2D downscale2x_Float32_XXXA;
    ImageDownsampler2D downscale2x_Float32_XXX;
    ImageDownsampler2D downscale2x_Float32_XX;
    ImageDownsampler2D downscale2x_Float32_X;
    ImageDownsampler2D downscale2x_Float32_A;
    ImageDownsampler2D downscale2x_Float32_XA;

    //
    //  CUBEMAP Float32 versions
    //

    ImageDownsamplerCube downscale2x_Float32_XXXA_cube;
    ImageDownsamplerCube downscale2x_Float32_XXX_cube;
    ImageDownsamplerCube downscale2x_Float32_XX_cube;
    ImageDownsamplerCube downscale2x_Float32_X_cube;
    ImageDownsamplerCube downscale2x_Float32_A_cube;
    ImageDownsamplerCube downscale2x_Float32_XA_cube;

    //
    //  Blur Float32 versions
    //

    ImageBlur2D separableBlur_Float32_XXXA;
    ImageBlur2D separableBlur_Float32_XXX;
    ImageBlur2D separableBlur_Float32_XX;
    ImageBlur2D separableBlur_Float32_X;
    ImageBlur2D separableBlur_Float32_A;
    ImageBlur2D separableBlur_Float32_XA;

    //-----------------------------------------------------------------------------------
    //sRGB versions
    //-----------------------------------------------------------------------------------


    ImageDownsampler2D downscale2x_sRGB_XXXA8888;
    ImageDownsampler2D downscale2x_sRGB_AXXX8888;
    ImageDownsampler2D downscale2x_sRGB_XXX888;
    ImageDownsampler2D downscale2x_sRGB_XX88;
    ImageDownsampler2D downscale2x_sRGB_X8;
    ImageDownsampler2D downscale2x_sRGB_A8;
    ImageDownsampler2D downscale2x_sRGB_XA88;
    ImageDownsampler2D downscale2x_sRGB_AX88;

    //
    //  CUBEMAP sRGB versions
    //

    ImageDownsamplerCube downscale2x_sRGB_XXXA8888_cube;
    ImageDownsamplerCube downscale2x_sRGB_AXXX8888_cube;
    ImageDownsamplerCube downscale2x_sRGB_XXX888_cube;
    ImageDownsamplerCube downscale2x_sRGB_XX88_cube;
    ImageDownsamplerCube downscale2x_sRGB_X8_cube;
    ImageDownsamplerCube downscale2x_sRGB_A8_cube;
    ImageDownsamplerCube downscale2x_sRGB_XA88_cube;
    ImageDownsamplerCube downscale2x_sRGB_AX88_cube;

    //
    //  Blur sRGB versions
    //

    ImageBlur2D separableBlur_sRGB_XXXA8888;
    ImageBlur2D separableBlur_sRGB_AXXX8888;
    ImageBlur2D separableBlur_sRGB_XXX888;
    ImageBlur2D separableBlur_sRGB_XX88;
    ImageBlur2D separableBlur_sRGB_X8;
    ImageBlur2D separableBlur_sRGB_A8;
    ImageBlur2D separableBlur_sRGB_XA88;
    ImageBlur2D separableBlur_sRGB_AX88;

    struct FilterKernel
    {
        Nui8 kernel[5][5];
        int8 kernelStartX;
        int8 kernelEndX;
        int8 kernelStartY;
        int8 kernelEndY;
    };
    struct FilterSeparableKernel
    {
        Nui8 kernel[5];
        int8 kernelStart;
        int8 kernelEnd;
    };

    extern const FilterKernel c_filterKernels[3];
    extern const FilterSeparableKernel c_filterSeparableKernels[1];
    
    bool getDownsampler(PixelFormat format, ImageDownsampler2D *& sampler2D,
        ImageDownsamplerCube *& samplerCube, ImageBlur2D *& imageBlur2D, bool gammaCorrected, 
            NCount depth, NCount face, Filter filter);
}

#endif
