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
#include "NiiSIMDUtil.h"
#include "NiiPlatInfo.h"
#include "NiiVector3.h"
#include "NiiMatrix4.h"

namespace NII
{
    class _EngineInner OptimisedUtilGeneral : public SIMDUtil
    {
    public:
        /// @copydetails SIMDUtil::softwareVertexSkinning
        void softwareVertexSkinning(
            const NIIf * pSrcPos, NIIf * pDestPos,
            const NIIf * pSrcNorm, NIIf * pDestNorm,
            const NIIf * pBlendWeight, const Nui8 * pBlendIndex,
            const Matrix4f * const * blendMatrices,
            NCount srcPosStride, NCount destPosStride,
            NCount srcNormStride, NCount destNormStride,
            NCount blendWeightStride, NCount blendIndexStride,
            NCount numWeightsPerVertex,
            NCount numVertices)
        {
            // Source vectors
            Vector3f sourceVec = Vector3f::ZERO, sourceNorm = Vector3f::ZERO;
            // Accumulation vectors
            Vector3f accumVecPos, accumVecNorm;

            // Loop per vertex
            for (NCount vertIdx = 0; vertIdx < numVertices; ++vertIdx)
            {
                // Load source vertex elements
                sourceVec.x = pSrcPos[0];
                sourceVec.y = pSrcPos[1];
                sourceVec.z = pSrcPos[2];

                if (pSrcNorm)
                {
                    sourceNorm.x = pSrcNorm[0];
                    sourceNorm.y = pSrcNorm[1];
                    sourceNorm.z = pSrcNorm[2];
                }

                // Load accumulators
                accumVecPos = Vector3f::ZERO;
                accumVecNorm = Vector3f::ZERO;

                // Loop per blend weight
                //
                // Note: Don't change "Nui16" here!!! If use "NCount" instead,
                // VC7.1 unroll this loop to four blend weights pre-iteration, and then
                // loss performance 10% in this function. Ok, this give a hint that we
                // should unroll this loop manually for better performance, will do that
                // later.
                //
                for (NCount blendIdx = 0; blendIdx < numWeightsPerVertex; ++blendIdx)
                {
                    // Blend by multiplying source by blend matrix and scaling by weight
                    // Add to accumulator
                    // NB weights must be normalised!!
                    NIIf weight = pBlendWeight[blendIdx];
                    if (weight)
                    {
                        // Blend position, use 3x4 matrix
                        const Matrix4f& mat = *blendMatrices[pBlendIndex[blendIdx]];
                        accumVecPos.x +=
                            (mat[0][0] * sourceVec.x +
                                mat[0][1] * sourceVec.y +
                                mat[0][2] * sourceVec.z +
                                mat[0][3])
                            * weight;
                        accumVecPos.y +=
                            (mat[1][0] * sourceVec.x +
                                mat[1][1] * sourceVec.y +
                                mat[1][2] * sourceVec.z +
                                mat[1][3])
                            * weight;
                        accumVecPos.z +=
                            (mat[2][0] * sourceVec.x +
                                mat[2][1] * sourceVec.y +
                                mat[2][2] * sourceVec.z +
                                mat[2][3])
                            * weight;
                        if (pSrcNorm)
                        {
                            // Blend normal
                            // We should blend by inverse transpose here, but because we're assuming the 3x3
                            // aspect of the matrix is orthogonal (no non-uniform scaling), the inverse transpose
                            // is equal to the main 3x3 matrix
                            // Note because it's a normal we just extract the rotational part, saves us renormalising here
                            accumVecNorm.x +=
                                (mat[0][0] * sourceNorm.x +
                                    mat[0][1] * sourceNorm.y +
                                    mat[0][2] * sourceNorm.z)
                                * weight;
                            accumVecNorm.y +=
                                (mat[1][0] * sourceNorm.x +
                                    mat[1][1] * sourceNorm.y +
                                    mat[1][2] * sourceNorm.z)
                                * weight;
                            accumVecNorm.z +=
                                (mat[2][0] * sourceNorm.x +
                                    mat[2][1] * sourceNorm.y +
                                    mat[2][2] * sourceNorm.z)
                                * weight;
                        }
                    }
                }

                // Stored blended vertex in hardware buffer
                pDestPos[0] = accumVecPos.x;
                pDestPos[1] = accumVecPos.y;
                pDestPos[2] = accumVecPos.z;

                // Stored blended vertex in temp buffer
                if (pSrcNorm)
                {
                    // Normalise
                    accumVecNorm.normalise();
                    pDestNorm[0] = accumVecNorm.x;
                    pDestNorm[1] = accumVecNorm.y;
                    pDestNorm[2] = accumVecNorm.z;
                    // Advance pointers
                    advanceRawPointer(pSrcNorm, srcNormStride);
                    advanceRawPointer(pDestNorm, destNormStride);
                }

                // Advance pointers
                advanceRawPointer(pSrcPos, srcPosStride);
                advanceRawPointer(pDestPos, destPosStride);
                advanceRawPointer(pBlendWeight, blendWeightStride);
                advanceRawPointer(pBlendIndex, blendIndexStride);
            }
        }

