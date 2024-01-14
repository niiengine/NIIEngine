#include "PHPRefactoringPreviewDlg.h"
#include "gsgsProcess.h"
#include "gsgsPluginCommon.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "phprefactoring.h"
#include "phprefactoringdlg.h"
#include "phprefactoringoptions.h"
#include <sstream>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

static PHPRefactoring* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new PHPRefactoring(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Anders Jenbo"));
    info.SetName(wxT("PHPRefactoring"));
    info.SetDescription(_("Uses PHP Refactoring Browser to provide refactoring capabilities for php"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

PHPRefactoring::PHPRefactoring(IManager* manager)
    : Plugin(manager)
{
    m_manager = manager;
    m_longName = _("Uses PHP Refactoring Browser to provide refactoring capabilities for php");
    m_shortName = wxT("PHPRefactoring");
    m_settings.Load();
    m_settingsPhp.Load();
}

PHPRefactoring::~PHPRefactoring() {}

void PHPRefactoring::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void PHPRefactoring::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    item = new wxMenuItem(menu, wxID_SETTINGS, _("Options..."), wxEmptyString, wxITEM_NORMAL);
    menu->Append(item);
    pluginsMenu->Append(wxID_ANY, _("PHP Refactoring"), menu);

    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnMenuCommand, this, wxID_SETTINGS);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnExtractMethod, this, wxID_EXTRACT_METHOD);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnRenameLocalVariable, this, wxID_RENAME_LOCAL_VARIABLE);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnRenameClassProperty, this, wxID_RENAME_CLASS_PROPERTY);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnConvertLocalToInstanceVariable, this,
                   wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE);
    wxTheApp->Bind(wxEVT_MENU, &PHPRefactoring::OnOptimizeUseStatements, this, wxID_OPTIMIZE_USE_STATEMENTS);
    wxTheApp->Connect(wxID_RENAME_CLASS_AND_NAMESPACES, wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(PHPRefactoring::OnRenameClassAndNamespaces), NULL, this);
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_EDITOR, &PHPRefactoring::OnEditorContextMenu, this);
    gsgs_Event().Bind(wxEVT_PHP_SETTINGS_CHANGED, &PHPRefactoring::OnPhpSettingsChanged, this);
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_FOLDER, &PHPRefactoring::OnContextMenu, this);
}

void PHPRefactoring::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnMenuCommand, this, wxID_SETTINGS);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnExtractMethod, this, wxID_EXTRACT_METHOD);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnRenameLocalVariable, this, wxID_RENAME_LOCAL_VARIABLE);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnRenameClassProperty, this, wxID_RENAME_CLASS_PROPERTY);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnConvertLocalToInstanceVariable, this,
                     wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE);
    wxTheApp->Unbind(wxEVT_MENU, &PHPRefactoring::OnOptimizeUseStatements, this, wxID_OPTIMIZE_USE_STATEMENTS);
    wxTheApp->Disconnect(wxID_RENAME_CLASS_AND_NAMESPACES, wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(PHPRefactoring::OnRenameClassAndNamespaces), NULL, this);
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_EDITOR, &PHPRefactoring::OnEditorContextMenu, this);
    gsgs_Event().Unbind(wxEVT_PHP_SETTINGS_CHANGED, &PHPRefactoring::OnPhpSettingsChanged, this);
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_FOLDER, &PHPRefactoring::OnContextMenu, this);
}

void PHPRefactoring::OnEditorContextMenu(ContextMenuEvent& event)
{
    event.Skip();

    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor || !FileUtil::IsPHPFile(editor->GetFileName())) {
        return; // Not in php file, do not add options to context menu
    }

    wxMenu* refactoringMenu = new wxMenu;
    refactoringMenu->Append(wxID_EXTRACT_METHOD, _("Extract Method"));
    refactoringMenu->Append(wxID_RENAME_LOCAL_VARIABLE, _("Rename Local Variable"));
    refactoringMenu->Append(wxID_RENAME_CLASS_PROPERTY, _("Rename Class Property"));
    refactoringMenu->Append(wxID_CONVERT_LOCAL_TO_INSTANCE_VARIABLE, _("Convert Local to Instance Variable"));
    refactoringMenu->Append(wxID_OPTIMIZE_USE_STATEMENTS, _("Optimize use statements"));

    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(wxID_ANY, _("Refactoring"), refactoringMenu);
}

void PHPRefactoring::OnMenuCommand(wxCommandEvent& e)
{
    wxUnusedVar(e);

    PHPRefactoringDlg dlg(gsgs_Event().TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetPhprefactoringPhar(dlg.GetFilePickerPhprefactoringPhar()->GetFileName())
            .SetSkipPreview(dlg.GetCheckBoxSkipPreview()->GetValue())
            .Save();
    }
}

