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

#include "ExceptionManager.h"
#include "SimpleSymbolEngine.h"
#include "UILayoutEditorFrame.h"
#include "NiiLogManager.h"
#include "UILayoutConfig.h"
#include "StrHelper.h"
#include "app.h"
#include <wx/log.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>

using namespace NII;
namespace gsgs
{
    //-----------------------------------------------------------------------
    void LogError(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Display error message to user in a modal or modeless message box (with an error icon) 
#if USE_MODAL_MESSAGE_BOXES
    // Create a truly modal message box with an error icon to display the message 
        (void)ModalMessageBox(msg, wxT("LayoutEditor Error"), wxOK | wxICON_ERROR);
#else
    // Again, let wxWidgets process the error message and pop up a message box (with an error icon) 
    // to inform the user about it (default behavior). Works in both Debug and Release builds.
    // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
    // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
    // to pass the variable arguments to it.
        wxVLogError(szFormat, argptr);
#endif // USE_MODAL_MESSAGE_BOXES

        // Insert previous string into log as an error message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Error);

        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogSilentError(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Insert previous string into log as an error message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        // NOTE2: We are only logging to our own log file and ignoring the wxLog*
        // family of functions, since we want this to be silent
        N_Only(Log).log(StrHelper::ToNIIString(msg).c_str(), LL_Error);

        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogDebug(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Again, we let wxWidgets process the debug message.  In its default behavior it only does anything 
        // at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
        // to nothing in release mode (otherwise). 
        // NOTE: Under Windows, you must either run the program under debugger (e.g. in Visual 
        // Studio the message is output to the console output window) or use a 3rd party 
        // program such as DebugView to actually see the debug output. Under Unix the 
        // output goes to stderr so you don't need any tools to see it.
        // NOTE2: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
        // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
        // to pass the variable arguments to it.
        wxVLogDebug(szFormat, argptr);

        // Insert previous string into log as a standard message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Debug);

        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogSilentDebug(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Insert previous string into log as a standard message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        // NOTE2: We are only logging to our own log file and ignoring the wxLog*
        // family of functions, since we want this to be silent
        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Debug);

        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogWarning(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Display warning message to user in a modal or modeless message box (with an information icon) 
#if USE_MODAL_MESSAGE_BOXES
    // Create a truly modal message box with an information icon to display the message 
        (void)ModalMessageBox(msg, wxT("UI Layout Editor Warning"), wxOK | wxICON_EXCLAMATION);
#else
    // Again, let wxWidgets process the warning message and pop up a message box (with 
    // OK button and information icon) to inform the user about it (default behavior). 
    // Works in both Debug and Release builds.
    // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
    // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
    // to pass the variable arguments to it.
        wxVLogWarning(szFormat, argptr);
#endif // USE_MODAL_MESSAGE_BOXES

        // Insert previous string into log as an warning message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 

        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Warning);
        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogSilentWarning(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Insert previous string into log as an warning message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        // NOTE2: We are only logging to our own log file and ignoring the wxLog*
        // family of functions, since we want this to be silent

        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Warning);
        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogMessage(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Display normal message to user in a modal or modeless message box (with an information icon) 
#if USE_MODAL_MESSAGE_BOXES
    // Create a truly modal message box with an information icon to display the message 
        (void)ModalMessageBox(msg, wxT("LayoutEditor Info"), wxOK | wxICON_INFORMATION);
#else
    // Again, let wxWidgets process the normal message and pop up a message box (with OK 
    // button and information icon) to inform the user about it (default behavior). Works 
    // in both Debug and Release builds.
    // NOTE: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
    // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
    // to pass the variable arguments to it.
        wxVLogMessage(szFormat, argptr);
#endif // USE_MODAL_MESSAGE_BOXES

        // Insert previous string into log as an informative message (LayoutEditor.log)
        // NOTE: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Info);
        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogTrace(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Let wxWidgets process the trace message.  In its default behavior it only does anything 
        // at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands 
        // to nothing in release mode (otherwise). Behavior is the same as for debug messages 
        // (see LogDebug).
        // NOTE: Under Windows, you must either run the program under debugger (e.g. in Visual 
        // Studio the message is output to the console output window) or use a 3rd party 
        // program such as DebugView to actually see the debug output. Under Unix the 
        // output goes to stderr so you don't need any tools to see it.
        // NOTE2: Fortunately, the wxLog* family of functions have siblings named wxVLog* that 
        // accept the variadic list as option. Otherwise, it would be impossible in standard C/C++ 
        // to pass the variable arguments to it.
        // NOTE3: Unconditional log trace has been suppressed in recent wxWidgets versions, so
        // we use the log debug variant which is equivalent
        wxVLogDebug(szFormat, argptr);

        // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Insert previous string into log as a insane message (LayoutEditor.log)
        // NOTE: Here lies the difference with debug messages (see LogDebug & LogSilentDebug)
        // NOTE2: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 

        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Trace);
        // Cleanup
        va_end(argptr);
    }
    //-----------------------------------------------------------------------
    void LogSilentTrace(const wxChar *szFormat, ...)
    {
        // Make the variadic list point to the first argument after szFormat.
        va_list argptr;
        va_start(argptr, szFormat);

        // Again we let wxWidgets do the heavy work and parse the arguments supplied into a
        // string.
        wxString msg;
        (void)msg.PrintfV(szFormat, argptr);

        // Insert previous string into log as a insane message (LayoutEditor.log)
        // NOTE: Here lies the difference with debug messages (see LogDebug & LogSilentDebug)
        // NOTE2: If this function is called before GUI is properly initialized,
        // the GUI logger will be null (and so will the returned pointer). We need
        // to ignore logging in that case. A particular example is when the ini 
        // file is not found (first time LayoutEditor is run) and a warning is 
        // issued. 
        // NOTE2: We are only logging to our own log file and ignoring the wxLog*
        // family of functions, since we want this to be silent

        N_Only(Log).log(StrHelper::ToNIIString(msg), LL_Trace);
        // Cleanup
        va_end(argptr);
    }
    //------------------------------------------------------------------------
    int ExceptionMessageBox(const wxString& text, const wxString& title, long style)
    {
#if USE_MODAL_MESSAGE_BOXES
        return ModalMessageBox(text, title, style, true);
#else
        return SafeMessageBox(text, title, style);
#endif // USE_MODAL_MESSAGE_BOXES
    }
    //------------------------------------------------------------------------
    int SafeMessageBox(const wxString& text, const wxString& title, long style)
    {
        // Check OS
#ifdef __WINDOWS__
    // In windows we display a native message box which is safe to call
    // even if the application crashed
        return ::MessageBox(NULL, text, title, wx_static_cast(UINT, style));
#else
    // We're not in Windows, so print message to stderr 
        wxFprintf(stderr, _T("%s: %s\n"), title.c_str(), text.c_str());
        fflush(stderr);
        return LE_MB_RESULT_CANCEL;
#endif
    }
    //------------------------------------------------------------------------
    int ModalMessageBox(const wxString& text, const wxString& title, long style, const bool safe)
    {
        // Disable all windows in the application
        // NOTE: This is done in the class's constructor
        wxWindowDisabler *disableAllWindows = new wxWindowDisabler();

        // Display message box in normal (use wxWidgets) or safe mode (use native) and wait 
        // for user input
        int retValue;
        if (safe)
            retValue = SafeMessageBox(text, title, style);
        else
            retValue = wxMessageBox(text, title, style);

        // The user closed the message box, so we can reactivate all the windows
        // NOTE: This is done in the class's destructor
        delete disableAllWindows;
        disableAllWindows = NULL;

        // Restore focus to the main frame (if active). This is necessary, 
        // otherwise the application could appear minimized.
        // NOTE: If this function is called during startup or shutdown,
        // the main frame might not be available, so we need to check for it.
        UILayoutEditorFrame* const mainFrame = wxGetApp().GetLayoutEditorFrame();
        if (mainFrame)
            mainFrame->SetFocus();

        // Return user input
        return retValue;
    }
}
    // Vectored Exception Handling (VEH) is only supported on Windows platforms. 
    // NOTE: Therefore, obtaining the stack trace of any given exception is only possible
    // if running under Windows (at the moment). Plans are being made for linux
    // support.
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
//-----------------------------------------------------------------------
    static LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS exceptionInfo)
    {
        // Validations
        wxASSERT_MSG(exceptionInfo != NULL, wxT("ExceptionInfo parameter is NULL"));

        // Obtain exception code
        const DWORD exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;

        // If the exception was raised explicitly by ::OutputDebugStringA, transfer control 
        // to the appropriate handler (thus ignoring the exception).
        // IMPORTANT NOTE: An example of one such exception is whenever you call ::OutputDebugString* 
        // family of functions (which is invoked internally by wxWidgets when you use wxLogDebug(), 
        // wxLogTrace(), wxVLogDebug() and wxVLogTrace(), the latter two which happen to be used by 
        // LogDebug() and LogTrace()). In Unicode-versions (which is the one that interest us since 
        // LayoutEditor is built in Unicode), ::OutputDebugStringW() converts Unicode characters 
        // to ANSI and calls ::OutputDebugStringA(). ::OutputDebugStringA() registers its own 
        // exception handler and raises an exception with code 0x40010006UL via RaiseException(). 
        // This is done to communicate with the currently active debugger and to output the supplied 
        // text to the debug console. If there is a registered system debugger (i.e. one running at 
        // the moment), it handles the exception (and "eats" it, thus never triggering this VEH 
        // handler). Otherwise, the OutputDebugString()'s own handler is invoked (and this VEH 
        // handler is called and we must ignore the call). 
        //
        // For those wishing to know the internals, it does the following: attempts to open 
        // 'DBWIN_DATA_READY' and 'DBWIN_BUFFER_READY' named events, then named file mapping 
        // 'DBWIN_BUFFER'. If any of these operations fails, the message is dropped. In other 
        // words ::OutputDebugString() works by raising a structured exception 0x40010006UL, passing 
        // the length of the string and the string as exception parameters. Windows catches this 
        // exception and makes this data available through the shared memory-mapped file 
        // DBWIN_BUFFER. Because this buffer is shared, it must be protected from multi-threaded 
        // access (i.e. access to it must be synchronized, which basically means that only one 
        // thread  must be able to write to the memory-mapped file at any one time). This is done 
        // using two named Win32 events: One is used to indicate that a monitor is reading the 
        // memory and hence prevents writing to it (DBWIN_BUFFER_READY), and the other is used by 
        // the system to indicate that a new message has been written to the shared memory 
        // (DBWIN_DATA_READY). A thread that calls ::OutputDebugString() is blocked until the 
        // DBWIN_BUFFER_READY event is signaled, this means that the monitor process is coupled to 
        // the process generating the messages, and a poorly written monitor process can perform 
        // unwanted synchronization on the process generating messages. In other words, 
        // ::OutputDebugString couples your process to another process that will read the 
        // debug string. This is bad news, so you should not generate ::OutputDebugString messages 
        // in release code. Again, this means that you should only generate trace messages through 
        // the Debug class in debug builds.  
        // More info at 'Adding Diagnostics to .NET Code' (http://www.ddj.com/windows/184406003) and 
        // 'Tracing Code: Part 2' (http://www.ddj.com/windows/184416750) by Richard Grimes.
        // NOTE2: This is an informational exception (ERROR_SEVERITY_INFORMATIONAL), but
        // one that is expected and can be handled safely. Therefore, this is a special case, and
        // the remainder informational exceptions will be handled differently just below.
        // NOTE3: The reason why you *don't* see exception 0x40010006UL when running 
        // under a debugger is that the O/S maps the 'first-chance' exception event into 
        // an OUTPUT_DEBUG_STRING_EVENT - the RaiseException never makes it back into 
        // user-code in the process being debugged.
        if (exceptionCode == OUTPUT_DEBUG_STRING_EXCEPTION)
            return EXCEPTION_EXECUTE_HANDLER;

        // Obtain exception severity (error, warning, informational or success)
        // NOTE: Success should never occur.
        const DWORD severity = exceptionCode & ERROR_SEVERITY_MASK;

        // We're only interested in errors and warnings. Informational 
        // messages should be ignored since that are usually raised
        // explicitly by the program (via RaiseException) and aren't harmful
        // to the program's execution.
        // NOTE: When EXCEPTION_CONTINUE_EXECUTION is returned, the system 
        // attempts to restart execution of the process. Vectored exception 
        // handlers that appear later in the list won't be called, nor will 
        // any of the structured exception handlers. When the function 
        // returns EXCEPTION_CONTINUE_SEARCH (which is our case), the system moves on 
        // to the next vectored exception handler. After all vectored 
        // exception handlers have been called, the system starts 
        // with the structured exception handling list.
        if ((severity != ERROR_SEVERITY_ERROR) && (severity != ERROR_SEVERITY_WARNING))
            return EXCEPTION_CONTINUE_SEARCH;

        // DEBUG info
        LogSilentTrace(wxT("VectoredHandler - Entered VEH handler"));

        // Obtain exception flags
        const DWORD exceptionFlags = exceptionInfo->ExceptionRecord->ExceptionFlags;

        // Check if we're in the middle of an unwinding operation. If so, we ignore the 
        // exception (since it was already processed).
        // NOTE: Basically the exception system follows the chain of handlers once to find the 
        // handler that will deal with the exception.  Then this handler runs the chain again, 
        // with the 'unwind' bit set to give handlers a chance to do their own unwinding (in 
        // the C++ case this involves calling destructors). 
        if (IS_DISPATCHING(exceptionFlags))
        {
            // Get exception manager, the singleton which stores and manages all exception related info
            ExceptionManager* const exceptionManager = ExceptionManager::GetInstancePtr();

            // Store exception flags for later use (inside the try/catch block)
            exceptionManager->SetExceptionFlags(exceptionInfo->ExceptionRecord->ExceptionFlags);

            // Check if we have a Win32 exception (fatal) or a normal CPP exception
            const bool fatalException = ((exceptionCode & 0xf0000000) == 0xc0000000);

            // Store exception code
            exceptionManager->SetExceptionCode(exceptionCode);

            // Obtain and store exception type using RTTI (if available)
            wxString exceptionType;
#if defined(_CPPRTTI) && defined(_MSC_VER)
            exceptionManager->SetExceptionType(GetRTTIExceptionType(exceptionInfo));
            exceptionType = exceptionManager->GetExceptionType();
#endif // defined(_CPPRTTI) && defined(_MSC_VER)

            // Store exception timestamp
            // FIXME: Create util func : GetTimeStamp()
            const wxDateTime dt = wxDateTime::Now();
            exceptionManager->SetExceptionTime(dt.FormatISODate() + wxT(' ') + dt.FormatISOTime());

            // Obtain exception stack trace
            wxString stackTrace = SimpleSymbolEngine::instance().StackTrace(exceptionInfo->ContextRecord, exceptionCode, exceptionFlags, exceptionType);

            // Did we manage to retrieve the stack trace?
            if (stackTrace.IsEmpty())
                // Failed to get stack trace
                stackTrace = wxT("Failed to obtain stack trace -- Check log for details");
            else
            {
                // Stack trace obtained correctly, so let's check if there are already any 
                // unprocessed stack traces, which implies multiple stacks 
                // NOTE: Multiple stack traces are the result of one or more of the following 
                // three things:
                //       - "Eaten" exceptions (caught but not processed/logged exceptions);
                //       - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);
                //       - Nested exceptions (the exception is caught and a new one is thrown).
                // NOTE2: Please note that "eaten" exceptions might manifest themselves much 
                // later than when they first appear since they can only be detected when a second 
                // exception is thrown (or at program shutdown). That is why they receive special 
                // treatment at program's exit (see PrepareForExit).
#if CHECK_MULTIPLE_STACKS
                if (!exceptionManager->IsExceptionProcessed())
                    // Multiple stacks have been detected
                    exceptionManager->SetMultipleStacks(true);
#endif // CHECK_MULTIPLE_STACKS
            }

            // Store exception stack trace and mark it as needing processing (in the try/catch block)
            exceptionManager->SetExceptionStackTrace(stackTrace);
            exceptionManager->ExceptionProcessed(false);

            // Do we have a fatal exception?
            if (fatalException)
            {
                // Add Win32 fatal exception to log
                // NOTE: Fatal exception always terminate the application and are not caught in
                // try/catch blocks, so we need to add the exception to log right here
                // NOTE2: A fatal exception was detected and while you could theoretically display a message
                // to the user, it is best to avoid such practice and only log silently
                LogSilentError(wxT("VectoredHandler - Fatal Win32 exception detected:%s"), exceptionManager->GetExceptionVEHInfo());
            }
        }

        // Pass control to next exception handler
        // NOTE: When EXCEPTION_CONTINUE_EXECUTION is returned, the system 
        // attempts to restart execution of the process. Vectored exception 
        // handlers that appear later in the list won't be called, nor will 
        // any of the structured exception handlers. When the function 
        // returns EXCEPTION_CONTINUE_SEARCH (which is our case), the system moves on 
        // to the next vectored exception handler. After all vectored 
        // exception handlers have been called, the system starts 
        // with the structured exception handling list.
        return EXCEPTION_CONTINUE_SEARCH;
    }

    //-----------------------------------------------------------------------
#if defined(_CPPRTTI) && defined(_MSC_VER)
    wxString GetRTTIExceptionType(const EXCEPTION_POINTERS* const exceptionInfo)
    {
        // Validations
        wxASSERT_MSG(exceptionInfo != NULL, wxT("ExceptionInfo parameter is NULL"));

        // Try and get proper class name if the object has got RTTI information. However, we can 
        // only obtain the exception type if we have a CPP exception and a not a SEH one
        if (exceptionInfo->ExceptionRecord->ExceptionCode == CPP_EXCEPTION)
        {
            // Get exception object address
            // NOTE: This is an horrid hack to try and get thrown type. It basically employs the following trick - 
            // pretend the thrown object is an instance of a polymorphic class to make typeid() delay until runtime
            const DummyPolymorphic* const exceptionAddress = (DummyPolymorphic *)exceptionInfo->ExceptionRecord->ExceptionInformation[1];

            // Did we manage to obtain the exception address?           
            if (exceptionAddress != 0)
            {
                // Yes, so obtain exception type using RTTI (using our dirty trick)
                const type_info& exceptionType = typeid(*exceptionAddress);

                // Obtain exception name
                const char* const exceptionName = exceptionType.name();

                // Convert result to wxString and return it, supporting unicode
                // NOTE: The converter is ignored in non-unicode builds
                return wxString(exceptionName, wxConvUTF8);
            }
            // Check if the exception was re-thrown
            // NOTE: Microsoft implemented "throw;" by raising an exception with
            // 0 for the address of the object and for the address of the type information.
            // The C runtime code then looks in the internal per-thread data structure to
            // find the 'current exception' (_curexception).
            else if (exceptionInfo->ExceptionRecord->ExceptionInformation[2] == 0)
            {
                // Exception was re-thrown
                // NOTE: RTTI info doesn't work when you re-throw an exception (i.e. throw;) as
                // the exception address will be empty (NULL)
                LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (exception was re-thrown)"));
                return wxT("Unknown (exception was re-thrown - check prior stack traces for type)");
            }
            else
            {
                // Invalid or empty exception address
                LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (exceptionAddress is empty or invalid)"));
                return wxT("Unknown (exceptionAddress is empty or invalid)");
            }
        }
        else
        {
            // Non CPP Exception - SEH Win32 exception
            LogSilentWarning(wxT("GetRTTIExceptionType - Failed to obtain exception type (SEH Win32 Exception)"));
            return wxT("Unknown (SEH Win32 Exception)");
        }
    }
#endif // defined(_CPPRTTI) && defined(_MSC_VER)

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
namespace gsgs
{
    /** Declare the instance.*/
    ExceptionManager * ExceptionManager::m_instance = NULL;
        //////////////////////////////////////////////////////////////////////////
        // EXCEPTION MANAGER FUNCTIONS
        //////////////////////////////////////////////////////////////////////////

    //------------------------------------------------------------------------
    ExceptionManager::ExceptionManager() :
            m_exceptionProcessed(true) // This starts as true
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
            , m_multipleStacks(false),
            m_handler(NULL),
            m_exceptionStackTrace(),
#if APPEND_MULTIPLE_STACKS
            m_exceededStackTraces(false),
#endif // APPEND_MULTIPLE_STACKS
            m_exceptionTime(),
            m_exceptionCode(0UL),
            m_exceptionFlags(0UL)
#if defined(_CPPRTTI) && defined(_MSC_VER)
            , m_exceptionType()
#endif // defined(_CPPRTTI) && defined(_MSC_VER)
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    {
            // NOTE: Prefer constructor initialization lists (which are 
            // often more efficient) to assignment inside the 
            // constructor. Members in the initialization list should 
            // appear in the order they are declared in the class, as 
            // this is the order they will be initialized (no matter what).
            // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

            // Empty at the moment 
    }

    //------------------------------------------------------------------------
    ExceptionManager::~ExceptionManager()
    {
            // Swallow any exceptions found to prevent the destructor from emitting them.
            // NOTE: An exception should never leave a destructor as it may yield
            // undefined behavior or even lead to program abnormal termination 
            // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
            try
            {
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
                // Remove exception VEH handler
                UnRegisterExceptionHandler();

#if APPEND_MULTIPLE_STACKS
                // Free memory allocated by the elements in the array
                m_exceptionStackTrace.Clear();
#endif // APPEND_MULTIPLE_STACKS
#endif //defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES        
            }
            // Add exception to log
            LOG_EXCEPTION_SAFE("ExceptionManager::~ExceptionManager", "Failed to destroy exception manager", "Failed to destroy exception manager", "Failed to destroy exception manager");
    }
    //-----------------------------------------------------------------------
    wxString ExceptionManager::GetExceptionInfo(const wxString& location, const wxString& text, const wxString& exceptionMessage, const wxString& exceptionFileName, int exceptionLine)
    {
            // Build exception basic info (location/Descriptive text)
            wxString exceptionInfo = wxString::Format(wxT("LayoutEditor has detected and caught an exception. The following detailed info is available:\n\nDescription: %s\nException Location: %s"), text.c_str(), location.c_str());

            // Check if we are to add the exception message, exception filename and exception line
            // NOTE: The last two are only relevant for GUI exceptions
            if (!exceptionMessage.IsEmpty())
                (void)exceptionInfo.Append(wxString::Format(wxT("\nException Message: %s"), exceptionMessage.c_str()));
            if (!exceptionFileName.IsEmpty())
                (void)exceptionInfo.Append(wxString::Format(wxT("\nException Filename: %s"), exceptionFileName.c_str()));
            if (exceptionLine != -1)
                (void)exceptionInfo.Append(wxString::Format(wxT("\nException Line: %d"), exceptionLine));

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
            // Add VEH info (stack/code/type/flags) if running on windows platform under debug builds
            (void)exceptionInfo.Append(GetExceptionVEHInfo());

            // Reset multiple stack traces info
            m_multipleStacks = false;
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

            // Mark the exception as processed
            ExceptionProcessed(true);

            // Return exception info
            return exceptionInfo;
    }

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    //-----------------------------------------------------------------------
    wxString ExceptionManager::GetExceptionVEHInfo()
    {
            wxString exceptionInfo;

#if CHECK_MULTIPLE_STACKS
            // Do we have multiple stack traces?
            if (GetMultipleStacks())
            {
                // Yes, so add warning to inform user 
                // NOTE: Depending on the value of the APPEND_MULTIPLE_STACKS define, all the stack
                // traces will be displayed (more recent first) (1), or only the more recent one (0)
#if APPEND_MULTIPLE_STACKS
                (void)exceptionInfo.Append(wxT("\n\nWARNING: Multiple stacks detected. This is the result of one or more of the following three things:\n   - \"Eaten\" exceptions (caught but not processed/logged exceptions);\n   - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);\n   - Nested exceptions (the exception is caught and a new one is thrown).\nAll stack traces will be displayed (more recent first)."));

                // Have we reached the stack trace limit?
                if (m_exceededStackTraces)
                {
                    // Inform the user as well and reset the limit
                    (void)exceptionInfo.Append(wxString::Format(wxT("\n\nWARNING: Maximum stack trace (%d) exceeded! Oldest stack traces will be discarted!"), MAXIMUM_STACK_TRACES));
                    m_exceededStackTraces = false;
                }
#else
                (void)exceptionInfo.Append(wxT("\n\nMultiple stacks detected. This is the result of one or more of the following three things:\n   - \"Eaten\" exceptions (caught but not processed/logged exceptions);\n   - Exception chains (the same exception is rethrown -- e.g. catch(Exception&) throw;);\n   - Nested exceptions (the exception is caught and a new one is thrown).\nOnly the most recent stack trace is displayed (the others are discarted)."));
#endif //APPEND_MULTIPLE_STACKS
            }
#endif //CHECK_MULTIPLE_STACKS

            // Add stack trace information
            (void)exceptionInfo.Append(wxString::Format(wxT("\n\nException Stack Trace (%s):\n\n%s"), m_exceptionTime, GetExceptionStackTrace()));

            return exceptionInfo;
    }
    //-----------------------------------------------------------------------
    void ExceptionManager::RegisterExceptionHandler()
    {
            // Register VEH exception handler that will be responsible for obtaining
            // the thrown exceptions (both CPP & win32 fatal ones) stack traces, codes and flags
            // NOTE: Only applicable on windows platforms
        m_handler = ::AddVectoredExceptionHandler(0, &VectoredHandler);
    }
    //-----------------------------------------------------------------------
    void ExceptionManager::UnRegisterExceptionHandler()
    {
            // Windows exception handling specific cleanup
            if (m_handler != NULL)
            {
                // Unregister VEH exception handler
                // NOTE: We won't delete the handler ourselves since ::RemoveVectoredExceptionHandler()
                // will take care of this for us
                if (0 == ::RemoveVectoredExceptionHandler(m_handler))
                    LogSilentError(wxT("LayoutEditor::PrepareForExit - Failed to remove vectored exception handler!"));
                m_handler = NULL;
            }
    }
        //-----------------------------------------------------------------------
    void ExceptionManager::SetExceptionStackTrace(const wxString& val)
    {
            // If we support multiple stacks, we need to add the old ones to the more
            // recent one. Otherwise, the application only retains the newest.
#if APPEND_MULTIPLE_STACKS
// Have we reached the stack trace limit?
            if (m_exceptionStackTrace.GetCount() == MAXIMUM_STACK_TRACES)
            {
                // Yes, so we need to remove the oldest stack trace and mark the limit as reached
                // NOTE: The limit prevents "exception number overload" in external code
                m_exceptionStackTrace.RemoveAt(m_exceptionStackTrace.GetCount() - 1);
                m_exceededStackTraces = true;
                LogSilentWarning(wxString::Format(wxT("SetExceptionStackTrace - Maximum stack trace (%d) exceeded! Oldest stack traces will be discarted!"), MAXIMUM_STACK_TRACES));
            }

            // Add new stack trace at the front of the array
            // NOTE: Newest stack traces always appear first
            m_exceptionStackTrace.Insert(val, 0);
#else
// Retain the newest stack trace
            m_exceptionStackTrace = val;
#endif // APPEND_MULTIPLE_STACKS
    }
}
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES