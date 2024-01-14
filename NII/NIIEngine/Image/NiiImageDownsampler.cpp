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

#include "NiiMatrix3.h"
#include "NiiVector3.h"

#include "NiiImageDownsampler.h"
#include "NiiPixelFormat.h"

namespace NII
{
    struct CubemapUVI
    {
        NIIf u;
        NIIf v;
        int face;
    };

    enum MajorAxis
    {
        MajorAxisX,
        MajorAxisY,
        MajorAxisZ,
    };

    struct FaceComponents
    {
        FaceComponents(Nui8 _uIdx, Nui8 _vIdx, NIIf _uSign, NIIf _vSign) :
            uIdx(_uIdx), vIdx(_vIdx), uSign(_uSign), vSign(_vSign){}

        Nui8 uIdx;
        Nui8 vIdx;
        NIIf uSign;
        NIIf vSign;
    };

    const FaceComponents c_faceComponents[6] =
    {
        FaceComponents( 2, 1, -0.5f, -0.5f ),
        FaceComponents( 2, 1,  0.5f, -0.5f ),
        FaceComponents( 0, 2,  0.5f,  0.5f ),
        FaceComponents( 0, 2,  0.5f, -0.5f ),
        FaceComponents( 0, 1,  0.5f, -0.5f ),
        FaceComponents( 0, 1, -0.5f, -0.5f )
    };

    struct FaceSwizzle
    {
        FaceSwizzle(int8 _iX, int8 _iY, int8 _iZ, NIIf _signX, NIIf _signY, NIIf _signZ) :
            iX(_iX), iY(_iY), iZ(_iZ), signX(_signX), signY(_signY), signZ(_signZ) {}

        int8 iX, iY, iZ;
        NIIf signX, signY, signZ;
    };

    static const FaceSwizzle c_faceSwizzles[6] =
    {
        FaceSwizzle( 2, 1, 0,  1,  1, -1 ),
        FaceSwizzle( 2, 1, 0, -1,  1,  1 ),
        FaceSwizzle( 0, 2, 1,  1,  1, -1 ),
        FaceSwizzle( 0, 2, 1,  1, -1,  1 ),
        FaceSwizzle( 0, 1, 2,  1,  1,  1 ),
        FaceSwizzle( 0, 1, 2, -1,  1, -1 ),
    };

    inline CubemapUVI cubeMapProject( Vector3f vDir )
    {
        CubemapUVI uvi;

        Vector3f absDir = Vector3f( fabsf( vDir.x ), fabsf( vDir.y ), fabsf( vDir.z ) );

        bool majorX = (absDir.x >= absDir.y) & (absDir.x >= absDir.z);
        bool majorY = (absDir.y >= absDir.x) & (absDir.y >= absDir.z);
        bool majorZ = (absDir.z >= absDir.x) & (absDir.z >= absDir.y);

        NIIf fNorm;
        MajorAxis majorAxis;
        majorAxis   = majorX ? MajorAxisX : MajorAxisY;
        fNorm       = majorX ? absDir.x : absDir.y;

        majorAxis   = majorY ? MajorAxisY : majorAxis;
        fNorm       = majorY ? absDir.y : fNorm;

        majorAxis   = majorZ ? MajorAxisZ : majorAxis;
        fNorm       = majorZ ? absDir.z : fNorm;

        const NIIf *fDirs = vDir.ptr();

        uvi.face = fDirs[majorAxis] >= 0 ? majorAxis * 2 : majorAxis * 2 + 1;

        fNorm = 1.0f / fNorm;

        uvi.u =  c_faceComponents[uvi.face].uSign * fDirs[c_faceComponents[uvi.face].uIdx] * fNorm + 0.5f;
        uvi.v =  c_faceComponents[uvi.face].vSign * fDirs[c_faceComponents[uvi.face].vIdx] * fNorm + 0.5f;

        return uvi;
    }

