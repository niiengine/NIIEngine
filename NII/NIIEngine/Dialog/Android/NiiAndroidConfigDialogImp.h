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

#ifndef _NII_AndroidConfigDialog_H__
#define _NII_AndroidConfigDialog_H__

#include "NiiPreInclude.h"
#include "NiiEngine.h"
#include "NiiRenderSys.h"

namespace NII
{
	class _EngineAPI ConfigDialog : public UtilityAlloc
	{
	public:
		ConfigDialog();
		~ConfigDialog();

	public:
		void init();

		void run();

		void cancel();

		bool display();
	protected:
		RenderSys * iSelectedRenderSystem;
		bool iDisplayStatus;
	};
}

#endif // __AndroidConfigDialog_H__
