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
#include "TechniqueWizard.h"

#include <wx/sizer.h>

#include "MaterialController.h"
#include "Project.h"
#include "TechniqueController.h"
#include "TechniquePage.h"
#include "Workspace.h"

BEGIN_EVENT_TABLE(TechniqueWizard, wxWizard)
	EVT_WIZARD_FINISHED(wxID_ANY, TechniqueWizard::OnFinish)
END_EVENT_TABLE()

TechniqueWizard::TechniqueWizard()
{
}

TechniqueWizard::~TechniqueWizard()
{
}

bool TechniqueWizard::Create(wxWindow* parent, int id /* = -1 */, const wxString& title /* = wxEmptyString */, const wxBitmap& bitmap /* = wxNullBitmap */, const wxPoint& pos /* = wxDefaultPosition */, long style /* = wxDEFAULT_DIALOG_STYLE */)
{
	bool result = wxWizard::Create(parent, id, title, bitmap, pos, style);

	mTechniquePage = new TechniquePage(this);

	GetPageAreaSizer()->Add(mTechniquePage);

	return result;
}

TechniquePage* TechniqueWizard::getTechniquePage() const
{
	return mTechniquePage;
}

void TechniqueWizard::OnFinish(wxWizardEvent& event)
{
	Project* project = mTechniquePage->getProject();
	MaterialController* mc = mTechniquePage->getMaterial();

	wxString name;
	mTechniquePage->getName(name);

	mc->createTechnique(name.c_str());
}
