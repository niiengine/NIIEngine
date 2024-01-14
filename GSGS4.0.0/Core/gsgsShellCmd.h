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
#ifndef shell_command_h
#define shell_command_h

#include "gsgsPreInclude.h"
#include "project.h"
#include "gsgsEvent.h"
#include "gsgsProcess.h"

class IProcess;
namespace gsgs
{
    class _gsgsAPI QueueCommand
    {
    public:
        enum {
            kBuild,
            kClean,
            kCustomBuild,
            kDebug,
            kRebuild,
            kExecuteNoDebug
        };
    public:
        QueueCommand(const wxString & project, const wxString & configuration, bool projectOnly, int kind);
        QueueCommand(int kind);
        ~QueueCommand();

        //----------------------------------------
        // Setters/Getters
        //----------------------------------------

        void SetConfiguration(const wxString & configuration);
        void SetProject(const wxString & project);
        const wxString & GetConfiguration() const;
        const wxString & GetProject() const;
        void SetProjectOnly(bool projectOnly);
        const bool & GetProjectOnly() const;
        void SetKind(int kind);
        const int & GetKind() const;
        void SetCleanLog(bool cleanLog);
        const bool& GetCleanLog() const;
        inline void SetCheckBuildSuccess(bool checkBuildSuccess) { m_checkBuildSuccess = checkBuildSuccess; }
        inline bool GetCheckBuildSuccess() const { return m_checkBuildSuccess; }
        inline void SetCustomBuildTarget(const wxString& target) { m_customBuildTarget = target; }
        inline const wxString& GetCustomBuildTarget() const { return m_customBuildTarget; }
        inline const wxString& GetSynopsis() const {
            if (m_synopsis.IsEmpty())
                m_synopsis = DeriveSynopsis();
            return m_synopsis;
        }
        inline void SetSynopsis(const wxString& synopsis) { m_synopsis = synopsis; }
        wxString DeriveSynopsis() const;
    private:
        wxString m_project;
        wxString m_configuration;
        wxString m_customBuildTarget;
        mutable wxString m_synopsis;
        int m_kind;
        bool m_cleanLog;
        bool m_checkBuildSuccess;
        bool m_projectOnly;
    };

    extern _gsgsAPI const wxEventType wxEVT_SHELL_COMMAND_ADDLINE;
    extern _gsgsAPI const wxEventType wxEVT_SHELL_COMMAND_STARTED;
    extern _gsgsAPI const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED;
    extern _gsgsAPI const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN;

    /**
     * @class BuildEventDetails
     * @brief associated data that (ClientData) which is sent with the
     * wxEVT_SHELL_COMMAND_STARTED and wxEVT_SHELL_COMMAND_STARTED_NOCLEAN events
     */
    class _gsgsAPI BuildEventDetails : public wxClientData
    {
    public:
        BuildEventDetails() {}
        virtual ~BuildEventDetails() {}

        inline void SetConfiguration(const wxString& configuration) { this->m_configuration = configuration; }
        inline void SetProjectName(const wxString& projectName) { this->m_projectName = projectName; }
        inline const wxString& GetConfiguration() const { return m_configuration; }
        inline const wxString& GetProjectName() const { return m_projectName; }
        inline void SetIsClean(bool isClean) { this->m_isClean = isClean; }
        inline void SetIsCustomProject(bool isCustomProject) { this->m_isCustomProject = isCustomProject; }
        inline bool IsClean() const { return m_isClean; }
        inline bool IsCustomProject() const { return m_isCustomProject; }
    private:
        wxString m_projectName;
        wxString m_configuration;
        bool m_isClean;
        bool m_isCustomProject;
    };

    class _gsgsAPI ShellCommand : public wxEvtHandler
    {
    public:
        // construct a compiler action
        ShellCommand(const QueueCommand & buildInfo);

        virtual ~ShellCommand() {}
        virtual void Process(IManager * manager) = 0;

        void AppendLine(const wxString & line);

        void SendStartMsg();

        void SendEndMsg();

        inline bool IsBusy() const { return m_proc != NULL; }

        void Stop();

        inline void SetInfo(const QueueCommand& info) { this->m_info = info; }
        inline const QueueCommand& GetInfo() const { return m_info; }
    protected:
        virtual void DoPrintOutput(const wxString& out);
        virtual void OnProcessOutput(gsgs::ProcessEvent& e);
        virtual void OnProcessTerminated(gsgs::ProcessEvent& e);

        void CleanUp();
        void DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly);
    protected:
        IProcess* m_proc;
        QueueCommand m_info;
    };

    class _gsgsAPI CompileRequest : public ShellCommand
    {
    public:
        /**
         * Construct a compilation request. The compiler thread will build the selected project and all
         * its dependencies as appeard in the build order dialog
         * \param projectName the selected project to build
         * \param configurationName the workspace selected configuration
         */
        CompileRequest(const QueueCommand& buildInfo, const wxString& fileName = wxEmptyString,
            bool runPremakeOnly = false, bool preprocessOnly = false);
        virtual ~CompileRequest();

        // process the request
        virtual void Process(IManager* manager = NULL);

        // setters/getters
        inline const wxString& GetProjectName() const { return m_info.GetProject(); }
    private:
        wxString m_fileName;
        bool m_premakeOnly;
        bool m_preprocessOnly;
    };

    class _gsgsAPI CustomBuildRequest : public ShellCommand
    {
    public:
        CustomBuildRequest(const QueueCommand & buildInfo, const wxString & fileName);
        virtual ~CustomBuildRequest();

        // process the request
        virtual void Process(IManager * manager = NULL);

        // setters/getters
        inline const wxString & GetProjectName() const { return m_info.GetProject(); }
    protected:
        /**
         * @brief if the build includes post or pre builds we need to wrap
         * the command with a script that prepend / append the user commands
         * @return true if the command was modified but this function, false (i.e. there are no
         * post/pre build events) otherwise
         */
        bool DoUpdateCommand(IManager* manager, wxString& cmd, ProjectPtr proj, BuildConfigPtr bldConf, bool isClean);
    private:
        wxString m_fileName;
    };
}
#endif