void PHPRefactoring::OnExtractMethod(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) { return; }

    int startLine = editor->LineFromPos(editor->GetSelectionStart()) + 1;
    int endLine = editor->LineFromPos(editor->GetSelectionEnd()) + 1;
    wxString method = wxGetTextFromUser("Name the new methode");
    if(method.IsEmpty()) { return; }

    if(method.Contains(" ")) {
        ::wxMessageBox(_("Methode name may not contain spaces"), "PHPRefactoring", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    std::stringstream ss;
    ss << startLine << "-" << endLine;
    std::string range = ss.str();
    wxString parameters = range + " " + method;
    RefactorFile("extract-method", parameters, editor);
}

void PHPRefactoring::OnRenameLocalVariable(wxCommandEvent& e) { RenameVariable("rename-local-variable"); }

void PHPRefactoring::OnRenameClassProperty(wxCommandEvent& e) { RenameVariable("rename-property"); }

void PHPRefactoring::RenameVariable(const wxString& action)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) { return; }

    wxString line;
    line << (editor->GetCurrentLine() + 1);
    wxString oldName = editor->GetWordAtCaret();
    if(oldName.StartsWith("$")) { oldName = oldName.Mid(1); }
    if(oldName.IsEmpty()) { return; }

    wxString newName = wxGetTextFromUser("New name for " + oldName);
    newName.Trim().Trim(false);

    // If it starts with $ sign, remove it
    if(newName.StartsWith("$")) { newName = newName.Mid(1); }

    // Sanity
    if(newName.IsEmpty()) { return; }

    wxString parameters = line + " " + oldName + " " + newName;
    RefactorFile(action, parameters, editor);
}

void PHPRefactoring::OnConvertLocalToInstanceVariable(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) { return; }

    wxString line;
    line << (editor->GetCurrentLine() + 1);
    wxString oldName = editor->GetWordAtCaret();
    if(oldName.StartsWith("$")) { oldName = oldName.Mid(1); }
    if(oldName.IsEmpty()) { return; }

    wxString parameters = line + " " + oldName;
    RefactorFile("convert-local-to-instance-variable", parameters, editor);
}

void PHPRefactoring::OnRenameClassAndNamespaces(wxCommandEvent& e)
{
    wxString msg;
    msg << _("This will sync namespaces and classes with folder and filenames, for all files in the selected folder, "
             "to comply with psr-0\nContinue?");
    if(wxYES != ::wxMessageBox(msg, "PHP Refactoring", wxYES_NO | wxCANCEL | wxCENTER)) { return; }

    RunCommand("fix-class-names " + m_selectedFolder, m_selectedFolder);
    // Reload the patched files
    gsgs_Event().PostReloadExternallyModifiedEvent(false);
}

void PHPRefactoring::OnOptimizeUseStatements(wxCommandEvent& e)
{
    IEditor* editor = m_manager->GetActiveEditor();
    if(!editor) { return; }

    RefactorFile("optimize-use", "", editor);
}

