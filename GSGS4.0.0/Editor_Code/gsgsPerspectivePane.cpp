
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include <wx/stattext.h>
#include "gsgsPerspectivePane.h"
#include "gsgsCommonHandler.h"
#include "gsgsEditor.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsWorkspacePane.h"
#include "gsgsFrame.h"
#include "globals.h"
#include "gsgsManager.h"
#include "gsgsParseWorker.h"
#include "gsgsLogManager.h"

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

namespace gsgs
{
    //-------------------------------------------------------------------------------
    class ListenerPropertyGrid : public wxPropertyGrid
    {
    public:
        ListenerPropertyGrid(wxWindow *parent, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxPG_DEFAULT_STYLE,
            const wxString& name = wxPropertyGridNameStr) :
            wxPropertyGrid(parent, id, pos, size, style, name)
        {
            mSpacePanel = static_cast<SpacePanel *>(parent);
        }

        void SetScrollbar(int orient, int pos, int thumbVisible,
            int range, bool refresh = true)
        {
            wxPropertyGrid::SetScrollbar(orient, pos, thumbVisible, 0, false);
            if (mMuteScrollDisable)
            {
                if (wxVERTICAL == orient)
                    mSpacePanel->UpdateVScrollBar(pos, thumbVisible, range, thumbVisible, true, false);
                else
                    mSpacePanel->UpdateHScrollBar(pos, thumbVisible, range, thumbVisible, true, false);
            }
        }

