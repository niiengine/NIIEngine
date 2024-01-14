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

#ifndef SUBVERSIONLOCALPROPERTIES_H
#define SUBVERSIONLOCALPROPERTIES_H

#include <wx/string.h>
#include <map>

typedef std::map<wxString, wxString>    SimpleTable;
typedef std::map<wxString, SimpleTable> GroupTable;

class SubversionLocalProperties
{
	wxString   m_url;
	GroupTable m_values;

public:
	static wxString BUG_TRACKER_URL;
	static wxString BUG_TRACKER_MESSAGE;
	static wxString FR_TRACKER_URL;
	static wxString FR_TRACKER_MESSAGE;

protected:
	wxString GetConfigFile();
	void     ReadProperties();
	void     WriteProperties();

public:
	SubversionLocalProperties(const wxString &url);
	virtual ~SubversionLocalProperties();

	wxString ReadProperty (const wxString &propName);
	void     WriteProperty(const wxString &name, const wxString &val);
};

#endif // SUBVERSIONLOCALPROPERTIES_H
