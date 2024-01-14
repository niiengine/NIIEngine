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

#ifndef _NII_MATRIX_SIMD_H_
#define _NII_MATRIX_SIMD_H_
namespace NII
{
    class Matrix4Simd :public Matrix4<NIIf>
    {
    public:
        Matrix4Simd operator+ ( const Matrix4Simd* m2 ) const
        {
            Matrix4Simd re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
    #define cccccccc 
            __asm{
                NIIASMt2(mov eax, [esp+4])            ;eax = In1
                NIIASMt2(mov edx, [this])            ;edx = In2
                NIIASMt2(movaps xmm0, [eax])        ;xmm0 = In1[0..3]
                NIIASMt2(movaps xmm1, [eax+16])        ;xmm1 = In1[4..7]
                NIIASMt2(movaps xmm2, [eax+32])        ;xmm2 = In1[8..11]
                NIIASMt2(movaps xmm3, [eax+48])        ;xmm3 = In1[12..15]
                NIIASMt2(addps xmm0, [edx])            ;xmm0 = In1[0..3] + In2[0..3]
                NIIASMt2(addps xmm1, [edx+16])        ;xmm1 = In1[4..7] + In2[4..7]
                NIIASMt2(addps xmm2, [edx+32])        ;xmm2 = In1[8..11] + In2[8..11]
                NIIASMt2(addps xmm3, [edx+48])        ;xmm3 = In1[12..15] + In2[12..15]
                NIIASMt2(lea eax, [re])                ;eax = out
                NIIASMt2(movaps [eax], xmm0)        ;Store sum into Out
                NIIASMt2(movaps [eax+16], xmm1)
                NIIASMt2(movaps [eax+32], xmm2)
                NIIASMt2(movaps [eax+48], xmm3)        
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])
                NIIASMt2(mov edx, [this])
                NIIASMt2(movq mm0, [eax])
                NIIASMt2(movq mm1, [eax+8])
                NIIASMt2(movq mm2, [eax+16])
                NIIASMt2(movq mm3, [eax+24])
                NIIASMt2(movq mm4, [eax+32])
                NIIASMt2(movq mm5, [eax+40])
                NIIASMt2(movq mm6, [eax+48])
                NIIASMt2(movq mm7, [eax+56])
                NIIASMt2(pfadd mm0, [edx])
                NIIASMt2(pfadd mm1, [edx+8])
                NIIASMt2(pfadd mm2, [edx+16])
                NIIASMt2(pfadd mm3, [edx+24])
                NIIASMt2(pfadd mm4, [edx+32])
                NIIASMt2(pfadd mm5, [edx+40])
                NIIASMt2(pfadd mm6, [edx+48])
                NIIASMt2(pfadd mm7, [edx+56])
                NIIASMt2(lea eax, [re])             ; =out
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movq [eax+8], mm1)
                NIIASMt2(movq [eax+16], mm2)
                NIIASMt2(movq [eax+24], mm3)
                NIIASMt2(movq [eax+32], mm4)
                NIIASMt2(movq [eax+40], mm5)
                NIIASMt2(movq [eax+48], mm6)
                NIIASMt2(movq [eax+56], mm7)
                NIIASMt(femms)
            }
    #endif    
            return re;            
        }
        /***/
        inline Matrix4Simd operator- ( const Matrix4Simd* m2 ) const
        {
            Matrix4Simd re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [esp+4])                ;eax = In1
                NIIASMt2(mov edx, [this])                ;edx = In2
                NIIASMt2(movaps xmm0, [eax])            ;xmm0 = In1[0..3]
                NIIASMt2(movaps xmm1, [eax+16])            ;xmm1 = In1[4..7]
                NIIASMt2(movaps xmm2, [eax+32])            ;xmm2 = In1[8..11]
                NIIASMt2(movaps xmm3, [eax+48])            ;xmm3 = In1[12..15]
                NIIASMt2(subps xmm0, [edx])                ;xmm0 = In1[0..3] - In2[0..3]
                NIIASMt2(subps xmm1, [edx+16])            ;xmm1 = In1[4..7] - In2[4..7]
                NIIASMt2(subps xmm2, [edx+32])            ;xmm2 = In1[8..11] - In2[8..11]
                NIIASMt2(subps xmm3, [edx+48])            ;xmm3 = In1[12..15] - In2[12..15]
                NIIASMt2(lea eax,[re])                    ;eax=out
                NIIASMt2(movaps [eax], xmm0)            ;Store difference into Out
                NIIASMt2(movaps [eax+16], xmm1)
                NIIASMt2(movaps [eax+32], xmm2)
                NIIASMt2(movaps [eax+48], xmm3)
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov eax, [esp+4])         ;=in1
                NIIASMt2(mov edx, [this])             ;=in2
                NIIASMt2(movq mm0, [eax])
                NIIASMt2(movq mm1, [eax+8])
                NIIASMt2(movq mm2, [eax+16])
                NIIASMt2(movq mm3, [eax+24])
                NIIASMt2(movq mm4, [eax+32])
                NIIASMt2(movq mm5, [eax+40])
                NIIASMt2(movq mm6, [eax+48])
                NIIASMt2(movq mm7, [eax+56])
                NIIASMt2(pfsub mm0, [edx])
                NIIASMt2(pfsub mm1, [edx+8])
                NIIASMt2(pfsub mm2, [edx+16])
                NIIASMt2(pfsub mm3, [edx+24])
                NIIASMt2(pfsub mm4, [edx+32])
                NIIASMt2(pfsub mm5, [edx+40])
                NIIASMt2(pfsub mm6, [edx+48])
                NIIASMt2(pfsub mm7, [edx+56])
                NIIASMt2(lep eax,[re])                ;=out
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movq [eax+8], mm1)
                NIIASMt2(movq [eax+16], mm2)
                NIIASMt2(movq [eax+24], mm3)
                NIIASMt2(movq [eax+32], mm4)
                NIIASMt2(movq [eax+40], mm5)
                NIIASMt2(movq [eax+48], mm6)
                NIIASMt2(movq [eax+56], mm7)
                NIIASMt(femms)
            }
    #endif
            return re;            
        }
        inline Matrix4Simd operator* (NIIf scalar) const
        {
            Matrix4Simd re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(mov eax, [this])                ;=in
                NIIASMt2(movss xmm4, [esp+4])          ;=scalar
                
                ; Get the matrix into registers */
                NIIASMt2(movaps xmm0, [eax])            ;xmm0 = m[0..3]
                NIIASMt2(movaps xmm1, [eax+16])        ;xmm1 = m[4..7]
                NIIASMt3(shufps xmm4, xmm4, 0x00)        ;xmm4 = s | s | s | s
                NIIASMt2(movaps xmm2, [eax+32])        ;xmm2 = m[8..11]
                NIIASMt2(movaps xmm3, [eax+48])        ;xmm3 = m[12..15]
                
                ;Scale the matrix in parallel */
                NIIASMt2(mulps xmm0, xmm4)
                NIIASMt2(mulps xmm1, xmm4)
                NIIASMt2(mulps xmm2, xmm4)
                NIIASMt2(mulps xmm3, xmm4)
                NIIASMt2(lea eax,[re])                    ;=out
                ;Store results */
                NIIASMt2(movaps [eax], xmm0)
                NIIASMt2(movaps [eax+16], xmm1)
                NIIASMt2(movaps [eax+32], xmm2)
                NIIASMt2(movaps [eax+48], xmm3)
            }

    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(mov edx, [this])
                NIIASMt2(lep eax, [re])
                NIIASMt2(movd mm0, [esp+4])
                NIIASMt2(punpckldq mm0, mm0)
                ;Attempt to make use of pipelined operations...
                ;Duplicate, begin multiply, and duplicate again, write result of first multiply
                NIIASMt2(movq mm1, mm0)
                NIIASMt2(pfmul mm1, [edx+8])
                NIIASMt2(movq mm2, mm0)
                NIIASMt2(movq [eax+8], mm1)
                NIIASMt2(pfmul mm2, [edx+16])
                NIIASMt2(movq mm3, mm0)
                NIIASMt2(movq [eax+16], mm2)
                NIIASMt2(pfmul mm3, [edx+24])
                NIIASMt2(movq mm4, mm0)
                NIIASMt2(movq [eax+24], mm3)
                NIIASMt2(pfmul mm4, [edx+32])
                NIIASMt2(movq mm5, mm0)
                NIIASMt2(movq [eax+32], mm4)
                NIIASMt2(pfmul mm5, [edx+40])
                NIIASMt2(movq mm6, mm0)
                NIIASMt2(movq [eax+40], mm5)
                NIIASMt2(pfmul mm6, [edx+48])
                NIIASMt2(movq mm7, mm0)
                NIIASMt2(pfmul mm0, [edx])
                NIIASMt2(movq [eax+48], mm6)
                NIIASMt2(pfmul mm7, [edx+56])
                NIIASMt2(movq [eax], mm0)
                NIIASMt2(movq [eax+56], mm7)
                
                NIIASMt(femms)
            }
    #endif
            return re;            
        }
        
        inline Matrix4Simd concatenate(const Matrix4Simd* m2)
        {
        //sse
            Matrix4Simd *re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
                NIIASMt2(movaps   (%0), %%xmm0)    /* xmm0 = pRight[0..3] */
                NIIASMt2(movaps 16(%0), %%xmm1)    /* xmm1 = pRight[5..7] */
                NIIASMt2(movaps 32(%0), %%xmm2)    /* xmm2 = pRight[8..11] */
                NIIASMt2(movaps 48(%0), %%xmm3)    /* xmm3 = pRight[12..15] */

                /* Processes 1/2 of the matrix at a time (2x4), unrolled loop */
                NIIASMt2(movss    (%1), %%xmm4)
                NIIASMt2(movss   4(%1), %%xmm6)
                NIIASMt2(movss  16(%1), %%xmm5)
                NIIASMt2(movss  20(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm4, %%xmm4)
                NIIASMt2(shufps $0x00, %%xmm5, %%xmm5)
                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm0, %%xmm4)
                NIIASMt2(mulps %%xmm0, %%xmm5)
                NIIASMt2(mulps %%xmm1, %%xmm6)
                NIIASMt2(mulps %%xmm1, %%xmm7)
                NIIASMt2(addps %%xmm7, %%xmm5)
                NIIASMt2(addps %%xmm6, %%xmm4)


                NIIASMt2(movss  8(%1), %%xmm6)
                NIIASMt2(movss 24(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm2, %%xmm6)
                NIIASMt2(mulps %%xmm2, %%xmm7)
                NIIASMt2(addps %%xmm6, %%xmm4)
                NIIASMt2(addps %%xmm7, %%xmm5)

                NIIASMt2(movss  12(%1), %%xmm6)
                NIIASMt2(movss  28(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm3, %%xmm6)
                NIIASMt2(mulps %%xmm3, %%xmm7)
                NIIASMt2(addps %%xmm6, %%xmm4)
                NIIASMt2(addps %%xmm7, %%xmm5)

                NIIASMt2(movaps %%xmm4, (%2))
                NIIASMt2(movaps %%xmm5, 16(%2))

                /* second half of the matrix */
                NIIASMt2(movss  32(%1), %%xmm4)
                NIIASMt2(movss  36(%1), %%xmm6)
                NIIASMt2(movss  48(%1), %%xmm5)
                NIIASMt2(movss  52(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm4, %%xmm4)
                NIIASMt2(shufps $0x00, %%xmm5, %%xmm5)
                NIIASMt2(mulps %%xmm0, %%xmm4)
                NIIASMt2(mulps %%xmm0, %%xmm5)

                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm1, %%xmm6)
                NIIASMt2(mulps %%xmm1, %%xmm7)
                NIIASMt2(addps %%xmm6, %%xmm4)
                NIIASMt2(addps %%xmm7, %%xmm5)


                NIIASMt2(movss 40(%1), %%xmm6)
                NIIASMt2(movss 56(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm2, %%xmm6)
                NIIASMt2(mulps %%xmm2, %%xmm7)
                NIIASMt2(addps %%xmm6, %%xmm4)
                NIIASMt2(addps %%xmm7, %%xmm5)

                NIIASMt2(movss  44(%1), %%xmm6)
                NIIASMt2(movss  60(%1), %%xmm7)
                NIIASMt2(shufps $0x00, %%xmm6, %%xmm6)
                NIIASMt2(shufps $0x00, %%xmm7, %%xmm7)
                NIIASMt2(mulps %%xmm3, %%xmm6)
                NIIASMt2(mulps %%xmm3, %%xmm7)
                NIIASMt2(addps %%xmm6, %%xmm4)
                NIIASMt2(addps %%xmm7, %%xmm5)

                NIIASMt2(movaps %%xmm4, 32(%2))
                NIIASMt2(movaps %%xmm5, 48(%2))

                :
                : NIIASMc(r) (m2), NIIASMc(r) (this), NIIASMc(r) (re)
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
                NIIASMt2(movl $4, %%ecx)
                NIIASMt2(MatrixMultiplyOf_3DNow_Loop%=:)
                NIIASMt2(movd   (%0), %%mm0)            /* mm0 = ? | x */
                NIIASMt2(movd  4(%0), %%mm2)            /* mm2 = ? | y */
                NIIASMt2(movd  8(%0), %%mm4)            /* mm4 = ? | z */
                NIIASMt2(movd 12(%0), %%mm6)            /* mm6 = ? | w */
                NIIASMt2(prefetch 16(%0))                /* prefetch_for_reading(in+4);  */
                NIIASMt2(prefetchw 16(%2))            /* prefetch_for_writing(out+4); */
                NIIASMt2(punpckldq %%mm0, %%mm0)        /* mm0 = x | x */
                NIIASMt2(punpckldq %%mm2, %%mm2)        /* mm2 = y | y */
                NIIASMt2(punpckldq %%mm4, %%mm4)        /* mm4 = z | z */
                NIIASMt2(punpckldq %%mm6, %%mm6)        /* mm6 = w | w */
                NIIASMt2(movq %%mm0, %%mm1)            /* mm1 = x | x */
                NIIASMt2(movq %%mm2, %%mm3)            /* mm3 = y | y */
                NIIASMt2(movq %%mm4, %%mm5)            /* mm5 = z | z */
                NIIASMt2(movq %%mm6, %%mm7)            /* mm7 = w | w */
                NIIASMt2(pfmul   (%1), %%mm0)            /* mm0 = x*m[1]  | x*m[0] */
                NIIASMt2(pfmul  8(%1), %%mm1)            /* mm1 = x*m[3]  | x*m[2] */
                NIIASMt2(pfmul 16(%1), %%mm2)         /* mm2 = y*m[5]  | y*m[4] */
                NIIASMt2(pfmul 24(%1), %%mm3)         /* mm3 = y*m[7]  | y*m[6] */
                NIIASMt2(pfmul 32(%1), %%mm4)         /* mm4 = z*m[9]  | z*m[8] */
                NIIASMt2(pfmul 40(%1), %%mm5)         /* mm5 = z*m[11] | z*m[10] */
                NIIASMt2(pfmul 48(%1), %%mm6)         /* mm6 = w*m[13] | w*m[12] */
                NIIASMt2(pfmul 56(%1), %%mm7)         /* mm7 = w*m[15] | w*m[14] */
                NIIASMt2(pfadd %%mm0, %%mm2)
                NIIASMt2(pfadd %%mm1, %%mm3)
                NIIASMt2(pfadd %%mm4, %%mm6)
                NIIASMt2(pfadd %%mm5, %%mm7)
                NIIASMt2(pfadd %%mm2, %%mm6)
                NIIASMt2(pfadd %%mm3, %%mm7)
                NIIASMt2(movq %%mm6,  (%2)\n)
                NIIASMt2(movq %%mm7, 8(%2)\n)
                NIIASMt2(addl $16, %0)
                NIIASMt2(addl $16, %2)
                NIIASMt2(loop MatrixMultiplyOf_3DNow_Loop%=)
                :
                : NIIASMc(r) (this), NIIASMc(r) (m2), NIIASMc(r) (re)
                : NIIASMc((%ecx)
            }

            /* Execute 'femms' if desired */
            #ifndef NII_NO_EMMS
            __asm{NIIASMt2(femms))
            #endif
    #endif    
            return     (*re);            
        }
        /***/
        inline Vector3Simd operator* ( const Vector3Simd* v ) const
        {
            Vector3Simd *re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
            NIIASMt2(movaps   (%1), %%xmm4)
            NIIASMt2(movaps %%xmm4, %%xmm5)
            NIIASMt2(movaps %%xmm4, %%xmm6)
            NIIASMt2(shufps $0x00, %%xmm4, %%xmm4)    /* xmm4 = x | x | x | x */
            NIIASMt2(shufps $0x55, %%xmm5, %%xmm5)    /* xmm5 = y | y | y | y */
            NIIASMt2(shufps $0xAA, %%xmm6, %%xmm6)    /* xmm6 = z | z | z | z */

            /* Multiply with each row */
            NIIASMt2(mulps   (%0), %%xmm4)
            NIIASMt2(mulps 16(%0), %%xmm5)
            NIIASMt2(mulps 32(%0), %%xmm6)

            /* Sum results, xmm0-2 = transformed x,y,z colums, xmm3 = w column */
            NIIASMt2(addps %%xmm4, %%xmm5)    /* xmm1 = tx + ty */
            NIIASMt2(addps 48(%0), %%xmm6)    /* xmm3 = tz + w */
            NIIASMt2(addps %%xmm5, %%xmm6)    /* xmm1 = tx + ty + tz + w*/

            /* Store translated vector */
            NIIASMt2(movaps %%xmm6, (%2))
            :
            : NIIASMc(r) (this), NIIASMc(r) (v), NIIASMc(r) (re)
            }
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
            NIIASMt2(movd   (%0), %%mm0)            /* mm0 = ? | x */
            NIIASMt2(movd  4(%0), %%mm2)            /* mm2 = ? | y */
            NIIASMt2(movd  8(%0), %%mm4)            /* mm4 = ? | z */
            NIIASMt2(punpckldq %%mm0, %%mm0)        /* mm0 = x | x */
            NIIASMt2(punpckldq %%mm2, %%mm2)        /* mm2 = y | y */
            NIIASMt2(punpckldq %%mm4, %%mm4)        /* mm4 = z | z */
            NIIASMt2(movq %%mm0, %%mm1)            /* mm1 = x | x */
            NIIASMt2(movq %%mm2, %%mm3)            /* mm3 = y | y */
            NIIASMt2(movq %%mm4, %%mm5)            /* mm5 = z | z */
            NIIASMt2(pfmul   (%1), %%mm0)            /* mm0 = x*m[1]  | x*m[0] */
            NIIASMt2(pfmul  8(%1), %%mm1)            /* mm1 = x*m[3]  | x*m[2] */
            NIIASMt2(pfmul 16(%1), %%mm2)         /* mm2 = y*m[5]  | y*m[4] */
            NIIASMt2(pfmul 24(%1), %%mm3)         /* mm3 = y*m[7]  | y*m[6] */
            NIIASMt2(pfmul 32(%1), %%mm4)         /* mm4 = z*m[9]  | z*m[8] */
            NIIASMt2(pfmul 40(%1), %%mm5)         /* mm5 = z*m[11] | z*m[10] */
            NIIASMt2(pfadd 48(%1), %%mm4)         /* mm4 = z*m[9] + m[13]  | z*m[8] + m[12] */
            NIIASMt2(pfadd 56(%1), %%mm5)            /* mm5 = z*m[11] + m[15] | z*m[10] + m[14] */

            /* Sum it... (not displaying register contents.... */

            /* Combine X and Y column into mm2-3 */
            NIIASMt2(pfadd %%mm0, %%mm2)
            NIIASMt2(pfadd %%mm1, %%mm3)

            /* Combine with ZW column into mm2-3 */
            NIIASMt2(pfadd %%mm4, %%mm2)
            NIIASMt2(pfadd %%mm5, %%mm3)

            NIIASMt2(movq %%mm2,  (%2))
            NIIASMt2(movq %%mm3, 8(%2))
            :
            : NIIASMc(r) (v), NIIASMc(r) (this), NIIASMc(r) (re)
            }

            /* Execute 'femms' if desired */
            #ifndef NII_NO_EMMS
            __asm{NIIASMt1(femms)}
            #endif
    #endif
            return (*re);        
        }
        /***/
        inline Vector4<NIIf> operator * (const Vector4<NIIf> * v) const
        {
            Vector4<NIIf> *re;
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
            NIIASMt2(movaps (%1), %%xmm4)
            NIIASMt2(movaps %%xmm4, %%xmm5)
            NIIASMt2(movaps %%xmm4, %%xmm6)
            NIIASMt2(movaps  %%xmm4, %%xmm7)
            NIIASMt2(shufps $0x00, %%xmm4, %%xmm4)
            NIIASMt2(shufps $0x55, %%xmm5, %%xmm5)
            NIIASMt2(shufps $0xAA, %%xmm6, %%xmm6)
            NIIASMt2(shufps $0xFF, %%xmm7, %%xmm7)

            /* Multiply with each row */
            NIIASMt2(mulps   (%0), %%xmm4)
            NIIASMt2(mulps 16(%0), %%xmm5)
            NIIASMt2(mulps 32(%0), %%xmm6)
            NIIASMt2(mulps 48(%0), %%xmm7)

            /* Sum results */
            NIIASMt2(addps %%xmm4, %%xmm5)
            NIIASMt2(addps %%xmm6, %%xmm7)
            NIIASMt2(addps %%xmm5, %%xmm7)

            /* Store translated vector */
            NIIASMt2(movaps %%xmm7, (%2))
            :
            : NIIASMc(r) (this), NIIASMc(r) (v), NIIASMc(r) (re)
            }        
    #elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
            __asm{
            NIIASMt2(movd   (%0), %%mm0)            /* mm0 = ? | x */
            NIIASMt2(movd  4(%0), %%mm2)            /* mm2 = ? | y */
            NIIASMt2(movd  8(%0), %%mm4)            /* mm4 = ? | z */
            NIIASMt2(movd 12(%0), %%mm6)            /* mm6 = ? | w */
            NIIASMt2(punpckldq %%mm0, %%mm0)        /* mm0 = x | x */
            NIIASMt2(punpckldq %%mm2, %%mm2)        /* mm2 = y | y */
            NIIASMt2(punpckldq %%mm4, %%mm4)        /* mm4 = z | z */
            NIIASMt2(punpckldq %%mm6, %%mm6)        /* mm6 = w | w */
            NIIASMt2(movq %%mm0, %%mm1)            /* mm1 = x | x */
            NIIASMt2(movq %%mm2, %%mm3)            /* mm3 = y | y */
            NIIASMt2(movq %%mm4, %%mm5)            /* mm5 = z | z */
            NIIASMt2(movq %%mm6, %%mm7)            /* mm7 = w | w */
            NIIASMt2(pfmul   (%1), %%mm0)            /* mm0 = x*m[1]  | x*m[0] */
            NIIASMt2(pfmul  8(%1), %%mm1)            /* mm1 = x*m[3]  | x*m[2] */
            NIIASMt2(pfmul 16(%1), %%mm2)         /* mm2 = y*m[5]  | y*m[4] */
            NIIASMt2(pfmul 24(%1), %%mm3)         /* mm3 = y*m[7]  | y*m[6] */
            NIIASMt2(pfmul 32(%1), %%mm4)         /* mm4 = z*m[9]  | z*m[8] */
            NIIASMt2(pfmul 40(%1), %%mm5)         /* mm5 = z*m[11] | z*m[10] */
            NIIASMt2(pfmul 48(%1), %%mm6)         /* mm6 = w*m[13] | w*m[12] */
            NIIASMt2(pfmul 56(%1), %%mm7)         /* mm7 = w*m[15] | w*m[14] */
            NIIASMt2(pfadd %%mm0, %%mm2)
            NIIASMt2(pfadd %%mm1, %%mm3)
            NIIASMt2(pfadd %%mm4, %%mm6)
            NIIASMt2(pfadd %%mm5, %%mm7)
            NIIASMt2(pfadd %%mm2, %%mm6)
            NIIASMt2(pfadd %%mm3, %%mm7)
            NIIASMt2(movq %%mm6,  (%2)\n)
            NIIASMt2(movq %%mm7, 8(%2)\n)
            :
            : NIIASMc(r) (v), NIIASMc(r) (this), NIIASMc(r) (re)
            }

            /* Execute 'femms' if desired */
            #ifndef NII_NO_EMMS
            __asm{NIIASMt2(femms)}
            #endif        
    #endif
            return (*re);        
        }
        
        /**转置矩针
        */
        inline void Transpose()
        {
    #if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
            __asm{
            NIIASMt2(movlps      (%0), %%xmm1)
            NIIASMt2(movlps     8(%0), %%xmm3)
            NIIASMt2(movhps    16(%0), %%xmm1)
            NIIASMt2(movhps    24(%0), %%xmm3)
            NIIASMt2(movlps    32(%0), %%xmm5)
            NIIASMt2(movlps    40(%0), %%xmm4)
            NIIASMt2(movhps    48(%0), %%xmm5)
            NIIASMt2(movhps    56(%0), %%xmm4)
            NIIASMt2(movaps    %%xmm1, %%xmm0)
            NIIASMt2(movaps    %%xmm3, %%xmm2)
            NIIASMt2(shufps    $0xDD, %%xmm5,%%xmm1)
            NIIASMt2(shufps    $0xDD, %%xmm4,%%xmm3)
            NIIASMt2(shufps    $0x88, %%xmm5,%%xmm0)
            NIIASMt2(shufps    $0x88, %%xmm4,%%xmm2)
            NIIASMt2(movaps %%xmm0,   (%0)\n)
            NIIASMt2(movaps %%xmm1, 16(%0)\n)
            NIIASMt2(movaps %%xmm2, 32(%0)\n)
            NIIASMt2(movaps %%xmm3, 48(%0)\n)
            :
            : NIIASMc(r) (this)
            }
    #endif            
        }
    };
}
#endif