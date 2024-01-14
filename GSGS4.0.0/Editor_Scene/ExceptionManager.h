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

#ifndef _EXCEPTION_MANAGER_H_
#define _EXCEPTION_MANAGER_H_

#include "UILayoutConfig.h"
#include "StrHelper.h"

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

  #if defined(_CPPRTTI) && defined(_MSC_VER)
    /** This dummy polymorphic class is used to obtain the class name of
    * the thrown exception in the VEH handler using RTTI. @see GetRTTIExceptionType
    * for more info.
    */
    class DummyPolymorphic { virtual ~DummyPolymorphic() {} };
  #endif // defined(_CPPRTTI) && defined(_MSC_VER)

    //////////////////////////////////////////////////////////////////////////
    // VEH HANDLER
    //////////////////////////////////////////////////////////////////////////

    /** VEH (Vectored Exception Handling) handler function called 
     * whenever an exception is thrown (both CPP & Win32 exceptions). The handler
     * is called before the try/catch block and is responsible for obtaining
     * the exception stack trace, code, type and flags (each item is optional as retrieval
     * is dependent on the value of the defines present in Config.h). It is also capable of handling
     * multiple stack traces if CHECK_MULTIPLE_STACKS is 1.
     * 
     * Thanks go to Roger Orr (rogero@howzatt.demon.co.uk) for all the help he provided in 
     * the process.
     *
     * @access public static 
     * @qualifier none
     * @param exceptionInfo Contains caught exception info (record & context)
     * @return LONG WINAPI EXCEPTION_CONTINUE_SEARCH under normal scenarios and 
     * EXCEPTION_EXECUTE_HANDLER in the case of ignorable user raised exceptions (see remarks)
     * @remarks Only works on Windows platforms, under debug builds and if CHECK_STACK_TRACES is set.
     * @remarks This might work in non-MS compilers but it has only been tested in VS2003 and VS2005.
     * @remarks The handler returns EXCEPTION_CONTINUE_SEARCH and occasionally EXCEPTION_EXECUTE_HANDLER 
     * but never EXCEPTION_CONTINUE_EXECUTION, because we don't want to override 
     * the windows exception mechanism (merely intercept it). In fact, the main pitfall of VEH 
     * is that under .Net debugging you get a lot of exceptions which the .Net runtime uses 
     * to talk to the debugger. These must get through the VEH code.
     */
    static LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS exceptionInfo);

  #if defined(_CPPRTTI) && defined(_MSC_VER)
    /** Attempts to retrieve the exception type (class name) using RTTI. The following
     * things must be considered:
     * a) It is MSVC only 
     * b) It only works with RTTI enabled (/GR+)
     * c) It uses a trick - pretend the thrown exception object is an instance of a polymorphic
     *    class to make typeid() delay until runtime. 
     * @access public 
     * @qualifier
     * @param exceptionInfo
     * @return wxString
     * @remarks Called internally by the @see VectoredHandler.
     */
    wxString GetRTTIExceptionType(const EXCEPTION_POINTERS* const exceptionInfo);
  #endif

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

namespace gsgs
{
    /** This function shows a message to the user in a safe way and should be safe to call even
     * before the application has been initialized or if it is currently in some other strange
     * state (for example, about to crash). Under Windows this function shows a message box
     * using a native dialog instead of wxMessageBox (which might be unsafe to call), elsewhere
     * it simply prints the message to the standard output using the title as prefix.
     * @access public
     * @qualifier none
     * @param text Message text to be displayed
     * @param title Message box window title
     * @param style MFC flags that specify the contents and behavior of the message box (e.g. MB_OK)
     * @return int Windows identifier of the message box's button the user pressed (e.g. IDOK)
     */
    int SafeMessageBox(const wxString& text, const wxString& title, long style);

    /** Displays a message to the user using a true modal window (i.e. all external application
     * input is turned off while while the user doesn't close the dialog) by disabling all other
     * windows. The message box can be displayed in normal or safe mode. In the first case, the
     * wxWidgets wxMessageBox() is used, whereas @see SafeMessageBox() is employed in
     * the second one.
     * @access public
     * @qualifier none
     * @param text Message text to be displayed
     * @param title Message box window title
     * @param style MFC flags that specify the contents and behavior of the message box (e.g. MB_OK)
     * when in safe mode, or wxWidgets flags instead (e.g. wxOK).
     * @param safe Display message box in safe mode (true) or in normal one (false)
     * @return int Windows identifier of the message box's button the user pressed (e.g. IDOK) when
     * in normal mode or wxWidgets equivalent identifier in safe mode (e.g. wxID_OK)
     */
    int ModalMessageBox(const wxString& text, const wxString& title, long style, const bool safe = false);

