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
#include "RootEventPlugin.h"

#include "RootEventArgs.h"

template<> RootEventPlugin* Ogre::Singleton<RootEventPlugin>::msSingleton = 0;

RootEventPlugin& RootEventPlugin::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );  
}

RootEventPlugin* RootEventPlugin::getSingletonPtr(void)
{
	return msSingleton;
}

RootEventPlugin::RootEventPlugin()
{
	registerEvents();
}

RootEventPlugin::~RootEventPlugin()
{
}

const String& RootEventPlugin::getName() const
{
	return "Root Event Plugin";
}

void RootEventPlugin::install()
{
}

void RootEventPlugin::init()
{
	fireEvent(Initialized, RootEventArgs());
}

void RootEventPlugin::shutdown()
{
	fireEvent(Shutdown, RootEventArgs());
}

void RootEventPlugin::uninstall()
{
}
void RootEventPlugin::registerEvents()
{
	registerEvent(Initialized);
	registerEvent(Shutdown);
}