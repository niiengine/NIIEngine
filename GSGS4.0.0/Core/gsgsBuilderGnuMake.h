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

#ifndef _GSGS_BuilderGnuMake_H_
#define _GSGS_BuilderGnuMake_H_

#include "gsgsPreInclude.h"
#include "project.h"

namespace gsgs
{
    class _gsgsAPI Builder
    {
    public:
        Builder(const wxString& name);
        virtual ~Builder();

        /**
         * Normalize the configuration name, this is done by removing any trailing and leading
         * spaces from the string, and replacing any space character with underscore.
         */
        static wxString NormalizeConfigName(const wxString& confgName);

        /**
         * \return the builder name
         */
        inline const wxString & GetName() const { return m_name; }

        // ================ API ==========================
        // The below API as default implementation, but can be
        // overridden in the derived class
        // ================ API ==========================

        /**
         * @brief set this builder as the active builder. It also makes sure that all other
         * builders are set as non-active
         */
        virtual void SetActive();

        /**
         * @brief return true if this builder is the active one
         * @return
         */
        virtual bool IsActive() const { return m_isActive; }

        // ================ API ==========================
        // The below API must be implemented by the
        // derived class
        // ================ API ==========================

        /**
         * Export the build system specific file (e.g. GNU makefile, Ant file etc)
         * to allow users to invoke them manualy from the command line
         * \param project project to export.
         * \param errMsg output
         * \return true on success, false otherwise.
         */
        virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
            bool isProjectOnly, bool force, wxString& errMsg) = 0;

