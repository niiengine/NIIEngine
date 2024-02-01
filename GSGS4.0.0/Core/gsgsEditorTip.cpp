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

#include "gsgsStyleManager.h"
#include "gsgsEditorTip.h"
#include "gsgsStyleManager.h"
#include "gsgsEditorConfig.h"
#include "globals.h"
#include "plugin.h"
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include "ieditor.h"
#include "imanager.h"

namespace gsgs
{
BEGIN_EVENT_TABLE(EditorTipWindow, wxPanel)
EVT_PAINT(EditorTipWindow::OnPaint)
EVT_ERASE_BACKGROUND(EditorTipWindow::OnEraseBg)
END_EVENT_TABLE()

#define TIP_SPACER 10

EditorTipWindow::EditorTipWindow(wxWindow* parent)
    : wxPanel(parent)
    , m_highlighIndex(0)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_font = gsgs_Style().GetLexer("text")->GetFontForSyle(0);
    Hide();
    gsgs_Event().Connect(wxEVT_CMD_COLOURS_FONTS_UPDATED,
                                  gsgsEventHandler(EditorTipWindow::OnEditoConfigChanged), NULL, this);
}

EditorTipWindow::~EditorTipWindow()
{
    gsgs_Event().Disconnect(wxEVT_CMD_COLOURS_FONTS_UPDATED,
                                     gsgsEventHandler(EditorTipWindow::OnEditoConfigChanged), NULL, this);
}

void EditorTipWindow::OnEraseBg(wxEraseEvent& e) { wxUnusedVar(e); }

