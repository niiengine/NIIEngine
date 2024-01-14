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

#ifndef _NII_SIMD_VECTOR_H_
#define _NII_SIMD_VECTOR_H_
#include "NIIPrereq.h"
namespace NII
{
    class _EngineAPI Vector3Simd : public Vector3<NIIf , Vector3S>
    {
    public :
        inline Vector3Simd()
        {
        }
        inline Vector3Simd( const NIIf fX, const NIIf fY, const NIIf fZ):Vector3(x,y,z)
        {
        }

        inline explicit Vector3Simd( const NIIf afCoordinate[3] ):Vector3(afCoordinate[0],afCoordinate[1],afCoordinate[2])
        {
        }

        inline explicit Vector3Simd( const NIIi afCoordinate[3] ):Vector3(afCoordinate[0],afCoordinate[1],afCoordinate[2])
        {
        }

        inline explicit Vector3Simd( NIIf* const r ):Vector3(r[0],r[1],r[2])
        {
        }

        inline explicit Vector3Simd( const NIIf scaler ):Vector3(scaler)
        {
        }
        /** SSE 和 3dnow!的Vector3 的operator + 实现
        */
        inline Vector3Simd & operator +(const Vector3Simd *rkVector)
        {
            Vector3Simd reVector;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [esp+4])        ;eax = &rkVector
                NIIASMt2(mov edx, [this])            ;edx = &this
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = ? | Z1 | Y1 | X1
                NIIASMt2(lea ecx, [reVector])        ;ecx = &reVector -- Hide a bit of latency in movaps Latency=1, Throughput=2 if I recall)
                NIIASMt2(addps xmm0, [edx])        ;xmm0 = ? | Z1+Z2 | Y1+Y2 | X1+X2
                NIIASMt2(movaps [ecx], xmm0)        ;Output = ? | Z1+Z2 | Y1+Y2 | X1+X2
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load vector 1
                NIIASMt2(movq mm0, [eax])                 ;前2个
                NIIASMt2(movd mm1, [eax+8])            ;后2个

                ;Add vector 2
                NIIASMt2(pfadd mm0, [edx])                 ;前2个相加
                NIIASMt2(lep eax, [reVector])             ;Hide 2 cycle latency of pfadd if possible
                NIIASMt2(pfadd mm1, [edx+8])            ;后2个相加

                ;Store
                NIIASMt2(movq [eax], mm0)               ;存储前2个
                NIIASMt2(movd [eax+8], mm1)                ;存储后2个

