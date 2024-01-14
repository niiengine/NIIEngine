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
#ifndef _PROJECTPAGE_H_
#define _PROJECTPAGE_H_

#include <wx/wizard.h>

class wxBoxSizer;
class wxButton;
class wxStaticText;
class wxTextCtrl;

class ProjectPage : public wxWizardPageSimple
{
public:
	ProjectPage(wxWizard* parent);
	virtual ~ProjectPage();

	void getName(wxString& name) const;
	void getLocation(wxString& location) const;

	void OnBrowse(wxCommandEvent& event);

protected:
	void createPage();

	wxBoxSizer* mSizer;
	wxStaticText* mNameLabel;
	wxTextCtrl* mNameText;
	wxStaticText* mLocationLabel;
	wxTextCtrl* mLocationText;
	wxButton* mBrowseButton;

	DECLARE_EVENT_TABLE()
};

#endif // _PROJECTPAGE_H_