        /// @copydetails SIMDUtil::softwareVertexMorph
        void softwareVertexMorph(NIIf t, const NIIf * pSrc1,
            const NIIf * pSrc2, NIIf * pDst, NCount pos1VSize, NCount pos2VSize,
            NCount dstVSize, NCount numVertices, bool morphNormals)
        {
            NCount src1Skip = pos1VSize / sizeof(NIIf) - 3 - (morphNormals ? 3 : 0);
            NCount src2Skip = pos2VSize / sizeof(NIIf) - 3 - (morphNormals ? 3 : 0);
            NCount dstSkip = dstVSize / sizeof(NIIf) - 3 - (morphNormals ? 3 : 0);

            Vector3f nlerpNormal;
            for (NCount i = 0; i < numVertices; ++i)
            {
                // x
                *pDst++ = *pSrc1 + t * (*pSrc2 - *pSrc1);
                ++pSrc1; ++pSrc2;
                // y
                *pDst++ = *pSrc1 + t * (*pSrc2 - *pSrc1);
                ++pSrc1; ++pSrc2;
                // z
                *pDst++ = *pSrc1 + t * (*pSrc2 - *pSrc1);
                ++pSrc1; ++pSrc2;

                if (morphNormals)
                {
                    // normals must be in the same buffer as pos
                    // perform an nlerp
                    // we don't have enough information for a spherical interp
                    nlerpNormal.x = *pSrc1 + t * (*pSrc2 - *pSrc1);
                    ++pSrc1; ++pSrc2;
                    nlerpNormal.y = *pSrc1 + t * (*pSrc2 - *pSrc1);
                    ++pSrc1; ++pSrc2;
                    nlerpNormal.z = *pSrc1 + t * (*pSrc2 - *pSrc1);
                    ++pSrc1; ++pSrc2;
                    nlerpNormal.normalise();
                    *pDst++ = nlerpNormal.x;
                    *pDst++ = nlerpNormal.y;
                    *pDst++ = nlerpNormal.z;
                }

                pSrc1 += src1Skip;
                pSrc2 += src2Skip;
                pDst += dstSkip;
            }
        }

        /// @copydetails SIMDUtil::affine
        void affine(Matrix4f * dst, const Matrix4f & base, const Matrix4f * src, NCount cnt)
        {
            for (NCount i = 0; i < cnt; ++i)
            {
                const Matrix4f & s = *src;
                Matrix4f & d = *dst;

                // TODO: Promote following code to Matrix4f class.

                d[0][0] = base[0][0] * s[0][0] + base[0][1] * s[1][0] + base[0][2] * s[2][0];
                d[0][1] = base[0][0] * s[0][1] + base[0][1] * s[1][1] + base[0][2] * s[2][1];
                d[0][2] = base[0][0] * s[0][2] + base[0][1] * s[1][2] + base[0][2] * s[2][2];
                d[0][3] = base[0][0] * s[0][3] + base[0][1] * s[1][3] + base[0][2] * s[2][3] + base[0][3];

                d[1][0] = base[1][0] * s[0][0] + base[1][1] * s[1][0] + base[1][2] * s[2][0];
                d[1][1] = base[1][0] * s[0][1] + base[1][1] * s[1][1] + base[1][2] * s[2][1];
                d[1][2] = base[1][0] * s[0][2] + base[1][1] * s[1][2] + base[1][2] * s[2][2];
                d[1][3] = base[1][0] * s[0][3] + base[1][1] * s[1][3] + base[1][2] * s[2][3] + base[1][3];

                d[2][0] = base[2][0] * s[0][0] + base[2][1] * s[1][0] + base[2][2] * s[2][0];
                d[2][1] = base[2][0] * s[0][1] + base[2][1] * s[1][1] + base[2][2] * s[2][1];
                d[2][2] = base[2][0] * s[0][2] + base[2][1] * s[1][2] + base[2][2] * s[2][2];
                d[2][3] = base[2][0] * s[0][3] + base[2][1] * s[1][3] + base[2][2] * s[2][3] + base[2][3];

                d[3][0] = 0;
                d[3][1] = 0;
                d[3][2] = 0;
                d[3][3] = 1;

                ++src;
                ++dst;
            }
        }

