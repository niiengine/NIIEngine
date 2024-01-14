
#include "app.h"
#include "gsgsPluginManager.h"
#include "gsgsImageManager.h"
#include "gsgsBuilderGnuMake.h"
#include "gsgsWindowManager.h"
#include "gsgsToolBar.h"
#include "gsgsTagManager.h"
#include "debugger.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsWorkspacePane.h"
#include "gsgsFileView.h"
#include "gsgsFrame.h"
#include "gsgsJob.h"
#include "gsgsMacroManager.h"
#include "gsgsManager.h"
#include "gsgsBuildTab.h"
#include "gsgsOptionsConfig.h"
#include "gsgsSessionManager.h"
#include "wx/xrc/xmlres.h"
#include <wx/toolbook.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    PluginManager* PluginManager::Get()
    {
        static PluginManager mag;
        ::SetManager(&mag);
        return &mag;
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::UnLoad()
    {
        // Before we unload the plugins, store the list of visible workspace tabs
        {
            wxArrayString temp;
            for(size_t i = 0; i < GetWorkspacePaneNotebook()->GetPageCount(); ++i) 
            {
                temp.Add(GetWorkspacePaneNotebook()->GetPageText(i));
            }
            gsgs_Config().Write("VisibleWorkspaceTabs", temp);
        }

        // Now do the same for the output view
        {
            wxArrayString temp;
            for(size_t i = 0; i < GetOutputPaneNotebook()->GetPageCount(); ++i) 
            {
                temp.Add(GetOutputPaneNotebook()->GetPageText(i));
            }
            gsgs_Config().Write("VisibleOutputTabs", temp);
        }

        {
            wxArrayString temp;
            for (size_t i = 0; i < GetPerspectivePaneNotebook()->GetPageCount(); ++i) 
            {
                temp.Add(GetPerspectivePaneNotebook()->GetPageText(i));
            }
            gsgs_Config().Write("VisiblePerspectiveTabs", temp);
        }

        std::map<wxString, Plugin*>::iterator plugIter = m_plugins.begin();
        for(; plugIter != m_plugins.end(); plugIter++) 
        {
            Plugin* plugin = plugIter->second;
            plugin->UnPlug();
            delete plugin;
        }

        m_dl.clear();
        m_plugins.clear();
    }
    //-----------------------------------------------------------------------------------
    PluginManager::~PluginManager() 
    {
    }
    //-----------------------------------------------------------------------------------
    PluginManager::PluginManager()
        //: m_bmpLoader(NULL)
    {
        m_menusToBeHooked.insert(MenuTypeFileExplorer);
        m_menusToBeHooked.insert(MenuTypeFileView_Workspace);
        m_menusToBeHooked.insert(MenuTypeFileView_Project);
        m_menusToBeHooked.insert(MenuTypeFileView_Folder);
        m_menusToBeHooked.insert(MenuTypeFileView_File);
        m_menusToBeHooked.insert(MenuTypeEditor);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::Load()
    {
        wxString ext;
    #if defined(__WXGTK__)
        ext = wxT("so");

    #elif defined(__WXMAC__)
        ext = wxT("dylib");

    #else
        ext = wxT("dll");
    #endif

        wxString fileSpec(wxT("*.") + ext);
        Config conf("plugins.conf");

        conf.ReadItem(&m_pluginsData);

        // set the managers
        // this code assures us that the shared objects will see the same instances as the application
        // does
        LanguageST::Get()->SetTagsManager(GetTagsManager());
        TagsManagerST::Get()->SetLanguage(LanguageST::Get());

        // Plugin loading policy
        GsGsApp* app = static_cast<GsGsApp*>(GetTheApp());
        GsGsApp::PluginPolicy pp = app->GetPluginLoadPolicy();
        wxArrayString allowedPlugins;
        if(pp == GsGsApp::PP_None) 
        {
            return;
        }

        else if(pp == GsGsApp::PP_FromList)
            allowedPlugins = app->GetAllowedPlugins();

        wxString pluginsDir = gsgs_Path().GetPluginsDirectory();
        if(wxDir::Exists(pluginsDir)) 
        {
            // get list of dlls
            wxArrayString files;
            wxDir::GetAllFiles(pluginsDir, &files, fileSpec, wxDIR_FILES);

            // Sort the plugins by A-Z
            std::sort(files.begin(), files.end());
            for(size_t i = 0; i < files.GetCount(); i++) 
            {
                wxString fileName(files.Item(i));
    #if defined(__WXMSW__) && CL_DEBUG_BUILD

                // Under MSW loading a release plugin while in debug mode will cause a crash
                if(!fileName.EndsWith("-dbg.dll")) { continue; }
    #elif defined(__WXMSW__)

                // filter debug plugins
                if(fileName.EndsWith("-dbg.dll")) { continue; }
    #endif

    #ifdef __WXGTK__
                wxFileName fnDLL(fileName);
                if(fnDLL.GetFullName().StartsWith("lib")) {
                    // don't attempt to load a library
                    continue;
                }
    #endif

                DynModule* dl = new DynModule();
                if(!dl->Load(fileName)) {
                    gsgs_Error(wxT("Failed to load plugin's dll: ") + fileName);
                    if(!dl->GetError().IsEmpty()) { gsgs_Error(dl->GetError()); }
                    wxDELETE(dl);
                    continue;
                }

                bool success(false);
                GET_PLUGIN_INFO_FUNC pfnGetPluginInfo = (GET_PLUGIN_INFO_FUNC)dl->GetSymbol(wxT("GetPluginInfo"), &success);
                if(!success) {
                    wxDELETE(dl);
                    continue;
                }

                // load the plugin version method
                // if the methods does not exist, handle it as if it has value of 100 (lowest version API)
                int interface_version(100);
                GET_PLUGIN_INTERFACE_VERSION_FUNC pfnInterfaceVersion =
                    (GET_PLUGIN_INTERFACE_VERSION_FUNC)dl->GetSymbol(wxT("GetPluginInterfaceVersion"), &success);
                if(success) {
                    interface_version = pfnInterfaceVersion();
                } else {
                    gsgs_Warning(wxT("Failed to find GetPluginInterfaceVersion() in dll: ") + fileName);
                    if(!dl->GetError().IsEmpty()) { gsgs_Warning(dl->GetError()); }
                }

                if(interface_version != PLUGIN_INTERFACE_VERSION) {
                    gsgs_Warning(wxString::Format(wxT("Version interface mismatch error for plugin '%s'. Plugin's interface "
                                                    "version is '%d', gsgs interface version is '%d'"),
                                                fileName.c_str(), interface_version, PLUGIN_INTERFACE_VERSION));
                    wxDELETE(dl);
                    continue;
                }

                // Check if this dll can be loaded
                PluginInfo* pluginInfo = pfnGetPluginInfo();

                wxString pname = pluginInfo->GetName();
                pname.MakeLower().Trim().Trim(false);

                // Check the policy
                if(pp == GsGsApp::PP_FromList && allowedPlugins.Index(pname) == wxNOT_FOUND) {
                    // Policy is set to 'from list' and this plugin does not match any plugins from
                    // the list, don't allow it to be loaded
                    wxDELETE(dl);
                    continue;
                }

                // If the plugin does not exist in the m_pluginsData, assume its the first time we see it
                bool firstTimeLoading = (m_pluginsData.GetPlugins().count(pluginInfo->GetName()) == 0);

                // Add the plugin information
                m_pluginsData.AddPlugin((*pluginInfo));

                if(firstTimeLoading && pluginInfo->HasFlag(PluginInfo::kDisabledByDefault)) {
                    m_pluginsData.DisablePlugin(pluginInfo->GetName());
                    wxDELETE(dl);
                    continue;
                }

                // Can we load it?
                if(!m_pluginsData.CanLoad(*pluginInfo)) {
                    gsgs_Warning(wxT("Plugin ") + pluginInfo->GetName() + wxT(" is not enabled"));
                    wxDELETE(dl);
                    continue;
                }

                // try and load the plugin
                GET_PLUGIN_CREATE_FUNC pfn = (GET_PLUGIN_CREATE_FUNC)dl->GetSymbol(wxT("CreatePlugin"), &success);
                if(!success) {
                    gsgs_Warning(wxT("Failed to find CreatePlugin() in dll: ") + fileName);
                    if(!dl->GetError().IsEmpty()) { gsgs_Warning(dl->GetError()); }

                    m_pluginsData.DisablePlugin(pluginInfo->GetName());
                    continue;
                }

                // Construct the plugin
                Plugin* plugin = pfn((IManager*)this);
                gsgs_Debug(wxT("Loaded plugin: ") + plugin->GetLongName());
                m_plugins[plugin->GetShortName()] = plugin;

                // Load the toolbar
                plugin->CreateToolBar(GetToolBar());

                // Keep the dynamic load library
                m_dl.push_back(dl);
            }
            gsgs_Frame()->GetAuiManager()->Update();
            GetToolBar()->Realize();
            // Let the plugins plug their menu in the 'Plugins' menu at the menu bar
            // the create menu will be placed as a sub menu of the 'Plugin' menu
            wxMenu* pluginsMenu = NULL;
            wxMenuItem* menuitem = gsgs_Frame()->GetMenuBar()->FindItem(XRCID("manage_plugins"), &pluginsMenu);
            if(pluginsMenu && menuitem) {
                std::map<wxString, Plugin*>::iterator iter = m_plugins.begin();
                for(; iter != m_plugins.end(); ++iter) {
                    Plugin* plugin = iter->second;
                    plugin->CreatePluginMenu(pluginsMenu);
                }
            }

            // save the plugins data
            conf.WriteItem(&m_pluginsData);
        }

        // Now that all the plugins are loaded, load from the configuration file
        // list of visible tabs
        static wxArrayString DefaultArray;
        if(DefaultArray.IsEmpty()) { DefaultArray.Add("NOT-FOUND"); }

        DetachedPanesInfo dpi;
        GetConfigTool()->ReadObject(wxT("DetachedPanesList"), &dpi);
        const wxArrayString& detachedPanes = dpi.GetPanes();

        {
            // Hide workspace tabs
            const wxArrayString& tabs = GetWorkspaceTabs();
            wxArrayString visibleTabs;
            gsgs_Config().Read("VisibleWorkspaceTabs", visibleTabs, DefaultArray);
            if(!((visibleTabs.size() == 1) && (visibleTabs.Item(0) == "NOT-FOUND"))) {
                for(size_t i = 0; i < tabs.size(); ++i) {
                    if((visibleTabs.Index(tabs.Item(i)) == wxNOT_FOUND) &&
                       (detachedPanes.Index(tabs.Item(i)) == wxNOT_FOUND)) {
                        // hidden tab - post an event
                        Event eventHide(wxEVT_SHOW_WORKSPACE_TAB);
                        eventHide.SetSelected(false).SetString(tabs.Item(i));
                        gsgs_Event().AddPendingEvent(eventHide);
                    }
                }
            }
        }

        {
            // Hide output tabs
            const wxArrayString& tabs = GetOutputTabs();
            wxArrayString visibleTabs;
            gsgs_Config().Read("VisibleOutputTabs", visibleTabs, DefaultArray);
            if(!((visibleTabs.size() == 1) && (visibleTabs.Item(0) == "NOT-FOUND"))) {
                for(size_t i = 0; i < tabs.size(); ++i) {
                    if((visibleTabs.Index(tabs.Item(i)) == wxNOT_FOUND) &&
                       (detachedPanes.Index(tabs.Item(i)) == wxNOT_FOUND)) {
                        // hidden tab - post an event
                        Event eventHide(wxEVT_SHOW_OUTPUT_TAB);
                        eventHide.SetSelected(false).SetString(tabs.Item(i));
                        gsgs_Event().AddPendingEvent(eventHide);
                    }
                }
            }
        }
        {
            // Hide output tabs
            const wxArrayString& tabs = GetPerspectiveTabs();
            wxArrayString visibleTabs;
            gsgs_Config().Read("VisiblePerspectiveTabs", visibleTabs, DefaultArray);
            if (!((visibleTabs.size() == 1) && (visibleTabs.Item(0) == "NOT-FOUND"))) {
                for (size_t i = 0; i < tabs.size(); ++i) {
                    if ((visibleTabs.Index(tabs.Item(i)) == wxNOT_FOUND) &&
                        (detachedPanes.Index(tabs.Item(i)) == wxNOT_FOUND)) 
                    {
                        // hidden tab - post an event
                        Event eventHide(wxEVT_SHOW_Perspective_TAB);
                        eventHide.SetSelected(false).SetString(tabs.Item(i));
                        gsgs_Event().AddPendingEvent(eventHide);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    IEditor* PluginManager::GetActiveEditor()
    {
        if(gsgs_Frame() && gsgs_Frame()->GetMainBook()) 
        {
            Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor(true);
            if(!editor) { return nullptr; }
            return dynamic_cast<IEditor*>(editor);
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    ConfigTool* PluginManager::GetConfigTool() 
    { 
        return &gsgs_EditorConfig(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::HookPopupMenu(wxMenu* menu, MenuType type)
    {
        std::map<wxString, Plugin*>::iterator iter = m_plugins.begin();
        for(; iter != m_plugins.end(); iter++) 
        {
            iter->second->HookPopupMenu(menu, type);
        }
    }
    //-----------------------------------------------------------------------------------
    TreeItemInfo PluginManager::GetSelectedTreeItemInfo(TreeType type)
    {
        TreeItemInfo info;
        switch(type) 
        {
        case TreeFileExplorer:
            return gsgs_Frame()->GetFileExplorer()->GetItemInfo();
        case TreeFileView:
            return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->GetSelectedItemInfo();
        default:
            return info;
        }
    }
    //-----------------------------------------------------------------------------------
    TreeCtrl* PluginManager::GetWorkspaceTree() 
    { 
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView(); 
    }
    //-----------------------------------------------------------------------------------
    TreeCtrl* PluginManager::GetFileExplorerTree() 
    { 
        return gsgs_Frame()->GetFileExplorer()->GetTree(); 
    }
    //-----------------------------------------------------------------------------------
    Notebook* PluginManager::GetOutputPaneNotebook() 
    { 
        return gsgs_Frame()->GetOutputPane()->GetNotebook(); 
    }
    //-----------------------------------------------------------------------------------
    Notebook* PluginManager::GetWorkspacePaneNotebook() 
    { 
        return gsgs_Frame()->GetWorkspacePane()->GetNotebook(); 
    }
    //-----------------------------------------------------------------------------------
    Notebook* PluginManager::GetPerspectivePaneNotebook() 
    { 
        return gsgs_Frame()->GetPerspectivePane()->GetNotebook(); 
    }
    //-----------------------------------------------------------------------------------
    IEditor* PluginManager::OpenFile(const wxString& fileName, const wxBitmap& bmp, const wxString& tooltip)
    {
        IEditor* editor = gsgs_Frame()->GetMainBook()->OpenFile(fileName, bmp, tooltip);
        if(editor) { editor->SetActive(); }
        return editor;
    }
    //-----------------------------------------------------------------------------------
    IEditor* PluginManager::OpenFile(const wxString& fileName, const wxString& projectName, int lineno, OF_extra flags)
    {
        IEditor* editor = gsgs_Frame()->GetMainBook()->OpenFile(fileName, projectName, lineno, wxNOT_FOUND, flags);
        if(editor) 
        { 
            editor->SetActive(); 
        }
        return editor;
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetStartupDirectory() const 
    { 
        return gsgs_Path().GetStartupDirectory(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::AddProject(const wxString& path) 
    { 
        gsgs_Manager()->AddProject(path); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::IsOpen() const 
    { 
        return gsgs_Workspace().IsOpen(); 
    }
    //-----------------------------------------------------------------------------------
    TagsManager* PluginManager::GetTagsManager() 
    { 
        return TagsManagerST::Get(); 
    }
    //-----------------------------------------------------------------------------------
    CxxWS* PluginManager::GetWorkspace() 
    { 
        return &gsgs_CppWorker(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths)
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolder(item, paths);
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths)
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolder(vdFullPath, paths);
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths)
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->AddFilesToVirtualFolderIntelligently(vdFullPath, paths);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files)
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->RedefineProjFiles(proj, path, files);
    }
    //-----------------------------------------------------------------------------------
    int PluginManager::GetToolbarIconSize()
    {
        // for now return 24 by default
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        if(options) 
        { 
            return options->GetIconsSize(); 
        }
        return 24;
    }
    //-----------------------------------------------------------------------------------
    wxAuiManager* PluginManager::GetAuiManager() 
    { 
        return m_dockingManager; 
    }
    //-----------------------------------------------------------------------------------
    EditorConfig* PluginManager::GetEnv() 
    { 
        return &gsgs_EditorConfig(); 
    }
    //-----------------------------------------------------------------------------------
    JobQueue* PluginManager::GetJobQueue() 
    { 
        return JobQueueSingleton::Instance(); 
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetProjectExecutionCommand(const wxString& projectName, wxString& wd)
    {
        return gsgs_Manager()->GetProjectExecutionCommand(projectName, wd, false);
    }
    //-----------------------------------------------------------------------------------
    wxApp* PluginManager::GetTheApp() 
    { 
        return wxTheApp; 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ReloadWorkspace()
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(evt);
    }
    //-----------------------------------------------------------------------------------
    Plugin* PluginManager::GetPlugin(const wxString& pluginName)
    {
        std::map<wxString, Plugin*>::iterator iter = m_plugins.find(pluginName);
        if(iter != m_plugins.end()) { return iter->second; }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    wxEvtHandler* PluginManager::GetOutputWindow() 
    { 
        return gsgs_Frame()->GetOutputPane()->GetOutputWindow(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::SaveAll() 
    { 
        return gsgs_Frame()->GetMainBook()->SaveAll(true, false); 
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetInstallDirectory() const 
    { 
        return gsgs_Path().GetInstallDir(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->CreateVirtualDirectory(parentPath, vdName);
    }
    //-----------------------------------------------------------------------------------
    OptionsConfig * PluginManager::GetEditorSettings()
    {
        // First try to use Editor::GetOptions, as it takes account of local preferences
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if(editor) { return editor->GetOptions(); }
        // Failing that...
        return gsgs_EditorConfig().GetOptions();
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::FindAndSelect(const wxString& pattern, const wxString& name, int pos)
    {
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if(editor) {
            editor->FindAndSelectV(pattern, name, pos, NavMgr::Get());
            editor->SetActive();
        }
    }
    //-----------------------------------------------------------------------------------
    TagEntryPtr PluginManager::GetTagAtCaret(bool scoped, bool impl)
    {
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if(!editor) return NULL;
        return editor->GetContext()->GetTagAtCaret(scoped, impl);
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::AllowToolbar()
    {
        long v = gsgs_EditorConfig().GetInteger("UseSingleToolbar", 1);
        return (v == 1 ? false : true);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::EnableToolbars()
    {
        // In case, plugins are now allowed to insert toolbars, disable the toolbars_menu item
        if(AllowToolbar() == false) 
        {
            int ii = gsgs_Frame()->GetMenuBar()->FindMenu(wxT("View"));
            if(ii != wxNOT_FOUND) {
                wxMenu* viewMenu = gsgs_Frame()->GetMenuBar()->GetMenu(ii);
                wxMenuItem* item = viewMenu->FindItem(XRCID("toolbars_menu"));
                if(item) { item->Enable(false); }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::SetStatusMessage(const wxString& msg, int seconds_to_live)
    {
        if(GetStatusBar()) { GetStatusBar()->SetMessage(msg, seconds_to_live); }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ProcessCommandQueue() 
    { 
        gsgs_Manager()->ProcessCommandQueue(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::PushQueueCommand(const QueueCommand& cmd) 
    { 
        gsgs_Manager()->PushQueueCommand(cmd); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::StopAndClearQueue() 
    { 
        gsgs_Manager()->StopBuild(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::IsBuildInProgress() const 
    { 
        return gsgs_Manager()->IsBuildInProgress(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::IsBuildEndedSuccessfully() const 
    { 
        return gsgs_Manager()->IsBuildEndedSuccessfully(); 
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetProjectNameByFile(wxString& fullPathFileName)
    {
        return gsgs_Manager()->GetProjectNameByFile(fullPathFileName);
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetProjectNameByFile(const wxString& file)
    {
        return gsgs_Manager()->GetProjectNameByFile(file);
    }
    //-----------------------------------------------------------------------------------
    BuildManager* PluginManager::GetBuildManager() 
    { 
        return &gsgs_Build(); 
    }
    //-----------------------------------------------------------------------------------
    BuildSettingsConfig* PluginManager::GetBuildSettingsConfigManager() 
    { 
        return &gsgs_Settings(); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::ClosePage(const wxString& title) 
    { 
        return gsgs_Frame()->GetMainBook()->ClosePage(title); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::ClosePage(const wxFileName& filename)
    {
        Book* book = gsgs_Frame()->GetMainBook();
        Editor* editor = book->FindEditor(filename.GetFullPath());
        return gsgs_Frame()->GetMainBook()->ClosePage(editor->GetParent());
    }
    //-----------------------------------------------------------------------------------
    wxWindow* PluginManager::FindPage(const wxString& text) 
    { 
        return gsgs_Frame()->GetMainBook()->FindPage(text); 
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::AddPage(wxWindow* win, const wxString& text, const wxString& tooltip, const wxBitmap& bmp,
                                bool selected)
    {
        return gsgs_Frame()->GetMainBook()->AddPage(win, text, tooltip, bmp, selected);
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::SelectPage(wxWindow* win) { return gsgs_Frame()->GetMainBook()->SelectPage(win); }
    //-----------------------------------------------------------------------------------
    IEditor* PluginManager::OpenFile(const BrowseRecord& rec) { return gsgs_Frame()->GetMainBook()->OpenFile(rec); }
    //-----------------------------------------------------------------------------------
    NavMgr* PluginManager::GetNavigationMgr() { return NavMgr::Get(); }
    //-----------------------------------------------------------------------------------
    void PluginManager::HookProjectSettingsTab(wxBookCtrlBase * book, const wxString & projectName, const wxString & configName)
    {
        std::map<wxString, Plugin*>::iterator iter = m_plugins.begin();
        for(; iter != m_plugins.end(); iter++) 
        {
            iter->second->HookProjectSettingsTab(book, projectName, configName);
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::UnHookProjectSettingsTab(wxBookCtrlBase * book, const wxString & projectName, const wxString & configName)
    {
        std::map<wxString, Plugin*>::iterator iter = m_plugins.begin();
        for(; iter != m_plugins.end(); iter++) 
        {
            iter->second->UnHookProjectSettingsTab(book, projectName, configName);
        }
    }
    //-----------------------------------------------------------------------------------
    IEditor * PluginManager::NewEditor() 
    { 
        return gsgs_Frame()->GetMainBook()->NewEditor(); 
    }
    //-----------------------------------------------------------------------------------
    wxStyledTextCtrl * PluginManager::CreateEditor(wxWindow* parent)
    {
        EditorScrolledPanel * temp = new EditorScrolledPanel(parent);
        temp->SetScrollEvent(true);
        temp->Show();
        return temp->getEditor();
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::IsShutdownInProgress() const 
    { 
        return gsgs_Manager()->IsShutdownInProgress(); 
    }
    //-----------------------------------------------------------------------------------
    ImageManager * PluginManager::GetStdIcons()
    {
        return &gsgs_Image();
    }
    //-----------------------------------------------------------------------------------
    wxArrayString PluginManager::GetProjectCompileFlags(const wxString& projectName, bool isCppFile)
    {
        if(IsOpen() == false) return wxArrayString();

        wxArrayString args;

        // Next apppend the user include paths
        wxString errMsg;

        // First, we need to find the currently active workspace configuration
        BuildMatrixPtr matrix = GetWorkspace()->GetBuildMatrix();
        if(!matrix) { return wxArrayString(); }

        wxString workspaceSelConf = matrix->GetSelectedConfigurationName();

        // Now that we got the selected workspace configuration, extract the related project configuration
        ProjectPtr proj = GetWorkspace()->FindProjectByName(projectName, errMsg);
        if(!proj) { return args; }

        wxString projectSelConf = matrix->GetProjectSelectedConf(workspaceSelConf, proj->GetName());
        BuildConfigPtr dependProjbldConf = GetWorkspace()->GetProjBuildConf(proj->GetName(), projectSelConf);
        if(dependProjbldConf && dependProjbldConf->IsCustomBuild() == false) {
            // Get the include paths and add them
            wxString projectIncludePaths = dependProjbldConf->GetIncludePath();
            wxArrayString projectIncludePathsArr = wxStringTokenize(projectIncludePaths, wxT(";"), wxTOKEN_STRTOK);
            for(size_t i = 0; i < projectIncludePathsArr.GetCount(); i++) {
                args.Add(wxString::Format(wxT("-I%s"), projectIncludePathsArr[i].c_str()));
            }
            // get the compiler options and add them
            wxString projectCompileOptions = dependProjbldConf->GetCompileOptions();
            wxArrayString projectCompileOptionsArr = wxStringTokenize(projectCompileOptions, wxT(";"), wxTOKEN_STRTOK);
            for(size_t i = 0; i < projectCompileOptionsArr.GetCount(); i++) {
                wxString cmpOption(projectCompileOptionsArr.Item(i));
                cmpOption.Trim().Trim(false);
                wxString tmp;
                // Expand backticks / $(shell ...) syntax supported by gsgs
                if(cmpOption.StartsWith(wxT("$(shell "), &tmp) || cmpOption.StartsWith(wxT("`"), &tmp)) {
                    cmpOption = tmp;
                    tmp.Clear();
                    if(cmpOption.EndsWith(wxT(")"), &tmp) || cmpOption.EndsWith(wxT("`"), &tmp)) { cmpOption = tmp; }
                    if(m_backticks.find(cmpOption) == m_backticks.end()) {
                        // Expand the backticks into their value
                        wxArrayString outArr;
                        // Apply the environment before executing the command
                        EnvSetter setter(&gsgs_EditorConfig(), NULL, projectName, dependProjbldConf->GetName());
                        ProcUtils::SafeExecuteCommand(cmpOption, outArr);
                        wxString expandedValue;
                        for(size_t j = 0; j < outArr.size(); j++) {
                            expandedValue << outArr.Item(j) << wxT(" ");
                        }
                        m_backticks[cmpOption] = expandedValue;
                        cmpOption = expandedValue;
                    } else {
                        cmpOption = m_backticks.find(cmpOption)->second;
                    }
                }
                args.Add(cmpOption);
            }
            // get the compiler preprocessor and add them as well
            wxString projectPreps = dependProjbldConf->GetPreprocessor();
            wxArrayString projectPrepsArr = wxStringTokenize(projectPreps, wxT(";"), wxTOKEN_STRTOK);
            for(size_t i = 0; i < projectPrepsArr.GetCount(); i++) {
                args.Add(wxString::Format(wxT("-D%s"), projectPrepsArr[i].c_str()));
            }
        }
        return args;
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::AddEditorPage(wxWindow* page, const wxString& name, const wxString& tooltip)
    {
        gsgs_Frame()->GetMainBook()->AddPage(page, name, tooltip, wxNullBitmap, true);
    }
    //-----------------------------------------------------------------------------------
    wxPanel * PluginManager::GetEditorPaneNotebook() 
    { 
        return gsgs_Frame()->GetMainBook(); 
    }
    //-----------------------------------------------------------------------------------
    wxWindow * PluginManager::GetActivePage() 
    { 
        return gsgs_Frame()->GetMainBook()->GetCurrentPage(); 
    }
    //-----------------------------------------------------------------------------------
    wxWindow * PluginManager::GetPage(size_t page) 
    { 
        return gsgs_Frame()->GetMainBook()->GetPage(page); 
    }
    //-----------------------------------------------------------------------------------
    wxString PluginManager::GetPageTitle(wxWindow* win) const
    {
        return gsgs_Frame()->GetMainBook()->GetPageTitle(win);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::SetPageTitle(wxWindow* win, const wxString& title)
    {
        gsgs_Frame()->GetMainBook()->SetPageTitle(win, title);
    }
    //-----------------------------------------------------------------------------------
    ProjectPtr PluginManager::GetSelectedProject() const
    {
        return gsgs_Frame()->GetWorkspaceTab()->GetFileView()->GetSelectedProject();
    }
    //-----------------------------------------------------------------------------------
    IEditor* PluginManager::FindEditor(const wxString& filename) const
    {
        return gsgs_Frame()->GetMainBook()->FindEditor(filename);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::EnableClangCodeCompletion(bool b) 
    { 
        wxUnusedVar(b); 
    }
    //-----------------------------------------------------------------------------------
    size_t PluginManager::GetPageCount() const 
    { 
        return gsgs_Frame()->GetMainBook()->GetPageCount(); 
    }
    //-----------------------------------------------------------------------------------
    size_t PluginManager::GetAllEditors(IEditor::List_t& editors, bool inOrder)
    {
        EditorVec_t tmpEditors;
        size_t flags = Book::kGetAll_IncludeDetached;
        if(inOrder) { flags |= Book::kGetAll_RetainOrder; }

        gsgs_Frame()->GetMainBook()->GetAllEditors(tmpEditors, flags);
        editors.insert(editors.end(), tmpEditors.begin(), tmpEditors.end());
        return editors.size();
    }
    //-----------------------------------------------------------------------------------
    size_t PluginManager::GetAllBreakpoints(BreakpointInfo::Vec_t& breakpoints)
    {
        breakpoints.clear();
        gsgs_Manager()->GetBreakpointsMgr()->GetBreakpoints(breakpoints);
        return breakpoints.size();
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::DeleteAllBreakpoints() 
    { 
        gsgs_Manager()->GetBreakpointsMgr()->DelAllBreakpoints(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::SetBreakpoints(const BreakpointInfo::Vec_t& breakpoints)
    {
        gsgs_Manager()->GetBreakpointsMgr()->DelAllBreakpoints();
        for(size_t i = 0; i < breakpoints.size(); ++i) 
        {
            gsgs_Manager()->GetBreakpointsMgr()->AddBreakpoint(breakpoints.at(i));
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::LoadPerspective(const wxString& perspectiveName)
    {
        gsgs_Manager()->GetPerspectiveManager().LoadPerspective(perspectiveName);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::SavePerspective(const wxString& perspectiveName)
    {
        gsgs_Manager()->GetPerspectiveManager().SavePerspective(perspectiveName, true);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ProcessEditEvent(wxCommandEvent& e, IEditor* editor)
    {
        Editor* lEditor = dynamic_cast<Editor*>(editor);
        if(lEditor) { lEditor->OnMenuCommand(e); }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::AppendOutputTabText(eOutputPaneTab tab, const wxString& text)
    {
        switch(tab) {
        case kOutputTab_Build:
            gsgs_Frame()->GetOutputPane()->GetBuildTab()->AppendLine(text);
            break;
        case kOutputTab_Output:
            gsgs_Frame()->GetOutputPane()->GetOutputWindow()->AppendText(text);
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ClearOutputTab(eOutputPaneTab tab)
    {
        switch(tab) {
        case kOutputTab_Build:
            gsgs_Frame()->GetOutputPane()->GetBuildTab()->Clear();
            break;
        case kOutputTab_Output:
            gsgs_Frame()->GetOutputPane()->GetOutputWindow()->Clear();
            break;
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::AddWorkspaceToRecentlyUsedList(const wxFileName& filename)
    {
        if(filename.Exists()) { gsgs_Workspace().AddRecently(filename.GetFullPath()); }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::StoreWorkspaceSession(const wxFileName& workspaceFile)
    {
        if(workspaceFile.Exists()) {
            SessionEntry session;
            gsgs_Frame()->GetMainBook()->CreateSession(session);
            session.SetWorkspaceName(workspaceFile.GetFullPath());
            gsgs_Session().Save(session.GetWorkspaceName(), session);
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::LoadWorkspaceSession(const wxFileName& workspaceFile)
    {
        SessionEntry session;
        if(gsgs_Session().GetSession(workspaceFile.GetFullPath(), session)) {
            gsgs_Frame()->GetMainBook()->RestoreSession(session);
            // Set this session as the 'Last' session
            gsgs_Session().SetLastSession(workspaceFile.GetFullPath());
        }
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::OpenFindInFileForPath(const wxString& path)
    {
        wxCommandEvent ff(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_in_files"));
        wxArrayString paths;
        paths.Add(path);
        ff.SetClientData(new wxArrayString(paths));
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(ff);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::OpenFindInFileForPaths(const wxArrayString& paths)
    {
        wxCommandEvent ff(wxEVT_COMMAND_MENU_SELECTED, XRCID("find_in_files"));
        ff.SetClientData(new wxArrayString(paths));
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(ff);
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ShowOutputPane(const wxString& selectedWindow) 
    { 
        gsgs_Manager()->ShowOutputPane(selectedWindow); 
    }
    //-----------------------------------------------------------------------------------
    size_t PluginManager::GetAllTabs(TabVec_t& tabs)
    {
        gsgs_Frame()->GetMainBook()->GetAllTabs(tabs);
        return tabs.size();
    }
    //-----------------------------------------------------------------------------------
    StatusBar* PluginManager::GetStatusBar()
    {
        if(Frame::m_initCompleted) 
        { 
            return gsgs_Frame()->GetMainStatusBar();
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ToggleOutputPane(const wxString& selectedWindow)
    {
        if(gsgs_Manager()->IsPaneVisible(wxT("Output View"))) {
            if(!selectedWindow.IsEmpty()) {
                wxString selectedTabName;
                Notebook* book = gsgs_Frame()->GetOutputPane()->GetNotebook();
                int where = book->GetSelection();
                if(where != wxNOT_FOUND) { selectedTabName = book->GetPageText(where); }
                if(selectedTabName == selectedWindow) 
                {
                    // The requested tab is already selected, just hide the pane
                    gsgs_Manager()->HidePane("Output View");
                } 
                else 
                {
                    // The output pane is visible, but the selected tab is not the one we wanted
                    // Select it
                    gsgs_Manager()->ShowOutputPane(selectedWindow);
                }
            } 
            else 
            {
                // The output pane is visible and the selected tab is the one we requested
                // So just hide it
                gsgs_Manager()->HidePane("Output View");
            }
        } 
        else 
        {
            // The output pane is hidden, show it and select the requested tab
            gsgs_Manager()->ShowOutputPane(selectedWindow);
        }
    }
    //-----------------------------------------------------------------------------------
    WorkspaceView* PluginManager::GetWorkspaceView() 
    { 
        return gsgs_Frame()->GetWorkspaceTab()->GetView(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::ShowToolBar(bool show)
    {
        wxCommandEvent event(wxEVT_MENU, XRCID("hide_tool_bar"));
        event.SetInt(show ? 1 : 0);
        event.SetEventObject(gsgs_Frame());
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::IsToolBarShown() const
    {
        if(gsgs_Frame()->GetMainToolBar()) {
            // we have native toolbar
            return gsgs_Frame()->GetMainToolBar()->IsShown();
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    bool PluginManager::CloseEditor(IEditor* editor, bool prompt)
    {
        return gsgs_Frame()->GetMainBook()->ClosePage(editor, prompt);
    }
    //-----------------------------------------------------------------------------------
    EditorBar* PluginManager::GetNavigationBar() 
    { 
        return gsgs_Frame()->GetMainBook()->GetEditorBar(); 
    }
    //-----------------------------------------------------------------------------------
    ToolBar* PluginManager::GetToolBar() 
    { 
        return gsgs_Frame()->GetMainToolBar(); 
    }
    //-----------------------------------------------------------------------------------
    void PluginManager::DisplayMessage(const wxString& message, int flags,
                                       const std::vector<std::pair<wxWindowID, wxString> >& buttons)
    {
        return gsgs_Frame()->GetMessageBar()->DisplayMessage(message, flags, buttons);
    }
    //-----------------------------------------------------------------------------------
}