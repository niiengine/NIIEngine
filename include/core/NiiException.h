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

#ifndef _NII_EXCEPTION_H_
#define _NII_EXCEPTION_H_

#include "NiiPreInclude.h"
#include "NiiString.h"
#if N_COMPILER == N_CPP_MSVC
#pragma warning(disable:4275)
#endif
#include <exception>

namespace NII
{
    /** 异常基类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Exception : public std::exception
    {
    public:
        Exception(const String & desc, const String & src);
        Exception(const String & desc, const String & src, const Ntchar * file, NIIl l, const Ntchar * func);
        Exception(const Exception & o);

        /// 需要兼容std::exception
        virtual ~Exception() throw();

        /// 赋值运算符
        void operator = (const Exception & o);

        /** 获取发生行号
        @@version NIIEngine 3.0.0
        */
        inline NIIl getLine() const                { return mLine; }

        /** 获取发生文件
        @version NIIEngine 3.0.0
        */
        inline const String & getFile() const      { return mFile; }

        /** 获取发生函数
        @version NIIEngine 3.0.0
        */
        inline const String & getSource() const    { return mSrc; }

        /** 获取错误描述
        @version NIIEngine 3.0.0
        */
        inline const String & getVerbose() const   { return mDescrip; }

        /** 返回描述这个错误的一个完整字符串
        @version NIIEngine 3.0.0
        */
        const String & getErrorVerbose() const;

        /** 重写 std::exception::what
        @version NIIEngien 3.0.0
        */
        const char * what() const throw();
    protected:
        NIIl mLine;
        String mType;
        String mDescrip;
        String mSrc;
        String mFile;
        mutable String mFullDescrip;
    };

#ifndef N_EXCEPT_DEF
    #define N_EXCEPT_DEF(name)                                                  \
    class _EngineAPI name##Exception : public NII::Exception                    \
    {                                                                           \
    public :                                                                    \
        name##Exception(const String & desc) :                                  \
            Exception(desc, _T(#name##"Exception")) {}                          \
        name##Exception(const String & desc,  const Ntchar * file, NIIl line,   \
            const Ntchar * func) : Exception(desc, _T(#name##"Exception"), file, line, func) {}    \
    };
#endif

#if _UNICODE
#define __STR2WSTR(str) L##str
#define _STR2WSTR(str) __STR2WSTR(str)
#define _N_FUNCTION__ _STR2WSTR(__FUNCTION__)
#define _N_FILE__ _STR2WSTR(__FILE__)
#else
#define _N_FUNCTION__ __FUNCTION__
#define _N_FILE__ __FILE__
#endif

#ifndef N_EXCEPT
    #define N_EXCEPT(name, desc) throw name##Exception(desc, N_FLF)
#endif

    N_EXCEPT_DEF(IO);
    N_EXCEPT_DEF(UnImpl);
    N_EXCEPT_DEF(Internal);
    N_EXCEPT_DEF(Runtime);
    N_EXCEPT_DEF(NotExist);
    N_EXCEPT_DEF(UniqueRepeat);
    N_EXCEPT_DEF(InvalidFile);
    N_EXCEPT_DEF(InvalidState);
    N_EXCEPT_DEF(InvalidParam);
    N_EXCEPT_DEF(InvalidRequest);
    N_EXCEPT_DEF(RenderingAPI);
    N_EXCEPT_DEF(UI);
    N_EXCEPT_DEF(Script);
}
#if N_COMPILER == N_CPP_MSVC
#pragma warning(default:4275)
#endif
#endif