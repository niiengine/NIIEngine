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
#include "PassWizard.h"

#include <wx/sizer.h>

#include "MaterialController.h"
#include "PassController.h"
#include "Project.h"
#include "TechniqueController.h"
#include "TechniquePage.h"
#include "Workspace.h"

BEGIN_EVENT_TABLE(PassWizard, wxWizard)
EVT_WIZARD_FINISHED(wxID_ANY, PassWizard::OnFinish)
END_EVENT_TABLE()

PassWizard::PassWizard()
{
}

PassWizard::~PassWizard()
{
}

bool PassWizard::Create(wxWindow * parent, int id /* = -1 */, const wxString & title /* = wxEmptyString */, const wxBitmap& bitmap /* = wxNullBitmap */, const wxPoint& pos /* = wxDefaultPosition */, long style /* = wxDEFAULT_DIALOG_STYLE */)
{
	bool result = wxWizard::Create(parent, id, title, bitmap, pos, style);

	mPassPage = new PassPage(this);

	GetPageAreaSizer()->Add(mPassPage);

	return result;
}

PassPage * PassWizard::getPassPage() const
{
	return mPassPage;
}

void PassWizard::OnFinish(wxWizardEvent & event)
{
	Project * project = mPassPage->getProject();
	MaterialController * mc = mPassPage->getMaterial();
	TechniqueController * tc = mPassPage->getShaderFusion();

	wxString name;
	mPassPage->getName(name);

	tc->create(name.c_str());
}
