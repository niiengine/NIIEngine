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
#ifndef gsgs_WORKSPACEMANAGER_H
#define gsgs_WORKSPACEMANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsFileUtil.h"
#include "gsgsSingleton.h"
#include "gsgsOptionsConfig.h"
#include "gsgsPluginCommon.h"
#include "IWorkspace.h"
#include "wx/xml/xml.h"

namespace gsgs
{
    class _gsgsAPI ConfigMappingEntry
    {
    public:
        ConfigMappingEntry(const wxString& project, const wxString& name)
            : m_project(project)
            , m_name(name)
        {
        }

        ConfigMappingEntry()
            : m_project(wxEmptyString)
            , m_name(wxEmptyString)
        {
        }

        virtual ~ConfigMappingEntry() {}
    public:
        wxString m_project;
        wxString m_name;
    };

    class _gsgsAPI WorkspaceConfiguration
    {
    public:
        typedef std::list<ConfigMappingEntry> ConfigMappingList;
    public:
        WorkspaceConfiguration();
        WorkspaceConfiguration(wxXmlNode* node);
        WorkspaceConfiguration(const wxString& name, bool selected);
        virtual ~WorkspaceConfiguration();
        wxXmlNode* ToXml() const;

        inline void SetSelected(bool selected)      {  m_isSelected = selected;  }
        inline bool IsSelected() const              {  return m_isSelected;  }
        inline void SetName(const wxString& name)   {  m_name = name; }
        inline const wxString& GetName() const      {  return m_name;  }
        inline void SetConfigMappingList(const ConfigMappingList& mapList)  {  m_mappingList = mapList;  }
        inline const ConfigMappingList& GetMapping() const                  {  return m_mappingList; }
        inline void SetEnvironmentVariables(const wxString& var)
        {
            m_environmentVariables = var;
        }
        inline const wxString& GetEnvironmentVariables() const 
        { 
            return m_environmentVariables; 
        }
        void RenameProject(const wxString& oldname, const wxString& newname);
    private:
        wxString m_name;
        ConfigMappingList m_mappingList;
        wxString m_environmentVariables;
        bool m_isSelected;
    };

    typedef SharedPtr<WorkspaceConfiguration> WorkspaceConfigurationPtr;

    class _gsgsAPI BuildMatrix
    {
    public:
        BuildMatrix(wxXmlNode* node);
        virtual ~BuildMatrix();

        wxXmlNode* ToXml() const;

        inline const std::list<WorkspaceConfigurationPtr>& GetConfigurations() const { return m_configurationList; }
        void RenameProject(const wxString& oldname, const wxString& newname);
        void RemoveConfiguration(const wxString& configName);
        void SetConfiguration(WorkspaceConfigurationPtr conf);
        wxString GetProjectSelectedConf(const wxString & configName, const wxString& project) const;
        wxString GetSelectedConfigurationName() const;
        int GetSelectedConfigurationIndex() const;
        void SetSelectedConfigurationName(const wxString & name);
        WorkspaceConfigurationPtr GetConfigurationByName(const wxString& name) const;
    protected:
        WorkspaceConfigurationPtr FindConfiguration(const wxString& name) const;
    private:
        std::list<WorkspaceConfigurationPtr> m_configurationList;
    };

    typedef SharedPtr<BuildMatrix> BuildMatrixPtr;
    
    // Denotes whether we're dealing with preferences at a global, workspace, project or (maybe one day) file level
    enum prefsLevel { pLevel_global, pLevel_workspace, pLevel_project, pLevel_file, pLevel_dunno };

    class LocalOptionsConfig;

    typedef SharedPtr<LocalOptionsConfig> LocalOptionsConfigPtr;

    template <typename T> class validVar
    {
    public:
        validVar()
            : valid(false)
        {
        }
        inline void Set(const T info)
        {
            datum = info;
            valid = true;
        }
        inline void Reset()             { valid = false; }
        inline T GetDatum() const       { return datum; }
        inline bool isValid() const     { return valid; }
    private:
        bool valid;
        T datum;
    };

