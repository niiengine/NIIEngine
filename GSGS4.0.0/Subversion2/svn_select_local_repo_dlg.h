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

#ifndef __svn_select_local_repo_dlg__
#define __svn_select_local_repo_dlg__

#include "subversion2_ui.h"

class Subversion2;
class SvnSelectLocalRepoDlg : public SvnSelectLocalRepoBase
{
	Subversion2 *m_svn;
	
protected:
	// Handlers for SvnSelectLocalRepoBase events.
	void OnPathSelected( wxCommandEvent& event );
	void OnPathActivated( wxCommandEvent& event );
	void OnMenu( wxMouseEvent& event );
	
	void OnRemoveEntry(wxCommandEvent &e);
	
public:
	/** Constructor */
	SvnSelectLocalRepoDlg( wxWindow* parent, Subversion2* plugin, const wxString &curpath );
	virtual ~SvnSelectLocalRepoDlg();
	wxString GetPath() const;
};

#endif // __svn_select_local_repo_dlg__
