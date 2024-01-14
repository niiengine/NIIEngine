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
#ifndef _TECHNIQUEPROPERTYGRIDPAGE_H_
#define _TECHNIQUEPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>

class TechniqueController;
class EventArgs;

class TechniquePropertyGridPage : public wxPropertyGridPage
{
public:
	TechniquePropertyGridPage(TechniqueController* controller);
	virtual ~TechniquePropertyGridPage();

	virtual void populate();

	void nameChanged(EventArgs& args);
	void schemeNameChanged(EventArgs& args);
	void lodIndexChanged(EventArgs& args);

protected:
	virtual void propertyChanged(wxPropertyGridEvent& event);
	
	TechniqueController* mController;

	wxPGId mNameId;
	wxPGId mSchemeNameId;
	wxPGId mLodIndexId;

	DECLARE_EVENT_TABLE();
};

#endif // _TECHNIQUEPROPERTYGRIDPAGE_H_