    class _gsgsAPI LocalOptionsConfig
    {
    public:
        LocalOptionsConfig(); // Used for setting local values
        LocalOptionsConfig(OptionsConfig * opts,
                           wxXmlNode* node); // Used for merging local values into the already-found global ones
        LocalOptionsConfig(LocalOptionsConfigPtr opts,
                           wxXmlNode* node); // Used for storing local values in a previously-empty instance
        virtual ~LocalOptionsConfig(void) {}

        inline bool HideChangeMarkerMarginIsValid() const       { return m_localhideChangeMarkerMargin.isValid(); }
        inline bool DisplayFoldMarginIsValid() const            { return m_localdisplayFoldMargin.isValid(); }
        inline bool DisplayBookmarkMarginIsValid() const        { return m_localdisplayBookmarkMargin.isValid(); }
        inline bool HighlightCaretLineIsValid() const           { return m_localhighlightCaretLine.isValid(); }
        inline bool TrimLineIsValid() const                     { return m_localTrimLine.isValid(); }
        inline bool AppendLFIsValid() const                     { return m_localAppendLF.isValid(); }
        inline bool DisplayLineNumbersIsValid() const           { return m_localdisplayLineNumbers.isValid(); }
        inline bool ShowIndentationGuidelinesIsValid() const    { return m_localshowIndentationGuidelines.isValid(); }
        inline bool IndentUsesTabsIsValid() const               { return m_localindentUsesTabs.isValid(); }
        inline bool IndentWidthIsValid() const                  { return m_localindentWidth.isValid(); }
        inline bool TabWidthIsValid() const                     { return m_localtabWidth.isValid(); }
        inline bool FileFontEncodingIsValid() const             { return m_localfileFontEncoding.isValid(); }
        inline bool ShowWhitespacesIsValid() const              { return m_localshowWhitspaces.isValid(); }
        inline bool EolModeIsValid() const                      { return m_localeolMode.isValid(); }

        //-------------------------------------
        // Setters/Getters
        //-------------------------------------

        inline bool GetHideChangeMarkerMargin() const
        {
            if(m_localhideChangeMarkerMargin.isValid()) 
            { 
                return m_localhideChangeMarkerMargin.GetDatum(); 
            }
            return false; // It's invalid anyway, so false will do as well as anything
        }
        inline bool GetDisplayFoldMargin() const
        {
            if(m_localdisplayFoldMargin.isValid()) 
            { 
                return m_localdisplayFoldMargin.GetDatum(); 
            }
            return false;
        }
        inline bool GetDisplayBookmarkMargin() const
        {
            if(m_localdisplayBookmarkMargin.isValid()) 
            { 
                return m_localdisplayBookmarkMargin.GetDatum(); 
            }
            return false;
        }
        inline bool GetHighlightCaretLine() const
        {
            if(m_localhighlightCaretLine.isValid()) 
            { 
                return m_localhighlightCaretLine.GetDatum(); 
            }
            return false;
        }
        inline bool GetTrimLine() const
        {
            if(m_localTrimLine.isValid()) 
            { 
                return m_localTrimLine.GetDatum(); 
            }
            return false;
        }
        inline bool GetAppendLF() const
        {
            if(m_localAppendLF.isValid()) 
            { 
                return m_localAppendLF.GetDatum(); 
            }
            return false;
        }
        inline bool GetDisplayLineNumbers() const
        {
            if(m_localdisplayLineNumbers.isValid()) 
            { 
                return m_localdisplayLineNumbers.GetDatum(); 
            }
            return false;
        }
        inline bool GetShowIndentationGuidelines() const
        {
            if(m_localshowIndentationGuidelines.isValid()) 
            { 
                return m_localshowIndentationGuidelines.GetDatum(); 
            }
            return false;
        }

        inline void SetHideChangeMarkerMargin(bool hideChangeMarkerMargin)
        {
            m_localhideChangeMarkerMargin.Set(hideChangeMarkerMargin);
        }
        inline void SetDisplayFoldMargin(bool b)                    { m_localdisplayFoldMargin.Set(b); }
        inline void SetDisplayBookmarkMargin(bool b)                { m_localdisplayBookmarkMargin.Set(b); }
        inline void SetHighlightCaretLine(bool b)                   { m_localhighlightCaretLine.Set(b); }
        inline void SetTrimLine(bool b)                             { m_localTrimLine.Set(b); }
        inline void SetAppendLF(bool b)                             { m_localAppendLF.Set(b); }
        inline void SetDisplayLineNumbers(bool b)                   { m_localdisplayLineNumbers.Set(b); }
        inline void SetShowIndentationGuidelines(bool b)            { m_localshowIndentationGuidelines.Set(b); }
        