                NIIASMt1(femms))
            }
    #endif
            return reVector;
        }
        /** SSE 和 3dnow!的Vector3 的operator += 实现
        */
        inline Vector3Simd & operator +=(const Vector3Simd *rkVector)
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [this])            ;eax = &this
                NIIASMt2(mov edx, [esp+4])            ;edx = &rkVector
                NIIASMt2(movaps xmm0, [eax])            ;xmm0 = ? | Z1 | Y1 | X1
                NIIASMt2(addps xmm0, [edx])            ;xmm0 = ? | Z1+Z2 | Y1+Y2 | X1+X2
                NIIASMt2(movaps [eax], xmm0)            ;V1 = ? | Z1+Z2 | Y1+Y2 | X1+X2
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [this])
                NIIASMt2(mov edx, [esp+4])

                ;Load vector 1
                NIIASMt2(movq mm0, [eax])
                NIIASMt2(movd mm1, [eax+8])

                ;Add vector 2
                NIIASMt2(pfadd mm0, [edx])
                NIIASMt2(pfadd mm1, [edx+8])

                ;Store
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movd [eax+8], mm1)

                NIIASMt2(femms)
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3的 operator - 实现
        */
        inline Vector3Simd & operator -(const Vector3Simd *rkVector)
        {
            //sse
            Vector3Simd reVector;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [esp+4])            ;eax = &Left
                NIIASMt2(mov edx, [this])            ;edx = &Right
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = ? | Lz | Ly | Lx
                NIIASMt2(lea ecx, [reVector])        ;ecx = &reVector -- Hide a bit of latency in movaps
                NIIASMt2(subps xmm0, [edx])            ;xmm0 = ? | Lz-Rz | Ly-Ry | Lx-Rx
                NIIASMt2(movaps [ecx], xmm0)        ;Output = ? | Lz-Rz | Ly-Ry | Lx-Rx
            }

    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load vector 1
                NIIASMt2(movq mm0, [eax])
                NIIASMt2(movd mm1, [eax+8])

                ;Add vector 2
                NIIASMt2(pfsub mm0, [edx])
                NIIASMt2(lea eax, [reVector])        ;Hide 2 cycle latency of pfadd if possible
                NIIASMt2(pfsub mm1, [edx+8])

                ;Store
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movd [eax+8], mm1)

                NIIASMt2(femms)
            }
    #endif
            return reVector;
        }
        /** SSE 和 3dnow!的Vector3的 operator -= 实现
        */
        inline Vector3Simd & operator -=(const Vector3Simd *rkVector)
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [this])            ;eax = &this
                NIIASMt2(mov edx, [esp+4])            ;edx = &rkVector
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = ? | Lz | Ly | Lx
                NIIASMt2(subps xmm0, [edx])            ;xmm0 = ? | Lz-Rz | Ly-Ry | Lx-Rx
                NIIASMt2(movaps [eax], xmm0)        ;Left = ? | Lz-Rz | Ly-Ry | Lx-Rx
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [this])
                NIIASMt2(mov edx, [esp+4])

                ;Load vector 1
                NIIASMt2(movq mm0, [eax])
                NIIASMt2(movd mm1, [eax+8])

                ;Add vector 2
                NIIASMt2(pfsub mm0, [edx])
                NIIASMt2(pfsub mm1, [edx+8])

                ;Store
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movd [eax+8], mm1)

                NIIASMt2(femms)
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3 的operator * 实现
        */
        inline Vector3Simd & operator *(const NIIf fScalar)
        {
            Vector3Simd reVector;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4

            __asm{
                NIIASMt2(movlps xmm0, [esp+4])                ;xmm0 = ? | ? | ? | scalar
                NIIASMt2(mov edx, [this])                    ;eax = &Input
                NIIASMt3(shufps xmm0, xmm0, 0x00)            ;xmm0 = scalar | scalar | scalar | scalar
                NIIASMt2(lea eax, [reVector])                ;edx = &reVector output (hide shufps latency with another instruction)
                NIIASMt2(mulps xmm0, [eax])                    ;xmm0 = ? | Z*scalar | Y*scalar | X*scalar
                NIIASMt2(movaps [edx], xmm0)                ;Output = ? | Z*scalar | Y*scalar | X*scalar
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov edx, [this])            ;eax = pIn
                NIIASMt2(movd mm0, [esp+4])            ;mm0 = 0 | scalar    (would movq be faster? The next punpckldq will just overwrite the entire upper half)

                NIIASMt2(movq mm1, [edx])            ;mm1 = y | x
                NIIASMt2(punpckldq mm0, mm0)        ;mm0 = scalar | scalar
                NIIASMt2(movd mm2, [edx+8])        ;mm2 = 0 | z  (same thing, would movq be faster?)
                NIIASMt2(pfmul mm1, mm0)            ;mm1 = y*scalar | x*scalar
                NIIASMt2(lea eax, [reVector])        ;eax = pOut
                NIIASMt2(pfmul mm2, mm0)            ;mm2 = 0*scalar | z*scalar
                NIIASMt2(movq [eax], mm1)
                NIIASMt2(movd [eax+8], mm2)

                NIIASMt2(femms)
            }
    #endif
            return reVector;
        }
        /** SSE 和 3dnow!的Vector3 的operator *= 实现
        */
        inline Vector3Simd & operator *=(const NIIf fScalar)
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(movlps xmm0, [esp+4])            ;xmm0 = ? | ? | scalar | ? (movlps is faster than movss. See http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/25112.PDF)
                NIIASMt2(mov eax, [this])                ;eax = &Output
                NIIASMt3(shufps xmm0, xmm0, 0x55)        ;xmm0 = scalar | scalar | scalar | scalar
                NIIASMt2(mulps xmm0, [eax])                ;xmm0 = ? | Z*scalar | Y*scalar | X*scalar
                NIIASMt2(movaps [eax], xmm0)            ;Output = ? | Z*scalar | Y*scalar | X*scalar
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [this])            ;eax = pOut
                NIIASMt2(movd mm0, [esp+4])            ;mm0 = 0 | scalar    (would movq be faster? The next punpckldq will just overwrite the entire upper half)

                NIIASMt2(movq mm1, [eax])            ;mm1 = y | x
                NIIASMt2(punpckldq mm0, mm0)        ;mm0 = scalar | scalar
                NIIASMt2(movd mm2, [eax+8])            ;mm2 = 0 | z  (same thing, would movq be faster?)
                NIIASMt2(pfmul mm1, mm0)            ;mm1 = y*scalar | x*scalar
                NIIASMt2(pfmul mm2, mm0)            ;mm2 = 0*scalar | z*scalar
                NIIASMt2(movq [eax], mm1)
                NIIASMt2(movd [eax+8], mm2)

                NIIASMt1(femms)
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3 的corssProduct 实现
        */
        inline Vector3Simd & corssProduct(const Vector3Simd * vec)
        {
            Vector3Simd reVector;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Fetch data and make duplicates
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = ? | Lz | Ly | Lx
                NIIASMt2(movaps xmm1, [edx])        ;xmm1 = ? | Rz | Ry | Rx
                NIIASMt2(movaps xmm2, xmm0)            ;xmm2 = ? | Lz | Ly | Lx
                NIIASMt2(movaps xmm3, xmm1)            ;xmm3 = ? | Rz | Ry | Rx

                ;Shuffle constants for calculation of cross product
                ;0xC9 == 1100|1001 == w | x | z | y
                ;0xD2 == 1101|0010 == w | y | x | z
                ;If you are having a hard time following these shuffles, look at the definition of a crossproduct, or check vector.c
                ;for the generic C implementation

                NIIASMt3(shufps xmm0, xmm0, 0xC9)        ;xmm0 = ? | Lx | Lz | Ly
                NIIASMt3(shufps xmm1, xmm1, 0xD2)        ;xmm1 = ? | Rz | Ry | Rx
                NIIASMt3(shufps xmm2, xmm2, 0xD2)        ;xmm2 = ? | Lz | Ly | Lx
                NIIASMt3(shufps xmm3, xmm3, 0xC9)        ;xmm3 = ? | Rx | Rz | Ry

                ;Multiply the two columns
                NIIASMt2(mulps xmm1, xmm0)                ;xmm1 = ? | Lx*Rz | Lz*Ry | Ly*Rx
                NIIASMt2(mulps xmm3, xmm2)                ;xmm3 = ? | Lz*Rx | Ly*Rz | Lx*Ry

                ;Hide latency and load edx with output pointer
                NIIASMt2(lea edx, [reVector])            ;edx = &Output

                ;Now just subtract
                NIIASMt2(subps xmm0, xmm2)                ;xmm0 = cross(Left, Right)

                NIIASMt2(movaps [edx], xmm0)            ;Left = cross(Left, Right)
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [vec])
                NIIASMt2(mov edx, [this])

                NIIASMt2(movq mm0, [eax])            ;mm0 = Ly | Lx
                NIIASMt2(movq mm1, [eax+8])            ;mm1 = ?? | Lz
                NIIASMt2(movq mm6, mm0)                ;mm6 = Ly | Lx

                NIIASMt2(punpckldq mm1, mm0)        ;mm1 = Lx | Lz
                NIIASMt2(movq mm2, [edx])            ;mm2 = Ry | Rx
                NIIASMt2(movq mm3, [edx+8])            ;mm3 = ?? | Rz

                NIIASMt2(lep ecx, [reVector])

                NIIASMt2(pfmul mm1, [edx+4])                                    ;mm1 = Lx*Rz | Lz*Ry    RIGHT TOP
                NIIASMt2(movq mm7, mm2)                                            ;mm7 = Ry | Rx
                NIIASMt2(punpckldq mm2, mm3)                                    ;mm2 = Rx | Rz
                NIIASMt2(pand mm6, [_SIMDx86_float_3DNOW_NO_W_MASK])            ;mm6 = 0  | Lx
                NIIASMt2(pfmul mm2, [eax+4])                                    ;mm2 = Lz*Rx | Ly*Rz    LEFT TOP
                NIIASMt2(pand mm7, [_SIMDx86_float_3DNOW_NO_W_MASK])            ;mm7 = 0  | Rx
                NIIASMt2(pfmul mm6, [edx+4])                                    ;mm6 = 0*Rz | Lx*Ry    LEFT BOTTOM
                NIIASMt2(pfsub mm2, mm1)                                        ;mm2 = Lz*Rx-Lx*Rz | Ly*Rz-Lz*Ry  DIFF1
                NIIASMt2(pfmul mm7, [eax+4])                                    ;mm7 = 0*Lz | Ly*Rx    RIGHT BOTTOM
                NIIASMt2(movq [ecx], mm2)                                        ;Out = xprod.y | xprod.x
                NIIASMt2(pfsub mm6, mm7)                                        ;mm6 = 0 |
                NIIASMt2(movd [ecx+8], mm6)                                        ;Out = xprod.z

                NIIASMt2(femms)
            }
    #endif
            return reVector;
        }
        /** SSE 和 3dnow!的Vector3的 dotProduct实现
        */
        inline NIIf dotProduct(const Vector3Simd * vec)//3
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <3
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load and multiply vectors
                NIIASMt2(movaps xmm0, [eax])            ;xmm0 = ? | Z1 | Y1 | X1
                NIIASMt2(mulps xmm0, [edx])                ;xmm1 = ? | Z1*Z2 | Y1*Y2 | X1*X2

                ;Perform horizontal sum
                NIIASMt2(movhlps xmm1, xmm0)            ;xmm1 = ? | ? | ? | Z1*Z2
                NIIASMt2(addss xmm1, xmm0)                ;xmm0 = ? | ? | ? | Z1*Z2 + X1*X2
                NIIASMt3(shufps xmm0, xmm0, 0x55)         ;xmm0 = ? | ? | ? | Y1*Y2
                NIIASMt2(addss xmm1, xmm0)                ;xmm1 = ? | ? | ? | Z1*Z2 + Y1*Y2 + X1*X2

                ;Save in st0 where C looks for result
                NIIASMt2(movss [esp-4], xmm1)
                NIIASMt1(fld dword ptr[esp-4])
            }
    #elif NII_PLAMODE == 3
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load vector 1 and mask off the 'w' component
                NIIASMt2(movaps xmm0, [eax])                            ;xmm0 = ? | z1 | y1 | x1
                NIIASMt2(andps xmm0, [SSE_NO_W_MASK])                    ;xmm0 = ? & 0x0 | z1 & 0xFFFFFFFF | y1 & 0xFFFFFFFF | x1 & 0xFFFFFFFF

                ;Multiply components then horizontally add
                NIIASMt2(mulps xmm0, [edx])                            ;xmm0 = 0 | z1*z2 | y1*y2 | x1*x2
                NIIASMt2(haddps xmm0, xmm0)                            ;xmm0 = ? | ?  | 0 + z1*z2 | y1*y2 + x1*x2
                NIIASMt2(haddps xmm0, xmm0)                            ;xmm0 = ? | ?  | ? | 0 + z1*z2 + y1*y2 + x1*x2

                NIIASMt2(movss [esp-4], xmm0)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                NIIASMt2(movq mm0, [eax])                ; y1 | x1
                NIIASMt2(movd mm1, [eax+8])                ;  0 | z1

                NIIASMt2(pfmul mm0, [edx])                ; y1*y2 | x1*x2
                NIIASMt2(pfmul mm1, [edx+8])            ; 0     | z1*z2

                NIIASMt2(pfadd mm1, mm0)                ; y1*y2 | x1*x2+z1*z2
                NIIASMt2(pfacc mm1, mm1)                ; x1*x2+y1*y2+z1*z2 | x1*x2+y1*y2+z1*z2

                NIIASMt2(movd [esp-4], mm1)
                NIIASMt1(femms)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3 的length() 实现
        */
        inline NIIf length() const
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <3
            __asm{
                NIIASMt2(mov edx, [this])                ;edx = &Src1
                NIIASMt2(movaps xmm0, [edx])            ;xmm0 = ? | Z1 | Y1 | X1
                NIIASMt2(mulps xmm0,xmm0)                ;xmm0 = ? | Z1*Z1 | Y1*Y1 | X1*X1
                NIIASMt2(movhlps xmm1, xmm0)            ;xmm1 = ? | ? | ? | Z1*Z1
                NIIASMt2(addss xmm1, xmm0)                ;xmm1 = ? | ? | ? | X1*X1+Z1*Z1
                NIIASMt3(shufps xmm0, xmm0, 0x00)        ;xmm0 = Y1*Y1 | Y1*Y1 | Y1*Y1 | Y1*Y1
                NIIASMt2(addss xmm1, xmm0)                ;xmm1 = ? | ? | ? | X1*X1+Y1*Y1+Z1*Z1 == magsq(Src1)

                ;Ok, we have the magnitude (length) squared, now we need the square root.
                ;Since this is the low precision version, we do the reciprocal of the reciprocal-square-root approximation.
                ;This is blazingly fast, much faster than a square root, but not so great precision (~3-4 decimal places at best)
                ;Compare to high-precision version below.

                NIIASMt2(rsqrtss xmm1, xmm1)            ;xmm1 = ? | ? | ? | 1/mag(Src1) (approx)
                NIIASMt2(rcpss xmm1, xmm1)                ;xmm1 = ? | ? | ? | mag(Src1) (approx)
                ; 2 sqrtss xmm1, xmm2                    ;xmm1 = ? | ? | ? | sqrtf(X1*X1+Y1*Y1+Z1*Z1) == mag(Src1)

                NIIASMt2(movss [esp-4], xmm1)            ;temp = magsq(Src1, Src1)
                NIIASMt11(fld dword,ptr[esp-4])                ;st0 = magsq(Src1, Src1)
            }
    #elif NII_PLAMODE == 3
            __asm{
                NIIASMt2(mov edx, [this])

                ;Load vector and mask off the 'w' component
                NIIASMt2(movaps xmm0, [edx])            ;xmm0 = ? | z | y | x
                NIIASMt2(andps xmm0, [SSE_NO_W_MASK])    ;xmm0 = ? & 0x0 | z & 0xFFFFFFFF | y & 0xFFFFFFFF | x & 0xFFFFFFFF

                ;Square components and horizontally add
                NIIASMt2(mulps xmm0, xmm0)                ;xmm0 = 0 | z*z | y*y | x*x
                NIIASMt2(haddps xmm0, xmm0)            ;xmm0 = ? | ?  | 0 + z*z | y*y + x*x
                NIIASMt2(haddps xmm0, xmm0)            ;xmm0 = ? | ?  | ? | 0 + z*z + y*y + x*x

                ;Cheap square root
                NIIASMt2(rsqrtss xmm0, xmm0)
                NIIASMt2(rcpss xmm0, xmm0)
                ; 2 sqrtss xmm0, xmm0

                NIIASMt2(movss [esp-4], xmm0)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov edx, [this])

                NIIASMt2(movq mm0, [edx])            ;mm0 = y | x
                NIIASMt2(movd mm1, [edx+8])            ;mm1 = 0  | z

                NIIASMt2(pfmul mm0, mm0)            ;mm0 = y*y | x*x
                NIIASMt2(pfmul mm1, mm1)            ;mm1 = 0*0 | z*z

                NIIASMt2(pfadd mm1, mm0)            ; y*y+0*0 | x*x+z*z
                NIIASMt2(pfacc mm1, mm1)            ; x*x+y*y+z*z | x*x+y*y+z*z

                ;Get 1.0/sqrtf(mag) in mm2, then multiply by original mag to get sqrtf(mag) in about 4 cycles (K6-2/Athlon)
                NIIASMt2(pfrsqrt mm2, mm1)            ; 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                NIIASMt2(pfmul mm1, mm2)            ; len(vector) | len(vector)
                ; 2 Do full precision reciprocal square root using hw Newton-Raphson iterations (follow AMD 3DNow! documents as their instructions are a bit unusual)
                ;movq mm1, mm7                        ;mm7 = x*x+y*y+z*z | x*x+y*y+z*z
                ;pfrsqrt mm2, mm1                    ;mm2 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;movq mm3, mm2                        ;mm3 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;pfmul mm2, mm2                        ;mm2 = x1 = x0*x0
                ;pfrsqit1 mm1, mm2                    ;mm1 = x2 = pfrsqit1(mag*mag, x1)
                ;pfrcpit2 mm1, mm3                    ;mm1 = 1/sqrtf(mag*mag) = pfrcpit2(x2, x0)

                ;pfmul mm1, mm7                        ;mm1 = mag*mag / sqrtf(mag*mag) = sqrtf(mag*mag) = mag

                NIIASMt1(movd [esp-4], mm1)
                NIIASMt1(femms)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3的 lengthSquared 实现
        */
        inline NIIf lengthSquared()
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <3
            __asm{
                NIIASMt2(mov edx, [this])                ;edx = &Src1
                NIIASMt2(movaps xmm0, [edx])            ;xmm0 = ? | Z1 | Y1 | X1
                NIIASMt2(mulps xmm0,xmm0)                ;xmm0 = ? | Z1*Z1 | Y1*Y1 | X1*X1
                NIIASMt2(movhlps xmm1, xmm0)            ;xmm1 = ? | ? | ? | Z1*Z1
                NIIASMt2(addss xmm1, xmm0)                ;xmm1 = ? | ? | ? | X1*X1+Z1*Z1
                NIIASMt3(shufps xmm0, xmm0, 0x00)        ;xmm0 = Y1*Y1 | Y1*Y1 | Y1*Y1 | Y1*Y1
                NIIASMt2(addss xmm1, xmm0)                ;xmm1 = ? | ? | ? | X1*X1+Y1*Y1+Z1*Z1 == magsq(Src1)
                NIIASMt2(movss [esp-4], xmm1)            ;temp = magsq(Src1, Src1)
                NIIASMt11(fld dword,ptr[esp-4])                ;st0 = magsq(Src1, Src1)
            }
    #elif NII_PLAMODE == 3
            __asm{
                NIIASMt2(mov edx, [this])

                ;Load vector and mask off the 'w' component
                NIIASMt2(movaps xmm0, [edx])                ;xmm0 = ? | z | y | x
                NIIASMt2(andps xmm0, [SSE_NO_W_MASK])        ;xmm0 = ? & 0x0 | z & 0xFFFFFFFF | y & 0xFFFFFFFF | x & 0xFFFFFFFF

                ;Square components and horizontally add
                NIIASMt2(mulps xmm0, xmm0)                ;xmm0 = 0 | z*z | y*y | x*x
                NIIASMt2(haddps xmm0, xmm0)                ;xmm0 = ? | ?  | 0 + z*z | y*y + x*x
                NIIASMt2(haddps xmm0, xmm0)                ;xmm0 = ? | ?  | ? | 0 + z*z + y*y + x*x

                NIIASMt2(movss [esp-4], xmm0)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov edx, [this])

                NIIASMt2(movq mm0, [edx])            ; y1 | x1
                NIIASMt2(movd mm1, [edx+8])        ;  0 | z1

                NIIASMt2(pfmul mm0, mm0)            ; y1*y2 | x1*x2
                NIIASMt2(pfmul mm1, mm1)            ; 0     | z1*z2

                NIIASMt2(pfadd mm1, mm0)            ; y1*y2 | x1*x2+z1*z2
                NIIASMt2(pfacc mm1, mm1)            ; x1*x2+y1*y2+z1*z2 | x1*x2+y1*y2+z1*z2

                NIIASMt2(movd [esp-4], mm1)
                NIIASMt2(femms)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3 的normalize 实现
        */
        inline void normalize()
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <3
            __asm{
                NIIASMt2(mov eax, [this])
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = ? | z | y | x
                NIIASMt2(movaps xmm2, xmm0)            ;Save original vector in xmm2
                NIIASMt2(mulps xmm0, xmm0)            ;Square terms

                ;Sum components
                NIIASMt2(movhlps xmm1, xmm0)        ;xmm1 = ? | ? | ? | z*z
                NIIASMt2(addss xmm1, xmm0)            ;xmm0 = ? | ? | ? | z*z + x*x
                NIIASMt3(shufps xmm0, xmm0, 0x55)     ;xmm0 = ? | ? | ? | y*y
                NIIASMt2(addss xmm1, xmm0)            ;xmm1 = ? | ? | ? | z*z + y*y + x*x

                ;Get reciprocal square root and multiply to perform cheap square root
                NIIASMt2(rsqrtss xmm1, xmm1)        ;xmm1 = ? | ? | ? | 1.0f / sqrtf(z*z + y*y + x*x) == mag(input)
                ;2 sqrtss xmm1, xmm1                ;xmm1 = ? | ? | ? | 1.0f / sqrtf(z*z + y*y + x*x) == mag(input)
                NIIASMt3(shufps xmm1, xmm1, 0x00)     ;xmm1 = mag(input) | mag(input) | mag(input) | mag(input)
                NIIASMt2(mulps xmm1, xmm2)            ;xmm0 = ? | z / mag() | y / mag() | x / mag()
                ;2 divps xmm2, xmm1                    ;xmm2 = ? | z / mag() | y / mag() | x / mag()
                ;Write out result
                NIIASMt2(movaps [eax], xmm1)
                ; 2 movaps [eax], xmm2
            }
    #elif    NII_PLAMODE == 3
            __asm{
                NIIASMt2(mov eax, [this])
                NIIASMt2(movaps xmm0, [eax])                ;xmm0 = ? | z | y | x
                NIIASMt2(andps xmm0, [SSE_NO_W_MASK])        ;xmm0 = ? & 0x0 | z & 0xFFFFFFFF | y & 0xFFFFFFFF | x & 0xFFFFFFFF
                NIIASMt2(movaps xmm2, xmm0)                    ;Save original vector in xmm2

                ;Square components and sum
                NIIASMt2(mulps xmm0, xmm0)                    ;xmm0 = 0*0 | z*z | y*y | x*x
                NIIASMt2(haddps xmm0, xmm0)                    ;xmm0 = 0*0+z*z | y*y+x*x | 0*0+z*z | y*y+x*x
                NIIASMt2(haddps xmm0, xmm0)                    ;xmm0 = 0*0+z*z+y*y+x*x | 0*0+z*z+y*y+x*x | 0*0+z*z+y*y+x*x | 0*0+z*z+y*y+x*x

                ;Get reciprocal square root (estimate) and multiply to perform cheap square root
                NIIASMt2(rsqrtps xmm1, xmm0)                ;xmm1 = 1.0f / sqrtf(z*z + y*y + x*x), all components; equal to 1.0/mag
                NIIASMt2(mulps xmm2, xmm1)                    ;xmm0 = 0/mag | z/mag | y/mag | x/mag
                ;2 Get full square root and divide
                ;2 sqrtss xmm1, xmm0                        ;xmm1 = ? | ? | ? | sqrtf(z*z + y*y + x*x) == mag. Since all values are the same, doing a scalar sqrt + shuffle rather than quad-square root (slower).
                ;2 shufps xmm1, xmm1, 0x00                    ;xmm1 = mag | mag | mag | mag
                ;2 divps xmm2, xmm1                            ;xmm0 = 0/mag | z/mag | y/mag | x/mag

                ;Write out result
                NIIASMt2(movaps [eax], xmm2)
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [this])

                NIIASMt2(movq mm0, [eax])            ;mm0 = y | x
                NIIASMt2(movd mm1, [eax+8])            ;mm1 = 0  | z

                NIIASMt2(movq mm0, mm4)                ;mm4 = y | x
                NIIASMt2(movq mm1, mm5)                ;mm5 = 0 | z

                NIIASMt2(pfmul mm0, mm0)            ;mm0 = y*y | x*x
                NIIASMt2(pfmul mm1, mm1)            ;mm1 = 0*0 | z*z

                NIIASMt2(pfadd mm1, mm0)            ; y*y+0*0 | x*x+z*z
                NIIASMt2(pfacc mm1, mm1)            ; x*x+y*y+z*z | x*x+y*y+z*z == magsquared(vector)

                ;Do full precision reciprocal square root using hw Newton-Raphson iterations (follow AMD 3DNow! documents as their instructions are a bit unusual)
                NIIASMt2(pfrsqrt mm2, mm1)            ;mm2 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                NIIASMt2(pfmul mm4, mm2)            ;mm4 = y/mag | x/mag
                NIIASMt2(pfmul mm5, mm2)            ;mm5 = 0     | z/mag
                ;2 Do full precision reciprocal square root
                ;2 pfrsqrt mm2, mm1                    ;mm2 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;2 movq mm3, mm2                    ;mm3 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;2 pfmul mm2, mm2                    ;mm2 = x1 = x0*x0
                ;2 pfrsqit1 mm1, mm2                ;mm1 = x2 = pfrsqit1(mag*mag, x1)
                ;2 pfrcpit2 mm1, mm3                ;mm1 = 1/sqrtf(mag*mag) = pfrcpit2(x2, x0)

                ;2 pfmul mm4, mm1                    ;mm4 = y/mag | x/mag
                ;2 pfmul mm5, mm1                    ;mm5 = 0     | z/mag

                NIIASMt2(movq [eax], mm4)
                NIIASMt2(movq [eax+8], mm5)

                NIIASMt2(femms)
            }
    #endif
        }
        /** SSE 和 3dnow!的Vector3 的distance 实现
        */
        inline NIIf distance(const Vector3<NIIf,Vector3S> * vec)
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <3
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load vector 1 and subtract vector 2 from it
                NIIASMt2(movaps xmm0, [eax])
                NIIASMt2(subps xmm0, [edx])

                ;Square the resultant
                NIIASMt2(mulps xmm0,xmm0)            ;xmm0 = ? | Z1*Z1 | Y1*Y1 | X1*X1

                ;Horizontally add the components
                NIIASMt2(movhlps xmm1, xmm0)        ;xmm1 = ? | ? | ? | Z1*Z1
                NIIASMt2(addss xmm1, xmm0)            ;xmm1 = ? | ? | ? | X1*X1+Z1*Z1
                NIIASMt3(shufps xmm0, xmm0, 0x00)    ;xmm0 = Y1*Y1 | Y1*Y1 | Y1*Y1 | Y1*Y1
                NIIASMt2(addss xmm1, xmm0)            ;xmm1 = ? | ? | ? | X1*X1+Y1*Y1+Z1*Z1 == magsq(Src1)

                ;Ok, we have the magnitude (length) squared, now we need the square root.
                ;Since this is the low precision version, we do the reciprocal of the reciprocal-square-root approximation.
                ;This is blazingly fast, much faster than a square root, but not so great precision (~3-4 decimal places at best)
                NIIASMt2(rsqrtss xmm1, xmm1)        ;xmm1 = ? | ? | ? | 1/mag(Src1) (approx)
                NIIASMt2(rcpss xmm1, xmm1)            ;xmm1 = ? | ? | ? | mag(Src1) (approx)
                ;2 sqrtss xmm1, xmm1

                NIIASMt2(movss [esp-4], xmm1)        ;temp = magsq(Src1, Src1)
                NIIASMt11(fld dword,ptr[esp-4])            ;st0 = magsq(Src1, Src1)
            }
    #elif    NII_PLAMODE == 3
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                ;Load vector 1 and subtract vector 2 to find resultant
                NIIASMt2(movaps xmm0, [eax])
                NIIASMt2(subps xmm0, [edx])

                ;Mask of 'w' component (which is undefined), then horizontally add to find magnitude squared
                NIIASMt2(andps xmm0, [SSE_NO_W_MASK])
                NIIASMt2(haddps xmm0, xmm0)
                NIIASMt2(haddps xmm0, xmm0)

                ;Cheap reciprocal of a reciprocal square root (fast, but imperfect)
                NIIASMt2(rsqrtss xmm0, xmm0)
                NIIASMt2(rcpss xmm0, xmm0)
                ; 2 sqrtss xmm0, xmm0

                NIIASMt2(movss [esp-4], xmm0)
                NIIASMt2(fld dword ptr[esp-4])
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])

                NIIASMt2(movq mm0, [eax])        ;mm0 = y1 | x1
                NIIASMt2(movd mm1, [eax+8])        ;mm1 = 0  | z1

                ;Find resultant D = V1 - V2
                NIIASMt2(pfsub mm0, [edx])        ;mm0 = y1-y2 | x1-x2   == dy | dx
                NIIASMt2(pfsub mm1, [edx+8])    ;mm1 = ????? | z1-z2   == ?? | dz

                ;Calculate magnitude of D
                NIIASMt2(pfmul mm0, mm0)                                ;mm0 = dy*dy | dx*dx
                NIIASMt2(pand mm1, [_SIMDx86_float_3DNOW_NO_W_MASK])    ;mm1 = 0 | dz
                NIIASMt2(pfacc mm0, mm0)                                ;mm0 = dy*dy+dx*dx | dy*dy+dx*dx
                NIIASMt2(pfmul mm1, mm1)                                ;mm1 = 0 | dz*dz
                NIIASMt2(pfadd mm1, mm0)                                ;mm1 = ? | dy*dy+dx*dx+dz*dz == dist*dist, stall for 1-2 cycles

                ;Find square root of the squared magnitude
                NIIASMt2(pfrsqrt mm2, mm1)    ;mm2 = 1/sqrtf(dist*dist)
                NIIASMt2(pfmul mm2, mm1)    ;mm2 = dist*dist/sqrtf(dist*dist) == dist
                ;2 movq mm1, mm7            ;mm7 = x*x+y*y+z*z | x*x+y*y+z*z
                ;2 pfrsqrt mm2, mm1            ;mm2 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;2 movq mm3, mm2            ;mm3 = x0 = 1/sqrtf(x*x+y*y+z*z) | 1/sqrtf(x*x+y*y+z*z)
                ;2 pfmul mm2, mm2            ;mm2 = x1 = x0*x0
                ;2 pfrsqit1 mm1, mm2        ;mm1 = x2 = pfrsqit1(mag*mag, x1)
                ;2 pfrcpit2 mm1, mm3        ;mm1 = 1/sqrtf(mag*mag) = pfrcpit2(x2, x0)
                ;2 pfmul mm1, mm7            ;mm1 = mag*mag / sqrtf(mag*mag) = sqrtf(mag*mag) = mag
                ;exit 3DNow and push result into x87 FPU
                NIIASMt2(movd [esp-4], mm2)
                ; 2 movd [esp-4], mm1
                NIIASMt1(femms)
                NIIASMt11(fld dword,ptr[esp-4])
            }
    #endif
        }
    private :
        static const NII_SIMD_ALIGNED_DECL2(Nui32,SSE_NO_W_MASK[4]);
    };
#define cccusec a,b,c
        NII_SIMD_ALIGNED_DECL3(PolygonSimd,NII::Vector3Simd,cccusec);
#undef  cccusec
}
#endif