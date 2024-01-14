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
#ifndef PROJECT_H
#define PROJECT_H

#include "gsgsPreInclude.h"
#include "gsgsJSON.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsTree.h"
#include "gsgsBuildConfig.h"
#include "wx/treectrl.h"
#include "wx/sharedptr.h"
#include "wx/xml/xml.h"
#include "xmlutils.h"

#define PROJECT_TYPE_STATIC_LIBRARY "Static Library"
#define PROJECT_TYPE_DYNAMIC_LIBRARY "Dynamic Library"
#define PROJECT_TYPE_EXECUTABLE "Executable"

namespace gsgs
{
    struct VisualWorkspaceNode {
        wxString name;
        int type;
        wxTreeItemId itemId;
    };

    class ProjectSettingsCookie
    {
    public:
        std::map<wxString, BuildConfigPtr>::const_iterator iter;
    };

    class _gsgsAPI ProjectSettings : public ObjectData
    {
    public:
        /**
         * Clone this project settings, the cloned object is allocated on the heap
         * and it is the responsibility of the caller to call delete when there are no
         * more refreneces to it
         * \return
         */
        ProjectSettings *Clone() const;

        /**
         * Construct ProjectSettings object from its XML representation
         * \param node XML node of the settings object
         */
        ProjectSettings(wxXmlNode *node);
        virtual ~ProjectSettings();

        /**
         * Serialize the object to XML representation
         */
        wxXmlNode *ToXml() const;

        // enumaration 
        /**
         * Find the first build configuration by name
         * \param configName build configuration name to find
         * \param merge merge with global settings or not
         * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
         */
        BuildConfigPtr GetBuildConfiguration(const wxString &configName, bool merge = false) const;

        /**
         * enumerate through all the build build configuration, this function returns the first build configuration found
         * \param cookie an object to allow this call to be reentrant by multiple threads
         * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
         */
        BuildConfigPtr GetFirstBuildConfiguration(ProjectSettingsCookie &cookie) const;
        /**
         * enumerate through all the build build configuration, this function returns the next build configuration found
         * cookie should be the same as passed to previous calls to GetFirstBuildConfiguration or
         * \param cookie an object to allow this call to be reentrant by multiple threads GetNextBuildConfiguration
         * \return build configuration smart pointer, will be deleted automatically when no more refrences to it
         */
        BuildConfigPtr GetNextBuildConfiguration(ProjectSettingsCookie &cookie) const;

        /**
         * Insert or Update BuildConfigPtr, if a build configuration with this name already exist,
         * it is replaced
         * \param bc build configuration object
         */
        void SetBuildConfiguration(const BuildConfigPtr bc);

        /**
         * Remove build configuration from the project settings. The build configuration objects stored under
         * this object are stored using smart pointers, thus if there are no more refrences to the removed build
         * configuration it will be deleted automatically
         * \param configName configuration name to remove.
         */
        void RemoveConfiguration(const wxString &configName);

        /**
         * \brief Return the global settings, common to all configurations
         */
        inline BuildConfigCommonPtr GetGlobalSettings() const { return m_globalSettings; }

        /**
         * \brief Set the global settings
         * \param global settings
         */
        inline void SetGlobalSettings(BuildConfigCommonPtr globalSettings) { m_globalSettings = globalSettings; }

        //setters/getters
        wxString GetProjectType(const wxString &confName);
        inline void SetProjectType(const wxString &type) { m_projectType = type; }
    private:
        std::map<wxString, BuildConfigPtr> m_configs;
        BuildConfigCommonPtr               m_globalSettings;
        wxString                           m_projectType;
    };

    typedef gsgs::SharedPtr<ProjectSettings> ProjectSettingsPtr;

