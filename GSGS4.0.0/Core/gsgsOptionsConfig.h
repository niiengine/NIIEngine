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
#ifndef OPTIONS_CONFIG_H
#define OPTIONS_CONFIG_H

#include "gsgsPreInclude.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/xml/xml.h>

namespace gsgs
{
    class _gsgsAPI EditorConfigSection
    {
    public:
        enum {
            kIndentStyleSet = (1 << 0),
            kIndentSizeSet = (1 << 1),
            kTabWidthSet = (1 << 2),
            kCharsetSet = (1 << 3),
            kTrimTrailingWhitespaceSet = (1 << 4),
            kInsertEOLSet = (1 << 5),
            kEOLStyleSet = (1 << 6),
        };

        typedef std::vector<EditorConfigSection> Vec_t;
    public:
        EditorConfigSection()
            : flags(0)
            , indent_style("space")
            , indent_size(4)
            , tab_width(4)
            , charset("utf-8")
            , trim_trailing_whitespace(false)
            , insert_final_newline(false)
        {
        }
        
        /**
         * @brief print this section properties to the log file
         */
        void PrintToLog();
        
        inline void SetFilename(const wxFileName & file)    {filename = file;}

        inline const wxFileName & GetFilename() const       {return filename;}
        
        inline void SetCharset(const wxString& charset)
        {
            this->charset = charset;
            this->flags |= kCharsetSet;
        }
        inline bool IsCharsetSet() const        { return this->flags & kCharsetSet; }

        inline void SetIndentSize(size_t indent_size)
        {
            this->indent_size = indent_size;
            this->flags |= kIndentSizeSet;
        }

        inline bool IsIndentSizeSet() const     { return (this->flags & kTabWidthSet) || (this->flags & kIndentSizeSet); }

        inline void SetIndentStyle(const wxString& indent_style)
        {
            this->indent_style = indent_style;
            this->flags |= kIndentStyleSet;
        }

        inline bool IsIndentStyleSet() const    { return this->flags & kIndentStyleSet; }

        inline void SetInsertFinalNewline(bool insert_final_newline)
        {
            this->insert_final_newline = insert_final_newline;
            this->flags |= kInsertEOLSet;
        }
        inline bool IsInsertFinalNewlineSet() const { return this->flags & kInsertEOLSet; }

        inline void SetTabWidth(size_t tab_width)
        {
            this->tab_width = tab_width;
            this->flags |= kTabWidthSet;
        }
        inline bool IsTabWidthSet() const       { return (this->flags & kTabWidthSet) || (this->flags & kIndentSizeSet); }

        inline void SetTrimTrailingWhitespace(bool trim_trailing_whitespace)
        {
            this->trim_trailing_whitespace = trim_trailing_whitespace;
            this->flags |= kTrimTrailingWhitespaceSet;
        }
        inline bool IsTrimTrailingWhitespaceSet() const { return this->flags & kTrimTrailingWhitespaceSet; }

        inline void SetEndOfLine(const wxString& end_of_line)
        {
            this->end_of_line = end_of_line;
            this->flags |= kEOLStyleSet;
        }
        inline bool IsSetEndOfLineSet() const   { return this->flags & kEOLStyleSet; }

        inline const wxString& GetCharset() const { return charset; }
        // According to the docs:
        // When set to tab, the value of tab_width (if specified) will be used
        inline size_t GetIndentSize() const
        {
            return (GetIndentStyle() == "tab" && (this->flags & kTabWidthSet)) ? tab_width : indent_size;
        }

        // If user set the tab_width, return its value, otherwise return the size of the indent size
        inline size_t GetTabWidth() const               { return (this->flags & kTabWidthSet) ? tab_width : indent_size; }
        inline const wxString& GetIndentStyle() const   { return indent_style; }
        inline bool IsInsertFinalNewline() const        { return insert_final_newline; }
        inline bool IsTrimTrailingWhitespace() const    { return trim_trailing_whitespace; }
        inline const wxString& GetEndOfLine() const     { return end_of_line; }

        wxArrayString patterns;

        size_t flags;

        wxString indent_style;
        size_t indent_size;
        size_t tab_width;
        wxString charset;
        bool trim_trailing_whitespace;
        bool insert_final_newline;
        wxString end_of_line;
        wxFileName filename; // the path to the .editorconfig file 
    };

