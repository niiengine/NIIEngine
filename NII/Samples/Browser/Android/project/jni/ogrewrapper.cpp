#include "ogrewrapper.h"
#include "AndroidLogListener.h"

#include <OgreGLES2Plugin.h>
#include <OgreParticleFXPlugin.h>

// Store global variables here
static Ogre::Root *g_root = 0;
static Ogre::GLES2Plugin *g_gles2Plugin = 0;
static Ogre::ParticleFXPlugin *g_pfxPlugin = 0;
static AndroidLogListener *g_ll = 0;
static Ogre::ViewWindow *g_renderWindow = 0; // The main render window

static Ogre::Timer g_timer;
static unsigned long g_lastTime = 0;

bool initOgreRoot(){
	try{
		// Create logs that funnel to android logs
		Ogre::LogManager *lm = N_new Ogre::LogManager();
		Ogre::Log *l = lm->createLog("AndroidLog", true, true, true);
		g_ll = N_new AndroidLogListener();
		l->add(g_ll);
		
		// Create a root object
		g_root = N_new Ogre::Root("", "", "");
		
		// Register the ES2 plugin
		g_gles2Plugin = N_new Ogre::GLES2Plugin();
		Ogre::Root::getSingleton().installPlugin(g_gles2Plugin);
		
		// Register particle plugin
		g_pfxPlugin = N_new Ogre::ParticleFXPlugin();
		Ogre::Root::getSingleton().installPlugin(g_pfxPlugin);
		
		// Grab the available render systems
		const Ogre::RenderSysList &renderSystemList = g_root->getRenderList();
		if(renderSystemList.empty())
		{
			return false;
		}
		
		// Set the render system and init
		Ogre::RenderSystem * system = renderSystemList.front();
		g_root->set(system);
		g_root->init(false);
		
		g_lastTime = g_timer.getMS();
		
		return true;
	}catch(Ogre::Exception &e){
	}
	return false;
}

Ogre::ViewWindow *initRenderWindow(unsigned int windowHandle, unsigned int width,
	unsigned int height, unsigned int contextHandle)
    {
	if(g_root != 0 && g_renderWindow == 0){
		Ogre::PropertyData params;
		params["externalWindowHandle"] = Ogre::StrConv::conv(windowHandle);
		params["externalGLContext"] = Ogre::StrConv::conv(contextHandle);
		
		g_renderWindow = g_root->createWindow("OgreAndroidPrimary", width, height, true, &params);
	}
	
	return g_renderWindow;
}

void destroyOgreRoot(){
	if(g_root)
		N_delete g_root;
	if(g_gles2Plugin)
		N_delete g_gles2Plugin;
	if(g_pfxPlugin)
		N_delete g_pfxPlugin;
	
	g_root = 0;
	g_gles2Plugin = 0;
	g_pfxPlugin = 0;
}

void destroyWindow()
{
	if(g_renderWindow)
    {
		g_renderWindow->destroy();
		Ogre::Root::getSingleton().detach(g_renderWindow);
	       
		g_renderWindow = 0; 
	}
}

Ogre::ViewWindow * getRenderWindow()
{
	return g_renderWindow;
}

void renderFrame()
{
	if(g_root && g_renderWindow)
    {
		/*
		unsigned long t = g_timer.getMS();
		unsigned long d = t - g_lastTime;
		
		if(d > 16){
			g_lastTime = t;
			
			if(d > 250)
				d = 250;
			g_root->renderFrame(d);
		}
		*/
		g_root->renderFrame();
	}
}