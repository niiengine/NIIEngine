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

#ifndef _NII_Bitwise_H_
#define _NII_Bitwise_H_

#include "NiiPreInclude.h"

#ifndef __has_builtin
    // Compatibility with non-clang compilers
    #define __has_builtin(x) 0
#endif

#if N_PLAT == N_PLAT_FREEBSD
    /// Undefine in <sys/endian.h> defined bswap macros for FreeBSD
    #undef bswap16
    #undef bswap32
    #undef bswap64
#endif

#if N_COMPILER == N_CPP_MSVC
    #include <intrin.h>
    #if N_ARCH == N_ARCH_32
        #pragma intrinsic(_BitScanForward)
        #pragma intrinsic(_BitScanReverse)
    #else
        #pragma intrinsic(_BitScanForward64)
        #pragma intrinsic(_BitScanReverse64)
    #endif
#endif

namespace NII
{
    /// 用于处理位模式的一类.
    class Bitwise
    {
    public:
        /** Returns value with reversed bytes order.
        */
        static N_FORCEINLINE Nui16 bswap16(Nui16 arg)
        {
#if N_COMPILER == N_CPP_MSVC && N_COMPILER_VER >= 1310
            return _byteswap_ushort(arg);
#elif (N_COMPILER == N_CPP_CLANG && __has_builtin(__builtin_bswap16)) || (N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 480)
            return __builtin_bswap16(arg);
#else
            return ((arg << 8) & 0xFF00) | ((arg >> 8) & 0x00FF);
#endif
        }

        /** Returns value with reversed bytes order.
        */
        static N_FORCEINLINE Nui32 bswap32(Nui32 arg)
        {
#if N_COMPILER == N_CPP_MSVC && N_COMPILER_VER >= 1310
            return _byteswap_ulong(arg);
#elif (N_COMPILER == N_CPP_CLANG && __has_builtin(__builtin_bswap32)) || (N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 430)
            return __builtin_bswap32(arg);
#else
            return ((arg & 0x000000FF) << 24) | ((arg & 0x0000FF00) << 8) | ((arg >> 8) & 0x0000FF00) | ((arg >> 24) & 0x000000FF);
#endif
        }

        /** Returns value with reversed bytes order.
        */
        static N_FORCEINLINE Nui64 bswap64(Nui64 arg)
        {
#if N_COMPILER == N_CPP_MSVC && N_COMPILER_VER >= 1310
            return _byteswap_uint64(arg);
#elif (N_COMPILER == N_CPP_CLANG && __has_builtin(__builtin_bswap64)) || (N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 430)
            return __builtin_bswap64(arg);
#else
            union {
                Nui64 sv;
                Nui32 ul[2];
            } tmp, result;
            tmp.sv = arg;
            result.ul[0] = bswap32(tmp.ul[1]);
            result.ul[1] = bswap32(tmp.ul[0]);
            return result.sv;
#endif
        }

        /** Reverses byte order of buffer. Use bswap16/32/64 instead if possible.
        */
        static inline void bswapBuffer(void * pData, size_t size)
        {
            char swapByte;
            for (char *p0 = (char*)pData, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1)
            {
                swapByte = *p0;
                *p0 = *p1;
                *p1 = swapByte;
            }
        }
        /** Reverses byte order of chunks in buffer, where 'size' is size of one chunk.
        */
        static inline void bswapChunks(void * pData, size_t size, size_t count)
        {
            for (size_t c = 0; c < count; ++c)
            {
                char swapByte;
                for (char *p0 = (char*)pData + c * size, *p1 = p0 + size - 1; p0 < p1; ++p0, --p1)
                {
                    swapByte = *p0;
                    *p0 = *p1;
                    *p1 = swapByte;
                }
            }
        }

        /** 返回一个值的最高有效位
        */
        static N_FORCEINLINE Nui32 mostSignificantBitSet(Nui32 value)
        {
            //                                     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
            static const unsigned char msb[16] = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

            Nui32 result = 0;
            if (value & 0xFFFF0000) { result += 16; value >>= 16; }
            if (value & 0x0000FF00) { result += 8; value >>= 8; }
            if (value & 0x000000F0) { result += 4; value >>= 4; }
            result += msb[value];
            return result - 1;
        }