    /**
     * \class ProjectItem
     * a node item that represents a displayable project item
     * 'Displayable' items are items that will be shown in the
     * FileView control
     *
     * \author Eran
     */
    class _gsgsAPI ProjectItem
    {
    public:
        // The visible items
        enum { TypeInvalid = -1, TypeVirtualDirectory, TypeProject, TypeFile, TypeWorkspace, TypeWorkspaceFolder };
    public:
        //---------------------------------------------------------------
        // Constructors, destructor and assignment operator
        //---------------------------------------------------------------
        ProjectItem(const wxString& key, const wxString& displayName, const wxString& file, int kind)
            : m_key(key)
            , m_displayName(displayName)
            , m_file(file)
            , m_kind(kind)
        {
        }

        ProjectItem()
            : m_key(wxEmptyString)
            , m_displayName(wxEmptyString)
            , m_file(wxEmptyString)
            , m_kind(TypeProject)
        {
        }

        virtual ~ProjectItem() {}

        ProjectItem(const ProjectItem& item) { *this = item; }

        ProjectItem& operator=(const ProjectItem& item)
        {
            if (this == &item) { return *this; }

            m_key = item.m_key;
            m_displayName = item.m_displayName;
            m_file = item.m_file;
            m_kind = item.m_kind;
            return *this;
        }

        //-----------------------------------------
        // Setters / Getters
        //-----------------------------------------
        inline const wxString& GetDisplayName() const { return m_displayName; }
        inline const wxString& GetFile() const { return m_file; }
        inline int GetKind() const { return m_kind; }

        inline void SetDisplayName(const wxString& displayName) { m_displayName = displayName; }
        inline void SetFile(const wxString& file) { m_file = file; }
        inline void SetKind(int kind) { m_kind = kind; }

        //------------------------------------------
        // operations
        inline const wxString& Key() const { return m_key; }

        // Aliases
        inline bool IsProject() const { return m_kind == TypeProject; }
        inline bool IsFile() const { return m_kind == TypeFile; }
        inline bool IsVirtualFolder() const { return m_kind == TypeVirtualDirectory; }
        inline bool IsInvalid() const { return m_kind == TypeInvalid; }
        inline bool IsWorkspaceFolder() const { return m_kind == TypeWorkspaceFolder; }
        inline bool IsWorkspace() const { return m_kind == TypeWorkspace; }
    public:
        wxString m_key;
        wxString m_displayName;
        wxString m_file;
        int m_kind;
    };

    // useful typedefs
    typedef Tree<wxString, ProjectItem> ProjectTree;
    typedef SharedPtr<ProjectTree> ProjectTreePtr;
    typedef TreeNode<wxString, ProjectItem> ProjectTreeNode;

    typedef SharedPtr<Project> ProjectPtr;
    typedef std::set<wxFileName> FileNameSet_t;
    typedef std::vector<wxFileName> FileNameVector_t;

    // -----------------------------------------
    // File meta data
    // -----------------------------------------
    class _gsgsAPI clProjectFile
    {
    public:
        clProjectFile()
            : m_flags(0)
            , m_xmlNode(nullptr)
        {
        }
        ~clProjectFile() {}

        void SetExcludeConfigs(Project* project, const gsgsStringSet& excludeConfigs);
        void SetExcludeConfigs(Project* project, const wxArrayString& excludeConfigs);

        inline void SetXmlNode(wxXmlNode* xmlNode) { this->m_xmlNode = xmlNode; }
        inline wxXmlNode* GetXmlNode() const { return m_xmlNode; }
        inline const gsgsStringSet& GetExcludeConfigs() const { return m_excludeConfigs; }
        inline gsgsStringSet& GetExcludeConfigs() { return m_excludeConfigs; }
        inline void SetFilenameRelpath(const wxString& filenameRelpath) { this->m_filenameRelpath = filenameRelpath; }
        inline const wxString& GetFilenameRelpath() const { return m_filenameRelpath; }
        inline void SetFilename(const wxString& filename) { this->m_filename = filename; }
        inline void SetVirtualFolder(const wxString& virtualFolder) { this->m_virtualFolder = virtualFolder; }
        inline const wxString& GetFilename() const { return m_filename; }
        inline const wxString& GetVirtualFolder() const { return m_virtualFolder; }
        inline void SetFlags(size_t flags) { this->m_flags = flags; }
        inline size_t GetFlags() const { return m_flags; }
        /**
         * @brief return true if this file should be execluded from the build of a specific configuration
         */
        inline bool IsExcludeFromConfiguration(const wxString& config) const { return m_excludeConfigs.count(config); }
        void Rename(Project* project, const wxString& newName);