        inline void SetIndentUsesTabs(const bool& indentUsesTabs)   { m_localindentUsesTabs.Set(indentUsesTabs); }
        
        inline bool GetIndentUsesTabs() const
        {
            if(m_localindentUsesTabs.isValid())             
            { 
                return m_localindentUsesTabs.GetDatum(); 
            }
            return false;
        }
        
        inline void SetIndentWidth(const int& indentWidth)          { m_localindentWidth.Set(indentWidth); }
        inline int GetIndentWidth() const
        {
            if(m_localindentWidth.isValid()) 
            { 
                return m_localindentWidth.GetDatum(); 
            }
            return wxNOT_FOUND;
        }
        inline void SetTabWidth(const int& tabWidth)                { m_localtabWidth.Set(tabWidth); }
        inline int GetTabWidth() const
        {
            if(m_localtabWidth.isValid()) 
            { 
                return m_localtabWidth.GetDatum(); 
            }
            return wxNOT_FOUND;
        }

        inline wxFontEncoding GetFileFontEncoding() const
        {
            if(m_localfileFontEncoding.isValid()) 
            { 
                return m_localfileFontEncoding.GetDatum(); 
            }
            return (wxFontEncoding)(wxFONTENCODING_MAX + 1);
        }
        void SetFileFontEncoding(const wxString& strFileFontEncoding);

        inline void SetShowWhitespaces(int showWhitespaces)         { m_localshowWhitspaces.Set(showWhitespaces); }
        inline int GetShowWhitespaces() const
        {
            if(m_localshowWhitspaces.isValid()) 
            { 
                return m_localshowWhitspaces.GetDatum(); 
            }
            return wxNOT_FOUND;
        }

        inline void SetEolMode(const wxString & eolMode)            { m_localeolMode.Set(eolMode); }
        inline wxString GetEolMode() const
        {
            if(m_localeolMode.isValid()) 
            { 
                return m_localeolMode.GetDatum(); 
            }
            return wxT("");
        }

        /**
         * Return an XML representation of this object
         * \return XML node
         */
        wxXmlNode* ToXml(wxXmlNode* parent = NULL, const wxString& nodename = wxT("Options")) const;
    private:
        validVar<bool> m_localdisplayFoldMargin;
        validVar<bool> m_localdisplayBookmarkMargin;
        validVar<bool> m_localhighlightCaretLine;
        validVar<bool> m_localTrimLine;
        validVar<bool> m_localAppendLF;
        validVar<bool> m_localdisplayLineNumbers;
        validVar<bool> m_localshowIndentationGuidelines;
        validVar<bool> m_localindentUsesTabs;
        validVar<int> m_localindentWidth;
        validVar<int> m_localtabWidth;
        validVar<wxFontEncoding> m_localfileFontEncoding;
        validVar<int> m_localshowWhitspaces;
        validVar<wxString> m_localeolMode;
        validVar<bool> m_localhideChangeMarkerMargin;
    };

    class _gsgsAPI LocalWorkspace
    {
        friend class LocalWorkspaceST;
    public:
        enum CC_FLAGS {
            EnableCpp11 = 0x00000001,
            EnableCpp14 = 0x00000002,
            EnableCpp17 = 0x00000004,
            EnableSWTLW = 0x00000008 // Save Parse folders to Workspace file.
        };
    public:
        LocalWorkspace() {}
        virtual ~LocalWorkspace() {}

        /**
         * @brief return list of pinned c++ projects
         */
        size_t GetPinnedProjects(wxArrayString & projects);
        bool SetPinnedProjects(const wxArrayString & projects);

        /**
         * @brief flush the XML content to the disk
         */
        inline void Flush() { SaveXmlFile(); }

