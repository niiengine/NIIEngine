#include "gsgsStatusBar.h"
#include "gsgsStyleManager.h"
#include "gsgsImageManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/fontenc.h>
#include <wx/fontmap.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

#define STATUSBAR_STATE_IDX             0
#define STATUSBAR_SCM_IDX               1
#define STATUSBAR_LINE_COL_IDX          2
#define STATUSBAR_ANIMATION_COL_IDX     3
#define STATUSBAR_WHITESPACE_INFO_IDX   4
#define STATUSBAR_EOL_COL_IDX           5
#define STATUSBAR_LANG_COL_IDX          6
#define STATUSBAR_ENCODING_COL_IDX      7
#define STATUSBAR_ICON_COL_IDX          8

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_STATUSBAR_CLICKED, Event);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // StatusBarView
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    StatusBarView::StatusBarView(const wxString & name)
        : m_name(name)
    {
    }
    //-----------------------------------------------------------------
    void StatusBarView::DrawText(wxDC & dc, wxCoord x, wxCoord y, const wxString & text)
    {
        dc.SetTextForeground(GetTextColour());
        dc.SetTextBackground(GetTextColour());
        dc.DrawText(text, x, y);
    }
    //-----------------------------------------------------------------
    void StatusBarView::DrawFieldSeparator(wxDC& dc, const wxRect& fieldRect)
    {
        // draw border line
        dc.SetPen(GetPenColour());
        wxPoint bottomPt, topPt;

        topPt = fieldRect.GetTopLeft();
        topPt.y += 2;

        bottomPt = fieldRect.GetBottomLeft();
        bottomPt.y += 1;
        dc.DrawLine(topPt, bottomPt);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // StatusBarTextView
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    void StatusBarTextView::Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art)
    {
        if (m_text.size())
        {
            m_rect = rect;
            wxSize textSize = dc.GetTextExtent(m_text);

            // Center text
            wxCoord textY = (rect.GetHeight() - textSize.GetHeight()) / 2 + rect.y;
            wxCoord textX;
            if (m_textAlign == wxALIGN_CENTER) {
                textX = (rect.GetWidth() - textSize.GetWidth()) / 2 + rect.x;
            }
            else {
                // left
                textX = rect.x + 5;
            }

            // draw border line
            art->DrawFieldSeparator(dc, rect);

            // Draw the text
            art->DrawText(dc, textX, textY /*+ 1*/, m_text);
        }
    }
    //-----------------------------------------------------------------
    void StatusBarTextView::SetText(const wxString& text)
    {
        m_text = text;
        // Render the new text
        if ((m_rect != wxRect()) && m_parent) {
            // valid rect
            StatusBarView::Ptr_t art = m_parent->GetArt();
            if (art->GetName() == m_parent->GetLastArtNameUsedForPaint()) {
#if defined(__WXOSX__) || defined(__WXMSW__)
                m_parent->Refresh();
                return;
#endif
                // Make sure we draw only when the "art" objects are in sync with the field
                // and with the bar itself
                if ((m_rect.GetHeight() > 0) && (m_rect.GetWidth() > 0)) {
                    wxBitmap bmp(m_rect.GetSize());
                    wxMemoryDC memoryDC;
                    memoryDC.SelectObject(bmp);
                    wxGCDC dc(memoryDC);
                    m_parent->PrepareDC(dc);

                    wxFont font = gsgs_Style().GetDefaultGuiFont();
                    dc.SetFont(font);
                    wxRect rect(m_rect.GetSize()); // Create the same rect size, but on 0,0

                    // Draw the field background
                    dc.SetBrush(art->GetBgColour());
                    dc.SetPen(art->GetBgColour());
                    dc.DrawRectangle(rect);

                    // Draw top separator line
                    wxPoint topLeft = rect.GetTopLeft();
                    wxPoint topRight = rect.GetTopRight();
                    topRight.x += 1;
                    dc.SetPen(art->GetSeparatorColour());
                    dc.DrawLine(topLeft, topRight);

                    // Draw the bottom separator using the pen colour
                    // this will give a "sink" look to the status bar
                    topLeft.y += 1;
                    topRight.y += 1;
                    dc.SetPen(art->GetPenColour());
                    dc.DrawLine(topLeft, topRight);

                    // Render will override m_rect, we so keep a copy
                    wxRect origRect = m_rect;
                    Render(dc, rect, art);
                    m_rect = origRect;
                    memoryDC.SelectObject(wxNullBitmap);

                    // bmp contains the field content, draw it
                    wxClientDC cdc(m_parent);
                    m_parent->PrepareDC(cdc);
                    cdc.DrawBitmap(bmp, m_rect.GetTopLeft(), true);
                }
            }
        }
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // StatusBarAnimationView
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    StatusBarAnimationView::StatusBarAnimationView(CustomStatusBar* parent, const wxBitmap& sprite,
        wxOrientation spriteOrientation, const wxSize& animSize)
        : StatusBarField(parent)
    {
        m_animation = new wxPNGAnimation(parent, sprite, spriteOrientation, animSize);

        m_width = animSize.GetWidth() + (2 * 5); // 2*5 here for spaces from the left and right
        // We need to tie the animation mouse events to the status bar, otherwise, whenever we hover or
        // move over the animation control - it will not be seen by the status bar
        m_animation->Bind(wxEVT_LEFT_DOWN, &StatusBarAnimationView::OnAnimationClicked, this);
    }
    //-----------------------------------------------------------------
    StatusBarAnimationView::~StatusBarAnimationView() 
    {
    }
    //-----------------------------------------------------------------
    void StatusBarAnimationView::Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art)
    {
        m_rect = rect;

        // Draw the left side border
        art->DrawFieldSeparator(dc, rect);

        // Position the animation
        wxSize animSize = m_animation->GetSize();
        wxCoord y = (rect.GetHeight() - animSize.GetHeight()) / 2 + rect.y;
        wxCoord x = (rect.GetWidth() - animSize.GetWidth()) / 2 + rect.x;
        m_animation->SetPanelColour(art->GetBgColour());
        m_animation->Move(wxPoint(x, y + 1));
    }
    //-----------------------------------------------------------------
    void StatusBarAnimationView::Start(long refreshRate)
    {
        m_animation->Start(refreshRate);
    }
    //-----------------------------------------------------------------
    void StatusBarAnimationView::Stop()
    {
        m_animation->Stop();
    }
    //-----------------------------------------------------------------
    void StatusBarAnimationView::OnAnimationClicked(wxMouseEvent& event)
    {
        event.Skip();
        CustomStatusBar* bar = dynamic_cast<CustomStatusBar*>(m_animation->GetParent());
        if (bar) {
            // Notify the bar that the animation owned by this field was clicked
            bar->AnimationClicked(this);
        }
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // StatusBarBitmapView
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    void StatusBarBitmapView::Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art)
    {
        // Center bitmap
        if (m_bitmap.IsOk()) 
        {
            m_rect = rect;
            // draw border line
            art->DrawFieldSeparator(dc, rect);

            int bmpHeight = m_bitmap.GetScaledHeight();
            int bmpWidth = m_bitmap.GetScaledWidth();
            wxCoord bmpY = (rect.GetHeight() - bmpHeight) / 2 + rect.y;
            wxCoord bmpX = (rect.GetWidth() - bmpWidth) / 2 + rect.x;
            // Draw the bitmap
            dc.DrawBitmap(m_bitmap, bmpX, bmpY + 1);
        }
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // CustomStatusBar
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    CustomStatusBar::CustomStatusBar(wxWindow* parent, wxWindowID id, long style)
        : wxStatusBar(parent, id, style)
        , m_art(new StatusBarView("Dark"))
        , m_mainText(new StatusBarTextView(this, 0))
        , mAutoStopGlint(false)
    {
        SetAutoLayout(false);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        m_mainText->Cast<StatusBarTextView>()->SetTextAlignment(wxALIGN_LEFT);
        m_timer = new wxTimer(this);
        m_glinttimer = new wxTimer(this);
        Bind(wxEVT_PAINT, &CustomStatusBar::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &CustomStatusBar::OnEraseBackround, this);
        //Bind(wxEVT_SIZE, &CustomStatusBar::OnSize, this);
        Bind(wxEVT_LEFT_DOWN, &CustomStatusBar::OnLeftDown, this);
        Bind(wxEVT_MOTION, &CustomStatusBar::OnMouseMotion, this);
        Bind(wxEVT_TIMER, &CustomStatusBar::OnTimer, this, m_timer->GetId());
        Bind(wxEVT_TIMER, &CustomStatusBar::OnGlint, this, m_glinttimer->GetId());
        m_CurrentColour = gsgs_Style().GetActiveMarkerColour();
    }
    //-----------------------------------------------------------------
    CustomStatusBar::~CustomStatusBar()
    {
        m_timer->Stop();
        m_glinttimer->Stop();
        Unbind(wxEVT_PAINT, &CustomStatusBar::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, &CustomStatusBar::OnEraseBackround, this);
        //Unbind(wxEVT_SIZE, &CustomStatusBar::OnSize, this);
        Unbind(wxEVT_LEFT_DOWN, &CustomStatusBar::OnLeftDown, this);
        Unbind(wxEVT_MOTION, &CustomStatusBar::OnMouseMotion, this);
        Unbind(wxEVT_TIMER, &CustomStatusBar::OnTimer, this, m_timer->GetId());
        Unbind(wxEVT_TIMER, &CustomStatusBar::OnGlint, this, m_glinttimer->GetId());
        wxDELETE(m_timer);
        wxDELETE(m_glinttimer);
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC abdc(this);
        PrepareDC(abdc);
        wxGCDC dc(abdc);
        wxRect rect = GetClientRect();

        dc.SetFont(gsgs_Style().GetDefaultGuiFont());

        // Remember which art name used for painting
        SetLastArtNameUsedForPaint(m_art->GetName());

        // Fill the background

        dc.SetBrush(m_CurrentColour);
        dc.SetPen(m_CurrentColour);
        dc.DrawRectangle(rect);

        // Calculate the fields length
        size_t totalLength = rect.GetWidth();
        size_t fieldsLength = DoGetFieldsWidth();

        // sanity
        size_t offsetX = 0;
        size_t offsetX2 = 0;
        if (totalLength <= fieldsLength) {
            offsetX = 0;
        }
        else {
            offsetX = totalLength - fieldsLength;
        }

        //===----------------------
        // Draw the main field
        //===----------------------
        // update the rect
        if (m_text.size())
        {
            wxRect mainRect(0, rect.y, offsetX, rect.height);
            dc.SetClippingRegion(mainRect);
            m_mainText->SetRect(mainRect);
            m_mainText->Cast<StatusBarTextView>()->Render(dc, mainRect, m_art);
            m_mainText->Cast<StatusBarTextView>()->SetTooltip(m_text);
            dc.DestroyClippingRegion();
        }
        //===----------------------
        // Draw the fields
        //===----------------------

        for (size_t i = 0; i < m_fields.size(); ++i) {
            wxSharedPtr<StatusBarField> & item = m_fields.at(i);
            // Prepare the rect
            if (item->GetLeftAlignment())
            {
                wxRect fieldRect(offsetX2, rect.y, item->GetWidth(), rect.height);
                dc.SetClippingRegion(fieldRect);
                item->Render(dc, fieldRect, m_art);
                dc.DestroyClippingRegion();
                offsetX2 += item->GetWidth();
            }
            else
            {
                wxRect fieldRect(offsetX, rect.y, item->GetWidth(), rect.height);
                dc.SetClippingRegion(fieldRect);
                item->Render(dc, fieldRect, m_art);
                dc.DestroyClippingRegion();
            }
            offsetX += item->GetWidth();
            if (offsetX > totalLength)
                return;
        }
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnEraseBackround(wxEraseEvent& event) 
    { 
        wxUnusedVar(event); 
    }
    //-----------------------------------------------------------------
    size_t CustomStatusBar::DoGetFieldsWidth()
    {
        size_t width(0);
        for (size_t i = 0; i < m_fields.size(); ++i) {
            width += m_fields.at(i)->GetWidth();
        }
        return width;
    }
    //-----------------------------------------------------------------
    StatusBarField::Ptr_t CustomStatusBar::GetField(size_t index)
    {
        if (index >= m_fields.size()) return StatusBarField::Ptr_t(NULL);
        return m_fields.at(index);
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::RemoveField(size_t index)
    {
        if (index >= m_fields.size()) return;
        m_fields.erase(m_fields.begin() + index);
        if (m_timer->IsRunning()) { m_timer->Stop(); }
        Refresh();
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnLeftDown(wxMouseEvent& event)
    {
        //event.Skip();
        wxPoint point = event.GetPosition();
        for (size_t i = 0; i < m_fields.size(); ++i) {
            if (m_fields.at(i)->HitTest(point)) {
                // fire an event
                Event event(wxEVT_STATUSBAR_CLICKED);
                event.SetEventObject(this);
                event.SetInt(i);
                GetEventHandler()->ProcessEvent(event);
                break;
            }
        }
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::ClearText()
    {
        m_text.Clear();
        if (m_timer->IsRunning())
        {
            m_timer->Stop();
        }
        Refresh();
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::SetGlint(const wxColour & c, int msecondsToGlint, int secondsToLive)
    {
        if (msecondsToGlint != wxNOT_FOUND)
        {
            m_glintColour.SetRGB(c.GetRGB());
            if (m_glintColour != m_CurrentColour)
            {
                m_glinttimer->Start(msecondsToGlint);

                if (secondsToLive != wxNOT_FOUND)
                {
                    mAutoStopGlint = true;
                    m_timer->Start(secondsToLive * 1000, true);
                }
            }
        }
        else
        {
            if (m_glinttimer->IsRunning())
            {
                m_glinttimer->Stop();
            }
            mAutoStopGlint = false;
            m_CurrentColour = c;
        }
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::SetText(const wxString& message, int secondsToLive)
    {
        if (m_timer->IsRunning()) 
        { 
            m_timer->Stop(); 
        }

        m_text = message;
        SetToolTip(message);

        // Make sure we draw only when the "art" objects are in sync with the field
        // and with the bar itself
        wxRect mainRect = DoGetMainFieldRect();

        // update the rect
        m_mainText->SetRect(mainRect);
        m_mainText->Cast<StatusBarTextView>()->SetText(m_text);
        m_mainText->Cast<StatusBarTextView>()->SetTooltip(m_text);

        if (secondsToLive != wxNOT_FOUND) { m_timer->Start(secondsToLive * 1000, true); }
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnMouseMotion(wxMouseEvent& event)
    {
        //event.Skip();
        SetToolTip(wxEmptyString);
        wxPoint point = event.GetPosition();
        for (size_t i = 0; i < m_fields.size(); ++i) {
            if (m_fields.at(i)->HitTest(point)) {
                SetToolTip(m_fields.at(i)->GetTooltip());
                return;
            }
        }
        SetToolTip(m_text);
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::AnimationClicked(StatusBarField* field)
    {
        for (size_t i = 0; i < m_fields.size(); ++i) {
            if (m_fields.at(i).get() == field) {
                // fire an event
                Event event(wxEVT_STATUSBAR_CLICKED);
                event.SetEventObject(this);
                event.SetInt(i);
                GetEventHandler()->ProcessEvent(event);
                break;
            }
        }
    }
    //-----------------------------------------------------------------
    wxRect CustomStatusBar::DoGetMainFieldRect()
    {
        // Calculate the fields length
        wxRect rect = GetClientRect();
        size_t totalLength = rect.GetWidth();
        size_t fieldsLength = DoGetFieldsWidth();

        size_t offsetX = 0;
        if (totalLength <= fieldsLength) {
            offsetX = 0;
        }
        else {
            offsetX = totalLength - fieldsLength;
        }
        wxRect mainRect(0, rect.y, offsetX, rect.height);
        return mainRect;
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnTimer(wxTimerEvent& event) 
    { 
        SetText("", wxNOT_FOUND);
        if (mAutoStopGlint)
        {
            SetGlint(m_art->GetBgColour(), wxNOT_FOUND, 0);
        }
    }
    //-----------------------------------------------------------------
    void CustomStatusBar::OnGlint(wxTimerEvent& event)
    {
        wxColour temp = m_CurrentColour;
        m_CurrentColour = m_glintColour;
        m_glintColour = temp;
        Refresh();
    }
    //-----------------------------------------------------------------
    bool StatusBarField::HitTest(const wxPoint& point) const 
    { 
        return m_rect.Contains(point); 
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // DefaultStatusBarView
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class _gsgsAPI DefaultStatusBarView : public StatusBarView
    {
    public:
        DefaultStatusBarView() : StatusBarView("Light")
        {
            OnColoursChanged(Event());
            gsgs_Event().Bind(wxEVT_SYS_COLOURS_CHANGED, &DefaultStatusBarView::OnColoursChanged, this);
        }
        virtual ~DefaultStatusBarView()
        {
            gsgs_Event().Unbind(wxEVT_SYS_COLOURS_CHANGED, &DefaultStatusBarView::OnColoursChanged, this);
        }

        virtual void DrawFieldSeparator(wxDC& dc, const wxRect& fieldRec)
        {
            wxUnusedVar(dc);
            wxUnusedVar(fieldRec);
        }

        virtual wxColour GetBgColour() const        { return m_bgColour; }
        virtual wxColour GetPenColour() const       { return m_penColour; }
        virtual wxColour GetTextColour() const      { return m_textColour; }
        virtual wxColour GetSeparatorColour() const { return m_separatorColour; }
    protected:
        void OnColoursChanged(Event& event)
        {
            event.Skip();
            m_bgColour = gsgs_Style().GetActiveMarkerColour();
            m_bgColour = m_bgColour.ChangeLightness(Colour::IsDark(m_bgColour) ? 105 : 95);

            m_penColour = m_bgColour;
            m_textColour.Set(0xFFFFFFFF);
            m_separatorColour = m_bgColour;
        }
    private:
        wxColour m_bgColour;
        wxColour m_penColour;
        wxColour m_textColour;
        wxColour m_separatorColour;
    };
    //-----------------------------------------------------------------
    static void GetWhitespaceInfo(wxStyledTextCtrl* ctrl, wxString& whitespace, wxString& eol)
    {
        whitespace << (ctrl->GetUseTabs() ? "tabs" : "spaces");
        int eolMode = ctrl->GetEOLMode();
        switch (eolMode)
        {
        case wxSTC_EOL_CR:
            eol << "CR";
            break;
        case wxSTC_EOL_CRLF:
            eol << "CRLF";
            break;
        case wxSTC_EOL_LF:
            eol << "LF";
            break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // StatusBar
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    StatusBar::StatusBar(wxWindow* parent, IManager* mgr)
        : CustomStatusBar(parent)
        , m_mgr(mgr)
    {
        SetArt(StatusBarView::Ptr_t(new DefaultStatusBarView));
        SetSize(wxSize(-1, 18));
        SetSizeHints(wxSize(-1, 18), wxSize(-1, 18));
        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &StatusBar::OnPageChanged, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &StatusBar::OnTheme, this);
        gsgs_Event().Bind(wxEVT_CMD_PAGE_CHANGED, &StatusBar::OnPageChanged, this);
        gsgs_Event().Bind(wxEVT_ALL_EDITORS_CLOSED, &StatusBar::OnAllEditorsClosed, this);
        gsgs_Event().Bind(wxEVT_BUILD_STARTED, &StatusBar::OnBuildStarted, this);
        gsgs_Event().Bind(wxEVT_BUILD_ENDED, &StatusBar::OnBuildEnded, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED, &StatusBar::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &StatusBar::OnEditorSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SHOWING, &StatusBar::OnGotoAnythingShowing, this);
        gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SELECTED, &StatusBar::OnActionSelected, this);
        Bind(wxEVT_STATUSBAR_CLICKED, &StatusBar::OnFieldClicked, this);

        StatusBarField::Ptr_t state(new StatusBarTextView(this, GetScaledSize(300)));
        state->Cast<StatusBarTextView>()->SetTextAlignment(wxALIGN_LEFT);
        state->SetLeftAlignment(true);
        AddField(state);

        AddField(StatusBarField::Ptr_t(new StatusBarBitmapView(this, GetScaledSize(30))));

        int twidth = GetTextWidth("Line 100000, Column 999, Pos 12345678, Len 4821182");
        StatusBarField::Ptr_t lineCol(new StatusBarTextView(this, twidth));
        AddField(lineCol);

        StatusBarField::Ptr_t buildAnimation(new StatusBarAnimationView(
            this, wxXmlResource::Get()->LoadBitmap("build-animation-sprite"), wxHORIZONTAL, wxSize(80, 7)));
        AddField(buildAnimation);

        AddField(StatusBarField::Ptr_t(new StatusBarTextView(this, GetScaledSize(80))));

        AddField(StatusBarField::Ptr_t(new StatusBarTextView(this, GetScaledSize(50))));

        AddField(StatusBarField::Ptr_t(new StatusBarTextView(this, GetTextWidth("_properties_"))));

        AddField(StatusBarField::Ptr_t(new StatusBarTextView(this, GetScaledSize(80))));

        AddField(StatusBarField::Ptr_t(new StatusBarBitmapView(this, GetScaledSize(30))));

        ImageManager* bl = &gsgs_Image();
        m_bmpBuildError = bl->LoadBitmap("error");
        m_bmpBuildWarnings = bl->LoadBitmap("warning");
    }
    //-----------------------------------------------------------------
    StatusBar::~StatusBar()
    {
        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &StatusBar::OnPageChanged, this);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &StatusBar::OnTheme, this);
        gsgs_Event().Unbind(wxEVT_CMD_PAGE_CHANGED, &StatusBar::OnPageChanged, this);
        gsgs_Event().Unbind(wxEVT_ALL_EDITORS_CLOSED, &StatusBar::OnAllEditorsClosed, this);
        gsgs_Event().Unbind(wxEVT_BUILD_STARTED, &StatusBar::OnBuildStarted, this);
        gsgs_Event().Unbind(wxEVT_BUILD_ENDED, &StatusBar::OnBuildEnded, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED, &StatusBar::OnWorkspaceClosed, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &StatusBar::OnEditorSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SHOWING, &StatusBar::OnGotoAnythingShowing, this);
        gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &StatusBar::OnActionSelected, this);
        Unbind(wxEVT_STATUSBAR_CLICKED, &StatusBar::OnFieldClicked, this);
    }
    //-----------------------------------------------------------------
    void StatusBar::OnPageChanged(wxCommandEvent& event)
    {
        event.Skip();
        DoUpdateView();
    }
    //-----------------------------------------------------------------
    void StatusBar::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        DoUpdateColour();
    }
    //-----------------------------------------------------------------
    void StatusBar::DoUpdateColour()
    {
        /*IEditor* editor = m_mgr->GetActiveEditor();
        /*StatusBarView::Ptr_t art(NULL);
        if (editor)
        {
            wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
            if (Colour::IsDark(bgColour)) {
                // Using dark theme background
                art.reset(new DefaultStatusBarView());
                SetArt(art);
            }
            else {
                art.reset(new DefaultStatusBarView());
                SetArt(art);
            }
        }
        else
        {
            // Non editor, set "normal" art
            art.reset(new DefaultStatusBarView());
            SetArt(art);
        }*/
        SetArt(StatusBarView::Ptr_t(new DefaultStatusBarView()));
        Refresh();
    }
    //-----------------------------------------------------------------
    void StatusBar::SetState(const wxString& state)
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_STATE_IDX);
        CHECK_PTR_RET(field);

        //wxString ucLang = state.Upper();
        field->Cast<StatusBarTextView>()->SetText(state);
        field->SetTooltip(state);
    }
    //-----------------------------------------------------------------
    void StatusBar::SetLanguage(const wxString& lang)
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_LANG_COL_IDX);
        CHECK_PTR_RET(field);

        wxString ucLang = lang.Upper();
        field->Cast<StatusBarTextView>()->SetText(ucLang);
        field->SetTooltip(lang);
    }
    //-----------------------------------------------------------------
    void StatusBar::SetLinePosColumn(const wxString& lineCol)
    {
        CallAfter(&StatusBar::DoSetLinePosColumn, lineCol);
    }
    //-----------------------------------------------------------------
    void StatusBar::DoSetLinePosColumn(const wxString& message)
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_LINE_COL_IDX);
        CHECK_PTR_RET(field);

        field->Cast<StatusBarTextView>()->SetText(message);
        field->SetTooltip(message);
    }
    //-----------------------------------------------------------------
    void StatusBar::OnAllEditorsClosed(wxCommandEvent& event)
    {
        event.Skip();
        Clear();
    }
    //-----------------------------------------------------------------
    void StatusBar::Clear()
    {
        //SetMessage("");
        SetText("");
        SetBuildBitmap(wxNullBitmap, "");
        StopAnimation();
        SetLanguage("");
        ClearWhitespaceInfo();
        SetEncoding("");
    }
    //-----------------------------------------------------------------
    void StatusBar::OnBuildEnded(BuildEvent& event)
    {
        event.Skip();
        StopAnimation();
        if (event.GetErrorCount()) {
            SetBuildBitmap(m_bmpBuildError, _("Build ended with errors\nClick to view"));
        }
        else if (event.GetWarningCount()) {
            SetBuildBitmap(m_bmpBuildWarnings, _("Build ended with warnings\nClick to view"));
        }
        else {
            SetBuildBitmap(wxNullBitmap, "");
        }
    }
    //-----------------------------------------------------------------
    void StatusBar::OnBuildStarted(BuildEvent & event)
    {
        event.Skip();
        SetBuildBitmap(wxNullBitmap, "");
        StartAnimation(100, "");
    }
    //-----------------------------------------------------------------
    void StatusBar::SetBuildBitmap(const wxBitmap & bmp, const wxString & tooltip)
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_ICON_COL_IDX);
        CHECK_PTR_RET(field);
        field->Cast<StatusBarBitmapView>()->SetBitmap(bmp);
        field->SetTooltip(tooltip);
        Refresh();
    }
    //-----------------------------------------------------------------
    void StatusBar::OnWorkspaceClosed(wxCommandEvent & event)
    {
        event.Skip();
        Clear();
    }
    //-----------------------------------------------------------------
    void StatusBar::StartAnimation(long refreshRate, const wxString & tooltip)
    {
        wxUnusedVar(tooltip);
        StatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
        CHECK_PTR_RET(field);
        field->Cast<StatusBarAnimationView>()->Start(refreshRate);
        field->SetTooltip(_("Build is in progress\nClick to view the Build Log"));
    }
    //-----------------------------------------------------------------
    void StatusBar::StopAnimation()
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
        CHECK_PTR_RET(field);
        field->Cast<StatusBarAnimationView>()->Stop();
        field->SetTooltip("");
    }
    //-----------------------------------------------------------------
    void StatusBar::OnFieldClicked(Event& event) 
    { 
        DoFieldClicked(event.GetInt()); 
    }
    //-----------------------------------------------------------------
    void StatusBar::SetWhitespaceInfo()
    {
        IEditor* activeEditor = GetManager()->GetActiveEditor();
        CHECK_PTR_RET(activeEditor);

        wxString whitespaceInfo, eolMode;
        GetWhitespaceInfo(activeEditor->GetCtrl(), whitespaceInfo, eolMode);

        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
            CHECK_PTR_RET(field);

            wxString ws = whitespaceInfo.Upper();
            field->Cast<StatusBarTextView>()->SetText(ws);
            field->SetTooltip(ws);
        }

        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
            CHECK_PTR_RET(field);

            wxString ws = eolMode.Upper();
            field->Cast<StatusBarTextView>()->SetText(ws);
            field->SetTooltip(ws);
        }
    }
    //-----------------------------------------------------------------
    void StatusBar::SetSourceControlBitmap(const wxBitmap& bmp, const wxString& outputTabName, const wxString& tooltip)
    {
        m_sourceControlTabName = outputTabName;
        m_bmpSourceControl = bmp;

        StatusBarField::Ptr_t field = GetField(STATUSBAR_SCM_IDX);
        CHECK_PTR_RET(field);

        field->Cast<StatusBarBitmapView>()->SetBitmap(m_bmpSourceControl);
        field->Cast<StatusBarBitmapView>()->SetTooltip(tooltip);
    }
    //-----------------------------------------------------------------
    void StatusBar::ClearWhitespaceInfo()
    {
        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
            CHECK_PTR_RET(field);

            field->Cast<StatusBarTextView>()->SetText(wxEmptyString);
            field->SetTooltip(wxEmptyString);
        }

        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
            CHECK_PTR_RET(field);

            field->Cast<StatusBarTextView>()->SetText(wxEmptyString);
            field->SetTooltip(wxEmptyString);
        }
    }
    //-----------------------------------------------------------------
    void StatusBar::SetEncoding(const wxString& enc)
    {
        StatusBarField::Ptr_t field = GetField(STATUSBAR_ENCODING_COL_IDX);
        CHECK_PTR_RET(field);

        wxString ucEnc = enc.Upper();
        field->Cast<StatusBarTextView>()->SetText(ucEnc);
        field->SetTooltip(ucEnc);
    }
    //-----------------------------------------------------------------
    void StatusBar::OnEditorSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        DoUpdateView();
    }
    //-----------------------------------------------------------------
    void StatusBar::DoUpdateView()
    {
        DoUpdateColour();

        // Update the language
        wxString language = "TEXT";
        IEditor* editor = GetManager()->GetActiveEditor();
        if (editor) {
            LexerStyle::Ptr_t lexer = gsgs_Style().GetLexerForFile(editor->GetFileName().GetFullPath());
            if (lexer) { language = lexer->GetName().Upper(); }
        }

        SetLanguage(language);

        wxString encodingName = wxFontMapper::GetEncodingName(gsgs_EditorConfig().GetOptions()->GetFileFontEncoding());
        SetEncoding(encodingName);
        SetWhitespaceInfo();
    }
    //-----------------------------------------------------------------
    void StatusBar::DoFieldClicked(int fieldIndex)
    {
        switch (fieldIndex)
        {
        case STATUSBAR_SCM_IDX:
        {
            if (m_sourceControlTabName.IsEmpty()) 
                return;
            StatusBarField::Ptr_t field = GetField(STATUSBAR_SCM_IDX);
            CHECK_PTR_RET(field);
            // Open the output view only if the bitmap is valid
            if (field->Cast<StatusBarBitmapView>()->GetBitmap().IsOk()) {
                m_mgr->ToggleOutputPane(m_sourceControlTabName);
            }
        }
        break;
        case STATUSBAR_ICON_COL_IDX:
        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_ICON_COL_IDX);
            CHECK_PTR_RET(field);
            // Open the output view only if the bitmap is valid
            if (field->Cast<StatusBarBitmapView>()->GetBitmap().IsOk()) { m_mgr->ToggleOutputPane("Build"); }
        }
        break;
        case STATUSBAR_ANIMATION_COL_IDX:
        {
            StatusBarField::Ptr_t field = GetField(STATUSBAR_ANIMATION_COL_IDX);
            CHECK_PTR_RET(field);
            // Open the output view only if the bitmap is valid
            if (field->Cast<StatusBarAnimationView>()->IsRunning()) { m_mgr->ToggleOutputPane("Build"); }
        }
        break;
        case STATUSBAR_ENCODING_COL_IDX:
        {
            // Show encoding menu
            wxMenu menu;
            wxFontEncoding fontEnc;
            OptionsConfig * options = gsgs_EditorConfig().GetOptions();
            size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
            std::map<int, wxString> encodingMenuItems;
            for (size_t i = 0; i < iEncCnt; i++) {
                fontEnc = wxFontMapper::GetEncoding(i);
                if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                    continue;
                }

                wxString encodingName = wxFontMapper::GetEncodingName(fontEnc);
                wxMenuItem* itemEnc = menu.Append(wxID_ANY, encodingName, "", wxITEM_CHECK);
                itemEnc->Check(fontEnc == options->GetFileFontEncoding());
                encodingMenuItems.insert(std::make_pair(itemEnc->GetId(), encodingName));
            }
            int selectedId = GetPopupMenuSelectionFromUser(menu);
            if (encodingMenuItems.count(selectedId) == 0) return;

            // Change the encoding
            wxString selectedEncodingName = encodingMenuItems.find(selectedId)->second;
            ::SetEditorFontEncoding(selectedEncodingName);
            // Update the status bar
            SetEncoding(selectedEncodingName);

        }
        break;
        case STATUSBAR_LANG_COL_IDX:
        {
            if (m_mgr->GetActiveEditor()) {
                StatusBarField::Ptr_t field = GetField(STATUSBAR_LANG_COL_IDX);
                CHECK_PTR_RET(field);

                IEditor* editor = m_mgr->GetActiveEditor();
                LexerStyle::Ptr_t lexer = gsgs_Style().GetLexerForFile(editor->GetFileName().GetFullPath());
                if (lexer) {
                    wxMenu menu;
                    wxMenu* themesMenu = new wxMenu();
                    wxMenu* langMenu = new wxMenu();

                    // Create the themes menu
                    std::map<int, wxString> themesMap;
                    std::map<int, wxString> langsMap;
                    wxArrayString themesArr = gsgs_Style().GetAvailableThemesForLexer(lexer->GetName());
                    for (size_t i = 0; i < themesArr.size(); ++i) {
                        wxMenuItem* itemTheme = themesMenu->Append(wxID_ANY, themesArr.Item(i), "", wxITEM_CHECK);
                        itemTheme->Check(themesArr.Item(i) == lexer->GetThemeName());
                        themesMap.insert(std::make_pair(itemTheme->GetId(), themesArr.Item(i)));
                    }

                    // Create the language menu
                    wxArrayString langs = gsgs_Style().GetAllLexersNames();
                    for (size_t i = 0; i < langs.size(); ++i) {
                        wxMenuItem* itemLang = langMenu->Append(wxID_ANY, langs.Item(i), "", wxITEM_CHECK);
                        itemLang->Check(langs.Item(i) == lexer->GetName());
                        langsMap.insert(std::make_pair(itemLang->GetId(), langs.Item(i)));
                    }
                    menu.Append(wxID_ANY, _("Language"), langMenu);
                    menu.Append(wxID_ANY, _("Colour Themes"), themesMenu);
                    int selectedId = GetPopupMenuSelectionFromUser(menu);
                    if (themesMap.count(selectedId)) {
                        // change the colour theme
                        wxBusyCursor bc;
                        gsgs_Style().SetTheme(themesMap.find(selectedId)->second);
                        gsgs_Style().Save();

                    }
                    else if (langsMap.count(selectedId)) {
                        // change the syntax highlight for the file
                        wxBusyCursor bc;
                        editor->SetSyntaxHighlight(langsMap.find(selectedId)->second);
                        SetLanguage(langsMap.find(selectedId)->second.Upper());
                    }
                }
            }
        }
        break;
        case STATUSBAR_WHITESPACE_INFO_IDX:
        {
            if (m_mgr->GetActiveEditor()) {
                // show a popup menu when clicking on the TABS/SPACES
                // field
                StatusBarField::Ptr_t field = GetField(STATUSBAR_WHITESPACE_INFO_IDX);
                CHECK_PTR_RET(field);
                wxStyledTextCtrl* stc = m_mgr->GetActiveEditor()->GetCtrl();
                wxMenu menu;
                wxMenuItem* idConvertToTabs = menu.Append(wxID_ANY, "Convert Indentations to Tabs");
                wxMenuItem* idConvertToSpaces = menu.Append(wxID_ANY, "Convert Indentations to Spaces");
                menu.AppendSeparator();
                wxMenuItem* idUseTabs = menu.Append(wxID_ANY, "Use Tabs", "", wxITEM_CHECK);
                wxMenuItem* idUseSpaces = menu.Append(wxID_ANY, "Use Spaces", "", wxITEM_CHECK);

                // Check the proper tabs vs spaces option
                menu.Check(idUseSpaces->GetId(), !stc->GetUseTabs());
                menu.Check(idUseTabs->GetId(), stc->GetUseTabs());
                int selectedId = GetPopupMenuSelectionFromUser(menu);
                if (selectedId == wxID_NONE) return;

                if (selectedId == idConvertToTabs->GetId()) {
                    wxCommandEvent evt(wxEVT_MENU, XRCID("convert_indent_to_tabs"));
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
                }
                else if (selectedId == idConvertToSpaces->GetId()) {
                    wxCommandEvent evt(wxEVT_MENU, XRCID("convert_indent_to_spaces"));
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
                }
                else if (selectedId == idUseSpaces->GetId()) {
                    stc->SetUseTabs(false);

                    // Update the configuration
                    OptionsConfig * options = gsgs_EditorConfig().GetOptions();
                    options->SetIndentUsesTabs(false);
                    gsgs_EditorConfig().SetOptions(options);

                }
                else if (selectedId == idUseTabs->GetId()) {
                    stc->SetUseTabs(true);

                    // Update the configuration
                    OptionsConfig * options = gsgs_EditorConfig().GetOptions();
                    options->SetIndentUsesTabs(true);
                    gsgs_EditorConfig().SetOptions(options);
                }
                SetWhitespaceInfo();
            }
        }
        break;
        case STATUSBAR_EOL_COL_IDX:
        {
            if (m_mgr->GetActiveEditor()) {
                // show a popup menu
                StatusBarField::Ptr_t field = GetField(STATUSBAR_EOL_COL_IDX);
                CHECK_PTR_RET(field);

                wxStyledTextCtrl* stc = m_mgr->GetActiveEditor()->GetCtrl();
                wxMenu menu;

                wxMenuItem* idDisplayEOL = menu.Append(wxID_ANY, _("Display EOL"), "", wxITEM_CHECK);
                menu.AppendSeparator();

                wxMenuItem* idUseLf = menu.Append(wxID_ANY, _("Use Linux Format (LF)"), "", wxITEM_CHECK);
                wxMenuItem* idUseCrLf = menu.Append(wxID_ANY, _("Use Windows Format (CRLF)"), "", wxITEM_CHECK);
                menu.AppendSeparator();

                wxMenuItem* idConvertToCrLF = menu.Append(wxID_ANY, _("Convert to Windows Format"));
                wxMenuItem* idConvertToLf = menu.Append(wxID_ANY, _("Convert to Linux Format"));

                // Check the proper tabs vs spaces option
                menu.Check(idUseLf->GetId(), stc->GetEOLMode() == wxSTC_EOL_LF);
                menu.Check(idUseCrLf->GetId(), stc->GetEOLMode() == wxSTC_EOL_CRLF);
                menu.Check(idDisplayEOL->GetId(), stc->GetViewEOL());

                int selectedId = GetPopupMenuSelectionFromUser(menu);
                if (selectedId == wxID_NONE) return;

                if (selectedId == idConvertToCrLF->GetId()) {
                    // This will also change the EOL mode to CRLF
                    wxCommandEvent evt(wxEVT_MENU, XRCID("convert_eol_win"));
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
                }
                else if (selectedId == idConvertToLf->GetId()) {
                    // This will also change the EOL mode to LF
                    wxCommandEvent evt(wxEVT_MENU, XRCID("convert_eol_unix"));
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
                }
                else if (selectedId == idUseLf->GetId()) {
                    stc->SetEOLMode(wxSTC_EOL_LF);
                }
                else if (selectedId == idUseCrLf->GetId()) {
                    stc->SetEOLMode(wxSTC_EOL_CRLF);
                }
                else if (selectedId == idDisplayEOL->GetId()) {
                    wxCommandEvent evt(wxEVT_MENU, XRCID("display_eol"));
                    evt.SetInt(stc->GetViewEOL() ? 0 : 1);
                    wxTheApp->GetTopWindow()->GetEventHandler()->AddPendingEvent(evt);
                }

                // Update the whitespace info
                CallAfter(&StatusBar::SetWhitespaceInfo);
            }
        }
        break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------
    void StatusBar::OnGotoAnythingShowing(GotoEvent& e)
    {
        e.Skip();
        const wxBitmap& defaultBitmap = gsgs_Image().LoadBitmap("colour-pallette");
        m_gotoAnythingTableSyntax.clear();
        m_gotoAnythingTableThemes.clear();
        // Get list of languages
        wxArrayString langs = gsgs_Style().GetAllLexersNames();
        for (size_t i = 0; i < langs.size(); ++i) {
            wxString text;
            text << _("Set Syntax: ") << langs.Item(i);
            m_gotoAnythingTableSyntax[text] = langs.Item(i);
            GotoEntry entry(text, "", wxID_ANY);
            e.GetEntries().push_back(entry);
        }

        wxArrayString themes = gsgs_Style().GetAllThemes();
        for (size_t i = 0; i < themes.size(); ++i) {
            wxString text;
            text << _("Select Theme: ") << themes.Item(i);
            m_gotoAnythingTableThemes[text] = themes.Item(i);
            GotoEntry entry(text, "", wxID_ANY);
            entry.SetBitmap(defaultBitmap);
            e.GetEntries().push_back(entry);
        }
    }
    //-----------------------------------------------------------------
    void StatusBar::OnActionSelected(GotoEvent& e)
    {
        e.Skip();
        const GotoEntry& entry = e.GetEntry();
        const wxString& desc = entry.GetDesc();
        if (m_gotoAnythingTableSyntax.count(desc)) {
            IEditor* editor = GetManager()->GetActiveEditor();
            CHECK_PTR_RET(editor);
            wxBusyCursor bc;
            const wxString& lang = m_gotoAnythingTableSyntax[desc];

            editor->SetSyntaxHighlight(lang);
            SetLanguage(lang.Upper());
        }
        else if (m_gotoAnythingTableThemes.count(desc)) {
            gsgs_Style().SetTheme(m_gotoAnythingTableThemes[desc]);
            gsgs_Style().Save();
        }
    }
    //-----------------------------------------------------------------
    int StatusBar::GetTextWidth(const wxString& text) const
    {
        const int SPACER = 10;
        wxBitmap bmp(1, 1);
        wxMemoryDC memDc;
        memDc.SelectObject(bmp);
        wxGCDC gcdc(memDc);
        gcdc.SetFont(gsgs_Style().GetDefaultGuiFont());
        int textWidth = gcdc.GetTextExtent(text).GetWidth();
        textWidth += 2 * SPACER;
        return textWidth;
    }
    //-----------------------------------------------------------------
}