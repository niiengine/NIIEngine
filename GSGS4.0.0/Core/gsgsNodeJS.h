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
#ifndef gsgsNODEJS_H
#define gsgsNODEJS_H

#include "gsgsPreInclude.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"

namespace gsgs
{
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_NODE_COMMAND_TERMINATED, gsgs::ProcessEvent);

    class _gsgsAPI NodeJS : public wxEvtHandler
    {
    protected:
        class ProcessData
        {
        protected:
            wxFileName filename;
            wxString output;
            wxString uid;
            wxEvtHandler* sink = nullptr;

        public:
            ProcessData() {}
            virtual ~ProcessData() {}
            void SetFilename(const wxFileName& filename) { this->filename = filename; }
            void SetOutput(const wxString& output) { this->output = output; }
            const wxFileName& GetFilename() const { return filename; }
            const wxString& GetOutput() const { return output; }
            wxString& GetOutput() { return output; }
            void SetUid(const wxString& uid) { this->uid = uid; }
            const wxString& GetUid() const { return this->uid; }
            wxString& GetUid() { return this->uid; }
            void SetSink(wxEvtHandler* sink) { this->sink = sink; }
            wxEvtHandler* GetSink() const { return sink; }
        };

    protected:
        bool m_initialised = false;
        wxFileName m_npm;
        wxFileName m_node;

        std::unordered_map<IProcess*, ProcessData> m_processes;

    protected:
        void OnProcessOutput(gsgs::ProcessEvent& event);
        void OnProcessTerminated(gsgs::ProcessEvent& event);
        void ProcessLintOuput(const wxFileName& fn, const wxString& output);

    private:
        NodeJS();
        void BindEvents();
        void UnBindEvents();

    public:
        static NodeJS& Get();
        virtual ~NodeJS();

        /**
         * @brief execute NodeJS script. Return the wxProcess executed
         */
        wxProcess* RunScript(const wxArrayString& argv, const wxString& workingDirectory = "",
                             size_t execFlags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | wxEXEC_HIDE_CONSOLE);

        /**
         * @brief initialise this class by finding all the binaries required
         */
        bool Initialise(const wxArrayString& hints = {});

        /**
         * @brief terminate this instance
         */
        void Shutdown();

        /**
         * @brief install an npm package
         */
        bool NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                        wxEvtHandler* sink = nullptr, const wxString& uid = "");

        /**
         * @brief install package without user interference
         */
        bool NpmSilentInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                              wxEvtHandler* sink = nullptr, const wxString& uid = "");

        /**
         * @brief run npm init in a directory
         */
        bool NpmInit(const wxString& workingDirectory, wxEvtHandler* sink = nullptr);

        /**
         * @brief lint file. The error are reported in the output view
         */
        void LintFile(const wxFileName& filename);

        bool IsInitialised() const { return m_initialised; }
        const wxFileName& GetNode() const { return m_node; }
        const wxFileName& GetNpm() const { return m_npm; }
    };
}
#endif // CLNODEJS_H
