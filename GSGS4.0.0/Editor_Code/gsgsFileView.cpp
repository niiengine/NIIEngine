
#include "gsgsFileView.h"
#include "gsgsToolsetManager.h"
#include "CommonController1.h"
#include "gsgsImageManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsPluginCommon.h"
#include "gsgsStyleManager.h"
#include "gsgsLogManager.h"
#include "gsgsEvent.h"
#include "gsgsTagManager.h"
#include "CommonController.h"
#include "gsgsEditorConfig.h"
#include "gsgsFileUtil.h"
#include "gsgsFrame.h"
#include "globals.h"
#include "gsgsManager.h"
#include "NewProjectDialog.h"
#include "gsgsPluginManager.h"
#include "gsgsProgressDialog.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsTree.h"
#include "gsgsWorkspacePane.h"
#include "gsgsWindowManager.h"
#include "gsgsOptionsConfig.h"
#include "wx/imaglist.h"
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/mimetype.h>
#include <wx/progdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/textdlg.h>
#include <wx/treectrl.h>
#include <wx/xrc/xmlres.h>
#include <wx/generic/choicdgg.h>
#include <wx/wupdlock.h>
#include <wx/stc/stc.h>
#include <wx/fontmap.h>

extern void wxC2ED4InitBitmapResources();

static bool bBitmapLoaded = false;

namespace gsgs
{
    class EditorSettingsLocal : public wxDialog
    {
    public:
        EditorSettingsLocal(OptionsConfig * higherOptions, wxXmlNode* node, enum prefsLevel level = pLevel_dunno,
                            wxWindow* parent = NULL, wxWindowID id = wxID_ANY,
                            const wxString& title = _("Local Preferences"));
        ~EditorSettingsLocal();

        LocalOptionsConfigPtr GetLocalOpts() const { return localOptions; }

    public:
        wxCheckBox* GetIndentsUsesTabsEnable() { return m_indentsUsesTabsEnable; }
        wxCheckBox* GetIndentsUsesTabs() { return m_indentsUsesTabs; }
        wxCheckBox* GetTabWidthEnable() { return m_tabWidthEnable; }
        wxStaticText* GetStaticTexttabWidth() { return m_staticTexttabWidth; }
        wxSpinCtrl* GetTabWidth() { return m_tabWidth; }
        wxCheckBox* GetIndentWidthEnable() { return m_indentWidthEnable; }
        wxStaticText* GetStaticTextindentWidth() { return m_staticTextindentWidth; }
        wxSpinCtrl* GetIndentWidth() { return m_indentWidth; }
        wxPanel* GetPanel2() { return m_panel2; }
        wxCheckBox* GetDisplayBookmarkMarginEnable() { return m_displayBookmarkMarginEnable; }
        wxCheckBox* GetDisplayBookmarkMargin() { return m_displayBookmarkMargin; }
        wxCheckBox* GetCheckBoxDisplayFoldMarginEnable() { return m_checkBoxDisplayFoldMarginEnable; }
        wxCheckBox* GetCheckBoxDisplayFoldMargin() { return m_checkBoxDisplayFoldMargin; }
        wxCheckBox* GetCheckBoxHideChangeMarkerMarginEnable() { return m_checkBoxHideChangeMarkerMarginEnable; }
        wxCheckBox* GetCheckBoxHideChangeMarkerMargin() { return m_checkBoxHideChangeMarkerMargin; }
        wxCheckBox* GetDisplayLineNumbersEnable() { return m_displayLineNumbersEnable; }
        wxCheckBox* GetDisplayLineNumbers() { return m_displayLineNumbers; }
        wxPanel* GetPanel3() { return m_panel3; }
        wxCheckBox* GetShowIndentationGuideLinesEnable() { return m_showIndentationGuideLinesEnable; }
        wxCheckBox* GetShowIndentationGuideLines() { return m_showIndentationGuideLines; }
        wxCheckBox* GetHighlightCaretLineEnable() { return m_highlightCaretLineEnable; }
        wxCheckBox* GetHighlightCaretLine() { return m_highlightCaretLine; }
        wxCheckBox* GetCheckBoxTrimLineEnable() { return m_checkBoxTrimLineEnable; }
        wxCheckBox* GetCheckBoxTrimLine() { return m_checkBoxTrimLine; }
        wxCheckBox* GetCheckBoxAppendLFEnable() { return m_checkBoxAppendLFEnable; }
        wxCheckBox* GetCheckBoxAppendLF() { return m_checkBoxAppendLF; }
        wxCheckBox* GetWhitespaceStyleEnable() { return m_whitespaceStyleEnable; }
        wxStaticText* GetStaticTextwhitespaceStyle() { return m_staticTextwhitespaceStyle; }
        wxChoice* GetWhitespaceStyle() { return m_whitespaceStyle; }
        wxCheckBox* GetChoiceEOLEnable() { return m_choiceEOLEnable; }
        wxStaticText* GetEOLstatic() { return m_EOLstatic; }
        wxChoice* GetChoiceEOL() { return m_choiceEOL; }
        wxCheckBox* GetFileEncodingEnable() { return m_fileEncodingEnable; }
        wxStaticText* GetStaticTextfileEncoding() { return m_staticTextfileEncoding; }
        wxChoice* GetFileEncoding() { return m_fileEncoding; }
        wxPanel* GetPanel4() { return m_panel4; }
        wxNotebook* GetNotebook23() { return m_notebook23; }
    protected:
        void DisplayHigherValues(const OptionsConfig * options);
        void DisplayLocalValues(const LocalOptionsConfigPtr options);

        void indentsUsesTabsUpdateUI(wxUpdateUIEvent& event);
        void indentWidthUpdateUI(wxUpdateUIEvent& event);
        void tabWidthUpdateUI(wxUpdateUIEvent& event);
        void displayBookmarkMarginUpdateUI(wxUpdateUIEvent& event);
        void checkBoxDisplayFoldMarginUpdateUI(wxUpdateUIEvent& event);
        void checkBoxHideChangeMarkerMarginUpdateUI(wxUpdateUIEvent& event);
        void displayLineNumbersUpdateUI(wxUpdateUIEvent& event);
        void showIndentationGuideLinesUpdateUI(wxUpdateUIEvent& event);
        void highlightCaretLineUpdateUI(wxUpdateUIEvent& event);
        void checkBoxTrimLineUpdateUI(wxUpdateUIEvent& event);
        void checkBoxAppendLFUpdateUI(wxUpdateUIEvent& event);
        void whitespaceStyleUpdateUI(wxUpdateUIEvent& event);
        void choiceEOLUpdateUI(wxUpdateUIEvent& event);
        void fileEncodingUpdateUI(wxUpdateUIEvent& event);

        void OnOK(wxCommandEvent& event);
    protected:
        wxNotebook* m_notebook23;
        wxPanel* m_panel2;
        wxCheckBox* m_indentsUsesTabsEnable;
        wxCheckBox* m_indentsUsesTabs;
        wxCheckBox* m_tabWidthEnable;
        wxStaticText* m_staticTexttabWidth;
        wxSpinCtrl* m_tabWidth;
        wxCheckBox* m_indentWidthEnable;
        wxStaticText* m_staticTextindentWidth;
        wxSpinCtrl* m_indentWidth;
        wxPanel* m_panel3;
        wxCheckBox* m_displayBookmarkMarginEnable;
        wxCheckBox* m_displayBookmarkMargin;
        wxCheckBox* m_checkBoxDisplayFoldMarginEnable;
        wxCheckBox* m_checkBoxDisplayFoldMargin;
        wxCheckBox* m_checkBoxHideChangeMarkerMarginEnable;
        wxCheckBox* m_checkBoxHideChangeMarkerMargin;
        wxCheckBox* m_displayLineNumbersEnable;
        wxCheckBox* m_displayLineNumbers;
        wxPanel* m_panel4;
        wxCheckBox* m_showIndentationGuideLinesEnable;
        wxCheckBox* m_showIndentationGuideLines;
        wxCheckBox* m_highlightCaretLineEnable;
        wxCheckBox* m_highlightCaretLine;
        wxCheckBox* m_checkBoxTrimLineEnable;
        wxCheckBox* m_checkBoxTrimLine;
        wxCheckBox* m_checkBoxAppendLFEnable;
        wxCheckBox* m_checkBoxAppendLF;
        wxCheckBox* m_whitespaceStyleEnable;
        wxStaticText* m_staticTextwhitespaceStyle;
        wxChoice* m_whitespaceStyle;
        wxCheckBox* m_choiceEOLEnable;
        wxStaticText* m_EOLstatic;
        wxChoice* m_choiceEOL;
        wxCheckBox* m_fileEncodingEnable;
        wxStaticText* m_staticTextfileEncoding;
        wxChoice* m_fileEncoding;
        DialogButtonSizer* m_stdBtnSizer17;
        Button* m_button19;
        Button* m_button21;

