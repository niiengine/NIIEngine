
#include "gsgsWorkBench.h"
#include "gsgsEditorConfig.h"
#include "gsgsWindowManager.h"
#include "gsgsScenePane.h"
#include "gsgsImageManager.h"
#include "gsgsToolBar.h"
#include "gsgsConfig.h"
#include "gsgsStyleManager.h"
#include "globals.h"
#include <wx/minifram.h>
#include <wx/frame.h>
#include <wx/popupwin.h>
#include <wx/dcbuffer.h>
#include <wx/xrc/xmlres.h>

namespace gsgs
{
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // Gripper
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class Gripper : public wxPanel
    {
    public:
        Gripper(wxWindow* parent)
            : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER)
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            Bind(wxEVT_LEFT_DOWN, &Gripper::OnLeftDown, this);
            Bind(wxEVT_LEFT_UP, &Gripper::OnLeftUp, this);
            Bind(wxEVT_MOTION, &Gripper::OnMotion, this);
            Bind(wxEVT_PAINT, &Gripper::OnPaint, this);
            Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
            wxTheApp->Bind(wxEVT_ACTIVATE_APP, &Gripper::OnAppActivated, this);
            //SetSizeHints(wxNOT_FOUND, 10);
            SetSizeHints(wxNOT_FOUND, 0);
        }

        ~Gripper()
        {
            Unbind(wxEVT_LEFT_DOWN, &Gripper::OnLeftDown, this);
            Unbind(wxEVT_LEFT_UP, &Gripper::OnLeftUp, this);
            Unbind(wxEVT_MOTION, &Gripper::OnMotion, this);
            Unbind(wxEVT_PAINT, &Gripper::OnPaint, this);
            wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &Gripper::OnAppActivated, this);
            // if(HasCapture()) { ReleaseMouse(); }
        }

        void OnAppActivated(wxActivateEvent& event)
        {
            event.Skip();
            //    if(!event.GetActive()) {
            //        // App being de-activated, release the mouse
            //        if(HasCapture()) { ReleaseMouse(); }
            //    }
        }

        void OnLeftDown(wxMouseEvent& event)
        {
            event.Skip();
            //    CaptureMouse();
            //    m_offset.x = event.GetPosition().x - GetPosition().x;
            //    m_offset.y = 0;
        }

        void OnLeftUp(wxMouseEvent& event)
        {
            event.Skip();
            // if(HasCapture()) { ReleaseMouse(); }
        }

        void OnMotion(wxMouseEvent& event)
        {
            event.Skip();
            //    if(HasCapture()) {
            //        wxPoint newPos = event.GetPosition();
            //        newPos.x -= m_offset.x;
            //        newPos.y -= m_offset.y;
            //        GetGrandParent()->Move(newPos);
            //    }
        }

        void OnPaint(wxPaintEvent& event)
        {
            wxAutoBufferedPaintDC dc(this);
            // gsgs_Style().DrawStippleBackground(GetClientRect(), dc);
            dc.SetPen(gsgs_Style().GetPanelBgColour());
            dc.SetBrush(gsgs_Style().GetPanelBgColour());
            dc.DrawRectangle(GetClientRect());
        }
    private:
        wxPoint m_offset;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // DebugToolbar
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class DebugToolbar : public wxPanel
    {
    public:
        DebugToolbar(wxWindow* parent);
        bool Show(bool show = true);
    private:
        ToolBar* m_tb = nullptr;
        Gripper* m_gripper = nullptr;
    };
    //-----------------------------------------------------------------
    DebugToolbar::DebugToolbar(wxWindow* parent)
    {
        Hide();
        if(!wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)) 
        { 
            return; 
        }

        SetSizer(new wxBoxSizer(wxVERTICAL));
        wxPanel* mainPanel = new wxPanel(this);
        mainPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
        m_gripper = new Gripper(mainPanel);
        mainPanel->GetSizer()->Add(m_gripper, 0, wxEXPAND);
        GetSizer()->Add(mainPanel, 1, wxEXPAND);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        int toolSize = options->GetIconsSize();

        // Create the toolbar itself
        //===-----------------------------------------------------------------------------
        // Debugger floating toolbar
        // The rest of the debugger buttons will go to a separate floating toolbar
        //===-----------------------------------------------------------------------------
        ImageManager& bmpLoader = gsgs_Image();
        m_tb = new ToolBar(mainPanel);
        m_tb->SetMiniToolBar(false);
        
        m_tb->AddTool(XRCID("start_debugger"), _("Continue"), bmpLoader.LoadBitmap(wxT("debugger_start"), toolSize),
                      _("Continue"));
        m_tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop"), toolSize),
                      _("Stop debugger"));
        m_tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt"), toolSize),
                      _("Pause debugger"));
        m_tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),
                      bmpLoader.LoadBitmap(wxT("debugger_restart"), toolSize), _("Restart debugger"));
        m_tb->AddTool(XRCID("show_cursor"), _("Show Current Line"),
                      bmpLoader.LoadBitmap(wxT("show_current_line"), toolSize), _("Show Current Line"));
        m_tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap(wxT("step_in"), toolSize), _("Step In"));
        m_tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap(wxT("next"), toolSize), _("Next"));
        m_tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap(wxT("step_out"), toolSize), _("Step Out"));
        m_tb->AddSeparator();
        m_tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"),
                      bmpLoader.LoadBitmap("rewind", toolSize), _("Toggle Rewind Commands"), wxITEM_CHECK);
        m_tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"),
                      bmpLoader.LoadBitmap("record", toolSize), _("Start Reverse Debug Recording"), wxITEM_CHECK);
        m_tb->AddSeparator();
        m_tb->Realize();
        // Let the top level window (i.e. the main frame) process this
        m_tb->Bind(wxEVT_TOOL, [&](wxCommandEvent& event) {
            wxFrame* topLevel = gsgs_Event().TopFrame();
            topLevel->GetEventHandler()->AddPendingEvent(event);
        });
        mainPanel->GetSizer()->Add(m_tb, 0, wxEXPAND);
        // wxSize tbSize = m_tb->GetSize();
        GetSizer()->Fit(this);
    }
    //-----------------------------------------------------------------
    bool DebugToolbar::Show(bool show)
    {
        bool res = wxWindow::Show(show);
        if(show) {
            int x;
            gsgs_Config().Read("DebugToolbar/x", x, wxNOT_FOUND);
            if(x != wxNOT_FOUND) {
                Move(x, 0);
            } else {
                CentreOnParent(wxHORIZONTAL);
            }
        } else {
            gsgs_Config().Write("DebugToolbar/x", GetPosition().x);
        }
        return res;
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // WorkBench
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    WorkBench::WorkBench(wxPanel* parent, WindowManager * mag) :
        mPanel(parent),
        mManager(mag),
        mDebugTool(NULL),
        mMain(NULL),
        mWSPane(NULL),
        mDBPane(NULL),
        mEPane(NULL),
        mSPane(NULL),
        mPSPane(NULL),
        mOPPane(NULL),
        mFullSashPos(800),
        mWorkspaeOpen(false)
    {
        gsgs_Event().Bind(wxEVT_INIT_DONE, &WorkBench::OnInitDone, this);
        gsgs_Event().Bind(wxEVT_DEBUG_STARTED, &WorkBench::OnDebugStarted, this);
        gsgs_Event().Bind(wxEVT_DEBUG_ENDED, &WorkBench::OnDebugEnded, this);
        gsgs_Event().Bind(wxEVT_CMD_CLOSE_WORKSPACE, &WorkBench::OnCloseWorkspace, this);
        //gsgs_Event().Bind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &WorkBench::OnNewWorkspace, this);
        gsgs_Event().Bind(wxEVT_CMD_OPEN_WORKSPACE, &WorkBench::OnOpenWorkspace, this);
        CreateViewImpl();
    }
    //-----------------------------------------------------------------
    WorkBench::~WorkBench()
    {
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &WorkBench::OnInitDone, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_STARTED, &WorkBench::OnDebugStarted, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_ENDED, &WorkBench::OnDebugEnded, this);
        gsgs_Event().Unbind(wxEVT_CMD_CLOSE_WORKSPACE, &WorkBench::OnCloseWorkspace, this);
        //gsgs_Event().Unbind(wxEVT_CMD_CREATE_NEW_WORKSPACE, &WorkBench::OnNewWorkspace, this);
        gsgs_Event().Unbind(wxEVT_CMD_OPEN_WORKSPACE, &WorkBench::OnOpenWorkspace, this);
        wxDELETE(mSPane);
        wxDELETE(mEPane);
    }
    //-----------------------------------------------------------------
    void WorkBench::ShowScenePane(bool show)
    {
        if (show)
        {
            mMain->SplitVertically(mSPane, mEPane, mFullSashPos);
        }
        else
        {
            mFullSashPos = mMain->GetSashPosition();
            mMain->Unsplit(mSPane);
        }
    }
    //-----------------------------------------------------------------
    void WorkBench::CreateViewImpl()
    {
        mWSPane = new WorkspacePane(mPanel, WORKSPACE_VIEW, mManager);
        mManager->AddPane(mWSPane, wxAuiPaneInfo()
            .CaptionVisible(true).Name(mWSPane->GetCaption()).Caption(mWSPane->GetCaption())
            .Left()
            .BestSize(200, wxNOT_FOUND).MinSize(200, wxNOT_FOUND).MaxSize(200, wxNOT_FOUND)
            .Layer(1)
            .Position(0)
            );
        mManager->RegisterDockWindow(XRCID("workspace_pane"), mWSPane->GetCaption());

        mDBPane = new DebuggerPane(mPanel, DEBUGGER_VIEW, mManager);
        mManager->AddPane(mDBPane, wxAuiPaneInfo()
            .CaptionVisible(true).Name(mDBPane->GetCaption()).Caption(mDBPane->GetCaption())
            .Bottom()
            .Layer(1)
            .BestSize(wxNOT_FOUND, 200).MinSize(wxNOT_FOUND, 200).MaxSize(wxNOT_FOUND, 200)
            .Position(0).PaneBorder(false)
            .CloseButton(true)
            .Hide());
        mManager->RegisterDockWindow(XRCID("debugger_pane"), mDBPane->GetCaption());
        ////////////////////////////////
        mMain = new SplitterWindow(mPanel);
        //-----------------------------
        mEPane = new wxPanel(mMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
        wxBoxSizer * edsize = new wxBoxSizer(wxVERTICAL);
        mEPane->SetSizer(edsize);
        gsgs_Style().RegisterWindow(mEPane);
        EditorBar * navbar = new EditorBar(mEPane);
        QuickFindBar * findbar = new QuickFindBar(mEPane);
        mDebugTool = new DebugToolbar(mEPane);
        mEditBook = new Book(mEPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER);

        edsize->Add(navbar, 0, wxEXPAND);
        edsize->Add(mDebugTool, 0, wxALIGN_LEFT);
        edsize->Add(findbar, 0, wxALIGN_RIGHT);
        edsize->Add(mEditBook, 1, wxEXPAND);
        //edsize->Fit(mEPane);

        mEditBook->SetFindBar(findbar);
        mEditBook->SetEditorBar(navbar);
        navbar->Hide();
        findbar->Hide();
        //-----------------------------
        mSPane = new ScenePane(mMain, Scene_VIEW, mEPane);
        mSPane->Hide();
        ///----------------------------
        mMain->Initialize(mEPane);
        ///////////////////////////////
        wxAuiPaneInfo sinfo;
        sinfo.CaptionVisible(true).Name(Main_VIEW).Caption(Main_VIEW).
            CenterPane().Position(0).BestSize(wxNOT_FOUND, wxNOT_FOUND).MinSize(wxNOT_FOUND, wxNOT_FOUND).MaxSize(wxNOT_FOUND, wxNOT_FOUND).
            PaneBorder(false);
        mManager->AddPane(mMain, sinfo);
        mManager->RegisterDockWindow(XRCID("main_pane"), Main_VIEW);

        mPSPane = new PerspectivePane(mPanel, Perspective_VIEW, mManager);
        mManager->AddPane(mPSPane, wxAuiPaneInfo()
            .CaptionVisible(true).Name(mPSPane->GetCaption()).Caption(Perspective_VIEW)
            .Right().BestSize(200, wxNOT_FOUND).MinSize(200, wxNOT_FOUND).MaxSize(200, wxNOT_FOUND)
            .Layer(1)
            .Position(0).PaneBorder(false)
            .Hide()
            .CloseButton(true));
        mManager->RegisterDockWindow(XRCID("perspective_pane"), mPSPane->GetCaption());

        mOPPane = new OutputPane(mPanel, OUTPUT_VIEW);
        mManager->AddPane(mOPPane, wxAuiPaneInfo()
            .CaptionVisible(true).Name(mOPPane->GetCaption()).Caption(OUTPUT_VIEW)
            .Bottom()
            .Layer(1)
            .Position(0).PaneBorder(false)
            .Hide()
            .BestSize(wxNOT_FOUND, 300).MinSize(wxNOT_FOUND, 60).MaxSize(wxNOT_FOUND, 300));
        mManager->RegisterDockWindow(XRCID("output_pane"), mOPPane->GetCaption());

        mEditBook->ShowNavBar(gsgs_EditorConfig().GetInteger(wxT("ShowNavBar"), 0) ? true : false);
    }
    //-----------------------------------------------------------------
    void WorkBench::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        if (mWorkspaeOpen)
        {
            ShowScenePane(true);
        }
    }
    //-----------------------------------------------------------------
    void WorkBench::OnDebugStarted(DebugEvent& event)
    {
        event.Skip();
        mDebugTool->Show();
        mDebugTool->GetParent()->GetSizer()->Layout();
    }
    //-----------------------------------------------------------------
    void WorkBench::OnDebugEnded(DebugEvent& event)
    {
        event.Skip();
        mDebugTool->Hide();
        mDebugTool->GetParent()->GetSizer()->Layout();
    }
    //-----------------------------------------------------------------
    void WorkBench::OnNewWorkspace(Event& event)
    {
        event.Skip();
    }
    //-----------------------------------------------------------------
    void WorkBench::OnOpenWorkspace(Event& event)
    {
        event.Skip();
        ShowScenePane(true);
        mWorkspaeOpen = true;
    }
    //-----------------------------------------------------------------
    void WorkBench::OnCloseWorkspace(Event& event)
    {
        event.Skip();
        ShowScenePane(false);
        mWorkspaeOpen = false;
    }
    //-----------------------------------------------------------------
}