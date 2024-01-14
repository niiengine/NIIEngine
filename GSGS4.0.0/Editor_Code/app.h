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
#ifndef gsgs_APP_H
#define gsgs_APP_H

#include "gsgsFrame.h"

#include "wx/debugrpt.h"

class wxSplashScreen;
class wxSingleInstanceChecker;
class wxDocManager;

namespace gsgs
{
    class PersistenceManager;
    class ImageEditorFrame;
    class UILayoutEditorFrame;
}

class GsGsApp : public wxApp
{
public:
    enum PluginPolicy 
    { 
        PP_None = 0, 
        PP_All, 
        PP_FromList 
    };
public:
    GsGsApp(void);
    virtual ~GsGsApp(void);

    void SetAllowedPlugins(const wxArrayString& allowedPlugins)     { this->m_allowedPlugins = allowedPlugins; }
    void SetPluginLoadPolicy(const PluginPolicy& pluginLoadPolicy)  { this->m_pluginLoadPolicy = pluginLoadPolicy; }
    const wxArrayString& GetAllowedPlugins() const                  { return m_allowedPlugins; }
    const PluginPolicy& GetPluginLoadPolicy() const                 { return m_pluginLoadPolicy; }
    void SetParserPaths(const wxArrayString& parserPaths)           { this->m_parserPaths = parserPaths; }
    const wxArrayString& GetParserPaths() const                     { return m_parserPaths; }

    void MacOpenFile(const wxString& fileName);

    void SetStartedInDebuggerMode(bool startedInDebuggerMode)       { m_startedInDebuggerMode = startedInDebuggerMode; }
    bool IsStartedInDebuggerMode() const                            { return m_startedInDebuggerMode; }

    void SetDebuggerArgs(const wxString& debuggerArgs)              { this->m_debuggerArgs = debuggerArgs; }
    void SetDebuggerWorkingDirectory(const wxString& debuggerWorkingDirectory)
    {
        this->m_debuggerWorkingDirectory = debuggerWorkingDirectory;
    }
    void SetExeToDebug(const wxString& exeToDebug)                  { this->m_exeToDebug = exeToDebug; }
    const wxString & GetDebuggerArgs() const                        { return m_debuggerArgs; }
    const wxString & GetDebuggerWorkingDirectory() const            { return m_debuggerWorkingDirectory; }
    const wxString & GetExeToDebug() const                          { return m_exeToDebug; }

    wxDocManager* GetDocManager() const                             { return m_docManager; }

    gsgs::ImageEditorFrame * GetImageEditorFrame()                  { return mImageEditorFrame; }

    gsgs::UILayoutEditorFrame* GetLayoutEditorFrame() const         { return mUILayoutEditorFrame; }

    gsgs::ImageEditorFrame * createImageEditor();

    gsgs::UILayoutEditorFrame* createLayoutEditor();

    void createEditorLog();
protected:
    virtual bool OnInit();
    virtual int OnExit();
    virtual void HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const;
    virtual void OnUnhandledException();
    void SetModificationStateAllDocuments(bool modificationState) const;
    void ProcessUnhandledException(const wxString& exceptionType, const wxString& exceptionText, const wxString& exceptionFileName = wxT(""), const int exceptionLine = -1) const;

#if wxUSE_DEBUGREPORT
    void GenerateDebugReport(wxDebugReport::Context ctx, const bool showPreview = true) const;
#endif
    virtual void OnFatalException();
    void OpenFolder(const wxString& path);
    void OpenFile(const wxString& path, long lineNumber);
    void OpenItem(const wxString& path, long lineNumber);

    bool CopySettings(const wxString& destDir, wxString& installPath);
    bool IsSingleInstance(const wxCmdLineParser& parser);
    void DoCopyGdbPrinters();
    void MSWReadRegistry();
    wxString DoFindMenuFile(const wxString& installDirectory, const wxString& requiredVersion);
    void checkRunPath();
    void PrintUsage(const wxCmdLineParser& parser);
protected:
    gsgs::Frame * mMainFrame;
    gsgs::ImageEditorFrame* mImageEditorFrame;
    gsgs::UILayoutEditorFrame * mUILayoutEditorFrame;
    gsgs::PersistenceManager* mPersistencManager;

    wxDocManager* m_docManager;
    wxLocale m_locale;
    wxSplashScreen * m_splash;
    wxSingleInstanceChecker * m_only;
    wxArrayString m_parserPaths;
    wxArrayString m_allowedPlugins;
    PluginPolicy m_pluginLoadPolicy;
    bool m_startedInDebuggerMode;

    // When starting in debugger mode
    wxString m_exeToDebug;
    wxString m_debuggerArgs;
    wxString m_debuggerWorkingDirectory;
    int mLogIndex;
};

DECLARE_APP(GsGsApp);

#endif
