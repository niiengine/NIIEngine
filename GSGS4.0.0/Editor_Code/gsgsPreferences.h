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

#ifndef _GSGSPREFERENCES_H
#define _GSGSPREFERENCES_H

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include "gsgsOptionsConfig.h"
#include "gsgsWindowManager.h"
#include "gsgsTreeCtrl.h"
#include "globals.h"
#include <wx/fontpicker.h>
#include <wx/dialog.h>
#include <wx/treebook.h>
#include <wx/checkbox.h>
#include <wx/srchctrl.h>
#include <wx/clrpicker.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/dataview.h>
#include <wx/radiobox.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>
#include <wx/propgrid/manager.h>

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

namespace gsgs
{
    class EditorOptionsGeneralGuidesPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorOptionsGeneralGuidesPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL);
        virtual ~EditorOptionsGeneralGuidesPanel();

        wxPropertyGridManager* GetPgMgrGeneral() const  { return m_pgMgrGeneral; }

        void Save(OptionsConfig * options);

        virtual void OnValueChanged(wxPropertyGridEvent& event);
    protected:
        wxPropertyGridManager* m_pgMgrGeneral;
        wxPGProperty* m_pgPropCategoryGeneral;
        wxPGProperty* m_pgPropDisplayLineNumbers;
        wxPGProperty* m_pgPropRelativeLineNumbers;
        wxPGProperty* m_pgPropHighlightMatchedBrace;
        wxPGProperty* m_pgPropHideEditMargin;
        wxPGProperty* m_pgPropShowIndentGuidelines;
        wxPGProperty* m_pgPropDisableSemiColonShift;
        wxPGProperty* m_pgPropWhitespaces;
        wxPGProperty* m_pgPropWhitespaceVisibility;
        wxPGProperty* m_pgPropEOLMode;
        wxPGProperty* m_pgPropCaretLine;
        wxPGProperty* m_pgPropEnableCaretLine;
        wxPGProperty* m_pgPropCaretLineColour;
        wxPGProperty* m_pgPropCaretLineAlpha;
        wxPGProperty* m_pgPropDebuggerMarker;
        wxPGProperty* m_pgPropHighlightDebuggerMarker;
        wxPGProperty* m_pgPropDebuggerLineColour;
        wxPGProperty* m_pgProp8;
        wxPGProperty* m_pgPropSmartCurly;
        wxPGProperty* m_pgPropSmartParentheses;
        wxPGProperty* m_pgPropSmartQuotes;
        wxPGProperty* m_pgPropCopyLineEmptySelection;
        wxPGProperty* m_pgProp16;
        wxPGProperty* m_pgPropWrapQuotes;
        wxPGProperty* m_pgPropWrapBrackets;
        wxPGProperty* m_pgProp56;
        wxPGProperty* m_pgPropZoomUsingCtrlScroll;
        wxPGProperty* m_pgProp565;
        wxPGProperty* m_pgPropCommentsIndented;
    };

    class EditorOptionsGeneralIndentationPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorOptionsGeneralIndentationPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
                long style = wxTAB_TRAVERSAL);
        ~EditorOptionsGeneralIndentationPanel();

        void Save(OptionsConfig * options);
    protected:
        wxCheckBox* m_checkBoxDisableSmartIdent;
        wxCheckBox* m_indentsUsesTabs;
        wxStaticText* m_staticText1;
        wxTextCtrl* m_indentWidth;
        wxStaticText* m_staticText2;
        wxTextCtrl* m_tabWidth;
    };

    class EditorOptionsGeneralRightMarginPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorOptionsGeneralRightMarginPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL);
        ~EditorOptionsGeneralRightMarginPanel();

        void Save(OptionsConfig * options);
    protected:
        // Virtual event handlers, overide them in your derived class
        virtual void OnRightMarginIndicator(wxCommandEvent& event);

        void EnableDisableRightMargin();
    protected:
        wxRadioButton* m_radioBtnRMDisabled;
        wxRadioButton* m_radioBtnRMLine;
        wxRadioButton* m_radioBtnRMBackground;
        wxStaticText* m_staticText1;
        wxTextCtrl* m_rightMarginColumn;
        wxStaticText* m_staticText2;
        wxColourPickerCtrl* m_rightMarginColour;
    };

    class EditorSettingsCaret : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsCaret(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsCaret();

        void Save(OptionsConfig * options);

        wxStaticText* GetStaticText1() const                    { return m_staticText1; }
        wxTextCtrl* GetSpinCtrlBlinkPeriod() const              { return m_spinCtrlBlinkPeriod; }
        wxStaticText* GetStaticText3() const                    { return m_staticText3; }
        wxTextCtrl* GetSpinCtrlCaretWidth() const               { return m_spinCtrlCaretWidth; }
        wxCheckBox* GetCheckBoxBlockCaret() const               { return m_checkBoxBlockCaret; }
        wxCheckBox* GetCheckBoxScrollBeyondLastLine() const     { return m_checkBoxScrollBeyondLastLine; }
        wxCheckBox* GetCheckBoxCaretUseCamelCase() const        { return m_checkBoxCaretUseCamelCase; }
        wxCheckBox* GetCheckBoxCaretOnVirtualSpace()const       { return m_checkBoxCaretOnVirtualSpace; }
        wxCheckBox* GetCheckBoxAdjustScrollbarSize()const       { return m_checkBoxAdjustScrollbarSize; }
    protected:
        wxStaticText* m_staticText1;
        wxTextCtrl* m_spinCtrlBlinkPeriod;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_spinCtrlCaretWidth;
        wxCheckBox* m_checkBoxBlockCaret;
        wxCheckBox* m_checkBoxScrollBeyondLastLine;
        wxCheckBox* m_checkBoxCaretUseCamelCase;
        wxCheckBox* m_checkBoxCaretOnVirtualSpace;
        wxCheckBox* m_checkBoxAdjustScrollbarSize;

    protected:
        virtual void OnCaretWidthUI(wxUpdateUIEvent& event);
    };

    class EditorOptionsGeneralSavePanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorOptionsGeneralSavePanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        ~EditorOptionsGeneralSavePanel();

        void Save(OptionsConfig * options);
    protected:
        // Virtual event handlers, overide them in your derived class
        virtual void OnTrimCaretLineUI(wxUpdateUIEvent& event);
    protected:
        wxCheckBox* m_checkBoxTrimLine;
        wxCheckBox* m_checkBoxDontTrimCurrentLine;
        wxCheckBox* m_checkBoxTrimModifiedLines;
        wxCheckBox* m_checkBoxAppendLF;
    };

    class EditorSettingsComments : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsComments(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsComments();

        void Save(OptionsConfig * options);

        wxCheckBox* GetCheckBoxSmartAddFiles() const        { return m_checkBoxSmartAddFiles; }
        wxStaticText* GetStaticText3() const                { return m_staticText3; }
        wxStaticLine* GetStaticline1() const                { return m_staticline1; }
        wxCheckBox* GetCheckBoxContCComment() const         { return m_checkBoxContCComment; }
        wxCheckBox* GetCheckBoxContinueCppComment() const   { return m_checkBoxContinueCppComment; }
        wxStaticText* GetStaticText7() const                { return m_staticText7; }
        wxStaticLine* GetStaticline2() const                { return m_staticline2; }
        wxStaticText* GetStaticText1() const                { return m_staticText1; }
        wxStaticText* GetStaticText2() const                { return m_staticText2; }
        wxCheckBox* GetCheckBoxCtrl() const                 { return m_checkBoxCtrl; }
        wxCheckBox* GetCheckBoxAlt() const                  { return m_checkBoxAlt; }
    protected:
        wxCheckBox* m_checkBoxSmartAddFiles;
        wxStaticText* m_staticText3;
        wxStaticLine* m_staticline1;
        wxCheckBox* m_checkBoxContCComment;
        wxCheckBox* m_checkBoxContinueCppComment;
        wxStaticText* m_staticText7;
        wxStaticLine* m_staticline2;
        wxStaticText* m_staticText1;
        wxStaticText* m_staticText2;
        wxCheckBox* m_checkBoxCtrl;
        wxCheckBox* m_checkBoxAlt;
    };

    class EditorSettingsCommentsDoxygenPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsCommentsDoxygenPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsCommentsDoxygenPanel();

        void Save(OptionsConfig * options);

        wxStaticText* GetStaticText22() const               { return m_staticText22; }
        wxStyledTextCtrl* GetStcClassTemplate() const       { return m_stcClassTemplate; }
        wxStaticText* GetStaticText26() const               { return m_staticText26; }
        wxStyledTextCtrl* GetStcFunctionTemplate() const    { return m_stcFunctionTemplate; }
        wxCheckBox* GetCheckBoxAutoInsert() const           { return m_checkBoxAutoInsert; }
        wxCheckBox* GetCheckBoxQtStyle() const              { return m_checkBoxQtStyle; }
    protected:
        wxStaticText* m_staticText22;
        wxStyledTextCtrl* m_stcClassTemplate;
        wxStaticText* m_staticText26;
        wxStyledTextCtrl* m_stcFunctionTemplate;
        wxCheckBox* m_checkBoxAutoInsert;
        wxCheckBox* m_checkBoxQtStyle;
    };

    class EditorSettingsFolding : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsFolding(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsFolding();
        void Save(OptionsConfig * options);
        wxCheckBox* GetDisplayMargin() const                { return m_displayMargin; }
        wxChoice* GetFoldStyle() const                      { return m_foldStyle; }
        wxCheckBox* GetUnderlineFolded() const              { return m_underlineFolded; }
        wxCheckBox* GetFoldElse() const                     { return m_foldElse; }
        wxCheckBox* GetFoldPreprocessors() const            { return m_foldPreprocessors; }
        wxCheckBox* GetFoldCompact() const                  { return m_foldCompact; }
        wxCheckBox* GetCheckBoxHighlightFolding()const      { return m_checkBoxHighlightFolding; }
    protected:
        virtual void OnFoldingMarginUI(wxUpdateUIEvent& event);
    protected:
        wxCheckBox* m_displayMargin;
        wxChoice* m_foldStyle;
        wxCheckBox* m_underlineFolded;
        wxCheckBox* m_foldElse;
        wxCheckBox* m_foldPreprocessors;
        wxCheckBox* m_foldCompact;
        wxCheckBox* m_checkBoxHighlightFolding;

        StringManager m_stringManager;
    };

    class EditorSettingsBookmarksPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsBookmarksPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsBookmarksPanel();

        void Save(OptionsConfig * options);

        wxCheckBox* GetDisplaySelection() { return m_displaySelection; }
        wxStaticText* GetStaticText3() { return m_staticText3; }
        wxChoice* GetBookMarkShape() { return m_bookMarkShape; }
        wxStaticText* GetStaticText10() { return m_staticText10; }
        wxChoice* GetChoiceBMType() { return m_choiceBMType; }
        wxStaticText* GetStaticText41() { return m_staticText41; }
        wxTextCtrl* GetBookmarkLabel() { return m_BookmarkLabel; }
        wxStaticText* GetStaticText4() { return m_staticText4; }
        wxColourPickerCtrl* GetBackgroundColor() { return m_backgroundColor; }
        wxStaticText* GetStaticText5() { return m_staticText5; }
        wxColourPickerCtrl* GetForegroundColor() { return m_foregroundColor; }
        wxStaticLine* GetStaticLine18() { return m_staticLine18; }
        wxStaticText* GetStaticText66() { return m_staticText66; }
        wxColourPickerCtrl* GetHighlightColor() { return m_highlightColor; }
        wxStaticText* GetStaticText54() { return m_staticText54; }
        wxSpinCtrl* GetSpinCtrlHighlightAlpha() { return m_spinCtrlHighlightAlpha; }
        wxCheckBox* GetClearHighlitWords() { return m_clearHighlitWords; }
    protected:
        virtual void OnBookmarkChanged(wxCommandEvent& event);
        void ChangeSelection(int index);
    protected:
        wxCheckBox* m_displaySelection;
        wxStaticText* m_staticText3;
        wxChoice* m_bookMarkShape;
        wxStaticText* m_staticText10;
        wxChoice* m_choiceBMType;
        wxStaticText* m_staticText41;
        wxTextCtrl* m_BookmarkLabel;
        wxStaticText* m_staticText4;
        wxColourPickerCtrl* m_backgroundColor;
        wxStaticText* m_staticText5;
        wxColourPickerCtrl* m_foregroundColor;
        wxStaticLine* m_staticLine18;
        wxStaticText* m_staticText66;
        wxColourPickerCtrl* m_highlightColor;
        wxStaticText* m_staticText54;
        wxSpinCtrl* m_spinCtrlHighlightAlpha;
        wxCheckBox* m_clearHighlitWords;

        struct BookmarkData {
            wxColour fg;
            wxColour bg;
            wxString label;
            wxString defaultLabel;
        };

        StringManager m_stringManager;
        std::vector<BookmarkData> m_bookmarksData;
        int m_previous;
    };

    class EditorSettingsDockingWindows : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsDockingWindows(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsDockingWindows();

        virtual void Save(OptionsConfig * options);

        bool IsRestartRequired();
        wxStaticText* GetStaticText46() { return m_staticText46; }
        wxChoice* GetChoiceTabStyle() { return m_choiceTabStyle; }
        wxStaticText* GetStaticTextTabHeight() { return m_staticTextTabHeight; }
        wxChoice* GetChoiceTabHeight() { return m_choiceTabHeight; }
        wxStaticText* GetStaticText63() { return m_staticText63; }
        wxColourPickerCtrl* GetColourPickerMarker() { return m_colourPickerMarker; }
        wxStaticText* GetStaticText38() { return m_staticText38; }
        wxChoice* GetChoiceWorkspaceTabsOrientation() { return m_choiceWorkspaceTabsOrientation; }
        wxStaticText* GetStaticText42() { return m_staticText42; }
        wxChoice* GetChoiceOutputTabsOrientation() { return m_choiceOutputTabsOrientation; }
        wxStaticLine* GetStaticLine61() { return m_staticLine61; }
        wxCheckBox* GetCheckBoxShowXButton() { return m_checkBoxShowXButton; }
        wxCheckBox* GetCheckBoxEditorTabsFollowsTheme() { return m_checkBoxEditorTabsFollowsTheme; }
        wxCheckBox* GetCheckBoxMouseScrollSwitchTabs() { return m_checkBoxMouseScrollSwitchTabs; }
        wxCheckBox* GetCheckBoxSortTabsDropdownAlphabetically() { return m_checkBoxSortTabsDropdownAlphabetically; }
        wxCheckBox* GetCheckBoxEnableTabSwitchingKey() { return m_checkBoxEnableTabSwitchingKey; }
        wxPanel* GetPanelTabs() { return m_panelTabs; }
        wxRadioBox* GetRadioBoxHint() { return m_radioBoxHint; }
        wxCheckBox* GetCheckBoxCustomCaptionColour() { return m_checkBoxCustomCaptionColour; }
        wxColourPickerCtrl* GetCpCaptionColour() { return m_cpCaptionColour; }
        wxCheckBox* GetCheckBoxHideCaptions() { return m_checkBoxHideCaptions; }
        wxCheckBox* GetCheckBoxEnsureCaptionsVisible() { return m_checkBoxEnsureCaptionsVisible; }
        wxCheckBox* GetCheckBoxNavBarSortDropdown() { return m_checkBoxNavBarSortDropdown; }
        wxPanel* GetPanel12() { return m_panel12; }
        wxCheckBox* GetCheckBoxDontFoldSearchResults() { return m_checkBoxDontFoldSearchResults; }
        wxCheckBox* GetCheckBoxDontOverrideSearchStringWithSelection()
        {
            return m_checkBoxDontOverrideSearchStringWithSelection;
        }
        wxPanel* GetPanel14() { return m_panel14; }
        wxCheckBox* GetCheckBoxHideOutputPaneOnClick() { return m_checkBoxHideOutputPaneOnClick; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfBuild() { return m_checkBoxHideOutputPaneNotIfBuild; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfSearch() { return m_checkBoxHideOutputPaneNotIfSearch; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfReplace() { return m_checkBoxHideOutputPaneNotIfReplace; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfReferences() { return m_checkBoxHideOutputPaneNotIfReferences; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfOutput() { return m_checkBoxHideOutputPaneNotIfOutput; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfTrace() { return m_checkBoxHideOutputPaneNotIfTrace; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfTasks() { return m_checkBoxHideOutputPaneNotIfTasks; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfBuildQ() { return m_checkBoxHideOutputPaneNotIfBuildQ; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfCppCheck() { return m_checkBoxHideOutputPaneNotIfCppCheck; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfSvn() { return m_checkBoxHideOutputPaneNotIfSvn; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfCscope() { return m_checkBoxHideOutputPaneNotIfCscope; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfGit() { return m_checkBoxHideOutputPaneNotIfGit; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfDebug() { return m_checkBoxHideOutputPaneNotIfDebug; }
        wxCheckBox* GetCheckBoxHideOutputPaneNotIfMemCheck() { return m_checkBoxHideOutputPaneNotIfMemCheck; }
        wxCheckBox* GetCheckBoxShowDebugOnRun() { return m_checkBoxShowDebugOnRun; }
        wxPanel* GetPanel16() { return m_panel16; }
        Notebook* GetNotebook10() { return m_notebook10; }
    protected:
        virtual void OnUseCustomCaptionColourUI(wxUpdateUIEvent& event);
        virtual void OnEnsureCaptionsVisibleUI(wxUpdateUIEvent& event);
        virtual void OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event);
    protected:
        Notebook* m_notebook10;
        wxPanel* m_panelTabs;
        wxStaticText* m_staticText46;
        wxChoice* m_choiceTabStyle;
        wxStaticText* m_staticTextTabHeight;
        wxChoice* m_choiceTabHeight;
        wxStaticText* m_staticText63;
        wxColourPickerCtrl* m_colourPickerMarker;
        wxStaticText* m_staticText38;
        wxChoice* m_choiceWorkspaceTabsOrientation;
        wxStaticText* m_staticText42;
        wxChoice* m_choiceOutputTabsOrientation;
        wxStaticLine* m_staticLine61;
        wxCheckBox* m_checkBoxShowXButton;
        wxCheckBox* m_checkBoxEditorTabsFollowsTheme;
        wxCheckBox* m_checkBoxMouseScrollSwitchTabs;
        wxCheckBox* m_checkBoxSortTabsDropdownAlphabetically;
        wxCheckBox* m_checkBoxEnableTabSwitchingKey;
        wxPanel* m_panel12;
        wxRadioBox* m_radioBoxHint;
        wxCheckBox* m_checkBoxCustomCaptionColour;
        wxColourPickerCtrl* m_cpCaptionColour;
        wxCheckBox* m_checkBoxHideCaptions;
        wxCheckBox* m_checkBoxEnsureCaptionsVisible;
        wxCheckBox* m_checkBoxNavBarSortDropdown;
        wxPanel* m_panel14;
        wxCheckBox* m_checkBoxDontFoldSearchResults;
        wxCheckBox* m_checkBoxDontOverrideSearchStringWithSelection;
        wxPanel* m_panel16;
        wxCheckBox* m_checkBoxHideOutputPaneOnClick;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfBuild;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfSearch;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfReplace;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfReferences;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfOutput;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfTrace;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfTasks;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfBuildQ;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfCppCheck;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfSvn;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfCscope;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfGit;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfDebug;
        wxCheckBox* m_checkBoxHideOutputPaneNotIfMemCheck;
        wxCheckBox* m_checkBoxShowDebugOnRun;
    };

    class EditorSettingsTerminal : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsTerminal(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsTerminal();

        void Save(OptionsConfig * options);

        wxStaticText* GetStaticText2() const                        { return m_staticText2; }
        wxChoice* GetChoiceTerminals() const                        { return m_choiceTerminals; }
    protected:
        virtual void OnUseGSGSTerminalUI(wxUpdateUIEvent& event) { event.Skip(); }
    protected:
        wxStaticText* m_staticText2;
        wxChoice* m_choiceTerminals;
    };

    class EditorSettingsMiscPanel : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsMiscPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsMiscPanel();

        void Save(OptionsConfig * options);
        virtual bool IsRestartRequired()    { return this->m_restartRequired; }
        wxStaticText* GetStaticText8()      { return m_staticText8; }
        wxChoice* GetToolbarIconSize()      { return m_toolbarIconSize; }
        wxCheckBox* GetSingleAppInstance()  { return m_singleAppInstance; }
        wxCheckBox* GetVersionCheckOnStartup(){ return m_versionCheckOnStartup; }
        wxCheckBox* GetCheckBoxPromptReleaseOnly() { return m_checkBoxPromptReleaseOnly; }
        wxCheckBox* GetCheckBoxRestoreSession() { return m_checkBoxRestoreSession; }
        wxStaticText* GetStaticText79()     { return m_staticText79; }
        wxTextCtrl* GetWebSearchPrefix()    { return m_webSearchPrefix; }
        wxCheckBox* GetCheckBoxDirect2D()   { return m_checkBoxDirect2D; }
        wxPanel* GetPanel1()                { return m_panel1;}
        wxStaticText* GetStaticText31()     { return m_staticText31; }
        wxStaticText* GetStaticText33()     { return m_staticText33; }
        wxStaticText* GetStaticText35()     { return m_staticText35; }
        wxStaticText* GetStaticText37()     { return m_staticText37; }
        wxStaticText* GetStaticText39()     { return m_staticText39; }
        wxStaticText* GetStaticText41()     { return m_staticText41;}
        wxStaticText* GetStaticText43()     { return m_staticText43; }
        wxStaticText* GetStaticText45()     { return m_staticText45; }
        wxStaticText* GetStaticText47()     { return m_staticText47; }
        wxStaticText* GetStaticText49()     { return m_staticText49; }
        wxTextCtrl* GetTextCtrlPattern()    { return m_textCtrlPattern; }
        wxPanel* GetPanel23()               { return m_panel23; }
        wxStaticText* GetStaticText70()     { return m_staticText70; }
        wxCheckBox* GetStatusbarShowLine()  { return m_statusbarShowLine; }
        wxCheckBox* GetStatusbarShowCol()   { return m_statusbarShowCol; }
        wxCheckBox* GetStatusbarShowPos()   { return m_statusbarShowPos; }
        wxCheckBox* GetStatusbarShowFileLength() { return m_statusbarShowFileLength; }
        wxCheckBox* GetStatusBarShowSelChars()  { return m_statusBarShowSelChars; }
        wxPanel* GetPanel44()               { return m_panel44; }
        wxStaticText* GetStaticText94()     { return m_staticText94; }
        wxTextCtrl* GetTextCtrlSeparation() { return m_textCtrlSeparation; }
        wxStaticText* GetStaticText97()     { return m_staticText97; }
        wxPanel* GetPanelTBar()             { return m_panelTBar; }
        wxStaticText* GetStaticText1()      { return m_staticText1; }
        wxChoice* GetFileEncoding()         { return m_fileEncoding; }
        wxCheckBox* GetSetLocale()          { return m_SetLocale; }
        wxStaticText* GetStaticTextLocale() { return m_staticTextLocale; }
        wxChoice* GetAvailableLocales()     { return m_AvailableLocales; }
        wxPanel* GetPanel2()                { return m_panel2; }
        wxStaticText* GetStaticText2()      { return m_staticText2; }
        Button* GetClearButton()          { return m_clearButton; }
        wxStaticText* GetStaticText3()      { return m_staticText3; }
        wxTextCtrl* GetMaxItemsFindReplace(){ return m_maxItemsFindReplace; }
        wxStaticText* GetStaticText4()      { return m_staticText4; }
        wxTextCtrl* GetSpinCtrlMaxOpenTabs(){ return m_spinCtrlMaxOpenTabs; }
        wxStaticText* GetStaticText53()     { return m_staticText53; }
        Button* GetButtonAnnoyingDialogs(){ return m_buttonAnnoyingDialogs; }
        wxPanel* GetPanel3()                { return m_panel3; }
        wxStaticText* GetStaticText6()      { return m_staticText6; }
        wxChoice* GetChoice4()              { return m_choice4; }
        Button* GetButtonOpenLog()        { return m_buttonOpenLog; }
        wxCheckBox* GetRedirectLogOutput()  { return m_redirectLogOutput; }
        wxPanel* GetPanel4()                { return m_panel4; }
        Notebook* GetNotebook2()          { return m_notebook2; }
    protected:
        virtual void OnUseDirect2DUI(wxUpdateUIEvent& event);
        virtual void OnPromptStableReleaseUI(wxUpdateUIEvent& event);
        virtual void OnResetAnnoyingDialogsAnswers(wxCommandEvent& event);
        virtual void OnLogoutputCheckUpdateUI(wxUpdateUIEvent& event);
        int FindAvailableLocales();

        // Handlers for EditorSettingsMiscPanel events.
        void OnClearButtonClick(wxCommandEvent& event);
        void OnClearUI(wxUpdateUIEvent &e);
        void OnEnableThemeUI(wxUpdateUIEvent& event);
        void LocaleStaticUpdateUI(wxUpdateUIEvent& event);
        void LocaleChkUpdateUI(wxUpdateUIEvent& event);
        void LocaleChoiceUpdateUI(wxUpdateUIEvent& event);
        void OnLogVerbosityChanged(wxCommandEvent& event);
        void OnShowLogFile(wxCommandEvent& event);
    protected:
        Notebook* m_notebook2;
        wxPanel* m_panel1;
        wxStaticText* m_staticText8;
        wxChoice* m_toolbarIconSize;
        wxCheckBox* m_singleAppInstance;
        wxCheckBox* m_versionCheckOnStartup;
        wxCheckBox* m_checkBoxPromptReleaseOnly;
        wxCheckBox* m_checkBoxRestoreSession;
        wxStaticText* m_staticText79;
        wxTextCtrl* m_webSearchPrefix;
        wxCheckBox* m_checkBoxDirect2D;
        wxPanel* m_panel23;
        wxStaticText* m_staticText31;
        wxStaticText* m_staticText33;
        wxStaticText* m_staticText35;
        wxStaticText* m_staticText37;
        wxStaticText* m_staticText39;
        wxStaticText* m_staticText41;
        wxStaticText* m_staticText43;
        wxStaticText* m_staticText45;
        wxStaticText* m_staticText47;
        wxStaticText* m_staticText49;
        wxTextCtrl* m_textCtrlPattern;
        wxPanel* m_panel44;
        wxStaticText* m_staticText70;
        wxCheckBox* m_statusbarShowLine;
        wxCheckBox* m_statusbarShowCol;
        wxCheckBox* m_statusbarShowPos;
        wxCheckBox* m_statusbarShowFileLength;
        wxCheckBox* m_statusBarShowSelChars;
        wxPanel* m_panelTBar;
        wxStaticText* m_staticText94;
        wxTextCtrl* m_textCtrlSeparation;
        wxStaticText* m_staticText97;
        wxPanel* m_panel2;
        wxStaticText* m_staticText1;
        wxChoice* m_fileEncoding;
        wxCheckBox* m_SetLocale;
        wxStaticText* m_staticTextLocale;
        wxChoice* m_AvailableLocales;
        wxPanel* m_panel3;
        wxStaticText* m_staticText2;
        Button* m_clearButton;
        wxStaticText* m_staticText3;
        wxTextCtrl* m_maxItemsFindReplace;
        wxStaticText* m_staticText4;
        wxTextCtrl* m_spinCtrlMaxOpenTabs;
        wxStaticText* m_staticText53;
        Button* m_buttonAnnoyingDialogs;
        wxPanel* m_panel4;
        wxStaticText* m_staticText6;
        wxChoice* m_choice4;
        Button* m_buttonOpenLog;
        wxCheckBox* m_redirectLogOutput;
        EditorOptionsGeneralIndentationPanel * mIndentationPanel;
        EditorOptionsGeneralRightMarginPanel * mRightMarginPanel;
        EditorSettingsCaret * mCaretPanel;
        EditorOptionsGeneralSavePanel * mGeneralSavePanel;
        EditorSettingsComments * mComments;
        EditorSettingsTerminal * mTerminal;

        bool     m_restartRequired;
        bool     m_oldSetLocale;
        wxString m_oldpreferredLocale;
        bool     m_oldMswUseTheme;
    };

    enum eTheme {
        kTHEME_UNKNOWN = -1,
        kTHEME_DARK,
        kTHEME_LIGHT,
    };

    class EditorSettingsSyntaxHighlight : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsSyntaxHighlight(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Colours and Fonts"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsSyntaxHighlight();

        bool IsRestartRequired() const;
        void Save(OptionsConfig * options);
        void Apply();
        ToolBar* GetToolbar() const                             { return m_toolbar; }
        wxCheckBox* GetCbUseCustomBaseColour() const            { return m_cbUseCustomBaseColour; }
        wxColourPickerCtrl* GetColourPickerBaseColour() const   { return m_colourPickerBaseColour; }
        wxStaticText* GetStaticText159() const                  { return m_staticText159; }
        wxFontPickerCtrl* GetFontPickerGlobal() const           { return m_fontPickerGlobal; }
        wxStaticText* GetStaticText155() const                  { return m_staticText155; }
        wxChoice* GetChoiceGlobalTheme() const                  { return m_choiceGlobalTheme; }
        wxStyledTextCtrl* GetStcPreview() const                 { return m_stcPreview; }
        wxPanel* GetPanelGlobalColours() const                  { return m_panelGlobalColours; }
        wxListBox* GetListBox() const                           { return m_listBox; }
        wxStaticText* GetStaticText70() const                   { return m_staticText70; }
        wxChoice* GetChoiceLexerThemes() const                  { return m_choiceLexerThemes; }
        wxStaticText* GetStaticText6() const                    { return m_staticText6; }
        wxFontPickerCtrl* GetGlobalFontPicker() const           { return m_globalFontPicker; }
        wxStaticText* GetStaticText7() const                    { return m_staticText7; }
        wxColourPickerCtrl* GetGlobalBgColourPicker() const     { return m_globalBgColourPicker; }
        wxStaticText* GetStaticText8() const                    { return m_staticText8; }
        wxTextCtrl* GetFileSpec() const                         { return m_fileSpec; }
        wxPanel* GetPanelGlobalSettings() const                 { return m_panelGlobalSettings; }
        wxListBox* GetProperties() const                        { return m_properties; }
        wxStaticText* GetStaticText2() const                    { return m_staticText2; }
        wxFontPickerCtrl* GetFontPicker() const                 { return m_fontPicker; }
        wxStaticText* GetStaticText3() const                    { return m_staticText3; }
        wxColourPickerCtrl* GetColourPicker() const             { return m_colourPicker; }
        wxStaticText* GetStaticText4() const                    { return m_staticText4; }
        wxColourPickerCtrl* GetBgColourPicker() const           { return m_bgColourPicker; }
        wxCheckBox* GetEolFilled() const                        { return m_eolFilled; }
        wxCheckBox* GetStyleWithinPreProcessor() const          { return m_styleWithinPreProcessor; }
        wxStaticLine* GetStaticline1() const                    { return m_staticline1; }
        wxStaticText* GetStaticText1() const                    { return m_staticText1; }
        Button* GetButton5() const                              { return m_button5; }
        Button* GetButton6() const                              { return m_button6; }
        Button* GetButton7() const                              { return m_button7; }
        Button* GetButton8() const                              { return m_button8; }
        Button* GetButton9() const                              { return m_button9; }
        wxPanel* GetPanelCustomize() const                      { return m_panelCustomize; }
        wxStaticText* GetStaticText9() const                    { return m_staticText9; }
        wxColourPickerCtrl* GetColourPickerSelTextBgColour() const { return m_colourPickerSelTextBgColour; }
        wxStaticText* GetStaticText94()const { return m_staticText94; }
        wxCheckBox* GetCheckBoxCustomSelectionFgColour()const { return m_checkBoxCustomSelectionFgColour; }
        wxStaticText* GetStaticText84()const { return m_staticText84; }
        wxColourPickerCtrl* GetColourPickerSelTextFgColour()const { return m_colourPickerSelTextFgColour; }
        wxPanel* GetPanelTextSelection()const { return m_panelTextSelection; }
        Notebook* GetNotebook2()const { return m_notebook2; }
        wxPanel* GetPanel25() const { return m_panel25; }
        wxPanel* GetPanelSyntaxHighlight() const { return m_panelSyntaxHighlight; }
        Notebook* GetNotebook() const { return m_notebook; }
        wxPanel* GetPanel171() const { return m_panel171; }
    protected:
        virtual void OnUseCustomBaseColour(wxCommandEvent& event);
        virtual void OnUseCustomColourUI(wxUpdateUIEvent& event);
        virtual void OnCustomBaseColourPIcked(wxColourPickerEvent& event);
        virtual void OnGlobalFontSelected(wxFontPickerEvent& event);
        virtual void OnGlobalThemeSelected(wxCommandEvent& event);
        virtual void OnLexerSelected(wxCommandEvent& event);
        virtual void OnTheme(wxCommandEvent& event);
        virtual void OnFontChanged(wxFontPickerEvent& event);
        virtual void OnColourChanged(wxColourPickerEvent& event);
        virtual void OnText(wxCommandEvent& event);
        virtual void OnItemSelected(wxCommandEvent& event);
        virtual void OnEolFilled(wxCommandEvent& event);
        virtual void OnStyleWithinPreprocessor(wxCommandEvent& event);
        virtual void OnStyleWithingPreProcessorUI(wxUpdateUIEvent& event);
        virtual void OnEditKeyWordsButton0(wxCommandEvent& event);
        virtual void OnEditKeyWordsButton1(wxCommandEvent& event);
        virtual void OnEditKeyWordsButton2(wxCommandEvent& event);
        virtual void OnEditKeyWordsButton3(wxCommandEvent& event);
        virtual void OnEditKeyWordsButton4(wxCommandEvent& event);
        virtual void OnSelTextChanged(wxColourPickerEvent& event);
        virtual void OnUseCustomFgTextColour(wxCommandEvent& event);
        virtual void OnTextSelFgUI(wxUpdateUIEvent& event);
        virtual void OnSelTextFgChanged(wxColourPickerEvent& event);
        //virtual void OnButtonOK(wxCommandEvent& event);
        //virtual void OnButtonApply(wxCommandEvent& event);
        virtual void OnButtonApplyUI(wxUpdateUIEvent& event);

        virtual void OnLoadEclipseThemeWebsite(wxCommandEvent& event);
        virtual void OnImportEclipseTheme(wxCommandEvent& event);
        virtual void OnRestoreDefaults(wxCommandEvent& event);
        virtual void OnExportAll(wxCommandEvent& event);
        virtual void OnToolExportAll(wxCommandEvent& event);
        virtual void OnExportSelective(wxCommandEvent& event);
        virtual void OnImport(wxCommandEvent& event);
        virtual void OnNewTheme(wxCommandEvent& event);
        virtual void OnOutputViewColourChanged(wxColourPickerEvent& event);

        void LoadLexer(const wxString& themeName);
        void SaveChanges();
        void Clear();
        void CreateLexerPage();
        StyleProperty::Map_t::iterator GetSelectedStyle();
        void EditKeyWords(int set);

        void DoUpdatePreview();
        void DoSetGlobalBgColour(const wxColour& colour);
        void DoShowTooltipForGlobalBgColourChanged();
        void DoExport(const wxArrayString& lexers = wxArrayString());
    protected:
        wxPanel* m_panel171;
        ToolBar* m_toolbar;
        Notebook* m_notebook;
        wxPanel* m_panelGlobalColours;
        wxCheckBox* m_cbUseCustomBaseColour;
        wxColourPickerCtrl* m_colourPickerBaseColour;
        wxStaticText* m_staticText159;
        wxFontPickerCtrl* m_fontPickerGlobal;
        wxStaticText* m_staticText155;
        wxChoice* m_choiceGlobalTheme;
        wxStyledTextCtrl* m_stcPreview;
        wxPanel* m_panelSyntaxHighlight;
        wxListBox* m_listBox;
        wxPanel* m_panel25;
        Notebook* m_notebook2;
        wxPanel* m_panelGlobalSettings;
        wxStaticText* m_staticText70;
        wxChoice* m_choiceLexerThemes;
        wxStaticText* m_staticText6;
        wxFontPickerCtrl* m_globalFontPicker;
        wxStaticText* m_staticText7;
        wxColourPickerCtrl* m_globalBgColourPicker;
        wxStaticText* m_staticText8;
        wxTextCtrl* m_fileSpec;
        wxPanel* m_panelCustomize;
        wxListBox* m_properties;
        wxStaticText* m_staticText2;
        wxFontPickerCtrl* m_fontPicker;
        wxStaticText* m_staticText3;
        wxColourPickerCtrl* m_colourPicker;
        wxStaticText* m_staticText4;
        wxColourPickerCtrl* m_bgColourPicker;
        wxCheckBox* m_eolFilled;
        wxCheckBox* m_styleWithinPreProcessor;
        wxStaticLine* m_staticline1;
        wxStaticText* m_staticText1;
        Button* m_button5;
        Button* m_button6;
        Button* m_button7;
        Button* m_button8;
        Button* m_button9;
        wxPanel* m_panelTextSelection;
        wxStaticText* m_staticText9;
        wxColourPickerCtrl* m_colourPickerSelTextBgColour;
        wxStaticText* m_staticText94;
        wxCheckBox* m_checkBoxCustomSelectionFgColour;
        wxStaticText* m_staticText84;
        wxColourPickerCtrl* m_colourPickerSelTextFgColour;
        LexerStyle::Ptr_t m_lexer;
        eTheme m_initialTheme = kTHEME_UNKNOWN;
        eTheme m_endingTheme = kTHEME_UNKNOWN;
        bool m_isModified;
        bool m_globalThemeChanged;
        bool m_globalBgColourChanged;
        bool m_useBaseColourInitial = false;
        bool m_useBaseColourEnding = false;
        static bool m_globalBgColourChangedTooltipShown;
    };

    class NewThemeDialog : public wxDialog
    {
    public:
        NewThemeDialog(wxWindow* parent, LexerStyle::Ptr_t lexer, wxWindowID id = wxID_ANY, const wxString& title = _("New Theme"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMMinDlgSize,
            long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
        virtual ~NewThemeDialog();

        wxStaticText* GetStaticText127() const { return m_staticText127; }
        wxTextCtrl* GetTextCtrlName() const { return m_textCtrlName; }
        wxStaticText* GetStaticText131() const { return m_staticText131; }
        wxChoice* GetChoiceLanguage() const { return m_choiceLanguage; }
        wxStaticText* GetStaticText135() const { return m_staticText135; }
        wxChoice* GetChoiceBaseTheme() const { return m_choiceBaseTheme; }

        wxString GetLexerName() const {
            return m_choiceLanguage->GetStringSelection();
        }

        wxString GetThemeName() const {
            return m_textCtrlName->GetValue();
        }

        wxString GetBaseTheme() const {
            return m_choiceBaseTheme->GetStringSelection();
        }
    protected:
        virtual void OnLexerSelected(wxCommandEvent& event);
        virtual void OnOkUI(wxUpdateUIEvent& event);
        void DoInitialize(LexerStyle::Ptr_t lexer);
    protected:
        wxStaticText* m_staticText127;
        wxTextCtrl* m_textCtrlName;
        wxStaticText* m_staticText131;
        wxChoice* m_choiceLanguage;
        wxStaticText* m_staticText135;
        wxChoice* m_choiceBaseTheme;
        DialogButtonSizer* m_stdBtnSizer117;
        Button* m_buttonOK;
        Button* m_buttonCancel;
    };

    struct AccelItemData : public wxClientData {
        WindowManager::HotKey m_menuItemData;
        AccelItemData(const WindowManager::HotKey & data)
            : m_menuItemData(data)
        {
        }
    };

    class EditorSettingsAccelTable : public wxPanel, public TreeBookNode
    {
    public:
        EditorSettingsAccelTable(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Keyboard Shortcuts"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL);
        virtual ~EditorSettingsAccelTable();
        void Save(OptionsConfig * options);
        void Apply();
        wxSearchCtrl* GetTextCtrlFilter() const         { return m_textCtrlFilter; }
        ThemedListCtrl* GetDvListCtrl() const           { return m_dvListCtrl; }
        Button* GetButtonEdit() const                   { return m_buttonEdit; }
        Button* GetButtonDefault() const                { return m_buttonDefault; }
    protected:
        virtual void OnText(wxCommandEvent& event);
        virtual void OnDVItemActivated(wxDataViewEvent& event);
        virtual void OnEditButton(wxCommandEvent& event);
        virtual void OnEditUI(wxUpdateUIEvent& event);
        virtual void OnButtonDefaults(wxCommandEvent& event);

        // Handlers for EditorSettingsAccelTable events
        void PopulateTable(const wxString & filter);

        void DoItemActivated();
        bool IsMatchesFilter(const wxString & filter, const WindowManager::HotKey & item);
        bool HasAccelerator(const wxString & accel, WindowManager::HotKey & who);
        AccelItemData* DoGetItemData(const wxDataViewItem& item);
        wxDataViewItem FindAccel(const WindowManager::HotKey & mid);
    protected:
        WindowManager::HotKeyList m_accelMap;
        wxSearchCtrl* m_textCtrlFilter;
        ThemedListCtrl* m_dvListCtrl;
        Button* m_buttonEdit;
        Button* m_buttonDefault;
    };

    class PreferencesDialog : public wxDialog
    {
    public:
        PreferencesDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize, long style = wxSIMPLE_BORDER);
        virtual ~PreferencesDialog();

        //wxTreebook* GetTreeBook() const         { return m_treeBook; }
        Button* GetOkButton() const             { return m_okButton; }
        Button* GetCancelButton() const         { return m_cancelButton; }
        Button* GetApplyButton() const          { return m_applyButton; }
        inline bool isRestartRquired() const    { return restartRquired; }
    protected:
        void Initialize();
        void DoSave();
        void DoApply();

        virtual void OnActivate(wxActivateEvent& event) { event.Skip(); }
        virtual void OnInitDialog(wxInitDialogEvent& event) { event.Skip(); }
        virtual void OnButtonOK(wxCommandEvent& event);
        virtual void OnButtonApply(wxCommandEvent& event);

        template <typename T> void AddSubPage(T* parent, wxWindow* ptr, wxString const& caption, bool selected = false)
        {
            m_treeBook->InsertPage(parent, ptr, caption, selected);
            mBookList.push_back(ptr);
        }

        template <typename T> void AddPage(T* ptr, wxString const& caption, bool selected = false)
        {
            m_treeBook->AddPage(ptr, caption, selected);
            mBookList.push_back(ptr);
        }
    protected:
        Treebook * m_treeBook;
        Button* m_okButton;
        Button* m_cancelButton;
        Button* m_applyButton;
        TreeBookNodeList mBookList;
        bool restartRquired;
    };
}
#endif