void EditorTipWindow::OnPaint(wxPaintEvent& e)
{
    wxUnusedVar(e);
    wxAutoBufferedPaintDC bdc(this);
    wxGCDC dc(bdc);
    PrepareDC(dc);

    if(m_args.IsEmpty()) return;

    // Define the colours used by this tooltip window
    gsgs::Colour colours = gsgs_Style().GetColours();
    IEditor* editor = GetManager()->GetActiveEditor();
    if(editor) {
        wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
        if(gsgs::Colour::IsDark(bgColour)) {
            colours.setBase(bgColour);
        } else {
            colours.setBase(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        }
    }

    wxColour bgColour, penColour, textColour, highlightBgColour, highlightFgColour;
    bgColour = colours.GetBgColour();
    penColour = colours.GetBorderColour();
    textColour = colours.GetItemTextColour();
    highlightBgColour = colours.GetSelItemBgColour();
    highlightFgColour = colours.GetSelItemTextColour();
    wxRect rr = GetClientRect();

    // draw the background using the parent background colour
    dc.SetBrush(bgColour);
    dc.SetPen(penColour);
    dc.DrawRectangle(rr);
    dc.SetFont(m_font);

    // Highlight the text
    dc.SetTextForeground(textColour);
    wxSize helperTextSize = dc.GetTextExtent("Tp");

    wxCoord x = TIP_SPACER;
    wxCoord y = 0;

    if(!m_header.IsEmpty()) {
        wxFont guiFont = m_font;
        dc.SetFont(guiFont);
        wxSize headerSize = dc.GetTextExtent(m_header);
        wxPoint headerPt;
        headerPt.x = rr.GetWidth() - headerSize.x - TIP_SPACER;
        headerPt.y = 0;
        dc.SetTextForeground(textColour);
        dc.DrawText(m_header, headerPt);
        y += helperTextSize.y;
        dc.DrawLine(0, y, rr.GetWidth(), y);
    }

    dc.SetFont(m_font);
    for(size_t i = 0; i < m_args.size(); ++i) {
        wxString line = m_args.Item(i);
        if((int)i == m_highlighIndex) {
            // wxFont f = m_font;
            // f.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetBrush(highlightBgColour);
            dc.SetPen(highlightBgColour);
            dc.SetTextForeground(highlightFgColour);
            wxRect selectionRect(0, y, rr.GetWidth(), helperTextSize.y);
            selectionRect.Deflate(1);
            dc.DrawRectangle(selectionRect);
            dc.DrawText(line, x, y);

        } else {
            dc.SetTextForeground(textColour);
            dc.DrawText(line, x, y);
        }
        y += helperTextSize.y;
    }

    if(!m_footer.IsEmpty()) {
        dc.SetPen(penColour);
        dc.DrawLine(0, y, rr.GetWidth(), y);

        // Draw the extra line ("1 OF N")
        m_footer.Clear();
        m_footer << (GetTip()->GetCurr() + 1) << " OF " << GetTip()->Count();

        wxFont guiFont = m_font;
        dc.SetFont(guiFont);
        wxSize extraLineSize = dc.GetTextExtent(m_footer);

        wxPoint extraLinePt;
        extraLinePt.x = rr.GetWidth() - extraLineSize.x - TIP_SPACER;
        extraLinePt.y = y + 2;
        dc.SetTextForeground(textColour);
        dc.DrawText(m_footer, extraLinePt);
    }
}

void EditorTipWindow::AddCallTip(gsgsTipInfoPtr tip)
{
    if(tip && tip->Count()) 
    {
        TipInfo ti;
        ti.tip = tip;
        ti.highlightIndex = 0;
        m_highlighIndex = 0;
        if(!m_selectedSignature.IsEmpty()) {
            tip->SelectSiganture(m_selectedSignature);
            m_selectedSignature.Clear();
        }
        m_tips.push_back(ti);
    }
}

gsgsTipInfoPtr EditorTipWindow::GetTip()
{
    if(m_tips.empty()) return NULL;

    return m_tips.at(m_tips.size() - 1).tip;
}

bool EditorTipWindow::IsEmpty() { return m_tips.empty(); }

void EditorTipWindow::Remove()
{
    if(!m_tips.empty()) {
        m_tips.pop_back();

        if(!m_tips.empty()) { m_highlighIndex = m_tips.at(m_tips.size() - 1).highlightIndex; }
    }

    if(m_tips.empty()) Deactivate();
}

void EditorTipWindow::Clear()
{
    m_tips.clear();
    m_tipText.Clear();

    m_highlighIndex = 0;
    m_point = wxPoint();
    m_lineHeight = 0;
}

bool EditorTipWindow::IsActive() { return IsShown(); }

void EditorTipWindow::Highlight(int argIdxToHilight)
{
    gsgsTipInfoPtr tip = GetTip();
    if(tip) {
        if(argIdxToHilight == wxNOT_FOUND) {
            Remove();
        } else {
            m_tipText = tip->Current();
            DoMakeMultipleLineTip();
            if((int)m_args.size() <= argIdxToHilight) {
                // The current tip does not match the requested arg to highlight
                // try to find a better tip
                if(!tip->SelectTipToMatchArgCount(argIdxToHilight)) {
                    Deactivate();
                    return;
                }

                // Update the tip text
                tip = GetTip();
                if(!tip) {
                    Deactivate();
                    return;
                }
                m_tipText = tip->Current();
                DoMakeMultipleLineTip();
            }
            m_highlighIndex = argIdxToHilight;
            DoLayoutTip();
        }
    } else {
        Deactivate();
    }
}

void EditorTipWindow::SelectNext(int argIdxToHilight)
{
    gsgsTipInfoPtr tip = GetTip();
    if(tip) {
        m_tipText = tip->Next();
        DoMakeMultipleLineTip();
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
    }
}

void EditorTipWindow::SelectPrev(int argIdxToHilight)
{
    gsgsTipInfoPtr tip = GetTip();
    if(tip) {
        m_tipText = tip->Prev();
        DoMakeMultipleLineTip();
        m_highlighIndex = argIdxToHilight;
        DoLayoutTip();
    }
}

wxString EditorTipWindow::GetText()
{
    gsgsTipInfoPtr tip = GetTip();
    if(tip) { return tip->All(); }
    return wxT("");
}

void EditorTipWindow::Activate(wxPoint pt, int lineHeight, wxColour parentBgColour)
{
    if(m_tips.empty()) return;

    m_point = pt;
    m_lineHeight = lineHeight;
    m_parentBgColour = parentBgColour;

    // update the font to the current editor
    m_font = gsgs_Style().GetLexer("text")->GetFontForSyle(0);

    DoAdjustPosition();
    if(!IsActive()) {
        Show();
        Refresh();
    }
}

void EditorTipWindow::Deactivate()
{
    Clear();
    if(IsShown()) Hide();
}

wxSize EditorTipWindow::DoGetTipSize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    wxGCDC dc(memDC);
    PrepareDC(dc);

    wxFont f = m_font;
    f.SetWeight(wxFONTWEIGHT_BOLD);

    dc.SetFont(f);
    wxSize helperTextSize = dc.GetTextExtent("Tp");

    int lineHeight = helperTextSize.y;
    int minLineWidth = wxNOT_FOUND;
    if(!m_footer.IsEmpty()) {
        // Multiple signatures
        minLineWidth = dc.GetTextExtent(m_footer).x;
    }

    if(!m_header.IsEmpty()) {
        wxSize headerSize = dc.GetTextExtent(m_header);
        minLineWidth = headerSize.x > minLineWidth ? headerSize.x : minLineWidth;
    }

    minLineWidth += (2 * TIP_SPACER);
    wxString tipContent = wxJoin(m_args, '\n');
    tipContent.Trim().Trim(false);

    wxSize sz;
    wxSize sz2;
    sz = dc.GetMultiLineTextExtent(tipContent);

    // add spacers
    sz.y = (m_args.size() * lineHeight);
    sz.x += (2 * TIP_SPACER);

    if(sz.x < minLineWidth) { sz.x = minLineWidth; }

    if(!m_footer.IsEmpty()) { sz.y += lineHeight; }
    if(!m_header.IsEmpty()) { sz.y += lineHeight; }
    return sz;
}

