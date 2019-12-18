/*
-----------------------------------------------------------------------------
大型多媒体框架
时间: 2018-5-7
文本编码: utf-8
所属公司: 深圳闽登科技有限公司
命名风格: 概论命名法
编程风格: 统筹式
管理模式: 分布式
内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)
主要成分: c++(80%) c(20%)
用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件
偏向用途: 立体游戏软件
主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com
------------------------------------------------------------------------------
*/
/*
Copyright (c) "2018-2019", Shenzhen Mindeng Technology Co., Ltd(www.niiengine.com),
		Mindeng Base Communication Application Framework
All rights reserved.
	Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
	Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.
	Neither the name of the "ORGANIZATION" nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior written
permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NII_ASM_math_H__
#define _NII_ASM_math_H__

#include "NiiPreInclude.h"

#if  N_COMPILER == N_CPP_MSVC
    #pragma warning (push)
    #pragma warning (disable : 4725)
#endif
namespace NII
{
    const NIIf pi = 4.0f * atan(1.0f);
    const NIIf half_pi = 0.5f * pi;
#if  N_COMPILER == N_CPP_MSVC && NII_ARCH_TYPE == N_ARCH_32
    #pragma warning( push )
    #pragma warning( disable: 4035 ) 
#endif

    NIIf asm_arccos(NIIf r) 
    {
        // return half_pi + arctan( r / -sqr( 1.f - r * r ) );
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32

        NIIf asm_one = 1.f;
        NIIf asm_half_pi = half_pi;
        __asm {
            fld r // r0 = r
            fld r // r1 = r0, r0 = r
            fmul r // r0 = r0 * r
            fsubr asm_one // r0 = r0 - 1.f
            fsqrt // r0 = sqrtf( r0 )
            fchs // r0 = - r0
            fdiv // r0 = r1 / r0
            fld1 // {{ r0 = atan( r0 )
            fpatan // }}
            fadd asm_half_pi // r0 = r0 + pi / 2
        } // returns r0
#else
        return NIIf(acos(r));
#endif
    }

    NIIf asm_arcsin(NIIf r) 
    {
        // return arctan( r / sqr( 1.f - r * r ) );
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32

        const NIIf asm_one = 1.f;
        __asm {
            fld r // r0 = r
            fld r // r1 = r0, r0 = r
            fmul r // r0 = r0 * r
            fsubr asm_one // r0 = r0 - 1.f
            fsqrt // r0 = sqrtf( r0 )
            fdiv // r0 = r1 / r0
            fld1 // {{ r0 = atan( r0 )
            fpatan // }}
        } // returns r0
#else
        return NIIf(asin(r));
#endif
    }

    NIIf asm_arctan(NIIf r) 
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        __asm {
            fld r // r0 = r
            fld1 // {{ r0 = atan( r0 )
            fpatan // }}
        } // returns r0
#else
        return NIIf(atan(r));
#endif
    }

    NIIf asm_sin(NIIf r) 
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        __asm {
            fld r // r0 = r
            fsin // r0 = sinf( r0 )
        } // returns r0
#else
        return sin(r);
#endif
    }

    NIIf asm_cos(NIIf r) 
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        __asm {
            fld r // r0 = r
            fcos // r0 = cosf( r0 )
        } // returns r0
#else
        return cos(r);
#endif
    }

    NIIf asm_tan(NIIf r) 
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32

        // return sin( r ) / cos( r );
        __asm {
            fld r // r0 = r
            fsin // r0 = sinf( r0 )
            fld r // r1 = r0, r0 = r
            fcos // r0 = cosf( r0 )
            fdiv // r0 = r1 / r0
        } // returns r0
#else
        return tan(r);
#endif
    }

    // returns a for a * a = r
    NIIf asm_sqrt(NIIf r)
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        __asm {
            fld r // r0 = r
            fsqrt // r0 = sqrtf( r0 )
        } // returns r0
#else
        return sqrtf(r);
#endif
    }

    // returns 1 / a for a * a = r
    // -- Use this for Vector normalisation!!!
    NIIf asm_rsq(NIIf r)
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        __asm {
            fld1 // r0 = 1.f
            fld r // r1 = r0, r0 = r
            fsqrt // r0 = sqrtf( r0 )
            fdiv // r0 = r1 / r0
        } // returns r0
#else
        return 1. / sqrtf(r);
#endif
    }

    // returns 1 / a for a * a = r
    // Another version
    NIIf apx_rsq(NIIf r) 
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
        const NIIf asm_dot5 = 0.5f;
        const NIIf asm_1dot5 = 1.5f;
        __asm {
            fld r // r0 = r
            fmul asm_dot5 // r0 = r0 * .5f
            mov eax, r // eax = r
            shr eax, 0x1 // eax = eax >> 1
            neg eax // eax = -eax
            add eax, 0x5F400000 // eax = eax & MAGICAL NUMBER
            mov r, eax // r = eax
            fmul r // r0 = r0 * r
            fmul r // r0 = r0 * r
            fsubr asm_1dot5 // r0 = 1.5f - r0
            fmul r // r0 = r0 * r
        } // returns r0
#else
        return 1. / sqrtf(r);
#endif
    }

    /* very MS-specific, commented out for now
       Finally the best invSqrt implementation?
       Use for vector normalisation instead of 1/length() * x,y,z
    */
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
    __declspec(naked) NIIf __fastcall invSqrt(NIIf fValue)
    {
        __asm
        {
            mov  eax, 0be6eb508h
            mov  dword ptr[esp-12],03fc00000h
            sub  eax, dword ptr[esp + 4]
            sub  dword ptr[esp+4], 800000h
            shr  eax, 1
            mov  dword ptr[esp -  8], eax

            fld  dword ptr[esp -  8]
            fmul st, st
            fld  dword ptr[esp -  8]
            fxch st(1)
            fmul dword ptr[esp +  4]
            fld  dword ptr[esp - 12]
            fld  st(0)
            fsub st,st(2)

            fld  st(1)
            fxch st(1)
            fmul st(3),st
            fmul st(3),st
            fmulp st(4),st
            fsub  st,st(2)

            fmul  st(2),st
            fmul  st(3),st
            fmulp st(2),st
            fxch  st(1)
            fsubp st(1),st

            fmulp st(1), st
            ret 4
        }
    }
