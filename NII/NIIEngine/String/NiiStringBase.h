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
#define Nstrstr     wcsstr
#define Nstrcmp     wcscmp
#define Nstrchr     wcschr
#define Nstrcspn    wcscspn
#define Nstrlen     wcslen
#define Nstrcat     wcscat    
#define Nstrcpy     wcscpy
#define Nstrncpy    wcsncpy
#define Nstprintf_s swprintf_s
#define Nstprintf   swprintf
#define Nfopen_s    _wfopen_s
#define Nfopen      _wfopen
#define Nfputs      fputws
#define Nfinddata   _wfinddata_t
#define Nfindfirst  _wfindfirst
#define Nfindnext   _wfindnext
#define Nstat       _wstat
#define Nstruct_stat struct _stat64i32
#define Nostream    std::wostream
#define Nofstream   std::wofstream
#define Nifstream   std::wifstream
#define Nfstream    std::wfstream
#define Nistringstream std::wistringstream
#define Nostringstream std::wostringstream
#define Ncout       std::wcout
#define Ncerr       std::wcerr
#define Nmemset     ::wmemset
#define Nmemcpy     ::wmemcpy
#define Ngetcwd     _wgetcwd
#define Naccess     _waccess
#define Nchmod      _wchmod
#define Nremove     ::_wremove
#define Nsnlen      std::wcsnlen
#define Nstol       std::wcstol
#ifdef _tgetenv
#define Ngetenv     _wgetenv
#else
#define Ngetenv     getenv
#endif
#ifdef WIN32
#define Nsntprintf  _snwprintf
#define Nvsntprintf _vsntprintf
#define Nmkdir(x)   _wmkdir(x)
#else
#define Nsntprintf  snwprintf
#define Nvsntprintf vsntprintf
#define Nmkdir(x)   wmkdir(x, 0744)
#endif

#else
    
#define Nprintf     printf
#define Nscanf_s    scanf
#define Nsscanf     sscanf
#define Natoi       atoi
#define Nstrstr     strstr
#define Nstrcmp     strcmp
#define Nstrchr     strchr
#define Nstrcspn    strcspn
#define Nstrlen     strlen
#define Nstrcat     strcat
#define Nstrcpy     strcpy
#define Nstrncpy    strncpy
#define Nstprintf_s sprintf_s
#define Nstprintf   sprintf
#define Nfopen_s    fopen_s
#define Nfopen      fopen
#define Nfputs      fputs
#define Nfinddata   _finddata_t
#define Nfindfirst  _findfirst
#define Nfindnext   _findnext
#define Nstat       _stat
#define Nstruct_stat struct _stat64i32
#define Nostream    std::ostream
#define Nofstream   std::ofstream
#define Nifstream   std::ifstream
#define Nfstream    std::fstream
#define Nistringstream std::istringstream
#define Nostringstream std::ostringstream
#define Ncout       std::cout
#define Ncerr       std::cerr
#define Nmemset     ::memset
#define Nmemcpy     ::memcpy
#define Ngetenv     getenv
#define Naccess     access
#define Nchmod      chmod
#define Nremove     ::remove
#define Nsnlen      std::strnlen
#define Nstol       std::strtol
#ifdef WIN32
#define Nsntprintf  _snprintf
#define Nvsntprintf _vsnprintf
#define Nmkdir(x)   _mkdir(x)
#define Ngetcwd     _getcwd
#else
#define Nsntprintf  snprintf
#define Nvsntprintf vsnprintf
#define Nmkdir(x)   mkdir(x, 0744)
#define Ngetcwd     getcwd
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
#if NII_StringCustomAlloc
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

typedef std::basic_string_view<char, std::char_traits<char> > _VStringViewBase;
typedef std::basic_string_view<wchar_t, std::char_traits<wchar_t> > _WStringViewBase;
    
typedef _VStringViewBase VStringView;
typedef _WStringViewBase WStringView;
typedef _VStringBase VString;
typedef _WStringBase WString;

typedef _VStringStreamBase VStringStream;
typedef _WStringStreamBase WStringStream;

#if NII_STRING
    #define NIILW(T) L##T
    #define _NIILW(T) NIILW(T)
    typedef _WStringViewBase    StringView;
    typedef _WStringBase        String;
    typedef _WStringStreamBase  StringStream;
#else
    #define NIILW(T) T
    #define _NIILW(T) T
    typedef _VStringViewBase    StringView;
    typedef _VStringBase        String;
    typedef _VStringStreamBase  StringStream;
#endif

    typedef vector<String>::type        StringList;
    typedef vector<WString>::type       WStringList;
    typedef vector<VString>::type       VStringList;
    typedef vector<StringList>::type    StringGroupList;
    typedef vector<WStringList>::type   WStringGroupList;
    typedef vector<VStringList>::type   VStringGroupList;
    
    typedef set<String>::type       StringSet;
    typedef set<WString>::type      WStringSet;
    typedef set<VString>::type      VStringSet;
    typedef set<StringSet>::type    StringGroupSet;
    typedef set<WStringSet>::type   WStringGroupSet;
    typedef set<VStringSet>::type   VStringGroupSet;
    
    typedef map<String, String>::type StringMap;
    typedef map<String, VString>::type VStringMap;
    typedef map<String, WString>::type WStringMap;
    
    typedef map<Nid, String>::type StringIdMap;
    typedef map<Nid, VString>::type VStringIdMap;
    typedef map<Nid, WString>::type WStringIdMap;
}
#endif