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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <io.h>

namespace OgreBites
{
	void FileSystemLayerImpl::getConfigPaths()
	{
		// try to determine the application's path
		DWORD bufsize = 256;
		char* resolved = 0;
		do
		{
			char* buf = OGRE_ALLOC_T(char, bufsize, Ogre::MEMCATEGORY_GENERAL);
			DWORD retval = GetModuleFileName(NULL, buf, bufsize);
			if (retval == 0)
			{
				// failed
				OGRE_FREE(buf, Ogre::MEMCATEGORY_GENERAL);
				break;
			}

			if (retval < bufsize)
			{
				// operation was successful.
				resolved = buf;
			}
			else
			{
				// buffer was too small, grow buffer and try again
				OGRE_FREE(buf, Ogre::MEMCATEGORY_GENERAL);
				bufsize <<= 1;
			}
		} while (!resolved);

		Ogre::String appPath = resolved;
		if (resolved)
			OGRE_FREE(resolved, Ogre::MEMCATEGORY_GENERAL);
		if (!appPath.empty())
		{
			// need to strip the application filename from the path
			Ogre::String::size_type pos = appPath.rfind('\\');
			if (pos != Ogre::String::npos)
				appPath.erase(pos);
		}
		else
		{
			// fall back to current working dir
			appPath = ".";
		}

		// use application path as config search path
		mConfigPaths.push_back(appPath + '\\');
	}
    //---------------------------------------------------------------------
	void FileSystemLayerImpl::prepareUserHome(const Ogre::String& subdir)
	{
		TCHAR path[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, 0, path)))
		{
			// need to convert to OEM codepage so that fstream can use
			// it properly on international systems.
			TCHAR oemPath[MAX_PATH];
			CharToOem(path, oemPath);
			mHomePath = oemPath;
			// create Ogre subdir
			mHomePath += "\\Ogre\\";
			if (! CreateDirectory(mHomePath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				// couldn't create directory, fall back to current working dir
				mHomePath.clear();
			}
			else
			{
				mHomePath += subdir + '\\';
				// create release subdir
				if (! CreateDirectory(mHomePath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
				{
					// couldn't create directory, fall back to current working dir
					mHomePath.clear();
				}
			}
		}

		if (mHomePath.empty())
		{
			// couldn't create dir in home directory, fall back to cwd
			mHomePath = "";
		}
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::fileExists(const Ogre::String & path) const
	{
		return _access(path.c_str(), 00) == 0;
	}
    //---------------------------------------------------------------------
	bool FileSystemLayerImpl::createDirectory(const Ogre::String& path)
	{
		return CreateDirectory(path.c_str(), NULL) != 0 || 
			GetLastError() == ERROR_ALREADY_EXISTS;
	}
}