        /// @copydetails SIMDUtil::calculateFaceNormals
        void calculateFaceNormals(const NIIf * positions, const GeoEdget::Triangle * triangles,
            Vector4f * faceNormals, NCount numTriangles)
        {
            for (; numTriangles; --numTriangles)
            {
                const GeoEdget::Triangle & t = *triangles++;
                NCount offset;

                offset = t.mVertIndex[0] * 3;
                Vector3f v1(positions[offset + 0], positions[offset + 1], positions[offset + 2]);

                offset = t.mVertIndex[1] * 3;
                Vector3f v2(positions[offset + 0], positions[offset + 1], positions[offset + 2]);

                offset = t.mVertIndex[2] * 3;
                Vector3f v3(positions[offset + 0], positions[offset + 1], positions[offset + 2]);

                Vector3f normal = (v2 - v1).crossProduct(v3 - v1);

                *faceNormals++ = Vector4f(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
            }
        }

        /// @copydetails SIMDUtil::calculateLightFacing
        void calculateLightFacing(const Vector4f & lightPos,
            const Vector4f * faceNormals, NIIb * lightFacings, NCount count)
        {
            for (NCount i = 0; i < count; ++i)
            {
                *lightFacings++ = (lightPos.dotProduct(*faceNormals++) > 0);
            }
        }

        /// @copydetails SIMDUtil::extrude
        void extrude(const Vector4f & lightPos, NIIf extrudeDist,
            const NIIf * pSrcPos, NIIf * pDestPos, NCount numVertices)
        {
            if (lightPos.w == 0.0f)
            {
                // Directional light, extrusion is along light direction

                Vector3f extrusionDir(
                    -lightPos.x,
                    -lightPos.y,
                    -lightPos.z);
                extrusionDir.normalise();
                extrusionDir *= extrudeDist;

                for (NCount vert = 0; vert < numVertices; ++vert)
                {
                    *pDestPos++ = *pSrcPos++ + extrusionDir.x;
                    *pDestPos++ = *pSrcPos++ + extrusionDir.y;
                    *pDestPos++ = *pSrcPos++ + extrusionDir.z;
                }
    }
            else
            {
                // Point light, calculate extrusionDir for every vertex
                N_assert(lightPos.w == 1.0f, "error");

                for (NCount vert = 0; vert < numVertices; ++vert)
                {
                    Vector3f extrusionDir(
                        pSrcPos[0] - lightPos.x,
                        pSrcPos[1] - lightPos.y,
                        pSrcPos[2] - lightPos.z);
                    extrusionDir.normalise();
                    extrusionDir *= extrudeDist;

                    *pDestPos++ = *pSrcPos++ + extrusionDir.x;
                    *pDestPos++ = *pSrcPos++ + extrusionDir.y;
                    *pDestPos++ = *pSrcPos++ + extrusionDir.z;
                }
            }
}
    };
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    SIMDUtil* _getOptimisedUtilGeneral(void)
    {
        static OptimisedUtilGeneral msOptimisedUtilGeneral;
        return &msOptimisedUtilGeneral;
    }
    //---------------------------------------------------------------------
#if N_HAVE_SSE || N_HAVE_NEON
    extern SIMDUtil * _getOptimisedUtilSSE(void);
#endif

#ifdef __DO_PROFILE__
    //---------------------------------------------------------------------
    class OptimisedUtilProfiler : public SIMDUtil
    {
    protected:

