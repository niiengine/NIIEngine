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
#ifndef _MATERIALPROPERTYGRIDPAGE_H_
#define _MATERIALPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>

class MaterialController;

class MaterialPropertyGridPage : public wxPropertyGridPage
{
public:
	MaterialPropertyGridPage(MaterialController* controller);
	virtual ~MaterialPropertyGridPage();
	virtual void populate();

protected:
	virtual void propertyChange(wxPropertyGridEvent& event);

	MaterialController* mController;

	wxPGId mPropertyNameId;
	wxPGId mPropertyReceiveShadowsId;
	wxPGId mPropertyTransparencyCastsShadowsId;

	DECLARE_EVENT_TABLE();
};

#endif // _MATERIALPROPERTYGRIDPAGE_H_