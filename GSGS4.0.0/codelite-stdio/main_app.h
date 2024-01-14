#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/app.h>
#include <wx/cmdline.h>
#include <gsgsEvent.h>
#include <wx/arrstr.h>

#ifdef __WXOSX__
#define APP_BASE wxApp
#else
#define APP_BASE wxAppConsole
#endif

class wxEventLoopBase;
class gsgs 
{
    class SubPrc;
}

class MainApp : public APP_BASE
{
public:
    MainApp();
    virtual ~MainApp();

    /**
     * @brief intialize the application
     */
    virtual bool OnInit();
    /**
     * @brief perform cleanup before exiting
     */
    virtual int OnExit();
protected:
    void DoStartup();
    
protected:
    /**
     * @brief parse the command line here
     * @return true on success, false otherwise
     */
    bool DoParseCommandLine(wxCmdLineParser& parser);
    void OnProcessOutput(gsgs::ProcessEvent& event);
    void OnProcessStderr(gsgs::ProcessEvent& event);
    void OnProcessTerminated(gsgs::ProcessEvent& event);
private:
    gsgs::SubPrc* m_child = nullptr;
    wxArrayString m_argv;
};

DECLARE_APP(MainApp)

#endif // MAINAPP_H
