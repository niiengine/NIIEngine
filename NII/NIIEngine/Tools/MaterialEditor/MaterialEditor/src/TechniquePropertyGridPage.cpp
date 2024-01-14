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
#include "TechniquePropertyGridPage.h"

#include <boost/bind.hpp>

#include <wx/propgrid/advprops.h>

#include "OgreTechnique.h"

#include "TechniqueController.h"
#include "TechniqueEventArgs.h"

BEGIN_EVENT_TABLE(TechniquePropertyGridPage, wxPropertyGridPage)
	EVT_PG_CHANGED(-1, TechniquePropertyGridPage::propertyChanged)
END_EVENT_TABLE()

TechniquePropertyGridPage::TechniquePropertyGridPage(TechniqueController * controller)
: mController(controller)
{
	mController->subscribe(TechniqueController::NameChanged, boost::bind(&TechniquePropertyGridPage::nameChanged, this, _1));
	mController->subscribe(TechniqueController::SchemeChanged, boost::bind(&TechniquePropertyGridPage::schemeNameChanged, this, _1));
	mController->subscribe(TechniqueController::LodIndexChanged, boost::bind(&TechniquePropertyGridPage::lodIndexChanged, this, _1));
}

TechniquePropertyGridPage::~TechniquePropertyGridPage()
{
}

void TechniquePropertyGridPage::populate()
{
	mNameId = Append(wxStringProperty(wxT("Name"), wxPG_LABEL, mController->getShaderFusion()->getName()));
	mSchemeNameId = Append(wxStringProperty(wxT("Scheme Name"), wxPG_LABEL, mController->getShaderFusion()->getSchemeID()));
	mLodIndexId = Append(wxIntProperty(wxT("LOD Index"), wxPG_LABEL, mController->getShaderFusion()->getLodIndex()));
}

void TechniquePropertyGridPage::propertyChanged(wxPropertyGridEvent & event)
{
	wxPGId id = event.GetProperty();
	if(id == mNameId)
	{
		mController->setName(event.GetPropertyValueAsString().c_str());
	}
	else if(id == mSchemeNameId)
	{
		mController->setSchemeID(event.GetPropertyValueAsString().c_str());
	}
	else if(id = mLodIndexId)
	{
		mController->setLodIndex(event.GetPropertyValueAsInt());
	}
}

void TechniquePropertyGridPage::nameChanged(EventArgs & args)
{
	TechniqueEventArgs tea = dynamic_cast<TechniqueEventArgs &>(args);
	TechniqueController * tc = tea.getTechniqueController();

	wxPGProperty * prop = GetPropertyPtr(mNameId);

	if(prop == NULL)
        return;

	prop->SetValueFromString(tc->getShaderFusion()->getName().c_str());
}

void TechniquePropertyGridPage::schemeNameChanged(EventArgs & args)
{
	TechniqueEventArgs tea = dynamic_cast<TechniqueEventArgs &>(args);
	TechniqueController * tc = tea.getTechniqueController();

	wxPGProperty * prop = GetPropertyPtr(mSchemeNameId);

	if(prop == NULL)
        return;

	prop->SetValueFromString(tc->getShaderFusion()->getSchemeID().c_str());
}

void TechniquePropertyGridPage::lodIndexChanged(EventArgs & args)
{
	TechniqueEventArgs tea = dynamic_cast<TechniqueEventArgs &>(args);
	TechniqueController * tc = tea.getTechniqueController();

	wxPGProperty * prop = GetPropertyPtr(mLodIndexId);

	if(prop == NULL)
        return;

	prop->SetValueFromInt(tc->getShaderFusion()->getLodIndex());
}