        /**
         * @brief delete this file
         * @param project
         */
        void Delete(Project* project, bool deleteXml = false);

        typedef wxSharedPtr<clProjectFile> Ptr_t;
        typedef std::vector<clProjectFile::Ptr_t> Vec_t;
    private:
        wxString m_filename;
        wxString m_virtualFolder;
        wxString m_filenameRelpath;
        gsgsStringSet m_excludeConfigs;
        wxXmlNode* m_xmlNode;
        size_t m_flags;
    };

    class _gsgsAPI clProjectFolder
    {
    private:
        wxString m_fullpath;
        wxString m_name;
        gsgsStringSet m_files;
        wxXmlNode* m_xmlNode;

    public:
        typedef wxSharedPtr<clProjectFolder> Ptr_t;
        typedef std::vector<clProjectFolder> Vect_t;

    public:
        clProjectFolder(const wxString& fullpath, wxXmlNode* node)
            : m_fullpath(fullpath)
            , m_xmlNode(node)
        {
            m_name = fullpath.AfterLast(':');
        }

        inline void SetFiles(const gsgsStringSet& files) { this->m_files = files; }
        inline void SetFullpath(const wxString& fullpath) { this->m_fullpath = fullpath; }
        inline void SetName(const wxString& name) { this->m_name = name; }
        inline void SetXmlNode(wxXmlNode* xmlNode) { this->m_xmlNode = xmlNode; }
        inline const gsgsStringSet& GetFiles() const { return m_files; }
        inline gsgsStringSet& GetFiles() { return m_files; }
        inline const wxString& GetFullpath() const { return m_fullpath; }
        inline const wxString& GetName() const { return m_name; }
        inline wxXmlNode* GetXmlNode() const { return m_xmlNode; }
        /**
         * @brief rename a file
         * @param fullpath the current file fullpath
         * @param newName name only. no path
         */
        bool RenameFile(Project* project, const wxString& fullpath, const wxString& newName);
        /**
         * @brief rename this folder
         */
        bool Rename(Project* project, const wxString& newName);

        /**
         * @brief add child folder
         * @param name
         * @return
         */
        clProjectFolder::Ptr_t AddFolder(Project* project, const wxString& name);

        /**
         * @brief add file to this folder
         */
        clProjectFile::Ptr_t AddFile(Project* project, const wxString& fullpath);

        /**
         * @brief return true if this folder has a child folder with a given name
         */
        bool IsFolderExists(Project* project, const wxString& name) const;

        /**
         * @brief reutrn child folder. Can be nullptr
         */
        clProjectFolder::Ptr_t GetChild(Project* project, const wxString& name) const;

        /**
         * @brief return list of all subfolders from this folder
         */
        void GetSubfolders(wxArrayString& folders, bool recursive = true) const;

        /**
         * @brief delete this folder and all its children
         */
        void DeleteRecursive(Project* project);

        /**
         * @brief delete all files from this folder
         */
        void DeleteAllFiles(Project* project);
    };

    /**
     * \ingroup LiteEditor
     *
     *
     * \date 04-15-2007
     *
     * \author Eran
     *
     * \par license
     * This code is absolutely free to use and modify. The code is provided "as is" with
     * no expressed or implied warranty. The author accepts no liability if it causes
     * any damage to your computer, causes your pet to fall ill, increases baldness
     * or makes your car start emitting strange noises when you start it up.
     * This code has no bugs, just undocumented features!
     *
     * \todo
     *
     * \bug
     *
     */
    class _gsgsAPI Project
    {

