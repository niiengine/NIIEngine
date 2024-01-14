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

#include "gsgsPreferences.h"
#include "gsgsEditorConfig.h"
#include "CommonController.h"
#include "CommonController1.h"
#include "gsgsComment.h"
#include "gsgsFrame.h"
#include "gsgsConsole.h"
#include "gsgsLogManager.h"
#include "gsgsZip.h"
#include "globals.h"
#include <wx/busyinfo.h>
#include <wx/choicdlg.h>
#include <wx/fontmap.h>

    //--------------------------------------------------------------------------
    // Declare the bitmap loading function
    extern void wxCrafterqEa5vvInitBitmapResources();

namespace gsgs
{
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorOptionsGeneralGuidesPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorOptionsGeneralGuidesPanel::EditorOptionsGeneralGuidesPanel(wxWindow* parent, wxWindowID id, 
        const wxPoint& pos, const wxSize& size, long style) : 
            wxPanel(parent, id, pos, size, style), 
            TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        wxArrayString m_pgMgrGeneralArr;
        wxArrayInt m_pgMgrGeneralIntArr;
        m_pgMgrGeneral = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxPG_DESCRIPTION | wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED);
        m_pgMgrGeneral->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        bSizer1->Add(m_pgMgrGeneral, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

        m_pgPropCategoryGeneral = m_pgMgrGeneral->Append(new wxPropertyCategory(_("General")));
        m_pgPropCategoryGeneral->SetHelpString(wxT(""));

        m_pgPropDisplayLineNumbers = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Show line numbers margin"), wxPG_LABEL, 1));
        m_pgPropDisplayLineNumbers->SetHelpString(_("Show line numbers margin"));

        m_pgPropRelativeLineNumbers = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Enable Relative line numbers"), wxPG_LABEL, 1));
        m_pgPropRelativeLineNumbers->SetHelpString(_("Use Relative line numbers"));

        m_pgPropHighlightMatchedBrace = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Highlight matched braces"), wxPG_LABEL, 1));
        m_pgPropHighlightMatchedBrace->SetHelpString(_("Highlight matched braces"));

        m_pgPropHideEditMargin = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Hide the edit margin"), wxPG_LABEL, 1));
        m_pgPropHideEditMargin->SetHelpString(_("Hide the edit margin ( the red/green marks when a line is modified)"));

        m_pgPropShowIndentGuidelines = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Show indentation guidelines"), wxPG_LABEL, 1));
        m_pgPropShowIndentGuidelines->SetHelpString(_("Show indentation guidelines (vertical lines)"));

        m_pgPropDisableSemiColonShift = m_pgMgrGeneral->AppendIn(m_pgPropCategoryGeneral, new wxBoolProperty(_("Disable semicolon shift"), wxPG_LABEL, 1));
        m_pgPropDisableSemiColonShift->SetHelpString(_("By default when typing \";\" next to a close brace \")\" gsgs will move the \";\" to the right\nThis option enables or disables this behavior"));

        m_pgPropWhitespaces = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Whitespaces")));
        m_pgPropWhitespaces->SetHelpString(wxT(""));

        m_pgMgrGeneralArr.Add(_("Invisible"));
        m_pgMgrGeneralArr.Add(_("Visible always"));
        m_pgMgrGeneralArr.Add(_("Visible after indentation"));
        m_pgPropWhitespaceVisibility = m_pgMgrGeneral->AppendIn(m_pgPropWhitespaces, new wxEnumProperty(_("Whitespace visibility"), wxPG_LABEL, m_pgMgrGeneralArr, m_pgMgrGeneralIntArr, 0));
        m_pgPropWhitespaceVisibility->SetHelpString(_("Whitespace visibility policy"));

        m_pgMgrGeneralArr.Clear();
        m_pgMgrGeneralIntArr.Clear();
        m_pgMgrGeneralArr.Add(_("Default"));
        m_pgMgrGeneralArr.Add(_("Mac (CR)"));
        m_pgMgrGeneralArr.Add(_("Windows (CRLF)"));
        m_pgMgrGeneralArr.Add(_("Unix (LF)"));
        m_pgPropEOLMode = m_pgMgrGeneral->AppendIn(m_pgPropWhitespaces, new wxEnumProperty(_("EOL Mode"), wxPG_LABEL, m_pgMgrGeneralArr, m_pgMgrGeneralIntArr, 0));
        m_pgPropEOLMode->SetHelpString(_("Set the editor's EOL mode (End Of Line)"));

        m_pgPropCaretLine = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Caret line")));
        m_pgPropCaretLine->SetHelpString(wxT(""));

        m_pgPropEnableCaretLine = m_pgMgrGeneral->AppendIn(m_pgPropCaretLine, new wxBoolProperty(_("Highlight caret line"), wxPG_LABEL, 0));
        m_pgPropEnableCaretLine->SetHelpString(_("Highlight caret line with a background colour"));

        m_pgPropCaretLineColour = m_pgMgrGeneral->AppendIn(m_pgPropCaretLine, new wxSystemColourProperty(_("Caret line background colour"), wxPG_LABEL, wxColour(wxT("rgb(171,222,230)"))));
        m_pgPropCaretLineColour->SetHelpString(_("Caret line background colour"));

        m_pgPropCaretLineAlpha = m_pgMgrGeneral->AppendIn(m_pgPropCaretLine, new wxIntProperty(_("Caret line colour alpha"), wxPG_LABEL, 30));
        m_pgPropCaretLineAlpha->SetHelpString(_("Set the caret line colour transparency value. Where 0 mean complete transparent and 255 means fully opaque"));

        m_pgPropDebuggerMarker = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Debugger Marker")));
        m_pgPropDebuggerMarker->SetHelpString(_("Debugger Marker"));

        m_pgPropHighlightDebuggerMarker = m_pgMgrGeneral->AppendIn(m_pgPropDebuggerMarker, new wxBoolProperty(_("Highlight debugger line"), wxPG_LABEL, 0));
        m_pgPropHighlightDebuggerMarker->SetHelpString(_("When debugging, highlight the current line with a background colour"));

        m_pgPropDebuggerLineColour = m_pgMgrGeneral->AppendIn(m_pgPropDebuggerMarker, new wxSystemColourProperty(_("Debugger line background colour")));
        m_pgPropDebuggerLineColour->SetValueToUnspecified();
        m_pgPropDebuggerLineColour->SetHelpString(_("Debugger line background colour"));

        m_pgProp8 = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Edit")));
        m_pgProp8->SetHelpString(wxT(""));

        m_pgPropSmartCurly = m_pgMgrGeneral->AppendIn(m_pgProp8, new wxBoolProperty(_("Smart curly brackets"), wxPG_LABEL, 1));
        m_pgPropSmartCurly->SetHelpString(_("When user types '{', automatically insert the closing brace"));

        m_pgPropSmartParentheses = m_pgMgrGeneral->AppendIn(m_pgProp8, new wxBoolProperty(_("Smart square brackets / Parentheses"), wxPG_LABEL, 1));
        m_pgPropSmartParentheses->SetHelpString(_("When user types '[' or '(' automatically insert the closing bracket.\nIn addition, if a user types ']' or ')' next to ']' or ')' just move the caret one position to the right"));

        m_pgPropSmartQuotes = m_pgMgrGeneral->AppendIn(m_pgProp8, new wxBoolProperty(_("Smart quotes"), wxPG_LABEL, 1));
        m_pgPropSmartQuotes->SetHelpString(_("When typing \" or ', automatically add another one to the right, unless one already exists (in this case, simply move the caret one position to the right)"));

        m_pgPropCopyLineEmptySelection = m_pgMgrGeneral->AppendIn(m_pgProp8, new wxBoolProperty(_("Copying empty selection copies caret line"), wxPG_LABEL, 1));
        m_pgPropCopyLineEmptySelection->SetHelpString(_("Whether copying an empty selection to the clipboard copies the entire contents of the caret line, or nothing"));

        m_pgProp16 = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Typing in selection")));
        m_pgProp16->SetHelpString(wxT(""));

        m_pgPropWrapQuotes = m_pgMgrGeneral->AppendIn(m_pgProp16, new wxBoolProperty(_("Wrap with quotes"), wxPG_LABEL, 1));
        m_pgPropWrapQuotes->SetHelpString(_("When typing ' or \" on a selection, instead of replacing the selection with the character, wrap it with quotes"));

        m_pgPropWrapBrackets = m_pgMgrGeneral->AppendIn(m_pgProp16, new wxBoolProperty(_("Wrap with brackets"), wxPG_LABEL, 1));
        m_pgPropWrapBrackets->SetHelpString(_("When typing '(' or '[' on a selection, instead of replacing the selection with the character, wrap it with brackets"));

        m_pgProp56 = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Zoom")));
        m_pgProp56->SetHelpString(wxT(""));

        m_pgPropZoomUsingCtrlScroll = m_pgMgrGeneral->AppendIn(m_pgProp56, new wxBoolProperty(_("Enable mouse zoom"), wxPG_LABEL, 1));
        m_pgPropZoomUsingCtrlScroll->SetHelpString(_("When holding Ctrl/CMD + scrolling with the mouse zoom the text"));

        m_pgProp565 = m_pgMgrGeneral->Append(new wxPropertyCategory(_("Comments")));
        m_pgProp565->SetHelpString(wxT(""));

        m_pgPropCommentsIndented = m_pgMgrGeneral->AppendIn(m_pgProp565, new wxBoolProperty(_("Indented line comments"), wxPG_LABEL, 0));
        m_pgPropCommentsIndented->SetHelpString(_("Indent line comments (C++-style comments) according to the indentation of the selected fragmant of the text"));

        // Connect events
        m_pgMgrGeneral->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EditorOptionsGeneralGuidesPanel::OnValueChanged), NULL, this);

        ::wxPGPropertyBooleanUseCheckbox(m_pgMgrGeneral->GetGrid());
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_pgPropSmartCurly->SetValue(options->GetAutoAddMatchedCurlyBraces());
        m_pgPropSmartParentheses->SetValue(options->GetAutoAddMatchedNormalBraces());
        m_pgPropSmartQuotes->SetValue(options->GetAutoCompleteDoubleQuotes());
        m_pgPropCopyLineEmptySelection->SetValue(options->GetCopyLineEmptySelection());
        m_pgPropWrapBrackets->SetValue(options->IsWrapSelectionBrackets());
        m_pgPropWrapQuotes->SetValue(options->IsWrapSelectionWithQuotes());
        m_pgPropZoomUsingCtrlScroll->SetValue(options->IsMouseZoomEnabled());
        m_pgPropCommentsIndented->SetValue(options->GetIndentedComments());

        m_pgPropDisplayLineNumbers->SetValue(options->GetDisplayLineNumbers());
        m_pgPropRelativeLineNumbers->SetValue(options->GetRelativeLineNumbers());
        m_pgPropHighlightMatchedBrace->SetValue(options->GetHighlightMatchedBraces());
        m_pgPropShowIndentGuidelines->SetValue(options->GetShowIndentationGuidelines());
        m_pgPropEnableCaretLine->SetValue(options->GetHighlightCaretLine());
        wxVariant caretLineColour, debuggerLineColour;
        caretLineColour << options->GetCaretLineColour();
        m_pgPropCaretLineColour->SetValue(caretLineColour);
        m_pgPropDisableSemiColonShift->SetValue(options->GetDisableSemicolonShift());
        m_pgPropHideEditMargin->SetValue(options->GetHideChangeMarkerMargin());
        m_pgPropHighlightDebuggerMarker->SetValue(options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line));
        debuggerLineColour << options->GetDebuggerMarkerLine();
        m_pgPropDebuggerLineColour->SetValue(debuggerLineColour);
        m_pgPropWhitespaceVisibility->SetChoiceSelection(options->GetShowWhitspaces());
        m_pgPropCaretLineAlpha->SetValue(options->GetCaretLineAlpha());

        // EOL
        // Default;Mac (CR);Windows (CRLF);Unix (LF)
        wxArrayString eolOptions;
        eolOptions.Add("Default");
        eolOptions.Add("Mac (CR)");
        eolOptions.Add("Windows (CRLF)");
        eolOptions.Add("Unix (LF)");
        int eolSel = eolOptions.Index(options->GetEolMode());
        if (eolSel != wxNOT_FOUND) {
            m_pgPropEOLMode->SetChoiceSelection(eolSel);
        }
    }
    //--------------------------------------------------------------------------
    EditorOptionsGeneralGuidesPanel::~EditorOptionsGeneralGuidesPanel()
    {
        m_pgMgrGeneral->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EditorOptionsGeneralGuidesPanel::OnValueChanged), NULL, this);
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralGuidesPanel::Save(OptionsConfig * options)
    {
        options->SetDisplayLineNumbers(m_pgPropDisplayLineNumbers->GetValue().GetBool());
        options->SetRelativeLineNumbers(m_pgPropRelativeLineNumbers->GetValue().GetBool());
        options->SetHighlightMatchedBraces(m_pgPropHighlightMatchedBrace->GetValue().GetBool());
        options->SetShowIndentationGuidelines(m_pgPropShowIndentGuidelines->GetValue().GetBool());
        options->SetHighlightCaretLine(m_pgPropEnableCaretLine->GetValue().GetBool());

        wxColourPropertyValue carteLineColour, debuggerLineColour;
        carteLineColour << m_pgPropCaretLineColour->GetValue();
        debuggerLineColour << m_pgPropDebuggerLineColour->GetValue();
        options->SetCaretLineColour(carteLineColour.m_colour);

        wxString eolMode = m_pgPropEOLMode->GetValueAsString();
        options->SetEolMode(eolMode);
        options->SetHideChangeMarkerMargin(m_pgPropHideEditMargin->GetValue().GetBool());
        options->SetDisableSemicolonShift(m_pgPropDisableSemiColonShift->GetValue().GetBool());
        options->SetDebuggerMarkerLine(debuggerLineColour.m_colour);
        options->EnableOption(OptionsConfig::Opt_Mark_Debugger_Line, m_pgPropHighlightDebuggerMarker->GetValue().GetBool());
        options->SetShowWhitspaces(m_pgPropWhitespaceVisibility->GetValue().GetInteger());
        options->SetCaretLineAlpha(m_pgPropCaretLineAlpha->GetValue().GetInteger());
        options->SetAutoAddMatchedCurlyBraces(m_pgPropSmartCurly->GetValue().GetBool());
        options->SetAutoAddMatchedNormalBraces(m_pgPropSmartParentheses->GetValue().GetBool());
        options->SetAutoCompleteDoubleQuotes(m_pgPropSmartQuotes->GetValue().GetBool());
        options->SetCopyLineEmptySelection(m_pgPropCopyLineEmptySelection->GetValue().GetBool());
        options->SetWrapSelectionBrackets(m_pgPropWrapBrackets->GetValue().GetBool());
        options->SetWrapSelectionWithQuotes(m_pgPropWrapQuotes->GetValue().GetBool());
        options->SetMouseZoomEnabled(m_pgPropZoomUsingCtrlScroll->GetValue().GetBool());
        options->SetIndentedComments(m_pgPropCommentsIndented->GetValue().GetBool());
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralGuidesPanel::OnValueChanged(wxPropertyGridEvent& event)
    {
        // event.Skip();
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorOptionsGeneralIndentationPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorOptionsGeneralIndentationPanel::EditorOptionsGeneralIndentationPanel(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);
        m_checkBoxDisableSmartIdent = new wxCheckBox(this, wxID_ANY, _("Disable Smart Indentation"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer1->Add(m_checkBoxDisableSmartIdent, 0, wxALL | wxEXPAND, 5);

        m_indentsUsesTabs = new wxCheckBox(this, wxID_ANY, _("Use tabs in indentation"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer1->Add(m_indentsUsesTabs, 0, wxALL | wxEXPAND, 5);

        wxFlexGridSizer* fgSizer1;
        fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
        fgSizer1->AddGrowableCol(1);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Columns per indentation level:"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_staticText1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        m_indentWidth = gsgs_Style().createTextCtrl(this, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_indentWidth, 0, wxALL | wxALIGN_RIGHT, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Columns per tab character in document:"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        m_tabWidth = gsgs_Style().createTextCtrl(this, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_tabWidth, 0, wxALL | wxALIGN_RIGHT, 5);

        bSizer1->Add(fgSizer1, 1, wxEXPAND, 5);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_checkBoxDisableSmartIdent->SetValue(options->GetDisableSmartIndent());
        m_indentsUsesTabs->SetValue(options->GetIndentUsesTabs());
        m_indentWidth->SetValue(StringUtils::wxIntToString(options->GetIndentWidth()));
        m_tabWidth->SetValue(StringUtils::wxIntToString(options->GetTabWidth()));
    }
    //--------------------------------------------------------------------------
    EditorOptionsGeneralIndentationPanel::~EditorOptionsGeneralIndentationPanel()
    {
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralIndentationPanel::Save(OptionsConfig * options)
    {
        options->SetIndentUsesTabs(m_indentsUsesTabs->IsChecked());
        options->SetIndentWidth(StringUtils::wxStringToInt(m_indentWidth->GetValue(), 4, 1, 20));
        options->SetTabWidth(StringUtils::wxStringToInt(m_tabWidth->GetValue(), 4, 1, 20));
        options->SetDisableSmartIndent(m_checkBoxDisableSmartIdent->IsChecked());
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorOptionsGeneralRightMarginPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorOptionsGeneralRightMarginPanel::EditorOptionsGeneralRightMarginPanel(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style) , 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer3);

        wxBoxSizer* bSizer4;
        bSizer4 = new wxBoxSizer(wxHORIZONTAL);

        m_radioBtnRMDisabled = new wxRadioButton(this, wxID_ANY, _("Disabled"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        m_radioBtnRMDisabled->SetValue(true);
        bSizer4->Add(m_radioBtnRMDisabled, 0, wxALL, 5);

        m_radioBtnRMLine = new wxRadioButton(this, wxID_ANY, _("Line"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer4->Add(m_radioBtnRMLine, 0, wxALL, 5);

        m_radioBtnRMBackground = new wxRadioButton(this, wxID_ANY, _("Background"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer4->Add(m_radioBtnRMBackground, 0, wxALL, 5);

        bSizer3->Add(bSizer4, 0, wxEXPAND, 5);

        wxFlexGridSizer* fgSizer2;
        fgSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
        fgSizer2->AddGrowableCol(1);
        fgSizer2->SetFlexibleDirection(wxBOTH);
        fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Indicator Column"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer2->Add(m_staticText1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        m_rightMarginColumn = gsgs_Style().createTextCtrl(this, wxID_ANY, _("80"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
        fgSizer2->Add(m_rightMarginColumn, 0, wxALL | wxALIGN_RIGHT, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Indicator Colour:"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer2->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

        m_rightMarginColour = new wxColourPickerCtrl(this, wxID_ANY, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE | wxCLRP_SHOW_LABEL);
        fgSizer2->Add(m_rightMarginColour, 0, wxALL | wxRIGHT | wxALIGN_RIGHT, 5);

        bSizer3->Add(fgSizer2, 1, wxALL | wxEXPAND, 5);

        // Connect Events
        m_radioBtnRMDisabled->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);
        m_radioBtnRMLine->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);
        m_radioBtnRMBackground->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);
        
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_radioBtnRMDisabled->SetValue(options->GetEdgeMode() == wxSTC_EDGE_NONE);
        m_radioBtnRMLine->SetValue(options->GetEdgeMode() == wxSTC_EDGE_LINE);
        m_radioBtnRMBackground->SetValue(options->GetEdgeMode() == wxSTC_EDGE_BACKGROUND);
        m_rightMarginColumn->ChangeValue(StringUtils::wxIntToString(options->GetEdgeColumn()));
        m_rightMarginColour->SetColour(options->GetEdgeColour());
        EnableDisableRightMargin();
    }
    //--------------------------------------------------------------------------
    EditorOptionsGeneralRightMarginPanel::~EditorOptionsGeneralRightMarginPanel()
    {
        // Disconnect Events
        m_radioBtnRMDisabled->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);
        m_radioBtnRMLine->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);
        m_radioBtnRMBackground->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator), NULL, this);

    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralRightMarginPanel::Save(OptionsConfig * options)
    {
        options->SetEdgeMode(m_radioBtnRMLine->GetValue() ? wxSTC_EDGE_LINE :
            m_radioBtnRMBackground->GetValue() ? wxSTC_EDGE_BACKGROUND : wxSTC_EDGE_NONE);
        options->SetEdgeColumn(StringUtils::wxStringToInt(m_rightMarginColumn->GetValue(), 80, 0, 256));
        options->SetEdgeColour(m_rightMarginColour->GetColour());
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralRightMarginPanel::OnRightMarginIndicator(wxCommandEvent& e)
    {
        EnableDisableRightMargin();
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralRightMarginPanel::EnableDisableRightMargin()
    {
        if (m_radioBtnRMDisabled->GetValue()) {
            m_rightMarginColour->Disable();
            m_rightMarginColumn->Disable();
            m_staticText1->Disable();
            m_staticText2->Disable();
        }
        else {
            m_rightMarginColour->Enable();
            m_rightMarginColumn->Enable();
            m_staticText1->Enable();
            m_staticText2->Enable();
        }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsCaret
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsCaret::EditorSettingsCaret(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(1);

        mainSizer->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Caret blink period (milliseconds):"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText1, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);

        m_spinCtrlBlinkPeriod = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT("500"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_spinCtrlBlinkPeriod->SetToolTip(_("Sets the caret blinking period in millisconds"));

        fgSizer1->Add(m_spinCtrlBlinkPeriod, 0, wxALL | wxEXPAND | wxALIGN_LEFT, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Caret width (pixels):"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_spinCtrlCaretWidth = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_spinCtrlCaretWidth->SetToolTip(_("Set the caret width in pixels"));

        fgSizer1->Add(m_spinCtrlCaretWidth, 0, wxALL | wxEXPAND | wxALIGN_LEFT, 5);

        wxBoxSizer* boxSizer4 = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(boxSizer4, 1, wxALL | wxEXPAND, 5);

        m_checkBoxBlockCaret = new wxCheckBox(this, wxID_ANY, _("Use a block caret"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxBlockCaret->SetValue(false);
        m_checkBoxBlockCaret->SetToolTip(_("Enable this to change the caret from a vertical line caret to a block shape caret"));

        boxSizer4->Add(m_checkBoxBlockCaret, 0, wxALL, 5);

        m_checkBoxScrollBeyondLastLine = new wxCheckBox(this, wxID_ANY, _("Allow caret to scroll beyond end of file"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxScrollBeyondLastLine->SetValue(false);
        m_checkBoxScrollBeyondLastLine->SetToolTip(_("When scrolling with the mouse, the scrolling can go beyond the end of file"));

        boxSizer4->Add(m_checkBoxScrollBeyondLastLine, 0, wxALL, 5);

        m_checkBoxCaretUseCamelCase = new wxCheckBox(this, wxID_ANY, _("Caret jumps between word segments"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxCaretUseCamelCase->SetValue(false);
        m_checkBoxCaretUseCamelCase->SetToolTip(_("Caret jumps between word segments marked by capitalisation (CamelCase) or underscores"));

        boxSizer4->Add(m_checkBoxCaretUseCamelCase, 0, wxALL, 5);

        m_checkBoxCaretOnVirtualSpace = new wxCheckBox(this, wxID_ANY, _("Allow caret to be placed beyond the end of line"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxCaretOnVirtualSpace->SetValue(false);
        m_checkBoxCaretOnVirtualSpace->SetToolTip(_("Allow the user to place the caret using the mouse beyond the end of line"));

        boxSizer4->Add(m_checkBoxCaretOnVirtualSpace, 0, wxALL, 5);

        m_checkBoxAdjustScrollbarSize = new wxCheckBox(this, wxID_ANY, _("Auto-adjust horizontal scrollbar width"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxAdjustScrollbarSize->SetValue(false);
        m_checkBoxAdjustScrollbarSize->SetToolTip(_("Auto adjust the horizontal toolbar to fit to the page content"));

        boxSizer4->Add(m_checkBoxAdjustScrollbarSize, 0, wxALL, 5);

        // Connect events
        m_staticText3->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsCaret::OnCaretWidthUI), NULL, this);
        m_spinCtrlCaretWidth->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsCaret::OnCaretWidthUI), NULL, this);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_spinCtrlBlinkPeriod->SetValue(StringUtils::wxIntToString(options->GetCaretBlinkPeriod()));
        m_spinCtrlCaretWidth->SetValue(StringUtils::wxIntToString(options->GetCaretWidth()));
        m_checkBoxCaretUseCamelCase->SetValue(options->GetCaretUseCamelCase());
        m_checkBoxScrollBeyondLastLine->SetValue(options->GetScrollBeyondLastLine());
        m_checkBoxAdjustScrollbarSize->SetValue(options->GetAutoAdjustHScrollBarWidth());
        m_checkBoxCaretOnVirtualSpace->SetValue(options->GetOptions() & OptionsConfig::Opt_AllowCaretAfterEndOfLine);
        m_checkBoxBlockCaret->SetValue(options->GetOptions() & OptionsConfig::Opt_UseBlockCaret);
    }
    //--------------------------------------------------------------------------
    EditorSettingsCaret::~EditorSettingsCaret()
    {
        m_staticText3->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsCaret::OnCaretWidthUI), NULL, this);
        m_spinCtrlCaretWidth->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsCaret::OnCaretWidthUI), NULL, this);

    }
    //---------------------------------------------------------------------------------
    void EditorSettingsCaret::Save(OptionsConfig * options)
    {
        options->SetCaretBlinkPeriod(StringUtils::wxStringToInt(m_spinCtrlBlinkPeriod->GetValue(), 500, 0));
        options->SetCaretWidth(StringUtils::wxStringToInt(m_spinCtrlCaretWidth->GetValue(), 2, 2, 10));
        options->SetCaretUseCamelCase(m_checkBoxCaretUseCamelCase->IsChecked());
        options->SetScrollBeyondLastLine(m_checkBoxScrollBeyondLastLine->IsChecked());
        options->SetAutoAdjustHScrollBarWidth(m_checkBoxAdjustScrollbarSize->IsChecked());

        size_t flags = options->GetOptions();
        flags &= ~OptionsConfig::Opt_AllowCaretAfterEndOfLine;

        if (m_checkBoxCaretOnVirtualSpace->IsChecked()) {
            flags |= OptionsConfig::Opt_AllowCaretAfterEndOfLine;
        }
        if (m_checkBoxBlockCaret->IsChecked()) {
            flags |= OptionsConfig::Opt_UseBlockCaret;
        }
        else {
            flags &= ~OptionsConfig::Opt_UseBlockCaret;
        }
        options->SetOptions(flags);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsCaret::OnCaretWidthUI(wxUpdateUIEvent& event)
    {
        event.Enable(!m_checkBoxBlockCaret->IsChecked());
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorOptionsGeneralSavePanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorOptionsGeneralSavePanel::EditorOptionsGeneralSavePanel(wxWindow* parent, wxWindowID id, 
        const wxPoint& pos, const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer3);

        m_checkBoxTrimLine = new wxCheckBox(this, wxID_ANY, _("When saving files, trim empty lines"), wxDefaultPosition, wxDefaultSize, 0);
        m_checkBoxTrimLine->SetValue(true);
        bSizer3->Add(m_checkBoxTrimLine, 0, wxALL, 5);

        wxFlexGridSizer* fgSizer1;
        fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->AddGrowableCol(1);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        m_checkBoxDontTrimCurrentLine = new wxCheckBox(this, wxID_ANY, _("Do not trim the caret line"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_checkBoxDontTrimCurrentLine, 0, wxRIGHT | wxLEFT, 20);

        fgSizer1->Add(0, 0, 1, wxEXPAND | wxTOP, 5);
        fgSizer1->Add(0, 0, 1, wxEXPAND | wxTOP, 5);
        fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

        m_checkBoxTrimModifiedLines = new wxCheckBox(this, wxID_ANY, _("Trim only modified lines"), wxDefaultPosition, wxDefaultSize, 0);
        fgSizer1->Add(m_checkBoxTrimModifiedLines, 0, wxRIGHT | wxLEFT, 20);

        bSizer3->Add(fgSizer1, 0, wxEXPAND | wxALL, 5);

        m_checkBoxAppendLF = new wxCheckBox(this, wxID_ANY, _("If missing, append EOL at end of file"), wxDefaultPosition, wxDefaultSize, 0);
        bSizer3->Add(m_checkBoxAppendLF, 0, wxALL, 5);

        // Connect Events
        m_checkBoxDontTrimCurrentLine->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorOptionsGeneralSavePanel::OnTrimCaretLineUI), NULL, this);
        m_checkBoxTrimModifiedLines->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorOptionsGeneralSavePanel::OnTrimCaretLineUI), NULL, this);
    
        long trim = gsgs_EditorConfig().GetInteger(wxT("EditorTrimEmptyLines"), 0);
        m_checkBoxTrimLine->SetValue(trim ? true : false);

        long appendLf = gsgs_EditorConfig().GetInteger(wxT("EditorAppendLf"), 0);
        m_checkBoxAppendLF->SetValue(appendLf ? true : false);

        long dontTrimCaretLine = gsgs_EditorConfig().GetInteger(wxT("DontTrimCaretLine"), 0);
        m_checkBoxDontTrimCurrentLine->SetValue((trim && dontTrimCaretLine) ? true : false);
        m_checkBoxTrimModifiedLines->SetValue(trim && gsgs_EditorConfig().GetOptions()->GetTrimOnlyModifiedLines());
    }
    //--------------------------------------------------------------------------
    EditorOptionsGeneralSavePanel::~EditorOptionsGeneralSavePanel()
    {
        // Disconnect Events
        m_checkBoxDontTrimCurrentLine->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorOptionsGeneralSavePanel::OnTrimCaretLineUI), NULL, this);
        m_checkBoxTrimModifiedLines->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorOptionsGeneralSavePanel::OnTrimCaretLineUI), NULL, this);

    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralSavePanel::Save(OptionsConfig * options)
    {
        bool enableTrim = m_checkBoxTrimLine->IsChecked();
        gsgs_EditorConfig().SetInteger("EditorTrimEmptyLines", enableTrim ? 1 : 0);
        gsgs_EditorConfig().SetInteger("EditorAppendLf", m_checkBoxAppendLF->IsChecked() ? 1 : 0);
        gsgs_EditorConfig().SetInteger(
            "DontTrimCaretLine", (enableTrim && m_checkBoxDontTrimCurrentLine->IsChecked()) ? 1 : 0);
        options->SetTrimOnlyModifiedLines(enableTrim && m_checkBoxTrimModifiedLines->IsChecked());
    }
    //---------------------------------------------------------------------------------
    void EditorOptionsGeneralSavePanel::OnTrimCaretLineUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_checkBoxTrimLine->IsChecked());
        if (!m_checkBoxTrimLine->IsChecked()) {
            event.Check(false);
        }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsComments
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsComments::EditorSettingsComments(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);

        mainSizer->Add(bSizer2, 0, wxALL | wxEXPAND, 5);

        m_checkBoxSmartAddFiles = new wxCheckBox(this, wxID_ANY, _("When adding new files to a project, place the files in the 'include' / 'src' folders respectively"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxSmartAddFiles->SetValue(false);

        bSizer2->Add(m_checkBoxSmartAddFiles, 0, wxALL, 10);

        wxBoxSizer* boxSizer1 = new wxBoxSizer(wxHORIZONTAL);

        bSizer2->Add(boxSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Comments:"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText3->Enable(false);

        boxSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);

        boxSizer1->Add(m_staticline1, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* boxSizer4 = new wxBoxSizer(wxVERTICAL);

        bSizer2->Add(boxSizer4, 0, wxALL, 5);

        m_checkBoxContCComment = new wxCheckBox(this, wxID_ANY, _("Hitting <ENTER> in a C style comment automatically adds a '*' to the next line"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxContCComment->SetValue(true);

        boxSizer4->Add(m_checkBoxContCComment, 0, wxALL, 5);

        m_checkBoxContinueCppComment = new wxCheckBox(this, wxID_ANY, _("Hitting <ENTER> in a C++ style comment section automatically adds a '//' to the next line"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxContinueCppComment->SetValue(false);

        boxSizer4->Add(m_checkBoxContinueCppComment, 0, wxALL, 5);

        wxBoxSizer* boxSizer5 = new wxBoxSizer(wxHORIZONTAL);

        bSizer2->Add(boxSizer5, 0, wxALL | wxEXPAND, 5);

        m_staticText7 = gsgs_Style().createStaticText(this, wxID_ANY, _("Code Navigation Accelerators:"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText7->Enable(false);

        boxSizer5->Add(m_staticText7, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_staticline2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);

        boxSizer5->Add(m_staticline2, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxHORIZONTAL);

        bSizer2->Add(bSizer3, 0, wxALL | wxALIGN_LEFT, 5);

        m_staticText1 = gsgs_Style().createStaticText(this, wxID_ANY, _("Code navigation key:"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_staticText1->SetToolTip(_("When using quick code navigation use this keys in combination with mouse click\nTo quickly go to implementation/declaration.\nNote that at least one box must be ticked, or it would be triggered by every left-click."));
        bSizer3->Add(m_staticText1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Mouse Left Click +"), wxDefaultPosition, wxSize(-1, -1), 0);
        bSizer3->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxCtrl = new wxCheckBox(this, wxID_ANY, _("Ctrl"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxCtrl->SetValue(true);

        bSizer3->Add(m_checkBoxCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        m_checkBoxAlt = new wxCheckBox(this, wxID_ANY, _("Alt"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxAlt->SetValue(true);

        bSizer3->Add(m_checkBoxAlt, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

        CommentConfigData data;
        gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);

        m_checkBoxContCComment->SetValue(data.GetAddStarOnCComment());
        m_checkBoxContinueCppComment->SetValue(data.GetContinueCppComment());
        m_checkBoxSmartAddFiles->SetValue(gsgs_EditorConfig().GetOptions()->GetOptions() & OptionsConfig::Opt_SmartAddFiles);

        size_t flags = gsgs_EditorConfig().GetOptions()->GetOptions();

        if (!(flags & (OptionsConfig::Opt_NavKey_Alt | OptionsConfig::Opt_NavKey_Control))) {
            flags = OptionsConfig::Opt_NavKey_Alt | OptionsConfig::Opt_NavKey_Control; // force the least-instrusive meta key default
        }

        m_checkBoxAlt->SetValue(flags & OptionsConfig::Opt_NavKey_Alt);
        m_checkBoxCtrl->SetValue(flags & OptionsConfig::Opt_NavKey_Control);
    }
    //--------------------------------------------------------------------------
    EditorSettingsComments::~EditorSettingsComments()
    {
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsComments::Save(OptionsConfig * options)
    {
        CommentConfigData data;
        gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);

        data.SetAddStarOnCComment(m_checkBoxContCComment->IsChecked());
        data.SetContinueCppComment(m_checkBoxContinueCppComment->IsChecked());

        gsgs_EditorConfig().WriteObject(wxT("CommentConfigData"), &data);
        size_t flags = options->GetOptions();
        if (m_checkBoxSmartAddFiles->IsChecked())
            flags |= OptionsConfig::Opt_SmartAddFiles;
        else
            flags &= ~OptionsConfig::Opt_SmartAddFiles;

        // clear the navigation key code
        flags &= ~(OptionsConfig::Opt_NavKey_Alt | OptionsConfig::Opt_NavKey_Control | OptionsConfig::Opt_NavKey_Shift);

        if (m_checkBoxCtrl->IsChecked())
            flags |= OptionsConfig::Opt_NavKey_Control;

        if (m_checkBoxAlt->IsChecked())
            flags |= OptionsConfig::Opt_NavKey_Alt;

        if (!(flags & (OptionsConfig::Opt_NavKey_Alt | OptionsConfig::Opt_NavKey_Control))) {
            flags |= OptionsConfig::Opt_NavKey_Alt | OptionsConfig::Opt_NavKey_Control; // force the least-instrusive meta key default
        }

        options->SetOptions(flags);
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsCommentsDoxygenPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsCommentsDoxygenPanel::EditorSettingsCommentsDoxygenPanel(wxWindow* parent, wxWindowID id,
        const wxPoint& pos, const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style),
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_staticText22 = gsgs_Style().createStaticText(this, wxID_ANY, _("Class doc template:"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        bSizer1->Add(m_staticText22, 0, wxALL, WXC_FROM_DIP(5));

        m_stcClassTemplate = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        // Configure the fold margin
        m_stcClassTemplate->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_stcClassTemplate->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_stcClassTemplate->SetMarginSensitive(4, true);
        m_stcClassTemplate->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_stcClassTemplate->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_stcClassTemplate->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_stcClassTemplate->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_stcClassTemplate->SetMarginWidth(2, 0);
        m_stcClassTemplate->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_stcClassTemplate->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_stcClassTemplate->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_stcClassTemplate->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_stcClassTemplate->SetMarginMask(3, 0);
        m_stcClassTemplate->SetMarginWidth(3, 0);
        // Select the lexer
        m_stcClassTemplate->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_stcClassTemplate->StyleClearAll();
        m_stcClassTemplate->SetWrapMode(2);
        m_stcClassTemplate->SetIndentationGuides(0);
        m_stcClassTemplate->SetEOLMode(2);
        m_stcClassTemplate->SetKeyWords(0, wxT(""));
        m_stcClassTemplate->SetKeyWords(1, wxT(""));
        m_stcClassTemplate->SetKeyWords(2, wxT(""));
        m_stcClassTemplate->SetKeyWords(3, wxT(""));
        m_stcClassTemplate->SetKeyWords(4, wxT(""));

        bSizer1->Add(m_stcClassTemplate, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText26 = gsgs_Style().createStaticText(this, wxID_ANY, _("Function doc template:"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        bSizer1->Add(m_staticText26, 0, wxALL, WXC_FROM_DIP(5));

        m_stcFunctionTemplate =
            new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        // Configure the fold margin
        m_stcFunctionTemplate->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_stcFunctionTemplate->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_stcFunctionTemplate->SetMarginSensitive(4, true);
        m_stcFunctionTemplate->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_stcFunctionTemplate->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_stcFunctionTemplate->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_stcFunctionTemplate->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_stcFunctionTemplate->SetMarginWidth(2, 0);
        m_stcFunctionTemplate->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_stcFunctionTemplate->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_stcFunctionTemplate->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_stcFunctionTemplate->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_stcFunctionTemplate->SetMarginMask(3, 0);
        m_stcFunctionTemplate->SetMarginWidth(3, 0);
        // Select the lexer
        m_stcFunctionTemplate->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_stcFunctionTemplate->StyleClearAll();
        m_stcFunctionTemplate->SetWrapMode(2);
        m_stcFunctionTemplate->SetIndentationGuides(0);
        m_stcFunctionTemplate->SetEOLMode(2);
        m_stcFunctionTemplate->SetKeyWords(0, wxT(""));
        m_stcFunctionTemplate->SetKeyWords(1, wxT(""));
        m_stcFunctionTemplate->SetKeyWords(2, wxT(""));
        m_stcFunctionTemplate->SetKeyWords(3, wxT(""));
        m_stcFunctionTemplate->SetKeyWords(4, wxT(""));

        bSizer1->Add(m_stcFunctionTemplate, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer29 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(boxSizer29, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxAutoInsert = new wxCheckBox(this, wxID_ANY, _("Auto generate document block"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_checkBoxAutoInsert->SetValue(true);
        m_checkBoxAutoInsert->SetToolTip(
            _("Generate document block when user types '/**' and hits ENTER\non top of a function"));

        boxSizer29->Add(m_checkBoxAutoInsert, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxQtStyle = new wxCheckBox(this, wxID_ANY, _("Use Qt document style"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_checkBoxQtStyle->SetValue(false);

        boxSizer29->Add(m_checkBoxQtStyle, 0, wxALL, WXC_FROM_DIP(5));

        CommentConfigData data;
        gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);
        m_checkBoxQtStyle->SetValue(data.IsUseQtStyle());
        m_checkBoxAutoInsert->SetValue(data.IsAutoInsert());
        m_stcClassTemplate->SetText(data.GetClassPattern());
        m_stcFunctionTemplate->SetText(data.GetFunctionPattern());
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++");
        if (lexer) {
            lexer->Apply(m_stcClassTemplate);
            lexer->Apply(m_stcFunctionTemplate);
        }
    }
    //--------------------------------------------------------------------------
    EditorSettingsCommentsDoxygenPanel::~EditorSettingsCommentsDoxygenPanel() 
    {
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsCommentsDoxygenPanel::Save(OptionsConfig *)
    {
        CommentConfigData data;
        gsgs_EditorConfig().ReadObject(wxT("CommentConfigData"), &data);

        wxString classPattern = m_stcClassTemplate->GetText();
        wxString funcPattern = m_stcFunctionTemplate->GetText();
        classPattern.Replace("\\n", "\n");
        funcPattern.Replace("\\n", "\n");

        data.SetClassPattern(classPattern);
        data.SetFunctionPattern(funcPattern);
        data.SetAutoInsert(m_checkBoxAutoInsert->IsChecked());
        data.SetUseQtStyle(m_checkBoxQtStyle->IsChecked());
        gsgs_EditorConfig().WriteObject(wxT("CommentConfigData"), &data);
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsFolding
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsFolding::EditorSettingsFolding(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_displayMargin = new wxCheckBox(this, wxID_ANY, _("Display Folding Margin"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_displayMargin->SetValue(true);

        mainSizer->Add(m_displayMargin, 0, wxALL | wxEXPAND, 5);

        wxStaticBoxSizer* sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("")), wxVERTICAL);

        mainSizer->Add(sbSizer1, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 5);

        wxArrayString m_foldStyleArr;
        m_foldStyle = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_foldStyleArr, 0);
        m_foldStyle->SetToolTip(_("Select the folding style"));

        sbSizer1->Add(m_foldStyle, 0, wxALL | wxEXPAND, 5);

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(0);
        fgSizer1->AddGrowableCol(1);

        sbSizer1->Add(fgSizer1, 0, wxLEFT | wxRIGHT | wxEXPAND, 5);

        m_underlineFolded = new wxCheckBox(this, wxID_ANY, _("Underline Folded Line"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_underlineFolded->SetValue(true);

        fgSizer1->Add(m_underlineFolded, 0, wxALL | wxEXPAND, 5);

        m_foldElse = new wxCheckBox(this, wxID_ANY, _("Fold At Else"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_foldElse->SetValue(false);

        fgSizer1->Add(m_foldElse, 0, wxALL | wxEXPAND, 5);

        m_foldPreprocessors = new wxCheckBox(this, wxID_ANY, _("Fold Preprocessors"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_foldPreprocessors->SetValue(false);

        fgSizer1->Add(m_foldPreprocessors, 0, wxALL | wxEXPAND, 5);

        m_foldCompact = new wxCheckBox(this, wxID_ANY, _("Fold Compact"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_foldCompact->SetValue(false);

        fgSizer1->Add(m_foldCompact, 0, wxALL | wxEXPAND, 5);

        m_checkBoxHighlightFolding = new wxCheckBox(this, wxID_ANY, _("Highlight Active Fold Block"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_checkBoxHighlightFolding->SetValue(false);
        m_checkBoxHighlightFolding->SetToolTip(_("Allows enabling/disabling the highlight folding block when it is selected. (i.e. block that contains the caret)"));

        fgSizer1->Add(m_checkBoxHighlightFolding, 0, wxALL, 5);

        CentreOnParent(wxBOTH);
        // Connect events
        m_foldStyle->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_underlineFolded->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldElse->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldPreprocessors->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldCompact->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_checkBoxHighlightFolding->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_displayMargin->SetValue(options->GetDisplayFoldMargin());
        m_underlineFolded->SetValue(options->GetUnderlineFoldLine());
        m_foldPreprocessors->SetValue(options->GetFoldPreprocessor());
        m_foldCompact->SetValue(options->GetFoldCompact());
        m_foldElse->SetValue(options->GetFoldAtElse());
        m_checkBoxHighlightFolding->SetValue(options->IsHighlightFoldWhenActive());

        const wxString FoldStyles[] = { wxTRANSLATE("Simple"),
                                        wxTRANSLATE("Arrows"),
                                        wxTRANSLATE("Flatten Tree Square Headers"),
                                        wxTRANSLATE("Flatten Tree Circular Headers") };

        m_stringManager.AddStrings(sizeof(FoldStyles) / sizeof(wxString), FoldStyles, options->GetFoldStyle(), m_foldStyle);
    }
    //--------------------------------------------------------------------------
    EditorSettingsFolding::~EditorSettingsFolding()
    {
        m_foldStyle->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_underlineFolded->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldElse->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldPreprocessors->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_foldCompact->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
        m_checkBoxHighlightFolding->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsFolding::OnFoldingMarginUI), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void EditorSettingsFolding::Save(OptionsConfig * options)
    {
        options->SetDisplayFoldMargin(m_displayMargin->GetValue());
        options->SetUnderlineFoldLine(m_underlineFolded->GetValue());
        options->SetFoldPreprocessor(m_foldPreprocessors->GetValue());
        options->SetFoldCompact(m_foldCompact->GetValue());
        options->SetFoldAtElse(m_foldElse->GetValue());
        options->SetHighlightFoldWhenActive(m_checkBoxHighlightFolding->IsChecked());

        // Get the foldstyle selection, unlocalised
        wxString foldStyle = m_stringManager.GetStringSelection();

        // thses 2 styles no longer exists...
        if (foldStyle == _("Arrows with Background Colour") || foldStyle == _("Simple with Background Colour"))
            foldStyle.Clear();

        if (foldStyle.IsEmpty()) {
            foldStyle = wxT("Arrows");
        }

        options->SetFoldStyle(foldStyle);
    }
    //-----------------------------------------------------------------------------
    void EditorSettingsFolding::OnFoldingMarginUI(wxUpdateUIEvent& event) { event.Enable(m_displayMargin->IsChecked()); }
    //-----------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsBookmarksPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsBookmarksPanel::EditorSettingsBookmarksPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
        , TreeBookNode()
        , m_previous(wxNOT_FOUND)
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer2);

        m_displaySelection = new wxCheckBox(this, wxID_ANY, _("Display Breakpoints / Bookmarks margin"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_displaySelection->SetValue(true);

        bSizer2->Add(m_displaySelection, 0, wxALL, 5);

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(1);

        bSizer2->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);

        m_staticText3 = gsgs_Style().createStaticText(this, wxID_ANY, _("Bookmark Shape:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_bookMarkShapeArr;
        m_bookMarkShape = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_bookMarkShapeArr, 0);

        fgSizer1->Add(m_bookMarkShape, 0, wxALL | wxEXPAND, 5);

        m_staticText10 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select bookmark type:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText10, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        wxArrayString m_choiceBMTypeArr;
        m_choiceBMTypeArr.Add(wxT("1 - Default bookmark"));
        m_choiceBMTypeArr.Add(wxT("2"));
        m_choiceBMTypeArr.Add(wxT("3"));
        m_choiceBMTypeArr.Add(wxT("4"));
        m_choiceBMTypeArr.Add(wxT("5 - Find-bar bookmark"));
        m_choiceBMType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), m_choiceBMTypeArr, 0);
        m_choiceBMType->SetToolTip(_("Select the bookmark type from the following list"));
        m_choiceBMType->SetSelection(0);

        fgSizer1->Add(m_choiceBMType, 0, wxALL | wxEXPAND, 5);

        m_staticText41 = gsgs_Style().createStaticText(this, wxID_ANY, _("Bookmark label:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText41, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_BookmarkLabel = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(-1, -1), 0);
        m_BookmarkLabel->SetToolTip(_("The label shown in e.g. a tooltip. You can set it to something descriptive if you wish."));

        fgSizer1->Add(m_BookmarkLabel, 0, wxALL | wxEXPAND, 5);

        m_staticText4 = gsgs_Style().createStaticText(this, wxID_ANY, _("Background colour:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_backgroundColor = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxSize(-1, -1), wxCLRP_DEFAULT_STYLE);

        fgSizer1->Add(m_backgroundColor, 0, wxALL | wxEXPAND, 5);

        m_staticText5 = gsgs_Style().createStaticText(this, wxID_ANY, _("Foreground colour:"), wxDefaultPosition, wxSize(-1, -1), 0);
        fgSizer1->Add(m_staticText5, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_foregroundColor = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxSize(-1, -1), wxCLRP_DEFAULT_STYLE);

        fgSizer1->Add(m_foregroundColor, 0, wxALL | wxEXPAND, 5);

        m_staticLine18 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxLI_HORIZONTAL);

        bSizer2->Add(m_staticLine18, 0, wxALL | wxEXPAND, 5);

        wxFlexGridSizer* flexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer4->SetFlexibleDirection(wxBOTH);
        flexGridSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer4->AddGrowableCol(1);

        bSizer2->Add(flexGridSizer4, 0, wxALL | wxEXPAND, 5);

        m_staticText66 = gsgs_Style().createStaticText(this, wxID_ANY, _("'Highlight Matching Word' colour:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer4->Add(m_staticText66, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_highlightColor = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxSize(-1, -1), wxCLRP_DEFAULT_STYLE);
        m_highlightColor->SetToolTip(_("This lets you set the 'Highlight Matching Word' colour (the colour of words that match the selection). To set the colour of the selection itself, see 'Settings > Syntax Highlight and Fonts'."));

        flexGridSizer4->Add(m_highlightColor, 0, wxALL | wxEXPAND, 5);

        m_staticText54 = gsgs_Style().createStaticText(this, wxID_ANY, _("'Highlight Matching Word' alpha:"), wxDefaultPosition, wxSize(-1, -1), 0);
        flexGridSizer4->Add(m_staticText54, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);

        m_spinCtrlHighlightAlpha = new wxSpinCtrl(this, wxID_ANY, wxT("128"), wxDefaultPosition, wxSize(-1, -1), wxSP_ARROW_KEYS);
        m_spinCtrlHighlightAlpha->SetToolTip(_("This affects the intensity of the colour set in the field above (for words matching the selection). Choose a value between 0 and 256. Higher values give a less-transparent background."));
        m_spinCtrlHighlightAlpha->SetRange(0, 255);
        m_spinCtrlHighlightAlpha->SetValue(128);

        flexGridSizer4->Add(m_spinCtrlHighlightAlpha, 0, wxALL | wxEXPAND, 5);

        m_clearHighlitWords = new wxCheckBox(this, wxID_ANY, _("Find/FindNext clears highlit matching words"), wxDefaultPosition, wxSize(-1, -1), 0);
        m_clearHighlitWords->SetValue(false);
        m_clearHighlitWords->SetToolTip(_("By default, 'Find', FindNext and FindPrevious  will clear all  current 'Highlight Matching Word' matches. Untick this box to prevent that happening."));

        bSizer2->Add(m_clearHighlitWords, 0, wxALL, 10);

        CentreOnParent(wxBOTH);
        // Connect events
        m_choiceBMType->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(EditorSettingsBookmarksPanel::OnBookmarkChanged), NULL, this);

        m_highlightColor->SetColour(wxString(wxT("LIGHT BLUE")));

        // get the editor's options from the disk
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        m_displaySelection->SetValue(options->GetDisplayBookmarkMargin());

        // These are localised inside m_bookMarkShape. However serialising the translated strings will break other
        // locales...
        const wxString UnlocalisedShapes[] = { wxTRANSLATE("Small Rectangle"), wxTRANSLATE("Rounded Rectangle"),
                                               wxTRANSLATE("Circle"), wxTRANSLATE("Small Arrow"), wxTRANSLATE("Bookmark") };
        m_stringManager.AddStrings(sizeof(UnlocalisedShapes) / sizeof(wxString), UnlocalisedShapes,
            options->GetBookmarkShape(), m_bookMarkShape);

        for (size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
            BookmarkData arr;
            wxColour col = options->GetBookmarkBgColour(n);
            if (!col.IsOk()) { col = options->GetBookmarkBgColour(0); }
            arr.bg = col;

            col = options->GetBookmarkFgColour(n);
            if (!col.IsOk()) { col = options->GetBookmarkFgColour(0); }
            arr.fg = col;

            arr.defaultLabel = m_choiceBMType->GetString(n); // Store the default  value
            arr.label = options->GetBookmarkLabel(n);
            if (!arr.label.empty()) {
                wxCHECK_RET(n < m_choiceBMType->GetCount(), "More bookmark types than there are Choice entries");
                m_choiceBMType->SetString(n, arr.label);
            }

            m_bookmarksData.push_back(arr);
        }
        ChangeSelection(0); // Fake a change to display the standard-bookmark values

        wxString val1 = gsgs_EditorConfig().GetString(wxT("WordHighlightColour"));
        if (val1.IsEmpty() == false) 
        { 
            m_highlightColor->SetColour(val1); 
        }

        long alpha = gsgs_EditorConfig().GetInteger(wxT("WordHighlightAlpha"));
        if (alpha != wxNOT_FOUND) 
        { 
            m_spinCtrlHighlightAlpha->SetValue(alpha); 
        }
        m_clearHighlitWords->SetValue(options->GetClearHighlitWordsOnFind());
    }
    //--------------------------------------------------------------------------
    EditorSettingsBookmarksPanel::~EditorSettingsBookmarksPanel()
    {
        m_choiceBMType->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(EditorSettingsBookmarksPanel::OnBookmarkChanged), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void EditorSettingsBookmarksPanel::Save(OptionsConfig * options)
    {
        options->SetDisplayBookmarkMargin(m_displaySelection->IsChecked());

        // Get the bookmark selection, unlocalised
        wxString bmShape = m_stringManager.GetStringSelection();
        if (bmShape.IsEmpty()) { bmShape = wxT("Bookmark"); }
        options->SetBookmarkShape(bmShape);

        ChangeSelection(m_choiceBMType->GetSelection()); // Fake a change to store any altered values

        for (size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
            BookmarkData& arr = m_bookmarksData.at(n);
            options->SetBookmarkBgColour(arr.bg, n);
            options->SetBookmarkFgColour(arr.fg, n);
            options->SetBookmarkLabel(arr.label, n);
        }

        gsgs_EditorConfig().SetString(wxT("WordHighlightColour"), m_highlightColor->GetColour().GetAsString());
        gsgs_EditorConfig().SetInteger(wxT("WordHighlightAlpha"), (long)m_spinCtrlHighlightAlpha->GetValue());
        options->SetClearHighlitWordsOnFind(m_clearHighlitWords->IsChecked());
    }
    //-----------------------------------------------------------------------------
    void EditorSettingsBookmarksPanel::ChangeSelection(int index)
    {
        wxCHECK_RET(index < CL_N0_OF_BOOKMARK_TYPES, "Bookmark type out of range");

        if (m_previous >= 0) {
            // Store the current data in case it was just altered
            BookmarkData& arr = m_bookmarksData.at(m_previous);
            arr.bg = m_backgroundColor->GetColour();
            arr.fg = m_foregroundColor->GetColour();
            arr.label = m_BookmarkLabel->GetValue();
            if (m_BookmarkLabel->GetValue().empty()) {
                m_choiceBMType->SetString(m_previous, arr.defaultLabel);
            }
            else {
                m_choiceBMType->SetString(m_previous, arr.label);
            }
        }

        BookmarkData arr = m_bookmarksData.at(index);
        m_backgroundColor->SetColour(arr.bg);
        m_foregroundColor->SetColour(arr.fg);
        m_BookmarkLabel->ChangeValue(arr.label);

        m_previous = index;
    }
    //-----------------------------------------------------------------------------
    void EditorSettingsBookmarksPanel::OnBookmarkChanged(wxCommandEvent& event) 
    { 
        ChangeSelection(event.GetSelection()); 
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // EditorSettingsDockingWindows
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    EditorSettingsDockingWindows::EditorSettingsDockingWindows(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style),
        TreeBookNode()
    {
        wxWindowUpdateLocker(this);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        m_notebook10 = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        m_notebook10->SetName(wxT("m_notebook10"));

        mainSizer->Add(m_notebook10, 1, wxEXPAND, WXC_FROM_DIP(0));

        m_panelTabs = new wxPanel(m_notebook10, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook10, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook10->AddPage(m_panelTabs, _("Team"), true);

        wxBoxSizer* boxSizer56 = new wxBoxSizer(wxVERTICAL);
        m_panelTabs->SetSizer(boxSizer56);

        wxFlexGridSizer* flexGridSizer36 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer36->SetFlexibleDirection(wxBOTH);
        flexGridSizer36->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer36->AddGrowableCol(1);

        boxSizer56->Add(flexGridSizer36, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText46 = gsgs_Style().createStaticText(m_panelTabs, wxID_ANY, _("Tab Style:"), wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        flexGridSizer36->Add(m_staticText46, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceTabStyleArr;
        m_choiceTabStyle = new wxChoice(m_panelTabs, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)),
            m_choiceTabStyleArr, 0);

        flexGridSizer36->Add(m_choiceTabStyle, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticTextTabHeight = gsgs_Style().createStaticText(m_panelTabs, wxID_ANY, _("Tab Height:"), wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        flexGridSizer36->Add(m_staticTextTabHeight, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceTabHeightArr;
        m_choiceTabHeightArr.Add(wxT("TALL"));
        m_choiceTabHeightArr.Add(wxT("MEDIUM"));
        m_choiceTabHeightArr.Add(wxT("SHORT"));
        m_choiceTabHeightArr.Add(wxT("TINY"));
        m_choiceTabHeight = new wxChoice(m_panelTabs, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)),
            m_choiceTabHeightArr, 0);
        m_choiceTabHeight->SetSelection(3);

        flexGridSizer36->Add(m_choiceTabHeight, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText63 = gsgs_Style().createStaticText(m_panelTabs, wxID_ANY, _("Active Tab Marker Colour:"), wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        flexGridSizer36->Add(m_staticText63, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_colourPickerMarker =
            new wxColourPickerCtrl(m_panelTabs, wxID_ANY, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
                wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), wxCLRP_DEFAULT_STYLE);

        flexGridSizer36->Add(m_colourPickerMarker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText38 = gsgs_Style().createStaticText(m_panelTabs, wxID_ANY, _("Workspace Pane Tabs Orientation:"), wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        flexGridSizer36->Add(m_staticText38, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceWorkspaceTabsOrientationArr;
        m_choiceWorkspaceTabsOrientationArr.Add(wxT("LEFT"));
        m_choiceWorkspaceTabsOrientationArr.Add(wxT("RIGHT"));
        m_choiceWorkspaceTabsOrientationArr.Add(wxT("TOP"));
        m_choiceWorkspaceTabsOrientationArr.Add(wxT("BOTTOM"));
        m_choiceWorkspaceTabsOrientation =
            new wxChoice(m_panelTabs, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)),
                m_choiceWorkspaceTabsOrientationArr, 0);
        m_choiceWorkspaceTabsOrientation->SetSelection(2);

        flexGridSizer36->Add(m_choiceWorkspaceTabsOrientation, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL,
            WXC_FROM_DIP(5));

        m_staticText42 = gsgs_Style().createStaticText(m_panelTabs, wxID_ANY, _("Output Pane Tabs Orientation:"), wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        flexGridSizer36->Add(m_staticText42, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceOutputTabsOrientationArr;
        m_choiceOutputTabsOrientationArr.Add(wxT("TOP"));
        m_choiceOutputTabsOrientationArr.Add(wxT("BOTTOM"));
        m_choiceOutputTabsOrientation =
            new wxChoice(m_panelTabs, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)),
                m_choiceOutputTabsOrientationArr, 0);
        m_choiceOutputTabsOrientation->SetSelection(0);

        flexGridSizer36->Add(m_choiceOutputTabsOrientation, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_staticLine61 = new wxStaticLine(m_panelTabs, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)),
            wxLI_HORIZONTAL);

        boxSizer56->Add(m_staticLine61, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxShowXButton = new wxCheckBox(m_panelTabs, wxID_ANY, _("Show close button on active tab"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        m_checkBoxShowXButton->SetValue(true);
        m_checkBoxShowXButton->SetToolTip(_("Show close button on active tab"));

        boxSizer56->Add(m_checkBoxShowXButton, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxEditorTabsFollowsTheme =
            new wxCheckBox(m_panelTabs, wxID_ANY, _("The editor tabs matches to the editor colour theme"),
                wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        m_checkBoxEditorTabsFollowsTheme->SetValue(true);
        m_checkBoxEditorTabsFollowsTheme->SetToolTip(_("The editor tabs matches to the editor colour theme"));

        boxSizer56->Add(m_checkBoxEditorTabsFollowsTheme, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxMouseScrollSwitchTabs = new wxCheckBox(m_panelTabs, wxID_ANY, _("Mouse scroll switches between tabs"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        m_checkBoxMouseScrollSwitchTabs->SetValue(false);

        boxSizer56->Add(m_checkBoxMouseScrollSwitchTabs, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxSortTabsDropdownAlphabetically =
            new wxCheckBox(m_panelTabs, wxID_ANY, _("Sort the 'More...' dropdown alphabetically"), wxDefaultPosition,
                wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        m_checkBoxSortTabsDropdownAlphabetically->SetValue(false);
        m_checkBoxSortTabsDropdownAlphabetically->SetToolTip(
            _("Display the 'More tabs...' dropdown sorted alphabetically, rather than by tab order\n"));

        boxSizer56->Add(m_checkBoxSortTabsDropdownAlphabetically, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxEnableTabSwitchingKey =
            new wxCheckBox(m_panelTabs, wxID_ANY, _("Enable tab switching with Ctrl-TAB key shortcut"), wxDefaultPosition,
                wxDLG_UNIT(m_panelTabs, wxSize(-1, -1)), 0);
        m_checkBoxEnableTabSwitchingKey->SetValue(true);

        boxSizer56->Add(m_checkBoxEnableTabSwitchingKey, 0, wxALL, WXC_FROM_DIP(5));

        m_panel12 = new wxPanel(m_notebook10, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook10, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook10->AddPage(m_panel12, _("Docking"));

        wxBoxSizer* boxSizer22 = new wxBoxSizer(wxVERTICAL);
        m_panel12->SetSizer(boxSizer22);

        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);

        boxSizer22->Add(bSizer2, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer70 = new wxBoxSizer(wxVERTICAL);

        bSizer2->Add(boxSizer70, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_radioBoxHintArr;
        m_radioBoxHintArr.Add(_("Transparent hint"));
        m_radioBoxHintArr.Add(_("Rectangle hint"));
        m_radioBoxHintArr.Add(_("Venetian blinds hints"));
        m_radioBoxHint = new wxRadioBox(m_panel12, wxID_ANY, _("Docking Style:"), wxDefaultPosition,
            wxDLG_UNIT(m_panel12, wxSize(-1, -1)), m_radioBoxHintArr, 1, wxRA_SPECIFY_ROWS);
        m_radioBoxHint->SetSelection(0);

        boxSizer70->Add(m_radioBoxHint, 1, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, WXC_FROM_DIP(5));

        wxStaticBoxSizer* staticBoxSizer69 =
            new wxStaticBoxSizer(new wxStaticBox(m_panel12, wxID_ANY, _("Docking Panes")), wxVERTICAL);

        bSizer2->Add(staticBoxSizer69, 0, wxALL | wxEXPAND, WXC_FROM_DIP(10));

        wxFlexGridSizer* flexGridSizer82 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer82->SetFlexibleDirection(wxBOTH);
        flexGridSizer82->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer82->AddGrowableCol(1);

        staticBoxSizer69->Add(flexGridSizer82, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxCustomCaptionColour = new wxCheckBox(m_panel12, wxID_ANY, _("Use custom caption colour"),
            wxDefaultPosition, wxDLG_UNIT(m_panel12, wxSize(-1, -1)), 0);
        m_checkBoxCustomCaptionColour->SetValue(false);

        flexGridSizer82->Add(m_checkBoxCustomCaptionColour, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL,
            WXC_FROM_DIP(5));

        m_cpCaptionColour = new wxColourPickerCtrl(m_panel12, wxID_ANY, *wxBLACK, wxDefaultPosition,
            wxDLG_UNIT(m_panel12, wxSize(-1, -1)), wxCLRP_DEFAULT_STYLE);

        flexGridSizer82->Add(m_cpCaptionColour, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxHideCaptions = new wxCheckBox(m_panel12, wxID_ANY, _("Hide Docking Windows captions"), wxDefaultPosition,
            wxDLG_UNIT(m_panel12, wxSize(-1, -1)), 0);
        m_checkBoxHideCaptions->SetValue(false);

        staticBoxSizer69->Add(m_checkBoxHideCaptions, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxEnsureCaptionsVisible =
            new wxCheckBox(m_panel12, wxID_ANY, _("Ensure captions are visible on mouse hover"), wxDefaultPosition,
                wxDLG_UNIT(m_panel12, wxSize(-1, -1)), 0);
        m_checkBoxEnsureCaptionsVisible->SetValue(false);
        m_checkBoxEnsureCaptionsVisible->SetToolTip(
            _("When the option 'Hide Docking Windows captions' is enabled, ensure captions are visible on mouse hover. "
                "This is useful so the user can still move around the docking  windows"));

        staticBoxSizer69->Add(m_checkBoxEnsureCaptionsVisible, 0, wxALL, WXC_FROM_DIP(5));

        wxStaticBoxSizer* staticBoxSizer72 =
            new wxStaticBoxSizer(new wxStaticBox(m_panel12, wxID_ANY, _("Navigation Bar")), wxVERTICAL);

        bSizer2->Add(staticBoxSizer72, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxNavBarSortDropdown = new wxCheckBox(m_panel12, wxID_ANY, _("Sort dropdown alphabetically"),
            wxDefaultPosition, wxDLG_UNIT(m_panel12, wxSize(-1, -1)), 0);
        m_checkBoxNavBarSortDropdown->SetValue(false);

        staticBoxSizer72->Add(m_checkBoxNavBarSortDropdown, 0, wxALL, WXC_FROM_DIP(5));

        m_panel14 = new wxPanel(m_notebook10, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook10, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook10->AddPage(m_panel14, _("Find / Find In Files"));

        wxBoxSizer* boxSizer20 = new wxBoxSizer(wxVERTICAL);
        m_panel14->SetSizer(boxSizer20);

        m_checkBoxDontFoldSearchResults = new wxCheckBox(m_panel14, wxID_ANY, _("Don't automatically fold Search results"),
            wxDefaultPosition, wxDLG_UNIT(m_panel14, wxSize(-1, -1)), 0);
        m_checkBoxDontFoldSearchResults->SetValue(false);
        m_checkBoxDontFoldSearchResults->SetToolTip(
            _("By default, all but the first results of 'Search' are automatically folded; you have to click on each "
                "subsequent file to see its contained matches. Tick this box to prevent this.\nYou can still fold and unfold "
                "results with the button in the output pane toolbar."));

        boxSizer20->Add(m_checkBoxDontFoldSearchResults, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxDontOverrideSearchStringWithSelection = new wxCheckBox(
            m_panel14, wxID_ANY, _("Don't override search string with current selection on find next/previous"),
            wxDefaultPosition, wxDLG_UNIT(m_panel14, wxSize(-1, -1)), 0);
        m_checkBoxDontOverrideSearchStringWithSelection->SetValue(false);
        m_checkBoxDontOverrideSearchStringWithSelection->SetToolTip(
            _("By default, the search string is overridden by the current text selection in the editor when doing a find "
                "next/previous. Tick this box to prevent this"));

        boxSizer20->Add(m_checkBoxDontOverrideSearchStringWithSelection, 0, wxALL, WXC_FROM_DIP(5));

        m_panel16 = new wxPanel(m_notebook10, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook10, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_notebook10->AddPage(m_panel16, _("Debug / Output panes"));

        wxBoxSizer* boxSizer21 = new wxBoxSizer(wxVERTICAL);
        m_panel16->SetSizer(boxSizer21);

        wxBoxSizer* boxSizer19 = new wxBoxSizer(wxVERTICAL);

        boxSizer21->Add(boxSizer19, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneOnClick = new wxCheckBox(
            m_panel16, wxID_ANY, _("When user clicks inside an editor, hide the output pane -- unless it's one of:"),
            wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneOnClick->SetValue(true);
        m_checkBoxHideOutputPaneOnClick->SetToolTip(
            _("When e.g. you compile your project, or use 'Find in Files', the Output Pane opens to show the results. If "
                "this box is ticked, it will automatically close as soon as you click in the editor."));

        boxSizer19->Add(m_checkBoxHideOutputPaneOnClick, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 4, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

        boxSizer19->Add(fgSizer1, 0, wxLEFT | wxRIGHT | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(20));

        m_checkBoxHideOutputPaneNotIfBuild =
            new wxCheckBox(m_panel16, wxID_ANY, _("Build"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfBuild->SetValue(false);
        m_checkBoxHideOutputPaneNotIfBuild->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing e.g. you may not want "
                "it to close while you correct one of many build errors."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfBuild, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfSearch =
            new wxCheckBox(m_panel16, wxID_ANY, _("Search"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfSearch->SetValue(false);
        m_checkBoxHideOutputPaneNotIfSearch->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfSearch, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfReplace =
            new wxCheckBox(m_panel16, wxID_ANY, _("Replace"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfReplace->SetValue(false);
        m_checkBoxHideOutputPaneNotIfReplace->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfReplace, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfReferences = new wxCheckBox(m_panel16, wxID_ANY, _("References"), wxDefaultPosition,
            wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfReferences->SetValue(false);
        m_checkBoxHideOutputPaneNotIfReferences->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfReferences, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfOutput =
            new wxCheckBox(m_panel16, wxID_ANY, _("Output"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfOutput->SetValue(false);
        m_checkBoxHideOutputPaneNotIfOutput->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfOutput, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfTrace =
            new wxCheckBox(m_panel16, wxID_ANY, _("Trace"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfTrace->SetValue(false);
        m_checkBoxHideOutputPaneNotIfTrace->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfTrace, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfTasks =
            new wxCheckBox(m_panel16, wxID_ANY, _("Tasks"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfTasks->SetValue(false);
        m_checkBoxHideOutputPaneNotIfTasks->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfTasks, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfBuildQ =
            new wxCheckBox(m_panel16, wxID_ANY, _("BuildQ"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfBuildQ->SetValue(false);
        m_checkBoxHideOutputPaneNotIfBuildQ->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfBuildQ, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfCppCheck =
            new wxCheckBox(m_panel16, wxID_ANY, _("CppCheck"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfCppCheck->SetValue(false);
        m_checkBoxHideOutputPaneNotIfCppCheck->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfCppCheck, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfSvn = new wxCheckBox(m_panel16, wxID_ANY, _("Subversion"), wxDefaultPosition,
            wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfSvn->SetValue(false);
        m_checkBoxHideOutputPaneNotIfSvn->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfSvn, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfCscope =
            new wxCheckBox(m_panel16, wxID_ANY, _("CScope"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfCscope->SetValue(false);
        m_checkBoxHideOutputPaneNotIfCscope->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfCscope, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfGit =
            new wxCheckBox(m_panel16, wxID_ANY, _("Git"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfGit->SetValue(false);
        m_checkBoxHideOutputPaneNotIfGit->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfGit, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfDebug =
            new wxCheckBox(m_panel16, wxID_ANY, _("Debug"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfDebug->SetValue(true);
        m_checkBoxHideOutputPaneNotIfDebug->SetToolTip(
            _("Don't automatically close the Debugger Pane on an editor click if this tab is showing. You probably don't "
                "want it to close whenever you set a breakpoint, for example."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfDebug, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxHideOutputPaneNotIfMemCheck =
            new wxCheckBox(m_panel16, wxID_ANY, _("MemCheck"), wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxHideOutputPaneNotIfMemCheck->SetValue(true);
        m_checkBoxHideOutputPaneNotIfMemCheck->SetToolTip(
            _("Don't automatically close the Output Pane on an editor click if this tab is showing."));

        fgSizer1->Add(m_checkBoxHideOutputPaneNotIfMemCheck, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxShowDebugOnRun = new wxCheckBox(m_panel16, wxID_ANY, _("Show 'Debug' tab on starting the debugger"),
            wxDefaultPosition, wxDLG_UNIT(m_panel16, wxSize(-1, -1)), 0);
        m_checkBoxShowDebugOnRun->SetValue(true);
        m_checkBoxShowDebugOnRun->SetToolTip(
            _("When starting the debugger, if the Debug tab is not visible, checking this will make it visible"));

        boxSizer21->Add(m_checkBoxShowDebugOnRun, 0, wxALL, WXC_FROM_DIP(5));

        m_cpCaptionColour->Connect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnUseCustomCaptionColourUI),
            NULL, this);
        m_checkBoxEnsureCaptionsVisible->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnEnsureCaptionsVisibleUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfBuild->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfSearch->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfReplace->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfReferences->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfOutput->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfTrace->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfTasks->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfBuildQ->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfCppCheck->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfSvn->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfCscope->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfGit->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfDebug->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfMemCheck->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();

        m_checkBoxHideOutputPaneOnClick->SetValue(options->GetHideOutpuPaneOnUserClick());
        m_checkBoxHideOutputPaneNotIfBuild->SetValue(options->GetHideOutputPaneNotIfBuild());
        m_checkBoxHideOutputPaneNotIfSearch->SetValue(options->GetHideOutputPaneNotIfSearch());
        m_checkBoxHideOutputPaneNotIfReplace->SetValue(options->GetHideOutputPaneNotIfReplace());
        m_checkBoxHideOutputPaneNotIfReferences->SetValue(options->GetHideOutputPaneNotIfReferences());
        m_checkBoxHideOutputPaneNotIfOutput->SetValue(options->GetHideOutputPaneNotIfOutput());
        m_checkBoxHideOutputPaneNotIfTrace->SetValue(options->GetHideOutputPaneNotIfTrace());
        m_checkBoxHideOutputPaneNotIfTasks->SetValue(options->GetHideOutputPaneNotIfTasks());
        m_checkBoxHideOutputPaneNotIfBuildQ->SetValue(options->GetHideOutputPaneNotIfBuildQ());
        m_checkBoxHideOutputPaneNotIfCppCheck->SetValue(options->GetHideOutputPaneNotIfCppCheck());
        m_checkBoxHideOutputPaneNotIfSvn->SetValue(options->GetHideOutputPaneNotIfSvn());
        m_checkBoxHideOutputPaneNotIfCscope->SetValue(options->GetHideOutputPaneNotIfCscope());
        m_checkBoxHideOutputPaneNotIfGit->SetValue(options->GetHideOutputPaneNotIfGit());
        m_checkBoxHideOutputPaneNotIfDebug->SetValue(options->GetHideOutputPaneNotIfDebug());
        m_checkBoxHideOutputPaneNotIfMemCheck->SetValue(options->GetHideOutputPaneNotIfMemCheck());
        m_checkBoxDontFoldSearchResults->SetValue(options->GetDontAutoFoldResults());
        m_checkBoxDontOverrideSearchStringWithSelection->SetValue(options->GetDontOverrideSearchStringWithSelection());
        m_checkBoxShowDebugOnRun->SetValue(options->GetShowDebugOnRun());
        m_radioBoxHint->SetSelection(options->GetDockingStyle());
        m_checkBoxHideCaptions->SetValue(!options->IsShowDockingWindowCaption());
        m_checkBoxEnsureCaptionsVisible->SetValue(options->IsEnsureCaptionsVisible());
        m_checkBoxShowXButton->SetValue(options->IsTabHasXButton());
        m_checkBoxNavBarSortDropdown->SetValue(options->IsSortNavBarDropdown());
        bool tempb;
        gsgs_Config().Read("UseCustomCaptionsColour", tempb, false);
        m_checkBoxCustomCaptionColour->SetValue(tempb);
        m_cpCaptionColour->SetColour(gsgs_Style().GetCaptionColour());

        m_choiceTabStyle->Append(TabView::GetRenderers());
        wxString selection;
        gsgs_Config().Read("TabStyle", selection, wxString("GTK3"));
        m_choiceTabStyle->SetStringSelection(selection);

        m_checkBoxEditorTabsFollowsTheme->SetValue(options->IsTabColourMatchesTheme());
        m_checkBoxMouseScrollSwitchTabs->SetValue(options->IsMouseScrollSwitchTabs());
        m_checkBoxSortTabsDropdownAlphabetically->SetValue(options->IsSortTabsDropdownAlphabetically());
        m_checkBoxEnableTabSwitchingKey->SetValue(options->IsCtrlTabEnabled());

        int sel(0);
        switch (options->GetNotebookTabHeight()) {
        case OptionsConfig::nbTabHt_Tiny:
            sel = 3;
            break;
        case OptionsConfig::nbTabHt_Short:
            sel = 2;
            break;
        case OptionsConfig::nbTabHt_Medium:
            sel = 1;
            break;
        default:
            sel = 0;
        }
        m_choiceTabHeight->SetSelection(sel);

#if USE_AUI_NOTEBOOK
        m_choiceOutputTabsOrientation->SetSelection(0);
        m_choiceWorkspaceTabsOrientation->SetSelection(2);
        m_choiceOutputTabsOrientation->Enable(false);
        m_choiceWorkspaceTabsOrientation->Enable(false);
#else
        switch (options->GetOutputTabsDirection()) {
        case wxTOP:
            m_choiceOutputTabsOrientation->SetSelection(0);
            break;
        case wxBOTTOM:
            m_choiceOutputTabsOrientation->SetSelection(1);
            break;
        default:
            break;
        }
#ifdef __WXOSX__
        switch (options->GetWorkspaceTabsDirection()) {
        case wxLEFT:
        case wxTOP:
            m_choiceWorkspaceTabsOrientation->SetSelection(2);
            break;
        case wxRIGHT:
        case wxBOTTOM:
            m_choiceWorkspaceTabsOrientation->SetSelection(3);
            break;
        default:
            break;
        }
#else
        switch (options->GetWorkspaceTabsDirection()) {
        case wxLEFT:
            m_choiceWorkspaceTabsOrientation->SetSelection(0);
            break;
        case wxRIGHT:
            m_choiceWorkspaceTabsOrientation->SetSelection(1);
            break;
        case wxTOP:
            m_choiceWorkspaceTabsOrientation->SetSelection(2);
            break;
        case wxBOTTOM:
            m_choiceWorkspaceTabsOrientation->SetSelection(3);
            break;
        default:
            break;
        }
#endif
#endif
        // Set the marker colour
        wxColour tempc;
        gsgs_Config().Read("ActiveTabMarkerColour", tempc, DefaultATMColour);
        m_colourPickerMarker->SetColour(tempc);
        m_checkBoxHideOutputPaneNotIfDebug->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
    }

    EditorSettingsDockingWindows::~EditorSettingsDockingWindows()
    {
        m_cpCaptionColour->Disconnect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnUseCustomCaptionColourUI),
            NULL, this);
        m_checkBoxEnsureCaptionsVisible->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnEnsureCaptionsVisibleUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfBuild->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfSearch->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfReplace->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfReferences->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfOutput->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfTrace->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfTasks->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfBuildQ->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfCppCheck->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfSvn->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfCscope->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfGit->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfDebug->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
        m_checkBoxHideOutputPaneNotIfMemCheck->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI), NULL,
            this);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsDockingWindows::Save(OptionsConfig * options)
    {
        gsgs_Config().Write("ActiveTabMarkerColour", m_colourPickerMarker->GetColour());
        gsgs_Config().Write("UseCustomCaptionsColour", m_checkBoxCustomCaptionColour->IsChecked());
        gsgs_Config().Write("CustomCaptionColour", m_cpCaptionColour->GetColour());

        options->SetHideOutpuPaneOnUserClick(m_checkBoxHideOutputPaneOnClick->IsChecked());
        options->SetHideOutputPaneNotIfBuild(m_checkBoxHideOutputPaneNotIfBuild->IsChecked());
        options->SetHideOutputPaneNotIfSearch(m_checkBoxHideOutputPaneNotIfSearch->IsChecked());
        options->SetHideOutputPaneNotIfReplace(m_checkBoxHideOutputPaneNotIfReplace->IsChecked());
        options->SetHideOutputPaneNotIfReferences(m_checkBoxHideOutputPaneNotIfReferences->IsChecked());
        options->SetHideOutputPaneNotIfOutput(m_checkBoxHideOutputPaneNotIfOutput->IsChecked());
        options->SetHideOutputPaneNotIfTrace(m_checkBoxHideOutputPaneNotIfTrace->IsChecked());
        options->SetHideOutputPaneNotIfTasks(m_checkBoxHideOutputPaneNotIfTasks->IsChecked());
        options->SetHideOutputPaneNotIfBuildQ(m_checkBoxHideOutputPaneNotIfBuildQ->IsChecked());
        options->SetHideOutputPaneNotIfCppCheck(m_checkBoxHideOutputPaneNotIfCppCheck->IsChecked());
        options->SetHideOutputPaneNotIfSvn(m_checkBoxHideOutputPaneNotIfSvn->IsChecked());
        options->SetHideOutputPaneNotIfCscope(m_checkBoxHideOutputPaneNotIfCscope->IsChecked());
        options->SetHideOutputPaneNotIfGit(m_checkBoxHideOutputPaneNotIfGit->IsChecked());
        options->SetHideOutputPaneNotIfDebug(m_checkBoxHideOutputPaneNotIfDebug->IsChecked());
        options->SetHideOutputPaneNotIfMemCheck(m_checkBoxHideOutputPaneNotIfMemCheck->IsChecked());
        options->SetDontAutoFoldResults(m_checkBoxDontFoldSearchResults->IsChecked());
        options->SetDontOverrideSearchStringWithSelection(m_checkBoxDontOverrideSearchStringWithSelection->IsChecked());
        options->SetShowDebugOnRun(m_checkBoxShowDebugOnRun->IsChecked());
        options->SetDockingStyle(m_radioBoxHint->GetSelection());
        options->SetShowDockingWindowCaption(!m_checkBoxHideCaptions->IsChecked());
        options->SetEnsureCaptionsVisible(m_checkBoxEnsureCaptionsVisible->IsChecked());
        options->SetCtrlTabEnabled(m_checkBoxEnableTabSwitchingKey->IsChecked());
        options->SetSortNavBarDropdown(m_checkBoxNavBarSortDropdown->IsChecked());
        options->SetTabColourMatchesTheme(m_checkBoxEditorTabsFollowsTheme->IsChecked());
        options->SetTabHasXButton(m_checkBoxShowXButton->IsChecked());
        options->SetMouseScrollSwitchTabs(m_checkBoxMouseScrollSwitchTabs->IsChecked());
        options->SetSortTabsDropdownAlphabetically(m_checkBoxSortTabsDropdownAlphabetically->IsChecked());

        int ht(0);
        switch (m_choiceTabHeight->GetSelection()) {
        case 3:
            ht = OptionsConfig::nbTabHt_Tiny;
            break;
        case 2:
            ht = OptionsConfig::nbTabHt_Short;
            break;
        case 1:
            ht = OptionsConfig::nbTabHt_Medium;
            break;
        default:
            ht = OptionsConfig::nbTabHt_Tall;
        }
        options->SetNotebookTabHeight(ht);
        wxString selection = m_choiceTabStyle->GetStringSelection();
        gsgs_Config().Write("TabStyle", selection);

        switch (m_choiceOutputTabsOrientation->GetSelection()) {
        case 0:
            options->SetOutputTabsDirection(wxTOP);
            break;
        case 1:
            options->SetOutputTabsDirection(wxBOTTOM);
            break;
        default:
            break;
        }
        switch (m_choiceWorkspaceTabsOrientation->GetSelection()) {
        case 0:
            options->SetWorkspaceTabsDirection(wxLEFT);
            break;
        case 1:
            options->SetWorkspaceTabsDirection(wxRIGHT);
            break;
        case 2:
            options->SetWorkspaceTabsDirection(wxTOP);
            break;
        case 3:
            options->SetWorkspaceTabsDirection(wxBOTTOM);
            break;
        default:
            break;
        }
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsDockingWindows::OnHideOutputPaneNotIfDebugUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_checkBoxHideOutputPaneOnClick->IsChecked());
    }
    //---------------------------------------------------------------------------------
    bool EditorSettingsDockingWindows::IsRestartRequired()
    {
        return false;
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsDockingWindows::OnEnsureCaptionsVisibleUI(wxUpdateUIEvent& event)
    {
#ifdef __WXMSW__
        event.Enable(m_checkBoxHideCaptions->IsChecked());
#else
        event.Enable(false);
        event.Check(false);
#endif
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsDockingWindows::OnUseCustomCaptionColourUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_checkBoxCustomCaptionColour->IsChecked());
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsTerminal
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsTerminal::EditorSettingsTerminal(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style) : 
        wxPanel(parent, id, pos, size, style),
        TreeBookNode()
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_staticText2 = gsgs_Style().createStaticText(this, wxID_ANY, _("Select the terminal to use:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        bSizer1->Add(m_staticText2, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_choiceTerminalsArr;
        m_choiceTerminals =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceTerminalsArr, 0);

        bSizer1->Add(m_choiceTerminals, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText2->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsTerminal::OnUseGSGSTerminalUI),
            NULL, this);

        wxArrayString terminals = ConsoleBase::GetAvailaleTerminals();
        m_choiceTerminals->Append(terminals);
        wxString selection = ConsoleBase::GetSelectedTerminalName();
        if (!selection.IsEmpty()) { m_choiceTerminals->SetStringSelection(selection); }
    }
    //--------------------------------------------------------------------------
    EditorSettingsTerminal::~EditorSettingsTerminal()
    {
        m_staticText2->Disconnect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(EditorSettingsTerminal::OnUseGSGSTerminalUI), NULL, this);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsTerminal::Save(OptionsConfig * options)
    {
        wxUnusedVar(options);
        gsgs_Config().Write("Terminal", m_choiceTerminals->GetStringSelection());
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsMiscPanel
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsMiscPanel::EditorSettingsMiscPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style), 
        TreeBookNode()
        , m_restartRequired(false)
    {
        wxWindowUpdateLocker templock(this);
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_notebook2 = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        m_notebook2->SetName(wxT("m_notebook2"));

        bSizer1->Add(m_notebook2, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_panel1 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel1, _("General"), true);

        wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
        m_panel1->SetSizer(bSizer2);

        wxFlexGridSizer* fgSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer4->SetFlexibleDirection(wxBOTH);
        fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer4->AddGrowableCol(1);

        bSizer2->Add(fgSizer4, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText8 = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Toolbar Icon Size:"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        fgSizer4->Add(m_staticText8, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_toolbarIconSizeArr;
        m_toolbarIconSizeArr.Add(wxT("Small Icons (16x16)"));
        m_toolbarIconSizeArr.Add(wxT("Large Icons (24x24)"));
        m_toolbarIconSize = new wxChoice(m_panel1, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), m_toolbarIconSizeArr, 0);
        m_toolbarIconSize->SetSelection(1);

        fgSizer4->Add(m_toolbarIconSize, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer98 = new wxBoxSizer(wxVERTICAL);

        bSizer2->Add(boxSizer98, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_singleAppInstance = new wxCheckBox(m_panel1, wxID_ANY, _("Allow only single instance running"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        m_singleAppInstance->SetValue(false);
        m_singleAppInstance->SetToolTip(_("There can be only one"));

        boxSizer98->Add(m_singleAppInstance, 0, wxALL, WXC_FROM_DIP(5));

        m_versionCheckOnStartup = new wxCheckBox(m_panel1, wxID_ANY, _("Check for new version on startup"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        m_versionCheckOnStartup->SetValue(true);
        m_versionCheckOnStartup->SetToolTip(_("When gsgs starts, it will connect to http://niiengine.com to check if a new version of gsgs was released"));

        boxSizer98->Add(m_versionCheckOnStartup, 0, wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer57 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer98->Add(boxSizer57, 1, wxEXPAND, WXC_FROM_DIP(5));

        boxSizer57->Add(15, -1, 0, wxALL, WXC_FROM_DIP(5));

        m_checkBoxPromptReleaseOnly = new wxCheckBox(m_panel1, wxID_ANY, _("Prompt for stable releases only"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        m_checkBoxPromptReleaseOnly->SetValue(false);
        m_checkBoxPromptReleaseOnly->SetToolTip(_("When checked, prompt me for new stable releases\nDon't prompt me for weekly builds"));

        boxSizer57->Add(m_checkBoxPromptReleaseOnly, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxRestoreSession = new wxCheckBox(m_panel1, wxID_ANY, _("Load last session on startup"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        m_checkBoxRestoreSession->SetValue(false);
        m_checkBoxRestoreSession->SetToolTip(_("When launched, gsgs will restore the last opened workspace + all open editors"));

        boxSizer98->Add(m_checkBoxRestoreSession, 0, wxALL, WXC_FROM_DIP(5));

        wxFlexGridSizer* flexGridSizer77 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer77->SetFlexibleDirection(wxBOTH);
        flexGridSizer77->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer77->AddGrowableCol(1);

        boxSizer98->Add(flexGridSizer77, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText79 = gsgs_Style().createStaticText(m_panel1, wxID_ANY, _("Web search prefix:"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        flexGridSizer77->Add(m_staticText79, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_webSearchPrefix = gsgs_Style().createTextCtrl(m_panel1, wxID_ANY, wxT("https://www.google.com/search?q="), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);

        flexGridSizer77->Add(m_webSearchPrefix, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_checkBoxDirect2D = new wxCheckBox(m_panel1, wxID_ANY, _("Windows Only: use Direct2D editor drawings"), wxDefaultPosition, wxDLG_UNIT(m_panel1, wxSize(-1, -1)), 0);
        m_checkBoxDirect2D->SetValue(true);

        boxSizer98->Add(m_checkBoxDirect2D, 0, wxALL, WXC_FROM_DIP(5));

        m_panel23 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel23, _("Frame Title"));

        wxBoxSizer* boxSizer25 = new wxBoxSizer(wxVERTICAL);
        m_panel23->SetSizer(boxSizer25);

        m_staticText31 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("You can use the following macros to construct your own frame title:"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        boxSizer25->Add(m_staticText31, 0, wxALL, WXC_FROM_DIP(5));

        wxFlexGridSizer* flexGridSizer29 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer29->SetFlexibleDirection(wxBOTH);
        flexGridSizer29->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer29->AddGrowableCol(1);

        boxSizer25->Add(flexGridSizer29, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText33 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("$user"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        wxFont m_staticText33Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText33Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText33->SetFont(m_staticText33Font);

        flexGridSizer29->Add(m_staticText33, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticText35 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("the current user name inside square brackets"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_staticText35, 0, wxALL, WXC_FROM_DIP(5));

        m_staticText37 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("$filename"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        wxFont m_staticText37Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText37Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText37->SetFont(m_staticText37Font);

        flexGridSizer29->Add(m_staticText37, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticText39 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("the current file name"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_staticText39, 0, wxALL, WXC_FROM_DIP(5));

        m_staticText41 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("$fullpath"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        wxFont m_staticText41Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText41Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText41->SetFont(m_staticText41Font);

        flexGridSizer29->Add(m_staticText41, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticText43 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("the current file fullpath"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_staticText43, 0, wxALL, WXC_FROM_DIP(5));

        m_staticText45 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("$workspace"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        wxFont m_staticText45Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        m_staticText45Font.SetWeight(wxFONTWEIGHT_BOLD);
        m_staticText45->SetFont(m_staticText45Font);

        flexGridSizer29->Add(m_staticText45, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticText47 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("the current workspace name inside square brackets"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_staticText47, 0, wxALL, WXC_FROM_DIP(5));

        m_staticText49 = gsgs_Style().createStaticText(m_panel23, wxID_ANY, _("Frame title:"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_staticText49, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlPattern = gsgs_Style().createTextCtrl(m_panel23, wxID_ANY, _("$workspace $fullpath"), wxDefaultPosition, wxDLG_UNIT(m_panel23, wxSize(-1, -1)), 0);
        flexGridSizer29->Add(m_textCtrlPattern, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panel44 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel44, _("Status Bar"));

        wxBoxSizer* boxSizer115 = new wxBoxSizer(wxVERTICAL);
        m_panel44->SetSizer(boxSizer115);

        m_staticText70 = gsgs_Style().createStaticText(m_panel44, wxID_ANY, _("What statistics to show in the status bar about an editor's text:"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        boxSizer115->Add(m_staticText70, 0, wxALL, WXC_FROM_DIP(5));

        m_statusbarShowLine = new wxCheckBox(m_panel44, wxID_ANY, _("Show current line"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        m_statusbarShowLine->SetValue(true);

        boxSizer115->Add(m_statusbarShowLine, 0, wxALL, WXC_FROM_DIP(5));

        m_statusbarShowCol = new wxCheckBox(m_panel44, wxID_ANY, _("Show current column"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        m_statusbarShowCol->SetValue(true);

        boxSizer115->Add(m_statusbarShowCol, 0, wxALL, WXC_FROM_DIP(5));

        m_statusbarShowPos = new wxCheckBox(m_panel44, wxID_ANY, _("Show current position within the file"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        m_statusbarShowPos->SetValue(false);
        m_statusbarShowPos->SetToolTip(_("e.g. character 1234 in the file"));

        boxSizer115->Add(m_statusbarShowPos, 0, wxALL, WXC_FROM_DIP(5));

        m_statusbarShowFileLength = new wxCheckBox(m_panel44, wxID_ANY, _("Show total length of the file"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        m_statusbarShowFileLength->SetValue(false);

        boxSizer115->Add(m_statusbarShowFileLength, 0, wxALL, WXC_FROM_DIP(5));

        m_statusBarShowSelChars = new wxCheckBox(m_panel44, wxID_ANY, _("Show number of selected chars"), wxDefaultPosition, wxDLG_UNIT(m_panel44, wxSize(-1, -1)), 0);
        m_statusBarShowSelChars->SetValue(false);
        m_statusBarShowSelChars->SetToolTip(_("Show the number of selected chars in the editor"));

        boxSizer115->Add(m_statusBarShowSelChars, 0, wxALL, WXC_FROM_DIP(5));

        m_panelTBar = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panelTBar, _("Tool Bar"));

        wxBoxSizer* boxSizer1152 = new wxBoxSizer(wxVERTICAL);
        m_panelTBar->SetSizer(boxSizer1152);

        wxBoxSizer* boxSizer95 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer1152->Add(boxSizer95, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText94 = gsgs_Style().createStaticText(m_panelTBar, wxID_ANY, _("Separate adjacent tool groups by:"), wxDefaultPosition, wxDLG_UNIT(m_panelTBar, wxSize(-1, -1)), 0);
        boxSizer95->Add(m_staticText94, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlSeparation = gsgs_Style().createTextCtrl(m_panelTBar, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panelTBar, wxSize(-1, -1)), 0);
        m_textCtrlSeparation->SetToolTip(_("The tools on the toolbar are grouped by their function e.g. Edit, Build. By default those groups are separated by little more than a vertical line. Here you can adjust the amount of separation."));

        boxSizer95->Add(m_textCtrlSeparation, 1, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_staticText97 = gsgs_Style().createStaticText(m_panelTBar, wxID_ANY, _("pixels"), wxDefaultPosition, wxDLG_UNIT(m_panelTBar, wxSize(-1, -1)), 0);
        boxSizer95->Add(m_staticText97, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_panel2 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel2, _("Encoding & Locale"));

        wxBoxSizer* bSizer4 = new wxBoxSizer(wxVERTICAL);
        m_panel2->SetSizer(bSizer4);

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(1);

        bSizer4->Add(fgSizer1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText1 = gsgs_Style().createStaticText(m_panel2, wxID_ANY, _("File font encoding:"), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);
        fgSizer1->Add(m_staticText1, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_fileEncodingArr;
        m_fileEncoding = new wxChoice(m_panel2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), m_fileEncodingArr, 0);

        fgSizer1->Add(m_fileEncoding, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        fgSizer1->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_SetLocale = new wxCheckBox(m_panel2, wxID_ANY, _("Enable localization"), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);
        m_SetLocale->SetValue(true);
        m_SetLocale->SetToolTip(_("View gsgs's strings translated into a different language, if available.  This will also make gsgs use other aspects of the locale."));

        fgSizer1->Add(m_SetLocale, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_staticTextLocale = gsgs_Style().createStaticText(m_panel2, wxID_ANY, _("Locale to use:"), wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), 0);
        fgSizer1->Add(m_staticTextLocale, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_AvailableLocalesArr;
        m_AvailableLocales = new wxChoice(m_panel2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel2, wxSize(-1, -1)), m_AvailableLocalesArr, 0);
        m_AvailableLocales->SetToolTip(_("These are the locales that are available on your system. There won't necessarily be gsgs translations for all of them."));

        fgSizer1->Add(m_AvailableLocales, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_panel3 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel3, _("History"));

        wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer2->SetFlexibleDirection(wxBOTH);
        fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer2->AddGrowableCol(1);
        m_panel3->SetSizer(fgSizer2);

        m_staticText2 = gsgs_Style().createStaticText(m_panel3, wxID_ANY, _("Clear recent workspace / files history"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_clearButton = new Button(m_panel3, wxID_CLEAR, _("&Clear"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        m_clearButton->SetDefault();

        fgSizer2->Add(m_clearButton, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(5));

        m_staticText3 = gsgs_Style().createStaticText(m_panel3, wxID_ANY, _("Max items kept in find / replace dialog:"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_maxItemsFindReplace = gsgs_Style().createTextCtrl(m_panel3, wxID_ANY, wxT("15"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_maxItemsFindReplace, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(5));

        m_staticText4 = gsgs_Style().createStaticText(m_panel3, wxID_ANY, _("Maximum number of tabs opened in the editor:"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_spinCtrlMaxOpenTabs = gsgs_Style().createTextCtrl(m_panel3, wxID_ANY, wxT("10"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_spinCtrlMaxOpenTabs, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(5));

        m_staticText53 = gsgs_Style().createStaticText(m_panel3, wxID_ANY, _("Reset annoying dialogs answers:"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText53, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_buttonAnnoyingDialogs = new Button(m_panel3, wxID_ANY, _("&Reset"), wxDefaultPosition, wxDLG_UNIT(m_panel3, wxSize(-1, -1)), 0);

        fgSizer2->Add(m_buttonAnnoyingDialogs, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_panel4 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panel4, _("Log"));

        wxBoxSizer* boxSizer11 = new wxBoxSizer(wxVERTICAL);
        m_panel4->SetSizer(boxSizer11);

        wxFlexGridSizer* flexGridSizer13 = new wxFlexGridSizer(0, 3, 0, 0);
        flexGridSizer13->SetFlexibleDirection(wxBOTH);
        flexGridSizer13->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer13->AddGrowableCol(1);
        flexGridSizer13->AddGrowableCol(2);

        boxSizer11->Add(flexGridSizer13, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText6 = gsgs_Style().createStaticText(m_panel4, wxID_ANY, _("gsgs's Log-file verbosity:"), wxDefaultPosition, wxDLG_UNIT(m_panel4, wxSize(-1, -1)), 0);
        flexGridSizer13->Add(m_staticText6, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choice4Arr;
        m_choice4Arr.Add(wxT("Error"));
        m_choice4Arr.Add(wxT("Warning"));
        m_choice4Arr.Add(wxT("Debug"));
        m_choice4Arr.Add(wxT("Developer"));
        m_choice4 = new wxChoice(m_panel4, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel4, wxSize(-1, -1)), m_choice4Arr, 0);
        m_choice4->SetToolTip(_("gsgs logs to file various events, this option controls the logging verbosity"));
        m_choice4->SetSelection(0);

        flexGridSizer13->Add(m_choice4, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_buttonOpenLog = new Button(m_panel4, wxID_ANY, _("Open.."), wxDefaultPosition, wxDLG_UNIT(m_panel4, wxSize(-1, -1)), 0);
        m_buttonOpenLog->SetToolTip(_("Open the log file into an editor"));

        flexGridSizer13->Add(m_buttonOpenLog, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_redirectLogOutput = new wxCheckBox(m_panel4, wxID_ANY, _("GTK only: Redirect stdout/stderr output to a log file"), wxDefaultPosition, wxDLG_UNIT(m_panel4, wxSize(-1, -1)), 0);
        m_redirectLogOutput->SetValue(true);
        m_redirectLogOutput->SetToolTip(_("If ticked, all output from e.g. cout or wxLogDebug will be redirected to the file .gsgs/gsgs-stdout-stderr.log"));

        boxSizer11->Add(m_redirectLogOutput, 0, wxALL, WXC_FROM_DIP(10));

        mIndentationPanel = new EditorOptionsGeneralIndentationPanel(m_notebook2);
        m_notebook2->AddPage(mIndentationPanel, _("Indentation"));
        mRightMarginPanel = new EditorOptionsGeneralRightMarginPanel(m_notebook2);
        m_notebook2->AddPage(mRightMarginPanel, _("Right Margin Indicator"));
        mCaretPanel = new EditorSettingsCaret(m_notebook2);
        m_notebook2->AddPage(mCaretPanel, _("Caret & Scrolling"));
        mGeneralSavePanel = new EditorOptionsGeneralSavePanel(m_notebook2);
        m_notebook2->AddPage(mGeneralSavePanel, _("Save Options"));
        mComments = new EditorSettingsComments(m_notebook2);
        m_notebook2->AddPage(mComments, _("Code"));
        mTerminal = new EditorSettingsTerminal(m_notebook2);
        m_notebook2->AddPage(mTerminal, _("Terminal"));

        // Connect events
        m_checkBoxPromptReleaseOnly->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnPromptStableReleaseUI), NULL, this);
        m_checkBoxDirect2D->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnUseDirect2DUI), NULL, this);
        m_SetLocale->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleChkUpdateUI), NULL, this);
        m_staticTextLocale->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleStaticUpdateUI), NULL, this);
        m_AvailableLocales->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleChoiceUpdateUI), NULL, this);
        m_clearButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnClearButtonClick), NULL, this);
        m_clearButton->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnClearUI), NULL, this);
        m_buttonAnnoyingDialogs->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnResetAnnoyingDialogsAnswers), NULL, this);
        m_choice4->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(EditorSettingsMiscPanel::OnLogVerbosityChanged), NULL, this);
        m_buttonOpenLog->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnShowLogFile), NULL, this);
        m_redirectLogOutput->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnLogoutputCheckUpdateUI), NULL, this);

        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        if (options->GetIconsSize() == 16) {
            m_toolbarIconSize->SetSelection(0);
        }
        else {
            m_toolbarIconSize->SetSelection(1);
        }

        m_oldSetLocale = options->GetUseLocale();
        m_SetLocale->SetValue(m_oldSetLocale);
        m_oldpreferredLocale = options->GetPreferredLocale();
        // Load the available locales and feed them to the wxchoice
        int select = FindAvailableLocales();
        if (select != wxNOT_FOUND) { m_AvailableLocales->SetSelection(select); }

        wxArrayString astrEncodings;
        wxFontEncoding fontEnc;
        int iCurrSelId = 0;
        size_t iEncCnt = wxFontMapper::GetSupportedEncodingsCount();
        for (size_t i = 0; i < iEncCnt; i++) {
            fontEnc = wxFontMapper::GetEncoding(i);
            if (wxFONTENCODING_SYSTEM == fontEnc) { // skip system, it is changed to UTF-8 in optionsconfig
                continue;
            }
            astrEncodings.Add(wxFontMapper::GetEncodingName(fontEnc));
            if (fontEnc == options->GetFileFontEncoding()) { iCurrSelId = i; }
        }

        m_fileEncoding->Append(astrEncodings);
        m_fileEncoding->SetSelection(iCurrSelId);

        bool temp;
        wxString tempstr;
        int tempi;
        gsgs_Config().Read(kConfigSingleInstance, temp, false);
        m_singleAppInstance->SetValue(temp);
        gsgs_Config().Read(kConfigCheckForNewVersion, temp, true);
        m_versionCheckOnStartup->SetValue(temp);
        gsgs_Config().Read(kConfigMaxItemsInFindReplaceDialog, tempi, 15);
        m_maxItemsFindReplace->ChangeValue(StringUtils::wxIntToString(tempi));
        gsgs_Config().Read(kConfigMaxOpenedTabs, tempi, 15);
        m_spinCtrlMaxOpenTabs->ChangeValue(StringUtils::wxIntToString(tempi));
        gsgs_Config().Read(kConfigLogVerbosity, tempi, FileLogger::Error);
        m_choice4->SetStringSelection(FileLogger::GetVerbosityAsString(tempi));
        gsgs_Config().Read(kConfigRestoreLastSession, temp, true);
        m_checkBoxRestoreSession->SetValue(temp);
        gsgs_Config().Read(kConfigFrameTitlePattern, tempstr, wxString("$workspace $fullpath"));
        m_textCtrlPattern->ChangeValue(tempstr);
        gsgs_Config().Read(kConfigStatusbarShowLine, temp, true);
        m_statusbarShowLine->SetValue(temp);
        gsgs_Config().Read(kConfigStatusbarShowColumn, temp, true);
        m_statusbarShowCol->SetValue(temp);
        gsgs_Config().Read(kConfigStatusbarShowPosition, temp, false);
        m_statusbarShowPos->SetValue(temp);
        gsgs_Config().Read(kConfigStatusbarShowLength, temp, false);
        m_statusbarShowFileLength->SetValue(temp);
        gsgs_Config().Read(kConfigStatusbarShowSelectedChars, temp, true);
        m_statusBarShowSelChars->SetValue(temp);
        gsgs_Config().Read(kConfigToolbarGroupSpacing, tempi, 30);
        m_textCtrlSeparation->ChangeValue(StringUtils::wxIntToString(tempi));
        gsgs_Config().Read(kConfigRedirectLogOutput, temp, true);
        m_redirectLogOutput->SetValue(temp);
        gsgs_Config().Read("PromptForNewReleaseOnly", temp, false);
        m_checkBoxPromptReleaseOnly->SetValue(temp);

        GetWebSearchPrefix()->ChangeValue(options->GetWebSearchPrefix());
        gsgs_Config().Read("Editor/UseDirect2D", temp, true);
        m_checkBoxDirect2D->SetValue(temp);
    }
    //--------------------------------------------------------------------------
    EditorSettingsMiscPanel::~EditorSettingsMiscPanel()
    {
        m_checkBoxPromptReleaseOnly->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnPromptStableReleaseUI), NULL, this);
        m_checkBoxDirect2D->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnUseDirect2DUI), NULL, this);
        m_SetLocale->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleChkUpdateUI), NULL, this);
        m_staticTextLocale->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleStaticUpdateUI), NULL, this);
        m_AvailableLocales->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::LocaleChoiceUpdateUI), NULL, this);
        m_clearButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnClearButtonClick), NULL, this);
        m_clearButton->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnClearUI), NULL, this);
        m_buttonAnnoyingDialogs->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnResetAnnoyingDialogsAnswers), NULL, this);
        m_choice4->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(EditorSettingsMiscPanel::OnLogVerbosityChanged), NULL, this);
        m_buttonOpenLog->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsMiscPanel::OnShowLogFile), NULL, this);
        m_redirectLogOutput->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsMiscPanel::OnLogoutputCheckUpdateUI), NULL, this);

    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnClearButtonClick(wxCommandEvent&)
    {
        gsgs_Workspace().RemoveAllRecently();
        gsgs_Frame()->GetMainBook()->ClearFileHistory();
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::Save(OptionsConfig * options)
    {
        gsgs_Config().Write(kConfigSingleInstance, m_singleAppInstance->IsChecked());
        gsgs_Config().Write(kConfigCheckForNewVersion, m_versionCheckOnStartup->IsChecked());
        gsgs_Config().Write(kConfigMaxItemsInFindReplaceDialog, StringUtils::wxStringToInt(m_maxItemsFindReplace->GetValue(), 15));
        gsgs_Config().Write(kConfigMaxOpenedTabs, StringUtils::wxStringToInt(m_spinCtrlMaxOpenTabs->GetValue(), 15));
        gsgs_Config().Write(kConfigRestoreLastSession, m_checkBoxRestoreSession->IsChecked());
        gsgs_Config().Write(kConfigFrameTitlePattern, m_textCtrlPattern->GetValue());
        gsgs_Config().Write(kConfigStatusbarShowLine, m_statusbarShowLine->IsChecked());
        gsgs_Config().Write(kConfigStatusbarShowColumn, m_statusbarShowCol->IsChecked());
        gsgs_Config().Write(kConfigStatusbarShowPosition, m_statusbarShowPos->IsChecked());
        gsgs_Config().Write(kConfigStatusbarShowLength, m_statusbarShowFileLength->IsChecked());
        gsgs_Config().Write(kConfigStatusbarShowSelectedChars, m_statusBarShowSelChars->IsChecked());
        gsgs_Config().Write(kConfigToolbarGroupSpacing, StringUtils::wxStringToInt(m_textCtrlSeparation->GetValue(), 30));

        // check to see of the icon size was modified
        int oldIconSize(24);

        OptionsConfig * oldOptions = gsgs_EditorConfig().GetOptions();
        if (oldOptions) { oldIconSize = oldOptions->GetIconsSize(); }

        int iconSize(24);
        if (m_toolbarIconSize->GetSelection() == 0) { iconSize = 16; }
        options->SetIconsSize(iconSize);

        bool setlocale = m_SetLocale->IsChecked();
        options->SetUseLocale(setlocale);
        wxString newLocaleString = m_AvailableLocales->GetStringSelection();
        // I don't think we should check if newLocaleString is empty; that's still useful information
        newLocaleString = newLocaleString.BeforeFirst(wxT(':')); // Store it as "fr_FR", not "fr_FR: French"
        options->SetPreferredLocale(newLocaleString);
        if ((setlocale != m_oldSetLocale) || (newLocaleString != m_oldpreferredLocale)) { m_restartRequired = true; }

        // save file font encoding
        options->SetFileFontEncoding(m_fileEncoding->GetStringSelection());
        TagsManagerST::Get()->SetEncoding(options->GetFileFontEncoding());

        if (oldIconSize != iconSize) {
            gsgs_EditorConfig().SetInteger(wxT("LoadSavedPrespective"), 0);
            // notify the user
            m_restartRequired = true;
        }
        else {
            gsgs_EditorConfig().SetInteger(wxT("LoadSavedPrespective"), 1);
        }

        size_t flags = options->GetOptions();

        // Clear old settings
        flags &= ~(OptionsConfig::Opt_IconSet_Classic);
        flags &= ~(OptionsConfig::Opt_IconSet_FreshFarm);
        flags &= ~(OptionsConfig::Opt_IconSet_Classic_Dark);
        flags |= OptionsConfig::Opt_IconSet_Classic;

        gsgs_Config().Write("RedirectLogOutput", m_redirectLogOutput->IsChecked());
        gsgs_Config().Write("PromptForNewReleaseOnly", m_checkBoxPromptReleaseOnly->IsChecked());
        options->SetOptions(flags);
        options->SetWebSearchPrefix(GetWebSearchPrefix()->GetValue());
        bool useDirect2D_Old;
        gsgs_Config().Read("Editor/UseDirect2D", useDirect2D_Old, true);
        gsgs_Config().Write("Editor/UseDirect2D", m_checkBoxDirect2D->IsChecked());
        m_restartRequired = (useDirect2D_Old != m_checkBoxDirect2D->IsChecked());
        mIndentationPanel->Save(options);
        mRightMarginPanel->Save(options);
        mCaretPanel->Save(options);
        mGeneralSavePanel->Save(options);
        mComments->Save(options);
        mTerminal->Save(options);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnClearUI(wxUpdateUIEvent& e)
    {
        wxArrayString a1; gsgs_Config().GetRecentOpen(a1);
        wxArrayString a2; gsgs_Config().GetRecentOpen(a2);
        e.Enable(!a1.IsEmpty() && !a2.IsEmpty());
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnEnableThemeUI(wxUpdateUIEvent& event) { event.Enable(false); }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::LocaleChkUpdateUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_AvailableLocales->GetCount() > 0);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::LocaleChoiceUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_SetLocale->IsChecked()); }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::LocaleStaticUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_SetLocale->IsChecked()); }
    //---------------------------------------------------------------------------------
    int EditorSettingsMiscPanel::FindAvailableLocales()
    {
        wxArrayString canonicalNames;
        int select(wxNOT_FOUND), sysdefault_sel(wxNOT_FOUND);
        m_AvailableLocales->Clear();

        int system_lang = wxLocale::GetSystemLanguage();
        if (system_lang == wxLANGUAGE_UNKNOWN) {
            // Least-stupid fallback value
            system_lang = wxLANGUAGE_ENGLISH_US;
        }

        for (int n = 0, lang = wxLANGUAGE_UNKNOWN + 1; lang < wxLANGUAGE_USER_DEFINED; ++lang) {
            const wxLanguageInfo* info = wxLocale::GetLanguageInfo(lang);
            // Check there *is* a Canonical name, as empty strings return a valid locale :/
            if ((info && !info->CanonicalName.IsEmpty()) && wxLocale::IsAvailable(lang)) {

                // Check we haven't already seen this item: we may find the system default twice
                if (canonicalNames.Index(info->CanonicalName) == wxNOT_FOUND) {
                    // Display the name as e.g. "en_GB: English (U.K.)"
                    m_AvailableLocales->Append(info->CanonicalName + wxT(": ") + info->Description);
                    canonicalNames.Add(info->CanonicalName);

                    if (info->CanonicalName == m_oldpreferredLocale) {
                        // Use this as the selection in the wxChoice
                        select = n;
                    }
                    if (lang == system_lang) {
                        // Use this as the selection if m_oldpreferredLocale isn't found
                        sysdefault_sel = n;
                    }
                    ++n;
                }
            }
        }

        return (select != wxNOT_FOUND) ? select : sysdefault_sel;
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnLogVerbosityChanged(wxCommandEvent& event)
    {
        FileLogger::SetVerbosity(event.GetString());
        gsgs_Config().Write("LogVerbosity", FileLogger::GetVerbosityAsNumber(m_choice4->GetStringSelection()));
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnShowLogFile(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxString logfile;
        logfile << gsgs_Path().GetUserDataDir() << gsgsPathSep << wxT("gsgs.log");

        gsgs_Frame()->GetMainBook()->OpenFile(logfile);
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnLogoutputCheckUpdateUI(wxUpdateUIEvent& event)
    {
#ifdef __WXGTK__
        event.Enable(true);
#else
        m_redirectLogOutput->SetValue(false);
        event.Enable(false);
#endif
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnResetAnnoyingDialogsAnswers(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        gsgs_Config().ClearAnnoyingDlgAnswers();
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnPromptStableReleaseUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_versionCheckOnStartup->IsChecked());
    }
    //---------------------------------------------------------------------------------
    void EditorSettingsMiscPanel::OnUseDirect2DUI(wxUpdateUIEvent& event)
    {
#ifdef __WXMSW__
        event.Enable(true);
#else
        event.Enable(false);
        event.Check(false);
#endif
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // PreferencesDialog
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    PreferencesDialog::PreferencesDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
        , restartRquired(false)
    {
        wxWindowUpdateLocker templock(this);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(mainSizer);

        wxBoxSizer* innerSizer = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(innerSizer, 1, wxEXPAND, WXC_FROM_DIP(5));

        //m_treeBook = new wxTreebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxBK_DEFAULT);
        //m_treeBook->SetFocus();
        //m_treeBook->SetName(wxT("m_treeBook"));
        m_treeBook = new Treebook(this, wxID_ANY, wxDefaultPosition);

        innerSizer->Add(m_treeBook, 1, wxALL | wxEXPAND, 0);

        wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

        mainSizer->Add(btnSizer, 0, wxALL | wxALIGN_CENTER, WXC_FROM_DIP(10));

        m_okButton = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_okButton->SetDefault();

        btnSizer->Add(m_okButton, 0, wxALL, WXC_FROM_DIP(5));

        m_cancelButton = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        btnSizer->Add(m_cancelButton, 0, wxALL, WXC_FROM_DIP(5));

        m_applyButton = new Button(this, wxID_APPLY, _("Apply"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        btnSizer->Add(m_applyButton, 0, wxALL, WXC_FROM_DIP(5));

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        this->Connect(wxEVT_ACTIVATE, wxActivateEventHandler(PreferencesDialog::OnActivate), NULL, this);
        this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(PreferencesDialog::OnInitDialog), NULL, this);
        m_okButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::OnButtonOK), NULL, this);
        m_applyButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::OnButtonApply), NULL, this);
        
        Initialize();
        //MSWSetNativeTheme(m_treeBook->GetTreeCtrl());
    }
    //--------------------------------------------------------------------------
    PreferencesDialog::~PreferencesDialog()
    {
        this->Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(PreferencesDialog::OnActivate), NULL, this);
        this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(PreferencesDialog::OnInitDialog), NULL, this);
        m_okButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::OnButtonOK), NULL, this);
        m_applyButton->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PreferencesDialog::OnButtonApply), NULL, this);
    }
    //--------------------------------------------------------------------------
    void PreferencesDialog::OnButtonOK(wxCommandEvent& event)
    {
        DoSave();
        EndModal(wxID_OK);
    }
    //--------------------------------------------------------------------------
    void PreferencesDialog::OnButtonApply(wxCommandEvent&) 
    { 
        DoSave();

    }
    //--------------------------------------------------------------------------
    void PreferencesDialog::DoSave()
    {
        // construct an OptionsConfig object and update the configuration
        OptionsConfig * options = new OptionsConfig(NULL);

        // for performance reasons, we start a transaction for the configuration
        // file
        gsgs_EditorConfig().Begin();

        TreeBookNodeList::iterator it, iend = mBookList.end();
        for (it = mBookList.begin(); it != iend; ++it) {
            if (*it) {
                TreeBookNode* child = *it;
                child->Save(options);

                if (!this->restartRquired) {
                    this->restartRquired = child->IsRestartRequired();
                }
            }
        }

        gsgs_EditorConfig().SetOptions(options);

        // save the modifications to the disk
        gsgs_EditorConfig().Save();

        // Notify plugins about settings changed
        PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
    }
    //--------------------------------------------------------------------------
    void PreferencesDialog::DoApply()
    {
        TreeBookNodeList::iterator it, iend = mBookList.end();
        for (it = mBookList.begin(); it != iend; ++it) 
        {
            if (*it) 
            {
                TreeBookNode* child = *it;
                child->Apply();

                if (!this->restartRquired) {
                    this->restartRquired = child->IsRestartRequired();
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void PreferencesDialog::Initialize()
    {
        wxWindowUpdateLocker winlock(m_treeBook);
        AddPage(new EditorOptionsGeneralGuidesPanel(m_treeBook), _("Edit"), true);
        AddPage(new EditorSettingsCommentsDoxygenPanel(m_treeBook), _("Documentation"));
        AddPage(new EditorSettingsFolding(m_treeBook), _("Folding"));
        AddPage(new EditorSettingsBookmarksPanel(m_treeBook), _("Bookmarks"));
        AddPage(new EditorSettingsDockingWindows(m_treeBook), _("Windows & Tabs"));

        // the Terminal page should NOT be added under Windows
        AddPage(new EditorSettingsMiscPanel(m_treeBook), _("Misc"));
        AddPage(new EditorSettingsSyntaxHighlight(m_treeBook), _("Colours and Fonts"));
        AddPage(new EditorSettingsAccelTable(m_treeBook), _("Keyboard Shortcuts"));
        SetMinSize(wxSize(300, 200));
    }
    //--------------------------------------------------------------------------
    static bool bBitmapLoaded = false;
    //-------------------------------------------------------------------------
    #define CXX_AND_JAVASCRIPT "c++"
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsSyntaxHighlight
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsSyntaxHighlight::EditorSettingsSyntaxHighlight(wxWindow* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, long style):
        wxPanel(parent, id, pos, size, style),
        TreeBookNode()
        , m_isModified(false)
        , m_globalThemeChanged(false)
        , m_globalBgColourChanged(false)
    {
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCrafterqEa5vvInitBitmapResources();
            bBitmapLoaded = true;
        }
        // Set icon(s) to the application/dialog
        /*wxIconBundle app_icons;
        {
            wxBitmap iconBmp = wxXmlResource::Get()->LoadBitmap(wxT("fonts-and-colours"));
            wxIcon icn;
            icn.CopyFromBitmap(iconBmp);
            app_icons.AddIcon(icn);
        }
        SetIcons(app_icons);*/

        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        m_panel171 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        bSizer1->Add(m_panel171, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer173 = new wxBoxSizer(wxVERTICAL);
        m_panel171->SetSizer(boxSizer173);

        m_toolbar = new ToolBar(m_panel171, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel171, wxSize(-1, -1)),
            wxTB_NODIVIDER | wxTB_FLAT);
        m_toolbar->SetToolBitmapSize(wxSize(16, 16));

        boxSizer173->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(5));
        
        m_notebook = new Notebook(m_panel171, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        m_notebook->SetName(wxT("m_notebook"));

        boxSizer173->Add(m_notebook, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panelGlobalColours = new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_panelGlobalColours->SetToolTip(_("Set global colours and fonts"));
        m_notebook->AddPage(m_panelGlobalColours, _("General"), true);

        wxBoxSizer* boxSizer82 = new wxBoxSizer(wxVERTICAL);
        m_panelGlobalColours->SetSizer(boxSizer82);

        wxStaticBoxSizer* staticBoxSizer179 =
            new wxStaticBoxSizer(new wxStaticBox(m_panelGlobalColours, wxID_ANY, _("IDE Theme")), wxVERTICAL);

        boxSizer82->Add(staticBoxSizer179, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* flexGridSizer181 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer181->SetFlexibleDirection(wxBOTH);
        flexGridSizer181->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer181->AddGrowableCol(1);

        staticBoxSizer179->Add(flexGridSizer181, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_cbUseCustomBaseColour = new wxCheckBox(m_panelGlobalColours, wxID_ANY, _("Use custom base colour:"),
            wxDefaultPosition, wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), 0);
        m_cbUseCustomBaseColour->SetValue(false);

        flexGridSizer181->Add(m_cbUseCustomBaseColour, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_colourPickerBaseColour =
            new wxColourPickerCtrl(m_panelGlobalColours, wxID_ANY, *wxBLACK, wxDefaultPosition,
                wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), wxCLRP_DEFAULT_STYLE);
        m_colourPickerBaseColour->SetToolTip(
            _("By default, gsgs uses the current desktop theme to build the colour for the various\ncontrols. You can "
                "override this by choosing a different colour here"));

        flexGridSizer181->Add(m_colourPickerBaseColour, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxStaticBoxSizer* staticBoxSizer182 =
            new wxStaticBoxSizer(new wxStaticBox(m_panelGlobalColours, wxID_ANY, _("Syntax Highlight")), wxVERTICAL);

        boxSizer82->Add(staticBoxSizer182, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* fgSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer4->SetFlexibleDirection(wxBOTH);
        fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer4->AddGrowableCol(1);

        staticBoxSizer182->Add(fgSizer4, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText159 = gsgs_Style().createStaticText(m_panelGlobalColours, wxID_ANY, _("Global font:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), 0);
        fgSizer4->Add(m_staticText159, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_fontPickerGlobal = new wxFontPickerCtrl(m_panelGlobalColours, wxID_ANY, wxNullFont, wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), wxFNTP_DEFAULT_STYLE);
        m_fontPickerGlobal->SetToolTip(_("Set a global font for all the  supported languages"));

        fgSizer4->Add(m_fontPickerGlobal, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText155 = gsgs_Style().createStaticText(m_panelGlobalColours, wxID_ANY, _("Global theme:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), 0);
        fgSizer4->Add(m_staticText155, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceGlobalThemeArr;
        m_choiceGlobalTheme = new wxChoice(m_panelGlobalColours, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, -1)), m_choiceGlobalThemeArr, 0);
        m_choiceGlobalTheme->SetToolTip(
            _("Set a global theme for all the supported languages.\nIf the theme is not available for a given language, "
                "gsgs will use the next available theme from\nthe same family"));

        fgSizer4->Add(m_choiceGlobalTheme, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_stcPreview = new wxStyledTextCtrl(m_panelGlobalColours, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalColours, wxSize(-1, 150)), 0);
        // Configure the fold margin
        m_stcPreview->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_stcPreview->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_stcPreview->SetMarginSensitive(4, true);
        m_stcPreview->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_stcPreview->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_stcPreview->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_stcPreview->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_stcPreview->SetMarginWidth(2, 0);
        m_stcPreview->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_stcPreview->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_stcPreview->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_stcPreview->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_stcPreview->SetMarginMask(3, 0);
        m_stcPreview->SetMarginWidth(3, 0);
        // Select the lexer
        m_stcPreview->SetLexer(wxSTC_LEX_NULL);
        // Set default font / styles
        m_stcPreview->StyleClearAll();
        m_stcPreview->SetWrapMode(0);
        m_stcPreview->SetIndentationGuides(0);
        m_stcPreview->SetKeyWords(0, wxT(""));
        m_stcPreview->SetKeyWords(1, wxT(""));
        m_stcPreview->SetKeyWords(2, wxT(""));
        m_stcPreview->SetKeyWords(3, wxT(""));
        m_stcPreview->SetKeyWords(4, wxT(""));

        boxSizer82->Add(m_stcPreview, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));
        m_stcPreview->SetMinSize(wxSize(-1, 150));

        m_panelSyntaxHighlight =
            new wxPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_panelSyntaxHighlight->SetToolTip(_("Customize your colours and font per language"));
        m_notebook->AddPage(m_panelSyntaxHighlight, _("Customize"), false);

        wxBoxSizer* boxSizer21 = new wxBoxSizer(wxHORIZONTAL);
        m_panelSyntaxHighlight->SetSizer(boxSizer21);

        wxArrayString m_listBoxArr;
        m_listBox = new wxListBox(m_panelSyntaxHighlight, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelSyntaxHighlight, wxSize(-1, -1)), m_listBoxArr, wxLB_SORT | wxLB_SINGLE);

        boxSizer21->Add(m_listBox, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panel25 = new wxPanel(m_panelSyntaxHighlight, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelSyntaxHighlight, wxSize(-1, -1)), wxTAB_TRAVERSAL);

        boxSizer21->Add(m_panel25, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);
        m_panel25->SetSizer(bSizer3);
       
        m_notebook2 = new Notebook(m_panel25, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, gsgsNookbookStyle);
        m_notebook2->SetName(wxT("m_notebook2"));

        bSizer3->Add(m_notebook2, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_panelGlobalSettings =
            new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panelGlobalSettings, _("Global Settings"), true);

        wxBoxSizer* bSizer6 = new wxBoxSizer(wxVERTICAL);
        m_panelGlobalSettings->SetSizer(bSizer6);

        wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer2->SetFlexibleDirection(wxBOTH);
        fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer2->AddGrowableCol(1);

        bSizer6->Add(fgSizer2, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText70 = gsgs_Style().createStaticText(m_panelGlobalSettings, wxID_ANY, _("Available Themes:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText70, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer95 = new wxBoxSizer(wxHORIZONTAL);

        fgSizer2->Add(boxSizer95, 0, wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_choiceLexerThemesArr;
        m_choiceLexerThemes = new wxChoice(m_panelGlobalSettings, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), m_choiceLexerThemesArr, 0);

        boxSizer95->Add(m_choiceLexerThemes, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText6 = gsgs_Style().createStaticText(m_panelGlobalSettings, wxID_ANY, _("Global Font:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText6, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_globalFontPicker =
            new wxFontPickerCtrl(m_panelGlobalSettings, wxID_ANY, wxNullFont, wxDefaultPosition,
                wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), wxFNTP_USEFONT_FOR_LABEL);
        m_globalFontPicker->SetToolTip(_("Use this to select a font to be used by *all* styles of this lexer"));

        fgSizer2->Add(m_globalFontPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText7 = gsgs_Style().createStaticText(m_panelGlobalSettings, wxID_ANY, _("Global background Colour:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText7, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_globalBgColourPicker = new wxColourPickerCtrl(m_panelGlobalSettings, wxID_ANY, *wxBLACK, wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)),
            wxCLRP_SHOW_LABEL | wxCLRP_USE_TEXTCTRL | wxCLRP_DEFAULT_STYLE);
        m_globalBgColourPicker->SetToolTip(
            _("Use this to select a background colour to be used by *all* styles of this lexer"));

        fgSizer2->Add(m_globalBgColourPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText8 = gsgs_Style().createStaticText(m_panelGlobalSettings, wxID_ANY, _("File Extensions:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), 0);
        fgSizer2->Add(m_staticText8, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_fileSpec = gsgs_Style().createTextCtrl(m_panelGlobalSettings, wxID_ANY, wxT(""), wxDefaultPosition,
            wxDLG_UNIT(m_panelGlobalSettings, wxSize(-1, -1)), 0);
        m_fileSpec->SetToolTip(_("Associate this lexer with files which have these extensions"));
        fgSizer2->Add(m_fileSpec, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_panelCustomize =
            new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panelCustomize, _("Styles"), false);

        wxBoxSizer* bSizer51 = new wxBoxSizer(wxVERTICAL);
        m_panelCustomize->SetSizer(bSizer51);

        wxBoxSizer* bSizer8 = new wxBoxSizer(wxHORIZONTAL);

        bSizer51->Add(bSizer8, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxArrayString m_propertiesArr;
        m_properties =
            new wxListBox(m_panelCustomize, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)),
                m_propertiesArr, wxLB_SORT | wxLB_SINGLE);

        bSizer8->Add(m_properties, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer9 = new wxBoxSizer(wxVERTICAL);

        bSizer8->Add(bSizer9, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer1->SetFlexibleDirection(wxBOTH);
        fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer1->AddGrowableCol(1);

        bSizer9->Add(fgSizer1, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText2 = gsgs_Style().createStaticText(m_panelCustomize, wxID_ANY, _("Style Font:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        fgSizer1->Add(m_staticText2, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_fontPicker = new wxFontPickerCtrl(m_panelCustomize, wxID_ANY, wxNullFont, wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxFNTP_USEFONT_FOR_LABEL);
        m_fontPicker->SetToolTip(_("Select a font to be used with the selected style"));

        fgSizer1->Add(m_fontPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText3 = gsgs_Style().createStaticText(m_panelCustomize, wxID_ANY, _("Foreground Colour:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        fgSizer1->Add(m_staticText3, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_colourPicker = new wxColourPickerCtrl(m_panelCustomize, wxID_ANY, *wxBLACK, wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)),
            wxCLRP_SHOW_LABEL | wxCLRP_USE_TEXTCTRL | wxCLRP_DEFAULT_STYLE);
        m_colourPicker->SetToolTip(_("Select the foreground colour for the selected style"));

        fgSizer1->Add(m_colourPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText4 = gsgs_Style().createStaticText(m_panelCustomize, wxID_ANY, _("Background Colour:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        fgSizer1->Add(m_staticText4, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_bgColourPicker = new wxColourPickerCtrl(m_panelCustomize, wxID_ANY, *wxBLACK, wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)),
            wxCLRP_SHOW_LABEL | wxCLRP_USE_TEXTCTRL | wxCLRP_DEFAULT_STYLE);
        m_bgColourPicker->SetToolTip(_("Select the background colour for the selected style"));

        fgSizer1->Add(m_bgColourPicker, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        fgSizer1->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_eolFilled = new wxCheckBox(m_panelCustomize, wxID_ANY, _("Style is EOL Filled"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        m_eolFilled->SetValue(false);
        m_eolFilled->SetToolTip(_("If a line ends with a character/word which has this style, the remaining of the line "
            "will be coloured with this style background colour"));

        fgSizer1->Add(m_eolFilled, 0, wxALL, WXC_FROM_DIP(5));

        fgSizer1->Add(0, 0, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_styleWithinPreProcessor = new wxCheckBox(m_panelCustomize, wxID_ANY, _("Styling Within Pre-processor Line"),
            wxDefaultPosition, wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        m_styleWithinPreProcessor->SetValue(false);

        fgSizer1->Add(m_styleWithinPreProcessor, 0, wxALL, WXC_FROM_DIP(5));

        m_staticline1 = new wxStaticLine(m_panelCustomize, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxLI_HORIZONTAL);

        bSizer9->Add(m_staticline1, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText1 = gsgs_Style().createStaticText(m_panelCustomize, wxID_ANY, _("Edit Lexer Keyword Sets:"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), 0);
        bSizer9->Add(m_staticText1, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);

        bSizer9->Add(bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_button5 = new Button(m_panelCustomize, wxID_ANY, _("Set &0"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxBU_EXACTFIT);

        bSizer5->Add(m_button5, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_button6 = new Button(m_panelCustomize, wxID_ANY, _("Set &1"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxBU_EXACTFIT);

        bSizer5->Add(m_button6, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_button7 = new Button(m_panelCustomize, wxID_ANY, _("Set &2"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxBU_EXACTFIT);

        bSizer5->Add(m_button7, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_button8 = new Button(m_panelCustomize, wxID_ANY, _("Set &3"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxBU_EXACTFIT);

        bSizer5->Add(m_button8, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_button9 = new Button(m_panelCustomize, wxID_ANY, _("Set &4"), wxDefaultPosition,
            wxDLG_UNIT(m_panelCustomize, wxSize(-1, -1)), wxBU_EXACTFIT);

        bSizer5->Add(m_button9, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_panelTextSelection =
            new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_notebook2, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_notebook2->AddPage(m_panelTextSelection, _("Text Selection"), false);

        wxBoxSizer* bSizer7 = new wxBoxSizer(wxVERTICAL);
        m_panelTextSelection->SetSizer(bSizer7);

        wxFlexGridSizer* fgSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
        fgSizer3->SetFlexibleDirection(wxBOTH);
        fgSizer3->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        fgSizer3->AddGrowableCol(1);

        bSizer7->Add(fgSizer3, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText9 = gsgs_Style().createStaticText(m_panelTextSelection, wxID_ANY, _("Selected Text Background Colour:"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)), 0);
        fgSizer3->Add(m_staticText9, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_colourPickerSelTextBgColour =
            new wxColourPickerCtrl(m_panelTextSelection, wxID_ANY, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT),
                wxDefaultPosition, wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)),
                wxCLRP_SHOW_LABEL | wxCLRP_USE_TEXTCTRL | wxCLRP_DEFAULT_STYLE);

        fgSizer3->Add(m_colourPickerSelTextBgColour, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText94 = gsgs_Style().createStaticText(m_panelTextSelection, wxID_ANY, _("Use Custom Selection Foreground Colour:"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)), 0);
        fgSizer3->Add(m_staticText94, 0, wxALL | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_checkBoxCustomSelectionFgColour = new wxCheckBox(m_panelTextSelection, wxID_ANY, wxT(""), wxDefaultPosition,
            wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)), 0);
        m_checkBoxCustomSelectionFgColour->SetValue(false);
        m_checkBoxCustomSelectionFgColour->SetToolTip(
            _("You can choose to override the default selection colouring by enabling this checkbox"));

        fgSizer3->Add(m_checkBoxCustomSelectionFgColour, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, WXC_FROM_DIP(5));

        m_staticText84 = gsgs_Style().createStaticText(m_panelTextSelection, wxID_ANY, _("Selected Text Foreground Colour:"),
            wxDefaultPosition, wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)), 0);
        fgSizer3->Add(m_staticText84, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_colourPickerSelTextFgColour =
            new wxColourPickerCtrl(m_panelTextSelection, wxID_ANY, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT),
                wxDefaultPosition, wxDLG_UNIT(m_panelTextSelection, wxSize(-1, -1)),
                wxCLRP_SHOW_LABEL | wxCLRP_USE_TEXTCTRL | wxCLRP_DEFAULT_STYLE);

        fgSizer3->Add(m_colourPickerSelTextFgColour, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer19 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer173->Add(boxSizer19, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        // Connect events
        m_cbUseCustomBaseColour->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomBaseColour), NULL, this);
        m_colourPickerBaseColour->Connect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomColourUI), NULL, this);
        m_colourPickerBaseColour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnCustomBaseColourPIcked),
            NULL, this);
        m_fontPickerGlobal->Connect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnGlobalFontSelected), NULL, this);
        m_choiceGlobalTheme->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnGlobalThemeSelected), NULL, this);
        m_listBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnLexerSelected),
            NULL, this);
        m_choiceLexerThemes->Connect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnTheme), NULL, this);
        m_globalFontPicker->Connect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnFontChanged), NULL, this);
        m_globalBgColourPicker->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_fileSpec->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnText), NULL, this);
        m_properties->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnItemSelected),
            NULL, this);
        m_fontPicker->Connect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnFontChanged), NULL, this);
        m_colourPicker->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_bgColourPicker->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_eolFilled->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEolFilled),
            NULL, this);
        m_styleWithinPreProcessor->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnStyleWithinPreprocessor), NULL,
            this);
        m_styleWithinPreProcessor->Connect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnStyleWithingPreProcessorUI), NULL, this);
        m_button5->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton0), NULL, this);
        m_button6->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton1), NULL, this);
        m_button7->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton2), NULL, this);
        m_button8->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton3), NULL, this);
        m_button9->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton4), NULL, this);
        m_colourPickerSelTextBgColour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnSelTextChanged), NULL,
            this);
        m_checkBoxCustomSelectionFgColour->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomFgTextColour),
            NULL, this);
        m_staticText84->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnTextSelFgUI), NULL, this);
        m_colourPickerSelTextFgColour->Connect(wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnTextSelFgUI), NULL, this);
        m_colourPickerSelTextFgColour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnSelTextFgChanged), NULL,
            this);

        // Get list of available lexers
        wxString lexerName;
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor(true);
        wxArrayString lexers = gsgs_Style().GetAllLexersNames();
        if (editor) { lexerName = editor->GetContext()->GetName().Lower(); }

        for (size_t i = 0; i < lexers.GetCount(); ++i) {
            if (lexers.Item(i) == "c++") {
                m_listBox->Append(CXX_AND_JAVASCRIPT);
            }
            else {
                m_listBox->Append(lexers.Item(i));
            }
        }

        if (!m_listBox->IsEmpty()) {
            if (lexerName.IsEmpty()) {
                m_listBox->Select(0);
            }
            else {
                m_listBox->SetStringSelection(lexerName == "c++" ? CXX_AND_JAVASCRIPT : lexerName);
            }
            LoadLexer(""); // Load the default active theme
        }

        // Load the global colours
        m_choiceGlobalTheme->Append(gsgs_Style().GetAvailableThemesForLexer("c++"));
        m_choiceGlobalTheme->SetStringSelection(gsgs_Style().GetGlobalTheme());

        // Set the current editor font to the default one
        wxFont font;
        gsgs_Config().Read("GlobalThemeFont", font, wxNullFont);
        if (font.IsOk()) { m_fontPickerGlobal->SetSelectedFont(font); }

        DoUpdatePreview();

        m_isModified = false;
        //WindowManager::Load(this);
        m_toolbar->SetMiniToolBar(false);
        m_toolbar->AddTool(wxID_NEW, _("New Theme"), gsgs_Image().LoadBitmap("file_new"));
        m_toolbar->AddTool(wxID_SAVE, _("Export"), gsgs_Image().LoadBitmap("file_save"));
        m_toolbar->AddTool(XRCID("export_all"), _("Export All"),
            gsgs_Image().LoadBitmap("file_save_all"));
        m_toolbar->AddTool(XRCID("import_zip"), _("Import from ZIP file"),
            gsgs_Image().LoadBitmap("file_open"));
        m_toolbar->AddTool(XRCID("revert_changes"), _("Reaload Default Settings"),
            gsgs_Image().LoadBitmap("file_reload"));
        m_toolbar->AddTool(XRCID("import_eclipse_theme"), _("Import Eclipse Theme"),
            gsgs_Image().LoadBitmap("eclipse"), "", wxITEM_DROPDOWN);
        m_toolbar->Realize();
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnNewTheme, this, wxID_NEW);
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnExportSelective, this, wxID_SAVE);
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnExportAll, this, XRCID("export_all"));
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnImport, this, XRCID("import_zip"));
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnRestoreDefaults, this, XRCID("revert_changes"));
        m_toolbar->Bind(wxEVT_TOOL, &EditorSettingsSyntaxHighlight::OnImportEclipseTheme, this, XRCID("import_eclipse_theme"));
        m_toolbar->Bind(wxEVT_TOOL_DROPDOWN,
            [&](wxCommandEvent& e) {
            wxMenu m;
            m.Append(XRCID("load_eclipse_theme_website"), _("Load Eclipse Themes WebSite.."));
            m.Bind(wxEVT_MENU, &EditorSettingsSyntaxHighlight::OnLoadEclipseThemeWebsite, this,
                XRCID("load_eclipse_theme_website"));
            m_toolbar->ShowMenuForButton(XRCID("import_eclipse_theme"), &m);
        },
            XRCID("import_eclipse_theme"));

        // Theme handling
        m_colourPickerBaseColour->SetColour(gsgs_Style().GetAppBgColour());
        m_useBaseColourInitial = gsgs_Style().IsCustomColour();
        m_useBaseColourEnding = m_useBaseColourInitial;
        m_cbUseCustomBaseColour->SetValue(m_useBaseColourInitial);
        if (m_cbUseCustomBaseColour) {
            m_initialTheme = Colour::IsDark(gsgs_Style().GetAppBgColour()) ? kTHEME_DARK : kTHEME_LIGHT;
            m_endingTheme = m_initialTheme;
        }
    }
    //--------------------------------------------------------------------------
    EditorSettingsSyntaxHighlight::~EditorSettingsSyntaxHighlight()
    {
        m_cbUseCustomBaseColour->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomBaseColour), NULL,
            this);
        m_colourPickerBaseColour->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomColourUI), NULL, this);
        m_colourPickerBaseColour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnCustomBaseColourPIcked),
            NULL, this);
        m_fontPickerGlobal->Disconnect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnGlobalFontSelected), NULL, this);
        m_choiceGlobalTheme->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnGlobalThemeSelected), NULL, this);
        m_listBox->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnLexerSelected), NULL, this);
        m_choiceLexerThemes->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnTheme), NULL, this);
        m_globalFontPicker->Disconnect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnFontChanged), NULL, this);
        m_globalBgColourPicker->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_fileSpec->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnText), NULL,
            this);
        m_properties->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnItemSelected), NULL, this);
        m_fontPicker->Disconnect(wxEVT_COMMAND_FONTPICKER_CHANGED,
            wxFontPickerEventHandler(EditorSettingsSyntaxHighlight::OnFontChanged), NULL, this);
        m_colourPicker->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_bgColourPicker->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnColourChanged), NULL, this);
        m_eolFilled->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEolFilled),
            NULL, this);
        m_styleWithinPreProcessor->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnStyleWithinPreprocessor),
            NULL, this);
        m_styleWithinPreProcessor->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnStyleWithingPreProcessorUI), NULL, this);
        m_button5->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton0), NULL, this);
        m_button6->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton1), NULL, this);
        m_button7->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton2), NULL, this);
        m_button8->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton3), NULL, this);
        m_button9->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnEditKeyWordsButton4), NULL, this);
        m_colourPickerSelTextBgColour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnSelTextChanged),
            NULL, this);
        m_checkBoxCustomSelectionFgColour->Disconnect(
            wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(EditorSettingsSyntaxHighlight::OnUseCustomFgTextColour), NULL,
            this);
        m_staticText84->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnTextSelFgUI), NULL,
            this);
        m_colourPickerSelTextFgColour->Disconnect(
            wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsSyntaxHighlight::OnTextSelFgUI), NULL, this);
        m_colourPickerSelTextFgColour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED,
            wxColourPickerEventHandler(EditorSettingsSyntaxHighlight::OnSelTextFgChanged),
            NULL, this);

        // Write the global font
        wxFont font = m_fontPickerGlobal->GetSelectedFont();
        if (font.IsOk()) { gsgs_Config().Write("GlobalThemeFont", font); }
    }
    //--------------------------------------------------------------------------
    bool EditorSettingsSyntaxHighlight::m_globalBgColourChangedTooltipShown = false;
    //--------------------------------------------------------------------------
    const wxString sampleText = "class Demo {\n"
        "private:\n"
        "    std::string m_str;\n"
        "    int m_integer;\n"
        "    \n"
        "public:\n"
        "    /**\n"
        "     * Creates a new demo.\n"
        "     * @param o The object\n"
        "     */\n"
        "    Demo(const Demo &other) {\n"
        "        m_str = other.m_str;\n"
        "        m_integer = other.m_integer;\n"
        "    }\n"
        "}";
    //--------------------------------------------------------------------------
    #define DARK_ICONS _("Dark Theme Icons Set")
    #define LIGHT_ICONS _("Light Theme Icons Set")
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::Save(OptionsConfig * options)
    {
        SaveChanges();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::Apply()
    {
        SaveChanges();
        gsgs_Frame()->GetMainBook()->ApplySettings();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::DoUpdatePreview()
    {
        // Populate the preview
        LexerStyle::Ptr_t previewLexer = gsgs_Style().GetLexer("c++", m_choiceGlobalTheme->GetStringSelection());
        if (previewLexer) 
        { 
            previewLexer->Apply(m_stcPreview, true); 
        }
        m_stcPreview->SetKeyWords(1, "Demo std string");
        m_stcPreview->SetKeyWords(3, "other");
        m_stcPreview->SetEditable(true);
        m_stcPreview->SetText(sampleText);
        m_stcPreview->HideSelection(true);
        m_stcPreview->SetEditable(false);
        ::CalcHScrollBar(m_stcPreview);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::Clear()
    {
        // Global Settings page
        m_choiceLexerThemes->Clear();
        m_globalFontPicker->SetSelectedFont(gsgs_Style().GetFont(wxSYS_ANSI_FIXED_FONT));
        m_globalBgColourPicker->SetColour(gsgs_Style().GetColour(wxSYS_COLOUR_WINDOW));
        m_fileSpec->ChangeValue("");

        // Customize page
        m_properties->Clear();
        m_fontPicker->SetSelectedFont(gsgs_Style().GetFont(wxSYS_ANSI_FIXED_FONT));
        m_colourPicker->SetColour(gsgs_Style().GetColour(wxSYS_COLOUR_WINDOWTEXT));
        m_bgColourPicker->SetColour(gsgs_Style().GetColour(wxSYS_COLOUR_WINDOW));
        m_eolFilled->SetValue(false);
        m_styleWithinPreProcessor->SetValue(false);

        // Text Selection page
        m_colourPickerSelTextBgColour->SetColour(gsgs_Style().GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
        m_isModified = false;
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::LoadLexer(const wxString& themeName)
    {
        wxWindowUpdateLocker locker(this);
        Clear();
        wxString lexer = m_listBox->GetStringSelection();
        if (lexer.IsEmpty()) return;

        if (lexer == CXX_AND_JAVASCRIPT) { lexer = "c++"; }

        m_lexer = gsgs_Style().GetLexer(lexer, themeName);
        CreateLexerPage();
        m_isModified = false;
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        wxString themeName = m_choiceLexerThemes->GetStringSelection();
        LoadLexer(themeName);
        m_isModified = true;
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::SaveChanges()
    {
        // Save all lexers once
        // Update the lexer
        CHECK_PTR_RET(m_lexer);

        // Make sure we got a valid text selection
        // foreground colour
        StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
        if (!selProp.IsNull()) {
            selProp.SetFgColour(m_colourPickerSelTextFgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX));
        }

        // Update the active theme for the lexer
        gsgs_Style().SetActiveTheme(m_lexer->GetName(), m_choiceLexerThemes->GetStringSelection());

        if (m_globalThemeChanged) {
            wxBusyInfo bi(_("Applying changes..."));
            wxBusyCursor bc;
            // Set the theme, it will update the global theme as well
            gsgs_Style().SetTheme(m_choiceGlobalTheme->GetStringSelection());
            m_globalThemeChanged = false;

            m_lexer = gsgs_Style().GetLexer(m_lexer->GetName());
            CallAfter(&EditorSettingsSyntaxHighlight::LoadLexer, m_lexer->GetThemeName());
        }

        // Save the base colour changes
        gsgs_Style().SetCustomColour(m_cbUseCustomBaseColour->IsChecked(), m_colourPickerBaseColour->GetColour());
        // Now save the changes to the file system
        gsgs_Style().Save();
        m_isModified = false;
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnColourChanged(wxColourPickerEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        m_isModified = true;
        // update colour
        wxObject* obj = event.GetEventObject();
        if (obj == m_colourPicker) 
        {
            wxColour colour = event.GetColour();
            StyleProperty::Map_t::iterator iter = GetSelectedStyle();
            iter->second.SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
        }
        else if (obj == m_bgColourPicker) 
        {
            wxColour colour = event.GetColour();
            StyleProperty::Map_t::iterator iter = GetSelectedStyle();
            iter->second.SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
        }
        else if (obj == m_globalBgColourPicker) 
        {
            wxColour colour = event.GetColour();
            DoSetGlobalBgColour(colour);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::EditKeyWords(int set)
    {
        CHECK_PTR_RET(m_lexer);

        wxString keywords = m_lexer->GetKeyWords(set);
        FreeTextDialog dlg(this, keywords);
        if (dlg.ShowModal() == wxID_OK) {
            m_isModified = true;
            m_lexer->SetKeyWords(dlg.GetValue(), set);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEditKeyWordsButton0(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EditKeyWords(0);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEditKeyWordsButton1(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EditKeyWords(1);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEditKeyWordsButton2(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EditKeyWords(2);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEditKeyWordsButton3(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EditKeyWords(3);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEditKeyWordsButton4(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EditKeyWords(4);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnEolFilled(wxCommandEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        m_isModified = true;
        StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
        StyleProperty::Map_t::iterator iter = GetSelectedStyle();
        if (iter != properties.end()) { iter->second.SetEolFilled(event.IsChecked()); }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnFontChanged(wxFontPickerEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        m_isModified = true;

        StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
        // update f
        wxObject* obj = event.GetEventObject();
        if (obj == m_fontPicker) {
            wxFont f = event.GetFont();
            StyleProperty::Map_t::iterator iter = GetSelectedStyle();

            iter->second.SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
            iter->second.SetFaceName(f.GetFaceName());
            iter->second.SetFontSize(f.GetPointSize());
            iter->second.SetUnderlined(f.GetUnderlined());
            iter->second.SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);

        }
        else if (obj == m_globalFontPicker) {
            wxFont f = event.GetFont();
            StyleProperty::Map_t::iterator iter = properties.begin();
            for (; iter != properties.end(); ++iter) {
                iter->second.SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
                iter->second.SetFaceName(f.GetFaceName());
                iter->second.SetFontSize(f.GetPointSize());
                iter->second.SetUnderlined(f.GetUnderlined());
                iter->second.SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);
            }
            // update the style f picker as well
            m_fontPicker->SetSelectedFont(f);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnItemSelected(wxCommandEvent& event)
    {
        CHECK_PTR_RET(m_lexer);

        // update colour picker & font pickers
        wxString selectionString = event.GetString();
        StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
        StyleProperty::Map_t::iterator iter = properties.begin();
        for (; iter != properties.end(); ++iter) {
            if (iter->second.GetName() == selectionString) {
                // update font & color
                StyleProperty p = iter->second;
                wxString colour = p.GetFgColour();
                wxString bgColour = p.GetBgColour();

                int size = p.GetFontSize();
                wxString face = p.GetFaceName();
                bool bold = p.IsBold();

                wxFont font = wxFont(size, wxFONTFAMILY_TELETYPE, p.GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                    bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, p.GetUnderlined(), face);
                m_fontPicker->SetSelectedFont(font);
                m_bgColourPicker->SetColour(bgColour);
                m_colourPicker->SetColour(colour);
                m_eolFilled->SetValue(p.GetEolFilled());
            }
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnOutputViewColourChanged(wxColourPickerEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        event.Skip();
        m_isModified = true;
        m_globalBgColourChanged = true;
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnSelTextChanged(wxColourPickerEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        event.Skip();
        StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
        if (!selProp.IsNull()) {
            m_isModified = true;
            selProp.SetBgColour(event.GetColour().GetAsString(wxC2S_HTML_SYNTAX));
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnStyleWithinPreprocessor(wxCommandEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        m_isModified = true;
        m_lexer->SetStyleWithinPreProcessor(event.IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnStyleWithingPreProcessorUI(wxUpdateUIEvent& event)
    {
        CHECK_PTR_RET(m_lexer);

        StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
        StyleProperty::Map_t::iterator iter = GetSelectedStyle();
        if (iter == properties.end())
            event.Enable(false);
        else if (iter->second.GetName() == "Preprocessor")
            event.Enable(true);
        else
            event.Enable(false);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnText(wxCommandEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        event.Skip();
        m_isModified = true;
        m_lexer->SetFileSpec(m_fileSpec->GetValue());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::CreateLexerPage()
    {
        CHECK_PTR_RET(m_lexer);

        const StyleProperty::Map_t& m_propertyList = m_lexer->GetLexerProperties();
        StyleProperty::Map_t::const_iterator it = m_propertyList.begin();
        StyleProperty selTextProperties;

        for (; it != m_propertyList.end(); it++) {
            if (it->second.GetId() != SEL_TEXT_ATTR_ID) {
                m_properties->Append(it->second.GetName());
            }
            else {
                selTextProperties = it->second;
            }
        }

        if (m_properties->GetCount()) { m_properties->SetSelection(0); }

        wxString initialColor = gsgs_Style().GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX);
        wxString bgInitialColor = gsgs_Style().GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
        wxFont initialFont = wxNullFont;
        // bool     initialEolFilled (false);
        bool initialStyleWithinPreProcessor(true);

        if (m_propertyList.empty() == false) {
            StyleProperty p;
            p = m_propertyList.begin()->second;
            initialColor = p.GetFgColour();
            bgInitialColor = p.GetBgColour();

            int size = p.GetFontSize();
            wxString face = p.GetFaceName();
            bool bold = p.IsBold();
            initialFont = wxFont(size, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL,
                bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, false, face);
        }
        initialStyleWithinPreProcessor = m_lexer->GetStyleWithinPreProcessor();
        const StyleProperty& defaultStyle = m_lexer->GetProperty(0);
        if (!defaultStyle.IsNull()) {
            m_colourPicker->SetColour(defaultStyle.GetFgColour());
            m_bgColourPicker->SetColour(defaultStyle.GetBgColour());
            m_globalBgColourPicker->SetColour(defaultStyle.GetBgColour());
        }

        m_fontPicker->SetSelectedFont(initialFont);
        m_globalFontPicker->SetSelectedFont(initialFont);
        m_fileSpec->ChangeValue(m_lexer->GetFileSpec());
        m_styleWithinPreProcessor->SetValue(initialStyleWithinPreProcessor);

        // Update selected text properties
        m_colourPickerSelTextBgColour->SetColour(selTextProperties.GetBgColour());
        m_colourPickerSelTextFgColour->SetColour(selTextProperties.GetFgColour());
        m_checkBoxCustomSelectionFgColour->SetValue(m_lexer->IsUseCustomTextSelectionFgColour());

        if (m_propertyList.empty()) {
            m_fontPicker->Enable(false);
            m_colourPicker->Enable(false);
        }

        // Fill the themes for this lexer
        m_choiceLexerThemes->Clear();
        wxArrayString themes = gsgs_Style().GetAvailableThemesForLexer(m_lexer->GetName());
        int sel = themes.Index(m_lexer->GetThemeName());
        if (sel == -1) { sel = 0; }
        m_choiceLexerThemes->Append(themes);
        if (!m_choiceLexerThemes->IsEmpty()) { m_choiceLexerThemes->SetSelection(sel); }
    }
    //--------------------------------------------------------------------------
    StyleProperty::Map_t::iterator EditorSettingsSyntaxHighlight::GetSelectedStyle()
    {
        wxString selectedProperty = m_properties->GetStringSelection();
        StyleProperty::Map_t& lexerProperties = m_lexer->GetLexerProperties();
        return std::find_if(lexerProperties.begin(), lexerProperties.end(), StyleProperty::FindByName(selectedProperty));
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnLexerSelected(wxCommandEvent& event)
    {
        wxString lexerName = m_listBox->GetStringSelection();
        if (lexerName == CXX_AND_JAVASCRIPT) { lexerName = "c++"; }

        if (m_isModified) { SaveChanges(); }
        m_isModified = false;
        m_lexer = gsgs_Style().GetLexer(lexerName);
        LoadLexer("");
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnButtonApplyUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(m_isModified); 
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnTextSelFgUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_checkBoxCustomSelectionFgColour->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnSelTextFgChanged(wxColourPickerEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        event.Skip();
        StyleProperty& selProp = m_lexer->GetProperty(SEL_TEXT_ATTR_ID);
        if (!selProp.IsNull()) {
            m_isModified = true;
            selProp.SetFgColour(event.GetColour().GetAsString(wxC2S_HTML_SYNTAX));
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnUseCustomFgTextColour(wxCommandEvent& event)
    {
        CHECK_PTR_RET(m_lexer);
        event.Skip();
        m_isModified = true;
        m_lexer->SetUseCustomTextSelectionFgColour(event.IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnNewTheme(wxCommandEvent& event)
    {
        // Create new theme
        CHECK_PTR_RET(m_lexer);
        NewThemeDialog dlg(this, m_lexer);
        if (dlg.ShowModal() == wxID_OK) {
            // Create new XML and load it
            LexerStyle::Ptr_t newLexer =
                gsgs_Style().CopyTheme(dlg.GetLexerName(), dlg.GetThemeName(), dlg.GetBaseTheme());
            if (newLexer) { LoadLexer(newLexer->GetName()); }
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnImport(wxCommandEvent& event)
    {
        wxString path = ::wxFileSelector(_("Select file"), "", "", "", "Zip Files (*.zip)|*.zip", wxFD_OPEN);
        if (path.IsEmpty()) return;

        wxFileName fn(path);
        ZipReader zr(fn);
        zr.Extract("lexers.json", gsgs_Path().GetTempDir());

        wxFileName fileToImport(gsgs_Path().GetTempDir(), "lexers.json");
        if (gsgs_Style().ImportLexersFile(fileToImport, true)) {
            //EndModal(wxID_OK);
            // relaunch the dialog
            wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(openEvent);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnExportSelective(wxCommandEvent& event)
    {
        // Get list of choices
        wxArrayString lexers = gsgs_Style().GetAllThemes();
        wxArrayInt choices;
        if (::wxGetSelectedChoices(choices, _("Select which themes to export:"), _("Export Themes"), lexers, this) ==
            wxNOT_FOUND) {
            return;
        }
        wxArrayString lexersToExport;
        for (size_t i = 0; i < choices.GetCount(); ++i) {
            wxString lexerName = lexers.Item(choices.Item(i)).Lower();
            lexersToExport.push_back(lexerName);
        }
        DoExport(lexersToExport);
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnExportAll(wxCommandEvent& event)
    {
        // EclipseThemeImporterManager importer;
        // importer.ImportCxxToAll();
        DoExport();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnToolExportAll(wxCommandEvent& event)
    {
        // EclipseThemeImporterManager importer;
        // importer.ImportCxxToAll();
        DoExport();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnRestoreDefaults(wxCommandEvent& event)
    {
        // Ask for confirmation
        if (::wxMessageBox(_("Are you sure you want to restore colours to factory defaults?\nBy choosing 'Yes', you will "
            "lose all your local modifications"),
            _("Confirm"), wxICON_WARNING | wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxCENTER, this) == wxYES) {
            // Restore defaults
            gsgs_Style().RestoreDefaults();
            // Dismiss the dialog
            //EndModal(wxID_OK);
            // and reload it
            wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(openEvent);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnImportEclipseTheme(wxCommandEvent& event)
    {
        wxFileDialog selector(this, _("Select eclipse XML theme file"), "", "", "Eclipse Theme Files (*.xml)|*.xml",
            wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);
        if (selector.ShowModal() == wxID_OK) {
            wxArrayString files;
            selector.GetPaths(files);
            if (files.IsEmpty()) return;
            for (size_t i = 0; i < files.size(); ++i) {
                gsgs_Style().ImportEclipseTheme(files.Item(i));
            }
            // Mark the dialg is modified and force a save
            wxBusyCursor bc;
            m_isModified = true;
            SaveChanges();

            ::wxMessageBox(_("File imported successfully!"));
            // Dismiss the dialog
            //EndModal(wxID_OK);
            // and reload it
            wxCommandEvent openEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("syntax_highlight"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(openEvent);
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnLoadEclipseThemeWebsite(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        ::wxLaunchDefaultBrowser("http://eclipsecolorthemes.org/");
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnGlobalThemeSelected(wxCommandEvent& event)
    {
        m_globalThemeChanged = true;
        m_isModified = true;
        DoUpdatePreview();

        LexerStyle::Ptr_t previewLexer =
            gsgs_Style().GetLexer("text", m_choiceGlobalTheme->GetStringSelection());

        if (previewLexer && previewLexer->IsDark() && m_cbUseCustomBaseColour->IsChecked()) {
            wxColour bgColour = gsgs_Style().GetBackgroundColourFromLexer(previewLexer);
            m_colourPickerBaseColour->SetColour(bgColour);
            m_endingTheme = kTHEME_DARK;
        }
        else if (previewLexer && m_cbUseCustomBaseColour->IsChecked()) {
            // Light colour
            wxColour bgColour = gsgs_Style().GetBackgroundColourFromLexer(previewLexer);
            m_colourPickerBaseColour->SetColour(bgColour);
            m_endingTheme = kTHEME_LIGHT;
        }
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnGlobalFontSelected(wxFontPickerEvent& event)
    {
        m_isModified = true;
        gsgs_Style().SetGlobalFont(event.GetFont());
        DoUpdatePreview();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::DoSetGlobalBgColour(const wxColour& colour)
    {
        StyleProperty::Map_t& properties = m_lexer->GetLexerProperties();
        StyleProperty::Map_t::iterator iter = properties.begin();
        for (; iter != properties.end(); ++iter) {
            // Dont change the text selection using the global font picker
            if (iter->second.GetName() == wxT("Text Selection")) continue;
            iter->second.SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
        }

        // update the style background colour as well
        m_bgColourPicker->SetColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::DoShowTooltipForGlobalBgColourChanged() 
    {
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::DoExport(const wxArrayString& lexers)
    {
        // Select the 'save' path
        wxString path = ::wxFileSelector(_("Save as"), "", "MySettings.zip", "", wxFileSelectorDefaultWildcardStr,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (path.IsEmpty()) { return; }

        wxFileName jsonFile(path);
        jsonFile.SetFullName("lexers.json");
        // Delete the file when done
        FileUtil::Deleter deleter(jsonFile);
        if (!gsgs_Style().ExportThemesToFile(jsonFile, lexers)) { return; }

        // Add the file to the zip
        ZipWriter zw(path);
        zw.Add(jsonFile);
        zw.Close();

        ::wxMessageBox(_("Settings have been saved into:\n") + zw.GetFilename().GetFullPath());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnUseCustomColourUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_cbUseCustomBaseColour->IsChecked());
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnCustomBaseColourPIcked(wxColourPickerEvent& event)
    {
        m_isModified = true;
        m_endingTheme = Colour::IsDark(event.GetColour()) ? kTHEME_DARK : kTHEME_LIGHT;
        event.Skip();
    }
    //--------------------------------------------------------------------------
    void EditorSettingsSyntaxHighlight::OnUseCustomBaseColour(wxCommandEvent& event)
    {
        m_isModified = true;
        m_useBaseColourEnding = event.IsChecked();
        if (event.IsChecked()) {
            // Adjust the colour to the selected theme
            LexerStyle::Ptr_t lexer =
                gsgs_Style().GetLexer("text", m_choiceGlobalTheme->GetStringSelection());
            wxColour bgColour = gsgs_Style().GetBackgroundColourFromLexer(lexer);
            if (bgColour.IsOk()) { m_colourPickerBaseColour->SetColour(bgColour); }
        }
        m_endingTheme = Colour::IsDark(m_colourPickerBaseColour->GetColour()) ? kTHEME_DARK : kTHEME_LIGHT;
        event.Skip();
    }
    //--------------------------------------------------------------------------
    bool EditorSettingsSyntaxHighlight::IsRestartRequired() const
    {
        return (m_useBaseColourEnding != m_useBaseColourInitial) || (m_initialTheme != m_endingTheme);
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // NewThemeDialog
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    NewThemeDialog::NewThemeDialog(wxWindow* parent, LexerStyle::Ptr_t lexer, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        if (!bBitmapLoaded) {
            // We need to initialise the default bitmap handler
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            wxCrafterqEa5vvInitBitmapResources();
            bBitmapLoaded = true;
        }

        wxBoxSizer* boxSizer115 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer115);

        wxFlexGridSizer* flexGridSizer125 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer125->SetFlexibleDirection(wxBOTH);
        flexGridSizer125->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer125->AddGrowableCol(1);

        boxSizer115->Add(flexGridSizer125, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText127 =
            gsgs_Style().createStaticText(this, wxID_ANY, _("Theme Name:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        flexGridSizer125->Add(m_staticText127, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlName = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_textCtrlName->SetToolTip(_("Set the theme name"));
        flexGridSizer125->Add(m_textCtrlName, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText131 =
            gsgs_Style().createStaticText(this, wxID_ANY, _("Language:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        flexGridSizer125->Add(m_staticText131, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceLanguageArr;
        m_choiceLanguage =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceLanguageArr, 0);
        m_choiceLanguage->SetToolTip(_("Make this theme for this language"));

        flexGridSizer125->Add(m_choiceLanguage, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_staticText135 =
            gsgs_Style().createStaticText(this, wxID_ANY, _("Based on Theme:"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer125->Add(m_staticText135, 0, wxALL | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        wxArrayString m_choiceBaseThemeArr;
        m_choiceBaseTheme =
            new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), m_choiceBaseThemeArr, 0);
        m_choiceBaseTheme->SetToolTip(_("Set the base theme for this new theme"));

        flexGridSizer125->Add(m_choiceBaseTheme, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_stdBtnSizer117 = new DialogButtonSizer();

        boxSizer115->Add(m_stdBtnSizer117, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, WXC_FROM_DIP(5));

        m_buttonOK = new Button(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_buttonOK->SetDefault();
        m_stdBtnSizer117->AddButton(m_buttonOK);

        m_buttonCancel = new Button(this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);
        m_stdBtnSizer117->AddButton(m_buttonCancel);
        m_stdBtnSizer117->Realize();

        //if (GetSizer()) { GetSizer()->Fit(this); }
        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_choiceLanguage->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(NewThemeDialog::OnLexerSelected),
            NULL, this);
        m_buttonOK->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewThemeDialog::OnOkUI), NULL, this);
        
        DoInitialize(lexer);
    }
    //--------------------------------------------------------------------------
    NewThemeDialog::~NewThemeDialog()
    {
        m_choiceLanguage->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED,
            wxCommandEventHandler(NewThemeDialog::OnLexerSelected), NULL, this);
        m_buttonOK->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(NewThemeDialog::OnOkUI), NULL, this);
    }
    //--------------------------------------------------------------------------
    void NewThemeDialog::OnOkUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(!m_textCtrlName->IsEmpty()); 
    }
    //--------------------------------------------------------------------------
    void NewThemeDialog::OnLexerSelected(wxCommandEvent& event)
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer(m_choiceLanguage->GetStringSelection());
        if (lexer) {
            DoInitialize(lexer);
        }
    }
    //--------------------------------------------------------------------------
    void NewThemeDialog::DoInitialize(LexerStyle::Ptr_t lexer)
    {
        m_choiceBaseTheme->Clear();
        m_choiceLanguage->Clear();

        // Populate the fields
        wxArrayString allLexers = gsgs_Style().GetAllLexersNames();
        //    int sel = allLexers.Index(lexer->GetName());
        m_choiceLanguage->Append(allLexers);
        m_choiceLanguage->SetStringSelection(lexer->GetName());

        m_choiceBaseTheme->Append(gsgs_Style().GetAvailableThemesForLexer(lexer->GetName()));
        if (!m_choiceBaseTheme->IsEmpty()) {
            m_choiceBaseTheme->SetSelection(0);
        }
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // EditorSettingsAccelTable
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    EditorSettingsAccelTable::EditorSettingsAccelTable(wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint & pos,
        const wxSize & size, long style) :
        wxPanel(parent, id,pos,size, style),
        TreeBookNode()
    {
        wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(bSizer1);

        wxBoxSizer* bSizer5 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(bSizer5, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_textCtrlFilter = new wxSearchCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTE_PROCESS_ENTER);
        m_textCtrlFilter->SetToolTip(
            _("Search for a keyboard shortcut either by its keyboard shortcut or by its description"));
        m_textCtrlFilter->SetFocus();
        m_textCtrlFilter->ShowSearchButton(true);
        m_textCtrlFilter->ShowCancelButton(false);

        bSizer5->Add(m_textCtrlFilter, 1, wxALL, WXC_FROM_DIP(5));

        wxBoxSizer* bSizer4 = new wxBoxSizer(wxHORIZONTAL);

        bSizer1->Add(bSizer4, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl = new ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(500, 300)),
            wxDV_VERT_RULES | wxDV_ROW_LINES | wxDV_SINGLE);

        bSizer4->Add(m_dvListCtrl, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl->AppendTextColumn(_("Keyboard Shortcut"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrl->AppendTextColumn(_("Description"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        wxBoxSizer* bSizer3 = new wxBoxSizer(wxVERTICAL);

        bSizer4->Add(bSizer3, 0, 0, WXC_FROM_DIP(5));

        m_buttonEdit = new Button(this, wxID_ANY, _("&Edit..."), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer3->Add(m_buttonEdit, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_buttonDefault =
            new Button(this, wxID_ANY, _("&Defaults"), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        bSizer3->Add(m_buttonDefault, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        // Connect events
        m_textCtrlFilter->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditorSettingsAccelTable::OnText), NULL, this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(EditorSettingsAccelTable::OnDVItemActivated), NULL, this);
        m_buttonEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsAccelTable::OnEditButton), NULL,
            this);
        m_buttonEdit->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsAccelTable::OnEditUI), NULL, this);
        m_buttonDefault->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsAccelTable::OnButtonDefaults),
            NULL, this);
        
        wxImageList* imageList = new wxImageList(16, 16);
        imageList->Add(gsgs_Image().LoadBitmap(wxT("list-control/16/sort_down")));
        imageList->Add(gsgs_Image().LoadBitmap(wxT("list-control/16/sort_up")));

        gsgs_Window().getHotKeyList(m_accelMap);
        PopulateTable("");

        m_textCtrlFilter->SetFocus();
    }
    //--------------------------------------------------------------------------
    EditorSettingsAccelTable::~EditorSettingsAccelTable()
    {
        m_textCtrlFilter->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(EditorSettingsAccelTable::OnText), NULL,
            this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(EditorSettingsAccelTable::OnDVItemActivated), NULL, this);
        m_buttonEdit->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EditorSettingsAccelTable::OnEditButton), NULL,
            this);
        m_buttonEdit->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(EditorSettingsAccelTable::OnEditUI), NULL, this);
        m_buttonDefault->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(EditorSettingsAccelTable::OnButtonDefaults), NULL, this);
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::Save(OptionsConfig * options)
    {
        gsgs_Window().SetAccelerators(m_accelMap);
        gsgs_Window().UpdateAccels();
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::Apply()
    {

    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::PopulateTable(const wxString& filter)
    {
        m_dvListCtrl->DeleteAllItems();

        WindowManager::HotKeyList filteredMap;
        if (filter.IsEmpty()) {
            filteredMap = m_accelMap;
        }
        else {
            for (WindowManager::HotKeyList::iterator iter = m_accelMap.begin(); iter != m_accelMap.end(); ++iter) {
                if (!IsMatchesFilter(filter, iter->second)) continue;
                filteredMap.insert(std::make_pair(iter->first, iter->second));
            }
        }

        if (filteredMap.empty()) { return; }

        // Add core entries
        std::vector<std::tuple<wxString, wxString, AccelItemData*> > V;
        for (WindowManager::HotKeyList::const_iterator iter = filteredMap.begin(); iter != filteredMap.end(); ++iter) {
            const WindowManager::HotKey & mid = iter->second;

            wxString desc = mid.parentMenu.BeforeFirst(':');
            if (desc.IsEmpty()) { desc << "Global Accelerator"; }
            desc << " | ";
            desc << mid.action.AfterLast(':');
            V.push_back(std::make_tuple(desc, mid.accel, new AccelItemData(mid)));
        }

        // Sort the items in the list, based on the description
        std::sort(V.begin(), V.end());
        std::for_each(V.begin(), V.end(), [&](const std::tuple<wxString, wxString, AccelItemData*>& entry) {
            const wxString& desc = std::get<0>(entry);
            const wxString& shortcut = std::get<1>(entry);
            AccelItemData* itemData = std::get<2>(entry);

            wxVector<wxVariant> cols;
            cols.push_back(shortcut);
            cols.push_back(desc);
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)itemData);
        });
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::OnButtonDefaults(wxCommandEvent& e)
    {
        // re-load the default key bindings settings
        m_accelMap.clear();
        m_textCtrlFilter->ChangeValue(""); // Clear the filter
        gsgs_Window().clearAllHotKey();
        gsgs_Window().getHotKeyList(m_accelMap);
        CallAfter(&EditorSettingsAccelTable::PopulateTable, wxString());
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::OnEditButton(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        DoItemActivated();
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::DoItemActivated()
    {
        wxDataViewItem sel = m_dvListCtrl->GetSelection();
        CHECK_ITEM_RET(sel);

        AccelItemData* itemData = DoGetItemData(sel);
        if (!itemData) return;

        // build the selected entry
        WindowManager::HotKey mid = itemData->m_menuItemData;
        if (gsgs_Window().showAddHotKeyDlg(this, mid) == wxID_OK) {
            // search the list for similar accelerator
            WindowManager::HotKey who;
            if (HasAccelerator(mid.accel, who)) {
                if (who.action == mid.action) { return; }
                if (wxMessageBox(wxString::Format(_("'%s' is already assigned to: '%s'\nWould you like to replace it?"),
                    mid.accel, who.action),
                    _("光栅故事"), wxYES_NO | wxCENTER | wxICON_QUESTION, this) != wxYES) {
                    return;
                }

                // Remove the old entry
                wxDataViewItem oldItem = FindAccel(mid);
                if (oldItem.IsOk()) {
                    AccelItemData* cd = DoGetItemData(oldItem);
                    if (cd) {
                        cd->m_menuItemData.accel.Clear();
                        int row = m_dvListCtrl->ItemToRow(oldItem);
                        m_dvListCtrl->SetValue(wxString(), row, 2);
                    }
                }

                WindowManager::HotKeyList::iterator iter = m_accelMap.find(who.resourceID);
                if (iter != m_accelMap.end()) {
                    iter->second.accel.Clear(); // Clear the accelerator
                }
            }

            // Update the client data
            itemData->m_menuItemData = mid;

            // Update the UI
            int row = m_dvListCtrl->ItemToRow(sel);
            if (row == wxNOT_FOUND) return;
            m_dvListCtrl->SetValue(mid.accel, row, 0);

            // and update the map
            WindowManager::HotKeyList::iterator iter = m_accelMap.find(itemData->m_menuItemData.resourceID);
            if (iter != m_accelMap.end()) { iter->second.accel = itemData->m_menuItemData.accel; }
        }
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::OnText(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        CallAfter(&EditorSettingsAccelTable::PopulateTable, m_textCtrlFilter->GetValue());
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::OnDVItemActivated(wxDataViewEvent& event)
    {
        wxUnusedVar(event);
        DoItemActivated();
    }
    //-------------------------------------------------------------------------------
    void EditorSettingsAccelTable::OnEditUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(m_dvListCtrl->GetSelectedItemsCount()); 
    }
    //-------------------------------------------------------------------------------
    bool EditorSettingsAccelTable::IsMatchesFilter(const wxString& filter, const WindowManager::HotKey& item)
    {
        wxString lcFilter = filter.Lower();
        lcFilter.Trim().Trim(false);
        if (lcFilter.IsEmpty()) 
            return true;

        const wxString & parentMenu = item.parentMenu;

        wxString haystack;
        haystack << parentMenu << " " << item.accel << " " << item.action;
        return FileUtil::FuzzyMatch(filter, haystack);
    }
    //-------------------------------------------------------------------------------
    bool EditorSettingsAccelTable::HasAccelerator(const wxString& accel, WindowManager::HotKey& who)
    {
        if (accel.IsEmpty()) 
            return false;
        WindowManager::HotKeyList::iterator it, iend = m_accelMap.end();
        for (it = m_accelMap.begin(); it != iend; ++it)
        {
            if (it->second.accel == accel)
            {
                who = it->second;
                return true;
            }
        }
        return false;
    }
    //-------------------------------------------------------------------------------
    AccelItemData* EditorSettingsAccelTable::DoGetItemData(const wxDataViewItem& item)
    {
        return reinterpret_cast<AccelItemData*>(m_dvListCtrl->GetItemData(item));
    }
    //-------------------------------------------------------------------------------
    wxDataViewItem EditorSettingsAccelTable::FindAccel(const WindowManager::HotKey& mid)
    {
        for (size_t i = 0; i < m_dvListCtrl->GetItemCount(); ++i) 
        {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            if (item.IsOk())
            {
                AccelItemData* cd = DoGetItemData(item);
                if (cd && cd->m_menuItemData.accel == mid.accel) 
                    return item;
            }
        }
        return wxDataViewItem();
    }
    //-------------------------------------------------------------------------------
}