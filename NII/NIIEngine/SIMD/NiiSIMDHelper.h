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

#ifndef _NII_SIMDHelper_H_
#define _NII_SIMDHelper_H_

#include "NiiPreInclude.h"
#include "NiiPlatInfo.h"

// 堆栈对齐
//
// If macro __NII_SIMD_ALIGN_STACK defined, means there requests
// special code to ensure stack align to a 16-bytes boundary.
//
// Note:
//   This macro can only guarantee callee stack pointer (esp) align
// to a 16-bytes boundary, but not that for frame pointer (ebp).
// Because most compiler might use frame pointer to access to stack
// variables, so you need to wrap those alignment required functions
// with extra function call.
//
#if defined(__INTEL_COMPILER)
// For intel's compiler, simply calling alloca seems to do the right
// thing. The size of the allocated block seems to be irrelevant.
#define NII_SIMD_ALIGN_STACK()   _alloca(16)
#define NII_SIMD_ALIGN_ATTRIBUTE

#elif N_CPU == NII_CPU_X86 && (N_COMPILER == N_CPP_GNUC || N_COMPILER == N_CPP_GCCE) && (N_ARCH != N_ARCH_64)
// mark functions with GCC attribute to force stack alignment to 16 bytes
#define NII_SIMD_ALIGN_ATTRIBUTE __attribute__((force_align_arg_pointer))

#elif defined(_MSC_VER)
// Fortunately, MSVC will align the stack automatically
#define NII_SIMD_ALIGN_ATTRIBUTE

#else
#define NII_SIMD_ALIGN_ATTRIBUTE
#endif


// Additional platform-dependent header files and declares.
//
// NOTE: Should be sync with N_HAVE_SSE macro.
//

#if N_HAVE_SSE
#include <xmmintrin.h>
#else N_HAVE_NEON
#include "SSE2NEON.h"

#define _mm_cmpnle_ps _mm_cmpgt_ps

N_FORCEINLINE __m128 _mm_loadh_pi(__m128 a, __m64 const * p)
{
    return vcombine_f32(vget_low_f32(a), vld1_f32((float32_t const *)p));
}

N_FORCEINLINE void _mm_storeh_pi(__m64 * p, __m128 a)
{
    vst1_f32((float32_t *)p, vget_high_f32((float32x4_t)a));
}

N_FORCEINLINE __m128 _mm_mul_ss(__m128 a, __m128 b)
{
    a[0] *= b[0];
    return a;
}

N_FORCEINLINE __m128 _mm_sub_ss(__m128 a, __m128 b)
{
    a[0] -= b[0];
    return a;
}
#endif

namespace NII
{
#if N_HAVE_SSE

#define __MM_RSQRT_PS(x)                _mm_rsqrt_ps(x)

/** 执行单精度浮点值的4x4矩阵转置.
    参数0, r1, r2, 和r3都是 __m128值,这些元素是构成一个4x4矩阵的相
    应行,矩阵的转置返回参数 r0, r1, r2,和r3 ,其中r0保存原矩阵0列,
    r1保存原矩阵1列,等等
*/
#define __MM_TRANSPOSE4x4_PS(r0, r1, r2, r3)                                        \
    {                                                                               \
        __m128 t3, t2, t1, t0;                                                      \
                                                                                    \
                                                            /* r00 r01 r02 r03 */   \
                                                            /* r10 r11 r12 r13 */   \
                                                            /* r20 r21 r22 r23 */   \
                                                            /* r30 r31 r32 r33 */   \
                                                                                    \
        t0 = _mm_unpacklo_ps(r0, r1);                       /* r00 r10 r01 r11 */   \
        t2 = _mm_unpackhi_ps(r0, r1);                       /* r02 r12 r03 r13 */   \
        t1 = _mm_unpacklo_ps(r2, r3);                       /* r20 r30 r21 r31 */   \
        t3 = _mm_unpackhi_ps(r2, r3);                       /* r22 r32 r23 r33 */   \
                                                                                    \
        r0 = _mm_movelh_ps(t0, t1);                         /* r00 r10 r20 r30 */   \
        r1 = _mm_movehl_ps(t1, t0);                         /* r01 r11 r21 r31 */   \
        r2 = _mm_movelh_ps(t2, t3);                         /* r02 r12 r22 r32 */   \
        r3 = _mm_movehl_ps(t3, t2);                         /* r03 r13 r23 r33 */   \
    }

/** 执行一个连续存储行的4x3矩阵转置到一个单精度浮点值的3x4矩阵,
    参数 v0, v1, 和 v2 是 __m128值,这些元素是构成一个连续存储
    的4x3矩阵相应行.矩阵的转置返回在参数v0, v1, 和v2,其中v0保
    存原矩阵的0列,v1保存原矩阵的1列,等等
*/
#define __MM_TRANSPOSE4x3_PS(v0, v1, v2)                                            \
    {                                                                               \
        __m128 t0, t1, t2;                                                          \
                                                                                    \
                                                            /* r00 r01 r02 r10 */   \
                                                            /* r11 r12 r20 r21 */   \
                                                            /* r22 r30 r31 r32 */   \
                                                                                    \
        t0 = _mm_shuffle_ps(v0, v2, _MM_SHUFFLE(3,0,3,0));  /* r00 r10 r22 r32 */   \
        t1 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(1,0,2,1));  /* r01 r02 r11 r12 */   \
        t2 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(2,1,3,2));  /* r20 r21 r30 r31 */   \
                                                                                    \
        v0 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(2,0,1,0));  /* r00 r10 r20 r30 */   \
        v1 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(3,1,2,0));  /* r01 r11 r21 r31 */   \
        v2 = _mm_shuffle_ps(t1, t0, _MM_SHUFFLE(3,2,3,1));  /* r02 r12 r22 r32 */   \
    }

