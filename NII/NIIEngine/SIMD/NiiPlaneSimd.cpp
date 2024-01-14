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
#include "NiiPlane.h"
#include "NiiMatrix3.h"

namespace NII
{
    //--------------------------------------------------------------------------------------------------
    Plane::Plane (const Vector3Simd::PolygonSimd * source)
    {
#if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
        NIIASM((
        NIIASMt((movups   (%0), %%xmm0))    /* pPoly->a */
        NIIASMt((movups 16(%0), %%xmm1))    /* pPoly->b */
        NIIASMt((movups 32(%0), %%xmm2))    /* pPoly->c */
        NIIASMt((movaps %%xmm0, %%xmm7))   /* Save 'a' into xmm7 */
        NIIASMt((subps %%xmm1, %%xmm0))
        NIIASMt((subps %%xmm1, %%xmm2))

        /* Now, just need the cross product of xmm0 and xmm2 */
        NIIASMt((movaps %%xmm0, %%xmm1))    /* xmm0 = xmm1 = Left */
        NIIASMt((movaps %%xmm2, %%xmm3))    /* xmm2 = xmm3 = Right */
        NIIASMt((shufps $0xC9, %%xmm0, %%xmm0))    /* Left.yxz */
        NIIASMt((shufps $0xD2, %%xmm1, %%xmm1))    /* Left.xzy */
        NIIASMt((shufps $0xD2, %%xmm2, %%xmm2))    /* Right.xzy */
        NIIASMt((shufps $0xC9, %%xmm3, %%xmm3))    /* Right.yxz */

        /* Multiply columns 1&2 and 3&4 */
        NIIASMt((mulps %%xmm0, %%xmm2))
        NIIASMt((mulps %%xmm1, %%xmm3))

        /* Got the cross product, OK */        
        NIIASMt((subps %%xmm3, %%xmm2))

        /* Begin calculation of 'd' component */


        /* AND off bits 96-127 (w component) */    
        NIIASMt((andps SSE_NO_W_MASK, %%xmm2))

        /* save xmm4 = 0 | z | y | x */
        NIIASMt((movaps %%xmm2, %%xmm4))

        /* Multiply with point 'a' on the polygon (saved in xmm7): xmm2 = 0 | a.z*z | a.y*y | a.x*x */
        NIIASMt((mulps %%xmm7, %%xmm2))

    #if NII_PLAMODE == 3

        /* horizontally add, xmm2 = x^2+y^2+z^2 | x^2+y^2+z^2 | x^2+y^2+z^2 | x^2+y^2+z^2  */
        NIIASMt((haddps %%xmm2, %%xmm2))
        NIIASMt((haddps %%xmm2, %%xmm2))
        
        /* change sign for top component, remove bits for others */
        NIIASMt((xorps _SIMDx86_float_POSPOSPOSNEG, %%xmm2))        /* xmm2 = -(x^2+y^2+z^2) | .... | ...*/
        NIIASMt((andps _SIMDx86_float_SSE_NO_XYZ_MASK, %%xmm2))        /* xmm2 = -(x^2+y^2+z^2) | 0 | 0 | 0 */
        NIIASMt((orps %%xmm2, %%xmm4))                            /* xmm4 = -(x^2+y^2+z^2) | z | y | x */

    #else

        NIIASMt((movhlps %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | 0   |  z^2 */
        NIIASMt((addss %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | 0   | z^2 + x^2 */
        NIIASMt((shufps $0x55, %%xmm2, %%xmm2))/* xmm2 = y^2 | y^2 | y^2 | y^2 */
        NIIASMt((addss %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | ?   | x^2+y^2+z^2 */

        /* Change sign */
        NIIASMt((xorps _SIMDx86_float_NEGPOSPOSPOS, %%xmm3)) /* xmm3 = ? | ? | ? | -(x^2+y^2+z^2)*/

        /* Move to w component location, mask off xyz, and OR with saved portions */
        NIIASMt((shufps $0x00, %%xmm3, %%xmm3))                /* xmm3 = -(x^2+y^2+z^2) | -(x^2+y^2+z^2) | -(x^2+y^2+z^2) | -(x^2+y^2+z^2)*/
        NIIASMt((andps _SIMDx86_float_SSE_NO_XYZ_MASK, %%xmm3)) /* xmm3 = -(x^2+y^2+z^2) | 0 | 0 | 0 */
        NIIASMt((orps %%xmm3, %%xmm4))                            /* xmm4 = -(x^2+y^2+z^2) | z | y | x */

    #endif /* Not using SSE3 */

        /* Save plane coefficients */
        NIIASMt((movups %%xmm4, (%1)))
        :
        : NIIASMc(r) (source), NIIASMc(r) (this)
        ))        
#endif
    }
    //---------------------------------------------------------------------------
    Plane::Plane (const Vector3Simde *  rkPoint0, const Vector3Simde *    rkPoint1,const Vector3Simde *rkPoint2)
    {
#if NII_PLAMODE >= 1&&     NII_PLAMODE <= 4
        NIIASM((
        NIIASMt((movups (%0), %%xmm0))    /* pA */
        NIIASMt((movups (%1), %%xmm1))    /* pB */
        NIIASMt((movups (%2), %%xmm2))    /* pC */
        NIIASMt((movaps %%xmm0, %%xmm7))   /* Save 'a' into xmm7 */
        NIIASMt((subps %%xmm1, %%xmm0))
        NIIASMt((subps %%xmm1, %%xmm2))

        /* Now, just need the cross product of xmm0 and xmm2 */
        NIIASMt((movaps %%xmm0, %%xmm1))    /* xmm0 = xmm1 = Left */
        NIIASMt((movaps %%xmm2, %%xmm3))    /* xmm2 = xmm3 = Right */
        NIIASMt((shufps $0xC9, %%xmm0, %%xmm0))    /* Left.yxz */
        NIIASMt((shufps $0xD2, %%xmm1, %%xmm1))    /* Left.xzy */
        NIIASMt((shufps $0xD2, %%xmm2, %%xmm2))    /* Right.xzy */
        NIIASMt((shufps $0xC9, %%xmm3, %%xmm3))    /* Right.yxz */

        /* Multiply columns 1&2 and 3&4 */
        NIIASMt((mulps %%xmm0, %%xmm2))
        NIIASMt((mulps %%xmm1, %%xmm3))

        /* Got the cross product, OK */
        NIIASMt((subps %%xmm3, %%xmm2))

        /* Begin calculation of 'd' component */


        /* AND off bits 96-127 (w component) */
        NIIASMt((andps _SIMDx86_float_SSE_NO_W_MASK, %%xmm2))

        /* save xmm4 = 0 | z | y | x */
        NIIASMt((movaps %%xmm2, %%xmm4))

        /* Multiply with point 'a' on the polygon (saved in xmm7): xmm2 = 0 | a.z*z | a.y*y | a.x*x */
        NIIASMt((mulps %%xmm7, %%xmm2))

    #if NII_PLAMODE == 3

        /* horizontally add, xmm2 = x^2+y^2+z^2 | x^2+y^2+z^2 | x^2+y^2+z^2 | x^2+y^2+z^2  */
        NIIASMt((haddps %%xmm2, %%xmm2))
        NIIASMt((haddps %%xmm2, %%xmm2))

        /* change sign for top component, remove bits for others */
        NIIASMt((xorps _SIMDx86_float_POSPOSPOSNEG, %%xmm2))        /* xmm2 = -(x^2+y^2+z^2) | .... | ...*/
        NIIASMt((andps _SIMDx86_float_SSE_NO_XYZ_MASK, %%xmm2))        /* xmm2 = -(x^2+y^2+z^2) | 0 | 0 | 0 */
        NIIASMt((orps %%xmm2, %%xmm4))                            /* xmm4 = -(x^2+y^2+z^2) | z | y | x */

    #else

        NIIASMt((movhlps %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | 0   |  z^2 */
        NIIASMt((addss %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | 0   | z^2 + x^2 */
        NIIASMt((shufps $0x55, %%xmm2, %%xmm2))    /* xmm2 = y^2 | y^2 | y^2 | y^2 */
        NIIASMt((addss %%xmm2, %%xmm3))        /* xmm3 = ?   | ?   | ?   | x^2+y^2+z^2 */

        /* Change sign */
        NIIASMt((xorps _SIMDx86_float_NEGPOSPOSPOS, %%xmm3)) /* xmm3 = ? | ? | ? | -(x^2+y^2+z^2)*/

        /* Move to w component location, mask off xyz, and OR with saved portions */
        NIIASMt((shufps $0x00, %%xmm3, %%xmm3))                /* xmm3 = -(x^2+y^2+z^2) | -(x^2+y^2+z^2) | -(x^2+y^2+z^2) | -(x^2+y^2+z^2)*/
        NIIASMt((andps _SIMDx86_float_SSE_NO_XYZ_MASK, %%xmm3)) /* xmm3 = -(x^2+y^2+z^2) | 0 | 0 | 0 */
        NIIASMt((orps %%xmm3, %%xmm4))                            /* xmm4 = -(x^2+y^2+z^2) | z | y | x */

    #endif /* Not using SSE3 */

        /* Save plane coefficients */
        NIIASMt((movups %%xmm4, (%3)))
        :
        : NIIASMc(r) (rkPoint0), NIIASMc(r) (rkPoint1), NIIASMc(r) (rkPoint2), NIIASMc(r) (this)
        ))

#endif    
    }
    //--------------------------------------------------------------------------------------------------  
    NIIf Plane::distance (const Vector3Simd * rkPoint) const
    {
    #if SSE
        NIIf dummy;
        NIIASM((
        NIIASMt((movups (%1), %%xmm0)) /* xmm0 = pPlane->d | pPlane->c | pPlane->b | pPlane->a */
        NIIASMt((movups (%2), %%xmm1)) /* xmm1 = ????????? | pPoint->z | pPoint->y | pPoint->x */
        
        NIIASMt((andps SIMDx86_float_SSE_NO_W_MASK, %%xmm1))   /* xmm1 = 0 | ... */
        NIIASMt((movaps %%xmm0, %%xmm7))                        /* xmm7 = pPlane... */

        NIIASMt((mulps %%xmm0, %%xmm1))                        /* xmm1 = d*0.0 | c*z | b*y | a*x */
        NIIASMt((shufps $0xFF, %%xmm7, %%xmm7))                /* xmm7 = d | d | d | d */
        
        #if SSE == 3
        NIIASMt((haddps %%xmm1, %%xmm1))                /* xmm1 = ... | ... | z*c + 0 | x*a + y*b */
        NIIASMt((haddps %%xmm1, %%xmm1))               /* xmm1 = ... | ... | dot(pPlane, pPoint) */
        NIIASMt((andps _SIMDx86_float_ABS, %%xmm1))    /* xmm1 = ??? | ??? | ??? | fabsf(dot(pPlane, pPoint)) */
        NIIASMt((addss %%xmm7, %%xmm1))                /* xmm1 = ? | ? | ? | fabsf(dot(pPlane, pPoint)) + pPlane->d */
        #else
        NIIASMt((movhlps %%xmm1, %%xmm2))      /* xmm2 = ???? | ???? | d*0.0 | z*c */
        NIIASMt((addss %%xmm1, %%xmm2))        /* xmm2 = ???? | ???? | ????  | x*a + z*c*/
        NIIASMt((shufps $x55, %%xmm1, %%xmm1)) /* xmm1 = ???? | ???? | ????  | y*b */
        NIIASMt((andps _SIMDx86_float_ABS, %%xmm1))    /* xmm1 = ??? | ??? | ??? | fabsf(dot(pPlane, pPoint)) */
        NIIASMt((addss %%xmm2, %%xmm1))        /* xmm1 = ???? | ???? | ????? |  fabsf(dot(pPlane, pPoint)) + pPlane->d */
        #endif
        NIIASMt((movss %%xmm1, -4(%%esp)))
        NIIASMt((flds -4(%%esp)))

        : NIIASMc((=t)) (dummy)
        : NIIASMc(r) (this), NIIASMc(r) (rkPoint)
        ))
    #endif
    }
    //--------------------------------------------------------------------------------------------------
}