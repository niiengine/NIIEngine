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
#ifndef _DOCMANAGER_H_
#define _DOCMANAGER_H_

#include <list>
#include <map>

#include <wx/string.h>

using namespace std;

typedef map<wxString, wxString> DocMap;

class DocManager
{
public:
	DocManager();
	virtual ~DocManager();

	void load(wxString& path);

	void addDoc(wxString& key, wxString& doc);
	void removeDoc(wxString& key);

	wxString* find(wxString& s);

protected:
	DocMap mDocs;
};

#endif // _DOCMANAGER_H_