        static N_FORCEINLINE Nui32 mostSignificantBitSet(Nui64 value)
        {
            Nui32 high = value >> 32;
            high = mostSignificantBitSet(high);
            if(high = -1)
            {
                Nui32 low = value & 0xffffffff;
                low = mostSignificantBitSet(low);
                return low;
            }
            high +=32;
            return high;
        }

        /** 返回大于或等于一个数字平方最接近的值
        @note 0的平方是0, 1的平方是1, 所以firstPO2From(0)==0还有firstPO2From(1)==1.
        */
        static N_FORCEINLINE Nui32 firstPO2From(Nui32 n)
        {
            --n;
            n |= n >> 16;
            n |= n >> 8;
            n |= n >> 4;
            n |= n >> 2;
            n |= n >> 1;
            ++n;
            return n;
        }

        /** 确定一个数字是2次方数
        @note 0 和 1 都看作2次方数.
        */
        template<typename T>
        static N_FORCEINLINE bool isPO2(T n)
        {
            return (n & (n-1)) == 0;
        }

        /// 返回一个数有效位模式,必须通过向右移删除右边的零.
        template<typename T>
        static N_FORCEINLINE Nui32 getBitShift(T mask)
        {
            if (mask == 0)
                return 0;

            Nui32 result = 0;
            while ((mask & 1) == 0)
            {
                ++result;
                mask >>= 1;
            }
            return result;
        }

        /** 通过一个值与原位码的运算,产生另一个期望的位码的值.
        @remark 这个规则对于转换颜色很有用
        */
        template<typename SrcT, typename DestT>
        static inline DestT convertBitPattern(SrcT srcValue, SrcT srcBitMask, DestT destBitMask)
        {
            // 屏蔽掉不相关的资源位(如果有的话)
            srcValue = srcValue & srcBitMask;

            // 转移资源到DWORD的底部
            const Nui32 srcBitShift = getBitShift(srcBitMask);
            srcValue >>= srcBitShift;

            // 从srcMask中获取资源最大的值
            const SrcT srcMax = srcBitMask >> srcBitShift;

            // 获取目的最大可用的
            const Nui32 destBitShift = getBitShift(destBitMask);
            const DestT destMax = destBitMask >> destBitShift;

            // 缩放资源值到目的,转移回去
            DestT destValue = (srcValue * destMax) / srcMax;
            return (destValue << destBitShift);
        }

        /// 转换N位颜色通道值到P位.它重复位模式填补P位.(这里 /((1<<n)-1)是固定点)
        static inline Nui32 fixedToFixed(Nui32 value, Nui32 n, Nui32 p)
        {
            if(n > p)
            {
                // 比可用的更少位;这个很容易
                value >>= n-p;
            }
            else if(n < p)
            {
                // 比这里需要更多的位.添冲使用老式的划分,比一个循环更好
                if(value == 0)
                    value = 0;
                else if(value == (static_cast<Nui32>(1)<<n)-1)
                    value = (1<<p)-1;
                else
                    value = value*(1<<p)/((1<<n)-1);
            }
            return value;
        }

        ///转换强制介于0.0和1.0值的浮点色彩到整形.工作在0和31之间的任何值
        static inline Nui32 floatToFixed(const NIIf value, const Nui32 bits)
        {
            if(value <= 0.0f) return 0;
            else if (value >= 1.0f) return (1<<bits)-1;
            else return (Nui32)(value * (1<<bits));
        }

        /// 定点到 NIIf
        static inline NIIf fixedToFloat(unsigned value, Nui32 bits)
        {
            return (NIIf)value/(NIIf)((1<<bits)-1);
        }

        static inline NIIf Ni16ToFloat( Ni16 v )
        {
            // -32768 & -32767 both map to -1 according to D3D10 rules.
            NIIf temp = v / 32767.0f;
            return temp > -1.0f ? temp : -1.0f;
        }
        