    // --------------------------------------------------------------
    // EditorConfigEvent event
    // --------------------------------------------------------------
    class _gsgsAPI EditorConfigEvent : public Event
    {
        EditorConfigSection m_editorConfigSection;

    public:
        EditorConfigEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        EditorConfigEvent(const EditorConfigEvent& event);
        EditorConfigEvent& operator=(const EditorConfigEvent& src);
        virtual ~EditorConfigEvent();
        virtual wxEvent* Clone() const { return new EditorConfigEvent(*this); }
        inline void SetEditorConfig(const EditorConfigSection& section)   { m_editorConfigSection = section; }
        inline const EditorConfigSection& GetEditorConfig() const         { return m_editorConfigSection; }
    };

    typedef void (wxEvtHandler::*clEditorConfigEventFunction)(EditorConfigEvent&);
    #define clEditorConfigEventHandler(func) wxEVENT_HANDLER_CAST(clEditorConfigEventFunction, func)

    class _gsgsAPI EditorConfigData
    {
    public:
        EditorConfigData();
        ~EditorConfigData();

        /**
         * @brief try and load a .editorconfig settings for 'filename'. We start looking from the current file location
         * and going up until we hit the root folder
         */
        bool LoadForFile(const wxFileName& filename, wxFileName& editorConfigFile);

        /**
         * @brief find the best section for a file
         */
        bool GetSectionForFile(const wxFileName& filename, EditorConfigSection& section);
    private:
        wxArrayString ProcessSection(wxString& strLine);
        void ProcessDirective(wxString& strLine);
        bool ReadUntil(wxChar delim, wxString& strLine, wxString& output);
    private:
        EditorConfigSection::Vec_t m_sections;
        bool m_rootFileFound;
    };

