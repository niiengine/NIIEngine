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

#include <wx/app.h>
#include <wx/menu.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include <wx/stattext.h>
#include "gsgsEditor.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsWorkspacePane.h"
#include "gsgsParseWorker.h"
#include "gsgsLogManager.h"
#include "gsgsScenePane.h"
#include "gsgsFrame.h"
#include "NiiPath.h"
#include "gsgsFileView.h"
#include "gsgsManager.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// SceneFrameRuler
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
class SceneFrameRuler : public wxPanel
{
public:
    SceneFrameRuler(wxWindow * parent, bool hor = true)
        :wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
        mHORIZONTAL(hor)
    {
        wxWindowUpdateLocker llt(this);
        SetAutoLayout(false);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
        Bind(wxEVT_PAINT, &SceneFrameRuler::OnPaint, this);

        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        if (mHORIZONTAL)
        {
            SetSize(-1, SB_WIDTH);
        }
        else
        {
            SetSize(SB_WIDTH, -1);
        }
    }
    ~SceneFrameRuler()
    {
        Unbind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
        Unbind(wxEVT_PAINT, &SceneFrameRuler::OnPaint, this);
    }

    void OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC pdc(this);
        wxGCDC dc(pdc);

        wxRect rect = GetClientRect();

        dc.SetBrush(gsgs_Style().GetAppBgColour());
        dc.SetPen(gsgs_Style().GetAppBgColour());
        dc.DrawRectangle(rect);
        dc.SetBrush(gsgs_Style().GetAppTextColour());
        dc.SetPen(gsgs_Style().GetAppTextColour());
        bool shortline = false;
        if (mHORIZONTAL)
        {
            //int cnt = rect.width - 11;
            int cnt = rect.width - 10;
            dc.DrawLine(0, rect.height - 1, cnt, rect.height -1);
            //dc.DrawLine(0, rect.height, cnt, rect.height);
            for (int i = 0; i < cnt; i+= 6)
            {
                if (i % 20 == 0)
                    dc.DrawLine(i, 1, i, rect.height);
                else
                {
                    if (shortline)
                    {
                        dc.DrawLine(i, rect.height - rect.height / 4 - 1, i, rect.height);
                        //dc.DrawLine(i, rect.height - rect.height / 4, i, rect.height);
                        shortline = false;
                    }
                    else
                    {
                        dc.DrawLine(i, rect.height - rect.height / 2, i, rect.height);
                        shortline = true;
                    }
                }
            }
        }
        else
        {
            //int cnt = rect.height - 11;
            int cnt = rect.height - 10;
            dc.DrawLine(rect.width - 1, 0, rect.width - 1, cnt);
            //dc.DrawLine(rect.width, 0, rect.width, cnt);
            for (int i = 0; i < cnt; i+= 6)
            {
                if (i % 20 == 0)
                    dc.DrawLine(1, i, rect.width, i);
                else
                {
                    if (shortline)
                    {
                        dc.DrawLine(rect.width - rect.width / 4 - 1, i, rect.width, i);
                        //dc.DrawLine(rect.width - rect.width / 4, i, rect.width, i);
                        shortline = false;
                    }
                    else
                    {
                        dc.DrawLine(rect.width - rect.width / 2, i, rect.width, i);
                        shortline = true;
                    }
                }
            }
        }
    }
