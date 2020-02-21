/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_STRING_BASE_H_
#define _NII_STRING_BASE_H_

#include <string>
#if (N_PLAT == N_PLAT_WIN32 || N_PLAT == N_PLAT_WIN8_PHONE || N_PLAT == N_PLAT_WINRT)
#include <tchar.h>
#else
#include <locale.h>
#endif

#ifdef _UNICODE
#define Nprintf     wprintf    
#define Nscanf_s    wscanf
#define Nsscanf     swscanf
#define Natoi       _wtoi
#define Nstrcmp     wcscmp
#define Nstrchr     wcschr
#define Nstrcspn    wcscspn
#define Nslen       wcslen
#define Nstrcat     wcscat    
#define Nstrcpy     wcscpy
#define Nstprintf   swprintf
#define Nfopen      _wfopen
#define Nfputs      fputws
#define Nfinddata   _wfinddata_t
#define Nfindfirst  _wfindfirst
#define Nfindnext   _wfindnext
#define Nremove     ::_wremove
#define Nstat       _wstat
#define Nstruct_stat struct _stat64i32
#define Nostream    std::wostream
#define Nofstream   std::wofstream
#define Nistringstream std::wistringstream
#define Nostringstream std::wostringstream
#define Ncout       std::wcout
#define Ncerr       std::wcerr
#define Nmemset     ::wmemset
#define Nmemcpy     ::wmemcpy
#ifdef WIN32
#define Nsntprintf  _snwprintf
#define Nvsntprintf _vsntprintf
#else
#define Nsntprintf  snwprintf
#define Nvsntprintf vsntprintf
#endif
#else
#define Nprintf     printf
#define Nscanf_s    scanf
#define Nsscanf     sscanf
#define Natoi       atoi
#define Nstrcmp     strcmp
#define Nstrchr     strchr
#define Nstrcspn    strcspn
#define Nslen       strlen
#define Nstrcat     strcat
#define Nstrcpy     strcpy
#define Nstprintf   sprintf
#define Nfopen      fopen
#define Nfputs      fputs
#define Nfinddata   _finddata_t
#define Nfindfirst  _findfirst
#define Nfindnext   _findnext
#define Nremove     ::remove
#define Nstat       _stat
#define Nstruct_stat struct _stat64i32
#define Nostream    std::ostream
#define Nofstream   std::ofstream
#define Nistringstream std::istringstream
#define Nostringstream std::ostringstream
#define Ncout       std::cout
#define Ncerr       std::cerr
#define Nmemset     ::memset
#define Nmemcpy     ::memcpy
#ifdef WIN32
#define Nsntprintf  _snprintf
#define Nvsntprintf _vsnprintf
#else
#define Nsntprintf  snprintf
#define Nvsntprintf vsnprintf
#endif
#endif

#ifdef WIN32
#define NsnprintfW  _snwprintf
#define NsnprintfV  _snprintf
#else
#define NsnprintfW  snwprintf
#define NsnprintfV  snprintf
#endif

#include <locale.h>  
#include <locale>  

using std::locale;

namespace NII
{
#if NII_StringCustomMemAlloc
    typedef std::basic_string<char, std::char_traits<char>, StlAlloc<char, N_Alloc> > _VStringBase;
    typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, StlAlloc<wchar_t, N_Alloc> > _WStringBase;

    typedef std::basic_stringstream<char, std::char_traits<char>,StlAlloc<char, N_Alloc> > _VStringStreamBase;
    typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>,StlAlloc<wchar_t, N_Alloc> > _WStringStreamBase;

    #define StdAllocTemp(T) std::basic_string<T, std::char_traits<T>, std::allocator<T> >
    #define CustomAllocTemp(T) std::basic_string<T, std::char_traits<T>, StlAlloc<T, N_Alloc> >

    template<typename T> bool operator <(const CustomAllocTemp(T) & l, const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) < 0;
    }
    template<typename T> bool operator <(const StdAllocTemp(T) & l, const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) < 0;
    }
    template<typename T> bool operator <=(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) <= 0;
    }
    template<typename T> bool operator <=(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) <= 0;
    }
    template<typename T> bool operator >(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) > 0;
    }
    template<typename T> bool operator >(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) > 0;
    }
    template<typename T> bool operator >=(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) >= 0;
    }
    template<typename T> bool operator >=(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) >= 0;
    }

    template<typename T> bool operator ==(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) == 0;
    }
    template<typename T> bool operator ==(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) == 0;
    }

    template<typename T> bool operator !=(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) != 0;
    }
    template<typename T> bool operator !=(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return l.compare(0,l.length(), o.c_str(), o.length()) != 0;
    }

    template<typename T> CustomAllocTemp(T) operator +=(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return CustomAllocTemp(T)(l) += o.c_str();
    }

    template<typename T> CustomAllocTemp(T) operator +=(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return CustomAllocTemp(T)(l.c_str()) += o.c_str();
    }

    template<typename T> CustomAllocTemp(T) operator +(const CustomAllocTemp(T) & l,const StdAllocTemp(T) & o)
    {
        return CustomAllocTemp(T)(l) += o.c_str();
    }

    template<typename T> CustomAllocTemp(T) operator +(const StdAllocTemp(T) & l,const CustomAllocTemp(T) & o)
    {
        return CustomAllocTemp(T)(l.c_str()) += o.c_str();
    }

    template<typename T> CustomAllocTemp(T) operator +(const T* l,const CustomAllocTemp(T) & o)
    {
        return CustomAllocTemp(T)(l) += o;
    }

    #undef StdAllocTemp
    #undef CustomAllocTemp
#else
    typedef std::string _VStringBase;
    typedef std::wstring _WStringBase;

    typedef std::basic_stringstream<char, std::char_traits<char>, std::allocator<char> > _VStringStreamBase;
    typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > _WStringStreamBase;
#endif

typedef _VStringBase VString;
typedef _WStringBase WString;

typedef _VStringStreamBase VStringStream;
typedef _WStringStreamBase WStringStream;

#if NII_STRING
    #define NIILW(T) L##T
    #define _NIILW(T) NIILW(T)
    typedef _WStringBase        String;
    typedef _WStringStreamBase  StringStream;
#else
    #define NIILW(T) T
    #define _NIILW(T) T
    typedef _VStringBase        String;
    typedef _VStringStreamBase  StringStream;
#endif

    typedef vector<String>::type StringList;
    typedef vector<WString>::type WStringList;
    typedef vector<VString>::type VStringList;
}
#endif