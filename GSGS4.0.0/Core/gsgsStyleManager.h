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
#ifndef _GSGSSTYLEMANAGER_H
#define _GSGSSTYLEMANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include "gsgsLexerStyle.h"
#include <wx/settings.h>
#include <wx/dcgraph.h>
#include <wx/stattext.h>

// 仲夏夜
#define DefaultATMColour        wxColour("#00a5ff")
#define DefaultHTMColour        wxColour("#00cfff")
#define DefaultBackgroundColour wxColour("#525252")
#define DefaultTextColour       wxColour("#a5a5a5")
#define DefaultActiveTextColour wxColour("#656565")

namespace gsgs
{
    class _gsgsAPI Colour
    {
    public:
        Colour();
        ~Colour() {}
        void setDefault();
        void setDark();
        void setBase(const wxColour& baseColour = wxColour());

        static bool IsDark(const wxColour& col);

        inline void SetAlternateColour(const wxColour& colour)  { alternateColour = colour; }
        inline void SetDarkBorderColour(const wxColour& colour) { darkBorderColour = colour; }
        inline void SetBgColour(const wxColour& colour)         { bgColour = colour; }
        inline void SetButtonColour(const wxColour& colour)     { buttonColour = colour; }
        inline void SetHoverBgColour(const wxColour& colour)    { hoverBgColour = colour; }
        inline void SetItemBgColour(const wxColour& colour)     { itemBgColour = colour; }
        inline void SetItemTextColour(const wxColour& colour)   { itemTextColour = colour; }
        inline void SetSelItemBgColour(const wxColour& colour)  { selItemBgColour = colour; }
        inline void SetSelItemTextColour(const wxColour& colour) { selItemTextColour = colour; }
        inline void SetHeaderBgColour(const wxColour& colour)   { headerBgColour = colour; }
        inline void SetHeaderHBorderColour(const wxColour& colour) { headerHBorderColour = colour; }
        inline void SetHeaderVBorderColour(const wxColour& colour) { headerVBorderColour = colour; }
        inline void SetSelbuttonColour(const wxColour& colour)  { selbuttonColour = colour; }
        inline void SetSelItemBgColourNoFocus(const wxColour& colour) { selItemBgColourNoFocus = colour; }
        inline void SetMatchedItemBgText(const wxColour& colour) { matchedItemBgText = colour; }
        inline void SetMatchedItemText(const wxColour& colour)  { matchedItemText = colour; }
        inline void SetBorderColour(const wxColour& colour)     { borderColour = colour; }
        inline void SetFillColour(const wxColour& colour)       { fillColour = colour; }
        inline void SetGrayText(const wxColour & colour)        { grayText = colour; }

        inline const wxColour& GetAlternateColour() const       { return alternateColour; }
        inline const wxColour & GetDarkBorderColour() const     { return darkBorderColour; }
        inline const wxColour& GetBgColour() const              { return bgColour; }
        inline const wxColour& GetButtonColour() const          { return buttonColour; }
        inline const wxColour& GetHoverBgColour() const         { return hoverBgColour; }
        inline const wxColour& GetItemBgColour() const          { return itemBgColour; }
        inline const wxColour& GetItemTextColour() const        { return itemTextColour; }
        inline const wxColour& GetSelItemBgColour() const       { return selItemBgColour; }
        inline const wxColour& GetSelItemTextColour() const     { return selItemTextColour; }
        inline const wxColour& GetHeaderBgColour() const        { return headerBgColour; }
        inline const wxColour& GetHeaderHBorderColour() const   { return headerHBorderColour; }
        inline const wxColour& GetHeaderVBorderColour() const   { return headerVBorderColour; }
        inline const wxColour& GetSelbuttonColour() const       { return selbuttonColour; }
        inline const wxColour& GetSelItemBgColourNoFocus() const { return selItemBgColourNoFocus; }
        inline const wxColour& GetMatchedItemBgText() const     { return matchedItemBgText; }
        inline const wxColour& GetMatchedItemText() const       { return matchedItemText; }
        inline const wxColour& GetBorderColour() const          { return borderColour; }
        inline const wxColour& GetFillColour() const            { return fillColour; }
        inline const wxColour& GetGrayText() const              { return grayText; }
    private:
        wxColour hoverBgColour;
        wxColour itemTextColour;
        wxColour itemBgColour;
        wxColour selItemTextColour;
        wxColour selItemBgColour;
        wxColour selItemBgColourNoFocus;
        wxColour buttonColour;
        wxColour selbuttonColour;
        wxColour bgColour;
        wxColour alternateColour;
        wxColour headerBgColour;
        wxColour headerHBorderColour;
        wxColour headerVBorderColour;
        wxColour matchedItemText;
        wxColour matchedItemBgText;
        wxColour borderColour;
        wxColour darkBorderColour;
        wxColour fillColour;
        wxColour grayText;
    };

