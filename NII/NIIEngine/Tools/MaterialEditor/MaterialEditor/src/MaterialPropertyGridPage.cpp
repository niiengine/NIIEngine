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
#pragma warning(disable:4800)

#include "MaterialPropertyGridPage.h"

#include "MaterialController.h"

BEGIN_EVENT_TABLE(MaterialPropertyGridPage, wxPropertyGridPage)
	EVT_PG_CHANGED(-1, MaterialPropertyGridPage::propertyChange)
END_EVENT_TABLE()

MaterialPropertyGridPage::MaterialPropertyGridPage(MaterialController* controller)
: mController(controller)
{
	
}

MaterialPropertyGridPage::~MaterialPropertyGridPage()
{
}

void MaterialPropertyGridPage::populate()
{
	mPropertyNameId = Append(wxStringProperty(wxT("Name"), wxPG_LABEL, mController->getMaterial()->getName()));
	mPropertyReceiveShadowsId = Append(wxBoolProperty(wxT("Receive Shadows"), wxPG_LABEL, mController->getMaterial()->isReceiveShadow()));
	mPropertyTransparencyCastsShadowsId = Append(wxBoolProperty(wxT("Transparency Casts Shadows"), wxPG_LABEL, mController->getMaterial()->isAlphaShadowCast()));
}

void MaterialPropertyGridPage::propertyChange(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();
	if(id == mPropertyNameId)
	{
		
	}
	else if(id == mPropertyReceiveShadowsId)
	{
		mController->setReceiveShadow(event.GetPropertyValueAsBool());
	}
	else if(id == mPropertyTransparencyCastsShadowsId)
	{
		mController->setAlphaShadowCast(event.GetPropertyValueAsBool());
	}
}