        /// 以本地字节序,写入一个n*8位整型值到内存
        static inline void intWrite(Nui8 * dest, const NIIi n, const Nui32 value)
        {
            switch(n)
            {
            case 1:
                dest[0] = (Nui8)value;
                break;
            case 2:
                ((Nui16 *)dest)[0] = (Nui16)value;
                break;
            case 3:
#if N_ENDIAN == N_ENDIAN_B
                dest[0] = (Nui8)((value >> 16) & 0xFF);
                dest[1] = (Nui8)((value >> 8) & 0xFF);
                dest[2] = (Nui8)(value & 0xFF);
#else
                dest[2] = (Nui8)((value >> 16) & 0xFF);
                dest[1] = (Nui8)((value >> 8) & 0xFF);
                dest[0] = (Nui8)(value & 0xFF);
#endif
                break;
            case 4:
                ((Nui32 *)dest)[0] = (Nui32)value;
                break;
            }
        }
        /// 以本地字节序,从内存中读取一个n*8位整型值
        static inline Nui32 intRead(const Nui8 * src, NIIi n)
        {
            switch(n)
            {
            case 1:
                return src[0];
            case 2:
                return ((Nui16 *)src)[0];
            case 3:
#if N_ENDIAN == N_ENDIAN_B
                return ((Nui32)src[0]<<16)| ((Nui32)src[1]<<8)| ((Nui32)src[2]);
#else
                return ((Nui32)src[0])| ((Nui32)src[1]<<8)| ((Nui32)src[2]<<16);
#endif
            case 4:
                return ((Nui32 *)src)[0];
            }
            return 0; // ?
        }

        /// 转换一个float32到一个float16(NV_half_float)(OpenEXR帮助)
        static inline Nui16 floatToHalf(NIIf i)
        {
            union
            {
                NIIf f;
                Nui32 i;
            } v;
            v.f = i;
            return floatToHalfI(v.i);
        }

        /// 转换一个在Nui32格式中的半个NIIf到一个Nui16 格式
        static inline Nui16 floatToHalfI(Nui32 i)
        {
            register NIIi s =  (i >> 16) & 0x00008000;
            register NIIi e = ((i >> 23) & 0x000000ff) - (127 - 15);
            register NIIi m =   i        & 0x007fffff;

            if (e <= 0)
            {
                if (e < -10)
                {
                    return 0;
                }
                m = (m | 0x00800000) >> (1 - e);

                return static_cast<Nui16>(s | (m >> 13));
            }
            else if (e == 0xff - (127 - 15))
            {
                if (m == 0) // Inf
                {
                    return static_cast<Nui16>(s | 0x7c00);
                }
                else    // NAN
                {
                    m >>= 13;
                    return static_cast<Nui16>(s | 0x7c00 | m | (m == 0));
                }
            }
            else
            {
                if (e > 30) // Overflow
                {
                    return static_cast<Nui16>(s | 0x7c00);
                }

                return static_cast<Nui16>(s | (e << 10) | (m >> 13));
            }
        }

        /// 转换一个float16 (NV_half_float) 到一个 float32(OpenEXR帮助)
        static inline NIIf halfToFloat(Nui16 y)
        {
            union { NIIf f; Nui32 i; } v;
            v.i = halfToFloatI(y);
            return v.f;
        }

        /// 转换Nui16格式为半个NIIf放到Nui32格式中
        static inline Nui32 halfToFloatI(Nui16 y)
        {
            register NIIi s = (y >> 15) & 0x00000001;
            register NIIi e = (y >> 10) & 0x0000001f;
            register NIIi m =  y        & 0x000003ff;

            if (e == 0)
            {
                if (m == 0) // Plus or minus zero
                {
                    return s << 31;
                }
                else // Denormalized number -- renormalize it
                {
                    while (!(m & 0x00000400))
                    {
                        m <<= 1;
                        e -=  1;
                    }

                    e += 1;
                    m &= ~0x00000400;
                }
            }
            else if (e == 31)
            {
                if (m == 0) // Inf
                {
                    return (s << 31) | 0x7f800000;
                }
                else // NaN
                {
                    return (s << 31) | 0x7f800000 | (m << 13);
                }
            }

            e = e + (127 - 15);
            m = m << 13;

            return (s << 31) | (e << 23) | m;
        }
        
