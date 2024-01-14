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

namespace NII
{
    void DOWNSAMPLE_NAME( Nui8 * _dstPtr, Nui8 const * _srcPtr, Ni32 dstWidth, Ni32 dstHeight,
                          Ni32 dstBytesPerRow, Ni32 srcWidth, Ni32 srcBytesPerRow,
                          const Nui8 kernel[5][5], const Ni8 kernelStartX, const Ni8 kernelEndX,
                          const Ni8 kernelStartY, const Ni8 kernelEndY )
    {
        NII_UINT8 * dstPtr = reinterpret_cast<NII_UINT8 *>( _dstPtr );
        NII_UINT8 const * srcPtr = reinterpret_cast<NII_UINT8 const *>( _srcPtr );

        srcBytesPerRow /= sizeof( NII_UINT8 );
        dstBytesPerRow /= sizeof( NII_UINT8 );

        Ni32 srcBytesPerRowSkip = srcBytesPerRow - srcWidth * NII_TOTAL_SIZE;
        Ni32 dstBytesPerRowSkip = dstBytesPerRow - dstWidth * NII_TOTAL_SIZE;

        for( Ni32 y = 0; y < dstHeight; ++y )
        {
            for( Ni32 x = 0; x < dstWidth; ++x )
            {
                int kStartY = std::max<int>( -y, kernelStartY );
                int kEndY = std::min<int>( dstHeight - y - 1, kernelEndY );

#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                for( int k_y = kStartY; k_y <= kEndY; ++k_y )
                {
                    int kStartX = std::max<int>( -x, kernelStartX );
                    int kEndX = std::min<int>( dstWidth - 1 - x, kernelEndX );

                    for( int k_x = kStartX; k_x <= kEndX; ++k_x )
                    {
                        Nui32 kernelVal = kernel[k_y + 2][k_x + 2];

#ifdef NII_DOWNSAMPLE_R
                        NII_UINT32 r = srcPtr[k_y * srcBytesPerRow + k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_R];
                        accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                        NII_UINT32 g = srcPtr[k_y * srcBytesPerRow + k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_G];
                        accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                        NII_UINT32 b = srcPtr[k_y * srcBytesPerRow + k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_B];
                        accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                        NII_UINT32 a = srcPtr[k_y * srcBytesPerRow + k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_A];
                        accumA += a * kernelVal;
#endif

                        divisor += kernelVal;
                    }
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
                srcPtr += NII_TOTAL_SIZE * 2;
            }

            dstPtr += dstBytesPerRowSkip;
            srcPtr += ( srcWidth - dstWidth * 2 ) * NII_TOTAL_SIZE + srcBytesPerRowSkip;
            srcPtr += srcBytesPerRow;
        }
    }
    //-----------------------------------------------------------------------------------
    void DOWNSAMPLE_CUBE_NAME( Nui8 *_dstPtr, Nui8 const **_allPtr, Ni32 dstWidth, Ni32 dstHeight,
                               Ni32 dstBytesPerRow, Ni32 srcWidth, Ni32 srcHeight,
                               Ni32 srcBytesPerRow, const Nui8 kernel[5][5], const Ni8 kernelStartX,
                               const Ni8 kernelEndX, const Ni8 kernelStartY, const Ni8 kernelEndY,
                               Nui8 currentFace )
    {
        NII_UINT8 * dstPtr = reinterpret_cast<NII_UINT8 *>( _dstPtr );
        NII_UINT8 const ** allPtr = reinterpret_cast<NII_UINT8 const **>( _allPtr );

        srcBytesPerRow /= sizeof( NII_UINT8 );
        dstBytesPerRow /= sizeof( NII_UINT8 );

        Quaternion kRotations[5][5];

        {
            Radian xRadStep( NII::Math::PI / ( srcWidth * 2.0f ) );
            Radian yRadStep( NII::Math::PI / ( srcHeight * 2.0f ) );

            for( int y = kernelStartY; y <= kernelEndY; ++y )
            {
                for( int x = kernelStartX; x <= kernelEndX; ++x )
                {
                    Matrix3f m3;
                    eulerXYZ(&m3, (Real)-y * yRadStep, (Real)x * xRadStep, Radian( 0 ) );
                    kRotations[y + 2][x + 2] = Quaternion( m3 );
                }
            }
        }

        const FaceSwizzle &faceSwizzle = c_faceSwizzles[currentFace];

        Real invSrcWidth = 1.0f / srcWidth;
        Real invSrcHeight = 1.0f / srcHeight;

        Ni32 dstBytesPerRowSkip = dstBytesPerRow - dstWidth * NII_TOTAL_SIZE;

        NII_UINT8 const * srcPtr = 0;

        for( Ni32 y = 0; y < dstHeight; ++y )
        {
            for( Ni32 x = 0; x < dstWidth; ++x )
            {
#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                Vector3f vForwardSample( ( x * 2 + 0.5f ) * invSrcWidth * 2.0f - 1.0f,
                                        ( y * 2 + 0.5f ) * invSrcHeight * -2.0f + 1.0f, 1.0f );

                for( int k_y = kernelStartY; k_y <= kernelEndY; ++k_y )
                {
                    for( int k_x = kernelStartX; k_x <= kernelEndX; ++k_x )
                    {
                        Nui32 kernelVal = kernel[k_y + 2][k_x + 2];

                        Vector3f tmp = kRotations[k_y + 2][k_x + 2] * vForwardSample;

                        Vector3f vSample;
                        vSample.ptr()[0] = tmp.ptr()[faceSwizzle.iX] * faceSwizzle.signX;
                        vSample.ptr()[1] = tmp.ptr()[faceSwizzle.iY] * faceSwizzle.signY;
                        vSample.ptr()[2] = tmp.ptr()[faceSwizzle.iZ] * faceSwizzle.signZ;

                        CubemapUVI uvi = cubeMapProject( vSample );

                        int iu = std::min( static_cast<int>( floorf( uvi.u * srcWidth ) ), srcWidth - 1 );
                        int iv = std::min( static_cast<int>( floorf( uvi.v * srcHeight ) ), srcHeight - 1 );

                        srcPtr = allPtr[uvi.face] + iv * srcBytesPerRow + iu * NII_TOTAL_SIZE;

#ifdef NII_DOWNSAMPLE_R
                        NII_UINT32 r = srcPtr[NII_DOWNSAMPLE_R];
                        accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                        NII_UINT32 g = srcPtr[NII_DOWNSAMPLE_G];
                        accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                        NII_UINT32 b = srcPtr[NII_DOWNSAMPLE_B];
                        accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                        NII_UINT32 a = srcPtr[NII_DOWNSAMPLE_A];
                        accumA += a * kernelVal;
#endif

                        divisor += kernelVal;
                    }
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
            }

            dstPtr += dstBytesPerRowSkip;
        }
    }
    //-----------------------------------------------------------------------------------
    void BLUR_NAME( Nui8 *_tmpPtr, Nui8 *_srcDstPtr, Ni32 width, Ni32 height, Ni32 mRowSize,
        const Nui8 kernel[5], const Ni8 kernelStart, const Ni8 kernelEnd )
    {
        NII_UINT8 * dstPtr = reinterpret_cast<NII_UINT8 *>( _tmpPtr );
        NII_UINT8 const * srcPtr = reinterpret_cast<NII_UINT8 const *>( _srcDstPtr );

        mRowSize /= sizeof( NII_UINT8 );

        const Ni32 bytesPerRowSkip = mRowSize - width * NII_TOTAL_SIZE;

        for( Ni32 y = 0; y < height; ++y )
        {
            for( Ni32 x = 0; x < width; ++x )
            {
#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                int kStartX = std::max<int>( -x, kernelStart );
                int kEndX = std::min<int>( width - 1 - x, kernelEnd );

                for( int k_x = kStartX; k_x <= kEndX; ++k_x )
                {
                    Nui32 kernelVal = kernel[k_x + 2];

#ifdef NII_DOWNSAMPLE_R
                    NII_UINT32 r = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_R];
                    accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                    NII_UINT32 g = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_G];
                    accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                    NII_UINT32 b = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_B];
                    accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                    NII_UINT32 a = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_A];
                    accumA += a * kernelVal;
#endif

                    divisor += kernelVal;
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
                srcPtr += NII_TOTAL_SIZE;
            }

