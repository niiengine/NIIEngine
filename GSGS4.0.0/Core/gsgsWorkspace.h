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
#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "gsgsPreInclude.h"
#include "gsgsSingleton.h"
#include "project.h"
#include "gsgsEvent.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsOptionsConfig.h"
#include "gsgsStringUtil.h"

#define WORKSPACE_XML_VERSION "10.0.0"
namespace gsgs
{
    class LocalWorkspace;

    class _gsgsAPI CxxWS : public IWorkspace
    {
    public:
        typedef std::unordered_map<wxString, ProjectPtr> ProjectMap_t;
    public:
        CxxWS();
        virtual ~CxxWS();

        static CxxWS & singleton();

        static void Release();

        wxString GetVersion() const;

        /**
         * @brief return the local workspace settings associated with this workspace
         * @return
         */
        inline LocalWorkspace* GetLocalWorkspace() const { return m_localWorkspace; }

        /// @copydetails IWorkspace::GetFileName
        wxFileName GetFileName() const { return m_fileName; }
        /// @copydetails IWorkspace::GetProjectFiles
        virtual void GetProjectFiles(const wxString & projectName, wxArrayString & files) const;
        /// @copydetails IWorkspace::GetWorkspaceFiles
        virtual void GetWorkspaceFiles(wxArrayString & files) const;
        /// @copydetails IWorkspace::IsBuildSupported
        virtual bool IsBuildSupported() const;
        /// @copydetails IWorkspace::IsProjectSupported
        virtual bool IsProjectSupported() const;
        /// @copydetails IWorkspace::GetFilesMask
        virtual wxString GetFilesMask() const;
        /// @copydetails IWorkspace::GetProjectFromFile
        virtual wxString GetProjectFromFile(const wxFileName& filename) const;
        /// @copydetails IWorkspace::GetProjectFileName
        virtual wxFileName GetProjectFileName(const wxString& projectName) const;
        /// @copydetails IWorkspace::GetActiveProjectName
        virtual wxString GetActiveProjectName() const;
        /// @copydetails IWorkspace::GetWorkspaceProjects
        virtual wxArrayString GetWorkspaceProjects() const;

        /**
         * @brief move 'projectName' to folder. Create the folder if it does not exists
         */
        bool MoveProjectToFolder(const wxString& projectName, const wxString& folderPath, bool saveAndReload);

        /**
         * @brief create workspace folder
         * @param path the path is separated by "/".e.g. "projects/plugins/C++"
         */
        bool CreateWorkspaceFolder(const wxString& path);

        /**
         * @brief delete workspace folder. Notice that this will also remove (but not delete) all the projects from the
         * workspace
         */
        void DeleteWorkspaceFolder(const wxString& path);

        void GetWorkspaceFolders(wxArrayString & out) const { DoVisitWorkspaceFolders(m_doc.GetRoot(), "", out); }

        /**
         * @brief create 'compile_commands' json object for the workspace projects (only the enabled ones)
         */
        cJSON * CreateCompileCommandsJSON() const;

        /**
         * @brief generate compile_flags.txt for each project
         */
        void CreateCompileFlags() const;

        /**
         * @brief return list of compilers used by all the projects (this is done for the active configuration only)
         */
        void GetCompilers(gsgsStringSet & compilers);

        /**
         * @brief replace compilers for all projects and build configurations
         * @param compilers a map of compilers where the key is the old compiler and the value is the new compiler
         */
        void ReplaceCompilers(gsgsStringMap & compilers);

        bool CreateWorkspace(const wxString & name, const wxString & path, wxString & errMsg);

        bool OpenWorkspace(const wxString & fileName, wxString & errMsg);

        /**
         * @brief this function opens the workspace, but does not open the tags
         * completion database etc. It only loads the XML and then adds all the
         * projects XML. It is mainly needed when you just need to explore the
         * workspace from a secondary thread with no events / UI to get in the
         * way
         */
        bool OpenReadOnly(const wxString & fileName, wxString & errMsg);

        void CloseWorkspace();

        void ReloadWorkspace();

        bool CreateProject(const wxString& name, const wxString& path, const wxString& type,
            const wxString & folder, bool addToBuildMatrix, wxString& errMsg);

