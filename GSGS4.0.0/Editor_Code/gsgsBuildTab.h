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

#ifndef gsgs_BUILDTAB_H
#define gsgs_BUILDTAB_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include "gsgsToolsetManager.h"
#include <wx/font.h>
#include <wx/fdrepdlg.h>
#include <wx/dataview.h>
#include <wx/stopwatch.h>
#include <wx/regex.h>
#include <wx/stc/stc.h>

namespace gsgs
{
    class Editor;
    enum LINE_SEVERITY {
        SV_NONE = 0,
        SV_WARNING = 1,
        SV_ERROR = 2,
        SV_SUCCESS = 3,
        SV_DIR_CHANGE = 4,
    };

    class BuildLineInfo
    {
    protected:
        wxString m_filename;
        int m_line_number;
        int m_column;
        LINE_SEVERITY m_severity;
        int m_lineInBuildTab;
        int m_regexLineMatch;

    public:
        BuildLineInfo()
            : m_line_number(-1)
            , m_column(wxNOT_FOUND)
            , m_severity(SV_NONE)
            , m_lineInBuildTab(-1)
            , m_regexLineMatch(0)
        {
        }
        ~BuildLineInfo() {}

        /**
         * @brief try to expand the file name and normalize it into a fullpath
         */
        void NormalizeFilename(const wxArrayString& directories, const wxString& cygwinPath);

        void SetColumn(int column) { this->m_column = column; }
        int GetColumn() const { return m_column; }
        void SetRegexLineMatch(int regexLineMatch) { this->m_regexLineMatch = regexLineMatch; }
        int GetRegexLineMatch() const { return m_regexLineMatch; }
        void SetFilename(const wxString& filename);
        void SetLineNumber(int line_number) { this->m_line_number = line_number; }
        void SetSeverity(LINE_SEVERITY severity) { this->m_severity = severity; }
        const wxString& GetFilename() const { return m_filename; }
        int GetLineNumber() const { return m_line_number; }
        LINE_SEVERITY GetSeverity() const { return m_severity; }
        void SetLineInBuildTab(int lineInBuildTab) { this->m_lineInBuildTab = lineInBuildTab; }
        int GetLineInBuildTab() const { return m_lineInBuildTab; }
    };

    class CmpPattern
    {
    protected:
        wxRegEx* m_regex;
        wxString m_fileIndex;
        wxString m_lineIndex;
        wxString m_colIndex;
        LINE_SEVERITY m_severity;

    public:
        CmpPattern(wxRegEx* re, const wxString& file, const wxString& line, const wxString& column, LINE_SEVERITY severity)
            : m_regex(re)
            , m_fileIndex(file)
            , m_lineIndex(line)
            , m_colIndex(column)
            , m_severity(severity)
        {
        }

        ~CmpPattern() { wxDELETE(m_regex); }

        /**
         * @brief return true if "line" matches this pattern
         * @param lineInfo [output]
         */
        bool Matches(const wxString& line, BuildLineInfo& lineInfo);

        void SetFileIndex(const wxString& fileIndex) { this->m_fileIndex = fileIndex; }
        void SetLineIndex(const wxString& lineIndex) { this->m_lineIndex = lineIndex; }
        void SetRegex(wxRegEx* regex) { this->m_regex = regex; }
        void SetSeverity(LINE_SEVERITY severity) { this->m_severity = severity; }
        const wxString& GetFileIndex() const { return m_fileIndex; }
        const wxString& GetLineIndex() const { return m_lineIndex; }
        void SetColIndex(const wxString& colIndex) { this->m_colIndex = colIndex; }
        const wxString& GetColIndex() const { return m_colIndex; }
        wxRegEx* GetRegex() { return m_regex; }
        LINE_SEVERITY GetSeverity() const { return m_severity; }
    };
    typedef SharedPtr<CmpPattern> CmpPatternPtr;

    class BuildTabSettingsData : public ObjectData
    {
        bool m_skipWarnings;
        wxString m_warnColour;
        wxString m_errorColour;
        wxString m_warnColourBg;
        wxString m_errorColourBg;
        bool m_boldErrFont;
        bool m_boldWarnFont;
        int m_showBuildPane;
        bool m_autoHide;
        bool m_autoShow;
        bool m_errorsFirstLine;
        int m_errorWarningStyle;
        int m_buildpaneScrollTo;
        wxFont m_buildFont;

    private:
        BuildTabSettingsData(const BuildTabSettingsData& rhs);
        BuildTabSettingsData& operator=(const BuildTabSettingsData& rhs);

    public:
        enum ErrorsWarningStyle { EWS_NoMarkers = 0x00000000, EWS_Bookmarks = 0x00000002, EWS_Annotate = 0x00000004 };

        enum ShowBuildPane { ShowOnStart, ShowOnEnd, DontShow };

        BuildTabSettingsData();
        ~BuildTabSettingsData();

        void Serialize(Serializer& arch);
        void DeSerialize(Serializer& arch);

        void SetBuildFont(const wxFont& buildFont) { this->m_buildFont = buildFont; }
        const wxFont& GetBuildFont() const { return m_buildFont; }
        
