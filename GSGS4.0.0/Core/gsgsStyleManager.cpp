#include "gsgsStyleManager.h"
#include "gsgsConfig.h"
#include "EclipseThemeImporterManager.h"
#include "gsgsPathManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "gsgsCommonHandler.h"
#include "ieditor.h"
#include "globals.h"
#include "gsgsJSON.h"
#include "imanager.h"
#include "xmlutils.h"
#include "plugin.h"
#include <wx/app.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/graphics.h>
#include <wx/renderer.h>
#include <wx/stc/stc.h>
#include <wx/busyinfo.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>
#include <wx/xrc/xmlres.h>
#include <wx/aui/auibar.h>
#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/html/htmlwin.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>

#if USE_AUI_NOTEBOOK
#include "gsgsNotebookTabArt.h"
#include <wx/aui/tabart.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#define DEFAULT_FACE_NAME "Consolas"
#define DEFAULT_FONT_SIZE 12
#elif defined(__WXMAC__)
#define DEFAULT_FACE_NAME "monaco"
#define DEFAULT_FONT_SIZE 12
#else // GTK, FreeBSD etc
#define DEFAULT_FACE_NAME "monospace"
#define DEFAULT_FONT_SIZE 12
#endif

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash^M
// See http://trac.wxwidgets.org/ticket/10883^M
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

#ifdef __WXOSX__
#define FACTOR_ALTROW_LIGHT 95
#define FACTOR_ALTROW_DARK 105
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 110
#else
#define FACTOR_ALTROW_LIGHT 98
#define FACTOR_ALTROW_DARK 102
#define FACTOR_SEL_BG_COLOUR_NOFOCUS 145
#endif

