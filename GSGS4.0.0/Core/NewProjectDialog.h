#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include "gsgsPreInclude.h"
#include "gsgsWindowManager.h"
#include "wxCodeCompletionBoxEntry.h"
#include "gsgsFileUtil.h"
#include "gsgsImageManager.h"
#include "gsgsTreeCtrl.h"
#include "gsgsGotoEntryManager.h"
#include "gsgsToolsetManager.h"
#include "LSP/CompletionItem.h"
#include "project.h"
#include <wx/timer.h>
#include <wx/bitmap.h>
#include <wx/stc/stc.h>
#include <wx/font.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/popupwin.h>
#include <wx/checkbox.h>
#include <wx/bannerwindow.h>
#include <wx/commandlinkbutton.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/xml/xml.h>
namespace gsgs
{
    class _gsgsAPI NewProjectDialog : public wxDialog
    {
    public:
        NewProjectDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Project"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
            long style = gsgsDlgStyle);
        virtual ~NewProjectDialog();
        ProjectData GetProjectData() const;

        wxStaticText* GetStaticText12()     { return m_staticText12; }
        wxTextCtrl* GetTextCtrlName()       { return m_textCtrlName; }
        wxStaticText* GetStaticText20()     { return m_staticText20; }
        wxDirPickerCtrl* GetDirPicker()     { return m_dirPicker; }
        wxStaticText* GetStaticText16()     { return m_staticText16; }
        wxChoice* GetChoiceType()           { return m_choiceType; }
        wxStaticText* GetStaticText24()     { return m_staticText24; }
        wxChoice* GetChoiceCompiler()       { return m_choiceCompiler; }
        wxStaticText* GetStaticText28()     { return m_staticText28; }
        wxChoice* GetChoiceDebugger()       { return m_choiceDebugger; }
        wxStaticText* GetStaticText32()     { return m_staticText32; }
        wxChoice* GetChoiceBuild()          { return m_choiceBuild; }
    protected:
        virtual void OnOKUI(wxUpdateUIEvent& event);
    protected:
        wxStaticText* m_staticText12;
        wxTextCtrl* m_textCtrlName;
        wxStaticText* m_staticText20;
        wxDirPickerCtrl* m_dirPicker;
        wxStaticText* m_staticText16;
        wxChoice* m_choiceType;
        wxStaticText* m_staticText24;
        wxChoice* m_choiceCompiler;
        wxStaticText* m_staticText28;
        wxChoice* m_choiceDebugger;
        wxStaticText* m_staticText32;
        wxChoice* m_choiceBuild;
        DialogButtonSizer* m_stdBtnSizer4;
        Button* m_button6;
        Button* m_button8;