    // Send a EditorConfigData event. This event is sent whenever an editor requires
    // to update its properties (tabs vs spaces, EOL style etc)
    // A plugin may capture the event and send back Config data by doing the following:
    // Call:
    // 1. event.Skip(false)
    // 2. Set the data using event.SetEditorConfig(...) method
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_EDITOR_CONFIG_LOADING, EditorConfigEvent);


    class _gsgsAPI OptionsConfig : public ObjectData
    {
    public:
        enum {
            Opt_Unused10 = (1 << 0),
            Opt_Unused11 = (1 << 1),
            Opt_Unused12 = (1 << 2),
            Opt_AutoCompleteCurlyBraces = (1 << 3),
            Opt_AutoCompleteNormalBraces = (1 << 4),
            Opt_SmartAddFiles = (1 << 5),
            Opt_IconSet_FreshFarm = (1 << 6),
            Opt_TabStyleTRAPEZOID = (1 << 7),
            Opt_IconSet_Classic = (1 << 8),
            Opt_AutoCompleteDoubleQuotes = (1 << 9),
            Opt_NavKey_Shift = (1 << 10), // (No longer actively used)
            Opt_NavKey_Alt = (1 << 11),
            Opt_NavKey_Control = (1 << 12),
            Opt_IconSet_Classic_Dark = (1 << 13),
            Opt_Mark_Debugger_Line = (1 << 14),
            Opt_TabNoXButton = (1 << 15),
            Opt_TabColourPersistent = (1 << 16),
            Opt_TabColourDark = (1 << 17),
            Opt_Use_GSGS_Terminal = (1 << 18),
            Opt_Unused14 = (1 << 19),
            Opt_Unused15 = (1 << 20),
            Opt_AllowCaretAfterEndOfLine = (1 << 21),
            Opt_HideDockingWindowCaption = (1 << 22),
            Opt_WrapQuotes = (1 << 23),
            Opt_WrapBrackets = (1 << 24),
            Opt_WrapCmdWithDoubleQuotes = (1 << 25),
            Opt_FoldHighlightActiveBlock = (1 << 26),
            Opt_EnsureCaptionsVisible = (1 << 27),
            Opt_DisableMouseCtrlZoom = (1 << 28),
            Opt_UseBlockCaret = (1 << 29),
            Opt_TabStyleMinimal = (1 << 30),
        };

        enum {
            Opt2_MouseScrollSwitchTabs = (1 << 0),
            Opt2_SortTabsDropdownAlphabetically = (1 << 1),
            Opt2_PlaceNavBarAtTheTop = (1 << 2),
            Opt2_DisableCtrlTabForTabSwitching = (1 << 3),
            Opt2_SortNavBarDropdown = (1 << 4),
        };

        enum { nbTabHt_Tiny = 1, nbTabHt_Short, nbTabHt_Medium, nbTabHt_Tall };

        inline void EnableOption(size_t flag, bool b)
        {
            if(b) {
                m_options |= flag;
            } else {
                m_options &= ~flag;
            }
        }

        inline bool HasOption(size_t flag) const { return m_options & flag; }
        inline void EnableOption2(size_t flag, bool b)
        {
            if(b) {
                m_options2 |= flag;
            } else {
                m_options2 &= ~flag;
            }
        }

        inline bool HasOption2(size_t flag) const { return m_options2 & flag; }
    public:
        OptionsConfig() {}
        OptionsConfig(wxXmlNode* node);
        virtual ~OptionsConfig(void);

        inline void SetOutputTabsDirection(const wxDirection & dir)     { m_outputTabsDirection = dir; }
        inline const wxDirection & GetOutputTabsDirection() const       { return m_outputTabsDirection; }

        inline void SetWorkspaceTabsDirection(const wxDirection & dir)  { m_workspaceTabsDirection = dir; }
        inline const wxDirection & GetWorkspaceTabsDirection() const    { return m_workspaceTabsDirection; }
        wxString GetEOLAsString() const;
        //-------------------------------------
        // Setters/Getters
        //-------------------------------------
        inline void SetTabColourMatchesTheme(bool b)                    { EnableOption(Opt_TabColourPersistent, !b); }
        inline bool IsTabColourMatchesTheme() const;
        inline void SetTabColourDark(bool b)                            { EnableOption(Opt_TabColourDark, b); }
        inline bool IsTabColourDark() const;
        inline void SetTabHasXButton(bool b)                            { EnableOption(Opt_TabNoXButton, !b); }
        inline bool IsTabHasXButton() const                             { return !HasOption(Opt_TabNoXButton); }
        inline bool IsMouseScrollSwitchTabs() const                     { return HasOption2(Opt2_MouseScrollSwitchTabs); }
        inline void SetMouseScrollSwitchTabs(bool b)                    { EnableOption2(Opt2_MouseScrollSwitchTabs, b); }
        inline bool IsSortTabsDropdownAlphabetically() const            { return HasOption2(Opt2_SortTabsDropdownAlphabetically); }
        inline void SetSortTabsDropdownAlphabetically(bool b)           { EnableOption2(Opt2_SortTabsDropdownAlphabetically, b); }
        inline bool IsNavBarTop() const                                 { return HasOption2(Opt2_PlaceNavBarAtTheTop); }
        inline void SetNavBarTop(bool b)                                { EnableOption2(Opt2_PlaceNavBarAtTheTop, b); }
        inline bool IsCtrlTabEnabled() const                            { return !HasOption2(Opt2_DisableCtrlTabForTabSwitching); }
        inline void SetCtrlTabEnabled(bool b)                           { EnableOption2(Opt2_DisableCtrlTabForTabSwitching, !b); }
        inline bool IsSortNavBarDropdown() const                        { return HasOption2(Opt2_SortNavBarDropdown); }
        inline void SetSortNavBarDropdown(bool b)                       { EnableOption2(Opt2_SortNavBarDropdown, b); }

        inline void SetOptions(size_t options)                          { m_options = options; }
        inline size_t GetOptions() const                                { return m_options; }
        inline void SetTrimOnlyModifiedLines(bool set)                  { m_trimOnlyModifiedLines = set; }
        inline bool GetTrimOnlyModifiedLines() const                    { return m_trimOnlyModifiedLines; }
        inline void SetPreferredLocale(const wxString& str)             { this->m_preferredLocale = str; }
        inline wxString GetPreferredLocale() const                      { return m_preferredLocale; }
        inline void SetUseLocale(bool useLocale)                        { this->m_useLocale = useLocale; }
        inline bool GetUseLocale() const                                { return m_useLocale; }
        inline void SetMswTheme(bool mswTheme)                          { this->m_mswTheme = mswTheme; }
        inline bool GetMswTheme() const                                 { return m_mswTheme; }
        inline void SetDockingStyle(int dockingStyle)                   { this->m_dockingStyle = dockingStyle; }
        inline int GetDockingStyle() const                              { return m_dockingStyle; }
        inline void SetAntiAliasing(bool set)                           { m_AntiAliasing = set; }
        inline bool GetAntiAliasing() const                             { return m_AntiAliasing; }
        inline void SetCaretUseCamelCase(bool caretUseCamelCase)        { this->m_caretUseCamelCase = caretUseCamelCase; }
        inline bool GetCaretUseCamelCase() const                        { return m_caretUseCamelCase; }
        inline void SetDontAutoFoldResults(bool dontAutoFoldResults)    { this->m_dontAutoFoldResults = dontAutoFoldResults; }
        inline bool GetDontAutoFoldResults() const                      { return m_dontAutoFoldResults; }
        inline void SetDontOverrideSearchStringWithSelection(bool dontOverrideSearchStringWithSelection) { m_dontOverrideSearchStringWithSelection = dontOverrideSearchStringWithSelection; }
        inline bool GetDontOverrideSearchStringWithSelection() const    { return m_dontOverrideSearchStringWithSelection; }
        inline void SetShowDebugOnRun(bool showDebugOnRun)              { this->m_showDebugOnRun = showDebugOnRun; }
        inline bool GetShowDebugOnRun() const                           { return m_showDebugOnRun; }
        inline bool GetDisableSemicolonShift() const                    { return m_disableSemicolonShift; }
        inline void SetDisableSemicolonShift(bool disableSemicolonShift) { this->m_disableSemicolonShift = disableSemicolonShift; }
        inline void SetDisableSmartIndent(bool disableSmartIndent)      { this->m_disableSmartIndent = disableSmartIndent; }
        inline bool GetDisableSmartIndent() const                       { return m_disableSmartIndent; }
        inline void SetTrimLine(bool trimLine)                          { this->m_TrimLine = trimLine; }
        inline bool GetTrimLine() const                                 { return m_TrimLine; }
        inline void SetAppendLF(bool appendLF)                          { this->m_AppendLF = appendLF; }
        inline bool GetAppendLF() const                                 { return m_AppendLF; }
        inline void SetFindBarAtBottom(bool findBarAtBottom)            { this->m_findBarAtBottom = findBarAtBottom; }
        inline bool GetFindBarAtBottom() const                          { return m_findBarAtBottom; }
        inline void SetShowReplaceBar(bool show)                        { m_showReplaceBar = show; }
        inline bool GetShowReplaceBar() const                           { return m_showReplaceBar; }
        inline void SetHideOutpuPaneOnUserClick(bool set)
        {
            this->m_hideOutpuPaneOnUserClick = set;
        }
        inline bool GetHideOutpuPaneOnUserClick() const { return m_hideOutpuPaneOnUserClick; }
        inline void SetHideOutputPaneNotIfBuild(bool set)
        {
            this->m_hideOutputPaneNotIfBuild = set;
        }
        inline bool GetHideOutputPaneNotIfBuild() const { return m_hideOutputPaneNotIfBuild; }
        inline void SetHideOutputPaneNotIfSearch(bool set)
        {
            this->m_hideOutputPaneNotIfSearch = set;
        }
        inline bool GetHideOutputPaneNotIfSearch() const { return m_hideOutputPaneNotIfSearch; }
        inline void SetHideOutputPaneNotIfReplace(bool set)
        {
            this->m_hideOutputPaneNotIfReplace = set;
        }
        inline bool GetHideOutputPaneNotIfReplace() const { return m_hideOutputPaneNotIfReplace; }
        inline void SetHideOutputPaneNotIfReferences(bool set)
        {
            this->m_hideOutputPaneNotIfReferences = set;
        }
        inline bool GetHideOutputPaneNotIfReferences() const { return m_hideOutputPaneNotIfReferences; }
        inline void SetHideOutputPaneNotIfOutput(bool set)
        {
            this->m_hideOutputPaneNotIfOutput = set;
        }
        inline bool GetHideOutputPaneNotIfOutput() const { return m_hideOutputPaneNotIfOutput; }
        inline void SetHideOutputPaneNotIfTrace(bool set)
        {
            this->m_hideOutputPaneNotIfTrace = set;
        }
        inline bool GetHideOutputPaneNotIfTrace() const { return m_hideOutputPaneNotIfTrace; }
        inline void SetHideOutputPaneNotIfTasks(bool set)
        {
            this->m_hideOutputPaneNotIfTasks = set;
        }
        inline bool GetHideOutputPaneNotIfTasks() const { return m_hideOutputPaneNotIfTasks; }
        inline void SetHideOutputPaneNotIfBuildQ(bool set)
        {
            this->m_hideOutputPaneNotIfBuildQ = set;
        }
        inline bool GetHideOutputPaneNotIfBuildQ() const { return m_hideOutputPaneNotIfBuildQ; }
        inline void SetHideOutputPaneNotIfCppCheck(bool set)
        {
            this->m_hideOutputPaneNotIfCppCheck = set;
        }
        inline bool GetHideOutputPaneNotIfCppCheck() const { return m_hideOutputPaneNotIfCppCheck; }
        inline void SetHideOutputPaneNotIfSvn(bool set)
        {
            this->m_hideOutputPaneNotIfSvn = set;
        }
        inline bool GetHideOutputPaneNotIfSvn() const { return m_hideOutputPaneNotIfSvn; }
        inline void SetHideOutputPaneNotIfCscope(bool set)
        {
            this->m_hideOutputPaneNotIfCscope = set;
        }
        inline bool GetHideOutputPaneNotIfCscope() const { return m_hideOutputPaneNotIfCscope; }
        inline void SetHideOutputPaneNotIfGit(bool set)
        {
            this->m_hideOutputPaneNotIfGit = set;
        }
        inline bool GetHideOutputPaneNotIfGit() const { return m_hideOutputPaneNotIfGit; }
        inline void SetHideOutputPaneNotIfDebug(bool set)
        {
            this->m_hideOutputPaneNotIfDebug = set;
        }
        inline bool GetHideOutputPaneNotIfDebug() const { return m_hideOutputPaneNotIfDebug; }
        inline void SetHideOutputPaneNotIfMemCheck(bool set)
        {
            this->m_hideOutputPaneNotIfMemCheck = set;
        }
        inline bool GetHideOutputPaneNotIfMemCheck() const { return m_hideOutputPaneNotIfMemCheck; }
        inline void SetHideChangeMarkerMargin(bool set)
        {
            this->m_hideChangeMarkerMargin = set;
        }

        inline bool GetHideChangeMarkerMargin() const { return m_hideChangeMarkerMargin; }

        inline bool GetIndentedComments() const { return m_indentedComments; }
        inline bool GetDisplayFoldMargin() const { return m_displayFoldMargin; }
        inline bool GetUnderlineFoldLine() const { return m_underlineFoldLine; }
        inline bool GetScrollBeyondLastLine() const { return m_scrollBeyondLastLine; }
        inline wxString GetFoldStyle() const { return m_foldStyle; }
        inline bool GetDisplayBookmarkMargin() const { return m_displayBookmarkMargin; }
        inline wxString GetBookmarkShape() const { return m_bookmarkShape; }
        wxColour GetBookmarkFgColour(size_t index) const;

        wxColour GetBookmarkBgColour(size_t index) const;

        wxString GetBookmarkLabel(size_t index) const;

        inline bool GetClearHighlitWordsOnFind() const { return m_clearHighlitWordsOnFind; }

        inline bool GetHighlightCaretLine() const { return m_highlightCaretLine; }
        inline bool GetDisplayLineNumbers() const { return m_displayLineNumbers; }
        inline bool GetRelativeLineNumbers() const { return m_relativeLineNumbers; }
        inline bool GetHighlightCurrentLineNumber() const { return m_highlightCurLineNumber; }
        inline bool GetShowIndentationGuidelines() const { return m_showIndentationGuidelines; }
        inline const wxColour & GetCaretLineColour() const { return m_caretLineColour; }

        inline void SetIndentedComments(bool b) { m_indentedComments = b; }
        inline void SetDisplayFoldMargin(bool b) { m_displayFoldMargin = b; }
        inline void SetUnderlineFoldLine(bool b) { m_underlineFoldLine = b; }
        inline void SetScrollBeyondLastLine(bool b) { m_scrollBeyondLastLine = b; }
        inline void SetFoldStyle(wxString s) { m_foldStyle = s; }
        inline void SetDisplayBookmarkMargin(bool b) { m_displayBookmarkMargin = b; }
        inline void SetBookmarkShape(wxString s) { m_bookmarkShape = s; }
        inline void SetBookmarkFgColour(wxColour c, size_t index);

        void SetBookmarkBgColour(wxColour c, size_t index);

        void SetBookmarkLabel(const wxString & label, size_t index);

        inline void SetClearHighlitWordsOnFind(bool b)  { m_clearHighlitWordsOnFind = b; }

        inline void SetHighlightCaretLine(bool b)           { m_highlightCaretLine = b; }
        inline void SetDisplayLineNumbers(bool b)           { m_displayLineNumbers = b; }
        inline void SetRelativeLineNumbers(bool b)          { m_relativeLineNumbers = b; }
        inline void SetHighlightCurrentLineNumber(bool b)   { m_highlightCurLineNumber = b; }
        inline void SetShowIndentationGuidelines(bool b)    { m_showIndentationGuidelines = b; }
        inline void SetCaretLineColour(const wxColour & c)  { m_caretLineColour = c; }

        inline void SetIndentUsesTabs(bool indentUsesTabs)  { this->m_indentUsesTabs = indentUsesTabs; }
        inline bool GetIndentUsesTabs() const               { return m_indentUsesTabs; }
        inline void SetIndentWidth(int indentWidth)         { this->m_indentWidth = indentWidth; }
        inline const int& GetIndentWidth() const            { return m_indentWidth; }
        inline void SetTabWidth(int tabWidth)               { this->m_tabWidth = tabWidth; }
        inline const int& GetTabWidth() const               { return m_tabWidth; }

        inline void SetExtraAscent(int ext)                 { m_extraAscent = ext; }
        inline int GetExtraAscent() const                   { return m_extraAscent; }

        inline void SetIconsSize(int size)              { m_iconsSize = size; }
        inline const int& GetIconsSize() const          { return m_iconsSize; }

        inline void SetFileFontEncoding(const wxString & strFileFontEncoding);
        inline wxFontEncoding GetFileFontEncoding() const { return m_fileFontEncoding; }

        inline void SetShowWhitspaces(const int& showWhitspaces){ this->m_showWhitspaces = showWhitspaces; }
        inline const int& GetShowWhitspaces() const             { return m_showWhitspaces; }

        inline void SetFoldAtElse(bool foldAtElse)       { this->m_foldAtElse = foldAtElse; }
        inline void SetFoldCompact(bool foldCompact)     { this->m_foldCompact = foldCompact; }
        inline bool GetFoldAtElse() const                { return m_foldAtElse; }
        inline bool GetFoldCompact() const               { return m_foldCompact; }
        inline void SetFoldPreprocessor(bool foldPreprocessor) { this->m_foldPreprocessor = foldPreprocessor; }
        inline bool GetFoldPreprocessor() const          { return m_foldPreprocessor; }

        inline void SetEdgeColour(const wxColour& edgeColour)   { this->m_edgeColour = edgeColour; }
        inline void SetEdgeColumn(int edgeColumn)               { this->m_edgeColumn = edgeColumn; }
        inline void SetEdgeMode(int edgeMode)                   { this->m_edgeMode = edgeMode; }

        inline const wxColour& GetEdgeColour() const            { return m_edgeColour; }
        inline int GetEdgeColumn() const                        { return m_edgeColumn; }
        inline int GetEdgeMode() const                           { return m_edgeMode; }

        inline void SetHighlightMatchedBraces(bool highlightMatchedBraces)
        {
            this->m_highlightMatchedBraces = highlightMatchedBraces;
        }
        inline bool GetHighlightMatchedBraces() const    { return m_highlightMatchedBraces; }

        inline void SetAutoAddMatchedCurlyBraces(bool autoAddMatchedBraces)
        {
            EnableOption(Opt_AutoCompleteCurlyBraces, autoAddMatchedBraces);
        }

        inline bool GetAutoAddMatchedCurlyBraces() const        { return HasOption(Opt_AutoCompleteCurlyBraces); }

        inline void SetAutoAddMatchedNormalBraces(bool b)       { EnableOption(Opt_AutoCompleteNormalBraces, b); }

        inline bool GetAutoAddMatchedNormalBraces() const       { return HasOption(Opt_AutoCompleteNormalBraces); }
        inline bool GetAutoCompleteDoubleQuotes() const         { return HasOption(Opt_AutoCompleteDoubleQuotes); }
        inline void SetAutoCompleteDoubleQuotes(bool b)         { EnableOption(Opt_AutoCompleteDoubleQuotes, b); }
        inline void SetHighlightFoldWhenActive(bool b)          { EnableOption(Opt_FoldHighlightActiveBlock, b); }
        inline bool IsHighlightFoldWhenActive() const           { return HasOption(Opt_FoldHighlightActiveBlock); }
        inline void SetFoldBgColour(const wxColour& bg)         { this->m_foldBgColour = bg; }
        inline const wxColour& GetFoldBgColour() const          { return m_foldBgColour; }
        inline void SetAutoAdjustHScrollBarWidth(bool autoAdjustHScrollBarWidth)
        {
            this->m_autoAdjustHScrollBarWidth = autoAdjustHScrollBarWidth;
        }
        inline bool GetAutoAdjustHScrollBarWidth() const { return m_autoAdjustHScrollBarWidth; }
        inline void SetCaretBlinkPeriod(const int& caretBlinkPeriod) { this->m_caretBlinkPeriod = caretBlinkPeriod; }
        inline void SetCaretWidth(const int& caretWidth) { this->m_caretWidth = caretWidth; }
        inline const int& GetCaretBlinkPeriod() const { return m_caretBlinkPeriod; }
        inline const int& GetCaretWidth() const { return m_caretWidth; }

        inline void SetCopyLineEmptySelection(const bool copyLineEmptySelection) { m_copyLineEmptySelection = copyLineEmptySelection; }
        inline bool GetCopyLineEmptySelection() const           { return m_copyLineEmptySelection; }

        inline void SetProgramConsoleCommand(const wxString& programConsoleCommand)
        {
            this->m_programConsoleCommand = programConsoleCommand;
        }

        inline const wxString& GetProgramConsoleCommand() const { return m_programConsoleCommand; }

        inline void SetEolMode(const wxString& eolMode)         { this->m_eolMode = eolMode; }
        inline const wxString& GetEolMode() const               { return m_eolMode; }
        inline void SetCaretLineAlpha(int caretLineAlpha)       { this->m_caretLineAlpha = caretLineAlpha; }
        inline int GetCaretLineAlpha() const                    { return m_caretLineAlpha; }
        inline bool GetDontTrimCaretLine() const                { return m_dontTrimCaretLine; }

        inline bool GetWordWrap() const                         { return m_wordWrap; }
        inline void SetWordWrap(bool wordWrap)                  { m_wordWrap = wordWrap; }

        inline void SetDebuggerMarkerLine(const wxColour& debuggerMarkerLine) { this->m_debuggerMarkerLine = debuggerMarkerLine; }
        inline const wxColour& GetDebuggerMarkerLine() const    { return m_debuggerMarkerLine; }

        inline void SetShowDockingWindowCaption(bool show)      { EnableOption(Opt_HideDockingWindowCaption, !show); }
        inline void SetEnsureCaptionsVisible(bool b)            { EnableOption(Opt_EnsureCaptionsVisible, b); }
        inline bool IsEnsureCaptionsVisible() const             { return HasOption(Opt_EnsureCaptionsVisible); }
        inline bool IsShowDockingWindowCaption() const          { return !HasOption(Opt_HideDockingWindowCaption); }
        inline bool IsWrapSelectionWithQuotes() const           { return HasOption(Opt_WrapQuotes); }
        inline bool IsWrapSelectionBrackets() const             { return HasOption(Opt_WrapBrackets); }
        inline void SetWrapSelectionWithQuotes(bool b)          { return EnableOption(Opt_WrapQuotes, b); }
        inline void SetWrapSelectionBrackets(bool b)            { return EnableOption(Opt_WrapBrackets, b); }
        inline void SetNotebookTabHeight(int nbTabHeight)       { m_nbTabHeight = nbTabHeight; }
        inline int GetNotebookTabHeight() const                 { return m_nbTabHeight; }
        inline void SetImageTabSize(const wxSize & size)        { mImageTabSize = size; }
        inline const wxSize & GetImageTabSize() const           { return mImageTabSize; }

        inline void MSWWrapCmdWithDoubleQuotes(bool b)          { EnableOption(Opt_WrapCmdWithDoubleQuotes, b); }
        inline bool MSWIsWrapCmdWithDoubleQuotes() const        { return true; }
        inline bool IsMouseZoomEnabled() const                  { return !HasOption(Opt_DisableMouseCtrlZoom); }
        inline void SetMouseZoomEnabled(bool b)                 { EnableOption(Opt_DisableMouseCtrlZoom, !b); }

        inline const wxString& GetWebSearchPrefix() const       { return m_webSearchPrefix; }
        inline void SetWebSearchPrefix(const wxString& prefix)  { this->m_webSearchPrefix = prefix; }

        void UpdateFromEditorConfig(const EditorConfigSection& section);

        /**
         * Return an XML representation of this object
         * \return XML node
         */
        wxXmlNode* ToXml() const;
    protected:
        wxString m_foldStyle;
        wxString m_bookmarkShape;
        wxString m_bookmarkBgColours;
        wxString m_bookmarkFgColours;
        wxString m_bookmarkLabels;
        wxColour m_caretLineColour;
        wxColour m_edgeColour;
        wxColour m_foldBgColour;
        wxFontEncoding m_fileFontEncoding;
        wxString m_programConsoleCommand;
        wxString m_eolMode;
        int m_indentWidth;
        int m_tabWidth;
        int m_iconsSize;
        int m_showWhitspaces;
        int m_edgeMode;
        int m_edgeColumn;
        int m_caretWidth;
        int m_caretBlinkPeriod;
        int m_extraAscent;
        bool m_copyLineEmptySelection;
        bool m_hideChangeMarkerMargin;
        bool m_hideOutpuPaneOnUserClick;
        bool m_hideOutputPaneNotIfBuild;
        bool m_hideOutputPaneNotIfSearch;
        bool m_hideOutputPaneNotIfReplace;
        bool m_hideOutputPaneNotIfReferences;
        bool m_hideOutputPaneNotIfOutput;
        bool m_hideOutputPaneNotIfTrace;
        bool m_hideOutputPaneNotIfTasks;
        bool m_hideOutputPaneNotIfBuildQ;
        bool m_hideOutputPaneNotIfCppCheck;
        bool m_hideOutputPaneNotIfSvn;
        bool m_hideOutputPaneNotIfCscope;
        bool m_hideOutputPaneNotIfGit;
        bool m_hideOutputPaneNotIfDebug;
        bool m_hideOutputPaneNotIfMemCheck;
        bool m_findBarAtBottom;
        bool m_showReplaceBar;
        bool m_TrimLine;
        bool m_AppendLF;
        bool m_disableSmartIndent;
        bool m_disableSemicolonShift;
        bool m_highlightCaretLine;
        bool m_clearHighlitWordsOnFind;
        bool m_displayLineNumbers;
        bool m_relativeLineNumbers = false;
        bool m_highlightCurLineNumber = true;
        bool m_showIndentationGuidelines;
        bool m_displayBookmarkMargin;
        bool m_displayFoldMargin;
        bool m_underlineFoldLine;
        bool m_scrollBeyondLastLine;
        bool m_dontAutoFoldResults;
        bool m_dontOverrideSearchStringWithSelection;
        bool m_showDebugOnRun;
        bool m_caretUseCamelCase;
        bool m_dontTrimCaretLine;
        bool m_wordWrap;
        bool m_indentedComments;
        bool m_mswTheme;
        bool m_useLocale;
        bool m_trimOnlyModifiedLines;
        bool m_indentUsesTabs;
        bool m_foldCompact;
        bool m_foldAtElse;
        bool m_foldPreprocessor;
        bool m_highlightMatchedBraces;
        bool m_autoAdjustHScrollBarWidth;
        bool m_AntiAliasing;
        size_t m_options;
        size_t m_options2;
        wxColour m_debuggerMarkerLine;
        wxDirection m_workspaceTabsDirection; // Up/Down/Left/Right
        wxDirection m_outputTabsDirection;    // Up/Down
        int m_nbTabHeight; // Should notebook tabs be too tall, too short or...
        int m_caretLineAlpha;
        int m_dockingStyle;
        wxSize mImageTabSize;
        wxString m_webSearchPrefix;
        wxString m_preferredLocale;
    };

    class _gsgsAPI TreeBookNode
    {
    public:
        TreeBookNode();
        virtual ~TreeBookNode();

        virtual void Save(OptionsConfig * options) = 0;
        virtual void Apply() {}
        virtual bool IsRestartRequired() { return false; }
    };
    typedef std::list<TreeBookNode*> TreeBookNodeList;
}
typedef gsgs::SharedPtr<gsgs::OptionsConfig> OptionsConfigPtr;
#endif