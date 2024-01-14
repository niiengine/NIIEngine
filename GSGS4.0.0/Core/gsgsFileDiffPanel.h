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

#ifndef gsgsDIFFSIDEBYSIDEPANEL_H
#define gsgsDIFFSIDEBYSIDEPANEL_H

#include "gsgsPreInclude.h"
#include "gsgsConfig.h"
#include "gsgsQuickFindBar.h"
#include "wxcrafter_plugin.h"
#include "NewProjectDialog.h"
#include "globals.h"
#include "imanager.h"
#include <thread>

namespace gsgs
{
    class _gsgsAPI DiffConfig : public ObjectData
    {
    public:
        enum {
            kCopyLeftToRightAndMove = (1 << 0),
            kCopyRightToLeftAndMove = (1 << 1),
            kShowLineNumbers = (1 << 2),
            kHideOverviewBar = (1 << 3),
            kIgnoreWhitespace = (1 << 4),
        };

        // View mode
        enum {
            kViewModeDefault = 0,
            kViewSingle = (1 << 0),
            kViewVerticalSplit = (1 << 1),
            kViewHorizontalSplit = (1 << 2),
            kAllViewModes = (kViewSingle | kViewVerticalSplit | kViewHorizontalSplit),
        };
    public:
        DiffConfig();
        virtual ~DiffConfig();

        virtual void FromJSON(const JSONObject& json);
        virtual JSONObject ToJSON() const;

        DiffConfig& SetViewMode(int mode)
        {
            m_viewFlags &= ~kAllViewModes;
            m_viewFlags |= mode;
            return *this;
        }

        bool IsSplitVertical() const
        {
            if(!(m_viewFlags & kAllViewModes)) {
                // none of the modes is selected, select the default "Vertical"
                return true;
            }
            return m_viewFlags & kViewVerticalSplit;
        }

        bool IsSplitHorizontal() const          { return m_viewFlags & kViewHorizontalSplit; }

        bool IsSingleViewMode() const           { return m_viewFlags & kViewSingle; }

        bool ShowLineNumbers() const            { return m_flags & kShowLineNumbers; }
        void SetShowLineNumbers(bool flag)
        {
            m_flags &= ~kShowLineNumbers;
            if(flag) 
            { 
                m_flags |= kShowLineNumbers; 
            }
        }

        bool IsOverviewBarShown() const         { return !(m_flags & kHideOverviewBar); }
        void SetOverviewBarShow(bool b)
        {
            m_flags &= ~kHideOverviewBar;
            if(!b) { m_flags |= kHideOverviewBar; }
        }

        void SetFlags(size_t flags)             { this->m_flags = flags; }
        size_t GetFlags() const                 { return m_flags; }
        bool IsIgnoreWhitespace() const         { return m_flags & kIgnoreWhitespace; }
        void SetIgnoreWhitespace(bool b)
        {
            m_flags &= ~kIgnoreWhitespace;
            if(b) { m_flags |= kIgnoreWhitespace; }
        }

        DiffConfig & SetLeftFile(const wxString& leftFile)
        {
            this->m_leftFile = leftFile;
            return *this;
        }
        DiffConfig& SetRightFile(const wxString& rightFile)
        {
            this->m_rightFile = rightFile;
            return *this;
        }
        const wxString& GetLeftFile() const     { return m_leftFile; }
        const wxString& GetRightFile() const    { return m_rightFile; }
        DiffConfig& Load();
        void Save();
    protected:
        size_t m_flags;
        size_t m_viewFlags;
        wxString m_leftFile;
        wxString m_rightFile;
    };

