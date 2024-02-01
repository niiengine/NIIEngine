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

#ifndef __svn_props_dialog__
#define __svn_props_dialog__

/**
@file
Subclass of SvnPropsBaseDlg, which is generated by wxFormBuilder.
*/

#include "subversion2_ui.h"
class Subversion2;
/** Implementing SvnPropsBaseDlg */
class SvnPropsDlg : public SvnPropsBaseDlg
{
	Subversion2 *m_plugin;
	wxString     m_url;

public:
	/** Constructor */
	SvnPropsDlg( wxWindow* parent, const wxString& url, Subversion2 *plugin );
	virtual ~SvnPropsDlg();

	wxString GetBugTrackerURL() const;
	wxString GetFRTrackerURL() const;
	wxString GetBugMsg() const;
	wxString GetFRMsg() const;

};

#endif // __svn_props_dialog__