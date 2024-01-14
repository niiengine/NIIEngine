#include "gsgsSplitterWindow.h"
#include "gsgsStyleManager.h"
#include <wx/dcbuffer.h>
#include <wx/renderer.h>
namespace gsgs
{
    //-----------------------------------------------------------------
    SplitterWindow::SplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                                   long style, const wxString& name)
    {
        Create(parent, id, pos, size, style);
    }
    //-----------------------------------------------------------------
    SplitterWindow::~SplitterWindow()
    {
        Unbind(wxEVT_PAINT, &SplitterWindow::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, &SplitterWindow::OnEraseBg, this);
        Unbind(wxEVT_LEFT_DCLICK, &SplitterWindow::OnDClick, this);
        gsgs_Event().Unbind(wxEVT_SYS_COLOURS_CHANGED, &SplitterWindow::OnSysColoursChanged, this);
    }
    //-----------------------------------------------------------------
    bool SplitterWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                                        const wxString& name)
    {
        if (wxSplitterWindow::Create(parent, id, pos, size, /*wxSP_LIVE_UPDATE |*/ style | wxBORDER_NONE, name))
        {

            SetBackgroundStyle(wxBG_STYLE_PAINT);
            Bind(wxEVT_PAINT, &SplitterWindow::OnPaint, this);
            Bind(wxEVT_ERASE_BACKGROUND, &SplitterWindow::OnEraseBg, this);
            Bind(wxEVT_LEFT_DCLICK, &SplitterWindow::OnDClick, this);
            gsgs_Event().Bind(wxEVT_SYS_COLOURS_CHANGED, &SplitterWindow::OnSysColoursChanged, this);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------
    void SplitterWindow::OnPaint(wxPaintEvent& event)
    {
        wxAutoBufferedPaintDC dc(this);
        DoDrawSash(dc);
    }
    //-----------------------------------------------------------------
    void SplitterWindow::OnEraseBg(wxEraseEvent&){}
    //-----------------------------------------------------------------
    void SplitterWindow::OnDClick(wxMouseEvent&){}
    //-----------------------------------------------------------------
    void SplitterWindow::DoDrawSash(wxDC& dc)
    {
        wxColour c = gsgs_Style().GetAppBgColour();

        wxRect rect = GetClientRect();
        dc.SetPen(c);
        dc.SetBrush(c);
        dc.DrawRectangle(rect);
    }
    //-----------------------------------------------------------------
    void SplitterWindow::OnSysColoursChanged(Event& event)
    {
        event.Skip();
        Refresh();
    }
    //-----------------------------------------------------------------
    void SplitterWindow::DrawSash(wxDC& dc)
    {
        wxUnusedVar(dc);
        Refresh();
    }
    //-----------------------------------------------------------------
}