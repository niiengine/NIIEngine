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
#ifndef _CALLTIPMANAGER_H_
#define _CALLTIPMANAGER_H_

#include <list>
#include <map>

#include <wx/string.h>

using namespace std;

typedef map<wxString, wxString> CallTipMap;
typedef list<wxString> TriggerList;

class CallTipManager
{
public:
	CallTipManager();
	virtual ~CallTipManager();

	void load(wxString& path);

	void addTip(wxString& key, wxString& tip);
	void removeTip(wxString& key);

	void addTrigger(wxChar& trigger);
	void removeTrigger(wxChar& trigger);
	bool isTrigger(wxChar& ch);

	wxString* find(wxString& s);

protected:
	CallTipMap mCallTips;
	TriggerList mTriggers;
};

#endif // _CALLTIPMANAGER_H_