            dstPtr += bytesPerRowSkip;
            srcPtr += bytesPerRowSkip;
        }

        dstPtr = reinterpret_cast<NII_UINT8 *>( _srcDstPtr );
        srcPtr = reinterpret_cast<NII_UINT8 const *>( _tmpPtr );

        for( Ni32 y = 0; y < height; ++y )
        {
            for( Ni32 x = 0; x < width; ++x )
            {
#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                int kStartY = std::max<int>( -y, kernelStart );
                int kEndY = std::min<int>( height - y - 1, kernelEnd );

                for( int k_y = kStartY; k_y <= kEndY; ++k_y )
                {
                    Nui32 kernelVal = kernel[k_y + 2];

#ifdef NII_DOWNSAMPLE_R
                    NII_UINT32 r = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_R];
                    accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                    NII_UINT32 g = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_G];
                    accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                    NII_UINT32 b = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_B];
                    accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                    NII_UINT32 a = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_A];
                    accumA += a * kernelVal;
#endif

                    divisor += kernelVal;
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
                srcPtr += NII_TOTAL_SIZE;
            }

            dstPtr += bytesPerRowSkip;
            srcPtr += bytesPerRowSkip;
        }
    }
    //-----------------------------------------------------------------------------------
    void BLUR_NAME( Nui8 * _tmpPtr, Nui8 * _srcDstPtr, Ni32 width, Ni32 height, const Nui8 kernel[5], const Ni8 kernelStart, const Ni8 kernelEnd )
    {
        NII_UINT8 *dstPtr = reinterpret_cast<NII_UINT8 *>( _tmpPtr );
        NII_UINT8 const *srcPtr = reinterpret_cast<NII_UINT8 const *>( _srcDstPtr );

        const size_t mRowSize = width * NII_TOTAL_SIZE;

        for( Ni32 y = 0; y < height; ++y )
        {
            for( Ni32 x = 0; x < width; ++x )
            {
#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                int kStartX = std::max<int>( -x, kernelStart );
                int kEndX = std::min<int>( width - 1 - x, kernelEnd );

                for( int k_x = kStartX; k_x <= kEndX; ++k_x )
                {
                    Nui32 kernelVal = kernel[k_x + 2];

#ifdef NII_DOWNSAMPLE_R
                    NII_UINT32 r = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_R];
                    accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                    NII_UINT32 g = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_G];
                    accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                    NII_UINT32 b = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_B];
                    accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                    NII_UINT32 a = srcPtr[k_x * NII_TOTAL_SIZE + NII_DOWNSAMPLE_A];
                    accumA += a * kernelVal;
#endif

                    divisor += kernelVal;
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
                srcPtr += NII_TOTAL_SIZE;
            }
        }

        dstPtr = reinterpret_cast<NII_UINT8 *>( _srcDstPtr );
        srcPtr = reinterpret_cast<NII_UINT8 const *>( _tmpPtr );

        for( Ni32 y = 0; y < height; ++y )
        {
            for( Ni32 x = 0; x < width; ++x )
            {
#ifdef NII_DOWNSAMPLE_R
                NII_UINT32 accumR = 0;
#endif
#ifdef NII_DOWNSAMPLE_G
                NII_UINT32 accumG = 0;
#endif
#ifdef NII_DOWNSAMPLE_B
                NII_UINT32 accumB = 0;
#endif
#ifdef NII_DOWNSAMPLE_A
                NII_UINT32 accumA = 0;
#endif

                Nui32 divisor = 0;

                int kStartY = std::max<int>( -y, kernelStart );
                int kEndY = std::min<int>( height - y - 1, kernelEnd );

                for( int k_y = kStartY; k_y <= kEndY; ++k_y )
                {
                    Nui32 kernelVal = kernel[k_y + 2];

#ifdef NII_DOWNSAMPLE_R
                    NII_UINT32 r = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_R];
                    accumR += NII_GAM_TO_LIN( r ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_G
                    NII_UINT32 g = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_G];
                    accumG += NII_GAM_TO_LIN( g ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_B
                    NII_UINT32 b = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_B];
                    accumB += NII_GAM_TO_LIN( b ) * kernelVal;
#endif
#ifdef NII_DOWNSAMPLE_A
                    NII_UINT32 a = srcPtr[k_y * mRowSize + NII_DOWNSAMPLE_A];
                    accumA += a * kernelVal;
#endif

                    divisor += kernelVal;
                }

#if defined( NII_DOWNSAMPLE_R ) || defined( NII_DOWNSAMPLE_G ) || defined( NII_DOWNSAMPLE_B )
                float invDivisor = 1.0f / divisor;
#endif

#ifdef NII_DOWNSAMPLE_R
                dstPtr[NII_DOWNSAMPLE_R] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumR * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_G
                dstPtr[NII_DOWNSAMPLE_G] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumG * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_B
                dstPtr[NII_DOWNSAMPLE_B] = static_cast<NII_UINT8>( NII_LIN_TO_GAM( accumB * invDivisor ) + NII_ROUND_HALF );
#endif
#ifdef NII_DOWNSAMPLE_A
                dstPtr[NII_DOWNSAMPLE_A] = static_cast<NII_UINT8>( ( accumA + divisor - 1u ) / divisor );
#endif

                dstPtr += NII_TOTAL_SIZE;
                srcPtr += NII_TOTAL_SIZE;
            }
        }
    }
}  // namespace NII

#undef NII_DOWNSAMPLE_A
#undef NII_DOWNSAMPLE_R
#undef NII_DOWNSAMPLE_G
#undef NII_DOWNSAMPLE_B
#undef DOWNSAMPLE_NAME
#undef DOWNSAMPLE_CUBE_NAME
#undef BLUR_NAME
#undef NII_TOTAL_SIZE