    /** Shows a message to the user.  The exact behaviour of this function depends
     * upon the configuration options set; basically it will call either the
     * ModalMessageBox or SafeMessageBox function.
     * @access public
     * @qualifier none
     * @param text Message text to be displayed
     * @param title Message box window title
     * @param style Flags that specify the contents and behavior of the message box.
     * @return int Identifier of the message box's button the user pressed (e.g. CELE_MBOK)
     */
    int ExceptionMessageBox(const wxString& text, const wxString& title, long style);

    /** Wrapper function around wxLogError which pops up a message box (with an error icon)
     * to inform the user about it (default behavior). It also saves the error message
     * to log (LayoutEditor.log) with logging level 'Errors'.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogError(const wxChar *szFormat, ...);

    /** Log function which saves the error message to log (LayoutEditor.log) with
     * logging level 'Errors'. It provides no wrapper around the wxLog* family of functions
     * since we want the output to be silent.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogSilentError(const wxChar *szFormat, ...);

    /** Wrapper function around wxLogDebug which in its default behavior only does anything
     * at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands
     * to nothing in release mode (otherwise). It also saves the error message to log (LayoutEditor.log)
     * with logging level 'Standard'.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogDebug(const wxChar *szFormat, ...);

    /** Log function which saves the error message to log (LayoutEditor.log) with
    * logging level 'Standard'. It provides no wrapper around the wxLog* family of functions
    * since we want the output to be silent.
    * @access public
    * @qualifier none
    * @param szFormat Format string to log (printf style)
    * @param ... Variable number of arguments to the format string
    * @return void
    * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
    * other string types (like GUI::String or char*) because it could lead to unpredictable
    * results and Win32 'Access Violation' exceptions that will crash the application (activating
    * LayoutEditor::OnFatalException() method).
    */
    void LogSilentDebug(const wxChar *szFormat, ...);

    /** Wrapper function around wxLogWarning which pops up a message box (with OK button)
     * to inform the user about it (default behavior). It also saves the error
     * message to log (LayoutEditor.log) with logging level 'Standard'.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogWarning(const wxChar *szFormat, ...);

    /** Log function which saves the error message to log (LayoutEditor.log) with
    * logging level 'Warnings'. It provides no wrapper around the wxLog* family of functions
    * since we want the output to be silent.
    * @access public
    * @qualifier none
    * @param szFormat Format string to log (printf style)
    * @param ... Variable number of arguments to the format string
    * @return void
    * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
    * other string types (like GUI::String or char*) because it could lead to unpredictable
    * results and Win32 'Access Violation' exceptions that will crash the application (activating
    * LayoutEditor::OnFatalException() method).
    */
    void LogSilentWarning(const wxChar *szFormat, ...);

    /** Wrapper function around wxLogMessage which pops up a message box (with OK button)
     * to inform the user about it (default behavior). It also saves the error
     * message to log (LayoutEditor.log) with logging level 'Informative'.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogMessage(const wxChar *szFormat, ...);

    /** Wrapper function around wxLogDebug which in its default behavior only does anything
     * at all in debug mode (when the preprocessor symbol __WXDEBUG__ is defined) and expands
     * to nothing in release mode (otherwise). It also saves the error message to log (LayoutEditor.log)
     * with logging level 'Insane'.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogTrace(const wxChar *szFormat, ...);

    /** Log function which saves the error message to log (LayoutEditor.log) with
     * logging level 'Insane'. It provides no wrapper around the wxLog* family of functions
     * since we want the output to be silent.
     * @access public
     * @qualifier none
     * @param szFormat Format string to log (printf style)
     * @param ... Variable number of arguments to the format string
     * @return void
     * @remarks Be sure that the arguments supplied are wxStrings or wxChar* and not
     * other string types (like GUI::String or char*) because it could lead to unpredictable
     * results and Win32 'Access Violation' exceptions that will crash the application (activating
     * LayoutEditor::OnFatalException() method).
     */
    void LogSilentTrace(const wxChar *szFormat, ...);
}
    //////////////////////////////////////////////////////////////////////////
    // MACROS
    //////////////////////////////////////////////////////////////////////////
    // defines used by message boxes and such
    // main purpose of these was to to be able to remove
    // some Win32 platform defines from the main code.
    // I guess they could have just been defined for other
    // platforms but such things are not exactly collision safe.
