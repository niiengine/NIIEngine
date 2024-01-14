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

#include "gsgsScrolledPanel.h"
#include "gsgsNotebook.h"
#include "gsgsFileUtil.h"
#include "gsgsImageManager.h"
#include <wx/dcscreen.h>
#include <wx/settings.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/wupdlock.h>

#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef __WXGTK__
#include <gtk/gtk.h>
#endif

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_CUSTOM_SCROLL, ScrollEvent);
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    // ScrollEvent
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    ScrollEvent::ScrollEvent(wxEventType commandType, int winid) : 
        wxCommandEvent(commandType, winid)
    {
    }
    //----------------------------------------------------------------------------------
    ScrollEvent::ScrollEvent(const ScrollEvent & event) : 
        wxCommandEvent(event)
    {
        *this = event;
    }
    //----------------------------------------------------------------------------------
    ScrollEvent::~ScrollEvent() {}
    //----------------------------------------------------------------------------------
    ScrollEvent& ScrollEvent::operator=(const ScrollEvent& src)
    {
        // wxCommandEvent operator =
        m_id = src.m_id;
        m_eventType = src.m_eventType;
        m_cmdString = src.m_cmdString;
        m_commandInt = src.m_commandInt;
        m_extraLong = src.m_extraLong;
        m_clientData = src.m_clientData;
        m_clientObject = src.m_clientObject;

        // this class members
        m_position = src.m_position;
        return *this;
    }
    //----------------------------------------------------------------------------------
    wxEvent * ScrollEvent::Clone() const { return new ScrollEvent(*this); }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    // CustomScrollBar
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    CustomScrollBar::CustomScrollBar(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
        //wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxBORDER_NONE | wxWANTS_CHARS),
        m_sbStyle(style)
    {
        Hide();
        SetAutoLayout(false);
        wxPanel::Create(parent, id, pos, size, wxTAB_TRAVERSAL | wxBORDER_NONE | wxWANTS_CHARS);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        if (style == wxSB_HORIZONTAL)
        {
            SetSize(-1, SB_WIDTH);
        }
        else
        {
            SetSize(SB_WIDTH, -1);
        }
        Bind(wxEVT_PAINT, &CustomScrollBar::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        Bind(wxEVT_LEFT_DOWN, &CustomScrollBar::OnMouseLeftDown, this);
        Bind(wxEVT_LEFT_UP, &CustomScrollBar::OnMouseLeftUp, this);
        Bind(wxEVT_MOUSE_CAPTURE_LOST, &CustomScrollBar::OnMouseCaptureLost, this);
        Bind(wxEVT_MOTION, &CustomScrollBar::OnMotion, this);
        Bind(wxEVT_IDLE, &CustomScrollBar::OnIdle, this);
        m_colours.setBase(gsgs_Style().GetAppBgColour());
    }
    //----------------------------------------------------------------------------------
    CustomScrollBar::~CustomScrollBar()
    {
        Unbind(wxEVT_PAINT, &CustomScrollBar::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        Unbind(wxEVT_LEFT_DOWN, &CustomScrollBar::OnMouseLeftDown, this);
        Unbind(wxEVT_LEFT_UP, &CustomScrollBar::OnMouseLeftUp, this);
        Unbind(wxEVT_MOUSE_CAPTURE_LOST, &CustomScrollBar::OnMouseCaptureLost, this);
        Unbind(wxEVT_MOTION, &CustomScrollBar::OnMotion, this);
        Unbind(wxEVT_IDLE, &CustomScrollBar::OnIdle, this);
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::UpdateScroll(int thumbSize, int range, int pageSize, int position, bool refesh)
    {
        m_thumbSize = thumbSize;
        m_range = range;
        m_pageSize = pageSize;
        // Set position triggers a refresh
        SetPosition(position, refesh);
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnPaint(wxPaintEvent& e)
    {
        wxAutoBufferedPaintDC dc1(this);
        wxGCDC dc(dc1);
        wxRect rect = GetClientRect();

        dc.SetBrush(m_colours.GetBgColour());
        dc.SetPen(m_colours.GetBgColour());
        dc.DrawRectangle(rect);

        if (mCornerSet)
        {
            const int radius = SB_RADIUS;
            dc.SetBrush(mCornerColour);
            dc.SetPen(mCornerColour);
            if(mCornerTypeMark & CT_LT)
                dc.DrawRectangle(wxRect(0, 0, radius, radius));
            if (mCornerTypeMark & CT_RT)
                dc.DrawRectangle(wxRect(rect.GetWidth() - radius, 0, radius, radius));
            if (mCornerTypeMark & CT_LB)
                dc.DrawRectangle(wxRect(0, rect.GetHeight() - radius, radius, radius));
            if (mCornerTypeMark & CT_RB)
                dc.DrawRectangle(wxRect(rect.GetWidth() - radius, rect.GetHeight() - radius, radius, radius));
        }

        dc.SetBrush(gsgs_Style().GetAppTextColour());
        dc.SetPen(gsgs_Style().GetAppTextColour());
        dc.DrawRoundedRectangle(rect, SB_RADIUS);
        rect.Deflate(1);
        dc.SetBrush(m_colours.GetBgColour());
        dc.SetPen(m_colours.GetBgColour());
        dc.DrawRoundedRectangle(rect, SB_RADIUS);

        if(!m_thumbRect.IsEmpty()) {
            rect = m_thumbRect;
            rect.Deflate(1);
            dc.SetPen(gsgs_Style().GetActiveMarkerColour());
            dc.SetBrush(gsgs_Style().GetActiveMarkerColour());
            dc.DrawRoundedRectangle(rect, SB_RADIUS);
        }
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnMouseLeftDown(wxMouseEvent& e)
    {
        //e.Skip();
        if(m_thumbRect.Contains(e.GetPosition())) {
            // Mouse down is inside the thumb rect
            m_mouseCapturePoint = e.GetPosition();
            m_thumbCapturePoint = m_thumbRect.GetTopLeft();
            CaptureMouse();
            mDragging = true;
        }
        else
        {
            m_mouseCapturePoint = wxPoint();
            mDragging = false;
            mClick = true;
        }
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::UpdateDrag(const wxPoint& pt)
    {
        wxRect rect = GetClientRect();
        wxRect temprec(m_thumbRect);
        if(IsVertical()) {
            int offset = pt.y - m_mouseCapturePoint.y;
            m_thumbRect.SetTop(m_thumbCapturePoint.y + offset);
            if(m_thumbRect.GetBottom() >= rect.GetHeight()) {
                m_thumbRect.SetY(rect.GetHeight() - m_thumbRect.GetHeight());
            } else if(m_thumbRect.GetY() < 0) {
                m_thumbRect.SetY(0);
            }
        } else {
            int offset = pt.x - m_mouseCapturePoint.x;
            m_thumbRect.SetLeft(m_thumbCapturePoint.x + offset);
            if(m_thumbRect.GetRight() >= rect.GetWidth()) {
                m_thumbRect.SetX(rect.GetWidth() - m_thumbRect.GetWidth());
            } else if(m_thumbRect.GetX() < 0) {
                m_thumbRect.SetX(0);
            }
        }
        if (m_thumbRect == temprec)
            return;
        Refresh();
        int pos = GetPositionFromPoint(m_thumbRect.GetTopLeft());
        if(m_thumbPosition != pos) {
            m_thumbPosition = pos;
            mNotifyContent = true;
        }
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnMouseLeftUp(wxMouseEvent& e)
    {
        //e.Skip();
        // Calculate the new starting position
        if(HasCapture()) { ReleaseMouse(); }

        if(mDragging) 
        {
            UpdateDrag(e.GetPosition());
        } 
        else if (mClick)
        {
            int pos = GetPositionFromPoint(e.GetPosition());
            if(m_thumbPosition != pos) { SetPosition(pos, true); }
            mClick = false;
        }
        //else 
        //{
        //    int pos = GetPositionFromPoint(e.GetPosition());
        //    if(m_thumbPosition != pos) { SetPosition(pos, true); }
        //}
        m_mouseCapturePoint = wxPoint();
        m_thumbCapturePoint = wxPoint();
        mDragging = false;
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnMouseCaptureLost(wxMouseCaptureLostEvent & e)
    {
        //e.Skip();
        // Calculate the new starting position
        if (HasCapture()) { ReleaseMouse(); }

        if (mDragging)
        {
            int pos = GetPositionFromPoint(m_thumbCapturePoint);
            SetPosition(pos, true);
        }
        m_mouseCapturePoint = wxPoint();
        m_thumbCapturePoint = wxPoint();
        mDragging = false;
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnMotion(wxMouseEvent& e)
    {
        //e.Skip();
        if(mDragging && wxGetMouseState().LeftIsDown()) 
        { 
            //CallAfter(&CustomScrollBar::UpdateDrag, e.GetPosition()); 
            UpdateDrag(e.GetPosition());
        }
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::SetColours(const Colour& c)
    {
        m_colours = c;
        Refresh();
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::SetColour(const wxColour& c)
    {
        m_colours.setBase(c);
        Refresh();
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::SetConrnerColour(int cornermark, const wxColour& colours)
    {
        mCornerTypeMark = cornermark;
        if (m_colours.GetBgColour() != mCornerColour)
        {
            mCornerColour = colours;
            mCornerSet = true;
        }
        else
        {
            mCornerSet = false;
        }
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::SetPosition(int pos, bool notify)
    {
        mThumbPendingPos = m_thumbPosition;
        if(pos >= m_range || pos < 0) { pos = 0; }
        m_thumbPosition = pos;

        // Normalise position
        if((m_thumbPosition + m_thumbSize) > m_range) { m_thumbPosition = m_range - m_thumbSize; }

        wxRect clientRect = GetClientRect();
        float majorDim = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
        if(majorDim == 0.0) {
            m_thumbPosition = 0;
            m_thumbRect = wxRect();
        }
        float percent = m_thumbSize / m_range;
        float thumbMajorDim = majorDim;
        if (percent < 1.0)
            thumbMajorDim = std::ceilf(percent * majorDim);
        else
            m_thumbPosition = 0;
        float thumbCoord = (m_thumbPosition / m_range) * majorDim;

        // Make sure that the thumb is always visible
        if(IsVertical()) {
            m_thumbRect.SetY(thumbCoord);
            m_thumbRect.SetX(0);
            m_thumbRect.SetWidth(clientRect.GetWidth());
            m_thumbRect.SetHeight(thumbMajorDim);
        } else {
            m_thumbRect.SetX(thumbCoord);
            m_thumbRect.SetY(0);
            m_thumbRect.SetHeight(clientRect.GetHeight());
            m_thumbRect.SetWidth(thumbMajorDim);
        }
        Refresh();
        if(notify) {
            mNotifyContent = true;
        }
    }
    //----------------------------------------------------------------------------------
    int CustomScrollBar::GetPositionFromPoint(const wxPoint& pt) const
    {
        wxRect clientRect = GetClientRect();
        float majorDim = IsVertical() ? clientRect.GetHeight() : clientRect.GetWidth();
        if(majorDim == 0.0) 
        { 
            return wxNOT_FOUND; 
        }
        float thumbCoord = IsVertical() ? pt.y : pt.x;
        //return std::ceilf((thumbCoord / majorDim) * m_range);
        return std::ceilf((thumbCoord * m_range) / majorDim);
    }
    //----------------------------------------------------------------------------------
    void CustomScrollBar::OnIdle(wxIdleEvent& event)
    {
        //event.Skip();
        if(mNotifyContent) 
        {
            mNotifyContent = false;
            ScrollEvent e(wxEVT_CUSTOM_SCROLL);
            e.SetEventObject(this);
            e.SetPosition(m_thumbPosition);
            GetEventHandler()->ProcessEvent(e);
        }
    }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    // ScrollBar
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    ScrollBar::ScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxScrollBar(parent, id, pos, size, style)
    {
        Hide();
    }
    //----------------------------------------------------------------------------------
    ScrollBar::~ScrollBar() {}
    //----------------------------------------------------------------------------------
    void ScrollBar::SetScrollbar(int position, int thumbSize, int range, int pageSize, bool notify)
    {
        m_thumb_size = thumbSize;
        m_range_size = range;
        wxScrollBar::SetScrollbar(position, thumbSize, range, pageSize, notify);
    }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    // ScrolledWindow
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    ScrolledWindow::ScrolledWindow() {}
    //----------------------------------------------------------------------------------
    ScrolledWindow::ScrolledWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, 
        const wxString& name)
        : wxWindow(parent, id, pos, size, style, name)
    {
        DoInitialize();
    }
    //----------------------------------------------------------------------------------
    bool ScrolledWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, 
        const wxString& name)
    {
        if (wxWindow::Create(parent, id, pos, size, style, name))
        {
            DoInitialize();
            return true;
        }
        return false;
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::SetConrnerColour(const wxColour& clr)
    { 
        if (GetBackgroundColour() != mCornerColour)
        {
            mCornerColour = clr;
            mCornerSet = true;
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::DoInitialize()
    {
        SetAutoLayout(false);
        mClient = 0;
        m_dragStartTime = (time_t)-1;
        m_vsb = new ScrollBar_t(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL);
        m_hsb = new ScrollBar_t(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL);
    #if USE_NATIVE_SCROLLBAR
        m_vsb->Bind(wxEVT_SCROLL_THUMBTRACK, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_LINEDOWN, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_LINEUP, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_PAGEDOWN, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_PAGEUP, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_BOTTOM, &ScrolledWindow::OnVScroll, this);
        m_vsb->Bind(wxEVT_SCROLL_TOP, &ScrolledWindow::OnVScroll, this);

        m_hsb->Bind(wxEVT_SCROLL_THUMBTRACK, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_LINEDOWN, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_LINEUP, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_PAGEDOWN, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_PAGEUP, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_BOTTOM, &ScrolledWindow::OnHScroll, this);
        m_hsb->Bind(wxEVT_SCROLL_TOP, &ScrolledWindow::OnHScroll, this);
    #else
        m_vsb->Bind(wxEVT_CUSTOM_SCROLL, &ScrolledWindow::OnVCustomScroll, this);
        m_hsb->Bind(wxEVT_CUSTOM_SCROLL, &ScrolledWindow::OnHCustomScroll, this);
    #endif
        Bind(wxEVT_MOUSEWHEEL, &ScrolledWindow::OnMouseWheel, this);
        Bind(wxEVT_CHAR_HOOK, &ScrolledWindow::OnCharHook, this);
        Bind(wxEVT_IDLE, &ScrolledWindow::OnIdle, this);
        Bind(wxEVT_LEFT_DOWN, &ScrolledWindow::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &ScrolledWindow::OnLeftUp, this);
        Bind(wxEVT_MOTION, &ScrolledWindow::OnMotion, this);
        Bind(wxEVT_LEAVE_WINDOW, &ScrolledWindow::OnLeaveWindow, this);
        Bind(wxEVT_SIZE, &ScrolledWindow::OnSize, this);

        /*Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& event) {
            event.Skip();
            Refresh();
        });
        Bind(wxEVT_KILL_FOCUS, [&](wxFocusEvent& event) {
            event.Skip();
            Refresh();
        });*/

    #ifdef __WXGTK__
        /// On GTK, UP/DOWN arrows is used to navigate between controls
        /// Disable it by capturing the event and calling 'Skip(false)'
        Bind(wxEVT_KEY_DOWN, [&](wxKeyEvent& event) {
            if(event.GetKeyCode() == WXK_UP || event.GetKeyCode() == WXK_DOWN) {
                event.Skip(false);
            } else {
                event.Skip(true);
            }
        });
    #endif
        m_tmpBmp = wxBitmap(1, 1);
        m_memDC = new wxMemoryDC(m_tmpBmp);
        m_gcdc = new wxGCDC(*m_memDC);
    }
    //----------------------------------------------------------------------------------
    ScrolledWindow::~ScrolledWindow()
    {
        // Destory the DCs in the reverse order of their creation
        setClient(0);
        wxDELETE(m_gcdc);
        wxDELETE(m_memDC);
    #if USE_NATIVE_SCROLLBAR
        m_vsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_LINEDOWN, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_LINEUP, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_PAGEUP, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_BOTTOM, &ScrolledWindow::OnVScroll, this);
        m_vsb->Unbind(wxEVT_SCROLL_TOP, &ScrolledWindow::OnVScroll, this);

        m_hsb->Unbind(wxEVT_SCROLL_THUMBTRACK, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_LINEDOWN, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_LINEUP, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_PAGEDOWN, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_PAGEUP, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_BOTTOM, &ScrolledWindow::OnHScroll, this);
        m_hsb->Unbind(wxEVT_SCROLL_TOP, &ScrolledWindow::OnHScroll, this);
    #else
        m_vsb->Unbind(wxEVT_CUSTOM_SCROLL, &ScrolledWindow::OnVCustomScroll, this);
        m_hsb->Unbind(wxEVT_CUSTOM_SCROLL, &ScrolledWindow::OnHCustomScroll, this);
    #endif

        Unbind(wxEVT_SIZE, &ScrolledWindow::OnSize, this);
        Unbind(wxEVT_MOUSEWHEEL, &ScrolledWindow::OnMouseWheel, this);
        Unbind(wxEVT_CHAR_HOOK, &ScrolledWindow::OnCharHook, this);
        Unbind(wxEVT_IDLE, &ScrolledWindow::OnIdle, this);
        Unbind(wxEVT_LEFT_DOWN, &ScrolledWindow::OnLeftDown, this);
        Unbind(wxEVT_LEFT_UP, &ScrolledWindow::OnLeftUp, this);
        Unbind(wxEVT_MOTION, &ScrolledWindow::OnMotion, this);
        Unbind(wxEVT_LEAVE_WINDOW, &ScrolledWindow::OnLeaveWindow, this);
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::setClient(wxWindow * obj, bool sizeEvent)
    {
        if (mClient && mClientSizeEvent)
        {
            mClientSizeEvent = false;
            mClient->Unbind(wxEVT_SIZE, &ScrolledWindow::OnClientSize, this);
        }
        mClient = obj;
        if (mClient && sizeEvent)
        {
            mClientSizeEvent = sizeEvent;
            mClient->Bind(wxEVT_SIZE, &ScrolledWindow::OnClientSize, this);
        }
    }
    //----------------------------------------------------------------------------------
#if USE_NATIVE_SCROLLBAR
    void ScrolledWindow::OnHScroll(wxScrollEvent& event)
    {
        //int newColumn = wxNOT_FOUND;
        if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) {
            int newColumn = event.GetPosition();
            ScollToColumn(newColumn);

        } else {
            int steps = wxNOT_FOUND;
            wxDirection direction = wxRIGHT;
            if(event.GetEventType() == wxEVT_SCROLL_LINEUP) {
                steps = 1;
                direction = wxLEFT;
            } else if(event.GetEventType() == wxEVT_SCROLL_LINEDOWN) {
                steps = 1;
                direction = wxRIGHT;
            } else if(event.GetEventType() == wxEVT_SCROLL_PAGEUP) {
                steps = m_hsb->GetPageSize();
                direction = wxLEFT;
            } else if(event.GetEventType() == wxEVT_SCROLL_PAGEDOWN) {
                steps = m_hsb->GetPageSize();
                direction = wxRIGHT;
            } else if(event.GetEventType() == wxEVT_SCROLL_TOP) {
                steps = 0;
                direction = wxLEFT;
            } else if(event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
                steps = 0;
                direction = wxRIGHT;
            }
            if(steps != wxNOT_FOUND) { ScrollColumns(steps, direction); }
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnVScroll(wxScrollEvent& event)
    {
        wxDirection direction = wxDOWN;
        //int newTopLine = wxNOT_FOUND;
        if(event.GetEventType() == wxEVT_SCROLL_THUMBTRACK) {
            int newTopLine = event.GetPosition();
            ScrollToRow(newTopLine);
        } else {
            int steps = wxNOT_FOUND;
            if(event.GetEventType() == wxEVT_SCROLL_LINEUP) {
                steps = 1;
                direction = wxUP;
            } else if(event.GetEventType() == wxEVT_SCROLL_LINEDOWN) {
                steps = 1;
                direction = wxDOWN;
            } else if(event.GetEventType() == wxEVT_SCROLL_PAGEUP) {
                steps = m_vsb->GetPageSize();
                direction = wxUP;
            } else if(event.GetEventType() == wxEVT_SCROLL_PAGEDOWN) {
                steps = m_vsb->GetPageSize();
                direction = wxDOWN;
            } else if(event.GetEventType() == wxEVT_SCROLL_TOP) {
                steps = 0;
                direction = wxUP;
            } else if(event.GetEventType() == wxEVT_SCROLL_BOTTOM) {
                steps = 0;
                direction = wxDOWN;
            }
            if(steps != wxNOT_FOUND) { ScrollRows(steps, direction); }
        }
    }
#endif
    //----------------------------------------------------------------------------------
#if CL_USE_CUSTOM_SCROLLBAR
    void ScrolledWindow::OnVCustomScroll(ScrollEvent& event) 
    {
        ScrollToRow(event.GetPosition()); 
        if (mScrollEvent)
        {
            wxScrollEvent e(wxEVT_SCROLL_CHANGED, 0, event.GetPosition(), wxSP_VERTICAL);
            GetEventHandler()->ProcessEvent(e);
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnHCustomScroll(ScrollEvent& event) 
    { 
        ScollToColumn(event.GetPosition());
        if (mScrollEvent)
        {
            wxScrollEvent e(wxEVT_SCROLL_CHANGED, 0, event.GetPosition(), wxSP_HORIZONTAL);
            GetEventHandler()->ProcessEvent(e);
        }
    }
#endif
    //----------------------------------------------------------------------------------
    void ScrolledWindow::UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize, bool size, bool notify)
    {
        wxWindowUpdateLocker tempee(this);
        if(thumbSize <= 0 && rangeSize <= 0 ) 
        {
            if (!m_vsb->IsAlwaysShow() && m_vsb->IsShown())
            {
                mVsbNeedShow = false;
                m_vsb->Hide();
                if (m_hsb->IsShown())
                {
                    if (mCornerSet)
                    {
                        m_hsb->SetConrnerColour(CT_LB | CT_RB, mCornerColour);
                    }
                }
                if (size)
                {
                    SendSizeEvent();
                    //mClient->SetSize(GetClientArea().GetSize());
                }
                return;
            }
        }

        // Keep the values
        m_pageSize = pageSize;
        m_position = position;
        m_thumbSize = thumbSize;
        m_rangeSize = rangeSize;

        // Hide the scrollbar if needed
        mVsbNeedShow = (thumbSize < rangeSize) && (m_vsb->IsShown() || !m_showSBOnFocus);
        if(!mVsbNeedShow && m_vsb->IsShown() && !m_vsb->IsAlwaysShow()) {
            m_vsb->Hide();
            if (m_hsb->IsShown())
            {
                if (mCornerSet)
                {
                    m_hsb->SetConrnerColour(CT_RT | CT_RB, mCornerColour);
                }
            }
            if (size)
            {
                SendSizeEvent();
                return;
                //mClient->SetSize(GetClientArea().GetSize());
            }
        } else if(mVsbNeedShow && !m_vsb->IsShown()) {
            m_vsb->Show();
            if (m_hsb->IsShown())
            {
                if (mCornerSet)
                {
                    m_vsb->SetConrnerColour(CT_RT, mCornerColour);
                    m_hsb->SetConrnerColour(CT_LB, mCornerColour);
                }
            }
            else
            {
                if (mCornerSet)
                {
                    m_vsb->SetConrnerColour(CT_RT | CT_RB, mCornerColour);
                }
            }
            if (size)
            {
                SendSizeEvent();
                return;
                //mClient->SetSize(GetClientArea().GetSize());
            }
        }
#if USE_NATIVE_SCROLLBAR
        m_vsb->SetScrollbar(position, thumbSize, rangeSize, pageSize, notify);
#else
        m_vsb->UpdateScroll(thumbSize, rangeSize, pageSize, position, notify);
#endif

    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnMouseWheel(wxMouseEvent& event)
    {
        m_wheelRotation += event.GetWheelRotation();
        int lines = m_wheelRotation / event.GetWheelDelta();
        m_wheelRotation -= lines * event.GetWheelDelta();

        if (lines != 0)
        {
            //wxScrollWinEvent newEvent;

            //newEvent.SetPosition(0);
            //newEvent.SetOrientation(event.GetWheelAxis() == 0 ? wxVERTICAL : wxHORIZONTAL);
            //newEvent.SetEventObject(m_win);

            if (event.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL)
            {
                //lines = -lines;
                int dsthtpos = GetHThumbPosition() - lines * event.GetLinesPerAction() * mScrollRateX;
                SetHThumbPosition(dsthtpos, true);

                /*wxMouseEvent event2(wxEVT_MOTION);
                event2.SetEventObject(this);
                event2.SetPosition(event.GetPosition());
                event2.m_wheelAxis = wxMOUSE_WHEEL_HORIZONTAL;
                GetEventHandler()->AddPendingEvent(event2);*/
            }
            else
            {
                int dstvtpos = GetVThumbPosition() - lines * event.GetLinesPerAction() * mScrollRateY;
                SetVThumbPosition(dstvtpos, true);

                /*wxMouseEvent event2(wxEVT_MOTION);
                event2.SetEventObject(this);
                event2.SetPosition(event.GetPosition());
                event2.m_wheelAxis = wxMOUSE_WHEEL_VERTICAL;
                GetEventHandler()->AddPendingEvent(event2);*/
            }
            //OnMotion(event2);
            /*if (event.IsPageScroll())
            {
                if (lines > 0)
                    newEvent.SetEventType(wxEVT_SCROLLWIN_PAGEUP);
                else
                    newEvent.SetEventType(wxEVT_SCROLLWIN_PAGEDOWN);

                m_win->GetEventHandler()->ProcessEvent(newEvent);
            }
            else
            {
                lines *= event.GetLinesPerAction();
                if (lines > 0)
                    newEvent.SetEventType(wxEVT_SCROLLWIN_LINEUP);
                else
                    newEvent.SetEventType(wxEVT_SCROLLWIN_LINEDOWN);

                int times = abs(lines);
                for (; times > 0; times--)
                    m_win->GetEventHandler()->ProcessEvent(newEvent);
            }*/
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnCharHook(wxKeyEvent& event)
    {
        event.Skip();
        wxKeyEvent keyDown = event;
        keyDown.SetEventType(wxEVT_KEY_DOWN);
        if(DoKeyDown(keyDown)) {
            event.Skip(false);
            return;
        }

        // Always process the HOME/END buttons
        // The following can be processed only once
        if(event.GetEventObject() == this) {
            if(event.GetKeyCode() == WXK_HOME) {
                ScrollRows(0, wxUP);
            } else if(event.GetKeyCode() == WXK_END) {
                ScrollRows(0, wxDOWN);
            } else if(event.GetKeyCode() == WXK_UP) {
                ScrollRows(1, wxUP);
            } else if(event.GetKeyCode() == WXK_DOWN) {
                ScrollRows(1, wxDOWN);
            } else if(event.GetKeyCode() == WXK_PAGEUP) {
                ScrollRows(GetPageSize(), wxUP);
            } else if(event.GetKeyCode() == WXK_PAGEDOWN) {
                ScrollRows(GetPageSize(), wxDOWN);
            }
        }
    }
    //----------------------------------------------------------------------------------
    int ScrolledWindow::GetPageSize() const 
    { 
        return m_pageSize; 
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnIdle(wxIdleEvent& event)
    {
        event.Skip();
        if(m_vsb && m_showSBOnFocus) {
            wxWindow* focus_win = wxWindow::FindFocus();
            bool inOurWindows = IsDescendant(focus_win);
            if(ShouldShowScrollBar() && !m_vsb->IsShown() && inOurWindows) {
                wxWindowUpdateLocker templ(this);
                // Update the scrollbar with the latest values
                m_vsb->Show();
                DoPositionVScrollbar();
                m_vsb->SetScrollbar(m_position, m_thumbSize, m_rangeSize, m_pageSize);
            } else if(!inOurWindows && m_vsb->IsShown()) {
                m_vsb->Hide();
            }
        }

        ProcessIdle();
    }
    //----------------------------------------------------------------------------------
    bool ScrolledWindow::ShouldShowScrollBar() const 
    { 
        return m_vsb && m_vsb->ShouldShow(); 
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnLeftDown(wxMouseEvent& event)
    {
        //event.Skip();
        // Not considering D'n'D so reset any saved values
        DoCancelDrag();

        // Prepare to DnDclTreeCtrl_DnD
        if(event.LeftIsDown()) {
            m_dragStartTime = wxDateTime::UNow();
            m_dragStartPos = wxPoint(event.GetX(), event.GetY());
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnLeftUp(wxMouseEvent& event)
    {
        //event.Skip();
        if(mDragging) {
            wxTreeItemId dropTarget = GetRow(event.GetPosition());
            if(dropTarget.IsOk()) {
                wxTreeEvent event(wxEVT_TREE_END_DRAG);
                event.SetEventObject(this);
                event.SetItem(dropTarget);
                GetEventHandler()->ProcessEvent(event);
            }
        }
        DoCancelDrag();
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnMotion(wxMouseEvent& event)
    {
        //event.Skip();
        if(m_dragStartTime.IsValid() && event.LeftIsDown() &&
           !mDragging) { // If we're tugging on the tab, consider starting D'n'D
            wxTimeSpan diff = wxDateTime::UNow() - m_dragStartTime;
            if(diff.GetMilliseconds() > 100 && // We need to check both x and y distances as tabs may be vertical
               ((abs(m_dragStartPos.x - event.GetX()) > 5) || (abs(m_dragStartPos.y - event.GetY()) > 5))) {
                DoBeginDrag(); // Sufficient time and distance since the LeftDown for a believable D'n'D start
            }
        }
        /*else
        {
            if (event.GetWheelAxis() == wxMOUSE_WHEEL_HORIZONTAL)
                ScollToColumn(GetHThumbPosition());
            else
                ScrollToRow(GetVThumbPosition());
        }*/
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnLeaveWindow(wxMouseEvent& event)
    {
        //event.Skip();
        DoCancelDrag();
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::DoBeginDrag()
    {
        if(!GetRow(m_dragStartPos).IsOk()) {
            DoCancelDrag();
            return;
        }

        wxTreeEvent event(wxEVT_TREE_BEGIN_DRAG);
        event.SetEventObject(this);
        GetEventHandler()->ProcessEvent(event);
        if(!event.IsAllowed()) { return; }

        // Change the cursor indicating DnD in progress
        SetCursor(wxCURSOR_HAND);
        mDragging = true;
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::DoCancelDrag()
    {
        m_dragStartTime.Set((time_t)-1); // Reset the saved values
        m_dragStartPos = wxPoint();
        SetCursor(wxCURSOR_DEFAULT);
        mDragging = false;
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::DoPositionVScrollbar()
    {
        if (mVsbNeedShow)
        {
            wxRect clientRect = GetClientSize();

            int height = clientRect.GetHeight();
            if (mHsbNeedShow) { height -= SB_WIDTH; }
            wxWindowUpdateLocker templ(m_vsb);
            m_vsb->SetSize(clientRect.GetWidth() - SB_WIDTH, 0, SB_WIDTH, height);
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::DoPositionHScrollbar()
    {
        if (mHsbNeedShow)
        {
            wxRect clientRect = GetClientSize();

            int width = clientRect.GetWidth();
            if (mVsbNeedShow) { width -= SB_WIDTH; }
            wxWindowUpdateLocker templ(m_hsb);
            m_hsb->SetSize(0, clientRect.GetHeight() - SB_WIDTH, width, SB_WIDTH);
        }
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnSize(wxSizeEvent&)
    {
        //DoPositionHScrollbar();
        //DoPositionVScrollbar();
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::OnClientSize(wxSizeEvent& event)
    {
        event.Skip();
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::UpdateHScrollBar(int position, int thumbSize, int rangeSize, int pageSize, bool size, bool notify)
    {
        wxWindowUpdateLocker tttlt(this);
        if(thumbSize <= 0 && rangeSize <= 0) {
            if (!m_hsb->IsAlwaysShow() && m_hsb->IsShown())
            {
                mHsbNeedShow = false;
                m_hsb->Hide();
                if (m_vsb->IsShown())
                {
                    if (mCornerSet)
                    {
                        m_vsb->SetConrnerColour(CT_RT | CT_RB, mCornerColour);
                    }
                }
                if (size)
                {
                    SendSizeEvent();
                    //mClient->SetSize(GetClientArea().GetSize());
                }
                return;
            }
        }
        mHsbNeedShow = thumbSize < rangeSize;
        if(mHsbNeedShow && !m_hsb->IsShown()) {
            m_hsb->Show();
            if (m_vsb->IsShown())
            {
                if (mCornerSet)
                {
                    m_hsb->SetConrnerColour(CT_LB, mCornerColour);
                    m_vsb->SetConrnerColour(CT_RT, mCornerColour);
                }
            }
            else
            {
                if (mCornerSet)
                {
                    m_hsb->SetConrnerColour(CT_LB | CT_RB, mCornerColour);
                }
            }
            if (size)
            {
                SendSizeEvent();
                return;
                //mClient->SetSize(GetClientArea().GetSize());
            }
        } else if(!mHsbNeedShow && m_hsb->IsShown() && !m_hsb->IsAlwaysShow()) {
            m_hsb->Hide();
            if (m_vsb->IsShown())
            {
                if (mCornerSet)
                {
                    m_vsb->SetConrnerColour(CT_RT | CT_RB, mCornerColour);
                }
            }
            if (size)
            {
                SendSizeEvent();
                return;
                //mClient->SetSize(GetClientArea().GetSize());
            }
        }
#if USE_NATIVE_SCROLLBAR
        m_hsb->SetScrollbar(position, thumbSize, rangeSize, pageSize, notify);
#else
        m_hsb->UpdateScroll(thumbSize, rangeSize, pageSize, position, notify);
#endif
    }
    //----------------------------------------------------------------------------------
    wxRect ScrolledWindow::GetClientArea() const
    {
        wxRect r = GetClientRect();
    #ifdef __WXOSX__
        // on OSX, the top-left is set to 2,2
        r.SetTopLeft(wxPoint(0, 0));
    #endif

        if(m_hsb->IsShown()) 
        { 
            r.SetHeight(r.GetHeight() - m_hsb->GetSize().GetHeight()); 
        }
        if(m_vsb->IsShown()) 
        { 
            r.SetWidth(r.GetWidth() - m_vsb->GetSize().GetWidth()); 
        }
        return r;
    }
    //----------------------------------------------------------------------------------
    void ScrolledWindow::GetClientAreaSize(int * width, int * height) const
    {
        wxRect r = GetClientRect();

#ifdef __WXOSX__
        // on OSX, the top-left is set to 2,2
        r.SetTopLeft(wxPoint(0, 0));
#endif

        if (m_hsb->IsShown())
        {
            r.SetHeight(r.GetHeight() - m_hsb->GetSize().GetHeight());
        }
        if (m_vsb->IsShown())
        {
            r.SetWidth(r.GetWidth() - m_vsb->GetSize().GetWidth());
        }
        *width = r.GetWidth();
        *height = r.GetHeight();
    }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    // DefaultScrolledWindow
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------
    DefaultScrolledWindow::DefaultScrolledWindow() : 
        ScrolledWindow(), mMuteDisable(false) {}
    //----------------------------------------------------------------------------------
    DefaultScrolledWindow::DefaultScrolledWindow(wxWindow *parent, wxWindowID winid, 
        const wxPoint& pos, const wxSize& size, long style, const wxString& name)
        : ScrolledWindow(parent, winid, pos, size, style, name),
        mMuteDisable(false)
    {
    }
    //-------------------------------------------------------------------------------
    void DefaultScrolledWindow::ScrollToRow(int firstLine)
    {
        if (GetVThumbPendingPosition() != firstLine)
        {
#ifndef __WXMAC__
            getClient()->Update();
#endif
            getClient()->ScrollWindow(0, GetVThumbPendingPosition() - firstLine);
            SetVThumbPendingPosition(firstLine);
        }
        /*else
        {
            getClient()->Refresh();
        }*/
    }
    //--------------------------------------------------------------------------------
    void DefaultScrolledWindow::ScollToColumn(int firstColumn)
    {
        if (GetHThumbPendingPosition() != firstColumn)
        {
#ifndef __WXMAC__
            getClient()->Update();
#endif
            getClient()->ScrollWindow(GetHThumbPendingPosition() - firstColumn, 0);
            SetHThumbPendingPosition(firstColumn);
        }
        /*else
        {
            getClient()->Refresh();
        }*/
    }
    //----------------------------------------------------------------------------------
    void DefaultScrolledWindow::OnSize(wxSizeEvent& event)
    {
        if (mClient)
        {
            wxSize csize = event.GetSize();
            wxSize tsize = event.GetSize();
            wxSize bsize = mClient->GetBestVirtualSize();
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
                m_vsb->SetSize(csize.x, 0, SB_WIDTH, csize.y);
            }
            if (csize.x < bsize.x)
            {
                wxWindowUpdateLocker templ(m_hsb);
                m_hsb->SetSize(0, csize.y, csize.x, SB_WIDTH);
            }

            wxWindowUpdateLocker clock(mClient);
            mClient->SetSize(csize);
            Refresh();
        }
    }
    //----------------------------------------------------------------------------------
    void DefaultScrolledWindow::OnClientSize(wxSizeEvent& event)
    {
        if (mClient->GetSizer())
        {
            wxSize csize = event.GetSize();
            wxSize bsize = mClient->GetBestVirtualSize();

            mClient->SetVirtualSize(bsize);

            UpdateVScrollBar(GetVThumbPosition(), csize.y, bsize.y, csize.y, false, true);
            UpdateHScrollBar(GetHThumbPosition(), csize.x, bsize.x, csize.x, false, true);

            //SetHThumbPosition(GetHThumbPosition(), false);
            //SetVThumbPosition(GetVThumbPosition(), false);

            wxWindowUpdateLocker templ(mClient);
            mClient->GetSizer()->SetDimension(-GetHThumbPosition(), -GetVThumbPosition(), bsize.GetWidth(), bsize.GetHeight());
        }
    }
    //----------------------------------------------------------------------------
    class NoDescriptionPanel : public wxPanel
    {
    public:
        NoDescriptionPanel(wxWindow* parent) :wxPanel(parent)
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            Bind(wxEVT_PAINT, &NoDescriptionPanel::OnPaint, this);
            Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        }
        ~NoDescriptionPanel()
        {
            Unbind(wxEVT_PAINT, &NoDescriptionPanel::OnPaint, this);
            Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        }
    
        void OnPaint(wxPaintEvent& e)
        {
            wxAutoBufferedPaintDC dc(this);
            wxRect clientRect(GetClientRect());
            wxColour bgColour = gsgs_Style().GetAppBgColour();
            wxColour penColour = gsgs_Style().GetActiveMarkerColour();

            dc.SetBrush(bgColour);
            dc.SetPen(bgColour);
            dc.DrawRectangle(clientRect);

            wxFont font(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("ËÎÌו"));
            dc.SetTextForeground(penColour);
            dc.SetFont(font);

            wxSize sz = dc.GetTextExtent("No Description");
            int textX = 0;
            int textY = 0;
            if (clientRect.GetWidth() - sz.x < 0)
                textX = ((sz.x - clientRect.GetWidth()) / 2);
            else
                textX = ((clientRect.GetWidth() - sz.x) / 2);

            if (clientRect.GetHeight() - sz.y < 0)
                textY = ((sz.y - clientRect.GetHeight()) / 2);
            else
                textY = ((clientRect.GetHeight() - sz.y) / 2);
            dc.DrawText("No Description", textX, textY);
        }
    };
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // SingleChoiceDialog
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    SingleChoiceDialog::SingleChoiceDialog() {}
    //----------------------------------------------------------------------------
    SingleChoiceDialog::SingleChoiceDialog(wxWindow* parent)
    {
        Create(parent);
    }
    //----------------------------------------------------------------------------
    SingleChoiceDialog::~SingleChoiceDialog()
    {
        m_searchCtrl->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SingleChoiceDialog::OnFilter), NULL,
            this);
        mOkButton->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SingleChoiceDialog::OnOKUI), NULL, this);
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::SetNotebookSelection(int selete)
    { 
        mNotebook->SetSelection(selete); 
    }
    //----------------------------------------------------------------------------
    int SingleChoiceDialog::GetNotebookSelection() const
    { 
        return mNotebook->GetSelection(); 
    }
    //----------------------------------------------------------------------------
    bool SingleChoiceDialog::Create(wxWindow* parent)
    {
        if (!wxDialog::Create(parent, wxID_ANY, wxT(""), wxDefaultPosition, gsgsDlgSize, gsgsDlgStyle, wxT("SingleChoiceDialog")))
            return false;
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer181 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer181);

        m_searchCtrl = new wxSearchCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTE_PROCESS_ENTER);
        m_searchCtrl->SetToolTip(_("Filter the results.\nHit ENTER to apply the filter"));
        m_searchCtrl->SetFocus();
        m_searchCtrl->ShowSearchButton(true);
        m_searchCtrl->ShowCancelButton(true);
        boxSizer181->Add(m_searchCtrl, 0, wxALL | wxEXPAND, 5);

        mNotebook = new Notebook(this, wxID_ANY, wxSize(256,256), wxDefaultPosition, wxDefaultSize, kNotebook_LeftTabs | kNotebook_ScrollTabs, wxTAB_TRAVERSAL | wxSIMPLE_BORDER,
            Notebook::TT_Scroll);
        boxSizer181->Add(mNotebook, 1, wxALL | wxEXPAND, 5);

        m_stdBtnSizer183 = new DialogButtonSizer();

        boxSizer181->Add(m_stdBtnSizer183, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

        mOkButton = new Button(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        mOkButton->SetDefault();
        m_stdBtnSizer183->AddButton(mOkButton);

        mCancelButton = new Button(this, wxID_CANCEL, wxT("CANCEL"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer183->AddButton(mCancelButton);
        m_stdBtnSizer183->Realize();

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        // Connect events
        m_searchCtrl->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SingleChoiceDialog::OnFilter), NULL,
            this);
        mOkButton->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(SingleChoiceDialog::OnOKUI), NULL, this);

        return true;
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::initItem(const wxArrayString& options, const WindowList & optionPanels, int selection)
    {
        m_options = options;
        m_optionPanels = optionPanels;

        if (m_options.size() > 20)
            m_searchCtrl->Show();
        else
            m_searchCtrl->Hide();

        DoInitialise();

        if (selection >= 0 && selection < (int)options.size()) {
            mNotebook->SetSelection(selection);
        }
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::initItem(const wxArrayString& options, int selection)
    {
        m_options = options;
        m_optionPanels.resize(options.size(), 0);

        if (m_options.size() > 20)
            m_searchCtrl->Show();
        else
            m_searchCtrl->Hide();

        DoInitialise();

        if (selection >= 0 && selection < (int)options.size()) {
            mNotebook->SetSelection(selection);
        }
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::OnOKUI(wxUpdateUIEvent& event)
    {
        /*wxDataViewItem item = mNotebook->GetSelection();
        event.Enable(item.IsOk());*/
        event.Enable(mNotebook->GetSelection() != -1);
    }
    //----------------------------------------------------------------------------
    wxString SingleChoiceDialog::GetSelection() const
    {
        return mNotebook->GetPageText(mNotebook->GetSelection());
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::OnItemActivated(wxCommandEvent& event)
    {
        EndModal(wxID_OK);
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::OnActivated(wxDataViewEvent& event)
    {
        EndModal(wxID_OK);
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::OnFilter(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DoInitialise();
    }
    //----------------------------------------------------------------------------
    void SingleChoiceDialog::DoInitialise()
    {
        mNotebook->DeleteAllPages();

        wxString filter = m_searchCtrl->GetValue();
        wxBitmap dummyBmp = gsgs_Image().LoadBitmap("101010");
        if (dummyBmp.IsOk() && m_options.size() == m_optionPanels.size())
        {
            wxWindowUpdateLocker winlok(mNotebook);
            for (size_t i = 0; i < m_options.size(); ++i) {
                if (FileUtil::FuzzyMatch(filter, m_options.Item(i))) {
                    wxWindow * tpanel = m_optionPanels[i];
                    if (tpanel == 0)
                        tpanel = new NoDescriptionPanel(mNotebook);
                    else
                        tpanel->Reparent(mNotebook);
                    mNotebook->AddPage(tpanel, m_options[i], false, dummyBmp);
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // ListChoiceDialog
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    ListChoiceDialog::ListChoiceDialog() {}
    //----------------------------------------------------------------------------
    ListChoiceDialog::ListChoiceDialog(wxWindow* parent, const wxArrayString& options, const WindowList & optionPanels,
        int selection)
    {
        Create(parent, options, optionPanels, selection);
    }
    //----------------------------------------------------------------------------
    ListChoiceDialog::~ListChoiceDialog()
    {
        m_searchCtrl->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ListChoiceDialog::OnFilter), NULL,
            this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(ListChoiceDialog::OnActivated), NULL, this);
        mOkButton->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ListChoiceDialog::OnOKUI), NULL, this);
    }
    //----------------------------------------------------------------------------
    bool ListChoiceDialog::Create(wxWindow* parent, const wxArrayString& options, const WindowList & optionPanels, int selection)
    {
        if (!wxDialog::Create(parent, wxID_ANY, wxT(""), wxDefaultPosition, gsgsDlgSize, wxNO_BORDER))
            return false;
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_options = options;
        m_optionPanels = optionPanels;
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer181 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer181);

        m_searchCtrl = new wxSearchCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTE_PROCESS_ENTER);
        m_searchCtrl->SetToolTip(_("Filter the results.\nHit ENTER to apply the filter"));
        m_searchCtrl->SetFocus();
        m_searchCtrl->ShowSearchButton(true);
        m_searchCtrl->ShowCancelButton(true);
        m_searchCtrl->Hide();
        boxSizer181->Add(m_searchCtrl, 0, wxALL | wxEXPAND, 5);

        if (options.size() > 20)
            m_searchCtrl->Show();

        m_dvListCtrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxDV_NO_HEADER | wxDV_ROW_LINES | wxDV_SINGLE);
        boxSizer181->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, 5);

        m_dvListCtrl->AppendTextColumn(_("My Column"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_stdBtnSizer183 = new DialogButtonSizer();

        boxSizer181->Add(m_stdBtnSizer183, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

        mOkButton = new Button(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        mOkButton->SetDefault();
        m_stdBtnSizer183->AddButton(mOkButton);

        mCancelButton = new Button(this, wxID_CANCEL, wxT("CANCEL"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer183->AddButton(mCancelButton);
        m_stdBtnSizer183->Realize();

        SetName(wxT("ListChoiceDialog"));
        SetMinClientSize(gsgsDlgSize);
        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }
        /*#if wxVERSION_NUMBER >= 2900
                if (!wxPersistenceManager::Get().Find(this)) {
                    wxPersistenceManager::Get().RegisterAndRestore(this);
                }
                else {
                    wxPersistenceManager::Get().Restore(this);
                }
        #endif*/
        // Connect events
        m_searchCtrl->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ListChoiceDialog::OnFilter), NULL,
            this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(ListChoiceDialog::OnActivated), NULL, this);
        mOkButton->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(ListChoiceDialog::OnOKUI), NULL, this);

        DoInitialise();
        if (selection >= 0 && selection < (int)options.size()) {
            m_dvListCtrl->Select(m_dvListCtrl->RowToItem(selection));
        }
        return true;
    }
    //----------------------------------------------------------------------------
    void ListChoiceDialog::OnOKUI(wxUpdateUIEvent& event)
    {
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        event.Enable(item.IsOk());
    }
    //----------------------------------------------------------------------------
    wxString ListChoiceDialog::GetSelection() const
    {
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        if (item.IsOk()) {
            wxStringClientData* cd = (wxStringClientData*)m_dvListCtrl->GetItemData(item);
            return cd->GetData();
        }
        return "";
    }
    //----------------------------------------------------------------------------
    void ListChoiceDialog::OnItemActivated(wxCommandEvent& event)
    {
        EndModal(wxID_OK);
    }
    //----------------------------------------------------------------------------
    void ListChoiceDialog::OnActivated(wxDataViewEvent& event)
    {
        EndModal(wxID_OK);
    }
    //----------------------------------------------------------------------------
    void ListChoiceDialog::OnFilter(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DoInitialise();
    }
    //----------------------------------------------------------------------------
    void ListChoiceDialog::DoInitialise()
    {
        for (int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
            wxStringClientData* cd = (wxStringClientData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
            wxDELETE(cd);
        }
        m_dvListCtrl->DeleteAllItems();

        wxString filter = m_searchCtrl->GetValue();
        for (size_t i = 0; i < m_options.size(); ++i) {
            if (FileUtil::FuzzyMatch(filter, m_options.Item(i))) {
                wxVector<wxVariant> cols;
                wxString displayString = m_options.Item(i).BeforeFirst('\n');
                displayString.Trim().Trim(false);
                cols.push_back(displayString);
                m_dvListCtrl->AppendItem(cols, (wxUIntPtr)(new wxStringClientData(m_options.Item(i))));
            }
        }
    }
    //----------------------------------------------------------------------------
}