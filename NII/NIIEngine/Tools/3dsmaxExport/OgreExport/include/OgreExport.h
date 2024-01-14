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


#include "OgreMaxConfig.h"
#include "OgreMaxMaterialExport.h"
#include "OgreMaxMeshExport.h"
#include "OgreMaxMeshXMLExport.h"

class OgreMaxExport;

INT_PTR CALLBACK ExportPropertiesDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK GeneralTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MeshTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SkeletalAnimationTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK VertexAnimationTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MaterialTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// utility functions -- we do these a lot, nice to be able to shortcut them
inline void disableControl(HWND dlg, INT id) { EnableWindow(GetDlgItem(dlg, id), FALSE); }
inline void enableControl(HWND dlg, INT id) { EnableWindow(GetDlgItem(dlg, id), TRUE); }

// export dialog tab pane handlers -- we delegate the processing of tab pane events to classes because it gets messy
// in a hurry if we try to handle all of this in a huge monolithic case statement

class OgreMaxExport_TabPaneHandler {
public:
	HWND getDialogHandle() { return m_hDlg; }
	void setExportInterface(ExpInterface* ei, Interface* i) { m_ei = ei; m_i = i; }
	virtual void onInitDialog(HWND hDlg) { m_hDlg = hDlg; }
	virtual void update() = 0;

protected:
	HWND m_hDlg;
	ExpInterface* m_ei;
	Interface* m_i;
};

class OgreMaxExport_General : public OgreMaxExport_TabPaneHandler {
	// this contains the switch/case statement that fires the events that this class handles
	friend INT_PTR CALLBACK GeneralTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	// config holds all of the settings 
	OgreMaxExport_General(OgreMax::Config& config, OgreMaxExport* e) : m_config(config), m_exp(e) { }
	virtual ~OgreMaxExport_General() {}

	// all dialog handlers need to do this -- on demand, read data from their panes
	void update();

	// all dialog handlers need to catch the WM_DESTROY message too
	void onDestroy();

	// event handlers
	void onInitDialog(HWND hDlg);
	void onExportDirectoryChanged(const std::string& newDirectory);
	void onInvertYZ(bool checked);
	void onSetScale(float newScale);

private:
	OgreMax::Config& m_config;
	OgreMaxExport* m_exp;
};

class OgreMaxExport_Mesh : public OgreMaxExport_TabPaneHandler {
	// this contains the switch/case statement that fires the events that this class handles
	friend INT_PTR CALLBACK MeshTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	// config holds all of the settings 
	OgreMaxExport_Mesh(OgreMax::Config& config, OgreMaxExport* e) : m_config(config), m_exp(e) { }
	virtual ~OgreMaxExport_Mesh() {}

	// all dialog handlers need to do this -- on demand, read data from their panes
	void update();

	// all dialog handlers need to catch the WM_DESTROY message too
	void onDestroy();

	// event handlers
	void onInitDialog(HWND hDlg);
	void onClickBinaryMesh();

protected:
	// internal utils
	void setControlStates();

private:
	OgreMax::Config& m_config;
	OgreMaxExport* m_exp;
};

class OgreMaxExport_SkeletalAnimation : public OgreMaxExport_TabPaneHandler {
	// this contains the switch/case statement that fires the events that this class handles
	friend INT_PTR CALLBACK SkeletalAnimationTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	// config holds all of the settings 
	OgreMaxExport_SkeletalAnimation(OgreMax::Config& config, OgreMaxExport* e) : m_config(config), m_exp(e) { }
	virtual ~OgreMaxExport_SkeletalAnimation() {}

	// all dialog handlers need to do this -- on demand, read data from their panes
	void update();

	// all dialog handlers need to catch the WM_DESTROY message too
	void onDestroy();

	// event handlers
	void onInitDialog(HWND hDlg);
	void onAddAnimation();
	void onDeleteAnimation();

protected:
	// utility methods
	void addAnimation();
	void deleteAnimation();

private:
	OgreMax::Config& m_config;
	OgreMaxExport* m_exp;
};

