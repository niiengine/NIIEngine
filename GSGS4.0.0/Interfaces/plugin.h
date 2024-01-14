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

#ifndef gsgs_PLUGIN_H
#define gsgs_PLUGIN_H

#ifdef __WXMSW__
#ifdef Yield
#undef Yield
#endif
#endif

#include "gsgsPreInclude.h"
#include "gsgsCommonHandler.h"
#include "plugin_version.h"
#include "gsgsJSON.h"
#include "gsgsConfig.h"
#include "imanager.h"
#include <wx/dynlib.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/app.h>
#include <wx/pen.h>

#ifdef _WIN32
#define STDCALL __stdcall
#define EXPORT __declspec(dllexport)
#else
#define STDCALL
#define EXPORT
#endif

#if defined(__WXMSW__) || defined(__WXGTK__)
#define CL_PLUGIN_API extern "C" EXPORT
#else
// OSX
#define CL_PLUGIN_API extern "C" EXPORT
#endif

namespace gsgs
{
    /**
     * Possible popup menu
     */
    enum MenuType {
        MenuTypeFileExplorer = 0,
        MenuTypeFileView_Workspace,
        MenuTypeFileView_Project,
        MenuTypeFileView_Folder,
        MenuTypeFileView_File,
        MenuTypeEditor
    };

    class _gsgsAPI PluginInfo
    {
    public:
        enum eFlags {
            kNone = 0,
            kDisabledByDefault = (1 << 0),
        };

        typedef std::map<wxString, PluginInfo> PluginMap_t;
    public:
        PluginInfo();
        ~PluginInfo();

        inline void SetAuthor(const wxString& author)           { m_author = author; }
        inline void SetDescription(const wxString& description) { m_description = description; }
        inline void SetName(const wxString& name)               { m_name = name; }
        inline void SetVersion(const wxString& version)         { m_version = version; }
        inline void SetLicense(const wxString& license)         { mLicense = license; }
        inline void SetDependence(const wxString & depend)      { mDependence = depend; }
        inline void EnableFlag(PluginInfo::eFlags flag, bool b)
        {
            if (b) {
                m_flags |= flag;
            }
            else {
                m_flags &= ~flag;
            }
        }
        inline bool HasFlag(PluginInfo::eFlags flag) const      { return m_flags & flag; }

        // Getters
        inline const wxString& GetAuthor() const                { return m_author; }
        inline const wxString& GetDescription() const           { return m_description; }
        inline const wxString& GetName() const                  { return m_name; }
        inline const wxString& GetVersion() const               { return m_version; }
        inline const wxString& GetLicense() const               { return mLicense; }
        inline const wxString& GetDependence() const            { return mDependence; }
        JSONObject ToJSON() const;
        void FromJSON(const gsgs::JSONObject & json);
    protected:
        wxString m_name;
        wxString m_author;
        wxString m_description;
        wxString m_version;
        wxString mLicense;
        wxString mDependence;
        size_t m_flags;
    };

    class _gsgsAPI PluginInfoArray : public ObjectData
    {
    public:
        PluginInfoArray();
        virtual ~PluginInfoArray();

        inline void SetPlugins(const PluginInfo::PluginMap_t& plugins)  { this->m_plugins = plugins; }
        inline const PluginInfo::PluginMap_t& GetPlugins() const        { return m_plugins; }
        void AddPlugin(const PluginInfo& plugin);
        bool CanLoad(const PluginInfo& plugin) const;
        void DisablePugins(const wxArrayString& plugins);
        void DisablePlugin(const wxString& plugin);
        inline const wxArrayString& GetDisabledPlugins() const          { return m_disabledPlugins; }

        virtual void FromJSON(const JSONObject & json);
        virtual JSONObject ToJSON() const;
    private:
        PluginInfo::PluginMap_t m_plugins;
        wxArrayString m_disabledPlugins;
    };

    class _gsgsAPI DynModule
    {
    public:
        DynModule();
        ~DynModule();

