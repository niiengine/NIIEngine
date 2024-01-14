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
#include "NiiI_A.h"

namespace NII
{
    // Fast itoa from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html for Linux since it seems like Linux doesn't support this function.
    char * i_a(int in, char * r, int base)
     {
        // check that the base if valid
        if (base < 2 || base > 16)
        {
            *r = 0;
            return r;
        }
        char * out = r;
        int quotient = in;

        int absQModB;

        do
        {
            // KevinJ - get rid of this dependency
            //*out = "0123456789abcdef"[ std::abs( quotient % base ) ];
            absQModB=quotient % base;
            if (absQModB < 0)
                absQModB=-absQModB;
            *out = "0123456789abcdef"[ absQModB ];
            ++out;
            quotient /= base;
        } while ( quotient );

        // Only apply negative sign for base 10
        if (in < 0 && base == 10)
            *out++ = '-';

        // KevinJ - get rid of this dependency
        // std::reverse( result, out );
        *out = 0;

        // KevinJ - My own reverse code
        char *start = r;
        char temp;
        out--;
        while (start < out)
        {
            temp=*start;
            *start=*out;
            *out=temp;
            start++;
            out--;
        }

        return r;
    }
}