#if defined(__WINDOWS__)
    #define LE_MB_RESULT_OK       IDOK
    #define LE_MB_RESULT_CANCEL   IDCANCEL
    #define LE_MB_TYPE_OK         MB_OK
    #define LE_MB_ICON_STOP       MB_ICONSTOP
    #define LE_MB_SYSTEMMODAL     MB_SYSTEMMODAL
#else
    #define LE_MB_RESULT_OK       1
    #define LE_MB_RESULT_CANCEL   2
    #define LE_MB_TYPE_OK         0
    #define LE_MB_ICON_STOP       0
    #define LE_MB_SYSTEMMODAL     0
#endif

/** Macro that catches, logs and shows the the thrown exception (either GUI based, standard
 * or non-standard) to the user.
 * @access public
 * @qualifier none
 * @param location Class and function where the the macro is called (e.g. LayoutEditor::HandleEvent)
 * @param Itxt Text to display when a GUI based exception is caught.
 * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
 * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
 * @remarks All parameters can be supplied as GUI strings, wxStrings or char*.
 * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this
 * is a bug). In Visual C++ 2005, it never does (correct behavior). See
 * http://members.cox.net/doug_web/eh.htm for more info.
 */
#define SHOW_EXCEPTION(location, Itxt, standardTxt, nonStandardTxt)\
    catch(const NII::Exception& e)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(Itxt)), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));\
    }\
    catch(const std::exception& ex)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(standardTxt)), ex.what()));\
    }\
    catch ( ... )\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(nonStandardTxt))));\
    }

 /** Macro that catches, logs and shows the the thrown exception (either GUI based, standard
  * or non-standard) to the user. It also returns the supplied value when an exception occurs.
  * @access public
  * @qualifier none
  * @param location Class and function where the the macro is called (e.g. LayoutEditor::HandleEvent)
  * @param Itxt Text to display when a GUI based exception is caught.
  * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
  * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
  * @param returnValue Value to return when an exception is caught.
  * @remarks All text parameters can be supplied as GUI strings, wxStrings or char*. The
  * return value can be anything.
  * @remarks When you want to return void, just supply an empty last macro parameter
  * (e.g. SHOW_EXCEPTION_RETURN("1", "2", "3", "4",) ). @see DialogMain::SetWidgetLook for an example.
  * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this
  * is a bug). In Visual C++ 2005, it never does (correct behavior). See
  * http://members.cox.net/doug_web/eh.htm for more info.
  */
#define SHOW_EXCEPTION_RETURN(location, Itxt, standardTxt, nonStandardTxt, returnValue)\
    catch(const NII::Exception& e)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(Itxt)), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));\
        return returnValue;\
    }\
    catch(const std::exception& ex)\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(standardTxt)), ex.what()));\
        return returnValue;\
    }\
    catch ( ... )\
    {\
        LogError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(nonStandardTxt))));\
        return returnValue;\
    }

  /** Macro that catches and logs the thrown exception (either GUI based, standard
   * or non-standard). If you want to display it to the user as well @see SHOW_EXCEPTION or
   * @see SHOW_EXCEPTION_RETURN.
   * @access public
   * @qualifier none
   * @param location Class and function where the the macro is called (e.g. LayoutEditor::HandleEvent)
   * @param Itxt Text to display when a GUI based exception is caught.
   * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
   * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
   * @remarks All parameters can be supplied as GUI strings, wxStrings or char*.
   * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this
   * is a bug). In Visual C++ 2005, it never does (correct behavior). See
   * http://members.cox.net/doug_web/eh.htm for more info.
   */