    enum eButtonKind
    {
        kNormal,
        kDropDown,
    };

    // When the version is 0, it means that we need to upgrade the colours for the line numbers
    // and for the default state
#define LEXERS_UPGRADE_LINENUM_DEFAULT_COLOURS 0

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_UPGRADE_LEXERS_START, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_UPGRADE_LEXERS_END, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_UPGRADE_LEXERS_PROGRESS, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SYS_COLOURS_CHANGED, Event);

    class _gsgsAPI StyleManager : public wxEvtHandler, public wxSystemSettings
    {
    public:
        static StyleManager& singleton();
        static void Release();

        ~StyleManager();

        void RegisterWindow(wxWindow * win);

        void UnRegisterWindow(wxWindow * win);

        inline bool IsLightTheme() const                        { return !Colour::IsDark(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)); }

        void LightColour(const wxColour& in, wxColour & out, float percent);
        void DarkColour(const wxColour& in, wxColour & out, float percent);
        inline const wxColour & GetPanelBgColour() const        { return GetColour(wxSYS_COLOUR_3DFACE); }
        inline const wxColour & GetPanelTextColour() const      { return GetColour(wxSYS_COLOUR_BTNTEXT); }
        inline const wxColour & GetButtonBgColour() const       { return GetColour(wxSYS_COLOUR_BTNFACE); }
        inline const wxColour & GetButtonTextColour() const     { return GetColour(wxSYS_COLOUR_BTNTEXT); }
        inline const wxColour & GetTextCtrlTextColour() const   { return GetColour(wxSYS_COLOUR_WINDOWTEXT); }
        inline const wxColour & GetTextCtrlBgColour() const     { return GetColour(wxSYS_COLOUR_WINDOW); }
        inline const wxColour & GetOutputPaneFgColour() const   { return GetColour(wxSYS_COLOUR_WINDOWTEXT); }
        inline const wxColour & GetOutputPaneBgColour() const   { return GetColour(wxSYS_COLOUR_WINDOW); }
        inline const wxColour & GetMenuTextColour() const       { return GetColour(wxSYS_COLOUR_MENUTEXT); }
        inline const wxColour & GetMenuBarBgColour(bool miniToolbar = true) { return GetColour(wxSYS_COLOUR_3DFACE); }
        inline const wxColour & GetMenuBarTextColour()          { return GetColour(wxSYS_COLOUR_MENUTEXT); }
        inline const wxColour & GetAppBgColour() const          { return mAppBackground; }
        inline const wxColour & GetAppTextColour() const        { return mAppTextColour; }
        inline const wxColour & GetAppActiveTextColour() const  { return mAppActiveColour; }
        inline void SetCustomColour(bool set, const wxColour & bgcolour);
        inline bool IsCustomColour() const                      { return mCustomColour; }
        void FillMenuBarBgColour(wxDC & dc, const wxRect & rect, bool miniToolbar = true);
        void TruncateText(const wxString & text, int maxWidth, wxDC & dc, wxString & fixedText);
        void PaintStraightGradientBox(wxDC & dc, const wxRect & rect, const wxColour & startColor, const wxColour & endColor, bool vertical);
        wxFont GetDefaultFixedFont();
        wxFont GetBestFixedFont(IEditor * editor = nullptr);
        const wxFont & GetDefaultGuiFont()                      { return mDefaultFont; }
        const wxFont & GetDefaultActiveGuiFont()                { return mDefaultActiveFont; }
        inline wxBitmap CreateDisabledBitmap(const wxBitmap& bmp) { return bmp.ConvertToDisabled(Colour::IsDark(GetAppBgColour()) ? 69 : 255); }
        wxSize GetBestSize(const wxString& label, int xspacer = 5, int yspacer = 5);

        void DrawButton(wxDC & dc, wxWindow* win, const wxRect& rect, const wxString& label, const wxBitmap& bmp, eButtonKind kind, int state);

        void DrawClose(wxDC & dc, wxWindow* win, const wxRect& rect, const wxColour& pen, const wxColour& bg, int state, int width = 1);

        void DrawAlter(wxDC & dc, wxWindow* win, const wxRect& rect, const wxColour& pen, const wxColour& bg, int state, int width = 1);

        void DrawMinimize(wxDC & dc, wxWindow* win, const wxRect& rect, const wxColour& pen, const wxColour& bg, int state, int width = 1);

        void DrawDropDownArrow(wxWindow* win, wxDC & dc, const wxRect& rect, bool hover, int padsize = 1, const wxColour& colour = wxColour());

        void DrawNativeChoice(wxWindow* win, wxDC & dc, const wxRect & rect, const wxString & label,
            const wxBitmap & bmp = wxNullBitmap, int align = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

        void DrawCustomChoice(wxWindow* win, wxDC & dc, const wxRect & rect, const wxString & label,
            const wxColour & baseColour = wxNullColour, const wxBitmap & bmp = wxNullBitmap,
            int align = wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

        bool DrawStippleBackground(const wxRect& rect, wxDC& dc);

        /**
         * @brief convert wxDC into wxGCDC
         * @param dc [in] dc
         * @param gdc [in/out] graphics DC
         * @return true on success, false otherwise
         */
        static bool GetGCDC(wxDC& dc, wxGCDC& gdc);

        /**
         * @brief Return true if the current theme dominant colour is dark
         */
        inline bool IsThemeDark() const                             { return Colour::IsDark(GetThemeBgColour()); }

        inline const wxColour & GetActiveMarkerColour() const       { return mATMColour; }
        inline const wxColour & GetHoverMarkerColour() const        { return mHTMColour; }

        /// Return the proper colour for painting panel's bg colour
        inline const wxColour & GetThemeBgColour() const            { return GetColour(wxSYS_COLOUR_WINDOW); }

        /// Return the proper colour for painting panel's border colour
        inline const wxColour & GetThemeBorderColour() const        { return GetColour(wxSYS_COLOUR_3DSHADOW); }

        /// Return the proper colour for painting panel's text
        inline const wxColour & GetThemeTextColour() const          { return GetColour(wxSYS_COLOUR_WINDOWTEXT); }

        /// Return the proper colour for painting tooltip bg colour
        inline const wxColour & GetThemeTipBgColour() const         { return Colour::IsDark(GetThemeBgColour()) ? GetThemeBgColour() : GetColour(wxSYS_COLOUR_3DFACE); }

        /// Return the proper colour for painting URL link
        inline wxColour GetThemeLinkColour()const                   { wxColour bgColour = GetThemeTipBgColour(); return Colour::IsDark(bgColour) ? "YELLOW" : "BLUE"; }

        /**
         * @brief return the AUI pane bg colour
         */
        inline const wxColour & GetAUIPaneBGColour() const          { return GetColour(wxSYS_COLOUR_3DFACE); }

        /**
         * @brief get the caption colour
         */
        inline const wxColour & GetCaptionColour() const            { return GetColour(wxSYS_COLOUR_ACTIVECAPTION); }

        /**
         * @brief return the colour suitable for drawing on the caption
         */
        inline const wxColour & GetCaptionTextColour() const        { return GetColour(wxSYS_COLOUR_CAPTIONTEXT); }

        /**
         * @brief stipple brush used for painting on various wxPanels
         */
        wxBrush GetStippleBrush();

        /**
         * @brief get colours object
         */
        inline const Colour & GetColours(bool dark = false) const     { return dark ? mDarkColours : mDefaultColours; }

        const wxColour & GetColour(wxSystemColour index) const;

        /**
         * @brief return a suitable background colour that matches the lexer's bg colour
         */
        wxColour GetBackgroundColourFromLexer(LexerStyle::Ptr_t lexer) const;

        /**
         * @brief Export themes to JSON file
         */
        bool ExportThemesToFile(const wxFileName & outputFile, const wxArrayString& names = wxArrayString()) const;

        /**
         * @brief import lexers from configuration file
         */
        bool ImportLexersFile(const wxFileName & inputFile, bool prompt = true);

        /**
         * @brief adjust the lexer colours to fit gsgs's general look and feel
         */
        void UpdateLexerColours(LexerStyle::Ptr_t lexer, bool force);

        /**
         * @brief create new theme for a lexer by copying an existing theme 'sourceTheme'
         * @param lexerName the lexer name
         * @param themeName the new theme name
         * @param sourceTheme source theme to copy the attributes from
         */
        LexerStyle::Ptr_t CopyTheme(const wxString & lexerName, const wxString& themeName, const wxString& sourceTheme);

        void SetGlobalFont(const wxFont & font);
        inline const wxFont & GetGlobalFont() const             { return mFont; }

        inline void SetGlobalTheme(const wxString & theme)      { m_globalTheme = theme; }
        inline const wxString & GetGlobalTheme() const          { return m_globalTheme; }
        
        /**
         * @brief reload the lexers from the configuration files
         */
        void Reload();

        /**
         * @brief load the lexers + global settings
         */
        void Load();

        /**
         * @brief save the lexers into their proper file name
         */
        void Save(bool forExport = false);

        /**
         * @brief set the active theme for a lexer by name
         */
        void SetActiveTheme(const wxString & lexerName, const wxString & themeName);
        /**
         * @brief return the lexer by name.
         * @param lexerName the lexer name, e.g. "c++"
         * @param theme optionally, return the lexer of a given theme
         */
        LexerStyle::Ptr_t GetLexer(const wxString & lexerName, const wxString & theme = wxEmptyString) const;

        /**
         * @brief return an array of themes availabel for a given lexer
         */
        wxArrayString GetAvailableThemesForLexer(const wxString & lexerName) const;

        /**
         * @brief return an array of available lexers
         */
        wxArrayString GetAllLexersNames() const;

        /**
         * @brief return list of all themes
         */
        wxArrayString GetAllThemes() const;

        /**
         * @brief return lexer for a file
         */
        LexerStyle::Ptr_t GetLexerForFile(const wxString& filename) const;

        /**
         * @brief restore the default colours
         * This is done by deleting the user defined XML files and
         */
        void RestoreDefaults();

        /**
         * @brief import an eclipse theme into gsgs
         */
        bool ImportEclipseTheme(const wxString& eclipseXml);

        /**
         * @brief set a unified theme for all lexers. If the requested theme is not available for a given lexer,
         * use the closest one
         * @param themeName
         */
        void SetTheme(const wxString & themeName);

        /**
         * @brief add new lexer (replace an existing one if exists)
         */
        void AddLexer(LexerStyle::Ptr_t lexer);

        wxStaticText * createStaticText(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxStaticTextNameStr);

        wxTextCtrl * createTextCtrl(wxWindow *parent, wxWindowID id,
            const wxString& value = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxTextCtrlNameStr);
    protected:
        StyleManager();

        void UpdateGlobalColours();

        void OnColoursChanged(Event & event);

        void OnAdjustTheme(Event & event);

        LexerStyle::Ptr_t DoAddLexer(wxXmlNode * node);

        LexerStyle::Ptr_t DoAddLexer(JSONObject json);

        void LoadJSON(const wxFileName& path);
    protected:
        typedef std::vector<LexerStyle::Ptr_t> Vec_t;
        typedef std::unordered_map<wxString, StyleManager::Vec_t> Map_t;

        std::unordered_set<wxWindow *> m_windows;
        StyleManager::Map_t m_lexersMap;
        StyleManager::Vec_t m_allLexers;
        wxTextAttr mTextAttr;
        wxString m_globalTheme;
        LexerStyle::Ptr_t m_defaultLexer;
        wxFont mFont;
        wxFont mDefaultFont;
        wxFont mDefaultActiveFont;
        wxColour mATMColour;
        wxColour mHTMColour;
        wxColour mAppActiveColour;
        wxColour mAppBackground;
        wxColour mAppTextColour;
        Colour mCustomColours;
        Colour mDarkColours;
        Colour mDefaultColours;
        int mVersion;
        bool mCustomColour;
        bool mInit;

        static StyleManager * gInstance;
    };
}
#define gsgs_Style() gsgs::StyleManager::singleton()
#endif // 
