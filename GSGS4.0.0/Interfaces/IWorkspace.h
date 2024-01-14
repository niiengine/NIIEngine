

#ifndef IWORKSPACE_H
#define IWORKSPACE_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI IWorkspace : public wxEvtHandler
    {
    public:
        typedef std::list<IWorkspace*> List_t;

    public:
        IWorkspace() {}
        virtual ~IWorkspace();

        virtual wxFileName GetFileName() const = 0;

        /**
         * @brief set the workspace type
         * For example: "C++ Workspace", "PHP Workspace" etc
         */
        inline void SetWorkspaceType(const wxString& type) { m_workspaceType = type; }

        /**
         * @brief return the workspace name
         */
        inline const wxString& GetWorkspaceType() const { return m_workspaceType; }

        /**
         * @brief is this workspace support the build concept?
         * e.g. for C++ workspace, the answer is 'true', for PHP workspace, this will be 'false'
         */
        virtual bool IsBuildSupported() const = 0;

        /**
         * @brief is this workspace support the project concept?
         */
        virtual bool IsProjectSupported() const = 0;

        /**
         * @brief return the file masking for this workspace
         */
        virtual wxString GetFilesMask() const = 0;

        /**
         * @brief return the project name of a file.
         * If the workspace does not support projects, return an empty string
         * If the we could not match a project for the given filename, return empty string
         */
        virtual wxString GetProjectFromFile(const wxFileName& filename) const = 0;

        /**
         * @brief return a list of files for this workspace
         * @param files [output] list of files in absolute path
         */
        virtual void GetWorkspaceFiles(wxArrayString& files) const = 0;
        /**
         * @brief return list of files belonged to the prokect. If the workspace does not support
         * projects, do not modify 'files'
         * @param projectName the project name
         * @param files [output] list of files in absolute path
         */
        virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const = 0;

        /**
         * @brief return the active project name. For workspace that does not support
         * projects, return an empty string
         */
        virtual wxString GetActiveProjectName() const = 0;

        /**
         * @brief return the underlying file for a given project name
         */
        virtual wxFileName GetProjectFileName(const wxString& projectName) const = 0;

        /**
         * @brief return list of projects for this workspace
         */
        virtual wxArrayString GetWorkspaceProjects() const = 0;
    protected:
        wxString m_workspaceType;
    };
}
#endif
