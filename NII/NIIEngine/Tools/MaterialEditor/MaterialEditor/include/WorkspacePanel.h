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
#ifndef _WORKSPACEPANEL_H_
#define _WORKSPACEPANEL_H_

#include <map>

#include <wx/panel.h>
#include <wx/treectrl.h>

class wxBitmapButton;
class wxCommandEvent;
class wxFlexGridSizer;
class wxImageList;
class wxMenu;

class EventArgs;
class MaterialController;
class PassController;
class Project;
class TechniqueController;

typedef std::map<Project*, wxTreeItemId> ProjectIdMap;
typedef std::map<MaterialController*, wxTreeItemId> MaterialIdMap;
typedef std::map<TechniqueController*, wxTreeItemId> TechniqueIdMap;
typedef std::map<PassController*, wxTreeItemId> PassIdMap;

class WorkspacePanel : public wxPanel
{
public:
	WorkspacePanel(wxWindow* parent,
				   wxWindowID id = wxID_ANY,
				   const wxPoint& pos = wxDefaultPosition,
				   const wxSize& size = wxDefaultSize,
				   long style = wxTAB_TRAVERSAL | wxNO_BORDER,
				   const wxString& name = wxT("Workspace Panel"));

	virtual ~WorkspacePanel();

	void OnRightClick(wxTreeEvent& event);
	void OnActivate(wxTreeEvent& event);
	void OnSelectionChanged(wxTreeEvent& event);
	void OnNewProject(wxCommandEvent& event);
	void OnNewMaterialScript(wxCommandEvent& event);
	void OnNewMaterial(wxCommandEvent& event);
	void OnNewTechnique(wxCommandEvent& event);
	void OnNewPass(wxCommandEvent& event);
	void OnAddMaterial(wxCommandEvent& event);
	void OnEdit(wxCommandEvent& event);
	void OnUpdateProjectMenuItem(wxUpdateUIEvent& event);
	void OnUpdateMaterialMenuItem(wxUpdateUIEvent& event);
	void OnUpdateTechniqueMenuItem(wxUpdateUIEvent& event);
	void OnUpdatePassMenuItem(wxUpdateUIEvent& event);

	// Workspace Event Handlers
	void projectAdded(EventArgs& args);
	void projectRemoved(EventArgs& args);

	// Project Event Handlers
	void projectNameChanged(EventArgs& args);
	void projectMaterialAdded(EventArgs& args);
	void projectMaterialRemoved(EventArgs& args);

	// Material Event Handlers
	void materialNameChanged(EventArgs& args);
	void materialTechniqueAdded(EventArgs& args);
	void materialTechniqueRemoved(EventArgs& args);

	// ShaderFusion Event Handlers
	void techniqueNameChanged(EventArgs& args);
	void techniquePassAdded(EventArgs& args);
	void techniquePassRemoved(EventArgs& args);

protected:
	void createPanel();
	wxImageList* getImageList();
	
	void appendNewMenu(wxMenu* menu);
	void showContextMenu(wxPoint point, wxTreeItemId id);
	void appendProjectMenuItems(wxMenu* menu);
	void appendMaterialMenuItems(wxMenu* memu);
	void appendTechniqueMenuItems(wxMenu* menu);
	void appendPassMenuItems(wxMenu* menu);

	Project* getProject(wxTreeItemId id);
	MaterialController* getMaterial(wxTreeItemId id);
	TechniqueController* getShaderFusion(wxTreeItemId id);
	PassController* getPass(wxTreeItemId id);

	bool isWorkspace(wxTreeItemId id);
	bool isProject(wxTreeItemId id);
	bool isMaterial(wxTreeItemId id);
	bool isTechnique(wxTreeItemId id);
	bool isPass(wxTreeItemId id);

	// Event Handling Utils
	void subscribe(Project* project);
	void subscribe(MaterialController* material);
	void subscribe(TechniqueController* technique);

	wxImageList* mImageList;
	wxFlexGridSizer* mSizer;
	wxPanel* mToolBarPanel;
	wxTreeCtrl* mTreeCtrl;

	wxTreeItemId mRootId;

	wxMenu* mNewMenu;

	ProjectIdMap mProjectIdMap;
	MaterialIdMap mMaterialIdMap;
	TechniqueIdMap mTechniqueIdMap;
	PassIdMap mPassIdMap;

	DECLARE_EVENT_TABLE()
};

#endif // _WORKSPACEPANEL_H_

