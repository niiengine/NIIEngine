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
    //--------------------------------------------------------------------
    Quaternion Quaternion::operator* (const Quaternion& rkQ) const
    {
#if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
        Quaternion *re;
        NIIASM((
        NIIASMt((movups (%0), %%xmm0))     /* xmm0 = Rw | Rz | Ry | Rx */
        NIIASMt((movups (%1), %%xmm4))     /* xmm1 = Lw | Lz | Ly | Lx */


        /* Duplicate Right throughout xmm0-xmm3 */
        NIIASMt((movaps %%xmm0, %%xmm1))    /* xmm1 = xmm0 */
        NIIASMt((movaps %%xmm0, %%xmm2))    /* xmm2 = xmm0 */
        NIIASMt((movaps %%xmm0, %%xmm3))    /* xmm3 = xmm0 */

        /* Duplicate Left throughout xmm4-xmm7 */
        NIIASMt((movaps %%xmm4, %%xmm5))    /* xmm5 = xmm4 */
        NIIASMt((movaps %%xmm4, %%xmm6))    /* xmm6 = xmm4 */
        NIIASMt((movaps %%xmm4, %%xmm7))    /* xmm7 = xmm4 */

        /*
            Broadcast elements in xmm4-xmm7 to create scalar registers
            ==================
            0000 0000 = xxxx = 0x00
            0101 0101 = yyyy = 0x55
            1010 1010 = zzzz = 0xAA
            1111 1111 = wwww = 0xFF
        */
        NIIASMt((shufps $0x00, %%xmm4, %%xmm4))        /* xmm4 = Rx | Rx | Rx | Rx */
        NIIASMt((shufps $0x55, %%xmm5, %%xmm5))        /* xmm5 = Ry | Ry | Ry | Ry */
        NIIASMt((shufps $0xAA, %%xmm6, %%xmm6))        /* xmm6 = Rz | Rz | Rz | Rz */
        NIIASMt((shufps $0xFF, %%xmm7, %%xmm7))        /* xmm7 = Rw | Rw | Rw | Rw */

        /*
            Set up columns
            ==============
            C1 = w | z | y | x = 1110 0100 =
            C2 = x | y | z | w = 0001 1011 = 0x1B
            C3 = y | x | w | z = 0100 1110 = 0x4E
            C4 = z | w | x | y = 1011 0001 = 0xB1
        */

        /* C1 is already w | z | y | x  format, no shufps needed */
        NIIASMt((shufps $0x1B, %%xmm1, %%xmm1))
        NIIASMt((shufps $0x4E, %%xmm2, %%xmm2))
        NIIASMt((shufps $0xB1, %%xmm3, %%xmm3))

        /* Multiply columns */
        NIIASMt((mulps %%xmm0, %%xmm7))        /* C1 *= Lw */
        NIIASMt((mulps %%xmm1, %%xmm4))        /* C2 *= Lx */
        NIIASMt((mulps %%xmm2, %%xmm5))        /* C3 *= Ly */
        NIIASMt((mulps %%xmm3, %%xmm6))        /* C4 *= Lz */

        /* Change the signs of the columns (C1, aka: xmm4, doesnt need it)*/
        NIIASMt((xorps _POSNEGPOSNEG, %%xmm4))        /* C2 = { + - + - } */
        NIIASMt((xorps _POSPOSNEGNEG, %%xmm5))        /* C3 = { + + - - } */
        NIIASMt((xorps _NEGPOSPOSNEG, %%xmm6))        /* C4 = { - + + - } */

        NIIASMt((addps %%xmm4, %%xmm5))        /* C2 += C1 */
        NIIASMt((addps %%xmm6, %%xmm7))        /* C4 += C3 */
        NIIASMt((addps %%xmm5, %%xmm7))        /* C4 += C2 */

        NIIASMt((movups %%xmm7, (%2)))            /* xmm7 = new quaternion, write it out */
        :
        : NIIASMc(r) (rkQ), NIIASMc(r) (this), NIIASMc(r) (re)
        ))
#elif NII_PLAMODE ==6
        Quaternion *re;
        NIIASM((
        /* Set up mm0 and mm1 as second column RIGHT */
        NIIASMt((pswapd  (%0), %%mm1))        /* mm1 = Rx | Ry */
        NIIASMt((pswapd 8(%0), %%mm0))        /* mm0 = Rz | Rw */

        /* Get the scalars */
        NIIASMt((movq  (%1), %%mm2))        /* mm2 = Ly | Lx */
        NIIASMt((movq 8(%1), %%mm7))        /* mm7 = Lw | Lz */

        /* Set up mm4 and mm5 as third column RIGHT */
        NIIASMt((pswapd %%mm0, %%mm4))        /* mm4 = Rw | Rz */
        NIIASMt((pswapd %%mm1, %%mm5))        /* mm5 = Ry | Rx */

        /* Copy Rz|Rw value from mm0 into mm6 for later */
        NIIASMt((movq %%mm0, %%mm6))

        /* Set up mm2 and mm3 as scalars */
        NIIASMt((movq %%mm2, %%mm3))        /* mm3 = mm2 = Ly | Lx */
        NIIASMt((punpckldq %%mm2, %%mm2))    /* mm2 = Lx | Lx */
        NIIASMt((punpckhdq %%mm3, %%mm3))    /* mm3 = Ly | Ly */

        /* Multiply Column 2 (X's, frees register mm2) */
        NIIASMt((pfmul %%mm2, %%mm0))
        NIIASMt((pfmul %%mm2, %%mm1))


        /* Change signs appropriately (+, -, +, -) */
        NIIASMt((pxor _SIMDx86_float_POSNEG, %%mm0))
        NIIASMt((pxor _SIMDx86_float_POSNEG, %%mm1))

        /* Move Rx|Ry value from mm5 to mm2 */
        NIIASMt((pswapd %%mm5, %%mm2))


        /* Multiply Colum 3 (Y's, frees register mm3) */
        NIIASMt((pfmul %%mm3, %%mm4))
        NIIASMt((pfmul %%mm3, %%mm5))

        /* Combine columns 2 and 3 (frees registers mm4 and mm5) */
        NIIASMt((pfadd %%mm4, %%mm0))
        NIIASMt((pfsub %%mm5, %%mm1))

        /*
            Current register status:
            ========================
            mm0 = C2+C3
            mm1 = C2+C3
            mm2 = Rx | Ry
            mm3 = free
            mm4 = free
            mm5 = free
            mm6 = Rz | Rw
            mm7 = Lw | Lz

            OK! This is current:
            -------------------
            Column 4 = mm2 | mm6


            This is needed:
            --------------
            Column 1 = mm3{swap(mm2)} | mm4{swap(mm6)}
            mm5 = Lz | Lz
            mm7 = Lw | Lw

            After that is done, just multiply, invert signs as needed, then add and store

        */

        /* Set up registeres */
        NIIASMt((pswapd %%mm2, %%mm3))        /* mm3 = Ry | Rx */
        NIIASMt((movq %%mm7, %%mm5))        /* mm5 = Lw | Lz */
        NIIASMt((pswapd %%mm6, %%mm4))        /* mm4 = Rw | Rz */
        NIIASMt((punpckhdq %%mm7, %%mm7))    /* mm7 = Lw | Lw */
        NIIASMt((punpckldq %%mm5, %%mm5))    /* mm5 = Lz | Lz */


        /* All registers set up! Lets bust out some fp-ops */

        /* Multiply w by first column */
        NIIASMt((pfmul %%mm7, %%mm3))
        NIIASMt((pfmul %%mm7, %%mm4))

        /* Multiply z by fourth column (remember 4th = { mm2 | mm6 } )*/
        NIIASMt((pfmul %%mm5, %%mm2))
        NIIASMt((pfmul %%mm5, %%mm6))

        /* Add results of C1 to C2+C3 */
        NIIASMt((pfadd %%mm3, %%mm0))
        NIIASMt((pfadd %%mm4, %%mm1))

        /* Change signs of fourth column to -, +, +, - */
        NIIASMt((pxor _SIMDx86_float_NEGPOS, %%mm2))
        NIIASMt((pxor _SIMDx86_float_POSNEG, %%mm6))

        /* Add fourth column. Result! */
        NIIASMt((pfadd %%mm2, %%mm0))
        NIIASMt((pfadd %%mm6, %%mm1))

        /* Store */
        NIIASMt((movq %%mm0,  (%2)))
        NIIASMt((movq %%mm1, 8(%2)))

        :
        : NIIASMc(r) (rkQ), NIIASMc(r) (this), NIIASMc(r) (re)
        ))
        /* 36 3DNow! Instructions (including movements) vs 70 x87 Instructions (-O3 -fomit-frame-pointer) */

        /* Execute 'femms' if desired */
        #ifndef NII_NO_EMMS
        NIIASM((NIIASMt(femms)))
        #endif
#endif         
    }
    //--------------------------------------------------------------------
    NIIf Quaternion::normalise(void)
    {
#if NII_PLAMODE == 1|| NII_PLAMODE == 2
        NIIASM((
        NIIASMt((movups (%0), %%xmm0))        /* xmm0 = w | z | y | x  */
        NIIASMt((movaps %%xmm0, %%xmm1))    /* xmm1 = w | z | y | x */
        NIIASMt((mulps %%xmm0, %%xmm0))    /* xmm0 = w*w | z*z | y*y | x*x */
        NIIASMt((movhlps %%xmm0, %%xmm2))    /* xmm2 = ? | ? | w*w | z*z */
        NIIASMt((addps %%xmm0, %%xmm2))    /* xmm2 = ? | ? | w*w+y*y | z*z+x*x */
        NIIASMt((movss %%xmm2, %%xmm0))    /* xmm0 = ? | ? | ? | z*z+x*x */
        NIIASMt((shufps $0x55, %%xmm2, %%xmm2))    /* xmm2 = w*w+y*y |  w*w+y*y |  w*w+y*y | w*w+y*y */
        NIIASMt((addss %%xmm0, %%xmm2))
    #ifdef HIPREC
        /* Full division by magnitude */
        NIIASMt((sqrtss %%xmm2, %%xmm2))
        NIIASMt((shufps $0x00, %%xmm2, %%xmm2))
        NIIASMt((divps %%xmm2, %%xmm1))
        NIIASMt((movups %%xmm1, (%0)))
    #else
        /* Multiply by reciprocal root approximation */
        NIIASMt((rsqrtss %%xmm2, %%xmm2))
        NIIASMt((shufps $0x00, %%xmm2, %%xmm2))
        NIIASMt((mulps %%xmm2, %%xmm1))
        NIIASMt((movups %%xmm1, (%0)))
    #endif
        :
        : NIIASMc(r) (this)
        ))
#elseif NII_PLAMODE == 3
        NIIASM((
        NIIASMt((movups (%0), %%xmm0))            /* xmm0 = w | z | y | x  */
        NIIASMt((movaps %%xmm0, %%xmm1))        /* xmm1 = xmm0 */
        NIIASMt((mulps %%xmm0, %%xmm0))        /* xmm0 = w*w | z*z | y*y | x*x */
        NIIASMt((haddps %%xmm0, %%xmm0))        /* xmm0 = */
        NIIASMt((haddps %%xmm0, %%xmm0))        /* xmm0 = magsq | magsq | magsq | magsq */
        NIIASMt((rsqrtps %%xmm0, %%xmm0))        /* xmm0 = rcp(mag) | rcp(mag) | rcp(mag) | rcp(mag) */
        NIIASMt((mulps %%xmm0, %%xmm1))
        NIIASMt((movups %%xmm1, (%0)))
        :
        : NIIASMc(r) (this)
        ))
#elif NII_PLAMODE >= 5 && NII_PLAMODE <=6
        NIIASM((
        NIIASMt((movq  (%0), %%mm0))        /* mm0 = y | x */
        NIIASMt((movq 8(%0), %%mm1))        /* mm1 = w | z */
        NIIASMt((movq %%mm0, %%mm2))        /* mm2 = y | x */
        NIIASMt((movq %%mm1, %%mm3))        /* mm3 = w | z */
        NIIASMt((pfmul %%mm0, %%mm0))        /* y*y | x*x */
        NIIASMt((pfmul %%mm1, %%mm1))        /* w*w | z*z */
        NIIASMt((pfadd %%mm1, %%mm0))        /* y*y + w*w | z*z + x*x */
        NIIASMt((pfacc %%mm0, %%mm0))        /* magsq | magsq */
        NIIASMt((pfrsqrt %%mm0, %%mm1))    /* mm1 = rcp(mag) | rcp(mag) (approx) */

    #ifdef HIPREC /* High precision */
        NIIASMt((movq %%mm1, %%mm4))        /* mm4 = mm1 = x0 = rcp(mag) */
        NIIASMt((pfmul %%mm1, %%mm1))        /* mm1 = x1 = x0*x0 */
        NIIASMt((pfrsqit1 %%mm1, %%mm0))    /* mm1 = x2 = pfrsqit1(val, x1) */
        NIIASMt((pfrcpit2 %%mm4, %%mm0))    /* mm2 = 1/sqrt(mag*mag) = pfrcpit2(x2, x0)*/
        NIIASMt((pfmul %%mm0, %%mm2))
        NIIASMt((pfmul %%mm0, %%mm3))

    #else /* Low precision */
        NIIASMt((pfmul %%mm1, %%mm2))
        NIIASMt((pfmul %%mm1, %%mm3))
    #endif
        NIIASMt((movq %%mm2,  (%0)))
        NIIASMt((movq %%mm3, 8(%0)))
        :
        : NIIASMc(r) (this)
        ))

        /* Execute FEMMS if desired */
        #ifndef NII_NO_EMMS
        NIIASM((NIIASMt((femms))))
        #endif
#endif        
    }
    //----------------------------------------------------------------------
}