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
#ifndef _PASSPAGE_H_
#define _PASSPAGE_H_

#include <wx/wizard.h>

class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxTextCtrl;

#include "Project.h"
#include "MaterialController.h"

class TechniqueController;

class PassPage : public wxWizardPageSimple
{
public:
	PassPage(wxWizard* parent);
	PassPage(wxWizard* parent, Project* project);
	PassPage(wxWizard* parent, Project* project, MaterialController* mc);
	PassPage(wxWizard* parent, Project* project, MaterialController* mc, TechniqueController* tc);
	virtual ~PassPage();

	void getName(wxString & name) const;

	Project * getProject() const;
	void setProject(Project * project);

	MaterialController * getMaterial() const;
	void setMaterial(MaterialController * mc);

	TechniqueController * getShaderFusion() const;
	void setTechnique(TechniqueController * mc);

	void populateMaterials(const MaterialControllerList * materials);
	void populateTechniques(const TechniqueControllerList * techniques);

	void OnProjectSelected(wxCommandEvent & event);
	void OnMaterialSelected(wxCommandEvent & event);
	//void OnTechniqueSelected(wxCommandEvent& event);

protected:
	void createPage();

	wxBoxSizer* mSizer;
	wxStaticText* mProjectLabel;
	wxComboBox* mProjectComboBox;
	wxStaticText* mMaterialLabel;
	wxComboBox* mMaterialComboBox;
	wxStaticText* mTechniqueLabel;
	wxComboBox* mTechniqueComboBox;
	wxStaticText* mNameLabel;
	wxTextCtrl* mNameText;

	Project* mProject;
	MaterialController* mMaterial;
	TechniqueController* mTechnique;

	DECLARE_EVENT_TABLE()
};

#endif // _PASSPAGE_H_