    public:
        typedef std::unordered_map<wxString, clProjectFile::Ptr_t> FilesMap_t;
        typedef std::unordered_map<wxString, clProjectFolder::Ptr_t> FoldersMap_t;

        friend class CxxWS;
        friend class clProjectFolder;
        friend class clProjectFile;

    private:
        wxXmlDocument m_doc;
        wxFileName m_fileName;
        wxString m_projectPath;
        bool m_tranActive;
        bool m_isModified;
        time_t m_modifyTime;
        CxxWS* m_workspace;
        ProjectSettingsPtr m_settings;
        wxString m_iconPath; /// Not serializable
        wxArrayString m_cachedIncludePaths;
        wxString m_workspaceFolder; // The folder in which this project is contained. Separated by "/"
        FilesMap_t m_filesTable;
        FoldersMap_t m_virtualFoldersTable;
        gsgsStringSet m_excludeFiles;
        gsgsStringSet emptySet;

    private:
        void DoUpdateProjectSettings();
        void DoBuildCacheFromXml();
        clProjectFile::Ptr_t FileFromXml(wxXmlNode* node, const wxString& vd);
        wxArrayString DoGetCompilerOptions(bool cxxOptions, bool clearCache = false, bool noDefines = true,
            bool noIncludePaths = true);
        wxArrayString DoGetUnPreProcessors(bool clearCache, const wxString& cmpOptions);

        clProjectFolder::Ptr_t GetRootFolder();

    public:
        /**
         * @brief return list of files that are excluded from *any* build configuration
         */
        inline const gsgsStringSet& GetExcludeFiles() const { return m_excludeFiles; }

        /**
         * @brief return the meta data about a file
         */
        clProjectFolder::Ptr_t GetFolder(const wxString& vdFullPath) const;

        /**
         * @brief return the meta data about a file
         */
        clProjectFile::Ptr_t GetFile(const wxString& fullpath) const;

        /**
         * @brief return the workspace associated with the project
         * If no workspace is associated, then the global workspace is returned
         */
        CxxWS* GetWorkspace();

        /**
         * @brief return the XML version
         */
        wxString GetVersion() const;

        /**
         * @brief return true if this project has children (virtual folders or files)
         */
        bool IsEmpty() const;

        /**
         * @brief return true if a virtual directory is empty
         */
        bool IsVirtualDirectoryEmpty(const wxString& vdFullPath) const;

        void GetFolders(const wxString& vdFullPath, wxArrayString& folders);

        /**
         * @brief get list of files under a given folder
         * @param parentFolder
         */
        void GetFiles(const wxString& vdFullPath, wxArrayString& files);

        /**
         * @brief clear the include path cache
         */
        void ClearIncludePathCache();

        /**
         * @brief a project was renamed - update our dependeices if needed
         */
        void ProjectRenamed(const wxString& oldname, const wxString& newname);
        void SetIconPath(const wxString& iconPath) { this->m_iconPath = iconPath; }
        const wxString& GetIconPath() const { return m_iconPath; }
        /**
         * @brief return set of compilers used by this project for the active build configuraion
         */
        void GetCompilers(gsgsStringSet& compilers);

        /**
         * @brief replace compilers by name. compilers contains a map of the "olbd" compiler
         * name and the new compiler name
         */
        void ReplaceCompilers(gsgsStringMap& compilers);

        /**
         * @brief the const version of the above
         */
        const CxxWS* GetWorkspace() const;
        inline const wxFileName& GetFileName() const { return m_fileName; }

        inline const wxString& GetProjectPath() const { return m_projectPath; }
        /**
         * \brief copy this project and all the files under to new_path
         * \param file_name the new path of the project
         * \param new_name the new project name
         * \param description the new project description
         */
        void CopyTo(const wxString& new_path, const wxString& new_name, const wxString& description);

        /**
         * \brief copy files (and virtual directories) from src project to this project
         * note that this call replaces the files that exists under this project
         * \param src
         */
        void SetFiles(ProjectPtr src);

        //--------------------------------------------------
        // Ctor - Dtor
        //--------------------------------------------------

