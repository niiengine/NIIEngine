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
#include "Workspace.h"

#include "EventArgs.h"
#include "Project.h"
#include "WorkspaceEventArgs.h"

template<> Workspace* Ogre::Singleton<Workspace>::msSingleton = 0;

Workspace& Workspace::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );  
}

Workspace* Workspace::getSingletonPtr(void)
{
	return msSingleton;
}

Workspace::Workspace()
{
	registerEvents();
}

Workspace::~Workspace()
{
	ProjectList::iterator it;
	for(it = mProjects.begin(); it != mProjects.end(); ++it)
	{
		delete *it;
	}
}

void Workspace::registerEvents()
{
	registerEvent(ProjectAdded);
	registerEvent(ProjectRemoved);
}

void Workspace::addProject(Project* project)
{
	mProjects.push_back(project);
	
	fireEvent(ProjectAdded, WorkspaceEventArgs(this, project));
}

void Workspace::removeProject(Project* project)
{
	mProjects.remove(project);
	fireEvent(ProjectAdded, WorkspaceEventArgs(this, project));
	delete project;
}

void Workspace::removeProject(const String& name)
{
	removeProject(getProject(name));
}

Project* Workspace::getProject(const String& name)
{
	Project* p;
	ProjectList::iterator it;
	for(it = mProjects.begin(); it != mProjects.end(); ++it)
	{
		p = (*it);
		if(p->getName() == name) return p;
	}

	return NULL;
}

const ProjectList* Workspace::getProjects() const
{
	return &mProjects;
}