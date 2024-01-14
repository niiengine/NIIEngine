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
#ifndef gsgs_EDITOR_CONFIG_H
#define gsgs_EDITOR_CONFIG_H

#include "gsgsPreInclude.h"
#include "gsgsLexerStyle.h"
#include "gsgsOptionsConfig.h"
#include "plugin.h"
#include "project.h"
#include "wx/thread.h"

namespace gsgs
{
    class EnvSetter;
    enum
    {
        View_Show_Workspace_Tab = 0x01,
        View_Show_Explorer_Tab = 0x02,
        View_Show_Tabs_Tab = 0x04,
        View_Show_Tabgroups_Tab = 0x08,
        View_Show_Service_Tab = 0x10,
        View_Show_Space_Tab = 0x20,
        View_Show_Material_Tab = 0x40,
        View_Show_Event_Tab = 0x80,
        View_Show_Property_Tab = 0x100,
        View_Show_Default = View_Show_Workspace_Tab | View_Show_Explorer_Tab |
        View_Show_Tabs_Tab | View_Show_Tabgroups_Tab | View_Show_Service_Tab |
        View_Show_Space_Tab | View_Show_Material_Tab | View_Show_Event_Tab |
        View_Show_Property_Tab
    };

    // Helper macro for getting the correct wxMBConv object from the user settings
#define clMBConv(converter)                                                                                  \
    wxCSConv __fontEncConv(gsgs_EditorConfig().GetOptions()->GetFileFontEncoding());                      \
    bool __useBuiltIn = (gsgs_EditorConfig().GetOptions()->GetFileFontEncoding() == wxFONTENCODING_UTF8); \
    wxMBConv* converter = (__useBuiltIn ? (wxMBConv*)&wxConvUTF8 : (wxMBConv*)&__fontEncConv);

/// A helper class that makes sure that escpaed $ signs
/// are kept unharmed
    class _gsgsAPI DollarEscaper
    {
    public:
        DollarEscaper(wxString & str);

        ~DollarEscaper();
    private:
        wxString & m_str;
    };

    class _gsgsAPI EnvMap
    {
    public:
        EnvMap();
        ~EnvMap();

        void Put(const wxString& key, const wxString& val);
        bool Get(const wxString& key, wxString& val);
        void Clear();
        size_t GetCount();
        bool Get(size_t index, wxString& key, wxString& val);
        bool Contains(const wxString& key);
        wxString String();
    private:
        wxArrayString m_keys;
        wxArrayString m_values;
    };

    class _gsgsAPI EvnVarList : public ObjectData
    {
    public:
        EvnVarList();
        virtual ~EvnVarList();

        virtual void DeSerialize(Serializer& arch);

        virtual void Serialize(Serializer& arch);

        inline void SetActiveSet(const wxString& activeSet)
        {
            if (activeSet != _("<Use Active Set>") && activeSet != USE_GLOBAL_SETTINGS)
                this->mCurrent = activeSet;
        }

        inline const wxString & GetActiveSet() const { return mCurrent; }

        inline void SetEnvVarSets(const gsgsStringMap & envVarSets) { mEnvList = envVarSets; }

        inline const gsgsStringMap & GetEnvVarSets() const { return mEnvList; }

        void AddVariable(const wxString & setName, const wxString & name, const wxString & value);

        /**
         * \brief inser variable into the set. If a variable is already exist with this name, replace it
         * \param setName
         * \param name
         * \param value
         */
        void InsertVariable(const wxString& setName, const wxString& name, const wxString& value);

        EnvMap GetVariables(const wxString& setName, bool includeWorkspaceEnvs, const wxString& projectName,
            const wxString& configName);
        bool IsSetExist(const wxString& setName);
    protected:
        wxString DoGetSetVariablesStr(const wxString& setName, wxString& selectedSetName);
    private:
        wxString mCurrent;
        gsgsStringMap mEnvList;
    };

    class _gsgsAPI SimpleLongValue : public ObjectData
    {
    public:
        SimpleLongValue();
        ~SimpleLongValue();

        void DeSerialize(Serializer& arch);
        void Serialize(Serializer& arch);

        inline void SetValue(const long& value)         { this->m_value = value; }
        inline const long& GetValue() const             { return m_value; }
    private:
        long m_value;
    };

    class _gsgsAPI SimpleStringValue : public ObjectData
    {
    public:
        SimpleStringValue();
        ~SimpleStringValue();

        void DeSerialize(Serializer& arch);
        void Serialize(Serializer& arch);