int EditorTipWindow::DoGetTextLen(wxDC& dc, const wxString& txt)
{
    int xx, yy;
    dc.GetTextExtent(txt, &xx, &yy, NULL, NULL, &m_font);
    return xx;
}

void EditorTipWindow::DoAdjustPosition()
{
    wxPoint pt = m_point;
    wxSize sz = DoGetTipSize();
    wxRect parentSize = GetParent()->GetClientRect();

    if(pt.y + m_lineHeight + sz.y > parentSize.height) {
        pt.y -= sz.y;

    } else {
        pt.y += m_lineHeight;
    }

    if(pt.x + sz.x > parentSize.width) {
        // our tip can not fit into the screen, shift it left
        pt.x -= ((pt.x + sz.x) - parentSize.width);

        if(pt.x < 0) pt.x = 0;
    }
    Move(pt);
}

void EditorTipWindow::DoLayoutTip()
{
    SetSize(DoGetTipSize());
    SetSizeHints(DoGetTipSize());
    DoAdjustPosition();
    Layout();
    Refresh();
}

void EditorTipWindow::SelectSignature(const wxString& signature)
{
    m_selectedSignature = signature;
    if(GetTip()) {
        GetTip()->SelectSiganture(m_selectedSignature);
        m_selectedSignature.clear();
    }
}

void EditorTipWindow::OnEditoConfigChanged(gsgs::Event& e)
{
    e.Skip();
    // the lexers were modified by the user, reload the font
    m_font = gsgs_Style().GetLexer("text")->GetFontForSyle(0);
    Refresh();
}

void EditorTipWindow::DoMakeMultipleLineTip()
{
    // Find the open brace first
    m_args.Clear();
    m_footer.Clear();
    m_header.Clear();

    wxString sig = m_tipText.AfterFirst('(');
    wxString returnValue = m_tipText.BeforeFirst('(');
    returnValue.Trim().Trim(false);
    if(returnValue.EndsWith(":")) {
        returnValue.RemoveLast();
        returnValue.Trim().Trim(false);
        m_header << "RETURNS:  " << returnValue;
    }

    sig = sig.BeforeLast(')');
    sig.Trim().Trim(false);
    if(sig.IsEmpty()) {
        m_args.Add("()");
        if(GetTip() && (GetTip()->Count() > 1)) { m_footer << GetTip()->GetCurr() << " OF " << GetTip()->Count(); }
        return;
    }

    int depth = 0;
    wxString currentArg;
    for(size_t i = 0; i < sig.length(); ++i) {
        wxChar ch = sig.at(i);
        if(ch == '<' || ch == '[' || ch == '{' || ch == '(') {
            ++depth;
            currentArg << ch;
        } else if(ch == '>' || ch == ']' || ch == '}' || ch == ')') {
            --depth;
            currentArg << ch;
            if(depth < 0) {
                // a parsing error
                m_tipText.Clear();
                m_args.Clear();
                return;
            }
        } else if(ch == ',' && (depth == 0)) {
            currentArg.Trim().Trim(false);
            m_args.Add(currentArg);
            currentArg.Clear();
        } else {
            currentArg << ch;
        }
    }

    if(!currentArg.IsEmpty()) {
        currentArg.Trim().Trim(false);
        m_args.Add(currentArg);
    }

    if(GetTip() && (GetTip()->Count() > 1)) { m_footer << GetTip()->GetCurr() << " OF " << GetTip()->Count(); }
}
}