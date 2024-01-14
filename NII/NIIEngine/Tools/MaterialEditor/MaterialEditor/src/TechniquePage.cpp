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
#include "TechniquePage.h"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "MaterialController.h"
#include "Workspace.h"

const long ID_MATERIAL_COMBO_BOX = wxNewId();

BEGIN_EVENT_TABLE(TechniquePage, wxWizardPageSimple)
	EVT_CHOICE(ID_MATERIAL_COMBO_BOX, TechniquePage::OnProjectSelected)
END_EVENT_TABLE()

TechniquePage::TechniquePage(wxWizard* parent)
: wxWizardPageSimple(parent), mProject(NULL), mMaterial(NULL)
{
	createPage();
}

TechniquePage::TechniquePage(wxWizard* parent, Project* project)
: wxWizardPageSimple(parent), mProject(project), mMaterial(NULL)
{
	createPage();
}


TechniquePage::TechniquePage(wxWizard* parent, Project* project, MaterialController* mc)
: wxWizardPageSimple(parent), mProject(project), mMaterial(mc)
{
	createPage();
}


TechniquePage::~TechniquePage()
{
}

void TechniquePage::createPage()
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
	mSizer->Add(mProjectComboBox, 0, wxALL | wxEXPAND, 5);

	// Material Label
	mMaterialLabel = new wxStaticText(this, wxID_ANY, wxT("Material:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mMaterialLabel, 0, wxALL, 5);

	// Material Combo Box
	mMaterialComboBox = new wxComboBox(this, ID_MATERIAL_COMBO_BOX);
	mSizer->Add(mMaterialComboBox, 0, wxALL | wxEXPAND, 5);

	// Name Label
	mNameLabel = new wxStaticText(this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0);
	mSizer->Add(mNameLabel, 0, wxALL, 5);

	// Name Text
	mNameText = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mSizer->Add(mNameText, 0, wxALL | wxEXPAND, 5);

	SetSizer(mSizer);
	Layout();
}

void TechniquePage::getName(wxString& name) const
{
	name = mNameText->GetValue();
}

Project* TechniquePage::getProject() const
{
	wxString project = mProjectComboBox->GetValue();

	return Workspace::getSingletonPtr()->getProject(project.c_str());
}

MaterialController* TechniquePage::getMaterial() const
{
	wxString material = mMaterialComboBox->GetValue();

	return getProject()->getMaterialController(material.c_str());
}

void TechniquePage::setProject(Project* project)
{
	mProjectComboBox->SetValue(project != NULL ? project->getName().c_str() : wxEmptyString);
	populateMaterials(project->getMaterials());
}

void TechniquePage::setMaterial(MaterialController* mc)
{
	mMaterialComboBox->SetValue(mc != NULL ? mc->getMaterial()->getName().c_str() : wxEmptyString);
}

void TechniquePage::OnProjectSelected(wxCommandEvent& event)
{
	Project* project = getProject();
	if(project != NULL)
		populateMaterials(project->getMaterials());
}

void TechniquePage::populateMaterials(const MaterialControllerList* materials)
{
	wxArrayString materialNames;
	MaterialControllerList::const_iterator it;
	for(it = materials->begin(); it != materials->end(); ++it)
	{
		materialNames.Add((*it)->getMaterial()->getName().c_str());
	}

	mMaterialComboBox->Clear();
	mMaterialComboBox->Append(materialNames);
}