        inline void SetValue(const wxString& value)     { this->m_value = value; }
        inline const wxString& GetValue() const         { return m_value; }
    private:
        wxString m_value;
    };

    class _gsgsAPI SimpleRectValue : public ObjectData
    {
    public:
        SimpleRectValue();
        ~SimpleRectValue();

        void DeSerialize(Serializer& arch);
        void Serialize(Serializer& arch);

        inline void SetRect(const wxRect& rect)         { this->m_rect = rect; }
        inline const wxRect& GetRect() const            { return m_rect; }
    private:
        wxRect m_rect;
    };

    class _gsgsAPI EditorConfig : public ConfigTool
    {
        friend class EnvSetter;
    public:
        static EditorConfig & singleton();
        static void Release();

        void Begin();
        void Save();

        inline void Init(const wxString& revision, const wxString& version)
        {
            m_svnRevision = revision;
            m_version = version;
        }

        /**
         * Load the configuration file
         * \param fileName configuration file name
         * \return true on success false otherwise
         */
        bool Load();

        wxString ExpandVariables(const wxString& in, bool applyEnvironment);

        /**
         * @brief return a list of the environment variabels as defined by the user
         * in the global environment table + workspace + project (this function only return the names of the variable, not
         * its value)
         */
        wxArrayString GetActiveSetEnvNames(bool includeWorkspace = true, const wxString& project = wxEmptyString);

        /**
         * Find lexer configuration and return a pointer to a LexerStyle object
         * \param lexer lexer name (e.g. Cpp, Java, Default etc..)
         * \return LexerStyle::Ptr_t
         */
        LexerStyle::Ptr_t GetLexer(const wxString& lexer);

        /**
         * @brief return the proper lexer based on the file's extension
         * @param filename
         * @return the file's lexer or the "Text" lexer
         */
        LexerStyle::Ptr_t GetLexerForFile(const wxString& filename);

        /**
         * Get the outputview's foreground colour, which is global to a theme
         * \return the colour as a wxString
         */
        wxString GetCurrentOutputviewFgColour() const;

        /**
         * Get the outputview's background colour, which is global to a theme
         * \return the colour as a wxString
         */
        wxString GetCurrentOutputviewBgColour() const;

        /**
         * Test if this configuration is loaded properly
         * \return true of a file is loaded into the configuration manager false otherwise
         */
        bool IsOk() const                       { return m_doc->IsOk(); }

        /**
         * Read the editor options from the configuration file
         * and return them as an object
         */
        OptionsConfig * GetOptions() const;

        /**
         * Set options to the configuration file, override them if they does not exist
         */
        void SetOptions(OptionsConfig * opts);

        /**
         * Return the database that should be used by the editor
         * \return
         */
        wxString GetTagsDatabase() const;

        /**
         * Set tags database to be use by editor (in addition to the workspace one)
         * \param &dbName
         */
        void SetTagsDatabase(const wxString& dbName);

        /**
         * save lexers settings
         */
        void SaveLexers();

        /**
         * get an array of recently opened items e.g. workspaces
         * \param files  [output] a place holder for the output
         * \param nodename  the type of item to get
         */
        void GetRecentItems(wxArrayString& files, const wxString& nodeName);

        /**
         * Set an array of recently opened items e.g. workspaces
         * \param files  list of files to save
         * \param nodename  the type of item to set
         */
        void SetRecentItems(const wxArrayString& files, const wxString& nodeName);

        /**
         * \brief write an archived object to the xml configuration
         * \param name object name
         * \param arch the archived object container
         */
        virtual bool WriteObject(const wxString& name, ObjectData* obj, bool event = true);

        /**
         * \brief read an archived object from the configuration
         * \param name object to read
         * \param arch [output]
         */
        virtual bool ReadObject(const wxString& name, ObjectData* obj);

        /**
         * Return the configuration version
         */
        wxString GetRevision() const;

        /**
         * Set the current configuration revision
         */
        void SetRevision(const wxString& rev);
        void SetInstallDir(const wxString& instlDir);

        /**
         * \brief convinience methods to store a single long value
         * \param name variable name
         * \param value value to store
         */
        void SetInteger(const wxString& name, long value);

        /**
         * \brief convinience methods to retrieve a single long value stored using
         * the 'SaveLongValue()' method
         * \param name variable name
         * \param value value
         * \return return true on success, false otherwise
         */
        long GetInteger(const wxString& name, long defaultValue = wxNOT_FOUND);