        // default constructor
        Project();
        virtual ~Project();

        /**
         * @brief return list of macros used in the configuration which could not be resolved
         */
        void GetUnresolvedMacros(const wxString& configName, wxArrayString& vars) const;

        /**
         * \return project name
         */
        wxString GetName() const;

        /**
         * \brief return the project description as appears in the XML file
         * \return project description
         */
        wxString GetDescription() const;

        //-----------------------------------
        // Project operations
        //-----------------------------------
        /**
         * Load project from file
         * \param path
         * \return
         */
        bool Load(const wxString& path);
        /**
         * \brief Create new project
         * \param name project name
         * \param description project description
         * \param path path of the file excluding  the file name (e.g. C:\)
         * \param projType project type: PROJECT_TYPE_STATIC_LIBRARY, PROJECT_TYPE_DYNAMIC_LIBRARY, PROJECT_TYPE_EXECUTABLE
         * \return true on success, false otherwise
         */
        bool Create(const wxString& name, const wxString& description, const wxString& path, const wxString& projType);

        /**
         * Add file to the project
         * \param fileName file full name and path
         * \param virtualDir owner virtual directory, if the virtual directory does not exist, a new one will be created
         *        and the file will be placed under it
         * \return
         */
        bool AddFile(const wxString& fileName, const wxString& virtualDir = wxEmptyString);

        /**
         * Add file to the project - dont check for file duplication, this
         * \param fileName file full name and path
         * \param virtualDir owner virtual directory, if the virtual directory does not exist, a new one will be created
         *        and the file will be placed under it
         * \return true on success, false otherwise
         */
        bool FastAddFile(const wxString& fileName, const wxString& virtualDir = wxEmptyString);

        /**
         * Remove file from the project
         * \param fileName file full path
         * \param virtualDir owner virtual directory
         * \return
         */
        bool RemoveFile(const wxString& fileName, const wxString& virtualDir = wxEmptyString);

        /**
         * Rename file from the project
         * \param fileName file full path
         * \param virtualDir owner virtual directory
         * \return true on success, false otherwise
         */
        bool RenameFile(const wxString& oldName, const wxString& virtualDir, const wxString& newName);

        /**
         * \brief change the name of a virtual folder
         * \param oldVdPath full path of the virtual folder
         * \param newName the new name *only* of the virtual folder (without the path)
         * \return true on success, false otherwise
         */
        bool RenameVirtualDirectory(const wxString& oldVdPath, const wxString& newName);

        /**
         * Create new virtual directory
         * \param vdFullPath VD path to add
         * \return
         */
        bool CreateVirtualDir(const wxString& vdFullPath, bool mkpath = false);

        /**
         * remove a virtual directory
         * \param vdFullPath VD path to remove
         * \return
         */
        bool DeleteVirtualDir(const wxString& vdFullPath);

        /**
         * Return list of files by a virtual directory
         * \param vdFullPath virtual directory
         * \param files [output] list of files under this vdFullPath. The files format are in absolute path!
         */
        void GetFilesByVirtualDir(const wxString& vdFullPath, wxArrayString& files, bool recurse = false);

        /**
         * Save project settings
         */
        void Save();

        /**
         * @brief return the file meta data. The file names on the list
         * are in fullpath
         */
        inline const FilesMap_t& GetFiles() const { return m_filesTable; }
        inline FilesMap_t& GetFiles() { return m_filesTable; }

        /**
         * @brief return the files as vector
         */
        void GetFilesAsVector(clProjectFile::Vec_t& files) const;

        /**
         * @brief return the files as vector of wxFileName
         */
        void GetFilesAsVectorOfFileName(std::vector<wxFileName>& files, bool absPath = true) const;

        /**
         * @brief return list of files space separated
         */
        wxString GetFilesAsString(bool absPath) const;

        /**
         * @brief return list of files as wxArrayString
         */
        void GetFilesAsStringArray(wxArrayString& files, bool absPath = true) const;