        // Setters
        void SetBoldErrFont(const bool& boldErrFont) { this->m_boldErrFont = boldErrFont; }
        void SetBoldWarnFont(const bool& boldWarnFont) { this->m_boldWarnFont = boldWarnFont; }
        void SetErrorColour(const wxString& errorColour) { this->m_errorColour = errorColour; }
        void SetErrorColourBg(const wxString& errorColourBg) { this->m_errorColourBg = errorColourBg; }
        void SetShowBuildPane(int showBuildPane) { this->m_showBuildPane = showBuildPane; }
        void SetSkipWarnings(const bool& skipWarnings) { this->m_skipWarnings = skipWarnings; }
        void SetBuildPaneScrollDestination(int buildpaneScrollTo) { this->m_buildpaneScrollTo = buildpaneScrollTo; }
        void SetWarnColour(const wxString& warnColour) { this->m_warnColour = warnColour; }
        void SetWarnColourBg(const wxString& warnColourBg) { this->m_warnColourBg = warnColourBg; }
        // Getters
        const bool& GetBoldErrFont() const { return m_boldErrFont; }
        const bool& GetBoldWarnFont() const { return m_boldWarnFont; }
        const wxString& GetErrorColour() const { return m_errorColour; }
        const wxString& GetErrorColourBg() const { return m_errorColourBg; }
        int GetShowBuildPane() const { return m_showBuildPane; }
        const bool& GetSkipWarnings() const { return m_skipWarnings; }
        int GetBuildPaneScrollDestination() const { return m_buildpaneScrollTo; }
        const wxString& GetWarnColour() const { return m_warnColour; }
        const wxString& GetWarnColourBg() const { return m_warnColourBg; }

        // Setters
        void SetAutoHide(const bool& autoHide) { this->m_autoHide = autoHide; }
        const bool& GetAutoHide() const { return m_autoHide; }

        void SetAutoShow(const bool& autoShow) { this->m_autoShow = autoShow; }
        const bool& GetAutoShow() const { return m_autoShow; }

        void SetErrorsFirstLine(const bool& errorsFirstLine) { this->m_errorsFirstLine = errorsFirstLine; }
        const bool& GetErrorsFirstLine() const { return m_errorsFirstLine; }

        void SetErrorWarningStyle(const int& errorWarningStyle) { this->m_errorWarningStyle = errorWarningStyle; }
        const int& GetErrorWarningStyle() const { return m_errorWarningStyle; }
    };

    struct CmpPatterns {
        std::vector<CmpPatternPtr> errorsPatterns;
        std::vector<CmpPatternPtr> warningPatterns;
    };

    class NewBuildTab : public wxPanel
    {
    public:
        NewBuildTab(wxWindow* parent);
        virtual ~NewBuildTab();

        void ScrollToBottom();
        bool GetBuildEndedSuccessfully() const { return m_errorCount == 0 && !m_buildInterrupted; }
        void SetBuildInterrupted(bool b) { m_buildInterrupted = b; }

        bool IsEmpty() const { return m_view->IsEmpty(); }

        bool IsBuildInProgress() const { return m_buildInProgress; }

        void Clear() { DoClear(); }

        wxString GetBuildContent() const;
        void AppendLine(const wxString& text);

    protected:
        void OnTheme(wxCommandEvent& event);
        void OnBuildStarted(Event& e);
        void OnBuildEnded(Event& e);
        void OnBuildAddLine(Event& e);
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnWorkspaceLoaded(wxCommandEvent& e);
        void OnNextBuildError(wxCommandEvent& e);
        void OnNextBuildErrorUI(wxUpdateUIEvent& e);
        void OnMenu(wxContextMenuEvent& e);
        void OnOpenInEditorUI(wxUpdateUIEvent& e);
        void OnOpenInEditor(wxCommandEvent& e);
        void OnClear(wxCommandEvent& e);
        void OnClearUI(wxUpdateUIEvent& e);
        void OnStyleNeeded(wxStyledTextEvent& event);
        void OnHotspotClicked(wxStyledTextEvent& event);
        void OnIdle(wxIdleEvent& event);
    protected:
        void InitView(const wxString& theme = "");
        void CenterLineInView(int line);
        void DoCacheRegexes();
        BuildLineInfo* DoProcessLine(const wxString& line);
        void DoProcessOutput(bool compilationEnded, bool isSummaryLine);
        void DoSearchForDirectory(const wxString& line);
        bool DoGetCompilerPatterns(const wxString& compilerName, CmpPatterns& patterns);
        void DoClear();
        void MarkEditor(Editor* editor);
        void DoToggleWindow();
        bool DoSelectAndOpen(int buildViewLine, bool centerLine);
        wxFont DoGetFont() const;
        void DoCentreErrorLine(BuildLineInfo* bli, Editor* editor, bool centerLine);
        void ColourOutput();
        CmpPatternPtr GetMatchingRegex(const wxString& lineText, LINE_SEVERITY& severity);
    private:
        enum BuildpaneScrollTo { ScrollToFirstError, ScrollToFirstItem, ScrollToEnd };

        typedef std::map<wxString, CmpPatterns> MapCmpPatterns_t;
        typedef std::multimap<wxString, BuildLineInfo*> MultimapBuildInfo_t;
        typedef std::list<BuildLineInfo*> BuildInfoList_t;

        wxString m_output;
        wxStyledTextCtrl* m_view;
        CompilerPtr m_cmp;
        MapCmpPatterns_t m_cmpPatterns;
        int m_warnCount;
        int m_errorCount;
        int m_maxlineWidth;
        int m_lastLineColoured;
        BuildTabSettingsData m_buildTabSettings;
        BuildTabSettingsData::ShowBuildPane m_showMe;
        wxStopWatch m_sw;
        MultimapBuildInfo_t m_buildInfoPerFile;
        wxArrayString m_directories;
        BuildpaneScrollTo m_buildpaneScrollTo;
        BuildInfoList_t m_errorsAndWarningsList;
        BuildInfoList_t m_errorsList;
        BuildInfoList_t::iterator m_curError;
        wxString m_cygwinRoot;
        std::map<int, BuildLineInfo*> m_viewData;
        bool m_buildInProgress;
        bool m_skipWarnings;
        bool m_buildInterrupted;
        bool m_autoHide;
    };
}
#endif
