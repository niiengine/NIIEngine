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
#include "NIIMath.h"
#include "asm_math.h"
#include "NiiVector2.h"
#include "NiiVector3.h"
#include "NiiVector4.h"
#include "NiiRay.h"
#include "NiiSphere.h"
#include "NiiAABox.h"
#include "NiiPlane.h"

#define FLOATSIGNBITSET(f) ((*(const Nul *)&(f)) >> 31)
#define INTSIGNBITSET(i) (((const Nul)(i)) >> 31)
#define IEEE_FLT_MANTISSA_BITS 23
#define IEEE_FLT_EXPONENT_BITS 8
#define IEEE_FLT_EXPONENT_BIAS 127
#define IEEE_FLT_SIGN_BIT 31

namespace NII
{
    //------------------------------------------------------------------------
    enum
    {
        LOOKUP_BITS     = 8,
        EXP_POS         = 23,
        EXP_BIAS        = 127,
        LOOKUP_POS      = (EXP_POS - LOOKUP_BITS),
        SEED_POS        = (EXP_POS - 8),
        SQRT_TABLE_SIZE = (2 << LOOKUP_BITS),
        LOOKUP_MASK     = (SQRT_TABLE_SIZE - 1)
    };
    //------------------------------------------------------------------------
    union _niifi
    {
        Nui32 i;
        NIIf f;
    };
    //------------------------------------------------------------------------
    const NIIf Math::POS_INFINITY   = N_TYPE_INFINITY(NIIf);
    const NIIf Math::NEG_INFINITY   = -N_TYPE_INFINITY(NIIf);
    const NIIf Math::PI             = 4.0f * atan(1.0f);
    const NIIf Math::PI2X           = 2.0f * PI;
    const NIIf Math::PI_2           = 0.5f * PI;
    const NIIf Math::PI_4           = 0.25f * Math::PI;
    const NIIf Math::Rad_Ang        = PI / 180.0f;
    const NIIf Math::Ang_Rad        = 180.0f / PI;
    const NIIf Math::SQRT_2            = 1.41421356237309504880f;
    const NIIf Math::SQRT_3            = 1.73205080756887729352f;
    const NIIf Math::SQRT_1_2       = 0.70710678118654752440f;
    const NIIf Math::SQRT_1_3       = 0.57735026918962576450f;
    const NIIf Math::LOG2           = log(2.0f);
    const NIIf Math::E              = 2.71828182845904523536f;
    //------------------------------------------------------------------------
    NCount Math::mTriMapSize;
    //------------------------------------------------------------------------
    NIIf Math::mTriMapFactor;
    NIIf * Math::mSinTable = NULL;
    NIIf * Math::mTanTable = NULL;
    Nui32 * Math::mSqrtTable = NULL;
    //-----------------------------------------------------------------
    Math::Math(NCount trigTableSize)
    {
        mTriMapSize = trigTableSize;
        mTriMapFactor = mTriMapSize / Math::PI2X;

        mSinTable = N_alloc_t(NIIf, mTriMapSize);
        mTanTable = N_alloc_t(NIIf, mTriMapSize);
        mSqrtTable = N_alloc_t(Nui32, SQRT_TABLE_SIZE);

        setupTriTable();
        setupSqrtTable();
    }
    //-----------------------------------------------------------------
    Math::~Math()
    {
        N_free(mSinTable);
        N_free(mTanTable);
        N_free(mSqrtTable);
    }
    //-----------------------------------------------------------------
    void Math::setupTriTable()
    {
        // Build trig lookup tables
        // Could get away with building only PI sized Sin table but simpler this
        // way. Who cares, it'll ony use an extra 8k of memory anyway and I like
        // simplicity.
        NIIf angle;
        for(NCount i = 0; i < mTriMapSize; ++i)
        {
            angle = Math::PI2X * i / mTriMapSize;
            mSinTable[i] = sin(angle);
            mTanTable[i] = tan(angle);
        }
    }
    //----------------------------------------------------------------
    void Math::setupSqrtTable()
    {
        union _niifi fi, fo;

        for(NCount i = 0; i < SQRT_TABLE_SIZE; ++i)
        {
            fi.i = ((EXP_BIAS-1)<<EXP_POS) | (i<<LOOKUP_POS);
            fo.f = (NIIf)(1.0 / sqrt(fi.f));
            mSqrtTable[i] = ((Nui32)(((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF))<<SEED_POS;
        }

        mSqrtTable[SQRT_TABLE_SIZE / 2] = ((Nui32)(0xFF))<<(SEED_POS);
    }
    //-----------------------------------------------------------------
    NIIf Math::sinTable(NIIf fValue)
    {
        // Convert range to index values, wrap if required
        NIIi idx;
        if (fValue >= 0)
        {
            idx = NIIi(fValue * mTriMapFactor) % mTriMapSize;
        }
        else
        {
            idx = mTriMapSize - (NIIi(-fValue * mTriMapFactor) % mTriMapSize) - 1;
        }
        return mSinTable[idx];
    }
    //-----------------------------------------------------------------
    NIIf Math::tanTable(NIIf fValue)
    {
        // Convert range to index values, wrap if required
        NIIi idx = NIIi(fValue *= mTriMapFactor) % mTriMapSize;
        return mTanTable[idx];
    }
    //-----------------------------------------------------------------
    NIIi Math::ISign (NIIi iValue)
    {
        return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
    }
    //-----------------------------------------------------------------
    void Math::SinCos(NIIf fValue, NIIf & fValueSin, NIIf & fValueCos)
    {
#if defined(_WIN32) && defined(MSVC)
        _asm
        {
            fld        fValue
            fsincos
            mov        ecx, fValueCos
            mov        edx, fValueSin
            fstp    dword ptr [ecx]
            fstp    dword ptr [edx]
        }
#else
        fValueSin = sinf(fValue);
        fValueCos = cosf(fValue);
#endif
    }
    //-----------------------------------------------------------------
    void Math::SinCos16(NIIf fValue, NIIf & fValueSin, NIIf & fValueCos)
    {
        NIIf t, d;

        if(( fValue < 0.0f ) || ( fValue >= Math::PI2X ))
        {
            fValue -= floorf( fValue / Math::PI2X ) * Math::PI2X;
        }
#if 1
        if(fValue < PI)
        {
            if(fValue > PI_2)
            {
                fValue = PI - fValue;
                d = -1.0f;
            }
            else
            {
                d = 1.0f;
            }
        }
        else
        {
            if(fValue > PI + PI_2)
            {
                fValue = fValue - PI2X;
                d = 1.0f;
            }
            else
            {
                fValue = PI - fValue;
                d = -1.0f;
            }
        }
#else
        fValue = PI - fValue;
        if(fabs(fValue) >= PI_2)
        {
            fValue = ((fValue < 0.0f) ? -PI : PI) - fValue;
            d = 1.0f;
        }
        else
        {
            d = -1.0f;
        }
#endif
        t = fValue * fValue;
        fValueSin = fValue * ( ( ( ( ( -2.39e-08f * t + 2.7526e-06f ) * t - 1.98409e-04f ) * t + 8.3333315e-03f ) * t - 1.666666664e-01f ) * t + 1.0f );
        fValueCos = d * ( ( ( ( ( -2.605e-07f * t + 2.47609e-05f ) * t - 1.3888397e-03f ) * t + 4.16666418e-02f ) * t - 4.999999963e-01f ) * t + 1.0f );
    }
    //-----------------------------------------------------------------
    void Math::SinCos64(NIId fValue, NIIf & fValueSin, NIIf & fValueCos)
    {
#if defined(_WIN32) && defined(MSVC)
        _asm
        {
            fld        fValue
            fsincos
            mov        ecx, fValueCos
            mov        edx, fValueSin
            fstp    qword ptr [ecx]
            fstp    qword ptr [edx]
        }
#else
        fValueSin = sin((NIIf)fValue);
        fValueCos = cos((NIIf)fValue);
#endif
    }
    //-----------------------------------------------------------------
    NIIf Math::Sin16(NIIf fValue, bool table)
    {
        if(!table)
        {
            NIIf s;

            if((fValue < 0.0f) || ( fValue >= PI2X))
            {
                fValue -= floorf( fValue / PI2X ) * PI2X;
            }
#if 1
            if(fValue < PI)
            {
                if(fValue > PI_2)
                {
                    fValue = PI - fValue;
                }
            }
            else
            {
                if(fValue > PI + PI_2)
                {
                    fValue = fValue - PI2X;
                }
                else
                {
                    fValue = PI - fValue;
                }
            }
#else
            fValue = PI - fValue;
            if(fabs(fValue) >= PI_2)
            {
                fValue = ((fValue < 0.0f) ? -PI : PI) - fValue;
            }
#endif
            s = fValue * fValue;
            return fValue * ( ( ( ( ( -2.39e-08f * s + 2.7526e-06f ) * s - 1.98409e-04f ) * s + 8.3333315e-03f ) * s - 1.666666664e-01f ) * s + 1.0f );
        }
        else
            return sinTable(fValue);
    }
    //-------------------------------------------------------------------
    NIIf Math::Cos16(NIIf fValue, bool table)
    {
        if(!table)
        {
            NIIf s, d;

            if((fValue < 0.0f) || (fValue >= PI2X))
            {
                fValue -= floorf(fValue / PI2X) * PI2X;
            }
#if 1
            if(fValue < PI)
            {
                if(fValue > PI_2)
                {
                    fValue = PI - fValue;
                    d = -1.0f;
                }
                else
                {
                    d = 1.0f;
                }
            }
            else
            {
                if(fValue > PI + PI_2)
                {
                    fValue = fValue - PI2X;
                    d = 1.0f;
                }
                else
                {
                    fValue = PI - fValue;
                    d = -1.0f;
                }
            }
#else
            fValue = PI - fValue;
            if(fabs(fValue) >= PI_2)
            {
                fValue = ((fValue < 0.0f) ? -PI : PI) - fValue;
                d = 1.0f;
            }
            else
            {
                d = -1.0f;
            }
#endif
            s = fValue * fValue;
            return d * (((((-2.605e-07f * s + 2.47609e-05f) * s - 1.3888397e-03f) * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
        }
        else
            return sinTable(fValue + PI_2);
    }
    //-----------------------------------------------------------------
    Radian Math::ACos (NIIf fValue)
    {
        if(-1.0f < fValue)
        {
            if(fValue < 1.0f)
                return Radian(acosf(fValue));
            else
                return Radian(0.0f);
        }
        else
        {
            return Radian(PI);
        }
    }
    //-----------------------------------------------------------------
    Radian Math::ACos16(NIIf fValue)
    {
        if(FLOATSIGNBITSET(fValue))
        {
            if(fValue <= -1.0f)
            {
                return Radian(PI);
            }
            fValue = fabs(fValue);
            return Radian(PI - (((-0.0187293f * fValue + 0.0742610f) * fValue - 0.2121144f ) * fValue + 1.5707288f ) * sqrt( 1.0f - fValue ));
        }
        else
        {
            if(fValue >= 1.0f)
            {
                return Radian(0.0f);
            }
            return Radian((((-0.0187293f * fValue + 0.0742610f) * fValue - 0.2121144f) * fValue + 1.5707288f ) * sqrt( 1.0f - fValue ));
        }
    }
    //-----------------------------------------------------------------
    NIId Math::ACos64(NIId fValue)
    {
        if(fValue <= -1.0f)
        {
            return PI;
        }
        if(fValue >= 1.0f)
        {
            return 0.0f;
        }
        return acos(fValue);
    }
    //-----------------------------------------------------------------
    Radian Math::ASin (NIIf fValue)
    {
        if(-1.0 < fValue)
        {
            if(fValue < 1.0)
                return Radian(asinf(fValue));
            else
                return Radian(PI_2);
        }
        else
        {
            return Radian(-PI_2);
        }
    }
    //----------------------------------------------------------------------
    Radian Math::ASin16(NIIf fValue)
    {
        if(FLOATSIGNBITSET(fValue))
        {
            if(fValue <= -1.0f)
            {
                return Radian(-PI_2);
            }
            fValue = fabs( fValue );
            return Radian(( ( ( -0.0187293f * fValue + 0.0742610f ) * fValue - 0.2121144f ) * fValue + 1.5707288f ) * sqrt( 1.0f - fValue ) - PI_2);
        }
        else
        {
            if ( fValue >= 1.0f )
            {
                return Radian(PI_2);
            }
            return Radian(PI_2 - ( ( ( -0.0187293f * fValue + 0.0742610f ) * fValue - 0.2121144f ) * fValue + 1.5707288f ) * sqrt( 1.0f - fValue ));
        }
    }
    //--------------------------------------------------------------------
    NIId Math::ASin64(NIId fValue)
    {
        if ( fValue <= -1.0f )
        {
            return -PI_2;
        }
        if ( fValue >= 1.0f )
        {
            return PI_2;
        }
        return asin( fValue );
    }
    //--------------------------------------------------------------------
    Radian Math::ATan16(NIIf fValue)
    {
        NIIf s;

        if ( fabs( fValue ) > 1.0f )
        {
            fValue = 1.0f / fValue;
            s = fValue * fValue;
            s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
                    * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * fValue;
            if ( FLOATSIGNBITSET( fValue ) )
            {
                return Radian(s - PI_2);
            }
            else
            {
                return Radian(s + PI_2);
            }
        }
        else
        {
            s = fValue * fValue;
            return Radian(( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
            * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * fValue);
        }
    }
    //------------------------------------------------------------------
    Radian Math::ATan216(NIIf fY,NIIf fX)
    {
        NIIf a, s;

        if ( fabs( fY ) > fabs( fX ) )
        {
            a = fX / fY;
            s = a * a;
            s = - ( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
                    * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a;
            if ( FLOATSIGNBITSET( a ) )
            {
                return Radian(s - PI_2);
            }
            else
            {
                return Radian(s + PI_2);
            }
        }
        else
        {
            a = fY / fX;
            s = a * a;
            return Radian(( ( ( ( ( ( ( ( ( 0.0028662257f * s - 0.0161657367f ) * s + 0.0429096138f ) * s - 0.0752896400f )
                * s + 0.1065626393f ) * s - 0.1420889944f ) * s + 0.1999355085f ) * s - 0.3333314528f ) * s ) + 1.0f ) * a);
        }
    }
    //---------------------------------------------------------------
    NIIf Math::Log16(NIIf fValue)
    {
        NIIi i, exponent;
        NIIf y, y2;

        i = *reinterpret_cast<NIIi *>(&fValue);
        exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
        i -= ( exponent + 1 ) << IEEE_FLT_MANTISSA_BITS;    // get value in the range [.5, 1>
        y = *reinterpret_cast<NIIf *>(&i);
        y *= 1.4142135623730950488f;                        // multiply with sqrt( 2 )
        y = ( y - 1.0f ) / ( y + 1.0f );
        y2 = y * y;
        y = y * ( 2.000000000046727f + y2 * ( 0.666666635059382f + y2 * ( 0.4000059794795f + y2 * ( 0.28525381498f + y2 * 0.2376245609f ) ) ) );
        y += 0.693147180559945f * ( (NIIf)exponent + 0.5f );
        return y;
    }
    //--------------------------------------------------------------------
    NIIi Math::ILog2(NIIf value)
    {
        return (((*reinterpret_cast<NIIi *>(&value)) >> IEEE_FLT_MANTISSA_BITS)&((1<<IEEE_FLT_EXPONENT_BITS)-1))-IEEE_FLT_EXPONENT_BIAS;
    }
    //--------------------------------------------------------------------
    NIIf Math::Exp16(NIIf fValue)
    {
        NIIi i, s, e, m, exponent;
        NIIf x, x2, y, p, q;

        x = fValue * 1.44269504088896340f;        // multiply with ( 1 / log( 2 ) )
#if 1
        i = *reinterpret_cast<NIIi *>(&x);
        s = ( i >> IEEE_FLT_SIGN_BIT );
        e = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
        m = ( i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 ) ) | ( 1 << IEEE_FLT_MANTISSA_BITS );
        i = ( ( m >> ( IEEE_FLT_MANTISSA_BITS - e ) ) & ~( e >> 31 ) ) ^ s;
#else
        i = (NIIi) x;
        if ( x < 0.0f )
        {
            i--;
        }
#endif
        exponent = ( i + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS;
        y = *reinterpret_cast<NIIf *>(&exponent);
        x -= (NIIf) i;
        if ( x >= 0.5f ) {
            x -= 0.5f;
            y *= 1.4142135623730950488f;    // multiply with sqrt( 2 )
        }
        x2 = x * x;
        p = x * ( 7.2152891511493f + x2 * 0.0576900723731f );
        q = 20.8189237930062f + x2;
        x = y * ( q + p ) / ( q - p );
        return x;
    }
    //-----------------------------------------------------------------
    NIIf Math::Sign (NIIf fValue)
    {
        if ( fValue > 0.0 )
            return 1.0;

        if ( fValue < 0.0 )
            return -1.0;

        return 0.0;
    }
    //-----------------------------------------------------------------
    NIIf Math::invSqrt(NIIf fValue, bool useTable)
    {
        if(!useTable)
        {
            return NIIf(asm_rsq(fValue));
        }
        else
        {
            Nui32 a = ((union _niifi *)(&fValue))->i;
            union _niifi seed;

            NIId y = fValue * 0.5f;
            seed.i = ((((3 * EXP_BIAS - 1) - ( (a >> EXP_POS) & 0xFF) ) >> 1)<<EXP_POS) | mSqrtTable[(a >> (EXP_POS - LOOKUP_BITS)) & LOOKUP_MASK];
            NIId r = seed.f;
            r = r * (1.5f - r * r * y);
            r = r * (1.5f - r * r * y);
            return (NIIf) r;
        }
    }
    //------------------------------------------------------------------
    NIIf Math::invSqrt16(NIIf value)
    {
        Nui32 a = ((union _niifi *)(&value))->i;
        union _niifi seed;

        NIId y = value * 0.5f;
        seed.i = ((((3 * EXP_BIAS - 1)-((a >> EXP_POS) & 0xFF))>>1)<< EXP_POS) | mSqrtTable[(a >> (EXP_POS - LOOKUP_BITS)) & LOOKUP_MASK];
        NIId r = seed.f;
        r = r * (1.5f - r * r * y);
        return (NIIf)r;
    }
    //------------------------------------------------------------------
    NIId Math::invSqrt64(NIId value)
    {
        Nui32 a = ((union _niifi *)(&value))->i;
        union _niifi seed;

        NIId y = value * 0.5f;
        seed.i = ((((3 * EXP_BIAS-1) - ((a >> EXP_POS) & 0xFF)) >> 1) << EXP_POS) | mSqrtTable[(a >> (EXP_POS-LOOKUP_BITS)) & LOOKUP_MASK];
        NIId r = seed.f;
        r = r * (1.5f - r * r * y);
        r = r * (1.5f - r * r * y);
        r = r * (1.5f - r * r * y);
        return r;
    }
    //------------------------------------------------------------------
    NIIf Math::RInvSqrt(NIIf value)
    {
        NIIl i;
        NIIf y, r;
        y = value * 0.5f;
        i= *reinterpret_cast<NIIl *>(&value);
        i = 0x5f3759df - (i >> 1);
        r = *reinterpret_cast<NIIf *>(&i);
        r = r * (1.5f - r * r * y);
        return r;
    }
    //------------------------------------------------------------------
    NIIf Math::Tan16(NIIf fValue, bool table)
    {
        if(!table)
        {
            NIIf s;
            bool reciprocal;

            if ( ( fValue < 0.0f ) || ( fValue >= PI ) ) {
                fValue -= floorf( fValue / PI ) * PI;
            }
#if 1
            if ( fValue < PI_2 )
            {
                if ( fValue > PI_4 )
                {
                    fValue = PI_2 - fValue;
                    reciprocal = true;
                }
                else
                {
                    reciprocal = false;
                }
            }
            else
            {
                if ( fValue > PI_2 + PI_4 )
                {
                    fValue = fValue - PI;
                    reciprocal = false;
                }
                else
                {
                    fValue = PI_2 - fValue;
                    reciprocal = true;
                }
            }
#else
            fValue = PI_2 - fValue;
            if ( fabs( fValue ) >= PI_4 )
            {
                fValue = ( ( fValue < 0.0f ) ? -PI_2 : PI_2 ) - fValue;
                reciprocal = false;
            }
            else
            {
                reciprocal = true;
            }
#endif
            s = fValue * fValue;
            s = fValue * ((((((9.5168091e-03f * s + 2.900525e-03f) * s + 2.45650893e-02f) * s + 5.33740603e-02f) * s + 1.333923995e-01f) * s + 3.333314036e-01f) * s + 1.0f);
            if(reciprocal)
            {
                return 1.0f / s;
            }
            else
            {
                return s;
            }
        }
        else
            return tanTable(fValue);
    }
    //-----------------------------------------------------------------
    NIIf Math::randomf()
    {
        return asm_rand() / asm_rand_max();
    }
    //-----------------------------------------------------------------
    NIIi Math::randomi()
    {
        return asm_randi();
    }
    //-----------------------------------------------------------------
    NIIf Math::random(NIIf low, NIIf high)
    {
        return (high - low) * randomf() + low;
    }
    //-----------------------------------------------------------------
    NIIi Math::randomi(NIIi low, NIIi high)
    {
        return (high - low) * randomi() + low;
    }
    //-----------------------------------------------------------------
    NIIf Math::randomNegPos()
    {
        return 2.0f * randomf() - 1.0f;
    }
    //--------------------------------------------------------------------
}