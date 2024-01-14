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
    //------------------------------------------------------------------------
	void FileSystemLayerImpl::getConfigPaths()
	{
	}
    //------------------------------------------------------------------------
	void FileSystemLayerImpl::prepareUserHome(const Ogre::String & subdir)
	{
	}
    //------------------------------------------------------------------------
	bool FileSystemLayerImpl::fileExists(const Ogre::String & path) const
	{
		return access(path.c_str(), 00) == 0;
	}
    //------------------------------------------------------------------------
	bool FileSystemLayerImpl::createDirectory(const Ogre::String& path)
	{
		return false;
	}
    //------------------------------------------------------------------------
}