void PHPRefactoring::RefactorFile(const wxString& action, const wxString& extraParameters, IEditor*& editor)
{
    wxString filePath, tmpfile, parameters, output;
    filePath = editor->GetFileName().GetFullPath();
    tmpfile << filePath << "-refactoring-browser.php";
    wxString oldContent = editor->GetEditorText();
    if(!FileUtil::WriteFileContent(tmpfile, oldContent)) {
        ::wxMessageBox(_("Can not refactor file:\nFailed to write temporary file"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    // Ensure that the temporary file is deleted once we are done with it
    FileUtil::Deleter fd(tmpfile);

    ::WrapWithQuotes(tmpfile);
    parameters = action + " " + tmpfile + " " + extraParameters;

    // Notify about indentation about to start
    wxCommandEvent evt(wxEVT_CODEFORMATTER_INDENT_STARTING);
    evt.SetString(filePath);
    gsgs_Event().ProcessEvent(evt);

    m_manager->SetStatusMessage(wxString::Format(wxT("%s: %s..."), _("Refactoring"), filePath.c_str()), 0);

    RunCommand(parameters);

    if(!FileUtil::ReadFileContent(tmpfile, output)) {
        ::wxMessageBox(_("Can not refactor file:\nfailed to read temporary file content"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    if(!oldContent.IsSameAs(output)) {
        // Update the editor
        gsgs::EditorStateLocker lk(editor->GetCtrl());
        editor->GetCtrl()->BeginUndoAction();
        editor->SetEditorText(output);
        editor->GetCtrl()->EndUndoAction();
    }

    m_manager->SetStatusMessage(_("Done"), 0);

    // Notify that a file was indented
    wxCommandEvent evtDone(wxEVT_CODEFORMATTER_INDENT_COMPLETED);
    evtDone.SetString(filePath);
    gsgs_Event().AddPendingEvent(evtDone);
}

void PHPRefactoring::RunCommand(const wxString& parameters, const wxString& workingDir)
{
    wxString phpPath, refactorPath, command;

    wxFileName php(m_settingsPhp.GetPhpExe());
    if(!php.Exists()) {
        ::wxMessageBox(_("Can not refactor file: Missing PHP executable path"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    phpPath = php.GetFullPath();
    ::WrapWithQuotes(phpPath);

    wxFileName refactor(m_settings.GetPhprefactoringPhar());
    if(!refactor.Exists()) {
        ::wxMessageBox(_("Can not refactor file: Missing PHP Refactoring Browser path"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    refactorPath = refactor.GetFullPath();
    ::WrapWithQuotes(refactorPath);

    command = phpPath + " " + refactorPath + " " + parameters;
    gsgsDEBUG() << "PHPRefactoring running:" << command << clEndl;
    ::WrapInShell(command);
    IProcess::Ptr_t process(
        ::CreateSyncProcess(command, IProcessCreateDefault | IProcessCreateWithHiddenConsole, workingDir));
    // CHECK_PTR_RET_FALSE(process);

    wxString patch, tmpfile;
    process->WaitForTerminate(patch);
    gsgsDEBUG() << "PHPRefactoring output:" << patch << clEndl;

    if(!patch.StartsWith("--- a/")) { // not a patch
        wxString errorMessage = "Refactoring failed!";
        wxRegEx reLine("[ \t]*on line ([0-9]+)");
        if(patch.Contains("Exception]")) { // has an error exception
            int start = patch.Find("Exception]");
            errorMessage = patch.Mid(start).AfterFirst('\n').BeforeFirst('\n');
            errorMessage = errorMessage.Trim().Trim(false);
        } else if(reLine.Matches(patch)) {
            wxString strLine = reLine.GetMatch(patch, 1);

            int start = patch.Find("error:") + 6;
            int end = patch.Find(" in ");
            errorMessage = patch.Mid(start, end - start);
            errorMessage = _("Can only refactor well formed code: ") + errorMessage + _(" on line ") + strLine;
        } else {
            errorMessage << _("\nExpected patch format. Received:\n") << patch;
        }

        // Truncate the message to something readable
        if(errorMessage.length() > 500) {
            errorMessage = errorMessage.Mid(0, 500);
            errorMessage << "...";
        }
        ::wxMessageBox(errorMessage, "PHP Refactoring", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }

    wxFileName fnTmpFile(wxFileName::CreateTempFileName("diff-XXXXXX"));
    tmpfile = fnTmpFile.GetFullPath();
    if(!FileUtil::WriteFileContent(tmpfile, patch)) {
        ::wxMessageBox(_("Can not refactor file:\nFailed to write temporary file"), "PHP Refactoring",
                       wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    FileUtil::Deleter fd(tmpfile);

    // Load the changes into the preview dialog
    PHPRefactoringPreviewDlg dlg(gsgs_Event().TopFrame(), patch);
    if(m_settings.GetSkipPreview() || dlg.ShowModal() == wxID_OK) {
        // Apply the patch
        try {
            Patch patcher;
            // We pass "--verbose" otherwise it crashes oftenly on Windows... go figure...
            patcher.patch(tmpfile, "", "--ignore-whitespace --verbose -p1 < ");
        } catch(gsgs::Exception& e) {
            wxMessageBox(e.What(), "gsgs", wxICON_ERROR | wxOK | wxCENTER, gsgs_Event().TopFrame());
        }
    }
}

void PHPRefactoring::OnContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    wxMenu* menu = new wxMenu();
    menu->Append(wxID_RENAME_CLASS_AND_NAMESPACES, _("Rename Class and Namespaces"));
    wxMenuItem* item = new wxMenuItem(event.GetMenu(), wxID_ANY, wxT("PHP Refactoring"));
    item->SetSubMenu(menu);
    item->SetBitmap(gsgs_Image().LoadBitmap("php-workspace"));
    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(item);
    m_selectedFolder = event.GetPath();
}

void PHPRefactoring::OnPhpSettingsChanged(gsgs::Event& event)
{
    event.Skip();
    m_settingsPhp.Load();
}
