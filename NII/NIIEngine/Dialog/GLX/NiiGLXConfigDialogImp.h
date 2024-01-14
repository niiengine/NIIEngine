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

#ifndef _NII_GLXCONFIGDIALOG_H__
#define _NII_GLXCONFIGDIALOG_H__

#include "NiiPreInclude.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"

namespace NII
{
	/** Defines the behaviour of an automatic renderer configuration dialog.
	@remarks
		NII comes with it's own renderer configuration dialog, which
		applications can use to easily allow the user to configure the
		settings appropriate to their machine. This class defines the
		interface to this standard dialog. Because dialogs are inherently
		tied to a particular platform's windowing system, there will be a
		different subclass for each platform.
	@author
		Steven J. Streeting
	*/
	class _EngineAPI ConfigDialog : public UtilityAlloc
	{
	public:
		ConfigDialog();

		/** Displays the dialog.
		@remarks
			This method displays the dialog and from then on the dialog
			interacts with the user independently. The dialog will be
			calling the relevant NII rendering systems to query them for
			options and to set the options the user selects. The method
			returns when the user closes the dialog.
		@return
			If the user accepted the dialog, <b>true</b> is returned.
		@par
			If the user cancelled the dialog (indicating the application
			should probably terminate), <b>false</b> is returned.
		@see
			RenderSys
		*/
		bool display();

	protected:
		RenderSys * mSelectedRenderSystem;
	};
}
#endif
