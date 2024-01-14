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
#include "ProjectPage.h"

#include <wx/button.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

BEGIN_EVENT_TABLE(ProjectPage, wxWizardPageSimple)
	EVT_BUTTON(wxID_ANY, ProjectPage::OnBrowse)
END_EVENT_TABLE()

ProjectPage::ProjectPage(wxWizard* parent)
: wxWizardPageSimple(parent)
{
	createPage();
}

ProjectPage::~ProjectPage()
{
}

void ProjectPage::createPage()
{
	mSizer = new wxBoxSizer(wxVERTICAL);
	
	mNameLabel = new wxStaticText(this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mNameLabel, 0, wxALL, 5);

	mNameText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mSizer->Add(mNameText, 0, wxALL | wxEXPAND, 5);

	mLocationLabel = new wxStaticText(this, wxID_ANY, wxT("Location:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mLocationLabel, 0, wxALL, 5);

	mLocationText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mSizer->Add(mLocationText, 0, wxALL | wxEXPAND, 5);

	mBrowseButton = new wxButton(this, wxID_ANY, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mBrowseButton, 0, wxALL | wxALIGN_RIGHT, 5);

	SetSizer(mSizer);
	Layout();
}

void ProjectPage::getName(wxString& name) const
{
	name = mNameText->GetValue();
}

void ProjectPage::getLocation(wxString& location) const
{
	location = mLocationText->GetValue();
}

void ProjectPage::OnBrowse(wxCommandEvent& event)
{
	wxDirDialog* dlg = new wxDirDialog(this, wxT("Project Directory"), wxT("/"), wxDD_NEW_DIR_BUTTON);
	if(dlg->ShowModal()== wxID_OK)
	{
		mLocationText->SetValue(dlg->GetPath());
		mLocationText->SetSelection(0, mLocationText->GetValue().length() - 1);
	}
}