#endif

    // returns a random number
    FORCEINLINE NIIf asm_rand()
    {
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
    #if 0
        #if NII_COMP_VER >= 1300

        static unsigned __int64 q = time( NULL );

        _asm {
            movq mm0, q

            // do the magic MMX thing
            pshufw mm1, mm0, 0x1E
            paddd mm0, mm1

            // move to integer memory location and free MMX
            movq q, mm0
            emms
        }

        return NIIf( q );
        #endif
    #else
        // VC6 does not support pshufw
        return NIIf(rand());
    #endif
#else
        // GCC etc
        return NIIf(rand());
#endif
    }

	// returns a random number
	FORCEINLINE NIIi asm_randi()
	{
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
#if 0
#if NII_COMP_VER >= 1300

		static unsigned __int64 q = time(NULL);

		_asm {
			movq mm0, q

			// do the magic MMX thing
			pshufw mm1, mm0, 0x1E
			paddd mm0, mm1

			// move to integer memory location and free MMX
			movq q, mm0
			emms
		}

		return q;
#endif
#else
		// VC6 does not support pshufw
		return rand();
#endif
#else
		// GCC etc
		return rand();
#endif
	}

    // returns the maximum random number
    FORCEINLINE NIIf asm_rand_max()
    {

    #if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
      #if 0
        #if NII_COMP_VER >= 1300

        return N_TYPE_MAX(unsigned __int64);
        return 9223372036854775807.0f;
        #endif
      #else
        // VC6 does not support unsigned __int64
        return NIIf(RAND_MAX);
      #endif

    #else
        // GCC etc
        return NIIf(RAND_MAX);

    #endif
    }

    // returns log2( r ) / log2( e )
    NIIf asm_ln(NIIf r) 
    {    
#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32

        const NIIf asm_1_div_log2_e = .693147180559f;
        const NIIf asm_neg1_div_3 = -.33333333333333333333333333333f;
        const NIIf asm_neg2_div_3 = -.66666666666666666666666666667f;
        const NIIf asm_2 = 2.f;

        NIIi log_2 = 0;

        __asm {
            // log_2 = ( ( r >> 0x17 ) & 0xFF ) - 0x80;
            mov eax, r
            sar eax, 0x17
            and eax, 0xFF
            sub eax, 0x80
            mov log_2, eax

            // r = ( r & 0x807fffff ) + 0x3f800000;
            mov ebx, r
            and ebx, 0x807FFFFF
            add ebx, 0x3F800000
            mov r, ebx

            // r = ( asm_neg1_div_3 * r + asm_2 ) * r + asm_neg2_div_3;   // (1)
            fld r
            fmul asm_neg1_div_3
            fadd asm_2
            fmul r
            fadd asm_neg2_div_3
            fild log_2
            fadd
            fmul asm_1_div_log2_e
        }
#else
        return log(r);
#endif
    }

#if  N_COMPILER == N_CPP_MSVC &&  NII_ARCH_TYPE == N_ARCH_32
#    pragma warning( pop )
#endif
} // namespace

#if  N_COMPILER == N_CPP_MSVC
#  pragma warning (pop)
#endif

#endif
