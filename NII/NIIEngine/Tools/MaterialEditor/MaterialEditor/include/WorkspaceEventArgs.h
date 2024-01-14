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
#ifndef _WORKSPACEEVENTARGS_H_
#define _WORKSPACEEVENTARGS_H_

#include "EventArgs.h"

class Project;
class Workspace;

class WorkspaceEventArgs : public EventArgs
{
public:
	WorkspaceEventArgs();
	WorkspaceEventArgs(Workspace* workspace, Project* project);

	virtual ~WorkspaceEventArgs();

	Workspace* getWorkspace();
	Project* getProject();

protected:
	Workspace* mWorkspace;
	Project* mProject;
};

#endif // _WORKSPACEEVENTARGS_H_