    /**
     * @class FileDiff
     * @brief Diff 2 files and return the result
     * @code

        // An example of using the FileDiff class:
        FileDiff d;
        d.Diff(filePath1, filePath2);
        const FileDiff::LineInfoVec_t & result = d.GetResult();

        // Create 2 strings "left" and "right"
        wxString leftContent, rightContent;
        for(size_t i = 0; i < result.size(); ++i) {
            // format the lines
            switch(result.at(i).m_type) {
            case FileDiff::LINE_ADDED:
                leftContent  << "- \n";
                rightContent << "+ " << result.at(i).m_line;
                break;
            case FileDiff::LINE_REMOVED:
                leftContent  << "+ " << result.at(i).m_line;
                rightContent << "- \n";
                break;
            case FileDiff::LINE_COMMON:
                leftContent  << " " << result.at(i).m_line;
                rightContent << " " << result.at(i).m_line;
                break;
            }
        }
     * @endcode
     */
    class _gsgsAPI FileDiff
    {
    public:
        static const int LINE_PLACEHOLDER = -2;
        static const int LINE_REMOVED = -1;
        static const int LINE_COMMON = 0;
        static const int LINE_ADDED = 1;

        struct _gsgsAPI LineInfo {
            int m_type;
            wxString m_line;
            LineInfo(const wxString &line, int type) : m_type(type), m_line(line) {}
            LineInfo() : m_type(LINE_PLACEHOLDER), m_line("\n") {}
        };
        typedef std::vector<LineInfo> LineInfoVec_t;
        typedef std::vector<std::pair<int, int> > SeqLinePair_t;

        enum DiffMode {
            kTwoPanes = 0x01,
            kOnePane = 0x02
        };
    public:
        FileDiff();
        virtual ~FileDiff();

        /**
         * @brief "diff" two files and store the result in the m_result member
         * When 2 files are identical, the result is empty
         */
        void Diff(const wxFileName& fnLeft, const wxFileName& fnRight, DiffMode mode);

        LineInfoVec_t& GetResultLeft()                  { return m_resultLeft; }

        LineInfoVec_t& GetResultRight()                 { return m_resultRight; }

        const SeqLinePair_t& GetSequences() const       { return m_sequences; }
    private:
        LineInfoVec_t m_resultLeft;
        LineInfoVec_t m_resultRight;
        SeqLinePair_t m_sequences;
    };

    class _gsgsAPI FileDiffPanel : public wxPanel
    {
        enum {
            ID_COPY_LEFT_TO_RIGHT = wxID_HIGHEST + 1,
            ID_COPY_LEFT_TO_RIGHT_AND_MOVE,
            ID_COPY_RIGHT_TO_LEFT,
            ID_COPY_RIGHT_TO_LEFT_AND_MOVE,
        };

        typedef std::vector<int> Markers_t;
    public:
        struct FileInfo {
            wxFileName filename;
            wxString title;
            bool readOnly;
            bool deleteOnExit;

            FileInfo(const wxFileName& fn, const wxString& caption, bool ro)
                : filename(fn)
                , title(caption)
                , readOnly(ro)
                , deleteOnExit(false)
            {
            }
            FileInfo()
                : readOnly(true)
                , deleteOnExit(false)
            {
            }
        };

