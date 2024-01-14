
#include "gsgsToolBar.h"
#include "gsgsStyleManager.h"
#include "gsgsConfig.h"
#include "gsgsImageManager.h"
#include "gsgsEditorConfig.h"

#include "globals.h"
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

namespace gsgs
{
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // ConfigurationPanel
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    ConfigurationPanel::ConfigurationPanel(wxWindow* parent, wxWindowID winid, const wxPoint& pos,
        const wxSize& size, long style)
        : wxPanel(parent, winid, pos, size, style)
    {
        wxWindowUpdateLocker ttl(this);
        SetSizer(new wxBoxSizer(wxHORIZONTAL));
        m_cfgChoice = new DefaultChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, {}, wxBU_EXACTFIT);
        GetSizer()->Add(m_cfgChoice, 0, wxSTRETCH_NOT);
        m_cfgChoice->SetText("Configure");

        m_machineChoice = new DefaultChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, {}, wxBU_EXACTFIT);
        GetSizer()->Add(m_machineChoice, 0, wxSTRETCH_NOT);
        m_machineChoice->SetText("Machine");

        m_strategyChoice = new DefaultChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, {}, wxBU_EXACTFIT);
        GetSizer()->Add(m_strategyChoice, 1, wxSTRETCH_NOT);
        m_strategyChoice->SetText("Strategy");

        m_cfgChoice->Bind(wxEVT_CHOICE, &ConfigurationPanel::OnConfigChoice, this);
        m_machineChoice->Bind(wxEVT_CHOICE, &ConfigurationPanel::OnMachineChoice, this);
        m_strategyChoice->Bind(wxEVT_CHOICE, &ConfigurationPanel::OnStrategyChoice, this);
        //m_cfgChoice->Bind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //m_machineChoice->Bind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //m_strategyChoice->Bind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //Bind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        gsgs_Style().RegisterWindow(this);
    }
    //-----------------------------------------------------------------
    ConfigurationPanel::~ConfigurationPanel()
    {
        m_cfgChoice->Unbind(wxEVT_CHOICE, &ConfigurationPanel::OnConfigChoice, this);
        m_machineChoice->Unbind(wxEVT_CHOICE, &ConfigurationPanel::OnMachineChoice, this);
        m_strategyChoice->Unbind(wxEVT_CHOICE, &ConfigurationPanel::OnStrategyChoice, this);
        //m_cfgChoice->Unbind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //m_machineChoice->Unbind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //m_strategyChoice->Unbind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
        //Unbind(wxEVT_SIZE, &ConfigurationPanel::OnSize, this);
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::Update(const wxArrayString& projects, const wxArrayString& cfg)
    {
        m_projects = projects;
        m_configurations = cfg;
        m_configurations.push_back(OPEN_CONFIG_MGR_STR);
        m_cfgChoice->Set(cfg);
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::SetActiveConfiguration(const wxString& activeConfiguration)
    {
        this->m_activeConfiguration = activeConfiguration;
        //m_cfgChoice->SetStringSelection(m_activeConfiguration); // this will ensure that the checkbox is placed correctly
        // override the text to include the active project name
        if(m_activeProject.IsEmpty())
            m_cfgChoice->SetStringSelection(m_activeConfiguration);
        else
        {
            m_cfgChoice->SetStringSelection(m_activeConfiguration);
            m_cfgChoice->SetText(m_activeProject + " | " + m_activeConfiguration);
        }
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::SetActiveProject(const wxString& activeProject)
    {
        this->m_activeProject = activeProject;
        if (m_activeProject.IsEmpty())
            m_cfgChoice->SetStringSelection(m_activeConfiguration);
        else
        {
            m_cfgChoice->SetStringSelection(m_activeConfiguration);
            m_cfgChoice->SetText(m_activeProject + " | " + m_activeConfiguration);
        }
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::OnConfigChoice(wxCommandEvent& event)
    {
        int where = event.GetSelection();
        if (where != wxNOT_FOUND) {
            wxString selectedString = m_cfgChoice->GetString(where);
            //if (selectedString != OPEN_CONFIG_MGR_STR) { SetActiveConfiguration(selectedString); }

            // Fire 'config-changed' event
            Event changeEvent(wxEVT_WORKSPACE_BUILD_CONFIG_CHANGED);
            changeEvent.SetString(selectedString);
            gsgs_Event().AddPendingEvent(changeEvent);
        }
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::OnMachineChoice(wxCommandEvent& event)
    {

    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::OnStrategyChoice(wxCommandEvent& event)
    {

    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::OnSize(wxSizeEvent& event)
    {
        //event.Skip();
        wxSize cacl = m_cfgChoice->GetSize() + m_machineChoice->GetSize() + m_strategyChoice->GetSize();
        wxWindowUpdateLocker ttl(this);
        SetSize(cacl.GetWidth(), 20);
        Refresh();
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::Clear() 
    {
    }
    //-----------------------------------------------------------------
    void ConfigurationPanel::SetConfigurations(const wxArrayString& cfg)
    {
        m_configurations = cfg;
        m_configurations.push_back(OPEN_CONFIG_MGR_STR);
        m_cfgChoice->Set(m_configurations);
    }
    //-----------------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_TOOLBAR_CUSTOMISE, wxCommandEvent);
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // CustomiseToolBarDlg
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    CustomiseToolBarDlg::CustomiseToolBarDlg(wxWindow* parent, ToolBar* tb, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style):
        wxDialog(parent, id, title, pos, size, style),
        m_buttons(tb->GetButtons())
    {
        wxWindowUpdateLocker templ(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        m_dvListCtrlItems = new ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxDV_VERT_RULES | wxDV_ROW_LINES | wxDV_SINGLE);

        wxWindowUpdateLocker templ2(m_dvListCtrlItems);

        boxSizer2->Add(m_dvListCtrlItems, 1, wxALL | wxEXPAND, 5);

        m_dvListCtrlItems->AppendIconTextColumn(_(""), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrlItems->AppendIconTextColumn(_("Item"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT,
                                                wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrlItems->AppendTextColumn(_("Description"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrlItems->SetBitmaps(&gsgs_Image().GetBitmaps());
        m_stdBtnSizer4 = new DialogButtonSizer();

        boxSizer2->Add(m_stdBtnSizer4, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        m_button6 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_button6->SetDefault();
        m_stdBtnSizer4->AddButton(m_button6);

        m_button8 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer4->AddButton(m_button8);
        m_stdBtnSizer4->Realize();

        if(GetParent()) {
            CentreOnParent(wxBOTH);
        } else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_button6->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CustomiseToolBarDlg::OnOK), NULL,
                           this);
                           
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        //const wxBitmap& bmp = gsgs_Image().LoadBitmap("cog", options->GetIconsSize());
        //m_dvListCtrlItems->SetRowHeight(bmp.GetScaledHeight() + 8);

        for(size_t i = 0; i < m_buttons.size(); ++i) {
            ToolBarButton* button = m_buttons[i];
            wxVector<wxVariant> cols;
            cols.push_back(wxVariant(!button->IsHidden()));
            if (button->IsSpacer()) {
            //    cols.push_back(::MakeBitmapIndexText(_("Spacer"), button->GetBmpIdx()));
            } 
            else if (button->IsSeparator())
            {
            //    cols.push_back(::MakeBitmapIndexText(_("Separator"), button->GetBmpIdx()));
            }
            else
            {
                cols.push_back(::MakeBitmapIndexText(button->GetLabel(), button->GetBmpIdx()));
                cols.push_back(wxVariant(button->GetLabel()));
                m_dvListCtrlItems->AppendItem(cols, (wxUIntPtr)button);
            }
        }
    }
    //-----------------------------------------------------------------------------------------
    CustomiseToolBarDlg::~CustomiseToolBarDlg()
    {
        m_button6->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CustomiseToolBarDlg::OnOK), NULL,
                              this);
    }
    //-----------------------------------------------------------------------------------------
    void CustomiseToolBarDlg::OnOK(wxCommandEvent& event)
    {
        event.Skip();
        for(int i = 0; i < m_dvListCtrlItems->GetItemCount(); ++i) {
            wxVariant val;
            //m_dvListCtrlItems->GetValue(val, i, 0);
            //m_dvListCtrlItems->IsItemChecked()
            wxDataViewItem item = m_dvListCtrlItems->RowToItem(i);
            ToolBarButton* button = reinterpret_cast<ToolBarButton*>(m_dvListCtrlItems->GetItemData(item));
            //button->Show(val.GetBool());
            button->Show(m_dvListCtrlItems->IsItemChecked(item));
        }
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarButton
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarButton::ToolBarButton(ToolBar* parent, wxWindowID id, int bmp,
        const wxString& label, size_t flags)
        : m_toolbar(parent)
        , m_id(id)
        , m_bmpidx(bmp)
        , m_label(label)
        , m_flags(flags)
        , m_renderFlags(0)
        , m_menu(nullptr)
    {
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton::~ToolBarButton() { wxDELETE(m_menu); }
    //-----------------------------------------------------------------------------------------
    void ToolBarButton::Render(wxDC& dc, const wxRect& rect)
    {
        m_buttonRect = rect;
        const Colour& colours = gsgs_Style().GetColours();
        wxColour textColour;
        wxColour buttonColour;

#if defined(__WXMSW__) || defined(__WXOSX__)
        wxColour bgHighlightColour("rgb(153, 209, 255)");
#else
        wxColour bgHighlightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        if (Colour::IsDark(bgHighlightColour)) { bgHighlightColour = bgHighlightColour.ChangeLightness(140); }
#endif
        wxColour penColour = bgHighlightColour;

        const wxColour bgColour = DefaultHTMColour;
        /*{
            wxColour b = wxColour("#00cfff");
            gcdc.SetPen(b);
            gcdc.SetBrush(b);
            gcdc.DrawRoundedRectangle(button->GetButtonRect(), 2.0);
        }*/
        if (IsEnabled() && (IsPressed() || IsChecked())) {
            wxColour pressBgColour = bgColour.ChangeLightness(70);
            wxRect highlightRect = m_buttonRect;
            //highlightRect.Inflate(1);

            penColour = pressBgColour;
            dc.SetBrush(pressBgColour);
            dc.SetPen(penColour);
            dc.DrawRoundedRectangle(highlightRect, 3.0);
            //dc.DrawRectangle(highlightRect);
            textColour = colours.GetSelItemTextColour();
            buttonColour = colours.GetSelbuttonColour();

        }
        else if (IsEnabled() && IsHover()) {
            wxColour hoverColour = bgColour;
            penColour = bgColour;
            wxRect highlightRect = m_buttonRect;
            //highlightRect.Inflate(1);
            dc.SetBrush(hoverColour);
            dc.SetPen(penColour);
            dc.DrawRoundedRectangle(highlightRect, 3.0);
            //dc.DrawRectangle(highlightRect);
            textColour = colours.GetSelItemTextColour();
            buttonColour = colours.GetSelbuttonColour();

        }
        else if (!IsEnabled())
        {
            // A disabled button
            textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
            buttonColour = textColour;
        }
        else {
            // Default
            if (Colour::IsDark(bgColour)) {
                buttonColour = colours.GetSelbuttonColour();
            }
            else {
                buttonColour = colours.GetButtonColour();
            }
            textColour = colours.GetItemTextColour();
        }
        wxCoord xx = m_buttonRect.GetX();
        wxCoord yy = 0;
        xx += m_toolbar->GetXSpacer();

        if (m_bmpidx != wxNOT_FOUND) {
            wxBitmap bmp;
            if (IsEnabled())
            {
                bmp = gsgs_Image().GetBitmap(m_bmpidx);
            }
            else
            { 
                bmp = gsgs_Style().CreateDisabledBitmap(gsgs_Image().GetBitmap(m_bmpidx)); 
            }
            yy = (m_buttonRect.GetHeight() - bmp.GetScaledHeight()) / 2 + m_buttonRect.GetY();
            dc.DrawBitmap(bmp, wxPoint(xx, yy));
            xx += bmp.GetScaledWidth();
            xx += m_toolbar->GetXSpacer();
        }

        if (!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
            dc.SetTextForeground(textColour);
            wxSize sz = dc.GetTextExtent(m_label);
            yy = (m_buttonRect.GetHeight() - sz.GetHeight()) / 2 + m_buttonRect.GetY();
            dc.DrawText(m_label, wxPoint(xx, yy));
            xx += sz.GetWidth();
            xx += m_toolbar->GetXSpacer();
        }

        if (IsMenuButton()) {
            // draw a drop down menu
            m_dropDownArrowRect =
                wxRect(xx, m_buttonRect.GetY(), /*(2 * m_toolbar->GetXSpacer()) +*/ TOOL_BAR_DROPDOWN_ARROW_SIZE,
                    m_buttonRect.GetHeight());
            if ((IsPressed() || IsHover()) && IsEnabled()) {
                dc.DrawLine(wxPoint(xx, m_buttonRect.GetY() + 2),
                    wxPoint(xx, m_buttonRect.GetY() + m_buttonRect.GetHeight() - 2));
            }
            xx += m_toolbar->GetXSpacer();
            if(IsEnabled())
                gsgs_Style().DrawDropDownArrow(m_toolbar, dc, m_dropDownArrowRect, false, 1, gsgs_Style().GetActiveMarkerColour());
            else
                gsgs_Style().DrawDropDownArrow(m_toolbar, dc, m_dropDownArrowRect, false, 1, gsgs_Style().GetAppActiveTextColour());
            xx += TOOL_BAR_DROPDOWN_ARROW_SIZE;
            xx += m_toolbar->GetXSpacer();
        }
        else
        {
            m_dropDownArrowRect = wxRect();
        }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarButton::Show(bool b) 
    { 
        EnableFlag(kHidden, !b); 
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // DefaultToolBarButton
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    DefaultToolBarButton::DefaultToolBarButton(ToolBar * parent, wxWindowID winid, int bmp, const wxString & label) :
        ToolBarButton(parent, winid, bmp, label)
    {
    }
    //-----------------------------------------------------------------------------------------
    DefaultToolBarButton::~DefaultToolBarButton() {}
    //-----------------------------------------------------------------------------------------
    wxSize DefaultToolBarButton::CalculateSize(wxDC& dc) const
    {
        wxSize sz;
        sz.y = 0;
        sz.x += m_toolbar->GetXSpacer();
        if (m_bmpidx != wxNOT_FOUND) {
            const wxBitmap & tempb = gsgs_Image().GetBitmap(m_bmpidx);
            sz.x += tempb.GetScaledWidth();
            sz.x += m_toolbar->GetXSpacer();

            int height = tempb.GetScaledHeight();
            sz.y = wxMax(sz.GetHeight(), height);
        }
        if (!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
            wxSize textSize = dc.GetTextExtent(m_label);
            sz.x += textSize.GetWidth();
            sz.x += m_toolbar->GetXSpacer();

            int height = textSize.GetHeight();
            sz.y = wxMax(sz.GetHeight(), height);
        }
        sz.y += (2 * m_toolbar->GetYSpacer());
        return sz;
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarMenuButton
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarMenuButton::ToolBarMenuButton(ToolBar * parent, wxWindowID winid, int bmp, const wxString & label) :
        ToolBarButton(parent, winid, bmp, label, kHasMenu)
    {
    }
    //-----------------------------------------------------------------------------------------
    ToolBarMenuButton::~ToolBarMenuButton() {}
    //-----------------------------------------------------------------------------------------
    wxSize ToolBarMenuButton::CalculateSize(wxDC& dc) const
    {
        wxSize sz;
        sz.x += m_toolbar->GetXSpacer();
        sz.y = 2 * m_toolbar->GetYSpacer();

        if (m_bmpidx != wxNOT_FOUND) 
        {
            const wxBitmap & tempb = gsgs_Image().GetBitmap(m_bmpidx);
            sz.x += tempb.GetScaledWidth();
            sz.x += m_toolbar->GetXSpacer();

            int height = tempb.GetScaledHeight() + (2 * m_toolbar->GetYSpacer());
            sz.y = wxMax(sz.GetHeight(), height);
        }

        if (!m_label.IsEmpty() && m_toolbar->IsShowLabels()) {
            wxSize textSize = dc.GetTextExtent(m_label);
            sz.x += textSize.GetWidth();
            sz.x += m_toolbar->GetXSpacer();

            int height = textSize.GetHeight() + (2 * m_toolbar->GetYSpacer());
            sz.y = wxMax(sz.GetHeight(), height);
        }

        sz.x += m_toolbar->GetXSpacer();
        sz.x += TOOL_BAR_DROPDOWN_ARROW_SIZE;
        sz.x += m_toolbar->GetXSpacer();
        return sz;
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarSpacer
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarSpacer::ToolBarSpacer(ToolBar* parent) :
        ToolBarButton(parent, wxID_SEPARATOR, wxNOT_FOUND, "", kDisabled | kSpacer)
    {
    }
    //-----------------------------------------------------------------------------------------
    ToolBarSpacer::~ToolBarSpacer() {}
    //-----------------------------------------------------------------------------------------
    wxSize ToolBarSpacer::CalculateSize(wxDC& dc) const
    {
        return wxSize(m_toolbar->GetXSpacer() * 10 + 1, -1);
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarSpacer::Render(wxDC&, const wxRect&){}
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarStretchSpace
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarStretchSpace::ToolBarStretchSpace(ToolBar* parent) :
        ToolBarButton(parent, wxID_SEPARATOR, wxNOT_FOUND, "", kDisabled | kStretchalbeSpace)
    {
    }
    //-----------------------------------------------------------------------------------------
    ToolBarStretchSpace::~ToolBarStretchSpace() {}
    //-----------------------------------------------------------------------------------------
    wxSize ToolBarStretchSpace::CalculateSize(wxDC& dc) const
    {
        wxUnusedVar(dc);
        return wxSize(GetWidth(), 0);
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarStretchSpace::Render(wxDC& dc, const wxRect& rect)
    {
        if (GetWidth() == 0) { return; }
        const wxColour & colour = gsgs_Style().GetAppBgColour();
        dc.SetPen(colour);
        dc.SetBrush(colour);
        dc.DrawRectangle(rect);
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarToggleButton
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarToggleButton::ToolBarToggleButton(ToolBar* parent, wxWindowID id, int bmp, const wxString& label) :
        DefaultToolBarButton(parent, id, bmp, label)
    {
        m_flags = kToggleButton;
    }
    //-----------------------------------------------------------------------------------------
    ToolBarToggleButton::~ToolBarToggleButton() {}
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarControl
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarControl::ToolBarControl(ToolBar* parent, wxWindow* control) :
        ToolBarButton(parent, wxID_ANY, wxNOT_FOUND, "", kControl)
        , m_ctrl(control)
    {
        m_ctrl->Reparent(parent);
    }
    //-----------------------------------------------------------------------------------------
    ToolBarControl::~ToolBarControl() 
    {
    }
    //-----------------------------------------------------------------------------------------
    wxSize ToolBarControl::CalculateSize(wxDC&) const
    {
        return m_ctrl->GetBestSize();
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarControl::Render(wxDC&, const wxRect& rect)
    {
        wxRect controlRect = m_ctrl->GetRect();
        controlRect = controlRect.CenterIn(rect, wxVERTICAL);
        controlRect.SetX(rect.GetX());
        {
            wxWindowUpdateLocker templock(m_ctrl);
            wxSize dds = m_ctrl->GetBestSize();
            m_ctrl->SetSize(controlRect.GetLeft(), controlRect.GetTop(), dds.x, dds.y);
            //m_ctrl->Move(controlRect.GetTopLeft());
            m_ctrl->Refresh();
            //m_ctrl->SetBackgroundColour(gsgs_Style().GetMenuBarBgColour(m_toolbar->HasFlag(ToolBar::kMiniToolBar)));
            //m_ctrl->SetForegroundColour(gsgs_Style().GetTextCtrlTextColour());
        }
        //if (!m_ctrl->IsShown()) { m_ctrl->Show(); }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarControl::Show(bool b)
    {
        ToolBarButton::Show(b);
        wxWindowUpdateLocker ttl(m_ctrl);
        m_ctrl->Show(b);
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBarSeparator
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBarSeparator::ToolBarSeparator(ToolBar * parent) :
        ToolBarButton(parent, wxID_SEPARATOR, wxNOT_FOUND, "", kDisabled | kSeparator)
    {
    }
    //-----------------------------------------------------------------------------------------
    ToolBarSeparator::~ToolBarSeparator() 
    {
    }
    //-----------------------------------------------------------------------------------------
    wxSize ToolBarSeparator::CalculateSize(wxDC& dc) const
    {
        return wxSize(m_toolbar->GetXSpacer() + 1 + m_toolbar->GetXSpacer(), -1);
    }
    //-----------------------------------------------------------------------------------------
    void ToolBarSeparator::Render(wxDC& dc, const wxRect& rect)
    {
        // FillMenuBarBgColour(dc, rect);
        wxCoord xx = rect.GetX();
        xx += m_toolbar->GetXSpacer();

        //wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        //dc.SetPen(c.ChangeLightness(150));
        dc.SetPen(gsgs_Style().GetActiveMarkerColour());
        dc.DrawLine(xx, rect.GetY() + 2, xx, rect.GetY() + rect.GetHeight() - 2);
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // ToolBar
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    ToolBar::ToolBar(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style,
        const wxString& name) : 
        wxPanel(parent, winid, pos, size, style, name)
        , m_popupShown(false)
        , m_flags(0)
    {
        wxWindowUpdateLocker templ(this);
        SetAutoLayout(false);
        m_bgColour = gsgs_Style().GetAppBgColour();
        m_useCustomBgColour = gsgs_Style().IsCustomColour();

        SetGroupSpacing(10);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetMiniToolBar(true);

        Bind(wxEVT_PAINT, &ToolBar::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &ToolBar::OnEraseBackground, this);
        Bind(wxEVT_LEFT_UP, &ToolBar::OnLeftUp, this);
        Bind(wxEVT_LEFT_DOWN, &ToolBar::OnLeftDown, this);
        Bind(wxEVT_LEFT_DCLICK, &ToolBar::OnLeftDown, this);
        Bind(wxEVT_MOTION, &ToolBar::OnMotion, this);
        Bind(wxEVT_LEAVE_WINDOW, &ToolBar::OnLeaveWindow, this);
        //Bind(wxEVT_SIZE, &ToolBar::OnSize, this);
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ToolBar::OnColoursChanged, this);

        // to make sure that the toolbar does not uget the focs, we restore the focus back to the previous window
        /*Bind(wxEVT_SET_FOCUS, [](wxFocusEvent& event) {
            event.Skip();
            wxWindow* oldFocus = event.GetWindow();
            if (oldFocus) { oldFocus->CallAfter(&wxWindow::SetFocus); }
        });*/
    }
    //-----------------------------------------------------------------------------------------
    ToolBar::~ToolBar()
    {
        Unbind(wxEVT_PAINT, &ToolBar::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, &ToolBar::OnEraseBackground, this);
        Unbind(wxEVT_LEFT_UP, &ToolBar::OnLeftUp, this);
        Unbind(wxEVT_MOTION, &ToolBar::OnMotion, this);
        Unbind(wxEVT_LEAVE_WINDOW, &ToolBar::OnLeaveWindow, this);
        Unbind(wxEVT_LEFT_DOWN, &ToolBar::OnLeftDown, this);
        Unbind(wxEVT_LEFT_DCLICK, &ToolBar::OnLeftDown, this);
        //Unbind(wxEVT_SIZE, &ToolBar::OnSize, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ToolBar::OnColoursChanged, this);

        for (size_t i = 0; i < m_buttons.size(); ++i) {
            delete m_buttons[i];
        }
        m_buttons.clear();
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);
        PrepareDC(dc);
        wxGCDC gcdc(dc);

        m_overflowButtons.clear();
        m_visibleButtons.clear();

        wxRect clientRect = GetClientRect();
#ifdef __WXOSX__
        clientRect.Inflate(1);
#endif

        if (m_useCustomBgColour)
        {
            dc.SetBrush(m_bgColour);
            dc.SetPen(m_bgColour);
            dc.DrawRectangle(clientRect);
            clientRect.SetWidth(clientRect.GetWidth() - CHEVRON_SIZE);
            dc.DrawRectangle(clientRect);
        }
        else
        {
            gsgs_Style().FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
            clientRect.SetWidth(clientRect.GetWidth() - CHEVRON_SIZE);
            gsgs_Style().FillMenuBarBgColour(gcdc, clientRect, HasFlag(kMiniToolBar));
        }

        // Prepare for drawings
        std::vector<ToolBarButtonList> groups;
        PrepareForDrawings(gcdc, groups, clientRect);

        int xx = 0;
        for (size_t i = 0; i < groups.size(); ++i)
        {
            RenderGroup(xx, groups[i], gcdc, (i == (groups.size() - 1)));
            // Use a spacer of 10 pixels between groups
            xx += m_groupSpacing;
        }

        // If we have overflow buttons, draw an arrow to the right
        if (!m_overflowButtons.empty() || IsCustomisationEnabled())
        {
            mDDBRect = GetClientRect();
            mDDBRect.SetX(mDDBRect.GetX() + (mDDBRect.GetWidth() - CHEVRON_SIZE - CHEVRONPadding_SIZE * 2));
            mDDBRect.SetWidth(CHEVRON_SIZE);
            gsgs_Style().DrawDropDownArrow(this, gcdc, mDDBRect, false);
        }
        else
        {
            mDDBRect = wxRect();
        }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::RenderGroup(int& xx, const ToolBarButtonList& grp, wxDC& gcdc, bool isLastGroup)
    {
        wxRect clientRect = GetClientRect();

        // Calculate the group size
        /*int groupWidth = 0;
        std::for_each(grp.begin(), grp.end(), [&](ToolBarButton* button) {
            wxSize buttonSize = button->CalculateSize(gcdc);
            groupWidth += buttonSize.GetWidth();
        });*/

        //    if(!isLastGroup) {
        //        wxRect bgRect = wxRect(wxPoint(xx, 0), wxSize(groupWidth, clientRect.GetHeight()));
        //#ifdef __WXOSX__
        //        bgRect.SetHeight(bgRect.GetHeight() - 5);
        //        bgRect = bgRect.CenterIn(clientRect, wxVERTICAL);
        //#endif
        //        bgRect.SetWidth(bgRect.GetWidth() + GetGroupSpacing() / 2);
        //        {
        //            wxColour lineColour = gsgs_Style().GetMenuBarBgColour(IsMiniToolBar());
        //            lineColour = lineColour.ChangeLightness(90);
        //            gcdc.SetPen(lineColour);
        //            gcdc.DrawLine(bgRect.GetTopRight(), bgRect.GetBottomRight());
        //        }
        //        bgRect.SetWidth(bgRect.GetWidth() + 1);
        //        {
        //            wxColour lineColour = gsgs_Style().GetMenuBarBgColour(IsMiniToolBar());
        //            lineColour = lineColour.ChangeLightness(110);
        //            gcdc.SetPen(lineColour);
        //            gcdc.DrawLine(bgRect.GetTopRight(), bgRect.GetBottomRight());
        //        }
        //    }

        // Now draw the buttons
        std::for_each(grp.begin(), grp.end(), [&](ToolBarButton* button) 
        {
            wxSize buttonSize = button->CalculateSize(gcdc);
            if ((xx + buttonSize.GetWidth()) > clientRect.GetRight()) 
            {
                if (button->IsControl()) 
                {
                    ToolBarControl* control = button->Cast<ToolBarControl>();
                    control->Show(false);
                }
                m_overflowButtons.push_back(button);
            }
            else 
            {
                if (button->GetId() == wxID_CLOSE)
                {
                    wxRect r(clientRect.GetWidth() - buttonSize.GetWidth(), 0, buttonSize.GetWidth(), clientRect.GetHeight());
                    r.Deflate(0, 2);
                    r = r.CenterIn(clientRect, wxVERTICAL);
                    button->Render(gcdc, r);
                }
                else
                {
                    wxRect r(xx, 0, buttonSize.GetWidth(), clientRect.GetHeight());
                    r.Deflate(0, 2);
                    r = r.CenterIn(clientRect, wxVERTICAL);
                    button->Render(gcdc, r);
                }
                m_visibleButtons.push_back(button);
            }
            xx += buttonSize.GetWidth();
        });
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnEraseBackground(wxEraseEvent&) {}
    //-----------------------------------------------------------------------------------------
    wxRect ToolBar::CalculateRect(wxDC& dc) const
    {
        wxRect rect;
        std::for_each(m_buttons.begin(), m_buttons.end(), [&](ToolBarButton* button) {
            wxSize buttonSize = button->CalculateSize(dc);
            rect.width += buttonSize.GetWidth();
            rect.height = wxMax(rect.GetHeight(), buttonSize.GetHeight());
        });
        // Always assume that we need the extra space for the chevron button
        rect.width += CHEVRON_SIZE + CHEVRONPadding_SIZE;
        return rect;
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::Realize()
    {
        wxBitmap bmp(1, 1);
        wxMemoryDC dc(bmp);
        wxGCDC gcdc(dc);
        gcdc.SetFont(gsgs_Style().GetDefaultGuiFont());
        wxWindowUpdateLocker templ(this);
        SetSizeHints(CalculateRect(gcdc).GetSize());
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnLeftUp(wxMouseEvent& event)
    {
        wxPoint pos = event.GetPosition();
        if (mDDBRect.Contains(pos)) {
            DoShowOverflowMenu();
        }
        else {
            for (size_t i = 0; i < m_visibleButtons.size(); ++i) {
                ToolBarButton* btn = m_visibleButtons[i];
                if (btn->Contains(pos)) {
                    if (!btn->IsEnabled()) {
                        // events are not fired for disabled buttons
                        return;
                    }
                    if (btn->IsToggle()) {
                        // Change the button state
                        btn->Check(!btn->IsChecked());

                        // The hover state is the opposite to the check status
                        btn->SetHover(!btn->IsChecked());

                        // Fire an event with proper IsChecked() set
                        wxCommandEvent clicked(wxEVT_TOOL, btn->GetId());
                        clicked.SetEventObject(this);
                        clicked.SetInt(btn->IsChecked() ? 1 : 0);
                        GetEventHandler()->AddPendingEvent(clicked);
                        Refresh();

                    }
                    else if (btn->InsideMenuButton(pos)) {
                        wxMenu* menu = btn->GetMenu();
                        if (menu) {
                            // We got the menu, show it
                            ShowMenuForButton(btn->GetId(), menu);
                        }
                        else {
                            wxCommandEvent clicked(wxEVT_TOOL_DROPDOWN, btn->GetId());
                            clicked.SetEventObject(this);
                            GetEventHandler()->ProcessEvent(clicked);
                        }
                        btn->SetPressed(false);
                        btn->SetHover(true);
                        Refresh();
                    }
                    else {
                        wxCommandEvent clicked(wxEVT_TOOL, btn->GetId());
                        clicked.SetEventObject(this);
                        GetEventHandler()->AddPendingEvent(clicked);
                        btn->SetPressed(false);
                        btn->SetHover(true);
                        Refresh();
                    }
                    break;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnLeftDown(wxMouseEvent& event)
    {
        //event.Skip();
        wxPoint pos = event.GetPosition();
        for (size_t i = 0; i < m_visibleButtons.size(); ++i) 
        {
            m_visibleButtons[i]->ClearRenderFlags();
            if (m_visibleButtons[i]->Contains(pos)) 
            { 
                m_visibleButtons[i]->SetPressed(true); 
                m_visibleButtons[i]->SetHover(true);
            }
        }
        //Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnMotion(wxMouseEvent& event)
    {
        wxPoint pos = event.GetPosition();
        bool refreshNeeded = false;
        bool tooltipValid = false;
        for (size_t i = 0; i < m_visibleButtons.size(); ++i)
        {
            ToolBarButton * button = m_visibleButtons[i];
            if (button->Contains(pos))
            {
                if (!button->IsHover())
                {
                    // a refresh is needed
                    button->SetHover(true);
                    refreshNeeded = true;
                }
                if (button->IsSeparator() || button->IsSpacer())
                {
                    UnsetToolTip();
                }
                else
                {
                    //button->SetHover(true);
                    tooltipValid = true;
                    if (button->GetLabel().IsEmpty() || !button->IsEnabled())
                    {
                        UnsetToolTip();
                    }
                    else {
                        SetToolTip(button->GetLabel());
                    }
                }
            }
            else
            {
                if (button->IsHover())
                {
                    // a refresh is needed
                    refreshNeeded = true;
                }
                // Clear the hover flag
                button->SetHover(false);
            }
        }
        if (!tooltipValid) { UnsetToolTip(); }
        if (refreshNeeded) { Refresh(); }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnLeaveWindow(wxMouseEvent& event)
    {
        if (!m_popupShown) {
            for (size_t i = 0; i < m_buttons.size(); ++i) {
                m_buttons[i]->ClearRenderFlags();
            }
            Refresh();
        }
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddButton(wxWindowID id, int bmp, const wxString& label)
    {
        return Add(new DefaultToolBarButton(this, id, bmp, label));
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddMenuButton(wxWindowID id, int bmp, const wxString& label)
    {
        return Add(new ToolBarMenuButton(this, id, bmp, label));
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddToggleButton(wxWindowID id, int bmp, const wxString& label)
    {
        return Add(new ToolBarToggleButton(this, id, bmp, label));
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::InsertBefore(wxWindowID where, ToolBarButton* button)
    {
        ToolBarButtonList::iterator iter =
            std::find_if(m_buttons.begin(), m_buttons.end(), [&](ToolBarButton* b) { return (b->GetId() == where); });
        if (iter == m_buttons.end()) { return NULL; }
        m_buttons.insert(iter, button);
        return button;
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::InsertAfter(wxWindowID where, ToolBarButton* button)
    {
        ToolBarButtonList::iterator iter =
            std::find_if(m_buttons.begin(), m_buttons.end(), [&](ToolBarButton* b) { return (b->GetId() == where); });
        if (iter == m_buttons.end()) { return NULL; }
        ++iter; // can be end()
        m_buttons.insert(iter, button);
        return button;
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::Add(ToolBarButton* button)
    {
        m_buttons.push_back(button);
        return button;
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::ShowMenuForButton(wxWindowID buttonID, wxMenu* menu)
    {
        ToolBarButtonList::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
            [&](ToolBarButton* b) { return (b->GetId() == buttonID); });
        if (iter == m_buttons.end()) { return; }
        ToolBarButton* button = *iter;
        m_popupShown = true;
        wxPoint menuPos = button->GetButtonRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif

        PopupMenu(menu, menuPos);
        m_popupShown = false;

        wxPoint pt = ::wxGetMousePosition();
        pt = ScreenToClient(pt);
        if (!GetClientRect().Contains(pt)) {
            wxMouseEvent dummy;
            OnLeaveWindow(dummy);
        }
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::FindById(wxWindowID id) const
    {
        ToolBarButtonList::const_iterator iter =
            std::find_if(m_buttons.begin(), m_buttons.end(), [&](ToolBarButton* b) { return (b->GetId() == id); });
        if (iter != m_buttons.end()) 
        { 
            return (*iter);
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------------
    bool ToolBar::DeleteById(wxWindowID id)
    {
        ToolBarButtonList::iterator iter =
            std::find_if(m_buttons.begin(), m_buttons.end(), [&](ToolBarButton* b) { return (b->GetId() == id); });
        if (iter == m_buttons.end()) { return false; }
        ToolBarButton* button = (*iter);
        delete button;
        m_buttons.erase(iter);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddSeparator() { return Add(new ToolBarSeparator(this)); }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddSpacer() { return Add(new ToolBarSpacer(this)); }
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddStretchableSpace() { return Add(new ToolBarStretchSpace(this)); }
    //-----------------------------------------------------------------------------------------
    void ToolBar::SetDropdownMenu(wxWindowID buttonID, wxMenu* menu)
    {
        ToolBarButton* button = FindById(buttonID);
        if (button)
        {
            button->SetMenu(menu);
        }
    }
    //-----------------------------------------------------------------------------------------
    wxMenu* ToolBar::FindMenuById(wxWindowID buttonID) const
    {
        ToolBarButton* button = FindById(buttonID);
        if (button)
        {
            return button->GetMenu();
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::ToggleTool(wxWindowID buttonID, bool toggle)
    {
        ToolBarButton* button = FindById(buttonID);
        if (button) 
        { 
            button->Check(toggle); 
        }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::UpdateWindowUI(long flags)
    {
        // Call update UI event per button
        if (flags & wxUPDATE_UI_FROMIDLE) 
        { 
            bool refreshNeeded = false;
            for (size_t i = 0; i < m_visibleButtons.size(); ++i) {
                ToolBarButton* button = m_visibleButtons.at(i);
                bool oldCheck = button->IsChecked();
                bool oldEnabled = button->IsEnabled();
                wxUpdateUIEvent event(button->GetId());
                event.Enable(oldEnabled);
                if (button->IsToggle())
                {
                    event.Check(oldCheck); 
                }
                if (GetEventHandler()->ProcessEvent(event))
                {
                    if (button->IsToggle()) 
                    { 
                        button->Check(event.GetChecked()); 
                    }
                    button->Enable(event.GetEnabled());
                    if((oldCheck != button->IsChecked()) || (oldEnabled != button->IsEnabled()))
                    { 
                        refreshNeeded = true;
                    }
                }
            }
            if(refreshNeeded)
                Refresh();
        }

        wxPanel::UpdateWindowUI(flags);
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::DoShowOverflowMenu()
    {
        // Show the drop down menu
        wxMenu menu;
        std::vector<int> checkedItems;
        bool last_was_separator = false;
        for (size_t i = 0; i < m_overflowButtons.size(); ++i) {
            ToolBarButton* button = m_overflowButtons[i];
            if (button->IsSeparator() && !last_was_separator) {
                menu.AppendSeparator();
                last_was_separator = true;
            }
            else if (!button->IsControl() && !button->IsSpacer()) {
                // Show all non-control buttons
                wxMenuItem* menuItem = new wxMenuItem(&menu, button->GetId(), button->GetLabel(), button->GetLabel(),
                    button->IsToggle() ? wxITEM_CHECK : wxITEM_NORMAL);
                if ((button->GetBmpIdx()!= wxNOT_FOUND) && !button->IsToggle()) 
                { 
                    menuItem->SetBitmap(gsgs_Image().GetBitmap(button->GetBmpIdx())); 
                }
                if (button->IsToggle() && button->IsChecked()) 
                { 
                    checkedItems.push_back(button->GetId()); 
                }
                menu.Append(menuItem);
                menuItem->Enable(button->IsEnabled());
                if (button->IsEnabled()) {
                    // Don't draw two separators one after the other
                    last_was_separator = false;
                }
            }
        }
        if (IsCustomisationEnabled()) {
            if (menu.GetMenuItemCount() && !last_was_separator) { menu.AppendSeparator(); }
            menu.Append(XRCID("customise_toolbar"), _("Customise..."));
            menu.Bind(wxEVT_MENU,
                [&](wxCommandEvent& event) {
                wxCommandEvent evtCustomise(wxEVT_TOOLBAR_CUSTOMISE);
                evtCustomise.SetEventObject(this);
                GetEventHandler()->AddPendingEvent(evtCustomise);
            },
                XRCID("customise_toolbar"));
        }
        // Show the menu
        m_popupShown = true;
        wxPoint menuPos = mDDBRect.GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif
        for (size_t i = 0; i < checkedItems.size(); ++i) {
            menu.Check(checkedItems[i], true);
        }
        menu.Bind(wxEVT_MENU, &ToolBar::OnOverflowItem, this, wxID_ANY);
        PopupMenu(&menu, menuPos);
        menu.Unbind(wxEVT_MENU, &ToolBar::OnOverflowItem, this, wxID_ANY);
        m_popupShown = false;

        wxPoint pt = ::wxGetMousePosition();
        pt = ScreenToClient(pt);
        if (!GetClientRect().Contains(pt)) {
            wxMouseEvent dummy;
            OnLeaveWindow(dummy);
        }
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnOverflowItem(wxCommandEvent& event)
    {
        // Update our button
        ToolBarButton* button = FindById(event.GetId());
        if (button && button->IsToggle()) 
        { 
            button->Check(event.IsChecked()); 
        }

        // Call the default action
        event.Skip();
    }
    //-----------------------------------------------------------------------------------------
    /*void ToolBar::OnSize(wxSizeEvent& event)
    {
        Refresh();
    }*/
    //-----------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddControl(wxWindow* control) 
    { 
        return Add(new ToolBarControl(this, control)); 
    }
    //------------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddToolInter(wxWindowID id, const wxString& label, const wxBitmap& bmp,
        const wxString& helpString, wxItemKind kind)
    {
        wxUnusedVar(helpString);

        switch (kind) {
        case wxITEM_DROPDOWN:
            return AddMenuButton(id, gsgs_Image().AddInterMap(bmp), label);
        case wxITEM_CHECK:
            return AddToggleButton(id, gsgs_Image().AddInterMap(bmp), label);
        case wxITEM_NORMAL:
        default:
            return AddButton(id, gsgs_Image().AddInterMap(bmp), label);
        }
    }
    //------------------------------------------------------------------------------------------
    ToolBarButton* ToolBar::AddTool(wxWindowID id, const wxString& label, const wxBitmap& bmp,
        const wxString& helpString, wxItemKind kind)
    {
        wxUnusedVar(helpString);

        switch (kind) {
        case wxITEM_DROPDOWN:
            return AddMenuButton(id, gsgs_Image().AddResMap(bmp), label);
        case wxITEM_CHECK:
            return AddToggleButton(id, gsgs_Image().AddResMap(bmp), label);
        case wxITEM_NORMAL:
        default:
            return AddButton(id, gsgs_Image().AddResMap(bmp), label);
        }
    }
    //-----------------------------------------------------------------------------------------
    int ToolBar::GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu)
    {
        ToolBarButtonList::iterator iter = std::find_if(m_buttons.begin(), m_buttons.end(),
            [&](ToolBarButton* b) { return (b->GetId() == buttonID); });
        if (iter == m_buttons.end()) { return wxID_NONE; }
        ToolBarButton* button = *iter;
        m_popupShown = true;
        wxPoint menuPos = button->GetButtonRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif

        int selection = GetPopupMenuSelectionFromUser(*menu, menuPos);
        m_popupShown = false;

        wxPoint pt = ::wxGetMousePosition();
        pt = ScreenToClient(pt);
        if (!GetClientRect().Contains(pt)) {
            wxMouseEvent dummy;
            OnLeaveWindow(dummy);
        }
        return selection;
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::PrepareForDrawings(wxDC& dc, std::vector<ToolBarButtonList>& grp, const wxRect& rect)
    {
        grp.clear();
        int totalWidth = 0;
        int stretchableButtons = 0;
        ToolBarButtonList curG;
        ToolBarButtonList spacers;
        for (size_t i = 0; i < m_buttons.size(); ++i)
        {
            ToolBarButton* button = m_buttons[i];

            // Don't include stretchable buttons in the total width
            if (!button->IsStretchableSpace()) { totalWidth += button->CalculateSize(dc).GetWidth(); }
            if (button->IsHidden())
            {
                continue;
            }

            if (button->IsStretchableSpace()) {
                stretchableButtons++;
                spacers.push_back(button);

                // A stretchable space is a one-one-group
                if (curG.size()) {
                    grp.push_back(curG);
                    curG.clear();
                }
                curG.push_back(button);
                grp.push_back(curG);
                curG.clear();
                continue;
            }
            else if (button->IsSpacer() || button->IsSeparator()) {
                // close this group and start a new one
                if (curG.size()) {
                    grp.push_back(curG);
                    curG.clear();
                }
                continue;
            }
            else {
                curG.push_back(button);
            }
        }
        if (curG.size()) 
        { 
            grp.push_back(curG); 
        }

        // Set a size to each stretchable button
        if (spacers.size()) 
        {
            int spacer_width = ((rect.GetWidth() - totalWidth - ((grp.size() - 1) * GetGroupSpacing())) / spacers.size());
            for (ToolBarButton* button : spacers) 
            {
                button->Cast<ToolBarStretchSpace>()->SetWidth(spacer_width < 0 ? 0 : spacer_width);
            }
        }
    }
    //-----------------------------------------------------------------------------------------
    int ToolBar::GetXSpacer() const 
    { 
        return HasFlag(kMiniToolBar) ? 2 : 4; 
    }
    //-----------------------------------------------------------------------------------------
    int ToolBar::GetYSpacer() const 
    { 
        return HasFlag(kMiniToolBar) ? 2 : 4; 
    }
    //-----------------------------------------------------------------------------------------
    void ToolBar::OnColoursChanged(Event& event)
    {
        event.Skip();
        m_bgColour = gsgs_Style().GetAppBgColour();
        m_useCustomBgColour = gsgs_Style().IsCustomColour(); 
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
}