        /**
         * Return the command that should be executed for performing the clean
         * task
         */
        virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) = 0;

        /**
         * Return the command that should be executed for performing the build
         * task for a given project
         */
        virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) = 0;

        //-----------------------------------------------------------------
        // Project Only API
        //-----------------------------------------------------------------
        /**
         * Return the command that should be executed for performing the clean
         * task - for the project only (excluding dependencies)
         */
        virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) = 0;

        /**
         * Return the command that should be executed for performing the build
         * task for a given project - for the project only (excluding dependencies)
         */
        virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) = 0;

        /**
         * \brief create a command to execute for compiling single source file
         * \param project
         * \param fileName
         * \param errMsg [output]
         * \return the command
         */
        virtual wxString GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments, const wxString& fileName) = 0;

        /**
         * \brief create a command to execute for preprocessing single source file
         * \param project
         * \param fileName
         * \param errMsg [output]
         * \return the command
         */
        virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
            const wxString& arguments, const wxString& fileName, wxString& errMsg) = 0;

        /**
         * @brief return the 'rebuild' command
         * @param project
         * @param confToBuild
         * @return
         */
        virtual wxString GetPORebuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments) = 0;

        /**
         * @brief return the output file path. Return here a path for the output file (the executable or library)
         * Use gsgs macros to make it 'portable' and configuration aware
         * @return if an empty string is returned, gsgs will use the default
         * $(IntermediateDirectory)/$(ProjectName)
         */
        virtual wxString GetOutputFile() const { return wxEmptyString; }
    protected:
        wxString m_name;
        bool m_isActive;
    };

    /*
     * Build using a generated CMakefile - this is made as a traditional multistep build :
     *  sources -> (preprocess) -> compile -> link -> exec/lib.
     */
    class _gsgsAPI BuilderNMake : public Builder
    {
    public:
        BuilderNMake();
        BuilderNMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
        virtual ~BuilderNMake();

        // Implement the Builder Interface
        virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
            bool isProjectOnly, bool force, wxString& errMsg);
        virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments, const wxString& fileName);
        virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments, const wxString& fileName, wxString& errMsg);
        virtual wxString GetPORebuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
    protected:
        virtual void CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        virtual void CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName,
            const wxString& projName, const wxArrayString& depsProj);
        virtual void CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        // Override default methods defined in the builder interface
        virtual wxString GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
            const wxString& arguments, bool isCommandlineCommand) const;
        bool SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName);

    private:
        void GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force, const wxArrayString& depsProj);
        void CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName, wxString& text);
        void CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, const wxString& projName);
        void CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text);
        void CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text);
        void CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text);

        wxString GetCdCmd(const wxFileName& path1, const wxFileName& path2);

        wxString ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf);
        wxString ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf);
        wxString ParseLibs(const wxString& libs);
        wxString ParsePreprocessor(const wxString& prep);
        bool HasPrebuildCommands(BuildConfigPtr bldConf) const;
        bool HasPostbuildCommands(BuildConfigPtr bldConf) const;

        wxString GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj,
            const wxString& confToBuild);
        wxString GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target,
            bool addCleanTarget, bool cleanOnly);
        wxString DoGetCompilerMacro(const wxString& filename);
        wxString DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp);
        wxString DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath = "");
    private:
        size_t m_objectChunks;
        clProjectFile::Vec_t m_allFiles;
    };

    /*
     * Build using a generated (Gnu) Makefile - this is made as a traditional multistep build :
     *  sources -> (preprocess) -> compile -> link -> exec/lib.
     */
    class _gsgsAPI BuilderGnuMake : public Builder
    {
    public:
        BuilderGnuMake();
        BuilderGnuMake(const wxString& name, const wxString& buildTool, const wxString& buildToolOptions);
        virtual ~BuilderGnuMake();

        // Implement the Builder Interface
        virtual bool Export(const wxString& project, const wxString& confToBuild, const wxString& arguments,
            bool isProjectOnly, bool force, wxString& errMsg);
        virtual wxString GetBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetPOBuildCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetPOCleanCommand(const wxString& project, const wxString& confToBuild, const wxString& arguments);
        virtual wxString GetSingleFileCmd(const wxString& project, const wxString& confToBuild, const wxString& arguments,
            const wxString& fileName);
        virtual wxString GetPreprocessFileCmd(const wxString& project, const wxString& confToBuild,
            const wxString& arguments, const wxString& fileName, wxString& errMsg);
        virtual wxString GetPORebuildCommand(const wxString& project, const wxString& confToBuild,
            const wxString& arguments);
    protected:
        virtual void CreateListMacros(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateSrcList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateObjectList(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        virtual void CreateLinkTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, wxString& targetName,
            const wxString& projName, const wxArrayString& depsProj);
        virtual void CreateFileTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        void CreateCleanTargets(ProjectPtr proj, const wxString& confToBuild, wxString& text);
        // Override default methods defined in the builder interface
        virtual wxString GetBuildToolCommand(const wxString& project, const wxString& confToBuild,
            const wxString& arguments, bool isCommandlineCommand) const;

        bool SendBuildEvent(int eventId, const wxString& projectName, const wxString& configurationName);

    private:
        void GenerateMakefile(ProjectPtr proj, const wxString& confToBuild, bool force, const wxArrayString& depsProj);
        void CreateConfigsVariables(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreateMakeDirsTarget(ProjectPtr proj, BuildConfigPtr bldConf, const wxString& targetName, wxString& text);
        void CreateTargets(const wxString& type, BuildConfigPtr bldConf, wxString& text, const wxString& projName);
        void CreatePreBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreatePostBuildEvents(ProjectPtr proj, BuildConfigPtr bldConf, wxString& text);
        void CreatePreCompiledHeaderTarget(BuildConfigPtr bldConf, wxString& text);
        void CreateCustomPreBuildEvents(BuildConfigPtr bldConf, wxString& text);
        void CreateCustomPostBuildEvents(BuildConfigPtr bldConf, wxString& text);

        wxString GetCdCmd(const wxFileName& path1, const wxFileName& path2);

        wxString ParseIncludePath(const wxString& paths, const wxString& projectName, const wxString& selConf);
        wxString ParseLibPath(const wxString& paths, const wxString& projectName, const wxString& selConf);
        wxString ParseLibs(const wxString& libs);
        wxString ParsePreprocessor(const wxString& prep);
        bool HasPrebuildCommands(BuildConfigPtr bldConf) const;
        bool HasPostbuildCommands(BuildConfigPtr bldConf) const;

        wxString GetProjectMakeCommand(const wxFileName& wspfile, const wxFileName& projectPath, ProjectPtr proj,
            const wxString& confToBuild);
        wxString GetProjectMakeCommand(ProjectPtr proj, const wxString& confToBuild, const wxString& target, size_t flags);
        wxString DoGetCompilerMacro(const wxString& filename);
        wxString DoGetTargetPrefix(const wxFileName& filename, const wxString& cwd, CompilerPtr cmp);
        wxString DoGetMarkerFileDir(const wxString& projname, const wxString& projectPath = "");
    protected:
        enum eBuildFlags {
            kCleanOnly = (1 << 0),
            kAddCleanTarget = (1 << 1),
            kIncludePreBuild = (1 << 2),
            kIncludePostBuild = (1 << 3),
        };
    private:
        size_t m_objectChunks;
        Project::FilesMap_t* m_projectFilesMetadata;
    };

    class _gsgsAPI BuildManager : public wxEvtHandler
    {
    public:
        typedef std::map<wxString, BuilderPtr>::const_iterator ConstIterator;
    public:
        static BuildManager& singleton();
        static void Release();
        void AddBuilder(BuilderPtr builder);
        void RemoveBuilder(const wxString & name);
        void GetBuilders(std::list<wxString> & list);
        BuilderPtr GetBuilder(const wxString & name);
    protected:
        BuildManager();
        virtual ~BuildManager();
    private:
        std::map<wxString, BuilderPtr> m_builders;
    };
}
#define gsgs_Build() gsgs::BuildManager::singleton()
#endif // COMPILERSDETECTORMANAGER_H
