#include "NodeDebuggerPane.h"
#include "NodeJSEvents.h"
#include "NodeJSWorkspaceView.h"
#include "NoteJSWorkspace.h"
#include "PHPSourceFile.h"
#include "PhpLexerAPI.h"
#include "WebToolsBase.h"
#include "WebToolsConfig.h"
#include "WebToolsSettings.h"
#include "gsgsImageManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsEvent.h"
#include "gsgsTagManager.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "gsgsTag.h"
#include "webtools.h"
#include <wx/dirdlg.h>
#include <wx/menu.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include "gsgsNodeJS.h"

static WebTools* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) { thePlugin = new WebTools(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("eran"));
    info.SetName(wxT("WebTools"));
    info.SetDescription(_("Support for JavaScript, CSS/SCSS, HTML, XML and other web development tools"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

WebTools::WebTools(IManager* manager)
    : Plugin(manager)
    , m_lastColourUpdate(0)
    , m_clangOldFlag(false)
{
    m_longName = _("Support for JavaScript, CSS/SCSS, HTML, XML and other web development tools");
    m_shortName = wxT("WebTools");

    // Initialise NodeJS
    WebToolsConfig& conf = WebToolsConfig::Get().Load();
    wxFileName fnNodeJS(conf.GetNodejs());
    wxArrayString hints;
    if(fnNodeJS.FileExists()) { hints.Add(fnNodeJS.GetPath()); }
    gsgs::NodeJS::Get().Initialise(hints);

    // Update the configuration with the findings
    conf.SetNodejs(gsgs::NodeJS::Get().GetNode().GetFullPath());
    conf.SetNpm(gsgs::NodeJS::Get().GetNpm().GetFullPath());

    // Register our new workspace type
    NodeJSWorkspace::Get(); // Instantiate the singleton by faking a call
    gsgs_Workspace().RegisterWorkspace(new NodeJSWorkspace(true));

    // Create the syntax highligher worker thread
    m_jsColourThread = new JavaScriptSyntaxColourThread(this);
    m_jsColourThread->Create();
    m_jsColourThread->Run();

    gsgs_Event().Bind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    gsgs_Event().Bind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &WebTools::OnTheme, this);

    // Context menu
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);

    // Workspace related events
    gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);

    // Theme management
    gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);

    // Debugger related
    gsgs_Event().Bind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSCliDebuggerStarted, this);
    gsgs_Event().Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    gsgs_Event().Bind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

    Bind(wxEVT_MENU, &WebTools::OnSettings, this, XRCID("webtools_settings"));
    Bind(wxEVT_NODE_COMMAND_TERMINATED, &WebTools::OnNodeCommandCompleted, this);

    m_jsCodeComplete.Reset(new JSCodeCompletion("", this));
    m_xmlCodeComplete.Reset(new XMLCodeCompletion(this));
    m_cssCodeComplete.Reset(new CSSCodeCompletion(this));
    m_jsctags.Reset(new gsgs::JSCTags());

    // Connect the timer
    m_timer = new wxTimer(this);
    m_timer->Start(3000);
    Bind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Bind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));
}

WebTools::~WebTools() { NodeJSWorkspace::Free(); }

void WebTools::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void WebTools::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("webtools_settings"), _("Settings..."));
    pluginsMenu->Append(wxID_ANY, _("WebTools"), menu);
    menu->SetNextHandler(this);
    this->SetPreviousHandler(menu);
}