#define LEXERS_VERSION_STRING "LexersVersion"
#define LEXERS_VERSION 5

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // Colour
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    Colour::Colour() {}
    //-----------------------------------------------------------------------------------
    void Colour::setDefault()
    {
        //setBase(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
        //itemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
        //selItemTextColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT);
        setBase(DefaultBackgroundColour);
        itemTextColour = DefaultTextColour;
        selItemTextColour = DefaultActiveTextColour;
    }
    //-----------------------------------------------------------------------------------
    void Colour::setDark()
    {
        setBase(wxColour("#5F6A6A"));
    }
    //-----------------------------------------------------------------------------------
    void Colour::setBase(const wxColour& baseColour)
    {
        if (!baseColour.IsOk()) 
        {
            setDefault();
            return;
        }
        bool is_dark = IsDark(baseColour);
        bool is_light = !is_dark;
        bgColour = baseColour;
        itemTextColour = is_light ? wxColour("#212121") : wxColour("#FDFEFE");

        if (is_light) {
            alternateColour = bgColour.ChangeLightness(FACTOR_ALTROW_LIGHT);
            hoverBgColour = bgColour.ChangeLightness(110);
            headerBgColour = bgColour.ChangeLightness(96);
            headerHBorderColour = headerBgColour.ChangeLightness(90);
            headerVBorderColour = headerBgColour.ChangeLightness(90);
            selItemTextColour = itemTextColour;
            selItemBgColour = bgColour.ChangeLightness(80);
            selbuttonColour = selItemTextColour.ChangeLightness(120);
            buttonColour = itemTextColour.ChangeLightness(120);
            selItemBgColourNoFocus = selItemBgColour.ChangeLightness(FACTOR_SEL_BG_COLOUR_NOFOCUS);
            grayText = itemTextColour.ChangeLightness(150);
        }
        else {
            alternateColour = bgColour.ChangeLightness(FACTOR_ALTROW_DARK);
            hoverBgColour = bgColour.ChangeLightness(90);
            headerBgColour = bgColour.ChangeLightness(105);
            headerHBorderColour = headerBgColour.ChangeLightness(112);
            headerVBorderColour = headerBgColour.ChangeLightness(112);
            selItemTextColour = itemTextColour;
            selItemBgColour = bgColour.ChangeLightness(130);
            selbuttonColour = selItemTextColour.ChangeLightness(80);
            buttonColour = itemTextColour.ChangeLightness(80);
            selItemBgColourNoFocus = bgColour.ChangeLightness(115);
            grayText = itemTextColour.ChangeLightness(50);
        }
        itemBgColour = bgColour;
        matchedItemBgText = wxColour("#8BC34A");
        matchedItemText = wxColour("#FDFEFE");
        fillColour = bgColour;
        borderColour = is_light ? bgColour.ChangeLightness(70) : bgColour.ChangeLightness(110);
        darkBorderColour = is_light ? bgColour.ChangeLightness(30) : bgColour.ChangeLightness(150);
    }
    //-----------------------------------------------------------------------------------
    #if USE_AUI_NOTEBOOK
    class MySimpleTabArt : public wxAuiSimpleTabArt
    {
        wxColour m_activeTabBgColour;

    public:
        MySimpleTabArt() { m_activeTabBgColour = *wxWHITE; }
        virtual ~MySimpleTabArt() {}
        wxAuiTabArt* Clone() { return new MySimpleTabArt(*this); }

        void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState,
                     wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
        {
            if(pane.active) {
                if(Colour::IsDark(m_activeTabBgColour)) { dc.SetTextForeground(*wxWHITE); }
            } else {
                // set the default text colour
                dc.SetTextForeground(gsgs_Style().GetPanelTextColour());
            }
            wxAuiSimpleTabArt::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
        }

        void SetActiveColour(const wxColour& colour)
        {
            m_activeTabBgColour = colour;
            wxAuiSimpleTabArt::SetActiveColour(colour);
        }
    };
    class MyDefaultTabArt : public wxAuiGenericTabArt
    {
        wxColour m_activeTabBgColour;

    public:
        MyDefaultTabArt() { m_activeTabBgColour = *wxWHITE; }
        virtual ~MyDefaultTabArt() {}
        wxAuiTabArt* Clone() { return new MyDefaultTabArt(*this); }

        void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState,
                     wxRect* outTabRect, wxRect* outButtonRect, int* xExtent)
        {
            if(pane.active) {
                if(Colour::IsDark(m_activeTabBgColour)) { dc.SetTextForeground(*wxWHITE); }
            } else {
                // set the default text colour
                dc.SetTextForeground(gsgs_Style().GetPanelTextColour());
            }
            wxAuiGenericTabArt::DrawTab(dc, wnd, pane, inRect, closeButtonState, outTabRect, outButtonRect, xExtent);
        }

        void SetActiveColour(const wxColour& colour)
        {
            m_activeTabBgColour = colour;
            wxAuiGenericTabArt::SetActiveColour(colour);
        }
    };
    #endif
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_SYS_COLOURS_CHANGED, Event);
    wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_START, Event);
    wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_END, Event);
    wxDEFINE_EVENT(wxEVT_UPGRADE_LEXERS_PROGRESS, Event);
    //-----------------------------------------------------------------------------------
    static const wxString LexerTextDefaultXML =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<Lexer Name=\"text\" Theme=\"Default\" IsActive=\"No\" UseCustomTextSelFgColour=\"Yes\" "
    "StylingWithinPreProcessor=\"yes\" Id=\"1\">"
    "  <KeyWords0/>"
    "  <KeyWords1/>"
    "  <KeyWords2/>"
    "  <KeyWords3/>"
    "  <KeyWords4/>"
    "  <Extensions/>"
    "  <Properties>"
    "    <Property Id=\"0\" Name=\"Default\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"34\" Name=\"Brace match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"cyan\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"35\" Name=\"Brace bad match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"red\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"37\" Name=\"Indent Guide\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-1\" Name=\"Fold Margin\" Bold=\"no\" Face=\"\" Colour=\"white\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-2\" Name=\"Text Selection\" Bold=\"no\" Face=\"\" Colour=\"#4E687D\" BgColour=\"#D6D2D0\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-3\" Name=\"Caret Colour\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-4\" Name=\"Whitespace\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"38\" Name=\"Calltip\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"33\" Name=\"Line Numbers\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"34\" Name=\"Brace match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"cyan\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"35\" Name=\"Brace bad match\" Bold=\"yes\" Face=\"\" Colour=\"black\" BgColour=\"red\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"37\" Name=\"Indent Guide\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-1\" Name=\"Fold Margin\" Bold=\"no\" Face=\"\" Colour=\"white\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-2\" Name=\"Text Selection\" Bold=\"no\" Face=\"\" Colour=\"#4E687D\" BgColour=\"#D6D2D0\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-3\" Name=\"Caret Colour\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"-4\" Name=\"Whitespace\" Bold=\"no\" Face=\"\" Colour=\"#7F7F7F\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"38\" Name=\"Calltip\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" Italic=\"no\" "
    "Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "    <Property Id=\"33\" Name=\"Line Numbers\" Bold=\"no\" Face=\"\" Colour=\"black\" BgColour=\"white\" "
    "Italic=\"no\" Underline=\"no\" EolFilled=\"no\" Alpha=\"50\" Size=\"11\"/>"
    "  </Properties>"
    "</Lexer>";

    //---------------------------------------------------------------------------
    StyleManager::StyleManager()
        : mInit(false)
    {
        mATMColour = DefaultATMColour;
        mHTMColour = DefaultHTMColour;
        mAppBackground = DefaultBackgroundColour;
        mAppTextColour = DefaultTextColour;
        mAppActiveColour = DefaultActiveTextColour;
        mDarkColours.setDark();
        mDefaultColours.setDefault();
        gsgs_Config().Read("UseCustomBaseColour", mCustomColour, false);
        if(mCustomColour) 
        {
            gsgs_Config().Read("BaseColour", mAppBackground, mAppBackground);
            mCustomColours.setBase(mAppBackground);
        }
        // Create a "go to lexer" when all is broken
        wxStringInputStream sis(LexerTextDefaultXML);
        wxXmlDocument doc;
        if (doc.Load(sis)) 
        {
            m_defaultLexer.Reset(new LexerStyle());
            m_defaultLexer->FromXml(doc.GetRoot());
        }

        mDefaultFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
#ifdef __WXGTK__
        static double ratio = 1.0;
        static bool once = false;
        if (!once) {
            GdkScreen* screen = gdk_screen_get_default();
            if (screen) {
                double res = gdk_screen_get_resolution(screen);
                ratio = (res / 96.);
            }
            once = true;
        }
#if wxCHECK_VERSION(3, 1, 2)
        float pointSize = mDefaultFont.GetFractionalPointSize() * ratio;
        mDefaultFont.SetFractionalPointSize(pointSize);
#else
        int pointSize = mDefaultFont.GetPointSize() * ratio;
        mDefaultFont.SetPointSize(pointSize);
#endif
#endif

        mDefaultActiveFont = mDefaultFont;
        mDefaultActiveFont.SetWeight(wxFONTWEIGHT_BOLD);

        mTextAttr.SetTextColour(GetActiveMarkerColour());

        gsgs_Config().Read(LEXERS_VERSION_STRING, mVersion, LEXERS_UPGRADE_LINENUM_DEFAULT_COLOURS);
        gsgs_Event().Bind(wxEVT_INFO_BAR_BUTTON, &StyleManager::OnAdjustTheme, this);
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &StyleManager::OnColoursChanged, this);
    }
    //---------------------------------------------------------------------------
    StyleManager::~StyleManager()
    {
        gsgs_Config().Write(LEXERS_VERSION_STRING, LEXERS_VERSION);
        gsgs_Event().Unbind(wxEVT_INFO_BAR_BUTTON, &StyleManager::OnAdjustTheme, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &StyleManager::OnColoursChanged, this);
    }
    //---------------------------------------------------------------------------
    const wxColour & StyleManager::GetColour(wxSystemColour index) const
    {
        if(mCustomColour) {
            switch(index) {
            case wxSYS_COLOUR_3DFACE:
            case wxSYS_COLOUR_LISTBOX:
                return mCustomColours.GetBgColour();
            case wxSYS_COLOUR_3DSHADOW:
                return mCustomColours.GetBorderColour();
            case wxSYS_COLOUR_3DDKSHADOW:
                return mCustomColours.GetDarkBorderColour();
            case wxSYS_COLOUR_WINDOWTEXT:
            case wxSYS_COLOUR_BTNTEXT:
                return mCustomColours.GetItemTextColour();
            case wxSYS_COLOUR_GRAYTEXT:
                return mCustomColours.GetGrayText();
            default:
                static wxColour temp1;
                temp1 = wxSystemSettings::GetColour(index);
                return temp1;
            }
        } else {
            static wxColour temp2;
            temp2 = wxSystemSettings::GetColour(index);
            return temp2;
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::OnColoursChanged(Event& event)
    {
        event.Skip();
        if(mCustomColour) 
        {
            // Make sure that the notebook colours are getting updated as well
            ThemeHandlerHelper helper(gsgs_Event().TopFrame());
            helper.UpdateNotebookColours(gsgs_Event().TopFrame());
        }
        // Notify about colours changes
        Event evtColoursChanged(wxEVT_SYS_COLOURS_CHANGED);
        gsgs_Event().AddPendingEvent(evtColoursChanged);
        CallAfter(&StyleManager::UpdateGlobalColours);
    }
    //---------------------------------------------------------------------------
    void StyleManager::UpdateGlobalColours()
    {
        std::for_each(m_windows.begin(), m_windows.end(), [&](wxWindow* win) 
        {
            win->SetBackgroundColour(gsgs_Style().GetAppBgColour());
            win->SetForegroundColour(GetColour(wxSYS_COLOUR_BTNTEXT));
            win->Refresh();
        });
    }
    //---------------------------------------------------------------------------
    void StyleManager::RegisterWindow(wxWindow* win)
    {
        win->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        win->SetForegroundColour(GetColour(wxSYS_COLOUR_BTNTEXT));
        m_windows.insert(win);
    }
    //---------------------------------------------------------------------------
    void StyleManager::UnRegisterWindow(wxWindow* win)
    {
        if(m_windows.count(win))
            m_windows.erase(win);
    }
    //---------------------------------------------------------------------------
    static void RGBtoHSB(int r, int g, int b, float* h, float* s, float* br)
    {
        float hue, saturation, brightness;
        int cmax = (r > g) ? r : g;
        if (b > cmax) cmax = b;
        int cmin = (r < g) ? r : g;
        if (b < cmin) cmin = b;

        brightness = ((float)cmax) / 255.0f;
        if (cmax != 0)
            saturation = ((float)(cmax - cmin)) / ((float)cmax);
        else
            saturation = 0;
        if (saturation == 0)
            hue = 0;
        else {
            float redc = ((float)(cmax - r)) / ((float)(cmax - cmin));
            float greenc = ((float)(cmax - g)) / ((float)(cmax - cmin));
            float bluec = ((float)(cmax - b)) / ((float)(cmax - cmin));
            if (r == cmax)
                hue = bluec - greenc;
            else if (g == cmax)
                hue = 2.0f + redc - bluec;
            else
                hue = 4.0f + greenc - redc;
            hue = hue / 6.0f;
            if (hue < 0) hue = hue + 1.0f;
        }
        (*h) = hue;
        (*s) = saturation;
        (*br) = brightness;
    }
    //---------------------------------------------------------------------------
    //////////////////////////////////////////////////
    // Colour methods to convert HSL <-> RGB
    //////////////////////////////////////////////////
    static float cl_min(float x, float y, float z)
    {
        float m = x < y ? x : y;
        m = m < z ? m : z;
        return m;
    }
    //---------------------------------------------------------------------------
    static float cl_max(float x, float y, float z)
    {
        float m = x > y ? x : y;
        m = m > z ? m : z;
        return m;
    }
    //---------------------------------------------------------------------------
    static void RGB_2_HSL(float r, float g, float b, float* h, float* s, float* l)
    {
        float var_R = (r / 255.0); // RGB from 0 to 255
        float var_G = (g / 255.0);
        float var_B = (b / 255.0);

        float var_Min = cl_min(var_R, var_G, var_B); // Min. value of RGB
        float var_Max = cl_max(var_R, var_G, var_B); // Max. value of RGB
        float del_Max = var_Max - var_Min;           // Delta RGB value

        *l = (var_Max + var_Min) / 2.0;

        if (del_Max == 0) { // This is a gray, no chroma...
            *h = 0;        // HSL results from 0 to 1
            *s = 0;
        }
        else { // Chromatic data...
            if (*l < 0.5)
                *s = del_Max / (var_Max + var_Min);
            else
                *s = del_Max / (2.0 - var_Max - var_Min);

            float del_R = (((var_Max - var_R) / 6.0) + (del_Max / 2.0)) / del_Max;
            float del_G = (((var_Max - var_G) / 6.0) + (del_Max / 2.0)) / del_Max;
            float del_B = (((var_Max - var_B) / 6.0) + (del_Max / 2.0)) / del_Max;

            if (var_R == var_Max)
                *h = del_B - del_G;
            else if (var_G == var_Max)
                *h = (1.0 / 3.0) + del_R - del_B;
            else if (var_B == var_Max)
                *h = (2.0 / 3.0) + del_G - del_R;

            if (*h < 0) *h += 1;
            if (*h > 1) *h -= 1;
        }
    }
    //---------------------------------------------------------------------------
    static float Hue_2_RGB(float v1, float v2, float vH) // Function Hue_2_RGB
    {
        if (vH < 0) vH += 1;
        if (vH > 1) vH -= 1;
        if ((6.0 * vH) < 1) return (v1 + (v2 - v1) * 6.0 * vH);
        if ((2.0 * vH) < 1) return (v2);
        if ((3.0 * vH) < 2) return (v1 + (v2 - v1) * ((2.0 / 3.0) - vH) * 6.0);
        return (v1);
    }
    //---------------------------------------------------------------------------
    static void HSL_2_RGB(float h, float s, float l, float* r, float* g, float* b)
    {
        if (s == 0) {        // HSL from 0 to 1
            *r = l * 255.0; // RGB results from 0 to 255
            *g = l * 255.0;
            *b = l * 255.0;
        }
        else {
            float var_2;
            if (l < 0.5)
                var_2 = l * (1.0 + s);
            else
                var_2 = (l + s) - (s * l);

            float var_1 = 2.0 * l - var_2;

            *r = 255.0 * Hue_2_RGB(var_1, var_2, h + (1.0 / 3.0));
            *g = 255.0 * Hue_2_RGB(var_1, var_2, h);
            *b = 255.0 * Hue_2_RGB(var_1, var_2, h - (1.0 / 3.0));
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::LightColour(const wxColour& color, wxColour & out, float percent)
    {
        if (percent == 0)
            out = color;

        float h, s, l, r, g, b;
        RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

        // reduce the Lum value
        l += (float)((percent * 5.0) / 100.0);
        if (l > 1.0) l = 1.0;

        HSL_2_RGB(h, s, l, &r, &g, &b);

        out.Set((unsigned char)r, (unsigned char)g, (unsigned char)b);
    }
    //---------------------------------------------------------------------------
    void StyleManager::TruncateText(const wxString& text, int maxWidth, wxDC& dc, wxString& fixedText)
    {
        int textH, textW;
        int rectSize = maxWidth + 4; // error size
        // int textLen = (int)text.Length();
        wxString tempText = text;

        fixedText = wxT("");
        dc.GetTextExtent(text, &textW, &textH);
        if (rectSize >= textW) {
            fixedText = text;
            return;
        }

        // The text does not fit in the designated area,
        // so we need to truncate it a bit
        wxString suffix = wxT("..");
        int w, h;
        dc.GetTextExtent(suffix, &w, &h);
        rectSize -= w;

        int mid = (text.size() / 2);
        wxString text1 = text.Mid(0, mid);
        wxString text2 = text.Mid(mid);
        int min = std::min(text1.size(), text2.size());
        for (int i = 0; i < min; ++i) {
            text1.RemoveLast();
            text2.Remove(0, 1);

            fixedText = text1 + suffix + text2;
            dc.GetTextExtent(fixedText, &textW, &textH);
            if (rectSize >= textW) { return; }
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor,
        const wxColour& endColor, bool vertical)
    {
        int rd, gd, bd, high = 0;
        rd = endColor.Red() - startColor.Red();
        gd = endColor.Green() - startColor.Green();
        bd = endColor.Blue() - startColor.Blue();

        /// Save the current pen and brush
        wxPen savedPen = dc.GetPen();
        wxBrush savedBrush = dc.GetBrush();

        if (vertical)
            high = rect.GetHeight() - 1;
        else
            high = rect.GetWidth() - 1;

        if (high < 1) return;

        for (int i = 0; i <= high; ++i) {
            int r = startColor.Red() + ((i * rd * 100) / high) / 100;
            int g = startColor.Green() + ((i * gd * 100) / high) / 100;
            int b = startColor.Blue() + ((i * bd * 100) / high) / 100;

            wxPen p(wxColour(r, g, b));
            dc.SetPen(p);

            if (vertical)
                dc.DrawLine(rect.x, rect.y + i, rect.x + rect.width, rect.y + i);
            else
                dc.DrawLine(rect.x + i, rect.y, rect.x + i, rect.y + rect.height);
        }

        /// Restore the pen and brush
        dc.SetPen(savedPen);
        dc.SetBrush(savedBrush);
    }
    //---------------------------------------------------------------------------
    bool Colour::IsDark(const wxColour& color)
    {
        float h, s, b;
        RGBtoHSB(color.Red(), color.Green(), color.Blue(), &h, &s, &b);
        return (b < 0.5);
    }
    //---------------------------------------------------------------------------
    void StyleManager::DarkColour(const wxColour& color, wxColour & out, float percent)
    {
        if (percent == 0) 
            out = color;

        float h, s, l, r, g, b;
        RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);

        // reduce the Lum value
        l -= (float)((percent * 5.0) / 100.0);
        if (l < 0) l = 0.0;

        HSL_2_RGB(h, s, l, &r, &g, &b);
        out.Set((unsigned char)r, (unsigned char)g, (unsigned char)b);
    }
    //---------------------------------------------------------------------------
    void StyleManager::SetCustomColour(bool set, const wxColour & bgcolour)
    {
        mCustomColour = set;
        gsgs_Config().Write("UseCustomBaseColour", mCustomColour);
        if (mCustomColour)
        {
            mAppBackground = bgcolour;
            gsgs_Config().Write("BaseColour", mAppBackground);
            mCustomColours.setBase(mAppBackground);

            Event event(wxEVT_CMD_COLOURS_FONTS_UPDATED);
            gsgs_Event().AddPendingEvent(event);
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::FillMenuBarBgColour(wxDC& dc, const wxRect& rect, bool miniToolbar)
    {
    #ifdef __WXMSW__
        const wxColour & brushColour(gsgs_Style().GetAppBgColour());
        wxColour bottomColour = brushColour.ChangeLightness(90);

        dc.SetPen(brushColour);
        dc.SetBrush(brushColour);
        dc.DrawRectangle(rect);

        //dc.SetPen(topColour);
        //dc.DrawLine(rect.GetTopLeft(), rect.GetTopRight());

        dc.SetPen(bottomColour);
        dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());

    #elif defined(__WXOSX__)

        const wxColour & bgColour = gsgs_Style().GetAppBgColour();
        dc.SetPen(bgColour);
        dc.SetBrush(bgColour);
        dc.DrawRectangle(rect);

        wxColour lineColour = bgColour.ChangeLightness(80);
        dc.SetPen(lineColour);
        dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    #else

        const wxColour & bgColour = gsgs_Style().GetAppBgColour();
        dc.SetPen(bgColour);
        dc.SetBrush(bgColour);
        dc.DrawRectangle(rect);

        wxColour lineColour = bgColour.ChangeLightness(90);
        dc.SetPen(lineColour);
        dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
    #endif
    }
    //---------------------------------------------------------------------------
    bool StyleManager::GetGCDC(wxDC& dc, wxGCDC& gdc)
    {
        wxGraphicsRenderer* const renderer = wxGraphicsRenderer::GetDefaultRenderer();
        wxGraphicsContext* context;

        if (wxPaintDC* paintdc = wxDynamicCast(&dc, wxPaintDC)) {
            context = renderer->CreateContext(*paintdc);

        }
        else if (wxMemoryDC* memdc = wxDynamicCast(&dc, wxMemoryDC)) {
            context = renderer->CreateContext(*memdc);

        }
        else {
            wxFAIL_MSG("Unknown wxDC kind");
            return false;
        }

        gdc.SetGraphicsContext(context);
        return true;
    }
    //---------------------------------------------------------------------------
    wxBrush StyleManager::GetStippleBrush()
    {
        wxMemoryDC memDC;
        wxColour bgColour = GetAUIPaneBGColour();
        // if(GetManager() && GetManager()->GetStatusBar() && GetManager()->GetStatusBar()->GetArt()) {
        //     bgColour = GetManager()->GetStatusBar()->GetArt()->GetBgColour();
        // }
        wxBitmap bmpStipple(3, 3);
        wxColour lightPen;
        wxColour darkPen;
        DarkColour(bgColour, lightPen, 5.0);
        LightColour(bgColour, darkPen, 3.0);
        memDC.SelectObject(bmpStipple);
        memDC.SetBrush(bgColour);
        memDC.SetPen(bgColour);
        memDC.DrawRectangle(wxPoint(0, 0), bmpStipple.GetSize());

        /// Draw all the light points, we have 3 of them
        memDC.SetPen(lightPen);
        memDC.DrawPoint(0, 2);
        memDC.DrawPoint(2, 0);

        /// and 2 dark points
        memDC.SetPen(darkPen);
        memDC.DrawPoint(0, 1);

        memDC.SelectObject(wxNullBitmap);
        return wxBrush(bmpStipple);
    }
    //---------------------------------------------------------------------------
    bool StyleManager::DrawStippleBackground(const wxRect& rect, wxDC& dc)
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(GetStippleBrush());
        dc.DrawRectangle(rect);
        return true;
    }
    //---------------------------------------------------------------------------
    wxFont StyleManager::GetDefaultFixedFont()
    {
        wxFont f(GetDefaultGuiFont());
        f.SetFamily(wxFONTFAMILY_TELETYPE);
        f.SetFaceName(DEFAULT_FACE_NAME);
        // f.SetPointSize(DEFAULT_FONT_SIZE);
        return f;
    }
    //---------------------------------------------------------------------------
    #ifdef __WXOSX__
    double wxOSXGetMainScreenContentScaleFactor();
    #endif
    #define DROPDOWN_ARROW_SIZE 20
    //---------------------------------------------------------------------------
    void StyleManager::DrawButton(wxDC& dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp,
        eButtonKind kind, int state)
    {
        // Draw the background
        wxRect clientRect = rect;
        dc.SetPen(GetPanelBgColour());
        dc.SetBrush(GetPanelBgColour());
        dc.DrawRectangle(clientRect);

        // Now draw the border around this control
        // clientRect.Deflate(2);

        wxColour baseColour = GetButtonBgColour();
        wxColour textColour = GetButtonTextColour();
        wxColour penColour = baseColour.ChangeLightness(80);

        int bgLightness = 0;
        switch (state) {
        case Button::State::BST_Hover:
    #ifdef __WXMSW__
            bgLightness = 140;
    #else
            bgLightness = 115;
    #endif
            break;
        case Button::State::BST_Press:
            bgLightness = 80;
            break;
        default:
        case Button::State::BST_Normal:
            bgLightness = 100;
            break;
        }

        wxRect downRect = rect;
        downRect.SetHeight(rect.GetHeight() / 2);
        downRect.SetY(rect.GetY() + (rect.GetHeight() / 2));

        wxColour bgColour = baseColour.ChangeLightness(bgLightness);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(bgColour);
        dc.DrawRectangle(clientRect);

        dc.SetBrush(bgColour.ChangeLightness(96));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(downRect);

        dc.SetPen(penColour);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(clientRect);

        clientRect.Deflate(1);
        wxRect textRect, arrowRect;
        textRect = clientRect;
        if (kind == eButtonKind::kDropDown) {
            // we need to save space for the drop down arrow
            int xx = textRect.x + (textRect.GetWidth() - DROPDOWN_ARROW_SIZE);
            int yy = textRect.y + ((textRect.GetHeight() - DROPDOWN_ARROW_SIZE) / 2);
            textRect.SetWidth(textRect.GetWidth() - DROPDOWN_ARROW_SIZE);
            arrowRect = wxRect(xx, yy, DROPDOWN_ARROW_SIZE, DROPDOWN_ARROW_SIZE);
        }

        if (bmp.IsOk()) {
            if (label.IsEmpty()) {
                // There is no label, draw the bitmap centred
                // Bitmaps are drawn to the _left_ of the text
                int bmpX = textRect.GetX() + ((textRect.GetWidth() - bmp.GetScaledWidth()) / 2);
                int bmpY = textRect.GetY() + ((textRect.GetHeight() - bmp.GetScaledHeight()) / 2);
                dc.DrawBitmap(bmp, bmpX, bmpY);
            }
            else {
                // Bitmaps are drawn to the _left_ of the text
                int xx = textRect.GetX();
                xx += 5;
                int bmpY = textRect.GetY() + ((textRect.GetHeight() - bmp.GetScaledHeight()) / 2);
                dc.DrawBitmap(bmp, xx, bmpY);
                xx += bmp.GetScaledWidth();
                textRect.SetX(xx);
            }
        }

        // Draw the label
        if (!label.IsEmpty()) {
            wxString truncatedText;
            TruncateText(label, textRect.GetWidth() - 5, dc, truncatedText);
            wxSize textSize = dc.GetTextExtent(label);
            int textY = textRect.GetY() + ((textRect.GetHeight() - textSize.GetHeight()) / 2);
            dc.SetClippingRegion(textRect);
            dc.SetFont(GetDefaultGuiFont());
            dc.SetTextForeground(textColour);
            dc.DrawText(truncatedText, textRect.GetX() + 5, textY);
            dc.DestroyClippingRegion();
        }

        // Draw the drop down button
        if (kind == eButtonKind::kDropDown) {
            dc.SetPen(penColour);
            dc.SetBrush(baseColour);
            DrawDropDownArrow(win, dc, arrowRect, false);
            dc.SetPen(penColour);
            dc.DrawLine(arrowRect.GetX(), clientRect.GetTopLeft().y, arrowRect.GetX(), clientRect.GetBottomLeft().y);
        }
    }
    //---------------------------------------------------------------------------
    wxSize StyleManager::GetBestSize(const wxString& label, int xspacer, int yspacer)
    {
        wxBitmap bmp(1, 1);
        wxMemoryDC memDC(bmp);
        memDC.SetFont(GetDefaultGuiFont());
        wxSize size = memDC.GetTextExtent(label);
        size.SetWidth(size.GetWidth() + (2 * xspacer));
        size.SetHeight(size.GetHeight() + (2 * yspacer));
        return size;
    }
    //---------------------------------------------------------------------------
    void StyleManager::DrawClose(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
        const wxColour& bgColour, int state, int width)
    {
    #if 0
        size_t flags = 0;
        switch (state) {
        case Button::State::BST_Hover:
            flags = wxCONTROL_CURRENT;
            break;
        case Button::State::BST_Press:
            flags = wxCONTROL_PRESSED;
            break;
        default:
            break;
        }
        wxRendererNative::Get().DrawTitleBarBitmap(win, dc, rect, wxTITLEBAR_BUTTON_CLOSE, flags);
        if (IsDark(bgColour)) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rect);
        }
    #else
        wxRect innerRect(rect);
        wxColour b = bgColour;
        wxColour xColour = penColour;
        switch (state) {
        case Button::State::BST_Hover:
            b = b.ChangeLightness(120);
            xColour = wxColour("#ff0000");
            break;
        case Button::State::BST_Press:
            xColour = b.ChangeLightness(120);
            b = wxColour("#df0000");
            break;
        default:
            break;
        }

        if (state != Button::State::BST_Normal) {
            dc.SetPen(b);
            dc.SetBrush(b);
            dc.DrawRoundedRectangle(rect, 3.0);
        }

        innerRect.Deflate(1);
        innerRect = innerRect.CenterIn(rect);
        dc.SetPen(wxPen(xColour, width));
        dc.SetBrush(xColour);
        dc.DrawLine(innerRect.GetTopLeft(), innerRect.GetBottomRight());
        dc.DrawLine(innerRect.GetTopRight(), innerRect.GetBottomLeft());
    #endif
    }
    //---------------------------------------------------------------------------
    void StyleManager::DrawAlter(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
        const wxColour& bgColour, int state, int width)
    {
    #if 0
        size_t flags = 0;
        switch (state) {
        case Button::State::BST_Hover:
            flags = wxCONTROL_CURRENT;
            break;
        case Button::State::BST_Press:
            flags = wxCONTROL_PRESSED;
            break;
        default:
            break;
        }
        wxRendererNative::Get().DrawTitleBarBitmap(win, dc, rect, wxTITLEBAR_BUTTON_MAXIMIZE, flags);
        if (IsDark(bgColour)) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rect);
        }
    #else
        // Calculate the circle radius:
        wxRect innerRect(rect);
        wxColour b = bgColour;
        wxColour xColour = penColour;
        switch (state) {
        case Button::State::BST_Hover:
            b = b.ChangeLightness(120);
            break;
        case Button::State::BST_Press:
            b = b.ChangeLightness(70);
            xColour = b.ChangeLightness(150);
            break;
        default:
            break;
        }

        if (state != Button::State::BST_Normal) {
            dc.SetPen(b);
            dc.SetBrush(b);
            dc.DrawRoundedRectangle(rect, 3.0);
        }

        innerRect.Deflate(2);
        innerRect = innerRect.CenterIn(rect);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(xColour, width));
        dc.DrawRectangle(innerRect);
        //innerRect.Deflate(0, 3);
        //dc.DrawLine(innerRect.GetTopLeft(), innerRect.GetTopRight());
    #endif
    }
    //---------------------------------------------------------------------------
    void StyleManager::DrawMinimize(wxDC& dc, wxWindow* win, const wxRect& rect, const wxColour& penColour,
        const wxColour& bgColour, int state, int width)
    {
#if 0
        size_t flags = 0;
        switch (state) {
        case Button::State::BST_Hover:
            flags = wxCONTROL_CURRENT;
            break;
        case Button::State::BST_Press:
            flags = wxCONTROL_PRESSED;
            break;
        default:
            break;
        }
        wxRendererNative::Get().DrawTitleBarBitmap(win, dc, rect, wxTITLEBAR_BUTTON_MAXIMIZE, flags);
        if (IsDark(bgColour)) {
            dc.SetBrush(*wxTRANSPARENT_BRUSH);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rect);
        }
