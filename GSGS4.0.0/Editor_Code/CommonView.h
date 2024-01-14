///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 18 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/checklst.h>
#include <wx/statbox.h>
#include <wx/choicebk.h>
#include <wx/combobox.h>
#include <wx/choice.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include "gsgsButton.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class OptionAddView
///////////////////////////////////////////////////////////////////////////////
class OptionAddView : public wxDialog
{
	private:

	protected:
		wxTextCtrl* m_text;
		wxStaticLine* m_staticline9;
		gsgs::Button* m_buttonOK;
		gsgs::Button* m_buttonCancel;

	public:

		OptionAddView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("OptionEdit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 481,299 ), long style = wxDEFAULT_DIALOG_STYLE );
		~OptionAddView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class OptionCommandAddView
///////////////////////////////////////////////////////////////////////////////
class OptionCommandAddView : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText1;
		wxCheckListBox* m_checkListOptions;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textOptions;
		wxTextCtrl* m_textCmdLn;
		wxStaticLine* m_staticline5;
		DialogButtonSizer* m_sdbSizer;
		gsgs::Button* m_sdbSizerOK;
		gsgs::Button* m_sdbSizerCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnOptionToggled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOptionsText( wxCommandEvent& event ) { event.Skip(); }


	public:

		OptionCommandAddView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,450 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~OptionCommandAddView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectBuildView
///////////////////////////////////////////////////////////////////////////////
class ProjectBuildView : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText;
		wxCheckListBox* m_checkListConfigurations;
		gsgs::Button* m_buttonBuild;
		gsgs::Button* m_buttonClean;
		gsgs::Button* m_button3;
		gsgs::Button* m_button4;
		gsgs::Button* m_buttonDown;
		gsgs::Button* m_buttonUp;
		wxStaticLine* m_staticline2;
		gsgs::Button* m_buttonClose;

		// Virtual event handlers, overide them in your derived class
		virtual void OnItemSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemToggled( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBuild( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnBuildUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClean( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCleanUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnCheckAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUnCheckAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveUp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveUpUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnMoveDown( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnMoveDownUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }


	public:

		ProjectBuildView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Batch Build"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 634,512 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectBuildView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebugCommandView
///////////////////////////////////////////////////////////////////////////////
class DebugCommandView : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrlName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrlCommand;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrtDebuggerCommand;
		wxStaticLine* m_staticline2;
		gsgs::Button* m_buttonOk;
		gsgs::Button* m_buttonCancel;

	public:

		DebugCommandView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Debugger Type"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 456,164 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DebugCommandView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ProjectDependView
///////////////////////////////////////////////////////////////////////////////
class ProjectDependView : public wxDialog
{
	private:

	protected:
		wxChoicebook* m_book;
		wxStaticLine* m_staticline1;
		gsgs::Button* m_buttonOK;
		gsgs::Button* m_buttonCancel;

	public:

		ProjectDependView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Dependencies"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 532,361 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ProjectDependView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DebugCoreDumpView
///////////////////////////////////////////////////////////////////////////////
class DebugCoreDumpView : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText2;
		wxComboBox* m_Core;
		gsgs::Button* m_buttonBrowseCore;
		wxStaticText* m_staticText21;
		wxComboBox* m_ExeFilepath;
		gsgs::Button* m_buttonBrowseExe;
		wxStaticText* m_staticText5;
		wxComboBox* m_WD;
		gsgs::Button* m_buttonBrowseWD;
		wxStaticText* m_staticText3;
		wxChoice* m_choiceDebuggers;
		wxStaticLine* m_staticline1;
		gsgs::Button* m_buttonDebug;
		gsgs::Button* m_buttonCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonBrowseCore( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseExe( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonBrowseWD( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonDebug( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDebugBtnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		DebugCoreDumpView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Debug a core dump"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 401,207 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~DebugCoreDumpView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SymbolsView
///////////////////////////////////////////////////////////////////////////////
class SymbolsView : public wxDialog
{
	private:

	protected:
		wxListCtrl* m_results;
		wxStaticLine* statLine;
		gsgs::Button* m_okButton;
		gsgs::Button* m_cancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnButtonOK( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		SymbolsView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Resolve Ambiguity"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 851,428 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SymbolsView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class NewItemView
///////////////////////////////////////////////////////////////////////////////
class NewItemView : public wxDialog
{
	private:

	protected:
		wxListCtrl* m_fileType;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_fileName;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_location;
		gsgs::Button* m_browseBtn;
		wxStaticLine* m_staticline1;
		gsgs::Button* m_okButton;
		gsgs::Button* m_cancel;

	public:

		NewItemView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 512,462 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~NewItemView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class EditConfigurationView
///////////////////////////////////////////////////////////////////////////////
class EditConfigurationView : public wxDialog
{
	private:

	protected:
		wxPanel* m_panel6;
		wxListBox* m_configurationsList;
		gsgs::Button* m_buttonDelete;
		gsgs::Button* m_buttonRename;
		wxStaticLine* m_staticline9;
		gsgs::Button* m_buttonClose;

	public:

		EditConfigurationView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Edit Configurations"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 338,199 ), long style = wxDEFAULT_DIALOG_STYLE );
		~EditConfigurationView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class FreeTextView
///////////////////////////////////////////////////////////////////////////////
class FreeTextView : public wxDialog
{
	private:

	protected:
		wxTextCtrl* m_text;
		wxStaticLine* m_staticline9;
		gsgs::Button* m_buttonOK;
		gsgs::Button* m_buttonCancel;

	public:

		FreeTextView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Edit"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 481,299 ), long style = wxDEFAULT_DIALOG_STYLE );
		~FreeTextView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class NewConfigurationView
///////////////////////////////////////////////////////////////////////////////
class NewConfigurationView : public wxDialog
{
	private:

	protected:
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textConfigurationName;
		wxStaticText* m_staticText2;
		wxChoice* m_choiceCopyConfigurations;
		wxStaticLine* m_staticline1;
		gsgs::Button* m_buttonOK;
		gsgs::Button* m_buttonCancel;

	public:

		NewConfigurationView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Configuration"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 352,199 ), long style = wxDEFAULT_DIALOG_STYLE );
		~NewConfigurationView();

};

///////////////////////////////////////////////////////////////////////////////
/// Class RenameFileView
///////////////////////////////////////////////////////////////////////////////
class RenameFileView : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textCtrlReplaceWith;
		wxStaticText* m_staticText1;
		wxCheckListBox* m_checkListMatches;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticTextIncludedInFile;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticTextFoundInLine;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticTextPattern;
		gsgs::Button* m_buttonOk;
		gsgs::Button* m_buttonCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnFileSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFileToggeled( wxCommandEvent& event ) { event.Skip(); }


	public:

		RenameFileView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Fix Include Statement"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 574,437 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~RenameFileView();

};

