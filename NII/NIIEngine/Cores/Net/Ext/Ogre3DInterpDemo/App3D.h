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
#ifndef __APP_3D_H
#define __APP_3D_H

#include "AppInterface.h"

namespace Ogre
{
	class Engine;
	class Camera;
	class SpaceManager;
	class ViewWindow;
	class Viewport;
	class SpaceNode;
	class Math;
	class Overlay;
	class Viewport;
}

class FadingTextList;

class App3D : public AppInterface
{
public:
	App3D();
	virtual ~App3D();
	virtual void PreConfigure(void);
	//virtual void StartupSound(int maxchannels, unsigned int flags, void *extradriverdata, unsigned logType, bool logActive=false);
	virtual bool Configure(void);
	// Note that Ogre is bugged - setting recursive to true just crashes
	virtual void PostConfigure(const char *defaultResourceConfigurationPath, bool recursive);
	virtual void InitSceneManager(Ogre::SpaceManager *sm=0);
	virtual void InitGUIManager(void);
	virtual void InitCamera(Ogre::Camera *cam=0);
	virtual void InitViewport(Ogre::Viewport *vp=0);
	virtual void update(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual void Render(AppTime curTimeMS);
	virtual void SetVisible(bool _isVisible);
	virtual bool IsVisible(void) const;
	virtual bool ShouldQuit(void) const;
	virtual void OnAppShutdown(void);
	virtual void SetState(int stateType, RunnableState* state);
	bool HasPixelShader2(void) const;
	// Returns filename written
	const char * TakeScreenshot(const char *prefix, const char *suffix);

	Ogre::SpaceManager* GetSceneManager(void) const;
	const char* GetWorkingDirectory(void) const;

	Ogre::Engine * root;
	Ogre::Camera * camera;
    Ogre::ViewWindow * window;
	Ogre::Viewport  * viewport;
	// This is here because the math trig tables don't get initialize until Math is instantiated, even though the tables are static.
	Ogre::Math *math;
	//Ogre::SpaceNode *gameplayNode;

protected:

	Ogre::SpaceManager* sceneManager;
	void UpdateDefault(AppTime curTimeMS,AppTime elapsedTimeMS);
	virtual char * GetWindowTitle(void) const=0;

	bool isVisible;
	char workingDirectory[512];
	bool hasPixelShader2;
};

#endif
