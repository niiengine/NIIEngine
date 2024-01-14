#ifndef __ogrewrapper_H__
#define __ogrewrapper_H__

#include <Ogre.h>

// Initialize the root and load the GLES2 render system
bool initOgreRoot();

// Initializes the render window
// Returns null if it fails
Ogre::ViewWindow *initRenderWindow(unsigned int windowHandle, unsigned int width, 
	unsigned int height, unsigned int contextHandle);

// Destroy Ogre's core systems
void destroyOgreRoot();

// Destroys the previously created ViewWindow 
void destroyWindow();

// Returns the render window created
Ogre::ViewWindow *getRenderWindow();

// Renders one frame
void renderFrame();

#endif