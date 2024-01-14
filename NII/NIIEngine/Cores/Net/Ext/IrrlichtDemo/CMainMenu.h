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

#ifndef __C_MAIN_MENU_H_INCLUDED__
#define __C_MAIN_MENU_H_INCLUDED__

#include <irrlicht.h>
#include "IGUIEditBox.h"

using namespace irr;

class CMainMenu : public IEventReceiver
{
public:

	CMainMenu();

	bool run(bool& outFullscreen, bool& outMusic, bool& outShadows,
		bool& outAdditive, bool &outVSync, bool& outAA,
		video::E_DRIVER_TYPE& outDriver,
		core::stringw &playerName);

	virtual bool OnEvent(const SEvent& event);

private:

	void setTransparency();

	gui::IGUIButton* startButton;
	IrrlichtDevice *MenuDevice;
	s32 selected;
	bool start;
	bool fullscreen;
	bool music;
	bool shadows;
	bool additive;
	bool transparent;
	bool vsync;
	bool aa;

	scene::IAnimatedMesh* quakeLevel;
	scene::ISceneNode* lightMapNode;
	scene::ISceneNode* dynamicNode;

	video::SColor SkinColor [ gui::EGDC_COUNT ];
	void getOriginalSkinColor();

	// RakNet: Store the edit box pointer so we can get the text later
	irr::gui::IGUIEditBox* nameEditBox;
};

#endif