#define LOG_EXCEPTION(location, Itxt, standardTxt, nonStandardTxt)\
    catch(const NII::Exception& e)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(Itxt)), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));\
    }\
    catch(const std::exception& ex)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(standardTxt)), ex.what()));\
    }\
    catch ( ... )\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(nonStandardTxt))));\
    }

   /** Macro that catches and logs the thrown exception (either GUI based, standard
    * or non-standard) in a complete safe way (i.e. no more exceptions can be generated
    * while processing the current one). If you want to display it to the user as well @see SHOW_EXCEPTION or
    * @see SHOW_EXCEPTION_RETURN.
    * @access public
    * @qualifier none
    * @param location Class and function where the the macro is called (e.g. LayoutEditor::HandleEvent)
    * @param Itxt Text to display when a GUI based exception is caught.
    * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
    * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
    * @remarks All parameters can be supplied as GUI strings, wxStrings or char*.
    * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this
    * is a bug). In Visual C++ 2005, it never does (correct behavior). See
    * http://members.cox.net/doug_web/eh.htm for more info.
    * @remarks This macro, since it may never issue an exception itself, is
    * very useful for catching exceptions inside a destructor (which should never
    * throw exceptions).
    * @remarks If a problem occurs during logging (highly unlikely) the new
    * freshly produced exception will be eaten and ignored and no indication
    * will exist of what happened.
    *
    */
#define LOG_EXCEPTION_SAFE(location, Itxt, standardTxt, nonStandardTxt)\
    catch(const NII::Exception& e)\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(Itxt)), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));\
        }\
        catch(...) {} \
    }\
    catch(const std::exception& ex)\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(standardTxt)), ex.what()));\
        }\
        catch(...) {} \
    }\
    catch ( ... )\
    {\
        try\
        {\
            LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(nonStandardTxt))));\
        }\
        catch(...) {} \
    }

    /** Macro that catches and logs the thrown exception (either GUI based, standard or
     * non-standard). If you want to display it to the user as well @see SHOW_EXCEPTION or
     * @see SHOW_EXCEPTION_RETURN. It also returns the supplied value when an exception occurs.
     * @access public
     * @qualifier none
     * @param location Class and function where the the macro is called (e.g. LayoutEditor::HandleEvent)
     * @param Itxt Text to display when a GUI based exception is caught.
     * @param standardTxt Text to display when a standard C++ exception, derived from std::exception, is caugh.
     * @param nonStandardTxt Text to display when a non standard, like a win32 exception, is caught.
     * @param returnValue Value to return when an exception is caught.
     * @remarks All text parameters can be supplied as GUI strings, wxStrings or char*. The
     * return value can be anything.
     * @remarks When you want to return void, just supply an empty last macro parameter
     * (e.g. LOG_EXCEPTION_RETURN("1", "2", "3", "4",) ). @see DialogMain::SetWidgetLook for a similar example
     * using @see SHOW_EXCEPTION_RETURN.
     * @remarks When you want to return void, just supply ; as the returnValue.
     * @remarks Please note that in pre-2005 Visual C++, catch(...) can catch Win32 exceptions (this
     * is a bug). In Visual C++ 2005, it never does (correct behavior). See
     * http://members.cox.net/doug_web/eh.htm for more info.
     */
#define LOG_EXCEPTION_RETURN(location, Itxt, standardTxt, nonStandardTxt, returnValue)\
    catch(const NII::Exception& e)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(Itxt)), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));\
        return returnValue;\
    }\
    catch(const std::exception& ex)\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(standardTxt)), ex.what()));\
        return returnValue;\
    }\
    catch ( ... )\
    {\
        LogSilentError(ExceptionManager::GetInstancePtr()->GetExceptionInfo(StrHelper::ToWXString(_T(location)), StrHelper::ToWXString(_T(nonStandardTxt))));\
        return returnValue;\
    }

namespace gsgs
{
    /** This singleton class holds and manages information about the exceptions thrown
     * by the application. When running under Windows platforms, it also stores all
     * relevant VEH handler related information, including but not limited to stack trace.
     */
    class ExceptionManager
    {
    public:
        ~ExceptionManager();

        static ExceptionManager* GetInstancePtr() 
        {
            if (!m_instance)
            {
                m_instance = new ExceptionManager();
            }
            return m_instance;
        }
        
