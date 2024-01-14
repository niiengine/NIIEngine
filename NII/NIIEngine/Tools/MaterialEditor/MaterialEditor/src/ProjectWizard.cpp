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
#include "ProjectWizard.h"

#include <wx/sizer.h>

#include "Project.h"
#include "Workspace.h"

BEGIN_EVENT_TABLE(ProjectWizard, wxWizard)
	EVT_WIZARD_FINISHED(wxID_ANY, ProjectWizard::OnFinish)
END_EVENT_TABLE()

ProjectWizard::ProjectWizard()
{
}

ProjectWizard::~ProjectWizard()
{
}

bool ProjectWizard::Create(wxWindow* parent, int id /* = -1 */, const wxString& title /* = wxEmptyString */, const wxBitmap& bitmap /* = wxNullBitmap */, const wxPoint& pos /* = wxDefaultPosition */, long style /* = wxDEFAULT_DIALOG_STYLE */)
{
	bool result = wxWizard::Create(parent, id, title, bitmap, pos, style);

	mProjectPage = new ProjectPage(this);

	GetPageAreaSizer()->Add(mProjectPage);

	return result;
}

ProjectPage* ProjectWizard::getProjectPage() const
{
	return mProjectPage;
}

void ProjectWizard::OnFinish(wxWizardEvent& event)
{
	wxString name;
	mProjectPage->getName(name);

	wxString location;
	mProjectPage->getLocation(location);

	Project* project = new Project(name.c_str());
	Workspace::getSingletonPtr()->addProject(project);
}