        std::list<ProjectPtr> m_list;
        ProjectData m_projectData;
        std::unordered_map<int, ProjectPtr> m_projectsMap;
    };

    class _gsgsAPI NewKeyShortcutDlg : public wxDialog
    {
    public:
        enum {
            ID_SUGGEST = 10001,
        };

        // On OSX, kCtrl => CMD and kRawCtrl => Actual Ctrl key
        enum { kAlt = (1 << 0), kCtrl = (1 << 1), kShift = (1 << 2) };

        struct KeyboardShortcut
        {
            size_t modifiers;
            wxString key;

            KeyboardShortcut()
                : modifiers(0)
            {
            }
        };
    public:
        NewKeyShortcutDlg(wxWindow* parent, const WindowManager::HotKey& mid, wxWindowID id = wxID_ANY,
            const wxString& title = _("Set New Key Accelerator"), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1, -1), long style = gsgsDlgStyle);
        virtual ~NewKeyShortcutDlg();
        wxString GetAccel() const;

        wxStaticText* GetStaticTextActionLabel() { return m_staticTextActionLabel; }
        wxStaticText* GetStaticTextAction() { return m_staticTextAction; }
        wxStaticText* GetStaticText10() { return m_staticText10; }
        wxTextCtrl* GetTextCtrl1() { return m_textCtrl1; }
        wxStaticText* GetStaticText12() { return m_staticText12; }
        wxCheckBox* GetCheckBoxCtrl() { return m_checkBoxCtrl; }
        wxCheckBox* GetCheckBoxAlt() { return m_checkBoxAlt; }
        wxCheckBox* GetCheckBoxShift() { return m_checkBoxShift; }
        Button* GetButtonClear() { return m_buttonClear; }
        Button* GetButton30() { return m_button30; }
        wxStaticLine* GetStaticLine39() { return m_staticLine39; }
        wxPanel* GetPanel1() { return m_panel1; }
    protected:
        virtual void OnSuggest(wxCommandEvent& event);
        virtual void OnClear(wxCommandEvent& event);
        virtual void OnClearUI(wxUpdateUIEvent& event);

        void OnKeyDown(wxKeyEvent& event);
        wxString ToString(wxKeyEvent& e);
        NewKeyShortcutDlg::KeyboardShortcut FromString(const wxString& accelString);

        void Initialise(const wxString& accel);
    protected:
        wxPanel* m_panel1;
        wxStaticText* m_staticTextActionLabel;
        wxStaticText* m_staticTextAction;
        wxStaticText* m_staticText10;
        wxTextCtrl* m_textCtrl1;
        wxStaticText* m_staticText12;
        wxCheckBox* m_checkBoxCtrl;
        wxCheckBox* m_checkBoxAlt;
        wxCheckBox* m_checkBoxShift;
        Button* m_buttonClear;
        Button* m_button30;
        wxStaticLine* m_staticLine39;
        DialogButtonSizer* m_stdBtnSizer22;
        Button* m_button24;
        Button* m_button26;
        WindowManager::HotKey m_mid;
    };

    class BoxTipWindow;
    class CodeBoxManager;

    class _gsgsAPI CodeBox : public wxPopupWindow
    {
        friend class CodeBoxManager;
    public:
        typedef std::vector<wxBitmap> BmpVec_t;

        enum eOptions {
            kNone = 0,
            kInsertSingleMatch = (1 << 0),
            kRefreshOnKeyType = (1 << 1),
            kNoShowingEvent = (1 << 2), // Dont send the wxEVT_CCBOX_SHOWING event
        };
    public:
        /**
         * @brief return the bitamp associated with this tag entry
         */
        static wxBitmap GetBitmap(TagEntryPtr tag);

        /**
         * @brief construct a code completion box
         * @param parent the parent window
         * @param eventObject will be passed as the eventObject of the various CodeCompleteEvent
         * events fired from this object
         */
        CodeBox(wxWindow* parent, wxEvtHandler* eventObject = NULL, size_t flags = 0);

        virtual ~CodeBox();

        wxPanel* GetCanvas()            { return m_canvas; }

        wxPanel* GetPanelComposite()    { return m_panelComposite; }

        /**
         * @brief show the completion box
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries);

        /**
         * @brief show the completion box
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl, const TagEntryPtrList& tags);

        /**
         * @brief show the completion box  (Language Server Protocol support)
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl, const LSP::CompletionItem::Vec_t& completions);

        void SetBitmaps(const CodeBox::BmpVec_t& bitmaps) { this->m_bitmaps = bitmaps; }
        const CodeBox::BmpVec_t& GetBitmaps() const { return m_bitmaps; }

        /**
         * @brief convert TagEntryPtr into Code Completion entry
         */
        static wxCodeCompletionBoxEntry::Ptr_t TagToEntry(TagEntryPtr tag);

        /// accessors
        void SetFlags(size_t flags) { this->m_flags = flags; }
        size_t GetFlags() const { return m_flags; }

        void SetStartPos(int startPos) { this->m_startPos = startPos; }
        int GetStartPos() const { return m_startPos; }

        void ScrollDown() { DoScrollDown(); }
        void ScrollUp() { DoScrollUp(); }

        void DoMouseScroll(wxMouseEvent& event);
    protected:
        void StcKeyDown(wxKeyEvent& event);
        void StcLeftDown(wxMouseEvent& event);
        void StcModified(wxStyledTextEvent& event);
        void StcCharAdded(wxStyledTextEvent& event);

        // Event handlers
        void OnLeftDClick(wxMouseEvent& event);
        void OnMouseScroll(wxMouseEvent& event);
        void OnEraseBackground(wxEraseEvent& event);
        void OnPaint(wxPaintEvent& event);

        static void InitializeDefaultBitmaps();
        void DoPgUp();
        void DoPgDown();
    protected:
        int GetSingleLineHeight() const;
        /**
         * @brief filter the results based on what the user typed in the editor
         * @return Should we refresh the content of the CC box (based on number of "Exact matches" / "Starts with" found)
         */
        bool FilterResults();
        void RemoveDuplicateEntries();
        void InsertSelection();
        wxString GetFilter();

        // For backward compatibility, we support initializing the list with TagEntryPtrList
        // These 2 functions provide conversion between wxCodeCompletionBoxEntry and TagEntryPtr
        wxCodeCompletionBoxEntry::Vec_t TagsToEntries(const TagEntryPtrList& tags);
        wxCodeCompletionBoxEntry::Vec_t LSPCompletionsToEntries(const LSP::CompletionItem::Vec_t& completions);
        static int GetImageId(TagEntryPtr entry);
        int GetImageId(LSP::CompletionItem::Ptr_t entry) const;
        void DoDisplayTipWindow();
        void DoDestroyTipWindow();

        void DoUpdateList();
        void DoScrollDown();
        void DoScrollUp();
        void DoDestroy();
        void DoDrawBottomScrollButton(wxDC& dc);
        void DoDrawTopScrollButton(wxDC& dc);
        bool CanScrollDown();
        bool CanScrollUp();
        void DoShowCompletionBox();
    protected:
        wxBoxSizer* mainSizer;
        wxPanel* m_panelComposite;
        wxBoxSizer* boxSizer;
        wxPanel* m_canvas;

        wxCodeCompletionBoxEntry::Vec_t m_allEntries;
        wxCodeCompletionBoxEntry::Vec_t m_entries;
        CodeBox::BmpVec_t m_bitmaps;
        static CodeBox::BmpVec_t m_defaultBitmaps;
        std::unordered_map<int, int> m_lspCompletionItemImageIndexMap;
        int m_index;
        wxString m_displayedTip;
        wxStyledTextCtrl* m_stc;
        wxFont m_ccFont;
        int m_startPos;
        bool m_useLightColours;

        /// When firing the various "CodeCompleteEvent"s, set the event object
        /// to this member. This help distinguish the real trigger of the code completion
        /// box
        wxEvtHandler* m_eventObject;

        /// Code completion tooltip that is shown next to the code completion box
        BoxTipWindow* m_tipWindow;

        /// The code completion box flags, see above enum for possible values
        size_t m_flags;

        /// Contains the scrollbar rectangle
        wxRect m_scrollArea;
        wxRect m_scrollTopRect;
        wxRect m_scrollBottomRect;

        /// Colours used by this class
        wxColour m_penColour;
        wxColour m_bgColour;
        wxColour m_separatorColour;
        wxColour m_textColour;
        wxColour m_selectedTextColour;
        wxColour m_selectedTextBgColour;
        wxColour m_alternateRowColour;

        /// Scrollbar bitmaps
        wxBitmap m_bmpUp;
        wxBitmap m_bmpDown;
        wxBitmap m_bmpUpEnabled;
        wxBitmap m_bmpDownEnabled;
        int m_lineHeight = 0;
    };

    class _gsgsAPI NameAndDescDlg : public wxDialog
    {
    public:
        /** Constructor */
        NameAndDescDlg(wxWindow* parent, IManager* manager, const wxString& name, 
            wxWindowID id = wxID_ANY, const wxString& title = _("Save Project As Template"), 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), 
                long style = gsgsDlgStyle);

        wxString GetName() const { return m_textCtrlName->GetValue(); }
        wxString GetDescription() const { return m_textCtrlDescription->GetValue(); }
        wxString GetType() const { return m_choiceType->GetValue(); }

        wxStaticText* GetStaticText3() { return m_staticText3; }
        wxTextCtrl* GetTextCtrlName() { return m_textCtrlName; }
        wxStaticText* GetStaticText4() { return m_staticText4; }
        wxComboBox* GetChoiceType() { return m_choiceType; }
        wxStaticText* GetStaticText2() { return m_staticText2; }
        wxTextCtrl* GetTextCtrlDescription() { return m_textCtrlDescription; }
    private:
        wxBoxSizer* bSizer1;
        wxFlexGridSizer* fgSizer1;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_textCtrlName;
        wxStaticText* m_staticText4;
        wxComboBox* m_choiceType;
        wxBoxSizer* bSizer3;
        wxStaticText* m_staticText2;
        wxTextCtrl* m_textCtrlDescription;
        DialogButtonSizer* m_stdBtnSizer2;
        Button* m_buttonOK;
        Button* m_button6;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TOOLTIP_DESTROY, Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TOOLTIP_ITEM_EXPANDING, Event);

    class _gsgsAPI ResizableTooltip : public wxMiniFrame
    {
    public:
        ResizableTooltip(wxEvtHandler* owner);
        virtual ~ResizableTooltip();
        
        DefaultTreeCtrl* GetTreeCtrl()      { return m_treeCtrl; }

        /**
         * @brief return the event owner for this tooltip
         * @return
         */
        wxEvtHandler* GetOwner()            { return m_owner; }

        /**
         * @brief delete all items in the tree control
         */
        virtual void Clear();

        /**
         * @brief move the tooltip to the mouse position and show it
         */
        virtual void ShowTip();

    protected:
        void DoSetFocus();

        virtual void OnKeyDown(wxTreeEvent& event);
        /**
         * @brief user is expanding a tree item
         * @param event
         */
        virtual void OnItemExpanding(wxTreeEvent& event);
    protected:
        DefaultTreeCtrl* m_treeCtrl;
        wxEvtHandler* m_owner;
        wxPoint m_topLeft;
        bool m_dragging;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_SYMBOL_SELECTED, Event);

    struct _gsgsAPI SelectSymbolDialogEntry {
        wxString name;
        wxBitmap bmp;
        wxString help;
        wxClientData* clientData;

        SelectSymbolDialogEntry()
            : bmp(wxNullBitmap)
            , clientData(NULL)
        {
        }

        typedef std::list<SelectSymbolDialogEntry> List_t;
    };

    class _gsgsAPI SelectSymbolDialog : public wxDialog
    {
    public:
        SelectSymbolDialog(wxWindow* parent, const SelectSymbolDialogEntry::List_t& entries);
        virtual ~SelectSymbolDialog();

        wxDataViewListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    protected:
        void AddSymbol(const wxString& name,
            const wxBitmap& bmp = wxNullBitmap,
            const wxString& help = "",
            wxClientData* clientData = NULL);
    public:
        /**
         * @brief return the selected item client data
         */
        wxClientData* GetSelection() const;
    protected:
        virtual void OnItemActivated(wxDataViewEvent& event);
        virtual void OnOKUI(wxUpdateUIEvent& event);
    protected:
        wxBoxSizer* boxSizer2;
        wxBoxSizer* boxSizer10;
        wxDataViewListCtrl* m_dvListCtrl;
        DialogButtonSizer* m_stdBtnSizer4;
        Button* m_buttonOK;
        Button* m_buttonCancel;
    };

    /** Implementing MemoryViewBase */
    class _gsgsAPI MemoryView : public wxPanel
    {
    public:
        MemoryView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize(-1,-1), long style = wxTAB_TRAVERSAL);
        virtual ~MemoryView();
                
        void Clear();
        
        wxString GetExpression() const { return m_textCtrlExpression->GetValue(); }
        
        size_t GetSize() const;
        
        size_t GetColumns() const;
        
        void SetViewString(const wxString & text);
        
        wxStaticText* GetStaticText2() { return m_staticText2; }
        wxTextCtrl* GetTextCtrlExpression() { return m_textCtrlExpression; }
        wxStaticText* GetStaticText1() { return m_staticText1; }
        wxChoice* GetChoiceSize() { return m_choiceSize; }
        wxStaticText* GetStaticText8() { return m_staticText8; }
        wxChoice* GetChoiceCols() { return m_choiceCols; }
        wxTextCtrl* GetTextCtrlMemory() { return m_textCtrlMemory; }
        Button* GetButtonEvaluate() { return m_buttonEvaluate; }
        Button* GetButtonUpdate() { return m_buttonUpdate; }
    protected:
        virtual void OnTextDClick(wxMouseEvent& event);
        virtual void OnTextEntered(wxCommandEvent& event);
        virtual void OnMemorySize(wxCommandEvent& event);
        virtual void OnNumberOfRows(wxCommandEvent& event);
        // Handlers for MemoryViewBase events.
        void OnEvaluate(wxCommandEvent& event);
        void OnEvaluateUI(wxUpdateUIEvent& event);
        void OnUpdate(wxCommandEvent& e);
        void OnUpdateUI(wxUpdateUIEvent& event);

        void UpdateDebuggerPane();
    protected:
        wxStaticText* m_staticText2;
        wxTextCtrl* m_textCtrlExpression;
        wxStaticText* m_staticText1;
        wxChoice* m_choiceSize;
        wxStaticText* m_staticText8;
        wxChoice* m_choiceCols;
        wxTextCtrl* m_textCtrlMemory;
        Button* m_buttonEvaluate;
        Button* m_buttonUpdate;
    };

    class _gsgsAPI OpenResourceDialogItemData : public wxClientData
    {
    public:
        OpenResourceDialogItemData()
            : m_file(wxT(""))
            , m_line(wxNOT_FOUND)
            , m_pattern(wxT(""))
            , m_name(wxT(""))
            , m_scope(wxT(""))
            , m_impl(false)
        {
        }

        OpenResourceDialogItemData(const wxString& file, int line, const wxString& pattern, const wxString& name,
            const wxString& scope)
            : m_file(file)
            , m_line(line)
            , m_pattern(pattern)
            , m_name(name)
            , m_scope(scope)
            , m_impl(false)
        {
        }

        virtual ~OpenResourceDialogItemData() {}

        bool IsOk() const;
    public:
        wxString m_file;
        int m_line;
        wxString m_pattern;
        wxString m_name;
        wxString m_scope;
        bool m_impl;
    };

    class _gsgsAPI OpenResourceDialog : public wxDialog
    {
    public:
        /** Constructor */
        OpenResourceDialog(wxWindow* parent, IManager* manager, const wxString& initialSelection, 
            wxWindowID id = wxID_ANY, const wxString& title = _("Open Resource"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
                long style = gsgsDlgStyle);
        virtual ~OpenResourceDialog();

        std::vector<OpenResourceDialogItemData*> GetSelections() const;
        wxArrayString& GetFilters() { return m_filters; }

        wxTextCtrl* GetTextCtrlResourceName()   { return m_textCtrlResourceName; }
        ThemedListCtrl* GetDataview()           { return m_dataview; }
        wxCheckBox* GetCheckBoxFiles()          { return m_checkBoxFiles; }
        wxCheckBox* GetCheckBoxShowSymbols()    { return m_checkBoxShowSymbols; }

        /**
         * \brief helper method for opening the selection
         * \param selection
         * \return
         */
        static void OpenSelection(const OpenResourceDialogItemData& selection, IManager* manager);
    protected:
        void DoPopulateList();
        void DoPopulateWorkspaceFile();
        bool MatchesFilter(const wxString& name);
        void DoPopulateTags();
        void DoSelectItem(const wxDataViewItem& item);
        void Clear();
        void DoAppendLine(const wxString& name, const wxString& fullname, bool boldFont,
            OpenResourceDialogItemData* clientData, int imgid);
        int DoGetTagImg(TagEntryPtr tag);
        OpenResourceDialogItemData* GetItemData(const wxDataViewItem& item) const;

    protected:
        virtual void OnEnter(wxCommandEvent& event);
        virtual void OnEntrySelected(wxDataViewEvent& event);
        virtual void OnEntryActivated(wxDataViewEvent& event);
        virtual void OnCheckboxfilesCheckboxClicked(wxCommandEvent& event);
        virtual void OnCheckboxshowsymbolsCheckboxClicked(wxCommandEvent& event);

        // Handlers for OpenResourceDialogBase events.
        void OnText(wxCommandEvent& event);
        void OnUsePartialMatching(wxCommandEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnOK(wxCommandEvent& event);
        void OnOKUI(wxUpdateUIEvent& event);
        void OnTimer(wxTimerEvent& event);

        DECLARE_EVENT_TABLE()

        void GetLineNumberFromFilter(const wxString& filter, wxString& modFilter, long& lineNumber);
    protected:
        wxTextCtrl* m_textCtrlResourceName;
        ThemedListCtrl* m_dataview;
        wxCheckBox* m_checkBoxFiles;
        wxCheckBox* m_checkBoxShowSymbols;
        DialogButtonSizer* m_stdBtnSizer2;
        Button* m_buttonOK;
        Button* m_button6;
        IManager* m_manager;
        std::unordered_multimap<wxString, wxString> m_files;
        std::unordered_map<wxString, int> m_fileTypeHash;
        wxTimer* m_timer;
        bool m_needRefresh;
        wxArrayString m_filters;
        wxArrayString m_userFilters;
        long m_lineNumber;
    };

    /** Implementing MacrosBaseDlg */
    class _gsgsAPI MacrosDlg : public wxDialog
    {
    public:
        enum { MacrosProject, MacrosCompiler, MacrosExternalTools };
    public:
        /** Constructor */
        MacrosDlg(wxWindow* parent, int content, ProjectPtr project, IEditor* editor,
            wxWindowID id = wxID_ANY, const wxString& title = _("Available Macros"), 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), 
            long style = gsgsDlgStyle);
        virtual ~MacrosDlg();

        wxListCtrl* GetListCtrlMacros() { return m_listCtrlMacros; }
    protected:
        // Handlers for MacrosBaseDlg events.
        void OnItemRightClick(wxListEvent& event);
        void OnCopy(wxCommandEvent& e);
        void Initialize();
        void AddMacro(const wxString& name, const wxString& desc);

        DECLARE_EVENT_TABLE()
    private:
        wxListCtrl* m_listCtrlMacros;
        DialogButtonSizer* m_stdBtnSizer2;
        Button* m_button4;
        Button* m_button6;
        long m_item;
        int m_content;
        ProjectPtr m_project;
        IEditor* m_editor;
    };

    class CxxWS;

    /** Implementing VirtualDirectorySelectorBase */
    class _gsgsAPI VirtualDirectorySelectorDlg : public wxDialog
    {
    public:
        /** Constructor */
        VirtualDirectorySelectorDlg(wxWindow* parent, CxxWS* wsp, const wxString& initialPath = wxEmptyString,
            const wxString& projectname = wxEmptyString, wxWindowID id = wxID_ANY,
            const wxString& title = _("Virtual Directory Selector"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
            long style = gsgsDlgStyle);
        ~VirtualDirectorySelectorDlg();

        wxStaticText* GetStaticText1() { return m_staticText1; }
        DefaultTreeCtrl* GetTreeCtrl() { return m_treeCtrl; }
        wxStaticText* GetStaticTextPreview() { return m_staticTextPreview; }
        Button* GetButtonOk() { return m_buttonOk; }
        Button* GetButtonCancel() { return m_buttonCancel; }
        Button* GetButton1() { return m_button1; }

        wxString GetVirtualDirectoryPath() const { return m_staticTextPreview->GetLabel(); }
        bool SelectPath(const wxString& path);
        wxTreeItemId FindItemForPath(const wxString& path);
        void SetText(const wxString& text);
        void SetSuggestedName(const wxString& suggestedName) { m_suggestedName = suggestedName; }
    public:
        static wxString DoGetPath(TreeCtrl* tree, const wxTreeItemId& item, bool validateFolder);

    protected:
        virtual void OnNewVDUI(wxUpdateUIEvent& event);
        virtual void OnNewVD(wxCommandEvent& event);

        // Handlers for VirtualDirectorySelectorBase events.
        void OnItemSelected(wxTreeEvent& event);
        void OnButtonOK(wxCommandEvent& event);
        void OnButtonCancel(wxCommandEvent& event);
        void OnButtonOkUI(wxUpdateUIEvent& event);

        void DoBuildTree();
    private:
        wxStaticText* m_staticText1;
        DefaultTreeCtrl* m_treeCtrl;
        wxStaticText* m_staticTextPreview;
        Button* m_buttonOk;
        Button* m_buttonCancel;
        Button* m_button1;
        CxxWS* m_workspace;
        wxString m_projectName;
        wxString m_initialPath;
        wxString m_suggestedName;
        bool m_reloadTreeNeeded;
    };

    class _gsgsAPI EnvironmentVariablesDlg : public wxDialog
    {
    public:
        EnvironmentVariablesDlg(wxWindow* parent, wxWindowID id = wxID_ANY, 
            const wxString& title = _("Environment Variables"), const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = gsgsMinDlgSize, long style = gsgsDlgStyle);
        virtual ~EnvironmentVariablesDlg();

        wxStaticText* GetStaticText1() { return m_staticText1; }
        wxStaticText* GetStaticText2() { return m_staticText2; }
        wxStyledTextCtrl* GetTextCtrlDefault() { return m_textCtrlDefault; }
        wxPanel* GetPanel1() { return m_panel1; }
        Notebook* GetBook() { return m_book; }
        Button* GetButtonNewSet() { return m_buttonNewSet; }
        Button* GetButtonDeleteSet() { return m_buttonDeleteSet; }
        Button* GetButton5() { return m_button5; }
        Button* GetButton16() { return m_button16; }
        Button* GetButton14() { return m_button14; }
    protected:
        virtual void OnCancel(wxCommandEvent& event);
        virtual void OnClose(wxCloseEvent& event);

        virtual void OnButtonOk(wxCommandEvent& event);
        virtual void OnDeleteSet(wxCommandEvent& event);
        virtual void OnDeleteSetUI(wxUpdateUIEvent& event);
        virtual void OnExport(wxCommandEvent& event);
        virtual void OnNewSet(wxCommandEvent& event);

        void DoAddPage(const wxString& name, const wxString& content, bool select);
        void DoAddNewSet();
    private:
        EditEventsHandler m_editEventsHander;
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        Notebook* m_book;
        wxPanel* m_panel1;
        wxStyledTextCtrl* m_textCtrlDefault;
        Button* m_buttonNewSet;
        Button* m_buttonDeleteSet;
        Button* m_button5;
        Button* m_button16;
        Button* m_button14;
    };

    class _gsgsAPI GotoAnythingDlg : public wxDialog
    {
    public:
        GotoAnythingDlg(wxWindow* parent, const std::vector<GotoEntry>& entries, wxWindowID id = wxID_ANY,
            const wxString& title = _("Goto Anything"), const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxSize(-1, -1), long style = gsgsDlgStyle);
        virtual ~GotoAnythingDlg();

        wxTextCtrl* GetTextCtrlSearch() { return m_textCtrlSearch; }
        wxDataViewListCtrl* GetDvListCtrl() { return m_dvListCtrl; }
    protected:
        void DoPopulate(const std::vector<GotoEntry>& entries, const std::vector<int>& indexes = std::vector<int>());
        void DoExecuteActionAndClose();
        void UpdateLastSearch();
        void ApplyFilter();
    protected:
        virtual void OnItemActivated(wxDataViewEvent& event);
        virtual void OnEnter(wxCommandEvent& event);
        virtual void OnKeyDown(wxKeyEvent& event);
        void OnIdle(wxIdleEvent& e);
        void DoSelectItem(const wxDataViewItem& item);
    private:
        wxTextCtrl* m_textCtrlSearch;
        wxDataViewListCtrl* m_dvListCtrl;
        const std::vector<GotoEntry>& m_allEntries;
        wxString m_currentFilter;
    };

}

#endif // NEWPROJECTDIALOG_H