        /**
         * @brief Get any local editor preferences, merging the values into the global options
         * \param options the global options
         * \param projectname the name of the currently active project
         */
        void GetOptions(OptionsConfig * options, const wxString& projectname);
        /**
         * @brief Sets any local editor preferences for the current workspace
         * \param opts the local options to save
         */
        bool SetWorkspaceOptions(LocalOptionsConfigPtr opts);
        /**
         * @brief Sets any local editor preferences for the named project
         * \param opts the local options to save
         * \param projectname the name of the project
         */
        bool SetProjectOptions(LocalOptionsConfigPtr opts, const wxString& projectname);

        /**
         * @brief Set virtual folder colours for the workspace
         */
        bool SetFolderColours(const FolderColour::Map_t& vdColours);

        /**
         * @brief Get virtual folder colours for the workspace
         */
        bool GetFolderColours(FolderColour::Map_t& vdColours);

        /**
         * @brief Returns the node where any current local workspace options are stored
         */
        wxXmlNode* GetLocalWorkspaceOptionsNode() const;
        /**
         * @brief Returns the node where any current local project options are stored
         * \param projectname the name of the project
         */
        wxXmlNode* GetLocalProjectOptionsNode(const wxString& projectname) const;

        /**
         * @brief return the workspace C++ parser specific include + exclude paths
         * @param inclduePaths [output]
         * @param excludePaths [output]
         */
        void GetParserPaths(wxArrayString& inclduePaths, wxArrayString& excludePaths);
        void SetParserPaths(const wxArrayString& inclduePaths, const wxArrayString& excludePaths);

        size_t GetParserFlags();
        void SetParserFlags(size_t flags);

        void GetParserMacros(wxString& macros);
        void SetParserMacros(const wxString& macros);

        void SetSearchInFilesMask(const wxString& findInFileMask);
        void GetSearchInFilesMask(wxString& findInFileMask, const wxString& defaultValue = "");

        void SetCustomData(const wxString& name, const wxString& value);
        wxString GetCustomData(const wxString& name);

        /**
         * @brief set and get the active environment variables set name
         */
        void SetActiveEnvironmentSet(const wxString& setName);
        wxString GetActiveEnvironmentSet();
    protected:
        /*void SaveWorkspaceOptions(LocalOptionsConfigPtr opts);
        void SaveProjectOptions(LocalOptionsConfigPtr opts, const wxString& projectname);*/
        bool SanityCheck();
        bool Create();
        bool SaveXmlFile();
        wxFileName DoGetFilePath() const;
    private:
        wxXmlDocument m_doc;
        wxFileName m_fileName;
    };
    
    class _gsgsAPI WorkspaceManager : public wxEvtHandler
    {
    public:
        static WorkspaceManager & singleton();

        inline void SetWorkspace(IWorkspace* workspace) { m_workspace = workspace; }

        inline IWorkspace * GetWorkspace()              { return m_workspace; }
        
        bool IsOpen() const;
        
        inline bool IsValid() const                     { return m_workspace != NULL; }

        bool HasBacktick(const wxString & backtick) const;
        
        bool GetBacktickValue(const wxString & backtick, wxString & value) const;
        
        void SetBacktickValue(const wxString & backtick, const wxString & value);
        
        void ClearBacktickCache();
        
        void AddRecently(const wxString & fileName);
        
        void GetRecently(wxArrayString & files);
        
        void RemoveAllRecently();

        inline FileHistory & GetRecentlyData()          { return mRecentlyData; }
        
        wxArrayString GetAllWorkspaces() const;
        
        wxArrayString GetUnifiedFilesMask() const;

        /**
         * @brief register new workspace type
         * @param workspace
         */
        void RegisterWorkspace(IWorkspace * workspace);
    protected:
        WorkspaceManager();
        virtual ~WorkspaceManager();

        void OnWorkspaceClosed(wxCommandEvent & e);
    private:
        IWorkspace * m_workspace;
        IWorkspace::List_t m_workspaces;
        FileHistory mRecentlyData;
        gsgsStringMap m_backticks;
    };
}
#define gsgs_Workspace() gsgs::WorkspaceManager::singleton()
#endif
