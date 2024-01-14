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
#include "DocManager.h"

#include "OgreDataStream.h"
#include "OgreString.h"

using Ogre::DataStream;
using Ogre::FileDataStream;
using Ogre::String;

DocManager::DocManager()
{
}

DocManager::~DocManager()
{
}

void DocManager::load(wxString& path)
{
	// TODO: Clear tips list

	std::ifstream fp;
	fp.open(path, std::ios::in | std::ios::binary);
	if(fp)
	{
		DataStream * stream = N_new FileDataStream(path.c_str(), &fp, false);

		int index = -1;
		String line;
		String key;
		while(!stream->end())
		{
			line = stream->readLine();

			// Ignore comments
			if(line.length() > 0 && line.at(0) == '#') continue;

			if(line.length() > 0 && line.at(0) == '[')
			{
				int endBrace = (int)line.find(']');
				if(endBrace != -1)
				{
					key = line.substr(1, endBrace - 1);
				}
			}
			else
			{
				if(mDocs.find(key) != mDocs.end())
					mDocs[key] = mDocs[key] + "\n" + line;
				else
					mDocs[key] = line;
			}
		}
        N_delete stream;
	}
}

void DocManager::addDoc(wxString& key, wxString& doc)
{
	mDocs[key] = doc;
}

void DocManager::removeDoc(wxString& key)
{
	DocMap::iterator it = mDocs.find(key);
	if(it != mDocs.end()) mDocs.erase(it);
}

wxString* DocManager::find(wxString& s)
{
	if(mDocs.find(s) != mDocs.end())
		return &mDocs[s];

	return NULL;
}