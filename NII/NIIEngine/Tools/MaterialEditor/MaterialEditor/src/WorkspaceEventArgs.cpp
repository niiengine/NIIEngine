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
#include "WorkspaceEventArgs.h"

WorkspaceEventArgs::WorkspaceEventArgs()
: mWorkspace(0), mProject(0)
{
}

WorkspaceEventArgs::WorkspaceEventArgs(Workspace* workspace, Project* project)
: mWorkspace(workspace), mProject(project)
{

}

WorkspaceEventArgs::~WorkspaceEventArgs()
{

}

Workspace* WorkspaceEventArgs::getWorkspace()
{
	return mWorkspace;
}

Project* WorkspaceEventArgs::getProject()
{
	return mProject;
}