void WebTools::UnPlug()
{
    // Store the configuration changes
    WebToolsConfig::Get().SaveConfig();
    Unbind(wxEVT_NODE_COMMAND_TERMINATED, &WebTools::OnNodeCommandCompleted, this);

    // Stop the debugger
    if(NodeJSWorkspace::Get()->IsOpen() && NodeJSWorkspace::Get()->GetDebugger() &&
       NodeJSWorkspace::Get()->GetDebugger()->IsRunning()) {
        NodeJSWorkspace::Get()->GetDebugger()->StopDebugger();
    }

    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_EDITOR, &WebTools::OnEditorContextMenu, this);
    gsgs_Event().Unbind(wxEVT_FILE_LOADED, &WebTools::OnFileLoaded, this);
    gsgs_Event().Unbind(wxEVT_FILE_SAVED, &WebTools::OnFileSaved, this);
    gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &WebTools::OnTheme, this);
    gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &WebTools::OnWorkspaceClosed, this);
    gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED, &WebTools::OnWorkspaceLoaded, this);
    gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &WebTools::OnEditorChanged, this);
    gsgs_Event().Unbind(wxEVT_NODEJS_DEBUGGER_STARTED, &WebTools::OnNodeJSCliDebuggerStarted, this);
    gsgs_Event().Unbind(wxEVT_NODEJS_DEBUGGER_STOPPED, &WebTools::OnNodeJSDebuggerStopped, this);
    gsgs_Event().Unbind(wxEVT_DBG_IS_PLUGIN_DEBUGGER, &WebTools::OnIsDebugger, this);

    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentLine, this, XRCID("comment_line"));
    wxTheApp->Unbind(wxEVT_MENU, &WebTools::OnCommentSelection, this, XRCID("comment_selection"));

    // Disconnect the timer events
    Unbind(wxEVT_TIMER, &WebTools::OnTimer, this, m_timer->GetId());
    m_timer->Stop();
    wxDELETE(m_timer);

    m_jsColourThread->Stop();
    wxDELETE(m_jsColourThread);
    m_jsCodeComplete.Reset(NULL);
}

void WebTools::DoRefreshColours(const wxString& filename)
{
    if(FileUtil::GetType(filename) == FileUtil::TypeJS) { m_jsColourThread->QueueFile(filename); }
}

void WebTools::ColourJavaScript(const JavaScriptSyntaxColourThread::Reply& reply)
{
    IEditor* editor = m_mgr->FindEditor(reply.filename);
    if(editor) {
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        ctrl->SetKeyWords(1, reply.properties);
        ctrl->SetKeyWords(3, reply.functions);
        m_lastColourUpdate = time(NULL);
    }
}

void WebTools::OnTheme(wxCommandEvent& event)
{
    event.Skip();
    IEditor::List_t editors;
    m_mgr->GetAllEditors(editors);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        // Refresh the files' colouring
        if(IsJavaScriptFile((*iter)->GetFileName())) {
            m_jsColourThread->QueueFile((*iter)->GetFileName().GetFullPath());
        }
    }
}

bool WebTools::IsJavaScriptFile(const wxFileName& filename)
{
    return FileUtil::GetType(filename.GetFullName()) == FileUtil::TypeJS;
}

bool WebTools::IsJavaScriptFile(const wxString& filename)
{
    return FileUtil::GetType(filename) == FileUtil::TypeJS;
}

void WebTools::OnWorkspaceClosed(wxCommandEvent& event) { event.Skip(); }

void WebTools::OnEditorChanged(wxCommandEvent& event)
{
    // If we have no JS files opened, cleanup the resources
    event.Skip();
}

void WebTools::OnSettings(wxCommandEvent& event)
{
    WebToolsSettings settings(m_mgr->GetTheApp()->GetTopWindow());
    if(settings.ShowModal() == wxID_OK) {
        NodeJSWorkspace::Get()->AllocateDebugger();
        if(m_jsCodeComplete) {
            m_jsCodeComplete->Reload();
            m_jsCodeComplete->ClearFatalError();
        }
        if(m_xmlCodeComplete) {
            m_xmlCodeComplete->Reload();
            m_jsCodeComplete->ClearFatalError();
        }
    }
}

void WebTools::OnTimer(wxTimerEvent& event)
{
    event.Skip();

    time_t curtime = time(NULL);
    if((curtime - m_lastColourUpdate) < 5) return;
    IEditor* editor = m_mgr->GetActiveEditor();

    // Sanity
    CHECK_PTR_RET(editor);
    CHECK_PTR_RET(editor->IsModified());
    if(!IsJavaScriptFile(editor->GetFileName())) return;

    // This file is a modified JS file
    m_lastColourUpdate = time(NULL);
    m_jsColourThread->QueueBuffer(editor->GetFileName().GetFullPath(), editor->GetTextRange(0, editor->GetLength()));
}

bool WebTools::IsJavaScriptFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileUtil::IsJavascriptFile(editor->GetFileName())) return true;

    // We should also support Code Completion when inside a PHP/HTML file, but within a script area
    if(FileUtil::IsPHPFile(editor->GetFileName())) {
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        int styleAtCurPos = ctrl->GetStyleAt(ctrl->GetCurrentPos());
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) { return true; }
    }
    return false;
}

