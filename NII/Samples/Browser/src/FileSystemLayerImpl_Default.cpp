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
#include "FileSystemLayerImpl.h"

namespace OgreBites
{
	void FileSystemLayerImpl::getConfigPaths()
	{
		// use current working directory
		mConfigPaths.push_back("");
	}
    //---------------------------------------------------------------------
	void FileSystemLayerImpl::prepareUserHome(const Ogre::String& subdir)
	{
		// use current working directory
		mHomePath = "";
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::fileExists(const Ogre::String& path) const
	{
		// always assume true
		return true;
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::createDirectory(const Ogre::String& path)
	{
		// always return false (nothing was created...)
		return false;
	}
}
