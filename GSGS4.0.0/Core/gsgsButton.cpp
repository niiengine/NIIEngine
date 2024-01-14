#include "gsgsButton.h"
#include "gsgsEvent.h"
#include "gsgsConfig.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/buffer.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/anybutton.h>
#include <wx/stockitem.h>
#include <wx/xrc/xmlres.h>

#define TEXT_SPACER 4

#ifdef __WXMSW__
#define BUTTON_RADIUS 0.0
#else
#define BUTTON_RADIUS 3.0
#endif
namespace gsgs
{
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // Button
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    Button::Button() {}
    //-----------------------------------------------------------------------------------------
    Button::Button(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
        const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
        m_buttonStyle(style)
    {
        wxWindowUpdateLocker ttl(this);
        wxControl::Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS, validator, name);
        if (label.empty() && wxIsStockID(id))
        {
            // On Windows, some buttons aren't supposed to have mnemonics
            SetText(wxGetStockLabel
            (
                id,
                id == wxID_OK || id == wxID_CANCEL || id == wxID_CLOSE
                ? wxSTOCK_NOFLAGS
                : wxSTOCK_WITH_MNEMONIC
            ));
        }
        else
        {
            SetText(label);
        }
        Initialise();
    }
    //-----------------------------------------------------------------------------------------
    bool Button::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
        const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    {
        m_buttonStyle = style;
        wxWindowUpdateLocker ta(this);
        if (wxControl::Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER | wxWANTS_CHARS, validator, name))
        { 
            if (label.empty() && wxIsStockID(id))
            {
                // On Windows, some buttons aren't supposed to have mnemonics
                SetText(wxGetStockLabel
                (
                    id,
                    id == wxID_OK || id == wxID_CANCEL || id == wxID_CLOSE
                    ? wxSTOCK_NOFLAGS
                    : wxSTOCK_WITH_MNEMONIC
                ));
            }
            else
            {
                SetText(label);
            }
            Initialise();
            return true; 
        }
        return false;
    }
    //-----------------------------------------------------------------------------------------
    Button::~Button() 
    { 
        Unbind(wxEVT_PAINT, &Button::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, &Button::OnErasebg, this);
        Unbind(wxEVT_BUTTON, &Button::OnButton, this);
        Unbind(wxEVT_LEFT_DOWN, &Button::OnLeftDown, this);
        Unbind(wxEVT_LEFT_DCLICK, &Button::OnLeftDown, this);
        Unbind(wxEVT_LEFT_UP, &Button::OnLeftUp, this);
        Unbind(wxEVT_MOUSE_CAPTURE_LOST, &Button::OnCaptureLost, this);
        Unbind(wxEVT_ENTER_WINDOW, &Button::OnEnter, this);
        Unbind(wxEVT_LEAVE_WINDOW, &Button::OnLeave, this);
        Unbind(wxEVT_SET_FOCUS, &Button::OnFocus, this);
        Unbind(wxEVT_KILL_FOCUS, &Button::OnFocus, this);
        Unbind(wxEVT_KEY_DOWN, &Button::OnKeyDown, this);
        Unbind(wxEVT_IDLE, &Button::OnIdle, this);
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC abdc(this);
        wxGCDC dc(abdc);
        PrepareDC(dc);
        Render(dc);

        //m_lastPaintFlags = GetDrawingFlags();

        if (HasFocus()) {
            wxRect clientRect = GetClientRect();
            wxRect focusRect = clientRect.Deflate(3);
            focusRect = focusRect.CenterIn(clientRect);
            wxRendererNative::GetGeneric().DrawFocusRect(this, abdc, focusRect, wxCONTROL_SELECTED);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnErasebg(wxEraseEvent& event) { wxUnusedVar(event); }
    //-----------------------------------------------------------------------------------------
    void Button::OnButton(wxCommandEvent& e)
    {
        e.Skip();
        e.SetId(GetId());
        e.SetEventObject(this);
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnLeftDown(wxMouseEvent& event)
    {
        event.Skip();

        if (!IsEnabled()) { return; }

        m_state = BST_Press;
        CaptureMouse();
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnLeftUp(wxMouseEvent& event)
    {
        event.Skip();
        
        if (HasCapture()) { ReleaseMouse(); }
        if (!IsEnabled()) { return; }

        wxRect rect = GetClientRect();
        if (rect.Contains(event.GetPosition())) {
            m_state = BST_Hover;
            PostClickEvent();
        }
        else {
            m_state = BST_Normal;
        }
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnCaptureLost(wxMouseCaptureLostEvent & e)
    {
        if (HasCapture()) { ReleaseMouse(); }
        if (IsEnabled())
        {
            m_state = BST_Normal;
        }
        //Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::Initialise()
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        m_colours.setBase(gsgs_Style().GetAppBgColour());

        Bind(wxEVT_PAINT, &Button::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, &Button::OnErasebg, this);
        Bind(wxEVT_BUTTON, &Button::OnButton, this);
        Bind(wxEVT_LEFT_DOWN, &Button::OnLeftDown, this);
        Bind(wxEVT_LEFT_DCLICK, &Button::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &Button::OnLeftUp, this);
        Bind(wxEVT_MOUSE_CAPTURE_LOST, &Button::OnCaptureLost, this);
        Bind(wxEVT_ENTER_WINDOW, &Button::OnEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &Button::OnLeave, this);
        Bind(wxEVT_SET_FOCUS, &Button::OnFocus, this);
        Bind(wxEVT_KILL_FOCUS, &Button::OnFocus, this);
        Bind(wxEVT_KEY_DOWN, &Button::OnKeyDown, this);
        Bind(wxEVT_IDLE, &Button::OnIdle, this);

        if (m_hasDropDownMenu)
            SetSizeHints(GetBestSize(), GetBestSize());
        else
            SetSizeHints(GetBestSize());
    }
    //-----------------------------------------------------------------------------------------
    void Button::Render(wxDC& dc)
    {
        wxRect clientRect = GetClientRect();
        wxRect rect = clientRect;
#ifdef __WXOSX__
        clientRect.Inflate(1);
#endif

        dc.SetBrush(m_colours.GetBgColour());
        dc.SetPen(m_colours.GetBgColour());
        dc.DrawRectangle(clientRect);

        bool isDisabled = !IsEnabled();
        bool isDark = Colour::IsDark(m_colours.GetBgColour());
        wxColour bgColour = m_colours.GetBgColour();
        wxColour borderColour = gsgs_Style().GetAppTextColour();
        switch (m_state) {
        case BST_Normal:
        case BST_Hover:
            bgColour = bgColour.ChangeLightness(isDark ? 80 : 95);
            break;
        case BST_Press:
            bgColour = bgColour.ChangeLightness(isDark ? 70 : 80);
            break;
        }

        if (isDark) {
            borderColour = bgColour.ChangeLightness(90);
        }

        // Draw the background
        if (isDisabled) 
        {
            bgColour = bgColour.ChangeLightness(110);
            borderColour = borderColour.ChangeLightness(110);
            // Draw the button border
            dc.SetPen(borderColour);
            dc.SetBrush(bgColour);
            dc.DrawRoundedRectangle(rect, BUTTON_RADIUS);
        }
        else
        {
            if (IsNormal()){
                // fill the button bg colour with gradient
                dc.GradientFillLinear(rect, bgColour, m_colours.GetBgColour(), wxNORTH);
                // draw the border
                dc.SetPen(borderColour);
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRoundedRectangle(rect, BUTTON_RADIUS);
            }
            else if(IsHover())
            {
                borderColour = borderColour.ChangeLightness(110);
                // fill the button bg colour with gradient
                dc.GradientFillLinear(rect, bgColour, borderColour, wxNORTH);
                // draw the border
                dc.SetPen(borderColour);
                dc.SetBrush(*wxTRANSPARENT_BRUSH);
                dc.DrawRoundedRectangle(rect, BUTTON_RADIUS);
            }
            else if (m_state == BST_Press) {
                // pressed button is drawns with flat bg colour and border
                wxColour pressedBgColour = bgColour.ChangeLightness(90);
                dc.SetPen(borderColour);
                dc.SetBrush(pressedBgColour);
                dc.DrawRoundedRectangle(rect, BUTTON_RADIUS);
            }
        }

        if (!isDisabled && !IsPressed()) {
            wxColour topLineColour = bgColour.ChangeLightness(115);
            wxRect rr = rect;
            rr.Deflate(2);
            dc.SetPen(topLineColour);
            dc.DrawLine(rr.GetTopLeft(), rr.GetTopRight());
        }

        // Draw the bitmap first
        int xx = TEXT_SPACER;
        if (GetBitmap().IsOk()) {
            wxRect bitmapRect(xx, 0, GetBitmap().GetScaledWidth(), GetBitmap().GetScaledHeight());
            bitmapRect = bitmapRect.CenterIn(rect, wxVERTICAL);
            dc.DrawBitmap(GetBitmap(), bitmapRect.GetTopLeft());
            xx += bitmapRect.GetWidth();
            xx += TEXT_SPACER;
        }

        // Draw the text
        int textWidth = rect.GetWidth() - TEXT_SPACER;
        if (GetBitmap().IsOk()) {
            // Reduce the space used by the bitmap
            textWidth -= GetBitmap().GetScaledWidth();
            textWidth -= TEXT_SPACER;
        }

        // Setup some colours (text and dropdown)
        wxColour textColour = m_colours.GetItemTextColour();
        wxColour dropDownColour = gsgs_Style().GetActiveMarkerColour();
        if (isDisabled) {
            dropDownColour = textColour = m_colours.GetGrayText();
        }
        else if (IsHover())
        {
            textColour = gsgs_Style().GetHoverMarkerColour();
        }
        else if (IsPressed()) {
            //textColour = textColour.ChangeLightness(isDark ? 70 : 110);
            textColour = gsgs_Style().GetActiveMarkerColour();
            dropDownColour = dropDownColour.ChangeLightness(isDark ? 70 : 110);
        }

        if (HasDropDownMenu()) { textWidth -= rect.GetHeight(); }
        wxRect textBoundingRect(xx, 0, textWidth, rect.GetHeight());
        wxRect arrowRect;
        if (HasDropDownMenu()) {
            arrowRect.SetX(textBoundingRect.GetRight() + TEXT_SPACER);
            arrowRect.SetY(0);
            arrowRect.SetWidth(rect.GetHeight());
            arrowRect.SetHeight(rect.GetHeight());
        }

        if (GetText().size()) {
            //textBoundingRect = textBoundingRect.CenterIn(rect, wxVERTICAL);
            dc.SetFont(gsgs_Style().GetDefaultGuiFont());
            dc.SetTextForeground(textColour);
            dc.SetClippingRegion(textBoundingRect);
            // Truncate the text to fit the drawing area
            wxString fixedText;
            gsgs_Style().TruncateText(GetText(), textBoundingRect.GetWidth(), dc, fixedText);

            dc.DrawLabel(fixedText, textBoundingRect, wxALIGN_CENTRE);
            dc.DestroyClippingRegion();
        }

        if (HasDropDownMenu()) {
            // Draw an arrow
            int arrowHeight = arrowRect.GetHeight() / 4;
            int arrowWidth = arrowRect.GetWidth() / 3;
            wxRect r(0, 0, arrowWidth, arrowHeight);
            r = r.CenterIn(arrowRect);

            wxPoint downCenterPoint = wxPoint(r.GetLeft() + r.GetWidth() / 2, r.GetBottom());
            dc.SetPen(wxPen(dropDownColour, 2));
            dc.DrawLine(r.GetTopLeft(), downCenterPoint);
            dc.DrawLine(r.GetTopRight(), downCenterPoint);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnEnter(wxMouseEvent& event)
    {
        event.Skip();
        if (!HasCapture()) {
            m_state = BST_Hover;
            Refresh();
        }
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnLeave(wxMouseEvent& event)
    {
        event.Skip();
        if (!HasCapture()) {
            m_state = BST_Normal;
            Refresh();
        }
    }
    //-----------------------------------------------------------------------------------------
    wxSize Button::GetBestSize() const
    {
        if (GetText().IsEmpty())
            return GetSize();
        wxBitmap bmp(1, 1);
        wxMemoryDC memDC(bmp);
        wxGCDC dc(memDC);
        dc.SetFont(gsgs_Style().GetDefaultGuiFont());

        int buttonHeight = GetSize().GetHeight();
        int buttonWidth = 0;
        {
            wxRect r = dc.GetTextExtent("Tp");
            r.Inflate(4);

            if(r.GetHeight() > buttonHeight)
                buttonHeight = r.GetHeight();
        }

        wxString sampleText;
        if (m_buttonStyle & wxBU_EXACTFIT) {
            sampleText = m_text;
        }
        else {
            sampleText = m_text + "TpTp";
        }

        wxRect sampleTextRect = dc.GetTextExtent(sampleText);
        //wxRect textRect = dc.GetTextExtent(m_text);

        // If the text does not fit into the default button size, increase it
        buttonWidth += TEXT_SPACER;

        // Add bitmap length
        if (GetBitmap().IsOk()) {
            buttonWidth += GetBitmap().GetScaledWidth();
            buttonWidth += TEXT_SPACER;
        }
        // Text width
        //buttonWidth += wxMax(textRect.GetWidth(), sampleTextRect.GetWidth());
        buttonWidth += sampleTextRect.GetWidth();
        buttonWidth += TEXT_SPACER;

        // Drop down arrow width
        if (HasDropDownMenu()) { buttonWidth += buttonHeight; }
        return wxSize(buttonWidth, buttonHeight);
    }
    //-----------------------------------------------------------------------------------------
    void Button::SetColours(const Colour& colours)
    {
        m_colours = colours;
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnFocus(wxFocusEvent& event)
    {
        //event.Skip();
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnKeyDown(wxKeyEvent& event)
    {
        event.Skip();
        if ((event.GetKeyCode() == WXK_SPACE) || (event.GetKeyCode() == WXK_NUMPAD_ENTER) ||
            (event.GetKeyCode() == WXK_RETURN)) {
            PostClickEvent();
            event.Skip(false);
        }
        else if (event.GetKeyCode() == WXK_TAB) {
            Navigate(event.ShiftDown() ? wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Button::PostClickEvent()
    {
        wxCommandEvent eventClick(wxEVT_BUTTON);
        eventClick.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(eventClick);
    }
    //-----------------------------------------------------------------------------------------
    void Button::OnIdle(wxIdleEvent& event)
    {
        //event.Skip();
        size_t flags = GetDrawingFlags();
        if (flags != m_lastPaintFlags) {
            // We need to refresh the window
            Refresh();
            m_lastPaintFlags = flags;
        }
    }
    //-----------------------------------------------------------------------------------------
    size_t Button::GetDrawingFlags() const
    {
        size_t flags = 0;
        if (IsEnabled()) { flags |= kDrawingFlagEnabled; }
        return flags;
    }
    //-----------------------------------------------------------------------------------------
    void Button::SetText(const wxString& text)
    {
        // strip menemonics, not very efficient...
        m_text = text;
        m_text.Replace("&&", "@@");
        m_text.Replace("&", "");
        m_text.Replace("@@", "&");
        wxWindowUpdateLocker ta(this);
        if(m_hasDropDownMenu)
            SetSizeHints(GetBestSize(), GetBestSize());
        else
            SetSizeHints(GetBestSize());
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::SetDefault() {}
    //-----------------------------------------------------------------------------------------
    void Button::SetHasDropDownMenu(bool hasDropDownMenu)
    {
        m_hasDropDownMenu = hasDropDownMenu;
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    void Button::SetBitmap(const wxBitmap& bmp)
    {
        m_bitmap = bmp;
        wxWindowUpdateLocker ta(this);
        if (m_hasDropDownMenu)
            SetSizeHints(GetBestSize(), GetBestSize());
        else
            SetSizeHints(GetBestSize());
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    const wxBitmap& Button::GetBitmap() const { return m_bitmap; }
    //-----------------------------------------------------------------------------------------
    void Button::ShowMenu(wxMenu& menu)
    {
        SetPressed();
        Refresh();

        wxPoint menuPos = GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif
        PopupMenu(&menu, menuPos);
        SetNormal();
        Refresh();
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // DefaultButton
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    DefaultButton::DefaultButton() {}
    //-----------------------------------------------------------------------------------------
    bool DefaultButton::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
        const wxSize& size, long style, const wxValidator& validator, const wxString& name)
    {
        bool res = Button::Create(parent, id, label, pos, size, style, validator, name);
        if (res) {
            gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultButton::OnTheme, this);
            ApplyTheme();
        }

        return res;
    }
    //-----------------------------------------------------------------------------------------
    DefaultButton::DefaultButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos,
        const wxSize& size, long style, const wxValidator& validator, const wxString& name)
        : Button(parent, id, label, pos, size, style, validator, name)
    {
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultButton::OnTheme, this);
        ApplyTheme();
    }
    //-----------------------------------------------------------------------------------------
    DefaultButton::~DefaultButton()
    {
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &DefaultButton::OnTheme, this);
    }
    //-----------------------------------------------------------------------------------------
    void DefaultButton::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        ApplyTheme();
    }
    //-----------------------------------------------------------------------------------------
    void DefaultButton::ApplyTheme()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        Colour colours;
        if (lexer->IsDark()) {
            colours.setBase(gsgs_Style().GetAppBgColour());
        }
        else {
            colours.setDefault();
        }

        SetColours(colours);
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    //Choice
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_CHOICE_MENU_SHOWING, wxNotifyEvent);
    //-----------------------------------------------------------------------------------------
    Choice::Choice() {}
    //-----------------------------------------------------------------------------------------
    Choice::~Choice() { Unbind(wxEVT_BUTTON, &Choice::OnClick, this); }
    //-----------------------------------------------------------------------------------------
    bool Choice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    {
        m_choices.insert(m_choices.end(), choices.begin(), choices.end());
        wxString initialValue;
        if (!choices.IsEmpty()) {
            m_selection = 0;
            initialValue = m_choices[0];
        }
        m_hasDropDownMenu = true;
        if (!Button::Create(parent, id, initialValue, pos, size, style, validator, name)) { return false; }
        Bind(wxEVT_BUTTON, &Choice::OnClick, this);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    Choice::Choice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }
    //-----------------------------------------------------------------------------------------
    int Choice::FindString(const wxString& s, bool caseSensitive) const
    {
        for (size_t i = 0; i < m_choices.size(); ++i) {
            if (caseSensitive) {
                if (m_choices[i] == s) { return i; }
            }
            else {
                if (s.CmpNoCase(m_choices[i]) == 0) { return i; }
            }
        }
        return wxNOT_FOUND;
    }
    //-----------------------------------------------------------------------------------------
    wxString Choice::GetString(size_t index) const
    {
        if (index >= m_choices.size()) { return ""; }
        return m_choices[index];
    }
    //-----------------------------------------------------------------------------------------
    void Choice::SetSelection(size_t index)
    {
        if (index < m_choices.size())
        {
            m_selection = index;
            SetText(m_choices[m_selection]);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Choice::SetString(size_t index, const wxString& str)
    {
        if (index < m_choices.size())
        {
            m_choices[index] = str;
            // if we are updating the selected item, refresh the view
            if (index == (size_t)m_selection) { SetText(m_choices[m_selection]); }
        }
    }
    //-----------------------------------------------------------------------------------------
    void Choice::SetStringSelection(const wxString& str)
    {
        int where = FindString(str, true);
        if (where != wxNOT_FOUND) 
        { 
            m_selection = where;
            SetText(m_choices[m_selection]);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Choice::OnClick(wxCommandEvent&)
    {
        // Check if the user allows us to disable the action
        wxNotifyEvent eventShowing(wxEVT_CHOICE_MENU_SHOWING);
        eventShowing.SetEventObject(this);
        GetEventHandler()->ProcessEvent(eventShowing);
        if (!eventShowing.IsAllowed()) { return; }

        wxMenu menu;
        int selectedIndex = wxNOT_FOUND;
        std::unordered_map<int, int> idToIndex;
        for (size_t i = 0; i < m_choices.size(); ++i) {
            int menuId = wxXmlResource::GetXRCID(m_choices[i]);
            const wxString& label = m_choices[i];
            wxMenuItem* item = menu.Append(menuId, label, label, wxITEM_CHECK);
            item->Check((int)i == m_selection);
            idToIndex.insert({ menuId, i });
            menu.Bind(wxEVT_MENU,
                [&](wxCommandEvent& e) {
                if (idToIndex.count(e.GetId())) { selectedIndex = idToIndex[e.GetId()]; }
            },
                menuId);
        }

        // Show the menu
        ShowMenu(menu);

        // Update the button label
        if (selectedIndex != wxNOT_FOUND) {
            SetSelection(selectedIndex);
            // Fire event
            wxCommandEvent evt(wxEVT_CHOICE);
            evt.SetEventObject(this);
            evt.SetInt(GetSelection());
            GetEventHandler()->AddPendingEvent(evt);
        }
    }
    //-----------------------------------------------------------------------------------------
    void Choice::Render(wxDC& dc)
    {
        if (m_popupShown) 
        { 
            SetPressed(); 
        }
        Button::Render(dc);
    }
    //-----------------------------------------------------------------------------------------
    int Choice::Append(const wxString& str)
    {
        m_choices.push_back(str);
        return (m_choices.size() - 1);
    }
    //-----------------------------------------------------------------------------------------
    void Choice::Append(const wxArrayString& items) 
    { 
        m_choices.insert(m_choices.end(), items.begin(), items.end()); 
    }
    //-----------------------------------------------------------------------------------------
    void Choice::Clear()
    {
        m_choices.clear();
        SetText("");
    }
    //-----------------------------------------------------------------------------------------
    void Choice::Set(const wxArrayString& items, const wxString & text)
    {
        m_choices.clear();
        Append(items);
        SetText(text);
    }
    //-----------------------------------------------------------------------------------------
    DefaultChoice::DefaultChoice() {}
    //-----------------------------------------------------------------------------------------
    DefaultChoice::~DefaultChoice()
    {
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &DefaultChoice::OnTheme, this);
    }
    //-----------------------------------------------------------------------------------------
    bool DefaultChoice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    {
        bool res = Choice::Create(parent, id, pos, size, choices, style, validator, name);
        if (res) {
            gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultChoice::OnTheme, this);
            ApplyTheme();
        }
        return res;
    }
    //-----------------------------------------------------------------------------------------
    DefaultChoice::DefaultChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
        const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
        : Choice(parent, id, pos, size, choices, style, validator, name)
    {
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultChoice::OnTheme, this);
        ApplyTheme();
    }
    //-----------------------------------------------------------------------------------------
    void DefaultChoice::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        ApplyTheme();
    }
    //-----------------------------------------------------------------------------------------
    void DefaultChoice::ApplyTheme()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        Colour colours;
        if (lexer->IsDark()) {
            colours.setBase(gsgs_Style().GetAppBgColour());
        }
        else {
            colours.setDefault();
        }

        SetColours(colours);
    }
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    // DialogButtonSizer
    //-----------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    IMPLEMENT_CLASS(DialogButtonSizer, wxBoxSizer)
    DialogButtonSizer::DialogButtonSizer()
        : wxBoxSizer(wxHORIZONTAL)
    {
        // Vertical buttons with lots of space on either side
        // looks rubbish on WinCE, so let's not do this for now.
        // If we are going to use vertical buttons, we should
        // put the sizer to the right of other controls in the dialog,
        // and that's beyond the scope of this sizer.
#ifndef __WXWINCE__
        bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
        // If we have a PDA screen, put yes/no button over
        // all other buttons, otherwise on the left side.
        if (is_pda)
            m_orient = wxVERTICAL;
#endif

        m_buttonAffirmative = NULL;
        m_buttonApply = NULL;
        m_buttonNegative = NULL;
        m_buttonCancel = NULL;
        m_buttonHelp = NULL;
    }

    void DialogButtonSizer::AddButton(Button *mybutton)
    {
        switch (mybutton->GetId())
        {
        case wxID_OK:
        case wxID_YES:
        case wxID_SAVE:
            m_buttonAffirmative = mybutton;
            break;
        case wxID_APPLY:
            m_buttonApply = mybutton;
            break;
        case wxID_NO:
            m_buttonNegative = mybutton;
            break;
        case wxID_CANCEL:
        case wxID_CLOSE:
            m_buttonCancel = mybutton;
            break;
        case wxID_HELP:
        case wxID_CONTEXT_HELP:
            m_buttonHelp = mybutton;
            break;
        default:
            break;
        }
    }

    void DialogButtonSizer::SetAffirmativeButton(Button *button)
    {
        m_buttonAffirmative = button;
    }

    void DialogButtonSizer::SetNegativeButton(Button *button)
    {
        m_buttonNegative = button;
    }

    void DialogButtonSizer::SetCancelButton(Button *button)
    {
        m_buttonCancel = button;
    }

    void DialogButtonSizer::Realize()
    {
#ifdef __WXMAC__
        Add(0, 0, 0, wxLEFT, 6);
        if (m_buttonHelp)
            Add((wxWindow*)m_buttonHelp, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 6);

        if (m_buttonNegative) {
            // HIG POLICE BULLETIN - destructive buttons need extra padding
            // 24 pixels on either side
            Add((wxWindow*)m_buttonNegative, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 12);
        }

        // extra whitespace between help/negative and cancel/ok buttons
        Add(0, 0, 1, wxEXPAND, 0);

        if (m_buttonCancel) {
            Add((wxWindow*)m_buttonCancel, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 6);
            // Cancel or help should be default
            // m_buttonCancel->SetDefaultButton();
        }

        // Ugh, Mac doesn't really have apply dialogs, so I'll just
        // figure the best place is between Cancel and OK
        if (m_buttonApply)
            Add((wxWindow*)m_buttonApply, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, 6);

        if (m_buttonAffirmative) {
            Add((wxWindow*)m_buttonAffirmative, 0, wxALIGN_CENTRE | wxLEFT, 6);

            if (m_buttonAffirmative->GetId() == wxID_SAVE) {
                // these buttons have set labels under Mac so we should use them
                m_buttonAffirmative->SetLabel(_("Save"));
                if (m_buttonNegative)
                    m_buttonNegative->SetLabel(_("Don't Save"));
            }
        }

        // Extra space around and at the right
        Add(12, 40);
#elif defined(__WXGTK20__)
        // http://library.gnome.org/devel/hig-book/stable/windows-alert.html.en
        // says that the correct button order is
        //
        //      [Help]                  [Alternative] [Cancel] [Affirmative]

        // Flags ensuring that margins between the buttons are 6 pixels.
        const wxSizerFlags
            flagsBtn = wxSizerFlags().Centre().Border(wxLEFT | wxRIGHT, 3);

        // Margin around the entire sizer button should be 12.
        AddSpacer(9);

        if (m_buttonHelp)
            Add(m_buttonHelp, flagsBtn);

        // Align the rest of the buttons to the right.
        AddStretchSpacer();

        if (m_buttonNegative)
            Add(m_buttonNegative, flagsBtn);

        if (m_buttonApply)
            Add(m_buttonApply, flagsBtn);

        if (m_buttonCancel)
            Add(m_buttonCancel, flagsBtn);

        if (m_buttonAffirmative)
            Add(m_buttonAffirmative, flagsBtn);

        // Ensure that the right margin is 12 as well.
        AddSpacer(9);
#elif defined(__WXMSW__)
        // Windows

        // right-justify buttons
        Add(0, 0, 1, wxEXPAND, 0);

        if (m_buttonAffirmative) {
            Add((wxWindow*)m_buttonAffirmative, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonAffirmative->ConvertDialogToPixels(wxSize(2, 0)).x);
        }

        if (m_buttonNegative) {
            Add((wxWindow*)m_buttonNegative, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonNegative->ConvertDialogToPixels(wxSize(2, 0)).x);
        }

        if (m_buttonCancel) {
            Add((wxWindow*)m_buttonCancel, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonCancel->ConvertDialogToPixels(wxSize(2, 0)).x);
        }
        if (m_buttonApply)
            Add((wxWindow*)m_buttonApply, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonApply->ConvertDialogToPixels(wxSize(2, 0)).x);

        if (m_buttonHelp)
            Add((wxWindow*)m_buttonHelp, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonHelp->ConvertDialogToPixels(wxSize(2, 0)).x);
#else
        // GTK+1 and any other platform

        // Add(0, 0, 0, wxLEFT, 5); // Not sure what this was for but it unbalances the dialog
        if (m_buttonHelp)
            Add((wxWindow*)m_buttonHelp, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonHelp->ConvertDialogToPixels(wxSize(4, 0)).x);

        // extra whitespace between help and cancel/ok buttons
        Add(0, 0, 1, wxEXPAND, 0);

        if (m_buttonApply)
            Add((wxWindow*)m_buttonApply, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonApply->ConvertDialogToPixels(wxSize(4, 0)).x);

        if (m_buttonAffirmative) {
            Add((wxWindow*)m_buttonAffirmative, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonAffirmative->ConvertDialogToPixels(wxSize(4, 0)).x);
        }

        if (m_buttonNegative) {
            Add((wxWindow*)m_buttonNegative, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonNegative->ConvertDialogToPixels(wxSize(4, 0)).x);
        }

        if (m_buttonCancel) {
            Add((wxWindow*)m_buttonCancel, 0, wxALIGN_CENTRE | wxLEFT | wxRIGHT, m_buttonCancel->ConvertDialogToPixels(wxSize(4, 0)).x);
            // Cancel or help should be default
            // m_buttonCancel->SetDefaultButton();
        }

#endif
    }
}

