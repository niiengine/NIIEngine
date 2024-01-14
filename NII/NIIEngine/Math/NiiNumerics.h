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

#ifndef _NII_NumericSolvers_H_
#define _NII_NumericSolvers_H_

#include "NiiPreInclude.h"

namespace NII
{
    typedef NIId PreciseReal;

    /** 为提Nii供数值解算
    @remark 手动解算线性代数
    */
    class _EngineAPI NumericSolver
    {
    public:
        /** 解决了一个nxn的线性方程组满秩
        @remark
            这个实现一个质朴的高斯消元算法.这个算法消极的,因此它在coeff 和 col中有副作用
        */
        static bool solveNxNLinearSysDestr(NIIi n, PreciseReal ** coeff, PreciseReal * col);
    };
}
#endif