        enum {
            kDeleteLeftOnExit = (1 << 0),
            kDeleteRightOnExit = (1 << 1),
            kLeftReadOnly = (1 << 2),
            kRightReadOnly = (1 << 3),
            kOriginSourceControl = (1 << 4),
            kSavePaths = (1 << 5),
        };
    public:
        FileDiffPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1, -1), long style = wxTAB_TRAVERSAL);
        virtual ~FileDiffPanel();
        
        wxTextCtrl* GetTextCtrlLeftFile() { return m_textCtrlLeftFile; }
        Button* GetButton290() { return m_button290; }
        wxStyledTextCtrl* GetStcLeft() { return m_stcLeft; }
        //wxPanel* GetPanelOverviewL() { return m_panelOverviewL; }
        //wxStaticText* GetStaticTextLeft() { return m_staticTextLeft; }
        wxPanel* GetSplitterPageLeft() { return m_splitterPageLeft; }
        wxTextCtrl* GetTextCtrlRightFile() { return m_textCtrlRightFile; }
        Button* GetButton294() { return m_button294; }
        wxStyledTextCtrl* GetStcRight() { return m_stcRight; }
        //wxPanel* GetPanelOverviewR() { return m_panelOverviewR; }
        //wxStaticText* GetStaticTextRight() { return m_staticTextRight; }
        wxPanel* GetSplitterPageRight() { return m_splitterPageRight; }
        //SplitterWindow* GetSplitter() { return m_splitter; }
        //wxPanel* GetPanelOverviewFull() { return m_panelOverviewFull; }

        void DoLayout();
        /**
         * @brief display a diff view for 2 files left and right
         */
        void Diff();

        /**
         * @brief mark the current diff origin from source control
         */
        void SetOriginSourceControl()           { m_flags |= kOriginSourceControl; }

        /**
         * @brief start a new empty diff
         */
        void DiffNew();

        /**
         * @brief start a new diff for two input files
         */
        void DiffNew(const wxFileName& left, const wxFileName& right);

        /**
         * @brief set the initial files to diff
         * Once set, you should call Diff() function
         */
        void SetFilesDetails(const FileDiffPanel::FileInfo& leftFile, const FileDiffPanel::FileInfo& rightFile);

        /**
         * @brief set whether to store the diff's filepaths for later reload
         */
        void SetSaveFilepaths(bool save)        { m_storeFilepaths = save; }
    public:
        //virtual void OnRefreshDiffUI(wxUpdateUIEvent& event);
        virtual void OnHorizontal(wxCommandEvent& event);
        virtual void OnVertical(wxCommandEvent& event);
        virtual void OnCopyFileFromRight(wxCommandEvent& event);
        virtual void OnCopyFileLeftToRight(wxCommandEvent& event);
        virtual void OnSaveChanges(wxCommandEvent& event);
        virtual void OnFind(wxCommandEvent& event);
        virtual void OnSaveChangesUI(wxUpdateUIEvent& event);
        virtual void OnCopyLeftToRight(wxCommandEvent& event);
        virtual void OnCopyRightToLeft(wxCommandEvent& event);
        virtual void OnCopyLeftToRightUI(wxUpdateUIEvent& event);
        virtual void OnCopyRightToLeftUI(wxUpdateUIEvent& event);
        virtual void OnNextDiffUI(wxUpdateUIEvent& event);
        virtual void OnPrevDiffUI(wxUpdateUIEvent& event);
        virtual void OnNextDiffSequence(wxCommandEvent& event);
        virtual void OnPrevDiffSequence(wxCommandEvent& event);
        virtual void OnRefreshDiff(wxCommandEvent& event);
        virtual void OnLeftStcPainted(wxScrollEvent& event);
        virtual void OnRightStcPainted(wxScrollEvent& event);
        virtual void OnIgnoreWhitespaceClicked(wxCommandEvent& event);
        virtual void OnIgnoreWhitespaceUI(wxUpdateUIEvent& event);
        virtual void OnShowLinenosClicked(wxCommandEvent& event);
        virtual void OnShowLinenosUI(wxUpdateUIEvent& event);
        virtual void OnShowOverviewBarClicked(wxCommandEvent& event);
        virtual void OnShowOverviewBarUI(wxUpdateUIEvent& event);
        virtual void OnPaint(wxPaintEvent& event);
        void OnPageClosing(wxNotifyEvent& event);

        void PrepareViews();
        void UpdateViews(const wxString& left, const wxString& right);
        void DoClean();
        void DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl);
        void DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
        void DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to);
        void DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos, int& placeHolderMarkerFirstLine,
            int& placeHolderMarkerLastLine);
        void DoGetLinesToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos);
        void DoSave(wxStyledTextCtrl* stc, const wxFileName& fn);

        bool CanNextDiff();
        bool CanPrevDiff();
        void DefineMarkers(wxStyledTextCtrl* ctrl);
    protected:
        virtual void OnBrowseLeftFile(wxCommandEvent& event);
        virtual void OnBrowseRightFile(wxCommandEvent& event);
        virtual void OnMouseWheel(wxMouseEvent& event);
        virtual void OnSingleView(wxCommandEvent& event);
        virtual void OnLeftPickerUI(wxUpdateUIEvent& event);
        virtual void OnRightPickerUI(wxUpdateUIEvent& event);
        virtual void OnPaneloverviewLeftDown(wxMouseEvent& event);

        void OnMenuCopyLeft2Right(wxCommandEvent& event);
        void OnMenuCopyRight2Left(wxCommandEvent& event);
        void OnCopyAllMenu(wxCommandEvent& event);
        void OnViewMenu(wxCommandEvent& event);
        void OnPreferences(wxCommandEvent& event);
    protected:
        wxString DoGetContentNoPlaceholders(wxStyledTextCtrl* stc) const;
        bool IsLeftReadOnly() const             { return m_flags & kLeftReadOnly; }
        bool IsRightReadOnly() const            { return m_flags & kRightReadOnly; }
        bool IsDeleteLeftOnExit() const         { return m_flags & kDeleteLeftOnExit; }
        bool IsDeleteRightOnExit() const        { return m_flags & kDeleteRightOnExit; }
        bool IsOriginSourceControl() const      { return m_flags & kOriginSourceControl; }
    protected:
        Markers_t m_leftRedMarkers;
        Markers_t m_leftGreenMarkers;
        Markers_t m_leftPlaceholdersMarkers;

        Markers_t m_rightGreenMarkers;
        Markers_t m_rightRedMarkers;
        Markers_t m_rightPlaceholdersMarkers;

        wxArrayInt m_overviewPanelMarkers;

        std::vector<std::pair<int, int> > m_sequences; // start-line - end-line pairs
        int m_cur_sequence;

        size_t m_flags;
        DiffConfig m_config;
        ToolBar* m_toolbar;
        QuickFindBar* m_findBar = nullptr;
        SplitterWindow* m_splitter;
        wxPanel* m_splitterPageLeft;
        wxTextCtrl* m_textCtrlLeftFile;
        Button* m_button290;
        wxStyledTextCtrl* m_stcLeft;
        wxPanel* m_panelOverviewL;
        //wxStaticText* m_staticTextLeft;
        wxPanel* m_splitterPageRight;
        wxTextCtrl* m_textCtrlRightFile;
        Button* m_button294;
        wxStyledTextCtrl* m_stcRight;
        //wxStaticText* m_staticTextRight;
        wxPanel* m_panelOverviewFull;
    
        bool m_storeFilepaths;
        bool m_darkTheme;
    };

    class _gsgsAPI FileDiffFrame : public wxFrame
    {
    public:
        FileDiffFrame(wxWindow* parent, const FileDiffPanel::FileInfo& left, const FileDiffPanel::FileInfo& right, bool originSourceControl);
        FileDiffFrame(wxWindow* parent);
        FileDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile);
        virtual ~FileDiffFrame();
    protected:
        void CreateMenuBar();
        void OnClose(wxCommandEvent& event);
    protected:
        FileDiffPanel* m_diffView = nullptr;
    };

    struct _gsgsAPI DiffViewEntry
    {
    public:
        inline bool IsExistsInLeft() const          { return m_existsInLeft; }
        inline bool IsExistsInRight() const         { return m_existsInRight; }
        inline bool IsExistsInBoth() const          { return m_existsInRight && m_existsInLeft; }
        inline bool IsOK() const                    { return !m_left.fullpath.empty() || !m_right.fullpath.empty(); }
        inline int GetImageId(bool left) const      { return GetImageId(left ? m_left : m_right); }

        inline wxString GetFullName() const
        {
            if (m_existsInLeft) {
                return wxFileName(GetLeft().fullpath).GetFullName();
            }
            else {
                return wxFileName(GetRight().fullpath).GetFullName();
            }
        }

        inline void SetLeft(const FileScan::EntryData& left)
        {
            this->m_left = left;
            m_existsInLeft = true;
        }
        inline void SetRight(const FileScan::EntryData& right)
        {
            this->m_right = right;
            m_existsInRight = true;
        }
        inline const FileScan::EntryData& GetLeft() const { return m_left; }
        inline const FileScan::EntryData& GetRight() const { return m_right; }
        typedef std::vector<DiffViewEntry> Vect_t;
        typedef std::unordered_map<wxString, DiffViewEntry> Hash_t;
    private:
        int GetImageId(const FileScan::EntryData& d) const
        {
            if (d.flags & FileScan::kIsFolder) {
                return gsgs_Image().GetMimeImageId(FileUtil::TypeFolder);
            }
            else {
                return gsgs_Image().GetMimeImageId(d.fullpath);
            }
        }
    protected:
        FileScan::EntryData m_left;
        FileScan::EntryData m_right;
        bool m_existsInLeft = false;
        bool m_existsInRight = false;
    };

    struct _gsgsAPI DiffView
    {
    public:
        inline void CreateEntry(const FileScan::EntryData& d, bool left)
        {
            DiffViewEntry entry;
            if (left) {
                entry.SetLeft(d);
            }
            else {
                entry.SetRight(d);
            }
            m_table.insert({ entry.GetFullName(), entry });
        }

        inline DiffViewEntry& GetEntry(const wxString& filename)
        {
            static DiffViewEntry nullEntry;
            if (HasFile(filename)) 
            { 
                return m_table[filename]; 
            }
            return nullEntry;
        }

        inline bool HasFile(const wxString& filename) const { return m_table.count(filename); }
        inline DiffViewEntry::Hash_t& GetTable()            { return m_table; }
        inline DiffViewEntry::Vect_t ToSortedVector() const
        {
            DiffViewEntry::Vect_t V;
            std::for_each(m_table.begin(), m_table.end(),
                [&](const DiffViewEntry::Hash_t::value_type& vt) { V.push_back(vt.second); });
            // sort the vector
            std::sort(V.begin(), V.end(), [&](const DiffViewEntry& a, const DiffViewEntry& b) {
                return a.GetFullName().CmpNoCase(b.GetFullName()) < 0;
            });
            return V;
        }
    protected:
        DiffViewEntry::Hash_t m_table;
    };

    class _gsgsAPI DiffFoldersFrame : public wxDialog
    {
    public:
        DiffFoldersFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Diff Folders"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize, long style = gsgsDlgStyle);

        virtual ~DiffFoldersFrame();
        void OnChecksum(int callId, const wxArrayString& checksumArray);

        ToolBar* GetToolbar() { return m_toolbar; }
        ThemedListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
        wxPanel* GetPanel4() { return m_panel4; }
    protected:
        void BuildTrees(const wxString& left, const wxString& right);
        void DoOpenDiff(const wxDataViewItem& item);
        void StopChecksumThread();
        bool CanUp() const;

    protected:
        virtual void OnClose(wxCommandEvent& event);
        virtual void OnNewCmparison(wxCommandEvent& event);
        virtual void OnItemActivated(wxDataViewEvent& event);
        virtual void OnItemContextMenu(wxDataViewEvent& event);

        void OnMenuDiff(wxCommandEvent& event);
        void OnCopyToRight(wxCommandEvent& event);
        void OnCopyToLeft(wxCommandEvent& event);
        void OnShowSimilarFiles(wxCommandEvent& event);
        void OnShowSimilarFilesUI(wxUpdateUIEvent& event);
        void OnRefresh(wxCommandEvent& event);
        void OnRefreshUI(wxUpdateUIEvent& event);
        void OnUpFolder(wxCommandEvent& event);
        void OnUpFolderUI(wxUpdateUIEvent& event);
    private:
        wxPanel* m_panel4;
        ToolBar* m_toolbar;
        ThemedListCtrl* m_dvListCtrl;

        wxString m_leftFolder;
        wxString m_rightFolder;
        std::thread* m_checksumThread = nullptr;
        DiffViewEntry::Vect_t m_entries;
        size_t m_depth = 0;
        bool m_showSimilarItems = false;
    };
}
#endif
