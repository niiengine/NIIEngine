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
#include "CallTipManager.h"

#include "OgreDataStream.h"
#include "OgreString.h"

using Ogre::DataStream;
using Ogre::FileDataStream;
using Ogre::String;

CallTipManager::CallTipManager()
{
}

CallTipManager::~CallTipManager()
{
}

void CallTipManager::load(wxString & path)
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

			// Ignore blank lines and comments (comment lines start with '#')
			if(line.length() > 0 && line.at(0) != '#')
			{
				if(line.at(0) == '[')
				{
					int endBrace = (int)line.find(']');
					if(endBrace != -1)
					{
						key = line.substr(1, endBrace - 1);
					}
				}
				else
				{
					if(mCallTips.find(key) != mCallTips.end())
						mCallTips[key] = mCallTips[key] + "\n" + line;
					else
						mCallTips[key] = line;
				}
			}
		}
        N_delete stream;
	}
}

void CallTipManager::addTip(wxString& key, wxString& tip)
{
	mCallTips[key] = tip;
}

void CallTipManager::removeTip(wxString& key)
{
	CallTipMap::iterator it = mCallTips.find(key);
	if(it != mCallTips.end()) mCallTips.erase(it);
}

void CallTipManager::addTrigger(wxChar& trigger)
{
	mTriggers.push_back(trigger);
}

void CallTipManager::removeTrigger(wxChar& trigger)
{
	TriggerList::iterator it;
	for(it = mTriggers.begin(); it != mTriggers.end(); ++it)
	{
		if((*it) == trigger)
		{
			mTriggers.erase(it);
			return;
		}
	}
}

bool CallTipManager::isTrigger(wxChar& ch)
{
	TriggerList::iterator it;
	for(it = mTriggers.begin(); it != mTriggers.end(); ++it)
	{
		if((*it) == ch) return true;
	}

	return false;
}

wxString* CallTipManager::find(wxString& s)
{
	if(mCallTips.find(s) != mCallTips.end())
		return &mCallTips[s];

	return NULL;
}