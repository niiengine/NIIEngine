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
/*
-----------------------------------------------------------------------------
Filename:    WindowEmbedding.cpp
Description: Stuff your windows full of OGRE
-----------------------------------------------------------------------------
*/
#include "Ogre.h"

using namespace Ogre;

void setupResources(void); // Just a prototype

void setupResources(void)
{
	// Load resource paths from config file
	ConfigFile cf;
	cf.load("resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while(seci.isNextValid())
	{
		secName = seci.getCurrentKey();
		ConfigFile::SettingsMultiMap *settings = seci.getAndNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			N_Only(ResourceScheme).add(archName, typeName, secName);
		}
	}
}
//---------------------------------------------------------------------
// Windows Test
//---------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "windows.h"

ViewWindow* renderWindow = 0;
bool winActive = false;
bool winSizing = false;

LRESULT CALLBACK TestWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (uMsg == WM_CREATE)
	{
		return 0;
	}


	if (!renderWindow)
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_ACTIVATE:
		winActive = (LOWORD(wParam) != WA_INACTIVE);
		break;

	case WM_ENTERSIZEMOVE:
		winSizing = true;
		break;

	case WM_EXITSIZEMOVE:
		renderWindow->notify();
		renderWindow->flush();
		winSizing = false;
		break;

	case WM_MOVE:
	case WM_SIZE:
		if(!winSizing)
			renderWindow->notify();
		break;

	case WM_GETMINMAXINFO:
		// Prevent the window from going smaller than some min size
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
		break;

	case WM_CLOSE:
		renderWindow->destroy(); // cleanup and call DestroyWindow
		PostQuitMessage(0); 
		return 0;
	case WM_PAINT:
		if (!winSizing)
		{
			renderWindow->flush();
			return 0;
		}
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


INT WINAPI EmbeddedMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	try 
	{
		// Create a new window

		// Style & size
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;

		// Register the window class
		WNDCLASS wc = { 0, TestWndProc, 0, 0, hInst,
			LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "TestWnd" };
		RegisterClass(&wc);

		HWND hwnd = CreateWindow("TestWnd", "Test embedding", dwStyle,
				0, 0, 800, 600, 0, 0, hInst, 0);

		Root root("", "");

		root.loadPlugin("RenderSystem_GL");
		//root.loadPlugin("RenderSystem_Direct3D9");
		root.loadPlugin("Plugin_ParticleFX");
		root.loadPlugin("Plugin_CgProgramManager");

		// select first renderer & init with no window
		root.set(*(root.getRenderList().begin()));
		root.init(false);

		// create first window manually
		PropertyData options;
		options["externalWindowHandle"] = StrConv::conv((size_t)hwnd);

		renderWindow = root.createWindow("embedded", 800, 600, false, &options);

		setupResources();
		ResourceSchemeManager::getSingleton().initialiseAllResourceGroups();

		SpaceManager * scene = root.createSpaceManager(NII::SpaceTypeVerbose::ST_Normal, "default");
		Camera * cam = scene->createCamera("cam");

		Viewport * vp = renderWindow->createViewport(cam);
		vp->setBgColour(Ogre::Colour(0.5, 0.5, 0.7));
		cam->setAutoAspectRatio(true);
		cam->setPos(NII::Vector3f(0, 0, 300));
		cam->setDirection(Vector3f(0, 0, -1));

		GeometrySpace * e = scene->createGeo("1", "ogrehead.mesh");
		static_cast<SpaceNode *>(scene->getOriginNode()->create())->attachObject(e);
		Light * l = scene->createLight("l");
		l->setPos(300, 100, -100);

		// message loop
		MSG msg;
		while(GetMessage(&msg, NULL, 0, 0 ) != 0)
		{ 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		} 

	} 
	catch(Exception & e) 
	{
		MessageBox( NULL, e.getFullDescription().c_str(), 
			"An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}
	return 0;
}
#endif