protected:
    bool mHORIZONTAL;
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// SceneViewPanel
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
SceneViewPanel::SceneViewPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
    ScrolledWindow(parent, id, pos, size, style | wxNO_BORDER | wxCLIP_CHILDREN),
    mView(0)
{
    SetBackgroundColour(gsgs_Style().GetAppBgColour());
    SetAutoLayout(false);
    mScene = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
    setClient(mScene, false);
}
//--------------------------------------------------------------------------------
SceneViewPanel::~SceneViewPanel()
{
}
//--------------------------------------------------------------------------------
void SceneViewPanel::setView(ViewWindow * view) 
{ 
    mView = view; 
    RenderSys * sys = N_EnginePtr()->getRender();
    sys->addConfig(N_PropertyWindow_wxWidgetMoveX, _T("wxWidgetMoveX"), _T("0"));
    sys->addConfig(N_PropertyWindow_wxWidgetMoveY, _T("wxWidgetMoveY"), _T("0"));
}
//--------------------------------------------------------------------------------
void SceneViewPanel::ScrollToRow(int firstLine)
{ 
    if (mView)
    {
        RenderSys * sys = N_EnginePtr()->getRender();
        sys->setConfig(_T("wxWidgetMoveY"), N_conv(Ni32(GetVThumbPosition())));
        sys->setConfig(_T("wxWidgetMoveX"), N_conv(Ni32(GetHThumbPosition())));
        wxScrollEvent e(wxEVT_SCROLL_CHANGED);
        e.SetEventObject(this);
        e.SetPosition(0);
        GetEventHandler()->ProcessEvent(e);
        Refresh();
    }
}
//--------------------------------------------------------------------------------
void SceneViewPanel::ScollToColumn(int firstColumn)
{ 
    if (mView)
    {
        RenderSys * sys = N_EnginePtr()->getRender();
        sys->setConfig(_T("wxWidgetMoveY"), N_conv(Ni32(GetVThumbPosition())));
        sys->setConfig(_T("wxWidgetMoveX"), N_conv(Ni32(GetHThumbPosition())));
        wxScrollEvent e(wxEVT_SCROLL_CHANGED);
        e.SetEventObject(this);
        e.SetPosition(0);
        GetEventHandler()->ProcessEvent(e);
        Refresh();
    }
}
//--------------------------------------------------------------------------------
void SceneViewPanel::initView(wxBoxSizer * target)
{
    wxFlexGridSizer * sizer1 = new wxFlexGridSizer(2);
    sizer1->AddGrowableCol(1); // 让第二列大小可变
    sizer1->AddGrowableRow(1); // 让第二行大小可变
    target->Add(sizer1, 1, wxEXPAND, 0);

    mVRuler = new SceneFrameRuler(GetParent(), false);
    mHRuler = new SceneFrameRuler(GetParent(), true);

    mOpen = new Button(GetParent(), wxID_ANY, _T(""), wxDefaultPosition, wxSize(SB_WIDTH, SB_WIDTH), wxNO_BORDER);
    sizer1->Add(mOpen, 1, wxEXPAND);
    sizer1->Add(mHRuler, 1, wxEXPAND);
    sizer1->Add(mVRuler, 1, wxEXPAND);
    sizer1->Add(this, 1, wxEXPAND);
}
//--------------------------------------------------------------------------------
void SceneViewPanel::OnSize(wxSizeEvent& event)
{
    DoSizeRuler();
    if (mView)
    {
        wxSize csize = event.GetSize();
        wxSize tsize = event.GetSize();
        wxSize bsize(mView->getWidth(), mView->getHeight());
        if (tsize.x < bsize.x)
        {
            csize.y = csize.y - SB_WIDTH;

            if (csize.y < bsize.y)
                csize.x = csize.x - SB_WIDTH;
        }
        else if (tsize.y < bsize.y)
        {
            csize.x = csize.x - SB_WIDTH;

            if (csize.x < bsize.x)
                csize.y = csize.y - SB_WIDTH;
        }

        if (csize.y < bsize.y)
        {
            wxWindowUpdateLocker templ(m_vsb);
            m_vsb->SetSize(tsize.x - SB_WIDTH, 0, SB_WIDTH, csize.y);
        }
        if (csize.x < bsize.x)
        {
            wxWindowUpdateLocker templ(m_hsb);
            m_hsb->SetSize(0, tsize.y - SB_WIDTH, csize.x, SB_WIDTH);
        }

        wxWindowUpdateLocker clock(mClient);
        mClient->SetSize(csize);

        UpdateVScrollBar(GetVThumbPosition(), csize.GetHeight(), mView->getHeight(), csize.GetHeight(), false, false);
        UpdateHScrollBar(GetHThumbPosition(), csize.GetWidth(), mView->getWidth(), csize.GetWidth(), false, true);
    }
    mScene->Refresh();
}
//--------------------------------------------------------------------------------
void SceneViewPanel::DoSizeRuler()
{
    wxRect clientRect = GetClientRect();
    //wxSize vsbSize = mVRuler->GetSize();

    wxWindowUpdateLocker llt1(mVRuler);
    mVRuler->SetSize(SB_WIDTH, clientRect.GetHeight());

    //wxSize hsbSize = mHRuler->GetSize();

    wxWindowUpdateLocker llt2(mHRuler);
    mHRuler->SetSize(clientRect.GetWidth(), SB_WIDTH);
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// ScenePane
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
ScenePane::ScenePane(wxWindow* parent, const wxString& caption, wxPanel* editor)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SIMPLE)
    , m_caption(caption)
    , mEditor(editor),
    mWindow(0),
    mFullXThumbPos(0),
    mMidXThumbPos(0),
    mXThumbPos(0),
    mFullYThumbPos(0),
    mMidYThumbPos(0),
    mYThumbPos(0),
    mMidSashPos(800),
    mSashPos(800),
    mLastRClick(true)
{
    CreateViewImpl();
    gsgs_Event().Bind(wxEVT_INIT_DONE, &ScenePane::OnInitDone, this);
    mScrollWindow->GetButton()->Bind(wxEVT_LEFT_DOWN, &ScenePane::OnLeftClick, this);
    mScrollWindow->GetButton()->Bind(wxEVT_RIGHT_DOWN, &ScenePane::OnLeftClick, this);
    SetBackgroundColour(gsgs_Style().GetAppBgColour());
    mRoot = 0;
    mResourcePath = _T("");
    mConfigPath = mResourcePath;
}
//--------------------------------------------------------------------------------
ScenePane::~ScenePane()
{
    gsgs_Event().Unbind(wxEVT_INIT_DONE, &ScenePane::OnInitDone, this);
    mScrollWindow->GetButton()->Unbind(wxEVT_LEFT_UP, &ScenePane::OnLeftClick, this);
    mScrollWindow->GetButton()->Unbind(wxEVT_LEFT_DCLICK, &ScenePane::OnLeftClick, this);
    //wxFileName::Rmdir(wxString(Path::GetTempDir().c_str()), wxPATH_RMDIR_RECURSIVE);
    if (mRoot)
        N_delete mRoot;
}
//--------------------------------------------------------------------------------
void ScenePane::go(void)
{
    if (!setup())
        return;

    if (mWindow)
    {
        mScrollWindow->setView(mWindow);
    }
    SendSizeEvent();
}
//--------------------------------------------------------------------------------
bool ScenePane::setup(void)
{
    String pluginsPath = mResourcePath + "plugins.cfg";
    mRoot = N_new Engine(_T("niiengine"), _T("plugins_niiengine"), _T("GSGS"));

    if (!configure())
        return false;

    chooseSceneManager();
    createCamera();
    createViewports();
    // Set default mipmap level (NB some APIs ignore this)
    N_Only(Texture).setMipMapCount(5);

    N_Only(ResourceScheme).setupAll(0, 0);

    return true;

}
//--------------------------------------------------------------------------------
bool ScenePane::configure(void)
{
    // Show the configuration dialog and init the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if (mRoot->config(true))
    {
        // If returned true, user clicked OK so init
        // Here we choose to let the system create a default rendering window by passing 'true'
        PropertyData temp;
        temp.addData(N_PropertyWindow_wxWidgetPanel, mScrollWindow->getSceneFrame());
        mWindow = mRoot->init(Engine::RM_Custom, true, &temp);

        Refresh();
        return true;
    }
    else
    {
        return false;
    }
}
//--------------------------------------------------------------------------------
void ScenePane::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera(0);
    // Position it at 500 in Z direction
    mCamera->setPos(Vector3f(0, 0, 500));
    // Look back along -Z
    mCamera->lookAt(Vector3f(0, 0, -300));
    mCamera->setNearClipDistance(5);

}
//--------------------------------------------------------------------------------
void ScenePane::createViewports(void)
{
    // Create one viewport, entire window
    Viewport* vp = mWindow->createViewport(0, mCamera);
    vp->setBgColour(NII::Colour(0, 0, 0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(NIIf(vp->getPixelWidth()) / NIIf(vp->getPixelHeight()));
}
//--------------------------------------------------------------------------------
void ScenePane::CreateViewImpl()
{
    wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    wxBoxSizer* boxSizer635 = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(boxSizer635, 0, wxEXPAND, 0);
    wxArrayString items8;
    items8.Add(wxT("Window : 1"));
    items8.Add(wxT("Window : 2"));
    items8.Add(wxT("Window : 3"));
    mWindowChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items8, wxBU_EXACTFIT);
    boxSizer635->Add(mWindowChoice, 0, wxSHRINK, 0);
    wxArrayString items;
    items.Add(wxT("Viewport : 1"));
    items.Add(wxT("Viewport : 2"));
    items.Add(wxT("Viewport : 3"));
    mViewportChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items, wxBU_EXACTFIT);
    boxSizer635->Add(mViewportChoice, 0, wxSHRINK, 0);
    wxArrayString items2;
    items2.Add(wxT("Camera : 1"));
    items2.Add(wxT("Camera : 2"));
    items2.Add(wxT("Camera : 3"));
    mCameraChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items2, wxBU_EXACTFIT);
    boxSizer635->Add(mCameraChoice, 0, wxSHRINK, 0);
    wxArrayString items5;
    items5.Add(wxT("Action : 1"));
    items5.Add(wxT("Action : 2"));
    items5.Add(wxT("Action : 3"));
    mActionChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items5, wxBU_EXACTFIT);
    boxSizer635->Add(mActionChoice, 0, wxSHRINK, 0);
    wxArrayString items6;
    items6.Add(wxT("Space : 1"));
    items6.Add(wxT("Space : 2"));
    items6.Add(wxT("Space : 3"));
    mSpaceChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items6, wxBU_EXACTFIT);
    boxSizer635->Add(mSpaceChoice, 0, wxSHRINK, 0);
    wxArrayString items7;
    items7.Add(wxT("Node : 1"));
    items7.Add(wxT("Node : 2"));
    items7.Add(wxT("Node : 3"));
    mNodeChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items7, wxBU_EXACTFIT);
    boxSizer635->Add(mNodeChoice, 0, wxSHRINK, 0);
    boxSizer635->AddSpacer(20);
    wxArrayString items3;
    items3.Add(wxT("Object : 1"));
    items3.Add(wxT("Object : 2"));
    items3.Add(wxT("Object : 3"));
    mObjectChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items3, wxBU_EXACTFIT);
    boxSizer635->Add(mObjectChoice, 0, wxSHRINK, 0);
    wxArrayString items4;
    items4.Add(wxT("Script : 1"));
    items4.Add(wxT("Script : 2"));
    items4.Add(wxT("Script : 3"));
    mScriptChoice = new Choice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, items4, wxBU_EXACTFIT);
    boxSizer635->Add(mScriptChoice, 0, wxSHRINK, 0);
    Button * tbutton = new DefaultButton(this, wxID_ANY, _("Action"), wxDefaultPosition, mScriptChoice->GetSize());
    boxSizer635->Add(tbutton, 1, wxSHRINK, 0);

    mScrollWindow = new SceneViewPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    mScrollWindow->initView(mainSizer);
    if (GetSizer()) 
    {
        GetSizer()->Fit(this);
        Layout();
    }
}
// -------------------------------------------------------------------------------
void ScenePane::OnLeftClick(wxMouseEvent& event)
{
    event.Skip();

    if (gsgs_Manager()->IsShowAllPanes())
    {
        mYThumbPos = mScrollWindow->GetVThumbPosition();
        mXThumbPos = mScrollWindow->GetHThumbPosition();
        mSashPos = static_cast<SplitterWindow *>(GetParent())->GetSashPosition();
    }
    else if (mLastRClick)
    {
        mFullYThumbPos = mScrollWindow->GetVThumbPosition();
        mFullXThumbPos = mScrollWindow->GetHThumbPosition();
        //mFullSashPos = static_cast<SplitterWindow *>(GetParent())->GetSashPosition();
    }
    else
    {
        mMidYThumbPos = mScrollWindow->GetVThumbPosition();
        mMidXThumbPos = mScrollWindow->GetHThumbPosition();
        mMidSashPos = static_cast<SplitterWindow *>(GetParent())->GetSashPosition();
    }
    if (gsgs_Manager()->TogglePanes())
    {
        SplitterWindow * swindow = static_cast<SplitterWindow *>(GetParent());
        if (mLastRClick)
        {
            swindow->SplitVertically(this, mEditor, mSashPos);
            mEditor->Show();
            mLastRClick = false;
        }
        else
        {
            swindow->SetSashPosition(mSashPos);
        }
        mScrollWindow->SetVThumbPosition(mYThumbPos, false);
        mScrollWindow->SetHThumbPosition(mXThumbPos);
    }
    else
    {
        mLastRClick = event.RightDown();
        SplitterWindow * swindow = static_cast<SplitterWindow *>(GetParent());
        if (mLastRClick)
        {
            swindow->Unsplit(mEditor);
            mScrollWindow->SetVThumbPosition(mFullYThumbPos, false);
            mScrollWindow->SetHThumbPosition(mFullXThumbPos);
        }
        else
        {
            swindow->SetSashPosition(mMidSashPos);
            mScrollWindow->SetVThumbPosition(mMidYThumbPos, false);
            mScrollWindow->SetHThumbPosition(mMidXThumbPos);
        }
    }
}
//--------------------------------------------------------------------------------
void ScenePane::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    //m_captionEnabler.Initialize(this, m_caption, gsgs_Frame()->GetAuiManager());
    go();
}
//--------------------------------------------------------------------------------