        void RenameProject(const wxString& old, const wxString& name);

        /**
         * \brief get a string property from the worksapce file
         * \returns property value or wxEmptyString
         */
        wxString GetStringProperty(const wxString& propName, wxString& errMsg);

        /**
         * Find a project by name
         * \param projName project name
         * \param errMsg [output] incase an error, report the error to the caller
         * \returns ProjectPtr or NULL if no match was found
         */
        ProjectPtr FindProjectByName(const wxString& projName, wxString& errMsg) const;

        /**
         * Return all project names under this workspace
         */
        void GetProjectList(wxArrayString& list) const;

        /**
         * Add an existing project to the workspace. If no workspace is open,
         * this function does nothing
         * \param path project file path name to add
         * \param errMsg [output] incase an error, report the error to the caller
         */
        bool AddProject(const wxString& path, wxString& errMsg);
        /**
         * @brief add project to the workspace into a given workspace folder
         */
        bool AddProject(const wxString& path, const wxString & workspaceFolder, wxString& errMsg);
        /**
         * Remove project from the workspace. This function does not delete
         * any file related to the project but simply removes it from the workspace
         * \param name project name to remove
         * \param errMsg [output] incase an error, report the error to the caller
         * \return true on success false otherwise
         */
        bool RemoveProject(const wxString& name, wxString & errMsg, const wxString & workspaceFolder);

        /**
         * @brief return the paths of all projects in the workspace (full paths)
         */
        wxArrayString GetAllProjectPaths();

        /**
         * Set project as active
         * \param name  project name
         */
        void SetActiveProject(const wxString& name);

        /**
         * Add new virtual directoy to workspace
         * \param vdFullPath virtual directory full path
         * \param errMsg [output] incase an error, report the error to the caller
         * \return true on success false otherwise
         */
        bool CreateVirtualDirectory(const wxString& vdFullPath, wxString& errMsg, bool mkPath = false);

        /**
         * @brief return true if the given virtual directory already exists
         */
        bool IsVirtualDirectoryExists(const wxString& vdFullPath);

        /**
         * Remove virtual directoy to workspace
         * \param vdFullPath virtual directory full path
         * \param errMsg [output] incase an error, report the error to the caller
         * \return true on success false otherwise
         */
        bool RemoveVirtualDirectory(const wxString& vdFullPath, wxString& errMsg);

        /**
         * Add new file to project. The project name is taken from the virtual directory full path
         * \param vdFullPath vritual directory full path including project
         * \param fileName file name to add
         * \param errMsg output
         * \return true on success, false otherwise
         */
        bool AddNewFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg);

        /**
         * Remove file from a project. The project name is taken from the virtual directory full path
         * \param vdFullPath vritual directory full path including project
         * \param fileName file name to remove
         * \param errMsg output
         * \return true on success, false otherwise
         */
        bool RemoveFile(const wxString& vdFullPath, const wxString& fileName, wxString& errMsg);

        /**
         * Save workspace & projects settings
         */
        void Save();

        /**
         * Return a node pointing to any workspace-wide editor preferences
         */
        wxXmlNode* GetWorkspaceEditorOptions() const;

        /**
         * Add or update local workspace options
         */
        void SetWorkspaceEditorOptions(LocalOptionsConfigPtr opts);

        /**
         * Return the configuration mapping for the workspace. 'Configuration Mapping' is
         * the build matrix that should be used for this workspace, it contains all possible
         * configurations and which project specific configuration should be built in that
         * configuration
         */
        BuildMatrixPtr GetBuildMatrix() const;

        /**
         * Store configuration mapping for the workspace, overriding the current one
         * \param mapping new mapping to store
         */
        void SetBuildMatrix(BuildMatrixPtr mapping);

        /**
         * Return the workspace name
         */
        wxString GetName() const;

        /**
         * return the project build configuration that matches the
         * workspace selected configuration from the build matrix
         */
        BuildConfigPtr GetProjBuildConf(const wxString& projectName, const wxString& confName) const;

        /**
         * \brief add project to the workspace build matrix. By default gsgs will try to match the best project
         * configuration
         * to the workspace configuration (i.e. Debug -> Debug, if no suitable match found, it will use the first one)
         * \param prj
         */
        void AddProjectToBuildMatrix(ProjectPtr prj);