        void SetScrollPos(int orient, int pos, bool refresh = true)
        {
            wxPropertyGrid::SetScrollPos(orient, pos, false);
            if (mMuteScrollDisable)
            {
                if (wxVERTICAL == orient)
                    mSpacePanel->SetVThumbPosition(pos, refresh);
                else
                    mSpacePanel->SetHThumbPosition(pos, refresh);
            }
        }
        void setLastScrollX(int x)              { mLastScrollX = x; }
        int getLastScrollX() const              { return mLastScrollX; }
        void setLastScrollY(int y)              { mLastScrollY = y; }
        int getLastScrollY() const              { return mLastScrollY; }
        void setMuteScrollDisable(bool set)     { mMuteScrollDisable = set; }
        bool getMuteScrollDisable() const       { return mMuteScrollDisable; }
    protected:
        SpacePanel * mSpacePanel;
        bool mMuteScrollDisable;
        int mLastScrollX = 0;
        int mLastScrollY = 0;
    };
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // SpacePanel
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    SpacePanel::SpacePanel(wxWindow * parent, const wxString& caption, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style) :
        ScrolledWindow(parent, id, pos, size, style | wxNO_BORDER | wxCLIP_CHILDREN),
        m_caption(caption)
    {
        SetAutoLayout(false);
        CreateViewImpl();
    }
    //-------------------------------------------------------------------------------
    SpacePanel::~SpacePanel()
    {
    }
    //----------------------------------------------------------------------------------
    bool SpacePanel::Create(wxWindow* parent, const wxString& caption, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
    {
        if (ScrolledWindow::Create(parent, id, pos, size, style))
        {
            m_caption = caption;
            CreateViewImpl();
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    void SpacePanel::OnSize(wxSizeEvent& event)
    {
        if (mClient)
        {
            wxSize csize = event.GetSize();
            wxSize tsize = event.GetSize();
            wxSize bsize = mClient->GetVirtualSize();
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
            Refresh();
        }
        else
        {
            DoPositionHScrollbar();
            DoPositionVScrollbar();
        }
    }
    //-------------------------------------------------------------------------------
    void SpacePanel::OnClientSize(wxSizeEvent& event)
    {
        wxSize csize = event.GetSize();
        wxSize bsize = mClient->GetVirtualSize();
        UpdateVScrollBar(GetVThumbPosition(), csize.y, bsize.y, csize.y, false, true);
        UpdateHScrollBar(GetHThumbPosition(), csize.x, bsize.x, csize.x, false, true);
        event.Skip();
    }
    //-------------------------------------------------------------------------------
    void SpacePanel::ScrollToRow(int firstLine)
    {
        ListenerPropertyGrid * tg = static_cast<ListenerPropertyGrid *>(mGrid);
        tg->setMuteScrollDisable(false);
        mGrid->Scroll(tg->getLastScrollX(), firstLine);
        tg->setMuteScrollDisable(true);
        Refresh();
    }
    //--------------------------------------------------------------------------------
    void SpacePanel::ScollToColumn(int firstColumn)
    {
        ListenerPropertyGrid * tg = static_cast<ListenerPropertyGrid *>(mGrid);
        tg->setMuteScrollDisable(false);
        mGrid->Scroll(firstColumn, tg->getLastScrollY());
        tg->setMuteScrollDisable(true);
        Refresh();
    }
    //-------------------------------------------------------------------------------
    bool SpacePanel::SetBackgroundColour(const wxColour& colour)
    {
        GetHScrollBar()->SetColour(colour);
        GetVScrollBar()->SetColour(colour);
        return ScrolledWindow::SetBackgroundColour(colour);
    }
    //----------------------------------------------------------------------------
    void SpacePanel::CreateViewImpl()
    {
        mGrid = new ListenerPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxNO_BORDER | wxPG_SPLITTER_AUTO_CENTER);
        mGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
       
        wxPGProperty * group1 = mGrid->Append(new wxPropertyCategory("Local Space"));
        mGrid->AppendIn(group1, new wxFloatProperty("Position X", "lposition x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Y", "lposition y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Z", "lposition z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation X", "lorientation x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Y", "lorientation y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Z", "lorientation z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale X", "lscale x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Y", "lscale y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Z", "lscale z", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Parient Space"));
        mGrid->AppendIn(group1, new wxFloatProperty("Position X", "pposition x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Y", "pposition y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Z", "pposition z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation X", "porientation x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Y", "porientation y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Z", "porientation z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale X", "pscale x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Y", "pscale y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Z", "pscale z", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Root Space"));
        mGrid->AppendIn(group1, new wxFloatProperty("Position X", "sposition x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Y", "sposition y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Position Z", "sposition z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation X", "sorientation x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Y", "sorientation y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Orientation Z", "sorientation z", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale X", "sscale x", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Y", "sscale y", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Scale Z", "sscale z", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Extended Option"));
        mGrid->AppendIn(group1, new wxBoolProperty("Identity Projection", wxPG_LABEL, false));
        mGrid->AppendIn(group1, new wxBoolProperty("Identity View", wxPG_LABEL, false));
        mGrid->AppendIn(group1, new wxBoolProperty("Identity Model", wxPG_LABEL, false));
        mGrid->AppendIn(group1, new wxBoolProperty("Cast Shadow", wxPG_LABEL, false));
        mGrid->AppendIn(group1, new wxBoolProperty("Receive Shadow", wxPG_LABEL, false));
        mGrid->AppendIn(group1, new wxBoolProperty("Debug Model", wxPG_LABEL, false));
        mGrid->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);
        SetVColour(gsgs_Style().GetAppTextColour());
        SetHColour(gsgs_Style().GetAppTextColour());
        setClient(mGrid);
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // MaterialPanel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    MaterialPanel::MaterialPanel(wxWindow* parent, const wxString& caption)
    {
        SpacePanel::Create(parent, caption);
    }
    //----------------------------------------------------------------------------
    MaterialPanel::~MaterialPanel()
    {

    }
    //----------------------------------------------------------------------------
    void MaterialPanel::CreateViewImpl()
    {
        mGrid = new ListenerPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxNO_BORDER | wxPG_SPLITTER_AUTO_CENTER);
        mGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);

        wxPGProperty * group1 = mGrid->Append(new wxPropertyCategory("Colour"));
        mGrid->AppendIn(group1, new wxStringProperty("Ambient", "Ambient", "255.255.255"));
        mGrid->AppendIn(group1, new wxStringProperty("Diffuse", "Diffuse", "255.255.255"));
        mGrid->AppendIn(group1, new wxStringProperty("Specular", "Specular", "255.255.255"));
        mGrid->AppendIn(group1, new wxStringProperty("Emissive", "Emissive", "255.255.255"));
        mGrid->AppendIn(group1, new wxStringProperty("Shininess", "Shininess", "255.255.255"));
        mGrid->AppendIn(group1, new wxStringProperty("Tracking", "Tracking", "255.255.255"));
        group1 = mGrid->Append(new wxPropertyCategory("Alpha"));
        mGrid->AppendIn(group1, new wxBoolProperty("Coverage", "Coverage", true));
        mGrid->AppendIn(group1, new wxBoolProperty("PrecentCoverage", "PrecentCoverage", true));
        mGrid->AppendIn(group1, new wxStringProperty("Reject", "Reject", "CmpMode"));
        mGrid->AppendIn(group1, new wxFloatProperty("RejectValue", "RejectValue", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Blend"));
        mGrid->AppendIn(group1, new wxStringProperty("RGB Mode", "RGBMode", "FrameBlendMode"));
        mGrid->AppendIn(group1, new wxStringProperty("Aplha Mode", "AplhaMode", "FrameBlendMode"));
        mGrid->AppendIn(group1, new wxStringProperty("RGB Factor", "RGBFactor", "ColourFactor"));
        mGrid->AppendIn(group1, new wxStringProperty("Aplha Factor", "AplhaFactor", "ColourFactor"));
        mGrid->AppendIn(group1, new wxStringProperty("RGB Blend", "RGBBlend", "ColourBlendMode"));
        mGrid->AppendIn(group1, new wxStringProperty("Aplha Blend", "Aplha Blend", "ColourBlendMode"));
        group1 = mGrid->Append(new wxPropertyCategory("Depth"));
        mGrid->AppendIn(group1, new wxBoolProperty("Check", "Check", false));
        mGrid->AppendIn(group1, new wxStringProperty("Compare", "Compare", "CmpMode"));
        mGrid->AppendIn(group1, new wxBoolProperty("Write", "Write", false));
        mGrid->AppendIn(group1, new wxFloatProperty("BiasConstant", "BiasConstant", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("BiasConstantFactor", "BiasConstantFactor", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("BiasSlopeScale", "BiasSlopeScale", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Stencil"));
        mGrid->AppendIn(group1, new wxBoolProperty("Both Side", "BothSide", false));
        mGrid->AppendIn(group1, new wxUIntProperty("Front Test Mask", "FrontTestMask", 0));
        mGrid->AppendIn(group1, new wxUIntProperty("Back Test Mask", "BackTestMask", 0.0f));
        mGrid->AppendIn(group1, new wxUIntProperty("Front Compress Mask", "FrontCompressMask", 0));
        mGrid->AppendIn(group1, new wxUIntProperty("Back Compress Mask", "BackCompressMask", 0));
        mGrid->AppendIn(group1, new wxUIntProperty("Front Compress Value", "FrontCompressValue", 0));
        mGrid->AppendIn(group1, new wxUIntProperty("Back Compress Value", "BackCompressValue", 0));
        mGrid->AppendIn(group1, new wxStringProperty("Front Stencil Fail Op", "FrontStencilFailOp", "StencilOpType"));
        mGrid->AppendIn(group1, new wxStringProperty("Back Stencil Fail Op", "BackStencilFailOp", "StencilOpType"));
        mGrid->AppendIn(group1, new wxStringProperty("Front Depth Fail Op", "FrontDepthFailOp", "StencilOpType"));
        mGrid->AppendIn(group1, new wxStringProperty("Back Depth Fail Op", "BackDepthFailOp", "StencilOpType"));
        mGrid->AppendIn(group1, new wxStringProperty("Front Test Pass Op", "FrontTestPassOp", "StencilOpType"));
        mGrid->AppendIn(group1, new wxStringProperty("Back Test Pass Op", "BackTestPassOp", "StencilOpType"));
        group1 = mGrid->Append(new wxPropertyCategory("Fog"));
        mGrid->AppendIn(group1, new wxStringProperty("Mode", "FogMode", "FogMode"));
        mGrid->AppendIn(group1, new wxStringProperty("Colour", "FogColour", "255.255.255"));
        mGrid->AppendIn(group1, new wxFloatProperty("Begin", "FogBegin", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("End", "FogEnd", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Density", "Density", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Point"));
        mGrid->AppendIn(group1, new wxFloatProperty("Size", "Size", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Min Size", "MinSize", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Max Size", "MaxSize", 0.0f));
        mGrid->AppendIn(group1, new wxBoolProperty("Sprites", "Sprites", false));
        mGrid->AppendIn(group1, new wxBoolProperty("Attenuate", "Attenuate", false));
        mGrid->AppendIn(group1, new wxFloatProperty("Constant", "Constant", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Linear", "Linear", 0.0f));
        mGrid->AppendIn(group1, new wxFloatProperty("Quadratic", "Quadratic", 0.0f));
        group1 = mGrid->Append(new wxPropertyCategory("Texture"));
        mGrid->AppendIn(group1, new wxStringProperty("Texture0", "Texture0", ""));
        mGrid->AppendIn(group1, new wxStringProperty("Texture1", "Texture1", ""));
        mGrid->AppendIn(group1, new wxStringProperty("Texture2", "Texture2", ""));
        mGrid->AppendIn(group1, new wxStringProperty("Texture3", "Texture3", ""));
        group1 = mGrid->Append(new wxPropertyCategory("Program"));
        mGrid->AppendIn(group1, new wxStringProperty("ST_VS", "ST_VS", ""));
        mGrid->AppendIn(group1, new wxStringProperty("ST_FS", "ST_FS", ""));
        mGrid->AppendIn(group1, new wxStringProperty("ST_GS", "ST_GS", ""));
        SetVColour(gsgs_Style().GetAppTextColour());
        SetHColour(gsgs_Style().GetAppTextColour());
        setClient(mGrid);
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // EventPanel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    EventPanel::EventPanel(wxWindow* parent, const wxString& caption)
    {
        SpacePanel::Create(parent, caption);
    }
    //----------------------------------------------------------------------------
    EventPanel::~EventPanel()
    {

    }
    //----------------------------------------------------------------------------
    void EventPanel::CreateViewImpl()
    {
        mGrid = new ListenerPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxNO_BORDER | wxPG_SPLITTER_AUTO_CENTER);
        mGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);

        wxPGProperty * group1 = mGrid->Append(new wxPropertyCategory("Parient Event"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 1", "pEvent 1", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 2", "pEvent 2", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 3", "pEvent 3", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 4", "pEvent 4", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 5", "pEvent 5", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 6", "pEvent 6", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 7", "pEvent 7", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 8", "pEvent 8", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 9", "pEvent 9", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 10", "pEvent 10", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 11", "pEvent 11", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 12", "pEvent 12", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 13", "pEvent 13", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 14", "pEvent 14", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Event 15", "pEvent 15", "None"));
        group1 = mGrid->Append(new wxPropertyCategory("Event"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 1", "Event 1", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 2", "Event 2", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 3", "Event 3", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 4", "Event 4", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 5", "Event 5", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 6", "Event 6", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 7", "Event 7", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 8", "Event 8", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 9", "Event 9", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 10", "Event 10", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 11", "Event 11", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 12", "Event 12", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 13", "Event 13", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 14", "Event 14", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Event 15", "Event 15", "None"));
        SetVColour(gsgs_Style().GetAppTextColour());
        SetHColour(gsgs_Style().GetAppTextColour());
        setClient(mGrid);
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // PropertyPanel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    PropertyPanel::PropertyPanel(wxWindow* parent, const wxString& caption)
    {
        SpacePanel::Create(parent, caption);
    }
    //----------------------------------------------------------------------------
    PropertyPanel::~PropertyPanel()
    {

    }
    //----------------------------------------------------------------------------
    void PropertyPanel::CreateViewImpl()
    {
        mGrid = new ListenerPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxNO_BORDER | wxPG_SPLITTER_AUTO_CENTER);
        mGrid->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);

        wxPGProperty * group1 = mGrid->Append(new wxPropertyCategory("Parient Property"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 1", "pProperty 1", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 2", "pProperty 2", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 3", "pProperty 3", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 4", "pProperty 4", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 5", "pProperty 5", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 6", "pProperty 6", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 7", "pProperty 7", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 8", "pProperty 8", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 9", "pProperty 9", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 10", "pProperty 10", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 11", "pProperty 11", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 12", "pProperty 12", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 13", "pProperty 13", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 14", "pProperty 14", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Parent Property 15", "pProperty 15", "None"));
        group1 = mGrid->Append(new wxPropertyCategory("Property"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 1", "Property 1", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 2", "Property 2", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 3", "Property 3", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 4", "Property 4", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 5", "Property 5", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 6", "Property 6", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 7", "Property 7", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 8", "Property 8", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 9", "Property 9", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 10", "Property 10", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 11", "Property 11", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 12", "Property 12", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 13", "Property 13", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 14", "Property 14", "None"));
        mGrid->AppendIn(group1, new wxStringProperty("Property 15", "Property 15", "None"));
        SetVColour(gsgs_Style().GetAppTextColour());
        SetHColour(gsgs_Style().GetAppTextColour());
        setClient(mGrid);
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // PerspectivePane
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    PerspectivePane::PerspectivePane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr)
        : wxPanel(parent)
        , m_caption(caption)
        , m_mgr(mgr)
    {
        wxWindowUpdateLocker temp(this);
        CreateViewImpl();
        gsgs_Event().Bind(wxEVT_INIT_DONE, &PerspectivePane::OnInitDone, this);
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &PerspectivePane::OnSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_SHOW_Perspective_TAB, &PerspectivePane::OnTogglePerspectiveTab, this);
    }
    //----------------------------------------------------------------------------
    PerspectivePane::~PerspectivePane()
    {
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &PerspectivePane::OnInitDone, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &PerspectivePane::OnSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_SHOW_Perspective_TAB, &PerspectivePane::OnTogglePerspectiveTab, this);
    }
    //----------------------------------------------------------------------------
    #define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false
    //----------------------------------------------------------------------------
    void PerspectivePane::CreateViewImpl()
    {
        wxBoxSizer * mainSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(mainSizer);
        long style = kNotebook_UnderlineActiveTab;
    #if USE_AUI_NOTEBOOK
        style |= wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_SPLIT;
    #else
    // add notebook for tabs
    #ifdef __WXOSX__
        style |= (kNotebook_Default | kNotebook_AllowDnD | kNotebook_LeftTabs);
    #else
        style |= (kNotebook_Default | kNotebook_AllowDnD);
    #endif
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetWorkspaceTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;

        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) 
        {
            style &= ~kNotebook_LightTabs;
            style |= kNotebook_DarkTabs;
        }

        if(gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs()) 
        { 
            style |= kNotebook_MouseScrollSwitchTabs; 
        }

    #endif

        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, style);
        m_book->Bind(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, &PerspectivePane::OnWorkspaceBookFileListMenu, this);

    #if !USE_AUI_NOTEBOOK
        m_book->SetArt(GetNotebookRenderer());
    #endif

        // Calculate the widest tab (the one with the 'Perspective' label)
        int xx, yy;
        wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        wxWindow::GetTextExtent(_("Perspective"), &xx, &yy, NULL, NULL, &fnt);

        mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

        // create tabs (possibly detached)
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject(wxT("DetachedPanesList"), &dpi);

        wxArrayString detachedPanes;
        detachedPanes = dpi.GetPanes();

        IManager* mgr = PluginManager::Get();

        wxString name = _("Space");
        if(IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_Pane1 = new SpacePanel(cp, name);
            cp->SetChildNoReparent(m_Pane1);
        } 
        else 
        {
            m_Pane1 = new SpacePanel(m_book, name);
            m_book->AddPage(m_Pane1, name, false);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_Pane1)));
        mgr->AddPerspectiveTab(name);

        name = _("Material");
        if (IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_Pane2 = new MaterialPanel(cp, name);
            cp->SetChildNoReparent(m_Pane2);
        }
        else 
        {
            m_Pane2 = new MaterialPanel(m_book, name);
            m_book->AddPage(m_Pane2, name, false);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_Pane2)));
        mgr->AddPerspectiveTab(name);

        name = _("Event");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_Pane3 = new EventPanel(cp, name);
            cp->SetChildNoReparent(m_Pane3);
        }
        else
        {
            m_Pane3 = new EventPanel(m_book, name);
            m_book->AddPage(m_Pane3, name, false);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_Pane3)));
        mgr->AddPerspectiveTab(name);

        name = _("Property");
        if (IS_DETACHED(name))
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, wxNullBitmap, wxSize(200, 200));
            m_Pane4 = new PropertyPanel(cp, name);
            cp->SetChildNoReparent(m_Pane4);
        }
        else
        {
            m_Pane4 = new PropertyPanel(m_book, name);
            m_book->AddPage(m_Pane4, name, false);
        }
        m_tabs.insert(std::make_pair(name, Tab(name, m_Pane4)));
        mgr->AddPerspectiveTab(name);

        if(m_book->GetPageCount() > 0) 
        { 
            m_book->SetSelection((size_t)0); 
        }
        //m_mgr->Update();
    }
    //----------------------------------------------------------------------------
    typedef struct 
    {
        wxString text;
        wxWindow* win;
        wxBitmap bmp;
    } tagTabInfo;
    //----------------------------------------------------------------------------
    void PerspectivePane::DoShowTab(bool show, const wxString& title)
    {
        if(!show) 
        {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) 
            {
                if(m_book->GetPageText(i) == title) 
                {
                    // we've got a match
                    m_book->RemovePage(i);
                    wxWindow* win = DoGetControlByName(title);
                    if(win) { win->Show(false); }
                    break;
                }
            }
        } 
        else 
        {
            for(size_t i = 0; i < m_book->GetPageCount(); i++) 
            {
                if(m_book->GetPageText(i) == title) 
                {
                    // requested to add a page which already exists
                    return;
                }
            }

            // Fetch the list of detached panes
            // If the mainframe is NULL, read the
            // list from the disk, otherwise use the
            // dockable pane menu

            // Read it from the disk
            DetachedPanesInfo dpi;
            gsgs_EditorConfig().ReadObject(wxT("DetachedPanesList"), &dpi);
            wxArrayString detachedPanes;
            detachedPanes = dpi.GetPanes();

            if(IS_DETACHED(title)) return;

            wxWindow * win = DoGetControlByName(title);
            if(win) 
            {
                win->Show(true);
                m_book->InsertPage(0, win, title, true);
            }
        }
    }
    //----------------------------------------------------------------------------
    wxWindow* PerspectivePane::DoGetControlByName(const wxString& title)
    {
        if(title == _("Space"))
            return m_Pane1;
        else if(title == _("Material"))
            return m_Pane2;
        else if (title == _("Event"))
            return m_Pane3;
        else if (title == _("Property"))
            return m_Pane4;
        return NULL;
    }
    //----------------------------------------------------------------------------
    bool PerspectivePane::IsTabVisible(int flag)
    {
        wxWindow* win(NULL);
        wxString title;

        switch(flag) 
        {
        case View_Show_Space_Tab:
            title = _("Space");
            win = DoGetControlByName(_("Space"));
            break;
        case View_Show_Material_Tab:
            title = _("Material");
            win = DoGetControlByName(_("Material"));
            break;
        case View_Show_Event_Tab:
            title = _("Event");
            win = DoGetControlByName(_("Event"));
            break;
        case View_Show_Property_Tab:
            title = _("Property");
            win = DoGetControlByName(_("Property"));
            break;
        }

        if(!win || title.IsEmpty()) 
            return false;

        // if the control exists in the notebook, return true
        for(size_t i = 0; i < m_book->GetPageCount(); ++i) 
        {
            if(m_book->GetPageText(i) == title) 
            { 
                return true; 
            }
        }
        return win && win->IsShown();
    }
    //----------------------------------------------------------------------------
    void PerspectivePane::OnInitDone(wxCommandEvent& event)
    {
        event.Skip();
        //m_captionEnabler.Initialize(this, m_caption, gsgs_Frame()->GetAuiManager());
    }
    //----------------------------------------------------------------------------
    void PerspectivePane::SelectTab(const wxString& tabTitle)
    {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) 
        {
            if(m_book->GetPageText(i) == tabTitle) 
            {
                // requested to add a page which already exists
                m_book->SetSelection(i);
            }
        }
    }
    //----------------------------------------------------------------------------
    void PerspectivePane::OnSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        long style = 0;
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetWorkspaceTabsDirection();
        if (tempdir == wxBOTTOM)
            style = kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style = kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style = kNotebook_LeftTabs;
    #if !USE_AUI_NOTEBOOK
        m_book->SetArt(GetNotebookRenderer());
    #endif
        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs | kNotebook_BottomTabs | style);
        } else {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs | kNotebook_BottomTabs | style);
        }
    }
    //----------------------------------------------------------------------------
    void PerspectivePane::OnTogglePerspectiveTab(Event& event)
    {
        // Handle the core tabs
        if(m_tabs.count(event.GetString()) == 0) 
        {
            event.Skip();
            return;
        }

        const Tab& t = m_tabs.find(event.GetString())->second;
        if(event.IsSelected()) 
        {
            // Insert the page
            int where = TabTogglerHelper::IsTabInNotebook(GetNotebook(), t.m_label);
            if(where == wxNOT_FOUND) {
                GetNotebook()->AddPage(t.m_window, t.m_label, true, t.m_bmp);
            } else {
                GetNotebook()->SetSelection(where);
            }
        } 
        else 
        {
            // hide the tab
            int where = GetNotebook()->GetPageIndex(t.m_label);
            if(where != wxNOT_FOUND) 
            { 
                GetNotebook()->RemovePage(where); 
            }
        }
    }
    //----------------------------------------------------------------------------
    TabViewPtr_t PerspectivePane::GetNotebookRenderer() { return TabView::CreateRenderer(m_book->GetStyle()); }
    //----------------------------------------------------------------------------
    void PerspectivePane::OnWorkspaceBookFileListMenu(ContextMenuEvent& event)
    {
        wxMenu* menu = event.GetMenu();
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject("DetachedPanesList", &dpi);

        wxMenu* hiddenTabsMenu = new wxMenu();
        const wxArrayString& tabs = GetManager()->GetPerspectiveTabs();
        for(size_t i = 0; i < tabs.size(); ++i) 
        {
            const wxString& label = tabs.Item(i);
            if((m_book->GetPageIndex(label) != wxNOT_FOUND)) 
            {
                continue;
            }
            
            if(hiddenTabsMenu->GetMenuItemCount() == 0) 
            {
                // we are adding the first menu item
                menu->AppendSeparator();
            }
            
            int tabId = wxXmlResource::GetXRCID(wxString() << "perspective_tab_" << label);
            hiddenTabsMenu->Append(tabId, label);

            // If the tab is detached, disable it's menu entry
            if(dpi.GetPanes().Index(label) != wxNOT_FOUND) { hiddenTabsMenu->Enable(tabId, false); }

            // Bind the event
            hiddenTabsMenu->Bind(wxEVT_MENU,
                                 // Use lambda by value here so we make a copy
                                 [=](wxCommandEvent& e) {
                                     Event eventShow(wxEVT_SHOW_Perspective_TAB);
                                     eventShow.SetSelected(true).SetString(label);
                                     gsgs_Event().AddPendingEvent(eventShow);
                                 },
                                 tabId);
        }
        if(hiddenTabsMenu->GetMenuItemCount() == 0) 
        {
            wxDELETE(hiddenTabsMenu);
        } 
        else 
        {
            menu->AppendSubMenu(hiddenTabsMenu, _("Hidden Tabs"), _("Hidden Tabs"));
        }
    }
    //----------------------------------------------------------------------------
}