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
#ifndef _MATERIALEVENTARGS_H_
#define _MATERIALEVENTARGS_H_

#include "EventArgs.h"

class MaterialController;
class TechniqueController;

class MaterialEventArgs : public EventArgs
{
public:
	MaterialEventArgs(MaterialController* mc);
	MaterialEventArgs(MaterialController* mc, TechniqueController* tc);

	MaterialController * getMaterialController() const;
	TechniqueController * getTechniqueController() const;
protected:
	MaterialController * mMaterialController;
	TechniqueController * mTechniqueController;
};

#endif // _MATERIALEVENTARGS_H_