        /**
         * @brief return the workspace environment variables
         */
        wxString GetParserMacros();

        /**
         * @brief return the workspace environment variables
         */
        wxString GetEnvironmentVariabels();

        /**
         * @brief set the workspace environment variables. These environment variables
         * will be kept in the workspace file (i.e. they are portable)
         */
        void SetEnvironmentVariabels(const wxString& envvars);

        /**
         * @brief return the selected workspace configuration
         */
        WorkspaceConfigurationPtr GetSelectedConfig() const;

        /**
         * return the last modification time (on disk) of editor's underlying file
         */
        time_t GetFileLastModifiedTime() const;

        /**
         * return/set the last modification time that was made by the editor
         */
        time_t GetWorkspaceLastModifiedTime() const { return m_modifyTime; }

        void SetWorkspaceLastModifiedTime(time_t time) { m_modifyTime = time; }

        /**
         * @brief return true if a workspace is opened
         */
        bool IsOpen() const;

        /**
         * @brief return the path to the workspace private folder (WORKSPACE_PATH/.gsgs)
         * @return the path to the folder, if there is no workspace open, return an empty path
         */
        wxString GetPrivateFolder() const;

        /**
         * @brief return the tags database full path
         */
        wxFileName GetTagsFileName() const;

        /**
         * @brief return project by name
         */
        ProjectPtr GetProject(const wxString& name) const;

        /**
         * @brief return the active project
         */
        ProjectPtr GetActiveProject() const;

        /**
         * @brief clear the workspace include path cache (for each project)
         */
        void ClearIncludePathCache();

        /**
         * @brief return list of files that are exluded for a given workspace configuration
         */
        size_t GetExcludeFilesForConfig(std::vector<wxString>& files, const wxString& workspaceConfigName = "");

    private:
        void DoUpdateBuildMatrix();
        /**
         * @brief mark all projects as non-active
         */
        void DoUnselectActiveProject();

        /**
         * @brief load projects from the XML file
         */
        void DoLoadProjectsFromXml(wxXmlNode* parentNode, const wxString& folder, std::vector<wxXmlNode*>& removedChildren);

        // return the wxXmlNode instance for the give path
        // the path is separated by "/"
        // return NULL if no such virtual directory exists
        wxXmlNode* DoGetWorkspaceFolderXmlNode(const wxString& path);

        /**
         * @brief return the wxXmlNode for this project
         */
        wxXmlNode* DoGetProjectXmlNode(const wxString& projectName);

        /**
         * @brief create workspace folder and return the wxXmlNode. If this path already exists, return it
         * @param path the path is separated by "/".e.g. "projects/plugins/C++"
         */
        wxXmlNode* DoCreateWorkspaceFolder(const wxString& path);

        bool DoLoadWorkspace(const wxString& fileName, wxString& errMsg);

        /**
         * @brief return list of all projects nodes in this workspace
         */
        std::list<wxXmlNode*> DoGetProjectsXmlNodes() const;

        void DoVisitWorkspaceFolders(wxXmlNode* parent, const wxString& curpath, wxArrayString& paths) const;
        /**
         * Do the actual add project
         * \param path project file path
         * \param errMsg [output] incase an error, report the error to the caller
         */
        ProjectPtr DoAddProject(const wxString& path, const wxString& projectVirtualFolder, wxString& errMsg);
        ProjectPtr DoAddProject(ProjectPtr proj);

        void RemoveProjectFromBuildMatrix(ProjectPtr prj);

        bool SaveXmlFile();

        void SyncToLocalWorkspaceSTParserPaths();
        void SyncFromLocalWorkspaceSTParserPaths();
        void SyncToLocalWorkspaceSTParserMacros();
        void SyncFromLocalWorkspaceSTParserMacros();
    protected:
        wxXmlDocument m_doc;
        wxFileName m_fileName;
        ProjectMap_t m_projects;
        BuildMatrixPtr m_buildMatrix;
        LocalWorkspace* m_localWorkspace = nullptr;
        time_t m_modifyTime;
        bool m_saveOnExit;
    };
}
#define gsgs_CppWorker() gsgs::CxxWS::singleton()
#endif // WORKSPACE_H