        /**
         * Return a node pointing to any project-wide editor preferences
         */
        wxXmlNode* GetProjectEditorOptions() const;

        /**
         * Add or update local project options
         */
        void SetProjectEditorOptions(LocalOptionsConfigPtr opts);

        /**
         * Return the project build settings object by name
         */
        ProjectSettingsPtr GetSettings() const;

        /**
         * Add or update settings to the project
         */
        void SetSettings(ProjectSettingsPtr settings);

        /**
         * Update global settings to the project
         */
        void SetGlobalSettings(BuildConfigCommonPtr settings);

        /**
         * Get the project's file toplevel directory, extensions, ignorefiles, exclude paths and regexes for use when
         * reconciling with filesystem reality
         */
        void GetReconciliationData(wxString& toplevelDir, wxString& extensions, wxArrayString& ignoreFiles,
            wxArrayString& excludePaths, wxArrayString& regexes);

        /**
         * Set the project's file toplevel directory, extensions, ignorefiles, exclude paths and regexes for use when
         * reconciling with filesystem reality
         */
        void SetReconciliationData(const wxString& toplevelDir, const wxString& extensions,
            const wxArrayString& ignoreFiles, const wxArrayString& excludePaths,
            wxArrayString& regexes);
        //-----------------------------------
        // visual operations
        //-----------------------------------
        ProjectTreePtr AsTree();

        /**
         * \brief return the build order for a given configuration
         * \param configuration
         */
        wxArrayString GetDependencies(const wxString& configuration) const;

        /**
         * \brief set the dependencies for this project for a given configuration
         * \param deps
         * \param configuration
         */
        void SetDependencies(wxArrayString& deps, const wxString& configuration);

        /**
         * Return true if a file already exist under the project
         */
        bool IsFileExist(const wxString& fileName);
        /**
         * \brief See if filePath exists in the project as a symlink
         * \param filePath the filepath to match
         * \param fileNameInProject will be filled with any matching name
         * \return true if a matching name was found
         */
        bool IsFileExist(const wxString& filePath, wxString& fileNameInProject);

        /**
         * \brief return true of the project was modified (in terms of files removed/added)
         */
        bool IsModified();

        /**
         * \brief
         */
        void SetModified(bool mod);

        // Transaction support to reduce overhead of disk writing
        inline void BeginTranscation() { m_tranActive = true; }
        inline void CommitTranscation() { Save(); }
        inline bool InTransaction() const { return m_tranActive; }

        wxString GetVDByFileName(const wxString& file);

        /**
         * \brief return Tree representation of all virtual folders of this project
         * \return tree node. return NULL if no virtual folders exist
         */
        TreeNode<wxString, VisualWorkspaceNode>* GetVirtualDirectories(TreeNode<wxString, VisualWorkspaceNode>* workspace);

        /**
         * @brief return the user saved information for custom data
         * @param name the object key
         * @param obj [output] container for the output
         * @return true on success.
         */
        bool GetUserData(const wxString& name, ObjectData* obj);

        /**
         * @brief save user data in the project settings
         * @param name the name under which the data is to be saved
         * @param obj the data
         * @return true on success.
         */
        bool SetUserData(const wxString& name, ObjectData* obj);

        /**
         * @brief set the project internal type (usually used to indicate internal types for the project
         * like 'GUI' or 'UnitTest++' etc.
         * @param internalType
         */
        void SetProjectInternalType(const wxString& internalType);
        /**
         * @brief return the project internal type
         * @return
         */
        wxString GetProjectInternalType() const;

        /**
         * @brief return the project icon index (used by the NewProjectDialog)
         */
        wxString GetProjectIconName() const;
        /**
         * @brief return the plugins' data. This data is copied when using 'save project as template' functionality
         * @param plugin plugin name
         * @return plugins data or wxEmptyString
         */
        wxString GetPluginData(const wxString& pluginName);

        /**
         * @brief set the plugin data. This data is copied when using 'save project as template' functionality
         * @param plugin the plugins' name
         * @param data the data
         */
        void SetPluginData(const wxString& pluginName, const wxString& data, bool saveToXml = true);

