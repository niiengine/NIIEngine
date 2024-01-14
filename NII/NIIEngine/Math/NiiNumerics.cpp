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
#include "NiiNumerics.h"

#include "NiiCommon.h"

namespace NII
{
    //---------------------------------------------------------------------------------------
    bool NumericSolver::solveNxNLinearSysDestr(NIIi n, PreciseReal **coeff, PreciseReal *col)
    {
        // we'll use standard row reduction; since we only care about systems with unique
        // solutions, our job is slightly easier.  more can probably be done later to improve
        // precision versus this naive method

        NIIi i, j;

        for(j=0; j<n; j++)
        {
            // look for a row with a leading coefficient we can use to cancel the rest
            NIIi nonzeroIndex = -1;
            for(i=j; i<n; i++) {
                if (coeff[i][j] != 0.0) {
                    nonzeroIndex = i;
                    break;
                }
            }
            if (nonzeroIndex < 0) 
                return false;
            PreciseReal *tptr = coeff[j];
            coeff[j] = coeff[nonzeroIndex];
            coeff[nonzeroIndex] = tptr;
            PreciseReal tval = col[j];
            col[j] = col[nonzeroIndex];
            col[nonzeroIndex] = tval;
            nonzeroIndex = j;

            // normalize row to have leading coeff of 1 and kill other rows' entries
            PreciseReal invelt = 1.0 / coeff[nonzeroIndex][j];
            NIIi k;
            for (k=j; k<n; k++)
                coeff[nonzeroIndex][k] *= invelt;
            col[nonzeroIndex] *= invelt;
            for (i=0; i<n; i++) {
                if (i==nonzeroIndex || coeff[i][j] == 0.0)
                    continue;
                PreciseReal temp = coeff[i][j];
                for (k=j; k<n; k++)
                    coeff[i][k] -= temp * coeff[nonzeroIndex][k];
                col[i] -= temp * col[nonzeroIndex];
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------------------
}