class OgreMaxExport_VertexAnimation : public OgreMaxExport_TabPaneHandler {
	// this contains the switch/case statement that fires the events that this class handles
	friend INT_PTR CALLBACK VertexAnimationTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	// config holds all of the settings 
	OgreMaxExport_VertexAnimation(OgreMax::Config& config, OgreMaxExport* e) : m_config(config), m_exp(e) { }
	virtual ~OgreMaxExport_VertexAnimation() {}

	// all dialog handlers need to do this -- on demand, read data from their panes
	void update();

	// all dialog handlers need to catch the WM_DESTROY message too
	void onDestroy();

	// event handlers
	void onInitDialog(HWND hDlg);

private:
	OgreMax::Config& m_config;
	OgreMaxExport* m_exp;
};

class OgreMaxExport_Material : public OgreMaxExport_TabPaneHandler {
	// this contains the switch/case statement that fires the events that this class handles
	friend INT_PTR CALLBACK MaterialTabDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	// config holds all of the settings 
	OgreMaxExport_Material(OgreMax::Config& config, OgreMaxExport* e) : m_config(config), m_exp(e) { }
	virtual ~OgreMaxExport_Material() {}

	// all dialog handlers need to do this -- on demand, read data from their panes
	void update();

	// all dialog handlers need to catch the WM_DESTROY message too
	void onDestroy();

	// event handlers
	void onInitDialog(HWND hDlg);

private:
	OgreMax::Config& m_config;
	OgreMaxExport* m_exp;
};

class OgreMaxExport : public SceneExport { 

	friend INT_PTR CALLBACK ExportPropertiesDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	friend class OgreMaxExport_General;
	friend class OgreMaxExport_Mesh;
	friend class OgreMaxExport_SkeletalAnimation;
	friend class OgreMaxExport_VertexAnimation;
	friend class OgreMaxExport_Material;

public:
	OgreMaxExport(HINSTANCE hInst);
	virtual ~OgreMaxExport();
	virtual int ExtCount();					// Number of extemsions supported
	virtual const TCHAR * Ext(int n);					// Extension #n (i.e. "3DS")
	virtual const TCHAR * LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	virtual const TCHAR * ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	virtual const TCHAR * AuthorName();				// ASCII Author name
	virtual const TCHAR * CopyrightMessage();			// ASCII Copyright message
	virtual const TCHAR * OtherMessage1();			// Other message #1
	virtual const TCHAR * OtherMessage2();			// Other message #2
	virtual unsigned int Version();					// Version number * 100 (i.e. v3.01 = 301)
	virtual void ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);	// Export file
	virtual BOOL SupportsOptions(int ext, DWORD options); // Returns TRUE if all option bits set are supported for this extension

protected:
	OgreMax::Config m_config;
	OgreMax::MaterialMap m_materialMap;

	HINSTANCE m_hInstance;
	HWND m_hWndDlgExport;
	OgreMaxExport_TabPaneHandler* m_tabDisplay;

	// Export implementations
	OgreMax::MeshExporter m_meshExporter;
	OgreMax::MeshXMLExporter m_meshXMLExporter;
	OgreMax::MaterialExporter m_materialExporter;

	// Property Page Dialog Handlers
	OgreMaxExport_General m_tabGeneral;
	OgreMaxExport_Mesh m_tabMesh;
	OgreMaxExport_SkeletalAnimation m_tabSkeletalAnimation;
	OgreMaxExport_VertexAnimation m_tabVertexAnimation;
	OgreMaxExport_Material m_tabMaterial;

	// utility methods
	BOOL setupExportProperties();
	bool export();

	// properties page message handlers
	void onTabSelectChange(HWND hTabControl, INT iTabId);
};

class OgreMaxExportClassDesc : public ClassDesc {
public:
	int IsPublic();
	void * Create(BOOL loading = FALSE);
	const TCHAR * ClassName();
	SClass_ID SuperClassID();
	Class_ID ClassID();
	const TCHAR* Category();
};
