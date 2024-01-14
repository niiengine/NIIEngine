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
#include "macUtils.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

namespace OgreBites
{
    //---------------------------------------------------------------------
	void FileSystemLayerImpl::getConfigPaths()
	{
		mConfigPaths.push_back(Ogre::macBundlePath() + "/Contents/Resources/");
		mConfigPaths.push_back(Ogre::macBundlePath() + "/");
	}
    //---------------------------------------------------------------------
	void FileSystemLayerImpl::prepareUserHome(const Ogre::String & subdir)
	{
		struct passwd * pwd = getpwuid(getuid());
		if(pwd)
		{
			mHomePath = pwd->pw_dir;
		}
		else
		{
			// try the $HOME environment variable
			mHomePath = getenv("HOME");
		}

		if(!mHomePath.empty())
		{
			// create an Ogre subdir in application support
			mHomePath.append("/Library/Application Support/Ogre/");
			if(mkdir(mHomePath.c_str(), 0755) != 0 && errno != EEXIST)
			{
				// can't create dir
				mHomePath.clear();
			}
			else
			{
				// now create the given subdir
				mHomePath.append(subdir + '/');
				if(mkdir(mHomePath.c_str(), 0755) != 0 && errno != EEXIST)
				{
					// can't create dir
					mHomePath.clear();
				}
			}
		}

		if (mHomePath.empty())
		{
			// couldn't create dir in home directory, fall back to cwd
			mHomePath = "./";
		}
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::fileExists(const Ogre::String & path) const
	{
		return access(path.c_str(), R_OK) == 0;
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::createDirectory(const Ogre::String & path)
	{
		return !mkdir(path.c_str(), 0755) || errno == EEXIST;
	}
    //---------------------------------------------------------------------
}
