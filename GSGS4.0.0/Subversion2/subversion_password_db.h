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

#ifndef SUBVERSIONPASSWORDDB_H
#define SUBVERSIONPASSWORDDB_H

#include <wx/fileconf.h>
#include "gsgsPluginCommon.h"

class SubversionPasswordDb {
	wxFileConfig *m_fileConfig;
public:
	SubversionPasswordDb();
	virtual ~SubversionPasswordDb();
	
	/**
	 * @brief find login details for given URL 
	 * @param url repository URL
	 * @param user [output]
	 * @param password [output]
	 * @return true if the URL is found in the database, false otherwise
	 */
	bool GetLogin(const wxString &url, wxString &user, wxString &password);
	/**
	 * @brief store login info for given url
	 * @param url url as key
	 * @param user user name 
	 * @param password password
	 */
	void SetLogin(const wxString &url, const wxString &user, const wxString &password);
	
	/**
	 * @brief delete login details for the given URL
	 * @param url
	 */
	void DeleteLogin(const wxString &url);
};

#endif // SUBVERSIONPASSWORDDB_H