bool WebTools::InsideJSComment(IEditor* editor)
{
    int curpos = editor->PositionBeforePos(editor->GetCurrentPosition());
    int styleAtCurPos = editor->GetCtrl()->GetStyleAt(curpos);
    if(FileUtil::IsJavascriptFile(editor->GetFileName())) {
        // Use the Cxx macros
        return styleAtCurPos == wxSTC_C_COMMENT || styleAtCurPos == wxSTC_C_COMMENTLINE ||
               styleAtCurPos == wxSTC_C_COMMENTDOC || styleAtCurPos == wxSTC_C_COMMENTLINEDOC ||
               styleAtCurPos == wxSTC_C_COMMENTDOCKEYWORD || styleAtCurPos == wxSTC_C_COMMENTDOCKEYWORDERROR ||
               styleAtCurPos == wxSTC_C_PREPROCESSORCOMMENT;
    } else if(FileUtil::IsPHPFile(editor->GetFileName())) {
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return styleAtCurPos == wxSTC_HJ_COMMENT || styleAtCurPos == wxSTC_HJ_COMMENTLINE ||
                   styleAtCurPos == wxSTC_HJ_COMMENTDOC;
        }
    }
    return false;
}

bool WebTools::InsideJSString(IEditor* editor)
{
    int curpos = editor->PositionBeforePos(editor->GetCurrentPosition());
    int styleAtCurPos = editor->GetCtrl()->GetStyleAt(curpos);
    if(FileUtil::IsJavascriptFile(editor->GetFileName())) {
        // Use the Cxx macros
        return styleAtCurPos == wxSTC_C_STRING || styleAtCurPos == wxSTC_C_CHARACTER ||
               styleAtCurPos == wxSTC_C_STRINGEOL || styleAtCurPos == wxSTC_C_STRINGRAW ||
               styleAtCurPos == wxSTC_C_HASHQUOTEDSTRING;
    } else if(FileUtil::IsPHPFile(editor->GetFileName())) {
        if(styleAtCurPos >= wxSTC_HJ_START && styleAtCurPos <= wxSTC_HJA_REGEX) {
            return styleAtCurPos == wxSTC_HJ_DOUBLESTRING || styleAtCurPos == wxSTC_HJ_SINGLESTRING ||
                   styleAtCurPos == wxSTC_HJ_STRINGEOL;
        }
    }
    return false;
}

void WebTools::OnCommentLine(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(IsJavaScriptFile(editor)) {
        e.Skip(false);
        editor->ToggleLineComment("//", wxSTC_C_COMMENTLINE);
    }
}

void WebTools::OnCommentSelection(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    if(IsJavaScriptFile(editor)) {
        e.Skip(false);
        editor->CommentBlockSelection("/*", "*/");
    } else if(IsHTMLFile(editor)) {
        e.Skip(false);
        editor->CommentBlockSelection("<!-- ", " -->");
    }
}

bool WebTools::IsHTMLFile(IEditor* editor)
{
    CHECK_PTR_RET_FALSE(editor);
    if(FileUtil::GetType(editor->GetFileName().GetFullName()) == FileUtil::TypeHtml) return true;

    // We should also support Code Completion when inside a mixed PHP and HTML file
    if(FileUtil::IsPHPFile(editor->GetFileName())) {

        // Check to see if we are inside a PHP section or not
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
        return !PHPSourceFile::IsInPHPSection(buffer);
    }
    return false;
}

void WebTools::OnNodeJSDebuggerStopped(gsgs::DebugEvent& event)
{
    event.Skip();

    gsgsDEBUG1() << "Saving NodeJS debugger perspective";

    wxString layoutFileName = "nodejs.layout";
    if(event.GetEventType() == wxEVT_NODEJS_DEBUGGER_STOPPED) { layoutFileName = "nodejs_cli.layout"; }
    wxFileName fnNodeJSLayout(gsgs_Path().GetUserDataDir(), layoutFileName);
    fnNodeJSLayout.AppendDir("config");
    FileUtil::WriteFileContent(fnNodeJSLayout, m_mgr->GetAuiManager()->SavePerspective());

    if(!m_savePerspective.IsEmpty()) {
        m_mgr->GetAuiManager()->LoadPerspective(m_savePerspective);
        m_savePerspective.clear();
    }
}

void WebTools::EnsureAuiPaneIsVisible(const wxString& paneName, bool update)
{
    wxAuiPaneInfo& pi = m_mgr->GetAuiManager()->GetPane(paneName);
    if(pi.IsOk() && !pi.IsShown()) { pi.Show(); }
    if(update) { m_mgr->GetAuiManager()->Update(); }
}

