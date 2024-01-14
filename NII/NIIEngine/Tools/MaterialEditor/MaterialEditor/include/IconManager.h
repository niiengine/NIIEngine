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
#ifndef _ICONMANAGER_H_
#define _ICONMANAGER_H_

#include "wx/bitmap.h"
#include "OgreSingleton.h"

#include "../xpm/close.xpm"
#include "../xpm/copy.xpm"
#include "../xpm/cut.xpm"
#include "../xpm/material.xpm"
#include "../xpm/material_script.xpm"
#include "../xpm/ogre.xpm"
#include "../xpm/pass.xpm"
#include "../xpm/paste.xpm"
#include "../xpm/program_script.xpm"
#include "../xpm/project.xpm"
#include "../xpm/project_new.xpm"
#include "../xpm/project_save.xpm"
#include "../xpm/save.xpm"
#include "../xpm/save_as.xpm"
#include "../xpm/shader.xpm"
#include "../xpm/technique.xpm"
#include "../xpm/workspace.xpm"

class IconManager : public Ogre::Singleton<IconManager>
{
public:
	IconManager();
	~IconManager();

	enum IconType
	{
		CLOSE,
		COPY,
		CUT,
		MATERIAL,
		MATERIAL_SCRIPT,
		OGRE,
		PASS,
		PASTE,
		PROGRAM_SCRIPT,
		PROJECT,
		PROJECT_NEW,
		PROJECT_SAVE,
		SAVE,
		SAVE_AS,
		SHADER,
		TECHNIQUE,
		WORKSPACE
	};

	const wxBitmap& getIcon(IconType type) const;

	static IconManager& getSingleton(void);
	static IconManager* getSingletonPtr(void);

private:
	typedef std::map<IconType, wxBitmap> IconMap;
	IconMap mIconMap;
};
#endif
