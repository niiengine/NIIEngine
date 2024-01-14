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

#include "UILayoutEditorFrame.h"
#include "UILayoutEditorCanvas.h"
#include "UILayoutDialogMain.h"
#include "ExceptionManager.h"
#include "StrHelper.h"
#include "NiiException.h"
#include "NiiFontManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiPixelBufferManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiUISchemeManager.h"
#include "NiiScriptModule.h"
#include "NiiEngine.h"
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include "app.h"
// Toolbar icons
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/delete.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/unlock.xpm"
#include "bitmaps/lock.xpm"

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define BORDER                  5
#define DEFAULT_SIZE			25

#define ASSERT_INFO wxT("Dialog not correctly constructed!")

//////////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////////

#define ASSERT_DOCUMENT_MSG wxT("The m_document member is NULL!")

using namespace NII;

namespace gsgs
{
    //////////////////////////////////////////////////////////////////////////
    // DIALOG GRID EVENT TABLE
    //////////////////////////////////////////////////////////////////////////
    //--------------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(DialogGrid, wxDialog)
    EVT_BUTTON( ID_OK_BUTTON,	DialogGrid::OnOk)
    EVT_BUTTON( ID_CANCEL_BUTTON,	DialogGrid::OnCancel)
    END_EVENT_TABLE()
    //////////////////////////////////////////////////////////////////////////
    // DIALOGGRID FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    DialogGrid::DialogGrid( wxWindow* parent): 
        m_visible(wx_static_cast(wxCheckBox*, NULL)),
        m_snapModeChoice(wx_static_cast(wxRadioBox*, NULL)),
        m_size(wx_static_cast(wxTextCtrl*, NULL)),
        wxDialog(parent, wxID_ANY, wxT("Grid settings"), wxDefaultPosition, wxSize(300, 255))
    {

        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Initialize dialog
        Setup();
    }
    //-----------------------------------------------------------------------
    void DialogGrid::Setup()
    {
        // Add the dialog's controls
        wxStaticBox* const mainGroup = new wxStaticBox(this, wxID_ANY, wxT("Update settings:"));
        m_visible = new wxCheckBox(this, wxID_ANY, wxT("grid visible") );

        // Snap options
        const wxString options[] = {wxT("Snap X"), wxT("Snap Y"), wxT("Snap both")};
        m_snapModeChoice = new wxRadioBox(this, wxID_ANY, wxT("Snap mode:"), wxDefaultPosition, wxDefaultSize, 3, options);

        wxStaticText* const size = gsgs_Style().createStaticText(this, wxID_ANY, wxT("Size in pixels:") );
        // Validate for positive number
        m_size = new wxTextCtrl(this, wxID_ANY);

        wxButton* const ok = new wxButton(this, ID_OK_BUTTON, wxT("&OK") );
        wxButton* const cancel = new wxButton(this, ID_CANCEL_BUTTON, wxT("&Cancel") );

        // Mark 'OK' button as the default one (i.e. activated when user presses enter)
        ok->SetDefault();

        // Layout the dialog vertically
        wxBoxSizer* const rootSizer = new wxBoxSizer(wxVERTICAL);

        // Put components -except for the buttons- inside a group
        // NOTE: No need to free the custodial pointer mainGroup 
        // since it will be handled internally by wxWidgets wxStaticBoxSizer::wxStaticBoxSizer()
        wxStaticBoxSizer* const settingsSizer = new wxStaticBoxSizer(mainGroup, wxVERTICAL);

        // These use entire row
        (void)settingsSizer->Add(m_visible, 0, wxALL, BORDER);
        (void)settingsSizer->Add(m_snapModeChoice, 0, wxALL, BORDER);

        // These components share one row
        // NOTE: No need to free the custodial pointers size, visibleSizer 
        // and settingsSizer since they will be handled internally by 
        // wxWidgets wxSizer::Add()
        wxBoxSizer* const visibleSizer = new wxBoxSizer(wxHORIZONTAL);
        (void)visibleSizer->Add(size, 1, wxALL, BORDER); // within their space, they may stretch
        (void)visibleSizer->Add(m_size, 1, wxALL, BORDER);
        (void)settingsSizer->Add(visibleSizer, 0, wxEXPAND | wxALL, BORDER);
        (void)rootSizer->Add(settingsSizer, 1, wxEXPAND | wxALL, BORDER);

        // The buttons share one row as well
        // NOTE: No need to free the custodial pointers ok, cancel and buttonSizer 
        // since they will be handled internally by wxWidgets wxSizer::Add()
        wxBoxSizer* const buttonSizer = new wxBoxSizer(wxHORIZONTAL);
        (void)buttonSizer->Add(ok, 0, wxALL, BORDER);
        (void)buttonSizer->Add(cancel, 0, wxALL, BORDER);
        (void)rootSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, BORDER);

        // Apply root sizer
        // NOTE: No need to free the custodial pointer rootSizer 
        // since it will be handled internally by wxWidgets wxWindowBase::SetSizer()
        SetSizer(rootSizer);

        // Represent current settings
        ShowCurrentSettings();

