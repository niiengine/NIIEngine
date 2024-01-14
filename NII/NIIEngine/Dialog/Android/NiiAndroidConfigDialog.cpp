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

#include "OgreLogManager.h"
#include "OgreConfigDialog.h"

namespace NII 
{
	ConfigDialog* dlg = NULL;

	ConfigDialog::ConfigDialog()
	{
		dlg = this;
	}
	
	ConfigDialog::~ConfigDialog()
	{
	}
	
	void ConfigDialog::init()
    {
	}
	
	void ConfigDialog::run()
	{

	}
	
	void ConfigDialog::cancel()
	{
	}

	bool ConfigDialog::display()
	{
		return true;
	}

};
