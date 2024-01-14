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
#include "MaterialPage.h"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "Project.h"
#include "Workspace.h"

BEGIN_EVENT_TABLE(MaterialPage, wxWizardPageSimple)
END_EVENT_TABLE()

MaterialPage::MaterialPage(wxWizard* parent)
: wxWizardPageSimple(parent)
{
	createPage();
}

MaterialPage::~MaterialPage()
{
}

void MaterialPage::createPage()
{
	mSizer = new wxBoxSizer(wxVERTICAL);
	
	// Project Label
	mProjectLabel = new wxStaticText(this, wxID_ANY, wxT("Project:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mProjectLabel, 0, wxALL, 5);

	// Project Combo Box
	wxArrayString projectNames;
	const ProjectList* projects = Workspace::getSingletonPtr()->getProjects();
	for(ProjectList::const_iterator it = projects->begin(); it != projects->end(); ++it)
		projectNames.Add((*it)->getName().c_str());
	
	// TODO: Select first Project
	mProjectComboBox = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, projectNames, wxCB_DROPDOWN);
	mProjectComboBox->SetEditable(false);
	mSizer->Add(mProjectComboBox, 0, wxALL | wxEXPAND, 5);
	
	// Name Label
	mNameLabel = new wxStaticText(this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mNameLabel, 0, wxALL, 5);

	// Name Text
	mNameText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mSizer->Add(mNameText, 0, wxALL | wxEXPAND, 5);

	SetSizer(mSizer);
	Layout();
}

void MaterialPage::getName(wxString& name) const
{
	name = mNameText->GetValue();
}

Project* MaterialPage::getProject() const
{
	wxString project = mProjectComboBox->GetValue();

	return Workspace::getSingletonPtr()->getProject(project.c_str());
}

void MaterialPage::setProject(Project* project)
{
	mProjectComboBox->SetValue(project != NULL ? project->getName().c_str() : wxEmptyString);
}