        // Center dialog
        Centre();
    }
    //------------------------------------------------------------------------
    void DialogGrid::ShowCurrentSettings()
    {
        // Validations
        wxASSERT_MSG(m_visible != NULL, ASSERT_INFO);
        wxASSERT_MSG(m_snapModeChoice, ASSERT_INFO);
        wxASSERT_MSG(m_size, ASSERT_INFO);

        m_visible->SetValue(UILayoutOptions::GetInstancePtr()->IsGridVisible());
        m_snapModeChoice->SetSelection(UILayoutOptions::GetInstancePtr()->GetSnapMode());
        m_size->SetValue(wxString::Format(wxT("%d"), UILayoutOptions::GetInstancePtr()->GetGridSize()));
    }
    //------------------------------------------------------------------------
    void DialogGrid::OnOk( wxCommandEvent& WXUNUSED(event) ) 
    {
        // Validations
        wxASSERT_MSG(m_visible, ASSERT_INFO);
        wxASSERT_MSG(m_snapModeChoice, ASSERT_INFO);
        wxASSERT_MSG(m_size, ASSERT_INFO);

        UILayoutOptions::GetInstancePtr()->SetGridVisible(m_visible->IsChecked());
        UILayoutOptions::GetInstancePtr()->SetSnapMode(m_snapModeChoice->GetSelection());
        int newSize = atoi(wxConvLibc.cWX2MB(m_size->GetValue()));
        if (newSize <= 0)
        {
            // Default
            newSize = DEFAULT_SIZE;
            LogWarning (wxT("Grid size defaulted to %d because of invalid input."), newSize);
        }
        UILayoutOptions::GetInstancePtr()->SetGridSize(newSize);
        (void)Close();
    }
    //------------------------------------------------------------------------
    void DialogGrid::OnCancel( wxCommandEvent& WXUNUSED(event) ) 
    {
        (void)Close();
    }
    //------------------------------------------------------------------------
    
    //////////////////////////////////////////////////////////////////////////
    // EDITORFRAME IMPLEMENTATION
    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_CLASS(UILayoutEditorFrame, wxDocParentFrame)
    //////////////////////////////////////////////////////////////////////////
    // EDITORFRAME EVENT TABLE
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(UILayoutEditorFrame, wxDocParentFrame)

    EVT_SIZE(UILayoutEditorFrame::OnResize)

    EVT_MENU(ID_OPEN_CUSTOM, UILayoutEditorFrame::OnOpenCustom)

    // Dialog options
    EVT_MENU(ID_VIEW_SHOW_HIDE_DIALOG, UILayoutEditorFrame::OnShowOrHideDialog)
    // Resizers
    EVT_MENU(ID_VIEW_640x480, UILayoutEditorFrame::On640x480)
    EVT_MENU(ID_VIEW_800x600, UILayoutEditorFrame::On800x600)
    EVT_MENU(ID_VIEW_1024x786, UILayoutEditorFrame::On1024x768)
    EVT_MENU(ID_VIEW_1280x800, UILayoutEditorFrame::On1280x800)
    //
    EVT_MENU(ID_VIEW_FULLSCREEN, UILayoutEditorFrame::OnFullScreen)
    //
    EVT_UPDATE_UI(wxID_CUT, UILayoutEditorFrame::OnUpdateEditCut)
    EVT_MENU(wxID_CUT, UILayoutEditorFrame::OnEditCut)
    //
    EVT_UPDATE_UI(wxID_COPY, UILayoutEditorFrame::OnUpdateEditCopy)
    EVT_MENU(wxID_COPY, UILayoutEditorFrame::OnEditCopy)
    //
    EVT_UPDATE_UI(wxID_PASTE, UILayoutEditorFrame::OnUpdateEditPaste)
    EVT_MENU(wxID_PASTE, UILayoutEditorFrame::OnEditPaste)
    //
    EVT_UPDATE_UI(wxID_DELETE, UILayoutEditorFrame::OnUpdateEditDelete)
    EVT_MENU(wxID_DELETE, UILayoutEditorFrame::OnEditDelete)
    //
    EVT_UPDATE_UI(wxID_SELECTALL, UILayoutEditorFrame::OnUpdateSelectAll) 
    EVT_MENU(wxID_SELECTALL, UILayoutEditorFrame::OnSelectAll)
    //
    EVT_UPDATE_UI(ID_VIEW_SET_BACKGROUND, UILayoutEditorFrame::OnUpdateSetBackground)
    EVT_MENU(ID_VIEW_SET_BACKGROUND, UILayoutEditorFrame::OnSetBackground)
    //
    EVT_UPDATE_UI(ID_VIEW_SHOW_BACKGROUND, UILayoutEditorFrame::OnUpdateShowBackground)
    EVT_MENU(ID_VIEW_SHOW_BACKGROUND, UILayoutEditorFrame::OnShowBackground)
    EVT_MENU(ID_VIEW_SET_GRID, UILayoutEditorFrame::OnSetGrid)
    EVT_MENU(ID_VIEW_SET_FONT, UILayoutEditorFrame::OnSetFont)
    // Enables/disables entire alignment submenu
    EVT_UPDATE_UI(ID_ALIGN, UILayoutEditorFrame::OnUpdateAlign)
    EVT_MENU(ID_ALIGN_LEFT, UILayoutEditorFrame::OnAlignLeft)
    EVT_MENU(ID_ALIGN_RIGHT, UILayoutEditorFrame::OnAlignRight)
    EVT_MENU(ID_ALIGN_TOP, UILayoutEditorFrame::OnAlignTop)
    EVT_MENU(ID_ALIGN_BOTTOM, UILayoutEditorFrame::OnAlignBottom)
    EVT_MENU(ID_ALIGN_WIDTH, UILayoutEditorFrame::OnAlignWidth)
    EVT_MENU(ID_ALIGN_HEIGHT, UILayoutEditorFrame::OnAlignHeight)

    EVT_MENU(ID_SKINS, UILayoutEditorFrame::OnLayoutSkin)
    EVT_UPDATE_UI(ID_SKINS, UILayoutEditorFrame::OnUpdateLayoutSkin)
    EVT_UPDATE_UI(ID_LOCK_SELECTION, UILayoutEditorFrame::OnUpdateLockSelection)
    EVT_MENU(ID_LOCK_SELECTION, UILayoutEditorFrame::OnLockSelection)

    END_EVENT_TABLE()
    //////////////////////////////////////////////////////////////////////////
    // EDITORFRAME FUNCTIONS
    //////////////////////////////////////////////////////////////////////////
    //------------------------------------------------------------------------
    UILayoutEditorFrame::UILayoutEditorFrame(wxDocManager *manager, wxFrame *frame, wxWindowID id, const wxString& title,
                             const wxPoint& pos, const wxSize& size, const long type):
        m_GUISystem(wx_static_cast(UIManager*, NULL)),
        m_dialogMain(wx_static_cast(DialogMain*, NULL)),
        m_renderCanvas(wx_static_cast(EditorCanvas*, NULL)),
        m_document(wx_static_cast(EditorDocument*, NULL)),
        mEditMenu(wx_static_cast(wxMenu*, NULL)),
        wxDocParentFrame(manager, frame, id, title, pos, size, type)
    {
        // NOTE: Prefer constructor initialization lists (which are 
        // often more efficient) to assignment inside the 
        // constructor. Members in the initialization list should 
        // appear in the order they are declared in the class, as 
        // this is the order they will be initialized (no matter what).
        // See item 4 in 'Effective C++ 3rd Edition' by Scott Meyers

        // Initialize dialog
        Setup();
    }
    //------------------------------------------------------------------------
    UILayoutEditorFrame::~UILayoutEditorFrame()
    {
        // Swallow any exceptions found to prevent the destructor from emitting them.
        // NOTE: An exception should never leave a destructor as it may yield
        // undefined behavior or even lead to program abnormal termination 
        // (see Item 8 in 'Effective C++ 3rd Edition' by Scott Meyers)
        try
        {
            // Windows exception handling specific cleanup
            // NOTE: We need to process this here and not in the LayoutEditor destructor, because
            // by then the Logger singleton no longer exists and it will not be possible to log
            // this info.
            #if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
            // Verify if there's any "eaten" (caught but not processed in a try/catch block) 
            // exception to log
            // NOTE: Please note that "eaten" exceptions might only manifest themselves much later
            // than when they first appeared since they are only detected when a second exception
            // is thrown (or at program shutdown - which is the case here).
            #if CHECK_EATEN_EXCEPTIONS
            if(!ExceptionManager::GetInstancePtr()->IsExceptionProcessed())
                // NOTE: Always use LogSilentError here, because we're exiting the application (perhaps even in an unstable state) and there's no need to bother the user
                LogSilentError(wxT("An \"eaten\" exception (caught but not processed/logged) was found.\n\n\"EATEN\" EXCEPTION INFO:%s"), ExceptionManager::GetInstancePtr()->GetExceptionVEHInfo());
            #endif // CHECK_EATEN_EXCEPTIONS

            // Unregister VEH exception handler
            ExceptionManager::GetInstancePtr()->UnRegisterExceptionHandler();
            #endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

            // Free member pointers
            if(m_dialogMain)
            {
                (void)m_dialogMain->Destroy();
                delete m_dialogMain;
            }
            if(m_renderCanvas)
                delete m_renderCanvas;

            // Zero out the member pointers
            // NOTE: All of these will be freed internally by wxWidgets
            m_document = wx_static_cast(EditorDocument*, NULL);
            mEditMenu = wx_static_cast(wxMenu*, NULL);
        }
        // Add exception to log
        LOG_EXCEPTION_SAFE("UILayoutEditorFrame::~UILayoutEditorFrame", "Failed to destroy editor frame", "Failed to destroy editor frame", "Failed to destroy editor frame");    
    }
    //------------------------------------------------------------------------
    void UILayoutEditorFrame::Setup()
    {
        // Should be enough to fix this bug: http://www.cegui.org.uk/mantis/view.php?id=80
        wxWindowBase::SetSizeHints(320, 200);

        // Add a menu bar, a toolbar and a statusbar
        AttachMenubar();
        AttachToolbar();
        (void)wxFrameBase::CreateStatusBar();

        // Don't do earlier, because we need the toolbar and statusbar in the calculations!
        RestoreIniSize();

        if (m_dialogMain == NULL)
        {
            // Create the main dialog
            // The dialog registers itself to document event so all important
            // changes will automatically be propagated to it.
            m_dialogMain = new DialogMain(this);
        }
    }
    //------------------------------------------------------------------------
    void UILayoutEditorFrame::AttachMenubar()
    {
        // Make a menubar (note that the Edit menu is a member!)
        wxMenu* const fileMenu = new wxMenu;
        mEditMenu = new wxMenu;
        wxMenu* const alignSubmenu = new wxMenu;
        wxMenu* const viewMenu = new wxMenu;
        wxMenu* const helpMenu = new wxMenu;
        wxMenuBar* const menubar = new wxMenuBar;

        // File items

        // Many of these ID's (the ones which start with "wxID") are already mapped
        // internally, so you won't see them in the EVENT_TABLE.
        (void)fileMenu->Append(wxID_NEW, wxT("&New...\tCtrl+N"), wxT("Create a new document."));
        (void)fileMenu->Append(ID_OPEN_CUSTOM, wxT("&Open...\tCtrl+O"), wxT("Open an existing document."));
        (void)fileMenu->Append(wxID_CLOSE, wxT("&Close"), wxT("Close the active document."));
        (void)fileMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"), wxT("Save the active document."));
        (void)fileMenu->Append(wxID_SAVEAS, wxT("Save &As..."), wxT("Save the active document with a new name."));
        (void)fileMenu->AppendSeparator();
        (void)fileMenu->Append(wxID_EXIT, wxT("E&xit\tCtrl+Q"), wxT("Quit the application; prompts to save document."));

        // Edit items
        (void)mEditMenu->Append(wxID_UNDO, wxT("&Undo\tCtrl+Z"), wxT("Undo the last action."));
        (void)mEditMenu->Append(wxID_REDO, wxT("&Redo\tCtrl+Y"), wxT("Redo the previously undone action."));
        (void)mEditMenu->AppendSeparator();
        (void)mEditMenu->Append(wxID_CUT, wxT("&Cut\tCtrl+X"), wxT("Cut the current selection."));
        (void)mEditMenu->Append(wxID_COPY, wxT("&Copy\tCtrl+C"), wxT("Copy the current selection."));
        (void)mEditMenu->Append(wxID_PASTE, wxT("&Paste\tCtrl+V"), wxT("Paste the latest cut or copied selection."));
        (void)mEditMenu->Append(wxID_DELETE, wxT("&Delete\tDel"), wxT("Delete the current selection."));
        (void)mEditMenu->AppendSeparator();
        (void)mEditMenu->Append(wxID_SELECTALL, wxT("Select &All \tCtrl+A"), wxT("Select all windows in the layout."));

        (void)mEditMenu->AppendSeparator();

        // Align items
        // NOTE: No need to free the custodial pointer alignSubmenu since
        // it will be handled internally by wxWidgets wxMenuBase::Append()
        (void)alignSubmenu->Append(ID_ALIGN_LEFT, wxT("&Left"), wxT("Give entire selection the same X1 value."));
        (void)alignSubmenu->Append(ID_ALIGN_RIGHT, wxT("&Right"), wxT("Give entire selection the same X2 value."));
        (void)alignSubmenu->Append(ID_ALIGN_TOP, wxT("&Top"), wxT("Give entire selection the same Y1 value."));
        (void)alignSubmenu->Append(ID_ALIGN_BOTTOM, wxT("&Bottom"), wxT("Give entire selection the same Y2 value."));
        (void)alignSubmenu->AppendSeparator();
        (void)alignSubmenu->Append(ID_ALIGN_WIDTH, wxT("&Width"), wxT("Give entire selection the same width."));
        (void)alignSubmenu->Append(ID_ALIGN_HEIGHT, wxT("&Height"), wxT("Give entire selection the same height."));
        (void)mEditMenu->Append(ID_ALIGN, wxT("Align"), alignSubmenu);

        (void)mEditMenu->Append(ID_SKINS, wxT("Apply Layout Skin..."), wxT("Applies a skin to the entire layout."));

        // Dialog options
        (void)viewMenu->Append(ID_VIEW_SHOW_HIDE_DIALOG, wxT("Toggle dialog"), wxT("Toggles the main dialog through minimize and restore."));
        (void)viewMenu->AppendSeparator();
        // View items
        (void)viewMenu->Append(ID_VIEW_640x480, wxT("Make exactly 640 x 480"), wxT("Convenience resizer for 640 x 480."));
        (void)viewMenu->Append(ID_VIEW_800x600, wxT("Make exactly 800 x 600"), wxT("Convenience resizer for 800 x 600."));
        (void)viewMenu->Append(ID_VIEW_1024x786, wxT("Make exactly 1024 x 768"), wxT("Convenience resizer for 1024 x 768."));
        (void)viewMenu->Append(ID_VIEW_1280x800, wxT("Make exactly 1280 x 800"), wxT("Convenience resizer for 1280 x 800 (wide)."));
        (void)viewMenu->Append(ID_VIEW_FULLSCREEN, wxT("Make fullscreen"), wxT("Convenience resizer to fullscreen."));
        (void)viewMenu->AppendSeparator();
        (void)viewMenu->Append(ID_VIEW_SET_BACKGROUND, wxT("Set background..."));
        (void)viewMenu->AppendCheckItem(ID_VIEW_SHOW_BACKGROUND, wxT("Show background..."));
        (void)viewMenu->AppendSeparator();
        (void)viewMenu->Append(ID_VIEW_SET_GRID, wxT("Set &grid...\tCtrl+G"), wxT("Set grid size and visibility."));
        (void)viewMenu->Append(ID_VIEW_SET_FONT, wxT("Set default &font...\tCtrl+F"), wxT("Set default font from now on."));

        // Help items
        (void)helpMenu->Append(wxID_ABOUT, wxT("&About"), wxT("Display program information, version number and copyright."));

        // Fill the menubar
        // NOTE: No need to free the custodial pointers fileMenu, viewMenu 
        // and helpMenu since they will be handled internally by wxWidgets wxMenuBar::Append()
        (void)menubar->Append(fileMenu, wxT("&File"));
        (void)menubar->Append(mEditMenu, wxT("&Edit"));
        (void)menubar->Append(viewMenu, wxT("&View"));
        (void)menubar->Append(helpMenu, wxT("&Help"));

        // Associate the menu bar with the frame
        // NOTE: No need to free the custodial pointer menubar since
        // it will be handled internally by wxWidgets wxFrameBase::SetMenuBar()
        wxFrameBase::SetMenuBar(menubar);
    }
    //------------------------------------------------------------------------
    void UILayoutEditorFrame::AttachToolbar()
    {
        // Create toolbar with icons-only
        wxToolBar* const toolBar = CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER, wxID_ANY);
        //
        (void)toolBar->AddTool(wxID_NEW, wxBitmap (new_xpm), wxT("New")); 
        (void)toolBar->AddTool(ID_OPEN_CUSTOM, wxBitmap (open_xpm), wxT("Open"));
        (void)toolBar->AddTool(wxID_SAVE, wxBitmap (save_xpm), wxT("Save"));
        (void)toolBar->AddSeparator();
        (void)toolBar->AddTool(wxID_CUT, wxBitmap (cut_xpm), wxT("Cut"));
        (void)toolBar->AddTool(wxID_COPY, wxBitmap (copy_xpm), wxT("Copy"));
        (void)toolBar->AddTool(wxID_PASTE, wxBitmap (paste_xpm), wxT("Paste"));
        (void)toolBar->AddTool(wxID_DELETE, wxBitmap (delete_xpm), wxT("Delete"));
        (void)toolBar->AddSeparator();

        (void)toolBar->AddTool(ID_LOCK_SELECTION, wxBitmap (lock_xpm), wxT("Lock Selection"));
        (void)toolBar->AddSeparator();

        (void)toolBar->AddTool(wxID_ABOUT, wxBitmap (help_xpm), wxT("About"));
        // Don't forget this one!
        (void)toolBar->Realize();

        // Assign toolbar
        // NOTE: No need to free the custodial pointer toolbar since
        // it will be handled internally by wxWidgets wxFrameBase::SetToolBar()
        wxFrameBase::SetToolBar(toolBar);    
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::SetDocument(EditorDocument* document)
    {
        m_document = document;

        // Always set the (new) document
        if (m_dialogMain)
        {
            m_dialogMain->SetDocument (document) ;
        }

        // Tell the document's command processor where it can find the Undo and Redo items to update
        // (enable/disable) accordingly.
        if (m_document)
        {
            m_document->GetCommandProcessor()->SetEditMenu(mEditMenu);
        }
    }

    //------------------------------------------------------------------------
    EditorCanvas *UILayoutEditorFrame::AttachCanvas(wxView *view)
    {
        int width, height;
        GetClientSize(&width, &height);

        // Create the GLCanvas. This approach provides MDI support maybe in the future
        //EditorCanvas* const canvas = new EditorCanvas(view, this, GetClientAreaOrigin(), wxDefaultSize);
        EditorCanvas* const canvas = new EditorCanvas(view, this, wxPoint(0,0), wxDefaultSize);

        // Tell it about the initial size
        //canvas->changeSize(width + BORDER_SIZE, height + BORDER_SIZE);
        canvas->changeSize(width, height);
        canvas->ClearBackground();

        // Create the GUI renderable
        InitializeGUI();

        // Copy into member
        m_renderCanvas = canvas;

        return canvas;
    }

    //------------------------------------------------------------------------
    wxString UILayoutEditorFrame::GetGUIDataRoot()
    {
        UILayoutOptions* const opt =  UILayoutOptions::GetInstancePtr();
        wxString appResourceDir(wxStandardPaths::Get().GetResourcesDir());

        // are data directories are specified in the INI config file?
        wxString anIniPath(opt->GetPathToLookNFeels()); // Just pick one folder to check this.
        if (!anIniPath.empty())
            return wxString();

        // see if there is a 'datafiles' directory in the resources location
        if (wxDir::Exists(appResourceDir + wxT("/datafiles")))
            return appResourceDir + wxT("/datafiles");

        // Can't establish where the files might be; ask the user instead
        // Start at the resources location
        const wxString defaultPath(appResourceDir);
        // Don't allow creating of a new folder.
        wxDirDialog dialog(this, wxT("Select your \"Datafiles\" folder once."),
            defaultPath, (wxDEFAULT_DIALOG_STYLE & ~wxDD_NEW_DIR_BUTTON));

        if (dialog.ShowModal() == wxID_OK)
            return dialog.GetPath();
        else
            return wxString();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::InitializeGUI()
    {
        // Setup support for TGA,JPG and PNG

        m_GUISystem = N_OnlyPtr(UI);

        //GUI::Logger::getSingleton().setLoggingLevel (GUI::Informative);
        InitializePaths();

        // Loads schemes and fonts (no other resources yet).
        LoadData();

        // clearing this queue actually makes sure it's created(!)
        //m_GUIRenderer->getDefaultRenderingRoot().clearGeometry(GUI::RQ_UNDERLAY);

        // subscribe handler to render overlay items
        m_GUISystem->getMainSheet()->
            bind(UISheet::PreRenderEvent, this, &UILayoutEditorFrame::RQHandler);
    }
    //------------------------------------------------------------------------
    void UILayoutEditorFrame::InitializePaths()
    {
        // Get the settings
        UILayoutOptions* const opt =  UILayoutOptions::GetInstancePtr();

        wxString dataRoot(GetGUIDataRoot());
        if (!dataRoot.empty())
        {
            opt->SetPathToConfigs(dataRoot + wxT("/configs/"));
            opt->SetPathToFonts(dataRoot + wxT("/fonts/"));
            opt->SetPathToImagesets(dataRoot + wxT("/imagesets/"));
            opt->SetPathToLookNFeels(dataRoot + wxT("/looknfeel/"));
            opt->SetPathToScripts(dataRoot + wxT("/lua_scripts/"));
            opt->SetPathToSchemes(dataRoot + wxT("/schemes/"));
            opt->SetPathToLayouts(dataRoot + wxT("/layouts/"));
        }	

        // Default to internal defaults
        N_Only(Font).setGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToFonts()), _T("configs")));
        N_Only(PixelBuffer).setGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToImagesets()), _T("imagesets")));
        N_Only(Widget).setGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToLayouts()), _T("layouts")));
        N_Only(Widget).setStyleGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToLookNFeels()), _T("looknfeel")));
        ScriptModule::setGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToScripts()), _T("lua_scripts")));
        N_Only(UI::Scheme).setGroup(N_Only(ResourceScheme).create(0, StrHelper::ToNIIString(opt->GetPathToSchemes()), _T("schemes")));

        // Paths set (also used in case user canceled out the dialog).
        if (opt->GetPathToLookNFeels().empty())	// Just pick one folder to check this.
        {
            LogError (wxT("UILayoutEditorFrame::InitializeGUI - Need to quit: you either canceled the Datafiles browser, or have an incorrect INI file.\nCheck your LayoutEditor.ini file for the 'XXXPaths=' fields."));
            (void)Close(true);    // No "veto"
        }
    }

    //------------------------------------------------------------------------
    bool UILayoutEditorFrame::GetFileNames(const wxString& path, const wxString& wildcard, wxArrayString& filenames) const
    {
        filenames.clear();
        if (!path.IsEmpty())
        {	// Find all layouts
            wxDir::GetAllFiles(path, &filenames, wildcard, wxDIR_FILES);
        }
        return filenames.GetCount() > 0;
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::LoadData() const
    {
        wxArrayString	filenames;
        size_t			index;
        UILayoutOptions* const opt =  UILayoutOptions::GetInstancePtr();

        // Load all ".scheme" files.
        wxString path = opt->GetPathToSchemes();
        if (GetFileNames(path, wxT("*.scheme"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                try
                {
                    // Dalfy: i added the resource folder to avoid defaulting.
                    N_Only(UI::Scheme).create(StrHelper::ToNIIString(wxFileName(filenames[index]).GetFullName()),0);
                }
                catch (const UniqueRepeatException & e)
                {
                    // This exception always seems to occur at the moment (for every datafile structure processing), 
                    // so we're explicitly not showing it to the user (as it gets very annoying). However, we still want
                    // to log the exception info composed of log basic info (description, location, message), line and 
                    // file info (GUI exception specific) and VEH info if under Windows (stack trace, exception code & flags)
                    LogDebug(ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("UILayoutEditorFrame::LoadData"), wxT("Unexpected Error"), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine()));
                }
                // If something goes wrong, show user
                SHOW_EXCEPTION("UILayoutEditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
            }
        }
        else
        {
            LogWarning(wxT("Warning: no schemes were found. Loading of layouts will most likely fail now."));
        }

        // Load all ".font" files (may have already been includes in a scheme file)
        path = opt->GetPathToFonts();
        if (GetFileNames(path, wxT("*.font"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                try
                {
                    // Dalfy: i added the resource folder to avoid defaulting.
                    N_Only(Font).create(StrHelper::ToNIIString(wxFileName(filenames[index]).GetFullName()), 0);
                }
                catch (const UniqueRepeatException&)
                {	// Obvious when already included
                }
                // If something goes wrong, show user
                SHOW_EXCEPTION("UILayoutEditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
            }
        }
        else
        {
            LogWarning(wxT("Warning: no fonts were found. Loading of layouts will most likely fail now."));
        }

        // Load all ".imageset" files (may have already been includes in a scheme file)
        path = opt->GetPathToImagesets();
        if (GetFileNames(path, wxT("*.pixelmap"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                try
                {
                    // Dalfy: i added the resource folder to avoid defaulting.
                    N_Only(PixelBuffer).create(StrHelper::ToNIIString(wxFileName(filenames[index]).GetFullName()));
                }
                catch (const UniqueRepeatException&)
                {	// Obvious when already included
                }
                // If something goes wrong, show user
                SHOW_EXCEPTION("UILayoutEditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
            }
        }
        else
        {
            LogWarning(wxT("Warning: no imagesets were found. Loading of layouts will most likely fail now."));
        }

        // Load all ".looknfeel" files (may have already been includes in a scheme file)
        path = opt->GetPathToLookNFeels();
        if (GetFileNames(path, wxT("*.style"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                try
                {
                    // Dalfy: i added the resource folder to avoid defaulting.
                    N_Only(Widget).loadStyle(StrHelper::ToNIIString(wxFileName(filenames[index]).GetFullName()));
                }
                catch (const UniqueRepeatException &)
                {	// Obvious when already included
                }
                // If something goes wrong, show user
                SHOW_EXCEPTION("UILayoutEditorFrame::LoadData", "Error", "Unexpected standard exception while loading data", "Unexpected non-standard exception while loading data");
            }
        }
        else
        {
            LogWarning(wxT("Warning: no imagesets were found. Loading of layouts will most likely fail now."));
        }
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnResize(wxSizeEvent& WXUNUSED(event)) 
    {
        // We need the client size
        int clientWidth, clientHeight;
        GetClientSize(&clientWidth, &clientHeight);


        if (m_renderCanvas)
        {
            // Apply settings to our GLCanvase
            m_renderCanvas->changeSize(clientWidth, clientHeight);
        }

        if (m_GUISystem)
        {
            // Apply to GUI and store in INI
            m_GUISystem->setViewSize(N_Engine().getView(),
                PlaneSizei(wx_static_cast(float, clientWidth),
                            wx_static_cast(float, clientHeight)));
            UILayoutOptions::GetInstancePtr()->SetCurrentResolution(clientWidth, clientHeight);
        }

        if (m_document)
        {   // Recalculate the resize boxes
            m_document->GetSelection()->Update();

            wx_static_cast(EditorView*, m_document->GetFirstView())->UpdateBackground();
        }
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::RestoreIniSize() 
    {
        // Load the previously stored resolution
        int lastWidth;
        int lastHeight;
        UILayoutOptions::GetInstancePtr()->GetCurrentResolution(&lastWidth, &lastHeight);

        // The OnResize handler will apply the setting to the renderer and INI file
        SetClientSize(lastWidth, lastHeight);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::Refresh(bool eraseBackground, const wxRect* rect)
    {
        if (m_document)
        {   // Recalculate the resize boxes
            m_document->GetSelection()->Update();
        }
        if (m_renderCanvas)
        {
            m_renderCanvas->Refresh(eraseBackground, rect);
            m_renderCanvas->Update();
        }
        // Call superclass
        wxFrame::Refresh(eraseBackground, rect);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnOpenCustom(wxCommandEvent& event)
    {
        wxArrayString	filenames;
        wxArrayString	choices;
        size_t			index;

        // Call helper
        UILayoutOptions* const opt =  UILayoutOptions::GetInstancePtr();
        wxString path = opt->GetPathToLayouts();
        // We accept .layout and .xml
        if (GetFileNames(path, wxT("*.layout"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                choices.Add(wxFileName(filenames[index]).GetFullName());
            }
        }
        if (GetFileNames(path, wxT("*.xml"), filenames))
        {
            for (index=0; index<filenames.GetCount(); index++)
            {
                choices.Add(wxFileName(filenames[index]).GetFullName());
            }
        }
        if (choices.GetCount() > 0)
        {	// Sort, because we appended from two arrays
            choices.Sort();
            wxSingleChoiceDialog chooser(this, wxT("Select from: ") + path, wxT("Please select a layout"), choices);
            if (chooser.ShowModal() == wxID_OK)
            {
                // Cegui knows where to find layouts, so the filename is enough
                wxGetApp().GetDocManager()->CreateDocument(chooser.GetStringSelection(), wxDOC_SILENT);
            }
        }	
        else
        {
            LogWarning(wxT("Warning: no layouts are found in: ") + path);
        }
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnShowOrHideDialog(wxCommandEvent& WXUNUSED(event)) 
    {
        if (m_dialogMain)
        {	// Toggle by minimizing or maximizing the dialog
            m_dialogMain->Iconize(!m_dialogMain->IsIconized());
        }
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::On640x480(wxCommandEvent& WXUNUSED(event)) 
    {
        Resize(640, 480);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::On800x600(wxCommandEvent& WXUNUSED(event)) 
    {
        Resize(800, 600);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::On1024x768(wxCommandEvent& WXUNUSED(event))
    {
        Resize(1024, 786);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::On1280x800(wxCommandEvent& WXUNUSED(event))
    {
        Resize(1280, 800); // Widescreen
    }

    void UILayoutEditorFrame::Resize(int aWidth, int aHeight)
    {
        // The OnResize handler will apply the setting to the renderer and INI file
        SetClientSize(aWidth, aHeight);
        UILayoutEditorFrame::Refresh();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnFullScreen(wxCommandEvent& WXUNUSED(event))
    {
        (void)wxFrame::ShowFullScreen(true);
        UILayoutEditorFrame::Refresh();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateEditCut(wxUpdateUIEvent& event) 
    {
        event.Enable (m_document && m_document->GetSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnEditCut(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        m_document->CutSelection();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateEditCopy(wxUpdateUIEvent& event) 
    {
        event.Enable (m_document && m_document->GetSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnEditCopy(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        m_document->CopySelection();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateEditPaste(wxUpdateUIEvent& event) 
    {
        event.Enable (m_document && m_document->GetCopiedSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnEditPaste(wxCommandEvent& WXUNUSED(event)) 
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        m_document->PasteCopiedSelection();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateEditDelete(wxUpdateUIEvent& event) 
    {	
        event.Enable (m_document && m_document->GetSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnEditDelete(wxCommandEvent& WXUNUSED(event)) 
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        m_document->DeleteSelection() ;
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateSetBackground(wxUpdateUIEvent& event) 
    {
        // Must have a document
        event.Enable (m_document != NULL);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnSetBackground(wxCommandEvent& WXUNUSED(event)) 
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

        // Restore directory before returning otherwise all file-access
        // will happen in this folder, including saving of layouts and such.
        const wxString currentDirectory = wxGetCwd();
        // Make sure to only include extensions, which are supported by GUI!
        wxFileDialog dialog(this, wxT("Open a background image"), currentDirectory, wxT(""), wxT("Layout files (*.tga;*.jpg;*.png)|*.tga;*.jpg;*.png"), wxFD_OPEN);
        if (dialog.ShowModal() == wxID_OK)
        {
            wx_static_cast(EditorView*, m_document->GetFirstView())->SetBackgroundImage(dialog.GetPath());
        }
        (void)wxSetWorkingDirectory(currentDirectory);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateShowBackground(wxUpdateUIEvent& event) 
    {
        // You don't need to have an open document in order to show or hide backgrounds
        //event.Enable (m_renderCanvas && m_renderCanvas->m_background.isVisible()) ;
        event.Check (m_renderCanvas && UILayoutOptions::GetInstancePtr()->IsBackgroundVisible());
        event.Enable (UILayoutOptions::GetInstancePtr()->GetCurrentBackground() != wxT("")) ;
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnShowBackground(wxCommandEvent& event) 
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

        wx_static_cast(EditorView*, m_document->GetFirstView())->SetBackgroundVisibility(event.IsChecked());
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnSetGrid(wxCommandEvent& WXUNUSED(event))
    {
        // Prompts the user with the grid dialog
        // We don't need the return value, since the dialog updates the UILayoutOptions itself
        (void)(new DialogGrid (this))->ShowModal();
        UILayoutEditorFrame::Refresh();
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnSetFont(wxCommandEvent& WXUNUSED(event))
    {
        // Prompts the user with the available fonts to choose one from.
        wxArrayString	fonts;	// Duplicate code from DialogMain, needs refactoring(?)

        FontManager::FontMap::const_iterator f, fend = N_Only(Font).getList().end();
        for(f = N_Only(Font).getList().begin(); f != fend; ++f)
        {
            (void)fonts.Add (StrHelper::ToWXString(f->second->getName()));
        }

        // Found any?
        if (fonts.GetCount() > 0)
        {
            wxSingleChoiceDialog fontChooser (this, wxT("Apply this font to new windows:"), wxT("Set default font"), fonts);
            if (fontChooser.ShowModal() == wxID_OK)
            {
                // Overwrite previous value
                UILayoutOptions::GetInstancePtr()->SetDefaultFont(fontChooser.GetStringSelection());
            }
        }
        else
        {
            LogWarning (wxT("No fonts available to choose from."));
        }
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateAlign(wxUpdateUIEvent& event) 
    {
        event.Enable (m_document && m_document->GetSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignLeft(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignLeft);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignRight(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignRight);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignTop(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignTop);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignBottom(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignBottom);	
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignWidth(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignWidth);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnAlignHeight(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        SelectionMover(m_document->GetSelection()).AlignSelection(SelectionMover::AlignHeight);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateSelectAll(wxUpdateUIEvent& event) 
    {
        event.Enable (m_document != NULL);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
    {
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);
        m_document->SelectAll();
    } 

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnLayoutSkin(wxCommandEvent& WXUNUSED(event))
    {
        // Validations
        wxASSERT_MSG(m_dialogMain != NULL, wxT("No layout is currently loaded!"));

        // Create a dialog that lists the registered Look'N'Feels and that asks the user to choose
        // one of the options. 
        const wxString skin = wxGetSingleChoice(StrHelper::ToWXString(_T("Choose skin that you want to apply to entire layout:")), 
            StrHelper::ToWXString(_T("Skin chooser")), m_dialogMain->GetAvailableLooks());

        // Apply chosen skin to ALL the widgets in the current layout
        if (skin.Length() > 0)
        {
            Ni32 temp;
            StrConv::conv(StrHelper::ToNIIString(skin), temp);
            m_dialogMain->SetWidgetsLook(temp);
        }
    } 

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateLayoutSkin(wxUpdateUIEvent& event) 
    {
        // Only enable the update layout skin when a document is open
        event.Enable (m_document != NULL);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnUpdateLockSelection(wxUpdateUIEvent& event) 
    {
        // Only enable the lock/unlock selection icon when the user has a valid widget selection
        event.Enable (m_document && m_document->GetSelection()->Size() > 0);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::OnLockSelection(wxCommandEvent& WXUNUSED(event))
    {
        // Validations
        wxASSERT_MSG(m_document != NULL, ASSERT_DOCUMENT_MSG);

        // Change the current lock selection status and update the lock/unlock 
        // toolbar icon to reflect this
        m_document->SwitchLockedSelection();
        UpdateLockedSelectionIcon(m_document);
    }

    //------------------------------------------------------------------------
    void UILayoutEditorFrame::UpdateLockedSelectionIcon(const EditorDocument* const document) const
    {
        // Do we have a valid document to analyze?
        if(!document)
            return;

        // Obtain toolbar
        wxToolBar* const toolBar = this->GetToolBar();

        // Is the current user selection locked?
        if(document->GetLockedSelection())
        {
            // Selection locked, so change to locked icon
            toolBar->SetToolNormalBitmap(ID_LOCK_SELECTION, wxBitmap (lock_xpm));
            toolBar->SetToolShortHelp(ID_LOCK_SELECTION, wxT("Unlock Selection"));
        }
        else
        {
            // Selection unlocked, so change to unlocked icon
            toolBar->SetToolNormalBitmap(ID_LOCK_SELECTION, wxBitmap (unlock_xpm));
            toolBar->SetToolShortHelp(ID_LOCK_SELECTION, wxT("Lock Selection"));
        }
    }
    //------------------------------------------------------------------------
    void UILayoutEditorFrame::RQHandler(const EventArgs* args)
    {
        if (static_cast<const GeometryLevelEventArgs*>(args)->mGL != GL_Overlay || !m_document)
            //return false;
            return;
        wx_static_cast(EditorView*, m_document->GetFirstView())->DrawBackground();

        //return true;
    }
    //------------------------------------------------------------------------
}