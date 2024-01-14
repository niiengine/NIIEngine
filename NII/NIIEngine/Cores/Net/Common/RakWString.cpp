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

#include "RakWString.h"
#include "NiiNetSerializer.h"
#include <string.h>
#include <wchar.h>
#include <stdlib.h>

namespace NII
{
namespace NII_NET
{
    // From http://www.joelonsoftware.com/articles/Unicode.html
    // Only code points 128 and above are stored using 2, 3, in fact, up to 6 bytes.
    #define MAX_BYTES_PER_UNICODE_CHAR sizeof(wchar_t)
    //------------------------------------------------------------------------
    RakWString::RakWString()
    {
        c_str = 0;
        c_strCharLength = 0;
    }
    //------------------------------------------------------------------------
    RakWString::RakWString(const RakString & right)
    {
        c_str = 0;
        c_strCharLength = 0;
        *this = right;
    }
    //------------------------------------------------------------------------
    RakWString::RakWString(const char * input)
    {
        c_str = 0;
        c_strCharLength = 0;
        *this = input;
    }
    //------------------------------------------------------------------------
    RakWString::RakWString(const wchar_t * input)
    {
        c_str = 0;
        c_strCharLength = 0;
        *this = input;
    }
    //------------------------------------------------------------------------
    RakWString::RakWString(const RakWString & right)
    {
        c_str = 0;
        c_strCharLength = 0;
        *this = right;
    }
    //------------------------------------------------------------------------
    RakWString::~RakWString()
    {
        N_free(c_str);
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator = (const RakWString & right)
    {
        Clear();
        if (right.IsEmpty())
            return *this;
        c_str = (wchar_t *)N_alloc((right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        if (!c_str)
        {
            c_strCharLength=0;
            return *this;
        }
        c_strCharLength = right.GetLength();
        memcpy(c_str,right.C_String(),(right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR);

        return *this;
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator = (const RakString & right)
    {
        return *this = right.C_String();
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator = (const wchar_t * const str)
    {
        Clear();
        if (str == 0)
            return *this;
        c_strCharLength = wcslen(str);
        if (c_strCharLength == 0)
            return *this;
        c_str = (wchar_t *) N_alloc((c_strCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        if (!c_str)
        {
            c_strCharLength = 0;
            return *this;
        }
        wcscpy(c_str,str);

        return *this;
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator = (wchar_t * str)
    {
        *this = ( const wchar_t * const) str;
        return *this;
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator = (const char * const str)
    {
        Clear();

    // Not supported on android
    #if !defined(ANDROID)
        if (str == 0)
            return *this;
        if (str[0] == 0)
            return *this;

        c_strCharLength = mbstowcs(NULL, str, 0);
        c_str = (wchar_t *) N_alloc( (c_strCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        if (!c_str)
        {
            c_strCharLength=0;
            return *this;
        }

        c_strCharLength = mbstowcs(c_str, str, c_strCharLength+1);
        if (c_strCharLength == (size_t) (-1))
        {
            N_printf("Couldn't convert string--invalid multibyte character.\n");
            Clear();
            return *this;
        }
    #else
        // mbstowcs not supported on android
        N_assert("mbstowcs not supported on Android" && 0);
    #endif // defined(ANDROID)

        return *this;
    }
    //------------------------------------------------------------------------
    RakWString& RakWString::operator = (char * str)
    {
        *this = ( const char * const) str;
        return *this;
    }
    //------------------------------------------------------------------------
    RakWString& RakWString::operator +=(const RakWString & right)
    {
        if (right.IsEmpty())
            return *this;
        size_t newCharLength = c_strCharLength + right.GetLength();
        wchar_t *newCStr;
        bool isEmpty = IsEmpty();
        if (isEmpty)
            newCStr = (wchar_t *) N_alloc((newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        else
            newCStr = (wchar_t *)N_realloc(c_str, (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR); // 需要修改
        if (!newCStr)
        {
            return *this;
        }
        c_str = newCStr;
        c_strCharLength = newCharLength;
        if (isEmpty)
        {
            memcpy(newCStr,right.C_String(),(right.GetLength() + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        }
        else
        {
            wcscat(c_str, right.C_String());
        }

        return *this;
    }
    //------------------------------------------------------------------------
    RakWString & RakWString::operator += (const wchar_t * const right)
    {
        if (right==0)
            return *this;
        size_t rightLength = wcslen(right);
        size_t newCharLength = c_strCharLength + rightLength;
        wchar_t *newCStr;
        bool isEmpty = IsEmpty();
        if (isEmpty)
            newCStr = (wchar_t *) N_alloc((newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        else
            newCStr = (wchar_t *)N_realloc(c_str, (newCharLength + 1) * MAX_BYTES_PER_UNICODE_CHAR); // 需要修改
        if (!newCStr)
        {
            return *this;
        }
        c_str = newCStr;
        c_strCharLength = newCharLength;
        if (isEmpty)
        {
            memcpy(newCStr,right,(rightLength + 1) * MAX_BYTES_PER_UNICODE_CHAR);
        }
        else
        {
            wcscat(c_str, right);
        }

        return *this;
    }
    //------------------------------------------------------------------------
    RakWString& RakWString::operator += (wchar_t * right)
    {
        return *this += (const wchar_t * const) right;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator==(const RakWString & right) const
    {
        if (GetLength()!=right.GetLength())
            return false;
        return wcscmp(c_str,right.C_String())==0;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator < (const RakWString & right) const
    {
        return wcscmp(c_str,right.C_String())<0;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator <= (const RakWString & right) const
    {
        return wcscmp(c_str,right.C_String())<=0;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator > (const RakWString & right) const
    {
        return wcscmp(c_str,right.C_String())>0;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator >= (const RakWString & right) const
    {
        return wcscmp(c_str,right.C_String())>=0;
    }
    //------------------------------------------------------------------------
    bool RakWString::operator!=(const RakWString & right) const
    {
        if (GetLength() != right.GetLength())
            return true;
        return wcscmp(c_str,right.C_String())!=0;
    }
    //------------------------------------------------------------------------
    void RakWString::Set(wchar_t * str)
    {
        *this = str;
    }
    //------------------------------------------------------------------------
    bool RakWString::IsEmpty(void) const
    {
        return GetLength() == 0;
    }
    //------------------------------------------------------------------------
    size_t RakWString::GetLength(void) const
    {
        return c_strCharLength;
    }
    //------------------------------------------------------------------------
    Nul RakWString::ToInteger(const RakWString & rs)
    {
        Nul hash = 0;
        int c;

        const char * str = (const char *)rs.C_String();
        size_t i;
        for (i = 0; i < rs.GetLength()*MAX_BYTES_PER_UNICODE_CHAR*sizeof(wchar_t); ++i)
        {
            c = *str++;
            hash = c + (hash << 6) + (hash << 16) - hash;
        }

        return hash;
    }
    //------------------------------------------------------------------------
    int RakWString::StrCmp(const RakWString & right) const
    {
        return wcscmp(C_String(), right.C_String());
    }
    //------------------------------------------------------------------------
    int RakWString::StrICmp(const RakWString & right) const
    {
    #if N_PLAT == N_PLAT_WIN32
        return _wcsicmp(C_String(), right.C_String());
    #else
        // Not supported
        return wcscmp(C_String(), right.C_String());
    #endif
    }
    //------------------------------------------------------------------------
    void RakWString::Clear(void)
    {
        N_free(c_str);
        c_str = 0;
        c_strCharLength = 0;
    }
    //------------------------------------------------------------------------
    void RakWString::Printf(void)
    {
        printf("%ls", C_String());
    }
    //------------------------------------------------------------------------
    void RakWString::FPrintf(FILE * fp)
    {
        fprintf(fp,"%ls", C_String());
    }
    //------------------------------------------------------------------------
    void RakWString::export(NetSerializer * out) const
    {
        export(c_str, out);
    }
    //------------------------------------------------------------------------
    void RakWString::export(const wchar_t * const in, NetSerializer * out)
    {
        size_t mbByteLength = wcslen(in);
        out->write((Nui16)mbByteLength);
        for (Nui32 i=0; i < mbByteLength; ++i)
        {
            Nui16 t;
            t = (Nui16) in[i];
            // Force endian swapping, and write to 16 bits
            out->write(t);
        }
    }
    //------------------------------------------------------------------------
    bool RakWString::import(NetSerializer * in)
    {
        Clear();

        size_t mbByteLength;
        in->read((Nui16)mbByteLength);
        if (mbByteLength > 0)
        {
            c_str = (wchar_t *)N_alloc(mbByteLength+1);
            c_strCharLength = mbByteLength;
            for (Nui32 i = 0; i < mbByteLength; ++i)
            {
                Nui16 t;
                // Force endian swapping, and read 16 bits
                in->read(t);
                c_str[i] = t;
            }
            c_str[mbByteLength] = 0;
            return true;
        }
        else
        {
            return true;
        }
    }
    //------------------------------------------------------------------------
    bool RakWString::import(wchar_t * out, NetSerializer * in)
    {
        size_t mbByteLength;
        in->read((Nui16)mbByteLength);
        if (mbByteLength > 0)
        {
            NCount i;
            for(i = 0; i < mbByteLength; ++i)
            {
                Nui16 t;
                // Force endian swapping, and read 16 bits
                in->read(t);
                out[i] = t;
            }
            out[mbByteLength] = 0;
            return true;
        }
        else
        {
            wcscpy(out,L"");
        }
        return true;
    }
    //------------------------------------------------------------------------
}
}
/*
NetSerializer bsTest;
RakWString testString("cat"), testString2;
testString = "Hllo";
testString = L"Hello";
testString += L" world";
testString2 += testString2;
RakWString ts3(L" from here");
testString2+=ts3;
RakWString ts4(L" 222");
testString2=ts4;
RakString rs("rakstring");
testString2+=rs;
testString2=rs;
bsTest.write(L"one");
bsTest.write(testString2);
bsTest.seekRead(0);
RakWString ts5, ts6;
wchar_t buff[99];
wchar_t *wptr = (wchar_t*)buff;
bsTest.read(wptr);
bsTest.read(ts5);
*/
