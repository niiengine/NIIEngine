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
#ifndef _PROPERTIESPANEL_H_
#define _PROPERTIESPANEL_H_

#include <map>

#include <wx/panel.h>

class wxGridSizer;
class wxPropertyGridManager;

class EventArgs;
class MaterialController;
class MaterialEventArgs;
class MaterialPropertyGridPage;
class PassController;
class PassEventArgs;
class PassPropertyGridPage;
class ProjectEventArgs;
class SelectionEventArgs;
class TechniqueController;
class TechniqueEventArgs;
class TechniquePropertyGridPage;

//typedef std::map<Project*, ProjectP> ProjectPageMap;
typedef std::map<MaterialController*, int> MaterialPageIndexMap;
typedef std::map<TechniqueController*, int> TechniquePageIndexMap;
typedef std::map<PassController*, int> PassPageIndexMap;

class PropertiesPanel : public wxPanel
{
public:
	PropertiesPanel(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL | wxNO_BORDER,
		const wxString& name = wxT("Properties Panel"));

	virtual ~PropertiesPanel();

	void selectionChanged(EventArgs& args);

	void projectRemoved(EventArgs& args);
	void materialRemoved(EventArgs& args);
	void techniqueRemoved(EventArgs& args);
	void passRemoved(EventArgs& args);

protected:
	wxGridSizer* mGridSizer;
	wxPropertyGridManager* mPropertyGrid;

	MaterialPageIndexMap mMaterialPageIndexMap;
	TechniquePageIndexMap mTechniquePageIndexMap;
	PassPageIndexMap mPassPageIndexMap;

	DECLARE_EVENT_TABLE()
};

#endif // _PROPERTIESPANEL_H_