        /**
         * @brief get all plugins data as map of plugin=value pair
         * @param pluginsDataMap [output]
         */
        void GetAllPluginsData(std::map<wxString, wxString>& pluginsDataMap);

        /**
         * @brief set all plugins data as map of plugin=value pair
         * @param pluginsDataMap
         */
        void SetAllPluginsData(const std::map<wxString, wxString>& pluginsDataMap, bool saveToFile = true);

        //----------------------------------
        // File modifications
        //----------------------------------

        /**
         * return the last modification time (on disk) of editor's underlying file
         */
        time_t GetFileLastModifiedTime() const;

        /**
         * return/set the last modification time that was made by the editor
         */
        inline time_t GetProjectLastModifiedTime() const { return m_modifyTime; }
        inline void SetProjectLastModifiedTime(time_t modificationTime) { m_modifyTime = modificationTime; }

        wxString GetBestPathForVD(const wxString& vdPath);

        /**
         * @brief return the project include paths (all backticks, $(shell ..) expanded)
         * The include paths are returned as an array in the order they appear in the
         * project settings
         */
        wxArrayString GetIncludePaths();

        /**
         * @brief return the pre-processors for this project.
         * The PreProcessors returned are from the build configuration
         * that matches the current workspace configuration
         */
        wxArrayString GetPreProcessors(bool clearCache = false);

        /**
         * @brief return the C++ Undefined Pre preprocessors
         * These are the defined by -U__SOMETHING__
         */
        wxArrayString GetCxxUnPreProcessors(bool clearCache = false);

        /**
         * @brief return the C Undefined Pre preprocessors
         * These are the defined by -U__SOMETHING__
         */
        wxArrayString GetCUnPreProcessors(bool clearCache = false);

        /**
         * @brief return the compiler. Optionally ommit the defines/include paths
         */
        wxArrayString GetCXXCompilerOptions(bool clearCache = false, bool noDefines = true, bool noIncludePaths = true);

        /**
         * @brief return the C compiler. Optionally ommit the defines/include paths
         */
        wxArrayString GetCCompilerOptions(bool clearCache = false, bool noDefines = true, bool noIncludePaths = true);

        /**
         * @brief return the compilation line for a C++ file in the project. This function returns the same
         * compilation line for all CXX or C files. So instead of hardcoding the file name it uses a placeholder for the
         * file
         * name which can later be replaced by the caller with the actual file name
         */
        wxString GetCompileLineForCXXFile(const gsgsStringMap& compilersGlobalPaths, BuildConfigPtr buildConf,
            const wxString& filenamePlaceholder = "$FileName", bool cxxFile = true);

        void ClearAllVirtDirs();

        /**
         * @brief sets the flags of a file
         * @param fileName the fullpath of the file
         * @param virtualDirPath virtual folder path (a:b:c)
         * @param flags the flags to set
         */
        void SetFileFlags(const wxString& fileName, const wxString& virtualDirPath, size_t flags);

        /**
         * @brief return the flags for a specific file in the project
         * @param fileName the fullpath of the file
         * @param virtualDirPath virtual folder path (a:b:c)
         * @return the virtual flags of a file or if the file does not exists, return 0
         */
        size_t GetFileFlags(const wxString& fileName, const wxString& virtualDirPath);

        /**
         * @brief return list of configurations for whom the current file is excluded from the build
         * @param fileName the fullpath of the file
         * @param virtualDirPath virtual folder path (a:b:c)
         */
        const gsgsStringSet& GetExcludeConfigForFile(const wxString& filename) const;

        /**
         * @brief set the exclude config list for a file
         * @param fileName the fullpath of the file
         * @param virtualDirPath virtual folder path (a:b:c)
         */
        void SetExcludeConfigsForFile(const wxString& filename, const gsgsStringSet& configs);

        /**
         * @brief add an exclude configuration for a file
         */
        void AddExcludeConfigForFile(const wxString& filename, const wxString& configName = "");

