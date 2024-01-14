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

#ifndef __OSXErrorDialog_H__
#define __OSXErrorDialog_H__

#include "NiiPreInclude.h"

namespace NII
{
	/** Class for displaying the error dialog if Ogre fails badly. */
	class _OgreExport ErrorDialog
	{
	public:
		ErrorDialog();

		/**
		@remarks
			Displays the error dialog.
		@param
			errorMessage The error message which has caused the failure.
		@param
			logName Optional name of the log to display in the detail pane.
		*/
		void display(const String& errorMessage, String logName = "");
	};
}
#endif