    const FilterKernel c_filterKernels[3] =
    {
        {
            //Point
            {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            },
            0, 0,
            0, 0
        },
        {
            //Linear
            {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 1, 0,
                0, 0, 1, 1, 0,
                0, 0, 0, 0, 0
            },
            0, 1,
            0, 1
        },
        {
            //Gaussian
            {
                1,  4,  7,  4, 1,
                4, 16, 26, 16, 4,
                7, 26, 41, 26, 7,
                4, 16, 26, 16, 4,
                1,  4,  7,  4, 1
            },
            -2, 2,
            -2, 2
        }
    };

    const FilterSeparableKernel c_filterSeparableKernels[1] =
    {
        {
            //Gaussian High
            {
                40, 161, 255, 161, 40
            },
            -2, 2
        }
    };
}

#define NII_GAM_TO_LIN( x ) x
#define NII_LIN_TO_GAM( x ) x
#define NII_UINT8 Nui8
#define NII_UINT32 uint32
#define NII_ROUND_HALF 0.5f

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_DOWNSAMPLE_A 3
#define NII_TOTAL_SIZE 4
#define DOWNSAMPLE_NAME downscale2x_XXXA8888
#define DOWNSAMPLE_CUBE_NAME downscale2x_XXXA8888_cube
#define BLUR_NAME separableBlur_XXXA8888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_TOTAL_SIZE 3
#define DOWNSAMPLE_NAME downscale2x_XXX888
	#define DOWNSAMPLE_CUBE_NAME downscale2x_XXX888_cube
#define BLUR_NAME separableBlur_XXX888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_XX88
	#define DOWNSAMPLE_CUBE_NAME downscale2x_XX88_cube
#define BLUR_NAME separableBlur_XX88
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_X8
	#define DOWNSAMPLE_CUBE_NAME downscale2x_X8_cube
#define BLUR_NAME separableBlur_X8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_A8
	#define DOWNSAMPLE_CUBE_NAME downscale2x_A8_cube
#define BLUR_NAME separableBlur_A8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_A 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_XA88
	#define DOWNSAMPLE_CUBE_NAME downscale2x_XA88_cube
#define BLUR_NAME separableBlur_XA88
#include "NiiImageDownsamplerImpl.inl"

//-----------------------------------------------------------------------------------
//Signed versions
//-----------------------------------------------------------------------------------

#undef NII_UINT8
#undef NII_UINT32
#define NII_UINT8 int8
#define NII_UINT32 int32

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_DOWNSAMPLE_A 3
#define NII_TOTAL_SIZE 4
#define DOWNSAMPLE_NAME downscale2x_Signed_XXXA8888
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_XXXA8888_cube
#define BLUR_NAME separableBlur_Signed_XXXA8888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_TOTAL_SIZE 3
#define DOWNSAMPLE_NAME downscale2x_Signed_XXX888
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_XXX888_cube
#define BLUR_NAME separableBlur_Signed_XXX888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_Signed_XX88
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_XX88_cube
#define BLUR_NAME separableBlur_Signed_XX88
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_Signed_X8
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_X8_cube
#define BLUR_NAME separableBlur_Signed_X8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_Signed_A8
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_A8_cube
#define BLUR_NAME separableBlur_Signed_A8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_A 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_Signed_XA88
#define DOWNSAMPLE_CUBE_NAME downscale2x_Signed_XA88_cube
#define BLUR_NAME separableBlur_Signed_XA88
#include "NiiImageDownsamplerImpl.inl"

//-----------------------------------------------------------------------------------
//Float32 versions
//-----------------------------------------------------------------------------------