#else
        wxRect innerRect(rect);
        wxColour b = bgColour;
        wxColour xColour = penColour;
        switch (state) {
        case Button::State::BST_Hover:
            b = b.ChangeLightness(120);
            break;
        case Button::State::BST_Press:
            b = b.ChangeLightness(70);
            xColour = b.ChangeLightness(150);
            break;
        default:
            break;
        }

        if (state != Button::State::BST_Normal) {
            dc.SetPen(b);
            dc.SetBrush(b);
            dc.DrawRoundedRectangle(rect, 3.0);
        }

        innerRect.Deflate(2);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(xColour, width));
        int theight = innerRect.GetBottom();
        dc.DrawLine(innerRect.GetLeft(), theight, innerRect.GetRight(), theight);
#endif
    }
    //---------------------------------------------------------------------------
    void StyleManager::DrawDropDownArrow(wxWindow* win, wxDC& dc, const wxRect& rect, bool hover, int padsize, const wxColour& colour)
    {
        // Draw an arrow
        wxRect buttonRect(rect);
        int sz = wxMin(rect.GetHeight(), rect.GetWidth());
        sz = wxMin(10, sz);
        int height = (sz / 3.0) * 2;
        buttonRect.SetHeight(height);
        buttonRect.SetWidth(sz);
        buttonRect = buttonRect.CenterIn(rect);

        wxColour buttonColour;
        if (colour.IsOk())
        {
            buttonColour = colour;
        }
        else
        {
            // No colour provided, provide one
            /*wxColour buttonFace = GetColour(wxSYS_COLOUR_BTNFACE);
            if (Colour::IsDark(buttonFace)) {
                buttonColour = buttonFace.ChangeLightness(150);
            }
            else {
                buttonColour = GetColour(wxSYS_COLOUR_3DDKSHADOW);
            }*/
            buttonColour = gsgs_Style().GetActiveMarkerColour();
        }
        if (hover)
        {
            dc.SetPen(DefaultHTMColour);
            dc.SetBrush(DefaultHTMColour);
            wxRect temp = rect;
            temp.Inflate(padsize);
            dc.DrawRoundedRectangle(temp, 2);
        }
        wxPoint downCenterPoint = wxPoint((buttonRect.GetBottomRight().x + buttonRect.GetBottomLeft().x) / 2, buttonRect.GetBottom());
        dc.SetPen(buttonColour);
        dc.SetBrush(buttonColour);
        wxPoint p3[3];
        p3[0] = buttonRect.GetTopLeft();
        p3[1] = buttonRect.GetTopRight();
        p3[2] = downCenterPoint;
        dc.DrawPolygon(3, p3);
        /*dc.DrawLine(buttonRect.GetTopLeft(), buttonRect.GetTopRight());
        dc.DrawLine(buttonRect.GetTopLeft(), downCenterPoint);
        dc.DrawLine(buttonRect.GetTopRight(), downCenterPoint);*/
    }
    //---------------------------------------------------------------------------
    #ifdef __WXMSW__
    #define X_MARGIN 6
    #else
    #define X_MARGIN 4
    #endif
    //---------------------------------------------------------------------------
    void StyleManager::DrawCustomChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
        const wxColour& baseColour, const wxBitmap& bmp, int align)
    {
        wxRect choiceRect = rect;
        // Fill the drop down button with the custom base colour
        dc.SetPen(baseColour);
        dc.SetBrush(baseColour);
        dc.DrawRectangle(rect);

        // Create colour pallete
        Colour c;
        c.setBase(baseColour);
        wxColour borderColour = c.GetBorderColour();
        wxColour arrowColour = c.GetDarkBorderColour();
        wxColour textColour = c.GetItemTextColour();

        int width = choiceRect.GetHeight();
        wxRect dropDownRect = wxRect(0, 0, width, width);
        int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
        dropDownRect.SetX(x);
        dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
        dc.SetBrush(baseColour);
        dc.SetPen(borderColour);
        dc.DrawRoundedRectangle(choiceRect, 3.0);
        DrawDropDownArrow(win, dc, dropDownRect, false, 1, arrowColour);

        // Common to all platforms: draw the text + bitmap
        wxRect textRect = choiceRect;
        textRect.SetWidth(textRect.GetWidth() - textRect.GetHeight());
        dc.SetClippingRegion(textRect);

        int xx = textRect.GetX() + X_MARGIN;
        if (bmp.IsOk()) {
            // Draw bitmap first
            wxRect bmpRect(xx, textRect.GetY(), bmp.GetScaledWidth(), bmp.GetScaledHeight());
            bmpRect = bmpRect.CenterIn(choiceRect, wxVERTICAL);
            dc.DrawBitmap(bmp, bmpRect.GetTopLeft());
            xx += bmpRect.GetWidth() + X_MARGIN;
        }
        dc.SetFont(GetDefaultGuiFont());
        wxSize textSize = dc.GetTextExtent(label);
        textRect.SetHeight(textSize.GetHeight());
        textRect = textRect.CenterIn(choiceRect, wxVERTICAL);
        wxString truncatedText;
        TruncateText(label, textRect.GetWidth(), dc, truncatedText);
        dc.SetTextForeground(textColour);
        dc.DrawText(truncatedText, xx, textRect.GetY());
        dc.DestroyClippingRegion();
    }
    //---------------------------------------------------------------------------
    void StyleManager::DrawNativeChoice(wxWindow* win, wxDC& dc, const wxRect& rect, const wxString& label,
        const wxBitmap& bmp, int align)
    {
        wxRect choiceRect = rect;
    #if defined(__WXMSW__) || defined(__WXGTK__)
    #ifdef __WXMSW__
        int width = GetMetric(wxSYS_SMALLICON_X);
    #else
        int width = choiceRect.GetHeight();
    #endif
        wxRect dropDownRect = wxRect(0, 0, width, width);
        int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
        dropDownRect.SetX(x);
        dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
        wxRendererNative::Get().DrawPushButton(win, dc, choiceRect, 0);
        wxRendererNative::Get().DrawDropArrow(win, dc, dropDownRect, 0);
    #else
        // OSX
        //wxColour bgColour = GetColour(wxSYS_COLOUR_3DFACE);
        wxColour bgColour = gsgs_Style().GetAppBgColour();
        if (IsDark(bgColour)) 
        {
            // On Dark theme (Mojave and later)
            int width = choiceRect.GetHeight();
            wxRect dropDownRect = wxRect(0, 0, width, width);
            int x = choiceRect.GetX() + choiceRect.GetWidth() - dropDownRect.GetWidth();
            dropDownRect.SetX(x);
            dropDownRect = dropDownRect.CenterIn(choiceRect, wxVERTICAL);
            wxColour borderColour = GetColour(wxSYS_COLOUR_BTNHIGHLIGHT);
            dc.SetBrush(GetColour(wxSYS_COLOUR_BTNFACE));
            dc.SetPen(borderColour);
            dc.DrawRoundedRectangle(choiceRect, 3.0);
            DrawDropDownArrow(win, dc, dropDownRect);
        }
        else {
            wxRendererNative::Get().DrawChoice(win, dc, choiceRect, 0);
        }
    #endif

        // Common to all platforms: draw the text + bitmap
        wxRect textRect = choiceRect;
        textRect.SetWidth(textRect.GetWidth() - textRect.GetHeight());
        dc.SetClippingRegion(textRect);

        int xx = textRect.GetX() + X_MARGIN;
        if (bmp.IsOk()) {
            // Draw bitmap first
            wxRect bmpRect(xx, textRect.GetY(), bmp.GetScaledWidth(), bmp.GetScaledHeight());
            bmpRect = bmpRect.CenterIn(choiceRect, wxVERTICAL);
            dc.DrawBitmap(bmp, bmpRect.GetTopLeft());
            xx += bmpRect.GetWidth() + X_MARGIN;
        }
        dc.SetFont(GetDefaultGuiFont());
        wxSize textSize = dc.GetTextExtent(label);
        textRect.SetHeight(textSize.GetHeight());
        textRect = textRect.CenterIn(choiceRect, wxVERTICAL);
        wxString truncatedText;
        TruncateText(label, textRect.GetWidth(), dc, truncatedText);
        dc.SetTextForeground(GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc.DrawText(truncatedText, xx, textRect.GetY());
        dc.DestroyClippingRegion();
    }
    //---------------------------------------------------------------------------
    wxFont StyleManager::GetBestFixedFont(IEditor* editor)
    {
        wxFont defaultFont = GetDefaultFixedFont();
        wxFont bestFont = defaultFont;
        wxUnusedVar(editor);
        //    if(editor) {
        //        bestFont = editor->GetCtrl()->StyleGetFont(0);
        //#if defined(__WXGTK__) && defined(__WXGTK3__)
        //        bestFont.SetPointSize(defaultFont.GetPointSize());
        //#endif
        //    }
        return bestFont;
    }
    //-----------------------------------------------------------------------------
    void StyleManager::Load()
    {
        if (mInit) 
            return;
        mInit = true;

        std::vector<wxXmlDocument*> llist;
        wxArrayString files;

        wxFileName userPath(gsgs_Path().GetUserLexersDir(), "");
        wxDir::GetAllFiles(userPath.GetPath(), &files, "lexer_*.xml");
        for (size_t i = 0; i < files.GetCount(); ++i) {
            wxString content;
            wxFFile xmlFile(files.Item(i), "rb");
            if (!xmlFile.IsOpened()) continue;
            if (xmlFile.ReadAll(&content, wxConvUTF8)) {
                wxXmlDocument* doc = new wxXmlDocument();
                wxStringInputStream sis(content);
                if (doc->Load(sis)) {
                    llist.push_back(doc);
                }
                else {
                    wxDELETE(doc);
                }
            }
            xmlFile.Close();
            gsgsRemoveFile(files.Item(i));
        }

        m_allLexers.clear();
        m_lexersMap.clear();

        gsgs_Config().GetObject("Theme").getValue(m_globalTheme, "Default");

        wxFileName ulexer(gsgs_Path().GetUserLexersDir(), "lexers.json");
        if (ulexer.FileExists()) {
            LoadJSON(ulexer);
        }
        else
        {
            wxFileName dlexer(gsgs_Path().GetLexersDir(), "lexers.json");
            LoadJSON(dlexer);
            for (size_t i = 0; i < llist.size(); ++i) {
                wxXmlDocument* doc = llist.at(i);
                DoAddLexer(doc->GetRoot());
            }
            Save();
        }
        gsgs_Config().Write(LEXERS_VERSION_STRING, LEXERS_VERSION);
        //Event event(wxEVT_CMD_COLOURS_FONTS_UPDATED);
        //gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------
    LexerStyle::Ptr_t StyleManager::DoAddLexer(wxXmlNode* node)
    {
        wxString lexerName = XmlUtils::ReadString(node, "Name");
        lexerName.MakeLower();
        if (lexerName.IsEmpty()) return NULL;

        LexerStyle::Ptr_t lexer(new LexerStyle);
        lexer->FromXml(node);

        // ensure that the theme name is capitalized - this helps
        // when displaying the content in a wxListBox sorted
        wxString themeName = lexer->GetThemeName();
        themeName = themeName.Mid(0, 1).Capitalize() + themeName.Mid(1);
        lexer->SetThemeName(themeName);

        if (lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("final")) {
            lexer->SetKeyWords(lexer->GetKeyWords(0) + " final", 0);
        }

        // Add C++ keyword "override"
        if (lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("override")) {
            lexer->SetKeyWords(lexer->GetKeyWords(0) + " override", 0);
        }

        // Hack: fix Java lexer which is using the same
        // file extensions as C++...
        if (lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) { lexer->SetFileSpec("*.java"); }

        // Append *.sqlite to the SQL lexer if missing
        if (lexer->GetName() == "sql" && !lexer->GetFileSpec().Contains(".sqlite")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.sqlite");
        }

        // Hack2: since we now provide our own PHP and javaScript lexer, remove the PHP/JS extensions from
        // the HTML lexer
        if (lexer->GetName() == "html" && (lexer->GetFileSpec().Contains(".php") || lexer->GetFileSpec().Contains("*.js"))) {
            lexer->SetFileSpec("*.htm;*.html;*.xhtml");
        }

        // Hack3: all the HTML support to PHP which have much more colour themes
        if (lexer->GetName() == "html" && lexer->GetFileSpec().Contains(".html")) {
            lexer->SetFileSpec("*.vbs;*.vbe;*.wsf;*.wsc;*.asp;*.aspx");
        }

        // Hack4: all the HTML support to PHP which have much more colour themes
        if (lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".qml")) {
            lexer->SetFileSpec("*.js;*.javascript;*.qml;*.json");
        }

        if (lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".html")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.html;*.htm;*.xhtml");
        }

        if (lexer->GetName() == "php" && !lexer->GetKeyWords(4).Contains("<?php")) {
            lexer->SetKeyWords(lexer->GetKeyWords(4) + " <?php <? ", 4);
        }

        // Add wxcp file extension to the JavaScript lexer
        if (lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".wxcp")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.wxcp");
        }

        if (lexer->GetName() == "javascript") {
            wxString jsWords = lexer->GetKeyWords(0);
            wxArrayString arrWords = ::wxStringTokenize(jsWords, " ", wxTOKEN_STRTOK);
            // use std::set to make sure that the elements are sorted
            std::set<wxString> uniqueSet;
            for (size_t i = 0; i < arrWords.size(); ++i) {
                uniqueSet.insert(arrWords.Item(i));
            }
            uniqueSet.insert("class");
            uniqueSet.insert("await");
            uniqueSet.insert("async");
            jsWords.clear();
            std::for_each(uniqueSet.begin(), uniqueSet.end(), [&](const wxString& word) { jsWords << word << " "; });
            lexer->SetKeyWords(jsWords, 0);
        }

        // Add *.scss file extension to the css lexer
        if (lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".scss")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.scss");
        }

        // Add *.less file extension to the css lexer
        if (lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".less")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.less");
        }

        // Set the JavaScript keywords
        if (lexer->GetName() == "php" && !lexer->GetKeyWords(1).Contains("instanceof")) {
            lexer->SetKeyWords(
                "break do instanceof typeof case else new var catch finally return void continue for switch while "
                "debugger function this with default if throw delete in try abstract  export interface  static "
                "boolean  extends "
                "long super"
                "byte final native synchronized char float package throws class goto private transient const "
                "implements  protected "
                " volatile double import  public enum "
                "int short null true false",
                1);
        }

        if (lexer->GetName() == "makefile" && !lexer->GetFileSpec().Contains("*akefile.am")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*akefile.in;*akefile.am");
        }

        // Upgrade the lexer colours
        UpdateLexerColours(lexer, false);

        if (m_lexersMap.count(lexerName) == 0) {
            m_lexersMap.insert(std::make_pair(lexerName, StyleManager::Vec_t()));
        }

        StyleManager::Vec_t& vec = m_lexersMap.find(lexerName)->second;

        // Locate an instance with this name and theme in
        // both the m_alllexers and vector for this lexer
        // name
        StyleManager::Vec_t::iterator iter =
            std::find_if(vec.begin(), vec.end(), LexerStyle::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
        if (iter != vec.end()) { vec.erase(iter); }
        iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
            LexerStyle::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
        if (iter != m_allLexers.end()) { m_allLexers.erase(iter); }
        vec.push_back(lexer);
        m_allLexers.push_back(lexer);
        return lexer;
    }
    //---------------------------------------------------------------------------
    wxArrayString StyleManager::GetAvailableThemesForLexer(const wxString& lexerName) const
    {
        StyleManager::Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
        if (iter == m_lexersMap.end()) return wxArrayString();

        wxArrayString themes;
        const StyleManager::Vec_t& lexers = iter->second;
        for (size_t i = 0; i < lexers.size(); ++i) {
            themes.Add(lexers.at(i)->GetThemeName());
        }

        // sort the list
        themes.Sort();
        return themes;
    }
    //---------------------------------------------------------------------------
    LexerStyle::Ptr_t StyleManager::GetLexer(const wxString& lexerName, const wxString& theme) const
    {
        Map_t::const_iterator iter = m_lexersMap.find(lexerName.Lower());
        if (iter == m_lexersMap.end()) return m_defaultLexer;

        // Locate the requested theme
        LexerStyle::Ptr_t firstLexer(NULL);
        LexerStyle::Ptr_t defaultLexer(NULL);

        if (theme.IsEmpty()) {
            // return the active theme
            const StyleManager::Vec_t& lexers = iter->second;
            for (size_t i = 0; i < lexers.size(); ++i) {

                if (!firstLexer) { firstLexer = lexers.at(i); }

                if (!defaultLexer && lexers.at(i)->GetThemeName() == "Default") { defaultLexer = lexers.at(i); }

                if (lexers.at(i)->IsActive()) return lexers.at(i);
            }

            // No match
            if (defaultLexer)
                return defaultLexer;
            else if (firstLexer)
                return firstLexer;
            else
                return m_defaultLexer;

        }
        else {
            const StyleManager::Vec_t& lexers = iter->second;
            LexerStyle::Ptr_t themeDefaultLexer = nullptr;
            for (size_t i = 0; i < lexers.size(); ++i) {
                if (lexers[i]->GetThemeName() == "Default") { themeDefaultLexer = lexers[i]; }
                if (lexers[i]->GetThemeName() == theme) { return lexers[i]; }
            }
            // We failed to find the requested theme for this language. If we have a "Default"
            // lexer, return it, else use the minimal lexer ("m_defaultLexer")
            return (themeDefaultLexer ? themeDefaultLexer : m_defaultLexer);
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::Save(bool forExport)
    {
        Map_t::const_iterator iter = m_lexersMap.begin();
        JSON root(cJSON_Array);
        JSONObject element = root.getObject();
        for (; iter != m_lexersMap.end(); ++iter) {
            const StyleManager::Vec_t& lexers = iter->second;
            for (size_t i = 0; i < lexers.size(); ++i) {
                element.arrayAppend(lexers.at(i)->ToJSON(forExport));
            }
        }

        wxFileName lexerFiles(gsgs_Path().GetUserDataDir(), "lexers.json");
        lexerFiles.AppendDir("lexers");
        root.save(lexerFiles);

        gsgs_Config().GetRootObject().addProperty("Theme", m_globalTheme);

        wxCommandEvent evtThemeChanged(wxEVT_GSGS_THEME_CHANGED);
        gsgs_Event().AddPendingEvent(evtThemeChanged);

        //Event event(wxEVT_CMD_COLOURS_FONTS_UPDATED);
        //gsgs_Event().AddPendingEvent(event);
    }
    //---------------------------------------------------------------------------
    wxArrayString StyleManager::GetAllLexersNames() const
    {
        wxArrayString names;
        for (size_t i = 0; i < m_allLexers.size(); ++i) {
            LexerStyle::Ptr_t lexer = m_allLexers.at(i);
            if (names.Index(lexer->GetName()) == wxNOT_FOUND) { names.Add(lexer->GetName()); }
        }
        names.Sort();
        return names;
    }
    //---------------------------------------------------------------------------
    LexerStyle::Ptr_t StyleManager::GetLexerForFile(const wxString& filename) const
    {
        if (filename.IsEmpty()) return GetLexer("text");

        wxFileName fnFileName(filename);
        wxString fileNameLowercase = fnFileName.GetFullName();
        fileNameLowercase.MakeLower();

        LexerStyle::Ptr_t defaultLexer(NULL);
        LexerStyle::Ptr_t firstLexer(NULL);

        // Scan the list of lexers, locate the active lexer for it and return it
        Vec_t::const_iterator iter = m_allLexers.begin();
        for (; iter != m_allLexers.end(); ++iter) {
            wxString fileMask = (*iter)->GetFileSpec();
            if (FileUtil::WildMatch(fileMask, filename)) {
                if ((*iter)->IsActive()) {
                    return *iter;

                }
                else if (!firstLexer) {
                    firstLexer = *iter;

                }
                else if (!defaultLexer && (*iter)->GetThemeName() == "Default") {
                    defaultLexer = *iter;
                }
            }
        }

        // If we got here, it means that we could not find an active lexer that matches
        // the file mask. However, if we did find a "firstLexer" it means
        // that we do have a lexer that matches the file extension, its just that it is not
        // set as active
        if (firstLexer) { return firstLexer; }

        // Try this:
        // Use the FileUtil to get the file type by examinig its content
        LexerStyle::Ptr_t lexerByContent; // Null by default
        FileUtil::FileType fileType = FileUtil::TypeOther;
        if (FileUtil::AutoDetectByContent(filename, fileType) && fileType != FileUtil::TypeOther) {
            switch (fileType) {
            case FileUtil::TypeScript:
                lexerByContent = GetLexer("script");
                break;
            case FileUtil::TypeJS:
                lexerByContent = GetLexer("javascript");
                break;
            case FileUtil::TypePhp:
                lexerByContent = GetLexer("php");
                break;
            case FileUtil::TypeSourceCpp:
                lexerByContent = GetLexer("c++");
                break;
            case FileUtil::TypeXml:
                lexerByContent = GetLexer("xml");
                break;
            case FileUtil::TypePython:
                lexerByContent = GetLexer("python");
                break;
            default:
                break;
            }
        }

        // If we managed to find a lexer by content, use it
        if (lexerByContent) return lexerByContent;

        // If we reached here, it means we could not locate an active lexer for this file type
        if (defaultLexer) {
            return defaultLexer;

        }
        else if (firstLexer) {
            return firstLexer;

        }
        else {
            // Return the "Text" lexer
            return GetLexer("text");
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::Reload()
    {
        m_allLexers.clear();
        m_lexersMap.clear();
        mInit = false;
        Load();
    }
    //---------------------------------------------------------------------------
    void StyleManager::SetActiveTheme(const wxString& lexerName, const wxString& themeName)
    {
        wxArrayString themes = GetAvailableThemesForLexer(lexerName);
        for (size_t i = 0; i < themes.GetCount(); ++i) {
            LexerStyle::Ptr_t lexer = GetLexer(lexerName, themes.Item(i));
            if (lexer && lexer->GetName() == lexerName) { lexer->SetIsActive(lexer->GetThemeName() == themeName); }
        }
    }
    //---------------------------------------------------------------------------
    LexerStyle::Ptr_t StyleManager::CopyTheme(const wxString& lexerName, const wxString& themeName,
        const wxString& sourceTheme)
    {
        LexerStyle::Ptr_t sourceLexer = GetLexer(lexerName, sourceTheme);
        CHECK_PTR_RET_NULL(sourceLexer);

        JSONObject json = sourceLexer->ToJSON();
        LexerStyle::Ptr_t newLexer(new LexerStyle());
        newLexer->FromJSON(json);

        // Update the theme name
        newLexer->SetThemeName(themeName);

        // Add it
        return DoAddLexer(newLexer->ToJSON());
    }
    //---------------------------------------------------------------------------
    void StyleManager::RestoreDefaults()
    {
        // First we delete the user settings
        {
            wxLogNull noLog;
            wxFileName fnLexersJSON(gsgs_Path().GetUserLexersDir(), "lexers.json");
            if (fnLexersJSON.Exists()) { gsgsRemoveFile(fnLexersJSON.GetFullPath()); }
        }

        // Now, we simply reload the settings
        Reload();
    }
    //---------------------------------------------------------------------------
    bool StyleManager::ImportEclipseTheme(const wxString& eclipseXml)
    {
        bool res = false;
        if (!eclipseXml.IsEmpty()) {
            EclipseThemeImporterManager importer;
            return importer.Import(eclipseXml);
        }
        return res;
    }
    //---------------------------------------------------------------------------
    void StyleManager::UpdateLexerColours(LexerStyle::Ptr_t lexer, bool force)
    {
        StyleProperty & defaultProp = lexer->GetProperty(0); // Default
        if (force || mVersion < 1) {
            // adjust line numbers
            if (lexer->IsDark()) {
                StyleProperty& lineNumbers = lexer->GetProperty(LINE_NUMBERS_ATTR_ID); // Line numbers
                if (!defaultProp.IsNull()) {
                    if (lexer->GetName() == "c++") {
                        defaultProp.SetFgColour(
                            wxColour(defaultProp.GetBgColour()).ChangeLightness(120).GetAsString(wxC2S_HTML_SYNTAX));
                    }
                    if (!lineNumbers.IsNull()) {
                        lineNumbers.SetFgColour(
                            wxColour(defaultProp.GetBgColour()).ChangeLightness(120).GetAsString(wxC2S_HTML_SYNTAX));
                        lineNumbers.SetBgColour(defaultProp.GetBgColour());
                    }
                }

            }
            else {
                lexer->SetLineNumbersFgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
                StyleProperty& lineNumbers = lexer->GetProperty(LINE_NUMBERS_ATTR_ID); // Line numbers
                if (!lineNumbers.IsNull()) { lineNumbers.SetBgColour(defaultProp.GetBgColour()); }

                // don't adjust PHP and HTML default colours, since they also affects the various operators
                // foreground colours
                if (lexer->GetName() != "php" && lexer->GetName() != "html" && lexer->GetName() != "text" &&
                    lexer->GetName() != "cmake" && lexer->GetName() != "xml") {
                    lexer->SetDefaultFgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
                }
            }
        }

        //=====================================================================
        // Second upgrade stage: adjust whitespace colour and fold margin
        //=====================================================================
        if (force || mVersion < 2) {
            // adjust line numbers
            StyleProperty& fold = lexer->GetProperty(FOLD_MARGIN_ATTR_ID);       // fold margin
            StyleProperty& whitespace = lexer->GetProperty(WHITE_SPACE_ATTR_ID); // whitespace
            if (lexer->IsDark()) {
                wxColour newCol = wxColour(defaultProp.GetBgColour()).ChangeLightness(110);

                fold.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
                fold.SetBgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
                whitespace.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));

            }
            else {
                wxColour newCol = wxColour(defaultProp.GetBgColour()).ChangeLightness(95);

                fold.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
                fold.SetBgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
                whitespace.SetFgColour(newCol.GetAsString(wxC2S_HTML_SYNTAX));
            }
        }

        //=====================================================================
        // Third upgrade stage: adjust whitespace colour and fold margin
        //=====================================================================
        if (force || mVersion < 3) {
            // remove the *.js;*.javascript from the C++ lexer
            if (lexer->GetName() == "c++") {
                lexer->SetFileSpec("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.ipp;*.hxx;*.h++");
            }
        }

        if ((lexer->GetName() == "c++") && (lexer->GetFileSpec().IsEmpty() || !lexer->GetFileSpec().Contains("*.cpp"))) {
            lexer->SetFileSpec("*.cxx;*.hpp;*.cc;*.h;*.c;*.cpp;*.l;*.y;*.c++;*.hh;*.ipp;*.hxx;*.h++;*.ino");
        }

        // Add Arduino sketches files as C++ (*.ino)
        if (lexer->GetName() == "c++" && !lexer->GetFileSpec().Contains(".ino")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.ino");
        }

        // Upgrade CSS colours
        if ((force || mVersion < 4) && lexer->GetName().Lower() == "css") {
            // adjust line numbers
            bool isDark = lexer->IsDark();
            StyleProperty& var = lexer->GetProperty(wxSTC_CSS_VARIABLE);
            StyleProperty& identifier = lexer->GetProperty(wxSTC_CSS_IDENTIFIER);
            StyleProperty& identifier2 = lexer->GetProperty(wxSTC_CSS_IDENTIFIER2);
            StyleProperty& identifier3 = lexer->GetProperty(wxSTC_CSS_IDENTIFIER3);
            StyleProperty& oper = lexer->GetProperty(wxSTC_CSS_OPERATOR);
            if (!var.IsNull()) {
                if (!identifier.IsNull()) { identifier.SetFgColour(var.GetFgColour()); }
                if (!identifier2.IsNull()) { identifier2.SetFgColour(var.GetFgColour()); }
                if (!identifier3.IsNull()) { identifier3.SetFgColour(var.GetFgColour()); }
                if (!oper.IsNull()) { oper.SetFgColour(isDark ? "WHITE" : "BLACK"); }
            }
        }

        if (force || mVersion < 5) {
            // Indentation guides (style #37)
            StyleProperty& indentGuides = lexer->GetProperty(37);
            indentGuides.SetFgColour(defaultProp.GetBgColour());
            indentGuides.SetBgColour(defaultProp.GetBgColour());
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::SetTheme(const wxString& themeName)
    {
        LexerStyle::Ptr_t lexer = GetLexer("c++", themeName);
        CHECK_PTR_RET(lexer);

        bool isDark = lexer->IsDark();
        wxString fallbackTheme;
        if (isDark) {
            fallbackTheme = "One Dark Like";
        }
        else {
            fallbackTheme = "Atom One Light";
        }

        const wxArrayString& lexers = GetAllLexersNames();
        for (size_t i = 0; i < lexers.size(); ++i) {
            wxArrayString themesForLexer = GetAvailableThemesForLexer(lexers.Item(i));
            if (themesForLexer.Index(themeName) == wxNOT_FOUND) {
                SetActiveTheme(lexers.Item(i), fallbackTheme);
            }
            else {
                SetActiveTheme(lexers.Item(i), themeName);
            }
        }
        SetGlobalTheme(themeName);

        //Colour colours;
        bool useCustomColour;
        gsgs_Config().Read("UseCustomBaseColour", useCustomColour, false);
        if (mCustomColour) {
            wxColour bgColour = GetBackgroundColourFromLexer(lexer);
            gsgs_Config().Write("BaseColour", bgColour);
        }
    }
    //---------------------------------------------------------------------------
    void StyleManager::LoadJSON(const wxFileName& path)
    {
        if (path.FileExists())
        {
            JSON root(path);
            JSONObject arr = root.getObject();
            int arrSize = arr.getArraySize();
            gsgs_Debug("Loading JSON file: %s (contains %d lexers)", path.GetFullPath(), arrSize);
            for (int i = 0; i < arrSize; ++i) {
                JSONObject json = arr.getArray(i);
                DoAddLexer(json);
            }
            gsgs_Debug("Loading JSON file...done");
        }
    }
    //---------------------------------------------------------------------------
    LexerStyle::Ptr_t StyleManager::DoAddLexer(JSONObject json)
    {
        LexerStyle::Ptr_t lexer(new LexerStyle());
        lexer->FromJSON(json);

        wxString lexerName = lexer->GetName().Lower();
        if (lexerName.IsEmpty()) return NULL;

        // ensure that the theme name is capitalized - this helps
        // when displaying the content in a wxListBox sorted
        wxString themeName = lexer->GetThemeName();
        themeName = themeName.Mid(0, 1).Capitalize() + themeName.Mid(1);
        lexer->SetThemeName(themeName);

        gsgsDEBUG1() << "Loading lexer:" << lexerName;

        if (lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("final")) {
            lexer->SetKeyWords(lexer->GetKeyWords(0) + " final", 0);
        }

        if (lexer->GetName() == "c++" && !lexer->GetKeyWords(0).Contains("override")) {
            lexer->SetKeyWords(lexer->GetKeyWords(0) + " override", 0);
        }

        // Hack: fix Java lexer which is using the same
        // file extensions as C++...
        if (lexer->GetName() == "java" && lexer->GetFileSpec().Contains(".cpp")) { lexer->SetFileSpec("*.java"); }

        // Append *.sqlite to the SQL lexer if missing
        if (lexer->GetName() == "sql" && !lexer->GetFileSpec().Contains(".sqlite")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.sqlite");
        }

        // Hack2: since we now provide our own PHP and javaScript lexer, remove the PHP/JS extensions from
        // the HTML lexer
        if (lexer->GetName() == "html" && (lexer->GetFileSpec().Contains(".php") || lexer->GetFileSpec().Contains("*.js"))) {
            lexer->SetFileSpec("*.htm;*.html;*.xhtml");
        }

        // Hack3: all the HTML support to PHP which have much more colour themes
        if (lexer->GetName() == "html" && lexer->GetFileSpec().Contains(".html")) {
            lexer->SetFileSpec("*.vbs;*.vbe;*.wsf;*.wsc;*.asp;*.aspx");
        }

        // Hack4: all the HTML support to PHP which have much more colour themes
        if (lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".qml")) {
            lexer->SetFileSpec("*.js;*.javascript;*.qml;*.json");
        }

        // Hack5: all the remove *.scss from the css lexer (it now has its own lexer)
        if (lexer->GetName() == "css" && lexer->GetFileSpec().Contains(".scss")) { lexer->SetFileSpec("*.css"); }

        // Add *.less file extension to the css lexer
        if (lexer->GetName() == "css" && !lexer->GetFileSpec().Contains(".less")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.less");
        }

        if (lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".html")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.html;*.htm;*.xhtml");
        }

        if (lexer->GetName() == "php" && !lexer->GetKeyWords(4).Contains("<?php")) {
            lexer->SetKeyWords(lexer->GetKeyWords(4) + " <?php <? ", 4);
        }

        if (lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".php5")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.php5");
        }

        if (lexer->GetName() == "php" && !lexer->GetFileSpec().Contains(".ctp")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.ctp");
        }

        // Add wxcp file extension to the JavaScript lexer
        if (lexer->GetName() == "javascript" && !lexer->GetFileSpec().Contains(".wxcp")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*.wxcp");
        }

        if (lexer->GetName() == "javascript") {
            wxString jsWords = lexer->GetKeyWords(0);
            wxArrayString arrWords = ::wxStringTokenize(jsWords, " ", wxTOKEN_STRTOK);
            // use std::set to make sure that the elements are sorted
            std::set<wxString> uniqueSet;
            for (size_t i = 0; i < arrWords.size(); ++i) {
                uniqueSet.insert(arrWords.Item(i));
            }
            uniqueSet.insert("class");
            uniqueSet.insert("await");
            uniqueSet.insert("async");
            uniqueSet.insert("extends");
            uniqueSet.insert("constructor");
            uniqueSet.insert("super");
            jsWords.clear();
            std::for_each(uniqueSet.begin(), uniqueSet.end(), [&](const wxString& word) { jsWords << word << " "; });
            lexer->SetKeyWords(jsWords, 0);
        }

        if (lexer->GetName() == "text") { lexer->SetFileSpec(wxEmptyString); }

        // Set the JavaScript keywords
        if (lexer->GetName() == "php" && !lexer->GetKeyWords(1).Contains("instanceof")) {
            lexer->SetKeyWords(
                "break do instanceof typeof case else new var catch finally return void continue for switch while "
                "debugger function this with default if throw delete in try abstract  export interface  static "
                "boolean  extends "
                "long super"
                "byte final native synchronized char float package throws class goto private transient const "
                "implements  protected "
                " volatile double import  public enum "
                "int short null true false",
                1);
        }

        if (lexer->GetName() == "makefile" && !lexer->GetFileSpec().Contains("*akefile.am")) {
            lexer->SetFileSpec(lexer->GetFileSpec() + ";*akefile.in;*akefile.am");
        }

        // Upgrade the lexer colours
        UpdateLexerColours(lexer, false);

        if (m_lexersMap.count(lexerName) == 0) {
            m_lexersMap.insert(std::make_pair(lexerName, Vec_t()));
        }

        Vec_t& vec = m_lexersMap.find(lexerName)->second;

        // Locate an instance with this name and theme in
        // both the m_alllexers and vector for this lexer
        // name
        Vec_t::iterator iter =
            std::find_if(vec.begin(), vec.end(), LexerStyle::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
        if (iter != vec.end()) { vec.erase(iter); }

        iter = std::find_if(m_allLexers.begin(), m_allLexers.end(),
            LexerStyle::FindByNameAndTheme(lexer->GetName(), lexer->GetThemeName()));
        if (iter != m_allLexers.end()) { m_allLexers.erase(iter); }
        vec.push_back(lexer);
        m_allLexers.push_back(lexer);
        return lexer;
    }
    //---------------------------------------------------------------------------
    void StyleManager::AddLexer(LexerStyle::Ptr_t lexer)
    {
        CHECK_PTR_RET(lexer);
        DoAddLexer(lexer->ToJSON());
    }
    //---------------------------------------------------------------------------
    wxStaticText * StyleManager::createStaticText(wxWindow *parent, wxWindowID id,
        const wxString& label, const wxPoint& pos, const wxSize& size, long style,
            const wxString& name)
    {
        wxStaticText * re = new wxStaticText(parent, id, label, pos, size, style, name);
        re->SetForegroundColour(GetAppTextColour());
        return re;
    }
    //---------------------------------------------------------------------------
    wxTextCtrl * StyleManager::createTextCtrl(wxWindow *parent, wxWindowID id,
        const wxString& value, const wxPoint& pos, const wxSize& size, long style, 
            const wxValidator& validator, const wxString& name)
    {
        wxTextCtrl * re = new wxTextCtrl(parent, id, value, pos, size, wxTE_RICH | wxNO_BORDER | style, validator, name);
        wxColour tempcc = gsgs_Style().GetAppBgColour().ChangeLightness(93);
        re->SetBackgroundColour(tempcc);
        re->SetForegroundColour(GetActiveMarkerColour());
        re->SetDefaultStyle(mTextAttr);
#if wxVERSION_NUMBER >= 3000
        if((wxTE_MULTILINE & style) == 0)
            re->SetHint(wxT(""));
#endif
        return re;
    }
    //---------------------------------------------------------------------------
    void StyleManager::SetGlobalFont(const wxFont& font)
    {
        mFont = font;

        // Loop for every lexer and update the font per style
        std::for_each(m_allLexers.begin(), m_allLexers.end(), [&](LexerStyle::Ptr_t lexer) {
            StyleProperty::Map_t& props = lexer->GetLexerProperties();
            StyleProperty::Map_t::iterator iter = props.begin();
            for (; iter != props.end(); ++iter) {
                StyleProperty& sp = iter->second;
                sp.SetFaceName(font.GetFaceName());
                sp.SetFontSize(font.GetPointSize());
                sp.SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
                sp.SetItalic(font.GetStyle() == wxFONTSTYLE_ITALIC);
                sp.SetUnderlined(font.GetUnderlined());
            }
        });
    }
    //---------------------------------------------------------------------------
    bool StyleManager::ExportThemesToFile(const wxFileName& outputFile, const wxArrayString& names) const
    {
        gsgsStringSet M;
        for (size_t i = 0; i < names.size(); ++i) {
            M.insert(names.Item(i).Lower());
        }

        JSON root(cJSON_Array);
        JSONObject arr = root.getObject();
        std::vector<LexerStyle::Ptr_t> Lexers;
        std::for_each(m_allLexers.begin(), m_allLexers.end(), [&](LexerStyle::Ptr_t lexer) {
            if (M.empty() || M.count(lexer->GetThemeName().Lower())) { Lexers.push_back(lexer); }
        });
        std::for_each(Lexers.begin(), Lexers.end(), 
            [&](LexerStyle::Ptr_t lexer) 
            { 
                arr.addProperty(lexer->ToJSON(true)); 
            }
        );
        wxString targ;
        root.getObject().getString(targ);
        return FileUtil::WriteFileContent(outputFile, targ);
    }
    //---------------------------------------------------------------------------
    bool StyleManager::ImportLexersFile(const wxFileName& inputFile, bool prompt)
    {
        JSON root(inputFile);
        if (!root.isValid()) {
            gsgsWARNING() << "Invalid lexers input file:" << inputFile << clEndl;
            return false;
        }

        if (prompt) {
            if (::wxMessageBox(
                _("Importing syntax highlight file will override any duplicate syntax highlight settings.\nContinue?"),
                "gsgs", wxICON_QUESTION | wxYES_NO | wxCANCEL | wxYES_DEFAULT, NULL) != wxYES) {
                return false;
            }
        }

        std::vector<LexerStyle::Ptr_t> Lexers;
        JSONObject arr = root.getObject();
        int arrSize = arr.getArraySize();
        for (int i = 0; i < arrSize; ++i) {
            JSONObject lexerObj = arr.getArray(i);
            LexerStyle::Ptr_t lexer(new LexerStyle());
            lexer->FromJSON(lexerObj);
            Lexers.push_back(lexer);
        }

        std::for_each(Lexers.begin(), Lexers.end(), [&](LexerStyle::Ptr_t lexer) {
            if (m_lexersMap.count(lexer->GetName()) == 0) { m_lexersMap[lexer->GetName()] = Vec_t(); }
            Vec_t& v = m_lexersMap[lexer->GetName()];
            Vec_t::iterator iter = std::find_if(
                v.begin(), v.end(), [&](LexerStyle::Ptr_t l) { return l->GetThemeName() == lexer->GetThemeName(); });
            if (prompt) {
                if (iter != v.end()) {
                    v.erase(iter);
                }
                v.push_back(lexer);
            }
            else {
                if (iter == v.end()) { v.push_back(lexer); }
            }
        });

        // Rebuild "m_allLexers" after the merge
        m_allLexers.clear();
        std::for_each(m_lexersMap.begin(), m_lexersMap.end(), [&](Map_t::value_type& vt) {
            std::for_each(vt.second.begin(), vt.second.end(),
                [&](LexerStyle::Ptr_t lexer) { m_allLexers.push_back(lexer); });
        });
        Save();
        Reload();
        return true;
    }
    //---------------------------------------------------------------------------
    wxArrayString StyleManager::GetAllThemes() const
    {
        gsgsStringSet themes;
        std::for_each(m_allLexers.begin(), m_allLexers.end(),
            [&](LexerStyle::Ptr_t lexer) { themes.insert(lexer->GetThemeName()); });
        wxArrayString arr;
        std::for_each(themes.begin(), themes.end(), [&](const wxString& name) { arr.push_back(name); });
        return arr;
    }
    //---------------------------------------------------------------------------
    void StyleManager::OnAdjustTheme(Event& event)
    {
        event.Skip();
        if (event.GetInt() == XRCID("adjust-current-theme"))
        {
            event.Skip(false);

            LexerStyle::Ptr_t lexer = GetLexer("text");
            if (lexer)
            {
                wxColour bgColour = GetBackgroundColourFromLexer(lexer);
                if (bgColour.IsOk())
                {
                    // Adjust the colours
                    // Save the base colour changes
                    mAppBackground = bgColour;
                    mCustomColour = true;
                    mCustomColours.setBase(mAppBackground);
                    gsgs_Config().Write("BaseColour", bgColour);
                    gsgs_Config().Write("UseCustomBaseColour", true);

                    // Notify this change
                    Event evt(wxEVT_CMD_COLOURS_FONTS_UPDATED);
                    gsgs_Event().AddPendingEvent(evt);

                    GetManager()->DisplayMessage(_("A gsgs restart is needed. Would you like to restart it now?"),
                        wxICON_QUESTION, { { XRCID("restart-gsgs"), _("Yes") }, { wxID_NO, _("No") } });
                }
            }
        }
    }
    //---------------------------------------------------------------------------
    wxColour StyleManager::GetBackgroundColourFromLexer(LexerStyle::Ptr_t lexer) const
    {
        if (lexer)
        {
            wxColour bgColour;
            if (lexer->IsDark()) {
                bgColour = lexer->GetProperty(0).GetBgColour();
                bgColour = bgColour.ChangeLightness(105);
            }
            else {
                bgColour = lexer->GetProperty(0).GetBgColour();
                bgColour = bgColour.ChangeLightness(95);
            }
            return bgColour;
        }
        return wxNullColour;
    }
    //---------------------------------------------------------------------------
    StyleManager * StyleManager::gInstance = 0;
    //---------------------------------------------------------------------------
    StyleManager & StyleManager::singleton()
    {
        if (gInstance == 0) { gInstance = new StyleManager(); }
        return *gInstance;
    }
    //---------------------------------------------------------------------------
    void StyleManager::Release()
    {
        if (gInstance) { delete gInstance; }
        gInstance = 0;
    }
    //---------------------------------------------------------------------------
}