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
#include "NiiStrConv.h"
#include "NiiVector2.h"
#include "NiiVector3.h"
#include "NiiVector4.h"
#include "NiiMatrix3.h"
#include "NiiMatrix4.h"
#include "NiiQuaternion.h"
#include "NiiColour.h"

#if N_PLAT == N_PLAT_WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <iconv.h>
    #include <errno.h>
    #include <locale>
#endif

#if defined(_MSC_VER) && _MSC_VER > 0
    #define PRINTF_64_BIT_MODIFIERW     L"%I64d"
    #define PRINTF_64_BIT_MODIFIERV     "%I64d"
    #define PRINTF_64U_BIT_MODIFIERW    L"%I64u"
    #define PRINTF_64U_BIT_MODIFIERV    "%I64u"
#else
    #define PRINTF_64_BIT_MODIFIERW     L"%ll"
    #define PRINTF_64_BIT_MODIFIERV     "%ll"
    #define PRINTF_64U_BIT_MODIFIERW    L"%llu"
    #define PRINTF_64U_BIT_MODIFIERV    "%llu"
#endif

namespace NII
{
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(NIIf in, WString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfW(&out[0], 32, L"%g", in);
        n > 0? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(NIIf in, VString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfV(&out[0], 32, "%g", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Nui16 in, WString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfW(&out[0], 32, L"%hu", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Nui16 in, VString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfV(&out[0], 32, "%hu", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Ni16 in, WString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfW(&out[0], 32, L"%hd", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Ni16 in, VString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfV(&out[0], 32, "%hd", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(NIId in, WString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfW(&out[0], 64, L"%lg", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(NIId in, VString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfV(&out[0], 64, "%lg", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Ni32 in, WString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfW(&out[0], 32, L"%d", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Ni32 in, VString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfV(&out[0], 32, "%d", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Nui32 in, WString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfW(&out[0], 32, L"%u", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Nui32 in, VString & out)
    {
        out.resize(32);
        NIIi n = NsnprintfV(&out[0], 32, "%u", in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(Nul in, String & out, Nui16 width, NIIb fill, std::ios::fmtflags flags)
    {
        //Ntchar buff[64];
        //NIIi n = Nsntprintf(buff, sizeof(buff), _T("%lu"), in);
        //return String(buff, n);

        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if (flags)
            stream.setf(flags);
        stream << in;
        out = stream.str();
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(NIIl in, String & out, Nui16 width, NIIb fill, std::ios::fmtflags flags)
    {
        //Ntchar buff[64];
        //NIIi n = Nsntprintf(buff, sizeof(buff), _T("%l"), in);
        //return String(buff, n);

        StringStream stream;
        stream.width(width);
        stream.fill(fill);
        if(flags)
            stream.setf(flags);
        stream << in;
        out = stream.str();
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Ni64 in, WString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfW(&out[0], 64, PRINTF_64_BIT_MODIFIERW, in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Ni64 in, VString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfV(&out[0], 64, PRINTF_64_BIT_MODIFIERV, in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const WString & StrConv::conv(Nui64 in, WString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfW(&out[0], 64, PRINTF_64U_BIT_MODIFIERW, in);
		n > 0 ? out.resize(128) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const VString & StrConv::conv(Nui64 in, VString & out)
    {
        out.resize(64);
        NIIi n = NsnprintfV(&out[0], 64, PRINTF_64U_BIT_MODIFIERV, in);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Vector2f & in, String & out)
    {
        out.resize(64);
        NIIi n = Nsntprintf(&out[0], 64, _T("%g %g"), in.x, in.y);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Vector3f & in, String & out)
    {
        out.resize(96);
        NIIi n = Nsntprintf(&out[0], 96, _T("%g %g %g"), in.x, in.y, in.z);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Vector4f & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.x, in.y, in.z, in.w);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Vector4i & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%d %d %d %d"), in.x, in.y, in.z, in.w);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Matrix3f & in, String & out)
    {
        out.resize(288);
        NIIi n = Nsntprintf(&out[0], 288, _T("%g %g %g %g %g %g %g %g %g"), in[0][0],
            in[0][1], in[0][2], in[1][0], in[1][1], in[1][2], in[2][0], in[2][1], in[2][2]);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(bool in, String & out)
    {
        if (in)
        {
            out = _T("true");
        }
        else
        {
            out = _T("no");
        }
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Matrix4f & in, String & out)
    {
        out.resize(512);
        NIIi n = Nsntprintf(&out[0], 512, _T("%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g"),
            in[0][0], in[0][1], in[0][2], in[0][3], in[1][0], in[1][1], in[1][2], in[1][3],
            in[2][0], in[2][1], in[2][2], in[2][3], in[3][0], in[3][1], in[3][2], in[3][3]);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Quaternion & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.w, in.x, in.y, in.z);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Colour & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.r, in.g, in.b, in.a);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const Rectf & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g %g %g"), in.mLeft, in.mTop, in.mRight, in.mBottom);
		n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const RelPosf & in, String & out)
    {
        out.resize(64);
        NIIi n = Nsntprintf(&out[0], 64, _T("%g %g"), in.mScale, in.mOffset);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const RelRectf & in, String & out)
    {
        out.resize(128);
        NIIi n = Nsntprintf(&out[0], 128, _T("%g %g,%g %g,%g %g,%g %g"),
            in.mLeft.mScale, in.mLeft.mOffset,
            in.mTop.mScale, in.mTop.mOffset,
            in.mRight.mScale, in.mRight.mOffset,
            in.mBottom.mScale, in.mBottom.mOffset);
        n > 0 ? out.resize(n) : out.resize(0);
        return out;
    }
    //-----------------------------------------------------------------------
    const String & StrConv::conv(const StringList & in, String & out)
    {
        StringStream stream;
        StringList::const_iterator i;
        StringList::const_iterator ibegin = in.begin();
        StringList::const_iterator iend = in.end();
        for (i = ibegin; i != iend; ++i)
        {
            if (i != ibegin)
                stream << _T(" ");

            stream << *i;
        }
        out = stream.str();
        return out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, NIIf & out, NIIf init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%g", &out);

#if N_PLAT == N_PLAT_ANDROID
        NIIi n = Nsscanf(in.c_str(), "%g", &out);

        if(n == 0)
        {
            // Nothing read, so try integer format
            NIIi ret2 = 0;
            n = Nsscanf(in.c_str(), "%d", &ret2);
            if(n == 1)
                out = (NIIf)ret2;
        }
#else
        // Use istringstream for direct correspondence with from
        _WStringStreamBase str(in);
        out = init;
        str >> out;
#endif
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, NIIf & out, NIIf init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%g", &out);

#if N_PLAT == N_PLAT_ANDROID
        NIIi n = Nsscanf(in.c_str(), "%g", &out);

        if (n == 0)
        {
            // Nothing read, so try integer format
            NIIi ret2 = 0;
            n = Nsscanf(in.c_str(), "%d", &ret2);
            if (n == 1)
                out = (NIIf)ret2;
        }
#else
        // Use istringstream for direct correspondence with from
        _VStringStreamBase str(in);
        out = init;
        str >> out;
#endif
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, NIId & out, NIId o)
    {
        out = o;
        swscanf(in.c_str(), L"%lg", &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, NIId & out, NIId o)
    {
        out = o;
        sscanf(in.c_str(), "%lg", &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Radian & out, const Radian & o)
    {
        return conv(in, out.mValue, o.mValue);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Ni16 & out, Ni16 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%hd", &out);

        // Use istringstream for direct correspondence with from
        _WStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Ni16 & out, Ni16 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%hd", &out);

        // Use istringstream for direct correspondence with from
        _VStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Nui16 & out, Nui16 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%hu", &out);

        // Use istringstream for direct correspondence with from
        _WStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Nui16 & out, Nui16 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%hu", &out);

        // Use istringstream for direct correspondence with from
        _VStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Ni32 & out, Ni32 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%d", &out);

        _WStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Ni32 & out, Ni32 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%d", &out);

        _VStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Nui32 & out, Nui32 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%u", &out);

        _WStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Nui32 & out, Nui32 init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%u", &out);

        _VStringStreamBase str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Ni64 & out, Ni64 init)
    {
        out = init;
        swscanf(in.c_str(), PRINTF_64_BIT_MODIFIERW, &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Ni64 & out, Ni64 init)
    {
        out = init;
        sscanf(in.c_str(), PRINTF_64_BIT_MODIFIERV, &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Nui64 & out, Nui64 init)
    {
        out = init;
        swscanf(in.c_str(), PRINTF_64U_BIT_MODIFIERW, &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Nui64 & out, Nui64 init)
    {
        out = init;
        sscanf(in.c_str(), PRINTF_64U_BIT_MODIFIERV, &out);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, NIIl & out, NIIl init)
    {
        //out = init;
        //Nsscanf(in.c_str(), " %l", &out);

        WStringStream str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, NIIl & out, NIIl init)
    {
        //out = init;
        //Nsscanf(in.c_str(), " %l", &out);

        VStringStream str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const WString & in, Nul & out, Nul init)
    {
        //out = init;
        //Nsscanf(in.c_str(), " %lu", &out);

        WStringStream str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, Nul & out, Nul init)
    {
        //out = init;
        //Nsscanf(in.c_str(), " %lu", &out);

        VStringStream str(in);
        out = init;
        str >> out;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, bool & out, bool init)
    {
        if ((StrUtil::beginsWith(in, _T("true")) || StrUtil::beginsWith(in, _T("yes")) || 
            StrUtil::beginsWith(in, _T("1"))))
            out = true;
        else if ((StrUtil::beginsWith(in, _T("false")) || StrUtil::beginsWith(in, _T("no")) || 
            StrUtil::beginsWith(in, _T("0"))))
            out = false;
        else
            out = init;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Vector2f & out, const Vector2f & init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%g %g", &out.x, &out.y);

        StringList vec;
        StrUtil::split(in, vec);
        if(vec.size() != 2)
        {
            out = init;
        }
        else
        {
            conv(vec[0], out.x);
            conv(vec[1], out.y);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Vector3f & out, const Vector3f & init)
    {
        //out = init;
        //Nsscanf(in.c_str(), "%g %g %g", &out.x, &out.y, &out.z);

        StringList vec;
        StrUtil::split(in, vec);
        if (vec.size() != 3)
        {
            out = init;
        }
        else
        {
            conv(vec[0], out.x);
            conv(vec[1], out.y);
            conv(vec[2], out.z);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Vector4f & out, const Vector4f & o)
    {
        StringList vec;
        StrUtil::split(in, vec);
        if (vec.size() != 4)
        {
            out = o;
        }
        else
        {
            conv(vec[0], out.x);
            conv(vec[1], out.y);
            conv(vec[2], out.z);
            conv(vec[3], out.w);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Vector4i & out, const Vector4i & o)
    {
        StringList vec;
        StrUtil::split(in, vec);
        if (vec.size() != 4)
        {
            out = o;
        }
        else
        {
            conv(vec[0], out.x);
            conv(vec[1], out.y);
            conv(vec[2], out.z);
            conv(vec[3], out.w);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Matrix3f & out, const Matrix3f & init)
    {
        // Split on space
        StringList vec;
        StrUtil::split(in, vec);

        if (vec.size() != 9)
        {
            out = init;
        }
        else
        {
            conv(vec[0], out._m[0]);conv(vec[1], out._m[1]);conv(vec[2], out._m[2]);
            conv(vec[3], out._m[3]);conv(vec[4], out._m[4]);conv(vec[5], out._m[5]);
            conv(vec[6], out._m[6]);conv(vec[7], out._m[7]);conv(vec[8], out._m[8]);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Matrix4f & out, const Matrix4f & init)
    {
        // Split on space
        StringList vec;
        StrUtil::split(in, vec);

        if (vec.size() != 16)
        {
            out = init;
        }
        else
        {
            conv(vec[0], out._m[0]);    conv(vec[1], out._m[1]);
            conv(vec[2], out._m[2]);    conv(vec[3], out._m[3]);
            conv(vec[4], out._m[4]);    conv(vec[5], out._m[5]);
            conv(vec[6], out._m[6]);    conv(vec[7], out._m[7]);
            conv(vec[8], out._m[8]);    conv(vec[9], out._m[9]);
            conv(vec[10], out._m[10]);  conv(vec[11], out._m[11]);
            conv(vec[12], out._m[12]);  conv(vec[13], out._m[13]);
            conv(vec[14], out._m[14]);  conv(vec[15], out._m[15]);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Quaternion & out, const Quaternion & init)
    {
        // Split on space
        StringList vec;
        StrUtil::split(in, vec);

        if(vec.size() == 3)
        {
            NIIf x, y, z;
            conv(vec[0], x);
            conv(vec[1], y);
            conv(vec[2], z);
            out.from(Radian(Angle(x)), Radian(Angle(y)), Radian(Angle(z)));
        }
        else if(vec.size() != 4)
        {
            out = init;
        }
        else
        {
            conv(vec[0], out.w);
            conv(vec[1], out.x);
            conv(vec[2], out.y);
            conv(vec[3], out.z);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Colour & out, const Colour & init)
    {
        // Split on space
        StringList vec;
        StrUtil::split(in, vec);
        if(vec.size() == 4)
        {
            conv(vec[0], out.r);
            conv(vec[1], out.g);
            conv(vec[2], out.b);
            conv(vec[3], out.a);
        }
        else if(vec.size() == 3)
        {
            conv(vec[0], out.r);
            conv(vec[1], out.g);
            conv(vec[2], out.b);
            out.a = 1.0f;
        }
        else
        {
            Nui32 val = init.getARGB();
            Nsscanf(vec[0].c_str(), _T("%8X"), &val);
            out.setARGB(val);
        }
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, Rectf & out, const Rectf & o)
    {
        out = o;
        Nsscanf(in.c_str(), _T("%g %g %g %g"), &out.mLeft, &out.mTop, &out.mRight, &out.mBottom);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, RelPosf & out, const RelPosf & o)
    {
        out = o;
        Nsscanf(in.c_str(), _T("%g %g"), &out.mScale, &out.mOffset);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, RelRectf & out, const RelRectf & o)
    {
        out = o;
        Nsscanf(in.c_str(), _T("(%g,%g),(%g,%g),(%g,%g),(%g,%g)"),
            &out.mLeft.mScale, &out.mLeft.mOffset,
            &out.mTop.mScale, &out.mTop.mOffset,
            &out.mRight.mScale, &out.mRight.mOffset,
            &out.mBottom.mScale, &out.mBottom.mOffset);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, StringList & out)
    {
        StrUtil::split(in, out);
    }
    //-----------------------------------------------------------------------
    bool StrConv::isNumber(const String & in)
    {
#if N_PLAT == N_PLAT_ANDROID
        NIIf test;
        NIIi n = Nsscanf(in.c_str(), "%g", &test);
        return n == 1;
#else
        StringStream str(in);
        NIIf tst;
        str >> tst;
        return !str.fail() && str.eof();
#endif
    }
    //-----------------------------------------------------------------------
#if N_PLAT == N_PLAT_WIN32
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, String & out)
    {
    #if NII_STRING
        if (in.empty())
        {
            out = _T("");
        }
        int length = ::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), (int)in.length() + 1, NULL, NULL);
        out.resize(length);
        ::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), (int)in.length() + 1, &out[0], (int)out.length());
    #else
        out = in;
    #endif
        //std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
        //return conv.from_bytes(src);
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, VString & out)
    {
    #if NII_STRING
        if (in.empty())
        {
            out = "";
        }
        int length = ::WideCharToMultiByte(CP_UTF8, 0, in.c_str(), (int)in.length() + 1, NULL, 0, NULL, NULL);
        out.resize(length);
        ::WideCharToMultiByte(CP_UTF8, 0, in.c_str(), in.length() + 1, &out[0], (int)out.length(), NULL, NULL);
    #else
        out = in;
    #endif

        //std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        //return conv.to_bytes(src);
    }
    //------------------------------------------------------------------------
    void StrConv::convUTF(const VString & in, Nui16 *& out)
    {
        int len = ::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), -1, 0, 0);
        if(!len)
            N_EXCEPT(InvalidRequest, _I18n("MultiByteToWideChar failed"));

        out = N_alloc_t(Nui16, len);
        MultiByteToWideChar(CP_UTF8, 0, in.c_str(), -1, reinterpret_cast<LPWSTR>(out), len);
    }
    //------------------------------------------------------------------------
    template<typename T> static void _fromUTF(UINT cp, const Nui16 * in, VString & out)
    {
        int len = WideCharToMultiByte(cp, 0, reinterpret_cast<LPCWSTR>(in), -1, 0, 0, 0, 0);
        if (!len)
            out = VString("");

        out.resize(sizeof(T) * len);
        WideCharToMultiByte(cp, 0, reinterpret_cast<LPCWSTR>(in), -1, reinterpret_cast<char *>(&out[0]), len, 0, 0);
    }
    //------------------------------------------------------------------------
    void StrConv::convUTF(const Nui16 * in, VString & out)
    {
    #if NII_STRING && !_UNICODE
        _fromUTF<Nui16>(CP_ACP, in, out);
    #else
        _fromUTF<Nui8>(CP_UTF8, in, out);
    #endif
    }
    //------------------------------------------------------------------------
#else
    //------------------------------------------------------------------------
    class IconvHelper
    {
    public:
        IconvHelper(const String & to, const String & from) :
            mFrom(from),
            mTo(to),
            mOp(iconv_open(mTo.c_str(), mFrom.c_str()))
        {
            if(mOp == reinterpret_cast<iconv_t>(-1))
                N_EXCEPT(InvalidRequest, _I18n("Failed to create conversion descriptor from \"") +
                    mFrom.c_str() + _I18n("\" to \"") + mTo.c_str() + "\"."));
        }

        ~IconvHelper()
        {
            iconv_close(mOp);
        }

        NCount iconv(const char ** inbuf, NCount * inbytesleft, char ** outbuf, NCount * outbytesleft)
        {
    #ifdef CEGUI_ICONV_USES_CONST_INBUF
            return ::iconv(mOp, inbuf, inbytesleft, outbuf, outbytesleft);
    #else
            return ::iconv(mOp, const_cast<char**>(inbuf), inbytesleft, outbuf, outbytesleft);
    #endif
        }

        void check(int err)
        {
            String reason;

            if(err == EINVAL)
                reason = "Incomplete " + mFrom + " sequence.";
            else if(err == EILSEQ)
                reason = "Invalid " + mFrom + " sequence.";
            else
                reason = "Unknown error.";

            N_EXCEPT(InvalidRequest, _I18n("Failed to convert from \"") + mFrom.c_str() +
                _I18n("\" to \"") + mTo.c_str() + "\": " + reason.c_str()));
        }
    private:
        std::string mFrom;
        std::string mTo;
        iconv_t mOp;
    };
    //------------------------------------------------------------------------
    template<typename t1, typename t2>
    static t1 iconvTranscode(IconvHelper & ich, const char * in, NCount len)
    {
        t2 * tmp = iconvTranscode<t2>(ich, in, len);
        t1 result(tmp);
        N_free(tmp);
        return result;
    }
    //------------------------------------------------------------------------
    template<typename T>
    static T * iconvTranscode(IconvHelper & ich, const char * in, NCount len)
    {
        // Handle empty strings
        if(len == 0)
        {
            T * re = N_alloc_t(T, 1);
            re[0] = 0;
            return re;
        }

        vector<T> out;
        out.resize(len);
        NCount out_count = 0;

        while(true)
        {
            char * out_buf = reinterpret_cast<char *>(&out[out_count]);
            NCount start_out_bytes_left = (out.size() - out_count) * sizeof(T);
            NCount out_bytes_left = start_out_bytes_left;

            NCount result = ich.iconv(&in, &len, &out_buf, &out_bytes_left);

            out_count += (start_out_bytes_left - out_bytes_left) / sizeof(T);

            if(result != static_cast<NCount>(-1))
            {
                T * re = N_alloc_t(T, out_count + 1);
                memcpy(re, &out[0], out_count * sizeof(T));
                re[out_count] = 0;
                return re;
            }

            if(errno != E2BIG)
                break;

            out.resize(out.size() + 8); // this is some arbitrary number
        }

        ich.check(errno);

        return 0;
    }
    //------------------------------------------------------------------------
    template<typename T> static NCount getStrLen(const T * buffer)
    {
        const T * b = buffer;
        while(*b++);

        return b - buffer - 1;
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const VString & in, String & out)
    {
        #ERR
    }
    //-----------------------------------------------------------------------
    void StrConv::conv(const String & in, VString & out)
    {
        #ERR
    }
    //------------------------------------------------------------------------
    void StrConv::convUTF(const VString & in, Nui16 *& out)
    {
        union
        {
            Nui32 i;
            char c[4];
        } bint = {0x01020304};

        if(bint.c[0] == 1)
            IconvHelper ich("UTF-16BE", "UTF-8");
        else
            IconvHelper ich("UTF-16LE", "UTF-8");
        out = iconvTranscode<Nui16>(ich, in.c_str(), getStrLen(in.c_str()));
    }
    //------------------------------------------------------------------------
    void StrConv::convUTF(const Nui16 * in, VString & out)
    {
        union
        {
            Nui32 i;
            char c[4];
        } bint = {0x01020304};

    #if NII_STRING
        if(bint.c[0] == 1)
            IconvHelper ich("WCHAR_T", "UTF-16BE");
        else
            IconvHelper ich("WCHAR_T", "UTF-16LE");
        out = iconvTranscode<WString, wchar_t>(ich, reinterpret_cast<const char *>(in),
            getStrLen(in));
    #else
        if (bint.c[0] == 1)
            IconvHelper ich("UTF-8", "UTF-16BE");
        else
            IconvHelper ich("UTF-8", "UTF-16LE");
        out = iconvTranscode<String, Nui8>(ich, reinterpret_cast<const char *>(in),
            getStrLen(in) * sizeof(Nui16));
    #endif
    }
    //------------------------------------------------------------------------
#endif
    //------------------------------------------------------------------------
    void StrConv::conv(const String & in, ScaleMode & out)
    {
        if (in == _T("vertical"))
        {
            out = SM_Y;
        }
        else if (in == _T("horizontal"))
        {
            out = SM_X;
        }
        else if (in == _T("min"))
        {
            out = SM_Min;
        }
        else if (in == _T("max"))
        {
            out = SM_Max;
        }
        else if (in == _T("true") || in == _T("True"))
        {
            out = SM_ALL;
        }
        else
        {
            out = SM_Unknow;
        }
    }
    //------------------------------------------------------------------------
    const String & StrConv::conv(ScaleMode in, String & out)
    {
        if(in == SM_Unknow)
        {
            out = _T("false");
        }
        else if(in == SM_Y)
        {
            out = _T("vertical");
        }
        else if(in == SM_X)
        {
            out = _T("horizontal");
        }
        else if(in == SM_Min)
        {
            out = _T("min");
        }
        else if(in == SM_Max)
        {
            out = _T("max");
        }
        else if(in == SM_ALL)
        {
            out = _T("true");
        }
        else
        {
            N_assert(false, "Invalid auto scaled mode");
            out = _T("false");
        }
        return out;
    }
    //------------------------------------------------------------------------
    void StrConv::conv(const String & in, ColourFactor & out)
    {
        if (in == _T("one"))
            out = CF_ONE;
        else if (in == _T("src"))
            out = CF_SRC;
        else if (in == _T("dst"))
            out = CF_DST;
        else if (in == _T("inv_dst"))
            out = CF_INV_DST;
        else if (in == _T("inv_src"))
            out = CF_INV_SRC;
        else if (in == _T("src_alpha"))
            out = CF_SRC_ALPHA;
        else if (in == _T("dst_alpha"))
            out = CF_DST_ALPHA;
        else if (in == _T("inv_dst_alpha"))
            out = CF_INV_DST_ALPHA;
        else if (in == _T("inv_src_alpha"))
            out = CF_INV_SRC_ALPHA;
        else if (in == _T("zero"))
            out = CF_ZERO;
        else
            out = CF_ZERO;
    }
    //------------------------------------------------------------------------
    const String & StrConv::conv(ColourFactor in, String & out)
    {
        if(in == CF_ONE)
            out = _T("one");
        else if(in == CF_SRC)
            out = _T("src");
        else if(in == CF_DST)
            out = _T("dst");
        else if(in == CF_INV_DST)
            out = _T("inv_dst");
        else if(in == CF_INV_SRC)
            out = _T("inv_src");
        else if(in == CF_SRC_ALPHA)
            out = _T("src_alpha");
        else if(in == CF_DST_ALPHA)
            out = _T("dst_alpha");
        else if(in == CF_INV_DST_ALPHA)
            out = _T("inv_dst_alpha");
        else if(in == CF_INV_SRC_ALPHA)
            out = _T("inv_src_alpha");
        else if(in == CF_ZERO)
            out = _T("zero");
        else
            out = _T("zero");
        return out;
    }
    //------------------------------------------------------------------------
    void StrConv::conv(const String & in, CmpMode & out)
    {
        if(in == _T("equal"))
            out = CPM_EQUAL;
        else if(in == _T("not_equal"))
            out = CPM_NOT_EQUAL;
        else if(in == _T("always_fail"))
            out = CPM_ALWAYS_FAIL;
        else if(in == _T("always_pass"))
            out = CPM_ALWAYS_PASS;
        else if(in == _T("greater_equal"))
            out = CPM_GREATER_EQUAL;
        else if(in == _T("greater"))
            out = CPM_GREATER;
        else if(in == _T("less"))
            out = CPM_LESS;
        else if(in == _T("less_equal"))
            out = CPM_LESS_EQUAL;
        else if(in == _T("none"))
            out = CPM_NONE;
        else
            out = CPM_NONE;
    }
    //------------------------------------------------------------------------
    const String & StrConv::conv(CmpMode in, String & out)
    {
        if(CPM_EQUAL == in)
            out = _T("equal");
        else if(CPM_NOT_EQUAL == in)
            out = _T("not_equal");
        else if(CPM_ALWAYS_FAIL)
            out = _T("always_fail");
        else if(CPM_ALWAYS_PASS)
            out = _T("always_pass");
        else if(CPM_GREATER_EQUAL)
            out = _T("greater_equal");
        else if(CPM_GREATER)
            out = _T("greater");
        else if(CPM_LESS)
            out = _T("less");
        else if(CPM_LESS_EQUAL)
            out = _T("less_equal");
        else if(CPM_NONE)
            out = _T("none");
        else
            out = _T("none");
        return out;
    }
    //------------------------------------------------------------------------
    void StrConv::conv(const String & in, ShaderLanguage & out)
    {
        if(in == _T("glsles"))
            out = SL_GLSLES;
        else if(in == _T("cg"))
            out = SL_CG;
        else if(in == _T("asm"))
            out = SL_ASM;
        else if(in == _T("alter"))
            out = SL_ALTER;
        else if(in == _T("glsl"))
            out = SL_GLSL;
        else if(in == _T("hlsl"))
            out = SL_HLSL;
        else if(in == _T("arbvp1"))
            out = SL_ARBVP1;
        else if(in == _T("vp20"))
            out = SL_VP_20;
        else if(in == _T("vp30"))
            out = SL_VP_30;
        else if(in == _T("vp40"))
            out = SL_VP_40;
        else if(in == _T("gp4vp"))
            out = SL_GP4_VP;
        else if(in == _T("gpu_vp"))
            out = SL_GPU_VP;
        else if(in == _T("arbfp1"))
            out = SL_ARBFP1;
        else if(in == _T("fp20"))
            out = SL_FP_20;
        else if(in == _T("fp30"))
            out = SL_FP_30;
        else if(in == _T("fp40"))
            out = SL_FP_40;
        else if(in == _T("gp4fp"))
            out = SL_GP4_FP;
        else if(in == _T("gpu_fp"))
            out = SL_GPU_FP;
        else if(in == _T("nvgp4"))
            out = SL_NV_GP4;
        else if(in == _T("gp4gp"))
            out = SL_GP4_GP;
        else if(in == _T("gpu_gp"))
            out = SL_GPU_GP;
        else if(in == _T("vs_1_1"))
            out = SL_VS_1_1;
        else if(in == _T("vs_2_0"))
            out = SL_VS_2_0;
        else if(in == _T("vs_2_a"))
            out = SL_VS_2_A;
        else if(in == _T("vs_2_x"))
            out = SL_VS_2_X;
        else if(in == _T("vs_3_0"))
            out = SL_VS_3_0;
        else if(in == _T("vs_4_0"))
            out = SL_VS_4_0;
        else if(in == _T("vs_4_1"))
            out = SL_VS_4_1;
        else if(in == _T("vs_5_0"))
            out = SL_VS_5_0;
        else if(in == _T("ps_1_1"))
            out = SL_PS_1_1;
        else if(in == _T("ps_1_2"))
            out = SL_PS_1_2;
        else if(in == _T("ps_1_3"))
            out = SL_PS_1_3;
        else if(in == _T("ps_1_4"))
            out = SL_PS_1_4;
        else if(in == _T("ps_2_0"))
            out = SL_PS_2_0;
        else if(in == _T("ps_2_a"))
            out = SL_PS_2_A;
        else if(in == _T("ps_2_b"))
            out = SL_PS_2_B;
        else if(in == _T("ps_2_x"))
            out = SL_PS_2_X;
        else if(in == _T("ps_3_0"))
            out = SL_PS_3_0;
        else if(in == _T("ps_3_x"))
            out = SL_PS_3_X;
        else if(in == _T("ps_4_0"))
            out = SL_PS_4_0;
        else if(in == _T("ps_4_1"))
            out = SL_PS_4_1;
        else if(in == _T("ps_5_0"))
            out = SL_PS_5_0;
        else if(in == _T("gs_4_0"))
            out = SL_GS_4_0;
        else if(in == _T("gs_4_1"))
            out = SL_GS_4_1;
        else if(in == _T("gs_5_0"))
            out = SL_GS_5_0;
        else
            out = SL_ASM;
    }
    //------------------------------------------------------------------------
    const String & StrConv::conv(ShaderLanguage in, String & out)
    {
        String re;
        if(in == SL_GLSLES)
            out = _T("glsles");
        else if(in == SL_CG)
            out = _T("cg");
        else if(in == SL_ASM)
            out = _T("asm");
        else if(in == SL_ALTER)
            out = _T("alter");
        else if(in == SL_GLSL)
            out = _T("glsl");
        else if(in == SL_HLSL)
            out = _T("hlsl");
        else if(in == SL_ARBVP1)
            out = _T("arbvp1");
        else if(in == SL_VP_20)
            out = _T("vp20");
        else if(in == SL_VP_30)
            out = _T("vp30");
        else if(in == SL_VP_40)
            out = _T("vp40");
        else if(in == SL_GP4_VP)
            out = _T("gp4vp");
        else if(in == SL_GPU_VP)
            out = _T("gpu_vp");
        else if(in == SL_ARBFP1)
            out = _T("arbfp1");
        else if(in == SL_FP_20)
            out = _T("fp20");
        else if(in == SL_FP_30)
            out = _T("fp30");
        else if(in == SL_FP_40)
            out = _T("fp40");
        else if(in == SL_GP4_FP)
            out = _T("gp4fp");
        else if(in == SL_GPU_FP)
            out = _T("gpu_fp");
        else if(in == SL_NV_GP4)
            out = _T("nvgp4");
        else if(in == SL_GP4_GP)
            out = _T("gp4gp");
        else if(in == SL_GPU_GP)
            out = _T("gpu_gp");
        else if(in == SL_VS_1_1)
            out = _T("vs_1_1");
        else if(in == SL_VS_2_0)
            out = _T("vs_2_0");
        else if(in == SL_VS_2_A)
            out = _T("vs_2_a");
        else if(in == SL_VS_2_X)
            out = _T("vs_2_x");
        else if(in == SL_VS_3_0)
            out = _T("vs_3_0");
        else if(in == SL_VS_4_0)
            out = _T("vs_4_0");
        else if(in == SL_VS_4_1)
            out = _T("vs_4_1");
        else if(in == SL_VS_5_0)
            out = _T("vs_5_0");
        else if(in == SL_PS_1_1)
            out = _T("ps_1_1");
        else if(in == SL_PS_1_2)
            out = _T("ps_1_2");
        else if(in == SL_PS_1_3)
            out = _T("ps_1_3");
        else if(in == SL_PS_1_4)
            out = _T("ps_1_4");
        else if(in == SL_PS_2_0)
            out = _T("ps_2_0");
        else if(in == SL_PS_2_A)
            out = _T("ps_2_a");
        else if(in == SL_PS_2_B)
            out = _T("ps_2_b");
        else if(in == SL_PS_2_X)
            out = _T("ps_2_x");
        else if(in == SL_PS_3_0)
            out = _T("ps_3_0");
        else if(in == SL_PS_3_X)
            out = _T("ps_3_x");
        else if(in == SL_PS_4_0)
            out = _T("ps_4_0");
        else if(in == SL_PS_4_1)
            out = _T("ps_4_1");
        else if(in == SL_PS_5_0)
            out = _T("ps_5_0");
        else if(in == SL_GS_4_0)
            out = _T("gs_4_0");
        else if(in == SL_GS_4_1)
            out = _T("gs_4_1");
        else if(in == SL_GS_5_0)
            out = _T("gs_5_0");
        else
            out = _T("asm");
        return out;
    }
    //------------------------------------------------------------------------
}