        bool Load(const wxString &name);
        void Detach();
        void* GetSymbol(const wxString &name, bool *success);

        inline const wxString & GetError() { return m_error; }
    private:
#if defined (__WXMSW__)
        wxDynamicLibrary m_lib;
#else //Mac OSX
        void *m_dllhandle;
#endif
        wxString m_error;
    };

    /**
    */
    class _gsgsAPI Plugin : public wxEvtHandler
    {
    public:
        Plugin(IManager* manager)
            : m_mgr(manager)
        {
        }
        virtual ~Plugin() {}

        //-----------------------------------------------
        // The interface
        //-----------------------------------------------
        /**
         * @brief return the plugin's short name
         * @return
         */
        virtual const wxString& GetShortName() const;
        /**
         * @brief return the plugin's long and more descriptive name
         * @return
         */
        virtual const wxString& GetLongName() const;

        /**
         * When gsgs loads all the plugins, this function is called.
         * \param toolbar
         */
        virtual void CreateToolBar(gsgs::ToolBar* toolbar) = 0;

        /**
         * Every plugin can place a sub menu in the 'Plugins' Menu at the menu bar
         * \param pluginsMenu
         */
        virtual void CreatePluginMenu(wxMenu* pluginsMenu) = 0;

        /**
         * \brief Call the plugin "shutdown" function
         */
        virtual void UnPlug() = 0;

        /**
         * Override this method to allow the plugin to
         * hook the popup menu by adding its entries.
         * \param menu menu to hook
         * \param type menu type
         * \sa  MenuType
         */
        virtual void HookPopupMenu(wxMenu* menu, MenuType type)
        {
            wxUnusedVar(type);
            wxUnusedVar(menu);
        };

        /**
         * @brief load image file from /path/to/install/plugins/resources/
         * @param name file name (name+extension)
         * @return Bitmap of wxNullBitmap if no match was found
         */
        virtual wxBitmap LoadBitmapFile(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_PNG)
        {
            wxBitmap bmp;
#ifdef __WXGTK__
            // /usr/share/codelite
            wxString pluginsDir = PathManager::Get().GetDataDir();
#else
#ifdef USE_POSIX_LAYOUT
            wxString pluginsDir(PathManager::Get().GetDataDir());
#else
            wxString pluginsDir(m_mgr->GetInstallDirectory() + wxT("/plugins"));
#endif
#endif
            wxString basePath(pluginsDir + wxT("/resources/"));

            bmp.LoadFile(basePath + name, type);
            if (bmp.IsOk()) { return bmp; }
            return wxNullBitmap;
        }

        /**
         * @brief allow the plugins to hook a tab in the project settings
         * @param notebook the parent
         * @param configName the associated configuration name
         */
        virtual void HookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName, const wxString& configName)
        {
            wxUnusedVar(notebook);
            wxUnusedVar(projectName);
            wxUnusedVar(configName);
        }

        /**
         * @brief Unhook any tab from the project settings dialog
         * @param notebook the parent
         * @param configName the associated configuration name
         */
        virtual void UnHookProjectSettingsTab(wxBookCtrlBase* notebook, const wxString& projectName, const wxString& configName)
        {
            wxUnusedVar(notebook);
            wxUnusedVar(projectName);
            wxUnusedVar(configName);
        }
    protected:
        wxString m_shortName;
        wxString m_longName;
        IManager* m_mgr;
    };

    _gsgsAPI extern const wxFont gsgsFont;
    _gsgsAPI extern const wxFont gsgsTitleFont;
    _gsgsAPI extern const wxFont gsgsBigTitleFont;

#define CHECK_CL_SHUTDOWN() { if(m_mgr->IsShutdownInProgress()) return;}

    // Every dll must contain at least this function
    typedef Plugin* (*GET_PLUGIN_CREATE_FUNC)(IManager*);
    typedef PluginInfo* (*GET_PLUGIN_INFO_FUNC)();
    typedef int(*GET_PLUGIN_INTERFACE_VERSION_FUNC)();
}
#endif // PLUGIN_H