#undef NII_UINT8
#undef NII_UINT32
#undef NII_ROUND_HALF
#define NII_UINT8 float
#define NII_UINT32 float
#define NII_ROUND_HALF 0.0f

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_DOWNSAMPLE_A 3
#define NII_TOTAL_SIZE 4
#define DOWNSAMPLE_NAME downscale2x_Float32_XXXA
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_XXXA_cube
#define BLUR_NAME separableBlur_Float32_XXXA
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_TOTAL_SIZE 3
#define DOWNSAMPLE_NAME downscale2x_Float32_XXX
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_XXX_cube
#define BLUR_NAME separableBlur_Float32_XXX
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_Float32_XX
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_XX_cube
#define BLUR_NAME separableBlur_Float32_XX
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_Float32_X
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_X_cube
#define BLUR_NAME separableBlur_Float32_X
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_Float32_A
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_A_cube
#define BLUR_NAME separableBlur_Float32_A
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_A 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_Float32_XA
#define DOWNSAMPLE_CUBE_NAME downscale2x_Float32_XA_cube
#define BLUR_NAME separableBlur_Float32_XA
#include "NiiImageDownsamplerImpl.inl"

//-----------------------------------------------------------------------------------
//sRGB versions
//-----------------------------------------------------------------------------------

#undef NII_GAM_TO_LIN
#undef NII_LIN_TO_GAM
#define NII_GAM_TO_LIN( x ) x * x
#define NII_LIN_TO_GAM( x ) sqrtf( x )

#undef NII_UINT8
#undef NII_UINT32
#undef NII_ROUND_HALF
#define NII_UINT8 Nui8
#define NII_UINT32 uint32
#define NII_ROUND_HALF 0.5f

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_DOWNSAMPLE_A 3
#define NII_TOTAL_SIZE 4
#define DOWNSAMPLE_NAME downscale2x_sRGB_XXXA8888
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_XXXA8888_cube
#define BLUR_NAME separableBlur_sRGB_XXXA8888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_DOWNSAMPLE_R 1
#define NII_DOWNSAMPLE_G 2
#define NII_DOWNSAMPLE_B 3
#define NII_TOTAL_SIZE 4
#define DOWNSAMPLE_NAME downscale2x_sRGB_AXXX8888
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_AXXX8888_cube
#define BLUR_NAME separableBlur_sRGB_AXXX8888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_DOWNSAMPLE_B 2
#define NII_TOTAL_SIZE 3
#define DOWNSAMPLE_NAME downscale2x_sRGB_XXX888
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_XXX888_cube
#define BLUR_NAME separableBlur_sRGB_XXX888
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_G 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_sRGB_XX88
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_XX88_cube
#define BLUR_NAME separableBlur_sRGB_XX88
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_sRGB_X8
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_X8_cube
#define BLUR_NAME separableBlur_sRGB_X8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_TOTAL_SIZE 1
#define DOWNSAMPLE_NAME downscale2x_sRGB_A8
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_A8_cube
#define BLUR_NAME separableBlur_sRGB_A8
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_R 0
#define NII_DOWNSAMPLE_A 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_sRGB_XA88
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_XA88_cube
#define BLUR_NAME separableBlur_sRGB_XA88
#include "NiiImageDownsamplerImpl.inl"

#define NII_DOWNSAMPLE_A 0
#define NII_DOWNSAMPLE_R 1
#define NII_TOTAL_SIZE 2
#define DOWNSAMPLE_NAME downscale2x_sRGB_AX88
#define DOWNSAMPLE_CUBE_NAME downscale2x_sRGB_AX88_cube
#define BLUR_NAME separableBlur_sRGB_AX88
#include "NiiImageDownsamplerImpl.inl"