        static inline Ni16 floatToSnorm16( NIIf v )
        {
            //According to D3D10 rules, the value "-1.0f" has two representations:
            //  0x1000 and 0x10001
            //This allows everyone to convert by just multiplying by 32767 instead
            //of multiplying the negative values by 32768 and 32767 for positive.
            return static_cast<Ni16>( std::clamp( v >= 0.0f ? (v * 32767.0f + 0.5f) : (v * 32767.0f - 0.5f), -32768.0f, 32767.0f ) );
        }

        static inline float snorm16ToFloat( Ni16 v )
        {
            // -32768 & -32767 both map to -1 according to D3D10 rules.
            return Ogre::max( v / 32767.0f, -1.0f );
        }

        static inline Ni8 floatToSnorm8( NIIf v )
        {
            //According to D3D10 rules, the value "-1.0f" has two representations:
            //  0x10 and 0x11
            //This allows everyone to convert by just multiplying by 127 instead
            //of multiplying the negative values by 128 and 127 for positive.
            return static_cast<Ni8>( std::clamp( v >= 0.0f ? (v * 127.0f + 0.5f) : (v * 127.0f - 0.5f), -128.0f, 127.0f ) );
        }

        static inline float snorm8ToFloat( Ni8 v )
        {
            // -128 & -127 both map to -1 according to D3D10 rules.
            return Ogre::max( v / 127.0f, -1.0f );
        }

        static inline Nui32 ctz32( Nui32 value )
        {
            if( value == 0 )
                return 32u;

        #if N_COMPILER == N_CPP_MSVC
            unsigned long trailingZero = 0;
            _BitScanForward( &trailingZero, value );
            return trailingZero;
        #else
            return __builtin_ctz( value );
        #endif
        }

        static inline Nui32 clz32( Nui32 value )
        {
            if( value == 0 )
                return 32u;

        #if N_COMPILER == N_CPP_MSVC
            unsigned long lastBitSet = 0;
            _BitScanReverse( &lastBitSet, value );
            return 31u - lastBitSet;
        #else
            return __builtin_clz( value );
        #endif
        }

        static inline Nui32 ctz64( uint64 value )
        {
            if( value == 0 )
                return 64u;

        #if N_COMPILER == N_CPP_MSVC
            unsigned long trailingZero = 0;
            #if N_ARCH == N_ARCH_32
                //Scan the low 32 bits.
                if( _BitScanForward( &trailingZero, static_cast<uint32>(value) ) )
                    return trailingZero;

                //Scan the high 32 bits.
                _BitScanForward( &trailingZero, static_cast<uint32>(value >> 32u) );
                trailingZero += 32u;
            #else
                _BitScanForward64( &trailingZero, value );
            #endif
            return trailingZero;
        #else
            return __builtin_ctzl( value );
        #endif
        }

        static inline Nui32 clz64( uint64 value )
        {
            if( value == 0 )
                return 64u;

        #if N_COMPILER == N_CPP_MSVC
            unsigned long lastBitSet = 0;
            #if N_ARCH == N_ARCH_32
                //Scan the high 32 bits.
                if( _BitScanReverse( &lastBitSet, static_cast<uint32>(value >> 32u) ) )
                    return 63u - (lastBitSet + 32u);

                //Scan the low 32 bits.
                _BitScanReverse( &lastBitSet, static_cast<uint32>(value) );
            #else
                _BitScanReverse64( &lastBitSet, value );
            #endif
            return 63u - lastBitSet;
        #else
            return __builtin_clzl( value );
        #endif
        }
    };
}

#if N_PLAT == N_PLAT_FREEBSD
    #define bswap16(x) __bswap16(x)
    #define bswap32(x) __bswap32(x)
    #define bswap64(x) __bswap64(x)
#endif

#endif