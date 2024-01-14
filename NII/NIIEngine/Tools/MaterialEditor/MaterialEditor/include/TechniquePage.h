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
#ifndef _TECHNIQUEPAGE_H_
#define _TECHNIQUEPAGE_H_

#include <wx/wizard.h>

#include "Project.h"

class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxTextCtrl;

class TechniqueController;

class TechniquePage : public wxWizardPageSimple
{
public:
	TechniquePage(wxWizard* parent);
	TechniquePage(wxWizard* parent, Project* project);
	TechniquePage(wxWizard* parent, Project* project, MaterialController* mc);
	virtual ~TechniquePage();

	void getName(wxString& name) const;

	Project* getProject() const;
	void setProject(Project* project);

	MaterialController* getMaterial() const;
	void setMaterial(MaterialController* mc);

	void populateMaterials(const MaterialControllerList* materials);

	void OnProjectSelected(wxCommandEvent& event);

protected:
	void createPage();

	wxBoxSizer* mSizer;
	wxStaticText* mProjectLabel;
	wxComboBox* mProjectComboBox;
	wxStaticText* mMaterialLabel;
	wxComboBox* mMaterialComboBox;
	wxStaticText* mNameLabel;
	wxTextCtrl* mNameText;

	Project* mProject;
	MaterialController* mMaterial;

	DECLARE_EVENT_TABLE()
};

#endif // _TECHNIQUEPAGE_H_