        StringManager m_EOLstringManager;
        StringManager m_WSstringManager;
        LocalOptionsConfigPtr localOptions;
        OptionsConfig * higherOptions;
        wxXmlNode* node;
    };
    //--------------------------------------------------------------------------
    EditorSettingsLocal::EditorSettingsLocal(OptionsConfig * hrOptions, wxXmlNode* nde, enum prefsLevel level,
        wxWindow* parent, wxWindowID id, const wxString& title)
        : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, _T("EditorSettingsLocal"))
        , higherOptions(hrOptions)
        , node(nde)
    {
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxC2ED4InitBitmapResources();
            bBitmapLoaded = true;
        }
        wxWindowUpdateLocker llt(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_notebook23 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxBK_DEFAULT, wxT("m_notebook23"));
        wxImageList* m_notebook23_il = new wxImageList(16, 16);
        m_notebook23->AssignImageList(m_notebook23_il);

        mainSizer->Add(m_notebook23, 1, wxALL | wxEXPAND, 5);

        m_panel2 = new wxPanel(m_notebook23, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        int m_panel2ImgIndex;
        m_panel2ImgIndex = m_notebook23_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("indent")));
        m_notebook23->AddPage(m_panel2, _("Indentation"), true, m_panel2ImgIndex);

        wxBoxSizer* boxSizer3 = new wxBoxSizer(wxVERTICAL);
        m_panel2->SetSizer(boxSizer3);

        wxFlexGridSizer* fgSizer31 = new wxFlexGridSizer(0, 2, 0, 20);
        fgSizer31->SetFlexibleDirection(wxBOTH);
        fgSizer31->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer31->AddGrowableCol(1);

        boxSizer3->Add(fgSizer31, 1, wxALL | wxEXPAND, 5);

        m_indentsUsesTabsEnable = new wxCheckBox(m_panel2, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_indentsUsesTabsEnable->SetValue(true);

        fgSizer31->Add(m_indentsUsesTabsEnable, 0, wxALL, 5);

        m_indentsUsesTabs = new wxCheckBox(m_panel2, wxID_ANY, _("Use tabs in indentation"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_indentsUsesTabs->SetValue(false);
        m_indentsUsesTabs->SetToolTip(_("If clear, only spaces will be used for indentation.\nIf set, a mixture of tabs and spaces will be used."));

        fgSizer31->Add(m_indentsUsesTabs, 0, wxALL, 5);

        m_tabWidthEnable = new wxCheckBox(m_panel2, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_tabWidthEnable->SetValue(true);

        fgSizer31->Add(m_tabWidthEnable, 0, wxALL, 5);

        wxBoxSizer* bSizer7 = new wxBoxSizer(wxHORIZONTAL);

        fgSizer31->Add(bSizer7, 1, wxEXPAND, 5);

        m_staticTexttabWidth = gsgs_Style().createStaticText(m_panel2, wxID_ANY, _("Columns per tab character in document:"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer7->Add(m_staticTexttabWidth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        bSizer7->Add(0, 0, 1, wxEXPAND, 5);

        m_tabWidth = new wxSpinCtrl(m_panel2, wxID_ANY, wxT("4"), wxDefaultPosition, wxSize(-1, -1), wxSP_ARROW_KEYS);
        m_tabWidth->SetRange(1, 20);
        m_tabWidth->SetValue(4);

        bSizer7->Add(m_tabWidth, 0, wxALL | wxEXPAND, 5);

        m_indentWidthEnable = new wxCheckBox(m_panel2, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_indentWidthEnable->SetValue(true);

        fgSizer31->Add(m_indentWidthEnable, 0, wxALL, 5);

        wxBoxSizer* bSizer6 = new wxBoxSizer(wxHORIZONTAL);

        fgSizer31->Add(bSizer6, 1, wxEXPAND, 5);

        m_staticTextindentWidth = gsgs_Style().createStaticText(m_panel2, wxID_ANY, _("Columns per indentation level:"), wxDefaultPosition, wxSize(-1, -1), 0);

        bSizer6->Add(m_staticTextindentWidth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        bSizer6->Add(0, 0, 1, wxEXPAND, 5);

        m_indentWidth = new wxSpinCtrl(m_panel2, wxID_ANY, wxT("4"), wxDefaultPosition, wxSize(-1, -1), wxSP_ARROW_KEYS);
        m_indentWidth->SetRange(1, 20);
        m_indentWidth->SetValue(4);

        bSizer6->Add(m_indentWidth, 0, wxALL | wxEXPAND, 5);

        m_panel3 = new wxPanel(m_notebook23, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        int m_panel3ImgIndex;
        m_panel3ImgIndex = m_notebook23_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("margins")));
        m_notebook23->AddPage(m_panel3, _("Margins"), false, m_panel3ImgIndex);

        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        m_panel3->SetSizer(boxSizer2);

        wxFlexGridSizer* fgSizer21 = new wxFlexGridSizer(0, 2, 0, 20);
        fgSizer21->SetFlexibleDirection(wxBOTH);
        fgSizer21->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer21->AddGrowableCol(1);

        boxSizer2->Add(fgSizer21, 1, wxALL | wxEXPAND, 5);

        m_displayBookmarkMarginEnable = new wxCheckBox(m_panel3, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_displayBookmarkMarginEnable->SetValue(true);

        fgSizer21->Add(m_displayBookmarkMarginEnable, 0, wxALL, 5);

        m_displayBookmarkMargin = new wxCheckBox(m_panel3, wxID_ANY, _("Display Breakpoints / Bookmarks margin"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_displayBookmarkMargin->SetValue(false);

        fgSizer21->Add(m_displayBookmarkMargin, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxDisplayFoldMarginEnable = new wxCheckBox(m_panel3, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_checkBoxDisplayFoldMarginEnable->SetValue(true);

        fgSizer21->Add(m_checkBoxDisplayFoldMarginEnable, 0, wxALL, 5);

        m_checkBoxDisplayFoldMargin = new wxCheckBox(m_panel3, wxID_ANY, _("Display Folding margin"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxDisplayFoldMargin->SetValue(false);
        m_checkBoxDisplayFoldMargin->SetToolTip(_("Display the margin that lets you 'fold' individual functions, or sections of functions, to hide their contents"));

        fgSizer21->Add(m_checkBoxDisplayFoldMargin, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxHideChangeMarkerMarginEnable = new wxCheckBox(m_panel3, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_checkBoxHideChangeMarkerMarginEnable->SetValue(true);

        fgSizer21->Add(m_checkBoxHideChangeMarkerMarginEnable, 0, wxALL, 5);

        m_checkBoxHideChangeMarkerMargin = new wxCheckBox(m_panel3, wxID_ANY, _("Hide change marker margin"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxHideChangeMarkerMargin->SetValue(false);
        m_checkBoxHideChangeMarkerMargin->SetToolTip(_("Display the margin in which a coloured line marks any altered line"));

        fgSizer21->Add(m_checkBoxHideChangeMarkerMargin, 0, wxALL, 5);

        m_displayLineNumbersEnable = new wxCheckBox(m_panel3, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_displayLineNumbersEnable->SetValue(true);

        fgSizer21->Add(m_displayLineNumbersEnable, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_displayLineNumbers = new wxCheckBox(m_panel3, wxID_ANY, _("Display line numbers"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_displayLineNumbers->SetValue(false);
        m_displayLineNumbers->SetToolTip(_("Display line numbers margin"));

        fgSizer21->Add(m_displayLineNumbers, 0, wxALL, 5);

        m_panel4 = new wxPanel(m_notebook23, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
        int m_panel4ImgIndex;
        m_panel4ImgIndex = m_notebook23_il->Add(wxXmlResource::Get()->LoadBitmap(wxT("misc")));
        m_notebook23->AddPage(m_panel4, _("Misc"), false, m_panel4ImgIndex);

        wxBoxSizer* boxSizer5 = new wxBoxSizer(wxVERTICAL);
        m_panel4->SetSizer(boxSizer5);

        wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 20);
        fgSizer2->SetFlexibleDirection(wxBOTH);
        fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer2->AddGrowableCol(1);

        boxSizer5->Add(fgSizer2, 0, wxALL | wxALIGN_LEFT, 5);

        m_showIndentationGuideLinesEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_showIndentationGuideLinesEnable->SetValue(true);

        fgSizer2->Add(m_showIndentationGuideLinesEnable, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_showIndentationGuideLines = new wxCheckBox(m_panel4, wxID_ANY, _("Show indentation guidelines"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_showIndentationGuideLines->SetValue(false);
        m_showIndentationGuideLines->SetToolTip(_("Display horizontal guides for matching braces \"{\""));

        fgSizer2->Add(m_showIndentationGuideLines, 0, wxALL, 5);

        m_highlightCaretLineEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_highlightCaretLineEnable->SetValue(true);

        fgSizer2->Add(m_highlightCaretLineEnable, 0, wxALL, 5);

        m_highlightCaretLine = new wxCheckBox(m_panel4, wxID_ANY, _("Highlight caret line"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_highlightCaretLine->SetValue(false);
        m_highlightCaretLine->SetToolTip(_("Set a different background colour for the line containing the caret"));

        fgSizer2->Add(m_highlightCaretLine, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxTrimLineEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_checkBoxTrimLineEnable->SetValue(true);

        fgSizer2->Add(m_checkBoxTrimLineEnable, 0, wxALL, 5);

        m_checkBoxTrimLine = new wxCheckBox(m_panel4, wxID_ANY, _("When saving files, trim empty lines"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxTrimLine->SetValue(false);

        fgSizer2->Add(m_checkBoxTrimLine, 0, wxALL, 5);

        m_checkBoxAppendLFEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_checkBoxAppendLFEnable->SetValue(true);

        fgSizer2->Add(m_checkBoxAppendLFEnable, 0, wxALL, 5);

        m_checkBoxAppendLF = new wxCheckBox(m_panel4, wxID_ANY, _("If missing, append EOL at end of file"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxAppendLF->SetValue(false);
        m_checkBoxAppendLF->SetToolTip(_("Make sure the file finishes with an end-of-line"));

        fgSizer2->Add(m_checkBoxAppendLF, 0, wxALL, 5);

        wxFlexGridSizer* flexGridSizer15 = new wxFlexGridSizer(0, 3, 0, 0);
        flexGridSizer15->SetFlexibleDirection(wxBOTH);
        flexGridSizer15->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer15->AddGrowableCol(2);

        boxSizer5->Add(flexGridSizer15, 1, wxALL | wxEXPAND | wxALIGN_LEFT, 5);

        m_whitespaceStyleEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_whitespaceStyleEnable->SetValue(true);

        flexGridSizer15->Add(m_whitespaceStyleEnable, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_staticTextwhitespaceStyle = gsgs_Style().createStaticText(m_panel4, wxID_ANY, _("Whitespace visibility:"), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer15->Add(m_staticTextwhitespaceStyle, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_whitespaceStyleArr;
        m_whitespaceStyle = new wxChoice(m_panel4, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_whitespaceStyleArr, 0);

        flexGridSizer15->Add(m_whitespaceStyle, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

        m_choiceEOLEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_choiceEOLEnable->SetValue(true);

        flexGridSizer15->Add(m_choiceEOLEnable, 0, wxALL, 5);

        m_EOLstatic = gsgs_Style().createStaticText(m_panel4, wxID_ANY, _("EOL Mode:"), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer15->Add(m_EOLstatic, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_choiceEOLArr;
        m_choiceEOL = new wxChoice(m_panel4, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceEOLArr, 0);
        m_choiceEOL->SetToolTip(_("Set the editor's EOL mode (End Of Line). When set to 'Default' gsgs will set the EOL according to the hosting OS"));

        flexGridSizer15->Add(m_choiceEOL, 0, wxALL | wxEXPAND, 5);

        m_fileEncodingEnable = new wxCheckBox(m_panel4, wxID_ANY, _("Use global setting"), wxDefaultPosition, wxSize(-1, -1), wxALIGN_RIGHT);
        m_fileEncodingEnable->SetValue(true);

        flexGridSizer15->Add(m_fileEncodingEnable, 0, wxALL, 5);

        m_staticTextfileEncoding = gsgs_Style().createStaticText(m_panel4, wxID_ANY, _("File font encoding"), wxDefaultPosition, wxSize(-1, -1), 0);

        flexGridSizer15->Add(m_staticTextfileEncoding, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_fileEncodingArr;
        m_fileEncoding = new wxChoice(m_panel4, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_fileEncodingArr, 0);

        flexGridSizer15->Add(m_fileEncoding, 0, wxALL | wxEXPAND, 5);

        wxBoxSizer* bSizer4 = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5);

        m_stdBtnSizer17 = new DialogButtonSizer();

        bSizer4->Add(m_stdBtnSizer17, 0, wxALL, 10);

        m_button19 = new Button(this, wxID_OK, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_button19->SetDefault();
        m_stdBtnSizer17->AddButton(m_button19);

        m_button21 = new Button(this, wxID_CANCEL, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_stdBtnSizer17->AddButton(m_button21);
        m_stdBtnSizer17->Realize();


/*#if wxVERSION_NUMBER >= 2900
        if (!wxPersistenceManager::Get().Find(m_notebook23)) {
            wxPersistenceManager::Get().RegisterAndRestore(m_notebook23);
        }
        else {
            wxPersistenceManager::Get().Restore(m_notebook23);
        }
#endif
        if (GetSizer()) {
            GetSizer()->Fit(this);
        }
        if (GetParent()) {
            CentreOnParent();
        }
        else {
            CentreOnScreen();
        }*/
        CentreOnScreen();
/*#if wxVERSION_NUMBER >= 2900
        if (!wxPersistenceManager::Get().Find(this)) {
            wxPersistenceManager::Get().RegisterAndRestore(this);
        }
        else {
            wxPersistenceManager::Get().Restore(this);
        }
#endif*/
        // Connect events
        m_indentsUsesTabsEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::indentsUsesTabsUpdateUI), NULL, this);
        m_tabWidthEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::tabWidthUpdateUI), NULL, this);
        m_indentWidthEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::indentWidthUpdateUI), NULL, this);
        m_displayBookmarkMarginEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::displayBookmarkMarginUpdateUI), NULL, this);
        m_checkBoxDisplayFoldMarginEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxDisplayFoldMarginUpdateUI), NULL, this);
        m_checkBoxHideChangeMarkerMarginEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxHideChangeMarkerMarginUpdateUI), NULL, this);
        m_displayLineNumbersEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::displayLineNumbersUpdateUI), NULL, this);
        m_showIndentationGuideLinesEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::showIndentationGuideLinesUpdateUI), NULL, this);
        m_highlightCaretLineEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::highlightCaretLineUpdateUI), NULL, this);
        m_checkBoxTrimLineEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxTrimLineUpdateUI), NULL, this);
        m_checkBoxAppendLFEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxAppendLFUpdateUI), NULL, this);
        m_whitespaceStyleEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::whitespaceStyleUpdateUI), NULL, this);
        m_choiceEOLEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::choiceEOLUpdateUI), NULL, this);
        m_fileEncodingEnable->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::fileEncodingUpdateUI), NULL, this);
        m_button19->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsLocal::OnOK), NULL, this);

        wxString label = title;
        if(level == pLevel_workspace) {
            label = wxT("Workspace local editor preferences");
        } else if(level == pLevel_project) {
            label = wxT("Project local editor preferences");
        }
        SetTitle(label);

        localOptions = new LocalOptionsConfig;
        LocalOptionsConfig pOC(localOptions, node);

        DisplayHigherValues(higherOptions); // Sets the 'global'? defaults, and the enabling checkboxes to disabling
        DisplayLocalValues(localOptions); // Sets any previous 'local' prefs, and the corresponding enabling-checkboxes to enabling

        //WindowManager::Load(this);
        //GetSizer()->Fit(this);
    }
    //--------------------------------------------------------------------------
    EditorSettingsLocal::~EditorSettingsLocal() 
    {
        m_indentsUsesTabsEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::indentsUsesTabsUpdateUI), NULL, this);
        m_tabWidthEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::tabWidthUpdateUI), NULL, this);
        m_indentWidthEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::indentWidthUpdateUI), NULL, this);
        m_displayBookmarkMarginEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::displayBookmarkMarginUpdateUI), NULL, this);
        m_checkBoxDisplayFoldMarginEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxDisplayFoldMarginUpdateUI), NULL, this);
        m_checkBoxHideChangeMarkerMarginEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxHideChangeMarkerMarginUpdateUI), NULL, this);
        m_displayLineNumbersEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::displayLineNumbersUpdateUI), NULL, this);
        m_showIndentationGuideLinesEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::showIndentationGuideLinesUpdateUI), NULL, this);
        m_highlightCaretLineEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::highlightCaretLineUpdateUI), NULL, this);
        m_checkBoxTrimLineEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxTrimLineUpdateUI), NULL, this);
        m_checkBoxAppendLFEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::checkBoxAppendLFUpdateUI), NULL, this);
        m_whitespaceStyleEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::whitespaceStyleUpdateUI), NULL, this);
        m_choiceEOLEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::choiceEOLUpdateUI), NULL, this);
        m_fileEncodingEnable->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsLocal::fileEncodingUpdateUI), NULL, this);
        m_button19->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsLocal::OnOK), NULL, this);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::DisplayHigherValues(const OptionsConfig * options)
    {
        // There should be 'global' (or workspace if this will be a project setting) values for each setting
        // Insert them all, but leave the enabling checkboxes ticked, so the items will be disabled
        m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
        m_indentWidth->SetValue(options->GetIndentWidth());
        m_tabWidth->SetValue(options->GetTabWidth());
        m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
        m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());

        m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
        m_checkBoxTrimLine->SetValue(options->GetTrimLine());
        m_checkBoxAppendLF->SetValue(options->GetAppendLF());

        m_checkBoxHideChangeMarkerMargin->SetValue(options->GetHideChangeMarkerMargin());
        m_checkBoxDisplayFoldMargin->SetValue(options->GetDisplayFoldMargin());
        m_displayBookmarkMargin->SetValue(options->GetDisplayBookmarkMargin());

        const wxString WhitespaceStyle[] = { wxTRANSLATE("Invisible"), wxTRANSLATE("Visible always"),
            wxTRANSLATE("Visible after indentation"), wxTRANSLATE("Indentation only") };
        wxString currentWhitespace;
        switch(options->GetShowWhitspaces()) {
        case wxSTC_WS_VISIBLEALWAYS:
            currentWhitespace = wxT("Visible always");
            break;
        case wxSTC_WS_VISIBLEAFTERINDENT:
            currentWhitespace = wxT("Visible after indentation");
            break;
        default:
            currentWhitespace = wxT("Invisible");
            break;
        }
        m_WSstringManager.AddStrings(
            sizeof(WhitespaceStyle) / sizeof(wxString), WhitespaceStyle, currentWhitespace, m_whitespaceStyle);

        const wxString EOLChoices[] = { wxTRANSLATE("Default"), wxT("Mac (CR)"), wxT("Windows (CRLF)"), wxT("Unix (LF)") };
        m_EOLstringManager.AddStrings(
            sizeof(EOLChoices) / sizeof(wxString), EOLChoices, options->GetEolMode(), m_choiceEOL);

        wxArrayString astrEncodings;
        wxFontEncoding fontEnc;
        int iCurrSelId = 0;
        size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
        for(size_t i = 0; i < iEncCnt; i++) {
            fontEnc = wxFontMapper::GetEncoding(i);
            if(wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                continue;
            }
            astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
            if(fontEnc == options->GetFileFontEncoding()) {
                iCurrSelId = i;
            }
        }
        m_fileEncoding->Append(astrEncodings);
        m_fileEncoding->SetSelection(iCurrSelId);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::DisplayLocalValues(const LocalOptionsConfigPtr options)
    {
        // There will be 'global' values already present for each setting
        // Override this with any valid previously-make local pref, then untick the corresponding enabling checkbox, so that
        // item will be enabled

        if(options->IndentUsesTabsIsValid()) {
            m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
            m_indentsUsesTabsEnable->SetValue(false);
        }
        if(options->IndentWidthIsValid()) {
            m_indentWidth->SetValue(options->GetIndentWidth());
            m_indentWidthEnable->SetValue(false);
        }

        if(options->TabWidthIsValid()) {
            m_tabWidth->SetValue(options->GetTabWidth());
            m_tabWidthEnable->SetValue(false);
        }
        if(options->DisplayLineNumbersIsValid()) {
            m_displayLineNumbers->SetValue(options->GetDisplayLineNumbers());
            m_displayLineNumbersEnable->SetValue(false);
        }
        if(options->ShowIndentationGuidelinesIsValid()) {
            m_showIndentationGuideLines->SetValue(options->GetShowIndentationGuidelines());
            m_showIndentationGuideLinesEnable->SetValue(false);
        }
        if(options->HighlightCaretLineIsValid()) {
            m_highlightCaretLine->SetValue(options->GetHighlightCaretLine());
            m_highlightCaretLineEnable->SetValue(false);
        }
        if(options->TrimLineIsValid()) {
            m_checkBoxTrimLine->SetValue(options->GetHighlightCaretLine());
            m_checkBoxTrimLineEnable->SetValue(false);
        }
        if(options->AppendLFIsValid()) {
            m_checkBoxAppendLF->SetValue(options->GetHighlightCaretLine());
            m_checkBoxAppendLFEnable->SetValue(false);
        }
        if(options->HideChangeMarkerMarginIsValid()) {
            m_checkBoxHideChangeMarkerMargin->SetValue(options->GetHideChangeMarkerMargin());
            m_checkBoxHideChangeMarkerMarginEnable->SetValue(false);
        }
        if(options->DisplayFoldMarginIsValid()) {
            m_checkBoxDisplayFoldMargin->SetValue(options->GetDisplayFoldMargin());
            m_checkBoxDisplayFoldMarginEnable->SetValue(false);
        }
        if(options->DisplayBookmarkMarginIsValid()) {
            m_displayBookmarkMargin->SetValue(options->GetDisplayBookmarkMargin());
            m_displayBookmarkMarginEnable->SetValue(false);
        }

        if(options->ShowWhitespacesIsValid()) {
            switch(options->GetShowWhitespaces()) {
            case wxSTC_WS_VISIBLEALWAYS:
                m_WSstringManager.SetStringSelection(wxT("Visible always"));
                break;
            case wxSTC_WS_VISIBLEAFTERINDENT:
                m_WSstringManager.SetStringSelection(wxT("Visible after indentation"));
                break;
            default:
                m_WSstringManager.SetStringSelection(wxT("Invisible"));
                break;
            }
            m_whitespaceStyleEnable->SetValue(false);
        }

        if(options->EolModeIsValid()) {
            m_EOLstringManager.SetStringSelection(options->GetEolMode());
            m_choiceEOLEnable->SetValue(false);
        }

        if(options->FileFontEncodingIsValid()) {
            wxArrayString entries = m_fileEncoding->GetStrings();
            for(size_t n = 0; n < entries.GetCount(); ++n) {
                if(entries.Item(n) == wxFontMapper::GetEncodingName(options->GetFileFontEncoding())) {
                    m_fileEncoding->SetSelection(n);
                    break;
                }
            }
            m_fileEncodingEnable->SetValue(false);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::OnOK(wxCommandEvent& event)
    {
        // Kill the old LocalOptionsConfigPtr, which now holds stale data
        localOptions.Reset(new LocalOptionsConfig);

        // Assume that, for any items still disabled, the user wants to use the global setting
        // That's true even if he decided to change an item, then disabled it again
        // or if there *was* a local pref, which is now to be deleted
        if(m_indentsUsesTabs->IsEnabled()) {
            GetLocalOpts()->SetIndentUsesTabs(m_indentsUsesTabs->GetValue());
        }
        if(m_indentWidth->IsEnabled()) {
            GetLocalOpts()->SetIndentWidth(m_indentWidth->GetValue());
        }
        if(m_tabWidth->IsEnabled()) {
            GetLocalOpts()->SetTabWidth(m_tabWidth->GetValue());
        }
        if(m_displayLineNumbers->IsEnabled()) {
            GetLocalOpts()->SetDisplayLineNumbers(m_displayLineNumbers->GetValue());
        }
        if(m_showIndentationGuideLines->IsEnabled()) {
            GetLocalOpts()->SetShowIndentationGuidelines(m_showIndentationGuideLines->GetValue());
        }
        if(m_highlightCaretLine->IsEnabled()) {
            GetLocalOpts()->SetHighlightCaretLine(m_highlightCaretLine->GetValue());
        }
        if(m_checkBoxTrimLine->IsEnabled()) {
            GetLocalOpts()->SetTrimLine(m_checkBoxTrimLine->GetValue());
        }
        if(m_checkBoxAppendLF->IsEnabled()) {
            GetLocalOpts()->SetAppendLF(m_checkBoxAppendLF->GetValue());
        }
        if(m_checkBoxHideChangeMarkerMargin->IsEnabled()) {
            GetLocalOpts()->SetHideChangeMarkerMargin(m_checkBoxHideChangeMarkerMargin->GetValue());
        }
        if(m_checkBoxDisplayFoldMargin->IsEnabled()) {
            GetLocalOpts()->SetDisplayFoldMargin(m_checkBoxDisplayFoldMargin->GetValue());
        }
        if(m_displayBookmarkMargin->IsEnabled()) {
            GetLocalOpts()->SetDisplayBookmarkMargin(m_displayBookmarkMargin->GetValue());
        }
        if(m_whitespaceStyle->IsEnabled()) {
            wxString Whitespace = m_WSstringManager.GetStringSelection();
            int style(wxSTC_WS_INVISIBLE); // invisible
            if(Whitespace == wxT("Visible always")) {
                style = wxSTC_WS_VISIBLEALWAYS;
            } else if(Whitespace == wxT("Visible after indentation")) {
                style = wxSTC_WS_VISIBLEAFTERINDENT;
            } else if(Whitespace == wxT("Indentation only")) {
                style = wxSTC_WS_VISIBLEAFTERINDENT;
            }
            GetLocalOpts()->SetShowWhitespaces(style);
        }
        if(m_choiceEOL->IsEnabled()) {
            GetLocalOpts()->SetEolMode(m_EOLstringManager.GetStringSelection());
        }
        if(m_fileEncoding->IsEnabled()) {
            GetLocalOpts()->SetFileFontEncoding(m_fileEncoding->GetStringSelection());
        }

        EndModal(wxID_OK);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::indentsUsesTabsUpdateUI(wxUpdateUIEvent& event)
    {
        m_indentsUsesTabs->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::indentWidthUpdateUI(wxUpdateUIEvent& event)
    {
        m_indentWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
        m_staticTextindentWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::tabWidthUpdateUI(wxUpdateUIEvent& event)
    {
        m_tabWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
        m_staticTexttabWidth->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::displayBookmarkMarginUpdateUI(wxUpdateUIEvent& event)
    {
        m_displayBookmarkMargin->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::checkBoxDisplayFoldMarginUpdateUI(wxUpdateUIEvent& event)
    {
        m_checkBoxDisplayFoldMargin->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::checkBoxHideChangeMarkerMarginUpdateUI(wxUpdateUIEvent& event)
    {
        m_checkBoxHideChangeMarkerMargin->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::displayLineNumbersUpdateUI(wxUpdateUIEvent& event)
    {
        m_displayLineNumbers->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::showIndentationGuideLinesUpdateUI(wxUpdateUIEvent& event)
    {
        m_showIndentationGuideLines->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::highlightCaretLineUpdateUI(wxUpdateUIEvent& event)
    {
        m_highlightCaretLine->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::checkBoxTrimLineUpdateUI(wxUpdateUIEvent& event)
    {
        m_checkBoxTrimLine->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::checkBoxAppendLFUpdateUI(wxUpdateUIEvent& event)
    {
        m_checkBoxAppendLF->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::whitespaceStyleUpdateUI(wxUpdateUIEvent& event)
    {
        m_whitespaceStyle->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
        m_staticTextwhitespaceStyle->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::choiceEOLUpdateUI(wxUpdateUIEvent& event)
    {
        m_choiceEOL->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
        m_EOLstatic->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsLocal::fileEncodingUpdateUI(wxUpdateUIEvent& event)
    {
        m_fileEncoding->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
        m_staticTextfileEncoding->Enable(!((wxCheckBox*)event.GetEventObject())->IsChecked());
    }
    //--------------------------------------------------------------------------
    static const wxString gsCustomTargetsMenu(wxT("Custom Build Targets"));
    //--------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(FileViewTree, DefaultTreeCtrl)
    EVT_TREE_BEGIN_DRAG(wxID_ANY, FileViewTree::OnItemBeginDrag)
    EVT_TREE_END_DRAG(wxID_ANY, FileViewTree::OnItemEndDrag)
    EVT_TREE_ITEM_MENU(wxID_ANY, FileViewTree::OnPopupMenu)
    EVT_TREE_SEL_CHANGED(wxID_ANY, FileViewTree::OnSelectionChanged)

    EVT_MENU(XRCID("local_workspace_prefs"), FileViewTree::OnLocalPrefs)
    EVT_MENU(XRCID("local_workspace_settings"), FileViewTree::OnLocalWorkspaceSettings)
    EVT_MENU(XRCID("remove_project"), FileViewTree::OnRemoveProject)
    EVT_MENU(XRCID("rename_project"), FileViewTree::OnRenameProject)
    EVT_MENU(XRCID("set_as_active"), FileViewTree::OnSetActive)
    EVT_MENU(XRCID("new_item"), FileViewTree::OnNewItem)
    EVT_MENU(XRCID("add_existing_item"), FileViewTree::OnAddExistingItem)
    EVT_MENU(XRCID("new_virtual_folder"), FileViewTree::OnNewVirtualFolder)
    EVT_MENU(XRCID("remove_virtual_folder"), FileViewTree::OnRemoveVirtualFolder)
    EVT_MENU(XRCID("local_project_prefs"), FileViewTree::OnLocalPrefs)
    EVT_MENU(XRCID("project_properties"), FileViewTree::OnProjectProperties)
    EVT_MENU(XRCID("remove_item"), FileViewTree::OnRemoveItem)
    EVT_MENU(XRCID("save_as_template"), FileViewTree::OnSaveAsTemplate)
    EVT_MENU(XRCID("build_order"), FileViewTree::OnBuildOrder)
    EVT_MENU(XRCID("clean_project"), FileViewTree::OnClean)
    EVT_MENU(XRCID("build_project"), FileViewTree::OnBuild)
    EVT_MENU(XRCID("pin_project"), FileViewTree::OnPinProject)
    EVT_MENU(XRCID("rebuild_project"), FileViewTree::OnReBuild)
    EVT_MENU(XRCID("generate_makefile"), FileViewTree::OnRunPremakeStep)
    EVT_MENU(XRCID("stop_build"), FileViewTree::OnStopBuild)
    EVT_MENU(XRCID("retag_project"), FileViewTree::OnRetagProject)
    EVT_MENU(XRCID("build_project_only"), FileViewTree::OnBuildProjectOnly)
    EVT_MENU(XRCID("clean_project_only"), FileViewTree::OnCleanProjectOnly)
    EVT_MENU(XRCID("rebuild_project_only"), FileViewTree::OnRebuildProjectOnly)
    EVT_MENU(XRCID("import_directory"), FileViewTree::OnImportDirectory)
    EVT_MENU(XRCID("reconcile_project"), FileViewTree::OnReconcileProject)
    EVT_MENU(XRCID("open_in_editor"), FileViewTree::OnOpenInEditor)
    EVT_MENU(XRCID("compile_item"), FileViewTree::OnCompileItem)
    EVT_MENU(XRCID("cxx_fileview_open_shell_from_filepath"), FileViewTree::OnOpenShellFromFilePath)
    EVT_MENU(XRCID("cxx_fileview_open_file_explorer"), FileViewTree::OnOpenFileExplorerFromFilePath)
    EVT_MENU(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuild)
    EVT_MENU(XRCID("preprocess_item"), FileViewTree::OnPreprocessItem)
    EVT_MENU(XRCID("rename_item"), FileViewTree::OnRenameItem)
    EVT_MENU(XRCID("rename_virtual_folder"), FileViewTree::OnRenameVirtualFolder)
    EVT_MENU(XRCID("colour_virtual_folder"), FileViewTree::OnSetBgColourVirtualFolder)
    EVT_MENU(XRCID("clear_virtual_folder_colour"), FileViewTree::OnClearBgColourVirtualFolder)
    EVT_MENU(XRCID("open_with_default_application"), FileViewTree::OnOpenWithDefaultApplication)

    EVT_UPDATE_UI(XRCID("remove_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("rename_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("set_as_active"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("new_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("add_existing_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("new_virtual_folder"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("remove_virtual_folder"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("project_properties"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("sort_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("remove_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("export_makefile"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("save_as_template"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("build_order"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("clean_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("build_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("rebuild_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("retag_project"), FileViewTree::OnRetagInProgressUI)
    EVT_UPDATE_UI(XRCID("retag_workspace"), FileViewTree::OnRetagInProgressUI)
    EVT_UPDATE_UI(XRCID("build_project_only"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("clean_project_only"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("rebuild_project_only"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("import_directory"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("reconcile_project"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("compile_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("exclude_from_build"), FileViewTree::OnExcludeFromBuildUI)
    EVT_UPDATE_UI(XRCID("preprocess_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("rename_item"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("generate_makefile"), FileViewTree::OnBuildInProgress)
    EVT_UPDATE_UI(XRCID("local_workspace_settings"), FileViewTree::OnBuildInProgress)
    END_EVENT_TABLE()

    static int PROJECT_IMG_IDX = wxNOT_FOUND;
    static int PROJECT_EXPAND_IMG_IDX = wxNOT_FOUND;
    static int FOLDER_IMG_IDX = wxNOT_FOUND;
    static int WORKSPACE_IMG_IDX = wxNOT_FOUND;
    static int WORKSPACE_EXPANDED_IMG_IDX = wxNOT_FOUND;
    static int ACTIVE_PROJECT_IMG_IDX = wxNOT_FOUND;
    static int FOLDER_EXPAND_IMG_IDX = wxNOT_FOUND;
    static int WORKSPACE_FOLDER_IMG_IDX = wxNOT_FOUND;
    static int WORKSPACE_FOLDER_EXPANDED_IMG_IDX = wxNOT_FOUND;
    //--------------------------------------------------------------------------
    FileViewTree::FileViewTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : DefaultTreeCtrl(parent, id, pos, size, style)
    {
        // Sorting method
        std::function<bool(const wxTreeItemId&, const wxTreeItemId&)> SortFunc = [&](const wxTreeItemId& itemA,
                                                                                     const wxTreeItemId& itemB) {
            FilewViewTreeItemData* a = static_cast<FilewViewTreeItemData*>(GetItemData(itemA));
            FilewViewTreeItemData* b = static_cast<FilewViewTreeItemData*>(GetItemData(itemB));
            if(a->GetData().IsVirtualFolder() && b->GetData().IsFile())
                return true;
            else if(a->GetData().IsFile() && b->GetData().IsVirtualFolder())
                return false;
            // same kind
            return (a->GetData().GetDisplayName().CmpNoCase(b->GetData().GetDisplayName()) < 0);
        };
        SetSortFunction(SortFunc);
        m_colourHelper.Reset(new TreeCtrlColourHelper(this));

        // Initialise images map
        ImageManager* bmpLoader = &gsgs_Image();

        // Prepare the standard mime-type image list
        FOLDER_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeFolder);
        FOLDER_EXPAND_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeFolderExpanded);
        WORKSPACE_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeWorkspace);
        WORKSPACE_EXPANDED_IMG_IDX = WORKSPACE_IMG_IDX;
        ACTIVE_PROJECT_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeProject);

        PROJECT_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeProject);
        PROJECT_EXPAND_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeProjectExpanded);
        WORKSPACE_FOLDER_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeWorkspaceFolder);
        WORKSPACE_FOLDER_EXPANDED_IMG_IDX = bmpLoader->GetMimeImageId(FileUtil::TypeWorkspaceFolderExpanded);
        SetBitmaps(&bmpLoader->GetBitmaps());

        gsgs_Event().Connect(wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree), NULL,
                                      this);
        gsgs_Event().Connect(wxEVT_CMD_BUILD_PROJECT_ONLY,
                                      wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
        gsgs_Event().Connect(wxEVT_CMD_CLEAN_PROJECT_ONLY,
                                      wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CONFIG_CHANGED, &FileViewTree::OnBuildConfigChanged, this);
        gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_SHOWING, &FileViewTree::OnFindInFilesShowing, this);

        Bind(wxEVT_TREE_ITEM_ACTIVATED, &FileViewTree::OnItemActivated, this);
        Bind(wxEVT_TREE_KEY_DOWN, &FileViewTree::OnTreeKeyDown, this);
        Bind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
        Bind(wxEVT_TREE_ITEM_EXPANDING, &FileViewTree::OnItemExpanding, this);
        Bind(wxEVT_TREE_DELETE_ITEM, &FileViewTree::OnItemExpanding, this);

        SetDropTarget(new FileOrFolderDropTarget(this));
        
        //wxFrame* frame = gsgs_Event().TopFrame();
        gsgs_Frame()->Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceNewWorkspaceFolder, this, XRCID("add_workspace_folder"));
        gsgs_Frame()->Bind(wxEVT_MENU, &FileViewTree::OnNewProject, this, XRCID("new_cxx_project"));
        gsgs_Frame()->Bind(wxEVT_MENU, &FileViewTree::OnAddProjectToWorkspaceFolder, this, XRCID("add_existing_cxx_project"));
        BuildTree();
    }
    //--------------------------------------------------------------------------
    FileViewTree::~FileViewTree()
    {
        gsgs_Event().Disconnect(wxEVT_REBUILD_WORKSPACE_TREE, wxCommandEventHandler(FileViewTree::OnBuildTree),
                                         NULL, this);
        gsgs_Event().Disconnect(wxEVT_CMD_BUILD_PROJECT_ONLY,
                                         wxCommandEventHandler(FileViewTree::OnBuildProjectOnlyInternal), NULL, this);
        gsgs_Event().Disconnect(wxEVT_CMD_CLEAN_PROJECT_ONLY,
                                         wxCommandEventHandler(FileViewTree::OnCleanProjectOnlyInternal), NULL, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CONFIG_CHANGED, &FileViewTree::OnBuildConfigChanged, this);
        gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &FileViewTree::OnFindInFilesShowing, this);
        Unbind(wxEVT_DND_FOLDER_DROPPED, &FileViewTree::OnFolderDropped, this);
        Unbind(wxEVT_TREE_ITEM_EXPANDING, &FileViewTree::OnItemExpanding, this);
        Unbind(wxEVT_TREE_DELETE_ITEM, &FileViewTree::OnItemExpanding, this);
        
        //wxFrame* frame = gsgs_Event().TopFrame();
        gsgs_Frame()->Unbind(wxEVT_MENU, &FileViewTree::OnWorkspaceNewWorkspaceFolder, this, XRCID("add_workspace_folder"));
        gsgs_Frame()->Unbind(wxEVT_MENU, &FileViewTree::OnNewProject, this, XRCID("new_cxx_project"));
        gsgs_Frame()->Unbind(wxEVT_MENU, &FileViewTree::OnAddProjectToWorkspaceFolder, this, XRCID("add_existing_cxx_project"));
    }
    //--------------------------------------------------------------------------
    void FileViewTree::BuildTree()
    {
        wxWindowUpdateLocker locker(this);
        Event event(wxEVT_WORKSPACE_VIEW_BUILD_STARTING);
        gsgs_Event().ProcessEvent(event);

        DeleteAllItems();
        m_workspaceFolders.clear();
        m_projectsMap.clear();
        m_excludeBuildFiles.clear();
        
        long flags = GetWindowStyle();
        SetWindowStyle(flags | wxTR_MULTIPLE);

        if(gsgs_Workspace().IsOpen()) 
        {
            wxFont defaultGuiFont = GetDefaultFont();
            // Add an invisible tree root
            ProjectItem data;
            data.m_displayName = gsgs_CppWorker().GetName();
            data.m_kind = ProjectItem::TypeWorkspace;

            wxTreeItemId root =
                AddRoot(data.m_displayName, WORKSPACE_IMG_IDX, WORKSPACE_EXPANDED_IMG_IDX, new FilewViewTreeItemData(data));
            SetItemFont(root, defaultGuiFont);

            wxArrayString list;
            gsgs_Manager()->GetProjectList(list);

            wxArrayString folders;
            gsgs_CppWorker().GetWorkspaceFolders(folders);
            for(size_t i = 0; i < folders.size(); ++i) {
                AddWorkspaceFolder(folders.Item(i));
            }

            for(size_t n = 0; n < list.GetCount(); n++) {
                BuildProjectNode(list.Item(n));
            }

            // set selection to first item
            SelectItem(root, HasFlag(wxTR_MULTIPLE) ? false : true);
        }

        // Always expand to the active project
        ExpandToPath(gsgs_CppWorker().GetActiveProjectName(), mLastCreateFile);
        mLastCreateFile = "";
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::GetSingleSelection() 
    { 
        return GetFocusedItem(); 
    }
    //--------------------------------------------------------------------------
    int FileViewTree::GetIconIndex(const ProjectItem& item)
    {
        ImageManager* bmpLoader = &gsgs_Image();
        int icondIndex(bmpLoader->GetMimeImageId(FileUtil::TypeText));
        switch(item.GetKind()) {
        case ProjectItem::TypeProject:
            icondIndex = PROJECT_IMG_IDX;
            break;
        case ProjectItem::TypeVirtualDirectory:
            icondIndex = FOLDER_IMG_IDX;
            break;
        case ProjectItem::TypeFile: {
            wxFileName filename(item.GetFile());
            icondIndex = bmpLoader->GetMimeImageId(filename.GetFullName());
            if(icondIndex == wxNOT_FOUND) icondIndex = bmpLoader->GetMimeImageId(FileUtil::TypeText);
        } break;
        case ProjectItem::TypeWorkspaceFolder:
            icondIndex = WORKSPACE_FOLDER_IMG_IDX;
            break;
        default:
            break;
        }
        return icondIndex;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::BuildProjectNode(const wxString& projectName)
    {
        wxString err_msg;
        ProjectPtr prj = gsgs_CppWorker().FindProjectByName(projectName, err_msg);

        // Add the folder containing this project
        wxTreeItemId rootItem = AddWorkspaceFolder(prj->GetWorkspaceFolder());

        FolderColour::Map_t coloursMap;
        FolderColour::List_t coloursList;
        gsgs_CppWorker().GetLocalWorkspace()->GetFolderColours(coloursMap);

        // Sort the list
        FolderColour::SortToList(coloursMap, coloursList);

        // We only the project node
        bool iconFromPlugin = false;
        int projectIconIndex = wxNOT_FOUND;
        DoGetProjectIconIndex(projectName, projectIconIndex, iconFromPlugin);

        ProjectItem item(projectName, projectName, prj->GetFileName().GetFullPath(), ProjectItem::TypeProject);
        wxTreeItemId hti = AppendItem(rootItem,               // parent
                                      projectName,            // display name
                                      projectIconIndex,       // item image index
                                      PROJECT_EXPAND_IMG_IDX, // selected item image
                                      new FilewViewTreeItemData(item));
        DoSetItemBackgroundColour(hti, coloursList, item);
        m_projectsMap.insert({ projectName, hti });

        // If the project has children (either VD or files) add a dummy item
        // So the user can "Expand" it
        if(!prj->IsEmpty()) {
            AppendItem(hti, "<dummy>", -1, -1,
                       new FilewViewTreeItemData(ProjectItem("", "", "", ProjectItem::TypeInvalid)));
        }

        if(projectName == gsgs_Manager()->GetActiveProjectName()) {
            //wxFont f = GetDefaultActiveFont();
            //f.SetWeight(wxFONTWEIGHT_BOLD);
            //f.SetStyle(wxFONTSTYLE_ITALIC);
            SetItemFont(hti, GetDefaultActiveFont());
            SetItemTextColour(hti, gsgs_Style().GetHoverMarkerColour());
            if(!iconFromPlugin) { SetItemImage(hti, ACTIVE_PROJECT_IMG_IDX, PROJECT_EXPAND_IMG_IDX); }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ShowFileContextMenu()
    {
        wxArrayTreeItemIds items;
        GetSelections(items);
        if(items.IsEmpty()) return;

        wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("file_tree_file"));
        if(!gsgs_Manager()->IsBuildInProgress()) {
            // Let the plugins alter it
            ContextMenuEvent event(wxEVT_CONTEXT_MENU_FILE);
            event.SetMenu(menu);

            wxArrayString files;
            for(size_t i = 0; i < items.GetCount(); ++i) {
                FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(items.Item(i)));
                if(data->GetData().GetKind() == ProjectItem::TypeFile) { files.Add(data->GetData().GetFile()); }
            }
            event.SetStrings(files);
            gsgs_Event().ProcessEvent(event);

            // Let the plugin hook their content (using the deprecated way)
            PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_File);
        }

        PopupMenu(menu);
        wxDELETE(menu);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ShowVirtualFolderContextMenu(FilewViewTreeItemData* itemData)
    {
        wxMenu* menu = wxXmlResource::Get()->LoadMenu("file_tree_folder");
        int where = FindMenuItemPosition(menu, XRCID("clear_virtual_folder_colour"));
        if(where != wxNOT_FOUND) {
            // Insert the "Set Custom Background Colour" menu entry. We do it like this
            // instead of using the menu.xrc file cause there is a bug that a bitmap can
            // not be set to an already existing menu item
            wxMenuItem* menuItem =
                new wxMenuItem(menu, XRCID("colour_virtual_folder"), _("Set Custom Background Colour..."));
            menuItem->SetBitmap(gsgs_Image().LoadBitmap("colour-pallette"));
            menu->Insert(where + 1, menuItem);
        }

        if(!gsgs_Manager()->IsBuildInProgress()) {
            // Let the plugins alter it
            ContextMenuEvent event(wxEVT_CONTEXT_MENU_VIRTUAL_FOLDER);
            event.SetMenu(menu);
            gsgs_Event().ProcessEvent(event);

            // Let the plugin hook their content (using the deprecated way)
            PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Folder);
        }

        PopupMenu(menu);
        wxDELETE(menu);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ShowProjectContextMenu(const wxString& projectName)
    {
        wxMenu menu;
        CreateProjectContextMenu(menu, projectName);
        PopupMenu(&menu);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ShowWorkspaceContextMenu()
    {
        // Load the basic menu
        wxMenu* menu = wxXmlResource::Get()->LoadMenu(wxT("workspace_popup_menu"));
        if(!gsgs_Manager()->IsBuildInProgress()) {
            // Let the plugins alter it
            ContextMenuEvent event(wxEVT_CONTEXT_MENU_WORKSPACE);
            event.SetMenu(menu);
            gsgs_Event().ProcessEvent(event);

            // Use the old system
            PluginManager::Get()->HookPopupMenu(menu, MenuTypeFileView_Workspace);
        }

        // Show it
        PopupMenu(menu);
        wxDELETE(menu);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnPopupMenu(wxTreeEvent& event)
    {
        if(event.GetItem().IsOk()) {
            wxTreeItemId item = event.GetItem();
            if(item.IsOk()) {
                FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
                switch(data->GetData().GetKind()) {
                case ProjectItem::TypeProject:
                    ShowProjectContextMenu(data->GetData().GetDisplayName());
                    break;
                case ProjectItem::TypeVirtualDirectory:
                    ShowVirtualFolderContextMenu(data);
                    break;
                case ProjectItem::TypeFile:
                    ShowFileContextMenu();
                    break;
                case ProjectItem::TypeWorkspace:
                    ShowWorkspaceContextMenu();
                    break;
                case ProjectItem::TypeWorkspaceFolder:
                    ShowWorkspaceFolderContextMenu();
                    break;
                default:
                    break;
                }
            }
        } else {
            PopupMenu(wxXmlResource::Get()->LoadMenu(wxT("file_view_empty")));
        }
    }
    //--------------------------------------------------------------------------
    TreeItemInfo FileViewTree::GetSelectedItemInfo()
    {
        wxTreeItemId item = GetSingleSelection();
        TreeItemInfo info;
        info.m_item = item;
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));

            // set the text of the item
            info.m_text = GetItemText(item);
            info.m_itemType = data->GetData().GetKind();
            info.m_fileName = data->GetData().GetFile();
            if(info.m_itemType == ProjectItem::TypeVirtualDirectory) {
                // in-case of virtual directories, set the file name to be the directory of
                // the project
                wxString path = GetItemPath(item);
                wxString project = path.BeforeFirst(wxT(':'));
                info.m_fileName = wxFileName(gsgs_Manager()->GetProjectCwd(project), wxEmptyString);
            }
        }
        return info;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoItemActivated(wxTreeItemId& item, wxEvent& event)
    {
        //-----------------------------------------------------
        // Each tree items, keeps a private user data that
        // holds the key for searching the its corresponding
        // node in the m_tree data structure
        //-----------------------------------------------------
        if(item.IsOk() == false) return;
        FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(!itemData) {
            event.Skip();
            return;
        }

        // if file item was hit, open it
        if(itemData->GetData().GetKind() == ProjectItem::TypeFile) {

            wxString filename = itemData->GetData().GetFile();
            wxString key = itemData->GetData().Key();
            wxString project;
            if(key.GetChar(0) == ':') {
                // All the entries I've tested have started with a : so exclude this one, otherwise the project is always ""
                project = key.AfterFirst(':').BeforeFirst(wxT(':'));
            } else {
                project = key.BeforeFirst(wxT(':'));
            }

            // Convert the file name to be in absolute path
            wxFileName fn(filename);
            fn.MakeAbsolute(gsgs_Manager()->GetProjectCwd(project));

            // send event to the plugins to see if they want the file opening in another way
            wxString file_path = fn.GetFullPath();
            Event activateEvent(wxEVT_TREE_ITEM_FILE_ACTIVATED);
            activateEvent.SetFileName(file_path);
            if(gsgs_Event().ProcessEvent(activateEvent)) return;

            gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);

        } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
            // make it active
            DoSetProjectActive(item);
        } else {
            event.Skip();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnOpenInEditor(wxCommandEvent& event)
    {
        wxArrayTreeItemIds items;
        size_t num = GetMultiSelection(items);
        for(size_t i = 0; i < num; i++) {
            wxTreeItemId item = items.Item(i);
            FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
                wxString filename = itemData->GetData().GetFile();
                wxString project = itemData->GetData().Key().BeforeFirst(wxT(':'));

                // Convert the file name to an absolute path
                wxFileName fn(filename);
                fn.MakeAbsolute(gsgs_Manager()->GetProjectCwd(project));

                // DON'T ask the plugins if they want the file opening in another way, as happens from a double-click
                // Here we _know_ the user wants to open in CL
                gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), project, -1);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRemoveProject(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeProject) { DoRemoveProject(data->GetData().GetDisplayName()); }
        }
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::AddFilesToVirtualFolder(const wxString& vdFullPath, wxArrayString& paths)
    {
        wxArrayString actualAdded;
        gsgs_Manager()->AddFilesToProject(paths, vdFullPath, actualAdded);

        // locate the item
        wxTreeItemId item = ItemByFullPath(vdFullPath);
        if(item.IsOk()) {
            for(size_t i = 0; i < actualAdded.Count(); i++) {

                // Add the tree node
                wxFileName fnFileName(actualAdded.Item(i));
                wxString path(vdFullPath);
                path += wxT(":");
                path += fnFileName.GetFullName();
                ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

                wxTreeItemId hti = AppendItem(item,                      // parent
                                              projItem.GetDisplayName(), // display name
                                              GetIconIndex(projItem),    // item image index
                                              GetIconIndex(projItem),    // selected item image
                                              new FilewViewTreeItemData(projItem));
                wxUnusedVar(hti);
            }

            Expand(item);
            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::AddFilesToVirtualFolderIntelligently(const wxString& vdFullPath, wxArrayString& paths)
    {
        // Try to put .cpp files in a :src and .h files in a :include dir
        // This should only happen if :src and :include are terminal subdirs of vdFullPath itself, not distant cousins
        // Note: This function is only used atm to place a pair of cpp/h files, so I'm not checking 'paths'.
        // If you use it for anything else in the future...

        // Check first for vdFullPath being one of the subdirs:
        wxString basename, srcname, includename;
        if(!vdFullPath.EndsWith(wxT(":src"), &basename)) { vdFullPath.EndsWith(wxT(":include"), &basename); }
        if(basename.empty()) {
            basename = vdFullPath; // If not, assume that we were passed the parent folder
        }

        // Either way, basename should now contain the putative parent of src and include
        // Check by getting treeitemids for all 3
        wxTreeItemId parentitem = ItemByFullPath(basename);
        if(!parentitem.IsOk()) { return false; }
        wxTreeItemId srcitem = DoGetItemByText(parentitem, wxT("src"));
        wxTreeItemId includeitem = DoGetItemByText(parentitem, wxT("include"));
        if(!(srcitem.IsOk() && includeitem.IsOk())) {
            return false; // The alleged parent folder doesn't have a relevant matching pair of children
        }

        // We're winning. Now it's just a matter of putting the cpp file into :src, etc
        wxArrayString cppfiles, hfiles;
        for(int c = (int)paths.GetCount() - 1; c >= 0; --c) {
            wxString file = paths.Item(c);
            if(file.Right(4) == wxT(".cpp")) {
                cppfiles.Add(file);
                paths.RemoveAt(c);
            } else if((file.Right(2) == wxT(".h")) || (file.Right(4) == wxT(".hpp"))) {
                hfiles.Add(file);
                paths.RemoveAt(c);
            }
        }
        // Finally do the Adds
        AddFilesToVirtualFolder(basename + wxT(":src"), cppfiles);
        AddFilesToVirtualFolder(basename + wxT(":include"), hfiles);
        // There shouldn't have been any other files passed; but if there were, add them to the selected folder
        if(paths.GetCount()) { AddFilesToVirtualFolder(vdFullPath, paths); }

        return true;
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::AddFilesToVirtualFolder(wxTreeItemId& item, wxArrayString& paths)
    {
        if(item.IsOk() == false) return false;

        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        switch(data->GetData().GetKind()) {
        case ProjectItem::TypeVirtualDirectory:
            // OK
            break;
        default:
            return false;
        }

        wxArrayString actualAdded;
        wxString vdPath = GetItemPath(item);
        wxString project;
        project = vdPath.BeforeFirst(wxT(':'));
        gsgs_Manager()->AddFilesToProject(paths, vdPath, actualAdded);
        for(size_t i = 0; i < actualAdded.Count(); i++) {

            // Add the tree node
            wxFileName fnFileName(actualAdded.Item(i));
            wxString path(vdPath);
            path += wxT(":");
            path += fnFileName.GetFullName();
            ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

            wxTreeItemId hti = AppendItem(item,                      // parent
                                          projItem.GetDisplayName(), // display name
                                          GetIconIndex(projItem),    // item image index
                                          GetIconIndex(projItem),    // selected item image
                                          new FilewViewTreeItemData(projItem));
            wxUnusedVar(hti);
        }

        Expand(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        return true;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnAddExistingItem(wxCommandEvent& WXUNUSED(event))
    {
        wxString start_path(wxEmptyString);
        wxTreeItemId item = GetSingleSelection();
        if (item.IsOk())
        {
            const wxString ALL(
                wxT("All Files (*)|*|") wxT("C/C++ Source Files (*.c;*.cpp;*.cxx;*.cc)|*.c;*.cpp;*.cxx;*.cc|")
                wxT("C/C++ Header Files (*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc)|*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"));

            wxString vdPath = GetItemPath(item);
            wxString project, vd;
            project = vdPath.BeforeFirst(wxT(':'));
            vd = vdPath.AfterFirst(wxT(':'));

            ProjectPtr proj = gsgs_Manager()->GetProject(project);
            start_path = proj->GetBestPathForVD(vd);

            wxArrayString paths;
            wxFileDialog dlg(this, _("Add Existing Item"), start_path, wxEmptyString, ALL,
                wxFD_MULTIPLE | wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (dlg.ShowModal() == wxID_OK) {
                dlg.GetPaths(paths);

                if (paths.IsEmpty() == false) {
                    // keep the last used path
                    wxFileName fn(paths.Item(0));
                    start_path = fn.GetPath();
                }
                AddFilesToVirtualFolder(item, paths);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnNewItem(wxCommandEvent& WXUNUSED(event))
    {
        wxTreeItemId item = GetSingleSelection();
        if (item.IsOk())
        {
            wxString path = GetItemPath(item);

            // Project name
            wxString projName = path.BeforeFirst(wxT(':'));
            wxString vd = path.AfterFirst(wxT(':'));
            wxString projCwd;
            ProjectPtr project = gsgs_Manager()->GetProject(projName);
            if (project) { projCwd = project->GetBestPathForVD(vd); }

            NewItemDlg dlg(gsgs_Frame(), projCwd);
            dlg.SetTitle(_("New Item"));
            if (dlg.ShowModal() == wxID_OK) { DoAddNewItem(item, dlg.GetFileName().GetFullPath(), path); }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnSetActive(wxCommandEvent& WXUNUSED(event))
    {
        wxTreeItemId item = GetSingleSelection();
        DoSetProjectActive(item);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoSetProjectActive(wxTreeItemId& item)
    {
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeProject) {
                UnselectAllProject(); // Clear any previously marked item
                gsgs_Manager()->SetActiveProject(data->GetData().GetDisplayName());
                //wxFont f = GetDefaultFont();
                //f.SetStyle(wxFONTSTYLE_ITALIC);
                //f.SetWeight(wxFONTWEIGHT_BOLD);
                SetItemFont(item, GetDefaultActiveFont());
                SetItemTextColour(item, gsgs_Style().GetHoverMarkerColour());
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRemoveVirtualFolder(wxCommandEvent& WXUNUSED(event))
    {
        wxTreeItemId item = GetSingleSelection();
        DoRemoveVirtualFolder(item);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRemoveItem(wxCommandEvent& WXUNUSED(event)) { DoRemoveItems(); }
    //--------------------------------------------------------------------------
    void FileViewTree::DoRemoveItems()
    {
        wxArrayTreeItemIds items;
        size_t num = GetMultiSelection(items);
        if(!num) { return; }

        bool ApplyToEachFileRemoval = false;
        bool ApplyToEachFileDeletion = false;
        bool AlsoDeleteFromDisc = false;

        // We need to get the item names and data first. Why? Because if they're projects and multiple,
        // removing the second one segs in GetItemText(item) or data->GetData() as 'item' is no longer valid.
        // Why doesn't this happen for files and VDs too? Good question :/
        wxArrayString namearray;
        wxArrayInt kindarray;
        std::vector<FilewViewTreeItemData*> itemdata;
        for(size_t i = 0; i < num; i++) {
            wxTreeItemId item = items.Item(i);
            if(item.IsOk()) {
                namearray.Add(GetItemText(item));
                FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
                itemdata.push_back(data);
                // Store the kind too, as this would become an invalid value
                kindarray.Add(data->GetData().GetKind());
            }
        }

        wxArrayString filesRemoved;
        wxString affectedProject;
        for(size_t i = 0; i < num; i++) {
            wxTreeItemId item = items.Item(i);
            if(!item.IsOk()) { continue; }

            wxString name = namearray.Item(i);
            FilewViewTreeItemData* data = itemdata.at(i);

            if(data) {
                switch(kindarray.Item(i)) {
                case ProjectItem::TypeFile: {
                    int result = wxID_YES;
                    if(ApplyToEachFileRemoval == false) {
                        wxString message;
                        message << _("Are you sure you want remove '") << name << wxT("' ?");
                        if((num > 1) && ((i + 1) < num)) {

                            // For multiple selections, use a YesToAll dialog
                            wxRichMessageDialog dlg(wxTheApp->GetTopWindow(), message, _("Confirm"),
                                                    wxYES_NO | wxYES_DEFAULT | wxCANCEL | wxCENTER | wxICON_QUESTION);
                            dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                            result = dlg.ShowModal();
                            ApplyToEachFileRemoval = dlg.IsCheckBoxChecked();

                        } else {
                            result = wxMessageBox(message, _("Are you sure?"), wxYES_NO | wxICON_QUESTION, this);
                        }
                    }
                    if(result == wxID_CANCEL || (result == wxID_NO && ApplyToEachFileRemoval == true)) {
                        return; // Assume Cancel or No+ApplyToEachFileRemoval means for folders etc too, not just files
                    }
                    if(result == wxID_YES || result == wxYES) {
                        wxTreeItemId parent = GetItemParent(item);
                        if(parent.IsOk()) {
                            wxString path = GetItemPath(parent);

                            // Remove the file. Do not fire an event here, we will send a "bulk" event
                            // with a list of all files removed
                            wxString fullpathOfFileRemoved;
                            gsgs_Debug("File removed %s", path);
                            if(affectedProject.IsEmpty()) { affectedProject = path.BeforeFirst(wxT(':')); }

                            if(gsgs_Manager()->RemoveFile(data->GetData().GetFile(), path, fullpathOfFileRemoved,
                                                            false)) {
                                filesRemoved.Add(fullpathOfFileRemoved);
                            }

                            wxString file_name(data->GetData().GetFile());
                            Delete(item);
                            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);

                            int DeleteThisItemFromDisc = false;
                            if(ApplyToEachFileDeletion == false) {
                                wxString message;
                                message << _("Do you also want to delete the file '") << name << _("' from disc?");
                                if((num > 1) && ((i + 1) < num)) {
                                    // For multiple selections, use a YesToAll dialog
                                    wxRichMessageDialog dlg(gsgs_Event().TopFrame(), message, _("Confirm"),
                                                            wxYES_NO | wxNO_DEFAULT | wxCANCEL | wxCENTER |
                                                                wxICON_QUESTION);
                                    dlg.ShowCheckBox(_("Remember my answer and apply it all files"), false);
                                    DeleteThisItemFromDisc = dlg.ShowModal();
                                    ApplyToEachFileDeletion = dlg.IsCheckBoxChecked();
                                } else {
                                    DeleteThisItemFromDisc = ::PromptMessageDialog(
                                        message, "fileview_del_file_from_disc", _("Yes"), _("No"), _("Cancel"),
                                        _("Remember my answer and don't ask me again"),
                                        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
                                }
                            }

                            if((DeleteThisItemFromDisc == wxID_YES || DeleteThisItemFromDisc == wxYES) ||
                               AlsoDeleteFromDisc) {
                                AlsoDeleteFromDisc = ApplyToEachFileDeletion; // If we're here, ApplyToAll means delete all

                                wxString message(_("An error occurred during file removal. Maybe it has been already "
                                                   "deleted or you don't have the necessary permissions"));
                                if(wxDirExists(name)) {
                                    if(!wxFileName::Rmdir(name, wxPATH_RMDIR_RECURSIVE)) {
                                        wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                    } else {
                                        // Folder was removed from the disc, notify about it
                                        FileEvent rmEvent(wxEVT_FOLDER_DELETED);
                                        rmEvent.GetPaths().Add(name);
                                        rmEvent.SetEventObject(this);
                                        gsgs_Event().AddPendingEvent(rmEvent);
                                    }
                                } else {
                                    if(wxFileName::FileExists(file_name)) {
                                        if(!gsgsRemoveFile(file_name)) {
                                            wxMessageBox(message, _("Error"), wxOK | wxICON_ERROR, this);
                                        } else {
                                            // File was removed from the disc, notify about it
                                            FileEvent rmEvent(wxEVT_FILE_DELETED);
                                            rmEvent.GetPaths().Add(file_name);
                                            rmEvent.SetEventObject(this);
                                            gsgs_Event().AddPendingEvent(rmEvent);
                                        }
                                    }
                                }
                            }
                        }
                    }
                } break;
                case ProjectItem::TypeVirtualDirectory:
                    DoRemoveVirtualFolder(item);
                    break;
                case ProjectItem::TypeProject:
                    DoRemoveProject(name);
                    break;
                default:
                    break;
                }
            }
        }

        // Notify plugins if we actually removed files
        if(!filesRemoved.IsEmpty()) {
            Event evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
            evtFileRemoved.SetStrings(filesRemoved);   // list of files removed
            evtFileRemoved.SetString(affectedProject); // the affected project
            evtFileRemoved.SetEventObject(this);
            gsgs_Event().ProcessEvent(evtFileRemoved);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoRemoveVirtualFolder(wxTreeItemId& item)
    {
        wxString name = GetItemText(item);
        wxString message(wxT("'") + name + wxT("'"));
        message << _(" and all its contents will be removed from the project.");

        if(wxMessageBox(message, _("光栅故事"), wxYES_NO | wxICON_WARNING) == wxYES) {
            wxString path = GetItemPath(item);
            gsgs_Manager()->RemoveVirtualDirectory(path);

            DeleteChildren(item);
            Delete(item);
            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnNewVirtualFolder(wxCommandEvent& WXUNUSED(event))
    {
        static int count = 0;
        wxString defaultName(wxT("NewDirectory"));
        defaultName << count++;

        wxTreeItemId item = GetSingleSelection();
        NewVirtualFolderDlg dlg(gsgs_Frame(), GetItemPath(item));
        if(dlg.ShowModal() == wxID_OK) {
            DoAddVirtualFolder(item, dlg.GetName());
            if(dlg.GetCreateOnDisk()) {
                // Create the path on the file system, but don't complain if it is already there
                wxFileName::Mkdir(dlg.GetDiskPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
        }
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::DoAddVirtualFolder(wxTreeItemId& parent, const wxString& text)
    {
        wxString path = GetItemPath(parent) + wxT(":") + text;

        // Virtual directory already exists?
        if(gsgs_Manager()->AddVirtualDirectory(path, true) == Manager::VD_EXISTS) 
            return wxTreeItemId();

        wxTreeItemId item;
        ProjectItem itemData(path, text, wxEmptyString, ProjectItem::TypeVirtualDirectory);
        item = AppendItem(parent,                    // parent
                          itemData.GetDisplayName(), // display name
                          GetIconIndex(itemData),    // item image index
                          GetIconIndex(itemData),    // selected item image
                          new FilewViewTreeItemData(itemData));

        Expand(parent);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        return item;
    }
    //--------------------------------------------------------------------------
    wxString FileViewTree::GetItemPath(const wxTreeItemId& item, const wxChar& sep) const
    {
        std::deque<wxString> queue;
        wxString text = GetItemText(item);
        queue.push_front(text);

        wxTreeItemId p = GetItemParent(item);
        while(true) {

            if(!p.IsOk() || p == GetRootItem()) break;

            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(p));
            if(!data) break;

            if(data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) {
                // We reached the top level
                break;
            }

            text = GetItemText(p);
            queue.push_front(text);
            p = GetItemParent(p);
        }

        wxString path;
        size_t count = queue.size();
        for(size_t i = 0; i < count; i++) {
            path += queue.front();
            path += sep;
            queue.pop_front();
        }

        if(!queue.empty()) {
            path += queue.front();
        } else {
            path = path.BeforeLast(sep);
        }

        return path;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnLocalPrefs(wxCommandEvent& event)
    {
        if(!gsgs_Workspace().IsOpen()) {
            return; // Probably not possible, but...
        }

        wxXmlNode* lwsnode = gsgs_CppWorker().GetLocalWorkspace()->GetLocalWorkspaceOptionsNode();
        // Don't check lwsnode: it'll be NULL if there are currently no local workspace options

        // Start by getting the global settings
        OptionsConfig * higherOptions = gsgs_EditorConfig().GetOptions();

        // If we're setting workspace options, run the dialog and return
        if(event.GetId() == XRCID("local_workspace_prefs")) {
            EditorSettingsLocal dlg(higherOptions, lwsnode, pLevel_workspace, this);
            if(dlg.ShowModal() == wxID_OK &&
               gsgs_CppWorker().GetLocalWorkspace()->SetWorkspaceOptions(dlg.GetLocalOpts())) {
                gsgs_Frame()->GetMainBook()->ApplySettings();
                // Notify plugins that some settings have changed
                PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
            }
            return;
        }

        // Otherwise we're getting project prefs
        wxTreeItemId item = GetSingleSelection();
        if(!item.IsOk()) { return; }

        wxXmlNode* lpnode = gsgs_CppWorker().GetLocalWorkspace()->GetLocalProjectOptionsNode(GetItemText(item));
        // Don't check lpnode: it'll be NULL if there are currently no local project options
        // Merge any local workspace options with the global ones inside 'higherOptions'
        LocalOptionsConfig wsOC(higherOptions, lwsnode);

        EditorSettingsLocal dlg(higherOptions, lpnode, pLevel_project, this);
        if(dlg.ShowModal() == wxID_OK &&
           gsgs_CppWorker().GetLocalWorkspace()->SetProjectOptions(dlg.GetLocalOpts(), GetItemText(item))) {
            gsgs_Frame()->GetMainBook()->ApplySettings();
            // Notify plugins that some settings have changed
            PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnProjectProperties(wxCommandEvent& WXUNUSED(event))
    {
        ProjectPtr p = GetSelectedProject();
        if(p) { gsgs_Frame()->GetWorkspaceTab()->OpenProjectSettings(p->GetName()); }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoRemoveProject(const wxString& name)
    {
        wxString message(_("You are about to remove project '"));
        message << name << wxT("' ");
        message << _(" from the workspace, click 'Yes' to proceed or 'No' to abort.");
        if(wxMessageBox(message, _("Confirm"), wxYES_NO) == wxYES) {
            gsgs_Manager()->RemoveProject(name, true);

            // Remove the project from the cache
            if(m_projectsMap.count(name)) { m_projectsMap.erase(name); }
        }
    }
    //--------------------------------------------------------------------------
    int FileViewTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
    {
        // used for SortChildren, reroute to our sort routine
        FilewViewTreeItemData *a = (FilewViewTreeItemData*)GetItemData(item1),
                              *b = (FilewViewTreeItemData*)GetItemData(item2);
        if(a && b) return OnCompareItems(a, b);

        return 0;
    }
    //--------------------------------------------------------------------------
    int FileViewTree::OnCompareItems(const FilewViewTreeItemData* a, const FilewViewTreeItemData* b)
    {
        // if dir and other is not, dir has preference
        if(a->GetData().GetKind() == ProjectItem::TypeVirtualDirectory && b->GetData().GetKind() == ProjectItem::TypeFile)
            return -1;
        else if(b->GetData().GetKind() == ProjectItem::TypeVirtualDirectory &&
                a->GetData().GetKind() == ProjectItem::TypeFile)
            return 1;

        // else let ascii fight it out
        return a->GetData().GetDisplayName().CmpNoCase(b->GetData().GetDisplayName());
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnSaveAsTemplate(wxCommandEvent& WXUNUSED(event))
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString name = GetItemText(item);
            ProjectPtr proj = gsgs_Manager()->GetProject(name);
            if(proj) {
                NameAndDescDlg dlg(gsgs_Frame(), PluginManager::Get(), name);
                if(dlg.ShowModal() == wxID_OK) {
                    wxString newName = dlg.GetName();
                    wxString desc = dlg.GetDescription();
                    wxString type = dlg.GetType();

                    newName = newName.Trim().Trim(false);
                    desc = desc.Trim().Trim(false);

                    if(!newName.IsEmpty()) {

                        // Copy the user template to the user folder
                        wxFileName userTemplates(gsgs_Path().GetUserDataDir(), "");
                        userTemplates.AppendDir("templates");
                        userTemplates.AppendDir("projects");
                        userTemplates.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

                        Project newProj(*proj);
                        newProj.SetProjectInternalType(type);
                        newProj.CopyTo(userTemplates.GetPath(), newName, desc);
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildOrder(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            DependenciesDlg dlg(gsgs_Frame(), GetItemText(item));
            dlg.ShowModal();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnClean(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            wxString conf;

            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }
            QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);

            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Clean"));
            }
            gsgs_Manager()->PushQueueCommand(buildInfo);
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuild(wxCommandEvent &)
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);

            wxString conf;
            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            QueueCommand buildInfo(projectName, conf, false, QueueCommand::kBuild);
            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Build"));
            }
            gsgs_Manager()->PushQueueCommand(buildInfo);
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnCompileItem(wxCommandEvent &)
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                Manager* mgr = gsgs_Manager();
                wxTreeItemId parent = GetItemParent(item);
                if(parent.IsOk()) {
                    wxString logmsg;
                    wxString path = GetItemPath(parent);
                    wxString proj = path.BeforeFirst(wxT(':'));
                    logmsg << _("Compiling file: ") << data->GetData().GetFile() << _(" of project ") << proj << wxT("\n");
                    mgr->CompileFile(proj, data->GetData().GetFile());
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnPreprocessItem(wxCommandEvent &)
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                Manager* mgr = gsgs_Manager();
                wxTreeItemId parent = GetItemParent(item);
                if(parent.IsOk()) {
                    wxString logmsg;
                    wxString path = GetItemPath(parent);
                    wxString proj = path.BeforeFirst(wxT(':'));
                    logmsg << _("Preprocessing file: ") << data->GetData().GetFile() << _(" of project ") << proj
                           << wxT("\n");
                    mgr->CompileFile(proj, data->GetData().GetFile(), true);
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnStopBuild(wxCommandEvent &)
    {
        gsgs_Manager()->StopBuild();
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnItemActivated(wxTreeEvent& event)
    {
        wxTreeItemId sle = event.GetItem();
        if (!(ItemHasChildren(sle) && IsExpanded(sle))) {
            event.Skip();
        }
        wxArrayTreeItemIds items;
        if(GetSelections(items)) {
            for(size_t i = 0; i < items.size(); ++i) {
                wxTreeItemId item = items.Item(i);
                DoItemActivated(item, event);
            }
        }
    }
    //--------------------------------------------------------------------------
    size_t FileViewTree::GetMultiSelection(wxArrayTreeItemIds& arr)
    {
        if(HasFlag(wxTR_MULTIPLE)) {
            // we are using multiple selection tree
            return GetSelections(arr);
        } else {
            wxTreeItemId sel = GetSelection();
            if(sel.IsOk()) {
                arr.Add(sel);
                return 1;
            }
            arr.Clear();
            return 0;
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRetagProject(wxCommandEvent &)
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            gsgs_Manager()->RetagProject(projectName, true);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRetagWorkspace(wxCommandEvent &)
    {
        gsgs_Manager()->RetagWorkspace(TagsManager::Retag_Quick);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildInProgress(wxUpdateUIEvent& event)
    {
        event.Enable(!gsgs_Manager()->IsBuildInProgress());
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnItemBeginDrag(wxTreeEvent& event)
    {
        wxArrayTreeItemIds selections;
        size_t num = GetMultiSelection(selections);

        // We allow dragging of homogenous items (all files or all projects)
        m_draggedFiles.Clear();
        m_draggedProjects.Clear();
        for(size_t n = 0; n < num; ++n) {
            wxTreeItemId item = selections[n];
            if(item.IsOk() && item != GetRootItem()) {
                // If it's a file, add it to the array
                FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
                if(data && data->GetData().GetKind() == ProjectItem::TypeFile) {
                    m_draggedFiles.Add(item);
                } else if(data && data->GetData().GetKind() == ProjectItem::TypeProject) {
                    m_draggedProjects.Add(item);
                }
            }
        }

        // Allow the event only if there were any valid selections
        if(m_draggedFiles.IsEmpty() && !m_draggedProjects.IsEmpty()) {
            // only projects are being dragged
            event.Allow();
        } else if(!m_draggedFiles.IsEmpty() && m_draggedProjects.IsEmpty()) {
            // only files are being dragged
            event.Allow();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnItemEndDrag(wxTreeEvent& event)
    {
        wxTreeItemId itemDst = event.GetItem();
        CHECK_ITEM_RET(itemDst);

        if(!m_draggedFiles.IsEmpty()) {
            // Files were being dragged
            DoFilesEndDrag(itemDst);

        } else if(!m_draggedProjects.IsEmpty()) {
            // Projects were being dragged
            DoProjectsEndDrag(itemDst);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildProjectOnly(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();

        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
            e.SetString(projectName);
            gsgs_Event().AddPendingEvent(e);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnCleanProjectOnly(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();

        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
            e.SetString(projectName);
            gsgs_Event().AddPendingEvent(e);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ExpandToPath(const wxString& project, const wxFileName& fileName)
    {
        std::unordered_map<wxString, wxTreeItemId>::iterator it = m_projectsMap.find(project);
        if (it != m_projectsMap.end())
        {
            if (fileName.IsOk()) {
                wxTreeItemId child = it->second;
                FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
                if (childData->GetData().GetDisplayName() == project) {
                    wxTreeItemId fileItem = fileName.GetName().IsEmpty()
                        ? child : FindItemByPath(child, gsgs_Manager()->GetProjectCwd(project), fileName.GetFullPath());
                    if (fileItem.IsOk()) {
                        SelectItem(fileItem);
                        EnsureVisible(fileItem);
                    }
                    else {
                        wxString message;
                        message << _("Failed to find file: ") << fileName.GetFullPath() << _(" in FileView.");
                        gsgsLogMessage(message);
                    }
                }
            }
            else {
                // just expand to the project
                wxTreeItemId child = m_projectsMap.find(project)->second;
                if (child.IsOk())
                {
                    SelectItem(child);
                    EnsureVisible(child);
                }
            }
        }
        /*else
        {
            if (fileName.IsOk()) {
                wxTreeItemId tt = GetRootItem();
                wxTreeItemId fileItem = fileName.GetName().IsEmpty() ? 
                    tt : FindItemByPath(tt, gsgs_Manager()->GetProjectCwd(project), fileName.GetFullPath());
                if (fileItem.IsOk()) {
                    SelectItem(fileItem);
                    EnsureVisible(fileItem);
                }
            }
        }*/
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::FindItemByPath(wxTreeItemId& projectHTI, const wxString& projectPath,
                                              const wxString& fileName)
    {
        if (projectHTI.IsOk() && ItemHasChildren(projectHTI))
        {
            wxString projectName = gsgs_CppWorker().GetProjectFromFile(fileName);
            ProjectPtr proj = gsgs_CppWorker().GetProject(projectName);
            if (!proj) return wxTreeItemId();

            wxString vdFullPath = proj->GetVDByFileName(fileName);

#if defined(__WXGTK__)
            wxString realpathItem = RealPath(fileName);
#endif

            wxTreeItemId curItem = projectHTI;
            wxArrayString dirs = ::wxStringTokenize(vdFullPath, ":", wxTOKEN_STRTOK);
            for (size_t i = 0; i < dirs.size(); ++i) {

                // Ensure that the 'curItem' is expanded before we loop
                DoBuildSubTreeIfNeeded(curItem);

                wxTreeItemIdValue cookie;
                wxTreeItemId child = GetFirstChild(curItem, cookie);
                bool matchFound = false;
                while (child.IsOk()) {
                    wxString name = GetItemText(child);
                    if (name == dirs.Item(i)) {
                        matchFound = true;
                        curItem = child;
                        break;
                    }
                    child = GetNextChild(curItem, cookie);
                }
                if (!matchFound) {
                    curItem = wxTreeItemId();
                    break;
                }
            }

            if (curItem.IsOk()) {
                DoBuildSubTreeIfNeeded(curItem);

                // We found the virtual folder that should contain the filename
                wxTreeItemIdValue cookie;
                wxTreeItemId child = GetFirstChild(curItem, cookie);
                while (child.IsOk()) {
                    FilewViewTreeItemData* childData = static_cast<FilewViewTreeItemData*>(GetItemData(child));
                    wxFileName fn(childData->GetData().GetFile());
                    fn.MakeAbsolute(projectPath);
                    if (fn.GetFullPath().CmpNoCase(fileName) == 0) {
                        return child;
                    }
#if defined(__WXGTK__)
                    else { // Try again, dereferencing fn
                        wxString fdest = RealPath(fn.GetFullPath());
                        if (fdest.CmpNoCase(realpathItem) == 0) { return child; }
                    }
#endif
                    child = GetNextChild(curItem, cookie);
                }
            }
        }
        return wxTreeItemId();
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::ItemByFullPath(const wxString& fullPath)
    {
        if(!ItemHasChildren(GetRootItem())) return wxTreeItemId();

        // Locate the project item
        wxString projname = fullPath.BeforeFirst(':');
        if(m_projectsMap.count(projname) == 0) return wxTreeItemId();

        wxTreeItemId parent = GetItemParent(m_projectsMap[projname]);
        wxArrayString texts = wxStringTokenize(fullPath, wxT(":"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < texts.GetCount(); i++) {
            parent = DoGetItemByText(parent, texts.Item(i));
            if(parent.IsOk() == false) { return wxTreeItemId(); }
        }
        return parent;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnImportDirectory(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxTreeItemId item = GetSingleSelection();
        if(!item.IsOk()) { return; }

        wxString vdPath = GetItemPath(item);
        wxString project;
        project = vdPath.BeforeFirst(wxT(':'));
        ProjectPtr proj = gsgs_Manager()->GetProject(project);

        bool extlessFiles(false);
        wxStringBoolMap_t dirs;
        wxArrayString files;
        wxArrayString all_files;
        wxString filespec;

        ImportFilesDialogNew dlg(gsgs_Frame());
        if(dlg.ShowModal() != wxID_OK) return;

        extlessFiles = dlg.ExtlessFiles();
        dlg.GetDirectories(dirs);
        filespec = dlg.GetFileMask();

        // get list of all files based on the checked directories
        wxStringBoolMap_t::const_iterator iter = dirs.begin();
        for(; iter != dirs.end(); ++iter) {
            int flags = iter->second ? (wxDIR_FILES | wxDIR_DIRS) : (wxDIR_FILES);
            wxDir::GetAllFiles(iter->first, &all_files, "", flags);
        }

        DoImportFolder(proj, dlg.GetBaseDir(), all_files, filespec, extlessFiles);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoImportFolder(ProjectPtr proj, const wxString& baseDir, const wxArrayString& all_files,
                                      const wxString& filespec, bool extlessFiles)
    {
        wxStringTokenizer tok(filespec, wxT(";"));
        gsgsStringSet specMap;
        while(tok.HasMoreTokens()) {
            wxString v = tok.GetNextToken();
            // Cater for *.*, and also for idiots asking for *.foo;*.*
            if(v == wxT("*.*")) {
                // Remove any previous entries, and stop looking for more: an empty specMap signals *.*
                specMap.clear();
                break;
            }
            v = v.AfterLast(wxT('*'));
            v = v.AfterLast(wxT('.')).MakeLower();
            specMap.insert(v);
        }

        // filter non interesting files
        wxArrayString files;
        for(size_t i = 0; i < all_files.GetCount(); i++) {
            wxFileName fn(all_files.Item(i));

            /* always excluded by default */
            const wxArrayString& dirs = fn.GetDirs();
            bool cont = true;
            for(size_t j = 0; j < dirs.GetCount() && cont; j++) {
                wxString filepath = fn.GetPath();
                if(dirs.Item(j) == wxT(".svn") || dirs.Item(j) == wxT(".cvs") || dirs.Item(j) == wxT(".arch-ids") ||
                   dirs.Item(j) == wxT("arch-inventory") || dirs.Item(j) == wxT("autom4te.cache") ||
                   dirs.Item(j) == wxT("BitKeeper") || dirs.Item(j) == wxT(".bzr") || dirs.Item(j) == wxT(".bzrignore") ||
                   dirs.Item(j) == wxT("CVS") || dirs.Item(j) == wxT(".cvsignore") || dirs.Item(j) == wxT("_darcs") ||
                   dirs.Item(j) == wxT(".deps") || dirs.Item(j) == wxT("EIFGEN") || dirs.Item(j) == wxT(".git") ||
                   dirs.Item(j) == wxT(".hg") || dirs.Item(j) == wxT("PENDING") || dirs.Item(j) == wxT("RCS") ||
                   dirs.Item(j) == wxT("RESYNC") || dirs.Item(j) == wxT("SCCS") || dirs.Item(j) == wxT("{arch}")) {
                    cont = false;
                    break;
                }
            }

            // skip the directory?
            if(!cont) continue;

            if(specMap.empty()) {
                files.Add(all_files.Item(i));

            } else if(fn.GetExt().IsEmpty() & extlessFiles) {
                files.Add(all_files.Item(i));

            } else if(specMap.find(fn.GetExt().MakeLower()) != specMap.end()) {
                files.Add(all_files.Item(i));
            }
        }

        wxString path = baseDir;
        //{ Fixe bug 2847625
        if(path.EndsWith(wxT("/")) || path.EndsWith(wxT("\\"))) { path.RemoveLast(); } //} Fixe bug 2847625

        wxFileName rootPath(path);

        // loop over the files and construct for each file a record with
        // the following information:
        // -virtual directory (full path, starting from project level)
        // -display name
        // -full path of the file
        proj->BeginTranscation();
        {
            // Create a progress dialog
            ProgressDlg* prgDlg = new ProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.GetCount());

            // get list of files
            for(size_t i = 0; i < files.GetCount(); i++) {
                wxFileName fn(files.Item(i));

                // if the file already exist, skip it
                if(proj->IsFileExist(fn.GetFullPath())) { continue; }

                FileViewItem fvitem;
                fvitem.fullpath = fn.GetFullPath();
                fvitem.displayName = fn.GetFullName();

                fn.MakeRelativeTo(path);

                wxString relativePath = fn.GetPath();
                relativePath.Replace(wxT("/"), wxT(":"));
                relativePath.Replace(wxT("\\"), wxT(":"));

                if(relativePath.IsEmpty()) {
                    // the file is probably under the root, add it under
                    // a virtual directory with the name of the target
                    // root folder
                    relativePath = rootPath.GetName();
                }
                // relativePath.Append(wxT(":"));

                fvitem.virtualDir = relativePath;
                DoAddItem(proj, fvitem);

                wxString msg;
                msg << _("Adding file: ") << fn.GetFullPath();
                prgDlg->Update((int)i, msg);
            }
            prgDlg->Destroy();
        }

        // save the project file to disk
        proj->CommitTranscation();

        // Reload the view
        CallAfter(&FileViewTree::BuildTree);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnReconcileProject(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxString projectName;

        // Allow the selected project to be reconciled, even if it's inactive
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* fvid = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(fvid && fvid->GetData().GetKind() == ProjectItem::TypeProject) {
                gsgs_Manager()->ReconcileProject(fvid->GetData().GetDisplayName());
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::RedefineProjFiles(ProjectPtr proj, const wxString& path, std::vector<wxString>& files)
    {
        wxFileName rootPath(path);

        // loop over the files and construct for each file a record with
        // the following information:
        // -virtual directory (full path, starting from project level)
        // -display name
        // -full path of the file
        proj->BeginTranscation();
        {
            // Create a progress dialog
            ProgressDlg* prgDlg = new ProgressDlg(NULL, _("Importing files ..."), wxT(""), (int)files.size());

            proj->ClearAllVirtDirs();

            wxString relativePath;
            for(size_t i = 0; i < files.size(); i++) {
                wxFileName fn(files[i]);

                FileViewItem fvitem;
                fvitem.fullpath = fn.GetFullPath();
                fvitem.displayName = fn.GetFullName();

                fn.MakeRelativeTo(path);

                // anchor all files to a base folder
                relativePath = rootPath.GetName() + wxT(":") + fn.GetPath() + wxT(":");
                relativePath.Replace(wxT("/"), wxT(":"));
                relativePath.Replace(wxT("\\"), wxT(":"));

                fvitem.virtualDir = relativePath;
                DoAddItem(proj, fvitem);

                wxString msg;
                msg << _("Adding file: ") << fn.GetFullPath();
                prgDlg->Update((int)i, msg);
            }
            prgDlg->Destroy();
        }

        // save the project file to disk
        proj->CommitTranscation();

        // reload the project
        wxString curr_proj_name(proj->GetName());
        bool was_active(gsgs_Manager()->GetActiveProjectName() == curr_proj_name);
        gsgs_Manager()->RemoveProject(proj->GetName(), false);
        gsgs_Manager()->AddProject(proj->GetFileName().GetFullPath());

        // restore the active project
        if(was_active) { MarkActive(curr_proj_name); }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoAddItem(ProjectPtr proj, const FileViewItem& item)
    {
        if (proj)
        {
            // first add the virtual directory, if it already exist,
            // this function does nothing
            proj->CreateVirtualDir(item.virtualDir, true);

            // add the file.
            // For performance reasons, we dont go through the Workspace API
            // but directly through the project API
            proj->FastAddFile(item.fullpath, item.virtualDir);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRunPremakeStep(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            gsgs_Manager()->RunCustomPreMakeCommand(projectName);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRenameItem(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                wxTreeItemId parent = GetItemParent(item);
                if(parent.IsOk()) {

                    wxString path = GetItemPath(parent);
                    wxString proj = path.BeforeFirst(wxT(':'));

                    ProjectPtr p = gsgs_Manager()->GetProject(proj);
                    if(p) {
                        // prompt user for new name
                        wxString newName = wxGetTextFromUser(_("New file name:"), _("Rename file:"), GetItemText(item));
                        if(newName.IsEmpty() == false) {

                            wxFileName tmp(data->GetData().GetFile());
                            tmp.SetFullName(newName);

                            if(tmp.FileExists()) {
                                wxMessageBox(_("A File with that name already exists!"), _("光栅故事"),
                                             wxICON_WARNING | wxOK);
                                return;
                            }

                            // rename the file (this will erase it from the symbol database and will
                            // also close the editor that it is currently opened in (if any))
                            if(gsgs_Manager()->RenameFile(data->GetData().GetFile(), tmp.GetFullPath(), path)) {
                                // update the item's info
                                data->SetDisplayName(tmp.GetFullName());
                                data->SetFile(tmp.GetFullPath());

                                // rename the tree item
                                SetItemText(item, tmp.GetFullName());

                                SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
                            }
                        }
                    } // p
                }     // parent.IsOk()
            }         // TypeFile
        }             // item.IsOk()
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRenameVirtualFolder(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            // got the item, prompt user for new name
            wxString newName =
                wxGetTextFromUser(_("New virtual folder name:"), _("Rename virtual folder:"), GetItemText(item));
            if(newName.IsEmpty() || newName == GetItemText(item)) {
                // user clicked cancel
                return;
            }

            // locate the project
            wxString path = GetItemPath(item);
            wxString proj = path.BeforeFirst(wxT(':'));

            path = path.AfterFirst(wxT(':'));
            ProjectPtr p = gsgs_Manager()->GetProject(proj);
            if(!p) {
                gsgsLogMessage(_("failed to rename virtual folder: ") + path + _(", reason: could not locate project ") +
                             proj);
                return;
            }

            if(!p->RenameVirtualDirectory(path, newName)) {
                gsgsLogMessage(_("failed to rename virtual folder: ") + path);
                return;
            }
            SetItemText(item, newName);
            SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnReBuild(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);

            wxString conf;
            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            // Custom build supports the 'Rebuild' target
            if(bldConf && bldConf->IsCustomBuild()) {
                QueueCommand buildInfo(projectName, conf, false, QueueCommand::kRebuild);
                if(bldConf && bldConf->IsCustomBuild()) {
                    buildInfo.SetKind(QueueCommand::kCustomBuild);
                    buildInfo.SetCustomBuildTarget(wxT("Rebuild"));
                }

                gsgs_Manager()->PushQueueCommand(buildInfo);
                gsgs_Manager()->ProcessCommandQueue();

            } else {
                gsgs_Frame()->RebuildProject(projectName);
            }
        }
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::DoGetItemByText(const wxTreeItemId& parent, const wxString& text)
    {
        if(!parent.IsOk()) { return wxTreeItemId(); }

        if(!ItemHasChildren(parent)) { return wxTreeItemId(); }

        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            if(GetItemText(child) == text) { return child; }
            child = GetNextChild(parent, cookie);
        }
        return wxTreeItemId();
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::CreateVirtualDirectory(const wxString& parentPath, const wxString& vdName)
    {
        // try to locate that VD first, if it exists, do nothing
        wxTreeItemId item = ItemByFullPath(wxString::Format(wxT("%s:%s"), parentPath.c_str(), vdName.c_str()));
        if(item.IsOk()) { return true; }

        wxString project = parentPath.BeforeFirst(wxT(':'));
        wxString parentVDs = parentPath.AfterFirst(wxT(':'));
        wxArrayString vds = ::wxStringTokenize(parentVDs, wxT(":"), wxTOKEN_STRTOK);

        wxTreeItemId curItem = ItemByFullPath(project);
        if(!curItem.IsOk())
            // Could not locate the project item...
            return false;

        wxString path = project;
        for(size_t i = 0; i < vds.GetCount(); i++) {
            path << wxT(":") << vds.Item(i);
            wxTreeItemId tmpItem = ItemByFullPath(path);
            if(!tmpItem.IsOk()) {
                curItem = DoAddVirtualFolder(curItem, vds.Item(i));
                if(curItem.IsOk() == false) {
                    // failed to add virtual directory
                    break;
                }
            } else {
                curItem = tmpItem;
            }
        }

        if(!curItem.IsOk()) return false;

        DoAddVirtualFolder(curItem, vdName);
        return true;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::MarkActive(const wxString& projectName)
    {
        // Find the project item ID by name and select it
        if(m_projectsMap.count(projectName)) 
        { 
            DoSetProjectActive(m_projectsMap.find(projectName)->second); 
        }
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::CreateAndAddFile(const wxString& filename, const wxString& vdFullPath)
    {
        wxTreeItemId item = ItemByFullPath(vdFullPath);
        return DoAddNewItem(item, filename, vdFullPath);
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::DoAddNewItem(wxTreeItemId& item, const wxString& filename, const wxString& vdFullpath)
    {
        if(item.IsOk() == false) { return false; }

        gsgs_Manager()->AddNewFileToProject(filename, vdFullpath);

        // Add the tree node
        wxFileName fnFileName(filename);
        wxString path(vdFullpath);

        path += wxT(":");
        path += fnFileName.GetFullName();
        ProjectItem projItem(path, fnFileName.GetFullName(), fnFileName.GetFullPath(), ProjectItem::TypeFile);

        wxTreeItemId hti = AppendItem(item,                      // parent
                                      projItem.GetDisplayName(), // display name
                                      GetIconIndex(projItem),    // item image index
                                      GetIconIndex(projItem),    // selected item image
                                      new FilewViewTreeItemData(projItem));
        wxUnusedVar(hti);
        Expand(item);
        SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
        return true;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRebuildProjectOnly(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);

            wxString conf;
            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            QueueCommand info(projectName, conf, true, QueueCommand::kRebuild);
            if(bldConf && bldConf->IsCustomBuild()) {
                info.SetKind(QueueCommand::kCustomBuild);
                info.SetCustomBuildTarget(wxT("Rebuild"));
            }

            gsgs_Manager()->PushQueueCommand(info);
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnLocalWorkspaceSettings(wxCommandEvent& e)
    {
        if(gsgs_Workspace().IsOpen()) {
            WorkspaceSettingsDlg dlg(gsgs_Frame(), gsgs_CppWorker().GetLocalWorkspace());
            if(dlg.ShowModal() == wxID_OK) {
                gsgs_Frame()->SelectBestEnvSet();
                // Update the new paths
                gsgs_Manager()->UpdateParserPaths(true);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRetagInProgressUI(wxUpdateUIEvent& event)
    {
        event.Enable(!gsgs_Manager()->GetRetagInProgress());
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnOpenWithDefaultApplication(wxCommandEvent& event)
    {
        wxArrayTreeItemIds items;
        GetMultiSelection(items);

        wxMimeTypesManager* mgr = wxTheMimeTypesManager;
        for(size_t i = 0; i < items.GetCount(); i++) {
            wxTreeItemId item = items.Item(i);
            FilewViewTreeItemData* itemData = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {

                wxFileName fn(itemData->GetData().GetFile());
                wxFileType* type = mgr->GetFileTypeFromExtension(fn.GetExt());
                bool bFoundCommand = false;
                wxUnusedVar(bFoundCommand);

                if(type) {

                    wxString cmd = type->GetOpenCommand(fn.GetFullPath());
                    delete type;

                    if(!cmd.IsEmpty()) {
                        bFoundCommand = true;
                        wxExecute(cmd);
                    }
                }

    #ifdef __WXGTK__
                if(!bFoundCommand && itemData && itemData->GetData().GetKind() == ProjectItem::TypeFile) {
                    // All hell break loose, try xdg-open
                    wxString cmd = wxString::Format(wxT("xdg-open \"%s\""), fn.GetFullPath().c_str());
                    wxExecute(cmd);
                }
    #endif
            }
        }
    }
    //--------------------------------------------------------------------------
    ProjectPtr FileViewTree::GetSelectedProject() const
    {
        wxArrayTreeItemIds selections;
        size_t count = GetSelections(selections);
        if(count == 0) { return NULL; }

        // Return the first item which is of type "TypeProject"
        for(size_t i = 0; i < count; ++i) {
            FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(selections.Item(i)));
            if(itemData && (itemData->GetData().GetKind() == ProjectItem::TypeProject)) {
                return gsgs_CppWorker().GetProject(GetItemText(selections.Item(i)));
            }
        }

        // None of the selected items is a project.
        // Return the project parent of the first item
        wxTreeItemId item = selections.Item(0);
        while(item.IsOk() && (item != GetRootItem())) {
            FilewViewTreeItemData* itemData = dynamic_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(!itemData) {
                return NULL;
            } else if(itemData->GetData().GetKind() == ProjectItem::TypeProject) {
                // Found a project
                return gsgs_CppWorker().GetProject(GetItemText(item));
            }
            item = GetItemParent(item);
        }

        // No project was found
        return NULL;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildTree(wxCommandEvent& e)
    {
        e.Skip();
        CallAfter(&FileViewTree::BuildTree);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildProjectOnlyInternal(wxCommandEvent& e)
    {
        e.Skip();
        wxString projectName = e.GetString();
        if(projectName.IsEmpty()) { projectName = gsgs_Manager()->GetActiveProjectName(); }

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, true, QueueCommand::kBuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Build"));
        }
        gsgs_Manager()->PushQueueCommand(info);
        gsgs_Manager()->ProcessCommandQueue();
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnCleanProjectOnlyInternal(wxCommandEvent& e)
    {
        e.Skip();
        wxString projectName = e.GetString();
        if(projectName.IsEmpty()) { projectName = gsgs_Manager()->GetActiveProjectName(); }

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, true, QueueCommand::kClean);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetKind(QueueCommand::kCustomBuild);
            info.SetCustomBuildTarget(wxT("Clean"));
        }

        gsgs_Manager()->PushQueueCommand(info);
        gsgs_Manager()->ProcessCommandQueue();
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnExcludeFromBuild(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        wxArrayTreeItemIds selections;
        size_t count = GetSelections(selections);
        for(size_t selectionIndex = 0; selectionIndex < count; selectionIndex++) {
            wxTreeItemId item = selections[selectionIndex];
            if(item.IsOk()) {
                FilewViewTreeItemData* data = ItemData(item);
                if(data && data->GetData().IsFile()) {
                    const ProjectItem& pi = data->GetData();
                    ProjectPtr proj = gsgs_Manager()->GetProject(pi.Key().BeforeFirst(':'));
                    if(proj) {
                        if(e.IsChecked()) {
                            proj->AddExcludeConfigForFile(pi.GetFile());
                            ExcludeFileFromBuildUI(item, true);

                        } else {
                            proj->RemoveExcludeConfigForFile(pi.GetFile());
                            ExcludeFileFromBuildUI(item, false);
                        }
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnExcludeFromBuildUI(wxUpdateUIEvent& event)
    {
        // by default enable it
        event.Check(IsFileExcludedFromBuild(GetSingleSelection()));
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::IsFileExcludedFromBuild(const wxTreeItemId& item) const
    {
        if(item.IsOk() && gsgs_CppWorker().IsOpen()) {
            FilewViewTreeItemData* data = ItemData(item);
            const ProjectItem& pi = data->GetData();
            if(pi.IsFile()) {
                wxString projectName = pi.Key().BeforeFirst(':');
                if(!projectName.IsEmpty()) {
                    ProjectPtr proj = gsgs_CppWorker().GetProject(projectName);
                    if(proj) { return proj->IsFileExcludedFromConfig(pi.GetFile()); }
                }
            }
        }
        return false;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnSelectionChanged(wxTreeEvent& e)
    {
        e.Skip();
        if (e.GetItem().IsOk())
        {
            FilewViewTreeItemData* data = dynamic_cast<FilewViewTreeItemData*>(GetItemData(e.GetItem()));
            if (data && data->GetData().GetKind() == ProjectItem::TypeProject) {
                wxCommandEvent evtProjectSelected(wxEVT_PROJECT_TREEITEM_CLICKED);
                evtProjectSelected.SetString(data->GetData().GetDisplayName());
                gsgs_Event().AddPendingEvent(evtProjectSelected);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoGetProjectIconIndex(const wxString& projectName, int& iconIndex, bool& fromPlugin)
    {
        fromPlugin = false;
        ColourEvent event(wxEVT_WORKSPACE_VIEW_CUSTOMIZE_PROJECT);
        event.SetEventObject(this);
        // set the project name
        event.SetString(projectName);
        if(gsgs_Event().ProcessEvent(event)) {
            iconIndex = event.GetInt();
            fromPlugin = true;

        } else {
            iconIndex = PROJECT_IMG_IDX;
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnRenameProject(wxCommandEvent& event)
    {
        CHECK_COND_RET(gsgs_CppWorker().IsOpen());
        wxTreeItemId item = GetSingleSelection();
        CHECK_ITEM_RET(item);

        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        if(data->GetData().GetKind() == ProjectItem::TypeProject) {
            wxString newname = ::wxGetTextFromUser(_("Project new name:"), _("Rename project"));
            newname.Trim().Trim(false);
            CHECK_COND_RET(!newname.IsEmpty());

            wxString oldname = data->GetData().GetDisplayName();
            // If the new name and the old name are the same, do nothing
            if(oldname == newname) return;

            // If a project with this name already exists, abort
            if(m_projectsMap.count(newname)) {
                ::wxMessageBox(_("A project with this name already exists in the workspace"), "gsgs",
                               wxOK | wxICON_WARNING | wxCENTRE, this);
                return;
            }

            if(!::IsVaidProjectName(newname)) {
                wxMessageBox(_("Project names may contain only the following characters [a-z0-9_-]"), "gsgs",
                             wxOK | wxICON_WARNING | wxCENTER, this);
                return;
            }

            // Calling 'RenameProject' will trigger a wxEVT_PROJ_RENAMED event
            gsgs_CppWorker().RenameProject(data->GetData().GetDisplayName(), newname);

            // Update the display name
            SetItemText(item, newname);

            // Update the user data
            data->GetData().SetDisplayName(newname);

            // Update the cache
            m_projectsMap.erase(oldname);
            m_projectsMap.insert(std::make_pair(newname, item));
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnFolderDropped(Event& event)
    {
        // User dragged a folder into our workspace
        const wxArrayString& folders = event.GetStrings();
        if(folders.size() != 1) {
            ::wxMessageBox(_("You can only drag one folder at a time"), "gsgs", wxOK | wxCENTER | wxICON_ERROR);
            return;
        }

        bool reloadWorkspaceIsNeeded(false);
        const wxString& folder = folders.Item(0);
        wxFileName workspaceFileName(folder, "");
        wxString errMsg;
        if(!gsgs_CppWorker().IsOpen()) {

            wxFileName fnWorkspace(folder, "");

            // If a workspace is already exist at the selected path - load it
            wxArrayString workspaceFiles;
            wxString workspaceFile;
            wxDir::GetAllFiles(folder, &workspaceFiles, "*.workspace", wxDIR_FILES);
            // Check the workspace type
            for(size_t i = 0; i < workspaceFiles.size(); ++i) {
                if(FileUtil::GetType(workspaceFiles.Item(i)) == FileUtil::TypeWorkspace) {
                    // Found a workspace in the folder, just open it
                    wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
                    evtOpenworkspace.SetString(workspaceFiles.Item(i));
                    evtOpenworkspace.SetEventObject(gsgs_Frame());
                    gsgs_Frame()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
                    return;
                }
            }

            wxString project_name = workspaceFileName.GetDirs().Last();
            project_name = FileUtil::NormaliseName(project_name);

            workspaceFileName.SetName(project_name);
            workspaceFileName.SetExt("workspace");

            // Create an empty workspace
            if(!gsgs_CppWorker().CreateWorkspace(project_name, folder, errMsg)) {
                ::wxMessageBox(_("Failed to create workspace:\n") + errMsg, "gsgs", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            // Create an empty project with sensible defaults
            ProjectData pd;
            CompilerPtr cmp = gsgs_Settings().GetDefaultCompiler(COMPILER_DEFAULT_FAMILY);
            if(cmp) {
                pd.m_cmpType = cmp->GetName();
            } else {
                pd.m_cmpType = "gnu g++"; // Default :/
            }

            pd.m_name = project_name;
            pd.m_path = folder;

            // Set a default empty project
            pd.m_srcProject.Reset(new Project());

    // Use sensible debugger defaults
    #ifdef __WXMAC__
            pd.m_debuggerType = "LLDB Debugger";
    #else
            pd.m_debuggerType = "GNU gdb debugger";
    #endif
            gsgs_Manager()->CreateProject(pd, "");
            reloadWorkspaceIsNeeded = true;
        }

        // to which project should we import the folder?
        wxArrayString projects;
        gsgs_CppWorker().GetProjectList(projects);
        if(projects.IsEmpty()) {
            ::wxMessageBox(_("Can't import files to workspace without projects"), "gsgs",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        wxString projectName;
        if(projects.GetCount() > 1) {
            int selection = projects.Index(gsgs_CppWorker().GetActiveProjectName());
            projectName = ::wxGetSingleChoice(_("Select project:"), _("Import files to project"), projects, selection);
        } else {
            // single project, just add it
            projectName = projects.Item(0);
        }

        // user cancelled?
        if(projectName.IsEmpty()) return;
        ProjectPtr pProj = gsgs_CppWorker().GetProject(projectName);
        CHECK_PTR_RET(pProj);

        wxArrayString all_files;
        wxDir::GetAllFiles(folder, &all_files, wxEmptyString, wxDIR_DIRS | wxDIR_FILES);

        ImportFilesSettings ifs;
        DoImportFolder(pProj, folder, all_files, ifs.GetFileMask(), ifs.GetFlags() & IFS_INCLUDE_FILES_WO_EXT);

        if(reloadWorkspaceIsNeeded) {
            // Now that we have created a workspace + one project reload the workspace
            wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("switch_to_workspace"));
            evtOpenworkspace.SetString(workspaceFileName.GetFullPath());
            evtOpenworkspace.SetEventObject(gsgs_Frame());
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
        }

        // And trigger a full reparse of the workspace
        wxCommandEvent evtOpenworkspace(wxEVT_MENU, XRCID("full_retag_workspace"));
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(evtOpenworkspace);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::FolderDropped(const wxArrayString& folders)
    {
        Event dummy;
        dummy.SetStrings(folders);
        OnFolderDropped(dummy);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnOpenShellFromFilePath(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                wxFileName fn(data->GetData().GetFile());
                FileUtil::OpenTerminal(fn.GetPath());
            } else if(data->GetData().GetKind() == ProjectItem::TypeProject) {
                ProjectPtr p = gsgs_CppWorker().GetProject(data->GetData().GetDisplayName());
                if(p) {
                    // Apply the environment before launching the terminal
                    EnvSetter env(p);
                    FileUtil::OpenTerminal(p->GetFileName().GetPath());
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnOpenFileExplorerFromFilePath(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data->GetData().GetKind() == ProjectItem::TypeFile) {
                wxFileName fn(data->GetData().GetFile());
                FileUtil::OpenFileExplorerAndSelect(fn.GetFullPath());
            } else if(data->GetData().GetKind() == ProjectItem::TypeProject) {
                ProjectPtr p = gsgs_CppWorker().GetProject(data->GetData().GetDisplayName());
                if(p) { FileUtil::OpenFileExplorerAndSelect(p->GetFileName()); }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::CreateProjectContextMenu(wxMenu& menu, const wxString& projectName, bool usedByFileView)
    {
        wxMenuItem* item(NULL);
        ImageManager * bmpLoader = &gsgs_Image();
        const wxBitmap & bmpBuild = bmpLoader->LoadBitmap("toolbars/16/build/build");
        const wxBitmap & bmpClean = bmpLoader->LoadBitmap("toolbars/16/build/clean");
        const wxBitmap & bmpSettings = bmpLoader->LoadBitmap("cog");
        const wxBitmap & bmpSort = bmpLoader->LoadBitmap("sort");
        const wxBitmap & bmpFolder = bmpLoader->LoadBitmap("folder-yellow");
        const wxBitmap & bmpConsole = bmpLoader->LoadBitmap("console");
        const wxBitmap & bmpColourPallette = bmpLoader->LoadBitmap("colour-pallette");
        const wxBitmap & bmpPin = bmpLoader->LoadBitmap("ToolPin");

        if(usedByFileView) {
            // When the menu is created for internal usage (i.e. by this class)
            // add the "Pin Project" menu item
            item = new wxMenuItem(&menu, XRCID("pin_project"), _("Pin Project"), _("Pin Project"));
            item->SetBitmap(bmpPin);
            menu.Append(item);
            menu.AppendSeparator();
        }

        item = new wxMenuItem(&menu, XRCID("build_project"), _("Build"), _("Build project"));
        item->SetBitmap(bmpBuild);
        menu.Append(item);

        item = new wxMenuItem(&menu, XRCID("rebuild_project"), _("Rebuild"), _("Rebuild project"));
        menu.Append(item);

        item = new wxMenuItem(&menu, XRCID("clean_project"), _("Clean"), _("Clean project"));
        item->SetBitmap(bmpClean);
        menu.Append(item);

        item = new wxMenuItem(&menu, XRCID("stop_build"), _("Stop Build"), _("Stop Build"));
        menu.Append(item);

        // Custom build targets
        // Add custom project targets
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf && bldConf->IsCustomBuild() && !bldConf->GetCustomTargets().empty()) {
            menu.AppendSeparator();
            wxMenuItem* item = NULL;
            // append the custom build targets
            const BuildConfig::StringMap_t& targets = bldConf->GetCustomTargets();
            if(!targets.empty()) {
                BuildSettingsConfig::singleton().SetTargets(projectName, targets);
                const BuildSettingsConfig::Map_t& targetsMap = BuildSettingsConfig::singleton().GetTargets();
                // get list of custom targets, and create menu entry for each target
                BuildSettingsConfig::Map_t::const_iterator iter = targetsMap.begin();
                for(; iter != targetsMap.end(); ++iter) {
                    item = new wxMenuItem(&menu,
                                          iter->first,        // Menu ID
                                          iter->second.first, // Menu Name
                                          wxEmptyString, wxITEM_NORMAL);
                    menu.Append(item);
                }
            }
        }

        menu.AppendSeparator();

        wxMenu* projectOnly = new wxMenu();
        projectOnly->Append(XRCID("build_project_only"), _("Build"));
        projectOnly->Append(XRCID("clean_project_only"), _("Clean"));
        projectOnly->Append(XRCID("rebuild_project_only"), _("Rebuild"));

        menu.Append(wxID_ANY, _("Project Only"), projectOnly);
        menu.AppendSeparator();

        item = new wxMenuItem(&menu, XRCID("build_order"), _("Build Order..."), _("Build Order..."));
        item->SetBitmap(bmpSort);
        menu.Append(item);
        menu.AppendSeparator();

        item = new wxMenuItem(&menu, XRCID("cxx_fileview_open_file_explorer"), _("Open Containing Folder"),
                              _("Open Containing Folder"));
        item->SetBitmap(bmpFolder);
        menu.Append(item);

        item = new wxMenuItem(&menu, XRCID("cxx_fileview_open_shell_from_filepath"), _("Open Shell"), _("Open Shell"));
        item->SetBitmap(bmpConsole);
        menu.Append(item);

        menu.AppendSeparator();
        menu.Append(XRCID("set_as_active"), _("Make Active (double click)"));
        menu.AppendSeparator();

        menu.Append(XRCID("import_directory"), _("Import Files From Directory..."));
        menu.Append(XRCID("reconcile_project"), _("Reconcile Project..."));
        menu.AppendSeparator();

        menu.Append(XRCID("new_virtual_folder"), _("New Virtual Folder"));
        menu.AppendSeparator();

        menu.Append(XRCID("remove_project"), _("Remove Project"));
        menu.AppendSeparator();
        menu.Append(XRCID("rename_project"), _("Rename Project"));
        menu.AppendSeparator();

        menu.Append(XRCID("save_as_template"), _("Save As Template..."));
        menu.AppendSeparator();
        menu.Append(XRCID("local_project_prefs"), _("Project Editor Preferences..."));

        menu.AppendSeparator();

        item = new wxMenuItem(&menu, XRCID("colour_virtual_folder"), _("Set Custom Background Colour..."));
        item->SetBitmap(bmpColourPallette);
        menu.Append(item);

        menu.Append(XRCID("clear_virtual_folder_colour"), _("Clear Background Colour"));
        menu.AppendSeparator();
        item = new wxMenuItem(&menu, XRCID("project_properties"), _("Settings..."), _("Settings..."));
        item->SetBitmap(bmpSettings);
        menu.Append(item);

        // Add the plugins menu
        if(!gsgs_Manager()->IsBuildInProgress()) {
            // Let the plugins alter it
            wxMenu* pluginsMenu = new wxMenu;
            ContextMenuEvent event(wxEVT_CONTEXT_MENU_PROJECT);
            event.SetMenu(pluginsMenu);
            pluginsMenu->SetParent(&menu);
            gsgs_Event().ProcessEvent(event);

            // Use the old system
            PluginManager::Get()->HookPopupMenu(pluginsMenu, MenuTypeFileView_Project);
            if(pluginsMenu->GetMenuItemCount()) {
                // we got something from the plugins
                menu.PrependSeparator();
                menu.Prepend(wxID_ANY, _("Plugins..."), pluginsMenu);
            } else {
                wxDELETE(pluginsMenu);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::UnselectAllProject()
    {
        std::for_each(m_projectsMap.begin(), m_projectsMap.end(), [&](std::pair<wxString, wxTreeItemId> p) {
            //wxFont f = GetDefaultFont();
            //f.SetWeight(wxFONTWEIGHT_NORMAL);
            //f.SetStyle(wxFONTSTYLE_NORMAL);
            SetItemFont(p.second, GetDefaultFont());
            SetItemTextColour(p.second, wxColour());
        });
    }
    //--------------------------------------------------------------------------
    wxTreeItemId FileViewTree::AddWorkspaceFolder(const wxString& folderPath)
    {
        if(folderPath.IsEmpty()) return GetRootItem();
        //wxTreeItemId folderItemId;
        wxArrayString folders = ::wxStringTokenize(folderPath, "/", wxTOKEN_STRTOK);
        wxString current;
        wxTreeItemId parentItem = GetRootItem();
        for(size_t i = 0; i < folders.size(); ++i) {
            if(!current.IsEmpty()) { current << "/"; }
            current << folders.Item(i);
            if(m_workspaceFolders.count(current) == 0) {
                // New folder
                ProjectItem itemData;
                itemData.SetKind(ProjectItem::TypeWorkspaceFolder);
                itemData.SetDisplayName(folders.Item(i));
                itemData.m_key = current;
                FilewViewTreeItemData* cd = new FilewViewTreeItemData(itemData);
                parentItem = AppendItem(parentItem, folders.Item(i), WORKSPACE_FOLDER_IMG_IDX,
                                        WORKSPACE_FOLDER_EXPANDED_IMG_IDX, cd);
                m_workspaceFolders.insert(std::make_pair(current, parentItem));
            } else {
                parentItem = m_workspaceFolders.find(current)->second;
            }
            //folderItemId = parentItem;
        }
        //return folderItemId;
        return parentItem;
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ShowWorkspaceFolderContextMenu()
    {
        wxMenu menu;
        menu.Append(XRCID("workspace_folder_new"), _("New Workspace Folder"));
        menu.Append(XRCID("workspace_folder_delete"), _("Delete"));
        menu.AppendSeparator();
        menu.Append(XRCID("new_cxx_project"), _("New Project..."));
        menu.AppendSeparator();
        menu.Append(XRCID("add_existing_cxx_project"), _("Add an Existing Project..."));

        menu.Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceFolderNewFolder, this, XRCID("workspace_folder_new"));
        menu.Bind(wxEVT_MENU, &FileViewTree::OnWorkspaceFolderDelete, this, XRCID("workspace_folder_delete"));

        PopupMenu(&menu);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnWorkspaceFolderDelete(wxCommandEvent& evt)
    {
        wxArrayTreeItemIds items, folderItems;
        GetMultiSelection(items);

        for(size_t i = 0; i < items.size(); ++i) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(items.Item(i)));
            if(data && data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) { folderItems.Add(items.Item(i)); }
        }

        CHECK_COND_RET(!folderItems.IsEmpty());

        wxString message;
        if(folderItems.size() > 1) {
            message << _("Are you sure you want to delete the following workspace folders:\n");
            for(size_t i = 0; i < folderItems.size(); ++i) {
                FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(i)));
                message << data->GetData().GetDisplayName() << "\n";
            }
            message << "?";
        } else {
            message << _("Are you sure you want to delete the workspace folder:\n");
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(0)));
            message << data->GetData().GetDisplayName() << "\n?";
        }

        if(::wxMessageBox(message, "gsgs", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER,
                          gsgs_Event().TopFrame()) != wxYES) {
            return;
        }

        // Delete each folder
        for(size_t i = 0; i < folderItems.size(); ++i) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(folderItems.Item(i)));
            gsgs_CppWorker().DeleteWorkspaceFolder(data->GetData().Key());
        }
        CallAfter(&FileViewTree::BuildTree);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnWorkspaceNewWorkspaceFolder(wxCommandEvent& evt)
    {
        wxString name = ::wxGetTextFromUser(_("Folder name:"), _("Create Workspace Folder"));
        if (name.size())
        {
            gsgs_CppWorker().CreateWorkspaceFolder(name);
            wxTreeItemId select = AddWorkspaceFolder(name);
            SelectItem(select);
            EnsureVisible(select);
            //CallAfter(&FileViewTree::BuildTree);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnWorkspaceFolderNewFolder(wxCommandEvent& evt)
    {
        wxTreeItemId item = GetSingleSelection();
        CHECK_ITEM_RET(item);

        wxString name = ::wxGetTextFromUser(_("Folder name:"), _("Create Folder"));
        if (name.size())
        {
            // Append the path
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            name.Prepend(data->GetData().Key() + "/");

            gsgs_CppWorker().CreateWorkspaceFolder(name);

            wxTreeItemId select = AddWorkspaceFolder(name);
            SelectItem(select);
            EnsureVisible(select);
            //CallAfter(&FileViewTree::BuildTree);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnNewProject(wxCommandEvent& evt)
    {
        wxString workspaceFolder;
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
            if(data && (data->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder)) {
                workspaceFolder = data->GetData().Key();
            }
        }
        gsgs_Manager()->ShowNewProjectWizard(workspaceFolder);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoFilesEndDrag(wxTreeItemId& itemDst)
    {
        // Files were being dragged
        wxString targetVD, fromVD;
        while(true) {
            if(!itemDst.IsOk()) { return; }
            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(itemDst));
            if(data && data->GetData().GetKind() == ProjectItem::TypeVirtualDirectory) {
                break; // Found a vd, so break out of the while loop
            }
            // We're only allowed to drag items between virtual folders, so find the parent folder
            itemDst = GetItemParent(itemDst);
        }

        wxTreeItemId target = itemDst;
        if(target.IsOk()) {
            targetVD = GetItemPath(target);
        } else {
            return;
        }

        for(size_t n = 0; n < m_draggedFiles.GetCount(); ++n) {
            wxTreeItemId itemSrc = m_draggedFiles.Item(n);
            wxTreeItemId fromItem = GetItemParent(itemSrc);
            if(fromItem.IsOk()) {
                fromVD = GetItemPath(fromItem);
            } else {
                continue;
            }

            if(fromVD == targetVD) {
                // Not much point dropping onto the same virtual dir
                continue;
            }

            // the file name to remove
            FilewViewTreeItemData* srcData = static_cast<FilewViewTreeItemData*>(GetItemData(itemSrc));

            // no tree-item-data? skip this one
            if(!srcData) continue;

            wxString filename = srcData->GetData().GetFile();

            ProjectItem itemData = srcData->GetData();

            // call the manager to remove them in the underlying project
            if(gsgs_Manager()->MoveFileToVD(filename, fromVD, targetVD)) {
                // remove the item from its current node, and place it under the
                // new parent node
                AppendItem(target,                    // parent
                           itemData.GetDisplayName(), // display name
                           GetIconIndex(itemData),    // item image index
                           GetIconIndex(itemData),    // selected item image
                           new FilewViewTreeItemData(itemData));
                Delete(itemSrc);
                Expand(target);
                SendCmdEvent(wxEVT_FILE_VIEW_REFRESHED);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoProjectsEndDrag(wxTreeItemId& itemDst)
    {
        // We allow dropping on workspace or an a "workspace folder"
        FilewViewTreeItemData* cd = static_cast<FilewViewTreeItemData*>(GetItemData(itemDst));
        CHECK_PTR_RET(cd);

        // Sanity
        if(m_draggedProjects.IsEmpty()) return;

        wxString targetPath;
        if(cd->GetData().GetKind() == ProjectItem::TypeWorkspaceFolder) {
            targetPath = cd->GetData().Key(); // The full path
        } else if(cd->GetData().GetKind() == ProjectItem::TypeWorkspace) {
            targetPath = "";
        } else {
            return; // We do not allow dropping on anything else than the workspace or a workspace folder
        }

        // Move the projects to the target folder and rebuild the tree view
        for(size_t i = 0; i < m_draggedProjects.size(); ++i) {
            FilewViewTreeItemData* d = static_cast<FilewViewTreeItemData*>(GetItemData(m_draggedProjects.Item(i)));
            gsgs_CppWorker().MoveProjectToFolder(d->GetData().GetDisplayName(), targetPath,
                                                         (i == (m_draggedProjects.size() - 1)));
        }
        CallAfter(&FileViewTree::BuildTree);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnSetBgColourVirtualFolder(wxCommandEvent& e)
    {
        wxTreeItemId item = GetSingleSelection();
        CHECK_ITEM_RET(item);

        // Get colour from the user
        wxColour col = ::wxGetColourFromUser(gsgs_Event().TopFrame());
        if (col.IsOk())
        {
            // Read the current colours map
            FolderColour::Map_t coloursMap;
            if (!gsgs_CppWorker().GetLocalWorkspace()->GetFolderColours(coloursMap)) return;
            // Colour the tree (it will also update the 'coloursMap' table)
            m_colourHelper->SetBgColour(item, col, coloursMap);
            // Store the settings
            gsgs_CppWorker().GetLocalWorkspace()->SetFolderColours(coloursMap);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnClearBgColourVirtualFolder(wxCommandEvent&)
    {
        wxTreeItemId item = GetSingleSelection();
        CHECK_ITEM_RET(item);

        // Fetch the current colours map
        FolderColour::Map_t coloursMap;
        if(!gsgs_CppWorker().GetLocalWorkspace()->GetFolderColours(coloursMap)) return;

        // Colour the tree (it will also update the 'coloursMap' table)
        m_colourHelper->ResetBgColour(item, coloursMap);

        // Update the local settings
        gsgs_CppWorker().GetLocalWorkspace()->SetFolderColours(coloursMap);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnAddProjectToWorkspaceFolder(wxCommandEvent& evt)
    {
        wxUnusedVar(evt);
        wxTreeItemId item = GetSingleSelection();
        CHECK_ITEM_RET(item);

        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        CHECK_PTR_RET(data);

        if(data->GetData().GetKind() != ProjectItem::TypeWorkspaceFolder) return;
        wxString workspaceFolder = data->GetData().Key();
        const wxString ALL(wxT("gsgs Projects (*.project)|*.project|") wxT("All Files (*)|*"));
        wxFileDialog dlg(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL, wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                         wxDefaultPosition);
        if(dlg.ShowModal() == wxID_OK) {
            wxString errmsg;
            if(!gsgs_CppWorker().AddProject(dlg.GetPath(), workspaceFolder, errmsg)) {
                ::wxMessageBox(errmsg, "gsgs", wxICON_ERROR | wxOK | wxCENTER, gsgs_Event().TopFrame());
            }
        }

        // Fire "Project-Added" event
        Event evtProjectAdded(wxEVT_PROJ_ADDED);
        gsgs_Event().AddPendingEvent(evtProjectAdded);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoSetItemBackgroundColour(const wxTreeItemId& item, const FolderColour::List_t& colours,
                                                 const ProjectItem& projectItem)
    {
        // Set the background colour for the item if it is a virtual folder or a file
        if(colours.empty()) { return; }
        wxColour bgColour;
        if(projectItem.GetKind() == ProjectItem::TypeVirtualDirectory || projectItem.GetKind() == ProjectItem::TypeFile ||
           projectItem.GetKind() == ProjectItem::TypeProject) {
            // A virtual folder, try to find a custom colour for it
            wxString itemPath = m_colourHelper->GetItemPath(this, item);
            const FolderColour& match = FolderColour::FindForPath(colours, itemPath);
            if(match.IsOk()) {
                bgColour = match.GetColour();
            } else {
                bgColour = wxNullColour;
            }
            if(bgColour.IsOk()) { SetItemBackgroundColour(item, bgColour); }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnItemExpanding(wxTreeEvent& e)
    {
        e.Skip();
        wxTreeItemId item = e.GetItem();
        DoBuildSubTreeIfNeeded(item);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoAddChildren(const wxTreeItemId& parentItem)
    {
        ProjectPtr proj = GetItemProject(parentItem);
        if(!proj) { return; }

        FolderColour::Map_t coloursMap;
        FolderColour::List_t coloursList;
        gsgs_CppWorker().GetLocalWorkspace()->GetFolderColours(coloursMap);

        // Sort the list
        FolderColour::SortToList(coloursMap, coloursList);

        // int iconIndex = GetIconIndex(node->GetData());

        FilewViewTreeItemData* cd = ItemData(parentItem);
        if(!cd) return;

        const ProjectItem& pi = cd->GetData();
        if(!pi.IsVirtualFolder() && !pi.IsProject()) return;

        wxArrayString folders, files;
        wxString vdFullPath;
        if(pi.IsVirtualFolder()) {
            vdFullPath = GetItemPath(parentItem);
            wxString vdFullPathNoProject = vdFullPath.AfterFirst(':');

            proj->GetFolders(vdFullPathNoProject, folders);
            proj->GetFiles(vdFullPathNoProject, files);
        } else {
            proj->GetFolders("", folders);
        }

        // First, we add the virtual folders
        for(size_t i = 0; i < folders.size(); ++i) {
            const wxString& childVdFullPath = folders.Item(i);
            wxString displayName = childVdFullPath.AfterLast(':');
            ProjectItem folderItem(proj->GetName() + ":" + childVdFullPath, displayName, "",
                                   ProjectItem::TypeVirtualDirectory);
            wxTreeItemId hti = AppendItem(parentItem,            // parent
                                          displayName,           // display name
                                          FOLDER_IMG_IDX,        // item image index
                                          FOLDER_EXPAND_IMG_IDX, // selected item image
                                          new FilewViewTreeItemData(folderItem));
            DoSetItemBackgroundColour(hti, coloursList, folderItem);
            if(!proj->IsVirtualDirectoryEmpty(childVdFullPath)) {
                // Add a dummy item So the user can "Expand" it
                AppendItem(hti, "<dummy>", -1, -1,
                           new FilewViewTreeItemData(ProjectItem("", "", "", ProjectItem::TypeInvalid)));
            }
        }

        BuildConfigPtr buildConf = proj->GetBuildConfiguration();
        wxString buildConfName = buildConf ? buildConf->GetName() : "";

        for(size_t i = 0; i < files.size(); ++i) {
            const wxString& filepath = files.Item(i);
            wxFileName fn(filepath);
            ProjectItem fileItem(vdFullPath + ":" + fn.GetFullName(), fn.GetFullName(), filepath, ProjectItem::TypeFile);

            int iconIndex = GetIconIndex(fileItem);
            wxTreeItemId hti = AppendItem(parentItem,                // parent
                                          fileItem.GetDisplayName(), // display name
                                          iconIndex,                 // item image index
                                          iconIndex,                 // selected item image
                                          new FilewViewTreeItemData(fileItem));
            DoSetItemBackgroundColour(hti, coloursList, fileItem);

            // If the file is disabled for the current build configuration, mark it as such
            clProjectFile::Ptr_t fileInfo = proj->GetFile(fn.GetFullPath());
            if(fileInfo && !buildConfName.IsEmpty() && fileInfo->IsExcludeFromConfiguration(buildConfName)) {
                // Set the item text with disabled colour
                ExcludeFileFromBuildUI(hti, true);
            }
        }
    }
    //--------------------------------------------------------------------------
    ProjectPtr FileViewTree::GetItemProject(const wxTreeItemId& item) const
    {
        wxTreeItemId cur = item;
        while(cur.IsOk() && (cur != GetRootItem())) {
            FilewViewTreeItemData* cd = ItemData(cur);
            if(cd && cd->GetData().IsProject()) {
                return gsgs_CppWorker().GetProject(cd->GetData().GetDisplayName());
            }
            cur = GetItemParent(cur);
        }
        return nullptr;
    }
    //--------------------------------------------------------------------------
    FilewViewTreeItemData* FileViewTree::ItemData(const wxTreeItemId& item) const
    {
        CHECK_ITEM_RET_NULL(item);
        wxTreeItemData* cd = GetItemData(item);
        CHECK_PTR_RET_NULL(cd);
        return static_cast<FilewViewTreeItemData*>(cd);
    }
    //--------------------------------------------------------------------------
    void FileViewTree::DoBuildSubTreeIfNeeded(const wxTreeItemId& parent)
    {
        if(parent.IsOk() && ItemHasChildren(parent)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId childItem = GetFirstChild(parent, cookie);

            FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(childItem));
            if(data && data->GetData().IsInvalid()) {
                // Delete the dummy children
                DeleteChildren(parent);

                // Append the real items
                DoAddChildren(parent);
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::ExcludeFileFromBuildUI(const wxTreeItemId& item, bool exclude)
    {
        FilewViewTreeItemData* data = static_cast<FilewViewTreeItemData*>(GetItemData(item));
        CHECK_PTR_RET(data);
        CHECK_COND_RET(data->GetData().IsFile());
        const wxString& filename = data->GetData().GetFile();
        if(exclude && m_excludeBuildFiles.count(filename)) { return; }
        if(!exclude && m_excludeBuildFiles.count(filename) == 0) { return; }

        if(exclude) {
            m_excludeBuildFiles.insert({ filename, item });
            SetItemTextColour(item, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        } else {
            m_excludeBuildFiles.erase(filename);
            SetItemTextColour(item, gsgs_Style().GetOutputPaneFgColour());
        }
    }
    //--------------------------------------------------------------------------
    bool FileViewTree::IsItemExcludedFromBuild(const wxTreeItemId& item, const wxString& configName) const { return false; }
    //--------------------------------------------------------------------------
    void FileViewTree::OnBuildConfigChanged(wxCommandEvent& e)
    {
        e.Skip();
        // Remove the Gray text from the all the "Exclude files"
        std::for_each(m_excludeBuildFiles.begin(), m_excludeBuildFiles.end(),
                      [&](const std::unordered_map<wxString, wxTreeItemId>::value_type& vt) {
                          SetItemTextColour(vt.second, gsgs_Style().GetOutputPaneFgColour());
                      });
        m_excludeBuildFiles.clear();

        std::unordered_map<wxString, wxTreeItemId> allFiles;
        // We need to collect list of all file items from the tree
        std::queue<wxTreeItemId> Q;
        Q.push(GetRootItem());
        while(!Q.empty()) {
            wxTreeItemId item = Q.front();
            Q.pop();

            if(!ItemHasChildren(item)) {
                FilewViewTreeItemData* d = ItemData(item);
                if(d && d->GetData().IsFile()) { allFiles.insert({ d->GetData().GetFile(), item }); }
            } else {
                wxTreeItemIdValue k;
                wxTreeItemId child = GetFirstChild(item, k);
                while(child.IsOk()) {
                    Q.push(child);
                    child = GetNextChild(item, k);
                }
            }
        }

        // Get list of all excluded files for this configuration
        std::vector<wxString> excludeFiles;
        if(gsgs_CppWorker().GetExcludeFilesForConfig(excludeFiles)) {
            for(size_t i = 0; i < excludeFiles.size(); ++i) {
                if(allFiles.count(excludeFiles[i])) {
                    // This file was expanded and we have a valid item id for it
                    ExcludeFileFromBuildUI(allFiles[excludeFiles[i]], true);
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnTreeKeyDown(wxTreeEvent& event)
    {
        event.Skip();
        if(event.GetKeyCode() == WXK_NUMPAD_DELETE || event.GetKeyCode() == WXK_DELETE) { DoRemoveItems(); }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnFindInFilesShowing(FindInFilesEvent& event)
    {
        event.Skip();
        if(IsShownOnScreen() && HasFocus()) {
            wxString selections;
            wxArrayTreeItemIds items;
            if(GetSelections(items) == 0) { return; }
            for(size_t i = 0; i < items.size(); ++i) {
                const wxTreeItemId& item = items.Item(i);
                FilewViewTreeItemData* cd = ItemData(item);
                if(!cd) { continue; }
                switch(cd->GetData().GetKind()) {
                case ProjectItem::TypeFile:
                    selections << "F: " << cd->GetData().GetFile() << "\n";
                    break;
                case ProjectItem::TypeVirtualDirectory:
                    selections << "V: " << GetItemPath(item, '/') << "\n";
                    break;
                case ProjectItem::TypeProject:
                    selections << "P: " << cd->GetData().GetDisplayName() << "\n";
                    break;
                case ProjectItem::TypeWorkspace:
                    selections << "W: " << cd->GetData().GetDisplayName() << "\n";
                    break;
                default:
                    break;
                }
            }
            selections.Trim();
            event.SetTransientPaths(selections);
        }
    }
    //--------------------------------------------------------------------------
    void FileViewTree::OnPinProject(wxCommandEvent& event)
    {
        wxTreeItemId item = GetSingleSelection();
        if(item.IsOk()) {
            wxString projectName = GetItemText(item);
            gsgs_Frame()->GetWorkspaceTab()->AddPinnedProject(projectName);
        }
    }
    //--------------------------------------------------------------------------
}