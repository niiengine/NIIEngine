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

#ifndef _NII_BASE_H_
#define _NII_BASE_H_

#include "NiiPlatInfo.h"
namespace NII
{

#define cccccccc Vector4S{NIIf x,y,z,w;Vector4S(){} Vector4S(NIIf a,NIIf b,NIIf c,NIIf d):x(a),y(b),z(c),w(d){}}
    NII_SIMD_ALIGNED_DECL22(cccccccc,Vector4S);
#undef     cccccccc
    template<typename V4>struct Vector4N
    {
        V4 x,y,z,w;
        Vector4N(){} 
        Vector4N(NIIf a,NIIf b,NIIf c,NIIf d):x(a),y(b),z(c),w(d){}
    };
    
#define cccccccc Vector3S{NIIf x,y,z,w;Vector3S(){} Vector3S(NIIf a,NIIf b,NIIf c):x(a),y(b),z(c),w(1.0f){} Vector3S(NIIf a,NIIf b,NIIf c,NIIf d):x(a),y(b),z(c),w(d){}}
    NII_SIMD_ALIGNED_DECL22(cccccccc,Vector3S);
#undef     cccccccc
    template<typename V3>struct Vector3N
    {
        V3 x,y,z;
        Vector3N(){}
        Vector3N(NIIf a,NIIf b,NIIf c):x(a),y(b),z(c){}
        Vector3N(NIIf a,NIIf b,NIIf c,NIIf d):x(a),y(b),z(c){}
    };
    
    #define cccccccc _m[4][4]
    NII_SIMD_ALIGNED_DECL3(Matrix4S,NIIf,cccccccc);
    #undef     cccccccc
    template<typename M4>struct Matrix4N
    {
        M4 _m[4][4];
    };
    #define cccccccc _m[3][4]
    NII_SIMD_ALIGNED_DECL3(Matrix3S,NIIf,cccccccc);
    #undef     cccccccc
    template<typename M3>struct Matrix3N
    {
        M3 _m[3][4];
    };
    #define cccccccc w, x, y, z
    NII_SIMD_ALIGNED_DECL3(QuaternionS,NIIf,cccccccc);
    #undef    cccccccc
    struct QuaternionN
    {
        NIIf w, x, y, z;
    };
}
#endif