        /** Indicates if the last thrown exception has been caught and handled 
         * (logged and/or shown to the user).
         * @access public 
         * @qualifier const
         * @return bool true if exception has been processed, false otherwise
         */
        bool IsExceptionProcessed() const       { return m_exceptionProcessed; }

        /** Marks the last thrown exception as caught and handled (logged 
         * and/or shown to the user) or not.
         * @access public 
         * @qualifier none 
         * @param val Boolean indicating if the last exception has been processed
         * @return void
         */
        void ExceptionProcessed(bool val) 
        {
            m_exceptionProcessed = val;
            #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES && APPEND_MULTIPLE_STACKS
            if(val)
                // We can now clear the stack trace chain
                // NOTE: We're using Empty() and not Clear() since we'll probably be reusing
                // the array (in the event of other exceptions)
                m_exceptionStackTrace.Empty();
            #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES && APPEND_MULTIPLE_STACKS
        }
       
#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        /** Returns last exception's stack trace. If @see APPEND_MULTIPLE_STACKS is
         * set to 1, the old stack traces (if they exist) are also returned.
         * @access public 
         * @qualifier const
         * @return wxString Last exception's stack trace
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        wxString GetExceptionStackTrace() const 
        {
        #if APPEND_MULTIPLE_STACKS
            // Convert array string holding the various stack traces to a single
            // string
            return StrHelper::ArrayStringToString(m_exceptionStackTrace, true);
        #else
            return m_exceptionStackTrace; 
        #endif // APPEND_MULTIPLE_STACKS
        
        }

        /** Stores the last exception's stack trace. If @see APPEND_MULTIPLE_STACKS is
         * set to 1, the old stack traces are added to the current one.
         * @access public 
         * @qualifier none
         * @param val Last exception's stack trace
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void SetExceptionStackTrace(const wxString& val);
        
        /** Obtains the last exception's time.
         * @access public 
         * @qualifier const
         * @return wxString Last exception's time
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        wxString GetExceptionTime() const       { return m_exceptionTime; }

        /** Sets the last exception's time.
         * @access public 
         * @qualifier none
         * @param val Last exception's time
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void SetExceptionTime(wxString val)     { m_exceptionTime = val; }

        /** Obtains last exception's code (DWORD).
         * @access public 
         * @qualifier const
         * @return DWORD Last exception's code
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        DWORD GetExceptionCode() const          { return m_exceptionCode; }

        /** Stores the last exception's code (DWORD).
         * @access public 
         * @qualifier none
         * @param val Last exception's code
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void SetExceptionCode(DWORD val)        { m_exceptionCode = val; }

        /** Obtains last exception's flags (DWORD).
         * @access public 
         * @qualifier const
         * @return DWORD Last exception's flags
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        DWORD GetExceptionFlags() const         { return m_exceptionFlags; }

        /** Stores the last exception's flags (DWORD).
         * @access public 
         * @qualifier none
         * @param val Last exception's flags
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void SetExceptionFlags(DWORD val)       { m_exceptionFlags = val; }

        /** Indicates if multiple stack traces were detected in 
         * the VEH handler.
         * @access public 
         * @qualifier const
         * @return bool true if multiple stack traces exist, false otherwise
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        bool GetMultipleStacks() const          { return m_multipleStacks; }

        /** Mark the existence of multiple stack traces or not.
         * @access public 
         * @qualifier none
         * @param val Boolean indicating if multiple stack traces exist or not
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void SetMultipleStacks(bool val)        { m_multipleStacks = val; }

    #if defined(_CPPRTTI) && defined(_MSC_VER)
        /** Returns the exception type (class name), obtained via RTTI.
         * @access public 
         * @qualifier const
         * @return wxString Exception type (class name)
         * @remarks Only relevant on Windows platforms under debug builds if 
         * running Visual studio with RTTI support and if
         * CHECK_STACK_TRACES is set.
         */
        wxString GetExceptionType() const       { return m_exceptionType; }

        /** Stores the exception type (class name), obtained via RTTI.
         * @access public 
         * @qualifier none
         * @param val Exception type (class name)
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds if
         * running Visual studio with RTTI support and if
         * CHECK_STACK_TRACES is set.
         */
        void SetExceptionType(wxString val)     { m_exceptionType = val; }
    #endif // defined(_CPPRTTI) && defined(_MSC_VER)

