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
#ifndef _TECHNIQUEEVENTARGS_H_
#define _TECHNIQUEEVENTARGS_H_

#include "EventArgs.h"

class PassController;
class TechniqueController;

class TechniqueEventArgs : public EventArgs
{
public:
	TechniqueEventArgs(TechniqueController* tc);
	TechniqueEventArgs(TechniqueController* tc, PassController* pc);

	TechniqueController* getTechniqueController() const;
	PassController* getPassController() const;

protected:
	TechniqueController* mTechniqueController;
	PassController* mPassController;
};

#endif // _PROJECTEVENTARGS_H_