        enum
        {
            IMPL_DEFAULT,
#if N_HAVE_SSE | N_HAVE_NEON
            IMPL_SSE,
#endif
            IMPL_COUNT
        };

        struct ProfileItem
        {
            Nui32 mAvgTicks;
            Nui32 mCount;
            uint64 mTotalTicks;
            uint64 mStartTick;

            ProfileItem(void)
                : mAvgTicks()
                , mCount()
                , mTotalTicks()
            {
            }

            void begin(void)
            {
                mStartTick = clock();
            }

            void end(void)
            {
                uint64 ticks = clock() - mStartTick;
                mTotalTicks += ticks;
                ++mCount;
                mAvgTicks = mTotalTicks / mCount;
            }
        };

        typedef ProfileItem ProfileItems[IMPL_COUNT];

        typedef vector<SIMDUtil*>::type OptimisedUtilList;
        OptimisedUtilList mOptimisedUtils;

    public:
        OptimisedUtilProfiler(void)
        {
            mOptimisedUtils.push_back(_getOptimisedUtilGeneral());
#if N_HAVE_SSE | N_HAVE_NEON
            //if (PlatInfo::isSupport(SimdInfo::CS_SSE))
            {
                mOptimisedUtils.push_back(_getOptimisedUtilSSE());
            }
#endif
        }

        virtual void softwareVertexSkinning(
            const NIIf *srcPosPtr, NIIf *destPosPtr,
            const NIIf *srcNormPtr, NIIf *destNormPtr,
            const NIIf *blendWeightPtr, const Nui8 * blendIndexPtr,
            const Matrix4f* const* blendMatrices,
            NCount srcPosStride, NCount destPosStride,
            NCount srcNormStride, NCount destNormStride,
            NCount blendWeightStride, NCount blendIndexStride,
            NCount numWeightsPerVertex,
            NCount numVertices)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil* impl = mOptimisedUtils[index];
            ProfileItem& profile = results[index];

            profile.begin();
            impl->softwareVertexSkinning(
                srcPosPtr, destPosPtr,
                srcNormPtr, destNormPtr,
                blendWeightPtr, blendIndexPtr,
                blendMatrices,
                srcPosStride, destPosStride,
                srcNormStride, destNormStride,
                blendWeightStride, blendIndexStride,
                numWeightsPerVertex,
                numVertices);
            profile.end();

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

        virtual void softwareVertexMorph(
            NIIf t,
            const NIIf *srcPos1, const NIIf *srcPos2,
            NIIf *dstPos,
            NCount pos1VSize, NCount pos2VSize, NCount dstVSize,
            NCount numVertices,
            bool morphNormals)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil* impl = mOptimisedUtils[index];
            ProfileItem & profile = results[index];

            profile.begin();
            impl->softwareVertexMorph(
                t,
                srcPos1, srcPos2,
                dstPos,
                pos1VSize, pos2VSize, dstVSize,
                numVertices,
                morphNormals);
            profile.end();

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

        virtual void affine(
            Matrix4* dstMatrices,
            const Matrix4& baseMatrix,
            const Matrix4* srcMatrices,
            
            NCount numMatrices)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil* impl = mOptimisedUtils[index];
            ProfileItem& profile = results[index];

            profile.begin();
            impl->affine(
                dstMatrices,
                baseMatrix,
                srcMatrices,
                numMatrices);
            profile.end();

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

        /// @copydetails SIMDUtil::calculateFaceNormals
        virtual void calculateFaceNormals(
            const NIIf * positions, const GeoEdget::Triangle * triangles,
                Vector4 * faceNormals, NCount numTriangles)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil* impl = mOptimisedUtils[index];
            ProfileItem& profile = results[index];

            profile.begin();
            impl->calculateFaceNormals(
                positions,
                triangles,
                faceNormals,
                numTriangles);
            profile.end();

            //
            //   Dagon SkeletonAnimation sample test results (CPU timestamp per-function call):
            //
            //                  Pentium 4 3.0G HT       Athlon XP 2500+
            //
            //      General     657080                  486494
            //      SSE         223559                  399495
            //

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

