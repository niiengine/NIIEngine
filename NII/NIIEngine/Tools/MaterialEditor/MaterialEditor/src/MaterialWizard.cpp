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
#include "MaterialWizard.h"

#include <wx/sizer.h>

#include "MaterialPage.h"
#include "Project.h"
#include "Workspace.h"

BEGIN_EVENT_TABLE(MaterialWizard, wxWizard)
	EVT_WIZARD_FINISHED(wxID_ANY, MaterialWizard::OnFinish)
END_EVENT_TABLE()

MaterialWizard::MaterialWizard()
{
}

MaterialWizard::~MaterialWizard()
{
}

bool MaterialWizard::Create(wxWindow* parent, int id /* = -1 */, const wxString& title /* = wxEmptyString */, const wxBitmap& bitmap /* = wxNullBitmap */, const wxPoint& pos /* = wxDefaultPosition */, long style /* = wxDEFAULT_DIALOG_STYLE */)
{
	bool result = wxWizard::Create(parent, id, title, bitmap, pos, style);

	mMaterialPage = new MaterialPage(this);

	GetPageAreaSizer()->Add(mMaterialPage);

	return result;
}

MaterialPage* MaterialWizard::getMaterialPage() const
{
	return mMaterialPage;
}

void MaterialWizard::OnFinish(wxWizardEvent& event)
{
	Project* project = mMaterialPage->getProject();

	wxString name;
	mMaterialPage->getName(name);

	project->createMaterial(name.c_str());
}
