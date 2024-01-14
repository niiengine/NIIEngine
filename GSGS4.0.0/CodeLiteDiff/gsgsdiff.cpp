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

#include "NewFileComparison.h"
#include "gsgsWindowManager.h"
#include "gsgsFileDiffPanel.h"
#include "gsgsdiff.h"
#include "gsgsLogManager.h"
#include "gsgsFileDiffPanel.h"
#include "wx/menu.h"
#include <wx/xrc/xmlres.h>

static gsgsDiffPlugin* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) 
    { 
        thePlugin = new gsgsDiffPlugin(manager); 
    }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("NIIEngine Team Group"));
    info.SetName(wxT("Diff Plugin"));
    info.SetDescription(_("Diff Plugin"));
    info.SetVersion(wxT("v2.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

gsgsDiffPlugin::gsgsDiffPlugin(IManager* manager)
    : Plugin(manager)
{
    m_longName = _("gsgs Diff Plugin");
    m_shortName = wxT("Diff Plugin");

    Bind(wxEVT_MENU, &gsgsDiffPlugin::OnDiff, this, XRCID("diff_compare_with"));
    gsgs_Event().Bind(wxEVT_CONTEXT_MENU_TAB_LABEL, &gsgsDiffPlugin::OnTabContextMenu, this);
    gsgs_Window().addHotKey("diff_new_comparison", "Ctrl-Shift-C",
                                                   "Plugins::Diff Tool::New File Comparison");
    gsgs_Window().addHotKey("diff_new_folder", "Ctrl-Alt-F",
                                                   "Plugins::Diff Tool::New Folder Comparison");
    wxTheApp->Bind(wxEVT_MENU, &gsgsDiffPlugin::OnNewDiff, this, XRCID("diff_new_comparison"));
    wxTheApp->Bind(wxEVT_MENU, &gsgsDiffPlugin::OnNewDiffFolder, this, XRCID("diff_new_folder"));
}

gsgsDiffPlugin::~gsgsDiffPlugin() {}

void gsgsDiffPlugin::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void gsgsDiffPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("diff_new_comparison"), _("New File Comparison"), _("Start new diff"));
    menu->Append(XRCID("diff_new_folder"), _("New Folder Comparison"), _("Start new folders diff"));
    pluginsMenu->Append(wxID_ANY, _("Diff Tool"), menu);
}

void gsgsDiffPlugin::UnPlug()
{
    wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED, &gsgsDiffPlugin::OnNewDiff, this, XRCID("diff_new_comparison"));
    gsgs_Event().Unbind(wxEVT_CONTEXT_MENU_TAB_LABEL, &gsgsDiffPlugin::OnTabContextMenu, this);
    Unbind(wxEVT_MENU, &gsgsDiffPlugin::OnDiff, this, XRCID("diff_compare_with"));
}

void gsgsDiffPlugin::OnNewDiff(wxCommandEvent& e)
{
    FileDiffFrame* diff = new FileDiffFrame(gsgs_Event().TopFrame());
    diff->Show();
}

void gsgsDiffPlugin::OnNewDiffFolder(wxCommandEvent& e)
{
    DiffFoldersFrame diff(gsgs_Event().TopFrame());
    diff.ShowModal();
}

void gsgsDiffPlugin::OnTabContextMenu(ContextMenuEvent& event)
{
    event.Skip();
    DoClear();
    gsgs::IEditor* activeEditor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    m_leftFile = activeEditor->GetFileName();

    // Edit the context menu
    wxMenuItem* mi = new wxMenuItem(event.GetMenu(), XRCID("diff_compare_with"), _("Compare with..."), "");
    mi->SetBitmap(m_mgr->GetStdIcons()->LoadBitmap("diff"));
    event.GetMenu()->AppendSeparator();
    event.GetMenu()->Append(mi);
    event.GetMenu()->Bind(wxEVT_MENU, &gsgsDiffPlugin::OnDiff, this, XRCID("diff_compare_with"));
}

void gsgsDiffPlugin::DoClear() { m_leftFile.Clear(); }

void gsgsDiffPlugin::OnDiff(wxCommandEvent& event)
{
    bool tempfile(false);
    NewFileComparison dlg(gsgs_Event().TopFrame(), m_leftFile);
    if(dlg.ShowModal() == wxID_OK) {
        if(m_leftFile.GetName().StartsWith("new")) {
            tempfile = true;
            m_leftFile = SaveEditorToTmpfile(m_mgr->GetActiveEditor());
            if(!m_leftFile.IsOk()) {
                gsgs_Debug("gsgsDiffPlugin::OnDiff: call to SaveEditorToTmpfile() failed for m_leftFile");
                return;
            }
        }
        wxString secondFile = dlg.GetTextCtrlFileName()->GetValue();
        if(secondFile.StartsWith("new")) {
            tempfile = true;
            gsgs::IEditor* editor = m_mgr->FindEditor(secondFile);
            if(!editor) {
                gsgs_Debug("gsgsDiffPlugin::OnDiff: call to FindEditor() failed");
                return;
            }
            wxFileName rightFn = SaveEditorToTmpfile(editor);
            if(!rightFn.IsOk()) {
                gsgs_Debug("gsgsDiffPlugin::OnDiff: call to SaveEditorToTmpfile() failed for secondFile");
                return;
            }
            secondFile = rightFn.GetFullPath();
        }

        // Check that we're not trying to diff an editor against itself
        // If we are and it's been edited, diff against the unaltered version
        if(m_leftFile.GetFullPath() == secondFile) {
            gsgs::IEditor* editor = m_mgr->FindEditor(secondFile);
            if(editor && editor->IsModified()) {
                wxFileName rightFn = SaveEditorToTmpfile(editor);
                if(!rightFn.IsOk()) {
                    gsgs_Debug("gsgsDiffPlugin::OnDiff: call to SaveEditorToTmpfile() failed for secondFile");
                    return;
                }
                secondFile = rightFn.GetFullPath();

            } else {
                gsgs_Debug("gsgsDiffPlugin::OnDiff: trying to diff an editor against itself");
                return;
            }
        }

        FileDiffFrame* diffView = new FileDiffFrame(gsgs_Event().TopFrame(), m_leftFile, secondFile, tempfile);
        diffView->Show();
    }
}

wxFileName gsgsDiffPlugin::SaveEditorToTmpfile(gsgs::IEditor* editor) const
{
    wxString content = editor->GetEditorText();
    if(content.empty()) {
        return wxFileName(); // Nothing to diff
    }

    wxString tpath(wxFileName::GetTempDir());
    tpath << gsgsPathSep << "gsgsdifftemp" << gsgsPathSep;
    wxFileName::Mkdir(tpath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    wxFileName tmpFile(wxFileName::CreateTempFileName(tpath + editor->GetFileName().GetName()));
    if(!tmpFile.IsOk()) { return wxFileName(); }

    tmpFile.SetExt(editor->GetFileName().GetExt());
    wxFFile fp(tmpFile.GetFullPath(), "w+b");
    if(fp.IsOpened()) {
        fp.Write(content);
        fp.Close();
    } else {
        return wxFileName();
    }

    return tmpFile;
}

