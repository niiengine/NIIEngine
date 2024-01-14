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
#ifndef __OVERLAY_HELPER_H
#define __OVERLAY_HELPER_H

#include "DS_List.h"

namespace Ogre
{
	class ViewWindow;
	class SpaceManager;
	class OverlayView;
	class TextOverlayGeo;
	class FrameOverlayGeo;
	class OverlayGeo;
	class Overlay;
}

// This classe makes it easier to use Ogre's overlay system.  It provides the ability to fade overlays in and out and to automatically delete them
// after time
class OverlayHelper
{
public:
	OverlayHelper();
	~OverlayHelper();
	void Startup(void);
	void Shutdown(void);
	void update(unsigned int elapsedTimeMS);

	// Just returns a global overlay that I store.  Useful functions on it are hide() and show()
	Ogre::Overlay* GetGlobalOverlay(void) const;

	// Fades an overlay element to some designated final alpha.  You can autodelete the overlay after fading as well.
	void FadeOverlayGeo(Ogre::OverlayGeo* element, unsigned int totalTime, unsigned int fadeTimeMS, NIIf finalAlpha, bool deleteAfterFade);

	// Equivalent to Ogre's function.  All OverlayGeos must be a child of a panel.
	Ogre::OverlayView* CreatePanel(const char *instanceName, bool addToGlobalOverlay=true);

	// Displays a single line of text.  Doesn't handle text clipping or wrapping.
	Ogre::TextOverlayGeo *CreateTextArea(const char *instanceName, const char *fontName, Ogre::OverlayView* parent);

	// Equivalent to Ogre's function.
	Ogre::FrameOverlayGeo *CreateBorderPanel(const char *instanceName, Ogre::OverlayView* parent);

	// Destroy any overlay created with the above.
	// Safer because it removes the element from its children and parent.
	void SafeDestroyOverlayGeo(Ogre::OverlayGeo *item);

	// For internal use
	struct TimedOverlay
	{
		TimedOverlay();
		~TimedOverlay();
		TimedOverlay(Ogre::OverlayGeo *overlayElement, unsigned int totalTime, unsigned int fadeTimeMS, NIIf finalAlpha, bool deleteAfterFade);
		Ogre::OverlayGeo *overlayElement;
		unsigned int remainingTimeMS;
		unsigned int fadeTimeMS;
		NIIf finalAlpha;
		NIIf startFadeAlpha;
		bool deleteAfterFade;
	};

protected:
	// A list of timed text elements with fade.
	// setColour
	DataStructures::List<TimedOverlay> timedOverlays;
	Ogre::Overlay* globalOverlay;
	unsigned int fadeTimeMSMS;
};

#endif
