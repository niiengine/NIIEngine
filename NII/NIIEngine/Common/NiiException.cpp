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
#include "NiiException.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"

#ifdef __BORLANDC__
    #include <stdio.h>
#endif

#if defined(_MSC_VER)
#include <windows.h>
#include <dbghelp.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__)
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <cstdlib>
#endif

namespace NII
{
    //------------------------------------------------------------------------
    static void dumpBacktrace(NCount frames)
    {
    #if defined(_DEBUG) || defined(DEBUG)
    #if defined(_MSC_VER)
        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES);

        if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
            return;

        HANDLE thread = GetCurrentThread();

        CONTEXT context;
        RtlCaptureContext(&context);

        STACKFRAME64 stackframe;
        ZeroMemory(&stackframe, sizeof(stackframe));
        stackframe.AddrPC.Mode = AddrModeFlat;
        stackframe.AddrStack.Mode = AddrModeFlat;
        stackframe.AddrFrame.Mode = AddrModeFlat;

    #if _M_IX86
        stackframe.AddrPC.Offset = context.Eip;
        stackframe.AddrStack.Offset = context.Esp;
        stackframe.AddrFrame.Offset = context.Ebp;
        DWORD machine_arch = IMAGE_FILE_MACHINE_I386;
    #elif _M_X64
        stackframe.AddrPC.Offset = context.Rip;
        stackframe.AddrStack.Offset = context.Rsp;
        stackframe.AddrFrame.Offset = context.Rbp;
        DWORD machine_arch = IMAGE_FILE_MACHINE_AMD64;
    #endif

        char symbol_buffer[1024];
        ZeroMemory(symbol_buffer, sizeof(symbol_buffer));
        PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbol_buffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = sizeof(symbol_buffer) - sizeof(SYMBOL_INFO);

        LogManager * logger = LogManager::getOnlyPtr();
        logger->log(_T("========== Start of Backtrace =========="));

        NCount frame_no = 0;
        while (StackWalk64(machine_arch, GetCurrentProcess(), thread, &stackframe,
            &context, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0) &&
                stackframe.AddrPC.Offset)
        {
            symbol->Address = stackframe.AddrPC.Offset;
            DWORD64 displacement = 0;
            char signature[256];

            if (SymFromAddr(GetCurrentProcess(), symbol->Address, &displacement, symbol))
                UnDecorateSymbolName(symbol->Name, signature, sizeof(signature), UNDNAME_COMPLETE);
            else
                sprintf_s(signature, sizeof(signature), "%llx", symbol->Address);

            IMAGEHLP_MODULE64 modinfo;
            modinfo.SizeOfStruct = sizeof(modinfo);

            const BOOL have_image_name =
                SymGetModuleInfo64(GetCurrentProcess(), symbol->Address, &modinfo);

            char outstr[512];
            sprintf_s(outstr, sizeof(outstr), "#%d %s +%#llx (%s)",
                frame_no, signature, displacement,
                (have_image_name ? modinfo.LoadedImageName : "????"));

            String temp;
            StrConv::conv(outstr, temp);
            logger->log(temp, LL_Fatal);

            if (++frame_no >= frames)
                break;

            if (!stackframe.AddrReturn.Offset)
                break;
        }

        logger->log(_T("==========  End of Backtrace  =========="));

        SymCleanup(GetCurrentProcess());
    #elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__)
        void * buffer[frames];
        const int received = backtrace(&buffer[0], frames);

        LogManager * logger = LogManager::getOnlyPtr();

        logger->log(_T("========== Start of Backtrace =========="));

        for (int i = 0; i < received; ++i)
        {
            char outstr[512];
            Dl_info info;
            if (dladdr(buffer[i], &info))
            {
                if (!info.dli_sname)
                    Nsntprintf(outstr, 512, "#%d %p (%s)", i, buffer[i], info.dli_fname);
                else
                {
                    ptrdiff_t offset = static_cast<char*>(buffer[i]) -
                                       static_cast<char*>(info.dli_saddr);

                    int demangle_result = 0;
                    char * demangle_name = abi::__cxa_demangle(info.dli_sname, 0, 0, &demangle_result);
                    Nsntprintf(outstr, 512, "#%d %s +%#tx (%s)",
                        i, demangle_name ? demangle_name : info.dli_sname, offset, info.dli_fname);
                    std::free(demangle_name);
                }
            }
            else
                Nsntprintf(outstr, 512, "#%d --- error ---", i);

            logger->logEvent(outstr);
        }

        logger->log(_T("==========  End of Backtrace  =========="));
    #endif
    #endif
    }
    //-----------------------------------------------------------------------
    Exception::Exception(const String & desc, const String & src) :
        mLine(0),
        mDescrip(desc),
        mSrc(src)
    {
    }
    //-----------------------------------------------------------------------
    Exception::Exception(const String & desc, const String & src,
         const Ntchar * file, NIIl line, const Ntchar * type) :
            mType(type),
            mDescrip(desc),
            mSrc(src),
            mFile(file),
            mLine(line)
    {
        // 记录此错误，从调试中标记，因为它可以被捕获和忽略
        if(LogManager::getOnlyPtr())
        {
            N_Only(Log).log(getErrorVerbose(), LL_Fatal, true);
        }
    }
    //------------------------------------------------------------------------
    Exception::Exception(const Exception & o) :
        mLine(o.mLine),
        mType(o.mType),
        mDescrip(o.mDescrip),
        mSrc(o.mSrc),
        mFile(o.mFile)
    {
    }
    //-------------------------------------------------------------------------
    void Exception::operator = (const Exception & o)
    {
        mDescrip = o.mDescrip;
        mSrc = o.mSrc;
        mFile = o.mFile;
        mLine = o.mLine;
        mType = o.mType;
    }
    //-------------------------------------------------------------------------
    Exception::~Exception() throw()
    {
    }
    //-------------------------------------------------------------------------
    const String & Exception::getErrorVerbose() const
    {
        if(mFullDescrip.empty())
        {
            StringStream desc;

            desc <<  _T("NII (EXCEPTION: ") << mType << _T("): ") << mDescrip << _T(" in ") << mSrc;

            if( mLine > 0 )
            {
                desc << _T(" at: ") << mFile << _T("(line ") << mLine << ")";
            }

            mFullDescrip = desc.str();
        }
        return mFullDescrip;
    }
    //-------------------------------------------------------------------------
    const char * Exception::what() const throw()
    {
        if(mFullDescrip.empty())
        {
            StringStream desc;

            desc <<  _I18n("NII EXCEPTION: ") << mType << "): " << mDescrip << _I18n(" in ") << mSrc;

            if(mLine > 0)
            {
                desc << _I18n(" at: ") << mFile << _I18n("(line: ") << mLine << ")";
            }

            mFullDescrip = desc.str();
        }
#if NII_STRING
        static VString temp;
        DataStream::ws2s(mFullDescrip, temp);
        return temp.c_str();
#else
        return mFullDescrip.c_str();
#endif
    }
    //-------------------------------------------------------------------------
}