        /**
         * \brief get string from the configuration identified by key
         * \param key key identifiying the string
         * \return wxEmptyString or the value
         */
        wxString GetString(const wxString& key, const wxString& defaultValue = "");

        /**
         * \brief
         * \param key
         * \param value
         */
        void SetString(const wxString& key, const wxString& value);

        /**
         * \brief should this pane remain open despite an editor click
         * \param caption string to identify the pane
         * \return true if the pane should stay open
         */
        bool GetPaneStickiness(const wxString& caption);

        /**
         * \brief sets whether this pane should remain open despite an editor click
         * \param caption string to identify the pane
         * \param stickiness true if the pane should stay open
         */
        void SetPaneStickiness(const wxString& caption, bool stickiness);

    protected:
        EditorConfig();
        virtual ~EditorConfig();
        wxXmlNode* GetLexerNode(const wxString& lexer);
        bool DoSave(const wxString & node = gsgsEmptyString) const;
        bool DoLoadDefaultSettings();

        wxString DoExpandVariables(const wxString& in);
        void ApplyEnv(gsgsStringMap* overrideMap, const wxString& project, const wxString& config);
        void UnApplyEnv();
    private:
        wxXmlDocument * m_doc;
        mutable OptionsConfig * mConfig;
        wxFileName m_fileName;
        wxCriticalSection m_cs;
        gsgsStringMap m_envSnapshot;
        int m_envApplied;
        wxString m_svnRevision;
        wxString m_version;
        wxString m_installDir;
        bool m_transcation;

        std::map<wxString, long> m_cacheLongValues;
        std::map<wxString, wxString> m_cacheStringValues;
        std::map<wxString, wxArrayString> m_cacheRecentItems;
    };

    class EnvSetter
    {
    public:
        EnvSetter(gsgsStringMap* om = NULL) :
            m_env(&EditorConfig::singleton()),
            m_restoreOldValue(false)
        {
            m_env->ApplyEnv(om, wxEmptyString, wxEmptyString);
        }

        EnvSetter(ProjectPtr proj) :
            m_env(&EditorConfig::singleton()),
            m_restoreOldValue(false)
        {
            wxString projname = proj->GetName();
            wxString buildConfName;
            BuildConfigPtr buildConf = proj->GetBuildConfiguration();
            if (buildConf)
            {
                buildConfName = buildConf->GetName();
            }
            m_env->ApplyEnv(NULL, projname, buildConfName);
        }

        EnvSetter(Project* proj) :
            m_env(&EditorConfig::singleton()),
            m_restoreOldValue(false)
        {
            wxString projname = proj->GetName();
            wxString buildConfName;
            BuildConfigPtr buildConf = proj->GetBuildConfiguration();
            if (buildConf)
            {
                buildConfName = buildConf->GetName();
            }
            m_env->ApplyEnv(NULL, projname, buildConfName);
        }

        EnvSetter(EditorConfig* conf, gsgsStringMap* om = NULL) :
            m_env(&EditorConfig::singleton()),
            m_restoreOldValue(false)
        {
            wxUnusedVar(conf);
            m_env->ApplyEnv(om, wxEmptyString, wxEmptyString);
        }

        EnvSetter(EditorConfig* conf, gsgsStringMap* om, const wxString& project, const wxString& buildConfig) :
            m_env(&EditorConfig::singleton()),
            m_restoreOldValue(false)
        {
            wxUnusedVar(conf);
            m_env->ApplyEnv(om, project, buildConfig);
        }

        EnvSetter(const wxString& var, const wxString& value) :
            m_env(NULL)
        {
            m_envName = var;
            m_restoreOldValue = ::wxGetEnv(m_envName, &m_oldEnvValue);
            ::wxSetEnv(m_envName, value);
        }

        ~EnvSetter()
        {
            if (m_env)
            {
                m_env->UnApplyEnv();
                m_env = NULL;
            }

            if (m_restoreOldValue)
            {
                // restore old env variable value
                ::wxSetEnv(m_envName, m_oldEnvValue);

            }
            else if (!m_envName.IsEmpty())
            {
                // we applied a single evn variable without old value
                // uset it
                ::wxUnsetEnv(m_envName);
            }
        }
    private:
        EditorConfig * m_env;
        wxString m_envName;
        wxString m_oldEnvValue;
        bool m_restoreOldValue;
    };
}
#define gsgs_EditorConfig() gsgs::EditorConfig::singleton()
#endif // LITEEDITOR_EDITOR_CONFIG_H