        /// @copydetails SIMDUtil::calculateLightFacing
        virtual void calculateLightFacing(
            const Vector4f& lightPos,
            const Vector4f* faceNormals,
            NIIb* lightFacings,
            NCount numFaces)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil * impl = mOptimisedUtils[index];
            ProfileItem & profile = results[index];

            profile.begin();
            impl->calculateLightFacing(
                lightPos,
                faceNormals,
                lightFacings,
                numFaces);
            profile.end();

            //
            //   Dagon SkeletonAnimation sample test results (CPU timestamp per-function call):
            //
            //                  Pentium 4 3.0G HT       Athlon XP 2500+
            //
            //      General     171875                  86998
            //      SSE          47934                  63995
            //

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

        virtual void extrude(const Vector4f & lightPos, NIIf extrudeDist,
            const NIIf * srcPositions, NIIf * destPositions, NCount numVertices)
        {
            static ProfileItems results;
            static NCount index;
            index = N_Engine().getCurrentFrame() % mOptimisedUtils.size();
            SIMDUtil* impl = mOptimisedUtils[index];
            ProfileItem& profile = results[index];

            profile.begin();
            impl->extrude(lightPos, extrudeDist, srcPositions, destPositions, numVertices);
            profile.end();

            //
            //   Dagon SkeletonAnimation sample test results (CPU timestamp per-function call):
            //
            //                                  Pentium 4 3.0G HT   Athlon XP 2500+
            //
            //      Directional Light, General   38106               92306
            //      Directional Light, SSE       27292               67055
            //
            //      Point Light, General        224209              155483
            //      Point Light, SSE             56817              106663
            //

            // You can put break point here while running test application, to
            // watch profile results.
            ++index;    // So we can put break point here even if in release build
        }

    };
#endif // __DO_PROFILE__

    //---------------------------------------------------------------------
    SIMDUtil * SIMDUtil::mImpl = 0;
    //---------------------------------------------------------------------
    void SIMDUtil::startup()
    {
        //
        // Some speed test results (averaged number of CPU timestamp (RDTSC) per-function call):
        //
        //   Dagon SkeletonAnimation sample - softwareVertexSkinning:
        //
        //                                      Pentium 4 3.0G HT       Athlon XP 2500+     Athlon 64 X2 Dual Core 3800+
        //
        //      Shared Buffers, General C       763677                  462903              473038
        //      Shared Buffers, Unrolled SSE    210030 *best*           369762              228328 *best*
        //      Shared Buffers, General SSE     286202                  352412 *best*       302796
        //
        //      Separated Buffers, General C    762640                  464840              478740
        //      Separated Buffers, Unrolled SSE 219222 *best*           287992 *best*       238770 *best*
        //      Separated Buffers, General SSE  290129                  341614              307262
        //
        //      PosOnly, General C              388663                  257350              262831
        //      PosOnly, Unrolled SSE           139814 *best*           200323 *best*       168995 *best*
        //      PosOnly, General SSE            172693                  213704              175447
        //
        //   Another my own test scene - softwareVertexSkinning:
        //
        //                                      Pentium P4 3.0G HT      Athlon XP 2500+
        //
        //      Shared Buffers, General C       74527                   -
        //      Shared Buffers, Unrolled SSE    22743 *best*            -
        //      Shared Buffers, General SSE     28527                   -
        //
        //
        // Note that speed test appears unaligned load/store instruction version
        // loss performance 5%-10% than aligned load/store version, even if both
        // of them access to aligned data. Thus, we should use aligned load/store
        // as soon as possible.
        //
        //
        // We are pick up the implementation based on test results above.
        //
#ifdef __DO_PROFILE__
        {
            static OptimisedUtilProfiler msOptimisedUtilProfiler;
            mImpl = &msOptimisedUtilProfiler;
        }
#else   // !__DO_PROFILE__

#if N_HAVE_SSE
        if (PlatInfo::isSupport(SimdInfo::CS_SSE))
        {
            mImpl = _getOptimisedUtilSSE();
        }
        else
#elif N_HAVE_NEON
        if (PlatInfo::isSupport(SimdInfo::CS_NEON))
        {
            mImpl = _getOptimisedUtilNEON();
        }
        else
#endif
        {
            mImpl = _getOptimisedUtilGeneral();
        }
#endif
    }
    void SIMDUtil::shutdown()
    {

    }
}