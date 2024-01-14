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
#include "MaterialEditorApp.h"

#include <wx/splash.h>

#include "OgreConfigFile.h"
#include "OgreLog.h"
#include "OgreLogManager.h"
#include "OgreResourceManager.h"

#include "IconManager.h"
#include "MaterialEditorFrame.h"
#include "SelectionService.h"
#include "Workspace.h"

using Ogre::ConfigFile;
using Ogre::LogManager;
using Ogre::ResourceSchemeManager;

MaterialEditorApp::~MaterialEditorApp()
{
}

bool MaterialEditorApp::OnInit()
{
	wxInitAllImageHandlers();

	wxBitmap bitmap;
	if(bitmap.LoadFile("../lexers/splash.png", wxBITMAP_TYPE_PNG))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
			2000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER | wxSTAY_ON_TOP);
	}

	// Create Selection Service
	new SelectionService();

	// Ensure Workspace is created
	new Workspace();

	// Create the IconManager
	new IconManager();

	MaterialEditorFrame* frame = new MaterialEditorFrame();
	frame->SetIcon(wxIcon(ogre_xpm));
	frame->Show(true);

	SetTopWindow(frame);

	return true;
}

int MaterialEditorApp::OnExit()
{
	// Minimally clean up the IconManager
	delete IconManager::getSingletonPtr();

	return 0;
}