    #if APPEND_MULTIPLE_STACKS
        /** Verifies if the maximum number of allowed stack traces has been
         * reached.
         * @access public 
         * @qualifier const
         * @return bool true if stack trace limit has been reaches, false otherwise
         */
        bool HasExceededStackTraces() const     { return m_exceededStackTraces; }

        /** Marks the stack trace limit as reached or not.
         * @access public 
         * @qualifier none
         * @param val Boolean which indicates if the stack trace limit has been reached
         * @return void
         */
        void ExceededStackTraces(bool val)      { m_exceededStackTraces = val; }
    #endif // APPEND_MULTIPLE_STACKS

#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

        /** Retrieves the last caught exception basic info (location, description and exception message),
         * GUI exceptions specific info (line & file) and optionally if 
         * running under Windows the VEH info (stack trace, code, type and flags) as
         * a single string. Depending on the defines present in Config.h, some of this info might 
         * or not be processed. It also marks the exception as processed.
         * @access public 
         * @qualifier const
         * @param location Location where the exception was caught
         * @param text Description of the caught exception
         * @param exceptionMessage Message contained in the exception itself (if available)
         * @param exceptionFileName Name of the file where the exception occurred
         * @param exceptionLine Line where the exception occurred
         * @return wxString String containing the built exception info
         */
        wxString GetExceptionInfo(const wxString& location, const wxString& text, const wxString& e=wxT(""), const wxString& exceptionFileName=wxT(""), int exceptionLine=-1);

    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        /** Retrieves the last exception VEH info (stack trace, code, type and flags). Depending
         * on the defines present in Config.h, some of this info might or not be processed.
         * @access public 
         * @qualifier none
         * @return wxString Last exception VEH info
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         * @remarks The @see SHOW_EXCEPTION_CODE, @see SHOW_EXCEPTION_FLAGS and 
         * @see SHOW_EXCEPTION_TYPE defines control which information is retrieved.
         */
        wxString GetExceptionVEHInfo();

        /** Registers the VEH handler in the system.
         * @access public 
         * @qualifier none
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void RegisterExceptionHandler();

        /** Unregisters the VEH handler from the system.
         * @access public 
         * @qualifier none
         * @return void
         * @remarks Only relevant on Windows platforms under debug builds and if
         * CHECK_STACK_TRACES is set.
         */
        void UnRegisterExceptionHandler();
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    private:
        /** The one-and-only instance of the class.*/
        static ExceptionManager *m_instance;

        /** Indicates if the last thrown exception has been caught and processed (logged
         * and/or shown to user).
         */
        bool m_exceptionProcessed;
        
    #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
        /** Indicates the existence of multiple stack traces 
         */
        bool m_multipleStacks;

        /** Pointer to the VEH handler that will manage all thrown exceptions (CPP & 
         * Win32) under Windows. 
         */
        void *m_handler;

        /** Holds the last exception's stack trace
         * @remarks This is either a normal string if @see APPEND_MULTIPLE_STACKS is set
         * to 0 (single stack trace) or an array string that can hold multiple stack 
         * traces otherwise.
         */
    #if APPEND_MULTIPLE_STACKS
        wxArrayString m_exceptionStackTrace;
    #else
        wxString m_exceptionStackTrace;
    #endif // APPEND_MULTIPLE_STACKS

    #if APPEND_MULTIPLE_STACKS
        /** Indicates if the maximum number of stack traces (@see MAXIMUM_STACK_TRACES)
         * has been reached
         */
        bool m_exceededStackTraces;
    #endif // APPEND_MULTIPLE_STACKS

        /** Holds the last exception's time
         */
        wxString m_exceptionTime;

        /** Holds the last exception's code
         */
        DWORD m_exceptionCode;

        /** Holds the last exception's flags
         */
        DWORD m_exceptionFlags;

    #if defined(_CPPRTTI) && defined(_MSC_VER)
        /** Holds the last exception's type (if available)
         */
        wxString m_exceptionType;
    #endif // defined(_CPPRTTI) && defined(_MSC_VER)
    #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES   

        ExceptionManager();
        ExceptionManager(ExceptionManager const &);
        ExceptionManager& operator=(ExceptionManager const &);
    };

}
#endif // _EXCEPTION_MANAGER_H_