/** 执行一个3x4矩阵转置到一个连续存储的单精度浮点值4x3,参
    数 v0, v1, 和v2是__m128值,这这些元素是构成一个3x4矩阵
    相应的行.矩阵的转置返回的参数0, v1, 和 v2,作为一个4x3
    矩阵连续存储 的行
*/
#define __MM_TRANSPOSE3x4_PS(v0, v1, v2)                                            \
    {                                                                               \
        __m128 t0, t1, t2;                                                          \
                                                                                    \
                                                            /* r00 r10 r20 r30 */   \
                                                            /* r01 r11 r21 r31 */   \
                                                            /* r02 r12 r22 r32 */   \
                                                                                    \
        t0 = _mm_shuffle_ps(v0, v2, _MM_SHUFFLE(2,0,3,1));  /* r10 r30 r02 r22 */   \
        t1 = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(3,1,3,1));  /* r11 r31 r12 r32 */   \
        t2 = _mm_shuffle_ps(v0, v1, _MM_SHUFFLE(2,0,2,0));  /* r00 r20 r01 r21 */   \
                                                                                    \
        v0 = _mm_shuffle_ps(t2, t0, _MM_SHUFFLE(0,2,2,0));  /* r00 r01 r02 r10 */   \
        v1 = _mm_shuffle_ps(t1, t2, _MM_SHUFFLE(3,1,2,0));  /* r11 r12 r20 r21 */   \
        v2 = _mm_shuffle_ps(t0, t1, _MM_SHUFFLE(3,1,1,3));  /* r22 r30 r31 r32 */   \
    }

/** 选定值,填写单精度浮点向量.参数'fp'是一个digit[0123] ,表示参数`v`的fp
*/
#define __MM_SELECT(v, fp)              _mm_shuffle_ps((v), (v), _MM_SHUFFLE((fp),(fp),(fp),(fp)))

/// 累加四个单精度浮点的向量值
#define __MM_ACCUM4_PS(a, b, c, d)      _mm_add_ps(_mm_add_ps(a, b), _mm_add_ps(c, d))

/** 2个四个单精度浮点的向量值的点积
*/
#define __MM_DOT4x4_PS(a0, a1, a2, a3, b0, b1, b2, b3)                              \
    __MM_ACCUM4_PS(_mm_mul_ps(a0, b0), _mm_mul_ps(a1, b1), _mm_mul_ps(a2, b2), _mm_mul_ps(a3, b3))

/** 单精度浮点4向量和3向量的值的点积
*/
#define __MM_DOT4x3_PS(r0, r1, r2, r3, v0, v1, v2)                                  \
    __MM_ACCUM4_PS(_mm_mul_ps(r0, v0), _mm_mul_ps(r1, v1), _mm_mul_ps(r2, v2), r3)

/// 累积三个单精度浮点值的向量
#define __MM_ACCUM3_PS(a, b, c)         _mm_add_ps(_mm_add_ps(a, b), c)

/** 2个3单精度浮点的向量点积
*/
#define __MM_DOT3x3_PS(r0, r1, r2, v0, v1, v2)                                      \
    __MM_ACCUM3_PS(_mm_mul_ps(r0, v0), _mm_mul_ps(r1, v1), _mm_mul_ps(r2, v2))

/// 计算2个向量相乘和另一个向量相加
#define __MM_MADD_PS(a, b, c)           _mm_add_ps(_mm_mul_ps(a, b), c)

/// 线性插值
#define __MM_LERP_PS(t, a, b)           __MM_MADD_PS(_mm_sub_ps(b, a), t, a)

/// 计算2个单精度值相乘和另一个浮点值相加
#define __MM_MADD_SS(a, b, c)           _mm_add_ss(_mm_mul_ss(a, b), c)

/// 线性插值
#define __MM_LERP_SS(t, a, b)           __MM_MADD_SS(_mm_sub_ss(b, a), t, a)

/// 与_mm_load_ps相同,但可帮助vc产生更优化的代码
#define __MM_LOAD_PS(p)                 (*(__m128*)(p))

/// 与 _mm_store_ps相同, but can help VC generate more optimised code.
#define __MM_STORE_PS(p, v)             (*(__m128*)(p) = (v))


    /** 基于是否对齐,辅助加载/存储SSE数据
    */
    template <bool aligned = false>
    struct SSEMemoryAccessor
    {
        static N_FORCEINLINE __m128 load(const NIIf *p)
        {
            return _mm_loadu_ps(p);
        }
        static N_FORCEINLINE void store(NIIf *p, const __m128& v)
        {
            _mm_storeu_ps(p, v);
        }
    };
    // 特别对齐访问
    template <>
    struct SSEMemoryAccessor<true>
    {
        static N_FORCEINLINE const __m128& load(const NIIf *p)
        {
            return __MM_LOAD_PS(p);
        }
        static N_FORCEINLINE void store(NIIf *p, const __m128& v)
        {
            __MM_STORE_PS(p, v);
        }
    };

    /** 检测给定指针是否需要为SSE改善对齐
    */
    static N_FORCEINLINE bool _isAlignedForSSE(const void *p)
    {
        return (((size_t)p) & 15) == 0;
    }

// stack aligned check
#if N_DEBUG
#define NII_CHECK_STACK_ALIGNED_FOR_SSE()   {__m128 test; N_assert(_isAlignedForSSE(&test), "error");}
#else
#define NII_CHECK_STACK_ALIGNED_FOR_SSE()
#endif

#endif
}

#endif