        /**
         * @brief remove exclude configuration for file
         */
        void RemoveExcludeConfigForFile(const wxString& filename, const wxString& configName = "");

        /**
         * @brief return true if 'filename' is excluded from a given configuration. If configName is empty string
         * use the current build configuration
         */
        bool IsFileExcludedFromConfig(const wxString& filename, const wxString& configName = "") const;

        /**
         * @brief add this project files into the 'compile_commands' json object
         */
        void CreateCompileCommandsJSON(JSONObject& compile_commands, const gsgsStringMap& compilersGlobalPaths);

        /**
         * @brief create compile_flags.txt file for this project
         * @param compilersGlobalPaths
         */
        void CreateCompileFlags(const gsgsStringMap& compilersGlobalPaths);

        void SetWorkspaceFolder(const wxString& workspaceFolders) { this->m_workspaceFolder = workspaceFolders; }
        const wxString& GetWorkspaceFolder() const { return m_workspaceFolder; }

        /**
         * @brief return the build configuration
         * @param configName configuration name. If non provided, returns the build configuration
         * that matches the current workspace configuration
         */
        BuildConfigPtr GetBuildConfiguration(const wxString& configName = "") const;

    private:
        /**
         * @brief associate this project with a workspace
         */
        void AssociateToWorkspace(CxxWS* workspace);

        wxString DoFormatVirtualFolderName(const wxXmlNode* node) const;

        void DoDeleteVDFromCache(const wxString& vd);
        wxArrayString DoBacktickToIncludePath(const wxString& backtick);
        wxArrayString DoBacktickToPreProcessors(const wxString& backtick);
        wxString DoExpandBacktick(const wxString& backtick);
        void DoGetVirtualDirectories(wxXmlNode* parent, TreeNode<wxString, VisualWorkspaceNode>* tree);

        // Recursive helper function
        void RecursiveAdd(wxXmlNode* xmlNode, ProjectTreePtr& ptp, ProjectTreeNode* nodeParent);

        // Return the node representing a virtual dir by name
        // if no such virtual dir exist, create it.
        wxXmlNode* GetVirtualDir(const wxString& vdFullPath);

        // Create virtual dir and return its xml node
        wxXmlNode* CreateVD(const wxString& vdFullPath, bool mkpath = false);

        /**
         * Return list of projects that this projects depends on
         */
        wxArrayString GetDependencies() const;

        /**
         * @brief convert all the files paths to Unix format
         * @param parent
         */
        void ConvertToUnixFormat(wxXmlNode* parent);

        bool SaveXmlFile();
    };

    class _gsgsAPI ProjectData
    {
    public:
        wxString m_name;           //< project name
        wxString m_path;           //< project directoy
        ProjectPtr m_srcProject;   //< source project
        wxString m_cmpType;        //< Project compiler type
        wxString m_debuggerType;   //< Selected debugger
        wxString m_sourceTemplate; //< The template selected by the user in the wizard
        wxString m_builderName;    //< The builder to use for this project

        ProjectData()
            : m_builderName("Default")
        {
        }
    };

    //-----------------------------------------------------------------
    // This class is related to the visual representation of the class
    // projects in the tree view
    //-----------------------------------------------------------------
    /**
     * Class FilewViewTreeItemData, a user defined class which stores a node private information
     *
     * \date 12-04-2007
     * \author Eran
     *
     */
    class _gsgsAPI FilewViewTreeItemData : public wxTreeItemData
    {
        ProjectItem m_item;

    public:
        FilewViewTreeItemData(const ProjectItem& item)
            : m_item(item)
        {
        }
        inline const ProjectItem& GetData() const { return m_item; }
        inline ProjectItem& GetData() { return m_item; }
        inline void SetDisplayName(const wxString& displayName) { m_item.SetDisplayName(displayName); }

        inline void SetFile(const wxString& file) { m_item.SetFile(file); }
    };
}
#endif // PROJECT_H
