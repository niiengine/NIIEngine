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

#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreMaterialManager.h"
#include "OgreCompositorManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreGpuProgramManager.h"
#include "OgreLodStrategyManager.h"

Ogre::LogManager* logMgr;
void setUpSuite()
{
    logMgr = new Ogre::LogManager();
    logMgr->createLog("OgreTest.log", true, true);
}

void tearDownSuite()
{
    // shutdown and release managers that might have been created
	if(Ogre::HighLevelGpuProgramManager::getSingletonPtr())
		delete Ogre::HighLevelGpuProgramManager::getSingletonPtr();
	if(Ogre::GpuProgramManager::getSingletonPtr())
		delete Ogre::GpuProgramManager::getSingletonPtr();
	if(Ogre::CompositorManager::getSingletonPtr())
		delete Ogre::CompositorManager::getSingletonPtr();
	if(Ogre::MaterialManager::getSingletonPtr())
		delete Ogre::MaterialManager::getSingletonPtr();
	if(Ogre::ResourceSchemeManager::getSingletonPtr())
		delete Ogre::ResourceSchemeManager::getSingletonPtr();

	if(Ogre::LogManager::getSingletonPtr())
		delete Ogre::LogManager::getSingletonPtr();
	if(Ogre::ResourceSchemeManager::getSingletonPtr())
		delete Ogre::ResourceSchemeManager::getSingletonPtr();
	if(Ogre::LodStrategyManager::getSingletonPtr())
		delete Ogre::LodStrategyManager::getSingletonPtr();

}