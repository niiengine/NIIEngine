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
#ifndef _MATERIALPAGE_H_
#define _MATERIALPAGE_H_

#include <wx/wizard.h>

class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxTextCtrl;

class Project;

class MaterialPage : public wxWizardPageSimple
{
public:
	MaterialPage(wxWizard* parent);
	virtual ~MaterialPage();

	void getName(wxString& name) const;
	Project* getProject() const;
	void setProject(Project* project);

protected:
	void createPage();

	wxBoxSizer* mSizer;
	wxStaticText* mProjectLabel;
	wxComboBox* mProjectComboBox;
	wxStaticText* mNameLabel;
	wxTextCtrl* mNameText;

	DECLARE_EVENT_TABLE()
};

#endif // _MATERIALPAGE_H_