void WebTools::OnWorkspaceLoaded(wxCommandEvent& event)
{
    event.Skip();
    wxFileName workspaceFile = event.GetString();
    if(FileUtil::GetType(workspaceFile.GetFullPath()) == FileUtil::TypeWorkspaceNodeJS) {
        // we need to do this in 2 steps: this is the because the service provider will auto register us on destuctions
        // by NAME this means, that 'reset' will first call 'new' followed by 'delete' so we will end up to be removed
        // from the list of service providers
        m_jsCodeComplete.Reset(nullptr);
        m_jsCodeComplete.Reset(new JSCodeCompletion(workspaceFile.GetPath(), this));
    } else {
        // For non NodeJS workspaces, create the .tern files under
        // the .gsgsworkspace folder
        workspaceFile.AppendDir(".gsgsworkspace");
        m_jsCodeComplete.Reset(nullptr);
        m_jsCodeComplete.Reset(new JSCodeCompletion(workspaceFile.GetPath(), this));
    }
}

bool WebTools::IsCSSFile(IEditor* editor)
{
    return (FileUtil::GetType(editor->GetFileName().GetFullName()) == FileUtil::TypeCSS);
}

void WebTools::OnFileLoaded(gsgs::Event& event)
{
    event.Skip();
    DoRefreshColours(event.GetFileName());
}

void WebTools::OnFileSaved(gsgs::Event& event)
{
    event.Skip();
    DoRefreshColours(event.GetFileName());
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        if(m_jsCodeComplete) { m_jsCodeComplete->ResetTern(false); }
        // Remove all compiler markers
        editor->DelAllCompilerMarkers();
        if(WebToolsConfig::Get().IsLintOnSave()) { gsgs::NodeJS::Get().LintFile(event.GetFileName()); }
    }
}

void WebTools::OnEditorContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    IEditor* editor = m_mgr->GetActiveEditor();
    if(editor && m_jsCodeComplete && IsJavaScriptFile(editor) && !InsideJSComment(editor)) {
        m_jsCodeComplete->AddContextMenu(event.GetMenu(), editor);
    }
}

void WebTools::OnIsDebugger(gsgs::DebugEvent& event)
{
    event.Skip(); // always call skip
    // event.GetStrings().Add("NodeJS Debugger");
}

void WebTools::OnNodeJSCliDebuggerStarted(gsgs::DebugEvent& event)
{
    event.Skip();
    m_savePerspective = GetManager()->GetAuiManager()->SavePerspective();

    wxWindow* parent = m_mgr->GetAuiManager()->GetManagedWindow();
    // Show the debugger pane
    if(!m_nodejsCliDebuggerPane) {
        m_nodejsCliDebuggerPane = new NodeDebuggerPane(parent);
        // Let the pane process the 'startup' event as well
        m_nodejsCliDebuggerPane->GetEventHandler()->ProcessEvent(event);
        GetManager()->GetAuiManager()->AddPane(m_nodejsCliDebuggerPane, wxAuiPaneInfo()
                                                                                  .MinSize(wxSize(-1, 300))
                                                                                  .Layer(5)
                                                                                  .Name("nodejs_cli_debugger")
                                                                                  .Caption("Node.js Debugger")
                                                                                  .CloseButton(false)
                                                                                  .MaximizeButton()
                                                                                  .Bottom()
                                                                                  .Position(0));
    }

    wxString layout;
    wxFileName fnNodeJSLayout(gsgs_Path().GetUserDataDir(), "nodejs_cli.layout");
    fnNodeJSLayout.AppendDir("config");
    if(FileUtil::ReadFileContent(fnNodeJSLayout, layout)) { m_mgr->GetAuiManager()->LoadPerspective(layout); }
    EnsureAuiPaneIsVisible("nodejs_cli_debugger", true);
}

void WebTools::OnNodeCommandCompleted(gsgs::ProcessEvent& event)
{
    event.Skip();
    if(event.GetString() == "npm-install-tern") {
        // tern installation completed, enable the code completion again
        GetManager()->SetStatusMessage("tern installed", 5);
        WebToolsConfig::Get().EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, true);
        if(m_jsCodeComplete) { m_jsCodeComplete->ResetTern(true); }
    }
}
