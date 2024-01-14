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
#ifndef _MATERIALEDITORFRAME_H_
#define _MATERIALEDITORFRAME_H_

#include <wx/wx.h>

#include "OgreRenderSystem.h"
#include "OgreRoot.h"

class wxAuiManager;
class wxAuiNotebook;
class wxNotebook;
class wxPropertyGridManager;
class wxTreeCtrl;

namespace
{
	class RenderSystem;
	class Root;
}

class DocPanel;
class EventArgs;
class LogPanel;
class PropertiesPanel;
class ResourcePanel;
class WorkspacePanel;
class wxOgre;

class MaterialEditorFrame : public wxFrame
{
public:
	MaterialEditorFrame(wxWindow* parent = NULL);
	~MaterialEditorFrame();

protected:
	void createAuiManager(void);
	void createAuiNotebookPane(void);
	void createManagementPane(void);
	void createInformationPane(void);
	void createPropertiesPane();
	void createOgrePane(void);

	void createMenuBar(void);
	void createFileMenu(void);
	void createEditMenu(void);
	void createViewMenu(void);
	void createToolsMenu(void);
	void createWindowMenu(void);
	void createHelpMenu(void);

	void OnActivate(wxActivateEvent& event);

	void OnActiveEditorChanged(EventArgs& args);

	void OnNewProject(wxCommandEvent& event);
	void OnNewMaterial(wxCommandEvent& event);
	void OnFileOpen(wxCommandEvent& event);
	void OnFileSave(wxCommandEvent& event);
	void OnFileSaveAs(wxCommandEvent& event);
	void OnFileClose(wxCommandEvent& event);
	void OnFileExit(wxCommandEvent& event);
	void OnEditUndo(wxCommandEvent& event);
	void OnEditRedo(wxCommandEvent& event);
	void OnEditCut(wxCommandEvent& event);
	void OnEditCopy(wxCommandEvent& event);
	void OnEditPaste(wxCommandEvent& event);
	void OnViewOpenGL(wxCommandEvent& event);
	void OnViewDirectX(wxCommandEvent& event);

private:
	wxMenuBar* mMenuBar;
	wxMenu* mFileMenu;
	wxMenu* mEditMenu;
	wxMenu* mViewMenu;
	wxMenu* mToolsMenu;
	wxMenu* mWindowMenu;
	wxMenu* mHelpMenu;

	wxAuiManager* mAuiManager;

	wxAuiNotebook* mAuiNotebook;
	wxAuiNotebook* mManagementNotebook;
	wxAuiNotebook* mInformationNotebook;
	WorkspacePanel* mWorkspacePanel;
	ResourcePanel* mResourcePanel;
	PropertiesPanel* mPropertiesPanel;

	Ogre::Root* mRoot;
	Ogre::GeometrySpace* mEntity;

	LogPanel* mLogPanel;
	DocPanel* mDocPanel;
	wxOgre* mOgreControl;

	Ogre::RenderSystem* mDirectXRenderSystem;
	Ogre::RenderSystem* mOpenGLRenderSystem;

	DECLARE_EVENT_TABLE();
};

#endif // _MATERIALEDITORFRAME_H_