#undef NII_GAM_TO_LIN
#undef NII_LIN_TO_GAM

    bool getDownsampler(PixelFormat format, ImageDownsampler2D *& sampler2D, ImageDownsamplerCube *& samplerCube, ImageBlur2D *& imageBlur2D,
        bool srgb, NCount depth, NCount array, Filter filter )
    {
        bool re = true;

        //ImageDownsampler2D * sampler2D = 0;
        //ImageDownsamplerCube * samplerCube = 0;
        //ImageBlur2D * imageBlur2D = 0;

        srgb |= PixelUtil::isSRGB( format );

        switch( format )
        {
        case PF_R8UN:
        case PF_R8ui:
            if(srgb)
            {
                sampler2D   = downscale2x_sRGB_X8;
                samplerCube = downscale2x_sRGB_X8_cube;
                imageBlur2D = separableBlur_sRGB_X8;
            }
            else
            {
                sampler2D   = downscale2x_X8;
                samplerCube = downscale2x_X8_cube;
                imageBlur2D = separableBlur_X8;
            }
            break;
        case PF_A8UN:
            if(srgb)
            {
                sampler2D   = downscale2x_sRGB_A8;
                samplerCube = downscale2x_sRGB_A8_cube;
                imageBlur2D = separableBlur_sRGB_A8;
            }
            else
            {
                sampler2D   = downscale2x_A8;
                samplerCube = downscale2x_A8_cube;
                imageBlur2D = separableBlur_A8;
            }
            break;
        case PF_R8G8UN:
        case PF_R8G8ui:
            if(srgb)
            {
                sampler2D   = downscale2x_sRGB_XX88;
                samplerCube = downscale2x_sRGB_XX88_cube;
                imageBlur2D = separableBlur_sRGB_X8;
            }
            else
            {
                sampler2D   = downscale2x_XX88;
                samplerCube = downscale2x_XX88_cube;
                imageBlur2D = separableBlur_X8;
            }
            break;
        case PF_R8G8B8A8UN:
        case PF_R8G8B8A8UN_SRGB:
        case PF_R8G8B8A8ui:
        case PF_B8G8R8A8UN:
        case PF_B8G8R8A8UN_SRGB:
            if(srgb)
            {
                sampler2D   = downscale2x_sRGB_XXXA8888;
                samplerCube = downscale2x_sRGB_XXXA8888_cube;
                imageBlur2D = separableBlur_sRGB_XXXA8888;
            }
            else
            {
                sampler2D   = downscale2x_XXXA8888;
                samplerCube = downscale2x_XXXA8888_cube;
                imageBlur2D = separableBlur_XXXA8888;
            }
            break;
        case PF_R8N:
        case PF_R8i:
            sampler2D   = downscale2x_Signed_X8;
            samplerCube = downscale2x_Signed_X8_cube;
            imageBlur2D = separableBlur_X8;
            break;
        case PF_R8G8N: case PF_R8G8i:
            sampler2D   = downscale2x_Signed_XX88;
            samplerCube = downscale2x_Signed_XX88_cube;
            imageBlur2D = separableBlur_Signed_X8;
            break;
        case PF_R8G8B8A8N: case PF_R8G8B8A8i:
            sampler2D   = downscale2x_Signed_XXXA8888;
            samplerCube = downscale2x_Signed_XXXA8888_cube;
            imageBlur2D = separableBlur_Signed_XXXA8888;
            break;
        case PF_R32G32B32A32:
            sampler2D   = downscale2x_Float32_XXXA;
            samplerCube = downscale2x_Float32_XXXA_cube;
            imageBlur2D = separableBlur_Float32_XXXA;
            break;
        case PF_R32G32B32:
            sampler2D   = downscale2x_Float32_XXX;
            samplerCube = downscale2x_Float32_XXX_cube;
            imageBlur2D = separableBlur_Float32_XXX;
            break;
        case PF_R32G32:
            sampler2D   = downscale2x_Float32_XX;
            samplerCube = downscale2x_Float32_XX_cube;
            imageBlur2D = separableBlur_Float32_XX;
            break;
        case PF_R32:
            sampler2D   = downscale2x_Float32_X;
            samplerCube = downscale2x_Float32_X_cube;
            imageBlur2D = separableBlur_Float32_X;
            break;
        default: //Keep compiler happy
            break;
        }

        //sampler2D = sampler2D;
        //samplerCube = samplerCube;
        //imageBlur2D = imageBlur2D;

        if( (depth == 1u && array == 1 && !sampler2D) ||
            (array == 6 && array == 1 && (!samplerCube || filter == F_Gaussian_Bilinear)) ||
            (array % 6 == 0 && depth == 1u) || (depth > 1 && array > 1))
        {
            re = false;
        }

        return re;
    }
